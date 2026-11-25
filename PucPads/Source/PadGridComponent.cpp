#include <JuceHeader.h>
#include "PadGridComponent.h"
#include "PadComponent.h"
#include "UiTheme.h"

//==============================================================================
PadGridComponent::PadGridComponent (juce::MixerAudioSource& mixerToUse, const juce::String& kitResource)
    : mixerSource (mixerToUse) // Inicializa a referencia do mixer
{
    // 1. Registra formatos de audio (necessario tanto para a base quanto para os pads)
    formatManager.registerBasicFormats();

    // 2. Template ainda sem kit: grade fantasma, sem audio (o rodape do
    //    LaunchPadComponent desabilita play/stop nesse caso)
    if (kitResource.isEmpty())
    {
        DBG("Template vazio: montando grade fantasma "
            + juce::String(kit.columns) + "x" + juce::String(kit.rows));

        buildEmptyTemplate();
        return;
    }

    DBG("--------------------------------------------------");
    DBG("Carregando o kit: " + kitResource);

    // 3. Acessa os dados do arquivo JSON do kit (mitigando o mangling do BinaryData)
    int dataSize = 0;
    const char* jsonData = BinaryData::getNamedResource (kitResource.toUTF8(), dataSize);

    if (dataSize == 0 || jsonData == nullptr)
    {
        DBG("ERRO FATAL: O arquivo '" + kitResource + "' nao foi encontrado!");
        return;
    }

    // 4. Parse do kit (schema v2 e, por compatibilidade, v1)
    juce::MemoryInputStream inputStream (jsonData, dataSize, false);
    kit = KitDefinition::fromJson (juce::JSON::parse (inputStream));

    if (! kit.isValid())
    {
        DBG("ERRO FATAL: JSON do kit invalido (sem paginas ou com grade invalida). "
            "Verifique virgulas faltando ou chaves incorretas!");
        return;
    }

    DBG("Kit '" + kit.name + "' | paginas: " + juce::String(kit.pages.size())
        + " | pads: " + juce::String(kit.getNumPads()));

    // 5. Base musical + pads de todas as paginas
    if (kit.backingTrack.isNotEmpty())
        loadBackingTrack (kit.backingTrack);

    buildPages();
    buildPageButtons();
    showPage (0);

    DBG("Processo de criacao finalizado. Pads criados: " + juce::String(kit.getNumPads()));
    DBG("--------------------------------------------------");
}

PadGridComponent::~PadGridComponent()
{
    mixerSource.removeInputSource (&backgroundSource);
}

//==============================================================================
void PadGridComponent::loadBackingTrack (const juce::String& trackName)
{
    const juce::String resourceName = trackName.replaceCharacter ('.', '_');

    int dataSize = 0;
    const char* trackData = BinaryData::getNamedResource (resourceName.toUTF8(), dataSize);

    if (trackData == nullptr || dataSize <= 0)
    {
        DBG("AVISO: base musical '" + trackName + "' nao encontrada.");
        return;
    }

    auto* reader = formatManager.createReaderFor (
        std::make_unique<juce::MemoryInputStream> (trackData, dataSize, false));

    if (reader == nullptr)
    {
        DBG("AVISO: falha ao decodificar a base musical '" + trackName + "'.");
        return;
    }

    backgroundReaderSource.reset (new juce::AudioFormatReaderSource (reader, true));
    backgroundSource.setSource (backgroundReaderSource.get(), 0, nullptr, reader->sampleRate);
    backingLoaded = true;

    // Conecta a base ao Mixer Principal!
    mixerSource.addInputSource (&backgroundSource, false);
}

//==============================================================================
void PadGridComponent::playBackingTrack()
{
    if (! backingLoaded)
        return;

    backgroundSource.setPosition (0);
    backgroundSource.start();
}

void PadGridComponent::stopBackingTrack()
{
    backgroundSource.stop();
}

//==============================================================================
void PadGridComponent::buildPages()
{
    for (auto& kitPage : kit.pages)
    {
        auto* page = pageViews.add (new Page());
        page->name = kitPage.name;

        for (auto& padSpec : kitPage.pads)
        {
            const juce::String resourceName = padSpec.audioFile.replaceCharacter ('.', '_');

            int dataSize = 0;
            const char* audioData = BinaryData::getNamedResource (resourceName.toUTF8(), dataSize);

            if (audioData == nullptr || dataSize <= 0)
            {
                DBG("... FALHA! Recurso " + resourceName + " nao encontrado (pad ignorado).");
                continue;
            }

            auto* newPad = page->pads.add (new PadComponent (padSpec, audioData, dataSize, mixerSource));

            // O grid resolve os choke groups porque precisa enxergar todos os pads
            newPad->onTriggered = [this] (PadComponent* triggered) { handlePadTriggered (triggered); };

            addChildComponent (newPad); // fica oculto ate a pagina ser exibida
        }
    }
}

