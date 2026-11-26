//==============================================================================
/*
    Teste de fumaca (headless) da tela de LaunchPad.

    Valida a nova estrutura de navegacao sem abrir janela/audio device:
      - 8 botoes de template no rodape;
      - template 1 carrega notes.json (grade unica, 64 pads, 3 choke groups);
      - templates sem kit abrem grade fantasma 8x8 (64 pads);
      - sem seletor de paginas dentro da grade (choke groups substituem as paginas);
      - play/stop da base ficam desabilitados quando nao ha base musical;
      - o view de um template e reaproveitado ao alternar entre templates.

    As fontes de dados (BinaryData) sao substituidas por leitura direta dos
    arquivos em PucPads/Arquivos_PucPads, para nao depender do BinaryData.cpp.
*/
//==============================================================================
#include <JuceHeader.h>
#include "LaunchPadComponent.h"
#include "PadComponent.h"

#include <cstdlib>

namespace
{
    int failures = 0;

    void check (bool condition, const juce::String& description)
    {
        std::cout << (condition ? "  [OK]   " : "  [FALHA] ") << description << std::endl;

        if (! condition)
            ++failures;
    }

    // Conta os filhos de um PadGridComponent por tipo
    struct GridCounts
    {
        int pads = 0;
        int buttons = 0;
    };

    GridCounts countGrid (juce::Component& grid)
    {
        GridCounts counts;

        for (int i = 0; i < grid.getNumChildComponents(); ++i)
        {
            auto* child = grid.getChildComponent (i);

            if (dynamic_cast<PadComponent*> (child) != nullptr)
                ++counts.pads;
            else if (dynamic_cast<juce::TextButton*> (child) != nullptr)
                ++counts.buttons;
        }

        return counts;
    }

    // Quantos pads de cada choke group existem na grade (RF02)
    int countPadsInGroup (juce::Component& grid, int group)
    {
        int total = 0;

        for (int i = 0; i < grid.getNumChildComponents(); ++i)
            if (auto* pad = dynamic_cast<PadComponent*> (grid.getChildComponent (i)))
                if (pad->getChokeGroup() == group)
                    ++total;

        return total;
    }

    // Primeiro pad de um choke group (occurrence = 0 e o primeiro da grade)
    PadComponent* findPadInGroup (juce::Component& grid, int group, int occurrence)
    {
        int seen = 0;

        for (int i = 0; i < grid.getNumChildComponents(); ++i)
            if (auto* pad = dynamic_cast<PadComponent*> (grid.getChildComponent (i)))
                if (pad->getChokeGroup() == group && seen++ == occurrence)
                    return pad;

        return nullptr;
    }

    // Bombeia a fila de mensagens: necessario para os timers do fade de choke
    // (Timer::callAfterDelay) rodarem. Requer JUCE_MODAL_LOOPS_PERMITTED=1,
    // definido no script de build deste teste.
    void runMessageLoop (int milliseconds)
    {
        juce::MessageManager::getInstance()->runDispatchLoopUntil (milliseconds);
    }

    float visibleGridBackingGain (LaunchPadComponent& launchPad)
    {
        for (int i = 0; i < launchPad.getNumChildComponents(); ++i)
            if (auto* grid = dynamic_cast<PadGridComponent*> (launchPad.getChildComponent (i)))
                if (grid->isVisible())
                    return grid->getBackingGain();

        return -1.0f;
    }

    PadGridComponent* findVisibleGrid (LaunchPadComponent& launchPad)
    {
        for (int i = 0; i < launchPad.getNumChildComponents(); ++i)
        {
            auto* child = launchPad.getChildComponent (i);

            if (auto* grid = dynamic_cast<PadGridComponent*> (child))
                if (grid->isVisible())
                    return grid;
        }

        return nullptr;
    }

    juce::TextButton* findTemplateButton (LaunchPadComponent& launchPad, const juce::String& label)
    {
        for (int i = 0; i < launchPad.getNumChildComponents(); ++i)
        {
            auto* child = launchPad.getChildComponent (i);

            if (auto* button = dynamic_cast<juce::TextButton*> (child))
                if (button->getButtonText() == label && button->getParentComponent() == &launchPad)
                    return button;
        }

        return nullptr;
    }


    // Cria um WAV estereo silencioso valido, para exercitar a base musical
    bool writeSilentWav (const juce::File& file, int sampleRate, double seconds)
    {
        juce::AudioBuffer<float> buffer (2, static_cast<int> (sampleRate * seconds));
        buffer.clear();

        juce::WavAudioFormat format;
        file.deleteFile(); // os assets do repo sao ponteiros LFS: garante arquivo novo
        std::unique_ptr<juce::FileOutputStream> stream (file.createOutputStream());

        if (stream == nullptr)
            return false;

        std::unique_ptr<juce::AudioFormatWriter> writer (
            format.createWriterFor (stream.get(), sampleRate, 2, 16, {}, 0));

        if (writer == nullptr)
            return false;

        stream.release(); // posse passa para o writer
        return writer->writeFromAudioSampleBuffer (buffer, 0, buffer.getNumSamples());
    }

