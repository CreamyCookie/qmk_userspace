// Copyright 2024 Joschua Gandert (@CreamyCookie)

#include QMK_KEYBOARD_H
#include "ducktopus.h"
#include "features/heuristic_tap_hold.h"

#        ifdef VIAL_ENABLE
#include "dynamic_keymap.h"
#        else
#include "keymap_introspection.h"
#        endif

#    ifndef TRACKBALL_NORMAL_DPI
#        define TRACKBALL_NORMAL_DPI 1600
#    endif  // !TRACKBALL_NORMAL_DPI

#    ifndef TRACKBALL_SCROLL_BUFFER_SIZE
#        define TRACKBALL_SCROLL_BUFFER_SIZE 80
#    endif  // !TRACKBALL_SCROLL_BUFFER_SIZE
// WAS 6, then 90

#    ifndef TRACKBALL_SNIPE_BUFFER_SIZE
#        define TRACKBALL_SNIPE_BUFFER_SIZE 5
#    endif  // !TRACKBALL_SNIPE_BUFFER_SIZE

#    ifndef TRACKBALL_POINTER_ACCELERATION_FACTOR
#        define TRACKBALL_POINTER_ACCELERATION_FACTOR 24
#    endif  // !TRACKBALL_POINTER_ACCELERATION_FACTOR

#    ifndef PD_KEY_TAP_ACTION_THRESHOLD
#        define PD_KEY_TAP_ACTION_THRESHOLD 200
#    endif  // !PD_KEY_TAP_ACTION_THRESHOLD

#    ifndef PD_KEY_TAP_MOVEMENT_FULL_DECAY_MS
#        define PD_KEY_TAP_MOVEMENT_FULL_DECAY_MS 1500
#    endif  // !PD_KEY_TAP_MOVEMENT_FULL_DECAY_MS


#    ifndef CONSTRAIN_HID
#        define CONSTRAIN_HID(value) ((value) < XY_REPORT_MIN ? XY_REPORT_MIN : ((value) > XY_REPORT_MAX ? XY_REPORT_MAX : (value)))
#    endif  // !CONSTRAIN_HID

/**
 * \brief Add optional acceleration effect.
 *
 * If `TRACKBALL_ENABLE_POINTER_ACCELERATION` is defined, add a simple and naive
 * acceleration effect to the provided value.  Return the value unchanged
 * otherwise.
 */
#    ifndef DISPLACEMENT_WITH_ACCELERATION
#        ifdef TRACKBALL_POINTER_ACCELERATION_ENABLE
#            define DISPLACEMENT_WITH_ACCELERATION(d) (CONSTRAIN_HID(d > 0 ? d * d / TRACKBALL_POINTER_ACCELERATION_FACTOR + d : -d * d / TRACKBALL_POINTER_ACCELERATION_FACTOR + d))
#        else  // !TRACKBALL_POINTER_ACCELERATION_ENABLE
#            define DISPLACEMENT_WITH_ACCELERATION(d) (d)
#        endif  // TRACKBALL_POINTER_ACCELERATION_ENABLE
#    endif      // !DISPLACEMENT_WITH_ACCELERATION

#define LAYER_GAME 0
#define LAYER_GFUN 1
#define LAYER_MAIN 2
#define LAYER_SYMB 3
#define LAYER_FUNC 4
#define LAYER_ADJU 5
#define LAYER_LMOD 6
#define LAYER_RMOD 7

#define DF_GAME    DF(LAYER_GAME)
#define DF_MAIN    DF(LAYER_MAIN)

#define MO_GAME     MO(LAYER_GAME)
#define MO_GFUN     MO(LAYER_GFUN)
#define MO_MAIN     MO(LAYER_MAIN)
#define MO_FUNC     MO(LAYER_FUNC)
#define MO_SYMB     MO(LAYER_SYMB)
#define MO_ADJU     MO(LAYER_ADJU)

#define D_SS      KC_MINS          // ß
#define D_Y       KC_Z             // Y
#define D_Z       KC_Y             // Z
#define D_UE      KC_LBRC          // Ü
#define D_OE      KC_SCLN          // Ö
#define D_AE      KC_QUOT          // Ä
#define D_HASH    KC_NUHS          // #
#define D_LT      KC_NUBS          // <
#define D_PLUS    KC_RBRC          // +
#define D_COMM    KC_COMMA         // ,
#define D_MINS    KC_SLSH          // -

