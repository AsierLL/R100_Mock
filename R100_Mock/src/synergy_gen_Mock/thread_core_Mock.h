/* generated thread header file - do not edit */
#ifndef THREAD_CORE_H_
#define THREAD_CORE_H_
//#include "bsp_api.h"
#include "hal_data_Mock.h"

#include "../../synergy_Mock/ssp/inc/framework/el/tx_api_Mock.h"
#ifdef __cplusplus
extern "C" void thread_core_entry_mock(void);
#else
extern void thread_core_entry_mock(void);
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern TX_SEMAPHORE trace_semaphore;
extern TX_QUEUE queue_core;
extern TX_EVENT_FLAGS_GROUP g_cdcacm_activate_event_flags0;
extern TX_SEMAPHORE comm_semaphore;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* THREAD_CORE_H_ */
