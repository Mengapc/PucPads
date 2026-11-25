#include "KitDefinition.h"

//==============================================================================
namespace
{
    // Paleta pedagogica (RF04) -- cada funcao musical tem sua cor neon no Dark Mode.
    constexpr const char* kColourEfeito   = "ff2ecc71"; // Verde
    constexpr const char* kColourLoop     = "ff1e90ff"; // Azul
    constexpr const char* kColourMelodia  = "ffff4fa3"; // Rosa
    constexpr const char* kColourHarmonia = "ffff8c1a"; // Laranja
    constexpr const char* kColourFallback = "ffffd700"; // Dourado institucional

    //==========================================================================
    // Le uma pagina de pads a partir do var "pads" (array de objetos).
    void parsePads (KitPage& page, const juce::var& padsVar)
    {
        auto* padsArray = padsVar.getArray();

        if (padsArray == nullptr)
        {
            DBG("AVISO: pagina '" + page.name + "' sem array 'pads'.");
            return;
        }

        for (auto& padVar : *padsArray)
        {
            auto* padObj = padVar.getDynamicObject();

            if (padObj == nullptr)
            {
                DBG("AVISO: entrada de pad invalida (nao e um objeto) ignorada.");
                continue;
            }

            KitPad pad;
            pad.note      = padObj->getProperty ("note").toString().trim();
            pad.audioFile = padObj->getProperty ("audioFile").toString().trim();

            if (pad.audioFile.isEmpty())
            {
                DBG("AVISO: pad sem 'audioFile' ignorado.");
                continue;
            }

            pad.function = padObj->getProperty ("function").toString().trim().toLowerCase();

            // 'color' (hex ARGB) tem prioridade sobre 'function': permite customizacao (RF04).
            const juce::String hexColour = padObj->getProperty ("color").toString().trim();
            pad.colour = hexColour.isNotEmpty() ? juce::Colour::fromString (hexColour)
                                                : KitDefinition::colourForFunction (pad.function);

            pad.chokeGroup = juce::jmax (0, static_cast<int> (padObj->getProperty ("chokeGroup")));

            const juce::String playMode = padObj->getProperty ("playMode").toString().trim().toLowerCase();
            pad.oneShot = (playMode == "oneshot");

            pad.gainDb = static_cast<float> (static_cast<double> (padObj->getProperty ("gainDb")));

            page.pads.add (pad);
        }
    }
}

//==============================================================================
int KitDefinition::getNumPads() const noexcept
{
    int total = 0;

    for (auto& page : pages)
        total += page.pads.size();

    return total;
}

//==============================================================================
KitDefinition KitDefinition::fromJson (const juce::var& json)
{
    KitDefinition kit;

    auto* root = json.getDynamicObject();

    if (root == nullptr)
    {
        DBG("ERRO: o JSON do kit nao e um objeto.");
        return kit;
    }

    //------------------------------------------------------------------
    // Metadados: o schema v2 usa o objeto "kit"; o v1 traz os campos na raiz.
    //------------------------------------------------------------------
    const juce::var kitVar = root->getProperty ("kit");

    if (auto* kitObj = kitVar.getDynamicObject())
    {
        kit.name         = kitObj->getProperty ("name").toString().trim();
        kit.author       = kitObj->getProperty ("author").toString().trim();
        kit.description  = kitObj->getProperty ("description").toString().trim();
        kit.backingTrack = kitObj->getProperty ("backingTrack").toString().trim();
    }

    if (kit.backingTrack.isEmpty())
        kit.backingTrack = root->getProperty ("backingTrack").toString().trim();

    if (kit.name.isEmpty())
        kit.name = root->getProperty ("name").toString().trim();

    //------------------------------------------------------------------
    // Grade (opcional; default 8x8, igual ao layout historico).
    //------------------------------------------------------------------
    const juce::var gridVar = root->getProperty ("grid");

    if (auto* gridObj = gridVar.getDynamicObject())
    {
        const int columns = static_cast<int> (gridObj->getProperty ("columns"));
        const int rows    = static_cast<int> (gridObj->getProperty ("rows"));

        if (columns > 0) kit.columns = columns;
        if (rows    > 0) kit.rows    = rows;
    }

    //------------------------------------------------------------------
    // Paginas: v2 usa "pages[]"; v1 usa o array "pads" na raiz.
    //------------------------------------------------------------------
    const juce::var pagesVar = root->getProperty ("pages");

    if (auto* pagesArray = pagesVar.getArray())
    {
        for (auto& pageVar : *pagesArray)
        {
            auto* pageObj = pageVar.getDynamicObject();

            if (pageObj == nullptr)
            {
                DBG("AVISO: pagina invalida (nao e um objeto) ignorada.");
                continue;
            }

            KitPage page;
            page.name = pageObj->getProperty ("name").toString().trim();

            if (page.name.isEmpty())
                page.name = "Pagina " + juce::String (kit.pages.size() + 1);

            parsePads (page, pageObj->getProperty ("pads"));

            if (page.pads.isEmpty())
            {
                DBG("AVISO: pagina '" + page.name + "' sem pads foi descartada.");
            }
            else
            {
                kit.pages.add (page);
            }
        }
    }
    else
    {
        // Compatibilidade com o schema v1: um unico banco com os pads da raiz.
        KitPage page;
        page.name = "Pads";
        parsePads (page, root->getProperty ("pads"));

        if (! page.pads.isEmpty())
            kit.pages.add (page);
    }

    if (kit.name.isEmpty())
        kit.name = "Kit sem nome";

    return kit;
}

//==============================================================================
juce::Colour KitDefinition::colourForFunction (const juce::String& functionName)
{
    if (functionName == "efeito")   return juce::Colour::fromString (kColourEfeito);
    if (functionName == "loop")     return juce::Colour::fromString (kColourLoop);
    if (functionName == "melodia")  return juce::Colour::fromString (kColourMelodia);
    if (functionName == "harmonia") return juce::Colour::fromString (kColourHarmonia);

    return juce::Colour::fromString (kColourFallback);
}
