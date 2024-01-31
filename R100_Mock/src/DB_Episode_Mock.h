/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        DB_Episode.h
 * @brief       Episode header file
 *
 * @version     v1
 * @date        20/06/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

#ifndef DB_EPISODE_H     // Entry, use file only if it's not already included.
#define DB_EPISODE_H

/******************************************************************************
**Includes
*/
#include "device_init.h"
#include "DRD/drd_Mock.h"
#include "types_basic_Mock.h"
#include "event_ids.h"
#include "thread_sysMon_entry_Mock.h"


/******************************************************************************
** Defines
*/
#define EPISODE_MAJOR_VERSION       0x01
#define EPISODE_MINOR_VERSION       0x00

#define EPISODE_BLOCK_SZ            (1024)      ///< episode block total size
#define EPISODE_BLOCK_DATA_SZ       (1008)      ///< episode block's data size
#define EPISODE_BLOCK_ECG_SZ        (400)       ///< number of ECG samples in a single block
#define EPISODE_BLOCK_ZP_SZ         (40)        ///< number of ZP  samples in a single block
#define EPISODE_BLOCK_HEADER_SZ     (12)          ///< size of the header in a single episode block

#define EPISODE_BLOCK_ZP_ADD        (EPISODE_BLOCK_HEADER_SZ + (EPISODE_BLOCK_ECG_SZ*2/5))
#define EPISODE_BLOCK_DATA_AUX_ADD  (EPISODE_BLOCK_ZP_ADD + 32)

#define NUM_EPISODES                (11)        ///< maximum number of episodes in a single uSD card
#define NUM_BLOCKS_EPISODE          (200000)    ///< maximum number of blocks in a single episode

/******************************************************************************
** typedefs
*/

/// Episode status
typedef enum
{
    eEPISODE_OK = 0,                            ///< fine, spare space
    eEPISODE_GETTING_FULL,                      ///< running out of space
    eEPISODE_FULL                               ///< out of space
} EPISODE_STS_e;

/// Definition of time structure
typedef struct {
    uint8_t        sec;         ///< seconds [00..59]
    uint8_t        min;         ///< minutes [00..59]
    uint8_t        hour;        ///< hours   [00..23]
} TIME_t;

typedef struct {
    uint8_t        date;        ///< date  [01..31]
    uint8_t        month;       ///< month [01..12]
    uint8_t        year;        ///< year from 2000 [00..99]
} DATE_t;

/// block types (for auxiliary data)
typedef enum
{
    eBLOCK_DATA_DEFAULT = 0,                    ///< default data
    eBLOCK_DATA_DRD     = 1                     ///< DRD result
} BLOCK_DATA_e;

/// ECG format
typedef enum {
    eECG_FORMAT_NONE   = 0,                     ///< no ecg
    eECG_FORMAT_RAW    = 1,                     ///< uncompressed
    eECG_FORMAT_SPIHT  = 2                      ///< SPIHT compressed
} ECG_FORMAT_e;


/// Episode file header (stored in a single episode block)
typedef struct {

    uint8_t                 episode_ver[4];         ///< episode format version
    uint8_t                 reserved [12];
    DEVICE_INFO_t           device_info;            ///< device info
    char_t                  major_version;          ///< episode file format major version
    char_t                  minor_version;          ///< episode file format minor version
    DATE_t                  power_on_date;          ///< power-on date
    TIME_t                  power_on_time;          ///< power-on time
    uint32_t                duration;               ///< episode duration in ecg samples
    ECG_FORMAT_e            ecg_format;             ///< ecg format
    uint8_t                 align_1 [3];
    DEVICE_SETTINGS_t       device_settings;        ///< device settings
    BATTERY_INFO_t          battery_info;           ///< battery information
    BATTERY_STATISTICS_t    battery_statistics;     ///< battery statistics
    uint8_t                 align_2 [8];
    ELECTRODES_DATA_t       electrodes_data;        ///< electrodes information and event data
    uint8_t                 align_3 [4];

    uint8_t                 dummy[EPISODE_BLOCK_SZ - 372];
} DB_EPISODE_HEADER_t;

