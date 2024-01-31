/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_core_entry.c
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
#include "device_init.h"

#include "Resources/Trace_Mock.h"
//#include "types_basic.h"
#include "types_app.h"
#include "thread_core_entry_Mock.h"
#include "thread_audio_entry.h"
#include "thread_drd_entry.h"
#include "thread_patMon_entry.h"
#include "thread_sysMon_entry_Mock.h"
#include "thread_comm_entry.h"

#include "HAL/thread_core_hal.h"
#include "synergy_gen_Mock/thread_core_Mock.h"
#include "thread_api.h"
#include "HAL/thread_sysMon_hal_Mock.h"
#include "Drivers/RTC.h"
#include "Resources/Keypad_Mock.h"
#include "event_ids.h"
#include "types_basic_Mock.h"
#include "DB_Episode_Mock.h"
#include "HAL/thread_audio_hal_Mock.h"
#include "HAL/thread_defibrillator_hal_Mock.h"

/******************************************************************************
 ** Macros
 */

/******************************************************************************
 ** Defines
 */
#define     CDCACM_FLAG     ((ULONG)0x0001)

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
WORKING_MODE_e      curr_working_mode = eWMODE_NOMODE;

DEVICE_SETTINGS_t*  pDevice_Settings;
AED_SETTINGS_t*     pAed_Settings;
MISC_SETTINGS_t*    pMisc_Settings;

/******************************************************************************
 ** Locals
 */
// USB CDC Device
static bool_t  patient_type_change = false;             // patient type change indicator
static DEVICE_INFO_t         my_Device_Info;          // pointer to the device info




/******************************************************************************
** Name:    Update_Patient_Type_Led
*****************************************************************************/
/**
** @brief   updates the patient type led
**
** @param   none
**
** @return  none
**
******************************************************************************/
static void Update_Patient_Type_Led_mock (void)
{
	printf("Update_Patient_Type_Led_mock\n");
    if (pMisc_Settings->glo_patient_adult)
    {
        Led_Off_mock (2);
    }
    else {
        Led_On_mock (2);
    }
}

/******************************************************************************
** Name:    Power_Off
*****************************************************************************/
/**
** @brief   controlled Power off the device
**
** @param   none
**
** @return  none
**
******************************************************************************/
static void Power_Off_mock (bool_t usb)
{

	printf("Power_Off_mock\n");
    // Write electrodes information in test file
    PowerOff_Write_Elec_mock();

    //Check device status
    Check_Device_Led_Status();

    // Do not detect more Cover close event
 //   tx_thread_terminate (&thread_drd);
  //  tx_thread_terminate (&thread_hmi);

    if (Is_Mode_Trainer())
    {
        //Play a message to inform the user
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_TRAINER_MODE, TRUE);
        do { printf("Sleep\n");//tx_thread_sleep (OSTIME_100MSEC);
        } while (Is_Audio_Playing_mock ());
    }

    if (usb)
    {
        if (Electrodes_Get_Signature() != eEL_SIGN_NONE)
        {
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_DISCONNECT_PATIENT, TRUE);
            do { printf("Sleep\n");//tx_thread_sleep (OSTIME_100MSEC);
            } while (Is_Audio_Playing_mock ());
            }
        }

    // shut-up --> be sure that the STOP command is executed !!!
    Audio_Message (eAUDIO_CMD_STOP, eAUDIO_ASYSTOLE, TRUE);

    NFC_Write_Device_Info_mock(true);
    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, ">>> KILL");

    // application tidy closure
   // tx_thread_terminate (&thread_audio);

    // proceed to discharge the main capacitor (just in case)
    Defib_Charge_mock(0, 0);

    // wait to finish all threads ...
    // real switch off with a voltage lower than 400V in the main capacitor
    //tx_thread_sleep (OSTIME_100MSEC);
    printf("Sleep\n");
    while (Defib_Get_Vc() > 400)
    {
        // refresh the supervisor ...
        thread_supervisor |= THREAD_ID_CORE;
      //  tx_thread_sleep (OSTIME_200MSEC);
        printf("Sleep\n");
        Defib_Charge_mock(0, 0);
    }

    // check if the Wifi option is enabled
   if (my_Device_Info.enable_b.wifi)
   {
       // close the episode
       DB_Episode_Set_Event_mock (eREG_WIFI_POWER_OFF);
       while(DB_Is_Episode_Sent() == FALSE)
       {
          // tx_thread_sleep (OSTIME_500MSEC);
           printf("Sleep\n");
       }
       //tx_thread_sleep (OSTIME_500MSEC);
       printf("Sleep\n");
   }
   else
   {
       // close the episode
       DB_Episode_Set_Event_mock (eREG_POWER_OFF);
       //tx_thread_sleep (OSTIME_500MSEC);
       printf("Sleep\n");
   }

    // suspend the remaining threads
   /* tx_thread_terminate (&thread_patMon);
    tx_thread_terminate (&thread_sysMon);
    tx_thread_terminate (&thread_defibrillator);
    tx_thread_terminate (&thread_recorder);
    tx_thread_terminate (&thread_comm);*/

    // close fx_media before power down
    //fx_media_close(&sd_fx_media);
    //tx_thread_sleep (OSTIME_200MSEC);
    printf("Sleep\n");

    // reprogram the auto-test depending on the manual power off
    R100_Program_Autotest();

    // this function ends the execution !!!
    R100_PowerOff_mock ();
}

