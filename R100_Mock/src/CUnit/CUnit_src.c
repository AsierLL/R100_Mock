/******************************************/

#include "CUnit_header.h"

#include <stdio.h>
#include <stdlib.h>


#include "time.h"

/******************************************/


void test_Fill_Test_Date(char *pStr){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char_t date[12];
    date[0]= '2';
    date[1]= '0';
    date[2]= ((tm.tm_year + 1900)%100)/10;
    date[3]= (tm.tm_year + 1900)%10;
    date[4]= '-';
    date[5]= (tm.tm_mon + 1)/10;
    date[6]= (tm.tm_mon + 1)%10;
    date[7]= '-';
    date[8]= tm.tm_mday / 10;
    date[9]= tm.tm_mday % 10;
    date[10] = 0;

    Fill_Test_Date(pStr);

    CU_ASSERT_STRING_EQUAL(pStr, date);

}

void  test_Fill_Test_Time(char *pStr){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char_t time[10];
    time[0]= tm.tm_hour / 10;
    time[1]= tm.tm_hour % 10;
    time[2]= ':';
    time[3]= tm.tm_min / 10;
    time[4]= tm.tm_min % 10;
    time[5]= ':';
    time[6]= tm.tm_sec / 10;
    time[7]= tm.tm_sec % 10;
    time[8] = 0;

    Fill_Test_Time(pStr);

    CU_ASSERT_STRING_EQUAL(pStr, time);

}

static CU_TestInfo tests_Check_Test_Date_Time[] = {

     // Register test case to test suite
       {"test_Check_Test_Date", (void *) test_Fill_Test_Date},
       {"test_Check_Test_Time", (void *) test_Fill_Test_Time},

       CU_TEST_INFO_NULL,
};


/************ Core ************/

void test_Is_Test_Mode_Montador(){
	//CONFIGURAR, FORZAR VALOR?
    CU_ASSERT_EQUAL(0 , Is_Test_Mode_Montador());
}

void test_USB_Device_Connected(){
	//CONFIGURAR, FORZAR VALOR?
    CU_ASSERT_EQUAL(1 , USB_Device_Connected_mock());
}

void test_Is_Test_Mode(){
	//CONFIGURAR, FORZAR VALOR?
    CU_ASSERT_EQUAL(0 , Is_Test_Mode());
}

void test_Is_Sysmon_Task_Initialized(){
	//CONFIGURAR, FORZAR VALOR?
    CU_ASSERT_EQUAL(0 , Is_Sysmon_Task_Initialized());
}

void test_Is_Task_Audio_Enabled(){
    CU_ASSERT_EQUAL(0 , Is_Task_Audio_Enabled());
}

static CU_TestInfo tests_threat_core[] = {

     // Register test case to test suite
     {"test_Is_Test_Mode_Montador", test_Is_Test_Mode_Montador},
     {"test_USB_Device_Connected", test_USB_Device_Connected},
     {"test_Is_Test_Mode", test_Is_Test_Mode},
     {"test_Is_Sysmon_Task_Initialized", test_Is_Sysmon_Task_Initialized},
     {"test_Is_Task_Audio_Enabled", test_Is_Task_Audio_Enabled},

       CU_TEST_INFO_NULL,
};


/******************************/

/************ drd ************/
/*
static CU_TestInfo tests_threat_drd[] = {

     // Register test case to test suite
     // NO TIENE FUNCINOES DE INICIALIZACION

       CU_TEST_INFO_NULL,
};
*/
/*****************************/

/************ patMon ************/
/*
static CU_TestInfo tests_threat_patMon[] = {

     // Register test case to test suite
     // patMon_HAL_Init
        //  ADS_Init --> dEVUELVE ERROR_ID_e --> Las funciones en su interior no devuelven nada
       CU_TEST_INFO_NULL,
};

/********************************/