    // Copia os assets do repositorio para um diretorio temporario e troca o
    // placeholder Musica_Jogo.wav por um WAV valido.
    juce::File prepareAssets()
    {
        const auto source = juce::File::getCurrentWorkingDirectory()
                                .getChildFile ("PucPads").getChildFile ("Arquivos_PucPads");
        const auto target = juce::File::getSpecialLocation (juce::File::tempDirectory)
                                .getChildFile ("pucpads-launchpad-assets");

        target.deleteRecursively();
        source.copyDirectoryTo (target);

        const auto wav = target.getChildFile ("Musica_Jogo.wav");
        std::cout << (writeSilentWav (wav, 44100, 0.25) ? "Base musical de teste criada"
                                                       : "FALHA ao criar base musical de teste")
                  << std::endl;

        // Os samples do repositorio sao ponteiros LFS: gera WAVs validos no lugar
        // para que os pads carreguem audio de verdade (testes de gate/choke).
        int samplesCriados = 0;

        for (const auto* name : { "C.mp3", "D.mp3", "E.mp3", "F.mp3", "G.mp3" })
            if (writeSilentWav (target.getChildFile (name), 44100, 2.0))
                ++samplesCriados;

        std::cout << "Samples de teste criados: " << samplesCriados << "/5" << std::endl;

        return target;
    }

    bool isButtonEnabled (LaunchPadComponent& launchPad, const juce::String& label)
    {
        for (int i = 0; i < launchPad.getNumChildComponents(); ++i)
        {
            auto* child = launchPad.getChildComponent (i);

            if (auto* button = dynamic_cast<juce::TextButton*> (child))
                if (button->getButtonText() == label)
                    return button->isEnabled();
        }

        return false;
    }
}

