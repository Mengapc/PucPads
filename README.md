# Software Design Document (SDD) – PucPads

## 1. Introdução

### 1.1 Propósito

Descrever o projeto PucPads, um software musical para performance e criação, que será utilizado no curso de Bacharelado em Produção Musical.

### 1.2 Escopo

O sistema tem como objetivo substituir o SuperPads Lights, aplicativo descontinuado, fornecendo aos estudantes uma plataforma estável, funcional e adaptável para prática, performance e criação musical.

### 1.3 Definições, Acrônimos e Abreviações

**MIDI**: Musical Instrument Digital Interface

**DAW**: Digital Audio Workstation

**Launchpad**: Controladora musical baseada em pads, usada para performance ao vivo e integração com softwares de música.

### 1.4 Referências

<img width="48" height="48" alt="unnamed" src="https://github.com/user-attachments/assets/cc9f5b2f-06dd-46e9-900d-9df1c8ba53d5" />  

SuperPads Lights (Opala Studios, até 2022)

[Link na Play Store](https://play.google.com/store/apps/details?id=com.opalastudios.superlaunchpad)

-------------------------------------------------------------------------------------------------------------------------------------
<img width="48" height="48" alt="unnamed (1)" src="https://github.com/user-attachments/assets/4285c2d0-9c93-4841-b225-dbf9cbdcc3af" />

SUPER PADS DJ: Music & Beats 

[Link na Play Store](https://play.google.com/store/apps/details?id=com.opalastudios.pads&pli=1)

---

## 2. Visão Geral do Sistema

### 2.1 Contexto

O aplicativo referência perdeu suporte, tornando-se inviável para uso acadêmico. Surge a necessidade de uma ferramenta própria para manter a prática pedagógica.

### 2.2 Usuários Alvo

 · Estudantes do curso de Produção Musical e Jogos Digitais.

 · Professores e monitores em atividades de performance e criação.

### 2.3 Funcionalidades Principais

| Funcionaliade                | Descrição                                                    |
| ---------------------------- | ------------------------------------------------             |
| Controle Musical             | Disparo de clipes, melodias, sequências e controle de mixer. |
| Performance ao Vivo          | Uso de pads com feedback visual (RGB).                       |
| Integração MIDI              | Compatibilidade com controladoras físicas (ex.: Launchpad).  |
| Visualização de Clipes       | Sincronização entre cores e status dos pads.                 |
| Customização                 | Personalização de escalas, padrões e efeitos visuais.        |

### 2.4 Análise de Alternativas (Concorrentes)

**Super Pads Lights DJ Launchpad**

 · Vantagens: Customização de áudio, cores e biblioteca extensa.

 · Limitações: App descontinuado, instabilidade de servidores.

**Fluxo de Telas**

├── 1              ← Tela dos Pads

│   ├── 1.1               ← Edição dos Pads.

│   ├── 1.2               ← Skins visuais.

│   ├── 1.3               ← Tutorial (How to Play).

├── 2              ← Tela de Kits

│   ├── 2.1               ← Aba Kits (gênero, artista, popularidade, novidades, país).

│   ├── 2.2               ← Aba Comunidade (upload/download de kits, busca).

│   ├── 2.3               ← Aba Perfil (kits baixados, áudios gravados, importados).

│   ├── 2.4               ← Aba Configurações (informações, upgrade de plano, cores).

<img width="1920" height="1080" alt="Telas do aplicativo" src="https://github.com/user-attachments/assets/edfe8705-c1ee-49d4-8c08-7c5c0f48df50" />

---

**SUPER PADS DJ: Music & Beats**

 · Vantagens: Interface simplificada, kits comunitários.

 · Limitações: Poucos pads, pouca customização, menos interatividade.

---

## 3. Requisitos do Sistema

### 3.1 Requisitos Funcionais

 · RF01: Permitir tocar músicas interagindo com os pads.

 · RF02: Importar arquivos de áudio próprios.

 · RF03: Gravar áudio pelo microfone do dispositivo.

 · RF04: Editar cores, padrões e áudios dos pads.

 · RF05: Disponibilizar biblioteca de kits (oficiais e da comunidade).

 · RF06: Conectar e controlar dispositivos MIDI físicos.

 · RF07: Acessar perfil do usuário com kits baixados/importados.

 · RF08: Oferecer tutorial interativo (How to Play).

### 3.2 Requisitos Não Funcionais

 · RNF01: Interface simples, responsiva e intuitiva.

 · RNF02: Suporte multiplataforma (desktop e mobile).

 · RNF03: Baixa latência na execução de sons.

 · RNF04: Compatibilidade com dispositivos MIDI padrão.

 · RNF05: Segurança e privacidade na comunidade online.

### 3.3 Ferramentas e Versões Necessárias (Ambiente de Desenvolvimento)

Para garantir a compilação correta do projeto PucPads, é necessário configurar o ambiente com as ferramentas abaixo:

| Ferramentas                  | Versão                                                                                             |
| ---------------------------- | ---------------------------------------------------------------------------------------------------|
| Framework de Áudio: JUCE     | Versão 8.0.10 ou superior                                                                          |
| IDE (Windows)                | Visual Studio 2022 ("Dev para Desktop com C++", MSVC v143 (ou mais recente) e Windows 10/11 SDK.)  |
| IDE (Mobile): Android Studio | Versão Jellyfish/Iguana (SDK: Android SDK Platform 34 e NDK: Side by side 26.x ou 27.x)            |
| Visualização de Clipes       | Sincronização entre cores e status dos pads.                 |
| Customização                 | Personalização de escalas, padrões e efeitos visuais.        |

### 3.4 Passos para Configuração (Desktop/Windows)

1. Faça o download do JUCE e extraia em um diretório de fácil acesso (ex: ```C:\JUCE```).

2. Abra o aplicativo Projucer (localizado na pasta do JUCE).

3. No Projucer, abra o arquivo ```PucPads.jucer``` localizado na raiz do projeto.

4. Vá em ```Global Paths``` (no menu do Projucer) e verifique se os caminhos para o JUCE Modules estão corretos.

5. Clique no ícone do seu Exportador (Visual Studio 2022) e depois no botão "Save and Open in IDE" (ou clique no ícone do VS).

### 3.5 Passos para Configuração (Android)

1. Instale o Android Studio e, pelo SDK Manager, baixe o NDK (Side by side) e o CMake.

2. Abra o arquivo ```PucPads.jucer``` no Projucer.

3. Vá na aba do exportador Android.

4. Configure os caminhos do SDK e NDK nas configurações globais do Projucer (```File > Global Paths```).

5. Salve o projeto no Projucer. Ele gerará os arquivos do Gradle na pasta ```Builds/Android```.

6. Abra a pasta ```Builds/Android``` através do Android Studio, conecte seu dispositivo (com Depuração USB ativada) e execute o build.

---

## 4. Arquitetura e Design de Interface

### 4.1 Arquitetura Geral

Aplicação modular baseada no framework JUCE, com:

 · Módulo de Áudio (execução de base, importação via JSON, gravação).

 · Módulo MIDI (conexão com dispositivos externos).

 · Módulo de Interface (pads 8x8, botões de controle, UI Institucional).

 · Módulo de Kits (biblioteca dinâmica e JSON parser).

### 4.2 Fluxo de Interface e Navegação

O diagrama de interface ilustra o storyboard de navegação, apresentando uma estética minimalista de alto contraste (Dark Mode). Ele incorpora a paleta de cores institucional da PUCPR, utilizando um fundo bordô profundo combinado com elementos interativos iluminados em dourado neon. O fluxo do usuário é estruturado em três telas principais interconectadas:

**· Menu Principal (Esquerda)**: A porta de entrada do aplicativo, apresentando de forma proeminente o brasão oficial da universidade. A navegação é construída através de uma lista de botões arredondados, proporcionando acesso rápido e intuitivo ao menu central, biblioteca de kits de áudio, painel de configurações e informações sobre o projeto.

**· Grade de Performance (Centro)**: O núcleo interativo do instrumento virtual. Esta tela apresenta uma grade preenchida com pads de disparo de áudio. O sistema de feedback visual é demonstrado por pads que acendem com um contorno dourado brilhante ao serem ativados, destacando-se contra o fundo escuro. A barra superior contém controles de gerenciamento de reprodução, permitindo ao usuário pausar, avançar ou retroceder a faixa de base (backing track) durante uma performance.

**· Configurações e Mixagem (Direita)**: Uma interface dedicada para ajustes técnicos avançados e roteamento de áudio. O painel inclui um mixer multicanal com faders independentes para controle de ganho, acompanhados por medidores visuais de nível. A seção inferior apresenta um visualizador de forma de onda (waveform), complementado por controles deslizantes para o corte preciso de samples (pontos de início e fim) e um conjunto de controles mestres para gravação e reprodução geral.

<img width="1920" height="1080" alt="Settings" src="https://github.com/user-attachments/assets/fb29c16e-c7ae-49fe-a794-8cbd728a7e4d" />

---

## 5.1 Plano de Trabalho e Considerações Finais

### 5.1 Plano de Trabalho

1. Conhecer características do app de referência.

2. Desenvolver protótipo funcional com integração MIDI.

3. Preencher relatórios conforme solicitado.

4. Concluir o projeto em parceria com a Escola de Belas Artes e a Escola Politécnica.

### 5.2 Considerações Finais

O projeto PucPads busca oferecer uma solução estável, moderna e adaptada às necessidades pedagógicas da Produção Musical, garantindo continuidade das práticas interrompidas com a descontinuidade do SuperPads Lights.

---

## 6. Relatório Técnico e Evolução do Desenvolvimento

Esta seção documenta o histórico de pesquisas estruturais e as decisões de engenharia adotadas ao longo do projeto.

### 6.1 Fase Exploratória (Unity)

Em uma fase inicial, a plataforma Unity foi utilizada para prototipar as funcionalidades centrais do aplicativo. O objetivo era validar a capacidade da engine de atender aos requisitos básicos do projeto.

* **Conquistas:** Implementação de ```AudioManager``` básico, Input System (mouse/touch) e interface visual de pads interativos.

* **Limitações Identificadas**: A latência nativa do sistema de áudio padrão mostrou-se um ponto de atenção crítico para a performance musical no Android. A implementação robusta de multitouch e a gestão de recursos (Garbage Collection via instanciação de GameObjects para áudio) indicaram alto risco de gargalos no mobile.

* **Vídeo Demonstrativo:**

- [Assistir ao Vídeo do Protótipo Unity (YouTube Shorts)](https://www.youtube.com/watch?v=NwCAiN9RlMc)

### 6.2 Transição e Consolidação do MVP (JUCE)

A decisão de transicionar para o JUCE foi fundamentada na necessidade de performance de áudio em tempo real. Enquanto a Unity exigiria contornos técnicos (workarounds) para gerenciar latência e polifonia, o JUCE oferece essas capacidades nativamente, sendo o padrão da indústria para software de áudio.

* **Arquitetura de Áudio Profissional:** Uso de ```juce::MixerAudioSource``` garantindo polifonia e latência mínima em ritmo e percussão.

* **Matriz 8x8 e Backing Tracks:** Reestruturação da grade de performance para o padrão de 64 pads com ocupação dinâmica de tela ```(resized())``` e player dedicado para faixas de base.

* **Configuração Dinâmica (JSON):** Leitura de arquivos JSON externos para configurar automaticamente quantidade de pads, arquivos de áudio associados e cores em hexadecimal (ARGB), permitindo criar kits sem recompilação.
    * Exemplo de estrutura implementada:
        ```
        {
         "backingTrack": "Musica_Jogo.wav",
         "pads": [
           {  "note": "Pad_1",
              "audioFile": "C.mp3",
              "color": "ffffd700"
           },
           {  "note": "Pad_2",
              "audioFile": "D.mp3",
              "color": "ffffc107"
           }  
          ]
        }
        ```

* **Identidade Visual e Feedback:** Implementação de Dark Mode na paleta institucional (Bordô/Dourado) e classe ```juce::Timer``` para animações táteis suaves de fade-out nos pads (60 FPS).

* **Suporte Mobile Comprovado:** Compilação final e testes bem sucedidos em hardware Android via USB Debugging com multitouch perfeito.

* **Status Final do MVP:**

- [Assistir ao Vídeo do Status Final (YouTube Shorts)](https://youtube.com/shorts/NXkurH1tSUU)

## 7. Referências de Pesquisa (Anexos)

### 7.1 Estrutura de Pesquisa sobre Áudio na Unity para Launchpad

#### _1. Fundamentos do Sistema de Áudio na Unity_

Componentes principais:

AudioSource: responsável por reproduzir sons.

AudioClip: arquivo de áudio importado na Unity.

AudioListener: “microfone virtual” da cena (normalmente na câmera principal).

Tipos de importação de áudio:

Compressão: PCM, Vorbis, ADPCM.

Configurações de streaming vs. memória.

Diferença entre 2D e 3D sound (para Launchpad → majoritariamente 2D).

Pesquisa sugerida:

Documentação Unity sobre AudioSource e AudioClip.

Testar diferentes formatos de áudio e latência (WAV, MP3, OGG).

#### _2. Disparo de Sons (Pads)_

Instanciar sons diferentes em pads distintos.

Técnicas para multiplayback: tocar várias vezes o mesmo som sem cortar o anterior.

Uso de AudioSource.PlayOneShot() (simples e rápido).

Criação de pool de AudioSources para evitar cortes em sons simultâneos.

Controle de volume individual por pad.

Pesquisa sugerida:

Comparar Play() vs. PlayOneShot().

Limites de polifonia no Unity (quantos sons simultâneos sem travar).

#### _3. Feedback Visual Sincronizado_

Eventos visuais ao disparar sons:

Mudança de cor (Image.color em UI, Material.color em 3D).

Efeitos de “pulse” ou “flash” com Animator ou Tweening (ex.: DOTween).

Sincronização básica:

Ativar efeito junto com o PlayOneShot().

Manter cor/efeito por tempo fixo ou pela duração do som (clip.length).

Pesquisa sugerida:

Capturar duração do áudio (AudioClip.length).

Usar corrotinas para animar feedback visual.

#### _4. Música de Fundo_

Carregar e reproduzir faixa completa.

Configuração: AudioSource.loop = true.

Controle separado:

Um AudioSource exclusivo para música base.

Outro para disparo dos pads (PlayOneShot).

Pesquisa sugerida:

Streaming vs. memória para faixas longas.

Controle da posição da música (AudioSource.time).

#### _5. Interação Livre com Pads_

Sem necessidade de quantização ou sincronização no MVP.

Garantir latência mínima no disparo de sons.

Testes de input:

Mouse/Teclado para protótipo inicial.

Multitouch no mobile (Input.touchCount).

Pesquisa sugerida:

Diferença de latência entre PC e mobile.

Input multitouch na Unity.

#### _6. Estrutura de Navegação_

Cenas:

Cena inicial (menu) → escolha de kit/música.

Cena principal → grade de pads interativos.

Estrutura mínima:

SceneManager.LoadScene() para troca de telas.

Armazenar escolhas com PlayerPrefs.

Pesquisa sugerida:

Sistema de UI com Canvas (botões, grids).

Uso de Grid Layout Group para organizar pads.

#### _7. Questões Avançadas (Futuras Versões)_

Redução de latência em Unity (especialmente mobile).

Sincronização com BPM/timeline.

Efeitos em tempo real (reverb, filtros, equalizadores via AudioMixer).

Customização de kits (upload de samples pelo usuário).

### 7.2 Áudio na Unity

Durante a pesquisa, foram identificados materiais práticos relevantes sobre como organizar, disparar e manipular sons no Unity.

#### Vídeo 1 – Introdução ao AudioManager

📎 [Introduction to AUDIO in Unity](https://www.youtube.com/watch?v=6OT43pvUyfY) 

1. Mostra a criação de um AudioManager para centralizar o controle de sons.

2. Evita a necessidade de criar múltiplos objetos de áudio na cena.

3. Focado em boas práticas para organização e controle em jogos.

#### Vídeo 2 – Boas e más práticas + Mixer

📎 [How To Add Sound Effects the RIGHT Way | Unity Tutorial](https://www.youtube.com/watch?v=DU7cgVsU2rM&t=87s)

1. Demonstra maneiras corretas e equivocadas de organizar áudios.

2. Aborda técnica de instanciar objeto temporário para tocar áudio e destruí-lo depois (reduz complexidade).

3. Introduz o uso de AudioMixer para criar sliders independentes:

     · Volume Master.

     · Volume da música.

     · Volume dos efeitos sonoros (SFX).

#### Vídeo 3 – Efeitos e automação de áudio

📎 [Introduction to Immersive Audio in Games](https://www.youtube.com/watch?v=b52rP-1xBnc)

1. Mostra a utilização de efeitos imersivos no áudio.

2. Destaca o uso de automação em efeitos como reverb, low pass filter, etc.

3. Relevante para futuras versões do projeto, focadas em imersão e customização avançada.

#### Documentação Oficial da Unity

📎 [Unity Manual – Sistema de Áudio](https://docs.unity3d.com/6000.2/Documentation/Manual/Audio.html)

1. Explica a arquitetura de áudio no Unity.

2. Detalhes sobre:

     · AudioSource e AudioClip.

     · Importação e compressão de formatos (WAV, MP3, OGG).

     · Diferença entre áudio 2D e 3D.

     · Uso de AudioMixer e efeitos nativos.

_Fonte essencial que será utilizada como base de documentação técnica ao longo do projeto._

#### Artigo – Integração de Música e SFX no Unity

📎 [Integrate Music and Sound Effects in Unity for Games – Moldstud](https://moldstud.com/articles/p-integrate-music-and-sound-effects-in-unity-for-games)

Resumo técnico (foco em recursos da Unity):

1. A Unity oferece um sistema robusto de áudio que pode ser aplicado além de jogos, em softwares musicais e interativos.

2. Principais recursos:

     · AudioSource e AudioClip para disparo e reprodução de sons.

     · AudioListener como ponto de referência de áudio na cena.

     · AudioMixer para mixagem, controle de volumes e aplicação de efeitos.

     · Formatos de áudio suportados: WAV, MP3, OGG, AIFF (com opções de compressão e stream).

3. Configurações importantes:

     · Escolha entre streaming (faixas longas, como música de fundo) ou carregamento em memória (samples curtos para pads).

     · Diferenciação entre áudio 2D (ideal para pads e softwares musicais) e áudio 3D (posicionamento espacial, menos relevante neste caso).

4. Efeitos e processamento:

     · Unity possui suporte nativo a filtros como Reverb, Low Pass, High Pass e Echo.

     · Possibilidade de automação de parâmetros para criar variações dinâmicas.

5. Boas práticas destacadas no artigo:

     · Separar música, efeitos e outros sons em diferentes mixers/canais.

     · Otimizar o uso de memória ao lidar com muitos samples.

     · Manter equilíbrio entre qualidade de áudio e performance (especialmente em mobile).

### 7.3 Input Touch na Unity

#### Vídeo 1 – Introdução rápida ao Input System (Touch)

📎 [Unity Touch Input Tutorial: From Basics to Advanced](https://www.youtube.com/watch?v=KrBWCUbfe4o)

1. Demonstra o funcionamento do Input System para interações por toque.

2. Ensina a configurar o celular para testes diretos em dispositivo mobile rodando pela Unity.

3. Mostra de forma rápida o potencial do sistema de touch sem necessidade de scripts complexos.

#### Vídeo 2 – Configuração básica com Input System

📎 [Mobile input/touch control for your Unity game](https://www.youtube.com/watch?v=NqrJHj9xlqY)

1. Apresenta a configuração inicial do Input System.

2. Foco em exemplos voltados a jogos: botões e joysticks virtuais.

3. Mostra scripts pré-criados pelo Unity Input System que podem ser adaptados para interação com pads.

#### Vídeo 3 – Input Touch detalhado com scripts

📎 [How to use Touch with NEW Input System - Unity Tutorial](https://www.youtube.com/watch?v=ERAN5KBy2Gs)

⭐ 📎 [Versão atualizada do mesmo vídeo](https://www.youtube.com/watch?v=4MOOitENQVg)

1. Explicação aprofundada sobre toques na tela e como capturar posições.

2. Demonstra duas abordagens:

3. Input gerenciado diretamente pelo Input System.

4. Input tratado manualmente via scripts.

5. Ensina a organizar a leitura do input de forma clara e escalável.

6. Exemplo prático: teletransportar um objeto para o ponto clicado na tela.

#### Documentação Oficial – Unity Input (Touch)

📎 [Unity Script Reference – Input](https://docs.unity3d.com/ScriptReference/Input.html)

1. Principais funções para toque:

     · Input.touchCount → número de toques simultâneos detectados.

     · Input.GetTouch(index) → retorna informações de um toque específico.

     · Touch.position → posição (x, y) na tela.

     · Touch.phase → estado do toque: Began, Moved, Stationary, Ended, Canceled.

     · Touch.deltaPosition → diferença de movimento desde o último frame.

     · Touch.fingerId → identifica cada dedo de forma única.

     · Input.multiTouchEnabled → habilita/desabilita múltiplos toques.

2. Aplicação no PucPads:

     · Usar toque único para disparar sons nos pads.

     · Usar multitouch para suportar a performance com múltiplos dedos ao mesmo tempo.
