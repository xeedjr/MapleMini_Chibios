/*
 ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for the LeafLabs Maple Mini.
 */

/*
 * Board identifier.
 */
#define BOARD_MAPLEMINI_STM32_F103
#define BOARD_NAME              "LeafLabs Maple Mini"

/*
 * Board frequencies.
 */
#define STM32_LSECLK            32768
#define STM32_HSECLK            8000000

/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 */
#define STM32F103xB

/*
 * IO pins assignments
 *
 * numbering is sorted by onboard/connectors, as from the schematics in
 * https://github.com/leaflabs/maplemini
 */

/* on-board */

#define INDICATE               		13
#define INDICATE_PORT          		GPIOC
#define USER_BUTTON            		7
#define USER_BUTTON_PORT       		GPIOB
#define USER_RELE1            		6
#define USER_RELE1_PORT       		GPIOB
#define USER_MB_RS485_RTS           12
#define USER_MB_RS485_RTS_PORT      GPIOA
#define USER_MB_USART       	  	UARTD1
#define USER_MB_RTS_ENABLE			1
#define USER_MB_TIMER 				GPTD1
#define USER_MB_THREAD_WORK_AREA_SIZE 1024
#define USER_MB_THREAD_PRIORITY 	osPriorityNormal
#define ENC28J60_SPI				SPID1
#define ENC28J60_CS					4
#define ENC28J60_CS_PORT			GPIOA
#define ENC28J60_INT				0
#define ENC28J60_INT_PORT			GPIOB
#define ENC28J60_RESET				1
#define ENC28J60_RESET_PORT			GPIOB


/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *   0 - Analog input.
 *   1 - Push Pull output 10MHz.
 *   2 - Push Pull output 2MHz.
 *   3 - Push Pull output 50MHz.
 *   4 - Digital input.
 *   5 - Open Drain output 10MHz.
 *   6 - Open Drain output 2MHz.
 *   7 - Open Drain output 50MHz.
 *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
 *   9 - Alternate Push Pull output 10MHz.
 *   A - Alternate Push Pull output 2MHz.
 *   B - Alternate Push Pull output 50MHz.
 *   C - Reserved.
 *   D - Alternate Open Drain output 10MHz.
 *   E - Alternate Open Drain output 2MHz.
 *   F - Alternate Open Drain output 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input with pull-up except:
 * PA2  - Alternate output  (USART2 TX).
 * PA3  - Normal input      (USART2 RX).
 * PA4  - Push Pull output 50MHz.      (ENC_CS).
 * PA5  - Push Pull output 50MHz.      (ENC_SCK).
 * PA6  - input.      (ENC_MISO).
 * PA7  - Push Pull output 50MHz.      (ENC_MOSI).
 * PA9  - Alternate output  (USART1 TX).
 * PA10 - Normal input      (USART1 RX).
 * PA12 - Alternate output  (USART1 RTS).
 */
#define VAL_GPIOACRL            0xB4B34B88      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x888384B8      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFFFF

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB0    - Push Pull output  (ENC_INT).
 * PB1    - Push Pull output  (ENC_RESET).
 */
#define VAL_GPIOBCRL            0x88888814      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x88888888      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFFF

/*
 * Port C setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIOCCRL            0x88888888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x88188888      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

/*
 * Port D setup.
 * Everything input with pull-up except:
 * PD0  - Normal input (XTAL).
 * PD1  - Normal input (XTAL).
 */
#define VAL_GPIODCRL            0x88888844      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x88888888      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIOECRL            0x88888888      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x88888888      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF

/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp) palClearPad(GPIOB, GPIOB_USB_DISC)

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp) palSetPad(GPIOB, GPIOB_USB_DISC)

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
void boardInit(void);

#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
