# Sofle Choc Pro LED Lighting Guide (Vial Keymap)

## Overview

The Keebart Sofle Choc Pro keyboard features **RGB Matrix lighting** with 60 individually addressable RGB LEDs (30 per half) powered by WS2812 LEDs. The lighting system provides per-key backlighting with extensive customization options through Vial.

## Hardware Specifications

### LED Configuration

- **Total LEDs**: 60 (30 per keyboard half)
- **LED Type**: WS2812 (addressable RGB)
- **Driver**: WS2812 vendor driver
- **Data Pin**: GP10 (RP2040)
- **Maximum Brightness**: 100 (configurable)
- **Controller**: RP2040 microcontroller

### LED Layout

Each key position has one RGB LED for backlighting. The LEDs are mapped directly to the keyboard matrix, providing uniform per-key illumination across the split keyboard layout (5 rows × 6 columns per half, plus thumb cluster keys).

## Features

### RGB Matrix Effects

The Vial keymap includes **52 different RGB Matrix animations**, organized into several categories:

#### Static Effects

- **Alphas Mods** - Different colors for alphanumeric and modifier keys
- **Gradient Up Down** - Vertical color gradient
- **Gradient Left Right** - Horizontal color gradient

#### Breathing Effects

- **Breathing** - Smooth pulsing effect
- **Hue Breathing** - Color-cycling pulse
- **Hue Pendulum** - Swinging hue effect
- **Hue Wave** - Wave of color changes

#### Cycling Effects

- **Cycle All** - All LEDs cycle through colors
- **Cycle Left Right** - Horizontal color sweep
- **Cycle Up Down** - Vertical color sweep
- **Cycle Out In** - Color cycles from edges to center
- **Cycle Out In Dual** - Dual-direction edge-to-center
- **Cycle Pinwheel** - Rotating pinwheel pattern
- **Cycle Spiral** - Spiral color pattern

#### Band Effects

- **Band Sat** - Saturation band
- **Band Val** - Brightness band
- **Band Pinwheel Sat** - Rotating saturation pinwheel
- **Band Pinwheel Val** - Rotating brightness pinwheel
- **Band Spiral Sat** - Spiral saturation pattern
- **Band Spiral Val** - Spiral brightness pattern

#### Rainbow Effects

- **Rainbow Moving Chevron** - Moving rainbow chevron
- **Rainbow Beacon** - Beacon with rainbow colors
- **Rainbow Pinwheels** - Rotating rainbow pinwheels

#### Special Effects

- **Dual Beacon** - Two beacon points
- **Raindrops** - Random raindrop effects
- **Jellybean Raindrops** - Colorful raindrop effects
- **Pixel Fractal** - Fractal pattern generation
- **Pixel Flow** - Flowing pixel pattern
- **Pixel Rain** - Digital rain effect
- **Digital Rain** - Matrix-style digital rain
- **Typing Heatmap** - Shows recently typed keys

#### Reactive Effects (respond to keypresses)

- **Solid Reactive Simple** - Simple key press reaction
- **Solid Reactive** - Key press with fade
- **Solid Reactive Wide** - Wide fade from keypress
- **Solid Reactive Multiwide** - Multiple wide fades
- **Solid Reactive Cross** - Cross-shaped reaction
- **Solid Reactive Multicross** - Multiple cross reactions
- **Solid Reactive Nexus** - Nexus-style reaction
- **Solid Reactive Multinexus** - Multiple nexus reactions
- **Splash** - Color splash on keypress
- **Multisplash** - Multiple color splashes
- **Solid Splash** - Solid color splash
- **Solid Multisplash** - Multiple solid splashes

## Configuration

### Power Management

- **RGB Matrix Sleep**: LEDs automatically turn off when the computer sleeps
- **USB Suspend Wakeup Delay**: 200ms delay on USB resume
- **Split Synchronization**: LED state synchronized between keyboard halves

### Framebuffer & Keypresses

The following features are enabled:

- `RGB_MATRIX_FRAMEBUFFER_EFFECTS` - Enables effects that use framebuffer (like typing heatmap)
- `RGB_MATRIX_KEYPRESSES` - Enables reactive effects that respond to keypresses

### Vial Integration

