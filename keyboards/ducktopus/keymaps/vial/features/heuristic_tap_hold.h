// Copyright 2024 Joschua Gandert (@CreamyCookie)

#pragma once

#include "quantum.h"

typedef enum {
    UNDECIDED,
    CHOSE_TAP,
    CHOSE_HOLD
} tap_hold_decision_options;

#if !defined(MIN)
#    define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#if !defined(MAX)
#    define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#if !defined(ABS)
#    define ABS(x) (((x) < 0) ? -(x) : (x))
#endif

#define SD(x, y) (((y) == 0) ? (x) : ((x) / (y)))

// utility functions
//=============================================================================
bool prev_chose_tap_and_was_same_tap_hold(void);
uint16_t get_heuristic_tap_hold_keycode(void);
uint16_t get_tap_keycode(uint16_t keycode);
bool is_on_left_hand(keyrecord_t* record);

// configure the heuristic tap hold
//=============================================================================
// If the next key (the key following the tap hold key) is wrapped
// (e.g. C(KC_E) down, T down, T up, C(KC_EE) up), then by default we will use
// a heuristic to choose if the key is tap or hold, but if you like you can
// also just return true here, so it will always end up as a hold.
bool should_choose_hold_when_next_to_heuristic_tap_hold_is_wrapped(void);

// We use another heuristic for cases such as C(KC_E) down, T down, A down
bool should_choose_hold_when_two_down_after_heuristic_tap_hold(void);

// When a tap hold key is held longer than MS_MAX_OVERLAP, this is called.
// If True is returned, we choose tap, else hold.
//
// By default, this will only return true if the previous key was also a tap
// hold key, had the same keycode, and tap was chosen.
//
// We do this to allow for a way to auto repeat the tap part of a tap hold key,
// as works for normal keys.
bool should_choose_tap_when_pressed_very_long_without_another_key(void);

// This is only called if the next key is on the same side as the tap hold key.
// The supplied arguments are all for the next key (its keycode and so on).
tap_hold_decision_options choose_when_next_to_heuristic_tap_hold_on_same_side(
        keyrecord_t* record, uint16_t keycode, bool is_left);

// You can check the current tap hold key code in this function and decide if
// you want to return true. This is useful for cases where the hold needs to be
// active directly (e.g. ctrl + scroll wheel on your mouse)
bool should_hold_instantly(void);

// call these in keymap.c to enable heuristic tap holds
//=============================================================================
void heuristic_tap_hold_task(void);
bool process_heuristic_tap_hold(uint16_t keycode, keyrecord_t *record);
