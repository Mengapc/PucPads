#include <JuceHeader.h>
#include "PadGridComponent.h"
#include "PadComponent.h"

//==============================================================================
namespace
{
    // Estilo institucional: bordo (#420012) com texto dourado (#FFD700).
    // Na aba ativa as cores se invertem, servindo de indicador de pagina.
    void applyInstitutionalStyle (juce::TextButton& button, bool active = false)
    {
        const juce::Colour bordo   = juce::Colour::fromString ("ff420012");
        const juce::Colour dourado = juce::Colour::fromString ("ffffd700");

        button.setColour (juce::TextButton::buttonColourId,  active ? dourado : bordo);
        button.setColour (juce::TextButton::textColourOffId, active ? bordo   : dourado);
        button.setColour (juce::TextButton::textColourOnId,  active ? bordo   : dourado);
    }
}

//==============================================================================
PadGridComponent::PadGridComponent(juce::MixerAudioSource& mixerToUse, juce::String kitFileToLoad)
    : mixerSource(mixerToUse) // Inicializa a referencia do mixer
{
    DBG("--------------------------------------------------");
    DBG("Carregando o kit: " + kitFileToLoad);

    // 1. Registra formatos de audio (para ler o MP3 da base)
    formatManager.registerBasicFormats();

    // 2. Botoes sempre existem: mesmo com kit invalido o usuario consegue voltar
    applyInstitutionalStyle (backButton);
    applyInstitutionalStyle (playButton);
    applyInstitutionalStyle (stopButton);

    addAndMakeVisible(backButton);
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);

    playButton.onClick = [this] { backgroundSource.setPosition(0); backgroundSource.start(); };
    stopButton.onClick = [this] { backgroundSource.stop(); };

    // 3. Acessa os dados do arquivo JSON ESPECIFICADO
    int dataSize = 0;
    const char* jsonData = BinaryData::getNamedResource(kitFileToLoad.toUTF8(), dataSize);

    if (dataSize == 0 || jsonData == nullptr)
    {
        DBG("ERRO FATAL: O arquivo '" + kitFileToLoad + "' nao foi encontrado!");
        return;
    }

    // 4. Parse do kit (schema v2 e, por compatibilidade, v1)
    juce::MemoryInputStream inputStream(jsonData, dataSize, false);
    kit = KitDefinition::fromJson(juce::JSON::parse(inputStream));

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
        loadBackingTrack(kit.backingTrack);

    buildPages();
    buildPageButtons();
    showPage(0);

    DBG("Processo de criacao finalizado. Pads criados: " + juce::String(kit.getNumPads()));
    DBG("--------------------------------------------------");
}

PadGridComponent::~PadGridComponent()
{
    mixerSource.removeInputSource(&backgroundSource);
}

//==============================================================================
void PadGridComponent::loadBackingTrack(const juce::String& trackName)
{
    const juce::String resourceName = trackName.replaceCharacter('.', '_');

    int dataSize = 0;
    const char* trackData = BinaryData::getNamedResource(resourceName.toUTF8(), dataSize);

    if (trackData == nullptr || dataSize <= 0)
    {
        DBG("AVISO: base musical '" + trackName + "' nao encontrada.");
        return;
    }

    auto* reader = formatManager.createReaderFor(
        std::make_unique<juce::MemoryInputStream>(trackData, dataSize, false));

    if (reader == nullptr)
    {
        DBG("AVISO: falha ao decodificar a base musical '" + trackName + "'.");
        return;
    }

    backgroundReaderSource.reset(new juce::AudioFormatReaderSource(reader, true));
    backgroundSource.setSource(backgroundReaderSource.get(), 0, nullptr, reader->sampleRate);

    // Conecta a base ao Mixer Principal!
    mixerSource.addInputSource(&backgroundSource, false);
}

//==============================================================================
void PadGridComponent::buildPages()
{
    for (auto& kitPage : kit.pages)
    {
        auto* page = pageViews.add(new Page());
        page->name = kitPage.name;

        for (auto& padSpec : kitPage.pads)
        {
            const juce::String resourceName = padSpec.audioFile.replaceCharacter('.', '_');

            int dataSize = 0;
            const char* audioData = BinaryData::getNamedResource(resourceName.toUTF8(), dataSize);

            if (audioData == nullptr || dataSize <= 0)
            {
                DBG("... FALHA! Recurso " + resourceName + " nao encontrado (pad ignorado).");
                continue;
            }

            auto* newPad = page->pads.add(new PadComponent(padSpec, audioData, dataSize, mixerSource));

            // O grid resolve os choke groups porque precisa enxergar todos os pads
            newPad->onTriggered = [this](PadComponent* triggered) { handlePadTriggered(triggered); };

            addChildComponent(newPad); // fica oculto ate a pagina ser exibida
        }
    }
}

