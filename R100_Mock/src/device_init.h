/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        device.h
 * @brief       Header file that manages the settings of the system
 *
 * @version     v1
 * @date        04/09/2018 
 * @author      ltorres
 *
 */

#ifndef DEVICE_INIT_H_
#define DEVICE_INIT_H_

/******************************************************************************
 ** Includes
 */
#include "types_basic_Mock.h"

/******************************************************************************
 ** Defines
 */
#define RSC_NAME_SZ                 (16)        ///< Size for generic names used in resources

// Application revision code
#define APP_REV_CODE                0x30323038  // 02.08
                                                // MM.nn
                                                // MM -> Synergy mayor version
                                                // mm -> Synergy minor version
#define NFC_REV_CODE                0x30323030  // 02.00
                                                // MM.nn
                                                // MM -> NFC mayor version
                                                // mm -> NFC minor version

#define     FNAME_VERSION           "version.info"
#define     UPGRADE_FILENAME        "firmware.srec"     ///< upgrade filename
#define     GOLDEN_FILENAME         "golden.srec"       ///< golden filename
#define     CFG_FILENAME            "R100.cfg"          // configuration filename
#define     USB_FILENAME            "USB.cfg"           // configuration filename
#define     WIFI_FILENAME           "wifi.dat"          // wifi configuration filename
#define     FNAME_DATE              "fw_date.info"      // firmware update date
#define     UPDATE_REG              "update_reg.log"   // update registers log

///< device options bitset
#define ENABLE_OPT_ACCEL            0x08        ///< enables the Accel
#define ENABLE_OPT_WIFI             0x04        ///< enables the Wifi
#define ENABLE_OPT_SIGFOX           0x02        ///< enables the Sigfox
#define ENABLE_OPT_GPS              0x01        ///< enables the GPS

#define NFC_BYTES_PER_PAGE          16          // number of bytes in a single page
#define NFC_LOCKING_BLOCK            0          // 1st block to store the locking bytes
#define NFC_DEVICE_ID_BLOCK          1          // 1st block to store the device identifier
#define NFC_DEVICE_INFO_BLOCK        4          // 1st block to store the device info
#define NFC_DEVICE_SETTINGS_BLOCK   11          // 1st block to store the device settings
#define NFC_DEVICE_COMM_BLOCK       15          // 1st block to store the device communication settings
#define NFC_DEVICE_COMM_KEY         24          // 1st block to store the device communication key

#define MAX_RCP_TIME                180         // maximum RCP time (in seconds)
#define MAX_ENERGY_A                200         // maximum energy for patient adult (joules)
#define MAX_ENERGY_P                90          // maximum energy for patient pediatric (joules)
#define MAX_CONSEC_SHOCKS           4           // maximum number of consecutive shocks
#define MAX_ASYSTOLE_TIME           60          // maximum asystole time (in minutes)

#define MIN_ENERGY_A                150         // minimum energy for patient adult (joules)
#define MIN_ENERGY_P                50          // minimum energy for patient pediatric (joules)

#define MAX_AUD_VOLUME              8           // maximum audio volume setting

/******************************************************************************
 ** Typedefs
 */

typedef enum {
    DEVELOP_DISABLE = 0,                        //< development operations disabled
    DEVELOP_TEST_2500_eSHOCK,                   //< test a 2500 shocks series over an external resistor
    DEVELOP_TEST_500_iSHOCK,                    //< test a  500 shocks series over an internal resistor
    DEVELOP_FORCE_SHOCKABLE,                    //< force a shockable rythm
    DEVELOP_TEST_RELAY,                         //< test the security relay
    DEVELOP_ZP_CALIBRATION,                     //< DEVELOP_ZP_CALIBRATION
    DEVELOP_SAVE_GPS_POSITION,                  //< DEVELOP_SAVE_GPS_POSITION
    DEVELOP_MANUFACTURE_CONTROL,                //< Special SW for Osatu production
    DEVELOP_DELETE_ERRORS                       //< Special SW for Osatu production, delete ther error in internal memory
} DEVELOP_OPERATIONS_t;

