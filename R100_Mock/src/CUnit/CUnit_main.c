#include "CUnit_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*int CUnit_main(void);
extern void test_Write_File();
void Inicializacion(char * fileName);

CU_ErrorCode initialize_result_file(char *, FILE*);
CU_ErrorCode uninitialize_result_file(FILE*);
CU_ErrorCode automated_list_all_tests(CU_pTestRegistry, char *, FILE*);
FILE  *   result_file;
FILE * f_pTestResultFile_;
*/

int CUnit_main(void){

	 if (CU_initialize_registry()) {
	      printf("\nInitialization of Test Registry failed.");
	    }
	    else {
	      AddTest();
	      CU_set_output_filename("TestAutomated");
	      CU_list_tests_to_file();
	      CU_automated_run_tests();
	      CU_cleanup_registry();
	    }
return 0;
}


    // Define the run mode for the basic interface
    // Verbose mode - maximum output of run details
    //    CU_BasicRunMode mode = CU_BRM_VERBOSE;

    // Define error action
    // Runs should be continued when an error condition occurs (if possible)
    //    CU_ErrorAction error_action = CUEA_IGNORE;

    // Initialize the framework test registry
    /*    if (CU_initialize_registry()) {
            printf("Initialization of Test Registry failed.\n");
        }
        else
        {
            // Call add test function
            test_Write_File();
           // test_CUnit();

            // Set the basic run mode, which controls the output during test
            CU_basic_set_mode(mode);

            // Set the error action
            CU_set_error_action(error_action);

            // Run all tests in all registered suites
//            printf("Tests completed with return value %d.\n",
//              CU_basic_run_tests());
            // Run all tests in all registered suites
       //     printf("Tests completed with return value %d.\n", CU_basic_run_tests());

        //    result = CU_basic_run_tests();

                      /*
                       *  Inicalizacion y cabecera antes
                       *
                       *  llamar CU_automated_run_tests
                       *      Dejar los handlers
                       *      automated_run_all_tests --> borrar la cabecera
                       *
                       *  Abrir fichero antes
                       *  llamar CU_list_tests_to_file
                       *      automated list all tests
                       *      se puede sacar la primera parte
                       *      crear las funciones que devuelvan valores concretos para hacer el resumen, estas fucniones se llaman desde nuestro proyecto
                       *
                       *
                       */
/*                      char f_szTestResultFileName[20] = "TestResults\0";

                      Inicializacion(f_szTestResultFileName);
                      printf("Test-ak aurretik");
                                            fflush(0);
                                            getchar();
                      automated_list_all_tests(CU_get_registry(), f_szTestResultFileName, result_file);
                      printf("Test-ak aurretik");
                      getchar();
                      CU_automated_run_tests();
                      printf("Test-ak eta gero");
                      getchar();
                      CU_list_tests_to_file(); //--> Llamar directamente a automated_list_all_tests
                      //i = result;
            // Clean up and release memory used by the framework
            CU_cleanup_registry();*/



/*void Inicializacion(char *fileName){
    int i = 0;
    result_file = NULL;

    assert(NULL != CU_get_registry());

    /* Ensure output makes it to screen at the moment of a SIGSEGV. */
/* Comentario mio    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    /* if a filename root hasn't been set, use the default one */
//    if (0 == strlen(fileName)) {
  //    CU_set_output_filename(fileName);
   // }

    //if (CUE_SUCCESS != initialize_result_file(fileName)) {

  //  if (CUE_SUCCESS != initialize_result_file(fileName, result_file)) {

        //DYNAMIC output

    //    i = 1;
  //    fprintf(stderr, "\n%s", "ERROR - Failed to create/initialize the result file.");
 //   }
  //  else {

