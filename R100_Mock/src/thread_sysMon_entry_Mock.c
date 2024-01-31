/******************************************************************************
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_sysMon_entry.c
 * @brief
 *
 * @version     v1
 * @date        16/01/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

/******************************************************************************
 ** Includes
 */
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "device_init.h"

#include "R100_Errors.h"
#include "types_basic_Mock.h"
#include "types_app.h"
#include "Resources/Trace_Mock.h"
#include "Resources/Comm.h"
#include "Drivers/RTC.h"
#include "thread_comm_entry.h"
#include "thread_patMon_entry.h"
#include "thread_sysMon_entry_Mock.h"

#include "DB_Episode_Mock.h"
#include "DB_Test_Mock.h"
#include "Drivers/I2C_1_Mock.h"
#include "Drivers/I2C_2_Mock.h"
#include "HAL/thread_audio_hal_Mock.h"
#include "HAL/thread_defibrillator_hal_Mock.h"
#include "HAL/thread_patMon_hal_Mock.h"
#include "Resources/Keypad_Mock.h"
#include "synergy_gen_Mock/thread_sysMon_Mock.h"
#include "thread_audio_entry.h"
#include "synergy_gen_Mock/thread_audio_Mock.h"
#include "thread_core_entry_Mock.h"
#include "synergy_gen_Mock/thread_core_Mock.h"

#include "synergy_gen_Mock/thread_comm_Mock.h"
#include "thread_api.h"



#ifdef UNIT_TESTS
#include "unit_tests.h"
#endif

/******************************************************************************
 ** Macros
 */

#define HEX_TO_ASCII(x)                 (uint8_t) (((x<=9) ? (x+'0') : ((x + 'A') - 10)))

#define GPS_ZERO                        "\0\0\0\0"

/******************************************************************************
 ** Defines
 */

//#define   DEF_PSU_EN                  IOPORT_PORT_08_PIN_08 ###### defined in "thread_defibrillator_hal.h"
//#define   IO_PSU_EN                   IOPORT_PORT_06_PIN_10 ###### defined in "thread_defibrillator_hal.h"
//#define     EXEC_TEST                   IOPORT_PORT_06_PIN_01

#define     VB_25C_SAFE_BATTERY         10000   // security voltage
#define     VB_00C_SAFE_BATTERY         9500    // security voltage


#define     VB_SAFE_BATTERY             9300   // security voltage
#define     SMON_DCMAIN_MAX             15500L          ///< maximun DC_main voltage (milivolts)
#define     SMON_DCMAIN_MIN             8500L           ///< minimun DC_main voltage (milivolts)

#define     SMON_18V_MAX                19000L          ///< maximun DC_main voltage (milivolts)
#define     SMON_18V_MIN                16000L          ///< minimun DC_main voltage (milivolts)

#define     BATTERY_LOW_CHARGE          20              ///< battery charge percent to advice low battery
#define     BATTERY_REPLACE_CHARGE      10              ///< battery charge percent to demand the battery replacement
#define     BATTERY_VERY_LOW_TEMP       (-25)           ///< battery minimum storage temperature limit
#define     BATTERY_VERY_HIGH_TEMP      (70)            ///< battery maximum storage temperature limit
#define     BATTERY_LOW_TEMP            (0)             ///< battery minimum operation temperature limit
#define     BATTERY_HIGH_TEMP           (50)            ///< battery maximum operation temperature limit

#define     LED_BLINK_OFF               0x00            ///< set the blink led (ON_OFF led) in Off
#define     LED_BLINK_ON                0x01            ///< set the blink led (ON_OFF led) in On

#define     FLASH_DATA_ADD              0x40100000      // data flash memory address
#define     FLASH_DATA_SZ               0x0004000       // 16K

#define     TIME_TO_AUTOTEST            3               // time to execute the automatic test in UTC 0

#define     ACCELERATOR_TIME            0               // accelerate the time count for test purposes

#define     WARNING_PERIOD_SAT          1200            // period to play CALL SAT (20min)


/// TODO place the siganture in the appropiate place
//const unsigned char __attribute__((section (".Sign_Section"))) my_signature[] = { "Reanibex_100" };
//unsigned char __attribute__((section (".Sign_Section"))) my_signature[] = { "Reanibex_100" };
static uint32_t Check_Battery_Voltage (uint32_t my_vb, int8_t my_temperature, AUDIO_ID_e *pAudio_msg);

/******************************************************************************
 ** Typedefs
 */

/// Battery date/time (read from the battery pack)
typedef struct {
    uint8_t     sec;                    ///< seconds (0:59)
    uint8_t     min;                    ///< minutes (0:59)
    uint8_t     hour;                   ///< hour    (0:23)
    uint8_t     weekday;                ///< weekday (1:7)
    uint8_t     date;                   ///< date    (1:31)
    uint8_t     month;                  ///< month   (1:12)
    uint8_t     year;                   ///< year    (0:99)
     int8_t     utc_time;               ///< UTC time (-11:12)
} BATTERY_RTC_t;

/******************************************************************************
 ** Constants
 */

/******************************************************************************
 ** Externs
 */

extern gps_config_t gps;
/******************************************************************************
 ** Globals
 */

uint32_t    thread_supervisor;                      // thread supervisor
uint8_t     access_key[12];
bool_t      write_elec_last_event = false;                  // write las electrodes registered event in test

/******************************************************************************
 ** Locals
 */
static BATTERY_INFO_t       battery_info;               // battery information
static BATTERY_STATISTICS_t battery_statistics;         // battery statistics
static BATTERY_RTC_t        battery_RTC;                // battery RTC
static uint16_t             battery_charge;             // battery charge percent
static int8_t               battery_temperature;        // battery temperature
static uint32_t             pon_date;                   // Power on date (formatted)
static uint32_t             pon_time;                   // Power on time (formatted) UTC 0
static DEVICE_DATE_t        save_sw_date;

static ELECTRODES_DATA_t    electrodes_data;            // connected electrodes information
static bool_t               electrodes_presence_flag;   // electrodes presence flag
static bool_t               electrodes_pending_event;   // electrodes pending event to register
static ZP_SEGMENT_e         electrodes_zp_segment;      // ZP segment to be processed in the electrodes context

static NV_DATA_t            nv_data;                    // non volatile data

static bool_t               test_mode = false;          // flag indicating the test mode activation
static bool_t               test_mode_montador = false;         // flag indicating the test mode activation for PSB test
static bool_t               mode_trainer = false;       // flag indicating the TRAINER mode
static bool_t               flag_sysmon_task = false;   // flag to indicate if the task Sysmon is initialized or not
static bool_t               first_time = true;
static uint8_t              first_time_gps = true;      // number of tries to get GPS position
static bool_t               flag_power_off = false;     // flag to power-off device or not
static bool_t               flag_transmission_message = FALSE;
static uint8_t              nFails_lb = 0;              // number of consecutive low battery conditions
static uint8_t              nFails_rb = 0;              // number of consecutive replace battery conditions

static uint8_t              pic_version_major, pic_version_minor;     // Mayor and minor version
//static  FX_MEDIA   *sd_media = (FX_MEDIA *)NULL;    // handler of the SD media card

/******************************************************************************
 ** Prototypes
 */
void HardFault_Handler (void);


static void prvGetRegistersFromStack_mock( uint32_t *pulFaultStackAddress )
{
    /* These are volatile to try and prevent the compiler/linker optimising them
    away as the variables never actually get used.  If the debugger won't show the
    values of the variables, make them global my moving their declaration outside
    of this function. */
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr; /* Link register. */
    volatile uint32_t pc; /* Program counter. */
    volatile uint32_t psr;/* Program status register. */

    printf("prvGetRegistersFromStack_mock\n");

   /* UNUSED(r0);
    UNUSED(r1);
    UNUSED(r2);
    UNUSED(r3);
    UNUSED(r12);
    UNUSED(lr);
    UNUSED(pc);
    UNUSED(psr);*/

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
    for( ;; );
}

void HardFault_Handler_mock (void)
{

	printf("HardFault_Handler_mock\n");
    /*
    __asm volatile
        (
            " tst lr, #4                                                \n"
            " ite eq                                                    \n"
            " mrseq r0, msp                                             \n"
            " mrsne r0, psp                                             \n"
            " ldr r1, [r0, #24]                                         \n"
            " ldr r2, handler2_address_const                            \n"
            " bx r2                                                     \n"
            " handler2_address_const: .word prvGetRegistersFromStack    \n"
        );*/

    //BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

/******************************************************************************
** Name:    Check_Test_Abort_User
*****************************************************************************/
/**
** @brief   Check if user abort running test
**
** @param   none
**
** @return  true or false
**
******************************************************************************/
static bool_t Check_Test_Abort_User_mock (DB_TEST_RESULT_t *pResult)
{
  /*  if((Key_Read (0) == KEY_STATUS_ON)|| ((pResult->test_id == TEST_MANUAL_ID) && (Key_Read (KEY_COVER) == KEY_STATUS_ON)) /*||
       ((pResult->test_id != TEST_MANUAL_ID) && (Key_Read (KEY_COVER) == KEY_STATUS_OFF)))
              ((pResult->test_id == TEST_MANUAL_ID) && (pResult->test_status == TEST_ABORTED)))
        //{
          //  return true;
        //}*/
	printf("Check_Test_Abort_User_mock\n");
        return false;
}
/******************************************************************************
** Name:    Get_Checksum_Add
*****************************************************************************/
/**
** @brief   Calculates the data array checksum (addition)
**
** @param   pBuffer pointer to the array to process
** @param   size    number of bytes to process
**
** @return  checksum result
**
******************************************************************************/
static uint8_t Get_Checksum_Add (uint8_t *pBuffer, uint32_t size)
{
    uint32_t    i;          // global use index
    uint32_t    add = 0;    // add count

    // calculate the data checksum
    for (i=0; i<size; i++)
    {
        add += (uint32_t) pBuffer [i];
    }
    return ((uint8_t) add);
}

/******************************************************************************
** Name:    Get_Checksum_Xor
*****************************************************************************/
/**
** @brief   Calculates the data array checksum (XOR)
**
** @param   pBuffer   pointer to the structure to fill
** @param   size      number of bytes to process
**
** @return  checksum result
**
******************************************************************************/
static uint8_t Get_Checksum_Xor (uint8_t *pBuffer, uint32_t size)
{
    uint32_t    i;          // global use index
    uint8_t     xor = 0;    // xor result

    // calculate the data checksum
    for (i=0; i<size; i++)
    {
        xor ^= pBuffer [i];
    }
    return xor;
}

/******************************************************************************
** Name:    Fill_Update_Reg
*****************************************************************************/
/**
** @brief   Initializes the update register data string
**
** @param   pStr : pointer to a data string
** @param   pDate : pointer to a date struct
** @param   pTime : pointer to a time struct
**
******************************************************************************/
static void Fill_Update_Reg (char_t *pStr, DEVICE_DATE_t *pDate, DEVICE_TIME_t *pTime)
{
    // Fill the string
    strcpy (pStr, "20xx-MM-DD//HH:mm:ss//xxxx");
    pStr[2] = (char_t) ((pDate->year  / 10) + '0');
    pStr[3] = (char_t) ((pDate->year  % 10) + '0');
    pStr[5] = (char_t) ((pDate->month / 10) + '0');
    pStr[6] = (char_t) ((pDate->month % 10) + '0');
    pStr[8] = (char_t) ((pDate->date  / 10) + '0');
    pStr[9] = (char_t) ((pDate->date  % 10) + '0');

    pStr[12] = (char_t) ((pTime->hour / 10) + '0');
    pStr[13] = (char_t) ((pTime->hour % 10) + '0');
    pStr[15] = (char_t) ((pTime->min  / 10) + '0');
    pStr[16] = (char_t) ((pTime->min  % 10) + '0');
    pStr[18] = (char_t) ((pTime->sec  / 10) + '0');
    pStr[19] = (char_t) ((pTime->sec  % 10) + '0');

    pStr[22] = (char_t)(((APP_REV_CODE >>24) & 0x0F) + '0');
    pStr[23] = (char_t)(((APP_REV_CODE >>16) & 0x0F) + '0');
    pStr[24] = (char_t)(((APP_REV_CODE >>8)  & 0x0F) + '0');
    pStr[25] = (char_t)((APP_REV_CODE & 0x0F) + '0');

    pStr[26] = '\r';
    pStr[27] = '\n';
}


/******************************************************************************
 ** Name:    RTC_Test_Time
 *****************************************************************************/
/**
 ** @brief   compensate UTC time zone with TEST TIME
 **
 ** @param   none
 **
 ** @return  autotest time (UTC applied)
 ******************************************************************************/
static uint8_t RTC_Test_Time_mock (void)
{
    int32_t     aux;

    printf("RTC_Test_Time_mock\n");
    // check the UTC time


    return 1;
}


/******************************************************************************
 ** Name:    Is_SAT_Error
 *****************************************************************************/
/**
 ** @brief   determine if an error requires a SAT service or not
 **
 ** @param   dev_error     error code to check
 **
 ** @return  if the error is categorized as SAT or not
 ******************************************************************************/
//static bool_t Is_SAT_Error (ERROR_ID_e dev_error)
bool_t Is_SAT_Error_mock (ERROR_ID_e dev_error)
{
  /*  if (R100_Errors_cfg[dev_error].warning_error == 1)  // Critical_error
    {
        nv_data.status_led_blink = (nv_data.status_led_blink == LED_BLINK_ON)? R100_Errors_cfg[dev_error].led_flashing: LED_BLINK_OFF;
        return true;
    }*/
	printf("Is_SAT ERROR_mock!\n");

    return false;
}

/******************************************************************************
** Name:    Get_RTC_Month
*****************************************************************************/
/**
** @brief   Get RTC month
**
** @param   none
**
** @return  month number
**
******************************************************************************/
uint8_t Get_RTC_Month (void)
{
    return battery_RTC.month;
}

/******************************************************************************
** Name:    NV_Data_Read
*****************************************************************************/
/**
** @brief   Function that gets the non volatile data from the internal Data Flash
**
** @param   pointer to the structure to fill
**
** @return  none
**
******************************************************************************/
void NV_Data_Read_mock(NV_DATA_t *pNV_data)
{
    uint8_t     i, xor;
    uint8_t     *pData;

    printf("NV_Data_Read_mock\n");

    // assign the buffer pointer
    pData = (uint8_t *) pNV_data;

    // read the non volatile structure
    /*g_flash0.p_api->open (g_flash0.p_ctrl, g_flash0.p_cfg);
    g_flash0.p_api->read (g_flash0.p_ctrl, pData, FLASH_DATA_ADD, sizeof(NV_DATA_t));
    g_flash0.p_api->close(g_flash0.p_ctrl);
*/
    // check the data integrity
    xor = 0;
    for (i=0; i<sizeof(NV_DATA_t); i++) xor ^= pData[i];

    //Check some NV data
    if(pNV_data->time_warning >= (24 << 16))
    {
        pNV_data->time_warning = 0;
    }

    if ((pNV_data->must_be_0x55 != 0x55) || xor)
    {
        pNV_data->time_warning = 0;
        pNV_data->time_test    = RTC_Test_Time_mock();
        pNV_data->test_id      = 0;
        pNV_data->test_pending = false;
        pNV_data->update_flag = 0;
    }
}

/******************************************************************************
** Name:    NV_Data_Write
*****************************************************************************/
/**
** @brief   Function that updates the test id and saves it to data flash memory
**
** @param   pointer to the structure to write in the Data Flash
**
** @return  none
**
******************************************************************************/
void NV_Data_Write_mock (NV_DATA_t *pNV_data)
{
    uint8_t     i, xor;
    uint8_t     *pData;
    uint32_t    nBytes;
    NV_DATA_t   my_NV_data;

    printf("NV_Data_Write_mock\n");
    // assign the buffer pointer
    pData = (uint8_t *) pNV_data;

    // read the current data and if there are not changes to register, bye-bye
    NV_Data_Read_mock (&my_NV_data);

    if (memcmp ((uint8_t *) &my_NV_data, pData, sizeof(NV_DATA_t)) == 0) return;

    // set the integrity items
    pNV_data->must_be_0x55 = 0x55;
    xor = 0;
    for (i=0; i<sizeof(NV_DATA_t)-1; i++) xor ^= pData[i];
    pData[i] = xor;

    // WARNING --> Be sure that the operation can be executed with the device powered
    // Power on the external circuits, adding an extra time to stabilize the power supplies ...
   /* g_ioport.p_api->pinWrite (IO_PSU_EN,  IOPORT_LEVEL_HIGH);
    tx_thread_sleep (OSTIME_20MSEC);

    // write into the non volatile structure
    nBytes = sizeof(NV_DATA_t) + ((sizeof(NV_DATA_t) % 4) ? (4 - (sizeof(NV_DATA_t) % 4)) : 0);
    g_flash0.p_api->open (g_flash0.p_ctrl, g_flash0.p_cfg);
    g_flash0.p_api->erase(g_flash0.p_ctrl, FLASH_DATA_ADD, nBytes/4);
    g_flash0.p_api->write(g_flash0.p_ctrl, (uint32_t) pData, FLASH_DATA_ADD, nBytes);
    g_flash0.p_api->close(g_flash0.p_ctrl);*/
}

/******************************************************************************
** Name:    Is_Sysmon_Task_Initialized
*****************************************************************************/
/**
** @brief   Is task initialized?
**
** @param   none
**
** @return  true or false
**
******************************************************************************/
bool_t  Is_Sysmon_Task_Initialized (void)
{
    return flag_sysmon_task;
}


/******************************************************************************
** Name:    Get_NV_Data
*****************************************************************************/
/**
** @brief   Function that gets the non volatile data
**
** @param   pointer to the structure to fill
**
** @return  pointer to nv_data
**
******************************************************************************/
NV_DATA_t* Get_NV_Data (void)
{
    return &nv_data;
}

/******************************************************************************
** Name:    Set_NV_Data_Error_IF_NOT_SAT
*****************************************************************************/
/**
** @brief   Function that sets the non volatile data error
**
** @param   void
**
** @return  void
**
******************************************************************************/
void Set_NV_Data_Error_IF_NOT_SAT (ERROR_ID_e error)
{
    if(Is_SAT_Error_mock (nv_data.error_code) == false)
    {
        nv_data.error_code = error;
    }
}

/******************************************************************************
** Name:    Is_Test_Mode
*****************************************************************************/
/**
** @brief   Function that report if the devioce is in test mode or not
**
** @param   none
**
** @return  true if is working in test mode, else false
**
******************************************************************************/
bool_t Is_Test_Mode (void)
{
    return test_mode;
}

/******************************************************************************
** Name:    Is_Test_Mode_Montador
*****************************************************************************/
/**
** @brief   Function that report if the devioce is in test mode montador or not
**
** @param   none
**
** @return  true if is working in test mode, else false
**
******************************************************************************/
bool_t Is_Test_Mode_Montador (void)
{
    return test_mode_montador;
}

/******************************************************************************
** Name:    Is_Mode_Trainer
*****************************************************************************/
/**
** @brief   Function that report if the device is in mode trainer or not
**
** @param   none
**
** @return  true if is trainer mode, else false
**
******************************************************************************/
bool_t Is_Mode_Trainer (void)
{
    return mode_trainer;
}

/******************************************************************************
** Name:    Set_Mode_Trainer
*****************************************************************************/
/**
** @brief   Function that report if the device is in mode trainer or not
**
** @param   none
**
** @return  true if is trainer mode, else false
**
******************************************************************************/
void Set_Mode_Trainer (bool_t aux)
{
    mode_trainer = aux;
}
/******************************************************************************
 ** Name:    diff_time_days
 *****************************************************************************/
/**
 ** @brief   computes the difference time between two dates (in days)
 **          (date_1 - date_2)
 **
 ** @param   date_1     date 1 in (YYYY:MM:DD) format
 ** @param   date_2     date 2 in (YYYY:MM:DD) format
 **
 ** @return  difference in number of days
 ******************************************************************************/
static uint32_t diff_time_days (uint32_t date_1, uint32_t date_2)
{
    uint32_t    nDays_1, nDays_2;

    nDays_1 = ((date_1 >> 16) * 365) + (((date_1 >> 8) & 0xFF) * 30) + (date_1  & 0xFF);
    nDays_2 = ((date_2 >> 16) * 365) + (((date_2 >> 8) & 0xFF) * 30) + (date_2  & 0xFF);

    // return the number of days between the two dates
    return (nDays_1 > nDays_2) ? (nDays_1 - nDays_2) : 0;
}

/******************************************************************************
 ** Name:    diff_time_seconds
 *****************************************************************************/
/**
 ** @brief   computes the difference time between two hours (in seconds)
 **          (time_1 - time_2)
 **
 ** @param   time_1     time 1 in (HH:MM:SS) format
 ** @param   time_2     time 2 in (HH:MM:SS) format
 **
 ** @return  difference in seconds
 ******************************************************************************/
static uint32_t diff_time_seconds (uint32_t time_1, uint32_t time_2)
{
    uint32_t    sec_1, sec_2;

    // if time overflow has been detected, add 24 hours to the first operator
    if (time_2 > time_1) time_1 += (24 << 16);

    sec_1 = ((time_1 >> 16) * 60 * 60) + (((time_1 >> 8) & 0xFF) * 60) + (time_1  & 0xFF);
    sec_2 = ((time_2 >> 16) * 60 * 60) + (((time_2 >> 8) & 0xFF) * 60) + (time_2  & 0xFF);

    // return the number of seconds between the two times
    return (sec_1 - sec_2);
}

/******************************************************************************
 ** Name:    device_pw
 *****************************************************************************/
/**
 ** @brief   generates the device password for USB operation based on
 **          device serial number
 **
 ** @param   pPassword   generated device USB password
 ** @param   pDevice_sn  device serial number
 ** @param   size        number of bytes to process
 **
 ** @return  none
 ******************************************************************************/
void Device_pw (uint8_t *pPassword, uint8_t *pDevice_sn, uint32_t size)
{
    uint32_t my_carry_add;
    uint8_t  my_buff[4];
    uint8_t  i,j;      // global index

    my_buff[0] = my_buff[1] = my_buff[2] = my_buff[3] = 0;
    my_carry_add = 0;

    for (i=0; i<size; i++)
    {
        my_carry_add += pDevice_sn[i];
        my_buff[1]   ^= pDevice_sn[i];              // xor all char in sn

        // add all set bit in byte
        for (j=0; j<8; j++)
        {
            if (pDevice_sn[i] & (0x01<<j)) my_buff[2]++;
        }
    }

    // add all char in sn
    my_buff[0] = (uint8_t) my_carry_add;

    // add offset to bit set count
    my_buff[2] = (uint8_t) (my_buff[2] + (my_carry_add >> 8));

    // calculate nibble combination
    i = (uint8_t) ( (my_buff[0] >> 4) + (my_buff[1] >> 4) + (my_buff[2] >> 4));
    j = (uint8_t) ( (my_buff[0] + my_buff[1] + my_buff[2]) << 4);
    my_buff[3] = (uint8_t) ((i & 0x0F) + (j & 0xF0));

    // create string
    pPassword[0] = HEX_TO_ASCII ((my_buff[0] >> 4));
    pPassword[1] = HEX_TO_ASCII ((my_buff[0] & 0x0F));

    pPassword[2] = HEX_TO_ASCII ((my_buff[1] >> 4));
    pPassword[3] = HEX_TO_ASCII ((my_buff[1] & 0x0F));

    pPassword[4] = HEX_TO_ASCII ((my_buff[2] >> 4));
    pPassword[5] = HEX_TO_ASCII ((my_buff[2] & 0x0F));

    pPassword[6] = HEX_TO_ASCII ((my_buff[3] >> 4));
    pPassword[7] = HEX_TO_ASCII ((my_buff[3] & 0x0F));

    pPassword[8]  = 'D';
    pPassword[9]  = 'A';
    pPassword[10] = 'T';
    pPassword[11] = 0x00;           // NULL termination string
}

/******************************************************************************
 ** Name:    Refresh_Wdg
 *****************************************************************************/
/**
 ** @brief   Refresh the watchdog timer
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
inline static void Refresh_Wdg_mock(void)
{
   // iWDT.p_api->refresh(iWDT.p_ctrl);
	printf("Refresh Wdg mock\n");
	/*fflush(0);
	getchar();*/
}

/******************************************************************************
** Name:    Pasatiempos_Listening
*****************************************************************************/
/**
** @brief   Pass time doing nothing while is listening an audio message
**          and refreshing the watchdog !!!
**
******************************************************************************/
static void Pasatiempos_Listening_mock (void)
{
	printf("Pasatiempos_Listening_mock\n");
    do {
        Refresh_Wdg_mock ();
        //tx_thread_sleep (OSTIME_100MSEC);
        Sleep(100);
        printf("Sleep\n");
    } while (Is_Audio_Playing_mock ());
}

/******************************************************************************
** Name:    Pasatiempos
*****************************************************************************/
/**
** @brief   Pass time doing nothing, but refreshing the watchdog !!!
**
** @param   nTicks      number of ticks to stay in the hamaca
**
******************************************************************************/
static void Pasatiempos_mock(uint32_t nTicks)
{
    uint32_t loop_time;

    printf("Pasatiempos_mock\n");
    while (nTicks)
    {
        Refresh_Wdg_mock();
        loop_time = (nTicks > 100) ? 100 : nTicks;
        //tx_thread_sleep (loop_time);
       // Sleep(loop_time);

        nTicks -= loop_time;
    }
}

/******************************************************************************
** Name:    Get_Checksum_Add
*****************************************************************************/
/**
** @brief   Calculates the data array checksum (addition)
**
** @param   pBuffer pointer to the array to process
** @param   size    number of bytes to process
**
** @return  checksum result
**
******************************************************************************/
static void Save_Data_Before_Off (ERROR_ID_e ec)
{
    //Bye bye and try again
    // set the test time to 3:00 am
    nv_data.time_test = RTC_Test_Time_mock();
    Trace_Arg_mock (TRACE_NEWLINE, "  AUTOTEST_HOUR= %4d", nv_data.time_test);
    nv_data.error_code = ec;

    // save in NFC error code and battery and electrode updated info
    NFC_Write_Device_Info_mock(true);

    // wait while playing all pending audio messages ...
//    Pasatiempos_Listening ();
}


/******************************************************************************
 ** Name:    Lock_on_Panic
 *****************************************************************************/
/**
 ** @brief   lock the device because the program can not be executed normally
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
void Lock_on_Panic_mock(uint8_t error, uint8_t num)
{
    uint32_t    i;

    printf("Lock_on_Panic_mock\n");

    for (i=1; i<10; i++)
    {
        Led_On_mock  (0);
        Led_On_mock  (2);
        Led_On_mock  (1);        printf("Sleep\n");//tx_thread_sleep (OSTIME_100MSEC);
        Led_Off_mock (0);
        Led_Off_mock (2);
        Led_Off_mock (1);       printf("Sleep\n");// tx_thread_sleep (OSTIME_100MSEC);

        if ((i % 3) == 0) Pasatiempos_mock (500);
    }

    Trace_Arg_mock (TRACE_NEWLINE, "ERROR = %d", (uint32_t)error);
    Trace_Arg_mock (TRACE_NEWLINE, "POS = %d",  (uint32_t)num);

    // Bye-Bye
    nv_data.status_led_blink = LED_BLINK_OFF;
    R100_PowerOff_mock();
}




/******************************************************************************
 ** Name:    R100_Check_Warning
 *****************************************************************************/
/**
 ** @brief   Check if beep audio must be emitted due to warning error
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void R100_Check_Warning_mock (void)
{
    uint32_t        err_open;           // SD media open result ...

    printf("R100_Check_Warning_mock\n");
    // Bug4689
 /*   if (Get_Device_Settings()->misc.glo_warning_alert == FALSE)
    {
        return;
    }*/

    // Emit a beep sound if electrodes or battery is wrong
   /* if (diff_time_seconds (pon_time, nv_data.time_warning) >= WARNING_PERIOD_SAT)
    {
        nv_data.time_warning = pon_time;        // reset counter

        if (R100_Errors_cfg[nv_data.error_code].led_flashing == 0)
        {

            // Power on the external circuits, adding an extra time to stabilize the power supplies ...
           // g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_HIGH);
            //tx_thread_sleep (OSTIME_20MSEC);
        	printf("Sleep\n");
            Refresh_Wdg_mock ();
            // Load Device Settings from NFC (the device info is read to be available in configuration mode)
            Settings_Open_From_NFC_mock();
            Refresh_Wdg_mock ();
            // Load Device Info --> NFC data MUST be read before !!!
            Device_Init_mock (NULL);
            Set_Device_Date_time_mock();

            // resume task used for alerting ...
           // tx_thread_resume (&thread_audio);
          //  tx_thread_resume (&thread_comm);

            // open the uSD
           /* err_open = fx_media_init0_open ();
            if ((err_open != FX_SUCCESS) && (err_open != FX_PTR_ERROR))
            {
                Lock_on_Panic ((uint8_t)err_open, 14);
            }
         //   tx_thread_sleep (OSTIME_100MSEC);
            printf("Sleep\n");
            // Load Audio resources from SD-Card and proceed to play the SAT message
            if (err_open = Load_Audio_Resources_mock(), err_open != eERR_NONE)
            {
                Lock_on_Panic_mock ((uint8_t)err_open, 15);
            }
            else
            {
                //tx_thread_sleep (OSTIME_100MSEC);
                printf("Sleep\n");
                Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_TONE, TRUE);
                Pasatiempos_Listening_mock ();
            }
        }
    }*/
}


/******************************************************************************
 ** Name:    Identify_PowerOn_Event
 *****************************************************************************/
/**
 ** @brief   identifies the agent that roots the power on
 **
 ** @param   none
 **
 ** @return  source of the power on
 ******************************************************************************/
//static POWER_ON_SOURCE_e Identify_PowerOn_Event(void)
extern POWER_ON_SOURCE_e Identify_PowerOn_Event_mock(void)
{
    uint8_t pin_state;      // RTCIO pin state
    int32_t pin_mask;       // mask of all power on agents (RTC, Cover, On-Off)

    printf("Identify_PowerOn_Event_mock\n");
    // check if PCB test must be done
   // g_ioport.p_api->pinRead (EXEC_TEST, &pin_state);
    if (pin_state == 0)
    {
        return PON_TEST_MONTADOR;
    }

    // powered from ON-OFF button ???
    //g_ioport.p_api->pinRead (KEY_ONOFF, &pin_state);
    pin_mask  = (uint8_t) (pin_state ? 0x00 : 0x01);

    // powered when the cover has been removed ...
   // g_ioport.p_api->pinRead (KEY_COVER, &pin_state);
    pin_mask |= (uint8_t) (pin_state ? 0x02 : 0x00);

    // powered automatically from RTC ???
    //g_ioport.p_api->pinRead (RTCIO_PIN, &pin_state);
    pin_mask |= (uint8_t) (pin_state ? 0x04 : 0x00);

    // powered when USB is connected
    //g_ioport.p_api->pinRead (VBUS_DET, &pin_state);
    pin_mask |= (uint8_t) (pin_state ? 0x08 : 0x00);

    // SHOCK button pressed ???
    //g_ioport.p_api->pinRead (KEY_SHOCK, &pin_state);
    pin_mask |= (uint8_t) (pin_state ? 0x00 : 0x10);

    // PAT_TYPE button pressed ???
    //g_ioport.p_api->pinRead (KEY_PATYPE, &pin_state);
    pin_mask |= (uint8_t) (pin_state ? 0x00 : 0x20);

    // if powered from KEY_ONOFF, check the other keys to identify as Test request
    // PAT_TYPE pressed ===> force a manual test
    //      0x28 = mask to check buttons and VBUS_DET
    //      0x21 = when pat type and ONOFF buttons are pressed with no VBUS in the USB port

    if ((pin_mask & 0x28) == 0x20)
    {
        return PON_TEST;
        // TODO apply changes to PCBs so it does not accidentally trigger Manual Test
        return PON_BUTTON;
    }

    // analyze the mask of all power on agents (RTC, Cover, On-Off)
    switch (pin_mask & 0x0F)
    {
        case 0b00000001: return PON_BUTTON;     // ONOFF pressed
        case 0b00000010: return PON_COVER;
        case 0b00000011: return PON_BUTTON;
        case 0b00000100: return PON_RTC;
        case 0b00000101: return PON_BUTTON;
//        case 0b00000110: return PON_COVER;
        case 0b00000110: return PON_RTC;
        case 0b00000111: return PON_BUTTON;
        case 0b00001000: return PON_USB;
        case 0b00001001: return PON_USB;
        case 0b00001010: return PON_USB;
        case 0b00001100: return PON_USB;
        case 0b00001011: return PON_USB;
        case 0b00001101: return PON_USB;
        case 0b00001110: return PON_USB;
        case 0b00001111: return PON_USB;
        default        : return PON_COVER;      // by default, just in case
    }
}