/******************************************************************************
** Name:    Process_Key
*****************************************************************************/
/**
** @brief   Process the user pressed key
**
** @param   none
**
** @return  none
**
******************************************************************************/
static void Process_Key_mock (EVENT_ID_e my_event)
{
    REG_EVENTS_e ev_id;     // event to register
    uint32_t     status;    // queue status
    printf("Process_Key_mock\n");

    switch(my_event)
    {
        case eEV_KEY_PATYPE:
            pMisc_Settings->glo_patient_adult ^= 0x01;
            ev_id = (pMisc_Settings->glo_patient_adult) ? eREG_SET_PATIENT_ADULT : eREG_SET_PATIENT_PEDIATRIC;
            DB_Episode_Set_Event_mock(ev_id);
            Update_Patient_Type_Led_mock ();
            patient_type_change = true;
            break;
        case eEV_KEY_ONOFF:
        case eEV_USB:
//            Led_Off (LED_ONOFF);
            Led_Off_mock (1);
            Led_Off_mock (2);
            Power_Off_mock(my_event == eEV_USB);
            break;
        case eEV_KEY_COVER:
            if (Check_Impedance() == eEV_PAT_GOOD_CONN) break; // if patient connected do not power off!
//            Led_Off (LED_ONOFF);
            Led_Off_mock (1);
            Led_Off_mock (2);
            Power_Off_mock(false);
            break;
        case eEV_KEY_SHOCK:
//          the shock key is processed in the AED state machine !!!
//          Led_Off (LED_SHOCK);
            break;
        case eEV_LOCK_CORE:
            //////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////
            // virtual key used to lock the thread and the FSM !!!
            while (1)
            {
                // refresh the supervisor ...
                thread_supervisor |= THREAD_ID_CORE;

                // Get a message from the queue (process ONLY events to switch off the device)
        //        status = tx_queue_receive(&queue_core, &my_event, 100);
                if ((status == TX_SUCCESS) && ((my_event == eEV_KEY_ONOFF) || (my_event == eEV_KEY_COVER)))
                {
                    Process_Key_mock (my_event);
                }
            }
            break;
        default:
            break;
    }
}

/******************************************************************************
 ** Name:    Is_Patient_Type_Change
 *****************************************************************************/
/**
 ** @brief   Check if patient type has changed
 **
 ** @param   void
 **
 ** @return  The connected electrodes type
 ******************************************************************************/
bool_t Is_Patient_Type_Change (void)
{
    bool_t  aux;

    aux = patient_type_change;
    patient_type_change = false;    // reset the change flag !!!
    return aux;
}


