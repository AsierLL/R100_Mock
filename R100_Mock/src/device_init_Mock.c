/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        settings.c
 * @brief       Manages the settings of the system
 *
 * @version     v1
 * @date        04/09/2018 
 * @author      ltorres
 *
 */

/******************************************************************************
 ** Includes
 */
#include "device_init.h"
#include "Drivers/I2C_2_Mock.h"
#include "thread_sysMon_entry_Mock.h"
#include "thread_audio_entry.h"
#include "synergy_gen_Mock/thread_recorder_Mock.h"

#include "R100_Errors.h"
#include "HAL/R100_Tables.h"
#include "HAL/thread_patMon_hal_Mock.h"
#include "Resources/Trace_Mock.h"
#include "types_basic_Mock.h"

/******************************************************************************
 ** Macros
 */

/******************************************************************************
 ** Defines
 */
#define SETTINGS_UNSET              0xFF
#define MAX_FILE_SIZE               2046


#define SWAP_INT16(x)               (x = (int16_t)  ((x<<8) | ((x>>8) & 0xFF)))
#define SWAP_UINT16(x)              (x = (uint16_t) ((x>>8) | (x<<8)))
#define SWAP_UINT32(x)              (x = ((x>>24)&0xff) | ((x<<8)&0xff0000) | ((x>>8)&0xff00) | ((x<<24)&0xff000000))
#define SWAP_UINT64(x)              (x = ((x>>56)&0xff) | ((x>>48)&0xff00) | ((x>>40)&0xff0000) | ((x>>32)&0xff000000) | ((x<<56)&0xff00000000000000) | ((x<<48)&0xff000000000000) | ((x<<40)&0xff00000000))

/******************************************************************************
 ** Typedefs
 */


const char_t my_compile_date[] = __DATE__;
const char_t my_compile_time[] =__TIME__;

/******************************************************************************
 ** Constants
 */

/******************************************************************************
 ** Externs
 */

/******************************************************************************
 ** Globals
 */
NFC_DEVICE_ID_t write_nfc_device_id;  // Global struct for comparing NFC reading and writing
NFC_DEVICE_ID_t read_nfc_device_id;   // Global struct for comparing NFC reading and writing

/******************************************************************************
 ** Locals
 */

static DEVICE_DATE_t        device_date;
static DEVICE_TIME_t        device_time;
static DEVICE_INFO_t        device_info;
static DEVICE_SETTINGS_t    device_settings;
static COMM_SETTINGS_t      device_comms;

static NFC_DEVICE_ID_t      nfc_device_id;
static NFC_DEVICE_INFO_t    nfc_device_info;
static NFC_SETTINGS_t       nfc_settings;
static NFC_DEVICE_COM_t     nfc_device_com;

static AED_SETTINGS_t*      p_aed_settings;
static MISC_SETTINGS_t*     p_misc_settings;

//static FX_FILE              cfg_file;

/******************************************************************************
 ** Prototypes
 */

/******************************************************************************
 ** Name:    Get_Compile_Date_Time
 *****************************************************************************/
/**
 ** @brief   Get compiler date and time
 **
 ** @param   none
 **
 ** @return  struc to fill the date and time
 **
 ******************************************************************************/
void Get_Compile_Date_Time(void)
{
    char_t month[2] = {0};
    char_t aux_data[5] = {0};
    char_t aux_time[5] = {0};

    if(my_compile_date[0] == 'J' && my_compile_date[1] == 'a' && my_compile_date[2] == 'n')
    {
        month[0] = '0';
        month[1] = '1';
    }
    if(my_compile_date[0] == 'F')
    {
        month[0] = '0';
        month[1] = '2';
    }
    if(my_compile_date[0] == 'M' && my_compile_date[1] == 'a' && my_compile_date[2] == 'r')
    {
        month[0] = '0';
        month[1] = '3';
    }
    if(my_compile_date[0] == 'A' && my_compile_date[1] == 'p')
    {
        month[0] = '0';
        month[1] = '4';
    }
    if(my_compile_date[0] == 'M' && my_compile_date[1] == 'a' && my_compile_date[2] == 'y')
    {
        month[0] = '0';
        month[1] = '5';
    }
    if(my_compile_date[0] == 'J' && my_compile_date[1] == 'u' && my_compile_date[2] == 'n')
    {
        month[0] = '0';
        month[1] = '6';
    }
    if(my_compile_date[0] == 'J' && my_compile_date[1] == 'u' && my_compile_date[2] == 'l')
    {
        month[0] = '0';
        month[1] = '7';
    }
    if(my_compile_date[0] == 'A' && my_compile_date[1] == 'u')
    {
        month[0] = '0';
        month[1] = '8';
    }
    if(my_compile_date[0] == 'S')
    {
        month[0] = '0';
        month[1] = '9';
    }
    if(my_compile_date[0] == 'O')
    {
        month[0] = '1';
        month[1] = '0';
    }
    if(my_compile_date[0] == 'N')
    {
        month[0] = '1';
        month[1] = '1';
    }
    if(my_compile_date[0] == 'D')
    {
        month[0] = '1';
        month[1] = '2';
    }

    memcpy(&aux_data[0], &month[0], 2);             //month
    memcpy(&aux_data[2], &my_compile_date[4], 2);   //day
    if(aux_data[2] == 0x20) aux_data[2] = '0';      // convert space to 0

    memcpy(&aux_time[0], &my_compile_time[0], 2);   // hour
    memcpy(&aux_time[2], &my_compile_time[3], 2);   // min

    nfc_device_info.compiler_date = (uint16_t)atoi(aux_data);
    nfc_device_info.compiler_time = (uint16_t)atoi(aux_time);
}

/******************************************************************************
 ** Name:    NFC_Read
 *****************************************************************************/
/**
 ** @brief   Read data from NFC memory
 **
 ** @param   pBuffer    pointer to buffer to fill
 ** @param   nBytes     number of bytes to read from NFC
 ** @param   offset     address offset (block)
 **
 ** @return  none
 **
 ******************************************************************************/
static void NFC_Read (uint8_t *pBuffer, uint32_t nBytes, uint32_t offset)
{
    uint32_t nBlocks, i;                            // global use counters
    uint8_t  my_buffer[NFC_BYTES_PER_PAGE * 2];     // page buffer

    // initialize the memory to be filled ...
    memset (pBuffer, SETTINGS_UNSET, nBytes);

    // loop reading the requested info (16 bytes per block)
    nBlocks = nBytes / NFC_BYTES_PER_PAGE;
    printf("Bloques: %d\n",nBlocks);
    for (i=0; i<nBlocks; i++)
    {
        // set the block address and proceed to read
        I2C2_WriteByte(i2C_ADD_NTAG, (uint8_t) (i + offset));
        I2C2_Read (i2C_ADD_NTAG, my_buffer, NFC_BYTES_PER_PAGE);
        memcpy (&pBuffer[i*16], my_buffer, NFC_BYTES_PER_PAGE);
    }
    nBytes %= 16;
    if (nBytes)
    {
        // set the block address and proceed to read
        I2C2_WriteByte(i2C_ADD_NTAG, (uint8_t) (i + offset));
        I2C2_Read (i2C_ADD_NTAG, my_buffer, NFC_BYTES_PER_PAGE);
        memcpy (&pBuffer[i*16], my_buffer, nBytes);
    }
}

