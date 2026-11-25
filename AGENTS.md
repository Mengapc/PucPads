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
| Módulos JUCE | `juce_audio_basics`, `juce_audio_devices`, `juce_audio_formats`, `juce_audio_processors`, `juce_audio_processors_headless`, `juce_audio_utils`, `juce_core`, `juce_data_structures`, `juce_events`, `juce_graphics`, `juce_gui_basics`, `juce_gui_extra` |
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
    │   ├── PadGridComponent.{h,cpp}    # Grade 8x8 + backing track + JSON
    │   ├── PadComponent.{h,cpp}        # Pad individual (sample + fade visual)
    │   └── SettingsComponent.{h,cpp}   # Mixer & Configurações (UI)
    ├── JuceLibraryCode/      # >>> GERADO PELO PROJUCER — não editar manualmente
    │   ├── JuceHeader.h
    │   ├── BinaryData.{h,cpp}          # Assets embutidos (~47 MB) — ver §5
    │   └── include_juce_*.{cpp,mm,c}
    └── Arquivos_PucPads/     # Assets de origem
        ├── C.mp3 D.mp3 E.mp3 F.mp3 G.mp3
        ├── Musica_Jogo.wav
        ├── notes.json        # kit "Melodia"
        └── drumkit.json      # kit "Bateria"
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
  | `menuScreen` | `MenuComponent` | Menu inicial (Kit Melodia / Kit Bateria / Mixer) |
  | `padGridScreen` | `PadGridComponent` | Grade de performance |
  | `settingsScreen` | `SettingsComponent` | Mixer & Configurações |

- Troca de tela por `showMenuScreen()`, `showPadScreen(kitFile)` e `showSettingsScreen()`.
- Roteia cliques em `buttonClicked(juce::Button*)` comparando ponteiros
  (ex.: `button == &menuScreen->kitMelodiaButton`).

**Dois padrões de callback coexistem** — respeite o do arquivo que estiver editando:

1. `backButton` (Grade e Settings) → `addListener(this)` + `buttonClicked()` no `MainComponent`.
2. `playButton`/`stopButton` (Grade) → lambdas em `onClick`, definidos dentro do próprio
   `PadGridComponent`.

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

- **`PadComponent`** (`Component` + `Timer`): carrega sample da memória (`BinaryData`), toca em
  `mouseDown`, para em `mouseUp`, e anima *fade* da cor ativa a **60 Hz**
  (`currentFade -= 0.05f`), produzindo o efeito neon. Semântica atual = **gate** (toca enquanto
  pressionado), não *one-shot*.
- **`PadGridComponent`**: lê o JSON do kit via `BinaryData::getNamedResource`, cria os
  `PadComponent`s em `juce::OwnedArray`, carrega a *backing track* e faz o *layout* da grade
  em `resized()`. Expõe `backButton`, `playButton`, `stopButton`.
  - **`resized()` reserva uma `footerArea` de 70 px que hoje não é usada** — é o espaço previsto
    para a futura navegação multi-páginas (RF01 / roadmap §6.2 do SDD).
- **`MenuComponent`**: três `TextButton`s públicos (`kitMelodiaButton`, `kitBateriaButton`,
  `settingsButton`).
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

### 5.2 Formato atual do JSON de kit

```json
{
  "backingTrack": "Musica_Jogo.wav",
  "pads": [
    { "note": "Pad_1", "audioFile": "C.mp3", "color": "ffffd700" },
    { "note": "Pad_2", "audioFile": "D.mp3", "color": "ffffc107" }
  ]
}
```

- `backingTrack`: nome do arquivo da faixa base (opcional).
- `pads[]`: ordem de exibição (preenchimento por linha, 8 colunas).
- `color`: **ARGB hex** lido por `juce::Colour::fromString` (ex.: `ffffd700` dourado;
  `ff420012` bordô).
- `note`: hoje é apenas um rótulo; **não** há mapeamento MIDI real.