/******************************************************************************
 ** Name:    Get_RCP_Rate
 *****************************************************************************/
/**
 ** @brief   Return the configured RCP rate
 **
 ** @param   void
 **
 ** @return  RCP rate (100, 110 or 120)
 ******************************************************************************/
uint8_t Get_RCP_Rate (void)
{
//    pAed_Settings
    return (uint8_t)pAed_Settings->aed_metronome_rate;
}

/******************************************************************************
** Name:    AED_Mode
*****************************************************************************/
/**
** @brief   Manages AED operation mode. Sends all received events to the
**          AED statechart.
** @param   void
**
** @return  void
**
******************************************************************************/
void AED_Mode_mock (void)
{
    uint32_t    status;
    EVENT_ID_e  my_event;

    printf("AED_Mode_mock\n");
    // LED continuous ON ...
    Led_On_mock (0);

    // Resume all threads
 /*   tx_thread_resume (&thread_audio);
    tx_thread_resume (&thread_patMon);
    tx_thread_resume (&thread_defibrillator);
    tx_thread_resume (&thread_recorder);
    tx_thread_resume (&thread_drd);
    tx_thread_resume (&thread_hmi);
    tx_thread_resume (&thread_comm);*/


    while (Is_Sysmon_Task_Initialized() == false)
    {
        //tx_thread_sleep (OSTIME_100MSEC);
        printf("Sleep/n");
    }

    // Audio task must be initialized
    while (Is_Task_Audio_Enabled() == false)
    {
        //tx_thread_sleep (OSTIME_100MSEC);
        printf("Sleep\n");
    }

//    // get the device info
    memcpy(&my_Device_Info, Get_Device_Info(), sizeof(DEVICE_INFO_t));
//
//    // check if the Wifi option is enabled
//    if (my_Device_Info.enable_b.wifi)
//    {
//        Comm_Wifi_Open();
//    }

    // Force AED state chart to start
    FSM_R100_AED_mock (eEV_AED_START);

    while(1)
    {
        // refresh the supervisor ...
        thread_supervisor |= THREAD_ID_CORE;

        //Check if defib is OK or in error
        if ((Defib_Get_State() != eDEFIB_STATE_IN_ERROR) && (Defib_Get_State() != eDEFIB_STATE_OUT_OF_SERVICE))
        {
            // Check electrodes connection
            my_event = Check_Impedance();
            FSM_R100_AED_mock (my_event);
        }

        // Get a message from the queue
   //     status = tx_queue_receive(&queue_core, &my_event, 100);
        if (status == TX_SUCCESS)
        {
            Process_Key_mock (my_event);
            FSM_R100_AED_mock (my_event);
        }
    }
}