/******************************************************************************
 ** Name:    Battery_Read_Info
 *****************************************************************************/
/**
 ** @brief   read the battery information
 **
 ** @param   pInfo     pointer to the structure to fill
 **
 ** @return  operation result (error code)
 ******************************************************************************/
/*static ssp_err_t Battery_Read_Info (BATTERY_INFO_t *pInfo)
{
    //ssp_err_t   ssp_error;      // ssp error code
	uint32_t ssp_error;
    uint8_t     checksum;       // data structure checksum

    //ssp_error = SSP_ERR_INVALID_POINTER;        // initialize the error code
    if (pInfo)
    {
        ssp_error = 1;

        // read the battery information from the battery pack (address 0x000)
        I2C1_Read_Eeprom (0x00, (uint8_t *) pInfo, sizeof(BATTERY_INFO_t));

        // check the data integrity ...
        checksum = Get_Checksum_Add ((uint8_t *) pInfo, sizeof(BATTERY_INFO_t)-1);
        if ((checksum != pInfo->checksum_add) || (pInfo->must_be_0xAA != 0xAA))
        {
            memset ((uint8_t *) pInfo, 0, sizeof(BATTERY_INFO_t));
            ssp_error = 0;
        }
    }

    // return the operation result
    return ssp_error;
}
*/
/******************************************************************************
 ** Name:    Battery_Get_Statistics
 *****************************************************************************/
/**
 ** @brief   get the battery statistics
 **
 ** @param   pData    pointer to battery statistics structure
 **
 ** @return  none
 ******************************************************************************/
void Battery_Get_Statistics (BATTERY_STATISTICS_t* pData)
{
    memcpy ((uint8_t *) pData, (uint8_t *) &battery_statistics, sizeof (BATTERY_STATISTICS_t));
}

/******************************************************************************
 ** Name:    Battery_Get_Info
 *****************************************************************************/
/**
 ** @brief   get the battery information
 **
 ** @param   pInfo     pointer to battery info structure
 **
 ** @return  none
 ******************************************************************************/
void Battery_Get_Info (BATTERY_INFO_t* pData)
{
    memcpy ((uint8_t *) pData, (uint8_t *) &battery_info, sizeof (BATTERY_INFO_t));
}

/******************************************************************************
 ** Name:    Battery_Read_Statistics
 *****************************************************************************/
/**
 ** @brief   read the battery statistics
 **
 ** @param   pStatistics     pointer to the structure to fill
 **
 ** @return  operation result (error code)
 ******************************************************************************/
/*static ssp_err_t Battery_Read_Statistics (BATTERY_STATISTICS_t *pStatistics)
{
    //ssp_err_t   ssp_error;      // ssp error code
	uint32_t ssp_error;
    uint8_t     checksum;       // data structure checksum

    ssp_error = 0;        // initialize the error code
    if (pStatistics)
    {
        ssp_error = 1;

        // read the battery information from the battery pack (address 0x300)
        I2C1_Read_Eeprom (0x300, (uint8_t *) pStatistics, sizeof(BATTERY_STATISTICS_t));

        // check the data integrity ...
        checksum = Get_Checksum_Xor ((uint8_t *) pStatistics, sizeof(BATTERY_STATISTICS_t)-1);
        if ((checksum != pStatistics->checksum_xor) || (pStatistics->must_be_0x55 != 0x55))
        {
            memset ((uint8_t *) pStatistics, 0, sizeof(BATTERY_STATISTICS_t));
            ssp_error = 2;
        }
    }

    // return the operation result
    return ssp_error;
}
*/
/******************************************************************************
 ** Name:    Battery_Write_Statistics
 *****************************************************************************/
/**
 ** @brief   write the battery statistics
 **
 ** @param   pStatistics     pointer to the structure to fill
 **
 ** @return  operation result (error code)
 ******************************************************************************/
/*static ssp_err_t Battery_Write_Statistics (BATTERY_STATISTICS_t *pStatistics)
{
    BATTERY_STATISTICS_t read_stat;
            //ssp_err_t   ssp_error;      // ssp error code
            uint32_t ssp_error;
    static  uint8_t     my_buffer[32];  // buffer to write a single page into the memory

    ssp_error = 0;        // initialize the error code
    if (pStatistics)
    {
        // fill the integrity markers ...
//        pStatistics->must_be_0x55 = 0x55;
        pStatistics->checksum_xor = Get_Checksum_Xor ((uint8_t *) pStatistics, sizeof(BATTERY_STATISTICS_t)-1);

        // fill the buffer to send ...
        my_buffer[0] = 0;
        memcpy (&my_buffer[1], (uint8_t *) pStatistics, sizeof(BATTERY_STATISTICS_t));

        // write the battery information to the battery pack (address 0x300)
        I2C1_Write_Eeprom (0x300, my_buffer, sizeof(BATTERY_STATISTICS_t)+1);

        // Check write process with a read operation
        //tx_thread_sleep (OSTIME_10MSEC);        // This time is necessary!!
        printf("Sleep\n");
        ssp_error = Battery_Read_Statistics (&read_stat);
        if ((ssp_error != 1) || (memcmp(pStatistics, &read_stat, sizeof (BATTERY_STATISTICS_t)) != 0))
        {
            return 2;
        }
    }

    // return the operation result
    return ssp_error;
}
*/
/******************************************************************************
 ** Name:    RTC_Normalize_Time
 *****************************************************************************/
/**
 ** @brief   compensate UTC time zone
 **
 ** @param   pRTC     pointer to the structure to fill
 **
 ** @return  none
 ******************************************************************************/
static void RTC_Normalize_Time (BATTERY_RTC_t *pRTC)
{
    struct tm   my_time;
    struct tm  *pTime;
    time_t      utc_time;

    // check the UTC time
    if ((pRTC->utc_time < UTC_MIN) || (pRTC->utc_time > UTC_MAX)) return;

    // fill the "tm" structure
    my_time.tm_sec = pRTC->sec;
    my_time.tm_min = pRTC->min;
    my_time.tm_hour = pRTC->hour;
    my_time.tm_mday = pRTC->date;
    my_time.tm_mon = pRTC->month - 1;
    my_time.tm_year = pRTC->year;
    my_time.tm_wday = 0;            // can be ignored
    my_time.tm_yday = 0;            // can be ignored
    my_time.tm_isdst = 0;           // use standard time

    // correct the UTC time
    utc_time = mktime (&my_time);
    utc_time += (pRTC->utc_time * 3600);
    pTime = gmtime (&utc_time);

    // fill the "application time" structure
    pRTC->sec   = (uint8_t) pTime->tm_sec;
    pRTC->min   = (uint8_t) pTime->tm_min;
    pRTC->hour  = (uint8_t) pTime->tm_hour;
    pRTC->date  = (uint8_t) pTime->tm_mday;
    pRTC->month = (uint8_t) (pTime->tm_mon + 1);
    pRTC->year  = (uint8_t) pTime->tm_year;
}

/***********************************************************************************************************************
 * Function Name: Battery_Read_RTC
 * Description  : read the battery RTC
 *
 * Arguments    : pRTC     pointer to the structure to fill
 * Return Value : operation result (error code)
 ***********************************************************************************************************************/
/*static ssp_err_t Battery_Read_RTC (BATTERY_RTC_t *pRTC)
{
    ssp_err_t       ssp_error;      // ssp error code
    BATTERY_RTC_t   rtc_bcd;        // battery RTC in BCD format

    ssp_error = SSP_ERR_INVALID_POINTER;        // initialize the error code
    if (pRTC)
    {
        ssp_error = SSP_SUCCESS;

        // read the battery information from the battery pack
        I2C1_Read_RTC ((uint8_t *) &rtc_bcd, sizeof(BATTERY_RTC_t));

        // convert from BCD to binary ...
        pRTC->sec      = (uint8_t) (((rtc_bcd.sec     >> 4) * 10) + (rtc_bcd.sec     & 0x0f));
        pRTC->min      = (uint8_t) (((rtc_bcd.min     >> 4) * 10) + (rtc_bcd.min     & 0x0f));
        pRTC->hour     = (uint8_t) (((rtc_bcd.hour    >> 4) * 10) + (rtc_bcd.hour    & 0x0f));
        pRTC->weekday  = (uint8_t) (((rtc_bcd.weekday >> 4) * 10) + (rtc_bcd.weekday & 0x0f));
        pRTC->date     = (uint8_t) (((rtc_bcd.date    >> 4) * 10) + (rtc_bcd.date    & 0x0f));
        pRTC->month    = (uint8_t) (((rtc_bcd.month   >> 4) * 10) + (rtc_bcd.month   & 0x0f));
        pRTC->year     = (uint8_t) (((rtc_bcd.year    >> 4) * 10) + (rtc_bcd.year    & 0x0f));
        pRTC->utc_time = (int8_t) Get_Device_Settings()->misc.glo_utc_time;

        // verify the date/time
        if ((pRTC->sec   > 59) || (pRTC->min   > 59) || (pRTC->hour  > 23) ||
            (pRTC->date  > 31) || (pRTC->month > 12) || (pRTC->year  > 99) ||
            (pRTC->date  == 0) || (pRTC->month == 0))
        {
            ssp_error = SSP_ERR_CLAMPED;

            // set a default value:  1-Jan 2020 at 00:00:00
            pRTC->sec     = 0;
            pRTC->min     = 0;
            pRTC->hour    = 0;
            pRTC->weekday = 1;
            pRTC->date    = 1;
            pRTC->month   = 1;
            pRTC->year    = 20;
        }

              // compensate the UTC time ...
//      RTC_Normalize_Time (pRTC);
    }


    // return the operation result
    return ssp_error;
}
*/
/***********************************************************************************************************************
 * Function Name: Battery_Read_Temperature
 * Description  : read the battery temperature sensor value
 *
 * Arguments    : pTemp     pointer to the temperature variable
 * Return Value : operation result (error code)
 ***********************************************************************************************************************/
/*static ssp_err_t Battery_Read_Temperature (int8_t *pTemp)
{
    int8_t pcb_temp;
    uint8_t error;

    // read from the temperature sensor mounted in the battery pack
    error = I2C1_Read_Temperature (pTemp);
    pcb_temp = (int8_t)patMon_Get_Temperature();

    if (error && (pcb_temp >=15)) *pTemp = pcb_temp;

    // return the operation result
    return 1;
}
*/
/******************************************************************************
 ** Name:    Battery_Get_Temperature
 *****************************************************************************/
/**
 ** @brief   get temperature value in battery
 **
 ** @param   none
 **
 ** @return  temperature in celsius degrees
 ******************************************************************************/
int8_t Battery_Get_Temperature (void)
{
    return battery_temperature;
}


/***********************************************************************************************************************
 * Function Name: Battery_Read_Update_Date
 * Description  : read the FW update date
 *
 * Arguments    : pDate     pointer to the date
 * Return Value : operation result (error code)
 ***********************************************************************************************************************/
/*static ssp_err_t Battery_Read_Update_Date (DEVICE_DATE_t *pDate)
{
    uint8_t err_open = 0;
    FX_FILE my_version_file;
    uint32_t nBytes;                    // read size

    sd_media = &sd_fx_media;    // initialize the SD media handler (just in case)

    // read from usd
    // get the mutex to access to the uSD
    tx_mutex_get(&usd_mutex, TX_WAIT_FOREVER);
    err_open = (uint8_t) fx_file_open(sd_media, &my_version_file, FNAME_DATE, FX_OPEN_FOR_READ);
    if (err_open == FX_SUCCESS)
    {
        err_open = (uint8_t) fx_file_read(&my_version_file, (uint8_t *) pDate, sizeof(DEVICE_DATE_t), &nBytes);
        err_open = (uint8_t) fx_file_close (&my_version_file);
        err_open = (uint8_t) fx_media_flush (sd_media);
    }

    // release the uSD mutex
    tx_mutex_put(&usd_mutex);

    if ((err_open) || (nBytes < (sizeof(DEVICE_DATE_t))))
    {
        pDate->date = 0;
        pDate->month = 0;
        pDate->year = 0;
    }

    // return the operation result
    return SSP_SUCCESS;
}
*/
/******************************************************************************
 ** Name:    Battery_Get_Charge
 *****************************************************************************/
/**
 ** @brief   computes the remaining capacity in the battery
 **
 ** @param   none
 **
 ** @return  remaining capacity (in %)
 ******************************************************************************/
uint16_t Battery_Get_Charge_mock (void)
{
    // NOTE: See "Reanibex_Life_Calculator.xlsx" file

	printf("Battery_Get_Charge_mock\n");
    // APPLY WITH NEW HW DJS 0014 and later version
    //#define CURRENT_WHEN_STANDBY_NEW_PCB    (9+1+37)    // device average current in standby  (in uA)
                                                        // 1% self discharge + battery's PCB current    --> 0.009mA/h
                                                        // standby (TLV3401) and LED every 10seconds, LED time 33msec    --> (0.001 + 0.037)mA/h




    #define CURRENT_WHEN_STANDBY_OLD_PCB_OLD_SW    (9+31+74)   // device average current in standby  (in uA)
                                                                // 1% self discharge + battery's PCB current    --> 0.009mA/h
                                                                // standby (LMC7221) and LED every 10seconds, LED time 66msec    --> (0.031 + 0.074)mA/h



    #define CURRENT_WHEN_STANDBY_OLD_PCB_NEW_SW    (9+31+37)   // DJS 0013 A version and more new...device average current in standby  (in uA)
                                                                // 1% self discharge + battery's PCB current    --> 0.009mA/h
                                                                // standby (LMC7221) and LED every 10seconds, LED time 33msec    --> (0.031 + 0.037)mA/h





    #define CURRENT_TEST_DAILY              (83)        // device average current in diary test (in uA/h)
    #define CURRENT_TEST_DAILY_COMMS        (650)       // device average current in diary test (in uA/h)
    #define CURRENT_TEST_MONTHLY            (270)       // device average current in monthly test (in uA/h)
    #define CURRENT_TEST_MONTHLY_COMMS      (4000)      // device average current in monthly test with communications (in uA/h), 3 minutes of GPS

    #define CURRENT_WHEN_RUNNING            86          // device average current when running  (in mA)
    #define CURRENT_WHEN_CHARGING           4000        // device average current when charging (in mA)
    #define CURRENT_WHEN_OPEN_COVER         630         // 630uA/hour during 20min (in uA)

    #define AVERAGE_TIME_TO_CHARGE          8           // average time to full charge the main capacitor (in seconds)
    #define AVERAGE_TIME_OPEN_COVER         (20)        // tick open cover 20 minutes

    uint32_t    nDays;                  // number of days to compute the standby discharge
    uint32_t    nTest_D, nTest_M;       // number of test
    uint32_t    discharge_standby_old;  // discharge in standby with old SW
    uint32_t    discharge_standby_new;  // discharge in standby with new SW
    uint32_t    discharge_standby_Manual_test;  // discharge in standby for doing manual Test
    uint32_t    discharge_standby = 0;      // discharge in standby
    uint32_t    discharge_running;      // discharge in running
    uint32_t    discharge_run_HV;       // discharge while running the HV circuits
    uint32_t    discharge_open_cover;   // discharge in standby with open cover
    uint32_t    my_date, sw_date;       // current date, SW update date
    uint32_t    nominal_capacity;       // compensated nominal capacity for the battery
    uint16_t    calc_cap;
    //uint32_t    CURRENT_WHEN_STANDBY = CURRENT_WHEN_STANDBY_OLD_PCB_OLD_SW;

    // check if the battery is a controlled battery (MUST contain valid information in the info and statistics structures)
   /* if ((battery_info.must_be_0xAA != 0xAA) || (battery_statistics.must_be_0x55 != 0x55))
    {
        return 0;
    }

    // apply a safety margin ...
    nominal_capacity = (battery_info.nominal_capacity * 95) / 100;
    if (nominal_capacity == 0) return 0;

    // format the current date ...
    my_date  = (uint32_t) (battery_RTC.year + 2000) << 16;
    my_date += (uint32_t)  battery_RTC.month <<  8;
    my_date += (uint32_t)  battery_RTC.date;


    // check if RTC is ok)
    if (my_date == 132382977)        // this value correspond do default date in case of RTC error
    {
        battery_charge = 0;
        return 0;
    }

    // format SW update date
    if (save_sw_date.year == 0)
    {
        sw_date = my_date;
    }
    else
    {
        sw_date  = (uint32_t) (save_sw_date.year + 2000) << 16;
        sw_date += (uint32_t)  save_sw_date.month <<  8;
        sw_date += (uint32_t)  save_sw_date.date;
    }
*/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APPLY WHEN NEW PCB and NEW SW --> BOARD VERSION DJS 0013 A and LATER
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*if(Get_NFC_Device_Info()->pcb_hw_version >= 13)
    {
        nDays     = diff_time_days (my_date, battery_info.manufacture_date);
        discharge_standby = (nDays * 24 * CURRENT_WHEN_STANDBY_NEW_PCB)/(nominal_capacity*10);
    }*/

    /*if(Get_NFC_Device_Info()->pcb_hw_version == 0)
    {
        //Trace_Arg (TRACE_NEWLINE, "sw_date = %d", (uint32_t)sw_date);
        //Trace_Arg (TRACE_NEWLINE, "my_date = %d", (uint32_t)my_date);

        // Software update date must be older or equal than current day
        if (diff_time_days(my_date, sw_date) == 0) sw_date = my_date;


        // calculate time to apply old consumptions
        nDays   = diff_time_days (sw_date, battery_info.manufacture_date);
        //Trace_Arg (TRACE_NEWLINE, "nDays_old = %d", (uint32_t)nDays);
        discharge_standby_old = (nDays * 24 * CURRENT_WHEN_STANDBY_OLD_PCB_OLD_SW)/(nominal_capacity*10);


        // calculate time to apply new consumptions
        if (nDays != 0)
        {
            //Battery is older than software update date
            nDays   = diff_time_days (my_date, sw_date);
        }
        else
        {
            // Battery is newer than software update, so count the number of days since battery manufacture date, not since sw_date
            nDays   = diff_time_days (my_date, battery_info.manufacture_date);
        }

        //Trace_Arg (TRACE_NEWLINE, "nDays_new = %d", (uint32_t)nDays);
        discharge_standby_new = (nDays * 24 * CURRENT_WHEN_STANDBY_OLD_PCB_NEW_SW)/(nominal_capacity*10);

        discharge_standby = discharge_standby_old + discharge_standby_new;
    }

    // Add monthly test consumptions
    nDays = diff_time_days (my_date, battery_info.manufacture_date);
    nTest_M = nDays / 30;

    //Trace_Arg (TRACE_NEWLINE, "nTest_Manual = %d", (uint32_t)battery_statistics.nTestManual);
    if (Get_Device_Info()->enable_b.sigfox || Get_Device_Info()->enable_b.wifi)
    {
        discharge_standby_Manual_test = (uint32_t)(battery_statistics.nTestManual * CURRENT_TEST_MONTHLY_COMMS)/(nominal_capacity*10);
        discharge_standby += (nTest_M  * CURRENT_TEST_MONTHLY_COMMS)/(nominal_capacity*10);
    }
    else
    {
        discharge_standby_Manual_test = (uint32_t)(battery_statistics.nTestManual * CURRENT_TEST_MONTHLY)/(nominal_capacity*10);
        discharge_standby += (nTest_M  * CURRENT_TEST_MONTHLY)/(nominal_capacity*10);
    }

    discharge_standby += discharge_standby_Manual_test;
    nTest_D =  nDays - nTest_M;

    if (Get_Device_Info()->enable_b.sigfox || Get_Device_Info()->enable_b.wifi)
    {
        discharge_standby += (nTest_D * CURRENT_TEST_DAILY_COMMS)/(nominal_capacity*10);
    }
    else
    {
        discharge_standby += (nTest_D * CURRENT_TEST_DAILY)/(nominal_capacity*10);
    }


    // get the battery discharge percent when running (monitoring the patient, maintenance, autotest, etc...)  ...
    discharge_running = (battery_statistics.runTime_total * CURRENT_WHEN_RUNNING * 10) / (nominal_capacity * 6);

    // get the battery discharge percent when running different HV circuits (capacitor precharge, full charge, shock, etc...)
    discharge_run_HV =  (uint32_t) (battery_statistics.nFull_charges * AVERAGE_TIME_TO_CHARGE * CURRENT_WHEN_CHARGING) /  (nominal_capacity * 36);

    // get the battery discharge percent when running different HV circuits (capacitor precharge, full charge, shock, etc...)
    discharge_open_cover =  (uint32_t) (battery_statistics.nTicks_open_cover * AVERAGE_TIME_OPEN_COVER * CURRENT_WHEN_OPEN_COVER) /  (nominal_capacity * 600);

    // return the remaining capacity
    if ((discharge_standby + discharge_running + discharge_run_HV + discharge_open_cover) > 100)
    {
        battery_charge = 1;
        return 1;
    }

    // Apply a compensation ONLY for new batteries
    if (nDays <= (8*30) &&                                                       // age less than 8 months
       ((discharge_standby_Manual_test + discharge_running + discharge_open_cover + discharge_run_HV) < (100-89)))  // used capacity less than (100-89)%
    {
        battery_charge = calc_cap = ((uint16_t) (100 - (discharge_standby/4 + discharge_running + discharge_run_HV + discharge_open_cover)));
    }
    else if (nDays <= (9*30) &&                                                       // age less than 9 months
            ((discharge_standby_Manual_test + discharge_running + discharge_open_cover + discharge_run_HV) < (100-89)))  // used capacity less than (100-89)%
            {
                battery_charge = calc_cap = ((uint16_t) (100 - (discharge_standby/2 + discharge_running + discharge_run_HV + discharge_open_cover)));
            }
    else
    {
        battery_charge = calc_cap = ((uint16_t) (100 - (discharge_standby + discharge_running + discharge_run_HV + discharge_open_cover)));
    }*/

    return battery_charge;

}

/******************************************************************************
 ** Name:    Battery_i2C_Init
 *****************************************************************************/
/**
 ** @brief   initializes the battery i2c port to communicate with the internals RTC and EEprom
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void Battery_i2C_Init_mock(void)
{
    //ssp_err_t   ssp_error;      // ssp error code

	int ssp_error=0;

	printf("Battery_i2C_Init_mock\n");
    // read the battery information from the battery pack (if any error is detected, set battery name as "Unknown")
   // ssp_error = Battery_Read_Info (&battery_info);
   // if (ssp_error != SSP_SUCCESS) strcpy ((char_t *) battery_info.name, "Unknown");

    // read the battery statistics from the battery pack (if any error is detected, set battery name as "Unknown")
  // ssp_error = Battery_Read_Statistics (&battery_statistics);
  //  if (ssp_error != SSP_SUCCESS) strcpy ((char_t *) battery_info.name, "Unknown");

    // read the battery current date & time from the battery pack
//   ssp_error = Battery_Read_RTC (&battery_RTC);

 //   ssp_error = Battery_Read_Temperature (&battery_temperature);

    // Save upgrade date
//    ssp_error = Battery_Read_Update_Date (&save_sw_date);

/*

    // check if must accelerate the time count
#if (ACCELERATOR_TIME == 1)
    battery_RTC.hour = battery_RTC.min % 24;
#endif

    // register the power on date
    pon_date  = (uint32_t) (battery_RTC.year + 2000) << 16;
    pon_date += (uint32_t)  battery_RTC.month <<  8;
    pon_date += (uint32_t)  battery_RTC.date;

    // register the power on time
    pon_time  = (uint32_t) (battery_RTC.hour << 16);
    pon_time += (uint32_t) (battery_RTC.min  <<  8);
    pon_time += (uint32_t) (battery_RTC.sec);
*/
}

/******************************************************************************
 ** Name:    Battery_i2C_Init_RTC
 *****************************************************************************/
/**
 ** @brief   initializes the battery i2c port to communicate with the internals RTC and EEprom
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void Battery_i2C_Init_RTC_mock(void)
{
    // read the battery current date & time from the battery pack
    //Battery_Read_RTC (&battery_RTC);
	printf("Battery_i2C_Init_RTC_mock\n");

  /*  // register the power on date
    pon_date  = (uint32_t) (battery_RTC.year + 2000) << 16;
    pon_date += (uint32_t)  battery_RTC.month <<  8;
    pon_date += (uint32_t)  battery_RTC.date;

    // register the power on time
    pon_time  = (uint32_t) (battery_RTC.hour << 16);
    pon_time += (uint32_t) (battery_RTC.min  <<  8);
    pon_time += (uint32_t) (battery_RTC.sec);
*/
}


/******************************************************************************
 ** Name:    R100_Check_SAT
 *****************************************************************************/
/**
 ** @brief   Power off the device programming an automatic power-on
 **
 ** @param   dev_error         current device error
 ** @param   force_warning     force the warning message
 **
 ** @return  none
 ******************************************************************************/
static void R100_Check_SAT_mock (ERROR_ID_e dev_error, bool_t force_warning)
{
    uint32_t        err_open;           // SD media open result ...

    printf("R100_Check_SAT_mock\n");
    // check if the error code requires SAT
    if (Is_SAT_Error_mock (dev_error) == FALSE)
    {
        return;
    }

    Trace_Arg_mock (TRACE_NEWLINE, "  DEVICE ERROR= %4d", dev_error);

    // check if led status must be blink or not
  /*  nv_data.status_led_blink = (nv_data.status_led_blink == LED_BLINK_ON)? R100_Errors_cfg[dev_error].led_flashing: LED_BLINK_OFF;

    nv_data.error_code = dev_error;

    // a critical error is detected --> warn the user periodically
    if (force_warning || (diff_time_seconds (pon_time, nv_data.time_warning) >= WARNING_PERIOD_SAT))
    {
        // Startup is nor RTC mode, so reconfigure clocks with PLL configuration
        bsp_clock_init_Reconfigure();

        // register the time for the next warning
        nv_data.time_warning = pon_time;

        // maintain the power-on switch active
        Rtc_Program_Wakeup (WAKEUP_POWERON);

        // Power on the external circuits, adding an extra time to stabilize the power supplies ...
      //  g_ioport.p_api->pinWrite (IO_PSU_EN,  IOPORT_LEVEL_HIGH);
      //  tx_thread_sleep (OSTIME_20MSEC);
        printf("Check SAT: Sleep\n");

        // report the startup event
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Powering Off --> Critical Error");

        // Load Device Settings from NFC (the device info is read to be available in configuration mode)
        Settings_Open_From_NFC_mock();

        // Load Device Info --> NFC data MUST be read before !!!
//        Device_Init (NULL);
//        // get the device info
//        memcpy(&my_Device_Info, Get_Device_Info(), sizeof(DEVICE_INFO_t));

        // resume task used for test ...
      //  tx_thread_resume (&thread_audio);
//        tx_thread_resume (&thread_comm);

        // open the uSD
      //  err_open = fx_media_init0_open ();
     //   if ((err_open != FX_SUCCESS) && (err_open != FX_PTR_ERROR))
     //  {
      //      Lock_on_Panic ((uint8_t)err_open, 10);
      //  }
      //  tx_thread_sleep (OSTIME_100MSEC);

        // Load Audio resources from SD-Card and proceed to play the SAT message
        if (err_open = Load_Audio_Resources_mock(), err_open != eERR_NONE)
        {
            Lock_on_Panic_mock ((uint8_t)err_open, 11);
        }
        else
        {
           // tx_thread_sleep (OSTIME_100MSEC);
        	printf("Sleep\n");
            if(dev_error == eERR_BATTERY_REPLACE) Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_REPLACE_BATTERY, TRUE);
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
            Pasatiempos_Listening_mock ();
        }
*/
        Battery_i2C_Init_mock();

//        if ((my_Device_Info.device_options & ENABLE_OPT_SIGFOX) != 0)
//        {
//            if(nv_data.check_sat_tx_flag == 0)
//            {
//                // Send Sigfox Alert
//                Comm_Sigfox_Open();
//                Refresh_Wdg ();
//
//                // Generate and Send Test report using Sigfox
//                Comm_Sigfox_Generate_Send_Alert(MSG_ID_COVER_OPEN_ALERT);
//                Refresh_Wdg ();
//
//                // Advice to the user periodically -- ???
//                // Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);
//                Pasatiempos (OSTIME_10SEC);
//                // Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);
//                Pasatiempos (OSTIME_10SEC);
//                //Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_DONE, TRUE);
//
//                nv_data.check_sat_tx_flag = 1;
//            }
//        }


        // Bye-Bye
        R100_PowerOff_mock();
    }
//}

/******************************************************************************
 ** Name:    R100_Check_Update
 *****************************************************************************/
/**
 ** @brief   Check if firmware has been updated
 **
 ** @param   pointer to power-on type
 **
 ** @return  none
 ******************************************************************************/
