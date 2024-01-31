/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        Comm_Trace.h
 * @brief       Header with functions related to the output trace through serial
 *              port
 *
 * @version     v1
 * @date        16/01/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef TRACE_H_
#define TRACE_H_

/******************************************************************************
 **Includes
 */

#include "../types_basic_Mock.h"

#include <stdio.h>



/******************************************************************************
 ** defines
 */

// FLAGS
#define TRACE_NO_FLAGS              0x00
#define TRACE_TIME_STAMP            0x01
#define TRACE_NEWLINE               0x02
#define TRACE_DISABLED              0x04

/******************************************************************************
 ** typedefs
 */

/******************************************************************************
 ** globals
 */

/******************************************************************************
 ** prototypes
 */

extern void     Trace_Init_mock (void);
extern void     Trace_mock      (uint8_t flags, const char *pString);
extern void     Trace_Arg_mock  (uint8_t flags, const char *pString, const uint32_t aux_code);
extern void     Trace_Char_mock (const char_t data_to_send);
extern uint8_t  Boot_Send_Message_mock (const uint8_t *pMsg, uint32_t size);
extern uint8_t  Boot_Receive_Message_mock (void);
extern void     Boot_Sync_mock (uint8_t nBeacons);

#ifdef UNIT_TESTS
void Trace_Test (const char_t data_to_send);
#endif

#endif /* TRACE_H_ */

/*
 ** $Log$
 **
 ** end of Comm_Trace.h
 ******************************************************************************/
