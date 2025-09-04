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