static void R100_Check_Update_mock (POWER_ON_SOURCE_e* paux)
{
    uint8_t         my_buf[] = {"L#"};      // Life-beat command for boot processor
    uint32_t        attributes, err_open;   // SD media open result ...
    uint32_t        nBytes;
    uint32_t        my_prev_ver;
    //FX_FILE         version_file;
    FILE			version_file;
    uint8_t         boot_msg;               // boot processor response char
    DEVICE_DATE_t   upgrade_date;
    DEVICE_TIME_t   upgrade_time;
    char_t          update_buffer[28];

    printf("R100_Check_Update_mock\n");

    // update performed flag
    if (nv_data.update_flag)
    {
        Led_On_mock (LED_ONOFF);
        // reset update flag
        nv_data.update_flag = 0;

        // maintain the power-on switch active
        Rtc_Program_Wakeup (WAKEUP_POWERON);
        Refresh_Wdg_mock ();

        // Power on the external circuits, adding an extra time to stabilize the power supplies ...
     //   g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_HIGH);
     //   tx_thread_sleep (OSTIME_20MSEC);
        printf("Sleep R100_Check_Update\n");

        // send life-beat to boot processor (including NULL character)
        boot_msg = Boot_Send_Message_mock(my_buf, 3);    // dummy transfer
        Refresh_Wdg_mock ();
        boot_msg = Boot_Send_Message_mock(my_buf, 3);    // test with Life-Beat
        Refresh_Wdg_mock ();
       // UNUSED(boot_msg);

        // Load Device Settings from NFC (the device info is read to be available in configuration mode)
        Settings_Open_From_NFC_mock();
        Refresh_Wdg_mock ();

        // Load Device Info --> NFC data MUST be read before !!!
        Device_Init_mock(NULL);

        // open the uSD
      /*  err_open = fx_media_init0_open ();
        if ((err_open != FX_SUCCESS) && (err_open != FX_PTR_ERROR))
        {
            Lock_on_Panic ((uint8_t)err_open, 16);
        }
        tx_thread_sleep (OSTIME_100MSEC);
*/
        // Check if a new configuration file is present in the uSD to update the Device Settings
        Settings_Open_From_uSD_mock ();

        R100_Check_SAT_mock(Get_NFC_Device_Info()->error_code,true);

        // resume task used for alerting ...
     //   tx_thread_resume (&thread_audio);

        // Load Audio resources from SD-Card and proceed to play audio message
        if (err_open = Load_Audio_Resources_mock(), err_open != eERR_NONE)
        {
            Lock_on_Panic_mock ((uint8_t)err_open, 17);
        }

        //tx_thread_sleep (OSTIME_100MSEC);
        Sleep(100);
        printf("Sleep 100msec\n");
        Refresh_Wdg_mock ();

        //Ask for update process result
        Refresh_Wdg_mock ();
        my_buf[0]='M';
        pic_version_major = Boot_Send_Message_mock(my_buf, 3);    // test with Life-Beat
        Refresh_Wdg_mock ();                             // refresh the watchdog timer
        my_buf[0]='m';
        pic_version_minor = Boot_Send_Message_mock(my_buf, 3);    // test with Life-Beat

        Trace_Arg_mock (TRACE_NEWLINE, "PIC MAYOR VERSION = %3d", pic_version_major);
        Trace_Arg_mock (TRACE_NEWLINE, "PIC MINOR VERSION = %3d", pic_version_minor);

        if ((pic_version_major == 0) ||
            (pic_version_minor == 0))
            {
                // old pic software
                pic_version_major = pic_version_minor= '0';
            }
        Refresh_Wdg_mock ();

        //ONLY for PIC mayor version greater or equal to '1'
        if (pic_version_major >= '1')
        {
            my_buf[0] = 'V';
            boot_msg = Boot_Send_Message_mock(my_buf, 3);
            Trace_Arg_mock (TRACE_NEWLINE, "PIC ack : %1c ", boot_msg);
            Refresh_Wdg_mock ();
        }
        else
        {
            boot_msg = 'T';
        }
        // Load and read version file on SD
        printf("Load and read version file on SD\n");
        //err_open = (uint8_t) fx_file_open(&sd_fx_media, &version_file, FNAME_VERSION, FX_OPEN_FOR_READ);
       /* if (err_open == FX_SUCCESS)
        {
            err_open = (uint8_t) fx_file_read(&version_file, (uint8_t *) &my_prev_ver, sizeof(uint32_t), &nBytes);
            err_open = (uint8_t) fx_file_close (&version_file);
            err_open = (uint8_t) fx_file_delete (&sd_fx_media, FNAME_VERSION);
            err_open = (uint8_t) fx_media_flush (&sd_fx_media);
        }
        else
        {
            nv_data.error_code = eERR_BOOT_PROCESSOR_UPDATE;
        }*/

        if ((APP_REV_CODE == my_prev_ver) || (err_open != 1))
        {
            Trace_Arg_mock (TRACE_NEWLINE, "Old version : %5d ", my_prev_ver);
            Trace_Arg_mock (TRACE_NEWLINE, "New version : %5d ", APP_REV_CODE);
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_UPGRADE_ERROR, TRUE);
            Pasatiempos_Listening_mock ();
        }

        if (boot_msg != 'T')
        {
            //Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_CALL_SAT, TRUE);
            //Pasatiempos_Listening ();
            nv_data.error_code = eERR_BOOT_PROCESSOR_UPDATE;
            // Bye-Bye
            R100_PowerOff_mock();
        }

        // Read Time
      // battery_RTC.utc_time = (int8_t) Get_Device_Settings()->misc.glo_utc_time;
        RTC_Normalize_Time (&battery_RTC);
        Set_Device_Date_time_mock();
        // Calculate upgrade date
        Get_Date (&upgrade_date.year, &upgrade_date.month, &upgrade_date.date);
        Get_Time (&upgrade_time.hour, &upgrade_time.min,   &upgrade_time.sec);

        // Register upgrade data ONLY ONCE, in order to asjust batery remaining capacity
       // err_open = (uint8_t) fx_file_attributes_read (&sd_fx_media, FNAME_DATE, (UINT *)&attributes);
        Refresh_Wdg_mock ();
      /*  if (err_open == FX_NOT_FOUND)
        {
            err_open = (uint8_t) fx_file_create(&sd_fx_media, FNAME_DATE);
            err_open = (uint8_t) fx_file_open(&sd_fx_media, &version_file, FNAME_DATE, FX_OPEN_FOR_WRITE);
            tx_mutex_get(&usd_mutex, TX_WAIT_FOREVER);
            err_open = (uint8_t) fx_file_write (&version_file, &upgrade_date, sizeof (DEVICE_DATE_t));
            err_open = (uint8_t) fx_media_flush (&sd_fx_media);
            err_open = (uint8_t) fx_file_close (&version_file);
            err_open = (uint8_t) fx_file_attributes_set(&sd_fx_media, FNAME_DATE, FX_HIDDEN);
            tx_mutex_put(&usd_mutex);
        }*/

        // Register the update date, time and app version
       // err_open = (uint8_t) fx_file_attributes_read (&sd_fx_media, UPDATE_REG, (UINT *)&attributes);
        Refresh_Wdg_mock ();
       /* if (err_open == FX_NOT_FOUND)
        {
            err_open = (uint8_t) fx_file_create(&sd_fx_media, UPDATE_REG);
        }*/

        Fill_Update_Reg(update_buffer, &upgrade_date, &upgrade_time);

      /*  err_open = (uint8_t) fx_file_open(&sd_fx_media, &version_file, UPDATE_REG, FX_OPEN_FOR_WRITE);
        tx_mutex_get(&usd_mutex, TX_WAIT_FOREVER);
        err_open = (uint8_t) fx_file_write (&version_file, update_buffer, sizeof (update_buffer));
        err_open = (uint8_t) fx_media_flush (&sd_fx_media);
        err_open = (uint8_t) fx_file_close (&version_file);
        err_open = (uint8_t) fx_file_attributes_set(&sd_fx_media, UPDATE_REG, FX_HIDDEN);
        tx_mutex_put(&usd_mutex);
*/
        printf("Fill_Update_Reg\n");
        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_DEVICE_READY, TRUE);
        Pasatiempos_Listening_mock ();
        *paux = PON_TEST;
    }
}

/******************************************************************************
 ** Name:    R100_Check_Cover
 *****************************************************************************/
/**
 ** @brief   Check if cover has been opened during an extended period of time
 **
 ** @param   dev_error         current device error
 ** @param   force_warning     force the warning message
 **
 ** @return  none
 ******************************************************************************/
static void R100_Check_Cover_mock (void)
{
    uint32_t    err_open;           // SD media open result ...
    uint8_t     pin_state;          // Cover pin state
    bool_t      flag_send_sigfox = true;


    printf("R100_Check_Cover_mock\n");
    // check if cover is open
   // g_ioport.p_api->pinRead (KEY_COVER, &pin_state);
    if (!pin_state)
    {
        // cover is closed
        nv_data.open_cover_first = true;

        //Check device status
        if (nv_data.prev_cover_status)
        {

            // maintain the power-on switch active
            Rtc_Program_Wakeup (WAKEUP_POWERON);

            // Power on the external circuits, adding an extra time to stabilize the power supplies ...
          //  g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_HIGH);
           // tx_thread_sleep (OSTIME_20MSEC);
        Sleep(20);
            printf("Sleep:Check Cover\n");

            // Startup is nor RTC mode, so reconfigure clocks with PLL configuration
            bsp_clock_init_Reconfigure();
            Battery_i2C_Init_mock();

            if (Is_SAT_Error_mock (nv_data.error_code) == false ) Check_Device_Led_Status();

            Refresh_Wdg_mock ();
            // Load Device Settings from NFC (the device info is read to be available in configuration mode)
            Settings_Open_From_NFC_mock();
            Refresh_Wdg_mock ();

            // Load Device Info --> NFC data MUST be read before !!!
            Device_Init_mock (NULL);

            // resume task used for alerting ...
           // tx_thread_resume (&thread_comm);
/*

            if (Get_Device_Info()->enable_b.sigfox && nv_data.open_cover_tx_flag == 1)
            {
                // Startup is nor RTC mode, so reconfigure clocks with PLL configuration
                bsp_clock_init_Reconfigure();

                nv_data.open_cover_tx_flag = 0;

                // Send Sigfox Alert
                Comm_Sigfox_Open();
                Refresh_Wdg_mock ();

                // Generate and Send Test report using Sigfox
                Comm_Sigfox_Generate_Send_Alert_mock(MSG_ID_COVER_CLOSE_ALERT);
                Pasatiempos_mock (20);
            }
*/
            nv_data.prev_cover_status = false;
            R100_PowerOff_mock();
        }
        return;

    }
    else
    {
        nv_data.prev_cover_status = true;          // cover is openned
    }

    // do not blink the status LED (periodic blink) while the cover is openned
    nv_data.status_led_blink = LED_BLINK_OFF;

    if (nv_data.open_cover_first == true) flag_send_sigfox = false; // In this case do not send sigfox message

    // open cover detected
    if ((nv_data.open_cover_first == true)        ||
        (diff_time_seconds (pon_time, nv_data.open_cover) >= WARNING_PERIOD_SAT))
    {
        // Startup is nor RTC mode, so reconfigure clocks with PLL configuration
        bsp_clock_init_Reconfigure();

        // register the time for the next warning
        nv_data.open_cover = pon_time;

        nv_data.open_cover_first = false;

        sMon_Inc_OpenCover();

        // maintain the power-on switch active
        Rtc_Program_Wakeup (WAKEUP_POWERON);

        // Power on the external circuits, adding an extra time to stabilize the power supplies ...
       //g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_HIGH);
       // tx_thread_sleep (OSTIME_20MSEC);
        Sleep(20);
        Refresh_Wdg_mock ();
        // Load Device Settings from NFC (the device info is read to be available in configuration mode)
        Settings_Open_From_NFC_mock();
        Refresh_Wdg_mock ();

        // Load Device Info --> NFC data MUST be read before !!!
        Device_Init_mock (NULL);

        // resume task used for alerting ...
       // tx_thread_resume (&thread_audio);
       // tx_thread_resume (&thread_comm);

        // open the uSD
        /*err_open = fx_media_init0_open ();
        if ((err_open != FX_SUCCESS) && (err_open != FX_PTR_ERROR))
        {
            Lock_on_Panic ((uint8_t)err_open, 12);
        }
        tx_thread_sleep (OSTIME_100MSEC);*/

        // Load Audio resources from SD-Card and proceed to play the SAT message
        /*if (err_open = Load_Audio_Resources_mock(), err_open != eERR_NONE)
        {
            Lock_on_Panic_mock ((uint8_t)err_open, 13);
        }
        else
        {
           // tx_thread_sleep (OSTIME_100MSEC);
        	printf("Sleep\n");
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_OPEN_COVER, TRUE);
            Pasatiempos_Listening_mock ();
        }
*/
        Battery_i2C_Init_mock();

        R100_Check_SAT_mock(Get_NFC_Device_Info()->error_code,true);
        R100_Check_SAT_mock(nv_data.error_code,true);

  /*      if (Get_Device_Info()->enable_b.sigfox)
        {
            if((nv_data.open_cover_tx_flag == 0) && (flag_send_sigfox))
            {
                // Send Sigfox Alert
                Comm_Sigfox_Open();
                Refresh_Wdg_mock ();

                // Generate and Send Test report using Sigfox
                Comm_Sigfox_Generate_Send_Alert_mock(MSG_ID_COVER_OPEN_ALERT);
                Refresh_Wdg_mock ();

                // Advice to the user periodically -- ???
                // Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);
                Pasatiempos_mock (10);
                // Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);
                Pasatiempos_mock (10);
                //Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_DONE, TRUE);

                nv_data.open_cover_tx_flag = 1;

                // wait while playing all pending audio messages ...
                // Pasatiempos_Listening ();
            }
        }
*/
        // Bye-Bye
        R100_PowerOff_mock();
    }
}
/******************************************************************************
 ** Name:    OneWire_Wait_While_Busy
 *****************************************************************************/
/**
 ** @brief   wait until OneWire is free
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void OneWire_Wait_While_Busy (void)
{
    uint8_t  status;
    uint8_t  my_buffer[2];

    my_buffer[0] = OWC_SET_REG_POINTER;
    my_buffer[1] = OWC_REG_STATUS;

    I2C2_Write (i2C_ADD_OW_MASTER, my_buffer, 2);

    // be sure that the OneWire controller is free
    do {
        I2C2_ReadByte (i2C_ADD_OW_MASTER, &status);

        // check the SD bit (short detected)
        if (status & 0x04)
        {
            I2C2_WriteByte (i2C_ADD_OW_MASTER, OWC_1WIRE_RESET);
            I2C2_ReadByte (i2C_ADD_OW_MASTER, &status);
        }
        Refresh_Wdg_mock ();                             // refresh the watchdog timer
    } while (status & 0x01);
}

/******************************************************************************
 ** Name:    Electrodes_Presence
 *****************************************************************************/
/**
 ** @brief   try to detect the presence of electrodes
 **
 ** @param   none
 **
 ** @return  electrodes presence flag (true if are present)
 ******************************************************************************/
static bool_t Electrodes_Presence (void)
{
    uint8_t  status;
    printf("Electrodes_Presence_mock\n");

    // 1-Wire reset/presence-detect cycle (be careful --- the reset pulse needs at least 70 usecs)
    I2C2_WriteByte (i2C_ADD_OW_MASTER, OWC_1WIRE_RESET);


    I2C2_ReadByte  (i2C_ADD_OW_MASTER, &status);

    I2C2_ReadByte  (i2C_ADD_OW_MASTER, &status);
    I2C2_ReadByte  (i2C_ADD_OW_MASTER, &status);
    I2C2_ReadByte  (i2C_ADD_OW_MASTER, &status);

    // return the presence flag (ppd-bit)
    return ((status & 0x02) != 0);
}

/******************************************************************************
 ** Name:    OneWire_Write_Byte
 *****************************************************************************/
/**
 ** @brief   write a single byte in the OneWire device (locking function)
 **
 ** @param   command identifier
 **
 ** @return  none
 ******************************************************************************/
static void OneWire_Write_Byte (uint8_t cmd)
{
    uint8_t  my_buffer[4];

    // be sure that the OneWire controller is free
 //   OneWire_Wait_While_Busy();

    // write the command in 1-wire line
    my_buffer[0] = OWC_WRITE_BYTE;
    my_buffer[1] = cmd;
    I2C2_Write (i2C_ADD_OW_MASTER, my_buffer, 2);
}

/******************************************************************************
 ** Name:    OneWire_Read_Array
 *****************************************************************************/
/**
 ** @brief   write a single byte in the OneWire device (locking function)
 **
 ** @param   pArray     pointer to destination array
 ** @param   nBytes     number of bytes to read
 **
 ** @return  none
 ******************************************************************************/
static void OneWire_Read_Array (uint8_t *pArray, uint8_t nBytes)
{
    uint8_t  my_buffer[4];
    uint8_t  i;

    // read data register from DS2482
    my_buffer[0] = OWC_SET_REG_POINTER;
    my_buffer[1] = OWC_REG_READ_DATA;

    // be sure that the OneWire controller is free
 //   OneWire_Wait_While_Busy();

    for (i=0; i<nBytes; i++)
    {
        I2C2_WriteByte (i2C_ADD_OW_MASTER, OWC_READ_BYTE);
  //      OneWire_Wait_While_Busy();
        I2C2_Write (i2C_ADD_OW_MASTER, my_buffer, 2);
        I2C2_ReadByte (i2C_ADD_OW_MASTER, &pArray[i]);
    }
}

/******************************************************************************
 ** Name:    Electrodes_Write_Scratchpad
 *****************************************************************************/
/**
 ** @brief   Writes a single scratchpad into electrodes memory
 **
 ** @param   pData     pointer to data
 ** @param   nBytes    number of bytes to write (must be up to 8)
 ** @param   address   memory address to write in
 **
 ** @return  none
 ******************************************************************************/
static void Electrodes_Write_Scratchpad_mock (uint8_t *pData, uint8_t nBytes, uint8_t address)
{
    uint8_t  auth_pattern[4];
    uint8_t  i;

    //tx_thread_sleep (OSTIME_10MSEC);
    printf("Electrodes_Write_Scratchpad_mock\n");

    // Reset 1-wire device
    OneWire_Wait_While_Busy();
    I2C2_WriteByte (i2C_ADD_OW_MASTER, OWC_1WIRE_RESET);  //OWC_1WIRE_RESET   OWC_DEVICE_RESET

    OneWire_Write_Byte (OWC_SKIP_ROM);          // write the skip ROM command in 1-wire line
    OneWire_Write_Byte (OW_WRITE_SCRATCHPAD);   // write scratchpad command in 1-wire line
    OneWire_Write_Byte (address);               // write TA1 address
    OneWire_Write_Byte (0x00);                  // write TA2 address
    for (i=0; i<nBytes; i++)
    {
        OneWire_Write_Byte (pData[i]);          // Write the data to scratchpad
    }

    // Read CRC
//      OneWire_Read_Array (auth_pattern, 2);

    // Reset 1-wire device
    OneWire_Wait_While_Busy();
    I2C2_WriteByte (i2C_ADD_OW_MASTER, OWC_1WIRE_RESET);

    OneWire_Write_Byte (OWC_SKIP_ROM);          // SKIP ROM command
    OneWire_Write_Byte (OW_READ_SCRATCHPAD);    // Read scratchpad command

    // Read from data register the data written in the scratchpad
    OneWire_Read_Array (auth_pattern, 3);

    // Reset 1-wire device
    OneWire_Wait_While_Busy();
    I2C2_WriteByte (i2C_ADD_OW_MASTER, OWC_1WIRE_RESET);

    OneWire_Write_Byte (OWC_SKIP_ROM);          // SKIP ROM command
    OneWire_Write_Byte (OW_COPY_SCRATCHPAD);    // Copy scratchpad command

    // Authorization code match
    for (i=0; i<3; i++)
    {
        OneWire_Write_Byte (auth_pattern[i]);
    }

    // Wait "Tprog" to complete the write operation
    //tx_thread_sleep (OSTIME_20MSEC);
    printf("Sleep: Electrodes Write\n");
    OneWire_Wait_While_Busy();
    OneWire_Read_Array (auth_pattern, 1);       // must read 0xAA as a 01 sequence
    OneWire_Wait_While_Busy();
    I2C2_WriteByte (i2C_ADD_OW_MASTER, OWC_1WIRE_RESET);
}

/******************************************************************************
 ** Name:    Electrodes_Write_Event
 *****************************************************************************/
/**
 ** @brief   Writes event into electrodes memory
 **
 ** @param   pEvent    pointer to data
 **
 ** @return  none
 ******************************************************************************/
static void Electrodes_Write_Event (ELECTRODES_EVENT_t *pEvent)
{
    uint8_t *pData;

    if (Is_Mode_Trainer()) return;  // In trainer mode do not save events

    electrodes_presence_flag = Electrodes_Presence ();
    if (electrodes_presence_flag)
    {
        pData = (uint8_t *) pEvent;
        Electrodes_Write_Scratchpad_mock (&pData[0], 8, 0x20);       // write date & time
        Electrodes_Write_Scratchpad_mock (&pData[8], 8, 0x28);       // write event identifier and checksum
    }
}

/******************************************************************************
 ** Name:    Electrodes_Read_Data
 *****************************************************************************/
/**
 ** @brief   read the electrodes all data
 **
 ** @param   pEvent    pointer to data
 **
 ** @return  none
 ******************************************************************************/
static void Electrodes_Read_Data_mock (ELECTRODES_DATA_t *pData)
{
    //uint8_t     checksum;       // data structure checksum

	printf("Electrodes_Read_Data_mock\n");



    // updates the presence flag !!!
    electrodes_presence_flag = Electrodes_Presence();

    Refresh_Wdg_mock ();                             // refresh the watchdog timer
    // some premature function return ...
    if (!pData) { return; }
    if (!electrodes_presence_flag) { memset ((uint8_t*) pData, 0, sizeof (ELECTRODES_DATA_t)); return; }
    if (pData->sn) { return; }

    // read the serial number
    OneWire_Write_Byte (OWC_READ_ROM);                  // write the read ROM command in 1-wire line
    OneWire_Read_Array ((uint8_t *) &pData->sn, 8);     // read data register from DS2482
    Refresh_Wdg_mock();                             // refresh the watchdog timer

    // updates the presence flag !!!
    electrodes_presence_flag = Electrodes_Presence();
    if (electrodes_presence_flag)
    {
        // read the electrodes info
        OneWire_Write_Byte (OWC_SKIP_ROM);                  // write the skip ROM command in 1-wire line
        OneWire_Write_Byte (OW_READ_MEMORY);                // write the read command in 1-wire line
        OneWire_Write_Byte (0x00);                          // write TA1 address
        OneWire_Write_Byte (0x00);                          // write TA2 address
        OneWire_Read_Array ((uint8_t *) &pData->info, sizeof (ELECTRODES_INFO_t));
        Refresh_Wdg_mock ();                             // refresh the watchdog timer

        // to reset the one wire
//        OneWire_Wait_While_Busy();
        I2C2_WriteByte (i2C_ADD_OW_MASTER, OWC_1WIRE_RESET);
        Refresh_Wdg_mock ();                             // refresh the watchdog timer

        // read the event register
        OneWire_Write_Byte (OWC_SKIP_ROM);                  // write the skip ROM command in 1-wire line
        OneWire_Write_Byte (OW_READ_MEMORY);                // write the read command in 1-wire line
        OneWire_Write_Byte (0x20);                          // write TA1 address
        OneWire_Write_Byte (0x00);                          // write TA2 address
        OneWire_Read_Array ((uint8_t *) &pData->event, sizeof (ELECTRODES_EVENT_t));
        Refresh_Wdg_mock ();                             // refresh the watchdog timer

        // to reset the one wire
 //       OneWire_Wait_While_Busy();
        I2C2_WriteByte (i2C_ADD_OW_MASTER, OWC_1WIRE_RESET);
/*
        // check the data integrity ...
        checksum = Get_Checksum_Xor ((uint8_t *) &pData->info, sizeof(ELECTRODES_INFO_t)-1);

        if (checksum != pData->info.checksum_add)
        {
            memset ((uint8_t*) pData, 0, sizeof (ELECTRODES_DATA_t));
        }
*/
    }
    else {
        memset ((uint8_t*) pData, 0, sizeof (ELECTRODES_DATA_t));
    }
}

/******************************************************************************
 ** Name:    Electrodes_Get_Signature
 *****************************************************************************/
/**
 ** @brief   get electrodes signature
 **
 ** @param   none
 **
 ** @return  electrodes signature
 ******************************************************************************/
EL_SIGN_e Electrodes_Get_Signature (void)
{
    if (electrodes_presence_flag)
    {
        if (Is_Mode_Trainer())
        {
           return ((strcmp((char_t *) electrodes_data.info.name, "TRAINER") == 0)? eEL_SIGN_TRAINER: eEL_SIGN_MUST_TRAINER);
        }
        if (electrodes_data.info.expiration_date >= pon_date)
        {
            return eEL_SIGN_BEXEN;
        }
        else if (electrodes_data.info.expiration_date)
        {
            return eEL_SIGN_BEXEN_EXPIRED;
        }
        else
        {
            return eEL_SIGN_UNKNOWN;
        }
    }
    return ((electrodes_zp_segment == eZP_SEGMENT_OPEN_CIRC) ? eEL_SIGN_NONE : eEL_SIGN_UNKNOWN);
}

/******************************************************************************
 ** Name:    Electrodes_Get_Data
 *****************************************************************************/
/**
 ** @brief   get the electrodes data (sn, info and event register)
 **
 ** @param   pData  pointer to electrodes structure
 **
 ** @return  none
 ******************************************************************************/
void Electrodes_Get_Data (ELECTRODES_DATA_t* pData)
{
    if (pData) { memcpy ((uint8_t *) pData, (uint8_t *) &electrodes_data, sizeof(ELECTRODES_DATA_t)); }
}

/******************************************************************************
 ** Name:    Electrodes_Register_Event
 *****************************************************************************/
/**
 ** @brief   command to register electrodes event
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
void Electrodes_Register_Event (void)
{
    electrodes_pending_event = electrodes_presence_flag;
}

/******************************************************************************
 ** Name:    Electrodes_Register_Shock
 *****************************************************************************/
/**
 ** @brief   command to register shock
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
void Electrodes_Register_Shock (void)
{
    // In trainer mode do not send anything related to sigfox
    if (!Is_Mode_Trainer())Comm_Sigfox_Open(); // Open Sigfox Comm.


    // Send message shock performed
    Comm_Sigfox_Generate_Send_Alert_mock(MSG_ID_SHOCK_DONE_ALERT);
}

/******************************************************************************
** Name:    Execute_Test_Leds_Keys
*****************************************************************************/
/**
** @brief   Function that executes the electrode test
**
** @param   pElectrodes     pointer to result parameters
** @param   auto_test   if the test is automatic or forced by an user
**
** @return  none
**
******************************************************************************/
static ERROR_ID_e Execute_Test_Leds_Keys_mock (void)
{
    uint8_t     rx_msg[5];
    uint8_t     led_bit = 0;
    uint32_t    cnt = 0;
    uint32_t    key = 0;

    printf("Execute_Test_Leds_Keys_mock\n");

    // First check leds
    Led_Off_mock (LED_ONOFF);
    Led_Off_mock (LED_SHOCK);
    Led_Off_mock (LED_PATYPE);

    //tx_thread_sleep (OSTIME_100MSEC);
    printf("Sleep: Execute test leds\n");
    rx_msg[0] = 0;

    Refresh_Wdg_mock ();
    while (cnt < 30)
    {
        cnt++;
        Led_Off_mock (LED_ONOFF);
        Led_Off_mock (LED_SHOCK);
        Led_Off_mock (LED_PATYPE);
        //tx_thread_sleep (OSTIME_100MSEC);
        printf("Sleep: Execute test\n");

        Comm_Uart_Send_mock("TL0");
        Refresh_Wdg_mock ();
        if (Comm_Uart_Wifi_Receive_mock((char_t *) rx_msg, 1, 3) == eERR_NONE)
        {
            if (rx_msg[0] == 255)        // all leds OFF
            {
                led_bit |= 0x08;
                break;
            }
            Refresh_Wdg_mock ();
        }
    }
    if (cnt == 30) return eERR_LEDS;
    cnt = 0;
    Refresh_Wdg_mock ();
    while (cnt < 30)
    {
        cnt++;
        Led_On_mock (LED_ONOFF);
        Led_Off_mock (LED_SHOCK);
        Led_Off_mock (LED_PATYPE);
       // tx_thread_sleep (OSTIME_100MSEC);
        printf("Sleep: Execute Test\n");
        Comm_Uart_Send_mock("TL0");
        Refresh_Wdg_mock ();
        if (Comm_Uart_Wifi_Receive_mock((char_t *) rx_msg, 1, 3) == eERR_NONE)
        {
            if (rx_msg[0] == 0x7F)        // led ONOFF ON, all leds OFF
            {
                led_bit |= 0x01;
                Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "LED ONOFF OK");
                Comm_Uart_Send_mock("L0F");
                break;
            }
            Refresh_Wdg_mock ();
        }
    }

    if (cnt == 30) return eERR_LEDS;
    cnt = 0;
    Refresh_Wdg_mock ();
    while (cnt < 30)
    {
        cnt++;
        Led_Off_mock (LED_ONOFF);
        Led_On_mock (LED_SHOCK);
        Led_Off_mock (LED_PATYPE);
        //tx_thread_sleep (OSTIME_100MSEC);
        printf("Sleep: Execute Test\n");
        Comm_Uart_Send_mock("TL0");
        Refresh_Wdg_mock ();
        if (Comm_Uart_Wifi_Receive_mock((char_t *) rx_msg, 1, 3) == eERR_NONE)
        {
            if (rx_msg[0] == 191)        // led LED_SHOCK ON, all leds OFF
            {
                led_bit |= 0x02;
                Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "LED_SHOCK OK");
                Comm_Uart_Send_mock("LSH");
                break;
            }
            Refresh_Wdg_mock ();
        }

    }
    if (cnt == 30) return eERR_LEDS;
    cnt = 0;
    Refresh_Wdg_mock ();
    while (cnt < 30)
    {
        cnt++;
        Led_Off_mock (LED_ONOFF);
        Led_Off_mock (LED_SHOCK);
        Led_On_mock (LED_PATYPE);
        Comm_Uart_Send_mock("TL0");
        Refresh_Wdg_mock ();
        //tx_thread_sleep (OSTIME_100MSEC);
        printf("Sleep: Execute Test\n");
        if (Comm_Uart_Wifi_Receive_mock((char_t *) rx_msg, 1, 3) == eERR_NONE)
        {
            if (rx_msg[0] == 223)        // led LED_PATYPE ON, all leds OFF
            {
                led_bit |= 0x04;
                Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "LED_PATYPE OK");
                Comm_Uart_Send_mock("LPT");
                break;
            }
            Refresh_Wdg_mock ();
        }
    }
    Led_Off_mock (LED_ONOFF);
    Led_Off_mock (LED_SHOCK);
    Led_Off_mock (LED_PATYPE);
    if (cnt == 30) return eERR_LEDS;
/*
    // Second check keys
    Trace ((TRACE_NEWLINE),"");
    Trace_Arg (TRACE_NO_FLAGS, "  ** KEY_ONOFF = %7d", Key_Read(KEY_ONOFF));

    Trace ((TRACE_NEWLINE),"");
    Trace_Arg (TRACE_NO_FLAGS, "  ** KEY_SHOCK = %7d", Key_Read(KEY_SHOCK));

    Trace ((TRACE_NEWLINE),"");
    Trace_Arg (TRACE_NO_FLAGS, "  ** KEY_COVER = %7d", Key_Read(KEY_COVER));

    Trace ((TRACE_NEWLINE),"");
    Trace_Arg (TRACE_NO_FLAGS, "  ** KEY_PATYPE = %7d", Key_Read(KEY_PATYPE));
*/
    key = 1;
    for (cnt = 0; cnt < 50; cnt++)
    {
        Refresh_Wdg_mock ();
        switch (key)
        {
            case 1:
                Comm_Uart_Send_mock("TK1");
                //tx_thread_sleep (OSTIME_1SEC);
                printf("Sleep: Execute Test\n");
                if ((Key_Read_mock(2) == 1) &&
                    (Key_Read_mock(0) == 0) &&
//                    (Key_Read(KEY_COVER) == KEY_STATUS_OFF) &&
                    (Key_Read_mock(1) == 0))
                {
                    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "KEY_ONOFF OK");
                    Comm_Uart_Send_mock("K0F");
                    Refresh_Wdg_mock ();
                    key = 2;
                }
                break;
            case 2:
                Comm_Uart_Send_mock("TK2");
                //tx_thread_sleep (OSTIME_1SEC);
                printf("Sleep: Execute Test\n");

                if ((Key_Read_mock(2) == 0) &&
                    (Key_Read_mock(0) == 1) &&
//                    (Key_Read(KEY_COVER) == KEY_STATUS_OFF) &&
                    (Key_Read_mock(1) == 0))
                {
                    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "KEY_SHOCK OK");
                    Comm_Uart_Send_mock("KSH");
                    Refresh_Wdg_mock ();
                    key = 3;
                }
                break;
            case 3:
                Comm_Uart_Send_mock("TK3");
                //tx_thread_sleep (OSTIME_1SEC);
                printf("Sleep: Execute Test\n");

                if ((Key_Read_mock(2) == 0) &&
                    (Key_Read_mock(0) == 0) &&
//                    (Key_Read(KEY_COVER) == KEY_STATUS_OFF) &&
                    (Key_Read_mock(1) == 1))
                {
                    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "KEY_PATYPE OK");
                    Comm_Uart_Send_mock("KPT");
                    Refresh_Wdg_mock ();
                    key = 4;
                }
                break;
            case 4:
                Comm_Uart_Send_mock("TC0");
                //tx_thread_sleep (OSTIME_2SEC);
                printf("Sleep: Execute Test\n");

                if ((Key_Read_mock(2) == 0) &&
                    (Key_Read_mock(0) == 0) &&
                    (Key_Read_mock(3) == 1) &&
                    (Key_Read_mock(1) == 0))
                {
                    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "COVER OK");
                    Comm_Uart_Send_mock("KCV");
                    Refresh_Wdg_mock ();
                    return OK;
                }
                break;
            default:
                break;
        }
    }
    return eERR_KEYS;
}

