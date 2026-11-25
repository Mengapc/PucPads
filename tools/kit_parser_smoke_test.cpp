/*
    Teste de fumaca do parser de kits (PucPads/Source/KitDefinition.cpp).

    Nao faz parte do binario do app: e um alvo de desenvolvimento para validar o
    schema v1/v2 sem abrir a interface. Roda como aplicacao de console usando os
    modulos core/events/graphics do JUCE.

    Como rodar (a partir da raiz do repositorio):

        JUCE_DIR=/caminho/para/JUCE ./tools/run_kit_parser_test.sh

    O script cuida do JuceHeader minimo, dos fontes C embutidos no juce_graphics
    e das libs de sistema. Saida esperada: todas as linhas "ok" e exit code 0.
*/

#include <iostream>

#include <JuceHeader.h>
#include "KitDefinition.h"

namespace
{
    int failures = 0;

    void check (bool condition, const juce::String& description)
    {
        if (condition)
        {
            std::cout << "  ok   " << description << std::endl;
        }
        else
        {
            std::cout << "  FALHA " << description << std::endl;
            ++failures;
        }
    }

    KitDefinition parseFile (const juce::File& file)
    {
        return KitDefinition::fromJson (juce::JSON::parse (file.loadFileAsString()));
    }

    juce::String argb (juce::Colour colour)
    {
        return colour.toDisplayString (false);
    }

    //------------------------------------------------------------------
    void testMelodiaKit (const juce::File& file)
    {
        std::cout << "notas: " << file.getFileName() << std::endl;

        const auto kit = parseFile (file);

        check (kit.isValid(), "kit valido");
        check (kit.name == "Kit Melodia", "kit.name = Kit Melodia");
        check (kit.backingTrack == "Musica_Jogo.wav", "backingTrack preservado");
        check (kit.columns == 8 && kit.rows == 8, "grade 8x8");
        check (kit.pages.size() == 2, "2 paginas");
        check (kit.getNumPads() == 128, "128 pads no total (64 por pagina)");

        if (kit.pages.size() != 2)
            return;

        const auto& melodia = kit.pages.getReference (0);
        const auto& harmonia = kit.pages.getReference (1);

        check (melodia.name == "Melodia", "pagina 1 = Melodia");
        check (harmonia.name == "Harmonia", "pagina 2 = Harmonia");
        check (melodia.pads.size() == 64, "pagina 1 com 64 pads");
        check (harmonia.pads.size() == 64, "pagina 2 com 64 pads");

        const auto& gatePad = melodia.pads.getReference (0);
        check (argb (gatePad.colour) == argb (KitDefinition::colourForFunction ("melodia")),
               "cor do pad de melodia vem da paleta (rosa)");
        check (! gatePad.oneShot, "melodia usa gate (padrao)");
        check (gatePad.chokeGroup == 0, "melodia sem choke group");
        check (gatePad.note == "C" && gatePad.audioFile == "C.mp3", "pad 0 = C/C.mp3");

        const auto& chokedPad = harmonia.pads.getReference (5);
        check (argb (chokedPad.colour) == argb (KitDefinition::colourForFunction ("harmonia")),
               "cor do pad de harmonia vem da paleta (laranja)");
        check (chokedPad.oneShot, "harmonia usa oneshot");
        check (chokedPad.chokeGroup == 1, "harmonia no choke group 1");
    }

    //------------------------------------------------------------------
    void testDrumkit (const juce::File& file)
    {
        std::cout << "bateria: " << file.getFileName() << std::endl;

        const auto kit = parseFile (file);

        check (kit.isValid(), "kit valido");
        check (kit.name == "Kit Bateria", "kit.name = Kit Bateria");
        check (kit.pages.size() == 2, "2 paginas");

        if (kit.pages.size() != 2)
            return;

        check (kit.pages.getReference (0).name == "Loops", "pagina 1 = Loops");
        check (kit.pages.getReference (1).name == "Efeitos", "pagina 2 = Efeitos");
        check (kit.pages.getReference (0).pads.getReference (0).chokeGroup == 2,
               "loops no choke group 2");
        check (kit.pages.getReference (1).pads.getReference (0).chokeGroup == 0,
               "efeitos sem choke group");
    }

