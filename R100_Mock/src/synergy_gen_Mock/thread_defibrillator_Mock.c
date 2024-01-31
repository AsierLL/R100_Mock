/* generated thread source file - do not edit */
#include "thread_defibrillator_Mock.h"

TX_THREAD thread_defibrillator;
void thread_defibrillator_create_mock(void);
static void thread_defibrillator_func_mock(ULONG thread_input);
static uint8_t thread_defibrillator_stack[1024];// BSP_PLACE_IN_SECTION_V2(".stack.thread_defibrillator") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init_mock(void);

TX_QUEUE queue_def;
static uint8_t queue_memory_queue_def[20];
extern bool_t g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void thread_defibrillator_create_mock(void) {

	printf("thread_defibrillator_create_mock\n");
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */
	UINT err_queue_def;
	//err_queue_def = tx_queue_create(&queue_def, (CHAR *) "Queue Defibrillator",
		//	1, &queue_memory_queue_def, sizeof(queue_memory_queue_def));
	if (TX_SUCCESS != err_queue_def) {
		//tx_startup_err_callback(&queue_def, 0);
	}


	thread_defibrillator_func_mock(0);
}

static void thread_defibrillator_func_mock(ULONG thread_input) {

	printf("thread_defibrillator_func_mock\n");
	/* Not currently using thread_input. */
	//SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init_mock();

	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	thread_defibrillator_entry_mock();
}
