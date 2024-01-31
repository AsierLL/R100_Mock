/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        DB_Test.c
 * @brief
 *
 * @version     v1
 * @date        20/07/2020
 * @author      lsanz
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

/******************************************************************************
 ** Includes
 */

#include "DB_Test_Mock.h"

#include "HAL/thread_patMon_hal_Mock.h"

#include "synergy_gen_Mock/thread_recorder_Mock.h"
#include "thread_drd_entry.h"
#include "R100_Errors.h"
#include "Resources/Trace_Mock.h"
#include "thread_sysMon_entry_Mock.h"


/******************************************************************************
 ** Macros
 */

/******************************************************************************
 ** Defines
 */
#define TEST_FOLDER_NAME            "TEST"      ///< folder name for Test


#define TEST_BLOCK_SZ               (1024)      ///< test block total size

#define NUM_TEST_FILES              (12)        ///< maximum number of test files in a single uSD card
#define NUM_BLOCKS_TEST             (30)        ///< maximum number of blocks in a single file
#define SIGFOX_FILENAME         "SIGFOX.dat"    ///< sigfox filename


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

DB_TEST_RESULT_t    R100_test_result;               // test result !!!

/******************************************************************************
 ** Locals
 */

//static  FX_MEDIA   *sd_media = (FX_MEDIA *)NULL;    ///< handler of the SD media card
static  FILE     test_file;                      ///< Test file handler

static  uint8_t     test_str[TEST_BLOCK_SZ];        // test string

static char_t       last_test_fullname [32];      // Last Test file name

static DEVICE_DATE_t *pDate;
static DEVICE_TIME_t *pTime;

static char_t   fullname [32];      // full name

static bool_t   first_test_write;
/******************************************************************************
 ** Prototypes
 */
static void     Assign_TestFile_Number_mock  (DB_TEST_RESULT_t *pResult, char_t *pName);


/******************************************************************************
** Name:    Assign_TestFile_Number
*****************************************************************************/
/**
** @brief   Fill the filename with the test file number. The test result identifier
**          determines if a new file must be created or the last one must be used
**          Each test file must register the test result of 30 days (IDs 0 to 29)
**
** @param   pResult : pointer to a test result structure
** @param   pName   : pointer to a filename to raster
**
** @return  none
**
******************************************************************************/
static void Assign_TestFile_Number_mock (DB_TEST_RESULT_t *pResult, char_t *pName)
{
  //  UNUSED(pResult);
    uint32_t    fx_res;
    //uint32_t    attributes;
    //uint32_t    i, j;
    uint8_t     month_id;

    pName[10] = pName[11] = '0';    // assign the number ID to a filename from the "Test_0000_XX.TXT" pattern
    printf("Assign TestFile_Number mock\n");
    // Set the default directory to TEST folder


    // read the current date & time from the RTC
    month_id = Get_RTC_Month ();
    pName[10] = (char_t) ((month_id/10) + '0');
    pName[11] = (char_t) ((month_id%10) + '0');

  ;
}

