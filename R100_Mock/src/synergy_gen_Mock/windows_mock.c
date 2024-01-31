/*
 * windows_mock.c
 *
 *  Created on: 2023 uzt. 19
 *      Author: millarramendi
 */
#include "windows_mock.h"
#include "../types_basic_Mock.h"




// Function to register the error callback
void registerErrorCallback(ErrorCallback errorCallback) {
    g_errorCallback = errorCallback;
}

// Function to invoke the error callback (if registered)
void invokeErrorCallback() {
    if (g_errorCallback != 0) {
        g_errorCallback();
    }
}
// Sample startup error callback function
void startupErrorCallback() {
    printf("Error occurred during startup!\n");
    // Additional error handling code can be added here.
}
void coreThreadErrorCallback() {
    printf("Error occurred during coreThreadInitialization!\n");
    // Additional error handling code can be added here.
}
extern void commSemErrorCallback()
{
    printf("Error occurred during commSemInitialization!\n");
    // Additional error handling code can be added here.
}


