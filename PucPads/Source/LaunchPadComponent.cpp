#include <JuceHeader.h>
#include "LaunchPadComponent.h"
#include "UiTheme.h"

//==============================================================================
namespace
{
    // Recursos de cada um dos 8 templates, na ordem do rodape.
    // Somente o template 1 tem kit por enquanto: os demais ficam prontos para
    // receber JSONs (ou o resultado do editor de pads). Quando essa lista passar
    // a vir de um arquivo, este array e o unico ponto a mudar.
    constexpr const char* kTemplateKitResources[8] =
    {
        "notes_json",
        "", "", "", "", "", "", ""
    };
}

//==============================================================================
LaunchPadComponent::LaunchPadComponent (juce::MixerAudioSource& mixerToUse)
    : mixerSource (mixerToUse)
{
    // 1. Slots do rodape
    templates.resize (numTemplates);

    for (int i = 0; i < numTemplates; ++i)
    {
        auto& slot = templates[(size_t) i];
        slot.label       = juce::String (i + 1);
        slot.kitResource = kTemplateKitResources[i];
    }

    // 2. Cromo: voltar + transporte da base
    UiTheme::applyButtonStyle (backButton);
    UiTheme::applyButtonStyle (playButton);
    UiTheme::applyButtonStyle (stopButton);

    addAndMakeVisible (backButton);
    addAndMakeVisible (playButton);
    addAndMakeVisible (stopButton);

    playButton.onClick = [this] { if (auto* view = getActiveView()) view->playBackingTrack(); };
    stopButton.onClick = [this] { if (auto* view = getActiveView()) view->stopBackingTrack(); };

    // 3. Rodape de templates + primeiro template ativo
    buildTemplateButtons();
    showTemplate (0);
}

LaunchPadComponent::~LaunchPadComponent()
{
    // O OwnedArray/unique_ptr cuidam da destruicao; cada view remove suas fontes do mixer
}

//==============================================================================
void LaunchPadComponent::buildTemplateButtons()
{
    for (int i = 0; i < numTemplates; ++i)
    {
        auto* button = templateButtons.add (new juce::TextButton (templates[(size_t) i].label));
        button->onClick = [this, i] { showTemplate (i); };

        UiTheme::applyButtonStyle (*button, i == currentTemplate);
        addAndMakeVisible (button);
    }
}

//==============================================================================
void LaunchPadComponent::showTemplate (int index)
{
    if (! juce::isPositiveAndBelow (index, static_cast<int> (templates.size())))
        return;

    // O template que sai de cena nao pode continuar soando
    if (auto* previous = getActiveView())
    {
        previous->stopAllPads();
        previous->stopBackingTrack();
    }

    currentTemplate = index;
    auto& slot = templates[(size_t) index];

    // Criado sob demanda e reaproveitado: os AudioTransportSource dos pads
    // permanecem carregados enquanto o usuario alterna entre os templates.
    if (slot.view == nullptr)
    {
        slot.view = std::make_unique<PadGridComponent> (mixerSource, slot.kitResource);
        slot.view->setBackingGain (backingGain); // templates criados depois herdam o volume do mixer
        addChildComponent (*slot.view);
    }

    for (int i = 0; i < static_cast<int> (templates.size()); ++i)
        if (auto* view = templates[(size_t) i].view.get())
            view->setVisible (i == index);

    updateTemplateButtons();
    updateTransportButtons();
    resized();
    repaint();
}

//==============================================================================
void LaunchPadComponent::setBackingGain (float gain)
{
    backingGain = juce::jlimit (0.0f, 1.0f, gain);

    for (auto& slot : templates)
        if (slot.view != nullptr)
            slot.view->setBackingGain (backingGain);
}

//==============================================================================
void LaunchPadComponent::updateTemplateButtons()
{
    for (int i = 0; i < templateButtons.size(); ++i)
        UiTheme::applyButtonStyle (*templateButtons.getUnchecked (i), i == currentTemplate);
}

void LaunchPadComponent::updateTransportButtons()
{
    auto* view = getActiveView();
    const bool hasBase = (view != nullptr && view->hasBackingTrack());

    playButton.setEnabled (hasBase);
    stopButton.setEnabled (hasBase);
}

//==============================================================================
PadGridComponent* LaunchPadComponent::getActiveView() const noexcept
{
    if (! juce::isPositiveAndBelow (currentTemplate, static_cast<int> (templates.size())))
        return nullptr;

    return templates[(size_t) currentTemplate].view.get();
}

//==============================================================================
void LaunchPadComponent::paint (juce::Graphics& g)
{
    g.fillAll (UiTheme::bordoEscuro());
}

//==============================================================================
void LaunchPadComponent::resized()
{
    auto bounds = getLocalBounds();

    // Cabecalho: voltar a esquerda; transporte da base centralizado
    auto headerArea = bounds.removeFromTop (headerHeight);
    backButton.setBounds (headerArea.removeFromLeft (60).reduced (10));

    auto centerHeader = headerArea.withSizeKeepingCentre (220, 30);
    playButton.setBounds (centerHeader.removeFromLeft (100));
    centerHeader.removeFromLeft (20);
    stopButton.setBounds (centerHeader.removeFromLeft (100));

    // Rodape: os 8 botoes de template
    auto footerArea = bounds.removeFromBottom (footerHeight);
    layoutTemplateButtons (footerArea.reduced (10, 12));

    if (auto* view = getActiveView())
        view->setBounds (bounds);
}

//==============================================================================
void LaunchPadComponent::layoutTemplateButtons (juce::Rectangle<int> area)
{
    const int count = templateButtons.size();

    if (count == 0)
        return;

    const int buttonWidth = juce::jmax (24, (area.getWidth() - templateGap * (count - 1)) / count);

    for (int i = 0; i < count; ++i)
    {
        templateButtons.getUnchecked (i)->setBounds (area.removeFromLeft (buttonWidth));

        if (i < count - 1)
            area.removeFromLeft (templateGap);
    }
}
