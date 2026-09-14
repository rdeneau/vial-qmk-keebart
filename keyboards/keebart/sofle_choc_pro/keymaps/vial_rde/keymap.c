// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
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
    SYMBOL,  // brackets, operators, punctuation
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
    EG_DEG,              // deg 0
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
    EG_GEAR,             // gear
    // Space: tap types a space, hold reaches SYMBOL, double tap locks it.
    EG_SPC,
    // Everything below is appended, never inserted: the eighteen keycodes above
    // are addressed by index in the .vil exports, and reordering them would
    // silently rewrite a saved layout.
    EG_QUES,             // ? !
    // AltGr+9 and AltGr+2 are dead on AZERTY; a trailing space makes them
    // literal, so one tap is enough. The backtick keeps its dead behaviour.
    EG_CIRC,             // ^
    EG_TILD,             // ~
    // Keycap emoji: digit, U+FE0F, U+20E3 - three code points each.
    EG_KC1, EG_KC2, EG_KC3, EG_KC4, EG_KC5,
    EG_KC6, EG_KC7, EG_KC8, EG_KC9, EG_KC0,
    EG_KCAST,            // keycap asterisk
    EG_ARRL,             // black left-pointing triangle
    EG_ARRR              // black right-pointing triangle
};

// Unicode map, transcribed from ergol-r_moergo.json (layers 1dk / 2dk /
// 3dk). UP(i, j) packs both indices in 7 bits, so every pair must stay
// under 128: the 1dk / 2dk pairs come first, singles and emoji after.
enum unicode_names {
    LAQUO,    LAQUO_UP,    // << / heavy left quote
    RAQUO,    RAQUO_UP,    // >> / heavy right quote
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
    SUP2, CURREN, RSQUO, PERMIL, DIVIDE, SECT, MICRO, CRARR, NBSP, DIAMOND,
    SHARP, PLUSMIN, PILCROW, TAB_CHR,
    // Emoji layer, single code point only
    E_QUEST, E_BUG, E_OK, E_ART, E_WAVE, E_MEMO, E_IDEA,
    E_THUMB, E_ALERT, E_SPARK, E_EYES, E_PRAY, E_LINK, E_TEST,
    E_UPSI, E_EXCL, E_ZAP, E_X, E_CONS, E_VERIF, E_ROBOT,
    E_HOUR, E_KISS, E_LEFT, E_RIGHT, E_UP, E_DOWN,
    E_STAR, E_MINUS, E_PLUS, E_SHIP, E_BROOM
};