/******************************************************************************
** Name:    Pastime_While_Audio_Playing
*****************************************************************************/
/**
** @brief   pass time while the audio message is playing
** @param   void
**
** @return  void
**
******************************************************************************/
void Pastime_While_Audio_Playing_mock (void)
{
    uint32_t    status;
    EVENT_ID_e  my_event;

    printf("Pastime_While_Audio_Playing_mock\n");

    do {
        // refresh the supervisor ...
        thread_supervisor |= THREAD_ID_CORE;

        // Get a message from the queue
     //   status = tx_queue_receive(&queue_core, &my_event, 50);
        if (status == TX_SUCCESS)
        {
            Process_Key_mock (my_event);
        }
    } while (Is_Audio_Playing_mock());
/*
    // if long messages are ON then leave max 1 second after message for better pacing
    if (pAed_Settings->aed_cpr_msg_long)
    {
        for (uint32_t idx = 0; idx<20; idx++)
        {
            // refresh the supervisor ...
            thread_supervisor |= THREAD_ID_CORE;

            // Get a message from the queue
            status = tx_queue_receive(&queue_core, &my_event, OSTIME_50MSEC);
            if (status == TX_SUCCESS)
            {
                Process_Key (my_event);
            }
        }
    }*/

}
/******************************************************************************
** Name:    Pastime_While_Audio_Playing
*****************************************************************************/
/**
** @brief   pass time while the audio message is playing
** @param   void
**
** @return  void
**
******************************************************************************/
RCP_BREAK_t Pastime_While_Audio_Playing_with_Break_PT_mock (uint32_t delay)
{
    uint32_t    status;
    uint32_t    idx = 0;
    EVENT_ID_e  my_event;

    printf("Pastime_While_Audio_Playing_with_Break_PT_mock\n");
    do {
        // refresh the supervisor ...
        thread_supervisor |= THREAD_ID_CORE;

        // Get a message from the queue
   //     status = tx_queue_receive(&queue_core, &my_event, 50);
        if (status == TX_SUCCESS)
        {
            Process_Key_mock (my_event);
        }

        // check if the patient type has been changed
        if (Is_Patient_Type_Change() == true) return RCP_BREAK_PATIENT_TYPE;

    } while (Is_Audio_Playing_mock());

    //leave "delay" after message
    if (delay)
    {
        idx = 0;
        while (idx<(delay/50))
        {
            // refresh the supervisor ...
            thread_supervisor |= THREAD_ID_CORE;

            // Get a message from the queue
      //      status = tx_queue_receive(&queue_core, &my_event, 50);
            if (status == TX_SUCCESS)
            {
                Process_Key_mock (my_event);
            }
            idx++;
        }
    }

    return RCP_BREAK_NONE;
}

/******************************************************************************
** Name:    Pastime_While_Audio_Playing_with_Break_PT_OC
*****************************************************************************/
/**
** @brief   pass time while the audio message is playing
** @param   void
**
** @return  if the loop has been break or not
**             RCP_BREAK_NONE         --> no break
**             RCP_BREAK_ZP           --> break due to impedance change
**             RCP_BREAK_PATIENT_TYPE --> break due to patient type change
**
******************************************************************************/
RCP_BREAK_t Pastime_While_Audio_Playing_with_Break_PT_OC_mock (void)
{
    uint32_t    status;
    EVENT_ID_e  my_event;

    printf("Pastime_While_Audio_Playing_with_Break_PT_OC_mock\n");
    do {
        // refresh the supervisor ...
        thread_supervisor |= THREAD_ID_CORE;

        // Get a message from the queue
   //     status = tx_queue_receive(&queue_core, &my_event, 50);
        if (status == TX_SUCCESS)
        {
            Process_Key_mock (my_event);
        }

        // check if the Zp value has a good connection
        if (patMon_Get_Zp_Segment () != eZP_SEGMENT_GOOD_CONN) return RCP_BREAK_ZP;

        // check if the patient type has been changed
        if (Is_Patient_Type_Change() == true) return RCP_BREAK_PATIENT_TYPE;

    } while (Is_Audio_Playing_mock());


    return RCP_BREAK_NONE;
}

/******************************************************************************
** Name:    Pastime_While_Audio_Playing_with_Break_PT_CC
*****************************************************************************/
/**
** @brief   pass time while the audio message is playing
** @param   void
**
** @return  if the loop has been break or not
**             RCP_BREAK_NONE         --> no break
**             RCP_BREAK_ZP           --> break due to impedance change
**             RCP_BREAK_PATIENT_TYPE --> break due to patient type change
**
******************************************************************************/
RCP_BREAK_t Pastime_While_Audio_Playing_with_Break_PT_CC_mock (void)
{
    uint32_t    status;
    EVENT_ID_e  my_event;

    printf("Pastime_While_Audio_Playing_with_Break_PT_CC_mock\n");

    do {
        // refresh the supervisor ...
        thread_supervisor |= THREAD_ID_CORE;

        // Get a message from the queue
  //      status = tx_queue_receive(&queue_core, &my_event, 50);
        if (status == TX_SUCCESS)
        {
            Process_Key_mock (my_event);
        }

        // check if the Zp value has a good connection
        if (patMon_Get_Zp_Segment () == eZP_SEGMENT_GOOD_CONN) return RCP_BREAK_ZP;

        // check if the patient type has been changed
        if (Is_Patient_Type_Change() == true) return RCP_BREAK_PATIENT_TYPE;

    } while (Is_Audio_Playing_mock());

    return RCP_BREAK_NONE;
}