//==============================================================================
int main()
{
    juce::ScopedJuceInitialiser_GUI juceInitialiser;

    // Assets de teste: copia do repositorio com uma base musical valida
    const auto assets = prepareAssets();
    setenv ("PADPADS_ASSETS", assets.getFullPathName().toRawUTF8(), 1);

    std::cout << "== LaunchPad: estrutura dos 8 templates ==" << std::endl;

    juce::MixerAudioSource mixer;
    LaunchPadComponent launchPad (mixer);
    launchPad.setBounds (0, 0, 450, 700);

    // --- Rodape ------------------------------------------------------------
    int templateButtons = 0;

    for (int i = 0; i < launchPad.getNumChildComponents(); ++i)
        if (auto* button = dynamic_cast<juce::TextButton*> (launchPad.getChildComponent (i)))
            if (button->getButtonText().containsOnly ("12345678") && button->getButtonText().length() == 1)
                ++templateButtons;

    check (templateButtons == 8, "8 botoes numerados no rodape (encontrados: " + juce::String (templateButtons) + ")");

    for (int i = 1; i <= 8; ++i)
        check (findTemplateButton (launchPad, juce::String (i)) != nullptr,
               "botao do template " + juce::String (i) + " existe");

    // --- Template 1 (notes.json) -------------------------------------------
    auto* firstGrid = findVisibleGrid (launchPad);
    check (firstGrid != nullptr, "template 1 tem uma grade visivel");

    if (firstGrid != nullptr)
    {
        const auto counts = countGrid (*firstGrid);
        check (counts.pads == 64, "template 1: 64 pads em uma unica grade (encontrados: " + juce::String (counts.pads) + ")");
        check (firstGrid->getNumPads() == 64, "template 1: getNumPads() = 64");
        check (counts.buttons == 0, "template 1: sem seletor de paginas (encontrados: " + juce::String (counts.buttons) + " botoes)");
        check (! firstGrid->isEmptyTemplate(), "template 1 nao e grade fantasma");

        // RF02: as tres familias com choke convivem com os pads sem grupo, na mesma grade
        check (countPadsInGroup (*firstGrid, 0) == 16, "template 1: 16 pads sem choke group (melodia)");
        check (countPadsInGroup (*firstGrid, 1) == 16, "template 1: 16 pads no choke group 1 (harmonia)");
        check (countPadsInGroup (*firstGrid, 2) == 16, "template 1: 16 pads no choke group 2 (loops)");
        check (countPadsInGroup (*firstGrid, 3) == 16, "template 1: 16 pads no choke group 3 (efeitos)");

        check (firstGrid->getBounds() == juce::Rectangle<int> (0, 60, 450, 570),
               "grade ocupa a area entre cabecalho (60) e rodape (70): " + firstGrid->getBounds().toString());
        check (firstGrid->hasBackingTrack(), "template 1 carregou a base musical");
        check (isButtonEnabled (launchPad, "Play Base"), "Play Base habilitado com base musical");
    }

    // --- Template 3 (sem kit) ---------------------------------------------
    std::cout << "== Template 3: grade fantasma ==" << std::endl;

    if (auto* third = findTemplateButton (launchPad, "3"))
        third->onClick();

    auto* ghostGrid = findVisibleGrid (launchPad);
    check (ghostGrid != nullptr && ghostGrid != firstGrid, "trocar de template criou/exibiu outra grade");

    if (ghostGrid != nullptr)
    {
        const auto counts = countGrid (*ghostGrid);
        check (ghostGrid->isEmptyTemplate(), "template 3 e grade fantasma");
        check (counts.pads == 64, "template 3: 64 pads fantasma 8x8 (encontrados: " + juce::String (counts.pads) + ")");
        check (counts.buttons == 0, "template 3: nenhum botao dentro da grade");
        check (countPadsInGroup (*ghostGrid, 0) == 64, "template 3: pads fantasma sem choke group");
        check (! isButtonEnabled (launchPad, "Play Base"), "Play Base desabilitado no template vazio");

        if (auto* ghostPad = findPadInGroup (*ghostGrid, 0, 0))
        {
            ghostPad->trigger();
            check (! ghostPad->getTransportSource().isPlaying(), "pad fantasma nao toca ao ser acionado");
            check (ghostPad->isEmptyPad(), "pad fantasma se identifica como vazio");
        }
    }

    check (firstGrid != nullptr && ! firstGrid->isVisible(), "grade do template 1 ficou oculta");

    // --- Volta ao template 1 (view reaproveitado) --------------------------
    std::cout << "== Volta ao template 1 ==" << std::endl;

    if (auto* again = findTemplateButton (launchPad, "1"))
        again->onClick();

    auto* backGrid = findVisibleGrid (launchPad);
    check (backGrid == firstGrid, "o view do template 1 foi reaproveitado (sem recriar AudioTransportSource)");
    check (isButtonEnabled (launchPad, "Play Base"), "Play Base volta a ficar habilitado no template 1");

    // --- Comportamento de audio: gate e choke group (RF02) ------------------
    std::cout << "== Pads: gate, choke group e volume da base ==" << std::endl;

    if (firstGrid != nullptr)
    {
        auto* gatePad = findPadInGroup (*firstGrid, 0, 0); // melodia: gate, sem grupo
        auto* chokeA  = findPadInGroup (*firstGrid, 2, 0); // loops: choke group 2
        auto* chokeB  = findPadInGroup (*firstGrid, 2, 1);

        check (gatePad != nullptr && chokeA != nullptr && chokeB != nullptr,
               "pads de teste encontrados na grade");

        if (gatePad != nullptr)
        {
            check (! gatePad->isEmptyPad(), "pad de melodia carregou audio");

            gatePad->trigger();
            check (gatePad->getTransportSource().isPlaying(), "gate: toca ao acionar");

            gatePad->releaseGate();
            check (! gatePad->getTransportSource().isPlaying(), "gate: para ao soltar (sem nota presa)");

            gatePad->releaseGate();
            check (! gatePad->getTransportSource().isPlaying(), "gate: soltura repetida e inofensiva");
        }

        if (chokeA != nullptr && chokeB != nullptr)
        {
            chokeA->trigger();
            check (chokeA->getTransportSource().isPlaying(), "choke: pad do grupo 2 toca");

            chokeB->trigger(); // mesmo grupo: deve cortar A com fade
            check (chokeA->getTransportSource().getGain() == 0.0f,
                   "choke: vizinho do mesmo grupo zera o ganho do pad anterior");

            runMessageLoop (120);
            check (! chokeA->getTransportSource().isPlaying(),
                   "choke: pad anterior e parado depois do fade");
            check (chokeB->getTransportSource().isPlaying(),
                   "choke: o pad acionado continua tocando");

            // Reacionar dentro da janela de fade cancela o corte pendente
            chokeA->trigger();
            chokeB->trigger();
            chokeA->trigger();
            runMessageLoop (120);

            check (chokeA->getTransportSource().isPlaying(),
                   "choke: reacionar o pad dentro do fade cancela o corte");
        }
    }

    // --- Volume da base musical --------------------------------------------
    launchPad.setBackingGain (0.5f);
    check (visibleGridBackingGain (launchPad) == 0.5f,
           "volume da base aplicado ao template ativo");

    if (auto* fourth = findTemplateButton (launchPad, "4"))
        fourth->onClick();

    check (visibleGridBackingGain (launchPad) == 0.5f,
           "template criado depois herda o volume da base");

    if (auto* again = findTemplateButton (launchPad, "1"))
        again->onClick();

    launchPad.setBackingGain (1.0f);
    check (visibleGridBackingGain (launchPad) == 1.0f, "volume volta a 100% para o teste de layout");

    // --- Redimensionamento -------------------------------------------------
    launchPad.setBounds (0, 0, 800, 1000);

    if (auto* grid = findVisibleGrid (launchPad))
        check (grid->getBounds() == juce::Rectangle<int> (0, 60, 800, 870),
               "grade acompanha o redimensionamento: " + grid->getBounds().toString());

    std::cout << (failures == 0 ? "\nTODAS AS VERIFICACOES PASSARAM" : "\nFALHAS: " + juce::String (failures))
              << std::endl;

    return failures == 0 ? 0 : 1;
}
