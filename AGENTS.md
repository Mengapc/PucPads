# AGENTS.md — Guia para Agentes de IA no PucPads

> Documento de orientação para agentes (e pessoas) que forem atuar neste repositório.
> Leia isto **antes** de escrever, refatorar ou propor mudanças.
>
> **Fonte da verdade do produto:** [`README.md`](README.md) — Software Design Document (SDD)
> revisado em *"Revisão README"* (commit `5740be2`). Este `AGENTS.md` traduz o SDD para
> orientações práticas de implementação e aponta o que já existe no código vs. o que é planejado.
>
> Última atualização: 2026-11-25 — alinhado ao SDD revisado (escopo PIBITI 2026–2027).

---

## 1. Visão Geral

**PucPads** é um software musical de alta performance (estilo *Launchpad*) escrito em **C++ com o
framework JUCE**, desenvolvido como projeto de **Iniciação Científica Tecnológica (PIBITI — PUCPR)**.
Substitui o descontinuado *SuperPads Lights* no apoio pedagógico e na performance ao vivo do curso
de Bacharelado em Produção Musical.

**Proposta pedagógica:** funcionar como instrumento facilitador visual e tátil para percepção
rítmica, melódica e harmônica, **sem exigir letramento musical prévio**.

**Estado atual (baseline consolidado):**

- Grade **8x8 (64 pads)** responsiva, populada dinamicamente por JSON.
- Motor de mixagem polifônico (`juce::MixerAudioSource`) + player dedicado de *backing track*.
- Feedback visual de decaimento a **60 FPS** via `juce::Timer`.
- Navegação entre 3 telas (Menu, Grade de Performance, Mixer/Configurações).
- Validado em **Android real** (multitouch, baixa latência).

**Plataformas-alvo:** Desktop (Windows) e Android.

---

## 2. Stack Tecnológica

| Item | Valor |
| --- | --- |
| Linguagem | C++ (padrão exigido pelo JUCE 8) |
| Framework | **JUCE 8.0.10 ou superior** |
| Gerador de projeto | **Projucer** (`PucPads/PucPads.jucer`) |
| IDE Desktop | Visual Studio 2022 (carga *Desenvolvimento para Desktop com C++*, MSVC v143+, Windows 10/11 SDK) |
| IDE Mobile | Android Studio Jellyfish/Iguana+ (SDK Platform 34, NDK side-by-side 26.x/27.x, CMake) |
| Módulos JUCE | `juce_audio_basics`, `juce_audio_devices`, `juce_audio_formats`, `juce_audio_processors`, `juce_audio_utils`, `juce_core`, `juce_data_structures`, `juce_events`, `juce_graphics`, `juce_gui_basics`, `juce_gui_extra` (todos via `useGlobalPath`, §8.2) |
| Opções de projeto | `JUCE_STRICT_REFCOUNTEDPOINTER=1`, `JUCE_USE_MP3AUDIOFORMAT=1` |
| Orientação Android | `androidScreenOrientation="portrait"` |

> **Não introduza dependências ou frameworks de terceiros** (nem CMake próprio) sem autorização
> expressa. O projeto é intencionalmente dependente apenas do JUCE.

---

## 3. Estrutura do Repositório

```
/ (raiz do git)
├── README.md                 # SDD — fonte da verdade de requisitos, arquitetura e roadmap
├── AGENTS.md                 # este guia
├── .gitignore / .gitattributes
└── PucPads/                  # Raiz do projeto JUCE (NÃO é a raiz do git)
    ├── PucPads.jucer         # Projeto do Projucer — fonte da verdade do build
    ├── Source/               # >>> Código editável manualmente
    │   ├── Main.cpp                    # Entry point (JUCEApplication + MainWindow)
    │   ├── MainComponent.{h,cpp}       # Raiz: mixer global + navegação de telas
    │   ├── MenuComponent.{h,cpp}       # Menu principal
    │   ├── LaunchPadComponent.{h,cpp}  # Tela única: 8 templates + play/stop da base
    │   ├── PadGridComponent.{h,cpp}    # Grade/páginas de UM template + backing track + choke groups
    │   ├── PadComponent.{h,cpp}        # Pad individual (sample, cor, playMode, fade visual)
    │   ├── UiTheme.h                   # Cromo institucional (bordô/dourado) — fonte única
    │   ├── KitDefinition.{h,cpp}       # Parser/schema do kit (v2, retrocompatível com v1)
    │   └── SettingsComponent.{h,cpp}   # Mixer & Configurações (UI)
    ├── JuceLibraryCode/      # >>> GERADO PELO PROJUCER — não editar manualmente
    │   ├── JuceHeader.h
    │   ├── BinaryData.{h,cpp}          # Assets embutidos (~47 MB) — ver §5
    │   └── include_juce_*.{cpp,mm,c}
    └── Arquivos_PucPads/     # Assets de origem
        ├── C.mp3 D.mp3 E.mp3 F.mp3 G.mp3
        ├── Musica_Jogo.wav
        ├── KIT_SCHEMA.md     # Especificação do schema v2 + navegação multi-páginas
        ├── notes.json        # kit "Melodia" (páginas Melodia / Harmonia)
        └── drumkit.json      # kit "Bateria" (páginas Loops / Efeitos)
```

Fora de `PucPads/`:

