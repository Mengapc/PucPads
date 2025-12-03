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

## 2. Visão Geral do Sistema

### 2.1 Contexto

Aplicativo anterior perdeu suporte, tornando-se inviável para uso acadêmico.

Necessidade de uma ferramenta própria para manter a prática pedagógica.

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

## 4. Análise de Alternativas (Concorrentes)

### 4.1 Super Pads Lights DJ Launchpad

| Vantagens                       | Limitações                     |
| ---------------------------- | --------------------------------- |
| Customização de áudio | App descontinuado  |
| Cores e biblioteca extensa  | Instabilidade de servidores |

### 4.2 SUPER PADS DJ: Music & Beats

| Vantagens                       | Limitações                                           |
| ---------------------------- | --------------------------------- |
| Interface simplificada | Poucos pads, pouca customização  |
| Kits comunitários  | Menos interatividade |

## 5. Arquitetura e Design

### 5.1 Arquitetura Geral

Aplicação modular, com:

Módulo de Áudio (execução, importação, gravação).

Módulo MIDI (conexão com dispositivos externos).

Módulo de Interface (pads, skins, tutoriais).

Módulo de Kits (biblioteca, comunidade, perfil).

### 5.2 Fluxo de Telas

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

## 6. Plano de Trabalho

1. Conhecer características do app de referência.

2. Desenvolver protótipo funcional com integração MIDI.

3. Colaborar com bolsista responsável pela base de dados.

4. Preencher relatórios conforme solicitado.

5. Concluir o projeto em parceria com a Escola de Belas Artes e a Escola Politécnica.

## 7. Considerações Finais

O projeto PucPads busca oferecer uma solução estável, moderna e adaptada às necessidades pedagógicas da Produção Musical, garantindo continuidade das práticas interrompidas com a descontinuidade do SuperPads Lights.

## 8. Plano de Pesquisa e Desenvolvimento

### 8.1 Estrutura de Pesquisa sobre Áudio na Unity para Launchpad

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

### 8.2 Metodologia de Pesquisa Técnica

Sugestão para organização prática:

Definir hipótese: exemplo → “Será que PlayOneShot() suporta polifonia sem cortes?”

Criar protótipo rápido na Unity para testar.

Documentar resultado: prints, código mínimo, observações.

Avaliar aplicabilidade: serve para o MVP ou fica para versões futuras?

## 9. Referências de Pesquisa

### 9.1 Áudio na Unity

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

### 9.2 Input Touch na Unity

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

## 10. Relatório de Progresso e Direcionamento Técnico

### 10.1 Progresso Alcançado com o Protótipo em Unity (Fase Exploratória)

Em uma fase inicial, a plataforma Unity foi utilizada para prototipar as funcionalidades centrais do aplicativo. O objetivo era validar a capacidade da engine de atender aos requisitos básicos do projeto. Os seguintes marcos foram alcançados:

* **Sistema de Gerenciamento de Áudio:** Foi desenvolvido um sistema de `AudioManager` para centralizar e controlar a reprodução dos sons. Utilizou-se um padrão de instanciar dinamicamente um Prefab contendo um componente `AudioSource` para cada som disparado.
* **Controle de Input Multiplataforma:** Implementado o `Input System` da Unity, criando uma base de controle que reconhece e responde a eventos tanto do mouse (desktop) quanto de toque único (mobile).
* **Implementação dos Pads Interativos:** Criada uma interface com componentes de pads, onde scripts referenciavam `AudioClips` específicos.

#### Limitações Identificadas na Unity
Durante o desenvolvimento, os seguintes desafios técnicos foram mapeados para atingir o escopo completo:
* **Latência de Áudio:** A latência nativa do sistema de áudio padrão da Unity mostrou-se um ponto de atenção crítico para a performance musical em Android.
* **Complexidade de Multitouch:** A implementação robusta de múltiplos toques exigiria o desenvolvimento de sistemas complexos de rastreamento de IDs de dedos.
* **Gestão de Recursos:** A instanciação e destruição constante de objetos (`GameObjects`) para tocar sons poderia gerar problemas de performance (Garbage Collection) em dispositivos móveis mais simples.

