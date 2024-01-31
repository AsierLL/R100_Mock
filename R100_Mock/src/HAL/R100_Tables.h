/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        R100_Tables.h
 * @brief       Header with ZP and Pulse tables definitions
 *
 * @version     v1
 * @date        16/01/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef R100_TABLES_H_
#define R100_TABLES_H_

/******************************************************************************
 **Includes
 */

#include "../types_basic_Mock.h"


/******************************************************************************
 ** defines
 */

#define MAX_ZP_ADCS             0x00FFFFFF  // maximum value for ZP in ADCs
#define MAX_ZP_OHMS             9999        // maximum value for ZP in Ohms

#define ZP_LIMIT_FOR_VALID_ECG  (zp_table[15].adcs)     // limit to consider (about 300 ohms)
#define ZP_FOR_CALIBRATION      (zp_table[32].adcs)     // empiric assignment to link the calibration ADCs to the closest zp_table value


/******************************************************************************
 ** typedefs
 */

///< Define the ZP Table structure
typedef struct ZP_TABLE_STRUCT
{
    uint32_t    ohms;           ///< impedance value in ohms
    uint32_t    adcs;           ///< impedance value in ADCs
} ZP_TABLE_t;
 
///< Define the Energy Table structure
typedef struct ENERGY_TABLE_STRUCT
{
    uint16_t    zp_ohms;        ///< impedance value in ohms
    uint16_t    t1_time;        ///< T1 time value in usecs
    uint16_t    v_e50;          ///< voltage capacitor for  50 Joules
    uint16_t    v_e65;          ///< voltage capacitor for  65 Joules
    uint16_t    v_e75;          ///< voltage capacitor for  75 Joules
    uint16_t    v_e90;          ///< voltage capacitor for  90 Joules
    uint16_t    v_e150;         ///< voltage capacitor for 150 Joules
    uint16_t    v_e175;         ///< voltage capacitor for 175 Joules
    uint16_t    v_e200;         ///< voltage capacitor for 200 Joules
} ENERGY_TABLE_t;


/******************************************************************************
 ** globals
 */
extern const ZP_TABLE_t     zp_table       [];    // Zp Table to convert ADCs to Ohms
extern const int32_t        zp_temperature [];    // Zp calibration variation due to temperature
extern const ENERGY_TABLE_t energy_table   [];    // Energy table to assign times and voltages
 
/******************************************************************************
 ** prototypes
 */


#endif /* R100_TABLES_H_ */

/*
 ** $Log$
 **
 ** end of R100_Tables.h
 ******************************************************************************/
