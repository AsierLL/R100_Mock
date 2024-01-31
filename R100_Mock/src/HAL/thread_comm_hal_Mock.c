/******************************************************************************
 * Name      : S3A7_REANIBEX_100                                              *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : MinGW32                                                        *
 * Target    : Reanibex Series                                                *
 ******************************************************************************/

/*!
 * @file        thread_comm_hal.c
 * @brief       HAL for Comm Thread
 *
 * @version     v1
 * @date        03/05/2019
 * @author      jarribas
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

/******************************************************************************
 ** Includes
 */

#include "thread_comm_hal_Mock.h"

#include <ctype.h>


#include "../synergy_gen_Mock/thread_comm_Mock.h"
#include "../Resources/Comm.h"
#include "../thread_api.h"
#include "../Resources/Trace_Mock.h"
#include "../Drivers/I2C_2_Mock.h"


#include "../thread_core_entry_Mock.h"
#include "../thread_drd_entry.h"
#include "../event_ids.h"
#include "thread_defibrillator_hal_Mock.h"




/******************************************************************************
 ** Macros
 */

/******************************************************************************
 ** Defines
 */

/******************************************************************************
 ** Typedefs
 */

/******************************************************************************
 ** Constants
 */

/******************************************************************************
 ** Externs
 */

/******************************************************************************
 ** Globals
 */

wifi_config_t wifi;
sigfox_config_t sigfox;
gps_config_t gps;
acc_config_t acc;

SIGFOX_TEST_RESULT_t  sigfox_test_result;    // test result structure
SIGFOX_FUNC_ALERT_t   sigfox_func_alert;     // functional alert structure
SIGFOX_GPS_POSITION_t sigfox_gps_position;   // functional gps structure
SIGFOX_ELEC_BATT_EXPIRATION_t   sigfox_expiration;     // battery and electrodes expiration dates structure

WIFI_TEST_RESULT_t  wifi_test_result;    // test result structure
WIFI_FUNC_ALERT_t   wifi_func_alert;     // functional alert structure
WIFI_GPS_POSITION_t wifi_gps_position;   // functional gps structure
WIFI_ELEC_BATT_EXPIRATION_t wifi_expiration;     // battery and electrodes expiration dates structure

static COMM_MOD_e   comm_selected_uart = eMOD_SIGFOX;
static bool_t       is_sigfox_free = true;

static ERROR_ID_e retError = eERR_NONE;

static WIFI_HOST_RESPONSE_t host_response;

/******************************************************************************
 ** Locals
 */

/******************************************************************************
 ** Prototypes
 */

/******************************************************************************
 ** Name:    Comm_Initialize
 *****************************************************************************/
/**
 ** @brief   Initialization of Comm UART
 ** @param   void
 **
 ******************************************************************************/
void Comm_Modules_Initialize(void)
{
/******************************************************************************************/
    // 0.- Reset WIFI SIGFOX GPS module & Initialize Comm UART
/******************************************************************************************/
    //Comm_Hardware_Reset(eMOD_SIGFOX);

    Comm_Uart_Init_mock();

    //retError = Comm_Uart_Set_Baud_Rate(BR_115200);
    retError = Comm_Uart_Set_Baud_Rate_mock(BR_9600);
}


/******************************************************************************
** Name:    Comm_Hardware_Off
*****************************************************************************/
/**
** @brief   Reset Wifi, Sigfox or GPS module
**
** @param   none
******************************************************************************/
void Comm_Hardware_Off_mock(COMM_MOD_e comm_mod)
{
    uint8_t  i2c_data;

    if((comm_mod != eMOD_WIFI) && (comm_mod != eMOD_SIGFOX) && (comm_mod != eMOD_GPS)) return;

    // Read PCF8574 previous status
    I2C2_ReadByte (i2C_ADD_GPIO, &i2c_data);

    switch(comm_mod)
    {
        case eMOD_WIFI:
            i2c_data = (uint8_t)(i2c_data & (~(1<<1))) | (0<<1);
            break;
        case eMOD_SIGFOX:
            i2c_data = (uint8_t)(i2c_data & (~(1<<2))) | (0<<2);
            break;
        case eMOD_GPS:
            i2c_data = (uint8_t)(i2c_data & ~1) | 0;
            break;
        default:
            break;
    }

    I2C2_WriteByte (i2C_ADD_GPIO, i2c_data);

    //(void) tx_thread_sleep (OSTIME_200MSEC);
    Sleep(OSTIME_200MSEC);
    printf("Sleep 200msec HW OFF\n");
}

/******************************************************************************
** Name:    Comm_Hardware_On
*****************************************************************************/
/**
** @brief   Reset Wifi, Sigfox or GPS module
**
** @param   none
******************************************************************************/
void Comm_Hardware_On_mock(COMM_MOD_e comm_mod)
{
    uint8_t  i2c_data;

    if((comm_mod != eMOD_WIFI) && (comm_mod != eMOD_SIGFOX) && (comm_mod != eMOD_GPS)) return;

    // Read PCF8574 previous status
    I2C2_ReadByte (i2C_ADD_GPIO, &i2c_data);

    switch(comm_mod)
    {
        case eMOD_WIFI:
            i2c_data = (uint8_t)(i2c_data & (~(1<<1))) | (1<<1);
            break;
        case eMOD_SIGFOX:
            i2c_data = (uint8_t)(i2c_data & (~(1<<2))) | (1<<2);
            break;
        case eMOD_GPS:
            i2c_data = (uint8_t)(i2c_data & ~1) | 1;
            break;
        default:
            break;
    }

    I2C2_WriteByte (i2C_ADD_GPIO, i2c_data);

    //(void) tx_thread_sleep (OSTIME_200MSEC);
    Sleep(OSTIME_200MSEC);
    printf("Sleep 200msec HW ON\n");
}


