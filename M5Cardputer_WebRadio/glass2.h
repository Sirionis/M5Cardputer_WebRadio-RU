#pragma once
#include <M5UnitGLASS2.h>

#define GLASS2_SDA 2
#define GLASS2_SCL 1

static M5UnitGLASS2 _g2(GLASS2_SDA, GLASS2_SCL);
static bool _g2_ready = false;

inline void glass2Init() {
    _g2_ready = _g2.init();
    if (_g2_ready) { _g2.fillScreen(TFT_BLACK); _g2.setTextColor(TFT_WHITE, TFT_BLACK); }
}

inline void glass2Show(const char* l1, const char* l2 = nullptr) {
    if (!_g2_ready) return;
    _g2.fillScreen(TFT_BLACK); _g2.setTextSize(1); _g2.setTextColor(TFT_WHITE, TFT_BLACK);
    if (l1) { _g2.setCursor(0,  8); _g2.print(l1); }
    if (l2) { _g2.setCursor(0, 40); _g2.print(l2); }
}

// Spectrum visualiser: station name on top 14px, 32 bars filling bottom 48px.
// bars[] values are heights in pixels (0-48). Call at ~20fps from updateFFT().
inline void glass2Spectrum(const char* name, const uint8_t* bars, uint8_t numBars) {
    if (!_g2_ready) return;
    _g2.fillRect(0, 0, 128, 14, TFT_BLACK);
    _g2.setTextSize(1); _g2.setTextColor(TFT_WHITE, TFT_BLACK);
    if (name) { _g2.setCursor(0, 3); _g2.print(name); }
    _g2.fillRect(0, 16, 128, 48, TFT_BLACK);
    uint8_t bw = 128 / numBars;
    for (uint8_t i = 0; i < numBars; i++) {
        uint8_t h = bars[i] > 48 ? 48 : bars[i];
        if (h > 0) {
            _g2.fillRect(i * bw, 64 - h, bw - 1, h, TFT_WHITE);
        }
    }
}
