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
        buildEmptyTemplate();
        return;
    }

    loadKit (kitResource);
}

PadGridComponent::~PadGridComponent()
{
    // Mesma regra do PadComponent: remover do mixer e o que garante que o audio
    // thread nao toque mais nesta base durante o desmonte.
    mixerSource.removeInputSource (&backgroundSource);
}

//==============================================================================
void PadGridComponent::loadKit (const juce::String& kitResource)
{
    DBG("--------------------------------------------------");
    DBG("Carregando o kit: " + kitResource);

    // 3. Acessa os dados do arquivo JSON do kit (mitigando o mangling do BinaryData)
    int dataSize = 0;
    const char* jsonData = BinaryData::getNamedResource (kitResource.toUTF8(), dataSize);

    if (dataSize == 0 || jsonData == nullptr)
    {
        DBG("ERRO: o recurso '" + kitResource + "' nao foi encontrado! Abrindo grade vazia.");
        buildEmptyTemplate();
        return;
    }

    // 4. Parse do kit (schema v2 e, por compatibilidade, v1)
    juce::MemoryInputStream inputStream (jsonData, dataSize, false);
    kit = KitDefinition::fromJson (juce::JSON::parse (inputStream));

    if (! kit.isValid())
    {
        DBG("ERRO: JSON do kit invalido (sem paginas ou com grade invalida). "
            "Verifique virgulas faltando ou chaves incorretas! Abrindo grade vazia.");

        // Um kit quebrado nao deve deixar a area de performance em branco: a
        // grade fantasma preserva o layout e serve de ponto de partida ao editor.
        buildEmptyTemplate();
        return;
    }

    // 5. Um template = uma grade. Kits antigos com varias paginas continuam
    //    validos no dado, mas a tela (sem seletor de paginas) exibe a primeira.
    if (kit.pages.size() > 1)
        DBG("AVISO: o kit '" + kit.name + "' tem " + juce::String (kit.pages.size())
            + " paginas; a tela usa o conceito de choke group, entao apenas a primeira ('"
            + kit.pages.getReference (0).name + "') sera exibida.");

    DBG("Kit '" + kit.name + "' | grade: " + juce::String (kit.columns) + "x" + juce::String (kit.rows)
        + " | paginas no JSON: " + juce::String (kit.pages.size()));

    // 6. Base musical + pads da grade
    if (kit.backingTrack.isNotEmpty())
        loadBackingTrack (kit.backingTrack);

    buildPads (kit.pages.getReference (0));
    layoutPads();

    DBG("Processo de criacao finalizado. Pads criados: " + juce::String (pads.size()));
    DBG("--------------------------------------------------");
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
    backgroundSource.setGain (backingGain);
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

void PadGridComponent::setBackingGain (float gain)
{
    backingGain = juce::jlimit (0.0f, 1.0f, gain);
    backgroundSource.setGain (backingGain);
}

//==============================================================================
void PadGridComponent::buildPads (const KitPage& kitPage)
{
    // Grade cheia e o limite fisico do kit: pads excedentes (JSON maior que a
    // grade) nao teriam lugar no layout e sao descartados aqui, com aviso.
    const int capacity = juce::jmax (0, kit.columns * kit.rows);

    for (auto& padSpec : kitPage.pads)
    {
        if (pads.size() >= capacity)
        {
            DBG("AVISO: o kit tem mais pads do que a grade " + juce::String (kit.columns) + "x"
                + juce::String (kit.rows) + " comporta; os excedentes foram ignorados.");
            break;
        }

        const juce::String resourceName = padSpec.audioFile.replaceCharacter ('.', '_');

        int dataSize = 0;
        const char* audioData = BinaryData::getNamedResource (resourceName.toUTF8(), dataSize);

        if (audioData == nullptr || dataSize <= 0)
        {
            DBG("... FALHA! Recurso " + resourceName + " nao encontrado (pad ignorado).");
            continue;
        }

        auto* newPad = pads.add (new PadComponent (padSpec, audioData, dataSize, mixerSource));

        // O grid resolve os choke groups porque precisa enxergar todos os pads
        newPad->onTriggered = [this] (PadComponent* triggered) { handlePadTriggered (triggered); };

        addAndMakeVisible (newPad);
    }
}

//==============================================================================
void PadGridComponent::buildEmptyTemplate()
{
    isGhostTemplate = true;

    const int numPads = juce::jmax (0, kit.columns * kit.rows);

    for (int i = 0; i < numPads; ++i)
    {
        // KitPad sem audioFile + bytes nulos => pad fantasma (nao toca, nao entra no mixer)
        KitPad ghostSpec;
        addAndMakeVisible (pads.add (new PadComponent (ghostSpec, nullptr, 0, mixerSource)));
    }

    layoutPads();
}

//==============================================================================
void PadGridComponent::stopAllPads()
{
    for (auto* pad : pads)
        pad->chokeStop();
}

//==============================================================================
void PadGridComponent::handlePadTriggered (PadComponent* triggerer)
{
    const int group = triggerer->getChokeGroup();

    if (group <= 0)
        return;

    // Choke group vale para toda a grade do template: pads do mesmo grupo se cortam
    for (auto* pad : pads)
    {
        if (pad != triggerer && pad->getChokeGroup() == group)
            pad->chokeStop();
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
    layoutPads();
}

//==============================================================================
void PadGridComponent::layoutPads()
{
    if (pads.isEmpty())
        return;

    const auto bounds = getLocalBounds();

    const int numCols = juce::jmax (1, kit.columns);
    const int numRows = juce::jmax (1, kit.rows);
    const int margin = 4;

    const int availableWidth  = bounds.getWidth()  - margin * (numCols + 1);
    const int availableHeight = bounds.getHeight() - margin * (numRows + 1);

    const int padWidth  = availableWidth  / numCols;
    const int padHeight = availableHeight / numRows;

    const int startX = bounds.getX() + margin;
    const int startY = bounds.getY() + margin;

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
