/*
 * thread_defibrillator_entry.h
 *
 *  Created on: 6 jun. 2018
 *      Author: ltorres
 */

#ifndef THREAD_DEFIBRILLATOR_ENTRY_H_
#define THREAD_DEFIBRILLATOR_ENTRY_H_

#include "thread_drd_entry.h"

// function prototypes ...
extern void Defib_Cmd_Precharge_mock (bool_t force);
extern void Defib_Cmd_Disarm_mock (void);
extern void Defib_Cmd_Charge_mock (uint16_t energy);
extern void Defib_Cmd_Shock_mock  (RYTHM_TYPE_e rythm_type);


#endif /* THREAD_DEFIBRILLATOR_ENTRY_H_ */