/******************************************************************************
** Name:    Execute_Test_Electrodes
*****************************************************************************/
/**
** @brief   Function that executes the electrode test
**
** @param   pElectrodes     pointer to result parameters
** @param   auto_test       if the test is automatic or forced by an user
**
** @return  none
**
******************************************************************************/
static void Execute_Test_Electrodes_mock (TEST_ELECTRODES_t *pElectrodes, bool_t auto_test, bool_t pat_mode)
{
    uint32_t    my_zp_ohm;                  // patient impedance in electrodes in adcs

    printf("Execute_Test_Electrodes_mock\n");
    // get data from the 1-Wire bus connected to the electrodes
    Electrodes_Read_Data_mock (&electrodes_data);

    if (pElectrodes)
    {
        // copy the data
        pElectrodes->sn              = electrodes_data.sn;
        pElectrodes->expiration_date = electrodes_data.info.expiration_date;
        pElectrodes->event_date      = electrodes_data.event.date;
        pElectrodes->event_time      = electrodes_data.event.time;
        pElectrodes->event_id        = electrodes_data.event.id;
        pElectrodes->error_code      = 0;
    }

    if (Is_Mode_Trainer()) return;

    // if electrodes are connected ...
    if (electrodes_presence_flag)
    {
        my_zp_ohm = patMon_Get_Zp_Ohms_mock();
        // Check if there is a patient connected
        if ((my_zp_ohm < 300) && (R100_test_result.test_id != 50))
        {
            //Play a message to inform the user
            Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_DISCONNECT_PATIENT, TRUE);
        }

        // check expiration date to advice the user (audio message) !!!!
        if ((battery_info.must_be_0xAA == 0xAA) &&
            (battery_statistics.must_be_0x55 == 0x55) &&
            (electrodes_data.info.expiration_date <= pon_date))
        {
            if (pElectrodes) pElectrodes->error_code = eERR_ELECTRODE_EXPIRED;
            if (!auto_test && pat_mode) DB_Episode_Set_Event_mock(eREG_ECG_EL_EXPIRED);
            if (!auto_test)
            {
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_ELECTRODES, TRUE);
                first_time = false;
            }
        }
        else if (electrodes_data.event.id != 0)
        {
            if (pElectrodes) pElectrodes->error_code = eERR_ELECTRODE_USED;
            if (!auto_test)
            {
                DB_Episode_Set_Event_mock(eREG_ECG_EL_USED);
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_ELECTRODES, TRUE);
                first_time = false;
            }
        }
    }
    else {
        // must connect electrodes ...
        pElectrodes->error_code = eERR_ELECTRODE_NOT_CONNECTED;
        if (!auto_test && !pat_mode) Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_CONNECT_ELECTRODES_DEVICE, TRUE);
    }
}

/******************************************************************************
** Name:    Execute_Test_Voltage
*****************************************************************************/
/**
** @brief   Function that executes the voltage test
**
** @param   pVoltage     pointer to result parameters
**
** @return  none
**
******************************************************************************/
static void Execute_Test_Voltage_mock (TEST_VOLTAGE_t *pVoltage)
{
    // Power Supply Enable for the defibrillator functions (charge, discharge & shock)
   // g_ioport.p_api->pinWrite (DEF_PSU_EN, IOPORT_LEVEL_HIGH);
   // tx_thread_sleep (OSTIME_200MSEC);
	printf("Execute_Test_Voltage_mock\n");

    // check voltage values
    pVoltage->dc_main = Defib_Get_Vbatt();
    if (pVoltage->dc_main < SMON_DCMAIN_MIN) { pVoltage->dc_main_error = eERR_SMON_DCMAIN_TOO_LOW;  }
    if (pVoltage->dc_main > SMON_DCMAIN_MAX) { pVoltage->dc_main_error = eERR_SMON_DCMAIN_TOO_HIGH; }

    pVoltage->dc_18v = Defib_Get_Vdefib();
    if (pVoltage->dc_18v < SMON_18V_MIN) { pVoltage->dc_18v_error = eERR_SMON_18V_TOO_LOW;  }
    if (pVoltage->dc_18v > SMON_18V_MAX) { pVoltage->dc_18v_error = eERR_SMON_18V_TOO_HIGH; }
}

/******************************************************************************
** Name:    Execute_Test_Misc
*****************************************************************************/
/**
** @brief   Function that executes the miscellaneous test
**
** @param   pMisc       pointer to result parameters
** @param   pDevice_sn  device serial number
** @param   test_id     test number
**
** @return  none
**
******************************************************************************/
static void Execute_Test_Misc_mock (TEST_MISC_t *pMisc, char_t *pDevice_sn, uint8_t test_id)
{
    uint8_t         my_buf[] = {"L#"};  // Life-beat command for boot processor
    uint8_t         boot_msg;           // boot processor response char
    uint8_t         tx_data[256];

    printf("Execute_Test_Misc_mock\n");

    pMisc->nfc = eERR_NONE;
    // verify that the read info from NFC is OK
    if (!isalpha((uint8_t) Get_Device_Info()->sn[0]) &&
        !isdigit((uint8_t) Get_Device_Info()->sn[0]) &&
        !Is_Test_Mode_Montador()                     &&
        (Get_Device_Info()->develop_mode != DEVELOP_MANUFACTURE_CONTROL))
    {
        pMisc->nfc = eERR_NFC_INFO;
    }

    memcpy(pDevice_sn, Get_Device_Info()->sn, sizeof(Get_Device_Info()->sn));

    // Check NV_data size
    if (sizeof(NV_DATA_t) > FLASH_DATA_SZ) pMisc->nfc = eERR_NV_SZ;

    // send life-beat to boot processor (including NULL character)
    Refresh_Wdg_mock ();                             // refresh the watchdog timer
    boot_msg = Boot_Send_Message_mock(my_buf, 3);    // dummy transfer
    Refresh_Wdg_mock ();                             // refresh the watchdog timer
    boot_msg = Boot_Send_Message_mock(my_buf, 3);    // test with Life-Beat

    // report bootloader error
    pMisc->boot = (boot_msg == 'Y' ) ? eERR_NONE : eERR_BOOT_PROCESSOR;
    boot_msg = '0';

    // execute SPI memory test
    if ((pMisc->boot == eERR_NONE) &&
       ((test_id == TEST_MANUAL_ID) ||
        (test_id == 30)             ||
        (Is_Test_Mode_Montador())))
    {
        Refresh_Wdg_mock ();                             // refresh the watchdog timer

        memset (tx_data, 0, sizeof(tx_data));
        tx_data[0] = 'X';
        tx_data[1] = '#';
        tx_data[2] = (uint8_t) (1 >> 8);
        tx_data[3] = (uint8_t) (1);
        tx_data[4] = (uint8_t) (1 >> 8);
        tx_data[5] = (uint8_t) (1);
        memset(&tx_data[6], 0xAA, 128);     //Read 128 bytes

        boot_msg = Boot_Send_Message_mock(tx_data, (6 + 128 + 1));

        Pasatiempos_mock(2);                  // Wait

        pMisc->boot = (boot_msg == 'N' ) ? eERR_NONE : eERR_BOOT_SPI_MEMORY;

        Refresh_Wdg_mock ();                             // refresh the watchdog timer

        // check if updates erros are saved
        tx_data[0] = 'V';
        tx_data[1] = '#';
        tx_data[2] = 0;
//        boot_msg = '0';

        boot_msg = Boot_Send_Message_mock(tx_data, 3);
        Trace_Arg_mock (TRACE_NEWLINE, "PIC ack : %1c ", boot_msg);
        Refresh_Wdg_mock ();

        if ((boot_msg != 'T') && (boot_msg != 0))
        {
            pMisc->boot = eERR_BOOT_PROCESSOR_UPDATE;
        }
    }


    // execute and register the audio test
    pMisc->audio = Audio_Test_mock (eAUDIO_TONE, ((test_id == TEST_MANUAL_ID) || (test_id == 30) || Is_Test_Mode_Montador()));

    Refresh_Wdg_mock ();                             // refresh the watchdog timer
}

/******************************************************************************
** Name:    Execute_ECG_Signal
*****************************************************************************/
/**
** @brief   Function that executes the ECG test
**
** @param   none
**
** @return  none
**
******************************************************************************/
static bool Execute_ECG_Signal_mock (void)
{
#define N_SAMPLES   500                // 480*1000/150 = ECG_WIN_SIZE_DRD*100/150
//#define N_SAMPLES   1000                // 480*1000/150 = ECG_WIN_SIZE_DRD*100/150
//#define N_SAMPLES   3200                // 480*1000/150 = ECG_WIN_SIZE_DRD*100/150
    uint32_t firstSample;               // first sample in the ECG series
    uint32_t i;                         // global counter
    int16_t  ecg_raw[N_SAMPLES];             // Raw ecg, as captured
    // verify the ECG from RCAL (must be an asistole)
    // wait to stabilize the signal ...

    // Resume threads needed for calibration
    //tx_thread_resume (&thread_audio);
    //tx_thread_resume (&thread_patMon);

    // Connect ECG sense to ZP_CAL
    //g_ioport.p_api->pinWrite (PAT_CAL, IOPORT_LEVEL_HIGH);

    printf("Execute_ECG_Signal_mock\n");
    // wait for patMon to stabilize zp values
    Pasatiempos_mock (2);

    // refresh the internal watchdog timer
    firstSample = patMon_Get_ECG_nSample();     // initialize the first sample identifier

    // wait for patMon to read samples
    Pasatiempos_mock (3);

    patMon_Get_ECG_Window (firstSample,N_SAMPLES, ecg_raw);

    // verify that all ECG samples (last 100 msec.) are in a window near the baseline
    for (i=0; i<(N_SAMPLES-1); i++)
    {
        Refresh_Wdg_mock ();
        // allow a xxxuV dispersion
        if (abs (ecg_raw[i] - ecg_raw[i+1]) > (15))
        {
Trace_Arg_mock (TRACE_NO_FLAGS, "  ** ecg_raw = %7d", (uint32_t) abs (ecg_raw[i] - ecg_raw[i+1]));
            return false;
        }
    }

    //Check external simulated signal
    // Connect ECG sense to external patient
    //g_ioport.p_api->pinWrite (PAT_CAL, IOPORT_LEVEL_LOW);
/*
    // wait for patMon to stabilize zp values
    Pasatiempos (OSTIME_2SEC);

while (1)
{
    firstSample = patMon_Get_ECG_nSample();     // initialize the first sample identifier

    // wait for patMon to read samples
    Pasatiempos (OSTIME_7SEC);

    patMon_Get_ECG_Window (firstSample,N_SAMPLES, ecg_raw);
}
    // verify that ECG samples becomes from simulator
    for (i=0; i<N_SAMPLES; i++)
    {
        // allow a 320uV dispersion
        if (abs (ecg_raw[i] - 100) > (60))
        {
            return false;
        }
    }
    */
    return true;
}

/******************************************************************************
** Name:    Execute_calibration
*****************************************************************************/
/**
** @brief   Function that executes the impedance calibration of the device
**
** @param   none
**
** @return  none
**
******************************************************************************/
static void Execute_calibration_mock (void)
{
    uint32_t    my_zp_ohms;                 // patient impedance in ohms
    uint32_t    my_zp_adcs;                 // patient impedance in adcs
    uint32_t    my_zp_cal;                  // patient impedance calibration in adcs
    char_t      my_string[32];              // local string

    printf("Execute_calibration_mock\n");

    // Resume threads needed for calibration
    //tx_thread_resume (&thread_audio);
   // tx_thread_resume (&thread_patMon);

    // wait for patMon to stabilize zp values
    Pasatiempos_mock (5);

    if (Electrodes_Presence())
    {
        // read calibration values
        my_zp_ohms = patMon_Get_Zp_Ohms_mock();
        my_zp_adcs = patMon_Get_Zp_ADCs();
        my_zp_cal  = patMon_Get_zp_CAL_ADCs();
        nv_data.error_code = eERR_NONE;

        // trace some variables ...
        Trace_Arg_mock (TRACE_NO_FLAGS, "  ** Zp = %7d", my_zp_adcs);
        Trace_Arg_mock (TRACE_NO_FLAGS, " # %4d", my_zp_ohms);
        Trace_Arg_mock (TRACE_NO_FLAGS, " #  Zp_cal = %7d", my_zp_cal);
        sprintf (my_string, " #  Tpcb = %2d", (int) patMon_Get_Temperature());
        Trace_mock (TRACE_NO_FLAGS, my_string);
        Trace_mock (TRACE_NEWLINE, " ");

        //Check zp values
        if ((my_zp_adcs < ADS_ZP_0_MIN) ||
            (my_zp_adcs > ADS_ZP_0_MAX) ||
            (my_zp_cal < ADS_CAL_MIN) ||
            (my_zp_cal > ADS_CAL_MAX))
        {
            nv_data.error_code = eERR_ADS_CALIBRATION;
            if (Is_Test_Mode_Montador() == false) Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
            return;
        }

        // Save the values into NFC info
        NFC_Write_Device_ID_mock(true,true);

        // Check if the data is written correctly
        Settings_Open_From_NFC_mock();                   // Read again NFC values

        if (memcmp (&read_nfc_device_id, &write_nfc_device_id, sizeof(NFC_DEVICE_ID_t)) != 0)
        {
            nv_data.error_code = eERR_NFC_INFO;
            if (Is_Test_Mode_Montador() == false) Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        }
        else if (Is_Test_Mode_Montador() == FALSE)
        {
            // play message
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_DEVICE_READY, TRUE);
        }

        // Save new calibraion values to continue with the test_montador
       if (Is_Test_Mode_Montador())Device_Init_mock(NULL);

    }
    else
    {
        /*if (Is_Test_Mode_Montador() == false)*/ Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CONNECT_ELECTRODES_DEVICE, TRUE);
    }
}

/******************************************************************************
** Name:    Execute_Test_Patmon
*****************************************************************************/
/**
** @brief   Function that executes the Patient Monitor test
**
** @param   pPatmon     pointer to result parameters
**
** @return  none
**
******************************************************************************/
static bool_t Execute_Test_Patmon_mock (TEST_PATMON_t *pPatmon, uint8_t test_id)
{
    uint32_t    my_zp_cal;                  // patient impedance calibration in adcs
    uint32_t    my_zp_ohm;                  // patient impedance in electrodes in adcs

    printf("Execute_Test_Patmon_mock\n");
    Pasatiempos_mock (1);          // wait to stabilize ZP measurement

    if (Is_Test_Mode_Montador())        // First calibrate the device
    {
        Comm_Uart_Send_mock("ZP1");
        Pasatiempos_mock (1);
        Execute_calibration_mock();
        Refresh_Wdg_mock ();
        NFC_Write_Device_ID_mock(true, true);      // Configure develop_mode to production mode
        Comm_Uart_Send_mock("ZP3");
        Pasatiempos_mock (1);
    }

    my_zp_ohm = patMon_Get_Zp_Ohms_mock();
    my_zp_cal = Get_Device_Info()->zp_adcs_calibration;

    // default error
    pPatmon->ADS_pat = eERR_NONE;

    if (nv_data.error_code == eERR_ADS_CALIBRATION)
    {
        pPatmon->ADS_pat = eERR_ADS_PAT_CONNECTED;
    }

    // Check if there is a patient connected
    if ((my_zp_ohm < 300) &&  (test_id != 50) && !Is_Test_Mode_Montador())
    {
        pPatmon->ADS_pat = eERR_ADS_PAT_CONNECTED;
        //Play a message to inform the user
        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_DISCONNECT_PATIENT, TRUE);
    }

    // Check if calibration impedance is in boundary
    if ((my_zp_cal > ADS_CAL_MAX) || (my_zp_cal < ADS_CAL_MIN))
    {
        pPatmon->ADS_cal = eERR_ADS_CALIBRATION;
    }
    else {
        pPatmon->ADS_cal = eERR_NONE;
    }

    pPatmon->ADS_comms = patMon_Get_Status ();
    pPatmon->ADS_temperature = patMon_Get_Temperature();
/*In Bizintek fixture ECG has a lot of noise
    if (Is_Test_Mode_Montador() )     // Check ECG signal
    {
        return (Execute_ECG_Signal ());
    }
*/
    return true;
}

/******************************************************************************
** Name:    Execute_Test_Comms
*****************************************************************************/
/**
** @brief   Function that executes the Comms test
**
** @param   pComms     pointer to result parameters
**
** @return  none
**
******************************************************************************/
static void Execute_Test_Comms_mock (TEST_COMMS_t *pComms)
{
    bool_t          my_error = eERR_NONE;   // check the presence of the Comms modules
    DEVICE_INFO_t   *my_pDevice_Info;          // pointer to the device info
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;

    printf("Execute_Test_Comms_mock\n");
    Refresh_Wdg_mock ();                         // refresh the watchdog timer

    // get the device info
    my_pDevice_Info = Get_Device_Info();

    // check the communication with the accelerometer (connected in the i2C-2 bus)
    if (my_pDevice_Info->enable_b.accelerometer)
    {
        //    my_error = Comms_Check_Accelerometer ();
        if (my_error) pComms->accelerometer = eERR_COMM_ACCELEROMETER;
    }
    else {
        pComms->accelerometer = eERR_OPTION_DISABLED;
    }

    // check the communication with the Wifi module (connected in the SCI-2 port)
    if (my_pDevice_Info->enable_b.wifi)
    {

        Comm_Wifi_Execute_HW_Test();

        // 15 seconds timeout for sigfox test
        tout = 15;
        //ref_timeout = tx_time_get() + tout;
        ref_timeout = tout;
        Refresh_Wdg_mock ();

       // while((tx_time_get() <= ref_timeout) && (Comm_Is_Wifi_HW_Test_Finished() == FALSE))
        while( (Comm_Is_Wifi_HW_Test_Finished() == FALSE))
        {
            Refresh_Wdg_mock ();
            //tx_thread_sleep (OSTIME_50MSEC);
            printf("Sleep: Execute Test Comms\n");
        }

        if (Comm_Get_Wifi_HW_Test_Result() != eERR_NONE) pComms->wifi = eERR_COMM_WIFI_INIT;
    }
    else {
        pComms->wifi = eERR_OPTION_DISABLED;
    }

    // check the communication with the Sigfox module (connected in the SCI-2 port)
    if (my_pDevice_Info->enable_b.sigfox)
    {
        Comm_Sigfox_Execute_HW_Test();

        // 15 seconds timeout for sigfox test
        tout = 15;
        //ref_timeout = tx_time_get() + tout;
        Refresh_Wdg_mock ();

        while( (Comm_Is_Sigfox_HW_Test_Finished() == FALSE))
        {
            Refresh_Wdg_mock ();
            //tx_thread_sleep (OSTIME_50MSEC);
            printf("Sleep: Execute Test Comms\n");
        }

        if (Comm_Get_Sigfox_HW_Test_Result() != eERR_NONE) pComms->sigfox = eERR_COMM_SIGFOX;
    }
    else {
        pComms->sigfox = eERR_OPTION_DISABLED;
    }

    // check the communication with the GPS module (connected in the SCI-2 port)
    if (my_pDevice_Info->enable_b.gps)
    {
        Comm_GPS_Execute_HW_Test();

        // 15 seconds timeout for gps test
        tout = 15;
        //ref_timeout = tx_time_get() + tout;
        Refresh_Wdg_mock ();

        while((Comm_Is_GPS_HW_Test_Finished() == FALSE))
        {
            Refresh_Wdg_mock ();
           //tx_thread_sleep (OSTIME_50MSEC);
            printf("Sleep Test 50 msec\n");
        }

        if (Comm_Get_GPS_HW_Test_Result() != eERR_NONE) pComms->gps = eERR_COMM_GPS;
    }
    else {
        pComms->gps = eERR_OPTION_DISABLED;
    }
}

/******************************************************************************
** Name:    Execute_Test_GPS
*****************************************************************************/
/**
** @brief   Function that executes the GPS test
**
** @param   pGPS_String     pointer to result string
**
** @return  none
**
******************************************************************************/
static void Execute_Test_GPS_mock (char *pGPS_String)
{
	printf("Execute_Test_GPS_mock\n");
    Refresh_Wdg_mock ();                             // refresh the watchdog timer
    strcpy (pGPS_String, "The GPS position must be represented here .......");
}

/******************************************************************************
** Name:    Execute_Test_Defib
*****************************************************************************/
/**
** @brief   Function that executes the Defibrillator test
**
** @param   pDefib     pointer to result parameters
**
** @return  none
**
******************************************************************************/
static void Execute_Test_Defib_mock (TEST_DEFIB_t *pDefib)
{
#define TEST_CURRENT        4000    // current used to charge the main capacitor during test

    DEFIB_STATE_e   my_state;       // defibrillator state
    uint32_t        my_time;        // time in milliseconds
    uint32_t        time1, time2;   // tick marks

    printf("Execute_Test_Defib_mock\n");
    ////////////////////////////////////////////////////////////////////
    // Step 1 ---> Charge the main capacitor to the precharge value
    Refresh_Wdg_mock ();                                     // refresh the watchdog timer
   // time1 = tx_time_get();

    //If the battery voltage is smaller than the limit, do not run the test and assign the replace battery error.
    //Battery_Read_Temperature(&battery_temperature);

    if((Defib_Get_Vbatt() < Check_Battery_Voltage (Defib_Get_Vbatt(), battery_temperature, NULL)))
    {
        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_BATTERY, TRUE);
        pDefib->error_code = eERR_BATTERY_REPLACE;
        return;
    }

    Defib_Charge_mock(PRE_CHARGE_VOLTAGE, TEST_CURRENT);     // soft charge

    sMon_Inc_FullCharges ();    // increment the number of full charges !!!
    while (1)
    {
        Refresh_Wdg_mock ();                                 // refresh the watchdog timer
        my_state = Defib_Get_State ();
       // time2 = tx_time_get();
        my_time = (time2-time1) * 50;
        if (my_state == eDEFIB_STATE_CHARGED)
        {
            pDefib->full_charging_voltage = Defib_Get_Vc();     // voltage of main capacitor
            pDefib->full_charging_time = (uint16_t) my_time;    // step time in milliseconds
            if (my_time < 1000)
            {
                Defib_Charge_mock(0, 0);
                pDefib->error_code = eERR_DEF_CHARGING_FAST;
                Refresh_Wdg_mock ();                                     // refresh the watchdog timer
                return;
            }
            Refresh_Wdg_mock ();
            break;

        }
        // check the charging error (slow charge) and the charging time --> must be at least 1 second !!!
        if (my_state == eDEFIB_STATE_IN_ERROR)
        {
            Defib_Charge_mock(0, 0);
            pDefib->error_code = eERR_DEF_CHARGING_SLOW;
            Refresh_Wdg_mock ();                                     // refresh the watchdog timer
            return;
        }
    }

    ////////////////////////////////////////////////////////////////////
    // Step 2 ---> DisCharge through the H-bridge
    Refresh_Wdg_mock ();                                     // refresh the watchdog timer

    //time1 = tx_time_get();
    Defib_Charge_mock(380, TEST_CURRENT);

    // check the current sense circuit
    // WARNING !!! ignore this test because the voltage level to measure is too low
    // Rsense  = 40 mohms
    // current = 46 mA
    // voltage in the ADC input = 1,8 mV
/*  tx_thread_sleep (OSTIME_50MSEC);
    pDefib->full_discharg_H_current = Defib_Get_IH();
    if (pDefib->full_discharg_H_current < 10)
    {
        Defib_Charge(0, 0);
        pDefib->error_code = eERR_DEF_CURRENT_SENSE;
        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_CALL_SAT, TRUE);
        return;
    } */

    while (1)
    {
        Refresh_Wdg_mock ();                                     // refresh the watchdog timer
        my_state = Defib_Get_State ();
        //time2 = tx_time_get();
        my_time = (time2-time1) * 50;
        if (my_state == eDEFIB_STATE_CHARGED)
        {
            pDefib->full_discharg_H_voltage = Defib_Get_Vc();   // voltage of main capacitor
            pDefib->full_discharg_H_time = (uint16_t) my_time;  // step time in milliseconds
            break;
        }

        // check the discharging error (can not discharge)
        if (my_time > 5000)
        {
            Defib_Charge_mock(0, 0);
            pDefib->error_code = eERR_DEF_DISCHARGING_H;
            Refresh_Wdg_mock ();                                     // refresh the watchdog timer
            return;
        }
    }

    ////////////////////////////////////////////////////////////////////
    // Step 3 ---> DisCharge through the relay
    Refresh_Wdg_mock ();                                     // refresh the watchdog timer
   // time1 = tx_time_get();
    Defib_Charge_mock(0, 0);
    Pasatiempos_mock (1);
 //   time2 = tx_time_get();
    my_time = (time2-time1) * 50;

    pDefib->full_discharg_R_voltage = Defib_Get_Vc();       // voltage of main capacitor
    pDefib->full_discharg_R_time = (uint16_t) my_time;                 // step time in milliseconds

    // verify that at least 50 V are discharged through the relay in the monitored time ...
    if ((pDefib->full_discharg_H_voltage < pDefib->full_discharg_R_voltage) ||
       ((pDefib->full_discharg_H_voltage - pDefib->full_discharg_R_voltage) < 50))
    {
        pDefib->error_code = eERR_DEF_DISCHARGING_R;
    }

    ////////////////////////////////////////////////////////////////////
    // Step 4 ---> continue discharging to zero
    Refresh_Wdg_mock ();                                     // refresh the watchdog timer
}


/******************************************************************************
** Name:    Execute_Test_Battery
*****************************************************************************/
/**
** @brief   Function that executes the battery test
**
** @param   pBattery     pointer to result parameters
** @param   auto_test   if the test is automatic or forced by an user
** @param   pat_mode    if the test is done in Patient mode
**
** @return  none
**
******************************************************************************/
static void Execute_Test_Battery_mock (TEST_BATTERY_t *pBattery, bool_t auto_test, bool_t pat_mode)
{

	printf("Execute_Test_Battery_mock\n");
    // fill the battery info !!!
    if ((battery_info.must_be_0xAA == 0xAA) &&
        ((memcmp (battery_info.name, "BEXENCARDIO", sizeof("BEXENCARDIO"))==0) ||
         (memcmp (battery_info.name, "BexenCardio", sizeof("BexenCardio"))==0)) &&
        (battery_info.nominal_capacity == 4200))
    {
        memcpy (pBattery->sn,   battery_info.sn,   sizeof(battery_info.sn  ));  // company name -- (default: BexenCardio)
        memcpy (pBattery->name, battery_info.name, sizeof(battery_info.name));  // company name -- (default: BexenCardio)
        pBattery->manufacture_date = battery_info.manufacture_date;             // Manufacture date (YYYY.MM.DD)
        pBattery->expiration_date  = battery_info.expiration_date;              // Expiration  date (YYYY.MM.DD)
        pBattery->nominal_capacity = battery_info.nominal_capacity;             // Battery nominal capacity (default: 4200 mAh)
        pBattery->version_major    = battery_info.version;                      // Battery version major
        pBattery->version_minor    = battery_info.revision_code;                // Battery version minor

        if (pBattery->expiration_date < pon_date)
        {
            pBattery->error_code = eERR_BATTERY_EXPIRED;
            Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_BATTERY, TRUE);
            Pasatiempos_Listening_mock ();
        }
    }
    else {
        strcpy ((char_t *) pBattery->name, "Unknown");                                     // company name -- (Unknown)
        pBattery->error_code = eERR_BATTERY_UNKNOWN;
    }

    // fill the battery statistics !!!
    if (battery_statistics.must_be_0x55 == 0x55)
    {
        pBattery->runTime_total   = battery_statistics.runTime_total;           // accumulated run time (in minutes)
//        pBattery->runTime_patient = battery_statistics.runTime_patient;         // accumulated run time with patient connected (in minutes)
        pBattery->nFull_charges   = battery_statistics.nFull_charges;           // number of full charges
        pBattery->rem_charge      = Battery_Get_Charge_mock ();                      // compute the battery charge ...

        if (pBattery->rem_charge <= BATTERY_LOW_CHARGE)
        {
            pBattery->error_code = eERR_BATTERY_LOW;
        }
    }
    battery_charge = pBattery->rem_charge;

    pBattery->bat_temperature = (int16_t) battery_temperature;

    if (!auto_test)
    {
        // If the battery is not identified ....
        if ((battery_info.must_be_0xAA != 0xAA) ||
            (battery_statistics.must_be_0x55 != 0x55) ||
            pBattery->error_code == eERR_BATTERY_UNKNOWN)
        {
            if (!pat_mode)
            {
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_BATTERY, TRUE);
                Pasatiempos_Listening_mock ();
            }
            pBattery->error_code = eERR_BATTERY_UNKNOWN;
            DB_Episode_Set_Event_mock(eREG_BAT_UNKNOWN);
        }
        // check battery charge to advice the user (audio message) !!!!
        else
        {
            //Battery_Read_Temperature(&battery_temperature);
            if (battery_charge <= BATTERY_REPLACE_CHARGE || Defib_Get_Vbatt() <= Check_Battery_Voltage (Defib_Get_Vbatt(), battery_temperature, NULL))
            {

                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_BATTERY, TRUE);
                pBattery->error_code = eERR_BATTERY_REPLACE;
                DB_Episode_Set_Event_mock(eREG_BAT_VERY_LOW_BATTERY);
                Pasatiempos_Listening_mock ();
            }
            else if (battery_charge <= BATTERY_LOW_CHARGE)
            {
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_LOW_BATTERY, TRUE);
                pBattery->error_code = eERR_BATTERY_LOW;
                DB_Episode_Set_Event_mock(eREG_BAT_LOW_BATTERY);
                Pasatiempos_Listening_mock ();
            }
            if (pBattery->bat_temperature < BATTERY_LOW_TEMP)
            {
                pBattery->error_code = eERR_BATTERY_TEMP_OUT_RANGE;
                DB_Episode_Set_Event_mock(eREG_BAT_LOW_TEMPERATURE);
            }
            else if (pBattery->bat_temperature > BATTERY_HIGH_TEMP)
            {
                pBattery->error_code = eERR_BATTERY_TEMP_OUT_RANGE;
                DB_Episode_Set_Event_mock(eREG_BAT_HIGH_TEMPERATURE);
            }
        }
    }
}

/******************************************************************************
 ** Name:    Load_GOLD_File
 *****************************************************************************/
/**
 * @brief   This function use to upload Golden file.
 * @param   void
 * 
 * @return true Golden firmware uploaded
 * @return false No update found in uSD.
 *
 ******************************************************************************/
static bool_t Load_GOLD_File_mock(void)
{
    uint8_t  fx_res;
    uint8_t  host_ack;
    uint32_t nBytes;
    uint32_t frameId, nFrame;
    static FILE  *update_file;
    static uint8_t  tx_data[256];
    static uint32_t n_errors;

    printf("Load_GOLD_File_mock\n");

    memset (tx_data, 0, sizeof(tx_data));
    tx_data[0] = 'G';
    tx_data[1] = '#';

    // Load and read configuration file on SD
    //fx_res = (uint8_t) fx_file_open(&sd_fx_media, &update_file, GOLDEN_FILENAME, FX_OPEN_FOR_READ);
    update_file = fopen(GOLDEN_FILENAME,"r");

    if (fx_res == 0)
    {
        Trace_mock (TRACE_NEWLINE, "");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "GOLDEN FILE AVAILABLE !!!");
        Trace_mock (                   TRACE_NEWLINE, "=============================");

        // warn user about incoming firmware update
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_UPGRADING, TRUE);

        Refresh_Wdg_mock ();                             // refresh the watchdog timer
        Boot_Sync_mock (3);                              // Synchronize the Boot processor
        Refresh_Wdg_mock ();                             // refresh the watchdog timer

        // Check file length
        nFrame = update_file->_bufsiz;//(uint32_t) ((update_file.fx_file_current_file_size) / 128 );
      /*  if ((update_file.fx_file_current_file_size) % 128 )
        {
            nFrame++;
        }*/
        for (frameId=1; frameId<=nFrame; frameId++)
        {
            n_errors = 0;
            //tx_thread_sleep (OSTIME_10MSEC);
            printf("Sleep\n");
//            Trace_Arg (TRACE_NEWLINE, "frameId:%4d ", frameId);
            tx_data[2] = (uint8_t) (frameId >> 8);
            tx_data[3] = (uint8_t) (frameId);
            tx_data[4] = (uint8_t) (nFrame >> 8);
            tx_data[5] = (uint8_t) (nFrame);
            fx_res = fread(&tx_data[6],128,1,update_file);//(uint8_t) fx_file_read(&update_file, &tx_data[6], 128, &nBytes);     //Read 128 bytes

            Refresh_Wdg_mock ();                             // refresh the watchdog timer

            if ((frameId%1500) == 0)     //Advice to the user periodically
            {
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_UPGRADING, TRUE);  // warn about incoming update
            }

            do {
                //tx_thread_sleep ((uint16_t)OSTIME_10MSEC);
                printf("Sleep\n");
                host_ack = Boot_Send_Message_mock(tx_data, (6 + 128 + 1));
                Refresh_Wdg_mock ();
                if(host_ack != 'N')
                {
                    n_errors++;
                    if (n_errors > 5)
                    {
                        R100_test_result.misc.boot = eERR_BOOT_PROCESSOR_UPDATE;
                        return FALSE;
                    }
                }
            } while (host_ack != 'N');
        }
        // Wait a little time
        Pasatiempos_mock (1000);
       // fx_res = (uint8_t) fx_file_close (&update_file);
		Pasatiempos_mock (100);
      //  fx_res = (uint8_t) fx_file_delete (&sd_fx_media, GOLDEN_FILENAME);
      //  fx_res = (uint8_t) fx_media_flush (&sd_fx_media);

        // Wait a little time to save gold programm into SPI memory
        Pasatiempos_mock (1000);

        return TRUE; // golden fw uploaded
    }
    return FALSE; // no update found in uSD
}

