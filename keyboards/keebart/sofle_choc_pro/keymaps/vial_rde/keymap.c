// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "transactions.h"
// Overrides the weak US ASCII lookup tables of send_string.c. Unicode input
// types its hex digits through send_char(), so without this the WinCompose
// sequence for U+00E0 comes out as "uaaea" on the French AZERTY host: KC_0
// types a-grave there, not a zero.
#include "sendstring_french.h"
#include "qmk_settings.h"

// The host OS layout is French AZERTY: the firmware only sends raw scancodes
// and the host turns them into Ergol-R glyphs (KC_A -> q, KC_W -> z,
// KC_SCLN -> m, KC_M -> ',', RALT(KC_E) -> EUR ...). Read every keycode below
// with that in mind.
enum layers {
    BASE,    // Ergol-R
    NAV_NUM, // navigation, editing, numpad, F1-F12
    RAISE,   // unchanged, still unreachable
    DK1,     // 1dk dead key: accents, typography, arrows
    EMOJI    // 3dk, reached by tapping the dead key twice
};

// Keys carrying two glyphs that the AZERTY host cannot pair on its own.
// Shift (or Caps Lock) selects the second glyph - see tap_dual_glyph().
//
// The enum starts at QK_KB_0, not SAFE_RANGE: that is the range Vial's
// "customKeycodes" in vial.json addresses, and both lists must match in order
// and in number or the GUI falls back to raw hex.
enum custom_keycodes {
    EG_DLR = QK_KB_0,    // $ 1
    EG_EUR,              // EUR 2
    EG_PCT,              // % 5
    EG_LPRN,             // ( 6
    EG_RPRN,             // ) 7
    EG_AT,               // @ 8
    EG_HASH,             // # 9
    EG_SLSH,             // / 0
    EG_COMM,             // , ;
    EG_DOT,              // . :
    EG_MINS,             // - _
    EG_PSCR,             // tap: PrtScr, hold: NAV_NUM, double tap: toggle NAV_NUM
    // Emoji whose glyph needs a U+FE0F variation selector, so two code points:
    // unicode_map holds one per entry, these go through send_unicode_string().
    EG_SMILE,            // white smiling face
    EG_INFO,             // information source
    EG_RECY,             // recycling symbol
    EG_POINT,            // index pointing up
    EG_WARN,             // warning sign
    EG_GEAR              // gear
};

// Unicode map, transcribed from ergol-r_moergo.json (layers 1dk / 2dk /
// 3dk). UP(i, j) packs both indices in 7 bits, so every pair must stay
// under 128: the 1dk / 2dk pairs come first, singles and emoji after.
enum unicode_names {
    LAQUO,    LAQUO_UP,    // << / heavy left quote
    RAQUO,    RAQUO_UP,    // >> / heavy right quote
    SHARP,    SHARP_UP,    // music sharp / plus-minus
    NEQ,      NEQ_UP,      // not equal / almost equal
    AE,       AE_UP,       // ae ligature
    OE,       OE_UP,       // oe ligature
    O_CIR,    O_CIR_UP,    // o circumflex
    O_SLSH,   O_SLSH_UP,   // o slash
    STAR,     STAR_UP,     // star / heavy asterisk
    TIMES,    TIMES_UP,    // multiply / identical to
    A_CIR,    A_CIR_UP,    // a circumflex
    A_GRV,    A_GRV_UP,    // a grave
    E_GRV,    E_GRV_UP,    // e grave
    E_ACU,    E_ACU_UP,    // e acute
    E_CIR,    E_CIR_UP,    // e circumflex
    BOX_UR,   BOX_UR_UP,   // box up-right / vertical-right
    BOX_H,    BOX_H_UP,    // box horizontal / vertical
    I_DIA,    I_DIA_UP,    // i diaeresis
    I_CIR,    I_CIR_UP,    // i circumflex
    U_CIR,    U_CIR_UP,    // u circumflex
    U_GRV,    U_GRV_UP,    // u grave
    LSAQ,     LSAQ_UP,     // single left quote / left triangle
    CROSS,    CROSS_UP,    // ballot X / heavy multiply
    C_CED,    C_CED_UP,    // c cedilla
    RSAQ,     RSAQ_UP,     // single right quote / right triangle
    MIDDOT,   MIDDOT_UP,   // middle dot / bullet
    ELLIP,    ELLIP_UP,    // ellipsis / small square
    DASH,     DASH_UP,     // en dash / em dash
    CHECK,    CHECK_UP,    // check / heavy check
    ARR_LR,   ARR_LR_UP,   // left-right arrow
    ARR_L,    ARR_L_UP,    // left arrow
    ARR_R,    ARR_R_UP,    // right arrow
    ARR_U,    ARR_U_UP,    // up arrow
    ARR_D,    ARR_D_UP,    // down arrow
    ARR_UH,   ARR_UH_UP,   // up arrowhead / up triangle
    ARR_DH,   ARR_DH_UP,   // down arrowhead / down triangle
    // 1dk characters with no 2dk counterpart
    SUP2, CURREN, RSQUO, DEGREE, DIVIDE, SECT, MICRO, CRARR, NBSP, DIAMOND,
    // Emoji layer, single code point only
    E_QUEST, E_BUG, E_OK, E_ART, E_WAVE, E_MEMO, E_IDEA,
    E_THUMB, E_ALERT, E_SPARK, E_EYES, E_PRAY, E_LINK, E_TEST,
    E_UPSI, E_EXCL, E_ZAP, E_X, E_CONS, E_VERIF, E_ROBOT,
    E_HOUR, E_KISS, E_LEFT, E_RIGHT, E_UP, E_DOWN
};

