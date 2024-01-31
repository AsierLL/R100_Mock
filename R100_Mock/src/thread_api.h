/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_api.h
 * @brief       Header with functions related to thread APIs
 *
 * @version     v1
 * @date        16/01/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef THREAD_API_H
#define THREAD_API_H
/******************************************************************************
**Includes
*/

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

extern TX_THREAD thread_audio;          // thread descriptor
extern TX_THREAD thread_core;           // thread descriptor
extern TX_THREAD thread_drd;            // thread descriptor
extern TX_THREAD thread_hmi;            // thread descriptor
extern TX_THREAD thread_patMon;         // thread descriptor
extern TX_THREAD thread_sysMon;         // thread descriptor
extern TX_THREAD thread_defibrillator;  // thread descriptor
extern TX_THREAD thread_recorder;       // thread descriptor
extern TX_THREAD thread_comm;           // thread descriptor

#endif  /*THREAD_API_H*/

/*
** $Log$
**
** end of thread_api.h
******************************************************************************/
