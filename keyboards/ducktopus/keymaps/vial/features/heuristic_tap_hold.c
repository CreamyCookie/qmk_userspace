// Copyright 2024 Joschua Gandert (@CreamyCookie)

#        if !defined(NO_ACTION_TAPPING)

#include "heuristic_tap_hold.h"


// The value here was chosen, because most zero overlap non-mod presses
// in the data set have shorter durations.
#define MS_MAX_OVERLAP              358
#define MS_MAX_DUR                32767



static tap_hold_decision_options tap_hold_decision = UNDECIDED;
static bool is_processing_record_due_to_us = false;
static uint16_t ms_min_overlap_for_hold_estimate = 0;

static bool ms_between_prev_release_and_this_press_was_set = false;

static uint16_t ms_between_prev_release_and_this_press = 0;
static int16_t ms_between_prev_release_and_heuristic_tap_hold_press = 0;

static uint16_t ms_prev_release_timer = 0;
static uint16_t ms_heuristic_tap_hold_press_timer = 0;
static uint16_t ms_overlap_timer = 0;
static uint16_t ms_next_to_heuristic_tap_hold_press_to_release_timer = 0;

static bool prev_was_mod = false;
static bool prev_to_heuristic_tap_hold_was_mod = false;
static bool heuristic_tap_hold_is_on_left = false;

static uint16_t heuristic_tap_hold_keycode = KC_NO;
static keyrecord_t heuristic_tap_hold_record;
static uint8_t heuristic_tap_hold_mods = 0;
static bool heuristic_tap_hold_keycode_was_held_instantly = false;

static uint16_t next_to_heuristic_tap_hold_keycode = KC_NO;
static keyrecord_t next_to_heuristic_tap_hold_record;
static uint8_t next_to_heuristic_tap_hold_mods = 0;
static uint16_t ms_between_heuristic_tap_hold_press_and_next_press = 0;

static uint16_t prev_heuristic_tap_hold_keycode = KC_NO;
static tap_hold_decision_options prev_tap_hold_decision = UNDECIDED;


static void reset_heuristic_tap_hold(void) {
    tap_hold_decision = UNDECIDED;
    ms_min_overlap_for_hold_estimate = 0;
    ms_between_prev_release_and_this_press_was_set = false;

    heuristic_tap_hold_keycode = KC_NO;
    heuristic_tap_hold_record = (keyrecord_t){0};
    heuristic_tap_hold_mods = 0;
    heuristic_tap_hold_keycode_was_held_instantly = false;

    next_to_heuristic_tap_hold_keycode = KC_NO;
    next_to_heuristic_tap_hold_record = (keyrecord_t){0};
    next_to_heuristic_tap_hold_mods = 0;
}


static uint16_t calculate_min_overlap_for_hold_in_ms(void) {
    // a MIN(MS_MAX_DUR is not necessary here due to the tap hold task
    const float prev_up_th_down_dur = (float) ms_between_prev_release_and_heuristic_tap_hold_press;

    const float th_down_next_down_dur = (float) ms_between_heuristic_tap_hold_press_and_next_press;

    const float guess = ABS(MAX(
            -prev_up_th_down_dur,
            MAX(
                3.0614197f,
                MAX(
                    1386.7545166f,
                    -136.1621093f * prev_up_th_down_dur - 315.5284118f
                ) - MAX(prev_up_th_down_dur, 325.5094909f) -
                    6.4232006f * th_down_next_down_dur +
                    302.9532165f
            )
    ));

    return MIN(MS_MAX_OVERLAP, MAX(1, (uint16_t) guess));
}


__attribute__((weak)) bool should_choose_hold_when_next_to_heuristic_tap_hold_is_wrapped(void) {
    const float next_dur = (float) timer_elapsed(
            ms_next_to_heuristic_tap_hold_press_to_release_timer);

    const float prev_up_th_down_dur = (float) ms_between_prev_release_and_heuristic_tap_hold_press;
    const float th_down_next_down_dur = (float) ms_between_heuristic_tap_hold_press_and_next_press;

    const float th_down_next_down_dur_reciprocal = SD(1.0f, th_down_next_down_dur);

    const float non_zero_divisor = MAX(
             1.1125613f + 558.6079711f * th_down_next_down_dur_reciprocal,
             MAX(
                 110.8752517f,
                 MAX(1.1125613f, -5.7630343f * prev_up_th_down_dur - 184.2279510f) * -prev_up_th_down_dur
             ) + SD(4170.0205078f, next_dur) - 179.2697753f
     );

    const float guess = MAX(prev_up_th_down_dur * 0.6423792f, 23.4521789f) / (
            542.2182617f * th_down_next_down_dur_reciprocal * non_zero_divisor
    );

    return guess > 0.5f || guess < -0.5f;
}

