/* generated thread source file - do not edit */
#include "thread_core_Mock.h"
#include "windows_mock.h"

TX_THREAD thread_core;
void thread_core_create_mock(void);
static void thread_core_func_mock(ULONG thread_input);
static uint8_t thread_core_stack[2048]; //BSP_PLACE_IN_SECTION_V2(".stack.thread_core") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init_mock(void);
TX_SEMAPHORE trace_semaphore;
TX_QUEUE queue_core;
static uint8_t queue_memory_queue_core[20];
TX_EVENT_FLAGS_GROUP g_cdcacm_activate_event_flags0;
TX_SEMAPHORE comm_semaphore;

extern uint32_t g_ssp_common_thread_count;


void thread_core_create_mock(void) {
	printf("thread_core_create_mock\n");
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */

	 // Parameters for CreateSemaphore:
	    // lpSemaphoreAttributes (NULL for default)
	    // lInitialCount (Initial count for the semaphore)
	    // lMaximumCount (Maximum count for the semaphore)
	    // lpName (NULL for an unnamed semaphore)
	    HANDLE err_trace_semaphore = CreateSemaphore(NULL, 1, 10, "Trace Semaphore");

	    if (err_trace_semaphore == NULL) {
	    		           // Semaphore creation failed
	    		           // Handle error here
	    		           //return 1;
	    		    	 // Register the startup error callback
	    		    	 registerErrorCallback(coreThreadErrorCallback);
	    		    	 invokeErrorCallback();
	    		    	printf("No se ha creado el semaforo Trace Semaphore ");
	    	}


	 HANDLE err_comm_semaphore = CreateSemaphore(NULL, 0, 10, "Comm Semaphore");

	 if (err_comm_semaphore == NULL) {
			           // Semaphore creation failed
	   		           // Handle error here
				       //return 1;
		  	    	 // Register the startup error callback
		 		    	 registerErrorCallback(commSemErrorCallback);
		   		    	 invokeErrorCallback();
		   		    	printf("No se ha creado el semaforo Comm ");
	   }
	thread_core_func_mock(0);

}

static void thread_core_func_mock(ULONG thread_input) {

	printf("thread_core_func_mock\n");
	/* Not currently using thread_input. */
	//SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init_mock();

	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	thread_core_entry_mock();
}