/************ hmi ************/
/*
static CU_TestInfo tests_threat_hmi[] = {

     // Register test case to test suite
     // NO TIENE FUNCIONES DE INICIALIZACION

       CU_TEST_INFO_NULL,
};

/*****************************/

/************ sysMon ************/

void test_Identify_PowerOn_Event(){
   // g_ioport.p_api->pinWrite(EXEC_TEST, IOPORT_LEVEL_LOW);

    int result = Identify_PowerOn_Event_mock();
    CU_ASSERT_EQUAL(PON_TEST_MONTADOR , result);


  //  g_ioport.p_api->pinWrite(KEY_ONOFF, 0);
    CU_ASSERT_EQUAL(PON_TEST_MONTADOR , Identify_PowerOn_Event_mock());

  //  g_ioport.p_api->pinWrite(KEY_COVER, 0);
    CU_ASSERT_EQUAL(PON_TEST_MONTADOR , Identify_PowerOn_Event_mock());

 //   g_ioport.p_api->pinWrite(RTCIO_PIN, 0);
    CU_ASSERT_EQUAL(PON_TEST_MONTADOR , Identify_PowerOn_Event_mock());

  //  g_ioport.p_api->pinWrite(VBUS_DET, 0);
    CU_ASSERT_EQUAL(PON_TEST_MONTADOR , Identify_PowerOn_Event_mock());

  //  g_ioport.p_api->pinWrite(KEY_SHOCK, 0);
    CU_ASSERT_EQUAL(PON_TEST_MONTADOR , Identify_PowerOn_Event_mock());

 //   g_ioport.p_api->pinWrite(KEY_PATYPE, 0);
    CU_ASSERT_EQUAL(PON_TEST_MONTADOR , Identify_PowerOn_Event_mock());
}


/*
void test_Load_FW_File(){
    CU_ASSERT_EQUAL(1 , Load_FW_File());
}

*/

void test_Is_SAT_Error(){
    int i=0;
        while (i<MAX_ERRORS)
        {

            if(R100_Errors_cfg[i].warning_error==1){
                //Se deberia comprobar que el valor que le pasamos esta dentro del rango 0-254
                CU_ASSERT_EQUAL(true , Is_SAT_Error_mock(i));
            //Debería de encender los leds correspondientes a ese error
            }
            else{

                CU_ASSERT_EQUAL(false , Is_SAT_Error_mock(i));
            }
            i++;
        }
        // Se consideran "warning" pero no se deberian pasar argumentos fuera del rango.
        CU_ASSERT_EQUAL(true , Is_SAT_Error_mock(300));
//        CU_ASSERT_EQUAL(true , Is_SAT_Error(-2));

}

/*void test_Settings_Open_From_uSD(){
    CU_ASSERT_EQUAL(Settings_Open_From_uSD(), 0); // Se espera 0, porque hasta ahora no hace bien la parte de leer del SD
}
*/