__attribute__((weak)) bool should_choose_hold_when_two_down_after_heuristic_tap_hold(void) {
    const float prev_up_th_down_dur = (float) ms_between_prev_release_and_heuristic_tap_hold_press;
    const float th_down_next_down_dur = (float) ms_between_heuristic_tap_hold_press_and_next_press;
    const float prev_is_mod = (float) prev_to_heuristic_tap_hold_was_mod;

    const float guess = (
            ABS(-0.0297553f * prev_up_th_down_dur - 9.2836914f) +
                (0.2559899f + prev_is_mod * (0.0180325f * MAX(-prev_up_th_down_dur, 17.5247516f))) *
                th_down_next_down_dur
    ) / (MAX(-prev_up_th_down_dur, 14.0228700f) * -9.2638397f);

    return guess > 0.5f || guess < -0.5f;
}


uint16_t get_tap_keycode(uint16_t keycode) {
    if (IS_QK_MOD_TAP(keycode)) {
        keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
    } else if (IS_QK_LAYER_TAP(keycode)) {
        keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    }
    return keycode;
}


bool prev_chose_tap_and_was_same_tap_hold(void) {
    return (
           prev_tap_hold_decision == CHOSE_TAP &&
           prev_heuristic_tap_hold_keycode == heuristic_tap_hold_keycode
   );
}


uint16_t get_heuristic_tap_hold_keycode(void) {
    return heuristic_tap_hold_keycode;
}


__attribute__((weak)) bool should_choose_tap_when_pressed_very_long_without_another_key(void) {
    return prev_chose_tap_and_was_same_tap_hold();
}


__attribute__((weak)) tap_hold_decision_options choose_when_next_to_heuristic_tap_hold_on_same_side(
        keyrecord_t* record, uint16_t keycode, bool is_left) {
    // by default, we only allow hold if the next key is on the other side
    return CHOSE_TAP;
}


__attribute__((weak)) bool should_hold_instantly(void) {
    return false;
}


// thanks to u/pgetreuer
bool is_on_left_hand(keyrecord_t* record) {
    keypos_t pos = record->event.key;

#        ifdef SPLIT_KEYBOARD
    return pos.row < MATRIX_ROWS / 2;
#        else
    return (MATRIX_COLS > MATRIX_ROWS) ? pos.col < MATRIX_COLS / 2 : pos.row < MATRIX_ROWS / 2;
#        endif
}


static void process_record_with_new_time(keyrecord_t* record) {
    record->event.time = (timer_read() | 1); // time must not be 0

    is_processing_record_due_to_us = true;
    process_record(record);
    is_processing_record_due_to_us = false;
}


// this is similar to tap_code16
/*
static void process_tap_record(keyrecord_t* record, bool is_tap_hold) {
    if (is_tap_hold) {
        record->tap.interrupted = true;
        record->tap.count = 1;
    }

    record->event.pressed = true;
    process_record_with_new_time(record);

    send_keyboard_report();
#        if TAP_CODE_DELAY > 0
        wait_ms(TAP_CODE_DELAY);
#        endif

    record->event.pressed = false;
    process_record_with_new_time(record);
}
*/


static void process_register_record(keyrecord_t* record) {
    record->event.pressed = true;
    process_record_with_new_time(record);
}


static void process_register_record_as_hold(keyrecord_t* record) {
    record->tap.count = 0;
    record->event.pressed = true;
    process_record_with_new_time(record);
}


static void process_unregister_record_as_hold(keyrecord_t* record) {
    record->tap.count = 0;
    record->event.pressed = false;
    process_record_with_new_time(record);
}


