/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "../types_basic_Mock.h"

/*#include "r_dmac.h"
#include "r_transfer_api.h"
#include "r_sdmmc.h"
#include "r_sdmmc_api.h"
#include "sf_block_media_sdmmc.h"
#include "sf_block_media_api.h"
#include "sf_el_fx.h"
#include "fx_api_Mock.h"
#include "r_rspi.h"
#include "r_spi_api.h"
#include "r_cgc_api.h"
#include "r_spi_api.h"
#include "sf_spi.h"
#include "sf_spi_api.h"
#include "r_elc.h"
#include "r_elc_api.h"
#include "r_fmi.h"
#include "r_fmi_api.h"
#include "r_ioport.h"
#include "r_ioport_api.h"
#include "r_cgc.h"
#include "r_cgc_api.h"*/
#ifdef __cplusplus
extern "C" {
#endif
void fx_common_init0_mock(void);
/* Transfer on DMAC Instance. */
//extern const transfer_instance_t g_transfer;

/** Block Media on SDMMC Instance */
//extern sf_block_media_instance_t g_sf_block_media_sdmmc0;
//extern sf_el_fx_t g_sf_el_fx_cfg;
//extern FX_MEDIA sd_fx_media;

//void sd_fx_media_err_callback(void *p_instance, void *p_data);
//ssp_err_t fx_media_init0_format(void);
uint32_t fx_media_init0_open_mock(void);
void fx_media_init0_mock(void);
/** SPI config */
//extern const spi_cfg_t g_spi0_cfg;
/** RSPI extended config */
//extern const spi_on_rspi_cfg_t g_spi0_ext_cfg;
/** SPI on RSPI Instance. */
//extern const spi_instance_t g_spi0;
/** SPI instance control */
//extern rspi_instance_ctrl_t g_spi0_ctrl;


//extern const transfer_instance_t g_spi0_transfer_rx;
//extern const transfer_instance_t g_spi0_transfer_tx;

void ux_common_init0_mock(void);
//#include "ux_api.h"
//#include "ux_dcd_synergy.h"
//#include "sf_el_ux_dcd_fs_cfg.h"
//void g_sf_el_ux_dcd_fs_0_err_callback(void *p_instance, void *p_data);
//#include "ux_api.h"
//#include "ux_dcd_synergy.h"

/* USBX Device Stack initialization error callback function. User can override the function if needed. */
//void ux_device_err_callback(void *p_instance, void *p_data);
void ux_device_init0_mock(void);
//#include "ux_api.h"
//#include "ux_device_class_storage.h"
/* USBX Mass Storage Class User Media Setup Callback Function */
//extern void ux_device_class_storage_user_setup(
//		UX_SLAVE_CLASS_STORAGE_PARAMETER *param);
/* USBX Mass Storage Class Media Read User Callback Function */

/* USBX Mass Storage Class Media Write User Callback Function */

/* USBX Mass Storage Class Media Status User Callback Function */

/* USBX Mass Storage Class Media Activate/Deactivate Callback Function */
/* USBX Mass Storage Class Parameter Setup Function */
void ux_device_class_storage_init0_mock(void);
/** ELC Instance */
//extern const elc_instance_t g_elc;
/** FMI on FMI Instance. */
//extern const fmi_instance_t g_fmi;
/** IOPORT Instance */
// const ioport_instance_t g_ioport;
/** CGC Instance */
//extern const cgc_instance_t g_cgc;
void g_common_init_mock(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* COMMON_DATA_H_ */
