#define VIAL_KEYBOARD_UID {0x4F, 0x2D, 0x5A, 0x8A, 0x49, 0x7C, 0xDF, 0x1D}

#define VIAL_UNLOCK_COMBO_ROWS {0, 5}
#define VIAL_UNLOCK_COMBO_COLS {0, 0}

// RGB Matrix custom configuration
#undef RGB_MATRIX_DEFAULT_MODE
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR
#undef RGB_MATRIX_DEFAULT_VAL
#define RGB_MATRIX_DEFAULT_VAL 19  // Brightness 7.5/100 (19/255)
// The board stays lit when the PC sleeps: RGB_MATRIX_SLEEP, in the keyboard's
// own config.h, only fires on a USB suspend, which Windows Modern Standby
// never sends. An idle timeout is what actually turns the LEDs off.
#define RGB_MATRIX_TIMEOUT 60000  // one minute without input

// Unicode input - WinCompose on Windows.
// The compose key is moved off Right Alt so AltGr stays usable on the AZERTY
// host layout. WinCompose must be configured with the same key (Scroll Lock).
#define UNICODE_SELECTED_MODES UNICODE_MODE_WINCOMPOSE
#define UNICODE_KEY_WINC KC_SCROLL_LOCK

// Auto Shift is toggled from Vial's QMK Settings tab and defaults to off after
// an EEPROM clear (quantum/qmk_settings.c:192, hardcoded). Only its timeout
// takes its default from here.
#define AUTO_SHIFT_TIMEOUT 175
