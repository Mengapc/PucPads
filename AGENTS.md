# AGENTS.md — Guia para Agentes de IA no PucPads

> Documento de orientação para agentes (e pessoas) que forem atuar neste repositório.
> Leia isto **antes** de escrever, refatorar ou propor mudanças. O objetivo é preservar
> a arquitetura, o estilo e a identidade visual já consolidados no projeto.
>
> Última atualização: 2026-10-09 (commit de referência: `c194cc7`).

---

## 1. Visão Geral

**PucPads** é um aplicativo de música (estilo *Launchpad*) desenvolvido em **C++ com o framework JUCE**,
criado como substituto acadêmico do descontinuado *SuperPads Lights* para o curso de Bacharelado em
Produção Musical da PUCPR.

O app apresenta uma **grade de pads (8x8 = 64 pads)** que disparam samples de áudio, com uma
*backing track* (faixa de base) controlável, feedback visual animado nos pads e identidade visual
institucional (Dark Mode bordô/dourado). A especificação completa está no [`README.md`](README.md)
(Software Design Document — SDD).

**Público-alvo do produto:** estudantes/professores de Produção Musical e Jogos Digitais.
**Plataformas-alvo:** Desktop (Windows) e Android.

---

## 2. Stack Tecnológica

| Item | Valor |
| --- | --- |
| Linguagem | C++ (padrão exigido pelo JUCE 8) |
| Framework | **JUCE 8.0.10 ou superior** |
| Gerador de projeto | **Projucer** (arquivo `PucPads/PucPads.jucer`) |
| IDE Desktop | Visual Studio 2022 (`MSVC v143`+, Windows 10/11 SDK) |
| IDE Mobile | Android Studio + Android SDK Platform 34 + NDK (side by side 26.x/27.x) + CMake |
| Módulos JUCE usados | `juce_audio_basics`, `juce_audio_devices`, `juce_audio_formats`, `juce_audio_processors`, `juce_audio_processors_headless`, `juce_audio_utils`, `juce_core`, `juce_data_structures`, `juce_events`, `juce_graphics`, `juce_gui_basics`, `juce_gui_extra` |
| Opções de projeto | `JUCE_STRICT_REFCOUNTEDPOINTER=1`, `JUCE_USE_MP3AUDIOFORMAT=1` |

**Não introduza novas dependências/frameworks** (bibliotecas de terceiros, CMake próprio, etc.)
sem autorização expressa. O projeto é intencionalmente dependente apenas do JUCE.

---

## 3. Estrutura do Repositório

```
PucPads/                      # Raiz do projeto (note: NÃO é a raiz do git)
├── PucPads.jucer             # Projeto do Projucer — fonte da verdade do build
├── Source/                   # >>> Código-fonte editável manualmente
│   ├── Main.cpp              # Entry point (JUCEApplication + MainWindow)
│   ├── MainComponent.{h,cpp} # Tela "raiz": áudio global + navegação entre telas
│   ├── MenuComponent.{h,cpp} # Tela de menu principal
│   ├── PadGridComponent.{h,cpp} # Tela de performance (grade 8x8 + backing track)
│   ├── PadComponent.{h,cpp}  # Componente de um pad individual
│   └── SettingsComponent.{h,cpp} # Tela "Mixer & Configurações"
├── JuceLibraryCode/          # >>> GERADO PELO PROJUCER — não editar manualmente
│   ├── JuceHeader.h
│   ├── BinaryData.{h,cpp}    # Assets embutidos (mp3/wav/json) — ver §5
│   └── include_juce_*.{cpp,mm,c}
└── Arquivos_PucPads/         # Assets de origem (samples, faixas e JSONs de kit)
    ├── C.mp3 D.mp3 E.mp3 F.mp3 G.mp3
    ├── Musica_Jogo.wav
    ├── notes.json
    └── drumkit.json
```

Na **raiz do repositório git** ficam apenas: `README.md`, `.gitignore`, `.gitattributes` e
este `AGENTS.md`.

> ⚠️ `JuceLibraryCode/` e as pastas `Builds/` são **saída do Projucer**. Nunca edite
> `BinaryData.cpp/h` nem `include_juce_*` à mão — qualquer "Save" no Projucer sobrescreve.
> Para alterar assets, mude os arquivos em `Arquivos_PucPads/` e regenere pelo Projucer.

---

## 4. Arquitetura

### 4.1 Modelo de telas (navegação)

`MainComponent` é um `juce::AudioAppComponent` **e** `juce::Button::Listener`. Ele:

- Mantém **um `juce::MixerAudioSource` global** (`mixerSource`) — o "barramento" de áudio
  de toda a aplicação. Pads e backing track são conectados/desconectados a ele.
