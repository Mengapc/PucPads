#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    Cromo institucional da aplicacao (RNF03).

    Estes valores estavam repetidos em MenuComponent e PadGridComponent; ficam
    aqui para existir uma unica fonte da verdade. A semantica pedagogica das
    cores dos pads continua em KitDefinition::colourForFunction().
*/
namespace UiTheme
{
    inline juce::Colour bordo()        { return juce::Colour::fromString ("ff420012"); }
    inline juce::Colour bordoEscuro()  { return juce::Colour::fromString ("ff2a000b"); }
    inline juce::Colour dourado()      { return juce::Colour::fromString ("ffffd700"); }
    inline juce::Colour bordaDourada() { return juce::Colour::fromString ("ffb08d35"); }

    // Botao institucional: bordo com texto dourado. No estado ativo as cores se
    // invertem (dourado com texto bordo), servindo de indicador de selecao.
    inline void applyButtonStyle (juce::TextButton& button, bool active = false)
    {
        button.setColour (juce::TextButton::buttonColourId,  active ? dourado() : bordo());
        button.setColour (juce::TextButton::textColourOffId, active ? bordo()   : dourado());
        button.setColour (juce::TextButton::textColourOnId,  active ? bordo()   : dourado());
    }

    //------------------------------------------------------------------
    // Choke groups (RF02): terceira camada de cor do app.
    //
    //   preenchimento do pad -> funcao musical (RF04, vem do kit)
    //   borda dourada        -> cromo institucional (RNF03)
    //   anel + selo do grupo -> choke group (esta paleta)
    //
    // As cores abaixo evitam os matizes da paleta funcional (verde/azul/rosa/
    // laranja) e do cromo (bordo/dourado) para os tres codigos nao se
    // confundirem. Sao ciclicas: do 7o grupo em diante as cores se repetem e o
    // numero do selo passa a ser a referencia exata.
    //------------------------------------------------------------------
    inline juce::Colour chokeGroupColour (int group) noexcept
    {
        static const juce::Colour palette[] =
        {
            juce::Colour (0xff00e5ff), // ciano
            juce::Colour (0xffb06cff), // violeta
            juce::Colour (0xffff3b30), // vermelho
            juce::Colour (0xffd4ff3f), // lima
            juce::Colour (0xff00ffc8), // turquesa
            juce::Colour (0xffc9d6df)  // prata
        };

        constexpr int paletteSize = (int) (sizeof (palette) / sizeof (palette[0]));

        if (group <= 0)
            return juce::Colours::transparentBlack;

        return palette[(group - 1) % paletteSize];
    }
}