/******************************************************************************
** Name:    Comm_Hardware_Reset
*****************************************************************************/
/**
** @brief   Reset Wifi, Sigfox or GPS module
**
** @param   none
******************************************************************************/
void Comm_Hardware_Reset(COMM_MOD_e comm_mod)
{
    if((comm_mod != eMOD_WIFI) && (comm_mod != eMOD_SIGFOX) && (comm_mod != eMOD_GPS)) return;

    Comm_Hardware_Off_mock(comm_mod);

    Comm_Hardware_On_mock(comm_mod);
}

/******************************************************************************
 ** Name:    Comm_Select_Uart
 *****************************************************************************/
/**
 ** @brief   Select Uart to send and receive data. This function sets the SN74CBT
 **          Multiplexer/Demultiplexer via the PCF8574 I/O Expander for I2C.
 ** @param   COMM_MOD_e Uart to select
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Select_Uart_mock(COMM_MOD_e comm_uart)
{
    uint8_t  i2c_data;

    if((comm_uart != eMOD_WIFI) && (comm_uart != eMOD_SIGFOX) && (comm_uart != eMOD_GPS)) return eERR_COMM_UART_SELECT;

    // Read PCF8574 previous status
    I2C2_ReadByte (i2C_ADD_GPIO, &i2c_data);

    // Clear UART bits
    i2c_data &= (uint8_t) ~( (1 << 6) | (1 << 7) );

    //Disable Enables bits !!!! not disable gps !!!!!
    i2c_data |= (uint8_t) ( (1 << 3) | (1 << 4) ); // | (1 << 5)

    switch(comm_uart)
    {
        case eMOD_WIFI:
            i2c_data = (uint8_t) (i2c_data & (~(1<<7))) | (1<<7);
            i2c_data &= (uint8_t) ~( 1 << 4);
            comm_selected_uart = eMOD_WIFI;
//Trace ((TRACE_NEWLINE),"TRACE WIFI");
            break;
        case eMOD_SIGFOX:
            is_sigfox_free = false;
            i2c_data = (uint8_t) (i2c_data & (~(1<<6))) | (1<<6);
            i2c_data = (uint8_t) (i2c_data & (~(1<<7))) | (1<<7);
            i2c_data &= (uint8_t) ~( 1 << 3);
            comm_selected_uart = eMOD_SIGFOX;
//Trace ((TRACE_NEWLINE),"TRACE SIGFOX");
            break;
        case eMOD_GPS:
            i2c_data = (uint8_t) (i2c_data & (~(1<<6))) | (1<<6);
            i2c_data &= (uint8_t) ~( 1 << 5);
            comm_selected_uart = eMOD_GPS;
//Trace ((TRACE_NEWLINE),"TRACE GPS");
            break;
        default:
//Trace ((TRACE_NEWLINE),"TRACE ERROR");
            return eERR_COMM_UART_SELECT;
            //break;
    }

    I2C2_WriteByte (i2C_ADD_GPIO, i2c_data);

    //(void) tx_thread_sleep (OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep 100 msec Select UART\n");

    return eERR_NONE;
}

/******************************************************************************
** Name:    Comm_Get_Selected_Uart
*****************************************************************************/
/**
** @brief   Returns the selected Comms UART
**
** @param   none
**
** @return  Selected Comms Uart
******************************************************************************/
COMM_MOD_e Comm_Get_Selected_Uart (void)
{
    return comm_selected_uart;
}