//==============================================================================
void PadGridComponent::buildPageButtons()
{
    // Com uma pagina unica nao ha navegacao: mantem a tela identica aos kits v1
    if (pageViews.size() <= 1)
        return;

    for (int i = 0; i < pageViews.size(); ++i)
    {
        auto* page = pageViews.getUnchecked(i);
        const juce::String label = page->name.isNotEmpty() ? page->name : juce::String(i + 1);

        auto* button = pageButtons.add(new juce::TextButton(label));
        button->onClick = [this, i] { showPage(i); };

        applyInstitutionalStyle(*button);
        addAndMakeVisible(button);
    }
}

//==============================================================================
void PadGridComponent::showPage(int pageIndex)
{
    if (pageViews.isEmpty())
        return;

    currentPage = juce::jlimit(0, pageViews.size() - 1, pageIndex);

    for (int i = 0; i < pageViews.size(); ++i)
    {
        const bool isActive = (i == currentPage);

        for (auto* pad : pageViews.getUnchecked(i)->pads)
        {
            pad->setVisible(isActive);

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
        applyInstitutionalStyle(*pageButtons.getUnchecked(i), i == currentPage);
}

//==============================================================================
void PadGridComponent::handlePadTriggered(PadComponent* triggerer)
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
void PadGridComponent::paint(juce::Graphics& g)
{
    // Fundo com um tom de Bordo (Burgundy) muito escuro e elegante
    g.fillAll(juce::Colour::fromString("ff2a000b"));
}

//==============================================================================
void PadGridComponent::resized()
{
    auto bounds = getLocalBounds();

    auto headerArea = bounds.removeFromTop(60);

    backButton.setBounds(headerArea.removeFromLeft(60).reduced(10));
    // Criamos um retangulo no centro do cabecalho para acomodar os dois botoes
    auto centerHeader = headerArea.withSizeKeepingCentre(220, 30);
    playButton.setBounds(centerHeader.removeFromLeft(100));
    centerHeader.removeFromLeft(20);
    stopButton.setBounds(centerHeader.removeFromLeft(100));

    // Rodape: reservado desde sempre (70 px) e agora usado pela navegacao de paginas
    auto footerArea = bounds.removeFromBottom(70);
    layoutPageButtons(footerArea.reduced(10, 12));

    layoutPage();
}

//==============================================================================
void PadGridComponent::layoutPageButtons(juce::Rectangle<int> area)
{
    const int count = pageButtons.size();

    if (count == 0)
        return;

    const int gap = 6;
    const int buttonWidth = juce::jmax(40, (area.getWidth() - gap * (count - 1)) / count);

    for (int i = 0; i < count; ++i)
    {
        pageButtons.getUnchecked(i)->setBounds(area.removeFromLeft(buttonWidth));

        if (i < count - 1)
            area.removeFromLeft(gap);
    }
}

//==============================================================================
void PadGridComponent::layoutPage()
{
    if (pageViews.isEmpty())
        return;

    auto bounds = getLocalBounds();
    bounds.removeFromTop(60);    // cabecalho
    bounds.removeFromBottom(70); // rodape de paginas

    const int numCols = kit.columns;
    const int numRows = kit.rows;
    const int margin = 4;

    const int totalMarginWidth  = margin * (numCols + 1);
    const int totalMarginHeight = margin * (numRows + 1);

    int padWidth  = (bounds.getWidth()  - totalMarginWidth)  / numCols;
    int padHeight = (bounds.getHeight() - totalMarginHeight) / numRows;

    if (padWidth  < 1) padWidth  = 1;
    if (padHeight < 1) padHeight = 1;

    const int startY = bounds.getY();
    auto& pads = pageViews.getUnchecked(currentPage)->pads;

    for (int i = 0; i < pads.size(); ++i)
    {
        const int col = i % numCols;
        const int row = i / numCols;

        if (row >= numRows)
            break;

        const int x = margin + (col * (padWidth + margin));
        const int y = startY + margin + (row * (padHeight + margin));

        pads[i]->setBounds(x, y, padWidth, padHeight);
    }
}
