/******************************************************************************
 * Name      : S3A7_REANIBEX_100                                              *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : MinGW32                                                        *
 * Target    : Reanibex Series                                                *
 ******************************************************************************/

/*!
 * @file        nx_secure.h
 * @brief       NetX secure for check certificates
 *
 * @version     v1
 * @date        08/10/2021
 * @author      ivicente
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

/******************************************************************************
 ** Defines
 */
#define     CERT_VALID          0       // The ertificate is valid
#define     CERT_EXP_SOON       1       // The certificate will expire soon
#define     CERT_NOT_VALID      2       // The ertificate is not valid

#define     WPA_EAP_CERT_FILENAME   "server.der"
#define     TLS_CA_CERT_FILENAME    "tls_ca.der"

/******************************************************************************
 ** Includes
 */



#include "HAL/thread_comm_hal_Mock.h"
#include "device_init.h"
#include "resources/Trace_Mock.h"

#include "R100_Errors.h"
//#include "types_basic.h"
#include "types_app.h"
#include "Resources/Trace_Mock.h"

#include "HAL/thread_defibrillator_hal_Mock.h"
#include "HAL/thread_patMon_hal_Mock.h"
#include "HAL/thread_audio_hal_Mock.h"
#include "thread_comm_entry.h"
#include "thread_patMon_entry.h"
#include "thread_sysMon_entry_Mock.h"
#include "synergy_gen_Mock/thread_sysMon_Mock.h"
#include "thread_audio_entry.h"
#include "synergy_gen_Mock/thread_audio_Mock.h"
#include "thread_core_entry_Mock.h"
#include "synergy_gen_Mock/thread_core_Mock.h"

#include "synergy_gen_Mock/thread_comm_Mock.h"
#include "thread_api.h"


/******************************************************************************
 ** prototypes
 */

void     nx_init_mock(void);
uint16_t check_TLS_Cacert(void);
uint16_t check_WPA_EAP_Cert(void);
