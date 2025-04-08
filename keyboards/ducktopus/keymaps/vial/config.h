#pragma once

#define VIAL_KEYBOARD_UID {0x5A, 0xD7, 0xFC, 0x1B, 0x2E, 0xD2, 0x63, 0x01}

#define VIAL_UNLOCK_COMBO_ROWS { 0, 5 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 5 }


// Enables This makes it easier for fast typists to use dual-function keys
#define PERMISSIVE_HOLD
#define TAPPING_TERM 0
#define TAP_CODE_DELAY 10

/* use this without: Vial
#ifndef TAPPING_TERM_PER_KEY
    #define TAPPING_TERM_PER_KEY
#endif
*/
