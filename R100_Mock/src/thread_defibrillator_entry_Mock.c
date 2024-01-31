/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_defibrillator_entry.c
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

#include "Resources/Trace_Mock.h"
#include "types_basic_Mock.h"
#include "HAL/R100_Tables.h"
#include "event_ids.h"
#include "HAL/thread_defibrillator_hal_Mock.h"
#include "HAL/thread_patMon_hal_Mock.h"

#include "synergy_gen_Mock/thread_defibrillator_Mock.h"
#include "thread_defibrillator_entry.h"

#include "thread_sysMon_entry_Mock.h"
#include "synergy_gen_Mock/thread_core_Mock.h"
#include "thread_core_entry_Mock.h"

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
static uint16_t     voltage_to_charge;          // voltage to charge in the main capacitor
static uint16_t     energy_to_shock;            // energy to shock in joules
static uint16_t     zp_when_shock;              // patient impedance when shock

/******************************************************************************
 ** Prototypes
 */

/******************************************************************************
** Name:    Send_Defib_Queue
*****************************************************************************/
/**
** @brief   Process events related to defib
**
** @param   ev event identifier
**
** @return  if the event has been processed or not
**
******************************************************************************/
static void Send_Defib_Queue_mock(EVENT_ID_e ev)
{
	printf("Send_Defib_Queue_mock");
//    Trace_Arg (TRACE_NEWLINE, "  *****************DEF QUEUE AVAILABLE = %5d", (uint32_t) (queue_def.tx_queue_available_storage));
    if (queue_def.tx_queue_available_storage == 0) return;
  //  tx_queue_send(&queue_def, &ev, TX_WAIT_FOREVER);

}
/******************************************************************************
 ** Name:    Defib_Cmd_Precharge
 *****************************************************************************/
/**
 ** @brief   Precharge the capacitor:
 **      --> If the capacitor has a voltage lower than the precharge
 **          voltage, proceed to charge.
 **      --> If the capacitor has a voltage higher than the precharge
 **          voltage, reuse the current voltage in the next shock OR
 **          force to charge to the precharge value
 **
 ** @param   force  if true, force the precharge voltage
 **                 if false, maintain the voltage if higher
 **
 ** @return  none
 ******************************************************************************/
void Defib_Cmd_Precharge_mock (bool_t force)
{
    uint16_t    my_voltage;

    printf("Defib_Cmd_Precharge_mock\n");

    if (Get_Device_Info()->develop_mode == DEVELOP_TEST_RELAY)        ///< special_mode; do not precharge
    {
        return;
    }

    my_voltage = Defib_Get_Vc();
    if (force || (my_voltage < (PRE_CHARGE_VOLTAGE - 100)))
    {

        // increment the number of full charges !!!
        sMon_Inc_FullCharges ();

        // command the defibrillator to charge the main capacitor !!!
        Send_Defib_Queue_mock (eEV_DEF_PRECHARGE);

        // wait to process the command in the defibrillator thread
        //tx_thread_sleep (OSTIME_50MSEC);
        printf("Sleep\n");
    }
}

/******************************************************************************
** Name:    Defib_Cmd_Disarm
 *****************************************************************************/
/**
 ** @brief   Disarm the capacitor:
 **
 ** @param   none
  **
 ** @return  none
 ******************************************************************************/
void Defib_Cmd_Disarm_mock (void)
{

	printf("Defib_Cmd_Disarm_mock\n");
    // command the defibrillator to disarm the main capacitor !!!
    Send_Defib_Queue_mock (eEV_DEF_DISARM);

    // wait to process the command in the defibrillator thread
    //tx_thread_sleep (OSTIME_50MSEC);
    printf("Sleep\n");
}

/******************************************************************************
 ** Name:    Defib_Cmd_Charge
 *****************************************************************************/
/**
 ** @brief   Command a capacitor charge
 **
 ** @param   energy         energy to discharge (in joules)
 **
 ** @return  none
 ******************************************************************************/
