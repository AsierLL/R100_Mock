/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_sysMon_entry.h
 * @brief       Header with functions related to the system monitor service
 *
 * @version     v1
 * @date        06/06/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef THREAD_SYSMON_ENTRY_H_
#define THREAD_SYSMON_ENTRY_H_

/******************************************************************************
 **Includes
 */
#include "DB_Test_Mock.h"
#include "HAL/thread_comm_hal_Mock.h"
#include "HAL/thread_patMon_hal_Mock.h"
#include "R100_Errors.h"
#include "nx_secure_Mock.h"

#include "types_basic_Mock.h"


/******************************************************************************
 ** defines
 */
#define THREAD_ID_sMON       0x00000001
#define THREAD_ID_CORE       0x00000002
#define THREAD_ID_COMM       0x00000004
#define THREAD_ID_DRD        0x00000008
#define THREAD_ID_HMI        0x00000010
#define THREAD_ID_DEFIB      0x00000020
#define THREAD_ID_AUDIO      0x00000040
#define THREAD_ID_PATMON     0x00000080
#define THREAD_ID_RECORDER   0x00000100

//#define LIFE_BEAT_PERIOD     3000         // life beat period in msecs.
#define LIFE_BEAT_PERIOD     10000          // life beat period in msecs.
#define WAKEUP_POWERON       10             // force the GPIO to ON to maintain the power


#define UTC_MIN             -12
#define UTC_MAX              14

#define     ADS_CAL_MIN                 5501000        ///< max ADS calibration value for an internal resistor 6K04 ohms 5501
#define     ADS_CAL_MAX                 5596000        ///< min ADS calibration value for an internal resistor 6K04 ohms 5596
#define     ADS_ZP_0_MIN                4025000        ///< max ADS 0 Ohms value for an external resistor
#define     ADS_ZP_0_MAX                4056000        ///< min ADS 0 Ohms value for an external resistor

/******************************************************************************
 ** typedefs
 */

/// Patmon test result structure
typedef struct
{
 //   uint32_t    ZP;             ///< internal impedance (ohms)
    ERROR_ID_e  sts;            ///< test result
} TEST_RESULT_ECGMON_t;

/// Data collected during the defibrillation shock
typedef struct
{
    uint32_t    vc;             ///< voltage in the main capacitor               (volts)
    uint32_t    time;           ///< time in curse or programed maximun time     (usecs)
    uint32_t    energy;         ///< energy computed or programed maximun energy (embedded Units)
} DEFIB_PULSE_DATA_t;

/// Summary of defibrillation pulse data for the phase-1
typedef struct
{
    uint32_t            shock_energy;   ///< energy to discharge on the patient (Jules)
    uint32_t            shock_zp;       ///< patient impedance                  (ohms)
    uint32_t            shock_alarms;   ///< alarms detected during the shock

    DEFIB_PULSE_DATA_t  start_p0;       ///< data programed at p0 start (pulse start)
    DEFIB_PULSE_DATA_t  start_p1;       ///< data programed at p1 start
    DEFIB_PULSE_DATA_t  end_p0;         ///< data collected at p0 end   (4 msecs)
    DEFIB_PULSE_DATA_t  end_p1;         ///< data collected at p1 end   (phase-1 end)
    DEFIB_PULSE_DATA_t  end_p2;         ///< data collected at p2 end   (phase-2 end)
} DEFIB_PULSE_BLACK_BOX_t;

/// Summary of defibrillation pulse data for the phase-1
typedef struct
{
    DEFIB_PULSE_BLACK_BOX_t pulse_data; ///< Pulse data
    ERROR_ID_e              sts;        ///< Test result
} TEST_RESULT_DEFIB_t;


