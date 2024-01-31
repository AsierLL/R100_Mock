#include "synergy_gen_Mock/thread_dummy_Mock.h"

/* Thread Dummy entry function */
void thread_dummy_entry_mock(void)
{
    /* TODO: add your own code here */
	printf("thread dummy_entry mock\n");
    while (1)
    {
       /* g_ioport.p_api->pinWrite (IOPORT_PORT_06_PIN_09, IOPORT_LEVEL_LOW);
        g_ioport.p_api->pinWrite (IOPORT_PORT_06_PIN_09, IOPORT_LEVEL_HIGH);
//      tx_thread_sleep (1);*/
    }
}
