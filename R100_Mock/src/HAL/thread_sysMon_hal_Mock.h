/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_systemMonitor_hal.h
 * @brief       Header with functions related to the systemMon BSP service
 *
 * @version     v1
 * @date        16/01/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef THREAD_SYSMON_HAL_H_
#define THREAD_SYSMON_HAL_H_

/******************************************************************************
 **Includes
 */
#include "../types_basic_Mock.h"

/******************************************************************************
 ** defines
 */

/******************************************************************************
 ** typedefs
 */

/******************************************************************************
 ** globals
 */

/******************************************************************************
 ** prototypes
 */

bool_t sysMon_USB_Device_Connected(void);

#endif /* THREAD_SYSMON_HAL_H_ */

/*
 ** $Log$
 **
 ** end of thread_sysMon_hal.h
 ******************************************************************************/
