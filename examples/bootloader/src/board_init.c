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

#include <sam.h>
#include "board_definitions.h"

/**
 * \brief system_init() configures the needed clocks and according Flash Read Wait States.
 * At reset:
 * - OSC8M clock source is enabled with a divider by 8 (1MHz).
 * - Generic Clock Generator 0 (GCLKMAIN) is using OSC8M as source.
 * We need to:
 * 1) Enable XOSC32K clock (External on-board 32.768Hz oscillator), will be used as DFLL48M reference.
 * 2) Put XOSC32K as source of Generic Clock Generator 1
 * 3) Put Generic Clock Generator 1 as source for Generic Clock Multiplexer 0 (DFLL48M reference)
 * 4) Enable DFLL48M clock
 * 5) Switch Generic Clock Generator 0 to DFLL48M. CPU will run at 48MHz.
 * 6) Modify PRESCaler value of OSCM to have 8MHz
 * 7) Put OSC8M as source for Generic Clock Generator 3
 */
// Constants for Clock generators
#define GENERIC_CLOCK_GENERATOR_MAIN      (0u)
#define GENERIC_CLOCK_GENERATOR_XOSC32K   (1u)
#define GENERIC_CLOCK_GENERATOR_OSCULP32K (2u) /* Initialized at reset for WDT */
#define GENERIC_CLOCK_GENERATOR_OSC8M     (3u)
// Constants for Clock multiplexers
#define GENERIC_CLOCK_MULTIPLEXER_DFLL48M (0u)

void board_init(void)
{
    // Set 1 Flash Wait State for 48MHz (2 for the L21 and C21), cf tables 20.9 and 35.27 in SAMD21 Datasheet
    NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_RWS_DUAL ; // two wait states
    // Turn on the digital interface clock
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_GCLK ;
    // ----------------------------------------------------------------------------------------------
    //Software reset the GCLK module to ensure it is re-initialized correctly
    //
    GCLK->CTRLA.reg = GCLK_CTRLA_SWRST ;
    while ((GCLK->CTRLA.reg & GCLK_CTRLA_SWRST) && (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_MASK));    // Wait for reset to complete
#if defined(CLOCKCONFIG_32768HZ_CRYSTAL)
    // ----------------------------------------------------------------------------------------------
    //Enable XOSC32K clock (External on-board 32.768Hz crystal oscillator)
    //
#define DPLLRATIO_LDR         2929u
#define DPLLRATIO_LDRFRAC     0u
    OSC32KCTRL->XOSC32K.reg = (OSC32KCTRL_XOSC32K_STARTUP(0x4u) | OSC32KCTRL_XOSC32K_XTALEN | OSC32KCTRL_XOSC32K_EN32K | OSC32KCTRL_XOSC32K_EN1K) ;
    OSC32KCTRL->XOSC32K.bit.ENABLE = 1 ;
    while ((OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY) == 0);         // Wait for oscillator stabilization
    OSCCTRL->DPLLRATIO.reg = (OSCCTRL_DPLLRATIO_LDR(DPLLRATIO_LDR) | OSCCTRL_DPLLRATIO_LDRFRAC(DPLLRATIO_LDRFRAC)) ;    // set PLL multiplier
    while (OSCCTRL->DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_MASK);
    OSCCTRL->DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK(0) ;  // select 32KHz crystal input
    OSCCTRL->DPLLPRESC.reg = 0;
    while (OSCCTRL->DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_MASK);
    OSCCTRL->DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE ;
    while (OSCCTRL->DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_MASK);
    while ((OSCCTRL->DPLLSTATUS.reg & OSCCTRL_DPLLSTATUS_CLKRDY) != OSCCTRL_DPLLSTATUS_CLKRDY);
    // Switch Generic Clock Generator 0 to PLL. Divide by two and the CPU will run at 48MHz.
    GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_MAIN].reg = (GCLK_GENCTRL_DIV(2) | GCLK_GENCTRL_SRC_DPLL96M | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN);
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_MASK);
#else // Internal Clock
    /* Change OSC48M divider to /1. CPU will run at 48MHz */
    OSCCTRL->OSC48MDIV.reg = OSCCTRL_OSC48MDIV_DIV(0);
    while (OSCCTRL->OSC48MSYNCBUSY.reg & OSCCTRL_OSC48MSYNCBUSY_OSC48MDIV);
#endif
}