/******************************************************************************
** Name:    DB_Test_Create
*****************************************************************************/
/**
** @brief   Creates a new test file
**
** @param   pResult : pointer to a test result structure
** @param   pFile   : pointer to a test file
**
** @return  operation result (true or false)
**
******************************************************************************/
static bool_t DB_Test_Create_mock (DB_TEST_RESULT_t *pResult, FILE *pFile)
{
    char_t   filename [32];      // file name
    char_t   header [259]= "//////////////////////////\r\n//////////////////////////\r\n//\r\n// TEST IDS:\r\n//\r\n// BASIC-AUTO   -> 0 - 28\r\n// FULL-AUTO    -> 29\r\n// MANUAL       -> 40\r\n// INITIAL      -> 50\r\n// FUNCTIONAL   -> 60\r\n//\r\n//////////////////////////\r\n//////////////////////////,\r\n";
    char_t   sn_str [8];         // serial number string (last 4 characters)
    uint8_t  sn_len;             // serial number string length
    uint32_t attributes, fx_res; // file system operation results
    bool_t   retCode=1;            // operation result

    printf("DB_TestCreate mock\n");

    // create the filename to use
    sn_len = (uint8_t) strlen(pResult->device_sn);
    if ((sn_len >= 4) && (sn_len < RSC_NAME_SZ))
    {
        sn_str[0] = pResult->device_sn[sn_len - 4];
        sn_str[1] = pResult->device_sn[sn_len - 3];
        sn_str[2] = pResult->device_sn[sn_len - 2];
        sn_str[3] = pResult->device_sn[sn_len - 1];
    }
    else {
        sn_str[0] = '0';
        sn_str[1] = '0';
        sn_str[2] = '0';
        sn_str[3] = '0';
    }
    sn_str[4] = 0;

    sprintf (filename, "R100_%s_XX.TXT", sn_str);

    Assign_TestFile_Number_mock(pResult, filename);


    // assign the full filename ...
    sprintf (fullname, "%s\\%s", TEST_FOLDER_NAME, filename);

    strcpy(last_test_fullname, fullname);


    Trace_Arg_mock (TRACE_NO_FLAGS | TRACE_NEWLINE, " %s", (uint32_t) last_test_fullname);

    // ... check if the requested test file exist. If not, create a new file


        // ... open the test file

        // ... write test id summary


        // ... flush data to physical media


  //  else
//    {
        // ... open the test file

 //   }

    // assign the operation result ...


    // ... flush data to physical media
    //fx_res = (uint8_t) fx_media_flush (sd_media);

    // return the operation result
    return retCode;
}

/******************************************************************************
** Name:    Fill_JSON_Header
*****************************************************************************/
/**
** @brief   Fill the string number to create a Json entry (using 3 tabs)
**
** @param   pJSON  : pointer to string to fill
** @param   pName  : pointer to entry name
**
******************************************************************************/
static void Fill_JSON_Header (char_t *pJSON, char_t *pName)
{
    uint32_t  str_start;
    uint32_t  nBytes;

    str_start = nBytes = strlen (pJSON);
    sprintf (&pJSON[nBytes], "\r\n\t\"%s\": {", pName);

    Trace_mock (TRACE_NO_FLAGS, &pJSON[str_start]);
}

/******************************************************************************
** Name:    Fill_JSON_Number
*****************************************************************************/
/**
** @brief   Fill the string number to create a Json entry (using 3 tabs)
**
** @param   pJSON  : pointer to string to fill
** @param   pName  : pointer to entry name
** @param   number : number to fill
** @param   colon  : includes an entry separator or not
**
******************************************************************************/
static void Fill_JSON_Number (char_t *pJSON, char_t *pName, int32_t number, bool_t colon)
{
    uint32_t  str_start;
    uint32_t  nBytes;

    str_start = nBytes = strlen (pJSON);

    pJSON[nBytes++] = '\r';
    pJSON[nBytes++] = '\n';
    pJSON[nBytes++] = '\t';
    pJSON[nBytes++] = '\t';

    sprintf (&pJSON[nBytes], "\"%s\": %i", pName, number);

    nBytes = strlen (pJSON);
    if (colon)
    {
        pJSON [nBytes++] = ',';
    }
    else {
        pJSON [nBytes++] = '\r';
        pJSON [nBytes++] = '\n';
        pJSON [nBytes++] = '\t';
        pJSON [nBytes++] = '}';
    }
    pJSON [nBytes] = 0;

    Trace_mock (TRACE_NO_FLAGS, &pJSON[str_start]);
}