/// Device Date
typedef struct
{
    uint8_t     year;          ///< year  (starting from 2000)
    uint8_t     month;         ///< month (1:12)
    uint8_t     date;          ///< date  (1:31)
} DEVICE_DATE_t;

/// Device Time
typedef struct
{
    uint8_t     hour;          ///< hour (0:23)
    uint8_t     min;           ///< minutes (0:59)
    uint8_t     sec;           ///< seconds (0:59)
} DEVICE_TIME_t;

/// Device Information
typedef struct
{
    char_t          model            [RSC_NAME_SZ];     ///< Device model
    char_t          sn               [RSC_NAME_SZ];     ///< Device serial number
    char_t          manufacture_date [RSC_NAME_SZ];     ///< Manufacturing date

    union
    {
        uint16_t    device_options;                     ///< bitset for device options
        struct
        {
            uint16_t gps            :1;                 ///< GPS
            uint16_t sigfox         :1;                 ///< Sigfox platform
            uint16_t wifi           :1;                 ///< Wifi remote access
            uint16_t accelerometer  :1;                 ///< Accelerometer for wake-up
            uint16_t reserved       :3;                 ///< reserved for future use
            uint16_t aed_fully_auto :1;                 ///< Fully auto AED
        } enable_b; /*!< BitSize */
    };

    uint32_t        zp_adcs_short;                      ///< zp with electrodes in short-circuit (in ADCs)
    uint32_t        zp_adcs_calibration;                ///< zp input connected to calibration (in ADCs)
    uint16_t        develop_mode;                       ///< development mode
    uint8_t         align_2;
    uint8_t         checksum;                           ///< Checksum of data integrity
} DEVICE_INFO_t;

/// AED settings
typedef struct
{
    // Configuration - AED
    uint16_t    aed_aCPR_init;              ///< adult -> Initial CPR time in seconds (can be 0)
    uint16_t    aed_aCPR_1;                 ///< adult -> After Shock Series CPR time in seconds (can be OFF)
    uint16_t    aed_aCPR_2;                 ///< adult -> No Shock Advised CPR time in seconds (can be OFF)
    uint16_t    aed_pCPR_init;              ///< pediatric -> Initial CPR time in seconds (can be 0)
    uint16_t    aed_pCPR_1;                 ///< pediatric -> After Shock Series CPR time in seconds (can be OFF)
    uint16_t    aed_pCPR_2;                 ///< pediatric -> No Shock Advised CPR time in seconds (can be OFF)
    uint16_t    aed_cpr_msg_long;           ///< Long/Short audio messages for CPR (0-Short; 1-Long)
    uint16_t    aed_analysis_cpr;           ///< Enables start an analysis cycle in CPR (0-No; 1-Yes)
    uint16_t    aed_asys_detect;            ///< Enables Asystole warning message (0-No; 1-Yes)
    uint16_t    aed_asys_time;              ///< Time on asystole before emitting asystole warning message (minutes)
    uint16_t    aed_metronome_en;           ///< Enables metronome for marking compression pace (0-No; 1-Yes)
    uint16_t    aed_feedback_en;            ///< Enables cpr feedback for marking compression pace (0-No; 1-Yes)
    uint16_t    aed_consec_shocks;          ///< Max Shocks in a Shock Series (1:4)
    uint16_t    aed_energy_shock1_a;        ///< Energy for 1st shock in shock series for adult (150J, 175J, 200J)
    uint16_t    aed_energy_shock2_a;        ///< Energy for 2nd shock in shock series for adult (150J, 175J, 200J)
    uint16_t    aed_energy_shock3_a;        ///< Energy for 3rd shock in shock series for adult (150J, 175J, 200J)
    uint16_t    aed_metronome_ratio_a;      ///< Metronome pace for adult (0-15:2; 1-30:2; 2-Only compressions)
    uint16_t    aed_energy_shock1_p;        ///< Energy for 1st shock in shock series for pediatric (50J, 65J, 75J, 90J)
    uint16_t    aed_energy_shock2_p;        ///< Energy for 2nd shock in shock series for pediatric (50J, 65J, 75J, 90J)
    uint16_t    aed_energy_shock3_p;        ///< Energy for 3rd shock in shock series for pediatric (50J, 65J, 75J, 90J)
    uint16_t    aed_metronome_ratio_p;      ///< Metronome pace for pediatric (0-15:2; 1-30:2; 2-Only compressions)
    uint16_t    aed_metronome_rate;         ///< Metronome rate in compressions per minute (100; 110; 120;)
} AED_SETTINGS_t;

