# M5Cardputer WebRadio — RU edition

[![build](https://github.com/Sirionis/M5Cardputer_WebRadio-RU/actions/workflows/build.yml/badge.svg)](https://github.com/Sirionis/M5Cardputer_WebRadio-RU/actions/workflows/build.yml)

Интернет-радио для [M5Cardputer](https://docs.m5stack.com/en/core/Cardputer):
19 российских станций зашиты в прошивку, SD-карта не нужна, интерфейс и
названия станций — на русском.

Форк [cyberwisk/M5Cardputer_WebRadio](https://github.com/cyberwisk/M5Cardputer_WebRadio)
(автор оригинала — Aurélio Avanzi).

<img width="3768" height="2169" alt="M5Cardputer WebRadio" src="https://github.com/user-attachments/assets/f7830b1f-385d-4ce9-9125-a81d26a4a661" />

## Что изменено по сравнению с оригиналом

* Список станций — российский, вместо бразильского.
* Прошивка автономна: станции лежат в самом бинарнике, SD-карта опциональна.
* Кириллица на экране — оригинал умел только ASCII.
* Интерфейс переведён на русский.
* Сборка в PlatformIO, воспроизводимая одной командой.
* Починены четыре бага, см. [«Исправленные баги»](#исправленные-баги).

## Прошивка

Готовые образы лежат на странице
[Releases](https://github.com/Sirionis/M5Cardputer_WebRadio-RU/releases) — их
собирает GitHub Actions из исходников этого репозитория, рядом с файлами лежат
`SHA256SUMS` и ссылка на конкретный прогон сборки. В git прошивки не хранятся.

| Файл | Куда писать |
|------|-------------|
| `WebRadio_RU.bin` | образ приложения: лоадер или `esptool` по адресу `0x10000` |
| `WebRadio_RU_full.bin` | полный образ 4 МБ: `esptool` по адресу `0x0` |

### Через лоадер (M5Launcher и подобные)

Скопировать `WebRadio_RU.bin` на SD-карту и выбрать в меню лоадера.

### Через esptool

Образ приложения пишется по адресу `0x10000`. Бутлоадер и таблица разделов уже
на устройстве, настройки Wi-Fi в NVS сохраняются:

```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 --baud 1500000 write_flash 0x10000 WebRadio_RU.bin
```

Полный образ `WebRadio_RU_full.bin` (bootloader + таблица разделов + boot_app0 +
приложение) пишется с `0x0` и нужен, если устройство прошивается с нуля. Он
обнуляет NVS — Wi-Fi придётся ввести заново:

```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 --baud 1500000 write_flash 0x0 WebRadio_RU_full.bin
```

## Первый запуск

При первом включении устройство просканирует эфир и покажет список сетей.
`;` и `.` двигают выбор, `ENTER` подтверждает, дальше вводится пароль.
Учётные данные сохраняются в NVS через
[Preferences](https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences),
так что дальше подключение идёт автоматически.

Чтобы стереть сохранённую сеть, зажмите BtnA (боковая кнопка) в момент
подключения — память очистится и устройство перезагрузится.

![Настройка Wi-Fi](https://github.com/cyberwisk/M5Cardputer_WebRadio/assets/3136312/531dfc77-a9b6-4a27-82ec-f0d6eeed2621)

## Клавиши

| Клавиша | Действие |
|---------|----------|
| `/` | следующая станция |
| `,` | предыдущая станция |
| `;` | громкость + |
| `.` | громкость − |
| `m` | выключить / включить звук |
| `s` | пауза / продолжить |
| `p` | быстрый переход на Радио ULTRA |
| `r` | переподключиться к текущему потоку (если станция зависла) |
| `o` | перезапустить воспроизведение |
| `f` | спектр (FFT) вкл / выкл |
| BtnA | при подключении — стереть сохранённую сеть Wi-Fi |

## Станции

Список зашит в `defaultStations[]` в
[M5Cardputer_WebRadio.ino](M5Cardputer_WebRadio/M5Cardputer_WebRadio.ino) и
продублирован в [station_list.txt](M5Cardputer_WebRadio/station_list.txt).
Все потоки — обычный HTTP: это заметно экономнее по RAM и CPU на ESP32-S3, чем
HTTPS. Формат — MP3 128 kbps либо AAC+ 96 kbps.

| # | Станция | Поток | Формат |
|---|---------|-------|--------|
| 1 | Радио ULTRA | `nashe1.hostingradio.ru/ultra-128.mp3` | MP3 128 |
| 2 | НАШЕ Радио | `nashe1.hostingradio.ru/nashe-128.mp3` | MP3 128 |
| 3 | НАШЕ Классик Рок | `nashe1.hostingradio.ru/nasheclassic.mp3` | MP3 128 |
| 4 | НАШЕ Панки | `nashe1.hostingradio.ru/nashepunks.mp3` | MP3 128 |
| 5 | ROCK FM | `nashe1.hostingradio.ru/rock-128.mp3` | MP3 128 |
| 6 | Радио ENERGY | `gpm.hostingradio.ru/gpm-energyfm495.aacp` | AAC+ |
| 7 | Авторадио | `gpm.hostingradio.ru/gpm-avtoradio495.aacp` | AAC+ |
| 8 | Европа Плюс | `ep128.hostingradio.ru:8030/ep128` | MP3 |
| 9 | Радио MAXIMUM | `maximum.hostingradio.ru/maximum96.aacp` | AAC+ 96 |
| 10 | Русское Радио | `rusradio.hostingradio.ru/rusradio96.aacp` | AAC+ 96 |
| 11 | DFM | `dfm.hostingradio.ru/dfm96.aacp` | AAC+ 96 |
| 12 | Радио RECORD | `radiorecord.hostingradio.ru/rr_main96.aacp` | AAC+ 96 |
| 13 | RECORD Русский Микс | `radiorecord.hostingradio.ru/rus96.aacp` | AAC+ 96 |
| 14 | Дорожное радио | `dorognoe.hostingradio.ru:8000/dorognoe` | MP3 |
| 15 | Радио JAZZ | `nashe1.hostingradio.ru/jazz-128.mp3` | MP3 128 |
| 16 | BEST FM | `nashe1.hostingradio.ru/best-128.mp3` | MP3 128 |
| 17 | Юмор FM | `gpm.hostingradio.ru/gpm-humorfm495.aacp` | AAC+ |
| 18 | Comedy Radio | `gpm.hostingradio.ru/gpm-comedyradio495.aacp` | AAC+ |
| 19 | Радио МАЯК | `icecast.vgtrk.cdnvideo.ru/mayakfm_mp3_128kbps` | MP3 128 |

Названия ROCK FM, DFM, BEST FM и Comedy Radio оставлены латиницей — так они
пишутся сами.

### Свой список станций

Файл `/station_list.txt` в корне SD-карты полностью заменяет встроенный список.
Формат — по одной станции на строку, `Название,URL`, до 24 строк, кодировка
UTF-8:

```
Радио ULTRA,http://nashe1.hostingradio.ru/ultra-128.mp3
Своя станция,http://example.org/stream
```

Название — до 39 байт (в UTF-8 кириллица занимает 2 байта на символ, то есть
примерно 19 русских букв), URL — до 99 символов. В строку экрана влезает
26 символов.

Без карты прошивка просто работает на встроенном списке.

## Кириллица

Встроенные шрифты M5GFX (`FreeMonoOblique9pt7b`, `Font0` и остальные) содержат
только ASCII, поэтому русский текст в оригинале выводился мусором. В
[ru_fonts.c](M5Cardputer_WebRadio/ru_fonts.c) добавлены два компактных
u8g2-шрифта Misc-Fixed (public domain, ASCII + кириллица), подключённые через
`lgfx::U8g2font`:

* `u8g2_font_9x15_t_cyrillic` — основной экран 240×135, 26 символов в строке;
* `u8g2_font_6x12_t_cyrillic` — Unit GLASS2 OLED 128×64, 21 символ в строке.

Вместе они занимают около 10 КБ флеша — против 300+ КБ у `efontJA`, который
обычно берут ради кириллицы.

ICY-метаданные российских станций приходят в UTF-8 (проверено на nashe1, gpm,
radiorecord, rusradio), а UTF-8 M5GFX декодирует сам. На случай серверов,
которые до сих пор отдают CP1251, в
[ru_fonts.h](M5Cardputer_WebRadio/ru_fonts.h) есть `toUtf8()`: строка сначала
проверяется на валидность UTF-8 и конвертируется только при ошибке. Длинные
строки режет `utf8Truncate()` — по границе символа, иначе `strncpy()` разрубает
двухбайтный символ пополам и на экране появляется мусорный глиф.

## Сборка из исходников

Проект собирается в PlatformIO:
espressif32 / arduino-esp32 2.0.17, плата `m5stack-stamps3`, разметка
`huge_app` (3 МБ под приложение), [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) 3.0.12.

```bash
pio run -e cardputer
```

Результат — `.pio/build/cardputer/firmware.bin`, это и есть `WebRadio_RU.bin`
для лоадера. Полный образ на 4 МБ собирается отдельно:

```bash
./tools/make_release_bin.sh
```

Прошить подключённое устройство напрямую:

```bash
pio run -e cardputer -t upload
```

Занято: около 1.36 МБ флеша из 3 МБ и 18% RAM.

Версии платформы и библиотек в `platformio.ini` зафиксированы точными номерами,
а не диапазонами: релизный бинарник должен собираться одинаково и сегодня, и
через год.

### Сборка в CI

[`.github/workflows/build.yml`](.github/workflows/build.yml) собирает прошивку
на каждый push и pull request и складывает результат в артефакты прогона.
По тегу `v*` тот же workflow выпускает Release: в него попадают образ
приложения, полный образ на 4 МБ, `bootloader.bin`, `partitions.bin` и
`SHA256SUMS`, а в описании — коммит, ссылка на прогон сборки, версии тулчейна
и занятая память.

Выпустить новую версию:

```bash
git tag -a v1.0.0 -m "v1.0.0"
git push origin v1.0.0
```

### Сборка в Arduino IDE

Скетч остаётся совместим с Arduino IDE. Понадобятся библиотеки M5Cardputer,
M5Unified, M5GFX, ESP32-audioI2S и Adafruit NeoPixel; плата — M5Stack StampS3
со следующими настройками:

<img width="858" height="912" alt="Настройки StampS3 в Arduino IDE" src="https://github.com/user-attachments/assets/e14d7af5-be04-4ffd-b0f8-c96559b7589f" />

## Исправленные баги

* `SD.begin()` вызывался без аргументов, с дефолтными пинами SPI — карта не
  находилась в принципе. Теперь используются реальные пины Cardputer
  (SCK 40 / MISO 39 / MOSI 14 / CS 12).
* При отсутствии карты загрузка вставала на `delay(4000)` с сообщением об
  ошибке. Теперь SD опциональна и не задерживает старт.
* Стартовая станция была жёстко задана индексом 3: если список с SD оказывался
  короче, чтение уходило за границу массива.
* Бегущая строка с названием трека не двигалась: `audio_showstreamtitle()`
  вызывается библиотекой только в момент смены метаданных, а прокрутка была
  написана в расчёте на постоянные вызовы. Прокрутка перенесена в `loop()`.

## Библиотеки

* [M5Unified](https://github.com/m5stack/M5Unified) и [M5GFX](https://github.com/m5stack/M5GFX)
* [M5Cardputer](https://github.com/m5stack/M5Cardputer)
* [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) 3.0.12
* [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
* Шрифты из [u8g2](https://github.com/olikraus/u8g2) (Misc-Fixed, public domain)

## Лицензия и авторство

Оригинальный проект — Aurélio Avanzi (Cyberwisk),
https://github.com/cyberwisk/M5Cardputer_WebRadio.
Поддержка Unit GLASS2 — [Swissola](https://github.com/Swissola).
