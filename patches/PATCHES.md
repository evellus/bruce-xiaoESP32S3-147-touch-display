# Patches to Bruce source files

Besides the board files, only these minimal edits to existing Bruce files are
needed. Apply them after copying the board files into your Bruce checkout.

## 1. `boards/pinouts/pins_arduino.h`  (register the board)
Add, just before the final `#endif`:
```c
#elif XIAO_147
#include "../xiao-147/pins_arduino.h"
```

## 2. `lib/TFT_eSPI/TFT_Drivers/ST7789_Rotation.h`  (fix mirrored display)
The JD9853A panel needs MADCTL MX in every rotation. Add `TFT_MAD_MX |` to the
four `writedata(...)` lines:
```c
case 0: writedata(TFT_MAD_MX | TFT_MAD_COLOR_ORDER);
case 1: writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_COLOR_ORDER);
case 2: writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_COLOR_ORDER);
case 3: writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_COLOR_ORDER);
```

## 3. `src/core/config.h`  (correct colors on this panel)
```c
int colorInverted = 0;   // was 1
```

## 4. `src/core/configPins.h`  (default orientation for this panel)
```c
int rotation = 0;   // was: ROTATION > 1 ? 3 : 1
```

That's all — four small edits. Everything else lives in the board files.