const uint32_t PROGMEM unicode_map[] = {
    [LAQUO]    = 0x00AB, [LAQUO_UP]    = 0x275D, // << / heavy left quote
    [RAQUO]    = 0x00BB, [RAQUO_UP]    = 0x275E, // >> / heavy right quote
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
    [PERMIL]   = 0x2030,                       // per mille sign
    [DIVIDE]   = 0x00F7,                       // division sign
    [SECT]     = 0x00A7,                       // section sign
    [MICRO]    = 0x00B5,                       // micro sign
    [CRARR]    = 0x21A9,                       // carriage return arrow
    [NBSP]     = 0x00A0,                       // no-break space
    [DIAMOND]  = 0x25C6,                       // black diamond
    [SHARP]    = 0x266F,                       // music sharp sign
    [PLUSMIN]  = 0x00B1,                       // plus-minus sign
    [PILCROW]  = 0x00B6,                       // pilcrow sign
    // A real U+0009, typed through WinCompose. KC_TAB would be indistinguishable
    // from the Tab key itself, and an editor is free to turn that into indent,
    // completion or focus change instead of a character.
    [TAB_CHR]  = 0x0009,                       // character tabulation
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
    [E_DOWN]   = 0x1F447,                      // point down
    [E_STAR]   = 0x2B50,                      // star
    [E_MINUS]  = 0x2796,                      // heavy minus sign
    [E_PLUS]   = 0x2795,                      // heavy plus sign
    [E_SHIP]   = 0x1F6A2,                      // ship
    [E_BROOM]  = 0x1F9F9                       // broom
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * BASE - Ergol-R (glyphs as rendered by the French AZERTY host)
 * ColL ->  0      1      2      3      4      5                           5      4      3      2      1      0 <- ColR
 * RowL ,-----------------------------------------.                    ,-----------------------------------------. RowR
 *   0  | Esc  | $  1 | EUR2 | "  3 | '  4 | %  5 |                    | (  6 | )  7 | @  8 | #  9 | deg0 | =  + |  5
 *      |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 *   1  | Tab  |   q  |   b  |   o  |   p  |   w  |                    |   j  |   m  |   d  | 1dk  |   y  | *  u |  6
 *      |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 *   2  |LShift|   a  |   s  |   e  |   n  |   f  |-------.    ,-------|   l  |   r  |   t  |   i  |   u  | ?  ! |  7
 *      |------+------+------+------+------+------| Space |    | Enter |------+------+------+------+------+------|
 *   3  |LCtrl |   z  |   x  |   c  |   v  | ,  ; |-------|    |-------| .  : |   h  |   g  | -  _ |   k  |PrtScn|  8
 *      `-----------------------------------------/       /     \      \-----------------------------------------'
 *   4             | LAlt | Left |Right | Del  | / Enter /       \ SYM  \  | Bspc |  Up  | Down | RGui |            9
 *                 |      |      |      |      |/       /         \      \ |      |      |      |      |
 *                 `----------------------------------'           '------''---------------------------'
 *
 * The upper row and the four ,;/.:/-_ keys emit their second glyph under
 * Shift or Caps Lock; KC_3, KC_4, KC_EQL and KC_NUHS already do so via the
 * host layout and stay bare keycodes.
 * PrtScn: tap = PrtScr, hold = NAV_NUM, double tap = toggle NAV_NUM.
 */

[BASE] = LAYOUT_split_4x6_5(
  KC_ESC,   EG_DLR,  EG_EUR,   KC_3,    KC_4,  EG_PCT,                    EG_LPRN, EG_RPRN,   EG_AT, EG_HASH,  EG_DEG,  KC_EQL,
  KC_TAB,     KC_A,    KC_B,   KC_O,    KC_P,    KC_Z,                       KC_J, KC_SCLN,    KC_D, OSL(DK1),   KC_Y, KC_NUHS,
  KC_LSFT,    KC_Q,    KC_S,   KC_E,    KC_N,    KC_F,                       KC_L,    KC_R,    KC_T,    KC_I,    KC_U, EG_QUES,
  KC_LCTL,    KC_W,    KC_X,   KC_C,    KC_V, EG_COMM,  KC_SPC,    KC_ENT, EG_DOT,    KC_H,    KC_G, EG_MINS,    KC_K, EG_PSCR,
                    KC_LALT, KC_LEFT, KC_RGHT, KC_DEL, KC_ENT,    EG_SPC, KC_BSPC,   KC_UP, KC_DOWN, KC_RGUI
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
/* SYMBOL - brackets, operators, punctuation (hold the right thumb Space)
 * Transcribed from the Glove80 Symbol layer; its Tab, Space, Enter, Ins, Esc,
 * Del and one-shot modifiers are left transparent, NAV_NUM already covers them.
 * Keycodes are AZERTY scancodes, so AltGr+4 types '{' and KC_1 types '&'. The
 * = - * . / cells use their keypad twins instead, which the host maps the same
 * way whatever its layout.
 * Esc leaves for BASE, the way out when a double tap on Space locked the layer.
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | BASE |      |  {   |  }   |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |  [   |  (   |  )   |  ]   |      |                    |  ~   |  ^   |      |      |      |  ?   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |  <   |  =   |  -   |  >   |  ,   |-------.    ,-------|  .   |  /   |      |      |      |  !   |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |  &   |  |   |  +   |  *   |  ;   |-------|    |-------|  :   |  \   |  `   |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      |  _   |      | /       /       \      \  |      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
 */
[SYMBOL] = LAYOUT_split_4x6_5(
  TO(BASE), _______, ALGR(KC_4), ALGR(KC_EQL), _______, _______,             _______,     _______, _______, _______, _______,  _______,
  _______, ALGR(KC_5), KC_5, KC_MINS, ALGR(KC_MINS), _______,                EG_TILD,    EG_CIRC, _______, _______, _______, S(KC_M),
  _______, KC_NUBS, KC_PEQL, KC_PMNS, S(KC_NUBS), KC_M,                      KC_PDOT,   KC_PSLS, _______, _______, _______, KC_SLSH,
  _______, KC_1, ALGR(KC_6), S(KC_EQL), KC_PAST, KC_COMM, _______, _______,   KC_DOT, ALGR(KC_8), ALGR(KC_7), _______, _______, _______,
                    _______, _______, KC_8, _______, _______,    _______, _______, _______, _______, _______
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
 * |  2   |  o/  |  <<  |  >>  |  '   | 0/00 |                    |      |      |      | shrp |  +-  |  !=  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | TAB  |  a^  |  oe  |  o^  |  par |  pil |                    |  o/  |  mu  | div  |EMOJI |  *   |  x   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |  a`  |  e`  |  e'  |  e^  |  ae  |-------.    ,-------|  |_  |  --  |  i:  |  i^  |  u^  |  u`  |
 * |------+------+------+------+------+------|       |    | back  |------+------+------+------+------+------|
 * |      |  <   |  X   |  c,  |  >   |      |-------|    |-------|  .   | ...  | diam |  -   |  ok  |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            | <->  |  <-  |  ->  |      | / ret'd /       \ NBSP \  |  ^^  |  up  | down |  vv  |
 *            `----------------------------------'           '------''---------------------------'
 */
[DK1] = LAYOUT_split_4x6_5(
  UM(SUP2), UM(CURREN), UP(LAQUO, LAQUO_UP), UP(RAQUO, RAQUO_UP), UM(RSQUO), UM(PERMIL),        _______, _______, _______, UM(SHARP), UM(PLUSMIN), UP(NEQ, NEQ_UP),
  UM(TAB_CHR), UP(A_CIR, A_CIR_UP), UP(OE, OE_UP), UP(O_CIR, O_CIR_UP), UM(SECT), UM(PILCROW),             UP(O_SLSH, O_SLSH_UP), UM(MICRO), UM(DIVIDE), OSL(EMOJI), UP(STAR, STAR_UP), UP(TIMES, TIMES_UP),
  _______,  UP(A_GRV, A_GRV_UP), UP(E_GRV, E_GRV_UP), UP(E_ACU, E_ACU_UP), UP(E_CIR, E_CIR_UP), UP(AE, AE_UP),                UP(BOX_UR, BOX_UR_UP), UP(BOX_H, BOX_H_UP), UP(I_DIA, I_DIA_UP), UP(I_CIR, I_CIR_UP), UP(U_CIR, U_CIR_UP), UP(U_GRV, U_GRV_UP),
  _______,  UP(LSAQ, LSAQ_UP), UP(CROSS, CROSS_UP), UP(C_CED, C_CED_UP), UP(RSAQ, RSAQ_UP), _______, _______, UM(CRARR), UP(MIDDOT, MIDDOT_UP), UP(ELLIP, ELLIP_UP), UM(DIAMOND), UP(DASH, DASH_UP), UP(CHECK, CHECK_UP), _______,
                    UP(ARR_LR, ARR_LR_UP), UP(ARR_L, ARR_L_UP), UP(ARR_R, ARR_R_UP), _______, UM(CRARR),   UM(NBSP), UP(ARR_UH, ARR_UH_UP), UP(ARR_U, ARR_U_UP), UP(ARR_D, ARR_D_UP), UP(ARR_DH, ARR_DH_UP)
),
/* EMOJI - 3dk, reached by tapping the dead key a second time.
 * The digit row carries the keycap emoji: three code points each, so they get
 * their own custom keycodes instead of a unicode_map entry.
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      | KC 1 | KC 2 | KC 3 | KC 4 | KC 5 |                    | KC 6 | KC 7 | KC 8 | KC 9 | KC 0 | Plus |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | Quest|  Bug |  Ok  | Palet| Wave |                    | Smile| Memo | iDea | Star |Thumb | KC * |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | Alert| Spark| Eyes | iNfo | Pray |-------.    ,-------| Link | Recyc| Test | Point| Upside| Excl |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |  Zap |   X  | Cons | Verif| Warn |-------|    |-------| Robot| Hour | Gear | Minus| Kiss |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            | LTri | Left |Right | RTri | / picker/       \ Ship \  | Broom|  Up  | Down | Menu |
 *            `----------------------------------'           '------''---------------------------'
 */
[EMOJI] = LAYOUT_split_4x6_5(
  _______,  EG_KC1,   EG_KC2,   EG_KC3,  EG_KC4,   EG_KC5,                   EG_KC6,   EG_KC7,  EG_KC8,  EG_KC9,  EG_KC0, UM(E_PLUS),
  _______, UM(E_QUEST), UM(E_BUG), UM(E_OK), UM(E_ART), UM(E_WAVE),         EG_SMILE, UM(E_MEMO), UM(E_IDEA), UM(E_STAR), UM(E_THUMB), EG_KCAST,
  _______, UM(E_ALERT), UM(E_SPARK), UM(E_EYES), EG_INFO, UM(E_PRAY),       UM(E_LINK), EG_RECY, UM(E_TEST), EG_POINT, UM(E_UPSI), UM(E_EXCL),
  _______, UM(E_ZAP), UM(E_X), UM(E_CONS), UM(E_VERIF), EG_WARN, KC_NO, KC_NO, UM(E_ROBOT), UM(E_HOUR), EG_GEAR, UM(E_MINUS), UM(E_KISS), KC_NO,
                    EG_ARRL, UM(E_LEFT), UM(E_RIGHT), EG_ARRR, G(KC_COMM),   UM(E_SHIP), UM(E_BROOM), UM(E_UP), UM(E_DOWN), KC_APP
),
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [BASE]  = { ENCODER_CCW_CW(KC_UP,   KC_DOWN), ENCODER_CCW_CW(KC_LEFT, KC_RGHT) },
    [NAV_NUM] = { ENCODER_CCW_CW(KC_WH_U, KC_WH_D), ENCODER_CCW_CW(KC_WH_L, KC_WH_R) },
    [SYMBOL] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
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
#define CLR_VIOLET  48, 0, 64      // #300040 (25% brightness)
#define CLR_PINK    64, 14, 46     // #400E2E (25% brightness)
#define CLR_OFF     0, 0, 0

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

// Keyboard post-init to set the default brightness
void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 0, 0); // Start with all LEDs off
    rgb_matrix_set_speed_noeeprom(128);
    rgb_matrix_enable_noeeprom();
    rgb_matrix_set_flags(LED_FLAG_ALL);
}

// rgb_matrix_set_color() takes a global LED index, but on a split half the core
// only subtracts the half offset for indices that belong to that half; an index
// from the other half is passed through and lands on a local LED of the same
// number. So the Esc colour, written at index 26, also reached the right half's
// twenty-seventh LED - the top right key, F11 on NavNum and = + on Symbol.
// Painting only what this pass owns is what keeps the two halves apart.
static uint8_t led_lo, led_hi;

static void set_led(uint8_t led, uint8_t red, uint8_t green, uint8_t blue) {
    if (led >= led_lo && led < led_hi) {
        rgb_matrix_set_color(led, red, green, blue);
    }
}

// Custom LED indicator function
//
// Every layer shows the same static map - white edges, yellow digit row, green
// home row, orange PrtScr - and the Esc key alone says which layer is active.
// That is the only readable signal left once a layer is locked: a per-layer
// overlay would need one map per layer and still be invisible while typing.
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state);

    led_lo = led_min;
    led_hi = led_max;

    for (uint8_t i = led_min; i < led_max; i++) {
        set_led(i, CLR_OFF);
    }

    // Outer columns - white. Left: Esc, Tab, Shift, Ctrl.
    set_led(matrix_to_led(0, 0), CLR_WHITE);
    set_led(matrix_to_led(1, 0), CLR_WHITE);
    set_led(matrix_to_led(2, 0), CLR_WHITE);
    set_led(matrix_to_led(3, 0), CLR_WHITE);
    // Right: * u, Bspc, PrtScr.
    set_led(matrix_to_led(6, 0), CLR_WHITE);
    set_led(matrix_to_led(7, 0), CLR_WHITE);
    set_led(matrix_to_led(8, 0), CLR_WHITE);

    // Digit row - yellow, both halves.
    for (uint8_t col = 1; col <= 5; col++) {
        set_led(matrix_to_led(0, col), CLR_YELLOW);
    }
    for (uint8_t col = 0; col <= 5; col++) {
        set_led(matrix_to_led(5, col), CLR_YELLOW);
    }

    // Home row resting keys - green: a s e n on the left, r t i u on the right.
    for (uint8_t col = 1; col <= 4; col++) {
        set_led(matrix_to_led(2, col), CLR_GREEN);
        set_led(matrix_to_led(7, col), CLR_GREEN);
    }

    // NAV_NUM repaints the two halves it redefines: the digit row is F1 to F12
    // there, and the right half is a numeric keypad.
    if (layer == NAV_NUM) {
        // F1-F5 on the left, F6-F11 on the right, F12 one row below.
        for (uint8_t col = 1; col <= 5; col++) {
            set_led(matrix_to_led(0, col), CLR_PINK);
        }
        for (uint8_t col = 0; col <= 5; col++) {
            set_led(matrix_to_led(5, col), CLR_PINK);
        }
        set_led(matrix_to_led(6, 0), CLR_PINK);

        // The keypad digits only: 7 8 9 / 4 5 6 / 1 2 3, then the two zeros on
        // the thumbs. The operators around them stay as the base map had them.
        for (uint8_t row = 6; row <= 8; row++) {
            for (uint8_t col = 2; col <= 4; col++) {
                set_led(matrix_to_led(row, col), CLR_YELLOW);
            }
        }
        set_led(matrix_to_led(9, 3), CLR_YELLOW);
        set_led(matrix_to_led(9, 2), CLR_YELLOW);
    }

    // The 1dk key - red, and violet on the 1dk layer itself, where the next tap
    // on it reaches the violet Emoji layer. NAV_NUM and EMOJI give that key
    // something else entirely, so the marker would only mislead there.
    if (layer == DK1) {
        set_led(matrix_to_led(6, 2), CLR_VIOLET);
    } else if (layer != NAV_NUM && layer != EMOJI) {
        set_led(matrix_to_led(6, 2), CLR_RED);
    }

    // PrtScr - orange, the key that reaches NAV_NUM.
    set_led(matrix_to_led(8, 0), CLR_ORANGE);

    // Esc - the active layer. BASE keeps the white of its column.
    switch (layer) {
        case NAV_NUM:
            set_led(matrix_to_led(0, 0), CLR_ORANGE);
            break;
        case SYMBOL:
            set_led(matrix_to_led(0, 0), CLR_BLUE);
            break;
        case DK1:
            set_led(matrix_to_led(0, 0), CLR_RED);
            break;
        case EMOJI:
            set_led(matrix_to_led(0, 0), CLR_VIOLET);
            break;
        default:
            break;
    }

    // Caps Lock indicator - green on the Shift key that switches it.
    if (host_keyboard_led_state().caps_lock) {
        set_led(matrix_to_led(2, 0), CLR_GREEN);
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
    {EG_DEG,  LSFT(KC_MINS),    LSFT(KC_0)}, // deg 0
    {EG_COMM, KC_M,             KC_COMM},    // , ;
    {EG_DOT,  LSFT(KC_COMM),    KC_DOT},     // . :
    {EG_MINS, KC_6,             KC_8},       // - _
    // Appended out of enum order, hence the lookup by keycode below.
    {EG_QUES, LSFT(KC_M),       KC_SLSH},    // ? !
};