/// structure to hold default data before writing to episode file
typedef struct
{
     int8_t     temperature;                    ///< Temperature in degrees (ºC)
    uint8_t     battery_charge;                 ///< battery charge (percent)
    uint16_t    battery_voltage;                ///< battery voltage (in mv)
    uint16_t    Vc;                             ///< Capacitor voltage (volts)
    uint16_t    shock_energy;                   ///< Energy used in last shock event
    uint16_t    rcp_time;                       ///< RCP time in last RCP event
    uint8_t     error;                          ///< Error code from error manager
    uint8_t     rythm_type;                     ///< rythm type
    uint8_t     event_offset_time[16];          ///< Event time in ECG samples*2 (2ms increments)
    uint8_t     event_id[16];                   ///< Event identifier
    uint8_t     dummy[8];                       ///< data align
} DB_DATA_DEFAULT_t;

#define DB_DATA_DRD_t   DRD_DATA_t

#define DATA_AUX_SIZE   52

//COMPILE_ASSERT(sizeof(DB_DATA_DRD_t) == 48);
//COMPILE_ASSERT(sizeof(DB_DATA_DEFAULT_t) == DATA_AUX_SIZE);

/// structure to hold data before writing to file in multiples of cluster size
typedef struct
{
    uint32_t    time_stamp;                     ///< time stamp in ECG samples
    uint16_t    data_type;                      ///< type of data in the block
    uint16_t    zp_ohms;                        ///< Patient impedance (in ohms)
    uint32_t    zp_cal;                         ///< dynamic ZP calibration (in ADCs)
     int16_t    ecg[EPISODE_BLOCK_ECG_SZ];      ///< ECG data section
    uint32_t    zp [EPISODE_BLOCK_ZP_SZ];       ///< ZP  data section
    uint8_t     data_aux[DATA_AUX_SIZE];        ///< auxiliary data in the block (DRD, etc...)
} DB_EPISODE_BLOCK_t;

//COMPILE_ASSERT(sizeof(DB_EPISODE_HEADER_t) == EPISODE_BLOCK_SZ);
//COMPILE_ASSERT(sizeof(DB_EPISODE_BLOCK_t)  == EPISODE_BLOCK_SZ);

/// structure to hold resampled data
typedef struct
{
    uint32_t    time_stamp;                     ///< time stamp in ECG samples
    uint16_t    data_type;                      ///< type of data in the block
    uint16_t    zp_ohms;                        ///< Patient impedance (in ohms)
    uint32_t    zp_cal;                         ///< dynamic ZP calibration (in ADCs)
     int16_t    ecg[EPISODE_BLOCK_ECG_SZ/5];      ///< ECG data section
    uint32_t    zp [EPISODE_BLOCK_ZP_SZ/5];       ///< ZP  data section
    uint8_t     data_aux[DATA_AUX_SIZE];        ///< auxiliary data in the block (DRD, etc...)
} DB_RESAMPLE_BLOCK_t;

/******************************************************************************
 ** globals
 */

/******************************************************************************
** prototypes
*/

extern  bool_t  DB_Episode_Create       ();
extern  bool_t  DB_Episode_Write_mock        ();
extern  void    DB_Episode_Set_Event_mock    (REG_EVENTS_e event_id);
extern  void    DB_Episode_Resample_mock     (char_t *pName);
extern  bool_t  DB_Is_Episode_Sent      (void);
extern  bool_t  Check_Episode_Present_mock   (char_t *pName);
extern  void    DB_Episode_Delete_mock       (char_t *pName);
extern  char_t* DB_Episode_Get_Current_Filename (void);
extern  void    DB_Episode_Set_Current_Filename (char_t *pName);


#endif  /*DB_EPISODE_H*/