/*
void test_Check_Battery_Voltage(){
    int my_temperature = 10;

    uint32_t vLimit_safe  = ((VB_25C_SAFE_BATTERY - VB_00C_SAFE_BATTERY) * (uint32_t) my_temperature) / 25;
    vLimit_safe += VB_00C_SAFE_BATTERY;

    CU_ASSERT_EQUAL(VB_00C_SAFE_BATTERY , Check_Battery_Voltage(Defib_Get_Vbatt(), 0,NULL));
    CU_ASSERT_EQUAL(VB_25C_SAFE_BATTERY , Check_Battery_Voltage(Defib_Get_Vbatt(), 25,NULL));
    CU_ASSERT_EQUAL(vLimit_safe , Check_Battery_Voltage(Defib_Get_Vbatt(), 10,NULL));
}*/
/*void test_NFC_Write_Device_Info(){
    CU_ASSERT_EQUAL(NFC_Write_Device_Info(0), 0);
//    CU_ASSERT_EQUAL(NFC_Write_Device_Info(0), 1); --> La funcion "fx_media_extended_space_available" de momento no va a funcionar correctamente.
    CU_ASSERT_EQUAL(NFC_Write_Device_Info(1), 2);
}


void test_NV_Data_Write(){
    NV_DATA_t CU_test_data;
    CU_ASSERT_EQUAL(NV_Data_Write(NULL), 0);

    // Falta comprobar la parte del memcmp
}*/
/*
 * void test_NFC_Write_Settings(){

    static NFC_SETTINGS_t CU_nfc_settings, result;

   // CU_TX_WAIT = 0;

    //Settings_Swap(&CU_nfc_settings);
    //NFC_Write ((uint8_t *)&CU_nfc_settings, sizeof (NFC_SETTINGS_t), NFC_DEVICE_SETTINGS_BLOCK);
    //Settings_Open_From_NFC();

    result = getNFC_Settings();
    NFC_Write_Settings();
    //NFC_Write();
//    CU_nfc_settings = getNFC_Settings();
    NFC_Read ((uint8_t *)&CU_nfc_settings, sizeof(NFC_SETTINGS_t), NFC_DEVICE_SETTINGS_BLOCK);
    Settings_Safe_Load(); // static --> extern
	CU_ASSERT_EQUAL(CU_nfc_settings.analysis_cpr, result.analysis_cpr);           ///< Enables start an analysis cycle in CPR
    CU_ASSERT_EQUAL(CU_nfc_settings.asys_detect, result.asys_detect);            ///< Enables Asystole warning message
    CU_ASSERT_EQUAL(CU_nfc_settings.asys_time, result.asys_time);              ///< Time on asystole before emitting asystole warning message

    CU_ASSERT_EQUAL(CU_nfc_settings.metronome_en, result.metronome_en);           ///< Enables metronome for marking compression pace
    CU_ASSERT_EQUAL(CU_nfc_settings.metronome_ratio_a, result.metronome_ratio_a);      ///< Metronome pace for adult
    CU_ASSERT_EQUAL(CU_nfc_settings.metronome_ratio_p, result.metronome_ratio_p);      ///< Metronome pace for pediatric
    CU_ASSERT_EQUAL(CU_nfc_settings.metronome_rate, result.metronome_rate);         ///< Metronome pace for pediatric
    CU_ASSERT_EQUAL(CU_nfc_settings.feedback_en, result.feedback_en);            ///< Enables cpr feedback for marking compression pace

    CU_ASSERT_EQUAL(CU_nfc_settings.consec_shocks, result.consec_shocks);          ///< Max Shocks in a Shock Series
    CU_ASSERT_EQUAL(CU_nfc_settings.energy_shock1_a, result.energy_shock1_a);        ///< Energy for 1st shock in shock series for adult
    CU_ASSERT_EQUAL(CU_nfc_settings.energy_shock2_a, result.energy_shock2_a);        ///< Energy for 2nd shock in shock series for adult
    CU_ASSERT_EQUAL(CU_nfc_settings.energy_shock3_a, result.energy_shock3_a);        ///< Energy for 3rd shock in shock series for adult
    CU_ASSERT_EQUAL(CU_nfc_settings.energy_shock1_p, result.energy_shock1_p);        ///< Energy for 1st shock in shock series for pediatric
    CU_ASSERT_EQUAL(CU_nfc_settings.energy_shock2_p, result.energy_shock2_p);        ///< Energy for 2nd shock in shock series for pediatric
    CU_ASSERT_EQUAL(CU_nfc_settings.energy_shock3_p, result.energy_shock3_p);        ///< Energy for 3rd shock in shock series for pediatric

    CU_ASSERT_EQUAL(CU_nfc_settings.patient_type, result.patient_type);           ///< Patient type (0-pediatric 1-adult)
    CU_ASSERT_EQUAL(CU_nfc_settings.mains_filter, result.mains_filter);           ///< mains filter (0-none 1-50 Hz 2-60 Hz)
    CU_ASSERT_EQUAL(CU_nfc_settings.language, result.language);               ///< device audio interface language
    CU_ASSERT_EQUAL(CU_nfc_settings.utc_time, result.utc_time);                ///< UTC time
    CU_ASSERT_EQUAL(CU_nfc_settings.audio_volume, result.audio_volume);           ///< device audio volume
    CU_ASSERT_EQUAL(CU_nfc_settings.trainer_mode, result.trainer_mode);
    CU_ASSERT_EQUAL(CU_nfc_settings.patient_alert, result.patient_alert);
    CU_ASSERT_EQUAL(CU_nfc_settings.movement_alert, result.movement_alert);
    CU_ASSERT_EQUAL(CU_nfc_settings.transmis_mode, result.transmis_mode);
    CU_ASSERT_EQUAL(CU_nfc_settings.warning_alert, result.warning_alert);

   // CU_TX_WAIT = 0xFFFFFFFFUL;
//  LA COMPROBACION TENDRIA QUE SER COMPARAR LOS VALORES NUEVOS QUE TENGA LA VARIABLE "cu_nfc_settings" CON LOS //   VALORES DE LA VARIABLE result añadiendoles los cambios que se hacen en la funcion que se está testeando (EN este caso la funcion SWAP).
    CU_ASSERT_EQUAL(CU_nfc_settings.aCPR_init, result.aCPR_init);              ///< adult --> Initial CPR time in seconds (can be 0)
    CU_ASSERT_EQUAL(CU_nfc_settings.aCPR_1, result.aCPR_1);                 ///< adult --> After Shock Series CPR time in seconds (can be OFF)
    CU_ASSERT_EQUAL(CU_nfc_settings.aCPR_2, result.aCPR_2);                 ///< adult --> No Shock Advised CPR time in seconds (can be OFF)
    CU_ASSERT_EQUAL(CU_nfc_settings.pCPR_init, result.pCPR_init);              ///< pediatric --> Initial CPR time in seconds (can be 0)
    CU_ASSERT_EQUAL(CU_nfc_settings.pCPR_1, result.pCPR_1);                 ///< pediatric --> After Shock Series CPR time in seconds (can be OFF)
    CU_ASSERT_EQUAL(CU_nfc_settings.pCPR_2, result.pCPR_2);                 ///< pediatric --> No Shock Advised CPR time in seconds (can be OFF)
    CU_ASSERT_EQUAL(CU_nfc_settings.cpr_msg_long, result.cpr_msg_long);           ///< Long/Short audio messages for CPR
}*/

