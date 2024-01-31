/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        DB_Test.h
 * @brief       Test File header file
 *
 * @version     v1
 * @date        20/07/2020
 * @author      lsanz
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef DB_TEST_H     // Entry, use file only if it's not already included.
#define DB_TEST_H

/******************************************************************************
**Includes
*/
#include "device_init.h"
#include "types_basic_Mock.h"




/******************************************************************************
** Defines
*/

#define NUM_BASIC_TEST      30                          // number of basic tests to perform a full test
#define TEST_FULL_ID        (NUM_BASIC_TEST - 1)        // identifier of the full test
#define TEST_MANUAL_ID      40                          // identifier of the manual test
#define TEST_INITIAL_ID     50                          // identifier of the initial test
#define TEST_FUNC_ID        60                          // identifier of the functional test



/******************************************************************************
** typedefs
*/



/// Structure to report the voltages test result
typedef struct
{
    uint16_t    dc_main;                ///< voltage in the DC_MAIN (mVolts)
    uint8_t     dc_main_error;          ///< error identifier
    uint16_t    dc_18v;                 ///< voltage in the DC_18V (mVolts)
    uint8_t     dc_18v_error;           ///< error identifier
} TEST_VOLTAGE_t;


/// Structure to report the battery test result
typedef struct
{
    char_t      sn  [20];               ///< Battery serial number -- format AAAA/21100001
    uint8_t     name[20];               ///< company name -- (default: BexenCardio)
    uint32_t    manufacture_date;       ///< Manufacture date (YYYY.MM.DD)
    uint32_t    expiration_date;        ///< Expiration  date (YYYY.MM.DD)
    uint32_t    nominal_capacity;       ///< Battery nominal capacity (default: 4200 mAh)
    uint16_t    version_major;          ///< Battery version (major)
    uint16_t    version_minor;          ///< Battery version (minor)
    uint32_t    runTime_total;          ///< accumulated run time (in minutes
    uint32_t    battery_state;          ///< battery state (0-Full, 1-Low battery, 2-replace)
    uint16_t    nFull_charges;          ///< number of full charges
    uint16_t    rem_charge;             ///< remaining charge percent
    int16_t     bat_temperature;        ///< battery temperature
    uint32_t    error_code;             ///< battery error code
} TEST_BATTERY_t;

/// Structure to report the electrodes test result
typedef struct
{
    uint64_t    sn;                     ///< electrode serial number
    uint32_t    expiration_date;        ///< electrode expiration date (YYYY.MM.DD)
    uint32_t    event_date;             ///< electrode event date (YYYY.MM.DD)
    uint32_t    event_time;             ///< electrode event time
    uint32_t    event_id;               ///< electrode event id bitset
    uint32_t    error_code;             ///< electrode error code
} TEST_ELECTRODES_t;

/// Structure to report the patient monitor test result
typedef struct
{
    uint32_t    ADS_cal;                ///< ADS ZP calibration resistance value
    uint32_t    ADS_pat;                ///< ADS ZP patient resistance value
    uint32_t    ADS_comms;              ///< ADS SPI comms available or not
    int32_t     ADS_temperature;        ///< internal temperature (�C)
} TEST_PATMON_t;

/// Structure to report the external comms test result
typedef struct
{
    uint32_t    wifi;                   ///< WiFi module comms check
    uint32_t    sigfox;                 ///< Sigfox module comms check
    uint32_t    gps;                    ///< GPS module comms check
    uint32_t    accelerometer;          ///< accelerometer check
} TEST_COMMS_t;

/// Structure to report the miscellaneous test result
typedef struct
{
    uint32_t    nfc;                    ///< NFC comms available or not
    uint32_t    boot;                   ///< Boot processor comms available or not
    uint32_t    audio;                  ///< audio signal check
} TEST_MISC_t;

/// Structure to report the defibrillator test result
typedef struct
{
    uint16_t    full_charging_time;                 ///< capacitor charging time in milliseconds
    uint16_t    full_charging_voltage;              ///< voltage of main capacitor after charging
    uint32_t    full_discharg_H_current;            ///< current in the H-Bridge
    uint16_t    full_discharg_H_time;               ///< discharging time of H bridge discharge
    uint16_t    full_discharg_H_voltage;            ///< voltage after H bridge discharge
    uint16_t    full_discharg_R_time;               ///< discharging time of R discharge
    uint16_t    full_discharg_R_voltage;            ///< voltage after R discharge
    uint32_t    error_code;                         ///< defibrillation circuit error code
} TEST_DEFIB_t;

/// Enum for test result status
typedef enum
{
    TEST_OK = 0,                            ///< Test with no critical error
    TEST_NOT_OK,                            ///< Test with critical error
    TEST_ABORTED                            ///< Test aborted
} TEST_STATUS_t;

// Structure to report the certificates load test result
typedef struct
{
    uint16_t tls_cert_exp;                       ///< TLS certificate check
    uint16_t wpa_eap_cert_exp;                   ///< WPA_EAP certificate check
    uint16_t tls_cert_load;                      ///< TLS certificate no loaded
    uint16_t wpa_eap_cert_load;                  ///< WPA_EAP certificate no loaded
}TEST_CERT_t;

/// structure to hold data before writing to file in multiples of cluster size
typedef struct
{
    char_t              device_sn[RSC_NAME_SZ];     ///< device serial number
    uint8_t             test_id;                    ///< test identifier (used internally to identify the test number)
    TEST_STATUS_t       test_status;                ///< test OK, NOT OK or ABORT
    uint32_t            error_code;                 ///< error code

    TEST_VOLTAGE_t      voltage;                    ///< voltages test results
    TEST_MISC_t         misc;                       ///< miscellaneous test results
    TEST_PATMON_t       patmon;                     ///< patient monitor test results
    TEST_ELECTRODES_t   electrodes;                 ///< electrodes test results
    TEST_BATTERY_t      battery;                    ///< battery test results
    TEST_COMMS_t        comms;                      ///< comms test results


    // Full test related variables ...
    char_t              gps_position [64];          ///< GPS position
    TEST_DEFIB_t        defib;                      ///< defibrillator test results

    TEST_CERT_t         certificate;                ///< Certificate test results

} DB_TEST_RESULT_t;


/******************************************************************************
** Globals
*/

extern DB_TEST_RESULT_t    R100_test_result;           // test result !!!



/******************************************************************************
** prototypes
*/

extern  void    Fill_Generic_Date (char_t *pStr, uint32_t my_date);
extern  void    Fill_Generic_Time (char_t *pStr, uint32_t my_time);
extern void Fill_Test_Date (char *pStr);

extern void Fill_Test_Time (char *pStr);

extern  bool_t  DB_Test_Generate_Report_mock (DB_TEST_RESULT_t *pResult, bool_t file_report);
extern  char_t* DB_Test_Get_Filename (void);
extern  void    DB_Sigfox_Store_Info_mock (char_t * device_id, char_t * pac_id);

extern  void    PowerOff_Write_Elec_mock(void);

#endif  /*DB_TEST_H*/