const uint32_t PROGMEM unicode_map[] = {
    [LAQUO]    = 0x00AB, [LAQUO_UP]    = 0x275D, // << / heavy left quote
    [RAQUO]    = 0x00BB, [RAQUO_UP]    = 0x275E, // >> / heavy right quote
    [SHARP]    = 0x266F, [SHARP_UP]    = 0x00B1, // music sharp / plus-minus
    [NEQ]      = 0x2260, [NEQ_UP]      = 0x2248, // not equal / almost equal
    [AE]       = 0x00E6, [AE_UP]       = 0x00C6, // ae ligature
    [OE]       = 0x0153, [OE_UP]       = 0x0152, // oe ligature
    [O_CIR]    = 0x00F4, [O_CIR_UP]    = 0x00D4, // o circumflex
    [O_SLSH]   = 0x00F8, [O_SLSH_UP]   = 0x00D8, // o slash
    [STAR]     = 0x2605, [STAR_UP]     = 0x2731, // star / heavy asterisk
    [TIMES]    = 0x00D7, [TIMES_UP]    = 0x2261, // multiply / identical to
    [A_CIR]    = 0x00E2, [A_CIR_UP]    = 0x00C2, // a circumflex
    [A_GRV]    = 0x00E0, [A_GRV_UP]    = 0x00C0, // a grave
    [E_GRV]    = 0x00E8, [E_GRV_UP]    = 0x00C8, // e grave
    [E_ACU]    = 0x00E9, [E_ACU_UP]    = 0x00C9, // e acute
    [E_CIR]    = 0x00EA, [E_CIR_UP]    = 0x00CA, // e circumflex
    [BOX_UR]   = 0x2514, [BOX_UR_UP]   = 0x251C, // box up-right / vertical-right
    [BOX_H]    = 0x2500, [BOX_H_UP]    = 0x2502, // box horizontal / vertical
    [I_DIA]    = 0x00EF, [I_DIA_UP]    = 0x00CF, // i diaeresis
    [I_CIR]    = 0x00EE, [I_CIR_UP]    = 0x00CE, // i circumflex
    [U_CIR]    = 0x00FB, [U_CIR_UP]    = 0x00DB, // u circumflex
    [U_GRV]    = 0x00F9, [U_GRV_UP]    = 0x00D9, // u grave
    [LSAQ]     = 0x2039, [LSAQ_UP]     = 0x25C1, // single left quote / left triangle
    [CROSS]    = 0x2717, [CROSS_UP]    = 0x2716, // ballot X / heavy multiply
    [C_CED]    = 0x00E7, [C_CED_UP]    = 0x00C7, // c cedilla
    [RSAQ]     = 0x203A, [RSAQ_UP]     = 0x25B7, // single right quote / right triangle
    [MIDDOT]   = 0x00B7, [MIDDOT_UP]   = 0x2022, // middle dot / bullet
    [ELLIP]    = 0x2026, [ELLIP_UP]    = 0x25AA, // ellipsis / small square
    [DASH]     = 0x2013, [DASH_UP]     = 0x2014, // en dash / em dash
    [CHECK]    = 0x2713, [CHECK_UP]    = 0x2714, // check / heavy check
    [ARR_LR]   = 0x2194, [ARR_LR_UP]   = 0x21D4, // left-right arrow
    [ARR_L]    = 0x2190, [ARR_L_UP]    = 0x21D0, // left arrow
    [ARR_R]    = 0x2192, [ARR_R_UP]    = 0x21D2, // right arrow
    [ARR_U]    = 0x2191, [ARR_U_UP]    = 0x21D1, // up arrow
    [ARR_D]    = 0x2193, [ARR_D_UP]    = 0x21D3, // down arrow
    [ARR_UH]   = 0x2B9D, [ARR_UH_UP]   = 0x25B2, // up arrowhead / up triangle
    [ARR_DH]   = 0x2B9F, [ARR_DH_UP]   = 0x25BC, // down arrowhead / down triangle
    [SUP2]     = 0x00B2,                       // superscript two
    [CURREN]   = 0x00A4,                       // currency sign
    [RSQUO]    = 0x2019,                       // typographic apostrophe
    [DEGREE]   = 0x00B0,                       // degree sign
    [DIVIDE]   = 0x00F7,                       // division sign
    [SECT]     = 0x00A7,                       // section sign
    [MICRO]    = 0x00B5,                       // micro sign
    [CRARR]    = 0x21A9,                       // carriage return arrow
    [NBSP]     = 0x00A0,                       // no-break space
    [DIAMOND]  = 0x25C6,                       // black diamond
    [E_QUEST]  = 0x2753,                      // question
    [E_BUG]    = 0x1F41B,                      // bug
    [E_OK]     = 0x1F44C,                      // ok hand
    [E_ART]    = 0x1F3A8,                      // artist palette
    [E_WAVE]   = 0x1F44B,                      // waving hand
    [E_MEMO]   = 0x1F4DD,                      // memo
    [E_IDEA]   = 0x1F4A1,                      // light bulb
    [E_THUMB]  = 0x1F44D,                      // thumbs up
    [E_ALERT]  = 0x1F6A8,                      // siren
    [E_SPARK]  = 0x2728,                      // sparkles
    [E_EYES]   = 0x1F440,                      // eyes
    [E_PRAY]   = 0x1F64F,                      // folded hands
    [E_LINK]   = 0x1F517,                      // link
    [E_TEST]   = 0x1F9EA,                      // test tube
    [E_UPSI]   = 0x1F643,                      // upside-down face
    [E_EXCL]   = 0x2757,                      // exclamation
    [E_ZAP]    = 0x26A1,                      // high voltage
    [E_X]      = 0x274C,                      // cross mark
    [E_CONS]   = 0x1F6A7,                      // construction
    [E_VERIF]  = 0x2705,                      // check mark button
    [E_ROBOT]  = 0x1F916,                      // robot
    [E_HOUR]   = 0x23F3,                      // hourglass
    [E_KISS]   = 0x1F61A,                      // kissing face
    [E_LEFT]   = 0x1F448,                      // point left
    [E_RIGHT]  = 0x1F449,                      // point right
    [E_UP]     = 0x1F446,                      // point up
    [E_DOWN]   = 0x1F447                       // point down
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * BASE - Ergol-R (glyphs as rendered by the French AZERTY host)
 * ColL ->  0      1      2      3      4      5                           5      4      3      2      1      0 <- ColR
 * RowL ,-----------------------------------------.                    ,-----------------------------------------. RowR
 *   0  | Esc  | $  1 | EUR2 | "  3 | '  4 | %  5 |                    | (  6 | )  7 | @  8 | #  9 | /  0 | =  + |  5
 *      |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 *   1  | Tab  |   q  |   b  |   o  |   p  |   w  |                    |   j  |   m  |   d  | 1dk  |   y  | *  u |  6
 *      |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 *   2  |LShift|   a  |   s  |   e  |   n  |   f  |-------.    ,-------|   l  |   r  |   t  |   i  |   u  | Bspc |  7
 *      |------+------+------+------+------+------| Space |    | Enter |------+------+------+------+------+------|
 *   3  |LCtrl |   z  |   x  |   c  |   v  | ,  ; |-------|    |-------| .  : |   h  |   g  | -  _ |   k  |PrtScn|  8
 *      `-----------------------------------------/       /     \      \-----------------------------------------'
 *   4             | LAlt | Left |Right | Del  | / Enter /       \Space \  | Bspc |  Up  | Down | RGui |            9
 *                 |      |      |      |      |/       /         \      \ |      |      |      |      |
 *                 `----------------------------------'           '------''---------------------------'
 *
 * The upper row and the four ,;/.:/-_ keys emit their second glyph under
 * Shift or Caps Lock; KC_3, KC_4, KC_EQL and KC_NUHS already do so via the
 * host layout and stay bare keycodes.
 * PrtScn: tap = PrtScr, hold = NAV_NUM, double tap = toggle NAV_NUM.
 */

[BASE] = LAYOUT_split_4x6_5(
  KC_ESC,   EG_DLR,  EG_EUR,   KC_3,    KC_4,  EG_PCT,                    EG_LPRN, EG_RPRN,   EG_AT, EG_HASH, EG_SLSH,  KC_EQL,
  KC_TAB,     KC_A,    KC_B,   KC_O,    KC_P,    KC_Z,                       KC_J, KC_SCLN,    KC_D, OSL(DK1),   KC_Y, KC_NUHS,
  KC_LSFT,    KC_Q,    KC_S,   KC_E,    KC_N,    KC_F,                       KC_L,    KC_R,    KC_T,    KC_I,    KC_U, KC_BSPC,
  KC_LCTL,    KC_W,    KC_X,   KC_C,    KC_V, EG_COMM,  KC_SPC,    KC_ENT, EG_DOT,    KC_H,    KC_G, EG_MINS,    KC_K, EG_PSCR,
                    KC_LALT, KC_LEFT, KC_RGHT, KC_DEL, KC_ENT,    KC_SPC, KC_BSPC,   KC_UP, KC_DOWN, KC_RGUI
),
/* NAV_NUM - navigation, editing, numpad, F-keys
 * Ctrl shortcuts are written in AZERTY scancodes: Ctrl+Z (undo) is C(KC_W) and
 * Ctrl+A (select all) is C(KC_Q), because the host swaps those letter pairs.
 * Esc leaves for BASE, the way out when a double tap on PrtScr locked the layer.
 * No Num Lock key: it is on by default on the host.
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | BASE |  F1  |  F2  |  F3  |  F4  |  F5  |                    |  F6  |  F7  |  F8  |  F9  | F10  | F11  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | Redo | Home |  Up  | End  | PgUp |                    |  KP/ |  KP7 |  KP8 |  KP9 |  KP* | F12  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | SelAl| Left | Down |Right | PgDn |-------.    ,-------|  KP- |  KP4 |  KP5 |  KP6 |  KP+ |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      | Undo | Cut  | Copy |Paste |PasteT|-------|    |-------|  KP. |  KP1 |  KP2 |  KP3 |  KP= |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      |      |      | /       /       \      \  | KP0  | KP0  | KP,  |KPEnt |
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
 */
[NAV_NUM] = LAYOUT_split_4x6_5(
  TO(BASE),  KC_F1,   KC_F2,   KC_F3,    KC_F4,    KC_F5,                    KC_F6,  KC_F7,  KC_F8,   KC_F9,  KC_F10,  KC_F11,
  _______, C(KC_Y), KC_HOME,   KC_UP,   KC_END,  KC_PGUP,                  KC_PSLS,  KC_P7,  KC_P8,   KC_P9, KC_PAST,  KC_F12,
  _______, C(KC_Q), KC_LEFT, KC_DOWN,  KC_RGHT,  KC_PGDN,                  KC_PMNS,  KC_P4,  KC_P5,   KC_P6, KC_PPLS, _______,
  _______, C(KC_W), C(KC_X), C(KC_C), C(KC_V), G(C(A(KC_V))), _______, _______, KC_PDOT, KC_P1, KC_P2, KC_P3, KC_PEQL, _______,
                    _______, _______, _______, _______, _______,  _______, KC_P0, KC_P0, KC_PCMM, KC_PENT
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
/* DK1 - 1dk dead key, transcribed from the Glove80 1dk layer.
 * Shift (or Caps Lock) gives the 2dk counterpart, so the whole 2dk layer is
 * folded into UP() pairs instead of a second dead key. One deviation follows
 * from that: the Glove80 puts a-circumflex on its Shift key, which has to stay
 * Shift here. It takes the ae slot, right above a-grave, and ae moves to the
 * free key at the end of the a-grave row.
 * Tapping the dead key again reaches the emoji layer, as the third tap does on
 * the Glove80.
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |  2   |  o/  |  <<  |  >>  |  '   |  deg |                    |      |      |      |  #   |  div |  !=  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |  a^  |  oe  |  o^  |  par |      |                    |  o/  |  mu  |      |EMOJI |  *   |  x   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |  a`  |  e`  |  e'  |  e^  |  ae  |-------.    ,-------|  |_  |  --  |  i:  |  i^  |  u^  |  u`  |
 * |------+------+------+------+------+------|       |    | back  |------+------+------+------+------+------|
 * |      |  <   |  X   |  c,  |  >   |      |-------|    |-------|  .   | ...  | diam |  -   |  ok  |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            | <->  |  <-  |  ->  |      | / ret'd /       \ NBSP \  |  ^^  |  up  | down |  vv  |
 *            `----------------------------------'           '------''---------------------------'
 */
[DK1] = LAYOUT_split_4x6_5(
  UM(SUP2), UM(CURREN), UP(LAQUO, LAQUO_UP), UP(RAQUO, RAQUO_UP), UM(RSQUO), UM(DEGREE),        _______, _______, _______, UP(SHARP, SHARP_UP), UM(DIVIDE), UP(NEQ, NEQ_UP),
  KC_TAB,   UP(A_CIR, A_CIR_UP), UP(OE, OE_UP), UP(O_CIR, O_CIR_UP), UM(SECT), _______,                 UP(O_SLSH, O_SLSH_UP), UM(MICRO), _______, OSL(EMOJI), UP(STAR, STAR_UP), UP(TIMES, TIMES_UP),
  _______,  UP(A_GRV, A_GRV_UP), UP(E_GRV, E_GRV_UP), UP(E_ACU, E_ACU_UP), UP(E_CIR, E_CIR_UP), UP(AE, AE_UP),                UP(BOX_UR, BOX_UR_UP), UP(BOX_H, BOX_H_UP), UP(I_DIA, I_DIA_UP), UP(I_CIR, I_CIR_UP), UP(U_CIR, U_CIR_UP), UP(U_GRV, U_GRV_UP),
  _______,  UP(LSAQ, LSAQ_UP), UP(CROSS, CROSS_UP), UP(C_CED, C_CED_UP), UP(RSAQ, RSAQ_UP), _______, _______, UM(CRARR), UP(MIDDOT, MIDDOT_UP), UP(ELLIP, ELLIP_UP), UM(DIAMOND), UP(DASH, DASH_UP), UP(CHECK, CHECK_UP), _______,
                    UP(ARR_LR, ARR_LR_UP), UP(ARR_L, ARR_L_UP), UP(ARR_R, ARR_R_UP), _______, UM(CRARR),   UM(NBSP), UP(ARR_UH, ARR_UH_UP), UP(ARR_U, ARR_U_UP), UP(ARR_D, ARR_D_UP), UP(ARR_DH, ARR_DH_UP)
),
/* EMOJI - 3dk, reached by tapping the dead key a second time.
 * The ten keycap emoji of the Glove80 row are dropped: each is a three code
 * point sequence and would need its own custom keycode.
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | Quest|  Bug |  Ok  | Palet| Wave |                    | Smile| Memo | iDea |      |Thumb |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | Alert| Spark| Eyes | iNfo | Pray |-------.    ,-------| Link | Recyc| Test | Point| Upside| Excl |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |  Zap |   X  | Cons | Verif| Warn |-------|    |-------| Robot| Hour | Gear |      | Kiss |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      | Left |Right |      | / picker/       \      \  |  Up  | Down |      | Menu |
 *            `----------------------------------'           '------''---------------------------'
 */
[EMOJI] = LAYOUT_split_4x6_5(
  _______, _______,  _______,  _______, _______,  _______,                 _______,  _______, _______, _______, _______, _______,
  _______, UM(E_QUEST), UM(E_BUG), UM(E_OK), UM(E_ART), UM(E_WAVE),         EG_SMILE, UM(E_MEMO), UM(E_IDEA), _______, UM(E_THUMB), _______,
  _______, UM(E_ALERT), UM(E_SPARK), UM(E_EYES), EG_INFO, UM(E_PRAY),       UM(E_LINK), EG_RECY, UM(E_TEST), EG_POINT, UM(E_UPSI), UM(E_EXCL),
  _______, UM(E_ZAP), UM(E_X), UM(E_CONS), UM(E_VERIF), EG_WARN, _______, _______, UM(E_ROBOT), UM(E_HOUR), EG_GEAR, _______, UM(E_KISS), _______,
                    _______, UM(E_LEFT), UM(E_RIGHT), _______, G(KC_COMM),   _______, _______, UM(E_UP), UM(E_DOWN), KC_APP
),
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [BASE]  = { ENCODER_CCW_CW(KC_UP,   KC_DOWN), ENCODER_CCW_CW(KC_LEFT, KC_RGHT) },
    [NAV_NUM] = { ENCODER_CCW_CW(KC_WH_U, KC_WH_D), ENCODER_CCW_CW(KC_WH_L, KC_WH_R) },
    [RAISE] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
    [DK1] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
    [EMOJI] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) }
};
#endif