/******************************************************************************
 ** Name:    Load_FW_File
 *****************************************************************************/
/**
 * @brief   This function use to upload firmware file.
 * @param   void
 * 
 * @return true Firmware is upload but update is pending
 * @return false No update found in uSD.
 *
 ******************************************************************************/
static bool_t Load_FW_File_mock(void)
{
    uint8_t  fx_res;
    uint8_t  host_ack;
    uint32_t nBytes, attributes;
    uint32_t frameId, nFrame;
    uint32_t my_app_ver = APP_REV_CODE;
    static FILE  *update_file, *version_file;
    static uint8_t  tx_data[256];
    static uint32_t n_errors;

    printf("Load_FW_File_mock\n");

    Refresh_Wdg_mock ();
    memset (tx_data, 0, sizeof(tx_data));
    tx_data[0] = 'F';
    tx_data[1] = '#';

    // Load and read configuration file on SD
    //fx_res = (uint8_t) fx_file_open(&sd_fx_media, &update_file, UPGRADE_FILENAME, FX_OPEN_FOR_READ);
   // update_file =fopen(UPGRADE_FILENAME,"r");
    R100_test_result.misc.boot = 0;     // reset error

    Refresh_Wdg_mock ();
    if (fx_res == 0)
    {
        Trace_mock (TRACE_NEWLINE, "");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "UPDATE AVAILABLE !!!");
        Trace_mock (                   TRACE_NEWLINE, "=============================");

   //     fx_res = (uint8_t) fx_file_attributes_read (&sd_fx_media, FNAME_VERSION, (UINT *)&attributes);
        Refresh_Wdg_mock ();
     //   if ((fx_res == FX_SUCCESS) || (attributes & FX_ARCHIVE))
     //   {
     //       fx_res = (uint8_t) fx_file_delete(&sd_fx_media, FNAME_VERSION);
     //       fx_res = (uint8_t) fx_media_flush (&sd_fx_media);
     //   }

        // record current firmware version
      //  fx_res = (uint8_t) fx_file_create (&sd_fx_media, FNAME_VERSION);
       /* fx_res = (uint8_t) fx_file_open (&sd_fx_media, &version_file, FNAME_VERSION, FX_OPEN_FOR_WRITE);
        fx_res = (uint8_t) fx_file_write (&version_file, (uint8_t *) &my_app_ver, sizeof(uint32_t));
        fx_res = (uint8_t) fx_media_flush (&sd_fx_media);
        fx_res = (uint8_t) fx_file_close (&version_file);

        // Hidden file
        fx_res = (uint8_t) fx_file_attributes_set(&sd_fx_media, FNAME_VERSION, FX_HIDDEN);
*/
        version_file =fopen(FNAME_VERSION,"w");
        fx_res = fwrite(&my_app_ver,sizeof(uint32_t),1,version_file);
        fclose(version_file);
        // warn user about incoming firmware update
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_UPGRADING, TRUE);

        Refresh_Wdg_mock ();                             // refresh the watchdog timer
        Boot_Sync_mock (3);                              // Synchronize the Boot processor
        Refresh_Wdg_mock ();                             // refresh the watchdog timer

        // Check file length
        nFrame = (uint32_t) ((update_file->_bufsiz) / 128 );
        if ((update_file->_bufsiz) % 128 )
        {
            nFrame++;
        }
        for (frameId=1; frameId<=nFrame; frameId++)
        {
            n_errors = 0;                       // reset number of errors
           // tx_thread_sleep (OSTIME_10MSEC);
//            Trace_Arg (TRACE_NEWLINE, "frameId:%4d ", frameId);
            tx_data[2] = (uint8_t) (frameId >> 8);
            tx_data[3] = (uint8_t) (frameId);
            tx_data[4] = (uint8_t) (nFrame >> 8);
            tx_data[5] = (uint8_t) (nFrame);
            //fx_res = (uint8_t) fx_file_read(&update_file, &tx_data[6], 128, &nBytes);     //Read 128 bytes
            fread(&tx_data[6],128,1,update_file);
            Refresh_Wdg_mock ();                             // refresh the watchdog timer

            if ((frameId%1500) == 0)     //Advice to the user periodically
            {
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_UPGRADING, TRUE);  // warn about incoming update
            }

            do {
                //tx_thread_sleep ((uint16_t)OSTIME_10MSEC);
                printf("Sleep\n");
                host_ack = Boot_Send_Message_mock(tx_data, (6 + 128 + 1));
                Refresh_Wdg_mock ();
                if(host_ack != 'N')
                {
                    n_errors++;
                    if (n_errors > 5)
                    {
                        R100_test_result.misc.boot = eERR_BOOT_PROCESSOR_UPDATE;
                        return FALSE;
                    }
                }
            } while (host_ack != 'N');
        }

		// Wait a little time
        Pasatiempos_mock (1000);
      //  fx_res = (uint8_t) fx_file_close (&update_file);
        fclose(update_file);
        Pasatiempos_mock (100);
        //fx_res = (uint8_t) fx_file_delete (&sd_fx_media, UPGRADE_FILENAME);
       // fx_res = (uint8_t) fx_media_flush (&sd_fx_media);

        // Wait a little time to save gold programm into SPI memory
        Pasatiempos_mock (1000);

        // update the non volatile data before update
        nv_data.update_flag = 1;
        NV_Data_Write_mock(&nv_data);

        return TRUE; // update pending
    }
    return FALSE; // no update found in uSD
}

/******************************************************************************
 ** Name:    R100_Check_SAT_RTC
 *****************************************************************************/
/**
 ** @brief   Power off the device programming an automatic power-on
 **
 ** @param   dev_error         current device error
 ** @param   force_warning     force the warning message
 **
 ** @return  eERR_NONE
 ******************************************************************************/
static ERROR_ID_e R100_Check_SAT_RTC_mock (ERROR_ID_e dev_error, bool_t force_warning)
{
    //UNUSED(force_warning);

	printf("R100_Check_SAT_RTC_mock\n");
    //uint32_t        err_open;           // SD media open result ...
    DB_TEST_RESULT_t R100_Test_Result;
    //bool_t flag_send_result = FALSE;

    // copy test variable
    memset(&R100_Test_Result,0,sizeof(DB_TEST_RESULT_t));
    nv_data.test_id++;
    if (nv_data.test_id >= NUM_BASIC_TEST) nv_data.test_id = 0;

    if ((R100_Errors_cfg[dev_error].daily_monthly == 1) &&  // Monthly recovery error
        (nv_data.test_id != TEST_FULL_ID))                  // no monthly test
        {
            //flag_send_result = TRUE;
            R100_Test_Result.test_status = TEST_ABORTED;
            R100_Test_Result.error_code = dev_error;
            return dev_error;       // save this error
        }
        else
        {
            return eERR_NONE;       // reset the error
        }
/*
    Trace_Arg (TRACE_NEWLINE, "  DEVICE ERROR= %4d", dev_error);

    // check if led status must be blink or not
    nv_data.status_led_blink = (nv_data.status_led_blink == LED_BLINK_ON)? R100_Errors_cfg[dev_error].led_flashing: LED_BLINK_OFF;

    // a critical error is detected --> warn the user periodically
    if (force_warning || (diff_time_seconds (pon_time, nv_data.time_warning) >= WARNING_PERIOD_SAT))
    {
        // register the time for the next warning
        nv_data.time_warning = pon_time;

        // maintain the power-on switch active
        Rtc_Program_Wakeup (WAKEUP_POWERON);

        // Power on the external circuits, adding an extra time to stabilize the power supplies ...
        g_ioport.p_api->pinWrite (IO_PSU_EN,  IOPORT_LEVEL_HIGH);
        tx_thread_sleep (OSTIME_20MSEC);

        // report the startup event
        Trace (TRACE_TIME_STAMP | TRACE_NEWLINE, "Powering Off --> Critical Error");

        // Load Device Settings from NFC (the device info is read to be available in configuration mode)
        Settings_Open_From_NFC ();

        // Load Device Info --> NFC data MUST be read before !!!
        Device_Init (NULL);

        // resume task used for test ...
        tx_thread_resume (&thread_audio);
        tx_thread_resume (&thread_comm);

        // open the uSD
        err_open = fx_media_init0_open ();
        if ((err_open != FX_SUCCESS) && (err_open != FX_PTR_ERROR))
        {
            Lock_on_Panic ();
        }
        tx_thread_sleep (OSTIME_100MSEC);

        // Load Audio resources from SD-Card and proceed to play the SAT message
        if (Load_Audio_Resources() != eERR_NONE)
        {
            Lock_on_Panic ();
        }
        else
        {
            tx_thread_sleep (OSTIME_100MSEC);
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
            Pasatiempos_Listening ();
        }
    }

    if(flag_send_result == TRUE)
    {
        // Power on the external circuits, adding an extra time to stabilize the power supplies ...
        g_ioport.p_api->pinWrite (IO_PSU_EN,  IOPORT_LEVEL_HIGH);
        tx_thread_sleep (OSTIME_20MSEC);

        // report the startup event
        Trace (TRACE_TIME_STAMP | TRACE_NEWLINE, "Powering Off --> Critical Error");

        // Load Device Settings from NFC (the device info is read to be available in configuration mode)
        Settings_Open_From_NFC ();

        // Load Device Info --> NFC data MUST be read before !!!
        Device_Init (NULL);

        // resume task used for test ...
        tx_thread_resume (&thread_audio);
        tx_thread_resume (&thread_comm);

        // open the uSD
        err_open = fx_media_init0_open ();
        if ((err_open != FX_SUCCESS) && (err_open != FX_PTR_ERROR))
        {
            Lock_on_Panic ();
        }
        tx_thread_sleep (OSTIME_100MSEC);

        // Load Audio resources from SD-Card and proceed to play the SAT message
        if (Load_Audio_Resources() != eERR_NONE)
        {
            Lock_on_Panic ();
        }
        else
        {
            tx_thread_sleep (OSTIME_100MSEC);
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
            Pasatiempos_Listening ();
        }

        // If module options are present
        if(Get_Device_Info()->enable_b.sigfox || Get_Device_Info()->enable_b.sigfox)
        {
            // Check if Comms modules are OK
            Execute_Test_Comms      (&R100_Test_Result.comms);

            if (Get_Device_Info()->enable_b.sigfox && (R100_Test_Result.comms.sigfox == eERR_NONE))
            {
                tx_thread_sleep (OSTIME_200MSEC);
                // Send Sigfox Alert
                Comm_Sigfox_Open();
                tx_thread_sleep (OSTIME_200MSEC);
                // Generate and Send Test report using Sigfox
                Comm_Sigfox_Generate_Send_Test(&R100_Test_Result);
                tx_thread_sleep (OSTIME_200MSEC);

                // Advice to the user periodically -- ???
                // Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);
                //Pasatiempos (OSTIME_10SEC);
                // Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);
                Pasatiempos (OSTIME_10SEC);
                //Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_DONE, TRUE);
            }
            else
            {
                if (Get_Device_Info()->enable_b.wifi && (R100_Test_Result.comms.wifi == eERR_NONE))
                {
                    // Establish Wifi connection and check if host is available
                    if(Execute_Wifi_Init_Host_Alive(&R100_Test_Result) == false)
                    {
                         // Do something if aborted?? Audio message, etc,...
                    }

                    // If server connection is Alive
                    if(Comm_Get_Wifi_Host_Alive_Flag() == TRUE)
                    {
                        if (Execute_Send_Wifi_Test_Frame(&R100_Test_Result) == false)
                        {
                            // Do something if aborted?? Audio message, etc,...
                        }
                        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_DONE, TRUE);
                        Pasatiempos (OSTIME_10SEC);
                    }
                }
            }
        }
    }

    // Bye-Bye
    R100_PowerOff();*/
}

/******************************************************************************
** Name:    Test_uSD_Resources
*****************************************************************************/
/**
** @brief   Function to check the presence of the audio resources
**          and firmware updates
**
** @param   none
**
** @return  none
**
******************************************************************************/
static void Test_uSD_Resources_mock (void)
{
    static  uint32_t        sd_opened = 0;
            uint32_t        err_open;           // SD media open result ...
            uint8_t         fx_res1, fx_res2;
    static  FILE         *update_file1, *update_file2;

    printf("Test_uSD_Resources_mock\n");
    // open the uSD
    if (!sd_opened)
    {
        // Initialize fx media in uSD
      /*  err_open = fx_media_init0_open ();
        if ((err_open != FX_SUCCESS) && (err_open != FX_PTR_ERROR))
        {
            Lock_on_Panic ((uint8_t)err_open, 18);
        }
        tx_thread_sleep (OSTIME_100MSEC);
*/
        // Load Audio resources from SD-Card
        if (Load_Audio_Resources_mock() == eERR_AUDIO_MAX)
        {
            //Check if a Firmware and golden files are detected in uSD, if not error
           // fx_res1 = (uint8_t) fx_file_open(&sd_fx_media, &update_file1, GOLDEN_FILENAME,  FX_OPEN_FOR_READ);
            update_file1 = fopen (GOLDEN_FILENAME,"r");
          //  fx_res2 = (uint8_t) fx_file_open(&sd_fx_media, &update_file2, UPGRADE_FILENAME, FX_OPEN_FOR_READ);
            update_file2 = fopen (UPGRADE_FILENAME,"r");
            if (update_file1==NULL|| (update_file2==NULL))
            {
                //error
                nv_data.error_code = eERR_AUDIO_MAX;
                Trace_mock (TRACE_NEWLINE, "NUMBER OF AUDIOS ERROR!!!");
                Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
                Pasatiempos_Listening_mock ();

                // close fx_media before power down
              //  fx_media_close(&sd_fx_media);
             //  tx_thread_sleep (OSTIME_200MSEC);

                // this function ends the execution !!!
                R100_PowerOff_mock();
            }
            else
            {
                // error in audios
                Set_Audio_Player_in_error();
            }

            //fx_res1 = (uint8_t) fx_file_close (&update_file1);
            //fx_res2 = (uint8_t) fx_file_close (&update_file2);
            fclose(update_file1);
            fclose(update_file2);
        }

        sd_opened++;
    }
}

/******************************************************************************
** Name:    Test_Wireless_Comms
*****************************************************************************/
/**
** @brief   Function to check the wireless comms. devices
**
** @param   pResult     pointer to result parameters
** @param   auto_test   if the test is automatic or forced by an user
**
** @return  none
**
******************************************************************************/
static void Test_Wireless_Comms_mock (DB_TEST_RESULT_t *pResult, bool_t auto_test)
{
            SIGFOX_MSG_ID_e gps_id;
            uint32_t        position;
            uint32_t        tout = (3000);    // 3 minutes timeout for GPS
            uint8_t         fx_res;
    static  FILE         *update_file;

    printf("Test_Wireless_Comms_mock\n");
    // check if the Sigfox option is enabled AND no sigfox error in comms test
    if (Get_Device_Info()->enable_b.sigfox && (pResult->comms.sigfox == eERR_NONE))
    {

        // 1. SIGFOX: Send test result through SIGFOX
        if (Execute_Send_Sigfox_Test_mock(pResult, auto_test) == false) { return; }
        Refresh_Wdg_mock ();
        if (Execute_Send_Sigfox_Exp_mock(pResult, auto_test) == false) { return; }
        Refresh_Wdg_mock ();
    }

    // check if the wifi option is enabled AND no wifi error in comms test
    if (Get_Device_Info()->enable_b.wifi && (pResult->comms.wifi == eERR_NONE))
    {
        // 0. WIFI:
        // In case of a MANUAL or AUTOTEST (Not MONTHLY!!!) Test, send test frame result through WIFI IF sigfox option active AND in error or NO sigfox option active
        if (((pResult->test_id < TEST_FULL_ID) || (pResult->test_id == TEST_MANUAL_ID)) &&
            ((Get_Device_Info()->enable_b.sigfox && (pResult->comms.sigfox != eERR_NONE)) || (! Get_Device_Info()->enable_b.sigfox)))
        {
            // Establish Wifi connection and check if host is available
            if (Execute_Wifi_Init_Host_Alive_mock(pResult) == false) { return; }

            // If server connection is Alive
            if (Comm_Get_Wifi_Host_Alive_Flag() == TRUE)
            {
                if (Execute_Send_Wifi_Test_Frame_mock(pResult) == false) { return; }
            }
        }

        // 0.5 WIFI:
        // In case of an AUTOTEST (Not MONTHLY!!!) Test, send Episode .WEE File through Wifi if there are pending
        if (pResult->test_id < TEST_FULL_ID)
        {
            // Send Test File, it will check if its a DIARY Test, and will connect to Wifi
            if (Execute_Send_Wifi_Episode_mock(pResult) == false) { return; }
        }

        // 1. WIFI:
        // In case of a MANUAL or MONTHLY Test, send Test File through Wifi
        // 2. WIFI:
        // In case of a MANUAL or MONTHLY Test, send Episode .WEE File through Wifi if there are pending
        // 3. WIFI:
        // In case of a MANUAL or MONTHLY Test, If there is a cfg or firmware available download them
        if ((pResult->test_id == TEST_MANUAL_ID) || (pResult->test_id == TEST_FULL_ID))
        {
            // Establish Wifi connection and check if host is available
            if (Execute_Wifi_Init_Host_Alive_mock(pResult) == false) { return; }
            // If server connection is Alive
            if (Comm_Get_Wifi_Host_Alive_Flag() == TRUE)
            {
                // Send Test File
                if (Execute_Send_Wifi_Test_File_mock(pResult) == false) { return; }

                // Send Episode .WEE File if any pending
                if (Execute_Send_Wifi_Episode_mock(pResult) == false) { return; }

                // check if the .cfg is available
                if (Comm_Wifi_Is_Cfg_Available())
                {
                    // Receive Wifi Configuration File
                    if (Execute_Receive_Wifi_Configuration_mock(pResult) == false) { return; }
                }

                // check if firmware is available
                if (Comm_Wifi_Is_Frmw_Available())
                {
                    // Receive Wifi Upgrade File
                    if (Execute_Receive_Wifi_Upgrade_mock(pResult)  == false) { return; }
                }
            }
        }
    }

    // Load and read configuration file on SD
   //fx_res = (uint8_t) fx_file_open(&sd_fx_media, &update_file, UPGRADE_FILENAME, FX_OPEN_FOR_READ);
    update_file = fopen(UPGRADE_FILENAME,"r");
    if (update_file == NULL)
    {
        // Do not perform GPS operations....
        Trace_mock (TRACE_NEWLINE, "");
    //  Trace (TRACE_TIME_STAMP | TRACE_NEWLINE, "UPDATE AVAILABLE !!");
  //      fx_res = (uint8_t) fx_file_close (&update_file);

    }
    else
    {
       // check if the GPS option is enabled
        if (Get_Device_Info()->enable_b.gps    && (pResult->comms.gps    == eERR_NONE)         &&
          ((Get_Device_Info()->enable_b.sigfox && (pResult->comms.sigfox == eERR_NONE))     ||
           (Get_Device_Info()->enable_b.wifi   && (pResult->comms.wifi   == eERR_NONE))))
        {
            // If a Monthly or Manual Test has been performed, send also the GPS Position
            if((pResult->test_id == TEST_FULL_ID) || (pResult->test_id == TEST_MANUAL_ID))
            {
                Refresh_Wdg_mock ();

                // Wait some time to get the GPS position...
                position = Execute_Get_Gps_Position_Test_mock (pResult, tout);

                if (position == 2) { return; }

                // position obtained
                if (position == 1)
                {
                    Refresh_Wdg_mock ();

                    // the GPS test is classified as manual or periodic (automatic)
                    gps_id = (pResult->test_id == TEST_MANUAL_ID) ? MSG_ID_MAN_TEST_GPS : MSG_ID_MON_TEST_GPS;

                    // Send through Sigfox
                    if(Get_Device_Info()->enable_b.sigfox && (pResult->comms.sigfox == eERR_NONE))
                    {
                        // Open Sigfox Comm...
                        Comm_Sigfox_Open();
                        Refresh_Wdg_mock ();

                        Comm_Sigfox_Generate_Send_Position(gps_id);
                        Refresh_Wdg_mock ();

                        for(uint8_t i = 0; i<200; i++)
                        {
                            // check if cover is closed or ON_OFF key is pressed
                            if (Check_Test_Abort_User_mock(pResult))
                            {
                                return;
                            }

                            if ((i%100) == 0)     //Advice to the user periodically
                            {
                                if(pResult->test_id == TEST_MANUAL_ID)
                                {
                                    flag_transmission_message = TRUE;
                                    Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                                }
                            }
                         //   tx_thread_sleep (OSTIME_100MSEC);
                            Refresh_Wdg_mock ();
                        }
                    }
                    else // Otherwise, send through Wifi
                    {
                        // Establish Wifi connection and check if host is available (just in case)
                        if (Execute_Wifi_Init_Host_Alive_mock(pResult) == false) { return; }

                        // If server connection is Alive
                        if (Comm_Get_Wifi_Host_Alive_Flag() == TRUE)
                        {
                            if (Execute_Send_Wifi_Position_Frame_mock(pResult, gps_id) == false) { return; }
                        }
                    }

                    Refresh_Wdg_mock ();

                    // wait while playing all pending audio messages ...
                    Pasatiempos_Listening_mock();
                }
            }
        }
    }

    // Play last transmission message
    if(flag_transmission_message)
    {
        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_DONE, TRUE);
        Pasatiempos_Listening_mock();
    }
}

/******************************************************************************
** Name:    Execute_Test_Cert
*****************************************************************************/
/**
** @brief   Function to load certificates
**
** @param   pResult     pointer to result parameters
**
** @return  none
**
******************************************************************************/
/*static void Execute_Test_Cert(TEST_CERT_t *pCertificate)
{
    // Initialize the NetX system.
    nx_init();

    memset((uint16_t*)&pCertificate->tls_cert_exp, 0, sizeof(TEST_CERT_t));
    memset((uint16_t*)&pCertificate->wpa_eap_cert_exp, 0, sizeof(TEST_CERT_t));
    memset((uint16_t*)&pCertificate->tls_cert_load, 0, sizeof(TEST_CERT_t));
    memset((uint16_t*)&pCertificate->wpa_eap_cert_load, 0, sizeof(TEST_CERT_t));
    // Check TLS CA & WPA-EAP cert expiration
    pCertificate->tls_cert_exp = check_TLS_Cacert();
    if(pCertificate->tls_cert_exp != 0)
    {
        if(pCertificate->tls_cert_exp == 1) pCertificate->tls_cert_exp = CERT_EXP_SOON;
        else pCertificate->tls_cert_exp = eRR_EXPIRED_CERT_TLS;
    }
    pCertificate->wpa_eap_cert_exp = check_WPA_EAP_Cert();
    if(pCertificate->wpa_eap_cert_exp != 0) 
    {
        if(pCertificate->wpa_eap_cert_exp == 1) pCertificate->wpa_eap_cert_exp = CERT_EXP_SOON;
        else pCertificate->wpa_eap_cert_exp = eRR_EXPIRED_WPA_EAP;
    }

    // Check if TLS CA & WPA-EAP certificates have been uploaded correctly
    pCertificate->tls_cert_load = (uint16_t) Wifi_Configure_TLS_Cacert(false);
    if(pCertificate->tls_cert_load != 0) pCertificate->tls_cert_load = eRR_NO_LOAD_CERT_TLS;
    pCertificate->wpa_eap_cert_load = (uint16_t) Wifi_Configure_WPA_EAP_Cert(false);
    if(pCertificate->wpa_eap_cert_load != 0) pCertificate->wpa_eap_cert_load = eRR_NO_LOAD_WPA_EAP;

}*/

/******************************************************************************
** Name:    Execute_Test
*****************************************************************************/
/**
** @brief   Function that executes the device test:
**              .- verify the resources memory (checksum)
**              .- power supplies    (DC-MAIN and +12volts)
**              .- Batteries percent (battery 1 and 2)
**              .- Temperature
**
** @param   pResult     pointer to result parameters
** @param   auto_test   if the test is automatic or forced by an user
**
** @return  none
**
******************************************************************************/
static void Execute_Test_mock (DB_TEST_RESULT_t *pResult, bool_t auto_test)
{
    uint32_t        err_open;   // SD media open result ...

    printf("Execute_Test_mock\n");
    // set in test mode blinking the ONOFF led ...
    test_mode = true;
    Led_Blink_mock (LED_ONOFF);

    // by default, blink the status LED while device is sleeping (periodic blink)
    nv_data.status_led_blink = LED_BLINK_ON;

    // maintain the power-on switch active
    Rtc_Program_Wakeup (WAKEUP_POWERON);

    // Power on the external circuits, adding an extra time to stabilize the power supplies ...
  //  g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_HIGH);
   // tx_thread_sleep (OSTIME_20MSEC);

    // report the startup event
    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Startup: TEST");

    // Initialize the file system for SD-Card, if inserted ...
    // fx_media_init0 ();
    // resume task used for test ...
   // tx_thread_resume (&thread_audio);
   // err_open = fx_media_init0_open ();
 /*   if ((err_open != FX_SUCCESS) && (err_open != FX_PTR_ERROR))
    {
        Trace (TRACE_TIME_STAMP + TRACE_NEWLINE, " uSD ERROR !!! LOCK DEVICE  !!!");

        Lock_on_Panic ((uint8_t)err_open, 19);
    }*/

    Battery_i2C_Init_mock();

    sMon_Inc_TestManual();

    // initialize the whole structure
    memset (pResult, 0, sizeof(DB_TEST_RESULT_t));

    Refresh_Wdg_mock ();
    // Load Device Settings from NFC (the device info is read to be available in configuration mode)
    Settings_Open_From_NFC_mock ();
    Refresh_Wdg_mock ();

    // Load Device Info --> NFC data MUST be read before !!!
    Device_Init_mock (pResult->device_sn);

    // Check if a new configuration file is present in the uSD to update the Device Settings
    Settings_Open_From_uSD_mock ();

    R100_Check_SAT_mock(Get_NFC_Device_Info()->error_code,true);

    // compensate the UTC time ...
    battery_RTC.utc_time = (int8_t) Get_Device_Settings()->misc.glo_utc_time;
    RTC_Normalize_Time (&battery_RTC);

    Set_Device_Date_time_mock();

    // resume task used for test and blink the ONOFF led ...
 //   tx_thread_resume (&thread_core);
 //   tx_thread_sleep (OSTIME_20MSEC); // wait pointer initialization just in case

/*    tx_thread_resume (&thread_audio);
    tx_thread_resume (&thread_patMon);
    tx_thread_resume (&thread_defibrillator);
    tx_thread_resume (&thread_hmi);
    tx_thread_resume (&thread_comm);
*/
    // test uSD resources (audio and firmware updates)
    Test_uSD_Resources_mock();

    // if an automatic test must be executed, update the test identifier ...
    if (auto_test)
    {
        pResult->error_code = R100_Check_SAT_RTC_mock (nv_data.error_code, false);
        pResult->test_id = nv_data.test_id;
    }
    else {
        // when test is executed due to an user request, force a full test
        pResult->test_id = TEST_MANUAL_ID;

        // Blink all leds to test them
        Led_Blink_mock (LED_ONOFF);
        Led_Blink_mock (LED_SHOCK);
        Led_Blink_mock (LED_PATYPE);
    }

    Trace_mock (TRACE_NEWLINE, "");
    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test running !!!");
    Trace_mock (                   TRACE_NEWLINE, "=============================");

    // sequential test of devices and accessories
    // be sure that the battery test is executed before electrodes test to
    // be able to check the expiration date
    // Don't forget to refresh the watchdog

     // Check if battery voltage is in the working temperature range else do not execute the test
    if ((battery_temperature > BATTERY_VERY_LOW_TEMP) &&
        (battery_temperature < BATTERY_VERY_HIGH_TEMP))
    {
        Execute_Test_Patmon_mock(&pResult->patmon, nv_data.test_id);
        // Patient impedance is detected...check again if patient is connected
        if (pResult->patmon.ADS_pat == eERR_ADS_PAT_CONNECTED)
        {
            Pasatiempos_mock (10);
            Execute_Test_Patmon_mock (&pResult->patmon, nv_data.test_id);
        }

        //Run battery and electrodes test
        Execute_Test_Battery_mock    (&pResult->battery, auto_test, false);
        Execute_Test_Electrodes_mock (&pResult->electrodes, auto_test, false);

        // If patient impedance is detected abort hardware testing
        if (!pResult->patmon.ADS_pat)
        {
            Execute_Test_Misc_mock       (&pResult->misc, pResult->device_sn, pResult->test_id);
            Execute_Test_Voltage_mock    (&pResult->voltage);
            Execute_Test_Comms_mock      (&pResult->comms);
            //Execute_Test_Cert       (&pResult->certificate);
            if (((pResult->test_id == TEST_FULL_ID) ||
                 (pResult->test_id == TEST_MANUAL_ID)) &&
                 (pResult->battery.error_code != eERR_BATTERY_REPLACE))

            {
                Execute_Test_GPS_mock   (pResult->gps_position);
                Execute_Test_Defib_mock (&pResult->defib);
            }

            // In case of electrodes not connected...check again if electrodes are inserted
            if ((pResult->electrodes.error_code == eERR_ELECTRODE_NOT_CONNECTED) && !auto_test)
            {
                Pasatiempos_mock (10000);
                Execute_Test_Electrodes_mock (&pResult->electrodes, auto_test, false);
            }

            // re-assign the error code
            if (pResult->error_code == eERR_NONE)   // there is not error before
            {
                nv_data.error_code = eERR_NONE;
                if (pResult->electrodes.error_code) nv_data.error_code = pResult->error_code = pResult->electrodes.error_code;
                if (pResult->battery.error_code)    nv_data.error_code = pResult->error_code = pResult->battery.error_code;
                if (pResult->misc.audio)            nv_data.error_code = pResult->error_code = pResult->misc.audio;
                if (pResult->misc.boot)             nv_data.error_code = pResult->error_code = pResult->misc.boot;
                if (pResult->misc.nfc)              nv_data.error_code = pResult->error_code = pResult->misc.nfc;
                if (pResult->defib.error_code)      nv_data.error_code = pResult->error_code = pResult->defib.error_code;
                if (pResult->patmon.ADS_cal)        nv_data.error_code = pResult->error_code = pResult->patmon.ADS_cal;
                if (pResult->patmon.ADS_comms)      nv_data.error_code = pResult->error_code = pResult->patmon.ADS_comms;
                if (pResult->voltage.dc_18v_error)  nv_data.error_code = pResult->error_code = pResult->voltage.dc_18v_error;
                if (pResult->voltage.dc_main_error) nv_data.error_code = pResult->error_code = pResult->voltage.dc_main_error;
            }
            if (pResult->error_code) pResult->test_status = TEST_NOT_OK;
            nv_data.status_led_blink = R100_Errors_cfg[pResult->error_code].led_flashing;
        }
        else
        {
            // Warning --> Patient is connected or a Patmon error is detected
            if (!Is_SAT_Error_mock (nv_data.error_code))
            {
                nv_data.error_code = pResult->patmon.ADS_pat;
            }
            pResult->error_code = nv_data.error_code;
            pResult->test_status = TEST_ABORTED;
        }
    }
    else
    {
        // Warning --> Temperature out of range
        if (!Is_SAT_Error_mock (nv_data.error_code))
        {
            nv_data.error_code = eERR_BATTERY_TEMP_OUT_RANGE_OFF;
        }
        pResult->error_code = (uint32_t) nv_data.error_code;
        pResult->battery.bat_temperature = (int32_t) battery_temperature;
        pResult->test_status = TEST_ABORTED;

        // the device is switched OFF --> consider to play a message before to power-off
        // TODO
    }

    if (pResult->error_code)
    {
        nv_data.status_led_blink = (nv_data.status_led_blink == LED_BLINK_ON)? R100_Errors_cfg[pResult->error_code].led_flashing: LED_BLINK_OFF;
        // register the time for the next warning
        nv_data.time_warning = pon_time;
        nv_data.open_cover = pon_time;
    }


    // Inform to the user about the test result
    Refresh_Wdg_mock ();     // refresh the internal watchdog timer
    if (pResult->test_id == TEST_MANUAL_ID)
    {
        if(pResult->test_status != TEST_ABORTED)
        {
            // play the audio message advertising Device state
            if(pResult->battery.error_code == eERR_BATTERY_REPLACE && (Is_SAT_Error_mock(pResult->error_code) == false))
            {
                /* Nothing to do */
            }
            else
            {
                if (Is_SAT_Error_mock(pResult->error_code))
                {
                    Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_CALL_SAT, TRUE);
                }
                else
                {
                    if ((nv_data.status_led_blink == LED_BLINK_ON) && (Is_Mode_Trainer() == FALSE))
                        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_DEVICE_READY, TRUE);
                }
            }
        }

        // In trainer mode remind with an audio message
        if (Is_Mode_Trainer()) Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRAINER_MODE, TRUE);
        Pasatiempos_Listening_mock ();
    }

    // register the result ...
    // auto_  test --> Generate and trace test results (serial port and file)
    // manual test --> Generate and trace test results (serial port and audio)
    Refresh_Wdg_mock ();     // refresh the internal watchdog timer
    DB_Test_Generate_Report_mock (pResult, true);
    Refresh_Wdg_mock ();     // refresh the internal watchdog timer

    // Inform to the user about the test result
    if (Check_Test_Abort_User_mock(pResult))
    {
        nv_data.error_code = pResult->error_code;
        return;
    }
    Refresh_Wdg_mock ();     // refresh the internal watchdog timer

    // If Sigfox and Wifi options are not present, skip all this section
    if (Get_Device_Info()->enable_b.sigfox || Get_Device_Info()->enable_b.wifi)
    {
        Test_Wireless_Comms_mock (pResult, auto_test);
    }

    nv_data.error_code = pResult->error_code;
}