/******************************************************************************
** Name:    Fill_JSON_String
*****************************************************************************/
/**
** @brief   Fill the string to create a Json entry (using 3 tabs)
**
** @param   pJSON  : pointer to string to fill
** @param   pName  : pointer to entry name
** @param   pStr   : string to copy on the JSON string
** @param   colon  : includes an entry separator or not
**
******************************************************************************/
static void Fill_JSON_String (char_t *pJSON, char_t *pName, char_t *pStr, bool_t colon)
{
    uint32_t  str_start;
    uint32_t  nBytes;

    str_start = nBytes = strlen (pJSON);

    pJSON[nBytes++] = '\r';
    pJSON[nBytes++] = '\n';
    pJSON[nBytes++] = '\t';
    pJSON[nBytes++] = '\t';

    sprintf (&pJSON[nBytes], "\"%s\": \"%s\"", pName, pStr);

    nBytes = strlen (pJSON);
    if (colon)
    {
        pJSON [nBytes++] = ',';
    }
    else {
        pJSON [nBytes++] = '\r';
        pJSON [nBytes++] = '\n';
        pJSON [nBytes++] = '\t';
        pJSON [nBytes++] = '}';
    }
    pJSON [nBytes] = 0;
    Trace_mock (TRACE_NO_FLAGS, &pJSON[str_start]);
}

/******************************************************************************
** Name:    Fill_Bitset_8
*****************************************************************************/
/**
** @brief   fill a binary bitset in a string
**
** @param   pStr   : pointer to a string
** @param   bitset : number to convert
**
******************************************************************************/
static void Fill_Bitset_8 (char_t *pStr, uint8_t bitset)
{
    pStr[0] = (bitset & 0x80) ? '1' : '0';
    pStr[1] = (bitset & 0x40) ? '1' : '0';
    pStr[2] = (bitset & 0x20) ? '1' : '0';
    pStr[3] = (bitset & 0x10) ? '1' : '0';
    pStr[4] = (bitset & 0x08) ? '1' : '0';
    pStr[5] = (bitset & 0x04) ? '1' : '0';
    pStr[6] = (bitset & 0x02) ? '1' : '0';
    pStr[7] = (bitset & 0x01) ? '1' : '0';
    pStr[8] = 0;
}

/******************************************************************************
** Name:    Fill_HEX_64
*****************************************************************************/
/**
** @brief   fill an hexadecimal number of 64 bits in a string
**
** @param   pStr   : pointer to a string
** @param   num_64 : number to convert
**
******************************************************************************/
static void Fill_HEX_64 (char_t *pStr, uint8_t *pNum64)
{
    uint32_t    i, j;       // global use counter
    uint8_t     nibble;

    for (i=j=0; i<8; i++)
    {
        nibble = pNum64[i] >> 4;
        pStr[j++] = (char_t) ((nibble > 9) ? ((nibble - 10) + 'A') : (nibble +'0'));
        nibble = pNum64[i] & 0x0F;
        pStr[j++] = (char_t) ((nibble > 9) ? ((nibble - 10) + 'A') : (nibble +'0'));
    }
    pStr[j] = 0;
}

/******************************************************************************
** Name:    Fill_Test_Date
*****************************************************************************/
/**
** @brief   Initializes the test date string
**
** @param   pStr : pointer to a Date string
**
******************************************************************************/
//static void Fill_Test_Date (char_t *pStr)
void Fill_Test_Date (char_t *pStr)
{
    // read the power on date and time ...
    pDate = Get_Device_Date();

    // Fill the date string
    strcpy (pStr, "20xx-MM-DD");
    pStr[2] = (char_t) ((pDate->year  / 10) + '0');
    pStr[3] = (char_t) ((pDate->year  % 10) + '0');
    pStr[5] = (char_t) ((pDate->month / 10) + '0');
    pStr[6] = (char_t) ((pDate->month % 10) + '0');
    pStr[8] = (char_t) ((pDate->date  / 10) + '0');
    pStr[9] = (char_t) ((pDate->date  % 10) + '0');
}