- Gerencia três telas como `unique_ptr`:

  | Membro | Classe | Papel |
  | --- | --- | --- |
  | `menuScreen` | `MenuComponent` | Menu inicial (Kit Melodia / Kit Bateria / Mixer) |
  | `padGridScreen` | `PadGridComponent` | Grade de performance |
  | `settingsScreen` | `SettingsComponent` | Mixer & Configurações |

- Troca de tela via métodos `showMenuScreen()`, `showPadScreen(kitFile)` e
  `showSettingsScreen()`, que fazem `setVisible` + `add/removeChildComponent`.
- Centraliza o roteamento de cliques em `buttonClicked(juce::Button*)`, comparando ponteiros
  de botões (ex.: `button == &menuScreen->kitMelodiaButton`).

### 4.2 Fluxo de áudio

```
PadComponent::transportSource ─┐
                               ├──► MainComponent::mixerSource ──► getNextAudioBlock()
PadGridComponent::backgroundSource ┘
```

- `MainComponent::setAudioChannels(0, 2)` — **0 entradas, 2 saídas** (somente reprodução).
- Cada `PadComponent` cria seu próprio `AudioFormatManager` + `AudioTransportSource` e se
  registra no mixer (`mixerSource.addInputSource`).
- A backing track é carregada em `PadGridComponent` e também registrada no mixer.

### 4.3 Componentes

- **`PadComponent`** (`Component` + `Timer`): carrega um sample da memória (`BinaryData`),
  toca em `mouseDown`, para em `mouseUp`, e usa `timerCallback()` a **60 Hz** para animar um
  *fade* da cor ativa (`currentFade -= 0.05f`) — dando o efeito "neon" ao soltar o pad.
- **`PadGridComponent`**: lê o JSON do kit via `BinaryData::getNamedResource`, cria os
  `PadComponent`s dinamicamente em um `juce::OwnedArray`, carrega a *backing track* e faz o
  *layout* da grade 8x8 em `resized()`. Também expõe os botões `backButton`, `playButton`
  e `stopButton`.
- **`MenuComponent`**: três `TextButton`s públicos (`kitMelodiaButton`, `kitBateriaButton`,
  `settingsButton`).
- **`SettingsComponent`**: sliders verticais (`masterVolumeSlider`, `backingTrackSlider`) e
  `backButton`. ⚠️ **Os sliders são apenas visuais — ainda não estão conectados a nenhum
  ganho/`Decibels`/mixer.** Conectar é um dos próximos passos naturais.

---

## 5. Assets: `BinaryData`, JSON e *mangling* de nomes

O projeto **embute os assets no binário** durante o build (via Projucer), não os lê do disco
em tempo de execução. Tudo é acessado com `BinaryData::getNamedResource(...)`.

### 5.1 Regra de nomenclatura de recursos

O Projucer "mangling" o nome do arquivo: **`.` → `_`** (mantendo maiúsculas/minúsculas).
Essa mesma regra é aplicada no código C++ antes da busca:

```cpp
juce::String resourceName = audioFileName.replaceCharacter('.', '_');
const char* data = BinaryData::getNamedResource(resourceName.toUTF8(), dataSize);
```

Exemplos: `C.mp3` → `C_mp3`, `Musica_Jogo.wav` → `Musica_Jogo_wav`,
`notes.json` → `notes_json`, `drumkit.json` → `drumkit_json`.

> ⚠️ Ao adicionar um novo arquivo de áudio/JSON, **sempre** registre-o no `PucPads.jucer`
> (`resource="1"`) e regenere via Projucer, para que ele passe a existir no `BinaryData`.

### 5.2 Formato do JSON de kit

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
- `pads[]`: lista de pads na **ordem de exibição** (preenchidos por linha, 8 colunas).
- `color`: string **ARGB hex** lida por `juce::Colour::fromString` (ex.: `ffffd700` = dourado
  opaco; `ff420012` = bordô).

> 🔎 Observação atual: `notes.json` e `drumkit.json` são **idênticos** (64 pads cada).
> Provável *débito*: o "Kit Bateria" deveria ter samples/comportamento distintos do "Kit Melodia".

### 5.3 Arquivos de áudio e Git LFS

`.gitattributes` marca `*.mp3`, `*.wav` e `*.aiff` como **Git LFS**. No working tree esses
arquivos aparecem como *ponteiros* de LFS (~130 bytes), **não** como áudio real. Os dados
sonoros reais estão **embutidos** em `PucPads/JuceLibraryCode/BinaryData.cpp` (~47 MB, comitado).

- Para ouvir/inspecionar os samples reais, rode `git lfs pull`.
- O build **não depende** de LFS: os bytes vêm do `BinaryData.cpp` já comitado.