**Evolução prevista (RF06 / roadmap):** o schema ganhará metadados de kit —
`nome do kit`, `nome da música (playback)` e `nome do autor/usuário` — além do mapeamento de
pads. Ao implementar `KitSerializer`, mantenha **retrocompatibilidade** com os JSONs atuais
(campos novos opcionais) ou versione o schema explicitamente.

### 5.3 Estado dos kits e Git LFS

- `notes.json` e `drumkit.json` são **byte a byte idênticos** (64 pads cada, mesmos samples
  C–G e mesma *backing track*). O "Kit Bateria" ainda não difere do "Kit Melodia".
- `.gitattributes` marca `*.mp3`, `*.wav`, `*.aiff` como **Git LFS**. No working tree esses
  arquivos são *ponteiros* (~130 bytes); os bytes reais estão em `BinaryData.cpp` (comitado, ~47 MB).
- Para ouvir/inspecionar os samples reais: `git lfs pull`. O **build não depende** de LFS.

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

### 6.2 Semântica de cores dos pads (RF04 — **alvo**, ainda não aplicada)

O SDD define um código de cores pedagógico para os pads:

| Cor | Função musical |
| --- | --- |
| **Verde** | Efeitos (SFX) |
| **Azul** | Loops |
| **Rosa** | Melodia |
| **Laranja** | Harmonia |

> ⚠️ **Divergência consciente:** os kits atuais usam apenas dourado/âmbar/branco
> (`ffffd700`, `ffffc107`, `ffffffff`) tanto para cromo quanto para pads. Migrar os pads para a
> paleta semântica **altera a identidade visual dos kits existentes** e é item de roadmap
> (SDD §6.2). Não faça essa troca "de passagem" dentro de outra tarefa: trate-a como mudança
> própria, validada com o responsável pelo produto, e atualize os JSONs **e** a documentação
> no mesmo commit.

---

## 7. Convenções de Código e Estilo

Siga de perto o **estilo padrão do JUCE** (é o que o projeto usa):

- **Indentação:** 4 espaços; chaves estilo *Allman*.
- **Namespace:** sempre qualifique `juce::` (`juce::Colour`, `juce::String`).
- **Nomes:** classes em `PascalCase` (`PadGridComponent`); métodos/variáveis/membros em
  `camelCase` (`showPadScreen`, `currentFade`); **sem** prefixo `m_`.
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
- Ao implementar *fade-out* de choke group (15–30 ms), faça o ganho ser aplicado **na thread de
  áudio** (ex.: `AudioTransportSource`/`Gain` interno ou envelope próprio lido do buffer), nunca
  com `Timer` de UI.
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
1. Extraia o JUCE (ex.: `C:\JUCE`) e abra o **Projucer**.
2. Abra `PucPads/PucPads.jucer`; confira `File > Global Paths` (módulos JUCE).
3. Exportador **Visual Studio 2022** → *Save and Open in IDE* → compile em
   `Builds/VisualStudio2022/`.

### 8.2 Android
1. No Android Studio (SDK Manager), instale **NDK (side by side)** e **CMake**.
2. No Projucer, configure SDK/NDK em `File > Global Paths`.
3. Salve para gerar o Gradle em `Builds/Android`, abra a pasta no Android Studio, conecte o
   dispositivo com Depuração USB e execute o build.

> ⚠️ **MODULEPATHS inconsistentes no `.jucer`:** nos dois exportadores (VS2022 e Android), quase
> todos os módulos apontam para `../../../Software/JUCE/modules`, mas
> `juce_audio_processors_headless` aponta para `../../../../Softwares/Faculdade/JUCE/modules`
> (caminho da máquina do autor). Se o build falhar com "module not found", corrija pelos
> **Global Paths do Projucer** — não edite caminhos absolutos no `.jucer`.

### 8.3 Limitação do sandbox de agente (DevBox)

O container (`devbox-base`) **não** tem JUCE, Projucer, MSVC nem NDK. Portanto:

- **Não é possível buildar o app** dentro do sandbox.
- Validável aqui: estrutura do `.jucer` vs. recursos, integridade dos JSONs de kit, revisão
  estática de C++ (`g++ -fsyntax-only` só com *stubs*, e sem valor probatório), `git` e docs.