//==============================================================================
void PadGridComponent::buildEmptyTemplate()
{
    isGhostTemplate = true;

    auto* page = pageViews.add (new Page());
    page->name = "Vazio";

    const int numPads = kit.columns * kit.rows;

    for (int i = 0; i < numPads; ++i)
    {
        // KitPad sem audioFile + bytes nulos => pad fantasma (nao toca, nao entra no mixer)
        KitPad ghostSpec;
        auto* ghostPad = page->pads.add (new PadComponent (ghostSpec, nullptr, 0, mixerSource));

        addChildComponent (ghostPad);
    }

    showPage (0);
}

//==============================================================================
void PadGridComponent::buildPageButtons()
{
    // Com uma pagina unica nao ha navegacao: a faixa de paginas nem aparece
    if (pageViews.size() <= 1)
        return;

    for (int i = 0; i < pageViews.size(); ++i)
    {
        auto* page = pageViews.getUnchecked (i);
        const juce::String label = page->name.isNotEmpty() ? page->name : juce::String (i + 1);

        auto* button = pageButtons.add (new juce::TextButton (label));
        button->onClick = [this, i] { showPage (i); };

        UiTheme::applyButtonStyle (*button);
        addAndMakeVisible (button);
    }
}

//==============================================================================
void PadGridComponent::showPage (int pageIndex)
{
    if (pageViews.isEmpty())
        return;

    currentPage = juce::jlimit (0, pageViews.size() - 1, pageIndex);

    for (int i = 0; i < pageViews.size(); ++i)
    {
        const bool isActive = (i == currentPage);

        for (auto* pad : pageViews.getUnchecked (i)->pads)
        {
            pad->setVisible (isActive);

            // Sample em execucao de uma pagina que saiu de cena nao deve continuar tocando
            if (! isActive)
                pad->chokeStop();
        }
    }

    updatePageButtons();
    layoutPage();
    repaint();
}

void PadGridComponent::updatePageButtons()
{
    for (int i = 0; i < pageButtons.size(); ++i)
        UiTheme::applyButtonStyle (*pageButtons.getUnchecked (i), i == currentPage);
}

//==============================================================================
void PadGridComponent::stopAllPads()
{
    for (auto* page : pageViews)
        for (auto* pad : page->pads)
            pad->chokeStop();
}

//==============================================================================
void PadGridComponent::handlePadTriggered (PadComponent* triggerer)
{
    const int group = triggerer->getChokeGroup();

    if (group <= 0)
        return;

    // Choke group vale para o kit inteiro: um pad de outra pagina tambem e interrompido
    for (auto* page : pageViews)
    {
        for (auto* pad : page->pads)
        {
            if (pad != triggerer && pad->getChokeGroup() == group)
                pad->chokeStop();
        }
    }
}

//==============================================================================
void PadGridComponent::paint (juce::Graphics& g)
{
    g.fillAll (UiTheme::bordoEscuro());
}

//==============================================================================
void PadGridComponent::resized()
{
    auto bounds = getLocalBounds();

    // Faixa de paginas: so quando o kit realmente tem navegacao
    if (! pageButtons.isEmpty())
        layoutPageButtons (bounds.removeFromTop (pageStripHeight).reduced (8, 4));

    layoutPage();
}

//==============================================================================
void PadGridComponent::layoutPageButtons (juce::Rectangle<int> area)
{
    const int count = pageButtons.size();

    if (count == 0)
        return;

    const int gap = 6;
    const int buttonWidth = juce::jmax (40, (area.getWidth() - gap * (count - 1)) / count);

    for (int i = 0; i < count; ++i)
    {
        pageButtons.getUnchecked (i)->setBounds (area.removeFromLeft (buttonWidth));

        if (i < count - 1)
            area.removeFromLeft (gap);
    }
}

//==============================================================================
void PadGridComponent::layoutPage()
{
    if (pageViews.isEmpty())
        return;

    auto bounds = getLocalBounds();

    if (! pageButtons.isEmpty())
        bounds.removeFromTop (pageStripHeight);

    const int numCols = kit.columns;
    const int numRows = kit.rows;
    const int margin = 4;

    const int availableWidth  = bounds.getWidth()  - margin * (numCols + 1);
    const int availableHeight = bounds.getHeight() - margin * (numRows + 1);

    if (availableWidth <= 0 || availableHeight <= 0)
        return;

    const int padWidth  = availableWidth  / numCols;
    const int padHeight = availableHeight / numRows;

    const int startX = bounds.getX() + margin;
    const int startY = bounds.getY() + margin;

    auto& pads = pageViews.getUnchecked (currentPage)->pads;

    for (int i = 0; i < pads.size(); ++i)
    {
        const int col = i % numCols;
        const int row = i / numCols;

        if (row >= numRows)
            break;

        const int x = startX + (col * (padWidth + margin));
        const int y = startY + (row * (padHeight + margin));

        pads[i]->setBounds (x, y, padWidth, padHeight);
    }
}