#define D_ACUT    KC_EQUAL         // ´ (dead)
#define D_CIRC    KC_GRV           // ^ (dead)
#define D_GRV     RSFT(D_ACUT)     // ` (dead)
#define D_BTIK    D_GRV            // same

#define D_DEG     RSFT(D_CIRC)     // °
#define D_EXLM    RSFT(KC_1)       // !
#define D_DQUO    RSFT(KC_2)       // "
#define D_SECT    RSFT(KC_3)       // §
#define D_DOLR    RSFT(KC_4)       // $
#define D_PERC    RSFT(KC_5)       // %
#define D_AMPR    RSFT(KC_6)       // &
#define D_SLSH    RSFT(KC_7)       // /
#define D_LPRN    RSFT(KC_8)       // (
#define D_RPRN    RSFT(KC_9)       // )
#define D_EQL     RSFT(KC_0)       // =
#define D_QUES    RSFT(D_SS)       // ?
#define D_ASTR    RSFT(D_PLUS)     // *
#define D_QUO     RSFT(D_HASH)     // '
#define D_GT      RSFT(D_LT)       // >
#define D_SCLN    RSFT(D_COMM)     // ;
#define D_COLN    RSFT(KC_DOT)     // :
#define D_UNDS    RSFT(D_MINS)     // _
#define D_LCBR    RALT(KC_7)       // {
#define D_LBRC    RALT(KC_8)       // [
#define D_RBRC    RALT(KC_9)       // ]
#define D_RCBR    RALT(KC_0)       // }
#define D_BSLS    RALT(D_SS)       // backslash
#define D_AT      RALT(KC_Q)       // @
#define D_EURO    RALT(KC_E)       // €
#define D_TILD    RALT(D_PLUS)     // ~
#define D_PIPE    RALT(D_LT)       // |
#define D_MICR    RALT(KC_M)       // µ

#define W_PREV    LSA(KC_TAB)      // previous window
#define W_NEXT    LALT(KC_TAB)     // next window
#define W_CLOS    LALT(KC_F4)      // close window

#define T_PREV    C(S(KC_TAB))     // previous tab
#define T_NEXT    LCTL(KC_TAB)     // next tab
#define T_CLOS    LCTL(KC_W)       // close tab

#define L_SAC           LSA(KC_LCTL)
#define L_AS            LALT(KC_LSFT)
#define L_CA            LCTL(KC_LALT)
#define L_SC            LSFT(KC_LCTL)
#define R_SC            RSFT(KC_RCTL)
#define R_CA            RCTL(KC_LALT)
#define R_AS            LALT(KC_RSFT)
#define R_SAC           LSA(KC_RCTL)
#define LT_GFN_ESC      LT(LAYER_GFUN, KC_ESC)
#define LT_SYM_BSP      LT(LAYER_SYMB, KC_BSPC)
#define CAT_I           LCA_T(KC_I)
#define CST_A           C_S_T(KC_A)
#define CT_E            LCTL_T(KC_E)
#define RCT_N           RCTL_T(KC_N)
#define CST_R           C_S_T(KC_R)
#define CAT             LCA_T(KC_T)
#define GT_D_OE         LGUI_T(D_OE)
#define RGT_DOT         RGUI_T(KC_DOT)
#define AT_ESC          LALT_T(KC_ESC)
#define LT_FN_SPA       LT(LAYER_FUNC, KC_SPACE)
#define CT_ENTER        LCTL_T(KC_ENTER)
#define RCT_DEL         RCTL_T(KC_DEL)
#define LT_FN_BSP       LT(LAYER_FUNC, KC_BSPC)

