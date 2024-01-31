/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        DB_Episode.c
 * @brief
 *
 * @version     v1
 * @date        21/06/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

/******************************************************************************
 ** Includes
 */

#include "DB_Episode_Mock.h"

#include "HAL/thread_defibrillator_hal_Mock.h"
#include "HAL/thread_patMon_hal_Mock.h"
#include "synergy_gen_Mock/thread_audio_Mock.h"                   // to get the uSD semaphore
#include "synergy_gen_Mock/thread_recorder_Mock.h"
#include "thread_drd_entry.h"
#include "thread_sysMon_entry_Mock.h"
#include "thread_comm_entry.h"
#include "R100_Errors.h"

/******************************************************************************
 ** Macros
 */

/******************************************************************************
 ** Defines
 */
#define EPI_FOLDER_NAME     "EPISODES"      ///< folder name for episodes

/******************************************************************************
 ** Typedefs
 */

/******************************************************************************
 ** Constants
 */

/******************************************************************************
 ** Externs
 */

// defined in the FSM
extern  uint16_t                FSM_RCP_time_secs;          // current RCP time in seconds
extern  uint16_t                FSM_shock_energy;           // energy to apply in the shock


/******************************************************************************
 ** Globals
 */

/******************************************************************************
 ** Locals
 */

//static FX_MEDIA *sd_media = (FX_MEDIA *)NULL;       ///< handler of the SD media card
static FILE   epi_file;                          ///< Episode file handler

static char_t  epi_filename [32];      // file name
static char_t  fullname [32];                       // episode full name

static uint32_t episode_time;                       // episode time counter
static uint8_t  episode_buffer [EPISODE_BLOCK_SZ];  // episode generic buffer
static  bool_t  episode_send_finish = false; // episode send finish close up flag

static DB_EPISODE_HEADER_t      *pEpi_Header;       // pointer to episode header
static DB_EPISODE_BLOCK_t       *pEpi_Block;        // pointer to episode block data
static DRD_RESULT_t             drd_result;

static DEVICE_DATE_t *pDate;
static DEVICE_TIME_t *pTime;
/******************************************************************************
 ** Prototypes
 */
static void     Assign_Episode_Number_mock   (char_t *pName);
static void     Episode_Header_Init     (void);

/******************************************************************************
** Name:    Assign_Episode_Number
*****************************************************************************/
/**
** @brief   Assign an episode number to a filename
**
** @param   pName: pointer to a filename to raster
**
** @return  none
**
******************************************************************************/
static void Assign_Episode_Number_mock (char_t *pName)
{
    uint32_t    attributes, fx_res;
    uint32_t    i;

   // sd_media = &sd_fx_media;        // initialize the SD media handler
    pName[10] = pName[11] = '0';    // assign the default filename
    printf("Assign Episode Number mock\n");

    // Set the default directory to EPI folder
   // fx_res = (uint8_t) fx_directory_default_set(sd_media, EPI_FOLDER_NAME);
    if (fx_res != 0)
    {
        // the EPI directory does not exist. Create it and bye-bye
     //   fx_res = (uint8_t) fx_directory_create(sd_media, EPI_FOLDER_NAME);
        // Hidden folder
       // fx_res = (uint8_t) fx_directory_attributes_set(sd_media, EPI_FOLDER_NAME, FX_HIDDEN);
        return;
    }

    // try to find the last file ...
    for (i=0; i<NUM_EPISODES; i++)
    {
        pName[10] = (char_t) ((i/10) + '0');
        pName[11] = (char_t) ((i%10) + '0');
     //   fx_res = (uint8_t) fx_file_attributes_read (sd_media, pName, (UINT *)&attributes);
        if ((fx_res != 0) )
        {
                // Reset the default directory to root
          //  fx_res = (uint8_t) fx_directory_default_set(sd_media, FX_NULL);
            return;

            /*// delete the next file in sequence for the next time
            if (i >= (NUM_EPISODES-1))
            {
                pName[10] = pName[11] = '0';
            }
            else if (pName[11] == '9')
            {
                pName[10]++;
                pName[11] = '0';
            }
            else {
                pName[11]++;
            }
            fx_res = (uint8_t) fx_file_delete (sd_media, pName);
            fx_res = (uint8_t) fx_media_flush (sd_media);
            // Reset the default directory to root
            fx_res = (uint8_t) fx_directory_default_set(sd_media, FX_NULL);

            // assign the filename to write in
            pName[10] = (char_t) ((i/10) + '0');
            pName[11] = (char_t) ((i%10) + '0');
            return;*/
        }
    }

    // the test folder is full. Delete the 2 first files in the sequence
    pName[10] = '0';
    pName[11] = '1';
 //   fx_res = (uint8_t) fx_file_delete (sd_media, pName);
    pName[10] = pName[11] = '0';
   // fx_res = (uint8_t) fx_file_delete (sd_media, pName);
    //fx_res = (uint8_t) fx_media_flush (sd_media);
    // Reset the default directory to root
    //fx_res = fx_directory_default_set(sd_media, FX_NULL);
}