void Defib_Cmd_Charge_mock (uint16_t energy)
{
    uint16_t    my_voltage;
    uint32_t    i;

    printf("Defib_Cmd_Charge_mock\n");

    // update the ZP value ...
    zp_when_shock = (uint16_t) patMon_Get_Zp_Ohms_mock();

    // assign the energy to shock
    energy_to_shock = energy;

    // assign the voltage required to develop the requested
    // energy over the connected patient impedance
    for (i=0; energy_table[i].zp_ohms; i++)
    {
        if (zp_when_shock <= energy_table[i].zp_ohms) { break; }
    }
    if (energy_table[i].zp_ohms == 0)
    {
        i--;
    }
    switch (energy)
    {
        case  50 : voltage_to_charge = energy_table[i].v_e50;   break;
        case  65 : voltage_to_charge = energy_table[i].v_e65;   break;
        case  75 : voltage_to_charge = energy_table[i].v_e75;   break;
        case  90 : voltage_to_charge = energy_table[i].v_e90;   break;
        case 150 : voltage_to_charge = energy_table[i].v_e150;  break;
        case 175 : voltage_to_charge = energy_table[i].v_e175;  break;
        case 200 : voltage_to_charge = energy_table[i].v_e200;  break;

        // this instruction can not be executed --> Lock the AED FSM !!!
        default  : return;
    }

    // increment the number of full charges (if proceed) !!!
    my_voltage = Defib_Get_Vc();
    if (my_voltage < (PRE_CHARGE_VOLTAGE - 100))
    {
        sMon_Inc_FullCharges ();
    }

    // command the defibrillator to charge the main capacitor !!!
    Send_Defib_Queue_mock (eEV_DEF_CHARGE);

    // wait to process the command in the defibrillator thread
    //tx_thread_sleep (OSTIME_50MSEC);
    printf("Sleep\n");
}

/******************************************************************************
 ** Name:    Defib_Cmd_Shock
 *****************************************************************************/
/**
 ** @brief   Command a shock
 **
 ** @param   rythm_type     type of the rythm
 **
 ** @return  none
 ******************************************************************************/
void Defib_Cmd_Shock_mock (RYTHM_TYPE_e rythm_type)
{
	printf("Defib_Cmd_Shock_mock\n");
    // update the ZP value ...
    zp_when_shock = (uint16_t) patMon_Get_Zp_Ohms_mock();

    // command the defibrillator to shock the patient !!!
    Send_Defib_Queue_mock((rythm_type == eRYTHM_SHOCKABLE_SYNC) ? eEV_DEF_SHOCK_SYNC : eEV_DEF_SHOCK_ASYNC);

    // Do not disturb ... and wait for shock done condition ...
    do {
        //tx_thread_sleep (OSTIME_40MSEC);
        printf("Sleep\n");
    } while (Defib_Get_State() == eDEFIB_STATE_SHOCKING);
}

/******************************************************************************
 ** Name:    thread_defibrillator_entry
 *****************************************************************************/
/**
 ** @brief   A description of the function
 ** @param   void
 **
 ** @return  void
 ** @todo    pending things to be done
 ******************************************************************************/
void thread_defibrillator_entry_mock(void)
{
    DEFIB_STATE_e   my_state;       // defibrillator state
    EVENT_ID_e      my_ev;
    UINT            status;

    printf("thread_defibrillator_entry_mock\n");
    // initializes the defibrillation controller
    Defib_Initialize_mock ();

    // defibrillator Loop
    while (1)
    {
        // Wait for an event and send it to the statechart
      //  status = tx_queue_receive(&queue_def, &my_ev, OSTIME_20MSEC);
        if (status == TX_QUEUE_EMPTY)
        {
            Defib_Monitor_mock ();
        }
        else if (status == TX_SUCCESS)
        {
            // ignore the event if the defibrillator is in Error or out of service ...
            my_state = Defib_Get_State();
            if ((my_state == eDEFIB_STATE_IN_ERROR) || (my_state == eDEFIB_STATE_OUT_OF_SERVICE))
            {
                // discharge to zero using the Relay
                Defib_Disarm_mock();
                my_ev = eEV_NULL;
            }

            // process the event
            switch(my_ev)
            {
            case eEV_DEF_PRECHARGE:
                Defib_Charge_mock(PRE_CHARGE_VOLTAGE, 1000);     // soft charge (1 Amp)
                break;
            case eEV_DEF_CHARGE:
                Defib_Charge_mock(voltage_to_charge, 4000);      // fast charge (4 Amp)
                break;
            case eEV_DEF_SHOCK_SYNC:
                Defib_Shock_Sync_mock (zp_when_shock);
                break;
            case eEV_DEF_SHOCK_ASYNC:
                Defib_Shock_mock (zp_when_shock);
                break;
            case eEV_DEF_DISARM:
                Defib_Charge_mock (0, 0);
                break;
            default:
                break;
            }
        }
    }
}