/******************************************************************************
** Name:    Execute_Startup_Test
*****************************************************************************/
/**
** @brief   Function that executes the device startup test:
**              .- verify the resources memory (checksum)
**              .- power supplies    (DC-MAIN and +12volts)
**              .- Battery state of charge
**              .- Battery temperature
**              .- PatMon operative
**
** @param   pResult     pointer to result parameters
** @param   auto_test   if the test is automatic or forced by an user
**
** @return  none
**
******************************************************************************/
static void Execute_Startup_Test_mock (DB_TEST_RESULT_t *pResult, uint8_t test_id)
{

	printf("Execute_Startup_Test_mock\n");
    // by default, blink the status LED while device is sleeping (periodic blink)
    nv_data.status_led_blink = LED_BLINK_ON;

    // initialize the whole structure
    memset (pResult, 0, sizeof(DB_TEST_RESULT_t));

    // when test is executed due to an user request, force a full test
    pResult->test_id = test_id;

    Trace_mock (TRACE_NEWLINE, "");
    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Device Running Test!!!");
    Trace_mock (                   TRACE_NEWLINE, "=============================");

    // sequential test of devices and accessories
    // be sure that the battery test is executed before electrodes test to
    // be able to check the expiration date
    // Dont forget to refresh the watchdog
 /*   Execute_Test_Misc_mock       (&pResult->misc, pResult->device_sn, false);
    Execute_Test_Electrodes_mock (&pResult->electrodes, false, true);
    Execute_Test_Voltage_mock    (&pResult->voltage);
    Execute_Test_Patmon_mock     (&pResult->patmon, test_id);
    Execute_Test_Battery_mock    (&pResult->battery, false, true);
*/
    // register the result ...
    // init  test --> Generate test results (file) with test id = 50
 //   DB_Test_Generate_Report_mock (pResult, true);

    // re-assign the error code
  /*  nv_data.error_code = eERR_NONE;
    if (pResult->electrodes.error_code) nv_data.error_code = pResult->error_code = pResult->electrodes.error_code;
    if (pResult->battery.error_code)    nv_data.error_code = pResult->error_code = pResult->battery.error_code;
    if (pResult->misc.audio)            nv_data.error_code = pResult->error_code = pResult->misc.audio;
    if (pResult->misc.boot)             nv_data.error_code = pResult->error_code = pResult->misc.boot;
    if (pResult->defib.error_code)      nv_data.error_code = pResult->error_code = pResult->defib.error_code;
    if (pResult->patmon.ADS_cal)        nv_data.error_code = pResult->error_code = pResult->patmon.ADS_cal;
    if (pResult->patmon.ADS_comms)      nv_data.error_code = pResult->error_code = pResult->patmon.ADS_comms;
    if (pResult->voltage.dc_18v_error)  nv_data.error_code = pResult->error_code = pResult->voltage.dc_18v_error;
    if (pResult->voltage.dc_main_error) nv_data.error_code = pResult->error_code = pResult->voltage.dc_main_error;

    // save in NFC error code and battery and electrode updated info
    NFC_Write_Device_Info_mock(true);

    if((nv_data.error_code != eERR_NONE) && (!Is_Test_Mode()))
    {
        if(Get_Device_Info()->enable_b.sigfox)
        {
            if(Comm_Is_Sigfox_Initialized() == FALSE)
            {
                // Open Sigfox Comm.
                Comm_Sigfox_Open();
            }
            // Generate and Send Test report using Sigfox
            Comm_Sigfox_Generate_Send_Test(pResult);
            //return;
        }

        /*if(Get_Device_Info()->enable_b.wifi)
        {
            if(Comm_Is_Wifi_Initialized() == FALSE)
            {
                // Open wifi Comm.
                Comm_Wifi_Open();
            }
            // Generate and Send Test report using Wifi
            Comm_Wifi_Generate_Send_Test_Frame(pResult);
        }*/
    //}
}

/******************************************************************************
** Name:    Execute_Test_Montador
*****************************************************************************/
/**
** @brief   Function that executes the device test:
**              .- verify the resources memory (checksum)
**              .- power supplies    (DC-MAIN and +12volts)
**              .- Batteries percent (battery 1 and 2)
**              .- Temperature
**
** @param   pResult     pointer to result parameters
** @param   auto_test   if the test is automatic or forced by an user
**
** @return  none
**
******************************************************************************/
static void Execute_Test_Montador_mock (DB_TEST_RESULT_t *pResult, bool_t auto_test)
{
            uint32_t        err_open;           // SD media open result ...
    static  uint32_t        sd_opened = 0;
            ERROR_ID_e retError = eERR_NONE;
            bool_t      abort_flag = false;
            bool_t      test_flag = false;
            char_t      aux[4];

     printf("Execute_Test_Montador_mock\n");

    // set in test mode
    test_mode_montador = true;

        // by default, blink the status LED
    nv_data.status_led_blink = LED_BLINK_ON;
    Led_On_mock (LED_ONOFF);

    Refresh_Wdg_mock ();     // refresh the internal watchdog timer

    // maintain the power-on switch active
    Rtc_Program_Wakeup (WAKEUP_POWERON);

    // Power on the external circuits, adding an extra time to stabilize the power supplies ...
  //  g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_HIGH);
  //  tx_thread_sleep (OSTIME_20MSEC);
    Sleep(20);

    Refresh_Wdg_mock ();     // refresh the internal watchdog timer

     // resume task used for test and blink the ONOFF led ...


    //HARI DENAK GERATZEN DITU///
 //   tx_thread_resume (&thread_core);
 /*   tx_thread_sleep (OSTIME_20MSEC); // wait pointer initialization just in case

    tx_thread_resume (&thread_audio);
    tx_thread_resume (&thread_patMon);
    tx_thread_resume (&thread_defibrillator);
    tx_thread_resume (&thread_hmi);
    tx_thread_resume (&thread_comm);*/

    // initialize the whole structure
    memset (pResult, 0, sizeof(DB_TEST_RESULT_t));

    Refresh_Wdg_mock ();
    // Load Device Settings from NFC (the device info is read to be available in configuration mode)
    Settings_Open_From_NFC_mock();
    Refresh_Wdg_mock ();
    // Load Device Info --> NFC data MUST be read before !!!
    Device_Init_mock (pResult->device_sn);
    Set_Device_Date_time_mock();

    // open the uSD
    if (!sd_opened)
    {
       // Initialize fx media in uSD
      /* err_open = fx_media_init0_open ();
       if ((err_open != FX_SUCCESS) && (err_open != FX_PTR_ERROR))
       {
           Lock_on_Panic ((uint8_t)err_open, 20);
           abort_flag = true;
       }
       tx_thread_sleep (OSTIME_100MSEC);*/

       // Load Audio resources from SD-Card
       if (err_open = Load_Audio_Resources_mock(), err_open != eERR_NONE)
       {
           Lock_on_Panic_mock ((uint8_t)err_open, 21);
       }


       sd_opened++;
    }

    Battery_i2C_Init_mock();

    pResult->test_id = TEST_FULL_ID;        // forze a FULL TEST

    Trace_mock (TRACE_NEWLINE, "");
    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test running !!!");
    Trace_mock (                   TRACE_NEWLINE, "=============================");

    // Open Uart Comms with the Arduino
    Comm_Uart_Init_mock();
   // tx_thread_sleep (OSTIME_100MSEC);
    Sleep(100);
    retError = Comm_Uart_Set_Baud_Rate_mock(BR_115200);
    if (retError != OK)
    {
        return;
    }
    //tx_thread_sleep (OSTIME_100MSEC);
    Sleep(100);

    // sequential test of devices and accessories
    // be sure that the battery test is executed before electrodes test to
    // be able to check the expiration date
    // Dont forget to refresh the watchdog
    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test Miscelanious: NFC");

    Execute_Test_Misc_mock       (&pResult->misc, pResult->device_sn, false);
    if (pResult->misc.nfc == OK)
    {
        //Comm_Uart_Send("OK5");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK5");
        Comm_Uart_Send_mock("OK5");
    }
    else
    {
        //Comm_Uart_Send("KO5");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 5");
        Comm_Uart_Send_mock("KO5");
        Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ERROR:%d ", pResult->misc.nfc);
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Pasatiempos_Listening_mock ();
        abort_flag = true;
        test_flag = true;
    }

    if (pResult->misc.boot == OK)
    {
        //Comm_Uart_Send("OK6");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK6");
        Comm_Uart_Send_mock("OK6");
    }
    else
    {
        //Comm_Uart_Send("KO6");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 6");
        Comm_Uart_Send_mock("KO6");
        Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ERROR:%d ", pResult->misc.boot);
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Pasatiempos_Listening_mock ();
        abort_flag = true;
        test_flag = true;
    }

    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test Miscelanious: AUDIO");
    if (pResult->misc.audio == OK)
    {
        //Comm_Uart_Send("OK7");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK7");
        Comm_Uart_Send_mock("OK7");
    }
    else
    {
        //Comm_Uart_Send("KO7");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 7");
        Comm_Uart_Send_mock("KO7");
        Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ERROR:%d ", pResult->misc.audio);
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Pasatiempos_Listening_mock ();
        test_flag = true;
    }

    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test Voltages...");
    Execute_Test_Voltage_mock    (&pResult->voltage);
    if ((pResult->voltage.dc_main_error == OK) &&
        (pResult->voltage.dc_18v_error == OK))
    {
        //Comm_Uart_Send("OK8");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK8");
        Comm_Uart_Send_mock("OK8");
    }
    else
    {
        //Comm_Uart_Send("KO8");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 8");
        Comm_Uart_Send_mock("KO8");
        Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ERROR_DC_MAIN:%d ", pResult->voltage.dc_main_error);
        Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ERROR_18V:%d ", pResult->voltage.dc_18v_error);
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Pasatiempos_Listening_mock ();
        abort_flag = true;
        test_flag = true;
    }

    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test Patmon...");
    if (Execute_Test_Patmon_mock(&pResult->patmon, 0) == false)
    {
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 9");
        Comm_Uart_Send_mock("KO9");
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Pasatiempos_Listening_mock ();
        test_flag = true;
    }
    else
    {
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK9");
        Comm_Uart_Send_mock("OK9");
    }
    if ((pResult->patmon.ADS_comms == OK) &&
        (pResult->patmon.ADS_pat == OK) &&
        (pResult->patmon.ADS_cal == OK))
    {
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK10");
        Comm_Uart_Send_mock("OK10");
    }
    else
    {
        //Comm_Uart_Send("KO10");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 10");
        Comm_Uart_Send_mock("K10");
        Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ERROR ADS COMMS:%d ", pResult->patmon.ADS_comms);
        Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ERROR ADS PAT:%d ", pResult->patmon.ADS_pat);
        Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ERROR ADS CAL:%d ", pResult->patmon.ADS_cal);
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Pasatiempos_Listening_mock ();
        abort_flag = true;
        test_flag = true;
    }

    if ((pResult->patmon.ADS_temperature < 40) && (pResult->patmon.ADS_temperature > 5))
    {
        //Comm_Uart_Send("OK11");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK11");
        Comm_Uart_Send_mock("O11");
    }
    else
    {
        //Comm_Uart_Send("KO11");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 11");
        Comm_Uart_Send_mock("K11");
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ADS_temperature:%d ", (uint32_t) pResult->patmon.ADS_temperature);
        Pasatiempos_Listening_mock ();
        abort_flag = true;
        test_flag = true;
    }

    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test Battery...");
    Execute_Test_Battery_mock    (&pResult->battery, auto_test, false);
   if (((strcmp((char_t *) pResult->battery.name, "BexenCardio") == 0) ||
        (strcmp((char_t *) pResult->battery.name, "BEXENCARDIO") == 0)) &&
       (pResult->battery.nominal_capacity == 4200))
       {
        //Comm_Uart_Send("OK12");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK12");
        Comm_Uart_Send_mock("O12");
       }
    else
    {
     //   Comm_Uart_Send("KO12");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 12");
        Comm_Uart_Send_mock("K12");
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Pasatiempos_Listening_mock ();
        test_flag = true;
    }

    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test Electrodes...");
    Execute_Test_Electrodes_mock (&pResult->electrodes, true, false);
    // These data is loaded into fixture's memory
    if (((pResult->electrodes.sn == UINT64_C(10664524126543164461)) ||
         (pResult->electrodes.sn == UINT64_C(7061644424646978861))) &&
        ((pResult->electrodes.expiration_date == 132581391) ||
         (pResult->electrodes.expiration_date == 132581647)))
    {
//        Comm_Uart_Send("OK13");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK13");
        Comm_Uart_Send_mock("O13");
    }
    else
    {
        Refresh_Wdg_mock ();
//        Comm_Uart_Send("KO13");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 13");
        Comm_Uart_Send_mock("K13");
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Pasatiempos_Listening_mock ();
        test_flag = true;
    }

    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test Leds&Keys...");
    if (Execute_Test_Leds_Keys_mock () == OK)
    {
        //Comm_Uart_Send("OK14");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK14");
        Comm_Uart_Send_mock("O14");
    }
    else
    {
        //Comm_Uart_Send("KO14");
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 14");
        Comm_Uart_Send_mock("K14");
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
        Pasatiempos_Listening_mock ();
        test_flag = true;
    }

    Refresh_Wdg_mock ();
    if (abort_flag == false)
    {
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Test Desfib...");
        Execute_Test_Defib_mock (&pResult->defib);
        Refresh_Wdg_mock ();
        if (pResult->defib.error_code)
        {
            //Comm_Uart_Send("KO15");
            Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "ERROR 15");
            Comm_Uart_Send_mock("K15");
            Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, "ERROR:%d", pResult->defib.error_code);
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
            Pasatiempos_Listening_mock ();
            test_flag = true;
        }
        else
        {
            Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "OK15");
            Comm_Uart_Send_mock("O15");

        }
    }
    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "");

    // register the result ...
    // auto_  test --> Generate and trace test results (serial port and file)
    // manual test --> Generate and trace test results (serial port and audio)
    DB_Test_Generate_Report_mock (pResult, auto_test);
    Refresh_Wdg_mock ();

    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, " ");

    // re-assign the error code
    nv_data.error_code = eERR_NONE;
    if (pResult->defib.error_code)      nv_data.error_code = pResult->defib.error_code;
    if (pResult->patmon.ADS_cal)        nv_data.error_code = pResult->patmon.ADS_cal;
    if (pResult->patmon.ADS_comms)      nv_data.error_code = pResult->patmon.ADS_comms;
    if (pResult->patmon.ADS_pat)        nv_data.error_code = pResult->patmon.ADS_pat;
    if (pResult->voltage.dc_18v_error)  nv_data.error_code = pResult->voltage.dc_18v_error;
    if (pResult->voltage.dc_main_error) nv_data.error_code = pResult->voltage.dc_main_error;
    if (pResult->misc.audio)            nv_data.error_code = pResult->misc.audio;
    if (pResult->misc.nfc)              nv_data.error_code = pResult->misc.nfc;
    if (pResult->misc.boot)             nv_data.error_code = pResult->misc.boot;

    if (test_flag == true)
    {
        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "*************    FAIL   ************");
        Trace_Arg_mock (TRACE_NEWLINE, "************* DEVICE ERROR %4d", nv_data.error_code);
        Trace_Arg_mock (TRACE_NEWLINE, "************* DEVICE ERROR %4d", nv_data.error_code);
        Trace_mock (TRACE_NEWLINE, "************* CLOSE THESE SCREEN");

        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_CALL_SAT, TRUE);
        while (1)
        {
            Pasatiempos_mock (100);
            Comm_Uart_Send_mock ("FAI");
            aux[0] = (char_t)  ((nv_data.error_code / 100) + '0');
            aux[1] = (char_t) (((nv_data.error_code % 100)/10) + '0');
            aux[2] = (char_t)  ((nv_data.error_code %  10) + '0');
            aux[3] = 0;

            Comm_Uart_Send_mock (aux);
        }
    }
    else
    {
        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_DEVICE_READY, TRUE);

        Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "*************    PASS   ************");
        Trace_mock (TRACE_NEWLINE, "*************    CLOSE THESE SCREEN");

        while(1)
        {
            Pasatiempos_mock (100);
            Comm_Uart_Send_mock ("PAS");
        }
    }

    Pasatiempos_Listening_mock ();

    // set the test time to 3:00 am
    nv_data.time_test = RTC_Test_Time();

    Pasatiempos_Listening_mock ();
}


/******************************************************************************
** Name:    Execute_Send_Sigfox_Test
*****************************************************************************/
/**
** @brief   Function that executes the sending of the Sigfox Test
**
** @param   tout  timeout
**
** @return  false if device must be power-off
**
******************************************************************************/
bool_t Execute_Send_Sigfox_Test_mock (DB_TEST_RESULT_t *pResult, bool_t auto_test)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;

    printf("Execute_Send_Sigfox_Test_mock\n");

    Refresh_Wdg_mock();

    if(Comm_Is_Sigfox_Initialized() == FALSE)
    {
        // Open wifi Comm.
        Comm_Sigfox_Open();
    }

    tout = 300;//(OSTIME_60SEC*5);

    ref_timeout = 300;// tx_time_get() + tout;

    Refresh_Wdg_mock ();

    while(  (Comm_Is_Sigfox_Initialized() == FALSE) &&
          (Comm_Get_Sigfox_Error() == eERR_NONE))
    {
        Refresh_Wdg_mock ();
       // tx_thread_sleep (OSTIME_50MSEC);
        // check if cover is closed or ON_OFF key is pressed
        if (Check_Test_Abort_User_mock(pResult))
        {
            return false;
        }

    }

    if(Comm_Is_Sigfox_Initialized())
    {
        Refresh_Wdg_mock ();

        //tout = (OSTIME_60SEC*5);

        //ref_timeout = tx_time_get() + tout;

        Refresh_Wdg_mock ();

        //Generate and Send Test report using Sigfox
        Comm_Sigfox_Generate_Send_Test(pResult);

        Refresh_Wdg_mock ();

        for(uint8_t i = 0; i<200; i++)
        {
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }

            if ((i%100) == 0)     //Advice to the user periodically
            {
                if(!auto_test)
                {
                    flag_transmission_message = TRUE;
                    Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                }
            }
          //  tx_thread_sleep (OSTIME_100MSEC);
            Refresh_Wdg_mock ();
            // break if test sended
            if(Comm_Is_Sigfox_Test_Sended() == TRUE) break;
        }
/*
        if(!auto_test)
        {
            Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_DONE, TRUE);
        }
*/
        // wait while playing all pending audio messages ...
        Pasatiempos_Listening_mock();
    }
    else
    {
        return false;
    }

    return true;
}

/******************************************************************************
** Name:    Execute_Send_Sigfox_Exp
*****************************************************************************/
/**
** @brief   Function that sends battery and electrodes expiration dates
**
** @param   tout  timeout
**
** @return  false if device must be power-off
**
******************************************************************************/
bool_t Execute_Send_Sigfox_Exp_mock (DB_TEST_RESULT_t *pResult, bool_t auto_test)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;

    printf("Execute_Send_Sigfox_Exp_mock\n");

    Refresh_Wdg_mock ();

    if(Comm_Is_Sigfox_Initialized() == FALSE)
    {
        // Open wifi Comm.
        Comm_Sigfox_Open();
    }

    tout = 300;//(OSTIME_60SEC*5);

    ref_timeout = 300;// tx_time_get() + tout;

    Refresh_Wdg_mock ();

    while((Comm_Is_Sigfox_Initialized() == FALSE) &&
          (Comm_Get_Sigfox_Error() == eERR_NONE))
    {
        Refresh_Wdg_mock ();
        //tx_thread_sleep (OSTIME_50MSEC);
        // check if cover is closed or ON_OFF key is pressed
        if (Check_Test_Abort_User_mock(pResult))
        {
            return false;
        }

    }

    if(Comm_Is_Sigfox_Initialized())
    {
        Refresh_Wdg_mock ();

        //tout = (OSTIME_60SEC*5);

        //ref_timeout = tx_time_get() + tout;

        Refresh_Wdg_mock ();

        //Generate and Send 
        Comm_Sigfox_Generate_Send_Expiration_mock(pResult);

        Refresh_Wdg_mock ();

        for(uint8_t i = 0; i<200; i++)
        {
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }

            if ((i%100) == 0)     //Advice to the user periodically
            {
                if(!auto_test)
                {
                    flag_transmission_message = TRUE;
                    Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                }
            }
         //   tx_thread_sleep (OSTIME_100MSEC);
            Refresh_Wdg_mock ();
            // break if test sended
            if(Comm_Is_Sigfox_Test_Sended() == TRUE) break;
        }
/*
        if(!auto_test)
        {
            Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_DONE, TRUE);
        }
*/
        // wait while playing all pending audio messages ...
        Pasatiempos_Listening_mock();
    }
    else
    {
        return false;
    }

    return true;
}

/******************************************************************************
** Name:    Execute_Wifi_Init_Host_Alive
*****************************************************************************/
/**
** @brief   Function that executes the wifi initialization and checks if the host is alive
**
** @param   none
**
** @return  TRUE if process completed FALSE if not
**
******************************************************************************/
bool_t Execute_Wifi_Init_Host_Alive_mock (DB_TEST_RESULT_t *pResult)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;
    uint32_t audio_timeout = 0;

    printf("Execute_Wifi_Init_Host_Alive_mock\n");

    Refresh_Wdg_mock ();

    if((Comm_Is_Wifi_Initialized() == FALSE) || (Comm_Get_Selected_Uart() != eMOD_WIFI))
    {
        // Open wifi Comm.
        Comm_Wifi_Open();

        tout =300;// (OSTIME_60SEC*5);

        ref_timeout = 300;// tx_time_get() + tout;

        Refresh_Wdg_mock ();

        audio_timeout = 2000;//tx_time_get() + OSTIME_20SEC;

        while( (Comm_Is_Wifi_Initialized() == FALSE))
        {
            Refresh_Wdg_mock ();
            //tx_thread_sleep (OSTIME_50MSEC);
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }
         /*   if(tx_time_get() > audio_timeout)
            {
                flag_transmission_message = TRUE;
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                audio_timeout = tx_time_get() + OSTIME_20SEC;
            }*/
        }
    }

    if(Comm_Is_Wifi_Initialized())
    {
        Refresh_Wdg_mock ();

        // 3 minute timeout for checking server alive
        tout =180;// (OSTIME_60SEC*3);

        ref_timeout = 180;//tx_time_get() + tout;

        Refresh_Wdg_mock ();

        Comm_Wifi_Is_Host_Alive();

        audio_timeout = 20000;//tx_time_get() + OSTIME_20SEC;

        while((Comm_Is_Wifi_Host_Alive_Finished() == FALSE))
        {
            Refresh_Wdg_mock ();
            //tx_thread_sleep (OSTIME_50MSEC);
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }
           /* if(tx_time_get() > audio_timeout)
            {
                flag_transmission_message = TRUE;
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                audio_timeout = tx_time_get() + OSTIME_20SEC;
            }*/
        }
    }

    return true;
}

/******************************************************************************
** Name:    Execute_Send_Wifi_Test_Frame
*****************************************************************************/
/**
** @brief   Function that executes the sending of the last performed test frame
**
** @param   tout  timeout
**
** @return  none
**
******************************************************************************/
bool_t Execute_Send_Wifi_Test_Frame_mock (DB_TEST_RESULT_t *pResult)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;
    uint32_t audio_timeout = 0;

    printf("Execute_Send_Wifi_Test_Frame_mock\n");

    // Establish Wifi connection and check if host is available
    //if(Execute_Wifi_Init_Host_Alive() == TRUE)
    //if(Comm_Get_Wifi_Host_Alive_Flag() == TRUE)
    {
        Refresh_Wdg_mock ();

        // 1 minute timeout for sending test file
        tout = 60000;//(OSTIME_60SEC);

        ref_timeout = 60000;//tx_time_get() + tout;

        Refresh_Wdg_mock ();

        Comm_Wifi_Generate_Send_Test_Frame(pResult);

        audio_timeout = 20000;//tx_time_get() + OSTIME_20SEC;

        while((Comm_Is_Wifi_Test_Frame_Sended() == FALSE))
        {
            Refresh_Wdg_mock ();
            //tx_thread_sleep (OSTIME_50MSEC);
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }
           /* if(tx_time_get() > audio_timeout)
            {
                flag_transmission_message = TRUE;
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                audio_timeout = tx_time_get() + OSTIME_20SEC;
            }*/
        }
    }

    return true;
}

/******************************************************************************
** Name:    Execute_Send_Wifi_Exp_Frame
*****************************************************************************/
/**
** @brief   Function that executes the sending of the battery and electrodes expiration date frame
**
** @param   tout  timeout
**
** @return  none
**
******************************************************************************/
bool_t Execute_Send_Wifi_Exp_Frame_mock (DB_TEST_RESULT_t *pResult)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;
    uint32_t audio_timeout = 0;

    printf("Execute_Send_Wifi_Exp_Frame_mock\n");

    // Establish Wifi connection and check if host is available
    //if(Execute_Wifi_Init_Host_Alive() == TRUE)
    //if(Comm_Get_Wifi_Host_Alive_Flag() == TRUE)
    {
        Refresh_Wdg_mock ();

        // 1 minute timeout for sending test file
        tout =60000;// (OSTIME_60SEC);

        ref_timeout = 60000;//tx_time_get() + tout;

        Refresh_Wdg_mock ();

        Comm_Wifi_Generate_Send_Exp_Frame_mock(pResult);

        audio_timeout = 20000;//tx_time_get() + OSTIME_20SEC;

        while((Comm_Is_Wifi_Exp_Frame_Sended() == FALSE))
        {
            Refresh_Wdg_mock ();
           // tx_thread_sleep (OSTIME_50MSEC);
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }
           /* if(tx_time_get() > audio_timeout)
            {
                flag_transmission_message = TRUE;
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                audio_timeout = tx_time_get() + OSTIME_20SEC;
            }*/
        }
    }

    return true;
}

/******************************************************************************
** Name:    Execute_Send_Wifi_Exp_Frame
*****************************************************************************/
/**
** @brief   Function that executes the sending of the battery and electrodes expiration date frame
**
** @param   tout  timeout
**
** @return  none
**
******************************************************************************/
bool_t Execute_Send_Wifi_Alert_Frame_mock (DB_TEST_RESULT_t *pResult)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;
    uint32_t audio_timeout = 0;

    printf("Execute_Send_Wifi_Alert_Frame_mock\n");

    // Establish Wifi connection and check if host is available
    //if(Execute_Wifi_Init_Host_Alive() == TRUE)
    //if(Comm_Get_Wifi_Host_Alive_Flag() == TRUE)
    {
        Refresh_Wdg_mock ();

        // 1 minute timeout for sending test file
        tout =60000; //(OSTIME_60SEC);

        ref_timeout= 60500;//= tx_time_get() + tout;

        Refresh_Wdg_mock ();

        Comm_Wifi_Generate_Send_Alert_Frame_mock(MSG_ID_BATT_ELECT_EXP);

        audio_timeout = 20500;//tx_time_get() + OSTIME_20SEC;

        while((Comm_Is_Wifi_Alert_Frame_Sended() == FALSE))
        {
            Refresh_Wdg_mock ();
           // tx_thread_sleep (OSTIME_50MSEC);
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }
          /*  if(tx_time_get() > audio_timeout)
            {
                flag_transmission_message = TRUE;
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                audio_timeout = tx_time_get() + OSTIME_20SEC;
            }*/
        }
    }

    return true;
}

/******************************************************************************
** Name:    Execute_Send_Wifi_Position_Frame
*****************************************************************************/
/**
** @brief   Function that executes the sending of the gps position
**
** @param   tout  timeout
**
** @return  none
**
******************************************************************************/
bool_t Execute_Send_Wifi_Position_Frame_mock (DB_TEST_RESULT_t *pResult, SIGFOX_MSG_ID_e gps_id)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;
    uint32_t audio_timeout = 0;

    printf("Execute_Send_Wifi_Position_Frame_mock\n");

    Refresh_Wdg_mock ();

    // 1 minute timeout for sending test file
    tout = 60000;//(OSTIME_60SEC);

    ref_timeout = 60500;//tx_time_get() + tout;

    Refresh_Wdg_mock ();

    Comm_Wifi_Generate_Send_Position_Frame(gps_id);

    audio_timeout = 20500;//tx_time_get() + OSTIME_20SEC;

    while( (Comm_Is_Wifi_Position_Frame_Sended() == FALSE))
    {
        Refresh_Wdg_mock ();
        //tx_thread_sleep (OSTIME_50MSEC);
        // check if cover is closed or ON_OFF key is pressed
        if (Check_Test_Abort_User_mock(pResult))
        {
            return false;
        }
        /*if(tx_time_get() > audio_timeout)
        {
            flag_transmission_message = TRUE;
            Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
            audio_timeout = tx_time_get() + OSTIME_20SEC;
        }*/
    }

    return true;
}


