#include <JuceHeader.h>
#include "PadGridComponent.h"
#include "PadComponent.h"

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
        else 
        {
        DBG("ERRO FATAL: Falha ao interpretar o JSON. Verifique se ha virgulas faltando ou chaves incorretas!");
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

                // Lê a string hexadecimal do JSON (ex: "ffff0000")
                juce::String colorString = padData["color"];

                // Converte para um objeto Colour do JUCE
                juce::Colour padColor = juce::Colour::fromString(colorString);

                if (dataSize > 0)
                {
                    DBG("... SUCESSO! Recurso " + resourceName + " encontrado. Criando o pad.");
                    auto* newPad = pads.add(new PadComponent(audioData, dataSize, mixerSource, padColor));
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

    juce::Colour corFundoBotao = juce::Colour::fromString("ff420012"); // Bordô
    juce::Colour corTextoBotao = juce::Colour::fromString("ffffd700"); // Dourado

    // Configura o Play
    playButton.setColour(juce::TextButton::buttonColourId, corFundoBotao);
    playButton.setColour(juce::TextButton::textColourOffId, corTextoBotao);

    // Configura o Stop
    stopButton.setColour(juce::TextButton::buttonColourId, corFundoBotao);
    stopButton.setColour(juce::TextButton::textColourOffId, corTextoBotao);

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
    // Fundo com um tom de Bordô (Burgundy) muito escuro e elegante
    g.fillAll(juce::Colour::fromString("ff2a000b"));
}

void PadGridComponent::resized()
{
    auto bounds = getLocalBounds();

    auto headerArea = bounds.removeFromTop(60);

    // Criamos um retângulo no centro do cabeçalho para acomodar os dois botões
    auto centerHeader = headerArea.withSizeKeepingCentre(220, 30);

    playButton.setBounds(centerHeader.removeFromLeft(100));
    centerHeader.removeFromLeft(20); // Espaço de 20px entre os botões
    stopButton.setBounds(centerHeader.removeFromLeft(100));

    const int numCols = 8;
    const int numRows = 8;
    int margin = 4;

    int totalMarginWidth = margin * (numCols + 1);
    int totalMarginHeight = margin * (numRows + 1);
    
    int padWidth = (bounds.getWidth() - totalMarginWidth) / numCols;
    int padHeight = (bounds.getHeight() - totalMarginHeight) / numRows;
    
    if (padWidth < 1) padWidth = 1;
    if (padHeight < 1) padHeight = 1;

    int startY = bounds.getY();

    for (int i = 0; i < pads.size(); ++i)
    {
        int col = i % numCols;
        int row = i / numCols;

        if (row >= numRows)
            break;

        int x = margin + (col * (padWidth + margin));
        int y = startY + margin + (row * (padHeight + margin));

        pads[i]->setBounds(x, y, padWidth, padHeight);
    }
}