/******************************************************************************
** Name:    Comm_Is_Sigfox_Free
*****************************************************************************/
/**
** @brief   Returns if Comms UART is free
**
** @param   none
**
** @return  true or false
******************************************************************************/
bool_t Comm_Is_Sigfox_Free (void)
{
    return is_sigfox_free;
}
/******************************************************************************
** Name:    Comm_Modules_Print_Configuration
*****************************************************************************/
/**
** @brief   Prints Module Information
**
 ** @param  COMM_MOD_e module to print info
**
** @return  none
******************************************************************************/
void Comm_Modules_Print_Configuration_mock(COMM_MOD_e comm_mod)
{
    Trace_mock ((TRACE_NEWLINE), " ");
    //(void) tx_thread_sleep(OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep 100msec comm module conf\n");
    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "======================================");
    //(void) tx_thread_sleep(OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep 100msec comm module conf\n");

    switch(comm_mod)
    {
        case eMOD_WIFI:
            Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "          WLAN CONFIGURATION");
            break;
        case eMOD_SIGFOX:
            Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "         SIGFOX DEVICE INFO");
            break;
        case eMOD_GPS:
            Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "          GPS CONFIGURATION");
            break;
        default:
            break;

    }

    //(void) tx_thread_sleep(OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep 100msec comm module conf\n");
    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "======================================");
    //(void) tx_thread_sleep(OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep 100msec comm module conf\n");

    switch(comm_mod)
    {
        case eMOD_WIFI:
            Trace_mock ((TRACE_TIME_STAMP), "WLAN SSID:    ");
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) wifi.wlan_ssid);
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_mock ((TRACE_TIME_STAMP), "WLAN PASSKEY: ");
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) wifi.wlan_pass);
            Sleep(OSTIME_100MSEC);
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            if(wifi.wlan_eap_security == eWIFI_WPA_EAP_SEC)
            {
                Trace_mock ((TRACE_TIME_STAMP| TRACE_NEWLINE), "WLAN EAP SUPPORTED: YES");
            }
            else
            {
                Trace_mock ((TRACE_TIME_STAMP| TRACE_NEWLINE), "WLAN EAP SUPPORTED: NO");
            }
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_mock ((TRACE_TIME_STAMP), "IPv4 ADDRESS: ");
            Sleep(OSTIME_100MSEC);
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) wifi.ip_address);
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
//            Trace ((TRACE_TIME_STAMP), "MAC ADDRESS:  ");
//            (void) tx_thread_sleep(OSTIME_100MSEC);
//            Trace_Arg (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) wifi.mac_address);
//            (void) tx_thread_sleep(OSTIME_100MSEC);
            Trace_mock ((TRACE_TIME_STAMP), "HOST NAME:    ");
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) wifi.host_name);
            break;
        case eMOD_SIGFOX:
            Trace_mock ((TRACE_TIME_STAMP), "DEVICE ID:      ");
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) sigfox.device_id);
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_mock ((TRACE_TIME_STAMP), "PAC ID:         ");
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) sigfox.device_pac);
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_mock ((TRACE_TIME_STAMP), "TX FREQ:        ");
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) sigfox.device_tx_freq);
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_mock ((TRACE_TIME_STAMP), "RX FREQ:        ");
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) sigfox.device_rx_freq);
            break;
        case eMOD_GPS:
            Trace_mock ((TRACE_TIME_STAMP), "DEVICE ID:      ");
            //(void) tx_thread_sleep(OSTIME_100MSEC);
            Sleep(OSTIME_100MSEC);
            printf("Sleep\n");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) gps.device_id);
            break;
        default:
            break;

    }

    //(void) tx_thread_sleep(OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep\n");
    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "======================================");
    //(void) tx_thread_sleep(OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep\n");

    // Store Sigfox registration info in a file
    if(comm_mod == eMOD_SIGFOX) DB_Sigfox_Store_Info_mock (sigfox.device_id, sigfox.device_pac);
}

/*************************************************************************************************************
**************************************************************************************************************
                                           WIFI MODULE FUNCTIONS
**************************************************************************************************************
*************************************************************************************************************/

/******************************************************************************
 ** Name:    Wifi_Initialize
 *****************************************************************************/
/**
 ** @brief   Initialization of Wifi module
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Wifi_Initialize_mock(void)
{
    // Select UART for WIFI
    retError = Comm_Select_Uart_mock(eMOD_WIFI);
    if(retError != eERR_NONE) return retError;

    retError = Comm_Uart_Set_Baud_Rate_mock(BR_115200);

    // Initialize BGLib library for comms with the module
    //Wifi_Initialize_BGLib();
    printf("Comm Wifi Initialize.\n");

/******************************************************************************************/
    // 1.- Configure WIFI module
/******************************************************************************************/

    // Check module <-> host comms
    //retError = Wifi_Check_Module_Status();
    printf("Wifi_Check_Module_Status\n");
    if(retError != eERR_NONE) return retError;

    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module Check Comm.... OK");
/*
    // Check (Configure?) module Uart settings
    retError = Wifi_Configure_Module_Uart();
    if(retError != eERR_NONE) return retError;

    Trace ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module Uart Configuration.... OK");
*/printf("Wifi_Configure_Module_Uart\n");

/******************************************************************************************/
    // 3. - Get module status
/******************************************************************************************/
    //retError = Wifi_Get_Module_Status();
    //if(retError != eERR_NONE) return retError;

//    retError = Wifi_Get_Module_MAC_Address();
//    if(retError != eERR_NONE) return retError;
//
//    Trace ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module MAC configuration.... OK");

/******************************************************************************************/
    // 4. - Configure TCP/IP Settings
/******************************************************************************************/
   // retError = Wifi_Configure_TCPIP_Settings();
	printf("Wifi_Configure_TCPIP_Settings\n");
    if(retError != eERR_NONE) return retError;

    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module Configure TCP/IP .... OK");

/******************************************************************************************/
    // 4. - Get Wi-Fi Network Credentials from the module
/******************************************************************************************/
//    Wifi_Get_Module_Network_Credentials();

/******************************************************************************************/
   // 5. - Compare if Wi-Fi Network Credentials are the same as stored in Flash