/******************************************************************************
** Name:    Fill_Test_Time
*****************************************************************************/
/**
** @brief   Initializes the test time string
**
** @param   pStr : pointer to a Time string
**
******************************************************************************/
//static void Fill_Test_Time (char_t *pStr)
void Fill_Test_Time (char_t *pStr)
{
    // read the power on date and time ...
    pTime = Get_Device_Time();

    // Fill the time string
    strcpy (pStr, "HH:mm:ss");
    pStr[0] = (char_t) ((pTime->hour / 10) + '0');
    pStr[1] = (char_t) ((pTime->hour % 10) + '0');
    pStr[3] = (char_t) ((pTime->min  / 10) + '0');
    pStr[4] = (char_t) ((pTime->min  % 10) + '0');
    pStr[6] = (char_t) ((pTime->sec  / 10) + '0');
    pStr[7] = (char_t) ((pTime->sec  % 10) + '0');
}

/******************************************************************************
** Name:    Fill_Generic_Date
*****************************************************************************/
/**
** @brief   Fill a string with a generic date
**
** @param   pStr  : pointer to a generic string
** @param   pDate : Date to convert
**
******************************************************************************/
void Fill_Generic_Date (char_t *pStr, uint32_t my_date)
{
    uint16_t    year;
    uint8_t     month, date;

    year  = (uint16_t) (my_date >> 16);
    month = (uint8_t)  (my_date >> 8);
    date  = (uint8_t)  (my_date & 0xFF);

    // Fill the date string
    strcpy (pStr, "YYYY-MM-DD");
    pStr[0] = (char_t) ((year / 1000) + '0');      year %= 1000;
    pStr[1] = (char_t) ((year /  100) + '0');      year %=  100;
    pStr[2] = (char_t) ((year /   10) + '0');
    pStr[3] = (char_t) ((year %   10) + '0');
    pStr[5] = (char_t) ((month / 10) + '0');
    pStr[6] = (char_t) ((month % 10) + '0');
    pStr[8] = (char_t) ((date  / 10) + '0');
    pStr[9] = (char_t) ((date  % 10) + '0');
}

/******************************************************************************
** Name:    Fill_Generic_Time
*****************************************************************************/
/**
** @brief   Fill a string with a generic Time
**
** @param   pStr  : pointer to a generic string
** @param   pTime : Time to convert
**
******************************************************************************/
void Fill_Generic_Time (char_t *pStr, uint32_t my_time)
{
    uint8_t     hour, min, sec;

    hour = (uint8_t) (my_time >> 16);
    min  = (uint8_t) (my_time >> 8);
    sec  = (uint8_t) (my_time & 0xFF);

    // Fill the date string
    strcpy (pStr, "HH:MM:SS");
    pStr[0] = (char_t) ((hour / 10) + '0');
    pStr[1] = (char_t) ((hour % 10) + '0');
    pStr[3] = (char_t) ((min  / 10) + '0');
    pStr[4] = (char_t) ((min  % 10) + '0');
    pStr[6] = (char_t) ((sec  / 10) + '0');
    pStr[7] = (char_t) ((sec  % 10) + '0');
}

