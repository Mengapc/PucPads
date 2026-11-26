# Formato de arquivo de kit — PucPads (schema v2)

Documento de referência do **schema JSON dos kits** usado pelo PucPads. O arquivo ainda aceita
`pages[]` por compatibilidade, mas a UI atual é **um template = uma grade**: a variedade interna
vem dos **choke groups** (§9), não de páginas (§10).

- **Versão do schema:** `2`
- **Leitor:** `PucPads/Source/KitDefinition.h` / `KitDefinition.cpp`
- **Consumidores:** `PadGridComponent` (layout da grade, choke groups), `PadComponent` (áudio, cor, modo de disparo)
- **Arquivos em uso:** `Arquivos_PucPads/notes.json` (Kit Melodia) e `Arquivos_PucPads/drumkit.json` (Kit Bateria)

> O parser é **retrocompatível**: arquivos no formato antigo (v1) continuam carregando sem
> alteração. Ver §10.

---

## 1. Visão geral

Um arquivo de kit descreve:

1. **Metadados** do kit (nome, autor, descrição, base musical);
2. **Grade** de exibição (colunas × linhas);
3. **Páginas** — cada página é um "banco" de pads exibido por vez, com nome próprio;
4. **Pads** — cada pad aponta para um sample embutido no binário, tem uma função musical
   (que define a cor), um modo de disparo e, opcionalmente, um grupo de exclusão mútua.

Exemplo mínimo (1 página, 2 pads):

```json
{
  "schemaVersion": 2,
  "kit": {
    "name": "Kit Exemplo",
    "author": "PucPads",
    "description": "Exemplo minimo do schema v2",
    "backingTrack": "Musica_Jogo.wav"
  },
  "grid": { "columns": 8, "rows": 8 },
  "pages": [
    {
      "name": "Melodia",
      "pads": [
        { "note": "C", "audioFile": "C.mp3", "function": "melodia", "playMode": "gate" },
        { "note": "D", "audioFile": "D.mp3", "function": "melodia", "playMode": "gate" }
      ]
    }
  ]
}
```

---

## 2. Estrutura da raiz

| Campo | Tipo | Obrigatório | Default | Descrição |
|---|---|---|---|---|
| `schemaVersion` | int | recomendado | `1` | Versão do schema. Use `2` em arquivos novos. |
| `kit` | objeto | sim (v2) | — | Metadados do kit (§3). |
| `grid` | objeto | não | `8 × 8` | Grade de exibição (§4). |
| `pages` | array | sim (v2) | — | Páginas do kit, na ordem de exibição (§5). |
| `backingTrack` | string | não | — | Aceito na raiz **apenas** por compatibilidade v1 (§10). |

## 3. Objeto `kit`

| Campo | Tipo | Obrigatório | Descrição |
|---|---|---|---|
| `name` | string | sim | Nome do kit (exibido em logs). |
| `author` | string | não | Autor/curador do kit. |
| `description` | string | não | Texto livre (exibido em logs; reservado para a UI). |
| `backingTrack` | string | não | Nome do arquivo da base musical em `Arquivos_PucPads/`. Sem ele, os botões *Play/Stop Base* ficam inertes. |

## 4. Objeto `grid`

| Campo | Tipo | Default | Descrição |
|---|---|---|---|
| `columns` | int > 0 | `8` | Colunas usadas no layout. |
| `rows` | int > 0 | `8` | Linhas usadas no layout. |

A capacidade da grade é `columns × rows`. Pads além da capacidade **não são exibidos**
(o validador acusa isso como erro e `PadGridComponent::buildPads()` descarta os excedentes com
aviso no log). Com `8 × 8`, a grade comporta 64 pads.

## 5. Objeto de página

| Campo | Tipo | Obrigatório | Descrição |
|---|---|---|---|
| `name` | string | sim (v2) | Rótulo da página no arquivo (ex.: `"Grade"`). Deve ser único no arquivo. A UI atual exibe apenas a primeira página. |
| `pads` | array | sim | Array de objetos de pad (§6), na ordem de leitura (linha a linha, 8 por linha). |

Páginas sem nenhum pad válido são **descartadas** pelo parser (com aviso no log).

## 6. Objeto de pad

Todos os campos são opcionais na leitura; na prática `audioFile` é obrigatório — pads sem ele
são ignorados.

