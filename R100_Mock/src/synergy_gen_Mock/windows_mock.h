/*
 * windows_mock.h
 *
 *  Created on: 2023 uzt. 19
 *      Author: millarramendi
 */

#ifndef SRC_SYNERGY_GEN_MOCK_WINDOWS_MOCK_H_
#define SRC_SYNERGY_GEN_MOCK_WINDOWS_MOCK_H_
#include "../types_basic_Mock.h"
#include <windows.h>
//WINDOWS
// Function pointer type for the error callback

HANDLE hThread;
typedef void (*ErrorCallback)(void);

// Global pointer to the error callback function
static ErrorCallback g_errorCallback = NULL;

extern void registerErrorCallback(ErrorCallback errorCallback);
extern void startupErrorCallback();
extern void coreThreadErrorCallback();
extern void commSemErrorCallback();

extern void invokeErrorCallback();
//WINDOWS Fin



#endif /* SRC_SYNERGY_GEN_MOCK_WINDOWS_MOCK_H_ */