/******************************************************************************
** Name:    Write_Test_File
*****************************************************************************/
/**
** @brief   Writes the test result in the test file
**          This way, the write sizes are multiple of cluster sizes, thus
**          achieving max performance.
**
** @param   pResult : pointer to a test result structure
** @param   pFile   : pointer to a test file
**
** @return  none
**
******************************************************************************/
static void Write_Test_File_mock (DB_TEST_RESULT_t *pResult, FILE *pFile)
{
    uint32_t  nBytes;
    char_t    my_str [32];          // buffer to fill with a generic string
    uint32_t  fx_res;
    char_t    sw_version [5];

    printf("Write_Test_File_mock\n");
   // UNUSED(fx_res);

    ////////////////////////////////////////////////////////////
    // print the Header
    memset  (test_str, ' ', TEST_BLOCK_SZ);
    memset  (test_str, '/', 45);
    if (!first_test_write)
    {
        test_str[0] = ',';
        test_str[1] = '\r';
        test_str[2] = '\n';
    }
    test_str[45] = 0;

    // some errors Must be reported ONLY in the full test
    if ((pResult->test_id != TEST_FULL_ID) && (pResult->test_id != TEST_MANUAL_ID) && (pResult->test_id != TEST_FUNC_ID))
    {
        pResult->voltage.dc_18v_error = 0;
    }

    Get_APP_SW_Version (sw_version);

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // Basic Test data
    {
        strcat ((char_t *) test_str, "\r\n{");
        Fill_JSON_Header ((char_t *) test_str, "Test Header");
        Fill_JSON_Number ((char_t *) test_str, "Test Id", (int32_t) pResult->test_id, true);
        Fill_JSON_String ((char_t *) test_str, "SN", (char_t *)  Get_Device_Info()->sn, true);
        Fill_JSON_String ((char_t *) test_str, "SW version", (char_t *) sw_version, true);
        Fill_Test_Date (my_str);
        Fill_JSON_String ((char_t *) test_str, "Date", my_str, true);
        Fill_Test_Time (my_str);
        Fill_JSON_String ((char_t *) test_str, "Time", my_str, true);
        //Fill_JSON_Number ((char_t *) test_str, "Temperature", pResult->patmon.ADS_temperature, true);
        Fill_JSON_Number ((char_t *) test_str, "Temperature", pResult->battery.bat_temperature, true);
        Fill_JSON_Number ((char_t *) test_str, "Test Result", pResult->test_status, true);
        Fill_JSON_Number ((char_t *) test_str, "Error Code",  (int32_t) pResult->error_code, false);

        strcat ((char_t *) test_str, ",");
        Fill_JSON_Header ((char_t *) test_str, "Voltages");
        Fill_JSON_Number ((char_t *) test_str, "Value DC_MAIN", (int32_t) pResult->voltage.dc_main, true);
        Fill_JSON_Number ((char_t *) test_str, "Error DC_MAIN", (int32_t) pResult->voltage.dc_main_error, true);
        Fill_JSON_Number ((char_t *) test_str, "Value DC_18V",  (int32_t) pResult->voltage.dc_18v,  true);
        Fill_JSON_Number ((char_t *) test_str, "Error DC_18V",  (int32_t) pResult->voltage.dc_18v_error,  false);

        strcat ((char_t *) test_str, ",");
        Fill_JSON_Header ((char_t *) test_str, "Misc");
        Fill_JSON_Number ((char_t *) test_str, "Storage NFC",    (int32_t) pResult->misc.nfc, true);
        Fill_JSON_Number ((char_t *) test_str, "Boot Processor", (int32_t) pResult->misc.boot, true);
        Fill_JSON_Number ((char_t *) test_str, "Audio",          (int32_t) pResult->misc.audio, false);

        strcat ((char_t *) test_str, ",");
        Fill_JSON_Header  ((char_t *) test_str, "Battery");
        Fill_JSON_String  ((char_t *) test_str, "Name",         (char_t *) pResult->battery.name, true);
        Fill_JSON_String  ((char_t *) test_str, "S/N",          (char_t *) pResult->battery.sn, true);

        Fill_JSON_Number  ((char_t *) test_str, "Rev_Major",    (int32_t) pResult->battery.version_major, true);
        Fill_JSON_Number  ((char_t *) test_str, "Rev_Minor",    (int32_t) pResult->battery.version_minor, true);
        Fill_Generic_Date (my_str,   pResult->battery.manufacture_date);
        Fill_JSON_String  ((char_t *) test_str, "Manufacture",  my_str, true);
        Fill_Generic_Date (my_str,   pResult->battery.expiration_date);
//        Fill_JSON_String  ((char_t *) test_str, "Expiration",   my_str, true);
//        Fill_JSON_Number  ((char_t *) test_str, "Nominal Cap",  (int32_t) pResult->battery.nominal_capacity,  true);
        Fill_JSON_Number  ((char_t *) test_str, "nTime run",    (int32_t) pResult->battery.runTime_total,  true);
        Fill_JSON_Number  ((char_t *) test_str, "battery state",(int32_t) pResult->battery.battery_state,  true);
        Fill_JSON_Number  ((char_t *) test_str, "VC charges",   (int32_t) pResult->battery.nFull_charges, true);
        Fill_JSON_Number  ((char_t *) test_str, "Rem charge",   (int32_t) pResult->battery.rem_charge, false);

        strcat ((char_t *) test_str, ",");
        Fill_JSON_Header ((char_t *) test_str, "Patient Monitor");
        Fill_JSON_Number ((char_t *) test_str, "ADS Comms",   (int32_t) pResult->patmon.ADS_comms, true);
        Fill_JSON_Number ((char_t *) test_str, "ADS Cal ZP",  (int32_t) pResult->patmon.ADS_cal, false);

        strcat ((char_t *) test_str, ",");
        Fill_JSON_Header ((char_t *) test_str, "Electrodes");
        Fill_HEX_64      (my_str,   (uint8_t *) &pResult->electrodes.sn);
        Fill_JSON_String ((char_t *) test_str, "S/N", my_str, true);
        Fill_Generic_Date(my_str,   pResult->electrodes.expiration_date);
        Fill_JSON_String ((char_t *) test_str, "Expiration", my_str, true);
        Fill_Generic_Date(my_str,   pResult->electrodes.event_date);
        Fill_JSON_String ((char_t *) test_str, "Event Date", my_str, true);
        Fill_Generic_Time(my_str,   pResult->electrodes.event_time);
        Fill_JSON_String ((char_t *) test_str, "Event Time", my_str, true);
        Fill_Bitset_8    (my_str,   (uint8_t) pResult->electrodes.event_id);
        Fill_JSON_String ((char_t *) test_str, "Event Bitset", my_str, false);

        strcat ((char_t *) test_str, ",");
        Fill_JSON_Header ((char_t *) test_str, "Comms");
        Fill_JSON_Number ((char_t *) test_str, "Accelerometer", (int32_t) pResult->comms.accelerometer, true);
        Fill_JSON_Number ((char_t *) test_str, "Wifi",          (int32_t) pResult->comms.wifi,          true);
        Fill_JSON_Number ((char_t *) test_str, "Sigfox",        (int32_t) pResult->comms.sigfox,        true);
        Fill_JSON_Number ((char_t *) test_str, "GPS",           (int32_t) pResult->comms.gps,           false);

        if ((pResult->test_id != TEST_FULL_ID) && (pResult->test_id != TEST_MANUAL_ID) && (pResult->test_id != TEST_FUNC_ID)) strcat ((char_t *) test_str, "\r\n}");
    }

    nBytes = strlen ((char_t *) test_str);
    test_str [nBytes] = ' ';

    // must register the test result ???
    if (pFile)
    {
    //   tx_mutex_get(&usd_mutex, TX_WAIT_FOREVER);
     //   fx_res = (uint8_t) fx_file_write (pFile, test_str, TEST_BLOCK_SZ);
     //   tx_mutex_put(&usd_mutex);

        // some errors Must be reported ONLY in the full test
        if ((pResult->test_id != TEST_FULL_ID) && (pResult->test_id != TEST_MANUAL_ID) && (pResult->test_id != TEST_FUNC_ID))
        {
            // flush data to the uSD and return to close file
            //fx_res = (uint8_t) fx_media_flush(sd_media);
            return;
        }
    }

    memset (test_str, ' ', TEST_BLOCK_SZ);

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // Full Test data
    {
        test_str[0] = ',';
        test_str[1] = 0;
        Fill_JSON_Header ((char_t *) test_str, "Test FULL");
        Fill_JSON_String ((char_t *) test_str, "GPS Position",                   pResult->gps_position,                  true);
        Fill_JSON_Number ((char_t *) test_str, "Error Code",                     (int32_t) pResult->defib.error_code,              true);
        Fill_JSON_Number ((char_t *) test_str, "Charging voltage (volts)",       (int32_t) pResult->defib.full_charging_voltage,   true);
        Fill_JSON_Number ((char_t *) test_str, "Charging time (msecs)",          (int32_t) pResult->defib.full_charging_time,      true);
        Fill_JSON_Number ((char_t *) test_str, "DisCharging H current (mA)",     (int32_t) pResult->defib.full_discharg_H_current, true);
        Fill_JSON_Number ((char_t *) test_str, "DisCharging H voltage (volts)",  (int32_t) pResult->defib.full_discharg_H_voltage, true);
        Fill_JSON_Number ((char_t *) test_str, "DisCharging H time (msecs)",     (int32_t) pResult->defib.full_discharg_H_time,    true);
        Fill_JSON_Number ((char_t *) test_str, "DisCharging R voltage (volts)",  (int32_t) pResult->defib.full_discharg_R_voltage, true);
        Fill_JSON_Number ((char_t *) test_str, "DisCharging R time (msecs)",     (int32_t) pResult->defib.full_discharg_R_time,    false);

        strcat ((char_t *) test_str, "\r\n}");
    }

    // must register the test result ???
    printf("Write Test File\n");
    if (pFile)
    {
        nBytes = strlen ((char_t *) test_str);
        test_str [nBytes] = ' ';
       //write to file
    }
}