/******************************************************************************
** Name:    Episode_Header_Init
*****************************************************************************/
/**
** @brief   Initializes the episode header values
**
******************************************************************************/
static void Episode_Header_Init(void)
{
    // Initialize and reset header
    memset ((uint8_t *) pEpi_Header, 0, sizeof (DB_EPISODE_HEADER_t));

    // Get device info
    memcpy ((uint8_t *) &pEpi_Header->device_info, Get_Device_Info(), sizeof (DEVICE_INFO_t));

    // Get device settings
    memcpy ((uint8_t *) &pEpi_Header->device_settings, Get_Device_Settings(), sizeof (DEVICE_SETTINGS_t));

    // read the power on date and time ...
    pDate = Get_Device_Date();
    pTime = Get_Device_Time();
    // Fill header additional information
    pEpi_Header->duration      = 0; // will be updated
    pEpi_Header->ecg_format    = eECG_FORMAT_NONE;
    pEpi_Header->major_version = EPISODE_MAJOR_VERSION;
    pEpi_Header->minor_version = EPISODE_MINOR_VERSION;
    pEpi_Header->power_on_date.year  = pDate->year;
    pEpi_Header->power_on_date.month = pDate->month;
    pEpi_Header->power_on_date.date  = pDate->date;
    pEpi_Header->power_on_time.hour  = pTime->hour;
    pEpi_Header->power_on_time.min   = pTime->min;
    pEpi_Header->power_on_time.sec   = pTime->sec;

    Battery_Get_Info (&pEpi_Header->battery_info);
    Battery_Get_Statistics (&pEpi_Header->battery_statistics);
    Electrodes_Get_Data (&pEpi_Header->electrodes_data);
}

