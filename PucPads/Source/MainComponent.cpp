#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    DBG("--------------------------------------------------");
    DBG("Iniciando a criacao dos pads a partir do JSON...");

    // 1. Acessa os dados do nosso arquivo JSON
    juce::String jsonString (BinaryData::pads_config_json, BinaryData::pads_config_jsonSize);
    
    // Verificando se o JSON foi carregado da memória
    if (jsonString.isEmpty())
    {
        DBG("ERRO CRITICO: Nao foi possivel carregar o arquivo 'pads_config.json' dos dados binarios. Ele foi adicionado corretamente no Projucer?");
    }
    else
    {
        DBG("JSON carregado com sucesso. Conteudo: " + jsonString);
    }
    
    // 2. Faz o "parse" (interpretação) do JSON
    juce::var result = juce::JSON::parse(jsonString);
    
    // 3. Pega o array "pads" de dentro do JSON
    if (result.isObject())
    {
        auto* obj = result.getDynamicObject();
        if (obj->hasProperty("pads"))
        {
            auto* padsArray = obj->getProperty("pads").getArray();
            DBG("Encontrou o array 'pads'. Numero de pads a serem criados: " + juce::String(padsArray->size()));
            
            // 4. FAZ O LOOP PARA CRIAR CADA PAD
            for (auto& padData : *padsArray)
            {
                juce::String audioFileName = padData["audioFile"];
                DBG("Lendo dados para o arquivo: " + audioFileName);
                audioFileName = audioFileName.replaceCharacter('.', '_');
                
                int dataSize = 0;
                const char* audioData = BinaryData::getNamedResource(audioFileName.toUTF8(), dataSize);
                
                if (dataSize > 0)
                {
                    DBG("Recurso binario encontrado para " + audioFileName + ". Criando o pad...");
                    auto* newPad = pads.add(new PadComponent(audioData, dataSize, mixerSource));
                    addAndMakeVisible(newPad);
                }
                else
                {
                    DBG("ERRO: Nao foi possivel encontrar o recurso binario para: " + audioFileName);
                    DBG("Verifique se o nome do arquivo no JSON bate EXATAMENTE (incluindo .wav e letras maiusculas/minusculas) com o nome do arquivo que voce adicionou no Projucer.");
                }
            }
        }
        else
        {
            DBG("ERRO: O JSON e um objeto valido, mas nao contem a propriedade 'pads'.");
        }
    }
    else
    {
        DBG("ERRO: Falha ao interpretar o JSON. O conteudo do arquivo 'pads_config.json' e um JSON valido?");
    }
    
    DBG("Processo de criacao de pads finalizado.");
    DBG("--------------------------------------------------");

    setSize (600, 400);
    setAudioChannels (0, 2);
}

MainComponent::~MainComponent()
{
    // Libera os recurso de áudio ao fechar
    shutdownAudio();
}

//================== Funções de áudio
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // Passagem de som
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Requisição de preenchimento de buffer para o próximo pedaço de som
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // Quando o áudio para, liberamos os recursos do audioplayer
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    int padSize = 100;
    int margin = 10;

    for (int i = 0; i < pads.size(); ++i)
    {
		int x = margin + i * (padSize + margin);
		pads[i]->setBounds(x, margin, padSize, padSize);
    }
}