---

## 6. Convenções de Código e Estilo

O projeto segue de perto o **estilo padrão do JUCE**. Mantenha-o:

- **Indentação:** 4 espaços; chaves no estilo *Allman* (abre na linha seguinte).
- **Namespace:** sempre qualifique `juce::` (ex.: `juce::Colour`, `juce::String`), como nos
  templates do JUCE.
- **Nomes:**
  - Classes/Tipos: `PascalCase` (`PadGridComponent`).
  - Métodos e variáveis: `camelCase` (`showPadScreen`, `currentFade`).
  - Membros privados: `camelCase` (o projeto não usa prefixo `m_`).
- **Headers:** use `#pragma once`.
- **Macros obrigatórias em classes JUCE:** encerre a classe com
  `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassName)`.
- **Smart pointers:** prefira `std::unique_ptr` e `juce::OwnedArray` para posse clara.
- **Listeners:** registre com `addListener` e **sempre remova** no destrutor
  (`removeListener`) para evitar *dangling pointers*.
- **Comentários:** o projeto usa comentários em **português**. Siga o idioma para manter
  consistência.
- **Cores institucionais:** use as constantes já estabelecidas para não quebrar a identidade:

  | Uso | Valor |
  | --- | --- |
  | Bordô (botões) | `ff420012` |
  | Bordô profundo (fundo) | `ff2a000b` |
  | Dourado neon (texto/destaque) | `ffffd700` |
  | Dourado âmbar | `ffffc107` |
  | Borda dourada dos pads | `ffb08d35` |

- **Debug:** use `DBG(...)` para logs de carregamento de kits (padrão já adotado em
  `PadGridComponent`).

### 6.1 Fluxo de edição recomendado

1. Edite **apenas** arquivos em `PucPads/Source/` e `PucPads/Arquivos_PucPads/`.
2. Se adicionar/remover arquivos do projeto, atualize o `PucPads.jucer`.
3. Abra/salve no **Projucer** para regenerar `JuceLibraryCode/` e os projetos de IDE.
4. Compile pela IDE e valide em desktop e (idealmente) Android.

---

## 7. Build e Execução

> O build **não é feito por linha de comando** neste projeto: depende do Projucer + IDE.
> Detalhes completos no [`README.md`](README.md) §3.4 (Desktop) e §3.5 (Android).

### Desktop (Windows / VS2022)
1. Baixe o JUCE e aponte os caminhos em **Projucer → Global Paths → Modules**.
2. Abra `PucPads/PucPads.jucer` no Projucer.
3. Exportador **Visual Studio 2022** → *Save and Open in IDE*.
4. Compile em `Builds/VisualStudio2022/`.

### Android
1. No Projucer, configure SDK/NDK em **File → Global Paths**.
2. Exportador **Android Studio** → salvar (gera `Builds/Android/`).
3. Abra `Builds/Android` no Android Studio, conecte o dispositivo (USB Debug) e rode.

> ⚠️ **Atenção aos MODULEPATHS no `.jucer`:** a maioria dos módulos usa
> `../../../Software/JUCE/modules`, mas `juce_audio_processors_headless` aponta para
> `../../../../Softwares/Faculdade/JUCE/modules` (caminho inconsistente, provavelmente
> específico da máquina do autor). Se o build falhar com "module not found", ajuste os
> *Global Paths* no Projucer — **não** edite caminhos absolutos no `.jucer` manualmente.

### 7.1 Limitação do ambiente de agente (sandbox DevBox)

O container base (`devbox-base`) **não** possui JUCE, Projucer, MSVC nem NDK. Portanto:

- Não é possível realizar o **build completo** dentro do sandbox.
- O que **pode** ser validado no sandbox: sintaxe/estrutura dos arquivos, consistência do
  `.jucer` e dos recursos, validade dos JSONs de kit, e revisão estática de C++.
- Para checagens leves de sintaxe C++ sem JUCE, use `g++ -fsyntax-only` com *stubs* apenas
  se estritamente necessário — mas isso **não substitui** o build real. Prefira revisão manual.

---

## 8. Guias rápidos de tarefas comuns

### Adicionar um novo sample/pad
1. Coloque o arquivo em `PucPads/Arquivos_PucPads/`.
2. Registre no `PucPads.jucer` com `compile="0" resource="1"`.
3. Referencie-o no JSON do kit (`"audioFile": "Novo.mp3"`).
4. Regenere pelo Projucer e recompile.

### Criar um novo kit
1. Copie `notes.json` para `NovoKit.json` em `Arquivos_PucPads/`.
2. Ajuste `backingTrack` e o array `pads`.
3. Registre no `.jucer` e regenere.
4. Em `MainComponent`, crie um botão no menu e chame
   `showPadScreen("NovoKit_json")` (lembre do *mangling* `.` → `_`).