/******************************************************************************
** Name:    DB_Episode_Open
*****************************************************************************/
/**
** @brief   Creates a new episode file
**
** @return  operation result (true or false)
**
******************************************************************************/
bool_t DB_Episode_Create_mock () //to check the different paths option
{
	int option = 1;
    char_t  sn_str [8];         // serial number string (last 4 characters)
    uint8_t sn_len;             // serial number string length
    uint8_t fx_res;             // file system operation results
    bool_t  retCode;            // operation result

    printf("DB_Episode_Create_mock\n");

   // sd_media = &sd_fx_media;    // initialize the SD media handler (just in case)

    pEpi_Header = (DB_EPISODE_HEADER_t *) episode_buffer;
    pEpi_Block  = (DB_EPISODE_BLOCK_t *)  episode_buffer;

    Episode_Header_Init();      // fill the episode header

    sn_len = (uint8_t) strlen(pEpi_Header->device_info.sn);
    if ((sn_len >= 4) && (sn_len < RSC_NAME_SZ))
    {
        sn_str[0] = pEpi_Header->device_info.sn[sn_len - 4];
        sn_str[1] = pEpi_Header->device_info.sn[sn_len - 3];
        sn_str[2] = pEpi_Header->device_info.sn[sn_len - 2];
        sn_str[3] = pEpi_Header->device_info.sn[sn_len - 1];
    }
    else {
        sn_str[0] = sn_str[1] = sn_str[2] = sn_str[3] = '0';
    }
    sn_str[4] = 0;

    sprintf (epi_filename, "R100_%s_XX.EPI", sn_str);

    Assign_Episode_Number_mock(epi_filename);

    sprintf (fullname, "%s\\%s", EPI_FOLDER_NAME, epi_filename);

    printf("DB Episode Create\n");
   // fx_res = (uint8_t) fx_directory_name_test(sd_media, EPI_FOLDER_NAME);
    //APP_ERR_TRAP(fx_res);
    if(option != 1) Lock_on_Panic_mock(fx_res, 1);

    if (option == 1)
    {
        // get the mutex to access to the uSD
     //   fx_res = (uint8_t)tx_mutex_get(&usd_mutex, TX_WAIT_FOREVER);

        // ... create it
       // fx_res = (uint8_t) fx_file_create(sd_media, fullname);
        //APP_ERR_TRAP(fx_res);
       // if(fx_res != FX_SUCCESS) Lock_on_Panic(fx_res, 2);

        // ... open it
       // fx_res = (uint8_t) fx_file_open(sd_media, &epi_file, fullname, FX_OPEN_FOR_WRITE);
        //APP_ERR_TRAP(fx_res);
    	printf("Escribir en fichero\n");
        if(option != 1) Lock_on_Panic_mock(fx_res, 3);

        // ... and write header content
   //     fx_res = (uint8_t) fx_file_write (&epi_file, (uint8_t *) pEpi_Header, sizeof (DB_EPISODE_HEADER_t));
        //APP_ERR_TRAP(fx_res);
       // if(fx_res != FX_SUCCESS) Lock_on_Panic(fx_res, 4);

        // ... flush data to physical media
     //   fx_res = (uint8_t) fx_media_flush (sd_media);
        //if(fx_res != FX_SUCCESS) Lock_on_Panic(fx_res, 5);

        // release the uSD mutex
       // fx_res = (uint8_t)tx_mutex_put(&usd_mutex);
    }

    // assign the operation result ...
    retCode = (option == 1);

    // ... flush data to physical media
    //fx_res = (uint8_t) fx_media_flush (sd_media);
    //APP_ERR_TRAP(fx_res);

    // initialize the episode time
    episode_time = patMon_Get_ECG_nSample();

    //initialize the device power up event
    //pEpi_Block->event_id = pEpi_Header->device_settings.misc.glo_patient_adult ? eREG_SET_PATIENT_ADULT : eREG_SET_PATIENT_PEDIATRIC;

    // return the operation result
    return retCode;
}