// The pairs no longer form one contiguous keycode range, so they are found by
// keycode rather than indexed. Twelve entries: a scan costs nothing.
static const dual_glyph_t *dual_glyph_for(uint16_t keycode) {
    for (uint8_t i = 0; i < ARRAY_SIZE(dual_glyphs); i++) {
        if (dual_glyphs[i].keycode == keycode) {
            return &dual_glyphs[i];
        }
    }
    return NULL;
}

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

// Layer keys: tap / hold / double tap --------------------------------------
//
// Hand-rolled because Vial owns tap_dance_actions[] (vial.c), so a static tap
// dance does not link. The tap keycode is held back until the double-tap window
// closes, otherwise a double tap would emit it first - which for PrtScr would
// flash the Windows capture overlay.

enum tap_layer_state { TL_IDLE, TL_HELD, TL_TAPPED };

typedef struct {
    uint8_t              layer;
    uint16_t             tap_keycode;
    enum tap_layer_state state;
    uint16_t             timer;
    bool                 locked; // layer left on by a double tap
} tap_layer_t;

static tap_layer_t tap_layers[] = {
    {NAV_NUM, KC_PSCR, TL_IDLE, 0, false},
    {SYMBOL,  KC_SPC,  TL_IDLE, 0, false},
};

static tap_layer_t *tap_layer_for(uint16_t keycode) {
    switch (keycode) {
        case EG_PSCR:
            return &tap_layers[0];
        case EG_SPC:
            return &tap_layers[1];
        default:
            return NULL;
    }
}

