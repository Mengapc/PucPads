# Software Design Document (SDD) – PucPads

## 1. Introdução

### 1.1 Propósito
Descrever a arquitetura, os requisitos técnicos e o plano de evolução do **PucPads**, um software musical de alta performance desenvolvido em C++/JUCE como projeto de Iniciação Científica Tecnológica (PIBITI – PUCPR), voltado ao apoio pedagógico e à performance ao vivo no curso de Bacharelado em Produção Musical.

### 1.2 Escopo
Substituir o aplicativo descontinuado *SuperPads Lights*, oferecendo aos estudantes e professores uma plataforma multiplataforma estável, de baixa latência e pedagogicamente adaptada. O sistema atua como instrumento facilitador visual e tátil para o ensino de percepção rítmica, melódica e harmônica, superando barreiras do modelo conservatorial tradicional sem exigir letramento musical prévio.

### 1.3 Definições, Acrônimos e Abreviações
- **MIDI:** *Musical Instrument Digital Interface* (protocolo de comunicação entre controladoras e softwares musicais).
- **DAW:** *Digital Audio Workstation*.
- **Choke Group (Grupo de Exclusão Mútua):** Agrupamento lógico no qual o disparo de um pad interrompe suavemente (via *fade-out* rápido) o áudio de outro pad pertencente ao mesmo grupo.
- **Launchpad:** Controladora física baseada em matriz de pads utilizada para performance e disparo de samples.

### 1.4 Referências de Mercado
<img width="48" height="48" alt="unnamed" src="https://github.com/user-attachments/assets/cc9f5b2f-06dd-46e9-900d-9df1c8ba53d5" />  