- **Nunca** declare "compila e funciona" sem build real na IDE — relate como não validado.

---

## 9. Guias rápidos de tarefas comuns

### Adicionar um sample/pad
1. Arquivo em `PucPads/Arquivos_PucPads/`.
2. Registre no `PucPads.jucer` (`compile="0" resource="1"`).
3. Referencie no JSON (`"audioFile": "Novo.mp3"`).
4. Regenere no Projucer e recompile.

### Criar um novo kit
1. Copie `notes.json` → `NovoKit.json` em `Arquivos_PucPads/`.
2. Ajuste `backingTrack` e `pads`.
3. Registre no `.jucer` e regenere.
4. Em `MainComponent`, crie o botão no menu e chame `showPadScreen("NovoKit_json")`
   (lembre do *mangling* `.` → `_`).

### Ajustar a grade de pads
A geometria está em `PadGridComponent::resized()`. O *layout* assume **8 colunas × 8 linhas**
fixas e só desenha os pads existentes (`for i < pads.size()`). Alterar `numCols`/`numRows`
muda a densidade. O cabeçalho tem 60 px e o **rodapé reserva 70 px** (ainda sem uso).

### Implementar navegação multi-páginas (RF01)
- O espaço do rodapé já está reservado em `resized()`.
- Sugestão de desenho: manter `pads` como array plano e calcular a página visível no *layout*,
  ou criar um contêiner de página por banco. Evite recriar `AudioTransportSource`s a cada troca
  de página (custo/latência); idealmente carregue o banco uma vez e apenas reposicione/ative.

### Implementar choke groups (RF02)
- Modelo: `int chokeGroup` por pad (vindo do JSON) + registro, no `PadGridComponent`, de qual pad
  está tocando por grupo. Ao acionar outro pad do mesmo grupo, aplicar *fade-out* de 15–30 ms
  no anterior e iniciar o novo.
- **Sem limite de 6 grupos** (diferencial declarado vs. concorrente).
- O *fade-out* deve ser no caminho de áudio (ver §7.1). Extensão de schema precisa ser
  retrocompatível (`"chokeGroup"` opcional, `0` = sem grupo).

---

## 10. Rastreabilidade: Requisitos ↔ Código

Tabela de apoio para localizar onde cada requisito do SDD vive (ou viverá). Atualize-a ao
implementar.

| Req. | Descrição resumida | Onde está / onde vai |
| --- | --- | --- |
| RF01 | Disparo polifônico + múltiplas páginas | Polifonia: `MixerAudioSource` (feito). Páginas: **planejado** em `PadGridComponent` (rodapé reservado) |
| RF02 | Choke groups ilimitados, fade-out 15–30 ms | **A implementar** (`PadComponent` + `PadGridComponent` + JSON) |
| RF03 | Pad Editor (volume, pitch, envelopes, trim, clonar/remover) | **A implementar** (`PadEditorComponent`, `juce::ResamplingAudioSource`) |
| RF04 | Cores semânticas por função musical | **Divergente hoje** — ver §6.2; afeta JSONs + `PadComponent` |
| RF05 | Importar arquivos e gravar pelo microfone | **A implementar** (`juce::FileChooser`; gravação WAV; revisar `setAudioChannels`) |
| RF06 | Backing track + salvar kits em JSON com metadados | Backing track: feito. Serialização: **`KitSerializer` a implementar** |
| RF07 | MIDI externo + compartilhamento de kits | **A implementar** (nenhum código MIDI hoje) |
| RNF01 | Baixa latência | Manter `MixerAudioSource`/buffers; não alocar na thread de áudio |
| RNF02 | Multiplataforma + multitouch | `resized()` dinâmico; `mouseDown/Up` já respondem a múltiplos ponteiros |
| RNF03 | Dark Mode bordô/dourado | Cores em `MenuComponent`, `PadGridComponent`, `SettingsComponent`, `PadComponent` |
| RNF04 | Gestão segura de memória | `unique_ptr`, `OwnedArray`, `removeInputSource` nos destrutores |

---

## 11. Testes e Verificação