void test_NFC_Write_Device_ID(){
UINT err;
   //err = tx_thread_create (&thread_sysMon, (CHAR *) "Thread SysMon", thread_sysMon_func, (ULONG) NULL,
     //                      &thread_sysMon_stack, 4096, 20, 20, 1, TX_AUTO_START);
// Create thread 1
//	pthread_create(&thread_sysMon, NULL, thread_sysMon_entry_mock, NULL);
//	pthread_join(thread_sysMon, NULL);
 /*  if (TX_SUCCESS != err)
   {
       tx_startup_err_callback (&thread_sysMon, 0);
   }*/
//tx_thread_wait_abort(thread_sysMon, NULL);  //aquí esperamos a que este hilo (el que es capaz de cambiar la variable, no?) termine y luego vendría el resto.
//CU_ASSERT_EQUAL(NFC_Write_Device_Info(0), 0);
//    CU_ASSERT_EQUAL(NFC_Write_Device_Info(0), 1); --> La funcion "fx_media_extended_space_available" de momento no va a funcionar correctamente.
  // CU_ASSERT_EQUAL(NFC_Write_Device_Info(1), 2);

}
/*
void test_R100_Startup(){
    CU_ASSERT_EQUAL(PON_RTC , R100_Startup());
}
*/
static CU_TestInfo tests_threat_sysMon[] = {

     // Register test case to test suite
       {"test_Identify_PowerOn_Event", test_Identify_PowerOn_Event},
//       {"test_Load_FW_File", test_Load_FW_File},
       {"test_Is_SAT_Error", test_Is_SAT_Error},//**OK
//       {"test_Settings_Open_From_uSD", test_Settings_Open_From_uSD},
  //     {"test_Check_Battery_Voltage", test_Check_Battery_Voltage},//**OK
//       {"test_NFC_Write_Device_Info", test_NFC_Write_Device_Info},
 //        {"test_NFC_Write_Settings", test_NFC_Write_Settings},//++ERROR++
//         {"test_NV_Data_Write", test_NV_Data_Write},
 //       {"test_NV_Data_Read", test_NV_Data_Read},**OK
//         {"test_Electrodes_Get_Signature", test_Electrodes_Get_Signature},**OK
//         {"test_NFC_Write_Device_ID", test_NFC_Write_Device_ID},++ERROR++
//       {"test_R100_Startup", test_R100_Startup},
       CU_TEST_INFO_NULL,
};