/******************************************************************************
 ** Name:    NFC_Write
 *****************************************************************************/
/**
 ** @brief   Write device info in the NFC memory
 **
 ** @param   pBuffer    pointer to buffer that contains the information to store
 ** @param   nBytes     number of bytes to write into the NFC memory
 ** @param   offset     address offset (block)
 **
 ** @return  none
 **
 ******************************************************************************/
static void NFC_Write_mock (uint8_t *pBuffer, uint32_t nBytes, uint32_t offset)
{
    uint32_t nBlocks, i;                        // global use counters

    printf("NFC_Write_mock\n");
    // loop reading the requested info (16 bytes per block)
    nBlocks = nBytes / NFC_BYTES_PER_PAGE;
    for (i=0; i<nBlocks; i++)
    {
        // proceed to write in the selected block
        I2C2_Write_NFC((uint8_t) (i + offset), &pBuffer[i * NFC_BYTES_PER_PAGE], NFC_BYTES_PER_PAGE);
      //  tx_thread_sleep (OSTIME_10MSEC);
    }
    nBytes %= 16;
    if (nBytes)
    {
        // proceed to write in the selected block
        I2C2_Write_NFC((uint8_t) (i + offset), &pBuffer[i * NFC_BYTES_PER_PAGE], (uint8_t) nBytes);
    }
}

/******************************************************************************
 ** Name:    NFC_Write_Device_ID
 *****************************************************************************/
/**
 ** @brief   Write device identifier into the NFC memory
 **
 ** @param   flag to write into NFC
 **
 ** @return  none
 **
 ******************************************************************************/
void NFC_Write_Device_ID_mock (bool_t flag_write, bool_t flag_cal_write)
{
    uint8_t my_buffer[16];
    NFC_DEVICE_ID_t *prueba;

    printf("NFC_Write_Device_ID_mock\n");

  /*  prueba = &nfc_device_id;

    // Write battery info into NFC
//    Trace (TRACE_TIME_STAMP | TRACE_NEWLINE, "Writing device ID into NFC");

    ////////////////////////////////////////////////////////////////////////////
    // update the device information ...
    ////////////////////////////////////////////////////////////////////////////
    nfc_device_id.structure_version = NFC_REV_CODE; //this is not necessary, NFC app forzes the revision code
    memcpy(nfc_device_id.device_sn, device_info.sn, RSC_NAME_SZ);
    memcpy(nfc_device_id.device_manufacture_date, device_info.manufacture_date, RSC_NAME_SZ);
    nfc_device_id.develop_mode = device_info.develop_mode;
    nfc_device_id.device_options = device_info.device_options;

    if (Is_Test_Mode_Montador())
    {
        // Save with DEVELOP_MANUFACTURE_CONTROL
        nfc_device_id.develop_mode = DEVELOP_MANUFACTURE_CONTROL;
    }

    // In Manufacture mode disable sigfox option
    if (nfc_device_id.develop_mode == DEVELOP_MANUFACTURE_CONTROL)
    {
        device_info.device_options &= 0xFD;
        nfc_device_id.device_options = device_info.device_options;
    }

    ////////////////////////////////////////////////////////////////////////////
    // update the impedance calibration information ...
    ////////////////////////////////////////////////////////////////////////////
    if (flag_cal_write)
    {
        nfc_device_id.zp_adcs_short       = patMon_Get_Zp_ADCs();
        nfc_device_id.zp_adcs_calibration = patMon_Get_zp_CAL_ADCs();
    }

    if (flag_write)
    {
        SWAP_UINT16 (nfc_device_id.develop_mode);

        SWAP_UINT32 (nfc_device_id.structure_version);

        SWAP_UINT16 (nfc_device_id.device_options);

        // unlock NFC ID memory area
        NFC_Read (my_buffer, NFC_BYTES_PER_PAGE, NFC_LOCKING_BLOCK);
        my_buffer[0]  = 0xAA;
        my_buffer[10] = my_buffer[11] = 0x00;
        NFC_Write_mock (my_buffer, NFC_BYTES_PER_PAGE, NFC_LOCKING_BLOCK);

        // write the device info in the NFC memory ...
        NFC_Write_mock ((uint8_t *)&nfc_device_id, sizeof (NFC_DEVICE_ID_t), NFC_DEVICE_ID_BLOCK);
        write_nfc_device_id = *prueba;

        // lock NFC ID memory area
        my_buffer[10] = my_buffer[11] = 0xFF;
        NFC_Write_mock (my_buffer, NFC_BYTES_PER_PAGE, NFC_LOCKING_BLOCK);
    }*/
}

/******************************************************************************
 ** Name:    NFC_Write_Device_Info
 *****************************************************************************/
/**
 ** @brief   Write device and accessories (electrodes & battery) info the NFC memory
 **
 ** @param   flag to write into NFC
 ** @param   flag to write calibration values
 **
 ** @return  none
 **
 ******************************************************************************/