    //------------------------------------------------------------------
    void testSchemaV1Compat (const juce::File& file)
    {
        std::cout << "compatibilidade: " << file.getFileName() << std::endl;

        const auto kit = parseFile (file);

        check (kit.isValid(), "kit v1 valido");
        check (kit.pages.size() == 1, "v1 vira 1 pagina");
        check (kit.backingTrack == "Musica_Jogo.wav", "backingTrack lido da raiz (v1)");
        check (kit.getNumPads() == 2, "2 pads lidos");

        if (kit.pages.isEmpty() || kit.pages.getReference (0).pads.size() < 2)
            return;

        const auto& pads = kit.pages.getReference (0).pads;
        const auto& first = pads.getReference (0);
        const auto& second = pads.getReference (1);

        check (argb (first.colour) == argb (juce::Colour::fromString ("ffff0000")),
               "v1 mantem a cor hex declarada");
        check (! first.oneShot, "v1 sem playMode => gate");
        check (first.chokeGroup == 0, "v1 sem chokeGroup => 0");
        check (second.chokeGroup == 3, "v1 aceita chokeGroup opcional");
        check (kit.columns == 8 && kit.rows == 8, "v1 sem grid => 8x8");
    }

    //------------------------------------------------------------------
    void testOverridesAndFallbacks (const juce::File& file)
    {
        std::cout << "regras: " << file.getFileName() << std::endl;

        const auto kit = parseFile (file);

        check (kit.isValid(), "kit valido");

        if (kit.pages.isEmpty())
            return;

        const auto& pads = kit.pages.getReference (0).pads;

        check (pads.size() == 3, "3 pads validos (o pad sem audioFile e descartado)");
        check (argb (pads.getReference (0).colour) == argb (juce::Colour::fromString ("ff00ff00")),
               "'color' explicito vence a paleta da funcao");
        check (argb (pads.getReference (1).colour)
                   == argb (KitDefinition::colourForFunction ("inexistente")),
               "funcao desconhecida cai no dourado institucional");
        check (pads.getReference (2).gainDb == -6.0f, "gainDb lido corretamente");
        check (kit.columns == 4 && kit.rows == 4, "grid customizado (4x4) respeitado");
    }

    //------------------------------------------------------------------
}

int main()
{
    const auto root = juce::File::getCurrentWorkingDirectory();
    const auto assets = root.getChildFile ("PucPads").getChildFile ("Arquivos_PucPads");
    const auto scratch = juce::File::getSpecialLocation (juce::File::tempDirectory)
                             .getChildFile ("pucpads_kit_tests");

    scratch.createDirectory();

    testMelodiaKit (assets.getChildFile ("notes.json"));
    testDrumkit (assets.getChildFile ("drumkit.json"));

    // --- Fixtures inline: v1 e regras de override/fallback ---
    const auto v1File = scratch.getChildFile ("kit_v1.json");
    v1File.replaceWithText (R"({
        "backingTrack": "Musica_Jogo.wav",
        "pads": [
            { "audioFile": "C.mp3", "color": "ffff0000" },
            { "audioFile": "D.mp3", "chokeGroup": 3 }
        ]
    })");

    testSchemaV1Compat (v1File);

    const auto rulesFile = scratch.getChildFile ("kit_rules.json");
    rulesFile.replaceWithText (R"({
        "schemaVersion": 2,
        "kit": { "name": "Regras" },
        "grid": { "columns": 4, "rows": 4 },
        "pages": [
            { "name": "Unica", "pads": [
                { "audioFile": "C.mp3", "function": "melodia", "color": "ff00ff00" },
                { "audioFile": "D.mp3", "function": "inexistente" },
                { "audioFile": "E.mp3", "gainDb": -6.0 },
                { "note": "sem audio" }
            ] }
        ]
    })");

    testOverridesAndFallbacks (rulesFile);

    std::cout << (failures == 0 ? "\nOK: parser aprovado nos testes de fumaca."
                                : "\nFALHOU: " + juce::String (failures) + " verificacao(oes).")
              << std::endl;

    return failures == 0 ? 0 : 1;
}
