/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_


#include "common_data_Mock.h"
/*#include "r_flash_lp.h"
#include "r_flash_api.h"
#include "r_iwdt.h"
#include "r_wdt_api.h"
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "r_icu.h"
#include "r_external_irq_api.h"
#include "r_rtc.h"
#include "r_rtc_api.h"*/
#ifdef __cplusplus
extern "C" {
#endif

void trace_uart_callback();

void hal_entry_mock(void);
void g_hal_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* HAL_DATA_H_ */
