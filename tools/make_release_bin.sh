#!/usr/bin/env bash
# Junta bootloader + tabela de particoes + boot_app0 + aplicacao em uma
# unica imagem de 4 MB, gravavel em 0x0 (mesmo layout da release original).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/.pio/build/cardputer"
PIO_HOME="${PLATFORMIO_CORE_DIR:-$HOME/.platformio}"
ESPTOOL="$PIO_HOME/packages/tool-esptoolpy/esptool.py"
# o python do venv do PlatformIO ja tem pyserial, o do sistema pode nao ter
PY="$PIO_HOME/penv/bin/python"
[ -x "$PY" ] || PY=python3
BOOT_APP0="$PIO_HOME/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"
OUT="${1:-$ROOT/M5Cardputer_WebRadio_RU.bin}"

"$PY" "$ESPTOOL" --chip esp32s3 merge_bin \
    -o "$OUT" \
    --flash_mode dio --flash_freq 80m --flash_size 4MB \
    --fill-flash-size 4MB \
    0x0000   "$BUILD/bootloader.bin" \
    0x8000   "$BUILD/partitions.bin" \
    0xe000   "$BOOT_APP0" \
    0x10000  "$BUILD/firmware.bin"

ls -l "$OUT"
