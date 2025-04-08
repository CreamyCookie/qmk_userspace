# Heuristic Tap Hold
*Smarter Key Press Decisions Powered by Genetic Algorithms*

---

This features completely overwrites the way tap hold keys work.

When the user presses `LCTL_T(KC_E)` and then `KC_V`, the decision to send `CTRL + V` or `EV` is made based on the duration that both keys are held down simultaneously. If the overlap is larger than a certain duration, it is a hold, and so `CTRL + V` will be sent. If they don't, it is considered a tap, and so `EV` will be sent.

However, the overlap duration is not a static duration like with the `TAPPING_TERM`. Instead, the duration is estimated using a heuristic when the `V` is pressed. It uses information like the duration between the two presses, as well as the press before. There are also two functions that directly make a choice. They are used when a key is wrapped or three keys are down. These functions were found using a genetic algorithm (specifically genetic programming and symbolic regression).

Note that the key events in the hold case (`LCTL_T(KC_E)` becomes `CTRL`) are sent immediately once the actual overlap is greater than the predicted one. As a result, these key events may be sent much faster than using the traditional tap hold system. 

For more information, see the [evolve_heuristic_tap_hold](https://github.com/CreamyCookie/evolve_heuristic_tap_hold) repository.


## How to use
**0.** Copy `heuristic_tap_hold.c` and `heuristic_tap_hold.h` to a folder `features` (in the same folder as your `keymap.c`)

**1.** Add `SRC += features/heuristic_tap_hold.c` to your `rules.mk`

**2.** Add `#define TAPPING_TERM 0` to your `config.h`. This means QMK will resolve every tap hold key press as a hold, which the heuristic tap hold code can do its job and possibly turn into a tap. **Optional**: Add `#define TAP_CODE_DELAY 10` as well. On my machine some apps would ignore key presses in shortcuts without this.

**3.** Add `#include "features/heuristic_tap_hold.h"` to the top of your `keymap.c`

**4.**  Add or update the `matrix_scan_user` function in your `keymap.c`:
```c
void matrix_scan_user(void) {
#        if !defined(NO_ACTION_TAPPING)
    heuristic_tap_hold_task();
#        endif // !NO_ACTION_TAPPING
}
```

**5.** Add or update the `process_record_user` in your `keymap.c`
```c
bool process_record_user(uint16_t keycode, keyrecord_t* record) {
#        if !defined(NO_ACTION_TAPPING)
    if (!process_heuristic_tap_hold(keycode, record)) {
        return false;
    }
#        endif // !NO_ACTION_TAPPING
}
```


## Limitation
### 1. Multiple tap hold keys
At the moment, multiple tap hold keys can't be held together. So `LCTL_T(KC_E)` + `LSFT_T(KC_T)` would just result in `ET`. I admit this is a major limitation and there are situations where I also miss it.

That said, it does seem more ergonomic to use one key for pressing multiple modifiers (`CTRL` + `SHIFT` + `F` with only two keys). That is what is used in my [keymap.c](../keymap.c).

Another upside of this limitation is that there will be fewer failed predictions. Consider again this situation:

`LCTL_T(KC_E)` + `LSFT_T(KC_T)` + `KC_A`

In the current implementation, the user knows that the second key press can only be a `T`. Assuming we usually get 90% correct: `P(correct) = 0.9 * 1 * 1 = 0.9`

If, on the other end, it could also be a `LSFT`, then the possible predictions are:

1. `KC_LCTL` + `KC_LSFT` + `KC_A`
2. `KC_LCTL` + `KC_T` + `KC_A`
3. `KC_E` + `KC_LSFT` + `KC_A`
4. `KC_E` + `KC_T` + `KC_A`

Of course that means there is a lower probability that we make the correct prediction. `P(correct) = 0.9 * 0.9 * 1 = 0.81`

### 2. Possibly more
I have not tested how this works with features like combos. It does, however, work with the `CAPS_WORD` feature, and since we send events via `process_record` (and not `register_code16`), it should work with most macros.

## History
One thing that always bothered me about the tap hold functionality is the delay of normal typing. So, just to test whether it could be done, I started with a tap-first-ask-later approach. That is, the code would send the `tap` part of a tap hold key. Then, when the next key was pressed (not send to host yet) and when the code predicted a `hold`, it would do the following:

It would send `backspace`. Then it would send the hold part of the tap hold. And last but not least, it would actually send the next key event. Of course, that was completely unusable, as this completely destroys any undo functionality. Not to mention that what the tap part did, would not be undone by backspace in every application. 