/******************************************************************************
** Name:    DB_Test_Generate_Report
*****************************************************************************/
/**
** @brief   Collects the test results and generates the report in a JSON file
**
** @param   pResult     : pointer to a test result structure
** @param   file_report : if must generate a file report or not
**
** @return  operation result
**
******************************************************************************/
bool_t DB_Test_Generate_Report_mock (DB_TEST_RESULT_t *pResult, bool_t file_report)
{
    uint32_t  fx_res;

  //  UNUSED(fx_res);

    if (!pResult) return FALSE;
    printf("DB_Test_Generate_Report mock\n");

    if (file_report)
    {
       // sd_media = &sd_fx_media;        // initialize the SD media handler

        // create the test file and initialize some test items (identifier, date and time)
        if (DB_Test_Create_mock (pResult, &test_file) == TRUE)
        {
            // register the test result into the uSD
            Write_Test_File_mock (pResult, &test_file);

            // close the test file and bye-bye
          //  fx_res = (uint8_t) fx_file_close(&test_file);
            // fx_res = fx_media_close (sd_media);
            // sd_media = (FX_MEDIA *) NULL;
        }
    }
    else {
        // report the test result (do not register the result in a file)
        Write_Test_File_mock (pResult, NULL);
    }

    // assume always that the file has been written OK
    return TRUE;
}