```
tools/
├── validate_kits.py          # Valida schema/referências de áudio dos JSONs de kit
├── kit_parser_smoke_test.cpp # Teste de fumaça do parser (executável de console)
├── run_kit_parser_test.sh    # Compila e roda o teste acima com JUCE
├── launchpad_smoke_test.cpp  # Teste de fumaça da navegação/templates (headless)
└── run_launchpad_test.sh     # Compila e roda o teste acima com JUCE
```

> ⚠️ `JuceLibraryCode/` e `Builds/` são **saída do Projucer**. Nunca edite `BinaryData.cpp/h`
> nem `include_juce_*` à mão — qualquer *Save* no Projucer sobrescreve.

---

## 4. Arquitetura

### 4.1 Telas e navegação

`MainComponent` é `juce::AudioAppComponent` **e** `juce::Button::Listener`:

- Mantém **um `juce::MixerAudioSource` global** (`mixerSource`) — barramento de áudio de toda a app.
- Gerencia três telas como `std::unique_ptr`:

  | Membro | Classe | Papel |
  | --- | --- | --- |
  | `menuScreen` | `MenuComponent` | Menu introdutório (Launch Pad / Mixer & Configurações) |
  | `launchPadScreen` | `LaunchPadComponent` | **Tela única de performance**: 8 templates + pads |
  | `settingsScreen` | `SettingsComponent` | Mixer & Configurações |

- Troca de tela por `showMenuScreen()`, `showLaunchPadScreen()` e `showSettingsScreen()`.
- As antigas telas **"Kit Melodia"** e **"Kit Bateria"** (botões do menu que abriam um kit fixo)
  foram **removidas**: os kits agora são *templates* escolhidos no rodapé do LaunchPad.
- Roteia cliques em `buttonClicked(juce::Button*)` comparando ponteiros
  (ex.: `button == &menuScreen->launchPadButton`).

**Dois padrões de callback coexistem** — respeite o do arquivo que estiver editando:

1. `backButton` (LaunchPad e Settings) → `addListener(this)` + `buttonClicked()` no `MainComponent`.
2. `playButton`/`stopButton` (LaunchPad), botões de template e botões de página → lambdas em
   `onClick`, definidos dentro do próprio componente dono.

**LaunchPad = uma tela, 8 templates** (RF01/estrutura atual):

- Cabeçalho de 60 px: `backButton` + `playButton`/`stopButton` da base musical do template ativo.
- Rodapé de 70 px: **8 `TextButton`s numerados** (`1`…`8`), um por template; o ativo fica invertido.
- Hoje **só o template 1 tem kit** (`notes_json`); os demais abrem uma **grade fantasma** 8 × 8
  (pads mudos, sem áudio) prontos para o futuro editor de pads.
- Cada template é criado **sob demanda** e fica em cache (`std::unique_ptr<PadGridComponent>`):
  alternar entre templates **não** recria os `AudioTransportSource`.
- `updateTransportButtons()` desabilita play/stop quando o template não tem base musical válida.

### 4.2 Fluxo de áudio

```
PadComponent::transportSource ─────┐
                                   ├──► MainComponent::mixerSource ──► getNextAudioBlock()
PadGridComponent::backgroundSource ┘
```

- `MainComponent::setAudioChannels(0, 2)` — **0 entradas, 2 saídas** (somente reprodução).
  Captura por microfone (RF05) **exigirá** revisar essa configuração para 1 entrada.
- Cada `PadComponent` tem seu próprio `AudioFormatManager` + `AudioTransportSource` e se registra
  no mixer (`mixerSource.addInputSource`).
- `~PadComponent()` e `~PadGridComponent()` removem suas fontes do mixer — **mantenha esse
  contrato** (RNF04: destruição determinística, sem vazamentos nem *dangling sources*).

### 4.3 Componentes

- **`KitDefinition`** (structs + parser, sem dependência de UI): lê o `juce::var` do kit e devolve
  `pages[]` → `KitPad` (cor já resolvida, `chokeGroup`, `oneShot`, `gainDb`). Aceita **v2 e v1**.
  Especificação completa em `PucPads/Arquivos_PucPads/KIT_SCHEMA.md`. É a única camada que conhece
  o formato do arquivo — componentes de UI não fazem *parse* de JSON.
- **`UiTheme`** (namespace em `UiTheme.h`): fonte única do cromo institucional (bordô/dourado) e
  do estilo de `TextButton` (normal × ativo). Não duplique esses hex em componentes novos.
- **`PadComponent`** (`Component` + `Timer`): recebe um `KitPad` + os bytes do sample
  (`BinaryData`), toca em `mouseDown`, e anima *fade* da cor ativa a **60 Hz**
  (`currentFade -= 0.05f`). Suporta dois modos (`playMode`):
  - `gate` (default, comportamento histórico): para em `mouseUp`;
  - `oneshot`: toca até o fim do sample; o brilho acompanha a duração.
  Expõe `chokeStop()` (fade-out de 25 ms + stop) e o callback `onTriggered`.
- **`PadComponent` — pad fantasma:** com `soundData == nullptr` o pad nasce **mudo**
  (`hasAudio == false`), não entra no mixer e ignora o toque; serve para a grade dos templates
  ainda sem kit.