- **VialRGB Support**: Full RGB control through Vial configurator
- **No Direct Control**: `VIALRGB_NO_DIRECT` - Prevents direct RGB register access for safety
- **Layer Count**: 16 layers supported
- **Split LED State**: LED state synced across both keyboard halves

## Controlling the LEDs

### Via Vial Configurator

1. Open the [Vial web configurator](https://vial.rocks/) or desktop application
2. Connect your keyboard
3. Navigate to the **Lighting** tab
4. Adjust the following settings:
   - **Effect**: Choose from 52 different animations
   - **Brightness**: 0-100 (max brightness limited to 100 for safety)
   - **Speed**: Animation speed
   - **Hue**: Base color hue (0-255)
   - **Saturation**: Color saturation (0-255)

### Via QMK Keycodes

You can add RGB control keycodes to your keymap for quick access:

| Keycode    | Function              |
| ---------- | --------------------- |
| `RGB_TOG`  | Toggle RGB on/off     |
| `RGB_MOD`  | Next RGB effect       |
| `RGB_RMOD` | Previous RGB effect   |
| `RGB_HUI`  | Increase hue          |
| `RGB_HUD`  | Decrease hue          |
| `RGB_SAI`  | Increase saturation   |
| `RGB_SAD`  | Decrease saturation   |
| `RGB_VAI`  | Increase brightness   |
| `RGB_VAD`  | Decrease brightness   |
| `RGB_SPI`  | Increase effect speed |
| `RGB_SPD`  | Decrease effect speed |

### Default Behavior

- LEDs turn on with the default effect on keyboard power-up
- Settings are saved to EEPROM and persist across reboots
- LEDs automatically disable when the computer goes to sleep

## Power Consumption

RGB LEDs can consume significant power, especially at high brightness levels. The maximum brightness is capped at 100 (out of 255) to:

- Reduce power draw from USB
- Prevent potential stability issues
- Extend LED lifespan
- Reduce heat generation

**Tip**: For battery-powered or mobile use, reduce brightness to 25-50% to conserve power.

## Troubleshooting

### LEDs not working

1. Ensure the keyboard is properly connected via USB
2. Check that RGB is enabled in Vial (not toggled off)
3. Try increasing brightness - it may be set too low
4. Flash the latest firmware

### LEDs on one half not working

1. Check the TRRS cable connection between keyboard halves
2. Ensure `SPLIT_LED_STATE_ENABLE` is defined in config (already enabled)
3. Try reconnecting the TRRS cable
4. Verify the data pin (GP10) connection on the affected half

### Uneven brightness or flickering

1. Reduce the brightness level
2. Check USB cable quality - use a high-quality USB-C cable
3. Try a different USB port with sufficient power
4. Reduce animation speed for demanding effects

### Wrong colors

1. Reset to default settings in Vial
2. Adjust hue and saturation values
3. Try a simpler effect to isolate the issue

## Technical Details

### LED Flags

All LEDs are configured with `flags: 4`, which means they are designated as **modifier keys** in the RGB matrix system. This affects how certain effects render on the keyboard.

### WS2812 Protocol

The WS2812 LEDs use a single-wire protocol with precise timing requirements. The RP2040's vendor driver handles this automatically with hardware support for stable, reliable LED control.

### Split Keyboard Synchronization

The keyboard uses serial communication (USART) via pin GP12 to sync LED state between the left and right halves. This ensures both halves display the same lighting effect in perfect synchronization.

## Customization

For advanced users who want to customize LED behavior beyond Vial's options:

1. Edit `keyboards/keebart/sofle_choc_pro/keymaps/vial/keymap.c`
2. Add custom RGB Matrix effects or keycodes
3. Modify the LED layout in `keyboard.json` if needed
4. Recompile and flash the firmware

Refer to the [QMK RGB Matrix documentation](https://docs.qmk.fm/#/feature_rgb_matrix) for more information on custom effects and programming.

## Resources

- [Vial Documentation](https://get.vial.today/manual/)
- [QMK RGB Matrix Feature](https://docs.qmk.fm/#/feature_rgb_matrix)
- [Keebart Official Website](https://keebart.com/products/sofle)
- [Sofle Keyboard Information](https://github.com/josefadamcik/SofleKeyboard)