/******************************************************************************
** Name:    DB_Episode_Write
*****************************************************************************/
/**
** @brief   Collects the episode block data and write to the episode file.
**          This way, the write sizes are multiple of cluster sizes, thus
**          achieving max performance.
**
** @return  operation result
**
******************************************************************************/
bool_t DB_Episode_Write_mock ()
{
#define MAX_EVENTS_BLOCK    16

    uint8_t  fx_res;
    uint32_t status;
    uint32_t event_time;
    int option = 1;

    static  uint32_t block_counter = 0;         // block counter to check the file limit
    static  uint32_t first_sample  = 0;         // first sample into register buffers
    static  uint8_t  event_buffer   [MAX_EVENTS_BLOCK];     // buffer to store event id
    static  uint8_t  event_timestamp[MAX_EVENTS_BLOCK];     // event timestamp represented as and offset of episode timestamp
    static  uint8_t  event_idx = 0;             // event buffer index
    static  bool_t   patient_conn = false;      // patient connection flag
    static  bool_t   episode_close = false;     // episode close up flag
    static  bool_t   episode_send = false;      // episode send flag
    uint32_t ref_timeout = 0;
    uint32_t tout = 0;

    bool_t  win_rdy_ecg, win_rdy_zp;            // window ready flags ...
    DB_DATA_DEFAULT_t   *pData_Aux;             // pointer to auxiliary data
    REG_EVENTS_e        my_event;

    printf("DB_Episode_Write_mock\n");
    // proceed ONLY if the media is available ...
    //if (!sd_media) return FALSE;

    if (Is_Mode_Trainer()) return FALSE;

    // check the event queue

  //  status = tx_queue_receive(&queue_recorder, &my_event, OSTIME_300MSEC);
    if (status == TX_SUCCESS && my_event)
    {
        event_time = patMon_Get_ECG_nSample();
        event_timestamp[event_idx] = (uint8_t)((event_time - episode_time)/2);
        event_buffer[event_idx++] = my_event;
        if (event_idx >= MAX_EVENTS_BLOCK) event_idx = MAX_EVENTS_BLOCK-1;

        // if the event correspond to a diagnosis result ready
        if (my_event == eREG_DRD_DIAG_RDY)
        {
            DRD_Get_Diagnosis_mock(&drd_result);
            pEpi_Block->data_type = eBLOCK_DATA_DRD;
        }

        // if the event correspond SAT error get error code and force the episode close
        if (my_event == eREG_SAT_ERROR)
        {
            //get error
            pData_Aux = (DB_DATA_DEFAULT_t *) &pEpi_Block->data_aux;
            pData_Aux->error = Get_NV_Data()->error_code;
            episode_close = true;
        }

        // if there has been at least one patient connection
        if (my_event == eREG_ECG_PAT_GOOD_CONN)
        {
            patient_conn = true;
        }

        // if there has been at least one patient connection
        if (my_event == eREG_POWER_OFF)
        {
            episode_close = true;
        }

        // if there has been at least one patient connection and Wifi option is enabled
        if (my_event == eREG_WIFI_POWER_OFF)
        {
            episode_close = true;
            episode_send = true;
        }
    }

    // try to get the ECG and ZP samples ...
    win_rdy_ecg = patMon_Get_ECG_Window (first_sample, EPISODE_BLOCK_ECG_SZ, pEpi_Block->ecg);
    win_rdy_zp  = patMon_Get_ZP_Window  (first_sample,  EPISODE_BLOCK_ZP_SZ, pEpi_Block->zp);

    // if the windows are ready, register into the uSD
    // if the error code is a SAT type, force to register into the uSD
    if ((win_rdy_ecg && win_rdy_zp) || (my_event == eREG_SAT_ERROR))
    {
        // assign the episode block data
        pEpi_Block->time_stamp = episode_time;
        episode_time += EPISODE_BLOCK_ECG_SZ;
        pEpi_Block->zp_ohms = (uint16_t) patMon_Get_Zp_Ohms_mock();
        pEpi_Block->zp_cal  = patMon_Get_zp_CAL_ADCs();

        pData_Aux = (DB_DATA_DEFAULT_t *) &pEpi_Block->data_aux;

        pData_Aux->temperature = Battery_Get_Temperature();
        pData_Aux->battery_charge = Get_Rem_Charge();
        pData_Aux->battery_voltage = Defib_Get_Vbatt();
        pData_Aux->Vc = Defib_Get_Vc();
        pData_Aux->shock_energy = FSM_shock_energy;
        pData_Aux->rcp_time = FSM_RCP_time_secs;
        pData_Aux->rythm_type = (uint8_t) ((pEpi_Block->data_type == eBLOCK_DATA_DRD) ? drd_result.rhythm : eRHYTHM_UNKNOWN);
        memcpy ((uint8_t *) &pData_Aux->event_offset_time, event_timestamp, sizeof (event_timestamp));
        memcpy ((uint8_t *) &pData_Aux->event_id, event_buffer, sizeof (event_buffer));

        // get the mutex to access to the uSD
      //  fx_res = (uint8_t)tx_mutex_get(&usd_mutex, TX_WAIT_FOREVER);

        //fx_res = (uint8_t) fx_media_flush (sd_media);
        //APP_ERR_TRAP(fx_res);
        if(option != 1) Lock_on_Panic_mock(fx_res, 0);

        // proceed to write the episode block
       // fx_res = (uint8_t) fx_file_write (&epi_file, (uint8_t *) pEpi_Block, sizeof(DB_EPISODE_BLOCK_t));
        //APP_ERR_TRAP(fx_res);
        //if(fx_res != FX_SUCCESS) Lock_on_Panic(fx_res, 6);

        // ... flush data to physical media
        //fx_res = (uint8_t) fx_media_flush (sd_media);
        //APP_ERR_TRAP(fx_res);
        //if(fx_res != FX_SUCCESS) Lock_on_Panic(fx_res, 7);

        printf("Escribir en fichero\n");
        // release the uSD mutex
        //fx_res = (uint8_t)tx_mutex_put(&usd_mutex);

        if (episode_close)
        {
            // if no patient was connected at least once delete the episode file
            if (!patient_conn)
            {
          //      fx_res = (uint8_t) fx_file_close(&epi_file);
            //    fx_res = (uint8_t) fx_file_delete(sd_media, fullname);
                Trace_Arg_mock (TRACE_NEWLINE, "DELETE SAME EPISODE = %d", (uint32_t)fx_res);
              //  fx_res = (uint8_t) fx_media_flush(sd_media);
            }
            else
            {
                // close the episode
                //fx_res = fx_media_close (sd_media);
                //fx_res = (uint8_t) fx_file_close(&epi_file);

                // delete the next file in sequence for the next time
                if (fullname[19] == '1' && fullname[20] == '0')
                {
                    fullname[19] = fullname[20] = '0';
                }
                else if (fullname[20] == '9')
                {
                    fullname[19]++;
                    fullname[20] = '0';
                }
                else {
                    fullname[20]++;
                }
                //fx_res = (uint8_t) fx_file_delete (sd_media, fullname);
                Trace_Arg_mock (TRACE_NEWLINE, "DELETE NEXT EPISODE = %d", (uint32_t)fx_res);
                //fx_res = (uint8_t) fx_media_flush (sd_media);
            }

            if((episode_send) && (patient_conn))
            {
               // Resample episode to be sended
               DB_Episode_Resample_mock(epi_filename);

               if(Comm_Is_Wifi_Initialized() == FALSE)
               {
                   // Open wifi Comm.
                   Comm_Wifi_Open();
               }

               // 5 minutes timeout for initializing Wifi module
               tout = 50;//(OSTIME_60SEC*5);

               ref_timeout = tout;//; tx_time_get() + tout;

               while((Comm_Is_Wifi_Initialized() == FALSE))
               {
                   //tx_thread_sleep (OSTIME_50MSEC);
            	   printf("Sleep\n");
               }

               if(Comm_Is_Wifi_Initialized())
               {

                   Comm_Wifi_Is_Host_Alive();

                   // 3 minutes timeout for sending episode file
                   tout = 30;//(OSTIME_60SEC*5);

                    while((Comm_Is_Wifi_Host_Alive_Finished() == FALSE))
                    {
                        //tx_thread_sleep (OSTIME_500MSEC);
                    	printf("Sleep\n");
                    }

                    if(Comm_Get_Wifi_Host_Alive_Flag() == TRUE)
                    {
                       // 5 minutes timeout for sending episode file

                    	tout = 50;//(OSTIME_60SEC*5);

                       ref_timeout = tout;//tx_time_get() + tout;

                       Comm_Wifi_Send_Episode();

                       while( (Comm_Is_Wifi_Episode_Sended() == FALSE))
                       {
                           //tx_thread_sleep (OSTIME_500MSEC);
                           printf("Sleep\n");
                       }
                    }
               }

               episode_send_finish = TRUE;
            }
            else
            {
                episode_send_finish = TRUE;
            }

            //sd_media = (FX_MEDIA *) NULL;
            return FALSE;
        }

        // restart the block LL;
        pEpi_Block->data_type = eBLOCK_DATA_DEFAULT;
        event_idx = 0;
        memset (event_timestamp, 0, sizeof(event_timestamp));
        memset (event_buffer, 0, sizeof(event_buffer));

        // update the buffer index !!!
        first_sample += EPISODE_BLOCK_ECG_SZ;

        // if the file reach the limit ... close the episode and bye-bye
        block_counter++;
        if (block_counter >= NUM_BLOCKS_EPISODE)
        {
            //fx_res = fx_media_close (sd_media);
            //fx_res = (uint8_t) fx_file_close(&epi_file);
            //sd_media = (FX_MEDIA *) NULL;
            return FALSE;
        }
    }

    if (episode_close)
    {
        // if no patient was connected at least once delete the episode file
        if (!patient_conn)
        {
            //fx_res = (uint8_t) fx_file_close(&epi_file);
            //fx_res = (uint8_t) fx_file_delete(sd_media, fullname);
            Trace_Arg_mock (TRACE_NEWLINE, "DELETE SAME EPISODE = %d", (uint32_t)fx_res);
            //fx_res = (uint8_t) fx_media_flush(sd_media);
        }
        else
        {
            // close the episode
            //fx_res = fx_media_close (sd_media);
           // fx_res = (uint8_t) fx_file_close(&epi_file);

            // delete the next file in sequence for the next time
            if (fullname[19] == '1' && fullname[20] == '0')
            {
                fullname[19] = fullname[20] = '0';
            }
            else if (fullname[20] == '9')
            {
                fullname[19]++;
                fullname[20] = '0';
            }
            else {
                fullname[20]++;
            }
            //fx_res = (uint8_t) fx_file_delete (sd_media, fullname);
            Trace_Arg_mock (TRACE_NEWLINE, "DELETE NEXT EPISODE = %d", (uint32_t)fx_res);
        //    fx_res = (uint8_t) fx_media_flush (sd_media);
        }

        //sd_media = (FX_MEDIA *) NULL;
        return FALSE;
    }

    // the file has been written OK
    return TRUE;
}

