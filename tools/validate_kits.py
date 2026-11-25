#!/usr/bin/env python3
"""Validador dos arquivos de kit (JSON) do PucPads.

Motivo: o parser de kits vive em C++ (PucPads/Source/KitDefinition.cpp), mas o build do
projeto depende do Projucer + IDE e nao roda no sandbox. Este script valida o DADO
(schema, referencias de audio, faixas aceitas) e simula as regras de resolucao de cor
do parser, permitindo detectar erros antes de regenerar o BinaryData.

Uso:
    python3 tools/validate_kits.py [arquivo.json ...]

Sem argumentos, valida PucPads/Arquivos_PucPads/notes.json e drumkit.json.
Sai com codigo 1 se houver qualquer erro.
"""

from __future__ import annotations

import json
import os
import re
import sys

ASSETS_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                          "PucPads", "Arquivos_PucPads")
DEFAULT_KITS = ["notes.json", "drumkit.json"]

# Paleta pedagogica (RF04) -- deve espelhar KitDefinition::colourForFunction
FUNCTION_COLOURS = {
    "efeito": "ff2ecc71",    # Verde
    "loop": "ff1e90ff",      # Azul
    "melodia": "ffff4fa3",   # Rosa
    "harmonia": "ffff8c1a",  # Laranja
}
FALLBACK_COLOUR = "ffffd700"  # Dourado institucional
PLAY_MODES = {"gate", "oneshot"}
HEX_ARGB = re.compile(r"^#?(?:[0-9a-fA-F]{8}|[0-9a-fA-F]{6})$")
DEFAULT_COLUMNS = 8
DEFAULT_ROWS = 8


def resolve_colour(pad: dict) -> str:
    """Espelha KitDefinition::fromJson: 'color' tem prioridade sobre 'function'."""
    explicit = str(pad.get("color", "")).strip()
    if explicit:
        return explicit.lstrip("#").lower()
    return FUNCTION_COLOURS.get(str(pad.get("function", "")).strip().lower(), FALLBACK_COLOUR)


def resource_name(audio_file: str) -> str:
    """Espelha o mangling do Projucer: '.' -> '_'."""
    return audio_file.replace(".", "_")