static void process_register_record_as_tap(keyrecord_t* record) {
    record->tap.interrupted = true;
    record->tap.count = 1;
    record->event.pressed = true;
    process_record_with_new_time(record);
}


static void process_unregister_record_as_tap(keyrecord_t* record) {
    record->tap.interrupted = true;
    record->tap.count = 1;
    record->event.pressed = false;
    process_record_with_new_time(record);
}


#define IS_TAP_HOLD_KEYCODE(kc) (IS_QK_MOD_TAP(kc) || IS_QK_LAYER_TAP(kc))


static void choose_heuristic_hold(void) {
    tap_hold_decision = CHOSE_HOLD;
    if (!heuristic_tap_hold_keycode_was_held_instantly) {
        process_register_record_as_hold(& heuristic_tap_hold_record);
    }
    if (next_to_heuristic_tap_hold_keycode == KC_NO) return;

    const bool is_tap_hold = IS_TAP_HOLD_KEYCODE(next_to_heuristic_tap_hold_keycode);
    if (is_tap_hold) {
        process_register_record_as_tap(& next_to_heuristic_tap_hold_record);
    } else {
        process_register_record(& next_to_heuristic_tap_hold_record);
    }

    // in many situations the key registered here, will directly be released,
    // so possibly wait, so that to the OS the key seems to have been pressed
    // for at least a millisecond (otherwise OSes / apps might ignore presses)
    send_keyboard_report();
#        if TAP_CODE_DELAY > 0
    wait_ms(TAP_CODE_DELAY);
#        endif
}


#define WITH_TEMP_MODS_ADDED(mods, ...) \
    do { \
        const uint8_t _real_mods = get_mods(); \
        add_mods(mods); \
        { __VA_ARGS__ } \
        set_mods(_real_mods); \
    } while (0)


#define WITH_TEMP_MODS(mods, ...) \
    do { \
        const uint8_t _real_mods = get_mods(); \
        set_mods(mods); \
        { __VA_ARGS__ } \
        set_mods(_real_mods); \
    } while (0)


static void choose_heuristic_tap(void) {
    if (heuristic_tap_hold_keycode_was_held_instantly) {
        // to nullify modifiers acting on their own (e.g. ALT)
        tap_code16(KC_F24);
        process_unregister_record_as_hold(& heuristic_tap_hold_record);
        send_keyboard_report();
    }

    tap_hold_decision = CHOSE_TAP;
    WITH_TEMP_MODS_ADDED(heuristic_tap_hold_mods, {
        process_register_record_as_tap(& heuristic_tap_hold_record);
    });
    send_keyboard_report();

    if (next_to_heuristic_tap_hold_keycode == KC_NO) {
        // we want to only wait once, if possible
#        if TAP_CODE_DELAY > 0
        wait_ms(TAP_CODE_DELAY);
#        endif
        return;
    }

    const bool is_tap_hold = IS_TAP_HOLD_KEYCODE(next_to_heuristic_tap_hold_keycode);
    WITH_TEMP_MODS_ADDED(next_to_heuristic_tap_hold_mods, {
        if (is_tap_hold) {
            process_register_record_as_tap(& next_to_heuristic_tap_hold_record);
        } else {
            process_register_record(& next_to_heuristic_tap_hold_record);
        }
    });

    send_keyboard_report();
#        if TAP_CODE_DELAY > 0
    wait_ms(TAP_CODE_DELAY);
#        endif
}


static int has_next_key_and_it_was_held_longer_than_estimate(void) {
    return (next_to_heuristic_tap_hold_keycode != KC_NO &&
            timer_elapsed(ms_overlap_timer) > ms_min_overlap_for_hold_estimate);
}


static void finish_heuristic_tap_hold(void) {
    if (tap_hold_decision == UNDECIDED) {
        if (has_next_key_and_it_was_held_longer_than_estimate()) {
            choose_heuristic_hold();
        } else {
            choose_heuristic_tap();
        }
    }

    if (tap_hold_decision == CHOSE_TAP) {
        process_unregister_record_as_tap(& heuristic_tap_hold_record);
        // prev_was_mod is already correct in this case (true if MODIFIER)
    } else if (tap_hold_decision == CHOSE_HOLD) {
        // other keys are unregistered whenever the user actually releases the key
        process_unregister_record_as_hold(& heuristic_tap_hold_record);
        // have to update this, because when it was first set, we hadn't made a choice yet
        prev_was_mod = true;
    }

    prev_heuristic_tap_hold_keycode = heuristic_tap_hold_keycode;
    prev_tap_hold_decision = tap_hold_decision;
    reset_heuristic_tap_hold();
}