/******************************************************************************
** Name:    DB_Episode_Set_Event
*****************************************************************************/
/**
** @brief   Writes an event in the episode block
**
** @param   event_id    [in] event identifier to register
**
** @return  void
**
******************************************************************************/
void DB_Episode_Set_Event_mock (REG_EVENTS_e event_id)
{
    printf("DB_Episode_Set_Event_mock\n");
	if (Is_Mode_Trainer()) return;

//Trace_Arg (TRACE_NEWLINE, "  *****************REC QUEUE AVAILABLE = %5d", (uint32_t) (queue_recorder.tx_queue_available_storage));
    if (queue_recorder.tx_queue_available_storage == 0) return;
   // tx_queue_send(&queue_recorder, &event_id, TX_NO_WAIT);
}

/******************************************************************************
** Name:    DB_Episode_Resample
*****************************************************************************/
/**
** @brief   Resamples an episode from 5:1
**
** @param   pName   pointer to the name of the episode to resample
**
** @return  void
**
******************************************************************************/
void DB_Episode_Resample_mock (char_t *pName)
{
    uint8_t  fx_res;                    // file system operation results
    uint8_t  end_file;                  // end of file
    uint32_t i;                         // global index
    uint32_t nBytes;                    // read size
    uint32_t my_strlen;                 // string length
    char_t   my_fullname [32];          // full name
    //FX_FILE my_epi_file;                // Episode file handler
    //FX_FILE my_wee_file;                // Wee file handler
    DB_RESAMPLE_BLOCK_t my_resample;    // resample buffer

    printf("DB_Episode_Resample_mock\n");
    if (!pName) return;

    //sd_media    = &sd_fx_media;    // initialize the SD media handler (just in case)
    pEpi_Block  = (DB_EPISODE_BLOCK_t *)  episode_buffer; // initialize episode buffer

    //////////////////////////////
    //////////////////////////////
    // open the episode file
    sprintf (my_fullname, "%s\\%s", EPI_FOLDER_NAME, pName);
    my_strlen = strlen (my_fullname);
    if ((my_fullname[my_strlen-4] != '.') ||
        (my_fullname[my_strlen-3] != 'E') ||
        (my_fullname[my_strlen-2] != 'P') ||
        (my_fullname[my_strlen-1] != 'I'))
    {
        return;
    }

    //fx_res = (uint8_t) fx_directory_name_test(sd_media, EPI_FOLDER_NAME);
  //  if (fx_res != FX_SUCCESS) return;

   // fx_res = (uint8_t) fx_file_open(sd_media, &my_epi_file, my_fullname, FX_OPEN_FOR_READ);
    if (fx_res != 1) return;

    //////////////////////////////
    //////////////////////////////
    // create the "wee" file
    my_fullname[my_strlen-3] = 'W';
    my_fullname[my_strlen-2] = 'E';
    my_fullname[my_strlen-1] = 'E';

    //fx_res = (uint8_t) fx_file_create(sd_media, my_fullname);
    if (fx_res != 1)
    {
      //  fx_res = (uint8_t) fx_file_close (&my_epi_file);
        return;
    }

    //fx_res = (uint8_t) fx_file_open(sd_media, &my_wee_file, my_fullname, FX_OPEN_FOR_WRITE);
    if (fx_res != 1)
    {
      //  fx_res = (uint8_t) fx_file_close (&my_epi_file);
        return;
    }

    //////////////////////////////
    //////////////////////////////
    // resample

    // read full episode header
    //fx_res = (uint8_t) fx_file_read  (&my_epi_file, (uint8_t *) pEpi_Block, sizeof(DB_EPISODE_HEADER_t), &nBytes);
    // write the episode header as it is
    printf("Leer Episodio\n");
    //fx_res = (uint8_t) fx_file_write (&my_wee_file, (uint8_t *) pEpi_Block, sizeof(DB_EPISODE_HEADER_t));
    printf("Escribir Episodio\n");
    // read and resample episode file
    do {

        //end_file = (uint8_t) fx_file_read (&my_epi_file, (uint8_t *) pEpi_Block, EPISODE_BLOCK_SZ, &nBytes);

        my_resample.time_stamp = pEpi_Block->time_stamp;
        my_resample.data_type = pEpi_Block->data_type;
        my_resample.zp_ohms = pEpi_Block->zp_ohms;
        my_resample.zp_cal = pEpi_Block->zp_cal;

        // decimate 1:5 ECG and ZP samples
        for (i=0; i<(EPISODE_BLOCK_ECG_SZ/5); i++)
        {
            my_resample.ecg[i] = pEpi_Block->ecg[i*5];
        }

        for (i=0; i<(EPISODE_BLOCK_ZP_SZ/5); i++)
        {
            my_resample.zp[i] = pEpi_Block->zp[i*5];
        }

        memcpy(&my_resample.data_aux, (uint8_t *) &pEpi_Block->data_aux, DATA_AUX_SIZE);

        //fx_res = (uint8_t) fx_file_write (&my_wee_file, (uint8_t *) &my_resample, 256);
        printf("Escribir \n");
        //APP_ERR_TRAP(fx_res);
        if(fx_res != 1) Lock_on_Panic_mock(fx_res, 8);

        // ... flush data to physical media
        //fx_res = (uint8_t) fx_media_flush (sd_media);
        //APP_ERR_TRAP(fx_res);
        if(fx_res != 1) Lock_on_Panic_mock(fx_res, 9);
        printf("EndOfFile?: %d \n", end_file);

    } while (end_file != 1);


    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // congratulations. work is done, proceed to close both files
    //fx_res = (uint8_t) fx_file_close (&my_epi_file);
    //fx_res = (uint8_t) fx_file_close (&my_wee_file);
    //fx_res = (uint8_t) fx_media_close (sd_media);
    //sd_media = (FX_MEDIA *) NULL;
}