//        CU_automated_run_tests();

     //   if (CUE_SUCCESS != uninitialize_result_file(result_file)) {
   //     if (CUE_SUCCESS != uninitialize_result_file(result_file)) {
            //DYNAMIC output
 //        fprintf(stderr, "\n%s", "ERROR - Failed to close/uninitialize the result files.");
   /*     }
    }
}

CU_ErrorCode initialize_result_file(char *szFilename, FILE *pFile)
    {
      uint32_t status;
      CU_set_error(CUE_SUCCESS);
      char  directoryName[20];
     char fileName[20];
     char str[512];
     int i = 0;

     strcpy(directoryName,"TestResults");
     strcpy(fileName, szFilename);

   //  status = (uint8_t) fx_media_init0_open();
   //  Sleep(100); //Definido en types_basic.h en el otro proyecto. He puesto la definicion de lo necesario en Automated.h

    // status = (uint8_t) fx_directory_create(&sd_fx_media, directoryName);

      if ((NULL == szFilename) || (strlen(szFilename) == 0)) {
        i = 1;

        CU_set_error(CUE_BAD_FILENAME);
      }
    //  else if (NULL == (f_pTestResultFile = fopen(szFilename, "w"))) {

      else if (NULL == (pFile= fopen(fileName, "w"))) {

    	  i = 2;
          CU_set_error(CUE_FOPEN_FAILED);
      }
      else {
          i = 3;
        //setvbuf(f_pTestResultFile, NULL, _IONBF, 0);

   /*     fprintf(pFile,
                "<?xml version=\"1.0\" ?> \n"
                "<?xml-stylesheet type=\"text/xsl\" href=\"CUnit-Run.xsl\" ?> \n"
                "<!DOCTYPE CUNIT_TEST_RUN_REPORT SYSTEM \"CUnit-Run.dtd\"> \n"
                "<CUNIT_TEST_RUN_REPORT> \n"
                "  <CUNIT_HEADER/> \n");*/

     /*           int how_much_space = snprintf(NULL, 0, "%s","<?xml version=\"1.0\" ?> \n"
                                      "<?xml-stylesheet type=\"text/xsl\" href=\"CUnit-Run.xsl\" ?> \n"
                                      "<!DOCTYPE CUNIT_TEST_RUN_REPORT SYSTEM \"CUnit-Run.dtd\"> \n"
                                      "<CUNIT_TEST_RUN_REPORT> \n"
                                      "  <CUNIT_HEADER/> \n");
                snprintf(str,how_much_space,"%s",
                   "<?xml version=\"1.0\" ?> \n"
                   "<?xml-stylesheet type=\"text/xsl\" href=\"CUnit-Run.xsl\" ?> \n"
                   "<!DOCTYPE CUNIT_TEST_RUN_REPORT SYSTEM \"CUnit-Run.dtd\"> \n"
                   "<CUNIT_TEST_RUN_REPORT> \n"
                   "  <CUNIT_HEADER/> \n");
        //fwrite(pFile, str, how_much_space);
         fprintf(pFile,str);
      }

      return CU_get_error();
    }

   CU_ErrorCode uninitialize_result_file(FILE *pFile)
    {
      char* szTime;
      time_t tTime = 0;
      int how_much_space;
      char str[256];

      //assert(NULL != f_pTestResultFile);
      assert(NULL != pFile);

      CU_set_error(CUE_SUCCESS);

      time(&tTime);
      szTime = ctime(&tTime);
    /* fprintf(pFile,
              "  <CUNIT_FOOTER> %s" CU_VERSION " - %s </CUNIT_FOOTER> \n"
              "</CUNIT_TEST_RUN_REPORT>",
              "File Generated By CUnit v",
              (NULL != szTime) ? szTime : "");*/
   /*  how_much_space = snprintf(NULL, 0, "%s",    "  <CUNIT_FOOTER> %s" CU_VERSION " - %s </CUNIT_FOOTER> \n"
              "</CUNIT_TEST_RUN_REPORT>",
              "File Generated By CUnit v",
               (NULL != szTime) ? szTime : "");
     snprintf(str,how_much_space,"%s",
              "  <CUNIT_FOOTER> %s" CU_VERSION " - %s </CUNIT_FOOTER> \n"
              "</CUNIT_TEST_RUN_REPORT>",
              "File Generated By CUnit v",
			  (NULL != szTime) ? szTime : "");
                    // fwrite(pFile,str, how_much_space);
      fprintf(pFile,str);
      //if (0 != fclose(f_pTestResultFile)) {
      if (fclose(pFile)) {

        CU_set_error(CUE_FCLOSE_FAILED);
      }

      return CU_get_error();
    }

CU_ErrorCode automated_list_all_tests(CU_pTestRegistry pRegistry, char *szFilename, FILE *pFile)
{
      CU_pSuite pSuite = NULL;
      CU_pTest  pTest = NULL;
      uint8_t status;
      //FILE* pTestListFile = NULL;
      FILE * pTestListFile = NULL;
      char* szTime;
      time_t tTime = 0;
      int i = 0;
      int how_much_space;
              char str[256];

      CU_set_error(CUE_SUCCESS);

      if (NULL == pRegistry) {
        i = 1;
        CU_set_error(CUE_NOREGISTRY);
      }
      else if ((NULL == szFilename) || (0 == strlen(szFilename))) {
        i = 2;
        CU_set_error(CUE_BAD_FILENAME);
      }
    //  else if (FX_SUCCESS != (status = fx_file_open(&sd_fx_media, pFile,szFilename, FX_OPEN_FOR_WRITE))) {
      else if (NULL == (pFile= fopen(szFilename, "w"))) {
          i = 3;
        CU_set_error(CUE_FOPEN_FAILED);
      }
      else {
       // setvbuf(pTestListFile, NULL, _IONBF, 0);
        i = 4;
       /*fprintf(pTestListFile,
                "<?xml version=\"1.0\" ?> \n"
                "<?xml-stylesheet type=\"text/xsl\" href=\"CUnit-List.xsl\" ?> \n"
                "<!DOCTYPE CUNIT_TEST_LIST_REPORT SYSTEM \"CUnit-List.dtd\"> \n"
                "<CUNIT_TEST_LIST_REPORT> \n"
                "  <CUNIT_HEADER/> \n"
                "  <CUNIT_LIST_TOTAL_SUMMARY> \n");*/