// RGB Matrix Custom Lighting Configuration
#ifdef RGB_MATRIX_ENABLE

// Color definitions
#define CLR_WHITE   60, 60, 60     // #3c3c3c (25% brightness)
#define CLR_YELLOW  64, 64, 0      // #404000 (25% brightness)
#define CLR_GREEN   0,   64, 0     // #004000 (25% brightness)
#define CLR_ORANGE  64, 35, 0      // #402300 (25% brightness)
#define CLR_RED     64, 0, 0       // #400000 (25% brightness)
#define CLR_BLUE    0, 0, 64       // #000040 (25% brightness)
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
    if (layer == BASE || layer == NAV_NUM) {
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

        // NAV thumb key - Orange
        rgb_matrix_set_color(matrix_to_led(4, 2), CLR_ORANGE); // NAV

        // Right thumb keys: Space[9,4], RCtrl[9,3], RAlt[9,2], Menu[9,1], RGui[9,0]
        // 3rd thumb key = RAlt at [9, 2] - Blue
        rgb_matrix_set_color(matrix_to_led(9, 2), CLR_BLUE); // RAlt
        // 4th thumb key = Menu at [9, 1] - NOT colored (remains OFF)
    }

    // NAV_NUM layer overlay - Orange for navigation and numpad keys
    if (layer == NAV_NUM) {
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

// Dual-glyph keys -----------------------------------------------------------
//
// Vial cannot express these: a dynamic tap dance only offers tap/hold, and a
// static key_overrides[] is ignored under Vial (keymap_introspection.c guards
// on !VIAL_KEY_OVERRIDE_ENABLE). A key override would also be blind to Caps
// Lock. So the pairs live here.

typedef struct {
    uint16_t keycode;
    uint16_t unshifted;
    uint16_t shifted;
} dual_glyph_t;

static const dual_glyph_t dual_glyphs[] = {
    {EG_DLR,  KC_RBRC,          LSFT(KC_1)}, // $ 1
    {EG_EUR,  RALT(KC_E),       LSFT(KC_2)}, // EUR 2
    {EG_PCT,  LSFT(KC_QUOT),    LSFT(KC_5)}, // % 5
    {EG_LPRN, KC_5,             LSFT(KC_6)}, // ( 6
    {EG_RPRN, KC_MINS,          LSFT(KC_7)}, // ) 7
    {EG_AT,   RALT(KC_0),       LSFT(KC_8)}, // @ 8
    {EG_HASH, RALT(KC_3),       LSFT(KC_9)}, // # 9
    {EG_SLSH, LSFT(KC_DOT),     LSFT(KC_0)}, // / 0
    {EG_COMM, KC_M,             KC_COMM},    // , ;
    {EG_DOT,  LSFT(KC_COMM),    KC_DOT},     // . :
    {EG_MINS, KC_6,             KC_8},       // - _
};

_Static_assert(ARRAY_SIZE(dual_glyphs) == EG_PSCR - EG_DLR,
               "dual_glyphs[] must stay aligned with the EG_* enum order");

// The French Windows layout already inverts the digit row under Caps Lock
// (KC_1 alone types '1'). Undo that so the pair above stays literal.
static uint16_t apply_caps_lock_digit_row(uint16_t keycode) {
    uint8_t basic = keycode & 0xFF;
    if (basic < KC_1 || basic > KC_0) {
        return keycode;
    }
    return keycode ^ QK_LSFT;
}

static bool dual_glyph_wants_shift(const dual_glyph_t *pair) {
    // These keys never reach process_caps_word (process_record_user consumes
    // them first), so Caps Word survives on its own. But its pending weak shift
    // would flip the glyph to the digit mid-word. Drop it, except on EG_MINS
    // where '_' is what snake_case wants - the same call QMK's default
    // caps_word_press_user makes for KC_MINS.
    if (is_caps_word_on() && pair->keycode != EG_MINS) {
        del_weak_mods(MOD_BIT(KC_LSFT));
    }

    uint8_t mods = get_mods() | get_weak_mods();
#ifndef NO_ACTION_ONESHOT
    mods |= get_oneshot_mods();
#endif
    // Same rule as unicodemap_index(): Shift XOR Caps Lock picks the second glyph.
    return ((mods & MOD_MASK_SHIFT) != 0) ^ host_keyboard_led_state().caps_lock;
}

static void tap_dual_glyph(const dual_glyph_t *pair, bool want_shifted) {
    bool caps = host_keyboard_led_state().caps_lock;

    uint16_t keycode = want_shifted ? pair->shifted : pair->unshifted;
    if (caps) {
        keycode = apply_caps_lock_digit_row(keycode);
    }

    // The physical Shift must not leak into the keycode we send.
    uint8_t saved_mods = get_mods();
    del_mods(MOD_MASK_SHIFT);
    del_weak_mods(MOD_MASK_SHIFT);
#ifndef NO_ACTION_ONESHOT
    del_oneshot_mods(MOD_MASK_SHIFT);
#endif
    send_keyboard_report();

    tap_code16(keycode);

    set_mods(saved_mods);
    send_keyboard_report();
}

// Auto Shift has two switches: QS_auto_shift_enable is Vial's QMK Settings
// toggle, which process_auto_shift tests first and returns on, while
// get_autoshift_state() only reports the AS_TOGG runtime state and stays true
// when the setting is off. Both must hold, or a key yielding to Auto Shift is
// simply never typed by anyone.
static bool autoshift_will_run(void) {
    return QS_auto_shift_enable && get_autoshift_state();
}

// Dead-key layer: hold reaches the 2dk counterpart --------------------------

static struct {
    uint16_t keycode;
    uint16_t timer;
    bool     pending;
} dk_hold = {KC_NO, 0, false};

// extra_shift is what the hold adds; unicodemap_index() reads the real Shift
// and Caps Lock state on its own.
static void dk_hold_emit(bool extra_shift) {
    if (extra_shift) {
        add_weak_mods(MOD_BIT(KC_LSFT));
    }
    register_unicodemap(unicodemap_index(dk_hold.keycode));
    if (extra_shift) {
        del_weak_mods(MOD_BIT(KC_LSFT));
    }
    dk_hold.pending = false;
}

// PrtScr: tap / hold / double tap ---------------------------------------------
//
// Hand-rolled because Vial owns tap_dance_actions[] (vial.c), so a static tap
// dance does not link. KC_PSCR is held back until the double-tap window closes,
// otherwise a double tap would flash the Windows capture overlay first.

enum pscr_state { PSCR_IDLE, PSCR_HELD, PSCR_TAPPED };

static struct {
    enum pscr_state state;
    uint16_t        timer;
    bool            locked; // layer left on by a double tap
} pscr = {PSCR_IDLE, 0, false};

static void pscr_press(void) {
    if (pscr.locked) {
        // Any further tap just releases the lock - no PrtScr.
        layer_off(NAV_NUM);
        pscr.locked = false;
        pscr.state  = PSCR_IDLE;
        return;
    }
    if (pscr.state == PSCR_TAPPED && timer_elapsed(pscr.timer) < TAPPING_TERM) {
        // Second tap inside the window: lock the layer, swallow the PrtScr.
        layer_on(NAV_NUM);
        pscr.locked = true;
        pscr.state  = PSCR_IDLE;
        return;
    }
    pscr.state = PSCR_HELD;
    pscr.timer = timer_read();
    layer_on(NAV_NUM);
}

static void pscr_release(void) {
    if (pscr.state != PSCR_HELD) {
        return;
    }
    layer_off(NAV_NUM);
    if (timer_elapsed(pscr.timer) < TAPPING_TERM) {
        // Might be the first half of a double tap: wait before typing PrtScr.
        pscr.state = PSCR_TAPPED;
        pscr.timer = timer_read();
    } else {
        pscr.state = PSCR_IDLE;
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // TO(BASE) on Esc, or any other exit, must not leave the lock flag set.
    if (!(state & (1UL << NAV_NUM))) {
        pscr.locked = false;
    }
    return state;
}

void matrix_scan_user(void) {
    if (dk_hold.pending && timer_elapsed(dk_hold.timer) >= get_generic_autoshift_timeout()) {
        dk_hold_emit(true);
    }
    if (pscr.state == PSCR_TAPPED && timer_elapsed(pscr.timer) >= TAPPING_TERM) {
        pscr.state = PSCR_IDLE;
        tap_code(KC_PSCR);
    }
}

// Auto Shift -----------------------------------------------------------------
//
// Hold reaches the 2dk counterpart on the dead-key layer. Vial's qmk_settings.c
// replaces get_auto_shifted_key() and, unpatched, never calls this hook; the
// fork carries a one-line fix restoring that fallback.
//
// process_record_user runs at quantum.c:364, process_auto_shift at :411, so the
// EG_* keys only reach Auto Shift because their case below yields to it while
// it is enabled. That works for them because nothing else in the chain types a
// custom keycode.
//
// The UP() pairs cannot use Auto Shift at all: process_unicode_common runs at
// :405, ahead of it, so the character was typed once on press and once more by
// autoshift_press_user on release. They get the small hold timer below instead,
// which consumes the key before either of those runs.

bool get_custom_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    return keycode >= EG_DLR && keycode <= EG_MINS;
}

void autoshift_press_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    if (keycode >= EG_DLR && keycode <= EG_MINS) {
        const dual_glyph_t *pair = &dual_glyphs[keycode - EG_DLR];
        // Hold reaches the digit, and so do Shift and Caps Lock: either is
        // enough, so the two decisions are OR-ed rather than XOR-ed.
        tap_dual_glyph(pair, shifted || dual_glyph_wants_shift(pair));
        return;
    }
    if (shifted) {
        add_weak_mods(MOD_BIT(KC_LSFT));
    }
    register_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
}

