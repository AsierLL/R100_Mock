/* generated thread header file - do not edit */
#ifndef THREAD_PATMON_H_
#define THREAD_PATMON_H_
#include "hal_data_Mock.h"
#include "../types_basic_Mock.h"
#include <windows.h>

#include "../../synergy_Mock/ssp/inc/framework/el/tx_api_Mock.h"
#ifdef __cplusplus
extern "C" void thread_patMon_entry_mock(void);
#else
extern void thread_patMon_entry_mock(void);
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern HANDLE g_events_PatMon;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* THREAD_PATMON_H_ */
