/* generated thread header file - do not edit */
#ifndef THREAD_AUDIO_H_
#define THREAD_AUDIO_H_
#include "hal_data_Mock.h"
#include "../types_basic_Mock.h"
#include "windows_mock.h"


//#include "bsp_api_Mock.h"
#include "../../synergy_Mock/ssp/inc/framework/el/tx_api_Mock.h"
#ifdef __cplusplus
extern "C" void thread_audio_entry_mock(void);
#else
extern void thread_audio_entry_mock(void);
#endif
#ifdef __cplusplus
extern "C" {
#endif

//extern TX_SEMAPHORE audio_semaphore;
extern HANDLE audio_semaphore;
extern HANDLE g_events_Audio;
extern HANDLE usd_mutex;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* THREAD_AUDIO_H_ */