void NFC_Write_Device_Info_mock (bool_t flag_write)
{
    uint32_t            err_open;           // SD media open result ...
    uint64_t            available_bytes;    // number of available bytes in the uSD
    BATTERY_INFO_t      my_battery_info;    // battery information
    ELECTRODES_DATA_t   my_electrodes_data; // electrodes information

    printf("NFC Write Device Info mock\n");

    // Write battery info into NFC
//    Trace (TRACE_TIME_STAMP | TRACE_NEWLINE, "Writing device info NFC");

    // uSD device open. try to know some characteristics ...
   // err_open = fx_media_extended_space_available(&sd_fx_media, &available_bytes);

    ////////////////////////////////////////////////////////////////////////////
    // update the uSD storage information ...
    ////////////////////////////////////////////////////////////////////////////
 /*   nfc_device_info.internal_memory_cap = 7372;         // initialize to 8GB
    nfc_device_info.internal_memory_rem = 7372;         // initialize to 8GB
    if (err_open == 0)
    {
        nfc_device_info.internal_memory_rem = (uint16_t) (available_bytes / (1024 * 1024));

    }
*/
    ////////////////////////////////////////////////////////////////////////////
    // update the battery information ...
    ////////////////////////////////////////////////////////////////////////////
 /*   Battery_Get_Info (&my_battery_info);
    memcpy(nfc_device_info.battery_sn, my_battery_info.sn, RSC_NAME_SZ);
    nfc_device_info.battery_manufacture_date = my_battery_info.manufacture_date;
    nfc_device_info.battery_expiration_date  = my_battery_info.expiration_date;
    nfc_device_info.battery_cap = (uint16_t) my_battery_info.nominal_capacity;
    nfc_device_info.battery_rem = (uint16_t) Battery_Get_Charge_mock();
*/
    ////////////////////////////////////////////////////////////////////////////
    // update the electrodes information ...
    ////////////////////////////////////////////////////////////////////////////
/*    Electrodes_Get_Data (&my_electrodes_data);

    memcpy (&nfc_device_info.electrodes_sn1, &(((uint8_t *) &my_electrodes_data.sn) [4]), 4);
    memcpy (&nfc_device_info.electrodes_sn2, &(((uint8_t *) &my_electrodes_data.sn) [0]), 4);
//  nfc_device_info.electrodes_sn1 = ((uint32_t *) &my_electrodes_data.sn) [1];
//  nfc_device_info.electrodes_sn2 = ((uint32_t *) &my_electrodes_data.sn) [0];
    nfc_device_info.electrodes_manufacture_date = my_electrodes_data.info.manufacture_date;
    nfc_device_info.electrodes_expiration_date  = my_electrodes_data.info.expiration_date;
    nfc_device_info.electrodes_event_date       = my_electrodes_data.event.date;
    nfc_device_info.electrodes_event_time       = my_electrodes_data.event.time;
    nfc_device_info.electrodes_event_id         = my_electrodes_data.event.id;

    ////////////////////////////////////////////////////////////////////////////
    // update the app version ...
    ////////////////////////////////////////////////////////////////////////////
    nfc_device_info.app_sw_version = APP_REV_CODE;

    ////////////////////////////////////////////////////////////////////////////
    // update the device status information ...
    ////////////////////////////////////////////////////////////////////////////
    nfc_device_info.error_code = Get_NV_Data()->error_code;

    if (flag_write)
    {
        SWAP_UINT32 (nfc_device_info.internal_memory_cap);
        SWAP_UINT32 (nfc_device_info.internal_memory_rem);

        SWAP_UINT32 (nfc_device_info.battery_manufacture_date);
        SWAP_UINT32 (nfc_device_info.battery_expiration_date);
        SWAP_UINT16 (nfc_device_info.battery_cap);
        SWAP_UINT16 (nfc_device_info.battery_rem);

        SWAP_UINT32 (nfc_device_info.electrodes_sn1);
        SWAP_UINT32 (nfc_device_info.electrodes_sn2);
        SWAP_UINT32 (nfc_device_info.electrodes_manufacture_date);
        SWAP_UINT32 (nfc_device_info.electrodes_expiration_date);
        SWAP_UINT32 (nfc_device_info.electrodes_event_date);
        SWAP_UINT32 (nfc_device_info.electrodes_event_time);
        SWAP_UINT32 (nfc_device_info.electrodes_event_id);

        SWAP_UINT32 (nfc_device_info.app_sw_version);
        SWAP_UINT16 (nfc_device_info.error_code);
        SWAP_UINT16 (nfc_device_info.compiler_date);
        SWAP_UINT16 (nfc_device_info.compiler_time);
        //SWAP_UINT16 (nfc_device_info.pcb_hw_version);

        // write the device info in the NFC memory ...
        NFC_Write_mock ((uint8_t *)&nfc_device_info, sizeof (NFC_DEVICE_INFO_t), NFC_DEVICE_INFO_BLOCK);
    }
    */
}

/******************************************************************************
** Name:    Settings_Swap
*****************************************************************************/
/**
** @brief   Swap settings variable
**
** @param   pNFC_Settings   pointer to the NFC settings to check
**
** @return  none
******************************************************************************/
static void Settings_Swap (NFC_SETTINGS_t *pNFC_Settings)
{
    // swap the read information
    SWAP_UINT16(pNFC_Settings->aCPR_init);              ///< adult --> Initial CPR time in seconds (can be 0)
    SWAP_UINT16(pNFC_Settings->aCPR_1);                 ///< adult --> After Shock Series CPR time in seconds (can be OFF)
    SWAP_UINT16(pNFC_Settings->aCPR_2);                 ///< adult --> No Shock Advised CPR time in seconds (can be OFF)
    SWAP_UINT16(pNFC_Settings->pCPR_init);              ///< pediatric --> Initial CPR time in seconds (can be 0)
    SWAP_UINT16(pNFC_Settings->pCPR_1);                 ///< pediatric --> After Shock Series CPR time in seconds (can be OFF)
    SWAP_UINT16(pNFC_Settings->pCPR_2);                 ///< pediatric --> No Shock Advised CPR time in seconds (can be OFF)
    SWAP_UINT16(pNFC_Settings->cpr_msg_long);           ///< Long/Short audio messages for CPR

    SWAP_UINT16(pNFC_Settings->analysis_cpr);           ///< Enables start an analysis cycle in CPR
    SWAP_UINT16(pNFC_Settings->asys_detect);            ///< Enables Asystole warning message
    SWAP_UINT16(pNFC_Settings->asys_time);              ///< Time on asystole before emitting asystole warning message

    SWAP_UINT16(pNFC_Settings->metronome_en);           ///< Enables metronome for marking compression pace
    SWAP_UINT16(pNFC_Settings->metronome_ratio_a);      ///< Metronome pace for adult
    SWAP_UINT16(pNFC_Settings->metronome_ratio_p);      ///< Metronome pace for pediatric
    SWAP_UINT16(pNFC_Settings->metronome_rate);         ///< Metronome pace for pediatric
    SWAP_UINT16(pNFC_Settings->feedback_en);            ///< Enables cpr feedback for marking compression pace

    SWAP_UINT16(pNFC_Settings->consec_shocks);          ///< Max Shocks in a Shock Series
    SWAP_UINT16(pNFC_Settings->energy_shock1_a);        ///< Energy for 1st shock in shock series for adult
    SWAP_UINT16(pNFC_Settings->energy_shock2_a);        ///< Energy for 2nd shock in shock series for adult
    SWAP_UINT16(pNFC_Settings->energy_shock3_a);        ///< Energy for 3rd shock in shock series for adult
    SWAP_UINT16(pNFC_Settings->energy_shock1_p);        ///< Energy for 1st shock in shock series for pediatric
    SWAP_UINT16(pNFC_Settings->energy_shock2_p);        ///< Energy for 2nd shock in shock series for pediatric
    SWAP_UINT16(pNFC_Settings->energy_shock3_p);        ///< Energy for 3rd shock in shock series for pediatric

    SWAP_UINT16(pNFC_Settings->patient_type);           ///< Patient type (0-pediatric 1-adult)
    SWAP_UINT16(pNFC_Settings->mains_filter);           ///< mains filter (0-none 1-50 Hz 2-60 Hz)
    SWAP_UINT16(pNFC_Settings->language);               ///< device audio interface language
    SWAP_INT16(pNFC_Settings->utc_time);                ///< UTC time
    SWAP_UINT16(pNFC_Settings->audio_volume);           ///< device audio volume
    SWAP_UINT16(pNFC_Settings->trainer_mode);
    SWAP_UINT16(pNFC_Settings->patient_alert);
    SWAP_UINT16(pNFC_Settings->movement_alert);
    SWAP_UINT16(pNFC_Settings->transmis_mode);
    SWAP_UINT16(pNFC_Settings->warning_alert);

}

/******************************************************************************
 ** Name:    NFC_Write_Settings
 *****************************************************************************/
/**
 ** @brief   Write device settings info the NFC memory. The NFC data structure
 **          is swapped
 **
 ** @param   none
 **
 ** @return  none
 **
 ******************************************************************************/