bool process_heuristic_tap_hold(uint16_t keycode, keyrecord_t *record) {
    if (is_processing_record_due_to_us || !IS_KEYEVENT(record->event)) return true;

    const bool is_pressed = record->event.pressed;

    if (is_pressed && keycode != prev_heuristic_tap_hold_keycode) {
        // We want this to always be reset on any key press that is not the
        // current tap hold key. That way, we can detect when a tap hold key
        // was first tapped and then held (long enough).
        //
        // See should_choose_tap_when_pressed_very_long_without_another_key.
        // If it returns true, we will hold the tap part of the tap hold key,
        // instead of the hold part.
        prev_heuristic_tap_hold_keycode = KC_NO;
        prev_tap_hold_decision = UNDECIDED;
    }

#        ifdef TAP_DANCE_ENABLE
    if (IS_QK_TAP_DANCE(keycode)) return true;
#        endif

#        ifdef COMBO_ENABLE
    if (IS_COMBOEVENT(record->event)) return true;
#        endif

    const bool is_mod_tap = IS_QK_MOD_TAP(keycode);
    const bool is_layer_tap = IS_QK_LAYER_TAP(keycode);
    const bool is_tap_hold = is_mod_tap || is_layer_tap;

    uint16_t tap_of_keycode_or_keycode = keycode;
    if (is_mod_tap) {
        tap_of_keycode_or_keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
    } else if (is_layer_tap) {
        tap_of_keycode_or_keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    }

    if (!is_pressed) {
        // released - we set these now, so we don't have to do it every time we return
        ms_prev_release_timer = timer_read();

        // two cases:
        // 1. tap hold 1 up, tap hold 2 down
        // 2. tap hold 2 down, tap hold 1 up -> currently not supported (tap hold 2 will be tap)
        const bool prev_held = prev_tap_hold_decision == CHOSE_HOLD;
        const bool is_hold = is_tap_hold && (prev_held || record->tap.count == 0);
        prev_was_mod = is_hold || IS_MODIFIER_KEYCODE(tap_of_keycode_or_keycode);

        ms_between_prev_release_and_this_press = 0;
        ms_between_prev_release_and_this_press_was_set = false;
    }

    bool heuristic_tap_hold_found = heuristic_tap_hold_keycode != KC_NO;

    // While the heuristic tap hold is pressed, all other tap holds presses and
    // releases must become taps. See Limitation 1.
    if (!is_pressed && is_tap_hold &&
            (!heuristic_tap_hold_found ||
                (keycode != heuristic_tap_hold_keycode && keycode != next_to_heuristic_tap_hold_keycode))
    ) {
        process_unregister_record_as_tap(record);
        return false;
    }

    if (is_pressed && is_tap_hold && !heuristic_tap_hold_found) {
        // new heuristic tap hold is starting
        ms_heuristic_tap_hold_press_timer = timer_read();

        heuristic_tap_hold_keycode = keycode;
        heuristic_tap_hold_record = * record;
        heuristic_tap_hold_mods = get_mods();
        heuristic_tap_hold_is_on_left = is_on_left_hand(record);

        prev_to_heuristic_tap_hold_was_mod = prev_was_mod;

        if (ms_between_prev_release_and_this_press_was_set) {
            // As timer_read is a 16-bit timer, it will wrap around every
            // 65536 milliseconds. To avoid incorrect values (and cap the
            // duration), we set a boolean in matrix_scan_user.
            ms_between_prev_release_and_heuristic_tap_hold_press = ms_between_prev_release_and_this_press;
        } else {
            ms_between_prev_release_and_heuristic_tap_hold_press = timer_elapsed(ms_prev_release_timer);
        }

        if (should_hold_instantly()) {
            heuristic_tap_hold_keycode_was_held_instantly = true;
            process_register_record_as_hold(record);
        }

        return false;
    }

    if (!heuristic_tap_hold_found) {
        return true;
    }

    // heuristic tap hold key was pressed before
    if (is_pressed) {
        if (tap_hold_decision == UNDECIDED) {
            if (next_to_heuristic_tap_hold_keycode == KC_NO) {
                // this is the first key after the tap hold key
                const bool is_left = is_on_left_hand(record);

                tap_hold_decision_options choice = UNDECIDED;
                if (is_left == heuristic_tap_hold_is_on_left) {
                    choice = choose_when_next_to_heuristic_tap_hold_on_same_side(record, keycode, is_left);
                }

                if (choice == CHOSE_TAP) {
                    choose_heuristic_tap();
                } else if (choice == CHOSE_HOLD) {
                    choose_heuristic_hold();
                } else {
                    ms_overlap_timer = timer_read();
                    next_to_heuristic_tap_hold_keycode = keycode;
                    next_to_heuristic_tap_hold_record = * record;
                    next_to_heuristic_tap_hold_mods = get_mods();

                    ms_between_heuristic_tap_hold_press_and_next_press = timer_elapsed(
                            ms_heuristic_tap_hold_press_timer);
                    ms_min_overlap_for_hold_estimate = calculate_min_overlap_for_hold_in_ms();
                    ms_next_to_heuristic_tap_hold_press_to_release_timer = timer_read();

                    return false;
                }
            } else {
                // this is the second (or later) key after the heuristic tap hold key was pressed
                if (should_choose_hold_when_two_down_after_heuristic_tap_hold()) {
                    choose_heuristic_hold();
                } else {
                    choose_heuristic_tap();
                }
            }
        }

        if (is_tap_hold) {
            // See Limitation 1
            process_register_record_as_tap(record);
            return false;
        }

        // let QMK handle normal key presses
        return true;
    }

    // released
    if (tap_hold_decision == UNDECIDED && keycode == next_to_heuristic_tap_hold_keycode) {
        // completely wrapped (CTRL down, V down, V up, CTRL up) by the heuristic tap hold key
        if (should_choose_hold_when_next_to_heuristic_tap_hold_is_wrapped()) {
            choose_heuristic_hold();
        } else {
            choose_heuristic_tap();
        }
    } else if (next_to_heuristic_tap_hold_keycode == KC_NO) {
        // pressed down before heuristic tap hold and now released
        ms_between_prev_release_and_heuristic_tap_hold_press = -MIN(
                MS_MAX_DUR,
                timer_elapsed(ms_heuristic_tap_hold_press_timer)
        );

        // prev_was_mod will be from the current event, as it is set on every release
        prev_to_heuristic_tap_hold_was_mod = prev_was_mod;
    }

    if (is_tap_hold) {
        if (keycode == heuristic_tap_hold_keycode) {
            finish_heuristic_tap_hold();
        } else {
            // See Limitation 1
            process_unregister_record_as_tap(record);
        }
        return false;
    }

    return true;
}


