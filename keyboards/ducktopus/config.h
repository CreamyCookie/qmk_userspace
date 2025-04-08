#pragma once

#define USE_SERIAL
#define SPLIT_HAND_PIN      GP26  // high = left, low = right
#define SPLIT_LAYER_STATE_ENABLE

#define SPLIT_USB_DETECT
#define SPLIT_USB_TIMEOUT 3000
#define SPLIT_USB_TIMEOUT_POLL 10

#define USE_SERIAL
#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_TX_PIN GP0
#define SERIAL_USART_RX_PIN GP1
#define SERIAL_PIO_USE_PIO0
#define SERIAL_USART_TIMEOUT     100  // USART driver timeout. default 100
#define SERIAL_USART_SPEED 921600
#define SERIAL_USART_PIN_SWAP
#define PMW33XX_CPI 1600

#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 200U
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED_MASK 0U

/* key matrix size */
// Columns are doubled-up
#define MATRIX_ROWS 12
#define MATRIX_COLS 6

#define MATRIX_COL_PINS { GP7, GP6, GP5, GP4, GP3, GP2 }
#define MATRIX_ROW_PINS { GP14, GP13, GP12, GP11, GP10, GP9 }

#define DIODE_DIRECTION COL2ROW

#define SPLIT_POINTING_ENABLE
#define ROTATIONAL_TRANSFORM_ANGLE  -25
#define POINTING_DEVICE_INVERT_Y
#define POINTING_DEVICE_RIGHT
#define PMW33XX_CS_PIN GP21
#define POINTING_DEVICE_TASK_THROTTLE_MS 1
#define PMW33XX_LIFTOFF_DISTANCE 0x02

#define TRACKBALL_ENABLE_POINTER_ACCELERATION

#define SPLIT_TRANSACTION_IDS_KB RPC_ID_KB_CONFIG_SYNC


#define DYNAMIC_KEYMAP_LAYER_COUNT 8
#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD

//// Rotary Encoder ////
//#define ENCODERS_PAD_A { }
//#define ENCODERS_PAD_B { }
//#define ENCODER_RESOLUTIONS { }

#ifdef ENCODER_ENABLE
#define ENCODERS_PAD_A { GP28 }
#define ENCODERS_PAD_B { GP27 }
#define ENCODER_RESOLUTIONS { 2 }
#define ENCODERS_PAD_A_RIGHT { GP28 }
#define ENCODERS_PAD_B_RIGHT { GP27 }
#define ENCODER_RESOLUTIONS_RIGHT { 2 }
#endif

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
