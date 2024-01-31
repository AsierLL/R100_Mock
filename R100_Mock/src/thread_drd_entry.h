/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_drd_entry
 * @brief       Header with functions related to the drd task
 *
 * @version     v1
 * @date        16/01/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef THREAD_DRD_ENTRY_H_
#define THREAD_DRD_ENTRY_H_

/******************************************************************************
 **Includes
 */
#include "DRD/drd_Mock.h"
#include "synergy_gen_Mock/thread_drd_Mock.h"

/******************************************************************************
 ** defines
 */
#define DRD_DIAG_FLAG ((ULONG)0x0001)

/******************************************************************************
 ** typedefs
 */
typedef enum{
     eDRD_DIAG_NOT_READY = 0,       ///< not ready
     eDRD_DIAG_UNKNOWN,             ///< not known
     eDRD_DIAG_NON_SHOCK,           ///< non shockable
     eDRD_DIAG_NON_SHOCK_ASYSTOLE,  ///< asystole (no shockable)
     eDRD_DIAG_SHOCK_ASYNC,         ///< shockable with asynchronous shock
     eDRD_DIAG_SHOCK_SYNC           ///< shockable with synchronous shock
}DRD_DIAG_e;

typedef enum {
    eRYTHM_UNKNOWN = 0,             ///< unknown rythm type
    eRYTHM_NON_SHOCKABLE,           ///< non shockable rythm type
    eRYTHM_SHOCKABLE_SYNC,          ///< shockable rythm type (synchronous)
    eRYTHM_SHOCKABLE_ASYNC,         ///< shockable rythm type (asynchronous)
} RYTHM_TYPE_e;

/******************************************************************************
 ** globals
 */

/******************************************************************************
 ** prototypes
 */
extern  void        DRD_Start_Analysis  (void);
extern  void        DRD_Stop_Analysis   (void);
extern  void        DRD_Diag_Ready      (void);
extern  bool_t      DRD_Is_Running      (void);
extern  DRD_DIAG_e  DRD_Get_Diagnosis_mock   (DRD_RESULT_t *p_DRD_Result);
extern  uint32_t    DRD_Get_Diag_nSample(void);

#endif /* THREAD_DRD_ENTRY_H_ */

/*
 ** $Log$
 **
 ** end of thread_drd_entry.h
 ******************************************************************************/
