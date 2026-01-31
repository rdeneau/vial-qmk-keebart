// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "transactions.h"

enum layers {
    BASE,  // default layer
    LOWER, // NAV layer
    RAISE  // raise layer
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * QWERTY
 * ColL ->  0      1      2      3      4      5                           5      4      3      2      1      0 <- ColR
 * RowL ,-----------------------------------------.                    ,-----------------------------------------. RowR
 *   0  | Esc  |  F2  |  F3  |  F4  |  F5  |  F6  |                    |  F7  |  F8  |  F9  | F10  | F11  | F12  |  5
 *      |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 *   1  | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  | Del  |  6
 *      |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 *   2  |LShift|   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  | Bspc |  7
 *      |------+------+------+------+------+------|BrwBack|    | Calc  |------+------+------+------+------+------|
 *   3  | Caps |   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  |RShift|  8
 *      `-----------------------------------------/       /     \      \-----------------------------------------'
 *   4             | LGui | LAlt | NAV  | LCtrl| / Enter /       \Space \  |RCtrl | RAlt | Menu | RGui |            9
 *                 |      |      |      |      |/       /         \      \ |      |      |      |      |
 *                 `----------------------------------'           '------''---------------------------'
 */

[BASE] = LAYOUT_split_4x6_5(
  KC_ESC,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,
  KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,  KC_DEL,
  KC_LSFT,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                      KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_BSPC,
  KC_CAPS,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B, KC_WBAK,    KC_CALC, KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_RSFT,
                 KC_LGUI, KC_LALT, TG(LOWER), KC_LCTL, KC_ENT,       KC_SPC, KC_RCTL, KC_RALT, KC_APP, KC_RGUI
),
/* LOWER - Navigation & Numpad
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |  F1  |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      | Home |  Up  | End  |PgUp  |                    |  /   |   7  |   8  |   9  |  *   |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      | Left | Down |Right |PgDn  |-------.    ,-------|  .   |   4  |   5  |   6  |  0   |      |
 * |------+------+------+------+------+------| Home  |    |  End  |------+------+------+------+------+------|
 * |      |      | Ins  | Del  | Bksp |PrtSc |-------|    |-------|  -   |   1  |   2  |   3  |  +   |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      |      |      | /       /       \      \  |      |      |      |      |
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
 */
[LOWER] = LAYOUT_split_4x6_5(
  _______, KC_F1, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
  _______, _______, KC_HOME,   KC_UP,  KC_END, KC_PGUP,                   KC_SLSH,   KC_7,    KC_8,    KC_9, KC_ASTR, _______,
  _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN,                   KC_DOT,    KC_4,    KC_5,    KC_6,    KC_0, _______,
  _______,  _______,  KC_INS,  KC_DEL, KC_BSPC, KC_PSCR, KC_HOME, KC_END, KC_MINS,   KC_1,    KC_2,    KC_3, KC_PLUS, _______,
                     _______, _______, _______, _______, KC_MPLY, KC_MPRV, _______, _______, _______, _______
),
/* RAISE
 * ,----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Esc  | Ins  | Pscr | Menu |      |      |                    |      |      |  Up  |      | DLine| Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  | LAt  | LCtl |LShift|      | Caps |-------.    ,-------|      | Left | Down | Rigth|  Del | Bspc |
 * |------+------+------+------+------+------|        |    |       |------+------+------+------+------+------|
 * |Shift | Undo |  Cut | Copy | Paste|      |-------|    |-------|      |      |      |      |      | Shift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            | LGUI | LAlt | LCTR |LOWER | / Enter /       \Space \  |RAISE | RCTR | RAlt | RGUI |
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
 */
[RAISE] = LAYOUT_split_4x6_5(
  _______, _______ , _______ , _______ , _______ , _______,                           _______,  _______  , _______,  _______ ,  _______ ,_______,
  _______,  KC_INS,  KC_PSCR,   KC_APP,  XXXXXXX, XXXXXXX,                        KC_PGUP, XXXXXXX,   KC_UP, XXXXXXX,C(KC_BSPC), KC_BSPC,
  _______, KC_LALT,  KC_LCTL,  KC_LSFT,  XXXXXXX, KC_CAPS,                       KC_PGDN,  KC_LEFT, KC_DOWN, KC_RGHT,  KC_DEL, KC_BSPC,
  _______, C(KC_Z), C(KC_X), C(KC_C), C(KC_V), XXXXXXX,  _______,       _______,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX, _______,
                         _______, _______, _______, _______, _______,       _______, _______, _______, _______, _______
),
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [BASE]  = { ENCODER_CCW_CW(KC_WH_L, KC_WH_R), ENCODER_CCW_CW(KC_WH_D, KC_WH_U) },
    [LOWER] = { ENCODER_CCW_CW(KC_LEFT, KC_RGHT), ENCODER_CCW_CW(KC_UP,   KC_DOWN) },
    [RAISE] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) }
};
#endif