void NFC_Write_Settings_mock (void)
{
	printf("NFC_Write_Settings_mock\n");
    // Write battery info into NFC
//    Trace (TRACE_TIME_STAMP | TRACE_NEWLINE, "Writing settings info NFC");



    // write the device info in the NFC memory ...
    NFC_Write_mock ((uint8_t *)&nfc_settings, sizeof (NFC_SETTINGS_t), NFC_DEVICE_SETTINGS_BLOCK);

}

/******************************************************************************
** Name:    Device_Init
*****************************************************************************/
/**
** @brief   Load device information and options from resources
**          Assume that all NFC data has been read previously and is
**          present in the NFC structures !!!
**
** @param   serial_num     pointer to buffer to store the serial number
**
** @return  none
**
******************************************************************************/
void Device_Init_mock (char * serial_num)
{

	printf("Device Init mock\n");
	/*fflush(0);
	getchar();
*/
    /*memset(&device_info, 0, sizeof(device_info));

    // MUST check ---> device_info.checksum
    strcpy(device_info.model, "Reanibex R100");
    memcpy(device_info.sn, nfc_device_id.device_sn, RSC_NAME_SZ);
    memcpy(device_info.manufacture_date, nfc_device_id.device_manufacture_date, RSC_NAME_SZ);


    // set the device options (previously read from NFC)
    device_info.device_options      = nfc_device_id.device_options;
    SWAP_UINT16 (device_info.device_options);

    device_info.develop_mode = nfc_device_id.develop_mode;
    SWAP_UINT16 (device_info.develop_mode);

    if(Is_Test_Mode_Montador() == false)
    {
        // set the ZP calibration data
        if ((nfc_device_id.zp_adcs_short > ADS_ZP_0_MIN) && (nfc_device_id.zp_adcs_short < ADS_ZP_0_MAX))
        {
            device_info.zp_adcs_short       = nfc_device_id.zp_adcs_short;
        }
        else
        {
            nfc_device_info.error_code = eERR_ZP_DATA;
        }

        if ((nfc_device_id.zp_adcs_calibration > ADS_CAL_MIN) && (nfc_device_id.zp_adcs_calibration < ADS_CAL_MAX))
        {
            device_info.zp_adcs_calibration       = nfc_device_id.zp_adcs_calibration;
        }
        else
        {
            nfc_device_info.error_code = eERR_ZP_DATA;
        }
    }
    else
    {

        device_info.zp_adcs_short       = nfc_device_id.zp_adcs_short;
        device_info.zp_adcs_calibration       = nfc_device_id.zp_adcs_calibration;
    }


    if (serial_num)
    {
        memcpy(serial_num, nfc_device_id.device_sn, RSC_NAME_SZ);
    }

    // Safe compiler date and time
    Get_Compile_Date_Time();
	*/
}

/******************************************************************************
** Name:    Set_Device_Date_time
*****************************************************************************/
/**
** @brief   Set device date and time
**
** @return
******************************************************************************/
void Set_Device_Date_time_mock (void)
{
	printf("Set_Device_Date_time_mock\n");
    Get_Date (&device_date.year, &device_date.month, &device_date.date);
    Get_Time (&device_time.hour, &device_time.min,   &device_time.sec);

    // Set date and time FX File System
    //fx_system_date_set((UINT)(device_date.year+2000), device_date.month, device_date.date);
//fx_system_time_set(device_time.hour, device_time.min, device_time.sec);
}

/******************************************************************************
** Name:    Get_Device_Date
*****************************************************************************/
/**
** @brief   Function for accessing device date variable
**
** @return  device date variable
******************************************************************************/
DEVICE_DATE_t* Get_Device_Date (void)
{
    return &device_date;
}

/******************************************************************************
** Name:    Get_Device_Time
*****************************************************************************/
/**
** @brief   Function for accessing device time variable
**
** @return  device time variable
******************************************************************************/
DEVICE_TIME_t* Get_Device_Time (void)
{
    return &device_time;
}

/******************************************************************************
** Name:    Get_Device_Info
*****************************************************************************/
/**
** @brief   Function for accessing device info variable
**
** @return  device info variable
******************************************************************************/
DEVICE_INFO_t* Get_Device_Info (void)
{
    return &device_info;
}

/******************************************************************************
** Name:    Get_NFC_Device_Info
*****************************************************************************/
/**
** @brief   Function for accessing NFC device info struct
**
** @return  NFC device info variable
******************************************************************************/
NFC_DEVICE_INFO_t* Get_NFC_Device_Info (void)
{
    return &nfc_device_info;
}

/******************************************************************************
** Name:    Get_NFC_Device_ID
*****************************************************************************/
/**
** @brief   Function for accessing NFC device identifier struct
**
** @return  NFC device id variable
******************************************************************************/
NFC_DEVICE_ID_t* Get_NFC_Device_ID (void)
{
    return &nfc_device_id;
}

