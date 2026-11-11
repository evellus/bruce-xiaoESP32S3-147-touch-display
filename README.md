# Bruce firmware for Seeed XIAO 1.47" IPS Touch Display (ESP32-S3)

Board-support files to run the [Bruce](https://github.com/pr3y/Bruce) firmware on
the **Seeed Studio XIAO 1.47" IPS Touch Display (ESP32-S3 Plus)** — a board Bruce
does not support out of the box.

Brings up the display, capacitive touch, the two user buttons and the backlight
on a board whose LCD controller (JD9853A) and touch controller (AXS5106L) are
both non-standard for Bruce.

## Hardware

| | |
|---|---|
| MCU | ESP32-S3R8 (8 MB PSRAM, 16 MB flash) |
| Display | 1.47" IPS, 172 x 320, driver **JD9853A** (driven as ST7789) |
| Touch | **AXS5106L** capacitive, I2C @ 0x63 |
| Buttons | USR1 (GPIO11), USR2 (GPIO42) |
| IMU | LSM6DS3 (I2C @ 0x6A) |
| Backlight | GPIO12 |

## What works

- Display (orientation, colors, brightness)
- Capacitive touch (calibrated to the panel)
- Buttons: USR1 = select, USR2 = next / hold = back
- Battery reading, brightness, WebUI and the usual Bruce features

## Pin map (GPIO)

| Function | GPIO | | Function | GPIO |
|---|---|---|---|---|
| TFT_CS | 3 | | SD_CS | 43 |
| TFT_DC | 4 | | Touch SDA | 5 |
| TFT_SCLK | 7 | | Touch SCL | 6 |
| TFT_MOSI | 9 | | Touch INT | 44 |
| TFT_RST | 13 (shared) | | Button USR1 | 11 |
| TFT_BL | 12 | | Button USR2 | 42 |

## Install into Bruce

1. Clone Bruce and copy these files in:
   ```
   git clone https://github.com/pr3y/Bruce.git
   cd Bruce
   cp -r <this-repo>/boards/xiao-147 boards/
   cp <this-repo>/boards/_boards_json/xiao-147.json boards/_boards_json/
   ```
2. Apply the four small edits in [`patches/PATCHES.md`](patches/PATCHES.md).
3. Build and flash:
   ```
   pio run -e xiao-147
   esptool --chip esp32s3 --port <PORT> --baud 460800 erase-flash
   esptool --chip esp32s3 --port <PORT> --baud 460800 write-flash 0x0 Bruce-xiao-147.bin
   ```

## Prebuilt firmware

[`firmware/Bruce-xiao-147.bin`](firmware/Bruce-xiao-147.bin) is a merged image.
Hold BOOT + plug USB if it won't connect, then flash it at `0x0`.

## Gotchas (hard-won notes)

- **macOS Intel: cryptography build fails.** Bruce's PlatformIO (pioarduino)
  pulls `esptool 5.3.0` -> `cryptography 50.0.2`, which has no prebuilt wheel for
  Intel macOS. Pin working versions and lock them in the platform's
  `penv_setup.py` `python_deps` (`"cryptography": "==48.0.1"`, `"esptool": "==5.0.2"`).
- **VS Code re-breaks it** by auto-reconfiguring the project. Build from a plain
  terminal, not the IDE button.
- **No spaces in the project path** (e.g. avoid a disk named `Mac 2`).
- **Mirrored text** = the ST7789_Rotation.h MX fix.
- **Dim screen after a minute** = the auto-dimmer (Config -> Dim), not the backlight.

## Credits

- [Bruce](https://github.com/pr3y/Bruce)
- AXS5106L touch protocol adapted from Seeed_GFX2 `Touch_AXS5106L` and the Zephyr
  `input_axs5106l` driver
- Seeed Studio hardware docs

## License

Board files under the same license as Bruce. Vendor libraries keep their own.