/******************************************************************************************/
 //   if(Wifi_Compare_Network_Credentials() == FALSE)
    {
/******************************************************************************************/
   // 6. - If Credentials are NOT the same, Set up the NEW Wi-Fi Network Credentials
/******************************************************************************************/
//        retError = Wifi_Set_Network_Down();
//        if(retError != eERR_NONE) return retError;

//        Wifi_Set_Module_Network_Credentials();

    }

//     retError = Wifi_Set_Wifi_Radio_Off();
//    if(retError != eERR_NONE) return retError;

    //retError = Wifi_Set_Wifi_Radio_On();
    printf("Wifi_Set_Wifi_Radio_On\n");
    if(retError != eERR_NONE) return retError;

    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module Radio ON.... OK");

    //retError =  Wifi_Scan_Network();
    printf("Wifi_Scan_Network\n");
    if(retError != eERR_NONE) return retError;

    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module Scan Network.... OK");
//
//    (void) tx_thread_sleep (OSTIME_30SEC);  // wait a little time


/******************************************************************************************/
   // 9. - Configure WPA-EAP Certificate
/******************************************************************************************/
    if(wifi.wlan_eap_security == eWIFI_WPA_EAP_SEC) /*&& (flash_wlan_eap_security == eWIFI_WPA_EAP_SEC)*/
    {
// - Cargar certificados igual que para TLS
     //IÑIGO
      //  Wifi_Configure_WPA_EAP_Cert(true);
    	printf("Wifi_Configure_WPA_EAP_Cert\n");
        if(retError != eERR_NONE) return retError;

        Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module Configure WPA_EAP Cert .... OK");
    }

/******************************************************************************************/
   // 9. - Configure TLS CA cert
/******************************************************************************************/

 //IÑIGO
/*     Wifi_Configure_TLS_Cacert(false);
    if(retError != eERR_NONE) return retError;

    Trace ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module Configure TLS CA Cert .... OK"); */

/*
--- Pasos para configurar el TLS con certificado !!!!
    // Wifi_Configure_TLS_Cacert(char_t* cacert, uint16_t bytes); // IÑIGO
    // if(retError != eERR_NONE) return retError;

    // Trace ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module Configure TLS CA Cert .... OK");

A.- Cargar el certificado de la CA del servidor

    1- void wifi_cmd_x509_add_certificate(uint8 store, uint16 size); // This command is used to add a certificate to a certificate store.
    Chequear respuesta No Error

    2- void wifi_cmd_x509_add_certificate_data(uint8 data_len, const uint8 *data_data);  // This command is used to upload a block of certificate data. EN BLOQUES DE 256!!!!
    Por cada bloque enviado se recibe respuesta wifi_rsp_x509_add_certificate, Chequear No Error
    Ejemplo:
    wifi_cmd_x509_add_certificate_data(255, certificate_data[0])
    wifi_cmd_x509_add_certificate_data(255, certificate_data[256])
    wifi_cmd_x509_add_certificate_data(255, certificate_data[512])
    wifi_cmd_x509_add_certificate_data(255, certificate_data[...])


    3- void wifi_cmd_x509_add_certificate_finish(); // This command is used to finish adding a certificate. The command must be called once all the certificate data has been uploaded.
    Chequear respuesta No Error

B.- Para hacer una conexion con TLS (utilizar funcion Wifi_Open_TLS_Client, ya creada)

    1.- void wifi_cmd_tcpip_tls_set_authmode(uint8 auth_mode); // This command is used to set the TLS certificate verification mode. 2: mandatory verification, verification failure causes connection
    to fail

    2.- void wifi_cmd_tcpip_tls_connect(ipv4 address, uint16 port, int8 routing); // This command is used to create a new TLS connection to a TLS server.

    3.- void wifi_cmd_endpoint_close(uint8 endpoint); // This command is used to close a protocol endpoint. The command must only be used for protocol endpoints such as TCP, UDP and TLS endpoints.

C.- Sustituir la funcion Wifi_Open_TCP_Client por Wifi_Open_TLS_Client en los respectivos servicios

*/

/******************************************************************************************/
   // 7. - Bring Network Up
/******************************************************************************************/
 //   retError = Wifi_Connect_Network();
    printf("Wifi_Connect_Network\n");
    if(retError != eERR_NONE) return retError;

    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Wifi Module Connect Network.... OK");

/******************************************************************************************/
   // 8. - Get IPv4 address
/******************************************************************************************/
//    retError = Wifi_Get_IPv4_Address();
//    if(retError != eERR_NONE) return retError;

    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Radio_Off
 *****************************************************************************/
/**
 ** @brief   Set Radio Wifi Off
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Wifi_Radio_Off_mock(void)
{
    //retError = Wifi_Set_Wifi_Radio_Off();
	printf("Wifi_Set_Wifi_Radio_Off\n");
    if(retError != eERR_NONE) return retError;

    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Is_Host_Server_Alive
 *****************************************************************************/
/**
 ** @brief   Check if host server is alive
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Wifi_Is_Host_Server_Alive_mock(void)
{
   // if(Wifi_Is_Host_Alive() != eERR_NONE) return eERR_COMM_WIFI_TX_RX_GET_SERVER_ALIVE;
	printf("Wifi_Is_Host_Server_Alive\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Send_Frame_Test_mock
 *****************************************************************************/