/******************************************************************************
** Name:    DB_Test_Get_Filename
*****************************************************************************/
/**
** @brief   Returns the test last full name
**
** @param   none
**
** @return  pointer to a test filename
**
******************************************************************************/
char_t* DB_Test_Get_Filename (void)
{
   return last_test_fullname;
}

/******************************************************************************
** Name:    PowerOff_Write_Elec
*****************************************************************************/
/**
** @brief   Before power off the device, write the last time the decive has been used in the test
**
** @param   none
**
** @return  void
**
******************************************************************************/
void PowerOff_Write_Elec_mock(void)
{
    uint32_t  fx_res = 0;
    uint32_t  nBytes = 0;
    char_t    my_str [32];          // buffer to fill with a generic string
    ELECTRODES_DATA_t my_elec_data;

    printf("PowerOff Write Elec mock\n");
    if(write_elec_last_event)
    {
       // fx_res = (uint8_t) fx_file_open(&sd_fx_media, &test_file, fullname, FX_OPEN_FOR_WRITE);

        if(fx_res == 0)
        {
            memset  (test_str, 0, TEST_BLOCK_SZ);

            Electrodes_Get_Data (&my_elec_data);

            strcat ((char_t *) test_str, ",");
            strcat ((char_t *) test_str, "\r\n{");
            Fill_JSON_Header ((char_t *) test_str, "Last use");
            Fill_Generic_Date(my_str,   my_elec_data.event.date);
            Fill_JSON_String ((char_t *) test_str, "Event Date", my_str, true);
            Fill_Generic_Time(my_str,   my_elec_data.event.time);
            Fill_JSON_String ((char_t *) test_str, "Event Time", my_str, true);
            Fill_JSON_Number ((char_t *) test_str, "Event ID",     (int32_t) my_elec_data.event.id, false);
            strcat ((char_t *) test_str, "\r\n}");

            nBytes = strlen ((char_t *) test_str);
            test_str [nBytes] = ' ';
          /*  fx_res = (uint8_t) tx_mutex_get(&usd_mutex, TX_WAIT_FOREVER);
            fx_res = (uint8_t) fx_file_write (&test_file, test_str, nBytes);
            fx_res = (uint8_t) fx_media_flush(sd_media);
            fx_res = (uint8_t) tx_mutex_put(&usd_mutex);

            fx_res = (uint8_t) fx_file_close(&test_file);*/
        }
    }

}

