/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  Copyright (c) 2015 Atmel Corporation/Thibaut VIARD.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _BOARD_DEFINITIONS_H_
#define _BOARD_DEFINITIONS_H_

/*
 * USB device definitions
 */
#define STRING_PRODUCT "1082-01-02 RedBoard"

/*
 * If BOOT_DOUBLE_TAP_ADDRESS is defined the bootloader is started by
 * quickly tapping two times on the reset button.
 * BOOT_DOUBLE_TAP_ADDRESS must point to a free SRAM cell that must not
 * be touched from the loaded application.
 */
#define BOOT_DOUBLE_TAP_ADDRESS           (0x20007FFCul)
#define BOOT_DOUBLE_TAP_DATA              (*((volatile uint32_t *) BOOT_DOUBLE_TAP_ADDRESS))

/*
 * If BOOT_LOAD_PIN is defined the bootloader is started if the selected
 * pin is tied LOW.
 */
//#define BOOT_LOAD_PIN                     PIN_PA21 // Pin 7
//#define BOOT_LOAD_PIN                     PIN_PA15 // Pin 5

#define BOOT_USART_MODULE                 SERCOM3
#define BOOT_USART_BUS_CLOCK_INDEX        MCLK_APBCMASK_SERCOM3
#define BOOT_USART_PER_CLOCK_INDEX        (0x16U)
#define BOOT_USART_PAD_SETTINGS           UART_RX_PAD1_TX_PAD0
#define BOOT_USART_PAD3                   PINMUX_PB23C_SERCOM5_PAD3
#define BOOT_USART_PAD2                   PINMUX_PB22C_SERCOM5_PAD2
#define BOOT_USART_PAD1                   PINMUX_UNUSED
#define BOOT_USART_PAD0                   PINMUX_UNUSED


/* Master clock frequency */
#define CPU_FREQUENCY                     (48000000ul)
#define VARIANT_MCK                       CPU_FREQUENCY

/* Frequency of the board main oscillator */
#define VARIANT_MAINOSC                   (32768ul)

/* Calibration values for DFLL48 pll */
#define NVM_SW_CALIB_DFLL48M_COARSE_VAL   (58)
#define NVM_SW_CALIB_DFLL48M_FINE_VAL     (64)

/*
 * LEDs definitions
 */
#define BOARD_LED_PORT                    (0)
#define BOARD_LED_PIN                     (16)
#define BOARD_LED_COUNT                   (9)
#define BOARD_LED_BITS                    (24)


/*
 * Serial port definitions
 */
#define SERIAL_PORT_GROUP                 (1)
#define SERIAL_PORT_RX                    (23)
#define SERIAL_PORT_TX                    (22)

/*
 * CAN definitions
 */
#define USECAN      CAN0
#define CAN_PORT    0
#define CAN_TX_PIN  24
#define CAN_RX_PIN  24
#define CAN_BIT_RATE  125000
#define CAN_QUANTA_BEFORE_SP (10 + 2)
#define CAN_QUANTA_AFTER_SP  (3 + 1)

/*
 * Other stuff
 */
#define BOOT_PIN_MASK (1U << (BOOT_LOAD_PIN & 0x1f))

#endif // _BOARD_DEFINITIONS_H_