#define _NONE_    KC_NO            // do nothing
#define ______    KC_TRNS          // take from lower layer



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [LAYER_GAME] = LAYOUT_5x6(
KC_ESC, KC_1,    KC_2,    KC_3,     KC_4,       KC_5,                     KC_6,   KC_7,    KC_8,       KC_9,    KC_0,    D_SS,
KC_T,   KC_TAB,  KC_Q,    KC_W,     KC_E,       KC_R,                     D_Z,    KC_U,    KC_WH_U,    KC_O,    KC_P,    KC_LGUI,
KC_G,   KC_LSFT, KC_A,    KC_S,     KC_D,       KC_F,   KC_MUTE, KC_BTN3, KC_H,   KC_BTN1, KC_BTN2,    KC_BTN3, KC_RCTL, KC_RSFT,
KC_B,   KC_LCTL, D_Y,     KC_X,     KC_C,       KC_V,                     KC_N,   KC_M,    KC_WH_D,    KC_DOT,  D_MINS,  KC_RCTL,
                 DF_GAME, KC_LEFT,  KC_RIGHT,                                     KC_UP,   KC_DOWN,    DF_MAIN,

                          KC_SPACE, KC_ENTER,   _NONE_,                   _NONE_, KC_RCTL, LT_SYM_BSP,
                                    LT_GFN_ESC, KC_LCTL
    ),

    [LAYER_GFUN] = LAYOUT_5x6(
______, ______, ______, ______, ______, ______,                   ______, ______, ______, ______, ______, ______,
______, ______, ______, ______, ______, ______,                   ______, ______, ______, ______, ______, ______,
______, KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_MUTE, KC_BTN3, KC_6,   KC_7,   KC_8,   KC_9,   KC_0,   ______,
______, ______, ______, ______, ______, ______,                   ______, ______, ______, ______, ______, ______,
                ______, ______, ______,                                   ______, ______, ______,

                        ______, ______, ______,                   ______, ______, ______,
                                ______, ______
    ),

    [LAYER_MAIN] = LAYOUT_5x6(
_NONE_,  KC_1, KC_2,    KC_3,      KC_4,     KC_5,                     KC_6,   KC_7,    KC_8,      KC_9,    KC_0, KC_BTN1,
KC_TAB,  KC_X, KC_V,    KC_L,      KC_C,     KC_W,                     KC_K,   KC_H,    KC_G,      KC_F,    KC_Q, KC_LGUI,
KC_LSFT, KC_U, CAT_I,   CST_A,     CT_E,     KC_O,   KC_MUTE, KC_BTN3, KC_S,   RCT_N,   CST_R,     CAT,     KC_D, KC_RSFT,
MO_SYMB, D_UE, GT_D_OE, D_AE,      KC_P,     D_Z,                      KC_B,   KC_M,    KC_COMM,   RGT_DOT, KC_J, MO_SYMB,
               D_Y,     KC_LEFT,   KC_RIGHT,                                   KC_UP,   KC_DOWN,   D_SS,

                        LT_FN_SPA, CT_ENTER, _NONE_,                   _NONE_, RCT_DEL, LT_FN_BSP,
                                   AT_ESC,   _NONE_
    ),

    [LAYER_SYMB] = LAYOUT_5x6(
______, ______, ______, ______, ______, ______,                   ______, ______, ______, ______, ______, ______,
______, D_DEG,  D_UNDS, D_LBRC, D_RBRC, D_CIRC,                   D_EXLM, D_LT,   D_GT,   D_EQL,  D_AMPR, ______,
______, D_BSLS, D_SLSH, D_LCBR, D_RCBR, D_ASTR, KC_MUTE, KC_BTN3, D_QUES, D_LPRN, D_RPRN, D_MINS, D_COLN, ______,
______, D_HASH, D_DOLR, D_PIPE, D_AT,   D_BTIK,                   D_PERC, D_PLUS, D_DQUO, D_QUO,  D_SCLN, ______,
                D_TILD, _NONE_, _NONE_,                                   D_MICR, D_EURO, D_GRV,

                        ______, ______, ______,                   ______, ______, ______,
                                ______, ______
    ),

    [LAYER_FUNC] = LAYOUT_5x6(
_NONE_, _NONE_, _NONE_,  _NONE_,        _NONE_,         _NONE_,                    _NONE_,  _NONE_,  _NONE_,  _NONE_,  _NONE_,         _NONE_,
______, KC_F1,  KC_F2,   KC_F3,         KC_F4,          _NONE_,                    _NONE_,  KC_HOME, KC_END,  _NONE_,  _NONE_,         ______,
______, KC_F5,  KC_F6,   KC_F7,         KC_F8,          KC_MPLY, KC_MUTE, KC_BTN3, KC_PGUP, KC_BTN1, KC_BTN2, KC_BTN3, LALT(KC_ENTER), ______,
______, KC_F9,  KC_F10,  KC_F11,        KC_F12,         KC_MNXT,                   KC_PGDN, W_PREV,  W_NEXT,  W_CLOS,  _NONE_,         ______,
                DF_GAME, LALT(KC_LEFT), LALT(KC_RIGHT),                                     T_PREV,  T_NEXT,  T_CLOS,

                         ______,        ______,         ______,                    ______,  ______,  ______,
                                        ______,         ______
    ),

    [LAYER_ADJU] = LAYOUT_5x6(
______, ______, ______, ______, ______, ______,                    QK_BOOT, ______, ______,       ______,     ______, ______,
______, ______, ______, ______, ______, ______,                    ______,  ______, ______,       ______,     ______, ______,
______, ______, ______, ______, ______, ______,  KC_MUTE, KC_BTN3, ______,  ______, LSA(KC_DOWN), LSA(KC_UP), ______, ______,
______, ______, ______, ______, ______, ______,                    ______,  ______, ______,       ______,     ______, ______,
                ______, ______, ______,                                     ______, ______,       ______,

                        ______, ______, QK_BOOT,                   ______,  ______, ______,
                                ______, ______
    ),

    [LAYER_LMOD] = LAYOUT_5x6(
______, _NONE_, _NONE_,  _NONE_, _NONE_,  _NONE_,                   ______, ______, ______, ______, ______, ______,
_NONE_, _NONE_, _NONE_,  _NONE_, _NONE_,  _NONE_,                   ______, ______, ______, ______, ______, ______,
______, L_AS,   L_CA,    L_SC,   KC_LCTL, _NONE_, KC_MUTE, KC_BTN3, ______, ______, ______, ______, ______, ______,
______, L_SAC,  KC_LGUI, _NONE_, KC_LALT, _NONE_,                   ______, ______, ______, ______, ______, ______,
                _NONE_,  _NONE_, _NONE_,                                    ______, ______, ______,

                         ______, ______,  ______,                   ______, ______, ______,
                                 ______,  ______
    ),

    [LAYER_RMOD] = LAYOUT_5x6(
______, ______, ______, ______, ______, ______,                   _NONE_, _NONE_,  _NONE_, _NONE_,  _NONE_, ______,
______, ______, ______, ______, ______, ______,                   _NONE_, _NONE_,  _NONE_, _NONE_,  _NONE_, _NONE_,
______, ______, ______, ______, ______, ______, KC_MUTE, KC_BTN3, _NONE_, KC_RCTL, R_SC,   R_CA,    R_AS,   ______,
______, ______, ______, ______, ______, ______,                   _NONE_, KC_LALT, _NONE_, KC_RGUI, R_SAC,  ______,
                ______, ______, ______,                                   _NONE_,  _NONE_, _NONE_,

                        ______, ______, ______,                   ______, ______,  ______,
                                ______, ______
    ),

};


