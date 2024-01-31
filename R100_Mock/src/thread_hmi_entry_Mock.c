/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_hmi_entry.c
 * @brief
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

#include "synergy_gen_Mock/thread_hmi_Mock.h"
#include "synergy_gen_Mock/thread_core_Mock.h"
#include "thread_core_entry_Mock.h"
#include "synergy_gen_Mock/thread_comm_Mock.h"
#include "Resources/Trace_Mock.h"
#include "Resources/Keypad_Mock.h"
#include "event_ids.h"
#include "HAL/thread_defibrillator_hal_Mock.h"           // to know when is charging the main capacitor
#include "types_basic_Mock.h"

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

/******************************************************************************
 ** Prototypes
 */

static void Read_Keys_mock  (void);

/******************************************************************************
 ** Name:    Read_Keys
 *****************************************************************************/
/**
 ** @brief   Check keys status to report to the core
 **          Assume that this function is called every 40 msecs.
 **
 ******************************************************************************/
static void Read_Keys_mock(void)
{
    static uint16_t     keymask_onoff;
    static uint16_t     keymask_shock;
    static uint16_t     keymask_cover;
    static uint16_t     keymask_patype;
    static bool_t       keymask_patype_release = TRUE;
    static bool_t       keymask_onoff_release = TRUE;
           //uint8_t      tx_event;
printf("Read_Keys_mock\n");
    // check keys sate
//	keymask_shock  = (uint16_t) (Key_Read (KEY_SHOCK)? (keymask_shock + 1):0);
//	keymask_cover  = (uint16_t) (Key_Read (KEY_COVER)? (keymask_cover + 1):0);

	if (Key_Read_mock (0))
    {
        if(keymask_onoff_release) keymask_onoff++;
    }
    else {
        if(keymask_onoff) keymask_onoff--;
        if(keymask_onoff == 0) keymask_onoff_release = true;
    }

    if (Key_Read_mock (1))
    {
        if(keymask_patype_release) keymask_patype++;
    }
    else {
	    if(keymask_patype) keymask_patype--;
	    if(keymask_patype == 0) keymask_patype_release = true;
    }

    // report that the ON-OFF key has been pressed
    if ((keymask_onoff == 8) && (keymask_onoff_release))
    {
        Send_Core_Queue_mock (eEV_KEY_ONOFF);       // 100msec loop x 8 = 800mseconds
        keymask_onoff_release = false;
    }

    // report that the SHOCK key has been pressed
    if (keymask_shock >= 2)        // 100msec loop x 2 = 200mseconds
    {
        Send_Core_Queue_mock (eEV_KEY_SHOCK);
        keymask_shock = (uint16_t)0;
    }

    // report that the cover key has been closed
    if (keymask_cover >= 16)        // 100msec loop x 16 = 1.6seconds
    {
        Send_Core_Queue_mock (eEV_KEY_COVER);
        keymask_cover = (uint16_t)0;
    }

    // report that the Patient type key has been pressed
    if ((keymask_patype == 3) && (keymask_patype_release))      // 100msec loop x 3 = 300mseconds
    {
        Send_Core_Queue_mock (eEV_KEY_PATYPE);
        keymask_patype_release = false;
    }
}

/******************************************************************************
 ** Name:    thread_hmi_entry
 *****************************************************************************/
/**
 ** @brief   Defines HMI thread behavior
 **
 ******************************************************************************/
void thread_hmi_entry_mock(void)
{
    uint8_t     loop_counter;

    printf("thread_hmi_entry_mock\n");

    // infinite loop reading keys and refreshing the blinking leds ...
    for (loop_counter = 0; (1); loop_counter++)
    {
        if ((loop_counter % 4) == 0)
        {
            Blink_Refresh_mock();
        }

        Read_Keys_mock ();
        //tx_thread_sleep (OSTIME_100MSEC);
        printf("Sleep\n");
    }
}
