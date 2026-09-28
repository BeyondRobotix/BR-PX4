/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file board_config.h
 *
 * Board internal definitions
 */

#pragma once

/****************************************************************************************************
 * Included Files
 ****************************************************************************************************/

#include <px4_platform_common/px4_config.h>
#include <nuttx/compiler.h>
#include <stdint.h>

#include <stm32_gpio.h>

/****************************************************************************************************
 * Definitions
 ****************************************************************************************************/

/* LEDs are active high */

#define GPIO_LED_BLUE           /* PF14 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN14)
#define GPIO_LED_GREEN          /* PI13 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTI|GPIO_PIN13)

#define BOARD_HAS_CONTROL_STATUS_LEDS   1
#define BOARD_OVERLOAD_LED              LED_RED
#define BOARD_ARMED_STATE_LED           LED_BLUE


/*
 * ADC channels
 *
 * These are the channel numbers of the ADCs of the microcontroller that
 * can be used by the Px4 Firmware in the adc driver
 */

/* ADC defines to be used in sensors.cpp to read from a particular channel */
#define ADC1_CH(n)                  (n)

/* Define GPIO pins used as ADC N.B. Channel numbers must match below  */
#define PX4_ADC_GPIO  \
	/* PF11 */  GPIO_ADC1_INP2, \
	/* PA6  */  GPIO_ADC12_INP3, \
	/* PF12 */  GPIO_ADC1_INP6, \
	/* PC2  */  GPIO_ADC123_INP12, \
	/* PC3  */  GPIO_ADC12_INP13

/* Define Channel numbers must match above GPIO pin IN(n)*/
#define ADC_PWR2_IN_CURRENT_CHANNEL     /* PF11 */  ADC1_CH(2)
#define ADC_USB_PWR_CURRENT_CHANNEL     /* PA6  */  ADC1_CH(3)
#define ADC_SCALED_V5_CHANNEL           /* PF12 */  ADC1_CH(6)
#define ADC_SERVO_RAIL_CHANNEL          /* PC2  */  ADC1_CH(12)
#define ADC_PWR1_IN_CURRENT_CHANNEL     /* PC3  */  ADC1_CH(13)

#define ADC_CHANNELS \
	((1 << ADC_PWR2_IN_CURRENT_CHANNEL) | \
	 (1 << ADC_USB_PWR_CURRENT_CHANNEL) | \
	 (1 << ADC_SCALED_V5_CHANNEL)       | \
	 (1 << ADC_SERVO_RAIL_CHANNEL)      | \
	 (1 << ADC_PWR1_IN_CURRENT_CHANNEL))

/* 5V sense is through a 1:1 divider */
#define ADC_V5_V_FULL_SCALE             (6.6f)

/* There is no analog battery input: batteries are monitored by digital power modules */
#define BOARD_NUMBER_BRICKS             0
#define BOARD_NUMBER_DIGITAL_BRICKS     2


/* Power supply control and monitoring GPIOs */

#define GPIO_VDD_5V_HIPOWER_EN          /* PJ0  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTJ|GPIO_PIN0)
#define GPIO_VDD_5V_PERIPH_EN           /* PJ1  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTJ|GPIO_PIN1)
#define GPIO_ETH_POWER_EN               /* PK6  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTK|GPIO_PIN6)

#define GPIO_VDD_BRICK1_VALID           /* PI7  */ (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTI|GPIO_PIN7)
#define GPIO_VDD_BRICK2_VALID           /* PI8  */ (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTI|GPIO_PIN8)
#define GPIO_VDD_5V_HIPOWER_nOC         /* PI5  */ (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTI|GPIO_PIN5)
#define GPIO_VDD_5V_PERIPH_nOC          /* PI4  */ (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTI|GPIO_PIN4)

#define BOARD_ADC_HIPOWER_5V_OC         (!px4_arch_gpioread(GPIO_VDD_5V_HIPOWER_nOC))
#define BOARD_ADC_PERIPH_5V_OC          (!px4_arch_gpioread(GPIO_VDD_5V_PERIPH_nOC))