### Ajustar a grade de pads
A geometria vive em `PadGridComponent::resized()`. Hoje o *layout* é **fixo em 8 colunas x
8 linhas**, mas só são desenhados os pads presentes no array (`for i < pads.size()`).
Alterar `numCols`/`numRows` muda a densidade da grade.

---

## 9. Testes e Verificação

- **Não existe** suíte de testes automatizados nem CI neste repositório.
- A verificação atual é **manual**: compilar e testar interação (clique/multitouch) no
  desktop e no Android.
- Se for introduzir testes, prefira o **`juce::UnitTest`** (nativo do JUCE) para testar
  *parsers* (JSON de kit, conversão de cores) e lógica pura — evite dependências externas.
  **Combine antes** de adicionar frameworks de teste de terceiros.
- Antes de qualquer entrega, valide no mínimo: carregamento dos dois kits, disparo dos pads,
  play/stop da backing track e navegação entre as três telas.

---

## 10. Git, Branches e Workflow

### Branches protegidas — **NUNCA** trabalhe diretamente nelas
`main`, `master`, `bugfix`, `hotfix`, `homolog`, `sabesp`.

### Fluxo recomendado
1. Crie uma branch de trabalho a partir de `main` (ex.: `devbox/<descricao>` ou
   `feature/<descricao>`).
2. Faça commits **pequenos e descritivos**. Mensagens do histórico estão em português
   (ex.: *"Adicionando tela de Mixer e arrumando as telas de pad"*) — mantenha o idioma e o
   estilo.
3. **Sempre** faça `push` após o commit, para garantir sincronismo remoto.
4. Abra *Merge Request* / *Pull Request* para revisão.

### Commits
- Mensagens atuais são livres (sem *Conventional Commits*), mas em português e no gerúndio
  (*"Adicionando..."*, *"Atualizando..."*). Siga o padrão observado.

---

## 11. Débitos Técnicos e Armadilhas Conhecidas

Ao evoluir o projeto, considere (e, quando possível, corrija) os pontos abaixo — todos
verificados no código atual:

1. **Sliders do `SettingsComponent` não têm efeito** — são visuais; falta ligá-los ao
   `mixerSource`/ganhos. ALTO impacto funcional (RF de mixer).
2. **`notes.json` == `drumkit.json`** — os dois kits são idênticos; o "Kit Bateria" deveria
   diferir.
3. **`BinaryData.cpp` com ~47 MB comitado** — infla o repositório. Avaliar re-gerar assets
   fora do git ou usar LFS também para o `BinaryData`.
4. **Pads só tocam enquanto pressionados** (`mouseUp` chama `transportSource.stop()`) —
   comportamento *gate*, não *one-shot*. Há comentário no código sobre "modo Gate". Definir
   a semântica desejada.
5. **`MainComponent::showSettingsScreen()` adiciona a tela repetidamente** e não prevê
   *reset* do estado; revisar ciclo de vida de listeners ao alternar telas.
6. **Cabeçalho `PadGridComponent.h` tem texto corrompido** em um comentário
   (`// O mixer \tma referencia...`). Corrigir ao tocar no arquivo.
7. **Caminho inconsistente de módulo no `.jucer`** (ver §7) — pode quebrar build em máquinas
   novas.
8. **Sem tratamento de `nullptr` no array `pads` do JSON** — se `pads` não existir, o código
   apenas não cria pads (ok), mas não há validação robusta de campos faltantes
   (`audioFile`, `color`).
9. **Sem CI/testes** — qualquer refatoração depende de validação manual.

---

## 12. Regras de Ouro para Agentes

1. **Analise antes de mexer:** siga a arquitetura de telas via `MainComponent` e o JUCE como
   base. Não reinvente padrões.
2. **Não edite `JuceLibraryCode/` nem `Builds/` manualmente.**
3. **Não introduza bibliotecas novas** sem autorização.
4. **Não quebre retrocompatibilidade** de APIs públicas internas sem avisar.
5. **Nunca** atue em branches protegidas; use branch de trabalho + push.
6. **Nunca** exponha segredos (tokens, senhas, `.env`, chaves). Não há segredos no repo hoje —
   mantenha assim.
7. **Mantenha o idioma:** comentários, mensagens de commit e UI em **português**.
8. **Mantenha a identidade visual** bordô/dourado (§6) salvo pedido explícito em contrário.
9. **Assets entram pelo Projucer**, respeitando o *mangling* `.` → `_`.
10. **Deixe rastro:** ao finalizar, reporte workspace, branch, commit e status do push.
