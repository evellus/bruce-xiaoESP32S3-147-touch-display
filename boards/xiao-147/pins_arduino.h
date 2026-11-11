#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

// ===== Seeed XIAO 1.47" IPS Touch Display (XIAO ESP32-S3 Plus) =====

#define USB_VID 0x2886
#define USB_PID 0x0056

// UART0 (nesta placa 43/44 tambem sao SD_CS e TOUCH_INT)
static const uint8_t TX = 43;
static const uint8_t RX = 44;

// UART2 nos pads inferiores (D11/D12)
static const uint8_t TXD2 = 38;
static const uint8_t RXD2 = 39;

// I2C interno: touch + IMU (tambem exposto nos pads I2C)
static const uint8_t SDA = 5;
static const uint8_t SCL = 6;

// Modified elsewhere
static const uint8_t SS = -1;
static const uint8_t MOSI = -1;
static const uint8_t MISO = -1;
static const uint8_t SCK = -1;

static const uint8_t G0 = 0;
static const uint8_t G1 = 1;
static const uint8_t G2 = 2;
static const uint8_t G3 = 3;
static const uint8_t G4 = 4;
static const uint8_t G5 = 5;
static const uint8_t G6 = 6;
static const uint8_t G7 = 7;
static const uint8_t G8 = 8;
static const uint8_t G9 = 9;
static const uint8_t G10 = 10;
static const uint8_t G11 = 11;
static const uint8_t G12 = 12;
static const uint8_t G13 = 13;
static const uint8_t G38 = 38;
static const uint8_t G39 = 39;
static const uint8_t G40 = 40;
static const uint8_t G41 = 41;
static const uint8_t G42 = 42;
static const uint8_t G43 = 43;
static const uint8_t G44 = 44;

static const uint8_t ADC1 = 1;
static const uint8_t ADC2 = 2;

// Microfone PDM onboard (D0 = clock, D1 = dados)
#define FM_RSTPIN -1
#define PIN_CLK 1
#define I2S_SCLK_PIN 1
#define I2S_DATA_PIN 2
#define PIN_DATA 2

// I2S nos pads inferiores (D11 = SD, D12 = SCK, D13 = WS)
#define BCLK 39
#define WCLK 40
#define DOUT 38

#define BAD_TX GROVE_SDA
#define BAD_RX GROVE_SCL

// Botoes: USR1 (D19 = GPIO11) e USR2 (D15 = GPIO42)
#define HAS_BTN 1
#define BTN_ALIAS "\"Sel\""
#define BTN_PIN 11
#define BTN_ACT LOW
#define SEL_BTN 11
#define NXT_BTN 42

#define IR_TX_PINS {{"Pad SDA", GROVE_SDA}, {"Pad D11", 38}, {"Pad D12", 39}}
#define IR_RX_PINS {{"Pad SCL", GROVE_SCL}, {"Pad D11", 38}, {"Pad D12", 39}}
#define TXLED -1
#define LED_ON HIGH
#define LED_OFF LOW

#define RF_TX_PINS {{"Pad SDA", GROVE_SDA}, {"Pad D11", 38}, {"Pad D12", 39}}
#define RF_RX_PINS {{"Pad SCL", GROVE_SCL}, {"Pad D11", 38}, {"Pad D12", 39}}

// Modulos externos opcionais (provisorio, so importa se ligar um modulo)
#define CC1101_GDO0_PIN 38
#define CC1101_SS_PIN 40
#define CC1101_MOSI_PIN SPI_MOSI_PIN
#define CC1101_SCK_PIN SPI_SCK_PIN
#define CC1101_MISO_PIN SPI_MISO_PIN

#define NRF24_CE_PIN 39
#define NRF24_SS_PIN 41
#define NRF24_MOSI_PIN SPI_MOSI_PIN
#define NRF24_SCK_PIN SPI_SCK_PIN
#define NRF24_MISO_PIN SPI_MISO_PIN


// ===== Tela 1.47" 172x320 (chip real: JD9853A; teste A usando ST7789) =====
#define HAS_SCREEN 1
#define ROTATION 3
#define MINBRIGHT 160

#define USER_SETUP_LOADED 1
#define USE_HSPI_PORT 1
#define ST7789_DRIVER 1
#define TFT_INVERSION_ON 1
#define CGRAM_OFFSET 1
#define TFT_RGB_ORDER 0
#define TFT_WIDTH 172
#define TFT_HEIGHT 320
#define TFT_BACKLIGHT_ON 1
#define TFT_BL 12
#define TFT_RST 13
#define TFT_DC 4
#define TFT_MOSI 9
#define TFT_SCLK 7
#define TFT_CS 3
#define TOUCH_CS -1
// Touch capacitivo AXS5106L via I2C (driver proprio no interface.cpp)
#define HAS_CAPACITIVE_TOUCH 1
#define HAS_TOUCH 1
#define SMOOTH_FONT 1
#define SPI_FREQUENCY 20000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000

// Cartao SD (compartilha SCK/MOSI com a tela)
#define SDCARD_CS 43
#define SDCARD_SCK 7
#define SDCARD_MISO 8
#define SDCARD_MOSI 9

// Pads I2C usados como "Grove"
#define GROVE_SDA 5
#define GROVE_SCL 6

#define SPI_SCK_PIN 7
#define SPI_MOSI_PIN 9
#define SPI_MISO_PIN 8
#define SPI_SS_PIN 43

#define DEVICE_NAME "XIAO 1.47"

#endif /* Pins_Arduino_h */
