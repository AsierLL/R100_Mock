/* generated main source file - do not edit */
//#include "bsp_api.h"
#include "../../synergy_Mock/ssp/inc/framework/el/tx_api_Mock.h"
#include "../types_basic_Mock.h"
#include "windows_mock.h"
DWORD WINAPI mockThreadFunction(LPVOID lpParam);
extern void thread_core_create_mock(void);
extern void thread_drd_create_mock(void);
extern void thread_patMon_create_mock(void);
extern void thread_hmi_create_mock(void);
extern void thread_sysMon_create_mock(void);
extern void thread_recorder_create_mock(void);
extern void thread_audio_create_mock(void);
extern void thread_defibrillator_create_mock(void);
extern void thread_comm_create_mock(void);
extern void thread_dummy_create_mock(void);

uint32_t g_ssp_common_thread_count;
bool g_ssp_common_initialized;
//TX_SEMAPHORE g_ssp_common_initialized_semaphore;
HANDLE semaphore;


#if defined(__ICCARM__)
#define WEAK_REF_ATTRIBUTE
#pragma weak tx_application_define_user               = tx_application_define_internal
#elif defined(__GNUC__)
#define WEAK_REF_ATTRIBUTE      __attribute__ ((weak, alias("tx_application_define_internal")))
#endif

#ifdef TX_USER_TRACE_BUFFER_DECLARE
TX_USER_TRACE_BUFFER_DECLARE;
#endif

void g_hal_init(void);

/** Weak reference for tx_err_callback */
#if defined(__ICCARM__)
#define tx_startup_err_callback_WEAK_ATTRIBUTE
#pragma weak tx_startup_err_callback  = tx_startup_err_callback_internal
#elif defined(__GNUC__)
#define tx_startup_err_callback_WEAK_ATTRIBUTE __attribute__ ((weak, alias("tx_startup_err_callback_internal")))
#endif
void tx_startup_err_callback_internal(void *p_instance, void *p_data);
void tx_startup_err_callback(void *p_instance, void *p_data)
tx_startup_err_callback_WEAK_ATTRIBUTE;

void tx_application_define_internal(void *first_unused_memory);

void tx_application_define_internal(void *first_unused_memory) {
	/* Does nothing. Default implementation of tx_application_define_user(). */
	//SSP_PARAMETER_NOT_USED(first_unused_memory);
}

void tx_application_define_internal(void *first_unused_memory);
void tx_application_define_user(void *first_unused_memory)
WEAK_REF_ATTRIBUTE;

void tx_application_define() {
	g_ssp_common_thread_count = 0;
	g_ssp_common_initialized = false;

	/* Create semaphore to make sure common init is done before threads start running. */
	UINT err;
	semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	printf("Semaforo SSPCOMM\n");
	if (semaphore == NULL) {
		           // Semaphore creation failed
		           // Handle error here
		           //return 1;
		    	 // Register the startup error callback
		    	 registerErrorCallback(startupErrorCallback);
		    	 invokeErrorCallback();
		    	printf("No se ha creado el semaforo SSP Common Init Sem ");
	}
	err = thread_init_for_test();
	//thread_sysMon_create_mock();
	//thread_audio_create_mock();
	thread_core_create_mock();
	/*thread_drd_create();
	thread_patMon_create();
	thread_hmi_create();
	thread_sysMon_create();
	thread_recorder_create();
	thread_audio_create();
	thread_defibrillator_create();
	thread_comm_create();
	thread_dummy_create();*/
	CUnit_main();
	CloseHandle(hThread);

#ifdef TX_USER_ENABLE_TRACE
	TX_USER_ENABLE_TRACE;
#endif

//	tx_application_define_user();
}

int thread_init_for_test()
{


	DWORD threadId;

		 // Create the thread
		    hThread = CreateThread(
		        NULL,                   // default security attributes
		        0,                      // default stack size
		        mockThreadFunction,         // thread function
		        NULL,          // optional parameter to thread function
		        0,                      // default creation flags
		        &threadId               // returns the thread identifier
		    );

		    if (hThread == NULL) {
		        printf("Mock Thread creation failed!\n");
		        return 1;
		    }

		    // Optionally, you can wait for the thread to complete using WaitForSingleObject or WaitForMultipleObjects.
		    // For example, to wait for the single thread created above:
		    // WaitForSingleObject(hThread, INFINITE);

		    // Clean up the thread handle when you're done with it


		    return 0;

}
/*********************************************************************************************************************
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user function
 *             with the prototype below.
 *             - void tx_startup_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void tx_startup_err_callback_internal(void *p_instance, void *p_data);
void tx_startup_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
//	SSP_PARAMETER_NOT_USED(p_instance);
//	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
//	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

void tx_startup_common_init_mock(void);
void tx_startup_common_init_mock(void) {

	printf("tx_startup_common_init_mock\n");
	/* First thread will take care of common initialization. */
	UINT err;

	DWORD waitResult = WaitForSingleObject(semaphore, INFINITE);

	//	TX_WAIT_FOREVER);
	if (waitResult == WAIT_OBJECT_0) {
	            // Semaphore acquired
	            printf("Thread: Semaphore acquired.\n");
	        } else if (waitResult == WAIT_FAILED) {
	            // Error occurred while waiting
	            // Handle error here
	            printf("Thread: Error occurred while waiting for semaphore.\n");
	          // break;
	        }


	/* Only perform common initialization if this is the first thread to execute. */
	if (false == g_ssp_common_initialized) {
		/* Later threads will not run this code. */
		g_ssp_common_initialized = true;

		/* Perform common module initialization. */
		g_hal_init();

		/* Now that common initialization is done, let other threads through. */
		/* First decrement by 1 since 1 thread has already come through. */
		g_ssp_common_thread_count--;
		while (g_ssp_common_thread_count > 0) {
			//err = tx_semaphore_put(&g_ssp_common_initialized_semaphore);
			ReleaseSemaphore(semaphore, 1, NULL);
			g_ssp_common_thread_count--;
		}
	}
}


int main(void) {
	//__disable_irq();
	//tx_kernel_enter();
	tx_application_define();

	return 0;
}
