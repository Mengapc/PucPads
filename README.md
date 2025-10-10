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

### 10.1 Progresso Alcançado com o Protótipo em JUCE (MVP – Outubro/2025)

Em fase de prototipação inicial, a plataforma JUCE foi adotada para o desenvolvimento do MVP, com os seguintes marcos técnicos alcançados:

* **Implementação de Pads Interativos:** Criada uma estrutura de componentes de pads dinâmicos, que reagem visualmente ao toque (mudança de cor) e emitem sons com baixa latência.
* **Suporte a Multitouch:** A arquitetura de componentes do JUCE permite o reconhecimento e processamento de múltiplos toques simultâneos na tela, essencial para a performance musical.
* **Configuração Dinâmica via JSON:** Os pads (quantidade, sons associados) são configurados através de um arquivo JSON externo, possibilitando a criação de "kits" personalizados sem necessidade de recompilação.
    * Exemplo de configuração JSON:
        ```json
        {
          "pads": [
            { "note": "C", "audioFile": "C.mp3" },
            { "note": "D", "audioFile": "D.mp3" },
            // ... outros pads ...
          ]
        }
        ```
* **Arquitetura de Áudio Robusta:** Foi implementado um sistema de mixagem de áudio (`juce::MixerAudioSource`) no componente principal, garantindo a reprodução simultânea de múltiplos samples de forma eficiente e sem sobrecarga do sistema.
* **Compatibilidade Mobile:** O protótipo foi compilado e testado com sucesso em um dispositivo Android real (via USB Debugging), confirmando a funcionalidade de toque, multitouch e performance de áudio no ambiente alvo.

- [Vídeo demonstrativo](https://www.youtube.com/watch?v=lgqJpiOi8es)

### 10.2 Progresso Alcançado com o Protótipo em Unity (MVP – Outubro/2025)

Em uma fase exploratória inicial, a plataforma Unity foi utilizada para prototipar as funcionalidades centrais do aplicativo. O objetivo era validar a capacidade da engine de atender aos requisitos básicos do projeto. Os seguintes marcos foram alcançados:

* **Sistema de Gerenciamento de Áudio:** Foi desenvolvido um sistema de AudioManager para centralizar e controlar a reprodução dos sons. Utilizou-se um padrão de instanciar dinamicamente um Prefab contendo um componente AudioSource para cada som disparado. O objeto era então destruído automaticamente ao final da reprodução do AudioClip, uma técnica para gerenciamento de recursos.

* **Controle de Input Multiplataforma:** Implementado o Input System da Unity, criando uma base de controle que reconhece e responde a eventos tanto do mouse (para testes em desktop) quanto de toque único (single-touch) na tela.

* **Implementação dos Pads Interativos:** Criada uma interface de usuário com componentes de pads, cada um contendo um script que referenciava um AudioClip específico. Ao receber um evento de input, o script do pad comunicava-se com o AudioManager para disparar o som correspondente.

 - [Vídeo Demonstrativo](https://www.youtube.com/watch?v=NwCAiN9RlMc)

### Próximos Passos e Limitações Identificadas
Durante o desenvolvimento na Unity, os seguintes itens foram identificados como os próximos passos necessários para atingir a paridade com o protótipo em JUCE e os requisitos do MVP:

* **Implementação de Multitouch:** O sistema atual responde a um único toque. A implementação de multitouch exigiria um desenvolvimento adicional para rastrear múltiplos fingerIds e gerenciar seus estados (Began, Ended) de forma independente.

* **Configuração de Kits via JSON:** A configuração de sons nos pads foi feita manualmente, associando os AudioClips através do inspetor da Unity. O próximo grande passo seria desenvolver um sistema para ler arquivos JSON e popular a grade de pads dinamicamente.

* **Feedback Visual:** A resposta visual dos pads ao toque (mudança de cor) ainda não havia sido implementada.

* **Latência de Áudio:** Embora funcional, a latência do sistema de áudio padrão da Unity foi identificada como um ponto de atenção que necessitaria de testes aprofundados e possíveis otimizações, especialmente para o ambiente mobile.

### 10.3 Análise Comparativa da Plataforma de Desenvolvimento (JUCE vs. Unity)

Conforme o plano inicial de pesquisa, foram avaliadas as capacidades de ambas as plataformas para o desenvolvimento do protótipo e da versão final. A experiência prática com o JUCE revelou pontos cruciais que impactam a eficiência e a qualidade do projeto:

* **Latência de Áudio:** O JUCE, sendo um framework nativo especializado em áudio, oferece controle direto sobre o hardware e uma arquitetura otimizada para baixíssima latência na reprodução de sons. Atingir o mesmo nível de responsividade na Unity exigiria soluções complexas e, possivelmente, a integração de plugins de áudio de terceiros, com resultados incertos em termos de performance e compatibilidade mobile.
* **Tratamento de Áudio Multicanal e Polifonia:** A gestão de múltiplos `AudioSources` simultâneos, mixagem e aplicação de efeitos são funcionalidades nativas e altamente eficientes no JUCE. Na Unity, essa implementação demandaria um esforço considerável para otimizar o uso de recursos e evitar cortes (clipping) ou travamentos.
* **Otimização de Tempo e Esforço:** A pesquisa inicial para a Unity (Seção 8.1) detalhava a necessidade de aprender a contornar as limitações de áudio e de UI complexas. O desenvolvimento atual em JUCE demonstra que o tempo e o aprendizado estão sendo investidos diretamente na plataforma que será utilizada na entrega final do aplicativo, eliminando o retrabalho de migração. A implementação de carregamento de presets via JSON na Unity, por exemplo, seria um estudo que precisaria ser refeito em um framework de áudio nativo.

### 10.4 Recomendação Estratégica para o Desenvolvimento

Com base no progresso alcançado, nas capacidades intrínsecas do framework JUCE para aplicações de áudio e na otimização do plano de trabalho, **recomenda-se formalmente que o desenvolvimento do aplicativo PucPads continue sendo realizado integralmente na plataforma JUCE.**

Esta abordagem garante:

* **Consistência e Estabilidade:** A base técnica já estabelecida é a mesma que será utilizada na versão final, garantindo um desenvolvimento contínuo e mais estável.
* **Performance Inerente:** O requisito crítico de baixa latência e alta fidelidade de áudio é nativamente endereçado pelo JUCE, evitando desafios técnicos adicionais para alcançar padrões profissionais.
* **Foco na Funcionalidade Musical:** Ao eliminar a necessidade de adaptar uma engine de jogos para uma aplicação de áudio, a equipe pode concentrar seus esforços na implementação das funcionalidades musicais avançadas (integração MIDI, efeitos, sequenciamento) e na experiência do usuário.
* **Alinhamento com Padrões da Indústria:** O JUCE é amplamente utilizado na indústria de tecnologia musical para o desenvolvimento de plugins de áudio (VST/AU) e DAWs, posicionando o projeto PucPads em uma plataforma tecnicamente respeitável e preparada para futuras expansões.

