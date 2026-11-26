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
        check (kit.pages.size() == 1, "1 pagina: o template e uma grade unica");
        check (kit.getNumPads() == 64, "64 pads na grade");

        if (kit.pages.size() != 1)
            return;

        const auto& grade = kit.pages.getReference (0);
        check (grade.pads.size() == 64, "64 pads na pagina unica");

        // Linhas 1-2: melodia (gate, sem choke group)
        const auto& melodiaPad = grade.pads.getReference (0);
        check (melodiaPad.note == "C" && melodiaPad.audioFile == "C.mp3", "pad 0 = C/C.mp3");
        check (argb (melodiaPad.colour) == argb (KitDefinition::colourForFunction ("melodia")),
               "cor do pad de melodia vem da paleta (rosa)");
        check (! melodiaPad.oneShot, "melodia usa gate (padrao)");
        check (melodiaPad.chokeGroup == 0, "melodia sem choke group");

        // Linhas 3-4: harmonia no grupo 1
        const auto& harmoniaPad = grade.pads.getReference (16);
        check (argb (harmoniaPad.colour) == argb (KitDefinition::colourForFunction ("harmonia")),
               "cor do pad de harmonia vem da paleta (laranja)");
        check (harmoniaPad.oneShot, "harmonia usa oneshot");
        check (harmoniaPad.chokeGroup == 1, "harmonia no choke group 1");

        // Linhas 5-6 e 7-8: loops e efeitos, com grupos proprios na MESMA grade
        check (grade.pads.getReference (32).chokeGroup == 2, "loops no choke group 2");
        check (grade.pads.getReference (48).chokeGroup == 3, "efeitos no choke group 3");

        int porGrupo[4] = { 0, 0, 0, 0 };

        for (const auto& pad : grade.pads)
            if (pad.chokeGroup >= 0 && pad.chokeGroup < 4)
                ++porGrupo[pad.chokeGroup];

        check (porGrupo[0] == 16 && porGrupo[1] == 16 && porGrupo[2] == 16 && porGrupo[3] == 16,
               "4 familias de 16 pads (sem grupo + choke groups 1, 2 e 3)");
    }

    //------------------------------------------------------------------
    void testDrumkit (const juce::File& file)
    {
        std::cout << "bateria: " << file.getFileName() << std::endl;

        const auto kit = parseFile (file);

        check (kit.isValid(), "kit valido");
        check (kit.name == "Kit Bateria", "kit.name = Kit Bateria");
        check (kit.pages.size() == 1, "1 pagina: loops e efeitos na mesma grade");

        if (kit.pages.size() != 1)
            return;

        const auto& grade = kit.pages.getReference (0);
        check (grade.pads.size() == 64, "64 pads na grade");
        check (grade.pads.getReference (0).chokeGroup == 1, "loops no choke group 1");
        check (grade.pads.getReference (32).chokeGroup == 2, "efeitos com choke no grupo 2");
        check (grade.pads.getReference (56).chokeGroup == 0, "efeitos livres sem choke group");
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
    void testParserLimits (const juce::File& file)
    {
        std::cout << "limites: " << file.getFileName() << std::endl;

        const auto kit = parseFile (file);

        check (kit.isValid(), "kit com valores fora da faixa continua valido");
        check (kit.columns == 16, "grid.columns acima do limite e truncado em 16");
        check (kit.rows == 8, "grid.rows negativo cai no padrao 8");

        if (kit.pages.isEmpty())
            return;

        const auto& pads = kit.pages.getReference (0).pads;
        check (pads.size() == 8, "8 pads validos");

        if (pads.size() < 8)
            return;

        check (pads.getReference (0).gainDb == 12.0f, "gainDb acima de +12 dB e limitado");
        check (pads.getReference (1).gainDb == -60.0f, "gainDb abaixo de -60 dB e limitado");
        check (argb (pads.getReference (2).colour)
                   == argb (KitDefinition::colourForFunction ("melodia")),
               "'color' invalido cai na cor da funcao (nao vira pad transparente)");
        check (argb (pads.getReference (3).colour) == argb (juce::Colour::fromString ("ffff00ff")),
               "cor de 6 digitos (#ff00ff) e completada com alpha opaco");
        check (! pads.getReference (4).oneShot, "playMode desconhecido cai em gate");
        check (pads.getReference (5).gainDb == 0.0f, "gainDb nao numerico vira 0 dB");
        check (pads.getReference (6).chokeGroup == 0, "chokeGroup nao inteiro e ignorado");
        check (pads.getReference (7).chokeGroup == 0, "chokeGroup negativo e ignorado");
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

    // --- Fixture inline: limites do parser (grade, ganho, cor, playMode) ---
    const auto limitsFile = scratch.getChildFile ("kit_limits.json");
    limitsFile.replaceWithText (R"({
        "schemaVersion": 2,
        "kit": { "name": "Limites" },
        "grid": { "columns": 999, "rows": -3 },
        "pages": [
            { "name": "Unica", "pads": [
                { "audioFile": "C.mp3", "gainDb": 40.0 },
                { "audioFile": "D.mp3", "gainDb": -200 },
                { "audioFile": "E.mp3", "function": "melodia", "color": "verde" },
                { "audioFile": "F.mp3", "color": "#ff00ff" },
                { "audioFile": "G.mp3", "playMode": "hold" },
                { "audioFile": "C.mp3", "gainDb": "alto" },
                { "audioFile": "D.mp3", "chokeGroup": "g1" },
                { "audioFile": "E.mp3", "chokeGroup": -4 }
            ] }
        ]
    })");

    testParserLimits (limitsFile);

    std::cout << (failures == 0 ? "\nOK: parser aprovado nos testes de fumaca."
                                : "\nFALHOU: " + juce::String (failures) + " verificacao(oes).")
              << std::endl;

    return failures == 0 ? 0 : 1;
}