| Campo | Tipo | Default | Descrição |
|---|---|---|---|
| `audioFile` | string | — | Nome do arquivo em `Arquivos_PucPads/` (ex.: `"C.mp3"`). **Obrigatório.** |
| `note` | string | `""` | Rótulo curto desenhado no pad (ex.: `"C"`, `"Loop 3"`). |
| `function` | enum | `""` | `efeito` · `loop` · `melodia` · `harmonia` — define a cor pela paleta (§7). |
| `color` | string ARGB | — | Hex de 8 dígitos (`"ffff4fa3"`). **Tem prioridade sobre `function`** (customização). |
| `playMode` | enum | `gate` | `gate` = toca enquanto pressionado; `oneshot` = toca até o fim do sample (§8). |
| `chokeGroup` | int ≥ 0 | `0` | Grupo de exclusão mútua; `0` = sem grupo (§9). |
| `gainDb` | float | `0.0` | Ganho individual do pad, em dB (faixa aceita: −60 a +12). |

---

## 7. Cores (RF04)

A cor do pad é resolvida em **duas etapas**, nesta ordem:

1. `color` presente e não vazio → usa o hex informado;
2. caso contrário → usa a paleta da `function`;
3. `function` ausente ou desconhecida → **dourado institucional** `ffffd700`.

| `function` | Cor | Hex | Papel pedagógico |
|---|---|---|---|
| `efeito` | Verde | `ff2ecc71` | Efeitos pontuais (one-shot, sem preocupação rítmica) |
| `loop` | Azul | `ff1e90ff` | Loops/ostinatos, geralmente com choke group |
| `melodia` | Rosa | `ffff4fa3` | Notas e frases melódicas |
| `harmonia` | Laranja | `ffff8c1a` | Acordes e acompanhamento |
| *(fallback)* | Dourado | `ffffd700` | Cor institucional — indica pad sem classificação |

**Separação de responsabilidades:** bordô `ff420012` e dourado `ffffd700` são o **cromo
institucional** (fundo, bordas, botões) e continuam válidos como `color` explícito. As cores
semânticas acima são a camada **pedagógica** e não substituem o cromo.

> ⚠️ Atenção à ordem das letras: `juce::Colour::fromString` espera **ARGB** (`AARRGGBB`), não
> `RRGGBBAA`. O alfa deve ser o primeiro par (`ff` = opaco).

## 8. `playMode`

| Valor | Comportamento | Uso típico |
|---|---|---|
| `gate` (default) | Inicia em `mouseDown`, para em `mouseUp`. | Melodia, notas percussivas "seguradas" |
| `oneshot` | Inicia em `mouseDown` e toca até o fim do sample; o brilho acompanha a duração. | Efeitos, loops, acordes |

`gate` é o comportamento histórico do app — usar o default garante que nenhum pad antigo mude.

## 9. `chokeGroup` (exclusão mútua)

Pads com o **mesmo número de grupo** (`> 0`) se interrompem mutuamente: ao acionar um pad, todos
os outros do mesmo grupo **da mesma grade/template** são interrompidos com *fade-out* de 25 ms
(evita estalo de corte seco).

- `0` ou ausente = sem grupo (o pad nunca é interrompido por outro);
- O grupo é escopado **por kit** (kits diferentes não se interferem);
- Sugestão de convenção: `1..9` para "famílias" (ex.: `1` acordes, `2` loops, `3` efeitos longos).

## 10. Páginas no arquivo × grade na tela

- A tela do template **não tem mais seletor de páginas**: `PadGridComponent` monta uma única grade
  com a primeira página do kit e distribui os pads em `columns × rows`.
- Kits com mais de uma página continuam **válidos** (parser v2 intacto), mas só a primeira é
  exibida — `loadKit()` registra o aviso em `DBG`. Kits novos devem ter **uma** página.
- A variedade dentro do template vem do `chokeGroup` (§9), sinalizado no pad por um **anel** com a
  cor do grupo e um **selo circular** com o número (canto superior direito); pads sem grupo mantêm
  a borda dourada institucional.
- O selo é omitido em pads menores que 34 × 22 px; a borda colorida permanece.
- As cores dos grupos vivem em `UiTheme::chokeGroupColour()` (6 cores cíclicas, independentes da
  paleta funcional do §7) e o número do selo é a referência exata quando a paleta cicla.

---

## 11. Compatibilidade com o schema v1

| Aspecto | v1 | v2 |
|---|---|---|
| Metadados | campos soltos na raiz (`backingTrack`) | objeto `kit` |
| Bancos | `pads` na raiz (um único banco) | `pages[].pads` |
| Grade | fixa 8 × 8 | `grid` (default 8 × 8) |
| Cor | só `color` (hex) | `function` (paleta) + `color` (override) |
| `chokeGroup` / `playMode` / `gainDb` | — | suportados |