/// Miscellaneous settings
typedef struct
{
    uint16_t    glo_filter;                 ///< Type of filter
    uint8_t     glo_auto_test_hour;         ///< Auto test hour (hours)
    uint8_t     glo_auto_test_min;          ///< Auto test hour (minutes)
    uint16_t    glo_patient_adult;          ///< Patient type (0-pediatric 1-adult)
    uint16_t    glo_language;               ///< audio language
    uint16_t    glo_volume;                 ///< audio volume from a 8 level scale (0 to 8)
     int16_t    glo_utc_time;               ///< UTC time
    uint16_t    glo_ecg_format;             ///< ECG format standard / cabrera //?????
    uint16_t    glo_trainer_mode;			///< send alerts (0-disabled; 1-enabled)
    uint16_t    glo_patient_alert;          ///< send alerts (0-disabled; 1-enabled)
    uint16_t    glo_movement_alert;         ///< send alerts (0-disabled; 1-enabled)
    uint16_t    glo_transmis_mode;          ///< send alerts (0-disabled; 1-enabled)
    uint16_t    glo_warning_alert;          ///< send alerts (0-disabled; 1-enabled)
} MISC_SETTINGS_t;

/// Communication settings
typedef struct
{
    uint16_t    wpa_eap_enabled;            ///< Use of WPA-Enterprise protocol (0:No; 1: Yes)
    uint8_t     wlan_ssid [32];             ///< Wifi WLAN SSID (Net name) MAX: 32 characters
    uint8_t     wlan_pass [32];             ///< Wifi WLAN Passkey MAX: 32 characters
} COMM_SETTINGS_t;

/// Device settings
typedef struct
{
    AED_SETTINGS_t  aed;                    ///< AED settings
    uint8_t         align_1[2];             ///< data align
    MISC_SETTINGS_t misc;                   ///< Miscellaneous  settings
    COMM_SETTINGS_t comm;                   ///< Communications settings (NOT USED!!!!! ONLY FOR ALIGNEMENT PURPOSES)
    uint8_t         align_2;                ///< data align
    uint8_t         checksum;               ///< Checksum of data integrity
} DEVICE_SETTINGS_t;