void autoshift_release_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    // The branch above types and releases in one go, nothing stays held.
    if (keycode >= EG_DLR && keycode <= EG_MINS) {
        return;
    }
    unregister_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
}

// Process key presses
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_O:
            if (record->event.pressed) {
                // Toggle O key color state when pressed
                user_state.o_key_is_green = !user_state.o_key_is_green;
            }
            break;

        case QK_UNICODEMAP_PAIR ... QK_UNICODEMAP_PAIR_MAX:
            if (!autoshift_will_run()) {
                break; // let process_unicodemap type it on press, as usual
            }
            if (record->event.pressed) {
                dk_hold.keycode = keycode;
                dk_hold.timer   = timer_read();
                dk_hold.pending = true;
            } else if (dk_hold.pending && dk_hold.keycode == keycode) {
                dk_hold_emit(false);
            }
            return false;

        case EG_PSCR:
            if (record->event.pressed) {
                pscr_press();
            } else {
                pscr_release();
            }
            return false;

        case EG_DLR ... EG_MINS:
            // Yield to Auto Shift when it is on, so holding reaches the digit.
            // It is consulted later in the chain and would never see these keys
            // otherwise. When it is off, type the glyph here instead.
            if (autoshift_will_run()) {
                return true;
            }
            if (record->event.pressed) {
                const dual_glyph_t *pair = &dual_glyphs[keycode - EG_DLR];
                tap_dual_glyph(pair, dual_glyph_wants_shift(pair));
            }
            return false;

        // Emoji needing a U+FE0F variation selector: two code points, which a
        // unicode_map entry cannot hold.
        case EG_SMILE ... EG_GEAR:
            if (record->event.pressed) {
                static const char *const vs16_emoji[] = {
                    "☺️", // white smiling face
                    "ℹ️", // information source
                    "♻️", // recycling symbol
                    "☝️", // index pointing up
                    "⚠️", // warning sign
                    "⚙️", // gear
                };
                send_unicode_string(vs16_emoji[keycode - EG_SMILE]);
            }
            return false;
    }
    return true;
}