**SuperPads Lights** (Opala Studios, descontinuado em 2022) – [Play Store](https://play.google.com/store/apps/details?id=com.opalastudios.superlaunchpad)

<img width="48" height="48" alt="unnamed (1)" src="https://github.com/user-attachments/assets/4285c2d0-9c93-4841-b225-dbf9cbdcc3af" />

**SUPER PADS DJ: Music & Beats** – [Play Store](https://play.google.com/store/apps/details?id=com.opalastudios.pads&pli=1)

---

## 2. Visão Geral do Sistema e Fundamentação Pedagógica

### 2.1 Contexto e Justificativa
Com a descontinuidade do *SuperPads Lights*, perdeu-se uma ferramenta acessível utilizada nas atividades práticas do curso de Produção Musical. O PucPads preenche essa lacuna combinando o motor de áudio profissional do framework JUCE com funcionalidades desenhadas especificamente para o contexto acadêmico e criativo.

### 2.2 Usuários-Alvo
- Estudantes dos cursos de Bacharelado em Produção Musical e Jogos Digitais (PUCPR).
- Professores e monitores em atividades de musicalização, percepção e performance digital.

### 2.3 Arquitetura Funcional e Semântica Visual
O aplicativo organiza os elementos musicais de forma visual e intuitiva na matriz 8x8 (64 pads), utilizando um código de cores semântico para auxiliar o aluno a identificar a função de cada som na estrutura da obra:

| Cor | Função Musical | Aplicação Pedagógica / Sonora |
| :--- | :--- | :--- |
| **Verde** | Efeitos (SFX) | Transições, impactos, ambiências e elementos pontuais. |
| **Azul** | Loops | Bases rítmicas contínuas e grooves de acompanhamento. |
| **Rosa** | Melodia | Frases melódicas, solos, vocais e linhas principais. |
| **Laranja** | Harmonia | Acordes, progressões harmônicas e pads de sustentação. |

### 2.4 Análise de Concorrentes (Resumo)
- **Super Pads Lights DJ Launchpad:** Possuía boa customização e biblioteca extensa, mas foi descontinuado e limitava o usuário a no máximo **6 grupos de exclusão (*choke groups*)**, restringindo arranjos mais complexos.
- **SUPER PADS DJ: Music & Beats:** Interface simplificada e kits comunitários, porém com grade reduzida e baixa flexibilidade de edição para uso pedagógico.

---

## 3. Requisitos do Sistema (Escopo Atualizado – PIBITI)

### 3.1 Requisitos Funcionais
- **RF01 – Performance e Disparo Polifônico:** Permitir a execução simultânea de samples na grade 8x8 com suporte a múltiplas páginas/bancos de sons na mesma sessão.
- **RF02 – Grupos de Exclusão Dinâmicos (Choke Groups):** Permitir agrupar pads para que não toquem simultaneamente, aplicando um *fade-out* curto na transição para evitar cliques/estalos, **sem a limitação de 6 grupos** do software de referência.
- **RF03 – Edição de Pads (Pad Editor):** Permitir ajustar volume individual, *pitch* (afinação), envelopes simples (*fade-in* / *fade-out*), corte básico (*trimming*), além de clonar ou remover pads.
- **RF04 – Padronização Semântica de Cores:** Atribuir e customizar cores dos pads conforme sua função musical (Verde, Azul, Rosa, Laranja).
- **RF05 – Importação e Gravação de Áudio:** Permitir importar arquivos locais da biblioteca do dispositivo (1ª opção) e gravar samples diretamente pelo microfone do dispositivo (2ª opção).
- **RF06 – Gestão de Backing Tracks e Kits:** Permitir selecionar a música de acompanhamento (*playback*) e salvar kits personalizados em JSON (contendo nome do kit, nome da música e nome do autor/usuário).
- **RF07 – Integração MIDI e Compartilhamento:** Suportar controladoras MIDI externas e possibilitar a exportação/compartilhamento de kits entre usuários.

### 3.2 Requisitos Não Funcionais
- **RNF01 – Baixa Latência:** Resposta imediata ao toque, adequada para execução rítmica em tempo real.
- **RNF02 – Suporte Multiplataforma:** Compilação nativa para Desktop (Windows) e Mobile (Android) com suporte robusto a *multitouch*.
- **RNF03 – Ergonomia e Identidade Visual:** Interface responsiva em *Dark Mode* de alto contraste, baseada na paleta institucional da PUCPR (Bordô profundo e Dourado Neon).
- **RNF04 – Gerenciamento Seguro de Memória:** Alocação e destruição determinística de fontes de áudio em C++, evitando vazamentos de memória e quedas de quadros (*frames*) durante a troca de telas e kits.

---

## 4. Arquitetura Técnica e Design de Interface

### 4.1 Arquitetura Modular (C++ / JUCE)
O sistema é estruturado de forma modular sobre o framework **JUCE 8.0.10**:
- **Gerenciador Principal (`MainComponent`):** Controla o ciclo de vida das telas via `std::unique_ptr`, gerencia o `juce::MixerAudioSource` central e garante a limpeza segura de recursos de áudio na navegação.
- **Núcleo de Performance (`PadGridComponent` & `PadComponent`):** Renderiza a grade 8x8 responsiva, gerencia a reprodução da *backing track*, executa o feedback visual de *fade-out* a 60 FPS via `juce::Timer` e interpreta os arquivos de configuração JSON.
- **Mixer e Configurações (`SettingsComponent`):** Interface dedicada ao roteamento de áudio, controle de ganho (Master e Base) e parâmetros técnicos.
- **Módulos Planejados (Próximas Sprints):** `PadEditorComponent` (edição de *pitch*, volume, *fade* e grupos), `AudioRecorderComponent` (captura via microfone) e `KitSerializer` (salvamento e exportação de kits).

### 4.2 Fluxo de Interface e Navegação
O diagrama de interface ilustra o *storyboard* de navegação em *Dark Mode*, estruturado em três telas principais interconectadas:

- **Menu Principal (Esquerda):** Porta de entrada introdutória do aplicativo, com acesso à tela de performance (*Launch Pad*) e ao painel de configurações/mixer.
- **Launch Pad (Centro):** Tela **única** de performance do instrumento virtual: matriz 8x8 responsiva, controles de transporte da *backing track* e botão de retorno no cabeçalho. O rodapé traz **8 botões de template** — cada um abre uma configuração diferente de Launch Pad, que o usuário poderá editar adiante.
- **Configurações e Mixagem (Direita):** Painel para ajustes técnicos avançados, faders independentes para controle de ganho, ajustes de parâmetros dos pads e controles de gravação/reprodução.

<img width="1920" height="1080" alt="Settings" src="https://github.com/user-attachments/assets/fb29c16e-c7ae-49fe-a794-8cbd728a7e4d" />

> Imagem gerada e editada com auxílio de inteligência artificial – Gemini.

---

## 5. Ambiente de Desenvolvimento e Build

### 5.1 Ferramentas e Versões
| Ferramenta | Especificação / Versão |
| :--- | :--- |
| **Framework de Áudio** | JUCE 8.0.10 ou superior |
| **IDE Desktop (Windows)** | Visual Studio 2022 (Carga: *Desenvolvimento para Desktop com C++*, MSVC v143+, Windows 10/11 SDK) |
| **IDE Mobile (Android)** | Android Studio Jellyfish / Iguana ou superior (SDK Platform 34, NDK Side-by-side 26.x/27.x, CMake) |

### 5.2 Configuração Desktop (Windows)
1. Extraia o JUCE em um diretório raiz (ex.: `C:\JUCE`) e abra o **Projucer**.
2. Abra o arquivo `PucPads.jucer` e verifique os caminhos em `File > Global Paths`.
3. Selecione o exportador **Visual Studio 2022** e clique em *Save and Open in IDE*.

### 5.3 Configuração Mobile (Android)
1. Garanta que o projeto esteja em um **caminho somente ASCII** (veja §5.4 — o Gradle aborta a configuração caso contrário).
2. No Android Studio (SDK Manager), instale o **NDK (Side by side)** e o **CMake**.
3. No Projucer, configure os caminhos do SDK e NDK em `File > Global Paths`.
4. Salve o projeto para gerar os arquivos Gradle em `Builds/Android`, abra a pasta no Android Studio e execute o build via Depuração USB.

### 5.4 Requisito de caminho do projeto (Windows)

> ⚠️ **O caminho do projeto não pode conter caracteres não-ASCII** (acentos, cedilha, `ç`, `ã`, espaços exóticos).
> O Android Gradle Plugin interrompe a avaliação de `:app` com:
> `Your project path contains non-ASCII characters. This will most likely cause the build to fail on Windows.`
> O erro é do **ambiente**, não do código: o build não chega a compilar nada.

O `build.gradle` é regenerado pelo Projucer, e o `Builds/Android/gradle.properties` também — o Projucer
reescreve esse arquivo a cada *Save* com apenas `android.useAndroidX=true`. Ou seja, editar esse arquivo
não é correção durável.

| Opção | Como fazer | Durável? | Risco |
| :--- | :--- | :--- | :--- |
| **A (recomendada)** | Mover/renomear a pasta do projeto para um caminho ASCII, ex.: `D:\Projetos\Programacao\PucPads` | ✅ sim | Exige limpar caches e reabrir a pasta no Android Studio |
| **B** | Criar uma *junction* ASCII e abrir o projeto por ela: `mklink /J D:\Projetos\PucPads "D:\Projetos\Programação\PucPads"` → abrir `D:\Projetos\PucPads\PucPads\Builds\Android` | ✅ sim, sem mover arquivos | Algumas ferramentas resolvem o caminho real (com acento) e o erro volta |
| **C** | `android.overridePathCheck=true` em `Builds/Android/gradle.properties` | ❌ não (o Projucer apaga no próximo *Save*) | Serve só para teste rápido |
| **D** | Mesma linha em `%USERPROFILE%\.gradle\gradle.properties` | ✅ sim (vale para toda a máquina) | Desliga a checagem em **todos** os projetos; o build pode falhar depois no NDK/aapt2 |

Após mover o projeto (opção A), descarte os caches gerados no caminho antigo — eles guardam caminhos absolutos:

```bat
cd /d D:\Projetos\Programacao\PucPads\PucPads\Builds\Android
rmdir /s /q .gradle app\.cxx app\build
del /q local.properties
```

No Android Studio, use `File > Invalidate Caches / Restart` e reabra a pasta `Builds\Android` pelo **novo**
caminho (o atalho de projeto recente continua apontando para o antigo).

---

## 6. Roadmap e Próximos Passos de Desenvolvimento (PIBITI 2026–2027)

### 6.1 O que já foi concluído (Baseline Atual)
- [x] Validação comparativa de engines (Unity vs. JUCE) e migração definitiva para C++/JUCE.
- [x] Motor de mixagem polifônica (`juce::MixerAudioSource`) e player dedicado para *backing tracks*.
- [x] Parser dinâmico de kits via JSON (`drumkit.json` / `notes.json`) com suporte a cores ARGB.
- [x] Grade responsiva 8x8 (64 pads) com feedback visual de decaimento suave a 60 FPS (`juce::Timer`).
- [x] Arquitetura de navegação entre telas (Menu Principal, Grade de Performance e Configurações/Mixer) com gerenciamento determinístico de memória.
- [x] Compilação e validação de *multitouch* e baixa latência em dispositivo Android real.
- [x] Tela única de *Launch Pad* com **8 templates** no rodapé (kits como templates) e navegação de páginas por kit.
- [x] Grupos de exclusão (*choke groups*) ilimitados com *fade-out* curto, lidos do JSON.

### 6.2 Próximos Passos Técnicos (Sprints em Planejamento)
- **Templates de Launch Pad e Navegação:**
  - Preencher os templates 2–8 com kits próprios (hoje apenas o template 1 tem JSON; os demais abrem uma grade vazia).
  - Extrair a lista de templates do código para um manifesto JSON/preferências quando houver edição pelo usuário.
- **Semântica de Cores (RF04):** os kits padrão já usam o código pedagógico (Verde: Efeitos, Azul: Loop, Rosa: Melodia, Laranja: Harmonia); falta apenas material sonoro real.
- **Aba de Edição de Pads (Pad Editor):**
  - Conectar os faders da `SettingsComponent` ao controle real de ganho (Master, Backing Track e volume individual por pad).
  - Implementar controle de *Pitch* em tempo real (`juce::ResamplingAudioSource`) e envelopes simples de *Fade-in* / *Fade-out*.
  - Adicionar funções utilitárias de edição: **Clonar Pad**, **Remover Pad** e corte básico de início/fim do sample (*trimming*).
- **Criação de Conteúdo (Importação e Gravação):**
  - Implementar seletor de arquivos nativo (`juce::FileChooser`) para importar samples da biblioteca local do dispositivo.
  - Implementar captura de áudio via microfone (`juce::AudioDeviceManager` + gravação em buffer/WAV) para criação de samples autorais em sala de aula.
- **Persistência, Salvamento de Kits e Compartilhamento:**
  - Criar tela de salvamento de kit para serializar em JSON: **Nome do Kit**, **Nome da Música (Playback)**, **Nome do Usuário** e mapeamento completo dos pads.
  - *(Opcional)* Implementar exportação/importação de pacotes de kits (`.zip`) para compartilhamento entre alunos e professores.

---

## 7. Histórico de Engenharia e Legado (Fase Exploratória)

> Esta seção preserva o registro técnico da etapa inicial do projeto para fins de rastreabilidade acadêmica.

### 7.1 Prototipagem Inicial na Engine Unity (2025)
Inicialmente, a engine Unity foi avaliada para validar a interface e o disparo básico de áudio (`AudioManager` com instanciação dinâmica de `AudioSource` e `Input System`). Embora funcional no desktop, os testes em Android revelaram gargalos críticos para uso musical:
- Latência nativa elevada no disparo rítmico.
- Overhead de CPU e picos de *Garbage Collection* causados pela criação/destruição contínua de `GameObjects` de áudio.
- Complexidade elevada para rastreamento simultâneo de múltiplos dedos (*multitouch*).
- **Registro em vídeo:** [Protótipo Unity](https://www.youtube.com/watch?v=NwCAiN9RlMc)

### 7.2 Validação do MVP em JUCE (Outubro/2025 – Maio/2026)
A migração para o framework C++/JUCE eliminou os gargalos de latência e gerenciamento de memória, permitindo acesso direto ao buffer de áudio do hardware e reconhecimento nativo de *multitouch* no Android.
- **Registros em vídeo:** [Protótipo Inicial JUCE](https://www.youtube.com/watch?v=lgqJpiOi8es) | [Status MVP Integrado](https://youtube.com/shorts/NXkurH1tSUU)

### 7.3 Diagrama de Referência (Fluxo do Aplicativo Legado)
Durante a análise de requisitos inicial, o fluxo de navegação do *SuperPads Lights* foi mapeado como referência base para a estruturação das telas do PucPads:
```
├── 1 ← Tela dos Pads
│   ├── 1.1 ← Edição dos Pads
│   ├── 1.2 ← Skins visuais
│   └── 1.3 ← Tutorial (How to Play)
└── 2 ← Tela de Kits
    ├── 2.1 ← Aba Kits (gênero, artista, popularidade, novidades)
    ├── 2.2 ← Aba Comunidade (upload/download de kits, busca)
    ├── 2.3 ← Aba Perfil (kits baixados, áudios gravados, importados)
    └── 2.4 ← Aba Configurações (informações, cores)
```

<img width="1920" height="1080" alt="Telas do aplicativo" src="https://github.com/user-attachments/assets/edfe8705-c1ee-49d4-8c08-7c5c0f48df50" />

---

## 8. Referências Técnicas e Registro de Pesquisa

Esta seção sintetiza as documentações oficiais e materiais de apoio consultados durante a fase exploratória (Unity) e a consolidação do produto (JUCE):

### 8.1 Documentações Oficiais
- **JUCE Framework:** [JUCE Documentation & Tutorials](https://docs.juce.com/) — Base principal para arquitetura de áudio em tempo real (`AudioAppComponent`, `MixerAudioSource`, `AudioTransportSource`), gerenciamento de buffers, *timers* de interface e exportação multiplataforma (Windows/Android).
- **Unity Audio System:** [Unity Manual – Sistema de Áudio](https://docs.unity3d.com/6000.2/Documentation/Manual/Audio.html) — Estudo de `AudioSource`, `AudioClip`, *AudioMixer*, polifonia e comparação entre *streaming* e carregamento em memória.
- **Unity Input (Touch):** [Unity Script Reference – Input](https://docs.unity3d.com/ScriptReference/Input.html) — Estudo de rastreamento de toques simultâneos (`touchCount`, `fingerId`, fases de toque) para validação de *multitouch* mobile.

### 8.2 Materiais de Apoio e Vídeos Técnicos (Fase Exploratória)
- **Arquitetura de Áudio e Mixagem:**
  - [Introduction to AUDIO in Unity](https://www.youtube.com/watch?v=6OT43pvUyfY) — Centralização de controle via `AudioManager`.
  - [How To Add Sound Effects the RIGHT Way | Unity Tutorial](https://www.youtube.com/watch?v=DU7cgVsU2rM&t=87s) — Boas práticas de instanciação temporária e separação de canais no mixer (Master, Música e SFX).
  - [Introduction to Immersive Audio in Games](https://www.youtube.com/watch?v=b52rP-1xBnc) — Conceitos de automação e processamento de efeitos (Reverb, filtros).
  - [Integrate Music and Sound Effects in Unity for Games – Moldstud](https://moldstud.com/articles/p-integrate-music-and-sound-effects-in-unity-for-games) — Diretrizes de otimização de memória e formatos de áudio em dispositivos móveis.
- **Interação Multi-touch Mobile:**
  - [Unity Touch Input Tutorial: From Basics to Advanced](https://www.youtube.com/watch?v=KrBWCUbfe4o) — Configuração de testes diretos em dispositivos Android e fundamentos do *Input System*.
  - [Mobile input/touch control for your Unity game](https://www.youtube.com/watch?v=NqrJHj9xlqY) — Adaptação de controles virtuais na tela.
  - [How to use Touch with NEW Input System - Unity Tutorial](https://www.youtube.com/watch?v=ERAN5KBy2Gs) ([Versão atualizada](https://www.youtube.com/watch?v=4MOOitENQVg)) — Leitura escalável de posições e eventos de toque via script.
