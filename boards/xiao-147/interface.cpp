#include "core/bus_HAL.h"
#include "core/utils.h"
#include "core/powerSave.h"
#include <Arduino.h>
#include <Wire.h>
#include <globals.h>
#include <interface.h>

// ===== Seeed XIAO 1.47" IPS Touch Display (ESP32-S3 Plus) =====
// Botoes: USR1 (GPIO11) = Selecionar | USR2 (GPIO42) = Proximo / Voltar (segurar)
// Touch: AXS5106L capacitivo via I2C (0x63, SDA=GPIO5, SCL=GPIO6, INT=GPIO44, RST=GPIO13)
// Driver portado do oficial Seeed_GFX2 (Touch_AXS5106L).

// ---------- Touch AXS5106L ----------
#define AXS_I2C_ADDR 0x63
#define AXS_TOUCH_SDA 5
#define AXS_TOUCH_SCL 6
#define AXS_TOUCH_INT 44
#define AXS_TOUCH_RST 13 // compartilhado com o LCD RST
#define AXS_TOUCH_DATA_REG 0x01
#define AXS_REPORT_LEN 14

static bool touchInitialized = false;

// Le o report de 14 bytes a partir do registrador 0x01 (metodo oficial Seeed).
static bool axs5106_read_report(uint8_t *buf, uint8_t len) {
    Wire.beginTransmission(AXS_I2C_ADDR);
    Wire.write((uint8_t)AXS_TOUCH_DATA_REG);
    if (Wire.endTransmission() != 0) return false;

    size_t n = Wire.requestFrom((int)AXS_I2C_ADDR, (int)len);
    if (n < len) return false;
    for (size_t i = 0; i < len; i++) buf[i] = Wire.read();
    return true;
}

// Retorna coordenadas ja mapeadas para a orientacao da tela.
static bool axs5106_read_touch(int16_t &sx, int16_t &sy) {
    uint8_t buf[AXS_REPORT_LEN] = {0};
    if (!axs5106_read_report(buf, AXS_REPORT_LEN)) return false;

    uint8_t touchNum = buf[1];
    if (touchNum == 0) return false;

    uint16_t x = ((buf[2] & 0x0F) << 8) | buf[3];
    uint16_t y = ((buf[4] & 0x0F) << 8) | buf[5];

    // ===== Mapeamento calibrado a partir de toques reais nos 4 cantos =====
    // Tela em paisagem (rot=3): 320 x 172.
    // Medidas cruas dos cantos:
    //   sup-esq  raw(154,298)   sup-dir raw(139,5)
    //   inf-esq  raw(15,307)    inf-dir raw(27,5)
    // => screen_x cresce quando y_cru DIMINUI (dir); screen_y cresce quando x_cru DIMINUI (baixo)
    const int RAW_X_TOP = 150, RAW_X_BOT = 20;    // x cru: topo ~150, base ~20
    const int RAW_Y_LEFT = 305, RAW_Y_RIGHT = 5;  // y cru: esq ~305, dir ~5
    const int SCR_W = 320, SCR_H = 172;

    long ox = (long)(RAW_Y_LEFT - (int)y) * SCR_W / (RAW_Y_LEFT - RAW_Y_RIGHT);
    long oy = (long)(RAW_X_TOP - (int)x) * SCR_H / (RAW_X_TOP - RAW_X_BOT);

    if (ox < 0) ox = 0; if (ox >= SCR_W) ox = SCR_W - 1;
    if (oy < 0) oy = 0; if (oy >= SCR_H) oy = SCR_H - 1;

    sx = ox;
    sy = oy;
    return true;
}

// ---------- GPIO / Botoes ----------
void _setup_gpio() {
    pinMode(SEL_BTN, INPUT_PULLUP);
    pinMode(NXT_BTN, INPUT_PULLUP);

    // --- Reset do touch AXS5106L ---
    // RST e compartilhado com o LCD. Fazemos o reset AQUI, antes de o Bruce
    // inicializar o LCD (que acontece depois e reinicializa a tela de novo).
    pinMode(AXS_TOUCH_INT, INPUT_PULLUP);
    pinMode(AXS_TOUCH_RST, OUTPUT);
    digitalWrite(AXS_TOUCH_RST, LOW);
    delay(200);
    digitalWrite(AXS_TOUCH_RST, HIGH);
    delay(300);

    // I2C do touch (compartilhado com o IMU)
    Wire.begin(AXS_TOUCH_SDA, AXS_TOUCH_SCL);
    Wire.setClock(400000);
    setSysI2CBus(&Wire);
    touchInitialized = true;
}

void _post_setup_gpio() {
    // Backlight - sequencia oficial da Seeed (forceBacklightOn):
    // digitalWrite HIGH primeiro, depois o PWM.
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    analogWrite(TFT_BL, 255);
    tft.invertDisplay(0);
}

// Bateria: divisor 316k/160k no GPIO10
int getBattery() {
    uint32_t mv = analogReadMilliVolts(10) * 476 / 160;
    if (mv < 2500) return 0;
    int pct = map((long)mv, 3300, 4200, 0, 100);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    return pct;
}

void _setBrightness(uint8_t brightval) {
    if (brightval > 100) brightval = 100;
    uint8_t duty = map(brightval, 0, 100, MINBRIGHT, 255);
    if (brightval == 0) duty = 0;
    analogWrite(TFT_BL, duty);
}

static bool selDown() { return digitalRead(SEL_BTN) == BTN_ACT; }
static bool nxtDown() { return digitalRead(NXT_BTN) == BTN_ACT; }

void InputHandler(void) {
    checkPowerSaveTime();
    PrevPress = false;
    NextPress = false;
    SelPress = false;
    AnyKeyPress = false;
    EscPress = false;

    // ---- Touch (com debounce de borda: dispara 1x por toque) ----
    if (touchInitialized) {
        static long t_tm = 0;
        static bool wasPressed = false;      // estado do toque na leitura anterior
        static long lastFireMs = 0;          // ultimo toque aceito
        if (millis() - t_tm > 20) {
            t_tm = millis();
            int16_t tx, ty;
            bool nowPressed = axs5106_read_touch(tx, ty);

            // So aceita na BORDA (dedo encostou agora) e com intervalo minimo.
            if (nowPressed && !wasPressed && (millis() - lastFireMs) > 250) {
                lastFireMs = millis();
                wasPressed = true;
                if (!wakeUpScreen()) AnyKeyPress = true;
                else goto END;
                touchPoint.x = tx;
                touchPoint.y = ty;
                touchPoint.pressed = true;
                touchHeatMap(touchPoint);
                goto END;
            }
            wasPressed = nowPressed;
        }
    }

    // ---- Botoes ----
    {
        bool a = selDown();
        bool b = nxtDown();
        if (a || b) {
            if (!wakeUpScreen()) AnyKeyPress = true;
            else goto END;
        }
        if (a) { SelPress = true; }
        if (b) {
            unsigned long t = millis();
            while (nxtDown() && (millis() - t) < 600);
            if ((millis() - t) >= 600) EscPress = true;
            else NextPress = true;
        }
    }
END:
    if (AnyKeyPress) {
        long tmp = millis();
        while ((millis() - tmp) < 200 && (selDown() || nxtDown()));
    }
}

String keyboard(String mytext, int maxSize, String msg) { return mytext; }

void powerOff() {}

void checkReboot() {}