void heuristic_tap_hold_task(void) {
    if (!ms_between_prev_release_and_this_press_was_set &&
        timer_elapsed(ms_prev_release_timer) >= MS_MAX_DUR
    ) {
        // prev release timer has been held too long
        ms_between_prev_release_and_this_press_was_set = true;
        ms_between_prev_release_and_this_press = MS_MAX_DUR;

        // enough time has passed between presses that this is not relevant anymore
        prev_heuristic_tap_hold_keycode = KC_NO;
        prev_tap_hold_decision = UNDECIDED;
    }

    if (heuristic_tap_hold_keycode == KC_NO || tap_hold_decision != UNDECIDED) {
        return;
    }

    if (has_next_key_and_it_was_held_longer_than_estimate()) {
        choose_heuristic_hold();
        return;
    }

    if (timer_elapsed(ms_heuristic_tap_hold_press_timer) > MS_MAX_OVERLAP) {
        // heuristic tap hold key has been held too long
        if (next_to_heuristic_tap_hold_keycode == KC_NO) {
            // no other key has been pressed
            if (should_choose_tap_when_pressed_very_long_without_another_key()) {
                choose_heuristic_tap();
                return;
            }
        }

        choose_heuristic_hold();
    }
}


#        endif // !NO_ACTION_TAPPING