#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [LAYER_GAME] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP) },
    [LAYER_GFUN] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP) },
    [LAYER_MAIN] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP) },
    [LAYER_FUNC] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP) },
    [LAYER_SYMB] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP) },
    [LAYER_ADJU] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP) },
    [LAYER_LMOD] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP) },
    [LAYER_RMOD] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP) },
};
#endif


// save space
#ifndef MAGIC_ENABLE
uint16_t keycode_config(uint16_t keycode) {
    return keycode;
}

uint8_t mod_config(uint8_t mod) {
    return mod;
}
#endif

static uint8_t cur_mod_layer;

static uint8_t get_all_mods(void) {
#        ifdef NO_ACTION_ONESHOT
    return get_mods(); //| get_weak_mods();
#        else
    return get_mods() | get_oneshot_mods(); //| get_weak_mods();
#        endif  // NO_ACTION_ONESHOT
}

/*
static void clear_all_mods(void) {
    clear_mods();
#        ifndef NO_ACTION_ONESHOT
    clear_oneshot_mods();
#        endif  // !NO_ACTION_ONESHOT
}
*/


static bool is_only_modifier_pressed(uint16_t mod_keycode_left, uint16_t mod_keycode_right) {
    const uint8_t mods = get_all_mods();
	return (mods == MOD_BIT(mod_keycode_left)) || (mods == MOD_BIT(mod_keycode_right));
}

/*
static bool is_only_shift_modifier_pressed(void) {
	return is_only_modifier_pressed(KC_LSFT, KC_RSFT);
}
*/

static bool is_only_alt_modifier_pressed(void) {
	return is_only_modifier_pressed(KC_LALT, KC_RALT);
}