/******************************************************************************
** Name:    Set_Default_Settings
******************************************************************************/
/**
** @brief   This function sets the default configuration
** @param   void
**
** @return  void
******************************************************************************/
static void    Set_Default_Settings (void)
{
    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Settings default values");

	// clear the settings structure !!!
    memset (((uint8_t *) &device_settings), 0, sizeof (DEVICE_SETTINGS_t));

    // Configuration - AED
    nfc_settings.aCPR_init         = device_settings.aed.aed_aCPR_init         = Get_NV_Data()->default_settings.aCPR_init;    // adult CPR at start (seconds)
    nfc_settings.aCPR_1            = device_settings.aed.aed_aCPR_1            = Get_NV_Data()->default_settings.aCPR_1;           // adult CPR1 (seconds)
    nfc_settings.aCPR_2            = device_settings.aed.aed_aCPR_2            = Get_NV_Data()->default_settings.aCPR_2;           // adult CPR2 (seconds)

    nfc_settings.pCPR_init         = device_settings.aed.aed_pCPR_init         = Get_NV_Data()->default_settings.pCPR_init;             // pediatric CPR at start (seconds)
    nfc_settings.pCPR_1            = device_settings.aed.aed_pCPR_1            = Get_NV_Data()->default_settings.pCPR_1;           // pediatric CPR1 (seconds)
    nfc_settings.pCPR_2            = device_settings.aed.aed_pCPR_2            = Get_NV_Data()->default_settings.pCPR_2;           // pediatric CPR2 (seconds)

    nfc_settings.cpr_msg_long      = device_settings.aed.aed_cpr_msg_long      = Get_NV_Data()->default_settings.cpr_msg_long;             // 0-Short; 1-Long
    nfc_settings.analysis_cpr      = device_settings.aed.aed_analysis_cpr      = Get_NV_Data()->default_settings.analysis_cpr;             // 0-disabled; 1-Enabled
    nfc_settings.asys_detect       = device_settings.aed.aed_asys_detect       = Get_NV_Data()->default_settings.asys_detect;             // 0-No; 1-Yes (Asystole detection)
    nfc_settings.asys_time         = device_settings.aed.aed_asys_time         = Get_NV_Data()->default_settings.asys_time;             // Asystole time (minutes)
    nfc_settings.metronome_en      = device_settings.aed.aed_metronome_en      = Get_NV_Data()->default_settings.metronome_en;             // 0-No; 1-Yes (Metronome enable)
    nfc_settings.feedback_en       = device_settings.aed.aed_feedback_en       = Get_NV_Data()->default_settings.feedback_en;             // 0-No; 1-Yes (Metronome enable)
    nfc_settings.consec_shocks     = device_settings.aed.aed_consec_shocks     = Get_NV_Data()->default_settings.consec_shocks;             // 1 shock
    nfc_settings.energy_shock1_a   = device_settings.aed.aed_energy_shock1_a   = Get_NV_Data()->default_settings.energy_shock1_a;           // Shock 1 energy adult (Joules)
    nfc_settings.energy_shock2_a   = device_settings.aed.aed_energy_shock2_a   = Get_NV_Data()->default_settings.energy_shock2_a;           // Shock 2 energy adult (Joules)
    nfc_settings.energy_shock3_a   = device_settings.aed.aed_energy_shock3_a   = Get_NV_Data()->default_settings.energy_shock3_a;           // Shock 3 energy adult (Joules)
    nfc_settings.metronome_ratio_a = device_settings.aed.aed_metronome_ratio_a = Get_NV_Data()->default_settings.metronome_ratio_a;             // 0-15:2; 1-30:2; 2-Only compressions (Metronome ratio for adult patient)
    nfc_settings.energy_shock1_p   = device_settings.aed.aed_energy_shock1_p   = Get_NV_Data()->default_settings.energy_shock1_p;            // Shock 1 energy pediat. (Joules)
    nfc_settings.energy_shock2_p   = device_settings.aed.aed_energy_shock2_p   = Get_NV_Data()->default_settings.energy_shock2_p;            // Shock 2 energy pediat. (Joules)
    nfc_settings.energy_shock3_p   = device_settings.aed.aed_energy_shock3_p   = Get_NV_Data()->default_settings.energy_shock3_p;            // Shock 3 energy pediat. (Joules)
    nfc_settings.metronome_ratio_p = device_settings.aed.aed_metronome_ratio_p = Get_NV_Data()->default_settings.metronome_ratio_p;             // 0-15:2; 1-30:2; 2-Only compressions (Metronome ratio for pediatric patient)
    nfc_settings.metronome_rate    = device_settings.aed.aed_metronome_rate    = Get_NV_Data()->default_settings.metronome_rate;             // 100; 110; 120; Metronome rate in compressions per minute

    // General
    nfc_settings.mains_filter = device_settings.misc.glo_filter           = Get_NV_Data()->default_settings.mains_filter;               // Notch filter: 0-None; 1-50Hz; 2-60Hz;
    nfc_settings.patient_type = device_settings.misc.glo_patient_adult    = Get_NV_Data()->default_settings.patient_type;                   // Patient type --> 0-Pediatric; 1-Adult
    nfc_settings.language     = device_settings.misc.glo_language         = Get_NV_Data()->default_settings.language;                      // default language
    nfc_settings.audio_volume = device_settings.misc.glo_volume           = Get_NV_Data()->default_settings.audio_volume;                      // assign a default audio volume
    nfc_settings.utc_time     = device_settings.misc.glo_utc_time         = Get_NV_Data()->default_settings.utc_time;                      // default Greenwich
	nfc_settings.trainer_mode   = device_settings.misc.glo_trainer_mode	  = Get_NV_Data()->default_settings.trainer_mode;
    nfc_settings.patient_alert  = device_settings.misc.glo_patient_alert  = Get_NV_Data()->default_settings.patient_alert;
    nfc_settings.movement_alert = device_settings.misc.glo_movement_alert = Get_NV_Data()->default_settings.movement_alert;
    nfc_settings.transmis_mode  = device_settings.misc.glo_transmis_mode  = Get_NV_Data()->default_settings.transmis_mode;
    nfc_settings.warning_alert  = device_settings.misc.glo_warning_alert  = Get_NV_Data()->default_settings.warning_alert;

    device_settings.misc.glo_auto_test_hour   = 0;
    device_settings.misc.glo_auto_test_min    = 0;
    device_settings.misc.glo_ecg_format       = 0;                      // ECG format 0: standard 1: cabrera
}

/******************************************************************************
** Name:    Save_Default_Settings
******************************************************************************/
/**
** @brief   This function sets the default configuration
** @param   void
**
** @return  void
******************************************************************************/
static void    Save_Default_Settings (void)
{
    NV_DATA_t data;

    // Save only settings
    NV_Data_Read_mock (&data);

    // Configuration - AED
    data.default_settings.aCPR_init = device_settings.aed.aed_aCPR_init;
    data.default_settings.aCPR_1 = device_settings.aed.aed_aCPR_1;
    data.default_settings.aCPR_2 = device_settings.aed.aed_aCPR_2;

    data.default_settings.pCPR_init = device_settings.aed.aed_pCPR_init;
    data.default_settings.pCPR_1 = device_settings.aed.aed_pCPR_1;
    data.default_settings.pCPR_2 = device_settings.aed.aed_pCPR_2;

    data.default_settings.cpr_msg_long = device_settings.aed.aed_cpr_msg_long;
    data.default_settings.analysis_cpr = device_settings.aed.aed_analysis_cpr;
    data.default_settings.asys_detect = device_settings.aed.aed_asys_detect;
    data.default_settings.asys_time = device_settings.aed.aed_asys_time;
    data.default_settings.metronome_en = device_settings.aed.aed_metronome_en;
    data.default_settings.feedback_en = device_settings.aed.aed_feedback_en;
    data.default_settings.consec_shocks = device_settings.aed.aed_consec_shocks;
    data.default_settings.energy_shock1_a = device_settings.aed.aed_energy_shock1_a;
    data.default_settings.energy_shock2_a = device_settings.aed.aed_energy_shock2_a;
    data.default_settings.energy_shock3_a = device_settings.aed.aed_energy_shock3_a;
    data.default_settings.metronome_ratio_a = device_settings.aed.aed_metronome_ratio_a;
    data.default_settings.energy_shock1_p = device_settings.aed.aed_energy_shock1_p;
    data.default_settings.energy_shock2_p = device_settings.aed.aed_energy_shock2_p;
    data.default_settings.energy_shock3_p = device_settings.aed.aed_energy_shock3_p;
    data.default_settings.metronome_ratio_p = device_settings.aed.aed_metronome_ratio_p;
    data.default_settings.metronome_rate = device_settings.aed.aed_metronome_rate;

    data.default_settings.patient_type = device_settings.misc.glo_patient_adult;
    data.default_settings.mains_filter = device_settings.misc.glo_filter;
    data.default_settings.language = device_settings.misc.glo_language;
    data.default_settings.audio_volume = device_settings.misc.glo_volume;
    data.default_settings.utc_time = device_settings.misc.glo_utc_time;
    data.default_settings.trainer_mode = device_settings.misc.glo_trainer_mode;
    data.default_settings.patient_alert = device_settings.misc.glo_patient_alert;
    data.default_settings.movement_alert = device_settings.misc.glo_movement_alert;
    data.default_settings.transmis_mode = device_settings.misc.glo_transmis_mode;
    data.default_settings.warning_alert = device_settings.misc.glo_warning_alert;

    NV_Data_Write_mock(&data);    // Save the values into NV
}