- **`LaunchPadComponent`**: dono do cromo da tela (voltar, play/stop, rodapé de templates) e da
  lista `std::vector<TemplateSlot>` (rótulo + recurso de kit + view em cache). `showTemplate(i)`
  interrompe o áudio do template que sai (`stopAllPads()` + `stopBackingTrack()`), troca a
  visibilidade e realinha a grade.
  - Os recursos dos 8 templates estão no array `kTemplateKitResources` (no `.cpp`): **único ponto
    a mudar** quando os templates passarem a vir de um manifesto/editor.
- **`PadGridComponent`**: renderiza **um** template. Carrega o kit via `KitDefinition`, cria
  **todos** os pads das páginas de uma vez em `juce::OwnedArray<Page>`, carrega a *backing track* e
  faz o *layout* da página ativa. **Não** tem mais `backButton`/`playButton`/`stopButton` — o
  transporte é comandado pelo `LaunchPadComponent` (`hasBackingTrack()`, `playBackingTrack()`,
  `stopBackingTrack()`).
  - Com `kitResource` vazio, monta a **grade fantasma** (`buildEmptyTemplate()`), sem áudio.
  - **`resized()`**: faixa de páginas de 34 px (**só se o kit tiver mais de uma página**) e a grade
    (`kit.columns × kit.rows`). O rodapé de 70 px agora pertence ao `LaunchPadComponent`.
  - **Choke groups (RF02)**: `handlePadTriggered()` interrompe os pads do mesmo grupo — inclusive
    de outras páginas do mesmo kit — via `PadComponent::chokeStop()`.
  - Trocar de página **não recria** `AudioTransportSource`s: apenas alterna visibilidade/layout e
    interrompe samples da página que saiu.
- **`MenuComponent`**: menu introdutório com dois `TextButton`s públicos (`launchPadButton` e
  `settingsButton`) e o título desenhado em `paint()`.
- **`SettingsComponent`**: `masterVolumeSlider`, `backingTrackSlider` (vertical), labels e
  `backButton`. ⚠️ **Os sliders são puramente visuais — não estão ligados a ganho algum.**
  Conectar é item explícito do roadmap (Pad Editor).

### 4.4 Módulos planejados (ainda **não** existem no código)

O SDD §4.1 prevê estes componentes. Não os presuma implementados:

| Módulo | Responsabilidade prevista |
| --- | --- |
| `PadEditorComponent` | Edição de volume, *pitch*, envelopes e choke groups |
| `AudioRecorderComponent` | Captura de samples via microfone |
| `KitSerializer` | Serialização/exportação de kits em JSON (e opcional `.zip`) |

> Nota: **choke groups** já estão implementados em `PadGridComponent`/`PadComponent` e vêm do
> JSON. O que falta do `PadEditorComponent` é a **edição** desses parâmetros pela UI.

---

## 5. Assets: `BinaryData`, JSON e *mangling* de nomes

Os assets são **embutidos no binário** durante o build (via Projucer), não lidos do disco em
tempo de execução. Acesso sempre por `BinaryData::getNamedResource(...)`.

### 5.1 Regra de nomenclatura de recursos

O Projucer faz *mangling*: **`.` → `_`** (mantendo maiúsculas/minúsculas). O código replica a
regra antes da busca:

```cpp
juce::String resourceName = audioFileName.replaceCharacter('.', '_');
const char* data = BinaryData::getNamedResource(resourceName.toUTF8(), dataSize);
```

Exemplos: `C.mp3` → `C_mp3`; `Musica_Jogo.wav` → `Musica_Jogo_wav`;
`notes.json` → `notes_json`; `drumkit.json` → `drumkit_json`.

> ⚠️ Ao adicionar um asset, registre-o no `PucPads.jucer` (`compile="0" resource="1"`) e
> regenere pelo Projucer — senão ele **não** existe em `BinaryData`.

### 5.2 Formato do JSON de kit (schema **v2**)

Especificação completa: **`PucPads/Arquivos_PucPads/KIT_SCHEMA.md`**. Resumo:

```json
{
  "schemaVersion": 2,
  "kit": { "name": "Kit Melodia", "author": "PucPads",
           "description": "...", "backingTrack": "Musica_Jogo.wav" },
  "grid": { "columns": 8, "rows": 8 },
  "pages": [
    { "name": "Melodia", "pads": [
        { "note": "C", "audioFile": "C.mp3", "function": "melodia", "playMode": "gate" },
        { "note": "D", "audioFile": "D.mp3", "function": "melodia", "playMode": "gate" }
    ] }
  ]
}
```

| Campo do pad | Efeito |
| --- | --- |
| `audioFile` | **Obrigatório**; nome do asset (mangling `.` → `_`). |
| `note` | Rótulo curto desenhado no pad. |
| `function` | `efeito`/`loop`/`melodia`/`harmonia` → cor pela paleta (§6.2). |
| `color` | ARGB hex; **tem prioridade** sobre `function`. |
| `playMode` | `gate` (default) ou `oneshot`. |
| `chokeGroup` | `0`/ausente = sem grupo; `> 0` = exclusão mútua. |
| `gainDb` | Ganho individual (−60…+12 dB; default `0`). |

