/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_core_hal.h
 * @brief       Header with functions related to the core BSP
 *
 * @version     v1
 * @date        16/01/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef THREAD_CORE_HAL_H_
#define THREAD_CORE_HAL_H_

/******************************************************************************
 **Includes
 */
#include "device_init.h"
#include "types_app.h"
#include "event_ids.h"

#include "thread_sysMon_entry_Mock.h"


/******************************************************************************
 ** defines
 */

#define     LED_VALUE_OFF   IOPORT_LEVEL_LOW
#define     LED_VALUE_ON    IOPORT_LEVEL_HIGH

/******************************************************************************
 ** typedefs
 */

/******************************************************************************
 ** globals
 */
extern  WORKING_MODE_e       curr_working_mode;

extern  DEVICE_SETTINGS_t*   pDevice_Settings;
extern  AED_SETTINGS_t*      pAed_Settings;
extern  MISC_SETTINGS_t*     pMisc_Settings;


/******************************************************************************
 ** prototypes
 */


extern  void        FSM_R100_AED_mock (EVENT_ID_e aed_event);

extern  void        Pastime_While_Audio_Playing_mock                  (void);
extern  RCP_BREAK_t Pastime_While_Audio_Playing_with_Break_PT_mock    (uint32_t delay);
extern  RCP_BREAK_t Pastime_While_Audio_Playing_with_Break_PT_OC_mock (void);
extern  RCP_BREAK_t Pastime_While_Audio_Playing_with_Break_PT_CC_mock (void);

extern  bool_t      Is_Patient_Type_Change (void);
extern  uint8_t     Get_RCP_Rate (void);
extern  void        Send_Core_Queue_mock(EVENT_ID_e ev);

/******************************************************************************
 ** Prototypes
 */
void     AED_Mode_mock                                     (void);
void     CFG_Mode_mock                                     (void);
bool_t   USB_Device_Connected_mock                         (void);

#endif /* THREAD_CORE_HAL_H_ */

/*
 ** $Log$
 **
 ** end of thread_core_hal.h
 ******************************************************************************/