/******************************************************************************
** Name:    Settings_NFC_Check
*****************************************************************************/
/**
** @brief   Checks the coherence of the device settings through NFC
**
** @param   pNFC_Settings   pointer to the NFC settings to check
**
** @return  bool_t indicating settings error
******************************************************************************/
static bool_t Settings_NFC_Check (NFC_SETTINGS_t *pNFC_Settings)
{
    Settings_Swap (pNFC_Settings);

    // check items
    if ((pNFC_Settings->aCPR_init > MAX_RCP_TIME)           ||
        (pNFC_Settings->aCPR_1    > MAX_RCP_TIME)           ||
        (pNFC_Settings->aCPR_2    > MAX_RCP_TIME)           ||
        (pNFC_Settings->pCPR_init > MAX_RCP_TIME)           ||
        (pNFC_Settings->pCPR_1    > MAX_RCP_TIME)           ||
        (pNFC_Settings->pCPR_2    > MAX_RCP_TIME)           ||
        (pNFC_Settings->cpr_msg_long > 2)                   ||
        (pNFC_Settings->analysis_cpr > 2)                   ||
        (pNFC_Settings->asys_detect  > 2)                   ||
        (pNFC_Settings->asys_time    > MAX_ASYSTOLE_TIME)   ||
        (pNFC_Settings->metronome_en > 2)                   ||
        (pNFC_Settings->metronome_ratio_a    > 2)           ||
        (pNFC_Settings->metronome_ratio_p    > 2)           ||
        (pNFC_Settings->metronome_rate    > 3)              ||
        (pNFC_Settings->feedback_en    > 2)                 ||
        (pNFC_Settings->consec_shocks > MAX_CONSEC_SHOCKS)  ||
       ((pNFC_Settings->energy_shock1_a != 150) &&
        (pNFC_Settings->energy_shock1_a != 175) &&
        (pNFC_Settings->energy_shock1_a != 200))            ||
       ((pNFC_Settings->energy_shock2_a != 150) &&
        (pNFC_Settings->energy_shock2_a != 175) &&
        (pNFC_Settings->energy_shock2_a != 200))            ||
       ((pNFC_Settings->energy_shock3_a != 150) &&
        (pNFC_Settings->energy_shock3_a != 175) &&
        (pNFC_Settings->energy_shock3_a != 200))            ||
       ((pNFC_Settings->energy_shock1_p != 50) &&
        (pNFC_Settings->energy_shock1_p != 65) &&
        (pNFC_Settings->energy_shock1_p != 75) &&
        (pNFC_Settings->energy_shock1_p != 90))             ||
       ((pNFC_Settings->energy_shock2_p != 50) &&
        (pNFC_Settings->energy_shock2_p != 65) &&
        (pNFC_Settings->energy_shock2_p != 75) &&
        (pNFC_Settings->energy_shock2_p != 90))             ||
       ((pNFC_Settings->energy_shock3_p != 50) &&
        (pNFC_Settings->energy_shock3_p != 65) &&
        (pNFC_Settings->energy_shock3_p != 75) &&
        (pNFC_Settings->energy_shock3_p != 90))             ||
        (pNFC_Settings->patient_type  > 1)                  ||
        (pNFC_Settings->mains_filter  > 3)                  ||
        (pNFC_Settings->language      > MAX_LANGUAGES)     ||
        (pNFC_Settings->audio_volume  > MAX_AUD_VOLUME)     ||
        ((pNFC_Settings->utc_time     < UTC_MIN) || (pNFC_Settings->utc_time > UTC_MAX))      ||
        (pNFC_Settings->trainer_mode  >= 2)     ||
        (pNFC_Settings->patient_alert >= 2)     ||
        (pNFC_Settings->movement_alert>= 2)     ||
        (pNFC_Settings->transmis_mode >= 2)     ||
        (pNFC_Settings->warning_alert >= 2))   return FALSE;

    // if the read information is OK ...
    return TRUE;
}

/******************************************************************************
** Name:    Settings_NFC_Check_Montador
*****************************************************************************/
/**
** @brief   Checks the coherence of the device settings through NFC and FIXED values for the PCB Manufacture
**
** @param   pNFC_Settings   pointer to the NFC settings to check
**
** @return  bool_t indicating settings error
******************************************************************************/
static bool_t Settings_NFC_Check_Montador (NFC_SETTINGS_t *pNFC_Settings)
{

    if (Settings_NFC_Check(pNFC_Settings) == FALSE) return FALSE;

    // Chect Exact values for the PCB's Manufacture
    if (pNFC_Settings->aCPR_init != 0) return FALSE;
    if (pNFC_Settings->aCPR_1    != 15) return FALSE;
    if (pNFC_Settings->aCPR_2    != 15) return FALSE;

    if (pNFC_Settings->pCPR_init != 0) return FALSE;
    if (pNFC_Settings->pCPR_1    != 15) return FALSE;
    if (pNFC_Settings->pCPR_2    != 15) return FALSE;

    // check shocking energies
    if (pNFC_Settings->energy_shock1_a != 200) return FALSE;
    if (pNFC_Settings->energy_shock2_a != 200) return FALSE;
    if (pNFC_Settings->energy_shock3_a != 200) return FALSE;

    if (pNFC_Settings->energy_shock1_p != 90) return FALSE;
    if (pNFC_Settings->energy_shock2_p != 90) return FALSE;
    if (pNFC_Settings->energy_shock3_p != 90) return FALSE;

    if (pNFC_Settings->metronome_rate != 2) return FALSE;

    // check other items
    if (pNFC_Settings->consec_shocks != 1) return FALSE;
    if (pNFC_Settings->patient_type  != 1) return FALSE;
    if (pNFC_Settings->mains_filter  != 1) return FALSE;
    if (pNFC_Settings->asys_time     != 0) return FALSE;
    if (pNFC_Settings->language      != 1) return FALSE;
    if (pNFC_Settings->audio_volume  != 4) return FALSE;

    // if the read information is OK ...
    return TRUE;
}