- [Vídeo Demonstrativo do Protótipo Unity](https://www.youtube.com/watch?v=NwCAiN9RlMc)

---

### 10.2 Progresso Alcançado com o Protótipo em JUCE (MVP – Outubro/2025)

Após a análise da Unity, a plataforma JUCE foi adotada para o desenvolvimento do MVP, resultando em um protótipo funcional e de alta performance. Os seguintes marcos técnicos foram atingidos:

* **Arquitetura de Áudio Profissional:** Implementado um sistema de mixagem (`juce::MixerAudioSource`) que garante a reprodução simultânea de múltiplos sons (polifonia) e música de fundo sem cortes ou latência perceptível.
* **Execução de Música de Fundo (Backing Tracks):** Implementado um player dedicado para faixas de base, com controles de Play/Stop independentes dos pads, permitindo o acompanhamento musical conforme o escopo do projeto.
* **Sistema de Navegação e Menus:** Desenvolvida uma arquitetura de gerenciamento de telas (`MainComponent` como gerenciador), permitindo a navegação fluida entre o Menu Principal e a Grade de Performance.
* **Configuração Dinâmica de Kits (JSON):** O aplicativo lê arquivos JSON externos para configurar automaticamente a quantidade de pads, os sons associados (`.mp3`/`.wav`) e as faixas de fundo, permitindo a criação de novos kits sem recompilar o código.
    * Exemplo de estrutura implementada:
        ```json
        {
          "backingTrack": "base_bateria.mp3",
          "pads": [
            { "note": "C",
              "audioFile": "C.mp3",
              "color": "ffff0000"  // Cor Vermelha (ARGB)
            },
            { "note": "D",
              "audioFile": "D.mp3",
              "color": "ff00ff00"  // Cor Verde (ARGB)
            }
          ]
        }
        ```
* **Suporte Nativo a Multitouch:** A arquitetura de componentes do JUCE permitiu o reconhecimento imediato de múltiplos toques simultâneos no Android.
* **Compatibilidade Mobile Comprovada:** O protótipo foi compilado e testado com sucesso em dispositivo Android real (via USB Debugging), validando a performance e a estabilidade.

- [Vídeo Demonstrativo do Protótipo JUCE](https://www.youtube.com/watch?v=lgqJpiOi8es)

---

### 10.3 Justificativa para a Transição de Plataforma (Unity para JUCE)

Inicialmente, a Unity foi considerada devido à familiaridade no desenvolvimento de interfaces. Contudo, a experiência prática revelou diferenças significativas em relação aos requisitos de um instrumento musical virtual.

A decisão de transicionar para o JUCE foi fundamentada na necessidade de **performance de áudio em tempo real**. Enquanto a Unity exigiria contornos técnicos (workarounds) para gerenciar latência e polifonia, o JUCE oferece essas capacidades nativamente, sendo o padrão da indústria para software de áudio. Essa mudança estratégica redirecionou o esforço de desenvolvimento: de "resolver problemas da engine" para "implementar funcionalidades musicais".

---

### 10.4 Análise Comparativa da Plataforma de Desenvolvimento (JUCE vs. Unity)

* **Latência de Áudio:** O JUCE oferece controle direto sobre o buffer de áudio do hardware, resultando em latência mínima essencial para ritmo e percussão. Na Unity, atingir latência similar em Android é complexo e instável.
* **Fluxo de Trabalho:** O JUCE facilita a manipulação de dados binários e áudio (como o carregamento de JSON e samples em memória) de forma mais eficiente para este domínio do que o sistema de Assets da Unity.
* **Otimização de Tempo:** O conhecimento adquirido em JUCE é diretamente aplicável à versão final do produto e a futuros desenvolvimentos de plugins VST/AU, agregando valor profissional ao projeto e aos alunos envolvidos.

---

### 10.5 Recomendação Estratégica para o Desenvolvimento

Com base no sucesso do MVP e na validação técnica, **recomenda-se formalmente que o desenvolvimento do aplicativo PucPads continue sendo realizado integralmente na plataforma JUCE.**

Esta abordagem garante:
1.  **Estabilidade e Performance:** Uso de uma ferramenta desenhada especificamente para áudio.
2.  **Escalabilidade:** Facilidade para adicionar recursos futuros como efeitos (Reverb, Delay), gravação de áudio e integração MIDI avançada.
3.  **Manutenibilidade:** Código C++ moderno, modular e separado da lógica visual de uma game engine.

### Status Final do Projeto (Vídeo)
Confira o funcionamento final do MVP com todas as funcionalidades integradas:
- [**Assistir ao Vídeo do Status Final (YouTube Shorts)**](https://youtube.com/shorts/NXkurH1tSUU)
2.  **Escalabilidade:** Facilidade para adicionar recursos futuros como efeitos (Reverb, Delay), gravação de áudio e integração MIDI avançada.
3.  **Manutenibilidade:** Código C++ moderno, modular e separado da lógica visual de uma game engine.
