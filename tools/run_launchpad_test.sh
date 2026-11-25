#!/usr/bin/env bash
#
# Compila e executa o teste de fumaca da tela de LaunchPad
# (tools/launchpad_smoke_test.cpp), sem abrir janela nem audio device.
#
# Requisitos:
#   - g++ / gcc (C++17) e as libs de sistema do JUCE no Linux:
#     libx11-dev, libxext-dev, libxinerama-dev, libxcursor-dev, libxrandr-dev,
#     libfreetype-dev, libfontconfig1-dev, libharfbuzz-dev, libcurl4-openssl-dev,
#     zlib1g-dev, libgl1-mesa-dev
#   - JUCE 8 acessivel via JUCE_DIR (raiz do repositorio do JUCE)
#
# Uso:
#   JUCE_DIR=/caminho/para/JUCE ./tools/run_launchpad_test.sh
#
# Variaveis opcionais:
#   BUILD_DIR  diretorio de build (default: $TMPDIR/pucpads-launchpad-test)
#
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
JUCE_DIR=${JUCE_DIR:-}
BUILD_DIR=${BUILD_DIR:-${TMPDIR:-/tmp}/pucpads-launchpad-test}

if [ -z "$JUCE_DIR" ] || [ ! -d "$JUCE_DIR/modules" ]; then
    echo "ERRO: defina JUCE_DIR apontando para a raiz do JUCE (deve conter 'modules/')." >&2
    exit 1
fi

FLAGS=(-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 -DJUCE_USE_MP3AUDIOFORMAT=0 -DJUCE_WEB_BROWSER=0)
LIBS=(-lpthread -ldl -lasound -lX11 -lXext -lXinerama -lXcursor -lXrandr -lGL -lfreetype -lfontconfig -lcurl -lz -lharfbuzz)

mkdir -p "$BUILD_DIR/objs"

# --- JuceHeader minimo: apenas os modulos usados pela tela -------------------
cat > "$BUILD_DIR/JuceHeader.h" <<'EOF'
#pragma once
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"
EOF

# --- BinaryData de teste: le os assets direto do repositorio ----------------
cat > "$BUILD_DIR/BinaryData.h" <<'EOF'
#pragma once
namespace BinaryData
{
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);
}
EOF

cat > "$BUILD_DIR/binarydata_stub.cpp" <<'EOF'
#include <JuceHeader.h>
#include <map>
#include <cstdlib>

namespace BinaryData
{
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes)
    {
        static std::map<juce::String, juce::MemoryBlock> cache;

        const juce::String resourceName (resourceNameUTF8);
        auto it = cache.find (resourceName);

        if (it == cache.end())
        {
            auto assetsDir = juce::File::getCurrentWorkingDirectory()
                                 .getChildFile ("PucPads").getChildFile ("Arquivos_PucPads");

            // O teste aponta PADPADS_ASSETS para uma copia com base musical valida
            if (const char* override = std::getenv ("PADPADS_ASSETS"))
                assetsDir = juce::File (juce::String::fromUTF8 (override));
            juce::MemoryBlock block;

            for (const auto& file : assetsDir.findChildFiles (juce::File::findFiles, false))
            {
                if (file.getFileName().replaceCharacter ('.', '_') == resourceName)
                {
                    file.loadFileAsData (block);
                    break;
                }
            }

            it = cache.emplace (resourceName, block).first;
        }

        dataSizeInBytes = static_cast<int> (it->second.getSize());

        return dataSizeInBytes > 0 ? static_cast<const char*> (it->second.getData()) : nullptr;
    }
}
EOF

# --- Simbolos que o Projucer normalmente gera --------------------------------
cat > "$BUILD_DIR/stubs.cpp" <<'EOF'
#include <JuceHeader.h>

namespace juce
{
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

# --- Fontes dos modulos JUCE (glob: cobre JUCE 8 e modulos divididos) -------
MODULE_SOURCES=()
for module in juce_core juce_events juce_graphics juce_data_structures \
              juce_audio_basics juce_audio_devices juce_audio_formats juce_gui_basics; do
    for source in "$JUCE_DIR/modules/$module/$module"*.cpp; do
        MODULE_SOURCES+=("$source")
    done
done

# --- Compila e roda ----------------------------------------------------------
g++ -std=c++17 -O0 \
    -I "$BUILD_DIR" -I "$ROOT_DIR/PucPads/Source" -I "$JUCE_DIR/modules" -I /usr/include/freetype2 \
    "${MODULE_SOURCES[@]}" \
    "$ROOT_DIR/PucPads/Source/KitDefinition.cpp" \
    "$ROOT_DIR/PucPads/Source/PadComponent.cpp" \
    "$ROOT_DIR/PucPads/Source/PadGridComponent.cpp" \
    "$ROOT_DIR/PucPads/Source/LaunchPadComponent.cpp" \
    "$ROOT_DIR/tools/launchpad_smoke_test.cpp" \
    "$BUILD_DIR/binarydata_stub.cpp" \
    "$BUILD_DIR/stubs.cpp" \
    "$BUILD_DIR"/objs/*.o \
    "${FLAGS[@]}" \
    -o "$BUILD_DIR/launchpad_smoke_test" \
    "${LIBS[@]}"

cd "$ROOT_DIR"
"$BUILD_DIR/launchpad_smoke_test"