Regras que **não** podem ser quebradas:
- Ordem dos pads = ordem de exibição (linha a linha).
- `schemaVersion` ausente ⇒ arquivo é tratado como v1.
- Campos desconhecidos são ignorados (permite evoluir sem quebrar leitor antigo).
- **`BinaryData` precisa ser regenerado no Projucer** depois de qualquer edição de JSON (§5.4).

### 5.3 Estado dos kits e Git LFS

- `notes.json` (Kit Melodia): páginas **Melodia** (`melodia`/gate) e **Harmonia**
  (`harmonia`/one-shot/choke group 1), 64 pads cada.
- `drumkit.json` (Kit Bateria): páginas **Loops** (`loop`/one-shot/choke group 2) e **Efeitos**
  (`efeito`/one-shot), 64 pads cada.
- Os dois kits usam **os mesmos 5 samples** (C–G, placeholders) — a diferença hoje é
  organizacional/semântica, não de conteúdo sonoro. Samples reais são débito de conteúdo (§13.2).
- `.gitattributes` marca `*.mp3`, `*.wav`, `*.aiff` como **Git LFS**. No working tree esses
  arquivos são *ponteiros* (~130 bytes); os bytes reais estão em `BinaryData.cpp` (comitado, ~47 MB).
- Para ouvir/inspecionar os samples reais: `git lfs pull`. O **build não depende** de LFS.

### 5.4 Após editar qualquer JSON de kit

O app lê os bytes **embutidos**, não o arquivo em disco:

1. `python3 tools/validate_kits.py` (schema + assets);
2. abrir `PucPads.jucer` no Projucer e **Save Project** (regenera `BinaryData.cpp/h`);
3. rebuild no IDE.

Sem o passo 2 o app continua lendo o kit antigo — sintoma clássico de "mudei e nada mudou".

---

## 6. Cores: identidade institucional × semântica de pads

Este é o ponto de maior risco de interpretação errada ao implementar UI. Existem **duas camadas**
de cor distintas:

### 6.1 Cromo da interface (implementado, **manter**)

RNF03 do SDD define *Dark Mode* de alto contraste na paleta institucional PUCPR:

| Uso | Valor |
| --- | --- |
| Bordô (botões) | `ff420012` |
| Bordô profundo (fundo de telas) | `ff2a000b` |
| Dourado neon (texto/destaque) | `ffffd700` |
| Dourado âmbar | `ffffc107` |
| Borda dourada dos pads | `ffb08d35` |

### 6.2 Semântica de cores dos pads (RF04 — **implementada**)

O SDD define um código de cores pedagógico. Ele é resolvido pelo parser
(`KitDefinition::colourForFunction`) e usado pelo `PadComponent`:

| Cor | Função musical | Hex |
| --- | --- | --- |
| **Verde** | Efeitos (SFX) | `ff2ecc71` |
| **Azul** | Loops | `ff1e90ff` |
| **Rosa** | Melodia | `ffff4fa3` |
| **Laranja** | Harmonia | `ffff8c1a` |
| *Fallback* | Sem `function` (ou desconhecida) | `ffffd700` (dourado institucional) |

Precedência: **`color` explícito** > `function` > fallback dourado. Ou seja, um pad pode manter uma
cor fora da paleta sem perder a classificação funcional.

> ⚠️ `juce::Colour::fromString` espera **ARGB** (`AARRGGBB`), não `RGBA`. O alfa vem primeiro
> (`ff` = opaco). Erro aqui gera cor transparente/quase preta, não erro de compilação.
>
> ⚠️ O cromo institucional (bordô `ff420012`, dourado `ffffd700`, borda `ffb08d35`) continua
> valendo para **moldura/botões**. As cores semânticas são apenas o preenchimento ativo dos pads.

---

## 7. Convenções de Código e Estilo

Siga de perto o **estilo padrão do JUCE** (é o que o projeto usa):

- **Indentação:** 4 espaços; chaves estilo *Allman*.
- **Namespace:** sempre qualifique `juce::` (`juce::Colour`, `juce::String`).
- **Nomes:** classes em `PascalCase` (`PadGridComponent`); métodos/variáveis/membros em
  `camelCase` (`showLaunchPadScreen`, `currentFade`); **sem** prefixo `m_`.
- **Headers:** `#pragma once`.
- **Macro obrigatória:** encerre classes JUCE com
  `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassName)`.
- **Posse de recursos:** `std::unique_ptr` e `juce::OwnedArray`.
- **Listeners:** registre com `addListener` e **remova sempre** no destrutor.
- **Idioma:** comentários, mensagens de commit e textos de UI em **português** (o código atual
  usa textos sem acento na UI, ex.: `"MIXER & CONFIGURACOES"`; mantenha o padrão do arquivo).
- **Debug:** `DBG(...)` para log de carregamento de kits (padrão de `PadGridComponent`).
- **Strings UTF-8:** use `juce::String::fromUTF8(...)` quando houver acentos.

### 7.1 Regras extras para código de áudio (RNF01/RNF04)

- **Nada de alocação dinâmica, locks ou I/O dentro do `getNextAudioBlock()`.** Prepare fontes em
  `prepareToPlay`/construtor e apenas misture no *callback*.
