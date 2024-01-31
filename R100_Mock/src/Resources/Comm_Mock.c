/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        Comm_Trace.c
 * @brief       All functions related to the comm modules through serial
 *
 * @version     v1
 * @date        17/04/2019
 * @author      jarribas
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

/******************************************************************************
 ** Includes
 */
#include "../../synergy_Mock/ssp/inc/framework/el/tx_api_Mock.h"
#include <time.h>
#include <stdio.h>

#include "Comm.h"
#include "Trace_Mock.h"
#include "../Drivers/RTC.h"
#include "../HAL/thread_comm_hal_Mock.h"
#include "../thread_core_entry_Mock.h"



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

extern TX_SEMAPHORE comm_semaphore;

/******************************************************************************
 ** Globals
 */

/******************************************************************************
 ** Locals
 */

static char comm_tx_buffer[256];              // buffer used to write traces
static char comm_rx_buffer[1024];              // buffer used to write traces

static char comm_data_buffer[256];           // buffer used to write data


static char wifi_rx_msg[256];                 // buffer used to copy received command responses
static char buffer_sCode[4];
//static bool_t receive_finish_status = FALSE;
static uint32_t received_bytes = 0;

/******************************************************************************
 ** Prototypes
 */
static void Process_GPS_Package_mock (void);

/******************************************************************************
 ** Name:    user_uart_callback
 *****************************************************************************/
/**
 ** @brief   Implements the callback of the UART
 ** @param   p_args       UART callback arguments
 **
 ** @return  void
 ** @todo    pending things to be done
 ******************************************************************************/
void comm_uart_callback_mock()
{
    //ssp_err_t err;

	printf("UART_CALLBACK mock \n");
}

/******************************************************************************
 ** Name:    Comm_Uart_Init
 *****************************************************************************/
/**
 ** @brief   Open connection SCI3 (UART). Initialize semaphore
 ** @param   void
 **
 ** @return  void
 ** @todo    pending things to be done
 ******************************************************************************/
void Comm_Uart_Init_mock(void)
{
	printf("UART_INIT mock\n");
}

/******************************************************************************
 ** Name:    Comm_Uart_Set_Baud_Rate
 *****************************************************************************/
/**
 ** @brief   Change SCI3 (UART) baud rate
 ** @param   uint32_t  baud_rate
 **
 ** @return  ERROR_ID_e error changing the baud rate of the Comm UART
 ******************************************************************************/
