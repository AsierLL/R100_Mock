/* generated thread source file - do not edit */
#include "thread_sysMon_Mock.h"
#include "windows_mock.h"



//TX_THREAD thread_sysMon;
void thread_sysMon_create_mock(void);
static void thread_sysMon_func_mock(ULONG thread_input);
static uint8_t thread_sysMon_stack[4096];// BSP_PLACE_IN_SECTION_V2(".stack.thread_sysMon") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init_mock(void);
//TX_SEMAPHORE i2c_2_semaphore;
HANDLE i2c_2_semaphore;
//extern bool_t g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern HANDLE g_ssp_common_initialized_semaphore;

void thread_sysMon_create_mock(void) {

	printf("thread_sysMon_create_mock\n");
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */
	//UINT err_i2c_2_semaphore;
	i2c_2_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
		printf("Semaforo I2C2 \n");
		if (i2c_2_semaphore == NULL) {
			           // Semaphore creation failed
			           // Handle error here
			           //return 1;
			    	 // Register the startup error callback
			    	 registerErrorCallback(startupErrorCallback);
			    	 invokeErrorCallback();
			    	printf("No se ha creado el semaforo i2c2 ");
		}

	thread_sysMon_func_mock(0);
}

static void thread_sysMon_func_mock(ULONG thread_input) {

	printf("thread_sysMon_func_mock\n");
	/* Not currently using thread_input. */
	//SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init_mock();

	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	thread_sysMon_entry_mock();
}
