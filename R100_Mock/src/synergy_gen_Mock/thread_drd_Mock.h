/* generated thread header file - do not edit */
#ifndef THREAD_DRD_H_
#define THREAD_DRD_H_
#include "hal_data_Mock.h"
#include "../types_basic_Mock.h"


#include "../../synergy_Mock/ssp/inc/framework/el/tx_api_Mock.h"
#ifdef __cplusplus
extern "C" void thread_drd_entry_mock(void);
#else
extern void thread_drd_entry_mock(void);
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern TX_QUEUE queue_drd;
extern TX_EVENT_FLAGS_GROUP ev_drd_diag_ready;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* THREAD_DRD_H_ */
