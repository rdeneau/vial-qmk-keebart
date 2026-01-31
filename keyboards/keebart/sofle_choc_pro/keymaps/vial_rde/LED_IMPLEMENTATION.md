# LED Lighting Implementation Summary

## Implementation Completed

### Files Modified

1. **keyboards/keebart/sofle_choc_pro/keymaps/vial_rde/keymap.c**
   - Added RGB Matrix custom lighting functions
   - Implemented `matrix_to_led()` helper function for LED index mapping
   - Implemented `keyboard_post_init_user()` to initialize RGB Matrix
   - Implemented `rgb_matrix_indicators_advanced_user()` for custom per-key lighting

2. **keyboards/keebart/sofle_choc_pro/keymaps/vial_rde/config.h**
   - Added RGB Matrix default configuration
   - Set default brightness to 75/255 (30%)
   - Set default mode to RGB_MATRIX_SOLID_COLOR

### Features Implemented

#### BASE Layer Colors

- **Edge Columns (First & Last)**: White (#FFFFFF)
  - Left: Columns 0 and 5
  - Right: Columns 0 and 5 (matrix rows 5-8)
  
- **Top Row (except edges)**: Grey (#999999)
  - Row 0, columns 1-4 (F2, F3, F4, F5)
  - Row 5, columns 1-4 (F7, F8, F9, F10)

- **Home Row**: Yellow (#FFEB3B)
  - Left: A, S, D, F (row 2, cols 1-4)
  - Right: J, K, L, ; (row 7, cols 4,3,2,1)

- **Special Keys**:
  - O key (row 6, col 3): Red (#DA4444)
  - NAV/TG(LOWER) thumb (row 4, col 2): Orange (#EE8833)
  - RAlt thumb (row 9, col 1): Blue/Pink (#7777DD)

- **All Other Keys**: OFF (Black #000000)

#### LOWER Layer Overlay (Orange #EE8833)

When LOWER layer is active, these keys turn orange:

**Navigation Cluster (Left Side)**:

- F1
- Home, Up, End, PgUp
- Left, Down, Right, PgDn
- Ins, Del, Bksp, PrtSc

**Numpad (Right Side)**:

- Numbers: 0-9
- Operators: /, *, -, +, ,

### Technical Details

#### LED Mapping

Created a lookup table that maps matrix positions to LED indices based on the RGB matrix layout defined in `keyboard.json`. The keyboard has 60 LEDs total (30 per half).

#### Color Definitions

```c
#define CLR_WHITE   255, 255, 255  // #ffffff
#define CLR_GREY    153, 153, 153  // #999999
#define CLR_YELLOW  255, 235, 59   // #ffeb3b
#define CLR_ORANGE  238, 136, 51   // #ee8833
#define CLR_RED     218, 68, 68    // #da4444
#define CLR_BLUE    119, 119, 221  // #7777dd
#define CLR_OFF     0, 0, 0
```

#### Default Brightness

Set to 75/255 which equals 30% brightness.

### How It Works

1. **Initialization**: On keyboard startup, `keyboard_post_init_user()` sets the RGB Matrix mode to SOLID_COLOR with all LEDs off initially.

2. **BASE Layer**: The `rgb_matrix_indicators_advanced_user()` function first turns all LEDs off, then applies the BASE layer color scheme (edge columns white, top row grey, home row yellow, special keys with their colors).

3. **LOWER Layer**: When LOWER layer is toggled on, the function detects this and overlays orange color on navigation and numpad keys while preserving BASE layer colors for other keys.

4. **Layer Switching**: Colors update automatically when switching between layers thanks to the layer detection in the indicator function.

### Testing Instructions

1. **Flash the firmware** to your keyboard
2. **Verify BASE layer**:
   - Edge columns should glow white
   - Top row (F2-F5, F7-F10) should be grey
   - Home row (ASDF/JKL;) should be yellow
   - O key should be red
   - NAV thumb should be orange
   - RAlt thumb should be blue/pink
   - All other keys should be off

3. **Toggle LOWER layer** (press NAV thumb key):
   - Navigation keys should turn orange
   - Numpad keys should turn orange
   - Other keys should maintain their BASE layer colors
   - Toggle off and colors should return to BASE layer

4. **Adjust brightness** using RGB_VAI/RGB_VAD keycodes if needed

### Notes

- The implementation uses `rgb_matrix_indicators_advanced_user()` which overrides the standard RGB Matrix animations
- Colors are applied dynamically based on the active layer
- Both keyboard halves will sync automatically (SPLIT_LED_STATE_ENABLE is already enabled in the main config)
- The default mode is set to SOLID_COLOR to ensure custom colors are visible

### Potential Issues & Solutions

**If LEDs don't light up**:

- Check that RGB Matrix is enabled in Vial
- Verify brightness is set above 0
- Ensure the keyboard is properly flashed

**If colors are wrong**:

- The matrix position mapping may need adjustment
- Use the LED index test mode in Vial to identify correct LED positions

**If LOWER layer doesn't change colors**:

- Verify the layer is actually activating (check OLED or use Vial to monitor)
- Ensure TG(LOWER) keycode is correctly mapped

### Files to Flash

After compilation, flash the resulting `.uf2` file to your keyboard by:

1. Enter bootloader mode (double-tap reset button)
2. Copy the `.uf2` file to the RPI-RP2 drive that appears
3. Keyboard will reboot with new firmware