/******************************************************************************
** Name:    Execute_Send_Wifi_Test_File
*****************************************************************************/
/**
** @brief   Function that executes the sending of the last performed test File
**
** @param   tout  timeout
**
** @return  none
**
******************************************************************************/
bool_t Execute_Send_Wifi_Test_File_mock (DB_TEST_RESULT_t *pResult)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;
    uint32_t audio_timeout = 0;

    printf("Execute_Send_Wifi_Test_File_mock\n");

    // Establish Wifi connection and check if host is available
    //if(Execute_Wifi_Init_Host_Alive() == TRUE)
    {
        Refresh_Wdg_mock ();

        // 5 minutes timeout for sending test file
        tout = 180000;//(OSTIME_60SEC*5);

        ref_timeout =180500;// tx_time_get() + tout;

        Refresh_Wdg_mock ();

        Comm_Wifi_Send_Test();

        Refresh_Wdg_mock ();

        audio_timeout = 20500;//tx_time_get() + OSTIME_20SEC;

        while((Comm_Is_Wifi_Test_Sended() == FALSE))
        {
            Refresh_Wdg_mock ();
            //tx_thread_sleep (OSTIME_50MSEC);
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }
          /*  if(tx_time_get() > audio_timeout)
            {
                flag_transmission_message = TRUE;
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                audio_timeout = tx_time_get() + OSTIME_20SEC;
            }*/
        }
    }
    return true;
}

/******************************************************************************
** Name:    Execute_Send_Wifi_Episode
*****************************************************************************/
/**
** @brief   Function that executes the sending of the wee episodes
**
** @param   tout  timeout
**
** @return  none
**
******************************************************************************/
bool_t Execute_Send_Wifi_Episode_mock (DB_TEST_RESULT_t *pResult)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;
    char_t  sn_str [8];         // serial number string (last 4 characters)
    uint8_t sn_len = 0;         // serial number string length
    static DEVICE_INFO_t         my_Device_Info;          // pointer to the device info
    char_t  filename [32];      // file name
    uint32_t audio_timeout = 0;

    Refresh_Wdg_mock ();

    // get the device info
    memcpy(&my_Device_Info, Get_Device_Info(), sizeof(DEVICE_INFO_t));

    sn_len = (uint8_t) strlen(my_Device_Info.sn);
    if ((sn_len >= 4) && (sn_len < RSC_NAME_SZ))
    {
        sn_str[0] = my_Device_Info.sn[sn_len - 4];
        sn_str[1] = my_Device_Info.sn[sn_len - 3];
        sn_str[2] = my_Device_Info.sn[sn_len - 2];
        sn_str[3] = my_Device_Info.sn[sn_len - 1];
    }
    else {
        sn_str[0] = sn_str[1] = sn_str[2] = sn_str[3] = '0';
    }
    sn_str[4] = 0;

    sprintf (filename, "R100_%s_0X.WEE", sn_str);

    for (uint8_t i=0; i<NUM_EPISODES; i++)
    {
        filename[11] = (char_t) (i + '0');

        if(Check_Episode_Present_mock(filename) == TRUE)
        {
            DB_Episode_Set_Current_Filename(filename);
            // In case of a DIARY Test, perform the Wifi connection just in case
            if(pResult->test_id < TEST_FULL_ID)
            {
                // Establish Wifi connection and check if host is available
                if(Execute_Wifi_Init_Host_Alive_mock(pResult) == false)
                {
                    return false;
                }
            }

            // If server connection is Alive
            if(Comm_Get_Wifi_Host_Alive_Flag() == TRUE)
            {

                Refresh_Wdg_mock ();

                // 5 minutes timeout for sending episode file
                tout = 180000;//(OSTIME_60SEC*5);

                ref_timeout = 180500;//tx_time_get() + tout;

                Refresh_Wdg_mock ();

                Comm_Wifi_Send_Episode();

                Refresh_Wdg_mock ();

                audio_timeout = 20500;// tx_time_get() + OSTIME_20SEC;

                while( (Comm_Is_Wifi_Episode_Sended() == FALSE))
                {
                    Refresh_Wdg_mock ();
                  //  tx_thread_sleep (OSTIME_50MSEC);
                    // check if cover is closed or ON_OFF key is pressed
                    if (Check_Test_Abort_User_mock(pResult))
                    {
                        return false;
                    }
                 /*   if(tx_time_get() > audio_timeout)
                    {
                        flag_transmission_message = TRUE;
                        Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                        audio_timeout = tx_time_get() + OSTIME_20SEC;
                    }*/
                }

//                if(Comm_Is_Wifi_Episode_Sended() == TRUE)
//                {
//                    DB_Episode_Delete(filename);
//                }
            }
        }
    }

    return true;
}

/******************************************************************************
** Name:    Execute_Receive_Wifi_Upgrade
*****************************************************************************/
/**
** @brief   Function that executes the receiving of the last firmware
**
** @param   tout  timeout
**
** @return  none
**
******************************************************************************/
bool_t Execute_Receive_Wifi_Upgrade_mock (DB_TEST_RESULT_t *pResult)
{
    uint32_t ref_timeout = 0;
    uint32_t audio_timeout = 0;

    printf("Execute_Receive_Wifi_Upgrade_mock\n");

    //Refresh_Wdg ();

    // Establish Wifi connection and check if host is available
//    if(Execute_Wifi_Init_Host_Alive() == TRUE)
//    {
        Refresh_Wdg_mock ();

        ref_timeout = 100000;//tx_time_get() + (OSTIME_60SEC * 15);

        Refresh_Wdg_mock ();

        Comm_Wifi_Get_Firmware();

        Refresh_Wdg_mock ();

        audio_timeout = 20500;//tx_time_get() + OSTIME_20SEC;

        while((Comm_Is_Wifi_Upgrade_Received() == FALSE))
        {
            Refresh_Wdg_mock ();
           // tx_thread_sleep (OSTIME_500MSEC);
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }
           /* if(tx_time_get() > audio_timeout)
            {
                flag_transmission_message = TRUE;
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                audio_timeout = tx_time_get() + OSTIME_20SEC;
            }*/
        }
//    }
    return true;
}


/******************************************************************************
** Name:    Execute_Receive_Wifi_Configuration
*****************************************************************************/
/**
** @brief   Function that executes the receiving of the last configuration
**
** @param   tout  timeout
**
** @return  none
**
******************************************************************************/
bool_t Execute_Receive_Wifi_Configuration_mock (DB_TEST_RESULT_t *pResult)
{
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;
    uint32_t audio_timeout = 0;

    //Refresh_Wdg ();

    // Establish Wifi connection and check if host is available
//    if(Execute_Wifi_Init_Host_Alive() == TRUE)
//    {
        Refresh_Wdg_mock ();

        tout = 120000;//(OSTIME_60SEC*5);

        ref_timeout = 120500;//tx_time_get() + tout;

        Refresh_Wdg_mock ();

        Comm_Wifi_Get_Configuration();

        Refresh_Wdg_mock ();

        audio_timeout =20500;// tx_time_get() + OSTIME_20SEC;

        while((Comm_Is_Wifi_Config_Received() == FALSE))
        {
            Refresh_Wdg_mock ();
            //tx_thread_sleep (OSTIME_50MSEC);
            // check if cover is closed or ON_OFF key is pressed
            if (Check_Test_Abort_User_mock(pResult))
            {
                return false;
            }
           /* if(tx_time_get() > audio_timeout)
            {
                flag_transmission_message = TRUE;
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);  // warn about transmission
                audio_timeout = tx_time_get() + OSTIME_20SEC;
            }*/
        }
//    }
    return true;
}

/******************************************************************************
** Name:    Execute_Save_Gps_Position
*****************************************************************************/
/**
** @brief   Function that executes the storing of the GPS position
**
** @param   none
**
** @return  none
**
******************************************************************************/
static void Execute_Save_Gps_Position_mock (void)
{
    // Resume threads needed for comms
//    tx_thread_resume (&thread_comm);

    // give some time
    Pasatiempos_mock (1000);

    // Open Gps Comm.
    Comm_GPS_Open();

    // refresh the internal watchdog timer
    Refresh_Wdg_mock ();

    while((memcmp(gps.lat_data, GPS_ZERO, 4) == 0) &&
          (memcmp(gps.long_data, GPS_ZERO, 4) == 0))
    {
        // Set GPS Get Data Command
        Comm_GPS_Get_Position();

        Pasatiempos_mock (4000);
    }

    // refresh the internal watchdog timer
    Refresh_Wdg_mock ();

    // Open Sigfox Comm.
    Comm_Sigfox_Open();

    Pasatiempos_mock (8000);

    Comm_Sigfox_Generate_Send_Position(MSG_ID_SAVED_POS_GPS);

    // refresh the internal watchdog timer
    Refresh_Wdg_mock ();

    if((memcmp(gps.lat_data, nv_data.latitude, 12) != 0) &&
       (memcmp(gps.long_data, nv_data.longitude, 12) != 0))
    {
        // save gps position to non-volatile memory
        memcpy (nv_data.latitude, gps.lat_data, 12);
        memcpy (nv_data.longitude, gps.long_data, 12);

        nv_data.lat_long_dir = 0;
        if (gps.N_S == 'N') nv_data.lat_long_dir = 0x2;
        if (gps.E_W == 'E') nv_data.lat_long_dir |= 0x1;

        // refresh the internal watchdog timer
        Refresh_Wdg_mock ();

        // update the non volatile data
        NV_Data_Write_mock(&nv_data);

        // refresh the internal watchdog timer
        Refresh_Wdg_mock ();
    }

    Pasatiempos_mock (8000);
}


/******************************************************************************
** Name:    Execute_Get_Gps_Position_Test
*****************************************************************************/
/**
** @brief   Function that executes the gps position after test
**
** @param   none
**

** @return  0 - RTC position is invalid
**          1 - RTC position is valid
**          2 - device must be powered-off
**
******************************************************************************/
uint32_t Execute_Get_Gps_Position_Test_mock (DB_TEST_RESULT_t *pResult, uint32_t tout )
{
    uint32_t ref_timeout = 0;
    uint8_t test_id = pResult->test_id;

    Refresh_Wdg_mock ();

    // Open Gps Comm.
    Comm_GPS_Open();
    Pasatiempos_mock (5000);

    // send commands to the GPS to configure it
    Comm_GPS_Send_Cmd();

    //if (GPS_Is_Test_Package_Received() == FALSE) return 0;

    ref_timeout =   tout;

   // while (tx_time_get() <= ref_timeout)
   // {
        // check if cover is closed or ON_OFF key is pressed
        if (Check_Test_Abort_User_mock(pResult))
        {
            return 2;
        }

        Refresh_Wdg_mock ();
        if((memcmp(gps.lat_data, GPS_ZERO, 4) == 0) &&
           (memcmp(gps.long_data, GPS_ZERO, 4) == 0))
        {
            // Set GPS Get Data Command
            Comm_GPS_Get_Position();
            Refresh_Wdg_mock ();
            Pasatiempos_mock (10000);
            if(test_id == TEST_MANUAL_ID)
            {
                Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);
                flag_transmission_message = TRUE;
            }

        }
        else
        {
            // a valid position has been obtained!!
//            Pasatiempos(OSTIME_20SEC);
//            Comm_GPS_Get_Position();
            return 1;
        }
   // }

    // Timeout --> no GPS position has been received
    return 0;
}

/******************************************************************************
** Name:    Execute_Periodic_Gps_Init
*****************************************************************************/
/**
** @brief   Function that executes the gps position periodically
**
** @param   none
**
** @return  none
**
******************************************************************************/
void Execute_Periodic_Gps_Init (void)
{
    if ((Comm_Get_Selected_Uart() != eMOD_GPS) && (Comm_Is_Sigfox_Free()))
    {
        Comm_GPS_Open();
        Comm_GPS_Get_Position();
    }
}

/******************************************************************************
** Name:    Execute_Periodic_Gps_Position
*****************************************************************************/
/**
** @brief   Function that executes the gps position periodically
**
** @param   none
**
** @return  none
**
******************************************************************************/
void Execute_Periodic_Gps_Position (void)
{
    if((memcmp(gps.lat_data, GPS_ZERO, 4) != 0) &&
       (memcmp(gps.long_data, GPS_ZERO, 4) != 0))
    {
        if((memcmp(gps.lat_data, nv_data.latitude, 4) != 0) &&
           (memcmp(gps.long_data, nv_data.longitude, 4) != 0))
        {
            // save gps position to non-volatile memory
            memcpy (nv_data.latitude, gps.lat_data, 4);
            memcpy (nv_data.longitude, gps.long_data, 4);

            // update the non volatile data
            NV_Data_Write_mock(&nv_data);
        }


        // In trainer mode do not send anything related to sigfox
        if (!Is_Mode_Trainer())Comm_Sigfox_Open(); // Open Sigfox Comm.

        Comm_Sigfox_Generate_Send_Position(MSG_ID_CHANGED_POS_GPS);

        first_time_gps = false;
    }

}

/*******************************************************************************************************************//**
 * @brief      Sets up system clocks in no RTC start up
 **********************************************************************************************************************/
void bsp_clock_init_Reconfigure (void)
{
    printf("Clock Init Reconfigure\n");
}

/******************************************************************************
** Name:    R100_Startup
*****************************************************************************/
/**
** @brief   Execute some startup functions:
**
**                >> maintain the power ON
**                >> identify the power-on source
**                -- if (automatic startup):
**                       >> must initialize the RTOS ... ????
**                       >> execute some tests
**                       >> wait an spare time
**                       >> program the next automatic power-on
**                       >> switch-off the device
**
**                -- if (press button or open the cover)
**                       >> normal startup. must initialize the RTOS ...
**
** @param   none
**
** @return  power-on source identifier
**
******************************************************************************/
static POWER_ON_SOURCE_e R100_Startup_mock(void)
{
    uint32_t            err_open;           // SD media open result ...
    POWER_ON_SOURCE_e   poweron_event;      // power on agent identifier
    uint8_t             my_str[16];         // generic string
    uint8_t             pic_ack;

   // UNUSED(pic_ack);
    printf("R100_Startup_mock\n");

    // initialize the internal RTC
    Rtc_Init ();

    // read the non volatile data and switch-on the ON-OFF led as soon as possible
    NV_Data_Read_mock (&nv_data);
    if (nv_data.status_led_blink == LED_BLINK_ON) Led_On_mock (LED_ONOFF);

    // refresh the watchdog timer
    Refresh_Wdg_mock ();

    // open the communication with the battery i2C devices
    // UTC value is 0 in this point
//    Battery_i2C_Init ();
    Battery_i2C_Init_RTC_mock();

    // initialize the i2C semaphore ...

  // tx_semaphore_ceiling_put (&i2c_2_semaphore, 1);
   ReleaseSemaphore(i2c_2_semaphore, 1, NULL);


    // 1.- Must identify the power-up source
    // --> Automatic power-on
    // --> User pressed button
    // --> User opens the cover
    poweron_event = Identify_PowerOn_Event_mock();

    // refresh the watchdog timer
    Refresh_Wdg_mock ();

    // initialize traces
    Trace_Init_mock ();

    // Execute automatic test ...
    if (poweron_event == PON_RTC)
    {

        // Check if firmware has been updated
        R100_Check_Update_mock(&poweron_event);

        if (poweron_event != PON_TEST)
        {

            // proceed to test the device automatically and periodically...
            // - if the battery hour is the programmed autotest hour
            if (battery_RTC.hour != nv_data.time_test)
            {
                // No test must be done
                nv_data.test_pending = true;

                // check if the device has been left with the cover opened
                R100_Check_Cover_mock();

                R100_Check_SAT_mock(nv_data.error_code, false);  // if SAT is needed, the function send an audio advice and power off

                R100_Check_Warning_mock ();

            }


            if ((battery_RTC.hour == nv_data.time_test) && nv_data.test_pending)
            {
                // Startup is nor RTC mode, so reconfigure clocks with PLL configuration
                bsp_clock_init_Reconfigure();

                // Execute some test ..diary or monthly.
                Led_On_mock (LED_ONOFF);
                nv_data.test_pending = false;

                Execute_Test_mock (&R100_test_result, true);

                if ((nv_data.test_id == TEST_FULL_ID)   &&
                    !R100_test_result.misc.boot         &&
                    (R100_test_result.battery.rem_charge > BATTERY_LOW_CHARGE))
                {
                    Load_GOLD_File_mock();
                    if (Load_FW_File_mock())
                    {
                        // Reboot bootloader processor and start update
                        Refresh_Wdg_mock ();

                        // Wait till Synergy is reseted by PIC
                        Led_Off_mock (LED_ONOFF);
                        while(1)
                        {
                            Led_Ongoing_Transmission ();
                            Pasatiempos_mock(500);
                        }
                    }

                    if (R100_test_result.misc.boot == eERR_BOOT_PROCESSOR_UPDATE)
                    {
                       nv_data.error_code = eERR_BOOT_PROCESSOR_UPDATE;
                    }
                }
                R100_PowerOff_mock();
            }

            // Re-schedule next auto-test
            R100_PowerOff_RTC_mock();
        }
    }

    // Startup is nor RTC mode, so reconfigure clocks with PLL configuration
    bsp_clock_init_Reconfigure();

    Led_On_mock (LED_ONOFF);
    nv_data.open_cover_first = true;

    Refresh_Wdg_mock ();                             // refresh the watchdog timer
    // force a Test when all keys are pressed
    if (poweron_event == PON_TEST)
    {
        // Execute some test ...
        Execute_Test_mock (&R100_test_result, false);

        // check if a new firmware is present in the uSD and update
        if (!R100_test_result.misc.boot &&
           ((R100_test_result.battery.rem_charge > BATTERY_LOW_CHARGE) || (R100_test_result.battery.rem_charge == 0)))
        {
            Load_GOLD_File_mock();
            if (Load_FW_File_mock())
            {
                // Reboot bootloader processor and start update
                Refresh_Wdg_mock ();

                // Wait till Synergy is reseted by PIC
                Led_Off_mock (LED_ONOFF);
                while(1)
                {
                    Led_Ongoing_Transmission ();
                    Pasatiempos_mock (500);
                }
            }

            if (R100_test_result.misc.boot == eERR_BOOT_PROCESSOR_UPDATE)
            {
               nv_data.error_code = eERR_BOOT_PROCESSOR_UPDATE;
               Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_UPGRADE_ERROR, TRUE);
               Pasatiempos_Listening_mock ();
            }
        }

        // Re-schedule next auto-test
        R100_PowerOff_mock ();
    }

    if (poweron_event == PON_TEST_MONTADOR)
    {
        // Execute some test ...
        Execute_Test_Montador_mock (&R100_test_result, false);

        // Re-schedule next auto-test
        R100_PowerOff_mock ();
    }

    // Power-ON by user (opening the cover OR pressing the power on button)
    // maintain the power-on switch active
    Rtc_Program_Wakeup (WAKEUP_POWERON);

    // Power on the external circuits, adding an extra time to stabilize the power supplies ...
   // g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_HIGH);
   // tx_thread_sleep (OSTIME_20MSEC);
    Sleep(20);

    // initialize the trace port and report the startup event
    if  (poweron_event == PON_BUTTON) Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Startup: Button");
    if  (poweron_event == PON_COVER)  Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Startup: Cover");
    //if  (poweron_event == PON_TEST)   Trace (TRACE_TIME_STAMP | TRACE_NEWLINE, "Startup: TEST");
    if  (poweron_event == PON_USB)    Trace_mock (TRACE_TIME_STAMP | TRACE_NEWLINE, "Startup: USB");
    nv_data.open_cover_tx_flag = 0; // reset the open cover flag (if not a RTC Startup)
    //nv_data.check_sat_tx_flag = 0; // reset the check sat flag (if not a RTC Startup)

    // Initialize the file system for SD-Card, if inserted ...
    // fx_media_init0 ();
    // resume task used for test ...



 //   tx_thread_resume (&thread_audio);
    WaitForSingleObject(hThread, INFINITE);
   // err_open = fx_media_init0_open ();
   // if (err_open != FX_SUCCESS)
  //  {
  //      Trace (TRACE_TIME_STAMP + TRACE_NEWLINE, " uSD ERROR !!! LOCK DEVICE  !!!");
  //      Lock_on_Panic ((uint8_t)err_open, 22);
  //  }

    // Add an extra time to conclude SD-Card initialization ...
  //  tx_thread_sleep (OSTIME_100MSEC);
    Sleep(20);

    Battery_i2C_Init_mock();     // NOTE: fx_media_init0_open must be done before
    Refresh_Wdg_mock ();
    // Load Device Settings from NFC (the device info is read to be available in configuration mode)
    Settings_Open_From_NFC_mock();

    Refresh_Wdg_mock ();
    // Load Device Info --> NFC data MUST be read before !!!

   /* Device_Init_mock (NULL);

    // Check if a new configuration file is present in the uSD to update the Device Settings
*/
   Settings_Open_From_uSD_mock ();


/*    if(Is_SAT_Error (nv_data.error_code) == false) nv_data.error_code = Get_NFC_Device_Info()->error_code;


    // compensate the UTC time ...
    battery_RTC.utc_time = (int8_t) Get_Device_Settings()->misc.glo_utc_time;
    RTC_Normalize_Time (&battery_RTC);

    Set_Device_Date_time_mock();
*/
    Refresh_Wdg_mock ();


    // refresh the watchdog timer
    // Load Audio resources from SD-Card
 /*   if ((err_open = Load_Audio_Resources_mock(), err_open != eERR_NONE) && (poweron_event != PON_USB))
    {
        Lock_on_Panic_mock ((uint8_t)err_open, 23);
    }
*/
    // trace the battery info !!!
 /*   Trace_mock     (TRACE_TIME_STAMP + TRACE_NEWLINE, " Battery Info !!!");
    Trace_mock     (TRACE_NO_FLAGS, "RTC (UTC 0)--> ");
    Fill_Generic_Date ((char_t *) my_str, pon_date);
    Trace_mock     (TRACE_NO_FLAGS, (char_t *) my_str);
    Trace_mock     (TRACE_NO_FLAGS, "  ");
    Fill_Generic_Time ((char_t *) my_str, pon_time);
    Trace_mock     (TRACE_NO_FLAGS, (char_t *) my_str);
    Trace_Arg_mock (TRACE_NEWLINE, "  SoC = %2d", Battery_Get_Charge_mock());
*/
    // check if the device requires SAT or not
 /*   if (Is_SAT_Error_mock (nv_data.error_code) &&
       (poweron_event != PON_USB)         &&
       (Get_Device_Info()->develop_mode != DEVELOP_ZP_CALIBRATION) &&
       (Get_Device_Info()->develop_mode != DEVELOP_DELETE_ERRORS))  // if SAT is needed, the function send an audio advice and power off
    {
        R100_Check_SAT_mock (nv_data.error_code, true);
    }

    // Check if mode trainer is enabled
    if (Get_Device_Settings()->misc.glo_trainer_mode == TRUE)
    {
        Trace_mock (TRACE_TIME_STAMP + TRACE_NEWLINE, "  MODO TRAINER !!!");
        mode_trainer = true;
    }
*/
    // Add an extra time to conclude initializations ...
   // tx_thread_sleep (OSTIME_100MSEC);
    Sleep(100);
    // read from the electrodes to identify them (in the same way, update the electrodes presence flag)

    Electrodes_Read_Data_mock (&electrodes_data);
        // initialize as open circuit
    electrodes_zp_segment = eZP_SEGMENT_OPEN_CIRC;

    // save the current settings and device info to the configuration file when power-up with USB connected
    if (poweron_event == PON_USB)
    {
        Settings_Open_From_NFC_mock();
        Settings_Save_To_uSD_mock();
    }

    // Delete errors and lock the system monitor
    if ((Get_Device_Info()->develop_mode == DEVELOP_DELETE_ERRORS) && (poweron_event != PON_USB) )
    {
        // Resume audio thread
   //     tx_thread_resume (&thread_audio);

        nv_data.error_code = eERR_NONE;

        // play ready message
        Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_DEVICE_READY, TRUE);

        // time to play audio
        Pasatiempos_Listening_mock ();

        // re-schedule next auto-test and POWER OFF!!
        R100_PowerOff_mock ();
    }


    // ZP calibration mode, lock the system monitor
    if ((Get_Device_Info()->develop_mode == DEVELOP_ZP_CALIBRATION) && (poweron_event != PON_USB) )
    {
        // Execute calibration process
        Execute_calibration_mock();

        // time to play audio
        Pasatiempos_mock(3000);

        // Save calibration values, re-schedule next auto-test and POWER OFF!!
        R100_PowerOff_mock ();
    }

    // Get USB pass
    Device_pw (access_key, (uint8_t *) Get_Device_Info()->sn, RSC_NAME_SZ);
    Refresh_Wdg_mock ();                             // refresh the watchdog timer

    if ((Get_Device_Info()->develop_mode == DEVELOP_SAVE_GPS_POSITION) && (poweron_event != PON_USB) )
    {
        // Execute gps_data obtaining process
        Execute_Save_Gps_Position_mock();

        // Save device info to NFC
        //NFC_Write_Device_Info();

        // refresh just in case
        Refresh_Wdg_mock ();

        // Save calibration values, re-schedule next auto-test and POWER OFF!!
        R100_PowerOff_mock ();
    }

    // Enable core (main) thread
 //   tx_thread_resume (&thread_core);

    // identify the event that has power-on the device
    return poweron_event;
}

/******************************************************************************
** Name:    sMon_Inc_OpenCover
*****************************************************************************/
/**
** @brief   increment the number of ticks with open cover
**
** @param   none
**
** @return  none
**
******************************************************************************/
inline void sMon_Inc_OpenCover (void)
{
    battery_statistics.nTicks_open_cover++;
}

/******************************************************************************
** Name:    sMon_Inc_TestManual
*****************************************************************************/
/**
** @brief   increment the number of IoT connections
**
** @param   none
**
** @return  none
**
******************************************************************************/
inline void sMon_Inc_TestManual (void)
{
    battery_statistics.nTestManual++;
}

/******************************************************************************
** Name:    sMon_Inc_FullCharges
*****************************************************************************/
/**
** @brief   increment the number of Vc full charges
**
** @param   none
**
** @return  none
**
******************************************************************************/
inline void sMon_Inc_FullCharges (void)
{
    battery_statistics.nFull_charges++;
}

/******************************************************************************
** Name:    Get_Date
*****************************************************************************/
/**
** @brief   report the date information
**
** @param   pYear     pointer to year
** @param   pMonth    pointer to month
** @param   pDate     pointer to date
**
** @return  none
**
******************************************************************************/
void Get_Date (uint8_t *pYear, uint8_t *pMonth, uint8_t *pDate)
{
    if (pYear)  *pYear  = battery_RTC.year;
    if (pMonth) *pMonth = battery_RTC.month;
    if (pDate)  *pDate  = battery_RTC.date;
}

/******************************************************************************
** Name:    Get_Time
*****************************************************************************/
/**
** @brief   report the time information
**
** @param   pHour    pointer to hour
** @param   pMin     pointer to minutes
** @param   pSec     pointer to seconds
**
** @return  none
**
******************************************************************************/
void Get_Time (uint8_t *pHour, uint8_t *pMin, uint8_t *pSec)
{
    if (pHour) *pHour = battery_RTC.hour;
    if (pMin)  *pMin  = battery_RTC.min;
    if (pSec)  *pSec  = battery_RTC.sec;
}

/******************************************************************************
** Name:    Get_Rem_Charge
*****************************************************************************/
/**
** @brief   report the remaining battery charge
**
** @param   none
**
** @return  remaining charge (%)
**
******************************************************************************/
uint8_t Get_Rem_Charge (void)
{
    return ((uint8_t) battery_charge);
}

/******************************************************************************
 ** Name:    Reset_Battery_Voltage
 *****************************************************************************/
/**
 ** @brief   rest counters
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void Reset_Battery_Voltage (void)
{
    nFails_lb = 0;
    nFails_rb = 0;
}
/******************************************************************************
 ** Name:    Check_Battery_Voltage
 *****************************************************************************/
/**
 ** @brief   check that the battery voltage is in range
 ** @param   my_vb          current battery voltage
 ** @param   my_temp        current temperature
 ** @param   pAudio_msg     pointer to the audio message to report
 **
 ** @return  replace battery limit only for test
 ******************************************************************************/
static uint32_t Check_Battery_Voltage (uint32_t my_vb, int8_t my_temperature, AUDIO_ID_e *pAudio_msg)
{
    #define COUNT_VOLTAGE_SAFE      5
            uint32_t    vLimit_safe;                    // limit used to compare the battery voltage
    static  uint8_t     nFails = 0;

    // assign voltage limits
    if (my_temperature >= 25)
    {
       vLimit_safe = VB_25C_SAFE_BATTERY;
    }
    else if (my_temperature <= 0)
    {
        vLimit_safe = VB_00C_SAFE_BATTERY;
    }
    else {
       vLimit_safe  = ((VB_25C_SAFE_BATTERY - VB_00C_SAFE_BATTERY) * (uint32_t) my_temperature) / 25;
       vLimit_safe += VB_00C_SAFE_BATTERY;
    }

    // check if the battery voltage value is OK ...
    nFails = (uint8_t) ((my_vb < vLimit_safe) ? (nFails + 1) : 0);
    if (my_vb <= VB_SAFE_BATTERY)
    {
        nFails = (COUNT_VOLTAGE_SAFE*2);
    }

    battery_statistics.battery_state = nFails;

    // check for consecutive failure conditions !!!
    if (nFails == COUNT_VOLTAGE_SAFE)
    {
       // parrot the audio message demanding to change the battery
       Trace_mock (TRACE_NEWLINE,"SAFE BATTERY VOLTAGE 1 !!!");
       *pAudio_msg = eAUDIO_LOW_BATTERY;
       DB_Episode_Set_Event_mock(eREG_BAT_LOW_BATTERY);
    }

    if (nFails == (COUNT_VOLTAGE_SAFE*2))
    {
       // parrot the audio message demanding to change the battery
       Trace_mock (TRACE_NEWLINE,"SAFE BATTERY VOLTAGE 2!!!");
       *pAudio_msg = eAUDIO_REPLACE_BATTERY;
       DB_Episode_Set_Event_mock(eREG_BAT_VERY_LOW_BATTERY);
       flag_power_off = true;
    }

    return vLimit_safe;

}

/******************************************************************************
** Name:    R100_Program_Autotest
*****************************************************************************/
/**
** @brief   Program the auto test to be executed at least 1 hour after the last manual power off
**
** @param   none
**
** @return  none
**
******************************************************************************/
void R100_Program_Autotest(void)
{
    uint8_t         test_hour;      // hour to program the auto test

    // read the current date & time from the RTC
    //Battery_Read_RTC (&battery_RTC);

    // reset open cover alarm
    nv_data.open_cover = pon_time;
    nv_data.time_warning = pon_time;

    // if powering off at the test hour, postpone the test 2 hours
    if (battery_RTC.hour == nv_data.time_test)
    {
        nv_data.time_test++;
        if (nv_data.time_test > 23) nv_data.time_test = 0;
        nv_data.time_test++;
        if (nv_data.time_test > 23) nv_data.time_test = 0;
    }
    else {
        // if powering off in the hour before the test hour, postpone the test 1 hour
        test_hour = (uint8_t) (nv_data.time_test ? (nv_data.time_test - 1) : 23);
        if (battery_RTC.hour == test_hour)
        {
            nv_data.time_test++;
            if (nv_data.time_test > 23) nv_data.time_test = 0;
        }
    }
}

/******************************************************************************
** Name:    R100_PowerOff
*****************************************************************************/
/**
** @brief   Power off the device programming an automatic power-on
**
** @param   none
**
** @return  none
**
******************************************************************************/
void R100_PowerOff_RTC_mock(void)
{
    bool_t finish_flag = false;

    printf("R100_PowerOff_RTC_mock\n");

   // UNUSED(finish_flag);

    // In trainer mode do not blink staus led
    if (Is_Mode_Trainer()) nv_data.status_led_blink = LED_BLINK_OFF;

/*
    // Only if device_init has done
    if (Get_Device_Info()->zp_adcs_calibration != 0)
    {
        Save_Data_Before_Off(nv_data.error_code);
    }
*/
    // update the non volatile data
    NV_Data_Write_mock(&nv_data);
    // Power off the external circuits
    //g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_LOW);

    // do not switch-off the LED --> wait to natural extinction due to absence of voltage
    // Led_Off (LED_ONOFF);


    // Program the next wake up (msecs) and wait to self destruction !!!
    Rtc_Program_Kill (LIFE_BEAT_PERIOD);

    while (1);                  // this instruction will not be executed !!!
}


