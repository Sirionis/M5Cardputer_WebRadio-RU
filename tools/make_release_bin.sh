#!/usr/bin/env bash
# Склеивает bootloader + таблицу разделов + boot_app0 + приложение в один
# образ на 4 МБ, который пишется с адреса 0x0.
#
# Использование: ./tools/make_release_bin.sh [путь/к/выходному.bin]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/.pio/build/cardputer"
PIO_HOME="${PLATFORMIO_CORE_DIR:-$HOME/.platformio}"
OUT="${1:-$ROOT/WebRadio_RU_full.bin}"

if [ ! -f "$BUILD/firmware.bin" ]; then
    echo "Нет $BUILD/firmware.bin - сначала выполните: pio run -e cardputer" >&2
    exit 1
fi

ESPTOOL="$PIO_HOME/packages/tool-esptoolpy/esptool.py"
[ -f "$ESPTOOL" ] || { echo "Не найден esptool: $ESPTOOL" >&2; exit 1; }

# в venv PlatformIO уже есть pyserial, системный python может его не иметь
PY="$PIO_HOME/penv/bin/python"
[ -x "$PY" ] || PY=python3

BOOT_APP0="$PIO_HOME/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"
[ -f "$BOOT_APP0" ] || { echo "Не найден boot_app0.bin: $BOOT_APP0" >&2; exit 1; }

"$PY" "$ESPTOOL" --chip esp32s3 merge_bin \
    -o "$OUT" \
    --flash_mode dio --flash_freq 80m --flash_size 4MB \
    --fill-flash-size 4MB \
    0x0000   "$BUILD/bootloader.bin" \
    0x8000   "$BUILD/partitions.bin" \
    0xe000   "$BOOT_APP0" \
    0x10000  "$BUILD/firmware.bin"

ls -l "$OUT"