/// Battery info (stored in the battery pack)
typedef struct {
    uint8_t     sn  [16];               ///< Battery serial number -- format AAAA/21100001
    uint8_t     name[16];               ///< company name -- (default: BEXENCARDIO)
    uint32_t    manufacture_date;       ///< Manufacture date (YYYY.MM.DD)
    uint32_t    expiration_date;        ///< Expiration  date (YYYY.MM.DD)
    uint32_t    nominal_capacity;       ///< Battery nominal capacity (default: 4200 mAh)
    uint8_t     version;                ///< Battery version
    uint8_t     revision_code;          ///< Revision code
    uint8_t     must_be_0xAA;           ///< Force to 0xAA
    uint8_t     checksum_add;           ///< data checksum
} BATTERY_INFO_t;

/// Battery statistics (stored in the battery pack)
typedef struct {
    uint32_t    runTime_total;          ///< accumulated run time (in minutes)
    uint32_t    battery_state;          ///< battery state depend on the safe voltage
    uint16_t    nFull_charges;          ///< number of full charges
    uint16_t    nTicks_open_cover;      ///< number of ticks with open cover
    uint16_t    nTestManual;            ///< number of manual tests
    uint8_t     must_be_0x55;           ///< reserved (fixed to 0x55)
    uint8_t     checksum_xor;           ///< data checksum
} BATTERY_STATISTICS_t;

/// Electrodes info header (stored in the electrodes internal memory)
typedef struct {
    uint8_t     lot_number[8];          ///< Lot number
    uint32_t    manufacture_date;       ///< Manufacture date (YYYY.MM.DD)
    uint32_t    expiration_date;        ///< Expiration  date (YYYY.MM.DD)
    uint8_t     name[12];               ///< Company name -- (default: BexenCardio)
    uint8_t     version;                ///< Version code / electrode type
    uint8_t     revision_code;          ///< Revision code
    uint8_t     reserved[1];            ///< Reserved
    uint8_t     checksum_add;           ///< Data checksum
} ELECTRODES_INFO_t;

/// Electrodes event (stored in the electrodes internal memory)
typedef struct {
    uint32_t    date;                   ///< formatted date
    uint32_t    time;                   ///< formatted time
    uint32_t    id;                     ///< event identifier (number of shocks)
    uint32_t    checksum_xor;           ///< data checksum
} ELECTRODES_EVENT_t;

/// Electrodes info structure
typedef struct {
    uint64_t            sn;             ///< electrodes serial number -- 1Wire memory address -- 0 if no electrodes are connected
    ELECTRODES_INFO_t   info;           ///< electrodes information header
    ELECTRODES_EVENT_t  event;          ///< electrodes event register
} ELECTRODES_DATA_t;

/// non volatile data (implemented in the internal data flash)
typedef struct {
    uint32_t    time_warning;           ///< formatted time (HH:MM:SS) to generate a warning
    uint8_t     time_test;              ///< time to execute the test (hour identifier)
    ERROR_ID_e  error_code;             ///< active error code (only codes that requires SAT)
    uint8_t     test_id;                ///< test identifier (test executed in the last session)
    bool_t      test_pending;           ///< flag to indicate that a test is pending to be executed
    char        latitude[12];           ///< device gps latitude
    char        longitude[12];          ///< device gps longitude
    uint8_t     lat_long_dir;           ///< bit1 N or S;bit0 E or W
    uint32_t    open_cover;             ///< counter to detect open cover state during extended periods
    uint8_t     open_cover_tx_flag;     ///< open cover transmission flag
    bool_t      open_cover_first;       ///< open cover first flag
    bool_t      prev_cover_status;      ///< previous cover status
    //uint8_t     check_sat_tx_flag;      ///< check sat transmission flag
    uint8_t     update_flag;            ///< update firmware flag
    uint8_t     status_led_blink;       ///< force the blink of the status LED (do not blink with expired electrodes or battery)
    NFC_SETTINGS_t  default_settings;   ///< default settings
    uint8_t     must_be_0x55;           ///< reserved (fixed to 0x55)
    uint8_t     checksum_xor;           ///< data checksum
} NV_DATA_t;


/******************************************************************************
 ** globals
 */

extern  uint32_t    thread_supervisor;          // thread supervisor
extern  bool_t      write_elec_last_event;      // write las electrodes registered event in test

/******************************************************************************
 ** prototypes
 */

