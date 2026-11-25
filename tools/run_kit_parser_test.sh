#!/usr/bin/env bash
#
# Compila e executa o teste de fumaca do parser de kits (tools/kit_parser_smoke_test.cpp).
#
# Requisitos:
#   - g++ / gcc (C++17) e as libs de sistema do JUCE no Linux:
#     libx11-dev, libfreetype-dev, libfontconfig1-dev, libharfbuzz-dev, libcurl4-openssl-dev, zlib
#   - JUCE 8 acessivel via JUCE_DIR (raiz do repositorio do JUCE)
#
# Uso:
#   JUCE_DIR=/caminho/para/JUCE ./tools/run_kit_parser_test.sh
#
# Variaveis opcionais:
#   BUILD_DIR  diretorio de build (default: $TMPDIR/pucpads-kit-test)
#
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
JUCE_DIR=${JUCE_DIR:-}
BUILD_DIR=${BUILD_DIR:-${TMPDIR:-/tmp}/pucpads-kit-test}

if [ -z "$JUCE_DIR" ] || [ ! -d "$JUCE_DIR/modules" ]; then
    echo "ERRO: defina JUCE_DIR apontando para a raiz do JUCE (deve conter 'modules/')." >&2
    exit 1
fi

FLAGS=(-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 -DJUCE_USE_MP3AUDIOFORMAT=0 -DJUCE_WEB_BROWSER=0)
LIBS=(-lpthread -ldl -lX11 -lfreetype -lfontconfig -lcurl -lz -lharfbuzz)

mkdir -p "$BUILD_DIR/objs"

# --- JuceHeader minimo: o teste so precisa de core/events/graphics ------------
cat > "$BUILD_DIR/JuceHeader.h" <<'EOF'
#pragma once
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
EOF

# --- Simbolos que o Projucer normalmente gera --------------------------------
cat > "$BUILD_DIR/stubs.cpp" <<'EOF'
#include <JuceHeader.h>

namespace juce
{
    extern const char* const juce_compilationDate;
    extern const char* const juce_compilationTime;
    const char* const juce_compilationDate = __DATE__;
    const char* const juce_compilationTime = __TIME__;
}
EOF

# --- Fontes C embutidas no juce_graphics (sheenbidi, png, jpg, webp, svg) ----
for source in "$JUCE_DIR"/modules/juce_graphics/*.c; do
    object="$BUILD_DIR/objs/$(basename "$source" .c).o"
    if [ ! -f "$object" ] || [ "$source" -nt "$object" ]; then
        gcc -O1 "${FLAGS[@]}" -c "$source" \
            -I "$JUCE_DIR/modules" \
            -I "$JUCE_DIR/modules/juce_graphics/unicode/sheenbidi/Headers" \
            -I "$JUCE_DIR/modules/juce_graphics/drawables/lunasvg" \
            -o "$object"
    fi
done

# --- Compila e roda ----------------------------------------------------------
g++ -std=c++17 -O0 \
    -I "$BUILD_DIR" -I "$ROOT_DIR/PucPads/Source" -I "$JUCE_DIR/modules" -I /usr/include/freetype2 \
    "$JUCE_DIR/modules/juce_core/juce_core.cpp" \
    "$JUCE_DIR/modules/juce_events/juce_events.cpp" \
    "$JUCE_DIR/modules/juce_graphics/juce_graphics.cpp" \
    "$ROOT_DIR/PucPads/Source/KitDefinition.cpp" \
    "$ROOT_DIR/tools/kit_parser_smoke_test.cpp" \
    "$BUILD_DIR/stubs.cpp" \
    "$BUILD_DIR"/objs/*.o \
    "${FLAGS[@]}" \
    -o "$BUILD_DIR/kit_parser_smoke_test" \
    "${LIBS[@]}"

cd "$ROOT_DIR"
"$BUILD_DIR/kit_parser_smoke_test"