/******************************************************************************
** Name:    Settings_Rationale
*****************************************************************************/
/**
** @brief   Rationalize the device settings
** @param   none
**
** @return  none
**
******************************************************************************/
static void Settings_Rationale_mock (void)
{

	 printf("in Settings Rationale Default...");

    // check limited items
   /* if (device_settings.aed.aed_aCPR_init           >            0) device_settings.aed.aed_aCPR_init           = 0;
    if (device_settings.aed.aed_aCPR_1              > MAX_RCP_TIME) device_settings.aed.aed_aCPR_1              = MAX_RCP_TIME;
    if (device_settings.aed.aed_aCPR_2              > MAX_RCP_TIME) device_settings.aed.aed_aCPR_2              = MAX_RCP_TIME;
    if (device_settings.aed.aed_metronome_ratio_a   >            2) device_settings.aed.aed_metronome_ratio_a   = 2;

    if (device_settings.aed.aed_pCPR_init           >            0) device_settings.aed.aed_pCPR_init           = 0;
    if (device_settings.aed.aed_pCPR_1              > MAX_RCP_TIME) device_settings.aed.aed_pCPR_1              = MAX_RCP_TIME;
    if (device_settings.aed.aed_pCPR_2              > MAX_RCP_TIME) device_settings.aed.aed_pCPR_2              = MAX_RCP_TIME;
    if (device_settings.aed.aed_metronome_ratio_p   >            2) device_settings.aed.aed_metronome_ratio_p   = 2;

    if (device_settings.aed.aed_asys_time > MAX_ASYSTOLE_TIME) device_settings.aed.aed_asys_time = MAX_ASYSTOLE_TIME;

    if (device_settings.aed.aed_energy_shock1_a > MAX_ENERGY_A) device_settings.aed.aed_energy_shock1_a = MAX_ENERGY_A;
    if (device_settings.aed.aed_energy_shock2_a > MAX_ENERGY_A) device_settings.aed.aed_energy_shock2_a = MAX_ENERGY_A;
    if (device_settings.aed.aed_energy_shock3_a > MAX_ENERGY_A) device_settings.aed.aed_energy_shock3_a = MAX_ENERGY_A;

    if (device_settings.aed.aed_energy_shock1_p > MAX_ENERGY_P) device_settings.aed.aed_energy_shock1_p = MAX_ENERGY_P;
    if (device_settings.aed.aed_energy_shock2_p > MAX_ENERGY_P) device_settings.aed.aed_energy_shock2_p = MAX_ENERGY_P;
    if (device_settings.aed.aed_energy_shock3_p > MAX_ENERGY_P) device_settings.aed.aed_energy_shock3_p = MAX_ENERGY_P;

    if (device_settings.aed.aed_consec_shocks > MAX_CONSEC_SHOCKS) device_settings.aed.aed_consec_shocks = MAX_CONSEC_SHOCKS;
    if (device_settings.aed.aed_consec_shocks == 0) device_settings.aed.aed_consec_shocks = 1;

    if (device_settings.misc.glo_language > MAX_LANGUAGES) device_settings.misc.glo_language = 1;
    if (device_settings.misc.glo_language == 0) device_settings.misc.glo_language = 1;

    if (device_settings.misc.glo_volume > MAX_AUD_VOLUME) device_settings.misc.glo_volume = MAX_AUD_VOLUME;

    // Initialize pointers for access
    p_aed_settings  = &(device_settings.aed);
    p_misc_settings = &(device_settings.misc);*/
}

/******************************************************************************
** Name:    Settings_Safe_Load
*****************************************************************************/
/**
** @brief   Safe assignment of device settings from the NFC structure
** @param   none
**
** @return  none
**
******************************************************************************/
static void Settings_Safe_Load_mock (void)
{
    bool_t   settings_OK;

    printf("Settings_Safe_Load_mock\n");
    fflush(0);




    if (Is_Test_Mode_Montador())
    {
        settings_OK = Settings_NFC_Check_Montador (&nfc_settings);
        printf("NFC Settings : %d",settings_OK);
        fflush(0);


        if (settings_OK)
        {

           //Comm_Uart_Send("OK5");
           Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK5");
        }
        else
        {
           //Comm_Uart_Send("KO5");
           Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 5");
           Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        }
    }
    else
    {
        settings_OK = Settings_NFC_Check (&nfc_settings);
    }


    if (settings_OK)
    {


        // Configuration - AED
        device_settings.aed.aed_aCPR_init         = nfc_settings.aCPR_init;
        device_settings.aed.aed_aCPR_1            = nfc_settings.aCPR_1;
        device_settings.aed.aed_aCPR_2            = nfc_settings.aCPR_2;
        device_settings.aed.aed_pCPR_init         = nfc_settings.pCPR_init;
        device_settings.aed.aed_pCPR_1            = nfc_settings.pCPR_1;
        device_settings.aed.aed_pCPR_2            = nfc_settings.pCPR_2;
        device_settings.aed.aed_cpr_msg_long      = nfc_settings.cpr_msg_long;

        device_settings.aed.aed_analysis_cpr      = nfc_settings.analysis_cpr;
        device_settings.aed.aed_asys_detect       = nfc_settings.asys_detect;
        device_settings.aed.aed_asys_time         = nfc_settings.asys_time;
        device_settings.aed.aed_metronome_en      = nfc_settings.metronome_en;
        device_settings.aed.aed_feedback_en       = nfc_settings.feedback_en;
        device_settings.aed.aed_consec_shocks     = nfc_settings.consec_shocks;
        device_settings.aed.aed_energy_shock1_a   = nfc_settings.energy_shock1_a;
        device_settings.aed.aed_energy_shock2_a   = nfc_settings.energy_shock2_a;
        device_settings.aed.aed_energy_shock3_a   = nfc_settings.energy_shock3_a;
        device_settings.aed.aed_metronome_ratio_a = nfc_settings.metronome_ratio_a;
        device_settings.aed.aed_energy_shock1_p   = nfc_settings.energy_shock1_p;
        device_settings.aed.aed_energy_shock2_p   = nfc_settings.energy_shock2_p;
        device_settings.aed.aed_energy_shock3_p   = nfc_settings.energy_shock3_p;
        device_settings.aed.aed_metronome_ratio_p = nfc_settings.metronome_ratio_p;
        device_settings.aed.aed_metronome_rate    = nfc_settings.metronome_rate;

        device_settings.misc.glo_patient_adult    = nfc_settings.patient_type;
        device_settings.misc.glo_filter           = nfc_settings.mains_filter;
        device_settings.misc.glo_language         = nfc_settings.language;
        device_settings.misc.glo_volume           = nfc_settings.audio_volume;
        device_settings.misc.glo_utc_time         = nfc_settings.utc_time;

        device_settings.misc.glo_trainer_mode     = nfc_settings.trainer_mode;
        Set_Mode_Trainer ((bool_t)device_settings.misc.glo_trainer_mode);
        device_settings.misc.glo_patient_alert    = nfc_settings.patient_alert;
        device_settings.misc.glo_movement_alert   = nfc_settings.movement_alert;
        device_settings.misc.glo_transmis_mode    = nfc_settings.transmis_mode;
        device_settings.misc.glo_warning_alert    = nfc_settings.warning_alert;

        device_settings.misc.glo_auto_test_hour   = 0;
        device_settings.misc.glo_auto_test_min    = 0;
        device_settings.misc.glo_ecg_format       = 0;  // ECG format 0: standard 1: cabrera

        Save_Default_Settings();                        // Valid settings has been detected --> save them like default settings

    }
    else {

        Set_Default_Settings();

                       // if any error is detected, load default values
        NFC_Write_Settings_mock ();                          // save in the NFC memory

    }

    // rationalize the device settings
    Settings_Rationale_mock();

}

/******************************************************************************
** Name:    Com_NFC_Check
*****************************************************************************/
/**
** @brief   Validates the wifi data validation key
**
** @param   pNFC_Settings   pointer to the NFC settings to check
**
** @return  bool_t indicating settings error
**
******************************************************************************/
static bool_t Com_NFC_Check (NFC_DEVICE_COM_t *pNFC_Device_Com)
{
    uint8_t password[16];
    SWAP_UINT16(nfc_device_com.wpa_eap_enabled);        ///< Use WPA-entreprise protocol

    // check if data key is valid
    Device_pw ((uint8_t *) &password, (uint8_t *) &nfc_device_id.device_sn, RSC_NAME_SZ);
    if (memcmp(pNFC_Device_Com->wifi_dat_key, password, 12) == 0) return TRUE;
    return FALSE;
}