- **Não existe** suíte de testes automatizados nem CI.
- Verificação atual é **manual**: compilar na IDE e testar interação (clique/multitouch) no
  desktop e no Android.
- Se for introduzir testes, prefira **`juce::UnitTest`** (nativo) para lógica pura — parser de
  JSON de kit, conversão de cores, cálculo de *layout* de páginas. **Combine antes** de adicionar
  frameworks de teste de terceiros.
- Checklist mínimo antes de entregar: carregar os dois kits; disparar pads (simultâneos);
  play/stop da *backing track*; navegar entre as três telas e voltar; sem *crash* ao trocar de
  kit/página repetidamente (verifique vazamentos com o *leak detector* do JUCE em Debug).

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
1. **Navegação multi-páginas + semântica de cores** nos kits padrão.
2. **Motor de choke groups** (ilimitado, *fade-out* 15–30 ms sem *clicks*).
3. **Pad Editor:** faders reais (Master, Backing, volume por pad), *pitch* em tempo real,
   envelopes, clonar/remover/trim.
4. **Criação de conteúdo:** importação por `juce::FileChooser` e gravação via microfone.
5. **Persistência e compartilhamento:** serializar kit (nome do kit, música, usuário, mapeamento)
   em JSON; opcional pacote `.zip`.

### 13.2 Débitos e armadilhas verificados no código atual
1. **Sliders de `SettingsComponent` sem efeito** — UI apenas; ligação ao mixer é pendente (RF03).
2. **`notes.json` == `drumkit.json`** — kits idênticos; "Kit Bateria" deveria diferir.
3. **Cores dos pads fora da semântica RF04** (dourado/âmbar), ver §6.2.
4. **`BinaryData.cpp` com ~47 MB comitado** — infla o repositório; avaliar regeneração fora do
   git ou LFS.
5. **Semântica de disparo é *gate*** (`mouseUp` → `transportSource.stop()`) — há comentário no
   código sobre "modo Gate". Definir com o produto se pads melódicos devem ser *one-shot*.
6. **Ciclo de vida assimétrico das telas:** `showMenuScreen()` **destrói** a `padGridScreen`
   (`reset()`) a cada retorno, mas `settingsScreen` é criada uma única vez e **nunca** é
   destruída (apenas escondida) — daí o `addAndMakeVisible` em toda entrada. Ao conectar os
   sliders ao mixer (RF03), cuidado para não acumular listeners/estado em `settingsScreen`,
   e lembre que os `AudioTransportSource` dos pads são recriados a cada entrada na grade.
7. **Comentário corrompido em `PadGridComponent.h`** (`// O mixer \tma referencia...`) — corrigir
   ao tocar no arquivo.
8. **MODULEPATH inconsistente no `.jucer`** (ver §8).
9. **Parser JSON sem validação de campos** — `audioFile`/`color` ausentes não têm tratamento
   explícito; `pads` inexistente apenas gera zero pads. Robustecer ao evoluir o schema (RF06).
10. **Sem CI/testes** — toda refatoração depende de validação manual.

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
9. **Cores:** cromo institucional bordô/dourado permanece (RNF03); pads migram para a semântica
   RF04 **somente** em tarefa dedicada e documentada.
10. **Áudio:** nada de alocação/lock no `getNextAudioBlock()`; fades em tempo de áudio.
11. **Deixe rastro:** ao finalizar, reporte workspace ID, branch, commit e status do push.

---

## 15. Histórico deste documento

| Data | Commit base do SDD | Mudanças |
| --- | --- | --- |
| 2026-11-25 | `5740be2` (*Revisão README*) | Revisão completa: enquadramento PIBITI, RF/RNF renumerados, semântica de cores RF04 (§6), módulos planejados (`PadEditorComponent`, `AudioRecorderComponent`, `KitSerializer`), tabela de rastreabilidade requisito↔código (§10), diretrizes de áudio em tempo real (§7.1), roadmap oficial (§13.1) e guias para choke groups/multi-páginas (§9) |
| 2026-11-24 | `c194cc7` | Criação inicial do guia (arquitetura, assets/`BinaryData`, build, convenções, débitos) |
