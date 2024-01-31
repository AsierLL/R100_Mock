/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        Keypad.c
 * @brief       All functions related to the keyboard and LEDs manage
 *
 * @version     v1
 * @date        16/01/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

/******************************************************************************
 ** Includes
 */

#include "Keypad_Mock.h"
#include "../types_app.h"

#include "../synergy_gen_Mock/hal_data_Mock.h"

/******************************************************************************
 ** Macros
 */

/******************************************************************************
 ** Defines
 */

/******************************************************************************
 ** Typedefs
 */

/******************************************************************************
 ** Constants
 */



/******************************************************************************
 ** Externs
 */

/******************************************************************************
 ** Globals
 */

/******************************************************************************
 ** Locals
 */

static uint32_t blinker[4];

/******************************************************************************
 ** Prototypes
 */

/******************************************************************************
 ** Name:    Led_On
 *****************************************************************************/
/**
 ** @brief   Turn on the led indicated by index
 ** @param   led_idx       Index of the led to turn on
 **
 ******************************************************************************/
void Led_On_mock(LED_IDENT_e led_idx)
{
	printf("Led_On mock \n");
	//g_ioport.p_api->pinWrite (led_io_port[led_idx], LED_VALUE_ON);
    blinker [led_idx] = 0;
}

/******************************************************************************
 ** Name:    Led_Off
 *****************************************************************************/
/**
 ** @brief   Turn off the led indicated by index
 ** @param   led_idx       Index of the led to turn off
 **
 ******************************************************************************/
void Led_Off_mock(LED_IDENT_e led_idx)
{
	printf("Led_Off mock\n");
    //g_ioport.p_api->pinWrite (led_io_port[led_idx], LED_VALUE_OFF);
    blinker [led_idx] = 0;
}

/******************************************************************************
 ** Name:    Led_Blink
 *****************************************************************************/
/**
 ** @brief   Blink the led indicated by index
 ** @param   led_idx       Index of the led to blink
 **
 ******************************************************************************/
void Led_Blink_mock(LED_IDENT_e led_idx)
{
	printf("Led_Blink mock: ident :%u \n", led_idx);

}

/******************************************************************************
 ** Name:    Led_Toggle
 *****************************************************************************/
/**
 ** @brief   Toggle the led indicated by index
 ** @param   led_idx       Index of the led to toggle
 **
 ******************************************************************************/
void Led_Toggle_mock(LED_IDENT_e led_idx)
{
	printf("Led_Toggle mock \n");
    blinker [led_idx] = 0;
}

/******************************************************************************
 ** Name:    Led_Ongoing_Transmission
 *****************************************************************************/
/**
 ** @brief   LED animation for transmission
 **
 ******************************************************************************/
void Led_Ongoing_Transmission (void)
{
    static uint8_t idx = 0;

    switch (idx)
    {
        case 0: Led_Toggle_mock(0);  break;
        case 1: Led_Toggle_mock(2); break;
        case 2: Led_Toggle_mock(1);  break;
        default: break;
    }
    (idx >=2 ) ? idx = 0 : idx++;
}

/******************************************************************************
 ** Name:    Blink_Refresh
 *****************************************************************************/
/**
 ** @brief   Refresh the blinking leds
 ** @param   none
 **
 ******************************************************************************/
void Blink_Refresh_mock (void)
{
    static uint8_t free_counter;
    uint8_t        i;
        free_counter++;

    // set/reset the blinking LEDs
    for (i=0; i<4; i++)
    {
    	printf("Blink_Refresh mock: %d \n", i);
    }
}

/******************************************************************************
 ** Name:    Key_Read
 *****************************************************************************/
/**
 ** @brief   Read from Key-board
 ** @param   key_idx       Key to check
 **
 ** @return  KEY_STATUS_e
 ******************************************************************************/
KEY_STATUS_e Key_Read_mock(uint32_t key_idx)//, KEY_STATUS_e state)
{
   KEY_STATUS_e status;
   status=1;


    printf("Key Read mock: %u \n", key_idx);

    return status;
}