- Ao implementar *fade-out* de choke group (15–30 ms), faça o ganho ser aplicado **no caminho de
  áudio**, nunca com um `Timer` de UI controlando o volume amostra a amostra.
  - Implementação atual: `PadComponent::chokeStop()` chama `transportSource.setGain(0)` (o
    `AudioTransportSource` aplica o ganho com rampa interna de 1 bloco) e agenda o `stop()` real
    ~25 ms depois via `juce::Timer::callAfterDelay`. É um *fade* de granularidade de bloco
    (~5–10 ms), suficiente para eliminar *clicks*, mas **não** é um envelope amostra a amostra.
  - Se o produto exigir *fade* exato, a evolução é um `AudioSource` próprio com
    `juce::LinearSmoothedValue` por voz (débito em §13.2).
- Revisar `setAudioChannels(0, 2)` apenas quando a captura por microfone (RF05) for implementada;
  ao mudar, valide que `prepareToPlay` continua sendo chamado em ambas as plataformas.

### 7.2 Fluxo de edição recomendado

1. Edite **apenas** `PucPads/Source/` e `PucPads/Arquivos_PucPads/` (e docs na raiz).
2. Adicionou/removeu arquivo do projeto? Atualize o `PucPads.jucer`.
3. Abra e salve no **Projucer** para regenerar `JuceLibraryCode/` e os projetos de IDE.
4. Compile na IDE e valide em desktop e, quando o item for de áudio/UI tátil, em Android.

---

## 8. Build e Execução

> O build **não é feito por linha de comando**: depende de Projucer + IDE. Referência oficial:
> [`README.md`](README.md) §5.2 (Desktop) e §5.3 (Android).

### 8.1 Desktop (Windows / VS2022)
1. Obtenha o **JUCE 8.0.10** (clone oficial; o ZIP da *release* também inclui o Oboe vendorizado) e abra o **Projucer**.
2. `File > Global Paths` → **Path to JUCE** = pasta que contém `modules/`. É a **única** fonte do caminho dos
   módulos: o `.jucer` usa `useGlobalPath="1"` e não guarda `<MODULEPATHS>` (ver §8.2).
3. Abra `PucPads/PucPads.jucer` e dê **Save Project** (regenera `JuceLibraryCode/` e `Builds/`).
4. Exportador **Visual Studio 2022** → *Save and Open in IDE* → compile em
   `Builds/VisualStudio2022/`.

### 8.2 Android
1. No Android Studio (SDK Manager), instale **NDK (side by side)** e **CMake**.
2. No Projucer, configure **Path to JUCE**, SDK e NDK em `File > Global Paths`.
3. **Save Project** para gerar o Gradle em `Builds/Android`, abra a pasta no Android Studio, conecte o
   dispositivo com Depuração USB e execute o build.

> ⚠️ **Caminho do projeto tem que ser ASCII.** Com acento/cedilha no caminho (ex.:
> `D:\Projetos\Programação\PucPads`), o Android Gradle Plugin aborta a avaliação de `:app` com
> `Your project path contains non-ASCII characters`. Não é erro de código e nenhum ajuste em
> `Source/` resolve — o build nem começa. Correções, com prós e contras, em
> [`README.md`](README.md) §5.4. Depois de mover a pasta, apague `Builds/Android/.gradle`,
> `app/.cxx`, `app/build` e `local.properties` (guardam caminhos absolutos antigos).


> ⚠️ **Caminho dos módulos JUCE = `File > Global Paths`.** O `.jucer` usa `useGlobalPath="1"` em todos os
> módulos e **não** tem `<MODULEPATHS>` (até o commit anterior, os exportadores traziam
> `../../../Software/JUCE/modules` — caminho da máquina do autor — e `juce_audio_processors_headless`
> apontava para um *fork* de faculdade). Sem o **Path to JUCE** configurado, o Projucer acusa módulo não
> encontrado; configurado, todos os exportadores resolvem o mesmo JUCE 8.0.10. Se o Android falhar com
> `CXX1429 ... add_subdirectory given source ".../juce_audio_devices/native/oboe" which is not an existing
> directory`, o caminho do JUCE está errado/antigo ou o `Builds/Android` foi gerado antes da correção:
> **Save Project** + limpar `app/.cxx`, `app/build`, `.gradle` e `local.properties`. Diagnóstico completo em
> [`README.md`](README.md) §5.5.
>
> ℹ️ `juce_audio_processors_headless` **não existe** no JUCE oficial (era módulo de *fork*, sem uso em
> `Source/`) e foi removido do projeto. Não o reabilite sem antes versioná-lo no repositório.

### 8.3 Limitação do sandbox de agente (DevBox)

O container (`devbox-base`) **não** traz JUCE, Projucer, MSVC nem NDK. O build oficial do app
continua dependendo de Projucer + IDE. O que **é** possível validar aqui:

| Verificação | Como |
| --- | --- |
| Dados dos kits | `python3 tools/validate_kits.py` |
| Parser C++ de verdade | `JUCE_DIR=... ./tools/run_kit_parser_test.sh` (clona/baixa JUCE 8 + libs de sistema) |
| Compilação dos componentes de UI | `g++ -c -fsyntax-only` com `-I PucPads/JuceLibraryCode -I $JUCE/modules` |
| Estrutura do `.jucer`, docs, git | inspeção |

- O teste do parser **executa código real** (não é *stub*) e já foi usado para validar o schema
  v2 + compatibilidade v1.
