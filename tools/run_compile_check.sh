#!/usr/bin/env bash
#
# Checagem de compilacao (sintaxe/tipos) de TODAS as fontes do app.
#
# Motivo: os outros dois harnesses (run_kit_parser_test.sh e
# run_launchpad_test.sh) compilam apenas parte do projeto. MainComponent e
# SettingsComponent, por exemplo, dependem de juce_audio_utils e do ProjectInfo
# gerado pelo Projucer, e ficariam sem verificacao nenhuma.
#
# Aqui rodamos `g++ -fsyntax-only` (sem link, rapido) em cada
# PucPads/Source/*.cpp, usando um JuceHeader equivalente ao do Projucer.
#
# Requisitos:
#   - g++ (C++17) e os headers de sistema do JUCE (X11, freetype, fontconfig,
#     harfbuzz, curl, zlib)
#   - JUCE 8 acessivel via JUCE_DIR (raiz do repositorio do JUCE)
#
# Uso:
#   JUCE_DIR=/caminho/para/JUCE ./tools/run_compile_check.sh
#
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
JUCE_DIR=${JUCE_DIR:-}
BUILD_DIR=${BUILD_DIR:-${TMPDIR:-/tmp}/pucpads-compile-check}

if [ -z "$JUCE_DIR" ] || [ ! -d "$JUCE_DIR/modules" ]; then
    echo "ERRO: defina JUCE_DIR apontando para a raiz do JUCE (deve conter 'modules/')." >&2
    exit 1
fi

mkdir -p "$BUILD_DIR"

# --- JuceHeader equivalente ao gerado pelo Projucer (mesmos modulos do .jucer)
cat > "$BUILD_DIR/JuceHeader.h" <<'EOF'
#pragma once
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "BinaryData.h"
#include "ProjectInfo.h"
EOF

cat > "$BUILD_DIR/BinaryData.h" <<'EOF'
#pragma once
namespace BinaryData
{
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);
}
EOF

# ProjectInfo e gerado pelo Projucer no build real; aqui basta o subconjunto
# usado por Main.cpp (a checagem nao linka, entao declarar basta).
cat > "$BUILD_DIR/ProjectInfo.h" <<'EOF'
#pragma once
namespace ProjectInfo
{
    extern const char* const projectName;
    extern const char* const companyName;
    extern const char* const versionString;
    extern const int         versionNumber;
}
EOF

FLAGS=(-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 -DJUCE_USE_MP3AUDIOFORMAT=0 -DJUCE_WEB_BROWSER=0)

echo "Checando compilacao das fontes do app (JUCE: $JUCE_DIR)"
echo

failures=0

for source in "$ROOT_DIR"/PucPads/Source/*.cpp; do
    log="$BUILD_DIR/$(basename "$source").log"
    printf '  %-26s' "$(basename "$source")"

    if g++ -std=c++17 -fsyntax-only "${FLAGS[@]}" \
        -I "$BUILD_DIR" -I "$ROOT_DIR/PucPads/Source" -I "$JUCE_DIR/modules" -I /usr/include/freetype2 \
        "$source" 2> "$log"; then
        echo "OK"
    else
        echo "FALHOU"
        sed -n '1,20p' "$log"
        failures=$((failures + 1))
    fi
done

echo

if [ "$failures" -ne 0 ]; then
    echo "FALHOU: $failures arquivo(s) com erro de compilacao."
    exit 1
fi

echo "OK: todas as fontes compilam (checagem sem link)."