/*
static bool has_shift_mod(void) {
    return is_modifier_held();
}

static bool is_modifier_held(void) {
#        ifdef NO_ACTION_ONESHOT
    return mod_config(get_mods()) & MOD_MASK_SHIFT;
#        else
    return mod_config(get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
#        endif  // NO_ACTION_ONESHOT
}
*/

// thanks to u/ideasman_42
#        ifdef NO_ACTION_ONESHOT
#define WITHOUT_MODS(...) \
    do { \
        const uint8_t _real_mods = get_mods(); \
        clear_all_mods(); \
        { __VA_ARGS__ } \
        set_mods(_real_mods); \
    } while (0)
#        else
#define WITHOUT_MODS(...) \
    do { \
        const uint8_t _real_mods = get_mods(); \
        const uint8_t _real_oneshot_mods = get_oneshot_mods(); \
        clear_all_mods(); \
        { __VA_ARGS__ } \
        set_mods(_real_mods); \
        set_oneshot_mods(_real_oneshot_mods); \
    } while (0)
#        endif  // NO_ACTION_ONESHOT

/*
can be used like this
WITHOUT_MODS({
    SEND_STRING(SS_TAP(X_LEFT));
});
*/



static uint16_t get_keycode_same_pos_in_layer(keyrecord_t* record, uint8_t layer) {
        // thanks to u/pgetreuer for his help here
        const keypos_t pos = record->event.key;

#        ifdef VIAL_ENABLE
        return dynamic_keymap_get_keycode(layer, pos.row, pos.col);
#        else
        return keycode_at_keymap_location(layer, pos.row, pos.col);
#        endif
}




//=============================================================================
// Alt Tab


//=============================================================================

static bool alt_tab_was_started = false;


bool process_alt_tab(uint16_t keycode, keyrecord_t* record) {
    const bool is_alt_tab_key = keycode == LALT(KC_TAB) || keycode == LSA(KC_TAB);

    if (!is_alt_tab_key) {
        if (alt_tab_was_started && keycode != KC_BTN1) {
            unregister_mods(MOD_BIT(KC_LALT));
            alt_tab_was_started = false;
        }
        return true;
    }

    if (!IS_LAYER_ON(LAYER_FUNC)) {
        return true;
    }

    if (!alt_tab_was_started) {
        register_mods(MOD_BIT(KC_LALT));
        send_keyboard_report();
        alt_tab_was_started = true;
    }

    if (record->event.pressed) {
        if (keycode == LALT(KC_TAB)) {
            tap_code16(KC_TAB);
        } else if (keycode == LSA(KC_TAB)) {
            tap_code16(LSFT(KC_TAB));
        }
    }
    return false;
}


static bool is_layer_switch_keycode(uint16_t keycode) {
    switch (keycode) {
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
        case QK_LAYER_MOD ... QK_LAYER_MOD_MAX:
        case QK_TO ... QK_TO_MAX:
        case QK_MOMENTARY ... QK_MOMENTARY_MAX:
        case QK_DEF_LAYER ... QK_DEF_LAYER_MAX:
        case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
        case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX:
        case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX:
            return true;
    }
    return false;
}


static bool will_switch_layer(uint16_t keycode, keyrecord_t* record) {
    // if pressed layer switch key and (not LT or a held LT)
    return record->event.pressed && is_layer_switch_keycode(keycode) &&
           (!IS_QK_LAYER_TAP(keycode) || record->tap.count == 0);
}


void process_layer_change_so_same_side_with_mod_layer(uint16_t keycode, keyrecord_t* record) {
    if (will_switch_layer(keycode, record)) {
        cur_mod_layer = is_on_left_hand(record) ? LAYER_LMOD : LAYER_RMOD;
    } else {
        cur_mod_layer = 0;
    }
}


bool process_space_to_underscore_for_caps_words(uint16_t keycode, keyrecord_t* record) {
    if (!is_caps_word_on() || !record->event.pressed) return true;

    const uint16_t tap_keycode = get_tap_keycode(keycode);
    if (tap_keycode != KC_SPACE) return true;

    // is a held tap hold
    if (tap_keycode != keycode && record->tap.count == 0) return true;

    tap_code16(D_UNDS);
    return false;
}


bool process_record_user(uint16_t keycode, keyrecord_t* record) {
#        if !defined(NO_ACTION_TAPPING)
    if (!process_heuristic_tap_hold(keycode, record)) {
        return false;
    }
#        endif // !NO_ACTION_TAPPING

    if (!process_alt_tab(keycode, record)) {
        return false;
    }

    process_layer_change_so_same_side_with_mod_layer(keycode, record);

#        ifdef CAPS_WORD_ENABLE
    if (!process_space_to_underscore_for_caps_words(keycode, record)) {
        return false;
    }
#        endif

    // true = the key press should continue to be processed as normal
    return true;
}


