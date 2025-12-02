#include <JuceHeader.h>
#include "PadGridComponent.h"
#include "PadComponent.h" // Precisamos incluir o PadComponent aqui!

//==============================================================================
PadGridComponent::PadGridComponent(juce::MixerAudioSource& mixerToUse, juce::String kitFileToLoad)
    : mixerSource(mixerToUse) // Inicializa a referência do mixer
{
    DBG("--------------------------------------------------");
    DBG("Carregando o kit: " + kitFileToLoad);

    // 1. Registra formatos de áudio (para ler o MP3 da base)
    formatManager.registerBasicFormats();

    // 1. Acessa os dados do arquivo JSON ESPECIFICADO
    int dataSize = 0;
    const char* jsonData = BinaryData::getNamedResource(kitFileToLoad.toUTF8(), dataSize);

    if (dataSize == 0 || jsonData == nullptr)
    {
        DBG("ERRO FATAL: O arquivo '" + kitFileToLoad + "' nao foi encontrado!");
        return;
    }

    // 1. Criamos um fluxo direto da memória (sem validação de texto)
    juce::MemoryInputStream inputStream(jsonData, dataSize, false);

    // 2. O JSON::parse lê direto do fluxo. Se tiver caractere estranho, ele lida sozinho.
    juce::var result = juce::JSON::parse(inputStream);

    if (result.isObject())
    {
        auto* obj = result.getDynamicObject();

        if (obj->hasProperty("backingTrack"))
        {
            juce::String trackName = obj->getProperty("backingTrack");
            juce::String resourceName = trackName.replaceCharacter('.', '_');

            int dataSize = 0;
            const char* trackData = BinaryData::getNamedResource(resourceName.toUTF8(), dataSize);

            if (dataSize > 0)
            {
                auto* reader = formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream>(trackData, dataSize, false));
                if (reader != nullptr)
                {
                    backgroundReaderSource.reset(new juce::AudioFormatReaderSource(reader, true));
                    backgroundSource.setSource(backgroundReaderSource.get(), 0, nullptr, reader->sampleRate);

                    // Conecta a base ao Mixer Principal!
                    mixerSource.addInputSource(&backgroundSource, false);
                }
            }
        }

        if (obj->hasProperty("pads"))
        {
            auto* padsArray = obj->getProperty("pads").getArray();

            DBG("Encontrou o array 'pads'. Numero de pads no JSON: " + juce::String(padsArray->size()));

            for (auto& padData : *padsArray)
            {
                juce::String audioFileName = padData["audioFile"];
                juce::String resourceName = audioFileName.replaceCharacter('.', '_');

                DBG("JSON: " + audioFileName + " -> Buscando Recurso: " + resourceName);

                int dataSize = 0;
                const char* audioData = BinaryData::getNamedResource(resourceName.toUTF8(), dataSize);

                if (dataSize > 0)
                {
                    DBG("... SUCESSO! Recurso " + resourceName + " encontrado. Criando o pad.");
                    auto* newPad = pads.add(new PadComponent(audioData, dataSize, mixerSource));
                    addAndMakeVisible(newPad);
                }
                else
                {
                    DBG("... FALHA! Recurso " + resourceName + " nao encontrado.");
                }
            }
        }
    }
    DBG("Processo de criacao finalizado. Total de pads criados: " + juce::String(pads.size()));
    DBG("--------------------------------------------------");

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);

    playButton.onClick = [this] { backgroundSource.setPosition(0); backgroundSource.start(); };
    stopButton.onClick = [this] { backgroundSource.stop(); };
}

PadGridComponent::~PadGridComponent()
{
    mixerSource.removeInputSource(&backgroundSource);
}

void PadGridComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PadGridComponent::resized()
{
    // ----- LÓGICA DA GRADE 4x4 -----
    auto bounds = getLocalBounds();

    auto headerArea = bounds.removeFromTop(50); // Reserva 50px no topo
    playButton.setBounds(headerArea.removeFromLeft(100).reduced(5));
    stopButton.setBounds(headerArea.removeFromLeft(100).reduced(5));

    const int numCols = 4;
    const int numRows = 4;
    int margin = 10;

    int totalMarginWidth = margin * (numCols + 1);
    int padSize = (bounds.getWidth() - totalMarginWidth) / numCols;

    if (padSize < 1) padSize = 1;

    int startY = bounds.getY();

    for (int i = 0; i < pads.size(); ++i)
    {
        int col = i % numCols;
        int row = i / numCols;

        if (row >= numRows)
            break;

        int x = margin + (col * (padSize + margin));
        int y = startY + margin + (row * (padSize + margin));

        pads[i]->setBounds(x, y, padSize, padSize);
    }
}