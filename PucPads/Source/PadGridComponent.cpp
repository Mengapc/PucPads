#include <JuceHeader.h>
#include "PadGridComponent.h"
#include "PadComponent.h" // Precisamos incluir o PadComponent aqui!

//==============================================================================
PadGridComponent::PadGridComponent(juce::MixerAudioSource& mixerToUse)
    : mixerSource(mixerToUse) // Inicializa a referência do mixer
{
    // ----- LÓGICA DE CRIAÇÃO DOS PADS -----
    DBG("--------------------------------------------------");
    DBG("Iniciando a criacao dos pads a partir do JSON...");

    juce::String jsonString(BinaryData::pads_config_json, BinaryData::pads_config_jsonSize);
    juce::var result = juce::JSON::parse(jsonString);

    if (result.isObject())
    {
        auto* obj = result.getDynamicObject();
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
}

PadGridComponent::~PadGridComponent()
{
}

void PadGridComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PadGridComponent::resized()
{
    // ----- LÓGICA DA GRADE 4x4 -----
    auto bounds = getLocalBounds();

    const int numCols = 4;
    const int numRows = 4;
    int margin = 10;

    int totalMarginWidth = margin * (numCols + 1);
    int padSize = (bounds.getWidth() - totalMarginWidth) / numCols;

    if (padSize < 1) padSize = 1;

    for (int i = 0; i < pads.size(); ++i)
    {
        int col = i % numCols;
        int row = i / numCols;

        if (row >= numRows)
            break;

        int x = margin + (col * (padSize + margin));
        int y = margin + (row * (padSize + margin));

        pads[i]->setBounds(x, y, padSize, padSize);
    }
}