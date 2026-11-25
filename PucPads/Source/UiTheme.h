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
}