/// Device settings
typedef struct
{
    uint16_t    aCPR_init;                  ///< adult --> Initial CPR time in seconds (can be 0)
    uint16_t    aCPR_1;                     ///< adult --> After Shock Series CPR time in seconds (can be OFF)
    uint16_t    aCPR_2;                     ///< adult --> No Shock Advised CPR time in seconds (can be OFF)
    uint16_t    pCPR_init;                  ///< pediatric --> Initial CPR time in seconds (can be 0)
    uint16_t    pCPR_1;                     ///< pediatric --> After Shock Series CPR time in seconds (can be OFF)
    uint16_t    pCPR_2;                     ///< pediatric --> No Shock Advised CPR time in seconds (can be OFF)
    uint16_t    cpr_msg_long;               ///< Long/Short audio messages for CPR
    uint16_t    analysis_cpr;               ///< Enables start an analysis cycle in CPR

    uint16_t    asys_detect;                ///< Enables Asystole warning message
    uint16_t    asys_time;                  ///< Time on asystole before emitting asystole warning message
    uint16_t    metronome_en;               ///< Enables metronome for marking compression pace
    uint16_t    metronome_ratio_a;          ///< Metronome pace for adult
    uint16_t    metronome_ratio_p;          ///< Metronome pace for pediatric
    uint16_t    metronome_rate;             ///< Metronome rate for both pediatric and adult
    uint16_t    feedback_en;                ///< Enables cpr feedback for marking compression pace
    uint16_t    consec_shocks;              ///< Max Shocks in a Shock Series

    uint16_t    energy_shock1_a;            ///< Energy for 1st shock in shock series for adult
    uint16_t    energy_shock2_a;            ///< Energy for 2nd shock in shock series for adult
    uint16_t    energy_shock3_a;            ///< Energy for 3rd shock in shock series for adult
    uint16_t    energy_shock1_p;            ///< Energy for 1st shock in shock series for pediatric
    uint16_t    energy_shock2_p;            ///< Energy for 2nd shock in shock series for pediatric
    uint16_t    energy_shock3_p;            ///< Energy for 3rd shock in shock series for pediatric
    uint16_t    patient_type;               ///< Patient type (0-pediatric 1-adult)
    uint16_t    mains_filter;               ///< Mains Filter (0-none, 1-50 Hz, 2-60 Hz)

    uint16_t    language;                   ///< language to use in audio messages
     int16_t    utc_time;                   ///< UTC time (-11 to +12)
    uint16_t    audio_volume;               ///< Audio volume (0:8)
    uint16_t    trainer_mode;               ///< trainer_mode (0-disabled; 1-enabled)
    uint16_t    patient_alert;              ///< send a alert in patient mode (0-disabled; 1-enabled)
    uint16_t    movement_alert;             ///< send a alert in movement detection (0-disabled; 1-enabled)
    uint16_t    transmis_mode;              ///< transmission mode (0-none;1-Sigfox;2-Wifi;3-Both)
    uint16_t    warning_alert;              ///< emit an alert if warning is detected (0-disabled; 1-enabled)
} NFC_SETTINGS_t;

/// Device communication credentials
typedef struct
{
    uint16_t    wpa_eap_enabled;            ///< Use of WPA-Enterprise protocol (0:No; 1: Yes)
    uint16_t    reserved_2 [7];             ///< ***** reserved for future use
    uint8_t     wlan_ssid [32];             ///< Wifi WLAN SSID (Net name) MAX: 32 characters
    uint8_t     wlan_pass [32];             ///< Wifi WLAN Passkey MAX: 32 characters
    uint8_t     wifi_dat_key[16];           ///< Wifi data validation key
} NFC_DEVICE_COM_t;

/// Device info
typedef struct
{
    uint32_t    structure_version;                      ///< structure version code
    uint16_t    develop_mode;                           ///< (0 - Max Uint16_t)
    uint16_t    device_options;                         ///< bitset for device options (b7: full_auto b2:Wifi; b1:SIGFOX; b0:GPS)
    uint32_t    zp_adcs_short;                          ///< zp with electrodes in short-circuit (in ADCs)
    uint32_t    zp_adcs_calibration;                    ///< zp input connected to calibration (in ADCs)
    uint8_t     device_sn               [RSC_NAME_SZ];  ///< device serial number
    uint8_t     device_manufacture_date [RSC_NAME_SZ];  ///< device manufacture date
} NFC_DEVICE_ID_t;

