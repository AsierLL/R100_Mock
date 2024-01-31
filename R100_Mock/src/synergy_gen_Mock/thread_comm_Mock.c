/* generated thread source file - do not edit */
#include "thread_comm_Mock.h"

TX_THREAD thread_comm;
void thread_comm_create_mock(void);
static void thread_comm_func_mock(ULONG thread_input);
static uint8_t thread_comm_stack[2048]; //BSP_PLACE_IN_SECTION_V2(".stack.thread_comm") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init_mock(void);
TX_QUEUE queue_comm;
static uint8_t queue_memory_queue_comm[20];
//extern bool_t g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void thread_comm_create_mock(void) {

	printf("thread_comm_create_mock\n");
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */
	UINT err_queue_comm;
	//err_queue_comm = tx_queue_create(&queue_comm, (CHAR *) "Queue Comm", 1,
	//		&queue_memory_queue_comm, sizeof(queue_memory_queue_comm));
	if (TX_SUCCESS != err_queue_comm) {
		//tx_startup_err_callback(&queue_comm, 0);
	}

	UINT err;
//	err = tx_thread_create(&thread_comm, (CHAR *) "Thread Comm",
//			thread_comm_func, (ULONG) NULL, &thread_comm_stack, 2048, 18, 18, 1,
	//		TX_DONT_START);
	if (TX_SUCCESS != err) {
	//	tx_startup_err_callback(&thread_comm, 0);
	}
}

static void thread_comm_func_mock(ULONG thread_input) {

	printf("thread_comm_func_mock\n");
	/* Not currently using thread_input. */
	//SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init_mock();

	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	thread_comm_entry_mock();
}