void matrix_scan_user(void) {
#        if !defined(NO_ACTION_TAPPING)
    heuristic_tap_hold_task();
#        endif // !NO_ACTION_TAPPING
}


tap_hold_decision_options choose_when_next_to_heuristic_tap_hold_on_same_side(
        keyrecord_t* record, uint16_t keycode, bool is_left) {
    // this special case is mostly for the LMOD and RMOD layers
    // see update_mod_layer_state
    // see process_layer_change_so_same_side_with_mod_layer

    if (IS_LAYER_OFF(LAYER_GAME) && IS_QK_LAYER_TAP(get_heuristic_tap_hold_keycode())) {
        const uint8_t mod_layer = is_left ? LAYER_LMOD : LAYER_RMOD;

        if (!IS_INTERNAL_KEYCODE(get_keycode_same_pos_in_layer(record, mod_layer))) {
            // is LT and we pressed something that is not KC_NO or KC_TRNS in the MOD layer
            return CHOSE_HOLD;
        }
    }

    return CHOSE_TAP;
}


bool should_choose_tap_when_pressed_very_long_without_another_key(void) {
    switch (get_tap_keycode(get_heuristic_tap_hold_keycode())) {
        case KC_BSPC:
            // if we return true here, the ADJU layer would not be reachable from the right
            return !IS_LAYER_ON(LAYER_SYMB);
        case KC_ENTER:
            return prev_chose_tap_and_was_same_tap_hold();
    }
    return false; // disabled for me, since it often leads to space not working as hold
}


bool should_hold_instantly() {
    // this is only a good idea for hold keys that are pressed alone
    switch (get_heuristic_tap_hold_keycode()) {
        case AT_ESC:
        case LT_GFN_ESC:
            return true;
    }
    return false;
}


/*
static uint16_t left_dpi = 0;
static uint16_t right_dpi = 0;

static void update_dpi(uint16_t dpi) {
    if (is_keyboard_left()) {
        if (left_dpi != dpi) {
            pointing_device_set_cpi_on_side(true, dpi);
            pointing_device_set_cpi_on_side(false, dpi);
        }
    } else {
        if (right_dpi != dpi) {
            pointing_device_set_cpi_on_side(true, dpi);
            pointing_device_set_cpi(dpi);
        }
    }
}

void pointing_device_init_user(void) {
}
*/

// ----------------------------------------------------------------------------
// SWITCH
static uint16_t pd_key_tap_timer = 0;

static int16_t pd_accumulated_x = 0;
static int16_t pd_accumulated_y = 0;

static void reset_pd_switch(void) {
	pd_accumulated_x = 0;
	pd_accumulated_y = 0;
	pd_key_tap_timer = 0;
}

static void handle_pd_switch(report_mouse_t* mouse_report) {
    // after a certain duration, we want to remove 100% of previous inputs
    // we're multiplying by 100 here, as we want to emulate floating point arithmetic (x*100)/(y*100) = x/y
    int16_t removal_percentage = timer_elapsed(pd_key_tap_timer) * 1000 / PD_KEY_TAP_MOVEMENT_FULL_DECAY_MS;
    pd_key_tap_timer = timer_read();

    if (removal_percentage > 100) {
        removal_percentage = 100;
    }

    // to avoid small creeping changes resulting in a keypress, we reduce previous input
    pd_accumulated_x = mouse_report->x * 10 + (pd_accumulated_x - (pd_accumulated_x * removal_percentage)/100);
    pd_accumulated_y = mouse_report->y * 10 + (pd_accumulated_y - (pd_accumulated_y * removal_percentage)/100);

    const int16_t abs_ax = abs(pd_accumulated_x);
    const int16_t abs_ay = abs(pd_accumulated_y);
    const uint16_t pd_threshold = PD_KEY_TAP_ACTION_THRESHOLD * 10;

    // x and y have to be different enough (i.e. there's a diagonal dead zone)
    if (abs(abs_ax - abs_ay) > pd_threshold) {
        if (abs_ax > abs_ay) {
            if (pd_accumulated_x > 0) {
                tap_code(KC_TAB);
                pd_accumulated_x -= pd_threshold;
            } else {
                register_code(KC_LSFT);
                tap_code(KC_TAB);
                unregister_code(KC_LSFT);
                pd_accumulated_x += pd_threshold;
            }
            pd_accumulated_y = 0;
        } else {
            if (pd_accumulated_y > 0) {
                pd_accumulated_y -= pd_threshold;
                tap_code(KC_DOWN);
            } else {
                pd_accumulated_y += pd_threshold;
                tap_code(KC_UP);
            }
            pd_accumulated_x = 0;
        }
    }

    // don't move the mouse
    mouse_report->x = 0;
    mouse_report->y = 0;
}