// RGB Matrix Custom Lighting Configuration
#ifdef RGB_MATRIX_ENABLE

// Color definitions
#define CLR_WHITE   238, 238, 238  // #eeeeee
#define CLR_YELLOW  255, 255, 0    // #ffff00
#define CLR_GREEN   0,   255, 0    // #00ff00
#define CLR_ORANGE  255, 140, 0    // #ff8c00
#define CLR_RED     255, 0, 0      // #ff0000
#define CLR_BLUE    0, 0, 255      // #0000ff
#define CLR_OFF     0, 0, 0

// State variable for O key color toggle (synced between halves)
typedef struct {
    bool o_key_is_green;
} user_state_t;

static user_state_t user_state = {
    .o_key_is_green = false
};

// Split keyboard synchronization callback
void user_state_sync(uint8_t initiator2target_buffer_size, const void* initiator2target_buffer, uint8_t target2initiator_buffer_size, void* target2initiator_buffer) {
    if (initiator2target_buffer_size == sizeof(user_state)) {
        memcpy(&user_state, initiator2target_buffer, sizeof(user_state));
    }
}

// Helper function to find LED index by matrix position
static uint8_t matrix_to_led(uint8_t row, uint8_t col) {
    // LED layout mapping from keyboard.json
    // Format: {LED_index, row, col}
    static const uint8_t led_map[][3] = {
        // Left half (LEDs 0-29)
        {0, 4, 5}, {1, 4, 4}, {2, 4, 3}, {3, 3, 5}, {4, 2, 5}, {5, 1, 5},
        {6, 0, 5}, {7, 0, 4}, {8, 1, 4}, {9, 2, 4}, {10, 3, 4}, {11, 4, 2},
        {12, 4, 1}, {13, 3, 3}, {14, 2, 3}, {15, 1, 3}, {16, 0, 3}, {17, 0, 2},
        {18, 1, 2}, {19, 2, 2}, {20, 3, 2}, {21, 4, 0}, {22, 3, 1}, {23, 2, 1},
        {24, 1, 1}, {25, 0, 1}, {26, 0, 0}, {27, 1, 0}, {28, 2, 0}, {29, 3, 0},
        // Right half (LEDs 30-59)
        {30, 9, 5}, {31, 9, 4}, {32, 9, 3}, {33, 8, 5}, {34, 7, 5}, {35, 6, 5},
        {36, 5, 5}, {37, 5, 4}, {38, 6, 4}, {39, 7, 4}, {40, 8, 4}, {41, 9, 2},
        {42, 9, 1}, {43, 8, 3}, {44, 7, 3}, {45, 6, 3}, {46, 5, 3}, {47, 5, 2},
        {48, 6, 2}, {49, 7, 2}, {50, 8, 2}, {51, 9, 0}, {52, 8, 1}, {53, 7, 1},
        {54, 6, 1}, {55, 5, 1}, {56, 5, 0}, {57, 6, 0}, {58, 7, 0}, {59, 8, 0}
    };

    for (uint8_t i = 0; i < 60; i++) {
        if (led_map[i][1] == row && led_map[i][2] == col) {
            return led_map[i][0];
        }
    }
    return 255; // Not found
}