ERROR_ID_e Comm_Uart_Set_Baud_Rate_mock(uint32_t baud_rate)
{
	int error = 1;

    printf("UART_Set_Baud_Rate mock \n");

    if (error != 0)
    {
        return eERR_COMM_UART_BAUD_RATE;
    }

    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Comm_Uart_Send
 *****************************************************************************/
/**
 ** @brief   Send a string through the UART
 **
 ** @param   pString       Pointer to string
 **
 ** @return  void
 ** @todo    pending things to be done
 ******************************************************************************/
void Comm_Uart_Send_mock(const char *pString)
{
    uint32_t sz_str;                    // String size
   // ssp_err_t   ssp_error;          // ssp error code

    // get the trace port
   // if (tx_semaphore_get (&comm_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
   // {

        //Copy the string to send to the terminal
        strcpy (comm_tx_buffer, pString);
        sz_str = strlen (comm_tx_buffer);

        if (Is_Test_Mode_Montador() == FALSE)
        {

            //Insert the CR+LF if flag is defined ...
            comm_tx_buffer[sz_str++] = '\r';
            comm_tx_buffer[sz_str++] = '\n';
            //comm_tx_buffer[sz_str++] = '\r';
        }

        //Transmit trace buffer through UART
        printf("UART_Send Mock %s \n", *pString);
       /* if (ssp_error != 0)
        {
            // SSP ASSERT ???
        }*/
   // }
}

/******************************************************************************
 ** Name:    Comm_Arg
 *****************************************************************************/
/**
 ** @brief   Send a string through the UART
 **
 ** @param   flags         Flags to print more information (Time or CRLF)
 ** @param   pString       Pointer to string
 ** @param   aux_code      Additional parameter or code
 **
 ** @return  void
 ** @todo    pending things to be done
 ******************************************************************************/
void Comm_Uart_Send_Arg_mock(uint8_t flags, const char *pString, const uint32_t aux_code)
{
   // ssp_err_t   ssp_error;          // ssp error code
    //rtc_time_t current_time;            // Current time
    uint32_t sz_str;                    // String size

    // get the trace port
   // if (tx_semaphore_get (&comm_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
   // {
        if (flags & TRACE_TIME_STAMP)
        {
            //insert the time stamp ...
          //  (void) iRTC.p_api->calendarTimeGet (iRTC.p_ctrl, &current_time);
            //sprintf (comm_tx_buffer, "%02d:%02d:%02d --> ", current_time.tm_hour, current_time.tm_min, current_time.tm_sec);
        }
        else
        {
            //clean buffer
            comm_tx_buffer[0] = '\0';
        }

        //Copy the string to send to the terminal
        strcat (comm_tx_buffer, pString);

        //Copy the code at the end of the buffer
        sprintf (buffer_sCode, " %lu", aux_code);
        strcat (comm_tx_buffer, buffer_sCode);

        sz_str = strlen (comm_tx_buffer);

        if (flags & TRACE_NEWLINE)
        {
            //Insert the CR+LF if flag is defined ...
            comm_tx_buffer[sz_str++] = '\n';
            comm_tx_buffer[sz_str++] = '\r';
        }

        //Transmit trace buffer through UART
        printf("UART_Send_Arg mock \n");
      //  ssp_error = commUART.p_api->write (commUART.p_ctrl, (uint8_t *) &comm_tx_buffer[0], sz_str);

      /*  if (ssp_error != SSP_SUCCESS)
        {
            // SSP ASSERT ???
        }*/

  //  }
}

/******************************************************************************
 ** Name:    Comm_Uart_Send_Data
 *****************************************************************************/
/**
 ** @brief   Send a data string through UART
 **
 ** @param   pString       Pointer to trace string
 ** @param   size          Data size
 **
 ** @return  void
 ** @todo    pending things to be done
 ******************************************************************************/
void Comm_Uart_Send_Data_mock(const char *pString, uint32_t size)
{
    //ssp_err_t   ssp_error;          // ssp error code

    // Clear Data buffer
//    memset(comm_data_buffer,0,sizeof(comm_data_buffer));

    // get the trace port
    //if (tx_semaphore_get (&comm_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
    //{
        //Copy the string to send to the terminal
        strncpy (comm_data_buffer, pString, size);

        //Insert the CR+LF if flag is defined ...
        comm_data_buffer[size++] = '\r';
        comm_data_buffer[size++] = '\n';

        //Transmit trace buffer through UART
        //ssp_error = commUART.p_api->write (commUART.p_ctrl, (uint8_t *) comm_data_buffer, size);
        printf("UART_Send_Data Mock: %s \n",*pString);
       /* if (ssp_error != SSP_SUCCESS)
        {
            // SSP ASSERT ???
        }*/

  //  }
}

/******************************************************************************
 ** Name:    Comm_Uart_Get_Msg_Buff_Ptr
 *****************************************************************************/
/**
 ** @brief   Get a received module response
 **
 ** @param   buf       Pointer to buffer to store the message
 ** @param   count     size of the message to be read
 **
 ** @return  void
 ** @todo    pending things to be done
 ******************************************************************************/
void Comm_Uart_Get_Msg_Buff_Ptr (char *buf, int count)
{
    for(int i = 0; i < count; ++i)
    {
        if((wifi_rx_msg[i] == '\n') || (wifi_rx_msg[i] == '\r'))
        {
            return;
        }
        buf[i] = wifi_rx_msg[i];
    }
}

/******************************************************************************
 ** Name:    Comm_Uart_Wifi_Send
 *****************************************************************************/
/**
 ** @brief   Send a string through the UART to the wifi module
 **
 ** @param   pString       Pointer to string
 ** @param   pString       Pointer to string
 **
 ** @return  void
 ** @todo    pending things to be done
 ******************************************************************************/
void Comm_Uart_Wifi_Send_mock(const char *pString, uint32_t length)
{
   // ssp_err_t   ssp_error;          // ssp error code

    // get the trace port
    //if (tx_semaphore_get (&comm_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
    //{

        // Clear TX buffer
        //memset(comm_tx_buffer,0,sizeof(comm_tx_buffer));

        //Copy the string to send to the terminal
        memcpy (comm_tx_buffer, pString, length);

        //Transmit trace buffer through UART
        //ssp_error = commUART.p_api->write (commUART.p_ctrl, (uint8_t *) comm_tx_buffer, length);
        printf("UART_WIFI_SEND mock: %s \n", *pString);

        /*if (ssp_error != SSP_SUCCESS)
        {
            // SSP ASSERT ???
        }*/
   // }
}

/******************************************************************************
 ** Name:    Comm_Uart_Wifi_Receive
 *****************************************************************************/
/**
 ** @brief   Get a received module response
 **
 ** @param   buf       Pointer to buffer to store the message
 ** @param   count     size of the message to be read
 **
 ** @return  ERROR_ID_e error if bytes not readed
 ** @todo    pending things to be done
 ******************************************************************************/
ERROR_ID_e Comm_Uart_Wifi_Receive_mock (char *rx_buf, uint32_t length, uint32_t tout)
{
    uint32_t    ref_timeout = 0;

    ref_timeout = tout;//tx_time_get() + tout;
    printf("Comm_Uart_Wifi_Receive_mock\n");

//    memset(comm_rx_buffer,0,sizeof(comm_rx_buffer));
    received_bytes = 0;

    if(length == 0)
    {
      //  while(tx_time_get() <= ref_timeout) printf("Sleep\n");//(void) tx_thread_sleep (OSTIME_50MSEC);  // wait a little time;
        length = received_bytes;

        //lint -e419 rx_buf size is the same as comm_rx_buffer (1024)
        memcpy(rx_buf, comm_rx_buffer, length);
//        memset(comm_rx_buffer,0,sizeof(comm_rx_buffer));
        received_bytes = 0;
        return eERR_NONE;
    }
    else
    {
     /*   while(tx_time_get() <= ref_timeout)
        {
            if(received_bytes == length)
            {
                memcpy(rx_buf, comm_rx_buffer, length);
//                memset(comm_rx_buffer,0,sizeof(comm_rx_buffer));
                received_bytes = 0;
                return eERR_NONE;
            }

            //(void) tx_thread_sleep (OSTIME_50MSEC);  // wait a little time
            printf("Sleep\n");
        }*/
        return eERR_COMM_UART_RX_ERROR;
    }
}

/******************************************************************************
 ** Name:    Process_GPS_Package
 *****************************************************************************/
/**
 ** @brief   Process GPS received package, checks if geoposition available
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void Process_GPS_Package_mock (void)
{
    //Trace(TRACE_NO_FLAGS, comm_rx_buffer);
	printf("Process_GPS_Package_mock\n");

    //$PMTK GPS Commands renponse
    if((comm_rx_buffer[0] == '$') &&
       (comm_rx_buffer[1] == 'P') &&
       (comm_rx_buffer[2] == 'M') &&
       (comm_rx_buffer[3] == 'T') &&
       (comm_rx_buffer[4] == 'K') &&
       (comm_rx_buffer[5] == '0') &&
       (comm_rx_buffer[6] == '1') &&
       (comm_rx_buffer[7] == '1'))
    {
        // GPS traces when power on  // Example: $PMTK011,MTKGPS*08
        Trace_mock(TRACE_NO_FLAGS, comm_rx_buffer);
      //  GPS_Set_Running(TRUE);
    }

    if((comm_rx_buffer[0] == '$') &&
       (comm_rx_buffer[1] == 'P') &&
       (comm_rx_buffer[2] == 'M') &&
       (comm_rx_buffer[3] == 'T') &&
       (comm_rx_buffer[4] == 'K') &&
       (comm_rx_buffer[5] == '0') &&
       (comm_rx_buffer[6] == '0') &&
       (comm_rx_buffer[7] == '1'))
    {
        // Command response
        Trace_mock(TRACE_NO_FLAGS, comm_rx_buffer);
    }

    //$GPRMC,144326.00,A,5107.0017737,N,11402.3291611,W,0.080,323.3,210307,0.0,E,A*20
    if((comm_rx_buffer[0] == '$') &&
       (comm_rx_buffer[1] == 'G') &&
       (comm_rx_buffer[2] == 'P') &&
       (comm_rx_buffer[3] == 'R') &&
       (comm_rx_buffer[4] == 'M') &&
       (comm_rx_buffer[5] == 'C'))
    {
        Trace_mock(TRACE_NO_FLAGS, comm_rx_buffer);
       // Gps_Set_Test_Package_Received(TRUE);
    }

    //$GNRMC,114455.000,A,4311.1651,N,00230.6201,W,2.51,82.98,210920,,,A,V*2D
    if((comm_rx_buffer[0] == '$') &&
       (comm_rx_buffer[1] == 'G') &&
       (comm_rx_buffer[2] == 'N') &&
       (comm_rx_buffer[3] == 'R') &&
       (comm_rx_buffer[4] == 'M') &&
       (comm_rx_buffer[5] == 'C'))
    {

        Trace_mock(TRACE_NO_FLAGS, comm_rx_buffer);

        // Check if position has changed
        if(memcmp(comm_rx_buffer, wifi_rx_msg, received_bytes) != 0)
        {
         //   Gps_Set_New_Position_Available(TRUE);
//            memset(wifi_rx_msg,0,sizeof(wifi_rx_msg));
            strncpy(wifi_rx_msg, comm_rx_buffer, received_bytes);
        }
        // Clear buffer
//        memset(comm_rx_buffer,0,sizeof(comm_rx_buffer));
        received_bytes = 0;
    }
    else
    {
        // Clear buffer
//        memset(comm_rx_buffer,0,sizeof(comm_rx_buffer));
        received_bytes = 0;
    }
}