- O binário do app (GUI + ALSA/Android) **não** é gerado aqui: exigiria o *build* completo do
  projeto pelo Projucer. **Nunca** declare "compila e funciona na IDE" sem build real.

---

## 9. Guias rápidos de tarefas comuns

### Adicionar um sample/pad
1. Arquivo em `PucPads/Arquivos_PucPads/`.
2. Registre no `PucPads.jucer` (`compile="0" resource="1"`).
3. Referencie no JSON (`"audioFile": "Novo.mp3"`) na página desejada.
4. `python3 tools/validate_kits.py` → **Save no Projucer** → recompile.

### Criar um novo kit (novo template de Launch Pad)
1. Copie `drumkit.json` → `NovoKit.json` em `Arquivos_PucPads/`.
2. Ajuste `kit`, `grid`, `pages` e os pads (schema em `KIT_SCHEMA.md`).
3. Registre no `.jucer` (`compile="0" resource="1"`) e regenere.
4. Em `LaunchPadComponent.cpp`, aponte um slot de `kTemplateKitResources` para
   `"NovoKit_json"` (lembre do *mangling* `.` → `_`). **Nada** muda no `MainComponent`.

### Adicionar uma página a um kit existente
- Acrescente um objeto `{ "name": "...", "pads": [...] }` em `pages`.
- `name` deve ser único e curto (vira rótulo do botão da faixa de páginas, logo abaixo do cabeçalho).
- Nenhuma mudança de código é necessária: o número de botões é derivado de `pages.size()`.
- Cada página suporta até `grid.columns × grid.rows` pads.

### Ajustar a grade de pads
A geometria está em `PadGridComponent::layoutPage()`. Agora ela usa **`kit.columns`/`kit.rows`**
(default 8 × 8, vindos do JSON) e só posiciona os pads existentes. Cabeçalho: 60 px (no
`LaunchPadComponent`); faixa de páginas: 34 px, só quando há mais de uma página; rodapé de
templates: 70 px. Para mudar a densidade, prefira editar o `grid` do JSON a mexer no código.

### Templates de LaunchPad (rodapé) — **implementado**
- O rodapé do `LaunchPadComponent` tem **8 botões numerados**; cada um é um template (um kit).
- Para dar um kit a outro template, basta preencher o slot em `kTemplateKitResources`
  (`LaunchPadComponent.cpp`) com o *resource name* mangled do JSON (ex.: `"drumkit_json"`).
  Se o JSON ainda não existir no `BinaryData`, o template simplesmente abre vazio.
- Templates sem kit exibem a grade fantasma (`PadGridComponent::buildEmptyTemplate()`), que é o
  ponto de partida do futuro editor.
- Ao criar/remover templates, ajuste `numTemplates` **e** o array (o teste
  `tools/run_launchpad_test.sh` cobre a contagem de botões).

### Navegação multi-páginas (RF01) — **implementado**
- `KitDefinition` entrega `pages[]`; `PadGridComponent` cria um `Page` (`OwnedArray<PadComponent>`)
  por página em `buildPages()`, gera um botão por página em `buildPageButtons()` e alterna em
  `showPage(i)`. Os botões vivem na **faixa de 34 px abaixo do cabeçalho** (o rodapé passou a ser
  dos templates).
- Trocas **não** recriam `AudioTransportSource`s — apenas `setVisible` + `layoutPage()`.
- A aba ativa é sinalizada invertendo bordô/dourado (§6.1); kits de página única não mostram botões.
- Ao estender (ex.: gesto de swipe, indicador "2/5"), mantenha `showPage()` como único ponto de
  troca de estado.

### Choke groups (RF02) — **implementado**
- `chokeGroup` vem do JSON; o escopo é o **kit inteiro** (não só a página visível).
- `PadGridComponent::handlePadTriggered()` interrompe os demais pads do grupo *antes* de o pad
  disparado começar a tocar (evita o auto-choke).
- `PadComponent::chokeStop()` faz o *fade* e agenda o `stop()`; um `chokeToken` invalida o stop
  pendente se o pad for reacionado dentro da janela de *fade*.
- **Sem limite de 6 grupos** (diferencial declarado vs. concorrente).

---

## 10. Rastreabilidade: Requisitos ↔ Código

Tabela de apoio para localizar onde cada requisito do SDD vive (ou viverá). Atualize-a ao
implementar.

