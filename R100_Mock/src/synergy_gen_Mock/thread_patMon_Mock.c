/* generated thread source file - do not edit */
#include "thread_patMon_Mock.h"
#include "windows_mock.h"

//TX_THREAD thread_patMon;
HANDLE thread_patMon;
void thread_patMon_create_mock(void);
static void thread_patMon_func_mock(ULONG thread_input);
static uint8_t thread_patMon_stack[2048];// BSP_PLACE_IN_SECTION_V2(".stack.thread_patMon") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init_mock(void);

//TX_EVENT_FLAGS_GROUP g_events_PatMon;
HANDLE g_events_PatMon;
extern bool_t g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void thread_patMon_create_mock(void) {

	printf("thread_patMon_create_mock\n");
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */
	UINT err_g_events_PatMon;
	//err_g_events_PatMon = tx_event_flags_create(&TX_THREAD thread_patMon;,
		//	(CHAR *) "Events_PatMon");
	CreateEvent(thread_patMon,1,1,(CHAR *) "Events_PatMon" );
	if (TX_SUCCESS != err_g_events_PatMon) {
	//	tx_startup_err_callback(&g_events_PatMon, 0);
	}


	thread_patMon_func_mock(0);
}

static void thread_patMon_func_mock(ULONG thread_input) {

	printf("thread_patMon_func_mock\n");
	/* Not currently using thread_input. */
	//SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init_mock();

	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	thread_patMon_entry_mock();
}