// Keyboard post-init to set default brightness and register split sync
void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 0, 0); // Start with all LEDs off
    rgb_matrix_set_speed_noeeprom(128);
    rgb_matrix_enable_noeeprom();
    rgb_matrix_set_flags(LED_FLAG_ALL);
    
    // Register split keyboard sync
    transaction_register_rpc(RPC_ID_USER_STATE_SYNC, user_state_sync);
}

// Sync state between keyboard halves
void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        // Sync state from master to slave
        transaction_rpc_send(RPC_ID_USER_STATE_SYNC, sizeof(user_state), &user_state);
    }
}

// Custom LED indicator function
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state);

    // First, set all LEDs to OFF
    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_matrix_set_color(i, CLR_OFF);
    }

    // BASE layer colors (always applied first)
    if (layer == BASE || layer == LOWER) {
        // Edge columns - White (first column left only)
        // Left half first column (col 0) - rows 0-3
        rgb_matrix_set_color(matrix_to_led(0, 0), CLR_WHITE);
        rgb_matrix_set_color(matrix_to_led(1, 0), CLR_WHITE);
        rgb_matrix_set_color(matrix_to_led(2, 0), CLR_WHITE);
        rgb_matrix_set_color(matrix_to_led(3, 0), CLR_WHITE);

        // Right half first column (col 0) (Del, Backspace, Shift)
        rgb_matrix_set_color(matrix_to_led(6, 0), CLR_WHITE);
        rgb_matrix_set_color(matrix_to_led(7, 0), CLR_WHITE);
        rgb_matrix_set_color(matrix_to_led(8, 0), CLR_WHITE);

        // Top row - Yellow (all F-keys: F2-F12)
        // Left half row 0, cols 1-5 (F2, F3, F4, F5, F6)
        rgb_matrix_set_color(matrix_to_led(0, 1), CLR_YELLOW);
        rgb_matrix_set_color(matrix_to_led(0, 2), CLR_YELLOW);
        rgb_matrix_set_color(matrix_to_led(0, 3), CLR_YELLOW);
        rgb_matrix_set_color(matrix_to_led(0, 4), CLR_YELLOW);
        rgb_matrix_set_color(matrix_to_led(0, 5), CLR_YELLOW);

        // Right half row 5, cols 1-5 (F12, F11, F10, F9, F8, F7)
        rgb_matrix_set_color(matrix_to_led(5, 0), CLR_YELLOW);
        rgb_matrix_set_color(matrix_to_led(5, 1), CLR_YELLOW);
        rgb_matrix_set_color(matrix_to_led(5, 2), CLR_YELLOW);
        rgb_matrix_set_color(matrix_to_led(5, 3), CLR_YELLOW);
        rgb_matrix_set_color(matrix_to_led(5, 4), CLR_YELLOW);
        rgb_matrix_set_color(matrix_to_led(5, 5), CLR_YELLOW);

        // Home row - Green (A S D F on left, J K L ; on right)
        rgb_matrix_set_color(matrix_to_led(2, 1), CLR_GREEN); // A
        rgb_matrix_set_color(matrix_to_led(2, 2), CLR_GREEN); // S
        rgb_matrix_set_color(matrix_to_led(2, 3), CLR_GREEN); // D
        rgb_matrix_set_color(matrix_to_led(2, 4), CLR_GREEN); // F

        rgb_matrix_set_color(matrix_to_led(7, 4), CLR_GREEN); // J
        rgb_matrix_set_color(matrix_to_led(7, 3), CLR_GREEN); // K
        rgb_matrix_set_color(matrix_to_led(7, 2), CLR_GREEN); // L
        rgb_matrix_set_color(matrix_to_led(7, 1), CLR_GREEN); // ;

        // Special keys
        // O key (dead key) - toggles between Red and Green
        if (user_state.o_key_is_green) {
            rgb_matrix_set_color(matrix_to_led(6, 2), CLR_GREEN);
        } else {
            rgb_matrix_set_color(matrix_to_led(6, 2), CLR_RED);
        }

        // TG(LOWER)/NAV thumb key - Orange
        rgb_matrix_set_color(matrix_to_led(4, 2), CLR_ORANGE); // NAV

        // Right thumb keys: Space[9,4], RCtrl[9,3], RAlt[9,2], Menu[9,1], RGui[9,0]
        // 3rd thumb key = RAlt at [9, 2] - Blue
        rgb_matrix_set_color(matrix_to_led(9, 2), CLR_BLUE); // RAlt
        // 4th thumb key = Menu at [9, 1] - NOT colored (remains OFF)
    }

    // LOWER layer overlay - Orange for navigation and numpad keys
    if (layer == LOWER) {
        rgb_matrix_set_color(matrix_to_led(0, 1), CLR_ORANGE); // F1
        rgb_matrix_set_color(matrix_to_led(5, 1), CLR_YELLOW); // F11 (remains Yellow, was actually Orange!?)

        // Navigation cluster (left side)
        rgb_matrix_set_color(matrix_to_led(1, 2), CLR_ORANGE); // Home
        rgb_matrix_set_color(matrix_to_led(1, 3), CLR_ORANGE); // Up
        rgb_matrix_set_color(matrix_to_led(1, 4), CLR_ORANGE); // End
        rgb_matrix_set_color(matrix_to_led(1, 5), CLR_ORANGE); // PgUp

        rgb_matrix_set_color(matrix_to_led(2, 2), CLR_ORANGE); // Left
        rgb_matrix_set_color(matrix_to_led(2, 3), CLR_ORANGE); // Down
        rgb_matrix_set_color(matrix_to_led(2, 4), CLR_ORANGE); // Right
        rgb_matrix_set_color(matrix_to_led(2, 5), CLR_ORANGE); // PgDn

        rgb_matrix_set_color(matrix_to_led(3, 2), CLR_ORANGE); // Ins
        rgb_matrix_set_color(matrix_to_led(3, 3), CLR_ORANGE); // Del
        rgb_matrix_set_color(matrix_to_led(3, 4), CLR_ORANGE); // Bksp
        rgb_matrix_set_color(matrix_to_led(3, 5), CLR_ORANGE); // PrtSc

        // Numpad (right side)
        rgb_matrix_set_color(matrix_to_led(6, 5), CLR_ORANGE); // /
        rgb_matrix_set_color(matrix_to_led(6, 4), CLR_ORANGE); // 7
        rgb_matrix_set_color(matrix_to_led(6, 3), CLR_ORANGE); // 8
        rgb_matrix_set_color(matrix_to_led(6, 2), CLR_ORANGE); // 9
        rgb_matrix_set_color(matrix_to_led(6, 1), CLR_ORANGE); // *

        rgb_matrix_set_color(matrix_to_led(7, 5), CLR_ORANGE); // ,
        rgb_matrix_set_color(matrix_to_led(7, 4), CLR_ORANGE); // 4
        rgb_matrix_set_color(matrix_to_led(7, 3), CLR_ORANGE); // 5
        rgb_matrix_set_color(matrix_to_led(7, 2), CLR_ORANGE); // 6
        rgb_matrix_set_color(matrix_to_led(7, 1), CLR_ORANGE); // 0

        rgb_matrix_set_color(matrix_to_led(8, 5), CLR_ORANGE); // -
        rgb_matrix_set_color(matrix_to_led(8, 4), CLR_ORANGE); // 1
        rgb_matrix_set_color(matrix_to_led(8, 3), CLR_ORANGE); // 2
        rgb_matrix_set_color(matrix_to_led(8, 2), CLR_ORANGE); // 3
        rgb_matrix_set_color(matrix_to_led(8, 1), CLR_ORANGE); // +
    }

    // Caps Lock indicator - Green when active
    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(matrix_to_led(3, 0), CLR_GREEN); // Caps Lock key
    }

    return false;
}

#endif // RGB_MATRIX_ENABLE

// Process key presses
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_O:
            if (record->event.pressed) {
                // Toggle O key color state when pressed
                user_state.o_key_is_green = !user_state.o_key_is_green;
            }
            break;
    }
    return true;
}
