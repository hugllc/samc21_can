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

#include <stdio.h>
#include <sam.h>
#include <stdbool.h>
#include <string.h>
#include "board_driver_serial.h"
#include "board_definitions.h"

#define WELCOME_STR "This is a test\r\n"

extern uint32_t __sketch_vectors_ptr; // Exported value from linker script
extern void board_init(void);

#if (defined DEBUG) && (DEBUG == 1)
volatile uint32_t *pulSketch_Start_Address;
#endif

static volatile bool main_b_cdc_enable = false;

/**
 * \brief Check the application startup condition
 *
 */
static void check_start_application(void)
{
#if (!defined DEBUG) || ((defined DEBUG) && (DEBUG == 0))
    uint32_t *pulSketch_Start_Address;
#endif
    /*
     * Test sketch stack pointer @ &__sketch_vectors_ptr
     * Stay in SAM-BA if value @ (&__sketch_vectors_ptr) == 0xFFFFFFFF (Erased flash cell value)
     */
    if (__sketch_vectors_ptr == 0xFFFFFFFF) {
        /* Stay in bootloader */
        return;
    }
    /*
     * Load the sketch Reset Handler address
     * __sketch_vectors_ptr is exported from linker script and point on first 32b word of sketch vector table
     * First 32b word is sketch stack
     * Second 32b word is sketch entry point: Reset_Handler()
     */
    pulSketch_Start_Address = &__sketch_vectors_ptr ;
    pulSketch_Start_Address++ ;
    /*
     * Test vector table address of sketch @ &__sketch_vectors_ptr
     * Stay in SAM-BA if this function is not aligned enough, ie not valid
     */
    if (((uint32_t)(&__sketch_vectors_ptr) & ~SCB_VTOR_TBLOFF_Msk) != 0x00) {
        /* Stay in bootloader */
        return;
    }
#if defined(BOOT_DOUBLE_TAP_ADDRESS)
#define DOUBLE_TAP_MAGIC 0x07738135
    if (RSTC->RCAUSE.bit.POR) {
        /* On power-on initialize double-tap */
        BOOT_DOUBLE_TAP_DATA = 0;
    } else {
        if (BOOT_DOUBLE_TAP_DATA == DOUBLE_TAP_MAGIC) {
            /* Second tap, stay in bootloader */
            BOOT_DOUBLE_TAP_DATA = 0;
            return;
        }
        /* First tap */
        BOOT_DOUBLE_TAP_DATA = DOUBLE_TAP_MAGIC;
        /* Wait 0.5sec to see if the user tap reset again.
         * The loop value is based on SAMD21 default 1MHz clock @ reset.
         */
        for (uint32_t i = 0; i < 125000; i++) /* 500ms */
            /* force compiler to not optimize this... */
        {
            __asm__ __volatile__("");
        }
        /* Timeout happened, continue boot... */
        BOOT_DOUBLE_TAP_DATA = 0;
    }
#endif
#if defined(BOOT_LOAD_PIN)
    volatile PortGroup *boot_port = (volatile PortGroup *)(&(PORT->Group[BOOT_LOAD_PIN / 32]));
    volatile bool boot_en;
    // Enable the input mode in Boot GPIO Pin
    boot_port->DIRCLR.reg = BOOT_PIN_MASK;
    boot_port->PINCFG[BOOT_LOAD_PIN & 0x1F].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
    boot_port->OUTSET.reg = BOOT_PIN_MASK;
    // Read the BOOT_LOAD_PIN status
    boot_en = (boot_port->IN.reg) & BOOT_PIN_MASK;
    // Check the bootloader enable condition
    if (!boot_en) {
        // Stay in bootloader
        return;
    }
#endif
    //  LED_on();
    /* Rebase the Stack Pointer */
    __set_MSP((uint32_t)(__sketch_vectors_ptr));
    /* Rebase the vector table base address */
    SCB->VTOR = ((uint32_t)(&__sketch_vectors_ptr) & SCB_VTOR_TBLOFF_Msk);
    /* Jump to application Reset Handler in the application */
    asm("bx %0"::"r"(*pulSketch_Start_Address));
}

/**
 *  \brief SAMD21 SAM-BA Main loop.
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{
    /* Jump in application if condition is satisfied */
    check_start_application();
    /* We have determined we should stay in the monitor. */
    /* System initialization */
    board_init();
    __enable_irq();
    /* UART is enabled in all cases */
    //serial_open();
    /* Initialize LEDs */
    //LED_test();
    /* Start the sys tick (1 ms) */
    SysTick_Config(F_CPU / 1000);
    /* Wait for a complete enum on usb or a '#' char on serial line */
    /* Enable clock for BOOT_USART_MODULE */
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM5 ;
    /* Set GCLK_GEN0 as source for GCLK_ID_SERCOMx_CORE */
    GCLK->PCHCTRL[(0x19)].reg = (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0);
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_MASK) {
        /* Wait for synchronization */
    }
    PORT->Group[SERIAL_PORT_GROUP].PINCFG[SERIAL_PORT_TX].bit.PMUXEN = 1;
    PORT->Group[SERIAL_PORT_GROUP].PINCFG[SERIAL_PORT_RX].bit.PMUXEN = 1;
    PORT->Group[SERIAL_PORT_GROUP].PMUX[SERIAL_PORT_TX / 2].reg = PORT_PMUX_PMUXO(3) |  PORT_PMUX_PMUXE(3);
    uart_basic_init(SERCOM5, 63018, UART_RX_PAD3_TX_PAD2);
    uart_write_buffer_polled(SERCOM5, (uint8_t *)WELCOME_STR, sizeof(WELCOME_STR) - 2);
    
    while (1) {
        //LED_toggle(0);
        /* Check if a '#' has been received */
        //if (!main_b_cdc_enable && serial_sharp_received())
        //{
        //  sam_ba_monitor_init(SAM_BA_INTERFACE_USART);
        /* SAM-BA on Serial loop */
        //  while(1)
        //  {
        //    sam_ba_monitor_run();
        //  }
        //}
    }
}

void SysTick_Handler(void)
{
    static uint16_t delay = 0;
    if (delay++ > 1000) {
        delay = 0;
    }
    //sam_ba_monitor_sys_tick();
}
