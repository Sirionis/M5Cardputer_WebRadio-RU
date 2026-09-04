
# M5Cardputer_WebRadio — RU edition

Самостоятельная (standalone) прошивка для M5Cardputer: список российских
интернет-станций зашит в бинарник, SD-карта **не нужна**.

## Прошивка

**Через лоадер (M5Launcher и подобные)** — `WebRadio_RU.bin`, 1.28 МБ,
это чистый образ приложения. Кинуть на SD-карту и выбрать в меню лоадера.

**Через esptool напрямую** — образ приложения пишется по адресу `0x10000`
(разделы и бутлоадер уже на устройстве, настройки Wi-Fi в NVS сохраняются):

```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 --baud 1500000 write_flash 0x10000 WebRadio_RU.bin
```

Если нужен полный образ на 4 МБ для записи с `0x0` (bootloader + таблица
разделов + boot_app0 + приложение) — он собирается скриптом
`./tools/make_release_bin.sh`. Учти, что такой образ обнуляет NVS, и Wi-Fi
придётся вводить заново.

## Сборка из исходников

Сборка сделана в PlatformIO (`platformio.ini` в корне репозитория):
espressif32 / arduino-esp32 2.0.17, board `m5stack-stamps3`, разметка `huge_app`
(3 МБ под приложение), ESP32-audioI2S 3.0.12.

```bash
~/.platformio/penv/bin/pio run -e cardputer
./tools/make_release_bin.sh
```

Первая команда собирает `.pio/build/cardputer/firmware.bin` — это и есть
`WebRadio_RU.bin` для лоадера. Вторая склеивает полный образ на 4 МБ.

## Станции

Список зашит в `defaultStations[]` в `M5Cardputer_WebRadio/M5Cardputer_WebRadio.ino`
и продублирован в `M5Cardputer_WebRadio/station_list.txt`. Все потоки — обычный
HTTP (меньше нагрузка на ESP32-S3, чем HTTPS), MP3 128 kbps или AAC+ 96 kbps.

| # | Станция | Поток |
|---|---------|-------|
| 1 | Радио ULTRA | `nashe1.hostingradio.ru/ultra-128.mp3` |
| 2 | НАШЕ Радио | `nashe1.hostingradio.ru/nashe-128.mp3` |
| 3 | НАШЕ Классик Рок | `nashe1.hostingradio.ru/nasheclassic.mp3` |
| 4 | НАШЕ Панки | `nashe1.hostingradio.ru/nashepunks.mp3` |
| 5 | ROCK FM | `nashe1.hostingradio.ru/rock-128.mp3` |
| 6 | Радио ENERGY | `gpm.hostingradio.ru/gpm-energyfm495.aacp` |
| 7 | Авторадио | `gpm.hostingradio.ru/gpm-avtoradio495.aacp` |
| 8 | Европа Плюс | `ep128.hostingradio.ru:8030/ep128` |
| 9 | Радио MAXIMUM | `maximum.hostingradio.ru/maximum96.aacp` |
| 10 | Русское Радио | `rusradio.hostingradio.ru/rusradio96.aacp` |
| 11 | DFM | `dfm.hostingradio.ru/dfm96.aacp` |
| 12 | Радио RECORD | `radiorecord.hostingradio.ru/rr_main96.aacp` |
| 13 | RECORD Русский Микс | `radiorecord.hostingradio.ru/rus96.aacp` |
| 14 | Дорожное радио | `dorognoe.hostingradio.ru:8000/dorognoe` |
| 15 | Радио JAZZ | `nashe1.hostingradio.ru/jazz-128.mp3` |
| 16 | BEST FM | `nashe1.hostingradio.ru/best-128.mp3` |
| 17 | Юмор FM | `gpm.hostingradio.ru/gpm-humorfm495.aacp` |
| 18 | Comedy Radio | `gpm.hostingradio.ru/gpm-comedyradio495.aacp` |
| 19 | Радио МАЯК | `icecast.vgtrk.cdnvideo.ru/mayakfm_mp3_128kbps` |

## Кириллица

Встроенные шрифты M5GFX (`FreeMonoOblique9pt7b`, `Font0` и прочие) содержат
только ASCII, поэтому русский текст раньше выводился мусором. В `ru_fonts.c`
добавлены два компактных u8g2-шрифта Misc-Fixed (public domain, ASCII +
кириллица), которые подключаются через `lgfx::U8g2font`:

* `u8g2_font_9x15_t_cyrillic` — основной экран 240×135, 26 символов в строке;
* `u8g2_font_6x12_t_cyrillic` — Unit GLASS2 OLED 128×64, 21 символ в строке.

Вместе они занимают около 10 КБ флеша — против 300+ КБ у `efontJA`, который
обычно берут ради кириллицы.

ICY-метаданные российских станций приходят в UTF-8 (проверено на nashe1, gpm,
radiorecord, rusradio), и M5GFX декодирует UTF-8 сам. На случай серверов,
которые до сих пор отдают CP1251, в `ru_fonts.h` есть `toUtf8()`: строка
проверяется на валидность UTF-8 и только при ошибке конвертируется из CP1251.
Обрезка длинных строк идёт через `utf8Truncate()` — по границе символа, иначе
`strncpy()` разрубает двухбайтный символ пополам.

Название трека теперь действительно едет бегущей строкой: `audio_showstreamtitle()`
вызывается библиотекой только в момент смены метаданных, поэтому прокрутка
крутится из `loop()`.

Если положить `/station_list.txt` в корень SD-карты, он полностью заменит
встроенный список (до 24 строк вида `Название,URL`). Без карты прошивка просто
работает на встроенном списке — загрузка больше не тормозит на 4 секунды с
сообщением об ошибке SD.

## Интерфейс

На русский переведены экраны настройки Wi-Fi в `CardWifiSetup.h` (поиск сетей,
выбор сети, ввод пароля, сообщения о подключении и очистке памяти) и служебные
надписи плеера. Названия станций тоже кириллицей — там, где это не ломает бренд
(`ROCK FM`, `DFM`, `BEST FM`, `Comedy Radio` оставлены как есть).

## Клавиши

| Клавиша | Действие |
|---------|----------|
| `/` `,` | следующая / предыдущая станция |
| `;` `.` | громкость + / − |
| `m` | mute |
| `s` | пауза / продолжить |
| `r` | переподключиться к текущему потоку |
| `f` | спектр (FFT) вкл/выкл |
| `o` | перезапуск воспроизведения |
| `p` | MP3 с SD-карты (`/mp3/...`) |

---

# M5Cardputer_WebRadio

M5Cardputer_WebRadio baseado nas bibliotecas: 

M5Unified : https://github.com/m5stack/M5Unified 

A lista de rádios fica em um arquivo texto(station_list.txt) na raiz do cartão SD
Ex:
Nome da Radio01,http://Link da Radio/stream01
Nome da Radio02,http://Link da Radio/stream02
- Tecla R reseta a conexão com o servidor se a radio travar ou não iniciar
- Tecla M liga e desliga o mudo
- Tecla F liga e desliga o FFT
- Capacidade de executar rádios AAC ou MP3
- Salva configurações de WIFI na memória

- Arquivo station_list.txt de exemplo
https://github.com/cyberwisk/M5Cardputer_WebRadio/blob/main/M5Cardputer_WebRadio/station_list.txt

Baseado nas bibliotecas:
M5Unified : https://github.com/m5stack/M5Unified
ESP32-audioI2S Versão 3.0.13 : https://github.com/schreibfaul1/ESP32-audioI2S

Aurelio

<img width="3768" height="2169" alt="IMG_20260508_171146" src="https://github.com/user-attachments/assets/f7830b1f-385d-4ce9-9125-a81d26a4a661" />


----
Wifi:

Com ajuda da biblioteca [Preferences.h](https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences) agora é possivel salvar as configurações de Wifi na 

EEPROM do StamoS3

Assim que ligar o dispositivo, ele vai pedir as configurações de SSID e Senha do WIFI e salvar 

![image](https://github.com/cyberwisk/M5Cardputer_WebRadio/assets/3136312/531dfc77-a9b6-4a27-82ec-f0d6eeed2621)

---------------
* Configurações necessárias do StampS3 na IDE do arduino:

<img width="858" height="912" alt="image" src="https://github.com/user-attachments/assets/e14d7af5-be04-4ffd-b0f8-c96559b7589f" />