Regras do parser:

- se existir `pages`, ele **ignora** `pads` da raiz;
- se **não** existir `pages`, o array `pads` da raiz vira **uma** página chamada `"Pads"`;
- `backingTrack` da raiz é lido quando `kit.backingTrack` está ausente;
- campos desconhecidos são ignorados (permite extensão futura sem quebrar leitores antigos).

Um kit v1 idêntico ao antigo continua funcionando sem edição.

## 12. Guia de migração v1 → v2

1. Copie os campos `backingTrack` (e, se houver, `name`) da raiz para o objeto `kit`.
2. Crie `"schemaVersion": 2`.
3. Envolva o array `pads` existente em `"pages": [{ "name": "...", "pads": [...] }]`.
4. Troque `color` por `function` **apenas** onde a cor for a da paleta; mantenha `color` quando a
   cor for uma escolha estética específica.
5. Adicione `playMode` e `chokeGroup` onde o comportamento atual já é o desejado (ver §8/§9).
6. Rode `python3 tools/validate_kits.py` e depois o teste do parser (§14).
7. Regenere o `BinaryData` no Projucer (§13).

## 13. Checklist obrigatório após editar um kit

Os JSONs são **embutidos no binário** via `BinaryData`. Editar o arquivo **não** altera o app
enquanto o `JuceLibraryCode/BinaryData.cpp` não for regerado:

1. Abrir `PucPads/PucPads.jucer` no Projucer;
2. **Save Project** (regenera `JuceLibraryCode/BinaryData.cpp` e `BinaryData.h`);
3. Rebuild no IDE.

Se o JSON crescer/altera e o Projucer não for salvo, o app continua lendo os bytes antigos — o
sintoma típico é "mudei o kit e nada mudou".

## 14. Validação

Dois níveis, ambos executáveis fora do IDE:

```bash
# 1. Dados: schema, referências de áudio, faixas, nomes de página duplicados
python3 tools/validate_kits.py

# 2. Parser C++ de verdade: roda o KitDefinition contra os kits + fixtures v1
JUCE_DIR=/caminho/para/JUCE ./tools/run_kit_parser_test.sh
```

O teste do parser cobre: leitura dos dois kits, resolução de cor (paleta, override e fallback),
`playMode`, `chokeGroup`, `gainDb`, grade customizada, descarte de pad sem `audioFile` e
compatibilidade com um kit v1 sintético.

## 15. Limites práticos (débito conhecido)

Todos os pads da grade (até `columns × rows`, 64 no 8 × 8) são instanciados no carregamento e
registrados no `MixerAudioSource`. Com samples placeholder (≈45 KB) isso é irrelevante; com kits
reais a soma de fontes de áudio pode pesar. A evolução planejada é um **pool de vozes** — a
estrutura de `KitPad` já separa o dado do componente de UI e permite essa troca sem tocar no
formato do arquivo.

---

## Validacao aplicada pelo parser (e pelo `tools/validate_kits.py`)

O parser **nunca falha** por causa de um campo opcional mal preenchido: ele avisa em `DBG` e cai
no valor padrao, para que um erro de digitacao no kit nao deixe a tela em branco. Regras:

| Campo | Regra | Se estiver invalido |
| --- | --- | --- |
| `grid.columns` / `grid.rows` | inteiro entre 1 e **16** | negativo/zero -> 8; acima de 16 -> 16 |
| `gainDb` | numero entre **-60** e **+12** dB | fora da faixa -> limitado; nao numerico -> 0 dB |
| `color` | hex **AARRGGBB** (8 digitos) ou **RRGGBB** (6, com ou sem `#`) | invalido -> cor da `function` (nunca pad transparente) |
| `playMode` | `gate` (padrao) ou `oneshot` | desconhecido -> `gate` |
| `chokeGroup` | inteiro >= 0 (0 = sem grupo) | nao inteiro ou negativo -> 0 |
| `audioFile` | obrigatorio e existente em `Arquivos_PucPads/` | pad descartado com aviso |

Observacao: `Colour::fromString()` do JUCE **ignora** caracteres nao hexadecimais, entao um valor
de 6 digitos era lido como alpha `00` (pad invisivel). O parser completa o alpha para `ff`.

Kit com estrutura invalida (sem paginas ou grade impossivel) abre a **grade fantasma** em vez de
deixar a area de performance vazia.