/******************************************************************************
** Name:    CFG_Mode
*****************************************************************************/
/**
** @brief   Manages device configuration mode
** @param   void
**
** @return  void
**
******************************************************************************/
void CFG_Mode_mock (void)
{
    // Initialize the USBX mass storage device class
    ux_common_init0_mock();
    ux_device_init0_mock();
    ux_device_class_storage_init0_mock();

   /* while(1)
    {
        // refresh the supervisor ...
        thread_supervisor |= THREAD_ID_CORE;
        //tx_thread_sleep (OSTIME_500MSEC);
        printf("Sleep\n");
    }*/
    printf("CFG_Mode On Mock\n");
}

/******************************************************************************
 ** Name:    USB_Device_Connected
 *****************************************************************************/
/**
 ** @brief   Return if a USB device is connected
 ** @param   void
 **
 ** @return  false : USB device not connected
 **          true  : USB device connected
 **
 ******************************************************************************/
bool_t USB_Device_Connected_mock()
{
    // check VBUS_DET signal to identify an USB host presence !!!
	printf("USB_Device_Connected_mock \n");
    return 1;
}

/******************************************************************************
 ** Name:    thread_core_entry
 *****************************************************************************/
/**
 ** @brief   Main task. Executes both, AED and CFG, operation modes.
 ** @param   void
 **
 ** @return  void
 ******************************************************************************/
void thread_core_entry_mock(void)
{

	printf("thread_core_entry_mock\n");
    // get pointers to all device settings
    pAed_Settings    = Get_AED_Settings();
    pMisc_Settings   = Get_Misc_Settings();
    pDevice_Settings = Get_Device_Settings();

    // refresh the supervisor ...
    thread_supervisor |= THREAD_ID_CORE;

    if (Is_Test_Mode_Montador())
    {
       /* while(1)
        {
            // refresh the supervisor ...
            thread_supervisor |= THREAD_ID_CORE;
            //tx_thread_sleep (OSTIME_500MSEC);
            printf("Sleep\n");
        }*/
    	 printf("Is_Test_Mode_Montador\n");
    }
    else if (USB_Device_Connected_mock())
    {
        Trace_mock ( (TRACE_TIME_STAMP | TRACE_NEWLINE), "CFG working mode... ");
        curr_working_mode = eWMODE_CFG;
        CFG_Mode_mock();
    }
    else if (Is_Test_Mode())
    {
        while(1)
        {
            // refresh the supervisor ...
            thread_supervisor |= THREAD_ID_CORE;
            //tx_thread_sleep (OSTIME_500MSEC);
            printf("Sleep\n");
        }
    }
    else
    {
        Trace_mock ( (TRACE_TIME_STAMP | TRACE_NEWLINE), "AED working mode... ");

        Update_Patient_Type_Led_mock ();
        curr_working_mode = eWMODE_AED;
        AED_Mode_mock();
    }
}

/******************************************************************************
** Name:    Send_Core_Queue
*****************************************************************************/
/**
** @brief   Process events related to DRD
**
** @param   ev event identifier
**
** @return  if the event has been processed or not
**
******************************************************************************/
void Send_Core_Queue_mock(EVENT_ID_e ev)
{

	printf("Send_Core_Queue_mock\n");
//    Trace_Arg (TRACE_NEWLINE, "  *****************QUEUE CORE AVAILABLE = %5d", (uint32_t) (queue_core.tx_queue_available_storage));
    if (queue_core.tx_queue_available_storage == 0) return;
  //  tx_queue_send(&queue_core, &ev, TX_WAIT_FOREVER);

}