/*         how_much_space = snprintf(NULL, 0, "%s",     "<?xml version=\"1.0\" ?> \n"
                                  "<?xml-stylesheet type=\"text/xsl\" href=\"CUnit-List.xsl\" ?> \n"
                "<!DOCTYPE CUNIT_TEST_LIST_REPORT SYSTEM \"CUnit-List.dtd\"> \n"
                "<CUNIT_TEST_LIST_REPORT> \n"
                "  <CUNIT_HEADER/> \n"
                "  <CUNIT_LIST_TOTAL_SUMMARY> \n");
         snprintf(str,how_much_space,"%s",
                "<?xml version=\"1.0\" ?> \n"
                "<?xml-stylesheet type=\"text/xsl\" href=\"CUnit-List.xsl\" ?> \n"
                "<!DOCTYPE CUNIT_TEST_LIST_REPORT SYSTEM \"CUnit-List.dtd\"> \n"
                "<CUNIT_TEST_LIST_REPORT> \n"
                "  <CUNIT_HEADER/> \n"
                "  <CUNIT_LIST_TOTAL_SUMMARY> \n");
         //fwrite(pFile,str, how_much_space);
         fprintf(pFile,str);

       /*  fprintf(pTestListFile,
                "    <CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> %s </CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> %u </CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> \n"
                "    </CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n",
				"Total Number of Test Cases",
                pRegistry->uiNumberOfSuites);*/
 /*       how_much_space = snprintf(NULL, 0, "%s",      "    <CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> %s </CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> %u </CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> \n"
                "    </CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n",
                "Total Number of Suites",
                pRegistry->uiNumberOfSuites);
        snprintf(str,how_much_space,"%s",
        		"    <CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> %s </CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> %u </CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> \n"
                "    </CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n",
                "Total Number of Suites",
                pRegistry->uiNumberOfSuites);
        //fwrite(pFile,str, how_much_space);
        fprintf(pFile,str);

    /*    fprintf(pTestListFile,
                "    <CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> %s </CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> %u </CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> \n"
                "    </CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n"
                "  </CUNIT_LIST_TOTAL_SUMMARY> \n",
                "Total Number of Test Cases",
                pRegistry->uiNumberOfTests);*/
 /*      how_much_space = snprintf(NULL, 0, "%s",   "    <CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> %s </CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> %u </CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> \n"
                "    </CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n"
                "  </CUNIT_LIST_TOTAL_SUMMARY> \n",
                "Total Number of Test Cases",
                pRegistry->uiNumberOfTests);
       snprintf(str,how_much_space,"%s",
                "    <CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> %s </CUNIT_LIST_TOTAL_SUMMARY_RECORD_TEXT> \n"
                "      <CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> %u </CUNIT_LIST_TOTAL_SUMMARY_RECORD_VALUE> \n"
                "    </CUNIT_LIST_TOTAL_SUMMARY_RECORD> \n"
                "  </CUNIT_LIST_TOTAL_SUMMARY> \n",
                "Total Number of Test Cases",
                pRegistry->uiNumberOfTests);
       //fwrite(pFile,str, how_much_space);
       fprintf(pFile,str);
      /* fprintf(pTestListFile,
                "  <CUNIT_ALL_TEST_LISTING> \n");*/
  /*     how_much_space = snprintf(NULL, 0, "%s",   "  <CUNIT_ALL_TEST_LISTING> \n");
       snprintf(str,how_much_space,"%s",
                "  <CUNIT_ALL_TEST_LISTING> \n");
       //fwrite(pFile,str, how_much_space);
       fprintf(pFile,str);

        pSuite = pRegistry->pSuite;
        while (NULL != pSuite) {
          assert(NULL != pSuite->pName);
          pTest = pSuite->pTest;

       /*   fprintf(pTestListFile,
                  "    <CUNIT_ALL_TEST_LISTING_SUITE> \n"
                  "      <CUNIT_ALL_TEST_LISTING_SUITE_DEFINITION> \n"
                  "        <SUITE_NAME> %s </SUITE_NAME> \n"
                  "        <INITIALIZE_VALUE> %s </INITIALIZE_VALUE> \n"
                  "        <CLEANUP_VALUE> %s </CLEANUP_VALUE> \n"
                  "        <ACTIVE_VALUE> %s </ACTIVE_VALUE> \n"
                  "        <TEST_COUNT_VALUE> %u </TEST_COUNT_VALUE> \n"
                  "      </CUNIT_ALL_TEST_LISTING_SUITE_DEFINITION> \n",
                   pSuite->pName,
                  (NULL != pSuite->pInitializeFunc) ? "Yes" : "No",
                  (NULL != pSuite->pCleanupFunc) ? "Yes" : "No",
                  (CU_FALSE != pSuite->fActive) ? "Yes" : "No",
                  pSuite->uiNumberOfTests);*/
  /*        how_much_space = snprintf(NULL, 0, "%s",   "    <CUNIT_ALL_TEST_LISTING_SUITE> \n"
                   "      <CUNIT_ALL_TEST_LISTING_SUITE_DEFINITION> \n"
                   "        <SUITE_NAME> %s </SUITE_NAME> \n"
                   "        <INITIALIZE_VALUE> %s </INITIALIZE_VALUE> \n"
                   "        <CLEANUP_VALUE> %s </CLEANUP_VALUE> \n"
                   "        <ACTIVE_VALUE> %s </ACTIVE_VALUE> \n"
                   "        <TEST_COUNT_VALUE> %u </TEST_COUNT_VALUE> \n"
                   "      </CUNIT_ALL_TEST_LISTING_SUITE_DEFINITION> \n",
                   pSuite->pName,
                   (NULL != pSuite->pInitializeFunc) ? "Yes" : "No",
                   (NULL != pSuite->pCleanupFunc) ? "Yes" : "No",
                   (CU_FALSE != pSuite->fActive) ? "Yes" : "No",
                   pSuite->uiNumberOfTests);
           snprintf(str,how_much_space,"%s",
                   "    <CUNIT_ALL_TEST_LISTING_SUITE> \n"
                   "      <CUNIT_ALL_TEST_LISTING_SUITE_DEFINITION> \n"
                   "        <SUITE_NAME> %s </SUITE_NAME> \n"
                   "        <INITIALIZE_VALUE> %s </INITIALIZE_VALUE> \n"
                   "        <CLEANUP_VALUE> %s </CLEANUP_VALUE> \n"
                   "        <ACTIVE_VALUE> %s </ACTIVE_VALUE> \n"
                   "        <TEST_COUNT_VALUE> %u </TEST_COUNT_VALUE> \n"
                   "      </CUNIT_ALL_TEST_LISTING_SUITE_DEFINITION> \n",
                   pSuite->pName,
                   (NULL != pSuite->pInitializeFunc) ? "Yes" : "No",
                   (NULL != pSuite->pCleanupFunc) ? "Yes" : "No",
                   (CU_FALSE != pSuite->fActive) ? "Yes" : "No",
                   pSuite->uiNumberOfTests);
         // fwrite(pFile,str, how_much_space);
           fprintf(pFile,str);

         /* fprintf(pTestListFile,
                  "      <CUNIT_ALL_TEST_LISTING_SUITE_TESTS> \n");*/
 /*         how_much_space = snprintf(NULL, 0, "%s",     "      <CUNIT_ALL_TEST_LISTING_SUITE_TESTS> \n");
          snprintf(str,how_much_space,"%s",
                  "      <CUNIT_ALL_TEST_LISTING_SUITE_TESTS> \n");
        //  fwrite(pFile,str, how_much_space);
          fprintf(pFile,str);

          while (NULL != pTest) {
            assert(NULL != pTest->pName);
        /*fprintf(pTestListFile,
                    "        <TEST_CASE_DEFINITION> \n"
                    "          <TEST_CASE_NAME> %s </TEST_CASE_NAME> \n"
                    "          <TEST_ACTIVE_VALUE> %s </TEST_ACTIVE_VALUE> \n"
                    "        </TEST_CASE_DEFINITION> \n",
                    pTest->pName,
                    (CU_FALSE != pSuite->fActive) ? "Yes" : "No");*/
 /*       how_much_space = snprintf(NULL, 0, "%s",   "        <TEST_CASE_DEFINITION> \n"
                    "          <TEST_CASE_NAME> %s </TEST_CASE_NAME> \n"
                    "          <TEST_ACTIVE_VALUE> %s </TEST_ACTIVE_VALUE> \n"
                    "        </TEST_CASE_DEFINITION> \n",
                    pTest->pName,
                   (CU_FALSE != pSuite->fActive) ? "Yes" : "No");
        snprintf(str,how_much_space,"%s", "        <TEST_CASE_DEFINITION> \n"
                    "          <TEST_CASE_NAME> %s </TEST_CASE_NAME> \n"
                    "          <TEST_ACTIVE_VALUE> %s </TEST_ACTIVE_VALUE> \n"
                    "        </TEST_CASE_DEFINITION> \n",
                    pTest->pName,
                    (CU_FALSE != pSuite->fActive) ? "Yes" : "No");
        //fwrite(pFile,str, how_much_space);
        fprintf(pFile,str);

         pTest = pTest->pNext;
      }

    /* fprintf(pTestListFile,
                  "      </CUNIT_ALL_TEST_LISTING_SUITE_TESTS> \n"
                  "    </CUNIT_ALL_TEST_LISTING_SUITE> \n");*/