static void tap_layer_press(tap_layer_t *tl) {
    if (tl->locked) {
        // Any further tap just releases the lock - no keycode emitted.
        layer_off(tl->layer);
        tl->locked = false;
        tl->state  = TL_IDLE;
        return;
    }
    if (tl->state == TL_TAPPED && timer_elapsed(tl->timer) < TAPPING_TERM) {
        // Second tap inside the window: lock the layer, swallow the keycode.
        layer_on(tl->layer);
        tl->locked = true;
        tl->state  = TL_IDLE;
        return;
    }
    tl->state = TL_HELD;
    tl->timer = timer_read();
    layer_on(tl->layer);
}

static void tap_layer_release(tap_layer_t *tl) {
    if (tl->state != TL_HELD) {
        return;
    }
    layer_off(tl->layer);
    if (timer_elapsed(tl->timer) < TAPPING_TERM) {
        // Might be the first half of a double tap: wait before emitting.
        tl->state = TL_TAPPED;
        tl->timer = timer_read();
    } else {
        tl->state = TL_IDLE;
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // TO(BASE) on Esc, or any other exit, must not leave a lock flag set.
    for (uint8_t i = 0; i < ARRAY_SIZE(tap_layers); i++) {
        if (!(state & (1UL << tap_layers[i].layer))) {
            tap_layers[i].locked = false;
        }
    }
    return state;
}

void matrix_scan_user(void) {
    if (dk_hold.pending && timer_elapsed(dk_hold.timer) >= get_generic_autoshift_timeout()) {
        dk_hold_emit(true);
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(tap_layers); i++) {
        tap_layer_t *tl = &tap_layers[i];
        if (tl->state == TL_TAPPED && timer_elapsed(tl->timer) >= TAPPING_TERM) {
            tl->state = TL_IDLE;
            tap_code(tl->tap_keycode);
        }
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
    return dual_glyph_for(keycode) != NULL;
}

void autoshift_press_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    const dual_glyph_t *pair = dual_glyph_for(keycode);
    if (pair != NULL) {
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
    if (dual_glyph_for(keycode) != NULL) {
        return;
    }
    unregister_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
}

// Double tap on Shift toggles Caps Lock -------------------------------------
//
// A tap is a press and release with no other key in between, inside the
// tapping term. Two of them in a row switch Caps Lock on; once it is on, a
// single tap switches it back off, so unlocking never needs the double tap.
static struct {
    uint16_t press_timer; // when the current Shift press started
    uint16_t tap_timer;   // when the last completed tap ended
    bool     held;
    bool     used;        // another key was pressed while Shift was down
    bool     tapped;      // one tap is already waiting for its twin
} sft = {0, 0, false, false, false};

static void shift_tap_toggle_caps(void) {
    // Shift is still registered at this point in the chain: Caps Lock must not
    // reach the host shifted.
    uint8_t saved_mods = get_mods();
    del_mods(MOD_MASK_SHIFT);
    send_keyboard_report();
    tap_code(KC_CAPS);
    set_mods(saved_mods);
    send_keyboard_report();
}

static void shift_double_tap(keyrecord_t *record) {
    if (record->event.pressed) {
        sft.held        = true;
        sft.used        = false;
        sft.press_timer = timer_read();
        return;
    }

    sft.held = false;
    if (sft.used || timer_elapsed(sft.press_timer) >= TAPPING_TERM) {
        sft.tapped = false; // a hold, or a Shift used as a modifier
        return;
    }
    if (host_keyboard_led_state().caps_lock) {
        shift_tap_toggle_caps();
        sft.tapped = false;
        return;
    }
    if (sft.tapped && timer_elapsed(sft.tap_timer) < TAPPING_TERM) {
        shift_tap_toggle_caps();
        sft.tapped = false;
        return;
    }
    sft.tapped    = true;
    sft.tap_timer = timer_read();
}

// Process key presses
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed && sft.held && keycode != KC_LSFT) {
        sft.used = true; // Shift is doing its usual job, not being tapped
    }

    switch (keycode) {
        case KC_LSFT:
            shift_double_tap(record);
            return true; // Shift keeps working as a modifier

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
        case EG_SPC: {
            tap_layer_t *tl = tap_layer_for(keycode);
            if (record->event.pressed) {
                tap_layer_press(tl);
            } else {
                tap_layer_release(tl);
            }
            return false;
        }

        case EG_DLR ... EG_MINS:
        case EG_QUES:
            // Yield to Auto Shift when it is on, so holding reaches the digit.
            // It is consulted later in the chain and would never see these keys
            // otherwise. When it is off, type the glyph here instead.
            if (autoshift_will_run()) {
                return true;
            }
            if (record->event.pressed) {
                const dual_glyph_t *pair = dual_glyph_for(keycode);
                tap_dual_glyph(pair, dual_glyph_wants_shift(pair));
            }
            return false;

        // AltGr+9 and AltGr+2 are dead keys on AZERTY: the trailing space is
        // what makes the accent literal, so one tap types one character.
        case EG_CIRC:
        case EG_TILD:
            if (record->event.pressed) {
                tap_code16(keycode == EG_CIRC ? ALGR(KC_9) : ALGR(KC_2));
                tap_code(KC_SPC);
            }
            return false;

        // Keycap emoji and the two pointing triangles: more code points than a
        // unicode_map entry can hold.
        case EG_KC1 ... EG_ARRR:
            if (record->event.pressed) {
                static const char *const sequences[] = {
                    "1️⃣", "2️⃣", "3️⃣", "4️⃣", "5️⃣",
                    "6️⃣", "7️⃣", "8️⃣", "9️⃣", "0️⃣",
                    "*️⃣", // keycap asterisk
                    "◀️",  // black left-pointing triangle
                    "▶️",  // black right-pointing triangle
                };
                send_unicode_string(sequences[keycode - EG_KC1]);
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
