/* generated thread source file - do not edit */
#include "thread_audio_Mock.h"
#include "windows_mock.h"



//TX_THREAD thread_audio;
void thread_audio_create_mock(void);
static void thread_audio_func_mock(ULONG thread_input);
static uint8_t thread_audio_stack[2048];// BSP_PLACE_IN_SECTION_V2(".stack.thread_audio") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init_mock(void);

//TX_SEMAPHORE audio_semaphore;
HANDLE audio_semaphore;
HANDLE g_events_Audio;
HANDLE usd_mutex;
extern bool_t g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void thread_audio_create_mock(void) {
	printf("thread_audio_create_mock\n");
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */
	audio_semaphore = CreateSemaphore(NULL, 1, 1, (CHAR *) "Audio Semaphore");
	printf("Semaforo audio \n");
	if (audio_semaphore == NULL) {
	        // Semaphore creation failed
	        // Handle error here
	        //return 1;
	   	 // Register the startup error callback
	  	 registerErrorCallback(startupErrorCallback);
	   	 invokeErrorCallback();
	     printf("No se ha creado el semaforo audio ");
	}
	 UINT err_g_events_Audio;
	 g_events_Audio = CreateEvent(NULL, 1, 1,(CHAR *) "Events Audio" );
	 if (g_events_Audio == NULL)
		    {
		 	 	 registerErrorCallback(startupErrorCallback);
		 	   	 invokeErrorCallback();
		 	     printf("No se ha creado los eventos de audio ");
		    }

	 UINT err_usd_mutex;
	 	 usd_mutex = CreateMutex (NULL, FALSE, (CHAR *) "uSD Mutex");
	 	    if (usd_mutex == NULL)
	 	    {
	 	    	 registerErrorCallback(startupErrorCallback);
	 	    	 invokeErrorCallback();
	 	    	 printf("No se ha creado  el Mutex usd");
	 	    }

	/*	UINT err;
		err = tx_thread_create(&thread_core, (CHAR *) "Thread Core",
				thread_audio_func, (ULONG) NULL, &thread_core_stack, 2048, 6, 6, 1,
				TX_DONT_START);
		if (TX_SUCCESS != err) {
			tx_startup_err_callback(&thread_core, 0);
		}*/

	thread_audio_func_mock(0);

}

static void thread_audio_func_mock(ULONG thread_input) {
	/* Not currently using thread_input. */
	//SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init_mock();


	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	thread_audio_entry_mock();
}