/******************************************************************************
** Name:    DB_Episode_Get_Current_Filename
*****************************************************************************/
/**
** @brief   Returns the last Episode name
**
** @param   none
**
** @return  pointer to a test filename
**
******************************************************************************/
char_t* DB_Episode_Get_Current_Filename (void)
{
    return epi_filename;
}

/******************************************************************************
** Name:    DB_Episode_Set_Current_Filename
*****************************************************************************/
/**
** @brief   Sets an episode filename (for sending WEE files from Test)
**
** @param   none
**
** @return  none
**
******************************************************************************/
void DB_Episode_Set_Current_Filename (char_t *pName)
{
    memset (epi_filename, 0, sizeof(epi_filename));

    memcpy(epi_filename,pName, sizeof(epi_filename));
}

/******************************************************************************
** Name:    DB_Is_Episode_Sent
*****************************************************************************/
/**
** @brief   checks if a episode has been send to the external server
**
** @param   none
**
** @return  True if sent, FALSE if not
**
******************************************************************************/
extern  bool_t  DB_Is_Episode_Sent (void)
{
    return episode_send_finish;
}

/******************************************************************************
** Name:    Check_Episode_Present
*****************************************************************************/
/**
** @brief   checks if a episode is present in the folder
**
** @param   pName: pointer to a filename to raster
**
** @return  True if present, FALSE if not
**
******************************************************************************/
bool_t Check_Episode_Present_mock (char_t *pName)
{
    uint32_t    attributes, fx_res;
    bool_t      result = FALSE;

    printf("Check_Episode_Present_mock\n");

    //sd_media = &sd_fx_media;        // initialize the SD media handler

    // Set the default directory to EPI folder
    //fx_res = (uint8_t) fx_directory_default_set(sd_media, EPI_FOLDER_NAME);
    if (fx_res != 1)
    {
        return FALSE;
    }

    //fx_res = (uint8_t) fx_file_attributes_read (sd_media, pName, (UINT *)&attributes);
    printf("Check Episode present, read");
    if ((fx_res != 1) || !(attributes & 1))
    {
        result = FALSE;
    }
    else
    {
        result = TRUE;
    }

    // Reset the default directory to root
    //fx_res = (uint8_t) fx_directory_default_set(sd_media, FX_NULL);
    return result;
}


/******************************************************************************
** Name:    DB_Episode_Delete
*****************************************************************************/
/**
** @brief   deletes a episode
**
** @param   pName: pointer to a filename
**
** @return  none
**
******************************************************************************/
void DB_Episode_Delete_mock (char_t *pName)
{
    uint32_t    fx_res;

    printf("Check_Episode_Present_mock\n");
    //sd_media = &sd_fx_media;        // initialize the SD media handler

    // Set the default directory to EPI folder
    //fx_res = (uint8_t) fx_directory_default_set(sd_media, EPI_FOLDER_NAME);
    if (fx_res != 1)
    {
        return;
    }

    //fx_res = (uint8_t) fx_file_delete (sd_media, pName);
    //fx_res = (uint8_t) fx_media_flush (sd_media);

    // Reset the default directory to root
    //fx_res = (uint8_t) fx_directory_default_set(sd_media, FX_NULL);
}
