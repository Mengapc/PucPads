#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    Estruturas de dados e parser dos arquivos de kit (JSON).

    O schema v2 esta documentado em Arquivos_PucPads/KIT_SCHEMA.md.
    O parser continua aceitando o schema v1 (array "pads" na raiz, sem "pages"),
    tratando-o como um kit de pagina unica -- nenhum kit antigo deixa de carregar.
*/

//==============================================================================
// Um pad individual, ja com a cor resolvida (hex do JSON ou paleta da funcao musical).
struct KitPad
{
    juce::String note;        // Rotulo exibido no pad (opcional)
    juce::String audioFile;   // Nome do arquivo, ex.: "C.mp3"
    juce::Colour colour;      // Cor ativa resolvida
    juce::String function;    // "efeito" | "loop" | "melodia" | "harmonia" | ""
    int          chokeGroup = 0;     // 0 = sem grupo de exclusao mutua
    bool         oneShot    = false; // false = gate (toca enquanto pressionado)
    float        gainDb     = 0.0f;  // Ganho individual do sample
};

//==============================================================================
// Uma pagina (banco) de pads dentro do kit.
struct KitPage
{
    juce::String name;
    juce::Array<KitPad> pads;
};

//==============================================================================
inline constexpr int kKitDefaultColumns = 8;
inline constexpr int kKitDefaultRows    = 8;

// Limites aplicados pelo parser (espelhados em tools/validate_kits.py):
//   - grade: 1..16 por lado. Sem teto, um "grid" errado no JSON criaria
//     milhares de pads e travaria a UI ao abrir o template.
//   - ganho: -60..+12 dB. Valor absurdo estouraria o mix e a audicao do usuario.
inline constexpr int   kKitMaxGridSide = 16;
inline constexpr float kKitMinGainDb   = -60.0f;
inline constexpr float kKitMaxGainDb   = 12.0f;

struct KitDefinition
{
    juce::String name;
    juce::String author;
    juce::String description;
    juce::String backingTrack;
    int columns = kKitDefaultColumns;
    int rows    = kKitDefaultRows;
    juce::Array<KitPage> pages;

    bool isValid() const noexcept { return ! pages.isEmpty() && columns > 0 && rows > 0; }
    int  getNumPads() const noexcept;

    // Aceita schema v2 ("kit" + "pages") e schema v1 ("backingTrack" + "pads").
    static KitDefinition fromJson (const juce::var& json);

    // Paleta pedagogica (RF04): cor neon de cada funcao musical.
    static juce::Colour colourForFunction (const juce::String& functionName);
};
