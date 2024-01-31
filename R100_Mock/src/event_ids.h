/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        event_ids.h
 * @brief       Event identifiers header file
 *
 * @version     v1
 * @date        21/05/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef EVENT_IDS_H_
#define EVENT_IDS_H_

/******************************************************************************
 **Includes
 */

/******************************************************************************
 ** defines
 */

/******************************************************************************

 ** typedefs
 */

// machine events
typedef enum
{
    eEV_NULL = 0,
    eEV_AED_START,              // command the AED_FSM
    eEV_ERROR,                  // ----
    eEV_POWER_OFF,              // ----

    eEV_EL_NONE,                // ----
    eEV_PAT_BAD_CONN,           // command the AED_FSM
    eEV_PAT_GOOD_CONN,          // command the AED_FSM
    eEV_PAT_OPEN_CIRC,          // command the AED_FSM
    eEV_PAT_SHORT_CIRC,         // command the AED_FSM
    eEV_PAT_UNKNOWN,            // command the AED_FSM

    eEV_DRD_START,              // command the DRD task
    eEV_DRD_STOP,               // command the DRD task
    eEV_DRD_ANALYZE,            // command the DRD task
    eEV_DRD_DIAG_RDY,           // command the DRD task

    eEV_DEF_PRECHARGE,          // command the defibrillator task to precharge
    eEV_DEF_CHARGE,             // command the defibrillator task to charge
    eEV_DEF_SHOCK_SYNC,         // command the defibrillator task to shock
    eEV_DEF_SHOCK_ASYNC,        // command the defibrillator task to shock
    eEV_DEF_DISARM,             // command the defibrillator tast to disarm

    eEV_KEY_ONOFF,              // when Key ON/OFF is pressed
    eEV_KEY_SHOCK,              // when Key SHOCK  is pressed
    eEV_KEY_PATYPE,             // when Key patient type is pressed
    eEV_KEY_COVER,              // when the cover is closed
    eEV_LOCK_CORE,              // command the lock the Core task
    eEV_USB,					// when USB is detected in patient mode

} EVENT_ID_e;

// registered events
typedef enum
{
    eREG_NULL = 0,

    eREG_CPR_START,
    eREG_DRD_START,
    eREG_DRD_DIAG_RDY,

    eREG_DEF_CHARGE,

    eREG_SHOCK_1_ADULT,
    eREG_SHOCK_2_ADULT,
    eREG_SHOCK_3_ADULT,
    eREG_SHOCK_1_PEDIATRIC,
    eREG_SHOCK_2_PEDIATRIC,
    eREG_SHOCK_3_PEDIATRIC,
    eREG_SHOCK_NOT_DELIVERED,

    eREG_SET_PATIENT_ADULT,
    eREG_SET_PATIENT_PEDIATRIC,

    eREG_ECG_PAT_OPEN_CIRC,
    eREG_ECG_PAT_BAD_CONN,
    eREG_ECG_PAT_SHORT_CIRC,
    eREG_ECG_PAT_GOOD_CONN,
    eREG_ECG_EL_NONE,
    eREG_ECG_EL_UNKNOWN,
    eREG_ECG_EL_BEXEN,
    eREG_ECG_EL_EXPIRED,
    eREG_ECG_EL_USED,

    eREG_DEF_ERROR_CHARGING,
    eREG_DEF_ERROR_SHOCK,

    eREG_BAT_UNKNOWN,                    // battery not recognized
    eREG_BAT_LOW_BATTERY,                // low charge in battery
    eREG_BAT_VERY_LOW_BATTERY,           // very low voltage in battery voltage
    eREG_BAT_LOW_TEMPERATURE,            // low temperature
    eREG_BAT_HIGH_TEMPERATURE,           // high temperature
    eREG_BAT_VERY_LOW_TEMPERATURE,       // very low temperature
    eREG_BAT_VERY_HIGH_TEMPERATURE,     // very high temperature

    eREG_SAT_ERROR,
    eREG_ERROR,

    eREG_POWER_OFF,
    eREG_WIFI_POWER_OFF

} REG_EVENTS_e;



/******************************************************************************
 ** globals
 */

/******************************************************************************
 ** prototypes
 */

#endif /* EVENT_IDS_H_ */

/*
 ** $Log$
 **
 ** end of event_ids.h
 ******************************************************************************/