/**
 ** @brief   Send Test frame using wifi connection
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e    Comm_Wifi_Send_Frame_Test_mock(void)
{
    //lint -e64 Known type mismatch passing a struct as a byte array
  //  if(Wifi_Send_Data_Frame((uint8_t *) &wifi_test_result) != eERR_NONE) return eERR_COMM_WIFI_TX_RX_POST_INFO_DATA;
	printf("Wifi_Send_Frame_Test\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Send_Frame_Alert
 *****************************************************************************/
/**
 ** @brief   Send Alert frame using wifi connection
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e    Comm_Wifi_Send_Frame_Alert_mock(void)
{
    //lint -e64 Known type mismatch passing a struct as a byte array
   // if(Wifi_Send_Data_Frame((uint8_t *) &wifi_func_alert) != eERR_NONE) return eERR_COMM_WIFI_TX_RX_POST_INFO_DATA;
	printf("Wifi_Send_Frame_Alert_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Send_Frame_Exp
 *****************************************************************************/
/**
 ** @brief   Send Expiration frame using wifi connection
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e    Comm_Wifi_Send_Frame_Exp_mock(void)
{
    //lint -e64 Known type mismatch passing a struct as a byte array
  //  if(Wifi_Send_Data_Frame((uint8_t *) &wifi_expiration) != eERR_NONE) return eERR_COMM_WIFI_TX_RX_POST_INFO_DATA;
	printf("Wifi_Send_Data_Frame\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Send_Frame_Gps
 *****************************************************************************/
/**
 ** @brief   Send Gps position frame using wifi connection
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e    Comm_Wifi_Send_Frame_Gps_mock(void)
{
    //lint -e64 Known type mismatch passing a struct as a byte array
  //  if(Wifi_Send_Data_Frame((uint8_t *) &wifi_gps_position) != eERR_NONE) return eERR_COMM_WIFI_TX_RX_POST_INFO_DATA;
	printf("Comm_Wifi_Send_Frame_Gps_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Send_Last_Test
 *****************************************************************************/
/**
 ** @brief   Send Last Test File
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e    Comm_Wifi_Send_Last_Test_mock(void)
{
   // if(Wifi_Send_Test() != eERR_NONE) return eERR_COMM_WIFI_TX_RX_POST_INFO_DATA;
	printf("Comm_Wifi_Send_Last_Test\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Send_Last_Episode
 *****************************************************************************/
/**
 ** @brief   Send Last Episode File
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e    Comm_Wifi_Send_Last_Episode_mock(void)
{
  // if(Wifi_Send_Episode() != eERR_NONE) return eERR_COMM_WIFI_TX_RX_POST_INFO_DATA;
	printf("Comm_Wifi_Send_Last_Episode_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Get_Upgrade_Firmware
 *****************************************************************************/
/**
 ** @brief   Get upgrade firmware from host server
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e    Comm_Wifi_Get_Upgrade_Firmware_mock(void)
{
  //  if(Wifi_Receive_Firmware() != eERR_NONE) return eERR_COMM_WIFI_TX_RX_GET_UPGRADE_FILE;
	printf("Comm_Wifi_Get_Upgrade_Firmware_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Get_Configuration_File_mock
 *****************************************************************************/
/**
 ** @brief   Get configuration file from host server
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e    Comm_Wifi_Get_Configuration_File_mock(void)
{
   // if(Wifi_Receive_Configuration() != eERR_NONE) return eERR_COMM_WIFI_TX_RX_GET_CONF_FILE;
	printf("Comm_Wifi_Get_Configuration_File_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Wifi_Execute_Test
 *****************************************************************************/