def validate(path: str) -> list[str]:
    errors: list[str] = []
    name = os.path.basename(path)

    try:
        with open(path, encoding="utf-8") as handle:
            data = json.load(handle)
    except (OSError, json.JSONDecodeError) as exc:
        return [f"{name}: nao foi possivel ler/parsear: {exc}"]

    if not isinstance(data, dict):
        return [f"{name}: a raiz do JSON deve ser um objeto"]

    version = data.get("schemaVersion", 1)
    if version not in (1, 2):
        errors.append(f"{name}: schemaVersion nao suportado ({version}); use 1 ou 2")

    # ---- Metadados ----
    kit_obj = data.get("kit") if isinstance(data.get("kit"), dict) else {}
    kit_name = kit_obj.get("name") or data.get("name") or ""
    if version == 2 and not kit_name:
        errors.append(f"{name}: schema v2 exige kit.name")

    backing = kit_obj.get("backingTrack") or data.get("backingTrack") or ""
    if backing:
        if not os.path.isfile(os.path.join(ASSETS_DIR, backing)):
            errors.append(f"{name}: backingTrack '{backing}' nao existe em Arquivos_PucPads/")
    else:
        print(f"  aviso: {name} sem backingTrack (play/stop ficarao sem audio)")

    # ---- Grade ----
    grid = data.get("grid") if isinstance(data.get("grid"), dict) else {}
    columns = int(grid.get("columns", DEFAULT_COLUMNS) or DEFAULT_COLUMNS)
    rows = int(grid.get("rows", DEFAULT_ROWS) or DEFAULT_ROWS)
    if columns <= 0 or rows <= 0:
        errors.append(f"{name}: grid.columns/rows devem ser > 0 (recebido {columns}x{rows})")

    # ---- Paginas (v2) ou pads na raiz (v1) ----
    if "pages" in data:
        pages = data["pages"]
        if not isinstance(pages, list) or not pages:
            return errors + [f"{name}: 'pages' deve ser uma lista nao vazia"]
    else:
        pads = data.get("pads")
        if not isinstance(pads, list):
            return errors + [f"{name}: sem 'pages' (v2) e sem 'pads' (v1)"]
        pages = [{"name": "Pads", "pads": pads}]

    seen_page_names: set[str] = set()
    total_pads = 0
    used_functions: dict[str, int] = {}
    used_groups: dict[int, int] = {}
    resolved_colours: set[str] = set()

    for page_index, page in enumerate(pages, start=1):
        if not isinstance(page, dict):
            errors.append(f"{name}: pagina {page_index} nao e um objeto")
            continue

        page_name = str(page.get("name", "")).strip()
        label = page_name or f"pagina {page_index}"

        if page_name:
            if page_name in seen_page_names:
                errors.append(f"{name}: nome de pagina duplicado '{page_name}'")
            seen_page_names.add(page_name)
        elif version == 2:
            errors.append(f"{name}: pagina {page_index} sem 'name' (obrigatorio no v2)")

        pads = page.get("pads")
        if not isinstance(pads, list):
            errors.append(f"{name}/{label}: 'pads' deve ser uma lista")
            continue

        capacity = columns * rows
        if len(pads) > capacity:
            errors.append(f"{name}/{label}: {len(pads)} pads excedem a grade {columns}x{rows} ({capacity})")
        if not pads:
            errors.append(f"{name}/{label}: pagina vazia")

        for pad_index, pad in enumerate(pads, start=1):
            where = f"{name}/{label}/pad {pad_index}"
            if not isinstance(pad, dict):
                errors.append(f"{where}: nao e um objeto")
                continue

            audio_file = str(pad.get("audioFile", "")).strip()
            if not audio_file:
                errors.append(f"{where}: 'audioFile' ausente")
            elif not os.path.isfile(os.path.join(ASSETS_DIR, audio_file)):
                errors.append(f"{where}: audio '{audio_file}' nao existe em Arquivos_PucPads/")

            function = str(pad.get("function", "")).strip().lower()
            if function:
                if function not in FUNCTION_COLOURS:
                    errors.append(f"{where}: function '{function}' invalida "
                                  f"(esperado: {', '.join(sorted(FUNCTION_COLOURS))})")
                used_functions[function] = used_functions.get(function, 0) + 1
            elif version == 2:
                print(f"  aviso: {where} sem 'function' (cor cai no dourado institucional)")

            colour = str(pad.get("color", "")).strip()
            if colour and not HEX_ARGB.match(colour):
                errors.append(f"{where}: 'color' '{colour}' nao e ARGB hex valido")

            play_mode = str(pad.get("playMode", "gate")).strip().lower()
            if play_mode not in PLAY_MODES:
                errors.append(f"{where}: playMode '{play_mode}' invalido "
                              f"(esperado: {', '.join(sorted(PLAY_MODES))})")

            choke_group = pad.get("chokeGroup", 0)
            if isinstance(choke_group, bool) or not isinstance(choke_group, int):
                errors.append(f"{where}: 'chokeGroup' deve ser inteiro")
            elif choke_group < 0:
                errors.append(f"{where}: 'chokeGroup' deve ser >= 0 (0 = sem grupo)")
            elif choke_group > 0:
                used_groups[choke_group] = used_groups.get(choke_group, 0) + 1

            gain = pad.get("gainDb", 0.0)
            if isinstance(gain, bool) or not isinstance(gain, (int, float)):
                errors.append(f"{where}: 'gainDb' deve ser numerico")
            elif not -60.0 <= float(gain) <= 12.0:
                errors.append(f"{where}: 'gainDb' fora da faixa -60..+12 dB ({gain})")

            resolved_colours.add(resolve_colour(pad))
            total_pads += 1

    # ---- Relatorio ----
    print(f"  kit: {kit_name or '(sem nome)'} | schema v{version} | "
          f"{len(pages)} pagina(s) | {total_pads} pad(s) | grade {columns}x{rows}")
    if used_functions:
        print("  funcoes: " + ", ".join(f"{k}={v}" for k, v in sorted(used_functions.items())))
    if used_groups:
        print("  choke groups: " + ", ".join(f"g{k}={v} pads" for k, v in sorted(used_groups.items())))
    print("  cores resolvidas: " + ", ".join(sorted(resolved_colours)))

    return errors


def main(argv: list[str]) -> int:
    targets = argv[1:] or [os.path.join(ASSETS_DIR, kit) for kit in DEFAULT_KITS]

    print(f"Validando {len(targets)} arquivo(s) de kit...\n")
    all_errors: list[str] = []

    for path in targets:
        print(f"- {os.path.basename(path)}")
        all_errors.extend(validate(path))
        print()

    if all_errors:
        print(f"FALHOU: {len(all_errors)} erro(s)")
        for error in all_errors:
            print(f"  x {error}")
        return 1

    print("OK: todos os kits estao validos.")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