/******************************************************************************
** Name:    DB_Sigfox_Generate_Info
*****************************************************************************/
/**
** @brief   Stores Sigfox module info in a file
**
** @param   device_id     : sigfox device id
** @param   pac_id        : sigfox pac id
**
** @return  operation result
**
******************************************************************************/
void DB_Sigfox_Store_Info_mock (char_t * device_id, char_t * pac_id)
{
    char_t   sigfox_str [50];    // buffer to fill with a generic string
    uint32_t attributes, fx_res; // file system operation results
    static FILE  sigfox_file;


    printf("DB_Sigfox_Store_Info_mock\n");
    if((device_id == NULL) || (pac_id == NULL)) return;

    // Initialize
    memset  (sigfox_str, 0, sizeof(sigfox_str));

    strcpy(sigfox_str,"DEVICE ID:");
    strcat(sigfox_str, device_id);
    strcat(sigfox_str,"PAC ID:");
    strcat(sigfox_str, pac_id);

    printf("DB_Sigfox_Store_Info\n");

    //sd_media = &sd_fx_media;        // initialize the SD media handler

    /*fx_res = (uint8_t) fx_directory_default_set(sd_media, FX_NULL);

    // ... check if the requested file exist. If so, delete and create a new one
    fx_res = (uint8_t) fx_file_attributes_read (sd_media, SIGFOX_FILENAME, (UINT *)&attributes);
    if ((fx_res == FX_SUCCESS) || (attributes & FX_ARCHIVE))
    {
        fx_res = (uint8_t) fx_file_delete (sd_media, SIGFOX_FILENAME);
        fx_res = (uint8_t) fx_media_flush (sd_media);
    }

    fx_res = (uint8_t) fx_file_create(sd_media, SIGFOX_FILENAME);

    // ... open the test file
    fx_res = (uint8_t) fx_file_open(sd_media, &sigfox_file, SIGFOX_FILENAME, FX_OPEN_FOR_WRITE);

    // ... write test id summary
    fx_res = (uint8_t) fx_file_write (&sigfox_file, sigfox_str, sizeof(sigfox_str));

    // ... flush data to physical media
    fx_res = (uint8_t) fx_media_flush (sd_media);

    // ... close the file
    fx_res = (uint8_t) fx_file_close(&sigfox_file);

    // Hidden file
    fx_res = (uint8_t) fx_file_attributes_set(sd_media, SIGFOX_FILENAME, FX_HIDDEN);
    */
}