/// Peripheral info
typedef struct
{
    uint32_t    free1;                                  ///< free1
    uint32_t    free2;                                  ///< free2
    uint32_t    internal_memory_cap;                    ///< internal memory capacity  (in MBytes)
    uint32_t    internal_memory_rem;                    ///< remaining internal memory (in MBytes)

    uint32_t    electrodes_sn1;                         ///< electrodes serial number 1 (from 1-Wire memory address)
    uint32_t    electrodes_sn2;                         ///< electrodes serial number 2 (from 1-Wire memory address)
    uint32_t    electrodes_manufacture_date;            ///< electrodes manufacture date (YYYY.MM.DD)
    uint32_t    electrodes_expiration_date;             ///< electrodes expiration  date (YYYY.MM.DD)
    uint32_t    electrodes_event_date;                  ///< last event date (YYYY.MM.DD)
    uint32_t    electrodes_event_time;                  ///< last event time (hh:mm:ss)
    uint32_t    electrodes_event_id;                    ///< last event identifier
    uint32_t    reserved_electrodes;                    ///< ***** reserved for future use

    uint8_t     battery_sn [RSC_NAME_SZ];               ///< battery serial number
    uint32_t    battery_manufacture_date;               ///< Battery manufacture date (YYYY.MM.DD)
    uint32_t    battery_expiration_date;                ///< Battery expiration  date (YYYY.MM.DD)
    uint16_t    battery_cap;                            ///< Battery nominal capacity (mAh)
    uint16_t    battery_rem;                            ///< Battery remaining capacity (mAh)
    uint32_t    reserved_battery;                       ///< ***** reserved for future use

    uint32_t    app_sw_version;                         ///< device SW TAG version
    uint16_t    error_code;                             ///< last recorded error code
    uint16_t    compiler_date;                          ///< compiler date(MM.DD)
    uint16_t    compiler_time;                          ///< compiler time(HH.MM)
    //uint16_t    pcb_hw_version;                         ///< decive PCB SW verion
    uint16_t    reserved_0[3];                          ///< ***** reserved for future use


} NFC_DEVICE_INFO_t;

/// Device info
typedef struct
{
    NFC_DEVICE_ID_t     dev_id;
    NFC_DEVICE_INFO_t   dev_info;
} NFC_MAIN_INFO_t;


/******************************************************************************
 ** Globals
 */
extern NFC_DEVICE_ID_t write_nfc_device_id; // Global struct for comparing NFC reading and writing
extern NFC_DEVICE_ID_t read_nfc_device_id;  // Global struct for comparing NFC reading and writing


/******************************************************************************
 ** Prototypes
 */

extern  void                Device_Init_mock     (char_t *serial_num);   ///< device initialization

// Device Info
extern  NFC_DEVICE_INFO_t*  Get_NFC_Device_Info (void);
extern  NFC_DEVICE_ID_t*    Get_NFC_Device_ID (void);
extern  DEVICE_INFO_t*      Get_Device_Info (void);
extern  DEVICE_DATE_t*      Get_Device_Date (void);
extern  DEVICE_TIME_t*      Get_Device_Time (void);
extern  void                Set_Device_Date_time_mock   (void);
extern  void                Get_APP_SW_Version  (char_t* sw_version);

// Device Settings
extern  void                Settings_Open_From_uSD_mock (void);      ///< open the default settings from the uSD
extern  void                Settings_Save_To_uSD_mock   (void);      ///< open the default settings from the uSD
extern  void                Settings_Open_From_NFC (void);      ///< open the default settings from the NFC
extern  AED_SETTINGS_t*     Get_AED_Settings       (void);      ///< Get AED Settings handler
extern  MISC_SETTINGS_t*    Get_Misc_Settings      (void);      ///< Get Miscellaneous Settings handler
extern  DEVICE_SETTINGS_t*  Get_Device_Settings    (void);      ///< Get Device Settings handler
extern  void                NFC_Write_Settings_mock     (void);      ///< Write the device settings
extern  void                NFC_Write_Device_ID_mock    (bool_t flag_write, bool_t flag_cal_write);      ///< Write the device identifier
extern  void                NFC_Write_Device_Info_mock   (bool_t flag_write); ///< Write the device & accessories info
extern  void                Wifi_Save_To_uSD_mock        (void);      ///< save wifi settings to uSD

extern  void                Get_Compile_Date_Time   (void);
#endif /* DEVICE_INIT_H_ */

/*
 ** $Log$
 **
 ** end of device.h
 ******************************************************************************/