/**
 ** @brief   Test of Wifi module
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Wifi_Execute_Test_mock(void)
{
    // Select UART for WIFI
    retError = Comm_Select_Uart_mock(eMOD_WIFI);
    if(retError != eERR_NONE) return retError;

    retError = Comm_Uart_Set_Baud_Rate_mock(BR_115200);

    // Initialize BGLib library for comms with the module
  //  Wifi_Initialize_BGLib();
    printf("Wifi_Initialize_BGLib\n");

    // Check module <-> host comms
   // retError = Wifi_Check_Module_Status();
    printf("Comm_Wifi_Execute_Test_mock\n");
    if(retError != eERR_NONE) return retError;

    return eERR_NONE;
}

/******************************************************************************
** Name:    Wifi_Is_MAC_Valid
*****************************************************************************/
/**
** @brief   Checks if a MAC address has a valid format
**
** @param   s   pointer of the MAC address string
**
** @return  bool_t  true if MAC format OK, false if not
******************************************************************************/
bool_t Comm_Wifi_Is_MAC_Valid(char_t *s)
{
    uint8_t i = 0;

    for(i = 0; i < 17; i++)
    {
        if (((i % 3) != 2) && (!isxdigit((uint8_t) s[i])))
        {
            return FALSE;
        }
        if(i % 3 == 2 && s[i] != ':')
        {
            return FALSE;
        }
    }
    if(s[17] != '\0')
    {
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************
** Name:    Wifi_Is_IPv4_Valid
*****************************************************************************/
/**
** @brief   Checks if a IPv4 address has a valid format
**
** @param   s   pointer of the IPv4 address string
**
** @return  bool_t  true if IPv4 format OK, false if not
******************************************************************************/
bool_t Comm_Wifi_Is_IPv4_Valid(char_t *s)
{
    static uint8_t i = 0;
    static uint8_t j = 0;
    static uint8_t numDots = 0;
    static char_t number[4] = {0000};
    static uint8_t len = 0;

    len = (uint8_t) strlen(s);

    if (!isdigit((uint8_t) s[0])) return FALSE;

    if (len < 7 || len > 15) return FALSE;

    number[3] = 0;

    for (i = 0 ; i< len; i++)
    {
        if (isdigit((uint8_t) s[i]))
        {
            number[j] = s[i];
            j++;
            if (j>3)
            {
                return FALSE;
            }
        }
        else if (s[i] == '.')
        {
            if (atoi(number) > 255)
            {
                return FALSE;
            }
            memset(number, '0', 3);
            number[3] = 0;
            j = 0;
            numDots++;
            if(numDots > 3)
            {
                return FALSE;
            }
        }
    }

    if (numDots == 3)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

void  Comm_Wifi_Save_Host_Response(char_t *resp)
{
    memset(&host_response, 0, sizeof(host_response));

    host_response.must_be_0xAA = (uint8_t) resp[0];

    host_response.timestamp = ( ((uint32_t)resp[1] & 0xFF) << 24) |
                              ( ((uint32_t)resp[2] & 0xFF) << 16) |
                              ( ((uint32_t)resp[3] & 0xFF) <<  8) |
                              ( ((uint32_t)resp[4] & 0xFF) <<  0) ;

    host_response.cfg_available = (uint8_t) resp[5];

    host_response.frmw_available = (uint8_t) resp[6];

    host_response.must_be_0xBB = (uint8_t) resp[7];
}

uint8_t  Comm_Wifi_Is_Cfg_Available(void)
{
   return host_response.cfg_available;
}

uint8_t  Comm_Wifi_Is_Frmw_Available(void)
{
    return host_response.frmw_available;
}

/*************************************************************************************************************
**************************************************************************************************************
                                           SIGFOX MODULE FUNCTIONS
**************************************************************************************************************
*************************************************************************************************************/


/******************************************************************************
 ** Name:    Comm_Sigfox_Initialize_mock
 *****************************************************************************/
/**
 ** @brief   Initialization of Sigfox module
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Initialize(void)
{
    static bool_t first_time = FALSE;
    is_sigfox_free = false;

    memset(&sigfox,0,sizeof(sigfox_config_t));

    retError = Comm_Select_Uart_mock(eMOD_SIGFOX);
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    retError = Comm_Uart_Set_Baud_Rate_mock(BR_9600);
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

  //  retError = Sigfox_Check_Module_Status();
    printf("Sigfox_Check_Module_Status\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    if(first_time == FALSE)
    {
      //  retError = Sigfox_Get_Module_DeviceID();
    	 printf("Sigfox_Check_Module_Status\n");
        if(retError != eERR_NONE)
        {
            is_sigfox_free = true;
            return retError;
        }

       // retError = Sigfox_Get_Module_PAC();
        printf("Sigfox_Get_Module_PAC\n");
        if(retError != eERR_NONE)
        {
            is_sigfox_free = true;
            return retError;
        }

       // retError = Sigfox_Get_TX_Frequency();
        printf("Sigfox_Get_TX_Frequency\n");
        if(retError != eERR_NONE)
        {
            is_sigfox_free = true;
            return retError;
        }

      //  retError = Sigfox_Get_RX_Frequency();
        printf("Sigfox_Get_RX_Frequency\n");
        if(retError != eERR_NONE)
        {
            is_sigfox_free = true;
            return retError;
        }

    //    retError = Sigfox_Get_Module_Temp();
        printf("Sigfox_Get_Module_Temp\n");
        if(retError != eERR_NONE)
        {
            is_sigfox_free = true;
            return retError;
        }

    //    retError = Sigfox_Get_Module_Voltages();
        printf("Sigfox_Get_Module_Voltages\n");
        if(retError != eERR_NONE)
        {
            is_sigfox_free = true;
            return retError;
        }

        first_time = TRUE;
    }
    is_sigfox_free = true;
    printf("Comm_Sigfox_Initialize\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Sigfox_Execute_Test
 *****************************************************************************/
/**
 ** @brief   Test of Sigfox module
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Execute_Test_mock(void)
{
    retError = Comm_Select_Uart_mock(eMOD_SIGFOX);
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }


    retError = Comm_Uart_Set_Baud_Rate_mock(BR_9600);
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }


    //retError = Sigfox_Check_Module_Status();
    printf("Sigfox_Check_Module_Status\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    is_sigfox_free = true;
    printf("Comm_Sigfox_Execute_Test_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Sigfox_Send_Test
 *****************************************************************************/
/**
 ** @brief   Send R100 Test
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Send_Test_mock(void)
{
    //lint -e64 Known type mismatch passing a struct as a byte array
    //retError = Sigfox_Send_Data_Frame((uint8_t *) &sigfox_test_result);
	printf("Sigfox_Send_Data_Frame\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    is_sigfox_free = true;
    printf("Comm_Sigfox_Send_Test_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Sigfox_Send_Alert_mock
 *****************************************************************************/
/**
 ** @brief   Send R100 Functional Alert
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Send_Alert_mock(void)
{
    //lint -e64 Known type mismatch passing a struct as a byte array
    //retError = Sigfox_Send_Data_Frame((uint8_t *) &sigfox_func_alert);
    printf("Sigfox_Send_Data_Frame\n");
    if(retError != eERR_NONE)
    {
  //      retError = Sigfox_Send_Data_Frame((uint8_t *) &sigfox_func_alert);
    	  printf("Sigfox_Send_Data_Frame\n");
        if(retError != eERR_NONE)
        {
            //retError = Sigfox_Send_Data_Frame((uint8_t *) &sigfox_func_alert);
        	  printf("Sigfox_Send_Data_Frame\n");
            if(retError != eERR_NONE)
            {
                is_sigfox_free = true;
                return retError;
            }
        }
    }

    is_sigfox_free = true;
    printf("Comm_Sigfox_Send_Alert_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Sigfox_Send_Exp
 *****************************************************************************/
/**
 ** @brief   Send battery and electrodes expiration date
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Send_Exp_mock(void)
{
    //lint -e64 Known type mismatch passing a struct as a byte array
    //retError = Sigfox_Send_Data_Frame((uint8_t *) &sigfox_expiration);
	printf("Sigfox_Send_Data_Frame\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    is_sigfox_free = true;
    printf("Comm_Sigfox_Send_Exp_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Sigfox_Send_Position
 *****************************************************************************/
/**
 ** @brief   Send R100 GPS Position
 ** @param   void
 **
 ** @return  ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Send_Position_mock(void)
{
    //lint -e64 Known type mismatch passing a struct as a byte array
    //retError = Sigfox_Send_Data_Frame((uint8_t *) &sigfox_gps_position);
	printf("Sigfox_Send_Data_Frame\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    is_sigfox_free = true;
    printf("Comm_Sigfox_Send_Position_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Sigfox_Start_Uplink_Test
 *****************************************************************************/
/**
 ** @brief   Perform sigfox module Uplink Test
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Start_Uplink_Test_mock(void)
{
    retError = Comm_Sigfox_Device_Info_Check_mock();
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    Comm_Modules_Print_Configuration_mock(eMOD_SIGFOX);

    //retError = Sigfox_Start_CW_Test();
    printf("Sigfox_Start_CW_Test\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Sigfox Uplink Test Started....");

    is_sigfox_free = true;
    printf("Comm_Sigfox_Start_Uplink_Test_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Sigfox_Stop_Uplink_Test_mock
 *****************************************************************************/
/**
 ** @brief   Stop sigfox module Uplink Test
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Stop_Uplink_Test_mock(void)
{
    //retError = Sigfox_Stop_CW_Test();
	 printf("Sigfox_Stop_CW_Test\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Sigfox Uplink Test Stopped....");
    is_sigfox_free = true;
    printf("Comm_Sigfox_Stop_Uplink_Test_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Sigfox_Start_Downlink_Test
 *****************************************************************************/
/**
 ** @brief   Start sigfox module Downlink Test (with RX GFSK test mode)
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Start_Downlink_Test_mock(void)
{
    //retError = Sigfox_Start_RX_GFSK_Test();
	printf("Sigfox_Start_RX_GFSK_Test\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }
    Trace_mock ((TRACE_TIME_STAMP | TRACE_NEWLINE), "Sigfox Downlink Test Started....");
    is_sigfox_free = true;
    printf("Comm_Sigfox_Start_Downlink_Test_mock\n");
    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Sigfox_Device_Info_Check
 *****************************************************************************/
/**
 ** @brief   Device information Check
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_Sigfox_Device_Info_Check_mock(void)
{
	printf("Comm_Sigfox_Device_Info_Check_mock\n");
/******************************************************************************************/
    // 0.-  SIGFOX Check Module Communication
    //retError = Sigfox_Check_Module_Status();
	   printf("Sigfox_Check_Module_Status\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;

        return retError;
    }
/******************************************************************************************/
    // 1.-  SIGFOX ID Check
 //   retError = Sigfox_Get_Module_DeviceID();
    printf("Sigfox_Get_Module_DeviceID\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }
/******************************************************************************************/
    // 2.-  SIGFOX PAC Code Check
  //  retError = Sigfox_Get_Module_PAC();
    printf("Sigfox_Get_Module_PAC\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }
/******************************************************************************************/
    // 3.-  SIGFOX RF Frequency Setup Check
  //  retError = Sigfox_Get_TX_Frequency();
    printf("Sigfox_Get_TX_Frequency\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

  //  retError = Sigfox_Get_RX_Frequency();
    printf("Sigfox_Get_RX_Frequency\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

    //Compare Sigfox TX & RX FREQ are the same as expected
 //   retError = Sigfox_Compare_Test_Frequencies();
    printf("Sigfox_Compare_Test_Frequencies\n");
    if(retError != eERR_NONE)
    {
        is_sigfox_free = true;
        return retError;
    }

/******************************************************************************************/
    is_sigfox_free = true;
    return eERR_NONE;
}

/*************************************************************************************************************
**************************************************************************************************************
                                           GPS MODULE FUNCTIONS
**************************************************************************************************************
*************************************************************************************************************/

/******************************************************************************
 ** Name:    Comm_GPS_Initialize
 *****************************************************************************/
/**
 ** @brief   Initialization of GPS module
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_GPS_Initialize_mock(void)
{
    memset(&gps,0,sizeof(gps));

 //   retError = Comm_Select_Uart(eMOD_GPS);
    printf("Comm_Select_Uart\n");
    if(retError != eERR_NONE) return retError;

  //  retError = Comm_Uart_Set_Baud_Rate(BR_9600);
    printf("Comm_Uart_Set_Baud_Rate\n");
    if(retError != eERR_NONE) return retError;
    printf("Comm_GPS_Initialize_mock\n");

    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_GPS_Send_Command
 *****************************************************************************/
/**
 ** @brief   Send GPS commands
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_GPS_Send_Command_mock(void)
{
	printf("Comm_GPS_Send_Command_mock\n");
    if(Comm_Is_Sigfox_Free())
    {
        if (Comm_Get_Selected_Uart() != eMOD_GPS)
        {
 //          retError = Comm_Select_Uart(eMOD_GPS);
        	printf("Comm_Select_Uart\n");
            if(retError != eERR_NONE) return retError;

  //          retError = Comm_Uart_Set_Baud_Rate(BR_9600);
            printf("Comm_Uart_Set_Baud_Rate\n");
            if(retError != eERR_NONE) return retError;
        }

        // In addition to these commands it is possible to load an EPO file with satellite predictions for a faster TTFF
        Comm_Uart_Send_mock("$PMTK286,1*23");        // AIC - Active Interference Cancellation

        Comm_Uart_Send_mock("$PMTK313,1*2E");        // Set SBAS enabled. Signal correction

        Comm_Uart_Send_mock("$PMTK301,2*2E");        // Set DGPS mode. Differential GPS

        Comm_Uart_Send_mock("$PMTK220,1000*1F");     // Set possition fix every 1 seconds. Update satellites registers

        Comm_Uart_Send_mock("$PMTK353,1,1,0,0,0*2B"); // GPS + GLONASS

        //Comm_Uart_Send("$PMTK869,1,1*35");  // EASY default enabled
    }
    else return eERR_COMM_GPS;

    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_GPS_Execute_Test
 *****************************************************************************/
/**
 ** @brief   Test of GPS module
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e Comm_GPS_Execute_Test_mock(void)
{
    retError = Comm_Select_Uart_mock(eMOD_GPS);
    if(retError != eERR_NONE) return retError;

    retError = Comm_Uart_Set_Baud_Rate_mock(BR_9600);
    if(retError != eERR_NONE) return retError;

    // Wait a little time...
   // (void) tx_thread_sleep(OSTIME_3SEC);
    Sleep(OSTIME_3SEC);
    printf("Sleep\n");

  /*  if(GPS_Is_Test_Package_Received() == FALSE)
    {
        return eERR_COMM_GPS;
    }*/
    printf("GPS_Is_Test_Package_Received\n");
    printf("Comm_GPS_Execute_Test_mock\n");

    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_GPS_Get_Position_Data_mock
 *****************************************************************************/
/**
 ** @brief   Get GPS Data
 ** @param   void
 **
 ** @return ERROR_ID_e error code
 ******************************************************************************/
ERROR_ID_e    Comm_GPS_Get_Position_Data_mock(void)
{
	printf("Gps_Is_New_Position_Available\n");
  //  if(Gps_Is_New_Position_Available() == TRUE)
   //{
        // Select UART for GPS
      //  retError = Comm_Select_Uart(eMOD_GPS);
	 	 printf("Comm_Select_Uart\n");
        if(retError != eERR_NONE) return retError;

        //retError = Comm_Uart_Set_Baud_Rate(BR_9600);
        printf("Comm_Uart_Set_Baud_Rate\n");
        if(retError != eERR_NONE) return retError;

     //   retError = Gps_Get_Lat_Long_Data();
        printf("Gps_Get_Lat_Long_Data\n");
        if(retError != eERR_NONE) return retError;

        // Proccessed new position
        printf("Gps_Set_New_Position_Available\n");
      //  Gps_Set_New_Position_Available(FALSE);
    //}

    Trace_mock ((TRACE_TIME_STAMP), "GPS ==> ");
    //(void) tx_thread_sleep(OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep\n");
    Trace_Arg_mock (TRACE_NO_FLAGS, " LAT:  %s  ", (uint32_t) gps.lat_data);
    //(void) tx_thread_sleep(OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep\n");
    Trace_Arg_mock (TRACE_NEWLINE, " LNG:  %s  ", (uint32_t) gps.long_data);
    //(void) tx_thread_sleep(OSTIME_100MSEC);
    Sleep(OSTIME_100MSEC);
    printf("Sleep\n");
    printf("Comm_GPS_Get_Position_Data_mock\n");
    return eERR_NONE;
}