/********************************/

/************ recorder ************/
/*
void test_Is_Mode_Trainer(){
    CU_ASSERT_EQUAL(1 , 1);
}

void test_DB_Episode_Create(){
    CU_ASSERT_EQUAL(1 , 1);
}

static CU_TestInfo tests_threat_recorder[] = {

     // Register test case to test suite
     /*
      * Is_Sysmon_Task_Initialized --> test realizado antes
      * Is_Task_Audio_Enabled --> test realizado antes
      * Is_Mode_Trainer
      * DB_Episode_Create

    {"test_Is_Sysmon_Task_Initialized", test_Is_Sysmon_Task_Initialized},
    {"test_Is_Task_Audio_Enabled", test_Is_Task_Audio_Enabled},
    {"test_Is_Mode_Trainer", test_Is_Mode_Trainer},
    {"test_DB_Episode_Create", test_DB_Episode_Create},


    CU_TEST_INFO_NULL,
};

/**********************************/

/************ audio ************/

void test_Audio_Message(){
    ERROR_ID_e result;

    // MIRAR A MANO QUE ES LO QUE DEBERIAN DEVOLVER?
    // COMPROBAR REPETIDOS

    // Audio Test
/*    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , Audio_Message(eAUDIO_CMD_PLAY, msg_id, TRUE));    */

    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_MODE_TEST, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // Check Electrode Type
    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_ELECTRODES, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);
/*    Load_Audio_Resources();
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_ELECTRODES, TRUE)); */

    // Execute Calibration          / Execute_Test_Montador / Execute_Test
    result = Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_CALL_SAT, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    result = Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_DEVICE_READY, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    result = Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_CONNECT_ELECTRODES_DEVICE, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // GPS / WIFI / SIGFOX / WIRELESS
    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_ONGOING, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_TRANSMISSION_DONE, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // BATTERY / DEFIB
    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_REPLACE_BATTERY, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_LOW_BATTERY, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // Execute_Test_Electrodes / Execute_Test_Patmon
    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_DISCONNECT_PATIENT, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_CONNECT_ELECTRODES_DEVICE, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // Execute Test Montador
    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_DEVICE_READY, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // Execute_Test
    result = Audio_Message(eAUDIO_CMD_CONCAT, eAUDIO_TRAINER_MODE, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // Load_FW_File / Load_Gold_File
    result = Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_UPGRADING, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // Power_Off
    result = Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_TRAINER_MODE, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_DISCONNECT_PATIENT, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    Audio_Message(eAUDIO_CMD_STOP, eAUDIO_ASYSTOLE, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // Check Cover
    result = Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_OPEN_COVER, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // Check SAT
    result = Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_REPLACE_BATTERY, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // CHeck Warning
    result = Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_TONE, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);

    // R100_Startup
    result = Audio_Message(eAUDIO_CMD_PLAY, eAUDIO_UPGRADE_ERROR, TRUE);
    CU_ASSERT_EQUAL(eERR_AUDIO_QUEUE_FULL , result);
}

// Manage Audio Queue  --> void