/*     how_much_space = snprintf(NULL, 0, "%s",     "      </CUNIT_ALL_TEST_LISTING_SUITE_TESTS> \n"
                  "    </CUNIT_ALL_TEST_LISTING_SUITE> \n");
     snprintf(str,how_much_space,"%s",
                  "      </CUNIT_ALL_TEST_LISTING_SUITE_TESTS> \n"
                  "    </CUNIT_ALL_TEST_LISTING_SUITE> \n");
     //fwrite(pFile,str, how_much_space);
     fprintf(pFile,str);

     pSuite = pSuite->pNext;
   }

  /* fprintf(pTestListFile, "  </CUNIT_ALL_TEST_LISTING> \n");*/
 /*  how_much_space = snprintf(NULL, 0, "%s",   "  </CUNIT_ALL_TEST_LISTING> \n");
   snprintf(str,how_much_space,"%s",
                           "  </CUNIT_ALL_TEST_LISTING> \n");
   //fwrite(pFile,str, how_much_space);
   fprintf(pFile,str);

   time(&tTime);
   szTime = ctime(&tTime);
  /* fprintf(pTestListFile,
                "  <CUNIT_FOOTER> %s" CU_VERSION " - %s </CUNIT_FOOTER> \n"
                "</CUNIT_TEST_LIST_REPORT>",
                "File Generated By CUnit v",
                (NULL != szTime) ? szTime : "");*/
/*    how_much_space = snprintf(NULL, 0, "%s",    "  <CUNIT_FOOTER> %s" CU_VERSION " - %s </CUNIT_FOOTER> \n"
                "</CUNIT_TEST_LIST_REPORT>",
                "File Generated By CUnit v",
                (NULL != szTime) ? szTime : "");
    snprintf(str,how_much_space,"%s",
                "  <CUNIT_FOOTER> %s" CU_VERSION " - %s </CUNIT_FOOTER> \n"
                "</CUNIT_TEST_LIST_REPORT>",
                "File Generated By CUnit v",
                (NULL != szTime) ? szTime : "");
    //fwrite(pFile,str, how_much_space);
    fprintf(pFile,str);

          //if (0 != fclose(pTestListFile)) {
    if (fclose(pTestListFile)) {

        CU_set_error(CUE_FCLOSE_FAILED);
         }
        }

        return CU_get_error();
    }*/

