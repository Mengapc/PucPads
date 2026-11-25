//==============================================================================
/*
    Teste de fumaca (headless) da tela de LaunchPad.

    Valida a nova estrutura de navegacao sem abrir janela/audio device:
      - 8 botoes de template no rodape;
      - template 1 carrega notes.json (2 paginas, 128 pads);
      - templates sem kit abrem grade fantasma 8x8 (64 pads, sem paginas);
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
        check (counts.pads == 128, "template 1: 128 pads (encontrados: " + juce::String (counts.pads) + ")");
        check (counts.buttons == 2, "template 1: 2 botoes de pagina (encontrados: " + juce::String (counts.buttons) + ")");
        check (! firstGrid->isEmptyTemplate(), "template 1 nao e grade fantasma");
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
        check (counts.buttons == 0, "template 3: sem botoes de pagina");
        check (! isButtonEnabled (launchPad, "Play Base"), "Play Base desabilitado no template vazio");
    }

    check (firstGrid != nullptr && ! firstGrid->isVisible(), "grade do template 1 ficou oculta");

    // --- Volta ao template 1 (view reaproveitado) --------------------------
    std::cout << "== Volta ao template 1 ==" << std::endl;

    if (auto* again = findTemplateButton (launchPad, "1"))
        again->onClick();

    auto* backGrid = findVisibleGrid (launchPad);
    check (backGrid == firstGrid, "o view do template 1 foi reaproveitado (sem recriar AudioTransportSource)");
    check (isButtonEnabled (launchPad, "Play Base"), "Play Base volta a ficar habilitado no template 1");

    // --- Redimensionamento -------------------------------------------------
    launchPad.setBounds (0, 0, 800, 1000);

    if (auto* grid = findVisibleGrid (launchPad))
        check (grid->getBounds() == juce::Rectangle<int> (0, 60, 800, 870),
               "grade acompanha o redimensionamento: " + grid->getBounds().toString());

    std::cout << (failures == 0 ? "\nTODAS AS VERIFICACOES PASSARAM" : "\nFALHAS: " + juce::String (failures))
              << std::endl;

    return failures == 0 ? 0 : 1;
}