/*
void test_Load_Audio_Resources(){
//    CU_ASSERT_EQUAL(eERR_NONE , Load_Audio_Resources());
    CU_ASSERT_EQUAL(eERR_AUDIO_MAX , Load_Audio_Resources());
}

void test_Audio_Init(){
    CU_ASSERT_EQUAL(1 , 1);
}
*/

static CU_TestInfo tests_threat_audio[] = {

     // Register test case to test suite
     // Es la unica funcion de Inicializacion del hilo --> Es void
//     {"test_Audio_Init", test_Audio_Init},
       {"test_Audio_Message", test_Audio_Message},
//       {"test_Load_Audio_Resources", test_Load_Audio_Resources},
       CU_TEST_INFO_NULL,
};

/*******************************/

/************ defibrillator ************/
/*
static CU_TestInfo tests_threat_defibrillator[] = {

     // Register test case to test suite
     /*
      * Esta es la unica funcino de inicializacion
      *     Defib_Initialize --> Devuelve un ERROR_ID_e pero no se recoge
      *         Defib_Pulse_Off
      *
       CU_TEST_INFO_NULL,
};

/***************************************/

/************ comm ************/
/*
static CU_TestInfo tests_threat_comm[] = {

     // Register test case to test suite
     /*
      * Esta es la unica funcion de inicializacion
      *     Comm_Modules_Initialize
      *         Comm_Uart_Init
      *         Comm_Uart_Set_Baud_Rate --> Devuelve un ERROR_ID_e

       CU_TEST_INFO_NULL,
};

/******************************/
void test01(){
	CU_ASSERT_EQUAL(2, 2);
}

static CU_TestInfo githubActions[] = {

		{"GitHub - Test01", NULL, NULL, test01},

       CU_TEST_INFO_NULL,
};

/************ TEST SUITE ************/

static CU_SuiteInfo suites[] = {
		{"GitHub Actions", NULL, NULL, githubActions},
		//{"TestSimpleAssert_Test_core", NULL, NULL, tests_threat_core},
		// {"TestSimpleAssert_Test_sysMon", NULL, NULL, tests_threat_sysMon},
        // {"TestSimpleAssert_TestDateTime", NULL, NULL, tests_Check_Test_Date_Time},
		// {"TestSimpleAssert_Test_audio", NULL, NULL, tests_threat_audio},
/*
        {"TestSimpleAssert_Test_drd", NULL, NULL, tests_threat_drd},
        {"TestSimpleAssert_Test_patMon", NULL, NULL, tests_threat_patMon},
        {"TestSimpleAssert_Test_hmi", NULL, NULL, tests_threat_hmi},
        {"TestSimpleAssert_Test_recorder", NULL, NULL, tests_threat_recorder},
        {"TestSimpleAssert_Test_defibrillator", NULL, NULL, tests_threat_defibrillator},
        {"TestSimpleAssert_Test_comm", NULL, NULL, tests_threat_comm},
*/
        CU_SUITE_INFO_NULL,
};

DWORD WINAPI mockThreadFunction(LPVOID lpParam) {
	printf("Sleep MockThread...");
	Sleep(100);
    return 0;
}


void test_CUnit(void) {

// Retrieve a pointer to the current test registry
    assert(NULL != CU_get_registry());

// Flag for whether a test run is in progress
    assert(!CU_is_test_running());

// Register the suites in a single CU_SuiteInfo array
    if (CU_register_suites(suites) != CUE_SUCCESS) {

        // Get the error message
        printf("Suite registration failed - %s\n", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}

void AddTest(void) {

// Retrieve a pointer to the current test registry
  //  assert(NULL != CU_get_registry());

// Flag for whether a test run is in progress
    assert(!CU_is_test_running());


// Register the suites in a single CU_SuiteInfo array
    if (CU_register_suites(suites) != CUE_SUCCESS) {

        // Get the error message
        printf("Suite registration failed - %s\n", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }

}

/******************************************/