| Req. | Descrição resumida | Onde está / onde vai |
| --- | --- | --- |
| RF01 | Disparo polifônico + múltiplas páginas/templates | Polifonia: `MixerAudioSource` (feito). Páginas: `KitDefinition::pages` + `PadGridComponent::showPage()` (faixa de 34 px). Templates: `LaunchPadComponent` + rodapé com 8 botões (feito) |
| RF02 | Choke groups ilimitados, fade-out 15–30 ms | **Feito** — `KitPad::chokeGroup` → `PadGridComponent::handlePadTriggered()` → `PadComponent::chokeStop()` (fade de 1 bloco + stop em 25 ms) |
| RF03 | Pad Editor (volume, pitch, envelopes, trim, clonar/remover) | **A implementar** (`PadEditorComponent`, `juce::ResamplingAudioSource`). `gainDb` por pad já é lido do JSON |
| RF04 | Cores semânticas por função musical | **Feito** — `KitDefinition::colourForFunction()` + `function`/`color` no JSON (§6.2) |
| RF05 | Importar arquivos e gravar pelo microfone | **A implementar** (`juce::FileChooser`; gravação WAV; revisar `setAudioChannels`) |
| RF06 | Backing track + salvar kits em JSON com metadados | Backing track: feito. Metadados (`kit.name/author/description`): **feito** no schema v2. Serialização/export (`KitSerializer`): **a implementar** |
| RF07 | MIDI externo + compartilhamento de kits | **A implementar** (nenhum código MIDI hoje) |
| RNF01 | Baixa latência | Manter `MixerAudioSource`/buffers; não alocar na thread de áudio |
| RNF02 | Multiplataforma + multitouch | `resized()` dinâmico; `mouseDown/Up` já respondem a múltiplos ponteiros |
| RNF03 | Dark Mode bordô/dourado | `UiTheme.h` (fonte única) consumida por `MenuComponent`, `LaunchPadComponent`, `PadGridComponent`, `PadComponent`, `SettingsComponent` |
| RNF04 | Gestão segura de memória | `unique_ptr`, `OwnedArray`, `removeInputSource` nos destrutores, `SafePointer` no *fade* assíncrono |

---

## 11. Testes e Verificação

Não existe CI nem suíte rodando na IDE, mas há **duas verificações automatizadas** em `tools/`:

| Comando | Cobre | Roda no sandbox? |
| --- | --- | --- |
| `python3 tools/validate_kits.py` | schema, `audioFile` existente, faixas de `gainDb`/`chokeGroup`, nomes de página duplicados, capacidade da grade, paleta | ✅ |
| `JUCE_DIR=... ./tools/run_kit_parser_test.sh` | parser C++ real: kits atuais, regras de cor, `playMode`, `chokeGroup`, `gainDb`, grade customizada, descarte de pad inválido, **compatibilidade v1** | ✅ (requer baixar JUCE + libs) |
| `JUCE_DIR=... ./tools/run_launchpad_test.sh` | tela de Launch Pad *headless*: 8 botões de template, template 1 com 128 pads/2 páginas, grade fantasma 8×8 nos templates vazios, play/stop habilitado só com base válida, *layout* (60/70 px) e cache de views | ✅ (requer baixar JUCE + libs) |

- **Adicione caso de teste** ao `kit_parser_smoke_test.cpp` sempre que o schema ganhar campo novo.
  Ele é um alvo de console (módulos core/events/graphics), não faz parte do binário do app.
- Verificação **manual** continua necessária para UI/áudio: compilar na IDE e testar
  clique/multitouch no desktop e no Android.
- Checklist mínimo antes de entregar: carregar o template 1; alternar entre os 8 templates
  (voltar ao 1 deve manter o kit carregado); trocar de página com sample tocando (o som deve ser
  interrompido); disparar pads do mesmo choke group em sequência (sem *click*); play/stop da
  *backing track* (desabilitado nos templates sem kit); navegar entre as três telas e voltar; sem
  *crash* ao trocar de template/página repetidamente (leak detector do JUCE em Debug).

---

## 12. Git, Branches e Workflow

### Branches protegidas — **NUNCA** trabalhe diretamente nelas
`main`, `master`, `bugfix`, `hotfix`, `homolog`, `sabesp`.

### Fluxo recomendado
1. Crie branch a partir de `main` (`devbox/<descricao>` ou `feature/<descricao>`).
2. Commits pequenos, em **português**, no estilo do histórico
   (*"Adicionando tela de Mixer e arrumando as telas de pad"*, *"Revisão README"*).
   Não há *Conventional Commits* no projeto.
3. **Sempre** `push` após o commit (sincronismo remoto e sobrevivência do workspace).
4. Abra *Pull Request* para revisão (o histórico já tem PRs, ex.: `#1`).

> O workspace DevBox expira em ~30 min. Commit + push são obrigatórios antes de encerrar.

---

## 13. Roadmap Técnico e Débitos Conhecidos

### 13.1 Próximas sprints (SDD §6.2) — prioridade oficial
1. ~~**Navegação multi-páginas + semântica de cores** nos kits padrão.~~ ✅ **Concluído**
   (schema v2, `PadGridComponent::showPage()`, paleta RF04 nos dois kits).
2. ~~**Motor de choke groups** (ilimitado, *fade-out* 15–30 ms sem *clicks*).~~ ✅ **Concluído**
   (fade de ~1 bloco + stop agendado; envelope amostra a amostra segue como débito §13.2.5).
3. **Pad Editor:** faders reais (Master, Backing, volume por pad), *pitch* em tempo real,
   envelopes, clonar/remover/trim.
4. **Criação de conteúdo:** importação por `juce::FileChooser` e gravação via microfone.
5. **Persistência e compartilhamento:** serializar kit (nome do kit, música, usuário, mapeamento)
   em JSON; opcional pacote `.zip`.

### 13.2 Débitos e armadilhas verificados no código atual
1. **Sliders de `SettingsComponent` sem efeito** — UI apenas; ligação ao mixer é pendente (RF03).
   `KitPad::gainDb` já existe no schema; falta a UI de edição.
2. **Conteúdo dos kits continua genérico** — as páginas agora diferem em função/choke group, mas
   os dois kits ainda usam os mesmos 5 samples (C–G). Falta material sonoro real de bateria/loops.
