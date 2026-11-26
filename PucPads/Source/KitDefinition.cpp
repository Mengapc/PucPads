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
    // Validacao/normalizacao dos campos opcionais. O parser nunca falha por causa
    // de um campo mal preenchido: avisa no log (DBG) e cai no valor padrao, para
    // que um kit com erro de digitacao continue abrindo em vez de travar a tela.
    //==========================================================================
    bool isHexString (const juce::String& text)
    {
        if (text.isEmpty())
            return false;

        for (int i = 0; i < text.length(); ++i)
            if (juce::CharacterFunctions::getHexDigitValue (text[i]) < 0)
                return false;

        return true;
    }

    // 'color' aceita AARRGGBB (8 digitos) ou RRGGBB (6), com ou sem '#'.
    // O parser do JUCE ignora caracteres nao hexadecimais: um valor de 6 digitos
    // seria lido como alpha 0 (pad invisivel), por isso normalizamos para 8.
    // Cor invalida cai na paleta da funcao musical em vez de virar transparente.
    juce::Colour resolvePadColour (const juce::String& hexColour, const juce::String& function)
    {
        if (hexColour.isEmpty())
            return KitDefinition::colourForFunction (function);

        const auto hex = hexColour.startsWithChar ('#') ? hexColour.substring (1) : hexColour;

        if ((hex.length() != 6 && hex.length() != 8) || ! isHexString (hex))
        {
            DBG ("AVISO: 'color' invalido (\"" + hexColour + "\"); usando a cor da funcao '"
                 + function + "'.");
            return KitDefinition::colourForFunction (function);
        }

        return juce::Colour::fromString (hex.length() == 6 ? "ff" + hex : hex);
    }

    int parseChokeGroup (const juce::var& value, const juce::String& where)
    {
        if (value.isVoid())
            return 0;

        if (! value.isInt())
        {
            DBG ("AVISO: " + where + " com 'chokeGroup' nao inteiro; ignorado (sem grupo).");
            return 0;
        }

        const int group = static_cast<int> (value);

        if (group < 0)
        {
            DBG ("AVISO: " + where + " com 'chokeGroup' negativo; ignorado (sem grupo).");
            return 0;
        }

        return group;
    }

    // playMode: "gate" (padrao) ou "oneshot". Valor desconhecido avisa e cai em gate.
    bool parseOneShot (const juce::String& playMode, const juce::String& where)
    {
        const auto mode = playMode.toLowerCase();

        if (mode.isEmpty() || mode == "gate")
            return false;

        if (mode == "oneshot")
            return true;

        DBG ("AVISO: " + where + " com playMode desconhecido (\"" + playMode + "\"); usando gate.");
        return false;
    }

    float parseGainDb (const juce::var& value, const juce::String& where)
    {
        if (value.isVoid())
            return 0.0f;

        if (! value.isInt() && ! value.isDouble())
        {
            DBG ("AVISO: " + where + " com 'gainDb' nao numerico; usando 0 dB.");
            return 0.0f;
        }

        const auto gain    = static_cast<float> (static_cast<double> (value));
        const auto clamped = juce::jlimit (kKitMinGainDb, kKitMaxGainDb, gain);

        if (clamped != gain)
            DBG ("AVISO: " + where + " com gainDb=" + juce::String (gain)
                 + " fora da faixa; limitado a " + juce::String (clamped) + " dB.");

        return clamped;
    }

    int parseGridSide (int value, int fallback, const char* field)
    {
        if (value <= 0)
        {
            if (value < 0)
                DBG ("AVISO: grid." + juce::String (field) + " negativo; usando o padrao "
                     + juce::String (fallback) + ".");

            return fallback;
        }

        if (value > kKitMaxGridSide)
        {
            DBG ("AVISO: grid." + juce::String (field) + "=" + juce::String (value)
                 + " acima do limite; limitado a " + juce::String (kKitMaxGridSide) + ".");

            return kKitMaxGridSide;
        }

        return value;
    }

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

            // Identificacao do pad nas mensagens de log (o audioFile e unico na pagina).
            const juce::String where = "pagina '" + page.name + "', pad '" + pad.audioFile + "'";

            pad.function = padObj->getProperty ("function").toString().trim().toLowerCase();

            // 'color' (hex) tem prioridade sobre 'function': permite customizacao (RF04).
            pad.colour = resolvePadColour (padObj->getProperty ("color").toString().trim(),
                                           pad.function);

            pad.chokeGroup = parseChokeGroup (padObj->getProperty ("chokeGroup"), where);

            const juce::String playMode = padObj->getProperty ("playMode").toString().trim();
            pad.oneShot = parseOneShot (playMode, where);

            pad.gainDb = parseGainDb (padObj->getProperty ("gainDb"), where);

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

        kit.columns = parseGridSide (columns, kKitDefaultColumns, "columns");
        kit.rows    = parseGridSide (rows,    kKitDefaultRows,    "rows");
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