extern void         R100_Program_Autotest(void);
extern void         R100_PowerOff        (void);
extern void         R100_PowerOff_RTC    (void);

extern void         sMon_Inc_OpenCover (void);
extern void         sMon_Inc_TestManual (void);
extern void         sMon_Inc_FullCharges (void);

extern void         Battery_Get_Info (BATTERY_INFO_t* pData);
extern void         Battery_Get_Statistics (BATTERY_STATISTICS_t* pData);
extern uint16_t     Battery_Get_Charge (void);
extern int8_t       Battery_Get_Temperature (void);

extern EL_SIGN_e    Electrodes_Get_Signature  (void);
extern void         Electrodes_Get_Data       (ELECTRODES_DATA_t* pData);
extern void         Electrodes_Register_Event (void);
extern void         Electrodes_Register_Shock (void);

extern void         Get_Date (uint8_t *pYear, uint8_t *pMonth, uint8_t *pDate);
extern void         Get_Time (uint8_t *pHour, uint8_t *pMin,   uint8_t *pSec);
extern uint8_t      Get_Rem_Charge(void);

extern NV_DATA_t*   Get_NV_Data  (void);
extern void         Set_NV_Data_Error_IF_NOT_SAT (ERROR_ID_e error);
extern bool_t       Is_Test_Mode (void);
extern bool_t       Is_Test_Mode_Montador (void);
extern bool_t       Is_Mode_Trainer (void);
extern void         Set_Mode_Trainer (bool_t aux);

extern void         Device_pw    (uint8_t *pPassword, uint8_t *pDevice_sn, uint32_t size);
extern uint32_t     Execute_Get_Gps_Position_Test_mock (DB_TEST_RESULT_t *pResult, uint32_t tout);
extern void         Execute_Periodic_Gps_Init     (void);
extern void         Execute_Periodic_Gps_Position (void);
extern bool_t       Execute_Send_Sigfox_Test_mock (DB_TEST_RESULT_t *pResult, bool_t autotest);
extern bool_t       Execute_Send_Sigfox_Exp_mock (DB_TEST_RESULT_t *pResult, bool_t autotest);
extern bool_t       Execute_Wifi_Init_Host_Alive_mock (DB_TEST_RESULT_t *pResult);
extern bool_t       Execute_Send_Wifi_Test_Frame_mock(DB_TEST_RESULT_t *pResult);
extern bool_t       Execute_Send_Wifi_Exp_Frame(DB_TEST_RESULT_t *pResult);
extern bool_t       Execute_Send_Wifi_Alert_Frame(DB_TEST_RESULT_t *pResult);
extern bool_t       Execute_Send_Wifi_Position_Frame_mock (DB_TEST_RESULT_t *pResult, SIGFOX_MSG_ID_e gps_id);
extern bool_t       Execute_Send_Wifi_Test_File_mock (DB_TEST_RESULT_t *pResult);
extern bool_t       Execute_Send_Wifi_Episode_mock (DB_TEST_RESULT_t *pResult);
extern bool_t       Execute_Receive_Wifi_Upgrade_mock (DB_TEST_RESULT_t *pResult);
extern bool_t       Execute_Receive_Wifi_Configuration_mock (DB_TEST_RESULT_t *pResult);
extern bool_t       Is_Sysmon_Task_Initialized (void);
extern void         NV_Data_Write (NV_DATA_t *pNV_data);
extern void         NV_Data_Read (NV_DATA_t *pNV_data);

extern void         Lock_on_Panic(uint8_t error, uint8_t num);
extern void         Check_Device_Led_Status (void);
extern POWER_ON_SOURCE_e Identify_PowerOn_Event(void);

extern uint8_t      Get_RTC_Month (void);

void                bsp_clock_init_Reconfigure          (void);
extern bool_t Is_SAT_Error_mock (ERROR_ID_e dev_error);


#endif /* THREAD_SYSMON_ENTRY_H_ */

/*
 ** $Log$
 **
 ** end of thread_sysMon_entry.h
 ******************************************************************************/