/******************************************************************************
** Name:    Wifi_Load_Config
*****************************************************************************/
/**
** @brief   Safe assignment of device wifi settings from the NFC structure
** @param   none
**
** @return  none
**
******************************************************************************/
static void Wifi_Load_Config (void)
{
    bool_t   settings_OK;

    settings_OK = Com_NFC_Check (&nfc_device_com);

    if (settings_OK)
    {
        device_comms.wpa_eap_enabled = nfc_device_com.wpa_eap_enabled;
        memcpy (device_comms.wlan_pass, nfc_device_com.wlan_pass, 32);
        memcpy (device_comms.wlan_ssid, nfc_device_com.wlan_ssid, 32);

        // save wifi settings to usd and delete NFC memory area of wifi
        Wifi_Save_To_uSD_mock();

        // clear NFC data in Wifi block area
        memset ((uint8_t *) &nfc_device_com, 0, sizeof (NFC_DEVICE_COM_t));
        NFC_Write_mock ((uint8_t *) &nfc_device_com, sizeof (NFC_DEVICE_COM_t), NFC_DEVICE_COMM_BLOCK);
        NFC_Write_mock ((uint8_t *) &nfc_device_com.wifi_dat_key, RSC_NAME_SZ, NFC_DEVICE_COMM_KEY);
    }
    else {
        // if any error is detected, dont load config
    }
}

/******************************************************************************
** Name:    Wifi_Save_To_uSD
*****************************************************************************/
/**
** @brief   Saves wifi to uSD
** @param   none
**
** @return  none
**
******************************************************************************/
void Wifi_Save_To_uSD_mock (void)
{
    uint8_t             fx_res;
    uint32_t            attributes;
    uint32_t            nBytes;
    uint8_t             my_buffer[128];
    printf("Wifi Save to uSD mock\n");

}

/******************************************************************************
** Name:    Settings_Open_From_uSD
*****************************************************************************/
/**
** @brief   Gets settings from uSD
** @param   none
**
** @return  none
**
******************************************************************************/
void Settings_Open_From_uSD_mock (void)
{


    printf("Settings Open From uSD mock\n");

    // Load and read configuration file on SD
    // if a "R100.cfg" is detected and a "firmware.srec" is detected, do not load "R100.cfg"




        // load device info (device options)
        // restore the calibration values
  /*      nfc_device_id.zp_adcs_short = device_info.zp_adcs_short;
        nfc_device_id.zp_adcs_calibration = device_info.zp_adcs_calibration;
*/
       Device_Init_mock(NULL);

        // store the NFC structure into the NFC structure
        NFC_Write_Device_Info_mock(false);
        NFC_Write_Device_ID_mock(false, false);

        // check if data is correct from the NFC structure ...
        Settings_Safe_Load_mock ();

        // store the NFC structure into the NFC memory
        NFC_Write_Device_Info_mock(true);
        NFC_Write_Device_ID_mock(true, false);
        NFC_Write_Settings_mock ();
}

/******************************************************************************
** Name:    Settings_Save_To_uSD
*****************************************************************************/
/**
** @brief   Saves settings and device info to uSD
** @param   none
**
** @return  none
**
******************************************************************************/
void Settings_Save_To_uSD_mock (void)
{
    uint8_t             fx_res;
    uint32_t            attributes;
    NFC_SETTINGS_t      my_settings;

    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Saving Settings to uSD");
    printf("Settings_Save_To_uSD_mock\n");


}

/******************************************************************************
** Name:    Settings_Open_From_NFC
*****************************************************************************/
/**
** @brief   Gets settings from NFC
**
** @param   none
**
** @return  none
**
******************************************************************************/
void Settings_Open_From_NFC_mock (void)
{
    NFC_DEVICE_ID_t *prueba;
    /*prueba = &nfc_device_id;
    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Reading Settings from NFC");

    // read the device ID (to be reported in configuration mode !!! )
    NFC_Read ((uint8_t *) &nfc_device_id, sizeof (NFC_DEVICE_ID_t), NFC_DEVICE_ID_BLOCK);
    read_nfc_device_id = *prueba;

    // read the device info (to be reported in configuration mode !!! )
    NFC_Read ((uint8_t *) &nfc_device_info, sizeof (NFC_DEVICE_INFO_t), NFC_DEVICE_INFO_BLOCK);

    // read the settings structure !!!
    NFC_Read ((uint8_t *) &nfc_settings, sizeof (NFC_SETTINGS_t), NFC_DEVICE_SETTINGS_BLOCK);

     // read the communications structure !!!
   NFC_Read ((uint8_t *) &nfc_device_com, sizeof (NFC_DEVICE_COM_t), NFC_DEVICE_COMM_BLOCK);
    // read the communications data key !!!

    NFC_Read ((uint8_t *) &nfc_device_com.wifi_dat_key, RSC_NAME_SZ, NFC_DEVICE_COMM_KEY);

    fflush(0);

    // safe load of device settings from the NFC structure ...
    Settings_Safe_Load_mock ();
*/
    // validate and if ok proceed to read and erase wifi credentials from NFC
    printf("to  WifiLoadConfig...");


   // Wifi_Load_Config(); // COMMENTED FOR MOCK VERSION
}

/******************************************************************************
 ** Name:    Get_AED_Settings
 *****************************************************************************/
/**
 ** @brief   Return AED Settings
 ** @param   void
 **
 ** @return  AED_SETTINGS_t structure containing the AED settings
 ******************************************************************************/
AED_SETTINGS_t* Get_AED_Settings(void)
{
    return p_aed_settings;
}

/******************************************************************************
 ** Name:    Get_Misc_Settings
 *****************************************************************************/
/**
 ** @brief   Return Miscellaneous Settings
 ** @param   void
 **
 ** @return  MISC_SETTINGS_t structure containing the miscellaneous settings
 ******************************************************************************/
MISC_SETTINGS_t* Get_Misc_Settings(void)
{
    return p_misc_settings;
}

/******************************************************************************
 ** Name:    Get_Device_Settings
 *****************************************************************************/
/**
 ** @brief   Return Device Settings
 ** @param   void
 **
 ** @return  DEVICE_SETTINGS_t structure containing the device settings
 ******************************************************************************/
DEVICE_SETTINGS_t*  Get_Device_Settings(void)
{
    return &device_settings;
}



/******************************************************************************
** Name:    Get_APP_SW_Version
 *****************************************************************************/
/**
 ** @brief   Return APP SW version in string format
 ** @param   void
 **
 ** @return  string
 ******************************************************************************/
void  Get_APP_SW_Version (char_t* sw_version)
{
    sw_version[0] = (char_t)(((APP_REV_CODE >>24) & 0x0F) + '0');
    sw_version[1] = (char_t)(((APP_REV_CODE >>16) & 0x0F) + '0');
    sw_version[2] = (char_t)(((APP_REV_CODE >>8)  & 0x0F) + '0');
    sw_version[3] = (char_t)((APP_REV_CODE & 0x0F) + '0');
    sw_version[4] = 0;

}