/******************************************************************************
** Name:    Check_Electrodes_Type
*****************************************************************************/
/**
** @brief   check electrodes type
**
** @param   none
**
** @return  none
**
******************************************************************************/
static void Check_Electrodes_Type_mock (bool_t forze)
{
	printf("Check_Electrodes_Type_mock\n");
    // update the electrodes context (presence flag, info, zp_segment, etc...)
    Electrodes_Read_Data_mock (&electrodes_data);

    if ((electrodes_presence_flag == false) && (forze == false))
    {
        first_time = true;
        nv_data.status_led_blink = LED_BLINK_OFF;
        return;
    }

    if (((first_time == true) && (Is_Mode_Trainer() == FALSE)) || forze)
    {
        // check expiration date to advice the user (audio message) !!!!
        if ((battery_info.must_be_0xAA == 0xAA) &&
            (battery_statistics.must_be_0x55 == 0x55) &&
            (electrodes_data.info.expiration_date <= pon_date))
        {
            DB_Episode_Set_Event_mock(eREG_ECG_EL_EXPIRED);
            Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_ELECTRODES, TRUE);
            nv_data.status_led_blink = LED_BLINK_OFF;
        }
        else if (electrodes_data.event.id != 0)
        {
           DB_Episode_Set_Event_mock(eREG_ECG_EL_USED);
           Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_ELECTRODES, TRUE);
           nv_data.status_led_blink = LED_BLINK_OFF;
        }
        first_time = false;
    }
}

/******************************************************************************
** Name:    Check_Electrodes_Type_RTC
*****************************************************************************/
/**
** @brief   check electrodes type
**
** @param   none
**
** @return  none
**
******************************************************************************/
static void Check_Electrodes_Type_RTC_mock (void)
{

	printf("Check_Electrodes_Type_RTC_mock\n");
	// maintain the power-on switch active
    Rtc_Program_Wakeup (WAKEUP_POWERON);

    // Power on the external circuits, adding an extra time to stabilize the power supplies ...
   // g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_HIGH);
  //  tx_thread_sleep (OSTIME_20MSEC);

    // update the electrodes context (presence flag, info, zp_segment, etc...)
    Electrodes_Read_Data_mock (&electrodes_data);

    if (electrodes_presence_flag == false)
    {
        first_time = true;
        nv_data.status_led_blink = LED_BLINK_OFF;
        if(Is_SAT_Error_mock (nv_data.error_code) == false)
        {
            nv_data.error_code = eERR_ELECTRODE_NOT_CONNECTED;
        }
        return;
    }

    if (Is_Mode_Trainer() == FALSE)
    {
        // check expiration date to advice the user (audio message) !!!!
        if ((battery_info.must_be_0xAA == 0xAA) &&
            (battery_statistics.must_be_0x55 == 0x55) &&
            (electrodes_data.info.expiration_date <= pon_date))
        {
            nv_data.status_led_blink = LED_BLINK_OFF;
            if(Is_SAT_Error_mock (nv_data.error_code) == false)
            {
                nv_data.error_code = eERR_ELECTRODE_EXPIRED;
            }
        }
        else if (electrodes_data.event.id != 0)
        {
            nv_data.status_led_blink = LED_BLINK_OFF;
            if(Is_SAT_Error_mock (nv_data.error_code) == false)
            {
                nv_data.error_code = eERR_ELECTRODE_USED;
            }
        }
    }
}
/******************************************************************************
 ** Name:    Check_Device_Led_Status
 *****************************************************************************/
/**
 ** @brief   determine if status led must be on
  **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
void Check_Device_Led_Status (void)
{
    uint8_t     pin_state;          // Cover pin state
    uint32_t    batt_charge;

    // If the led status is On....skip
    // If the led status is Off... check if can be on
    if (Is_SAT_Error_mock (nv_data.error_code) == true) return;

    // Check Battery status
    batt_charge = Battery_Get_Charge_mock();
    if (batt_charge <= BATTERY_REPLACE_CHARGE)
    {
            nv_data.status_led_blink = LED_BLINK_OFF;
            nv_data.error_code = eERR_BATTERY_REPLACE;
            return;
    }

    if (batt_charge <= BATTERY_LOW_CHARGE)
    {
        nv_data.status_led_blink = LED_BLINK_OFF;
        nv_data.error_code = eERR_BATTERY_LOW;
        return;
    }

   // g_ioport.p_api->pinRead (KEY_COVER, &pin_state);
    if (pin_state)
    {
        nv_data.status_led_blink = LED_BLINK_OFF;
        nv_data.prev_cover_status = true;          // cover open
        return;
    }

    // Check Electrodes Type
    nv_data.status_led_blink = LED_BLINK_ON;
    nv_data.error_code = eERR_NONE;
    nv_data.prev_cover_status = false;          // cover close
    Check_Electrodes_Type_RTC_mock();
}

/******************************************************************************
 ** Name:    Check_dead_man
 *****************************************************************************/
/**
 ** @brief   check if the dead-man effect is triggered to automatic power off
 **          the device. Use timeout of 3 minutes
 **          Assume that the function is called every 500 msecs. aprox.
 ** @param   void
 **
 ** @return  void
 ******************************************************************************/
static void Check_dead_man (void)
{
    static  uint32_t        time_without_patient;   // count the time without a patient connected
            ZP_SEGMENT_e    my_segment;             // ZP segment

    // read and process the patient connection
    my_segment = patMon_Get_Zp_Segment();
    if (my_segment == eZP_SEGMENT_GOOD_CONN)
    {
        time_without_patient = 0;
        return;
    }

    // other combinations can be analyzed ...
    // eZP_SEGMENT_UNKNOWN
    // eZP_SEGMENT_SHORT_CIRC
    // eZP_SEGMENT_BAD_CONN
    // eZP_SEGMENT_OPEN_CIRC

    time_without_patient++;
    if (time_without_patient >= (10 * 60 * 2))
    {
        // Send event to poweroff and close/delete episode
        Send_Core_Queue_mock (eEV_KEY_ONOFF);

        // reprogram the auto-test depending on the manual power off
        //R100_Program_Autotest();

        //Check device status
        //Check_Device_Led_Status();

        // this function ends the execution !!!
        //R100_PowerOff();
    }
}
/******************************************************************************
 ** Name:    thread_sysMon_entry
 *****************************************************************************/
/**
 ** @brief   startup to run the application and system monitoring
 ** @param   void
 **
 ** @return  void
 ******************************************************************************/
void thread_sysMon_entry_mock(void)
{
    uint32_t    time_to_audio;                  // time to parrot some messages ...
    uint32_t    time_to_audio_low_batt;         // time to parrot LOW_BATT messages ...
    uint32_t    time_to_statistic;              // time to count statistics ...
    uint32_t    time_to_trace;                  // time to send traces ...
    uint32_t    time_to_gps;                    // time to update gps ...
    uint32_t    time_to_gps_init;               // time to init gps ...
    uint32_t    time_to_gps_cmd;                // time to send gps commands
    uint32_t    my_zp_ohms;                     // patient impedance in ohms
    uint32_t    my_zp_adcs;                     // patient impedance in adcs
    uint32_t    my_zp_cal;                      // patient impedance calibration in adcs
    uint32_t    my_vb, my_v18;                          // battery voltage
    uint32_t    my_time;                        // local tick counter
    //ssp_err_t   ssp_error;                      // ssp error code
    uint8_t     my_str[16];                     // generic string
    uint8_t     usb_power;                      // usb bus voltage detection
    AUDIO_ID_e  message_to_play;                // audio message to play
    DEFIB_STATE_e       my_def_state;           // defibrillator state
    POWER_ON_SOURCE_e   poweron_event;          // power on event identifier
    //uint8_t     tx_event;                       // event to send to the core
    bool_t      register_SAT_condition = true;  // call sat flag
   // wdt_cfg_t   my_cfg;                         // used to check the watchdog configuration
    uint8_t     cnt_replace_batt_sat = 0;       // Counter to play message three times if there is a SAT error when it is running
    uint8_t     cnt_replace_batt = 0;           // Counter to play message three times if need to replace the battery when it is running
    uint8_t     cnt_replace_batt_init = 0;      // Counter to play message three times if need to replace the battery when init the R100

  //  UNUSED(ssp_error);
    printf("thread_sysMon_entry_mock\n");

    // Execute ASAP --> initializes the internal watchdog timer
    // initialized with 4,3 secs. of timeout
    // the WDT is configured as autostart. This function is used
    // with the debugger
   // ssp_error = iWDT.p_api->open(iWDT.p_ctrl, iWDT.p_cfg);
  //  ssp_error = iWDT.p_api->cfgGet(iWDT.p_ctrl, &my_cfg);
    Refresh_Wdg_mock ();

    flag_sysmon_task = false;

    // Initialize the R100 and identify the power-up event
    poweron_event = R100_Startup_mock ();

    // restart the audio to play as a parrot message
    message_to_play = (AUDIO_ID_e) NULL;

    // from USB, lock the system monitor
    if (poweron_event == PON_USB)
    {
        // check if the error code requires SAT
        if (Is_SAT_Error_mock (nv_data.error_code))
        {
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
            Pasatiempos_Listening_mock ();
        }

        // do nothing ...
    /*    while (1)
        {
            // wait for a loop time
    //        tx_thread_sleep (OSTIME_500MSEC);

            // refresh the internal watchdog timer
            if (thread_supervisor != 0)
            {
                Refresh_Wdg_mock ();
                thread_supervisor = 0;
            }

            // power off if USB cable is disconnected
    //        g_ioport.p_api->pinRead (VBUS_DET, &usb_power);

            if (!usb_power)
            {
                memset (my_str, 0, sizeof(my_str));
                memcpy (my_str, access_key, 8);
                strcat ((char_t *) my_str, ".dat");

                // remove the access key (just in case)
    //            fx_file_delete(&sd_fx_media, (char_t *) my_str);
    //            fx_media_flush (&sd_fx_media);

                // close fx_media before power down
    //            fx_media_close(&sd_fx_media);
    //            tx_thread_sleep (OSTIME_200MSEC);

                // reprogram the auto-test depending on the manual power off
                R100_Program_Autotest();

                // this function ends the execution !!!
                R100_PowerOff_mock();
            }
        }*/
    }

    // wait some time until Defib_Get_Vbatt read voltage
   // tx_thread_sleep (OSTIME_100MSEC);
    Sleep(100);
    //BATERIA
    /*while(cnt_replace_batt_init < 3)
    {
        // Check battery voltage and if is it low, terminate thread desfibrillator
        if(Battery_Get_Charge_mock () <= BATTERY_REPLACE_CHARGE)
        {
            cnt_replace_batt_init++;
            Set_NV_Data_Error_IF_NOT_SAT (eERR_BATTERY_REPLACE);

            // play the audio message
            Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_REPLACE_BATTERY, TRUE);
            Pasatiempos_Listening_mock ();

            if(Battery_Get_Charge_mock () == 0)      // battery is not identified
            {
                break;
            }

          //  tx_thread_sleep (OSTIME_500MSEC);

            if(cnt_replace_batt == 1)
            {
                // Do not detect more Cover close event
                tx_thread_terminate (&thread_drd);
                tx_thread_terminate (&thread_hmi);
                tx_thread_terminate (&thread_patMon);
                // proceed to discharge the main capacitor (just in case)
                Defib_Charge_mock(0, 0);
              //  tx_thread_sleep (OSTIME_100MSEC);
                // suspend the remaining threads
              //  tx_thread_terminate (&thread_defibrillator);
              //  tx_thread_terminate (&thread_recorder);
              //  tx_thread_terminate (&thread_comm);

            }

            if(cnt_replace_batt_init >= 3)
            {
                // Force to write into battery statistic Rem charge=1
//                battery_statistics.nFull_charges = 1000;
//                Battery_Write_Statistics (&battery_statistics);

            //    tx_thread_terminate (&thread_audio);

                // close fx_media before power down
             //   fx_media_close(&sd_fx_media);
             //   tx_thread_sleep (OSTIME_200MSEC);
                // reprogram the auto-test depending on the manual power off
                R100_Program_Autotest();

                // this function ends the execution !!!
                R100_PowerOff_mock();
            }
        }
        else
        {
            cnt_replace_batt_init = 3;
        }
    }*/

    // synchronize the Boot processor (fine tuning of the serial port)
    Refresh_Wdg_mock ();                             // refresh the watchdog timer
    Boot_Sync_mock (1);                              // Synchronize the Boot processor
    Refresh_Wdg_mock ();                             // refresh the watchdog timer

    // execute the startup test
    Execute_Startup_Test_mock(&R100_test_result, TEST_INITIAL_ID);
    Refresh_Wdg_mock ();

    // initialize some timers
    //my_time                 = tx_time_get();
    time_to_statistic       = my_time + 30000;    // time to count statistics ...
    time_to_audio           = my_time;                   // time to parrot some messages ...
    time_to_audio_low_batt  = my_time;
    time_to_trace           = my_time;                   // time to send traces to serial port ...
	time_to_gps_init        = my_time + 10000;    // time to update gps ...
	time_to_gps             = my_time + 30000;    // time to update gps ...
    time_to_gps_cmd         = my_time + 60000;    // time to update gps ...

	// Check if TEST MODE is enabled
	if (Is_Mode_Trainer())
	{
	    //Play a message to inform the user
	    Audio_Message (eAUDIO_CMD_PLAY, eAUDIO_TRAINER_MODE, TRUE);
	}

	flag_sysmon_task = true;    // task initialized

    // system monitor loop
    /*while (1)
    {
        // wait for a loop time
       // tx_thread_sleep (500);
       // my_time = tx_time_get();

        // refresh the internal watchdog timer
        if (thread_supervisor != 0)
        {
            Refresh_Wdg_mock ();
            thread_supervisor = 0;
        }

        if(Get_Device_Info()->develop_mode != DEVELOP_MANUFACTURE_CONTROL)
        {
            // check the Dead-Man condition
            Check_dead_man();
        }

        Check_Electrodes_Type_mock (false);

        electrodes_zp_segment = patMon_Get_Zp_Segment ();
        if ((electrodes_pending_event) && (Is_Mode_Trainer()== false))
        {
            // increment the registered number of shocks ...
            electrodes_data.event.id ++;

            // read the battery current date & time from the battery pack
           // ssp_error = Battery_Read_RTC (&battery_RTC);

            // register the power on date
            electrodes_data.event.date  = (uint32_t) (battery_RTC.year + 2000) << 16;
            electrodes_data.event.date += (uint32_t)  battery_RTC.month <<  8;
            electrodes_data.event.date += (uint32_t)  battery_RTC.date;

            // register the power on time
            electrodes_data.event.time  = (uint32_t) battery_RTC.hour << 16;
            electrodes_data.event.time += (uint32_t) battery_RTC.min  <<  8;
            electrodes_data.event.time += (uint32_t) battery_RTC.sec;

            // set the checksum
            electrodes_data.event.checksum_xor = electrodes_data.event.id ^ electrodes_data.event.date ^ electrodes_data.event.time;

            // register the event in the electrodes
            Electrodes_Write_Event (&electrodes_data.event);

            {
                // trace some variables ...
                Trace_mock     (TRACE_TIME_STAMP + TRACE_NEWLINE, " PATIENT CONNECTED !!!");
                Trace_mock     (TRACE_NO_FLAGS, "Registered Events --> ");
                Fill_Generic_Date ((char_t *) my_str, electrodes_data.event.date);
                Trace_mock     (TRACE_NO_FLAGS, (char_t *) my_str);
                Trace_mock     (TRACE_NO_FLAGS, "  ");
                Fill_Generic_Time ((char_t *) my_str, electrodes_data.event.time);
                Trace_mock     (TRACE_NO_FLAGS, (char_t *) my_str);
                Trace_Arg_mock (TRACE_NEWLINE, "  PatConn = %2d", electrodes_data.event.id);
            }
            electrodes_pending_event = false;
            write_elec_last_event = true;
        }

        // read some local values
   /*     my_zp_ohms = patMon_Get_Zp_Ohms_mock();
        my_zp_adcs = patMon_Get_Zp_ADCs();
        my_zp_cal  = patMon_Get_zp_CAL_ADCs();

        //my_temperature = patMon_Get_Temperature();
        //Battery_Read_Temperature(&battery_temperature);

        my_def_state = Defib_Get_State ();

        // check defibrillator and battery voltage values
        if (my_def_state == eDEFIB_STATE_CHARGING)
        {
            my_v18 = Defib_Get_Vdefib();
//            Trace_Arg (TRACE_NO_FLAGS, "  ** 18V value = %5d", my_v18);
            if ((my_v18 < SMON_18V_MIN) && ((nv_data.error_code == eERR_NONE) || (Is_SAT_Error_mock (nv_data.error_code) == false))) { R100_test_result.voltage.dc_18v_error = nv_data.error_code = eERR_SMON_18V_TOO_LOW;  }
            if ((my_v18 > SMON_18V_MAX) && ((nv_data.error_code == eERR_NONE) || (Is_SAT_Error_mock (nv_data.error_code) == false))) { R100_test_result.voltage.dc_18v_error = nv_data.error_code = eERR_SMON_18V_TOO_HIGH; }
        }

        /////////////////////////////////////////////////////////////////////////////////////
        // check the battery voltage ONLY when is not charging the main capacitor
        my_vb = Defib_Get_Vbatt();

        if ((my_def_state == eDEFIB_STATE_CHARGING) ||
            (my_def_state == eDEFIB_STATE_SHOCKING))
        {
            Reset_Battery_Voltage();
//            my_vb = 0;
//            Trace_Arg (TRACE_NO_FLAGS, "  ** my_def_state = %2d", my_def_state);
        }
        else
        {
            Check_Battery_Voltage (my_vb, battery_temperature, &message_to_play);
            if ((my_vb < SMON_DCMAIN_MIN)  && ((nv_data.error_code == eERR_NONE) || (Is_SAT_Error_mock (nv_data.error_code) == false))) { R100_test_result.voltage.dc_main_error = nv_data.error_code = eERR_SMON_DCMAIN_TOO_LOW;  }
            if ((my_vb > SMON_DCMAIN_MAX)  && ((nv_data.error_code == eERR_NONE) || (Is_SAT_Error_mock (nv_data.error_code) == false))) { R100_test_result.voltage.dc_main_error = nv_data.error_code = eERR_SMON_DCMAIN_TOO_HIGH; }
        }


        if (my_def_state == eDEFIB_STATE_IN_ERROR)
        {
            // register the error code
            if ((nv_data.error_code == eERR_NONE) ||
                (Is_SAT_Error_mock (nv_data.error_code) == false)) R100_test_result.error_code = nv_data.error_code = Defib_Get_ErrorId();
        }

        /////////////////////////////////////////////////////////////////////////////////////
        // with some critical errors, must call the SAT service
        if (Is_SAT_Error_mock (nv_data.error_code))
        {
            // parrot the audio message demanding SAT service
            message_to_play = eAUDIO_CALL_SAT;

            // If SAT error thread core is suspended and defibrillator disarmed
            thread_supervisor |= THREAD_ID_sMON;

            // just in case, lock the core task and set the defibrillator out of service ...
            Send_Core_Queue_mock (eEV_LOCK_CORE);

            Defib_Set_Out_of_Service();

            // the first entry in SAT-Error MUST execute next operations ....
            //   .- register the event in the episode (this event closes the episode)
            //   .- register in the test result file
            if (register_SAT_condition)
            {
                // set as event registered !!!
                register_SAT_condition = false;

                // register sat error in episodes
                DB_Episode_Set_Event_mock(eREG_SAT_ERROR);

                // register the sat error in the test log file
                R100_test_result.test_id = TEST_FUNC_ID;
                Execute_Test_Electrodes_mock (&R100_test_result.electrodes, true, false);
                Execute_Test_Battery_mock    (&R100_test_result.battery, true, false);
                DB_Test_Generate_Report_mock (&R100_test_result, true);

                if (Is_Mode_Trainer() == false)
                {
                    // Open Sigfox Comm.
                    Comm_Sigfox_Open();
                    // Generate and Send Test report using Sigfox
                    Comm_Sigfox_Generate_Send_Test(&R100_test_result);
                }
            }
        }

        // send some debug traces ...
        if (my_time >= time_to_trace)
        {
            time_to_trace = my_time + 1000;
            if(Get_Device_Info()->develop_mode != DEVELOP_SAVE_GPS_POSITION)
            {
                // trace some variables ...
                Trace_mock (TRACE_NEWLINE, "  ");
                Trace_mock (TRACE_TIME_STAMP, "  ");
                if (my_vb)
                {
                    Trace_Arg_mock (TRACE_NO_FLAGS, "  ** Vb (mV) = %5d", my_vb);
                    my_vb = 0;
                }
                else
                {
                    Trace_mock (TRACE_NO_FLAGS, "  ** Vb (mV) = -----");
                }
                Trace_Arg_mock (TRACE_NO_FLAGS, "  SoC = %2d", Battery_Get_Charge_mock());
    //            Trace_Arg (TRACE_NO_FLAGS, "  ** Zp_adc = %7d", my_zp_adcs);
                Trace_Arg_mock (TRACE_NO_FLAGS, " Zp_ohms = %4d", my_zp_ohms);
                Trace_Arg_mock (TRACE_NO_FLAGS, " # %7d", my_zp_cal);
                Trace_Arg_mock (TRACE_NO_FLAGS, "  ** Vc (V) = %4d", Defib_Get_Vc());
    //            Trace_Arg (TRACE_NO_FLAGS, "  ** FRCP (bpm) = %4d", FRCP_Get_Frequency());
                Trace_Arg_mock (TRACE_NO_FLAGS,  "  ** Tbatt = %2d", (uint32_t) battery_temperature);
    //            Trace_Arg (TRACE_NO_FLAGS,  "  ** Tpcb = %2d", (uint32_t) patMon_Get_Temperature());
            }

            // power off if USB cable is connected
          //  g_ioport.p_api->pinRead (VBUS_DET, &usb_power);

            if (usb_power)
            {
                // jlock the core task anf power off
                Send_Core_Queue_mock (eEV_USB);
                while(1);
            }

        }

        // update the battery statistics ...
        if (my_time >= time_to_statistic)
        {
            time_to_statistic = my_time + 60000;

            battery_charge = Battery_Get_Charge_mock ();

            if (battery_charge && (battery_charge <= BATTERY_REPLACE_CHARGE))
            {
                message_to_play = eAUDIO_REPLACE_BATTERY;
                Trace_mock (TRACE_NEWLINE,"REPLACE BATTERY PERCENT!!!");
                flag_power_off = true;
            }

            else if (battery_charge && (battery_charge <= BATTERY_LOW_CHARGE))
            {
                message_to_play = eAUDIO_LOW_BATTERY;
                Trace_mock (TRACE_NEWLINE,"LOW BATTERY PERCENT!!!");
            }

            if (battery_charge == 0)
            {
                message_to_play = eAUDIO_REPLACE_BATTERY;
            }

            // check if the battery monitor is present
            if (battery_statistics.must_be_0x55 == 0x55)
            {
                // update the statistics in the battery (time count in this task !!!)
                battery_statistics.runTime_total++;
                if (patMon_Get_Zp_Segment () == eZP_SEGMENT_GOOD_CONN)
                {
//                    battery_statistics.runTime_patient = 0;
                }

                // register the statistics ...
               /* if (Battery_Write_Statistics (&battery_statistics) != SSP_SUCCESS)
                {
                 //   tx_thread_sleep (OSTIME_10MSEC);
                    // try to write again
                    if (Battery_Write_Statistics (&battery_statistics) != SSP_SUCCESS)
                    {
                        Trace (TRACE_NEWLINE,"ERROR WRITING STATISTIC!!!");
                        message_to_play = eAUDIO_REPLACE_BATTERY;
                        battery_statistics.must_be_0x55 = 0;
                    }
                }
            }

            // Check battery temperature
           // Battery_Read_Temperature(&battery_temperature);

            if (battery_temperature < BATTERY_LOW_TEMP)
            {
                Set_NV_Data_Error_IF_NOT_SAT (eERR_BATTERY_TEMP_OUT_RANGE);
                message_to_play = eAUDIO_REPLACE_BATTERY;
                DB_Episode_Set_Event_mock(eREG_BAT_LOW_TEMPERATURE);
            }
            else if (battery_temperature > BATTERY_HIGH_TEMP)
            {
                Set_NV_Data_Error_IF_NOT_SAT (eERR_BATTERY_TEMP_OUT_RANGE);
                message_to_play = eAUDIO_REPLACE_BATTERY;
                DB_Episode_Set_Event_mock(eREG_BAT_HIGH_TEMPERATURE);
            }

            if (battery_temperature < BATTERY_VERY_LOW_TEMP)
            {
                Set_NV_Data_Error_IF_NOT_SAT (eERR_BATTERY_TEMP_OUT_RANGE_OFF);
                DB_Episode_Set_Event_mock(eREG_BAT_VERY_LOW_TEMPERATURE);
            }
            else if (battery_temperature > BATTERY_VERY_HIGH_TEMP)
            {
                Set_NV_Data_Error_IF_NOT_SAT (eERR_BATTERY_TEMP_OUT_RANGE_OFF);
                DB_Episode_Set_Event_mock(eREG_BAT_VERY_HIGH_TEMPERATURE);
            }

            // use this timeout to re-Synchronize the Boot processor
            Boot_Sync_mock (1);
        }

        // parrot audio messages ...
        if ((my_time >= time_to_audio) &&
            (message_to_play != eAUDIO_LOW_BATTERY) &&
            (message_to_play))
        {
            time_to_audio = my_time + 10000;

            // play the audio message
            Audio_Message (((message_to_play == eAUDIO_CALL_SAT || (message_to_play == eAUDIO_REPLACE_BATTERY && flag_power_off == false))?eAUDIO_CMD_PLAY:eAUDIO_CMD_CONCAT), message_to_play, TRUE);
            if (message_to_play)
            {
                if ((message_to_play == eAUDIO_REPLACE_BATTERY) && (flag_power_off == true))
                {
                    // Do not detect more Cover close event
                /*   tx_thread_terminate (&thread_drd);
                    tx_thread_terminate (&thread_hmi);
                    tx_thread_terminate (&thread_patMon);
                    // proceed to discharge the main capacitor (just in case)
                    Defib_Charge_mock(0, 0);
              /*      tx_thread_sleep (OSTIME_100MSEC);
                    // suspend the remaining threads
                    tx_thread_terminate (&thread_defibrillator);
                    tx_thread_terminate (&thread_recorder);
                    tx_thread_terminate (&thread_comm);

                    Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_BATTERY, FALSE);
                    Audio_Message (eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_BATTERY, FALSE);
                    Pasatiempos_Listening_mock ();

                   /* tx_thread_terminate (&thread_audio);
                    // close fx_media before power down
                    fx_media_close(&sd_fx_media);
                    tx_thread_sleep (OSTIME_200MSEC);

                    // reprogram the auto-test depending on the manual power off
                    R100_Program_Autotest();

                    // this function ends the execution !!!
                    R100_PowerOff_mock();
                }
                else if(message_to_play == eAUDIO_CALL_SAT)
                {
                    cnt_replace_batt_sat++;
                    if(cnt_replace_batt_sat >=3)
                    {
                        Pasatiempos_Listening_mock ();

                        // close fx_media before power down
                      //  fx_media_close(&sd_fx_media);
                      //  tx_thread_sleep (OSTIME_200MSEC);

                        // reprogram the auto-test depending on the manual power off
                        R100_Program_Autotest();

                        // this function ends the execution !!!
                        R100_PowerOff_mock();
                    }
                }
                else
                {
                    cnt_replace_batt_sat = 0;
                }

                if (message_to_play != eAUDIO_CALL_SAT) message_to_play = (AUDIO_ID_e) NULL;
            }
        }

        // parrot audio messages ...
        if (my_time >= time_to_audio_low_batt)
        {
            if (message_to_play == eAUDIO_LOW_BATTERY)
            {
                time_to_audio_low_batt = my_time + 60000;
                // play the audio message
                Audio_Message (eAUDIO_CMD_CONCAT, message_to_play, TRUE);
                message_to_play = (AUDIO_ID_e) NULL;
            }
        }

        // update the gps position ...
        // Only send once with patient connected
        if ((my_time >= time_to_gps_init)   &&
//bug4688            (electrodes_zp_segment == eZP_SEGMENT_GOOD_CONN) &&
            (first_time_gps)   &&
            (Is_Mode_Trainer() == FALSE))
        {
            time_to_gps_init = my_time + 30000;
            Execute_Periodic_Gps_Init();
        }

        if ((my_time >= time_to_gps)   &&
            (first_time_gps)   &&
            (Is_Mode_Trainer() == FALSE))
        {
           /* if (Gps_Is_New_Position_Available() == TRUE)
            {
                Comm_GPS_Get_Position();
            }
            Execute_Periodic_Gps_Position();
            time_to_gps = my_time + 10000;
        }

        // send commands to the GPS to configure it
       /* if(GPS_Is_Running() == true)
        {
            // Wait a lite time to stabilize GPS before send command
            time_to_gps_cmd = my_time + 2000;
            GPS_Set_Running(FALSE);
        }
        if(my_time >= time_to_gps_cmd && (first_time_gps) && (Is_Mode_Trainer() == FALSE) && Comm_Is_GPS_Cmd_Send() != eERR_NONE)
        {
            Comm_GPS_Send_Cmd();

            time_to_gps = my_time + 10000;
            time_to_gps_cmd = time_to_gps + 15000;
        }
    }*/
}

/******************************************************************************
** Name:    R100_PowerOff
*****************************************************************************/
/**
** @brief   Power off the device programming an automatic power-on
**
** @param   none
**
** @return  none
**
******************************************************************************/
void R100_PowerOff_mock(void)
{
    bool_t finish_flag = false;
    char_t aux[4];

    printf("R100_PowerOff_mock\n");

   // UNUSED(finish_flag);

    // In trainer mode do not blink staus led
    if (Is_Mode_Trainer()) nv_data.status_led_blink = LED_BLINK_OFF;

    // check if the battery monitor is present
    if (battery_statistics.must_be_0x55 == 0x55)
    {
        // Write statistic in the battery
       // Battery_Write_Statistics (&battery_statistics);
    }


    // Only if device_init has done
    if (Get_Device_Info()->zp_adcs_calibration != 0)
    {
        Check_Electrodes_Type_RTC_mock();
        Save_Data_Before_Off(nv_data.error_code);
    }

    // update the non volatile data
    NV_Data_Write_mock(&nv_data);
    // Power off the external circuits
   // g_ioport.p_api->pinWrite (IO_PSU_EN, IOPORT_LEVEL_LOW);

    // do not switch-off the LED --> wait to natural extinction due to absence of voltage
    // Led_Off (LED_ONOFF);

    if (Is_Test_Mode_Montador())
    {
    //    while (1)
    //    {
            Pasatiempos_mock (100);
     /*       if (nv_data.error_code)
            {
                Comm_Uart_Send_mock ("FAI");
                aux[0] = (char_t)  ((nv_data.error_code / 100) + '0');
                aux[1] = (char_t) (((nv_data.error_code % 100)/10) + '0');
                aux[2] = (char_t)  ((nv_data.error_code %  10) + '0');
                aux[3] = 0;
                Comm_Uart_Send_mock (aux);
            }
            else
            {
                Comm_Uart_Send_mock ("PAS");
            }*/
      //  }

        Trace_mock ((TRACE_NEWLINE),"");
        Trace_mock ((TRACE_NEWLINE),"********SWITCH OFF THE FIXTURE************");
        //while (1)
        //{
            Refresh_Wdg_mock ();
        //}
    }

    // Program the next wake up (msecs) and wait to self destruction !!!
    Rtc_Program_Kill (LIFE_BEAT_PERIOD);

    //while (1);                  // this instruction will not be executed !!!
}
