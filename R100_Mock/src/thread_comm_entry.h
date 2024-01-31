/******************************************************************************
 * Name      : S3A7_REANIBEX_100                                              *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : MinGW32                                                        *
 * Target    : Reanibex Series                                                *
 ******************************************************************************/

/*!
 * @file        thread_comm_entry.h
 * @brief       Header for Comm Thread entry
 *
 * @version     v1
 * @date        03/05/2019
 * @author      jarribas
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */
#ifndef THREAD_COMM_ENTRY_H_
#define THREAD_COMM_ENTRY_H_

/******************************************************************************
 ** Includes
 */
/*lint -save -e537 Spurious warning ignored due to include guards*/
#include "DB_Test_Mock.h"
#include "HAL/thread_comm_hal_Mock.h"
/*lint -restore*/

/******************************************************************************
 ** defines
 */

/******************************************************************************
 ** typedefs
 */

typedef enum
{
    eEV_COMM_MODULES_INITIALIZE = 0,        // Initialize All Comm Uart and Comm Modules

    eEV_COMM_SIGFOX_INITIALIZE,         // Initialize Sigfox Module Only
    eEV_COMM_SIGFOX_SEND_TEST,          // Send Test Result data using Sigfox
    eEV_COMM_SIGFOX_SEND_ALERT,         // Send Funtional Alert data using Sigfox
    eEV_COMM_SIGFOX_SEND_EXP,           // Send battery and electrodes expiration date
    eEV_COMM_SIGFOX_SEND_POSITION,      // Send GPS Position data using Sigfox
    eEV_COMM_SIGFOX_UPLINK_TEST,        // Perform Sigfox uplink Test
    eEV_COMM_SIGFOX_DOWNLINK_TEST,      // Perform Sigfox downlink Test
    eEV_COMM_SIGFOX_EXECUTE_TEST,       // Perform Sigfox Hardware Test

    eEV_COMM_WIFI_INITIALIZE,           // Initialize Wifi Module Only
    eEV_COMM_WIFI_IS_SERVER_ALIVE,      // Send and Receive heartbeat to host using Wifi
    eEV_COMM_WIFI_SEND_FRAME_TEST,      // Send Test Frame using Wifi
    eEV_COMM_WIFI_SEND_FRAME_ALERT,     // Send Alert Frame using Wifi
    eEV_COMM_WIFI_SEND_EXP,             // Send battery and electrodes expiration date
    eEV_COMM_WIFI_SEND_FRAME_GPS,       // Send Gps Frame using Wifi
    eEV_COMM_WIFI_SEND_TEST,            // Send Test File using Wifi
    eEV_COMM_WIFI_SEND_EPISODE,         // Send Episode File using Wifi
    eEV_COMM_WIFI_GET_UPGRADE_FILE,     // Receive Firmware File using Wifi
    eEV_COMM_WIFI_GET_CONFIG_FILE,      // Receive Config File using Wifi
    eEV_COMM_WIFI_EXECUTE_TEST,         // Perform Wifi Hardware Test

    eEV_COMM_GPS_INITIALIZE,
    eEV_COMM_GPS_GET_POS_DATA,
    eEV_COMM_GPS_EXECUTE_TEST,
    eEV_COMM_GPS_SEND_CMD,
    EV_COMM_MAX

} EV_COMM_e;

/******************************************************************************
 ** externs
 */

/******************************************************************************
 ** globals
 */

/******************************************************************************
 ** prototypes
 */

void          Comm_Sigfox_Open                        (void);
void          Comm_Sigfox_Generate_Send_Test          (DB_TEST_RESULT_t *pResult);
void          Comm_Sigfox_Generate_Send_Alert_mock         (SIGFOX_MSG_ID_e alert_id);
void          Comm_Sigfox_Generate_Send_Position      (SIGFOX_MSG_ID_e gps_id);
void          Comm_Sigfox_Generate_Send_Expiration_mock    (DB_TEST_RESULT_t *pResult);
void          Comm_Sigfox_Execute_HW_Test             (void);
bool_t        Comm_Is_Sigfox_Initialized              (void);
bool_t        Comm_Is_Sigfox_Test_Sended              (void);
bool_t        Comm_Is_Sigfox_HW_Test_Finished         (void);
ERROR_ID_e    Comm_Get_Sigfox_HW_Test_Result          (void);
ERROR_ID_e    Comm_Get_Sigfox_Error                   (void);
void          Comm_Set_Sigfox_Test_Sended             (bool_t aux);

void          Comm_Wifi_Open                          (void);
void          Comm_Wifi_Is_Host_Alive                 (void);
void          Comm_Wifi_Generate_Send_Test_Frame      (DB_TEST_RESULT_t *pResult);
void          Comm_Wifi_Generate_Send_Alert_Frame_mock     (WIFI_MSG_ID_e alert_id);
void          Comm_Wifi_Generate_Send_Position_Frame  (WIFI_MSG_ID_e gps_id);
void          Comm_Wifi_Generate_Send_Exp_Frame_mock       (DB_TEST_RESULT_t *pResult);
void          Comm_Wifi_Send_Test                     (void);
void          Comm_Wifi_Send_Episode                  (void);
void          Comm_Wifi_Get_Firmware                  (void);
void          Comm_Wifi_Get_Configuration             (void);
void          Comm_Wifi_Execute_HW_Test               (void);
bool_t        Comm_Is_Wifi_Initialized                (void);
bool_t        Comm_Is_Wifi_Host_Alive_Finished        (void);
bool_t        Comm_Is_Wifi_Test_Frame_Sended          (void);
bool_t        Comm_Is_Wifi_Exp_Frame_Sended           (void);
bool_t        Comm_Is_Wifi_Alert_Frame_Sended         (void);
bool_t        Comm_Is_Wifi_Position_Frame_Sended      (void);
bool_t        Comm_Is_Wifi_Test_Sended                (void);
bool_t        Comm_Is_Wifi_Episode_Sended             (void);
bool_t        Comm_Is_Wifi_Upgrade_Received           (void);
bool_t        Comm_Is_Wifi_Config_Received            (void);
bool_t        Comm_Is_Wifi_HW_Test_Finished           (void);
ERROR_ID_e    Comm_Get_Wifi_HW_Test_Result            (void);
bool_t        Comm_Get_Wifi_Host_Alive_Flag           (void);

void          Comm_GPS_Open                           (void);
void          Comm_GPS_Send_Cmd                       (void);
void          Comm_GPS_Get_Position                   (void);
void          Comm_GPS_Execute_HW_Test                (void);
bool_t        Comm_Is_GPS_HW_Test_Finished            (void);
ERROR_ID_e    Comm_Get_GPS_HW_Test_Result             (void);
ERROR_ID_e    Comm_Is_GPS_Cmd_Send                    (void);

#endif /* THREAD_COMM_ENTRY_H_ */

/*
 ** $Log$
 **
 ** end of thread_comm_entry.h
 ******************************************************************************/