3. **Todas as páginas são instanciadas no load** — 128 `AudioTransportSource` por kit hoje são
   inofensivos (samples de 45 KB). Com kits reais e muitas páginas, migrar para *pool de vozes*
   com carga por página (§`KIT_SCHEMA.md` §15).
4. **`BinaryData.cpp` com ~47 MB comitado** — infla o repositório; avaliar regeneração fora do
   git ou LFS.
5. **Fade de choke é de granularidade de bloco** (`setGain` + stop em 25 ms), não envelope
   amostra a amostra. Aceitável hoje; revisar se houver requisito de precisão.
6. **Ciclo de vida assimétrico das telas:** `showMenuScreen()` **destrói** a `launchPadScreen`
   (`reset()`, o que também derruba os views em cache dos templates), mas `settingsScreen` é criada
   uma única vez e **nunca** é destruída (apenas escondida) — daí o `addAndMakeVisible` em toda
   entrada. Consequência: sair do LaunchPad **recria todos os pads e recarrega o áudio** na próxima
   entrada.
7. **`playMode` é por pad, não por kit** — se o produto definir um modo global por kit, avaliar
   um `kit.defaultPlayMode` (aditivo, retrocompatível).
8. **Módulos JUCE resolvidos por máquina** (`useGlobalPath`, §8.2): quem clonar sem configurar
   `File > Global Paths` verá "module not found". Mitigação possível: versionar o JUCE como submódulo
   (`third_party/JUCE`) e documentar o *bootstrap*.
9. **Parser valida estrutura, não conteúdo sonoro** — `audioFile` inexistente é descartado com
   `DBG`, mas a UI não avisa o usuário que um pad "sumiu". Considerar indicador visual de pad
   inválido no Pad Editor.
10. **Sem CI** — as verificações de `tools/` são locais (executáveis, mas ninguém as roda
    automaticamente). Um workflow simples de CI cobrindo `validate_kits.py` é barato.
11. **Templates declarados em código** — os 8 slots (nome + kit) vivem em
    `kTemplateKitResources` (`LaunchPadComponent.cpp`). Quando os templates passarem a ser
    criados/editados pelo usuário (RF06), extrair para um manifesto JSON no `BinaryData` ou para
    preferências (`ApplicationProperties`), mantendo os 8 slots como *default*.
12. **Templates 2–8 e `drumkit.json` sem uso** — o kit "Bateria" continua no `BinaryData`, mas
    nenhum template o referencia hoje. É o candidato natural a template 2 quando o conteúdo sonoro
    real chegar.

---

## 14. Regras de Ouro para Agentes

1. **Leia o SDD (`README.md`) antes de propor features.** O escopo é PIBITI 2026–2027; não
   invente requisitos nem antecipe módulos fora do roadmap sem perguntar.
2. **Diferencie "existe" de "planejado":** use a tabela do §10. Não descreva como implementado
   algo que está apenas no roadmap.
3. **Não edite `JuceLibraryCode/` nem `Builds/`** manualmente.
4. **Sem novas dependências** sem autorização expressa.
5. **Não quebre retrocompatibilidade:** JSONs de kit, APIs públicas internas (`*Button` públicos)
   e o contrato de `removeInputSource` nos destrutores.
6. **Nunca** atue em branches protegidas; use branch de trabalho + push.
7. **Nunca** exponha segredos (tokens, senhas, chaves). Não há segredos no repo — mantenha assim.
8. **Idioma:** comentários, commits e UI em **português**.
9. **Cores:** cromo institucional bordô/dourado permanece (RNF03); pads usam a semântica RF04
   (`function` da paleta) com possibilidade de override por `color` — não "endureça" hex solto
   no código de UI: cor de pad vem do kit.
10. **Áudio:** nada de alocação/lock no `getNextAudioBlock()`; fades no caminho de áudio.
11. **Schema de kit:** qualquer campo novo deve ser **opcional com default** e coberto por caso
    no `tools/kit_parser_smoke_test.cpp` + `tools/validate_kits.py`. Nunca remova campo existente
    sem versionar.
12. **Deixe rastro:** ao finalizar, reporte workspace ID, branch, commit e status do push.

---

## 15. Histórico deste documento

| Data | Commit base do SDD | Mudanças |
| --- | --- | --- |
| 2026-11-25 | `5740be2` (*Revisão README*) | **Schema v2 + multi-páginas + choke groups:** novo `KitDefinition` (parser retrocompatível v1↔v2), páginas com botões no rodapé (§4.3/§9), paleta RF04 aplicada (§6.2), guias de edição de kit e regeneração de `BinaryData` (§5.2–§5.4), verificações automatizadas em `tools/` (§11), roadmap/débitos atualizados (§13) |
| 2026-11-25 | `5740be2` (*Revisão README*) | Revisão completa: enquadramento PIBITI, RF/RNF renumerados, semântica de cores RF04 (§6), módulos planejados (`PadEditorComponent`, `AudioRecorderComponent`, `KitSerializer`), tabela de rastreabilidade requisito↔código (§10), diretrizes de áudio em tempo real (§7.1), roadmap oficial (§13.1) e guias para choke groups/multi-páginas (§9) |
| 2026-11-24 | `c194cc7` | Criação inicial do guia (arquitetura, assets/`BinaryData`, build, convenções, débitos) |