// ----------------------------------------------------------------------------
// SCROLLING
static int16_t scroll_buffer_x = 0;
static int16_t scroll_buffer_y = 0;

static void reset_pd_scroll(void) {
	scroll_buffer_x = 0;
	scroll_buffer_y = 0;
}

static void handle_pd_scroll(report_mouse_t* mouse_report) {
    scroll_buffer_x += mouse_report->x;
    scroll_buffer_y -= mouse_report->y;
    mouse_report->x = 0;
    mouse_report->y = 0;
    if (abs(scroll_buffer_x) >= TRACKBALL_SCROLL_BUFFER_SIZE) {
        mouse_report->h = scroll_buffer_x > 0 ? 1 : -1;
        scroll_buffer_x = scroll_buffer_x > 0 ? scroll_buffer_x - TRACKBALL_SCROLL_BUFFER_SIZE : scroll_buffer_x + TRACKBALL_SCROLL_BUFFER_SIZE;
    }
    if (abs(scroll_buffer_y) >= TRACKBALL_SCROLL_BUFFER_SIZE) {
        mouse_report->v = scroll_buffer_y > 0 ? 1 : -1;
        scroll_buffer_y = scroll_buffer_y > 0 ? scroll_buffer_y - TRACKBALL_SCROLL_BUFFER_SIZE : scroll_buffer_y + TRACKBALL_SCROLL_BUFFER_SIZE;
    }
}

// ----------------------------------------------------------------------------
// SNIPING
static int16_t snipe_buffer_x = 0;
static int16_t snipe_buffer_y = 0;

static void reset_pd_snipe(void) {
	snipe_buffer_x = 0;
	snipe_buffer_y = 0;
}

static void handle_pd_snipe(report_mouse_t* mouse_report) {
    snipe_buffer_x += mouse_report->x;
    snipe_buffer_y += mouse_report->y;
    mouse_report->x = 0;
    mouse_report->y = 0;
    if (abs(snipe_buffer_x) > TRACKBALL_SNIPE_BUFFER_SIZE) {
        mouse_report->x = snipe_buffer_x > 0 ? 1 : -1;
        snipe_buffer_x = snipe_buffer_x > 0 ? snipe_buffer_x - TRACKBALL_SNIPE_BUFFER_SIZE : snipe_buffer_x + TRACKBALL_SNIPE_BUFFER_SIZE;
    }
    if (abs(snipe_buffer_y) > TRACKBALL_SNIPE_BUFFER_SIZE) {
        mouse_report->y = snipe_buffer_y > 0 ? 1 : -1;
        snipe_buffer_y = snipe_buffer_y > 0 ? snipe_buffer_y - TRACKBALL_SNIPE_BUFFER_SIZE : snipe_buffer_y + TRACKBALL_SNIPE_BUFFER_SIZE;
    }
}

static void pointing_device_task_trackball(report_mouse_t* mouse_report) {
	bool is_pseudo_main_layer = IS_LAYER_ON(LAYER_MAIN) || IS_LAYER_ON(LAYER_GAME);

	if (is_only_alt_modifier_pressed() && is_pseudo_main_layer) {
	    handle_pd_switch(mouse_report);
		return;
	} else {
		reset_pd_switch();

		if (IS_LAYER_ON(LAYER_ADJU)) {
		    handle_pd_snipe(mouse_report);
            return;
        } else {
            reset_pd_snipe();

             if (IS_LAYER_ON(LAYER_SYMB) || IS_LAYER_ON(LAYER_GFUN)) {
                handle_pd_scroll(mouse_report);
                return;
            } else {
                reset_pd_scroll();

                mouse_report->x = DISPLACEMENT_WITH_ACCELERATION(mouse_report->x);
                mouse_report->y = DISPLACEMENT_WITH_ACCELERATION(mouse_report->y);
            }
        }
	}
}