/* PWM output voltage select: LOW = 3.3V, HIGH = 5V */

#define GPIO_PWM_VOLT_SEL               /* PI6  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTI|GPIO_PIN6)


/* Spare GPIO outputs (PINIO1-3) */

#define GPIO_PINIO1                     /* PJ2  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTJ|GPIO_PIN2)
#define GPIO_PINIO2                     /* PJ5  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTJ|GPIO_PIN5)
#define GPIO_PINIO3                     /* PJ6  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTJ|GPIO_PIN6)


/* IMU heater */

#define GPIO_HEATER_OUTPUT              /* PG4  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTG|GPIO_PIN4)
#define HEATER_OUTPUT_EN(on_true)       px4_arch_gpiowrite(GPIO_HEATER_OUTPUT, (on_true))


/* PWM
 *
 * The board has 18 outputs, PX4 supports up to 16 (TIM5 CH3/CH4 on PH12/PI0 are unused)
 */
#define DIRECT_PWM_OUTPUT_CHANNELS   16


/* USB OTG FS
 *
 * PI11 VBUS_VALID (USB power on the power layer)
 */

#define GPIO_OTGFS_VBUS         /* PI11 */ (GPIO_INPUT|GPIO_PULLDOWN|GPIO_SPEED_100MHz|GPIO_PORTI|GPIO_PIN11)
#define BOARD_ADC_USB_CONNECTED (px4_arch_gpioread(GPIO_OTGFS_VBUS))


/* High-resolution timer */
#define HRT_TIMER               8  /* use timer8 for the HRT */
#define HRT_TIMER_CHANNEL       1  /* use capture/compare channel 1 */


/* RC Serial port */
#define RC_SERIAL_PORT          "/dev/ttyS0"
#define BOARD_SUPPORTS_RC_SERIAL_PORT_OUTPUT


/* SD Card */
#define SDIO_SLOTNO             0  /* Only one slot */
#define SDIO_MINOR              0

/* This board provides a DMA pool and APIs */
#define BOARD_DMA_ALLOC_POOL_SIZE 5120

/* This board provides the board_on_reset interface */
#define BOARD_HAS_ON_RESET 1


#define PX4_GPIO_INIT_LIST { \
		PX4_ADC_GPIO, \
		GPIO_CAN1_TX, \
		GPIO_CAN1_RX, \
		GPIO_CAN2_TX, \
		GPIO_CAN2_RX, \
		GPIO_VDD_5V_HIPOWER_EN, \
		GPIO_VDD_5V_PERIPH_EN, \
		GPIO_ETH_POWER_EN, \
		GPIO_VDD_BRICK1_VALID, \
		GPIO_VDD_BRICK2_VALID, \
		GPIO_VDD_5V_HIPOWER_nOC, \
		GPIO_VDD_5V_PERIPH_nOC, \
		GPIO_PWM_VOLT_SEL, \
		GPIO_PINIO1, \
		GPIO_PINIO2, \
		GPIO_PINIO3, \
		GPIO_HEATER_OUTPUT, \
		GPIO_OTGFS_VBUS, \
	}

#define BOARD_ENABLE_CONSOLE_BUFFER

#define FLASH_BASED_PARAMS

#define BOARD_NUM_IO_TIMERS 5


__BEGIN_DECLS

/****************************************************************************************************
 * Public Types
 ****************************************************************************************************/

/****************************************************************************************************
 * Public data
 ****************************************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************************************
 * Public Functions
 ****************************************************************************************************/

/****************************************************************************
 * Name: stm32_sdio_initialize
 *
 * Description:
 *   Initialize SDIO-based MMC/SD card support
 *
 ****************************************************************************/

int stm32_sdio_initialize(void);

/****************************************************************************************************
 * Name: stm32_spiinitialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the board.
 *
 ****************************************************************************************************/

extern void stm32_spiinitialize(void);

extern void stm32_usbinitialize(void);

extern void board_peripheral_reset(int ms);

#include <px4_platform_common/board_common.h>

#endif /* __ASSEMBLY__ */

__END_DECLS