report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (is_keyboard_master()) {
        pointing_device_task_trackball(&mouse_report);
    }
    return mouse_report;
}


// when using a layer switch key on the left side, we want the left mod layer
// to activate, so that it is easy to press, for example, ctrl + shift + f7
static layer_state_t update_mod_layer_state(layer_state_t state) {
    // turn off both mod layers
    state &= ~(((layer_state_t)1 << LAYER_LMOD) | ((layer_state_t)1 << LAYER_RMOD));

    const uint8_t layer = get_highest_layer(state);
    if (cur_mod_layer != 0 && layer > LAYER_MAIN && layer != LAYER_ADJU) {
        // turn on the one we want
        return state | ((layer_state_t)1 << cur_mod_layer);
    }
    return state;
}


// called every time the layer changes
layer_state_t layer_state_set_user(layer_state_t state) {
    state = update_tri_layer_state(state, LAYER_FUNC, LAYER_SYMB, LAYER_ADJU);
    state = update_mod_layer_state(state);

    if (!layer_state_cmp(state, LAYER_FUNC) && alt_tab_was_started) {
       unregister_mods(MOD_BIT(KC_LALT));
       alt_tab_was_started = false;
    }
    return state;
}

void keyboard_post_init_user(void) {
    default_layer_set(1UL << LAYER_MAIN);
    //pointing_device_set_cpi(TRACKBALL_NORMAL_DPI);
#ifdef CONSOLE_ENABLE
    debug_enable=true;
    debug_matrix=true;
    debug_keyboard=true;
    debug_mouse=true;
#else
    debug_enable=false;
    debug_matrix=false;
    debug_keyboard=false;
    debug_mouse=false;
#endif
}


/*
static void turn_on_layer_in_state(layer_state_t state, uint8_t layer) {
    state = state | ((layer_state_t)1 << layer);
}


static void turn_off_layer_in_state(layer_state_t state, uint8_t layer) {
    state = state & ~((layer_state_t)1 << layer);
}


inline static uint8_t extract_8_bit_mods_from_mod_tap(uint16_t keycode) {
    const uint8_t mods = mod_config(QK_MOD_TAP_GET_MODS(keycode));
    // if 5th bit is set, it is a right-side modifier, so << 4 (discarding the bit via overflow)
    return (mods & 0b10000) ? mods << 4 : mods;
}


static void register_hold(void) {
    if (IS_QK_LAYER_TAP(heuristic_tap_hold_keycode)) {
        uint8_t layer = QK_LAYER_TAP_GET_LAYER(heuristic_tap_hold_keycode);
        layer_on(layer);
    } else { // mod tap
        uint8_t mods = extract_8_bit_mods_from_mod_tap(heuristic_tap_hold_keycode);
        register_mods(mods);
    }
}


static void unregister_hold(void) {
    if (IS_QK_LAYER_TAP(heuristic_tap_hold_keycode)) {
        uint8_t layer = QK_LAYER_TAP_GET_LAYER(heuristic_tap_hold_keycode);
        layer_off(layer);
    } else { // mod tap
        uint8_t mods = extract_8_bit_mods_from_mod_tap(heuristic_tap_hold_keycode);
        unregister_mods(mods);
    }
}


static uint16_t get_keycode_in_current_layer_at(uint8_t col, uint8_t row) {
    // thanks to /u/Pimentoso for finding out how to do this
    // create fake event with the correct position
    const keyevent_t ev = (keyevent_t){ .key = (keypos_t){.row = col, .col = row}, .pressed = true, .time = (timer_read() | 1)};

    // ask QMK what the keycode for that event is
    return get_event_keycode(ev, false);
}


static void create_and_process_new_event(uint8_t col, uint8_t row, bool is_held, bool is_pressed) {
    keyrecord_t registered_record = (keyrecord_t){
#ifndef NO_ACTION_TAPPING
        .tap.interrupted = false,
        .tap.count       = is_held ? 0 : 1,
#endif
        .event = (keyevent_t){
            .type = KEY_EVENT,
            .key = (keypos_t){.row = row, .col = col},
            .pressed = is_pressed ? 1 : 0,
            .time = (timer_read() | 1), // time should not be 0
        },
    };

    is_processing_record = true;
    process_record(&registered_record);
    is_processing_record = false;
}
*/
