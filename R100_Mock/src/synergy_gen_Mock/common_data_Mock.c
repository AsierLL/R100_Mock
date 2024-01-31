/* generated common source file - do not edit */
#include "common_data_Mock.h"


/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void fx_common_init0(void)
 **********************************************************************************************************************/
void fx_common_init0_mock(void) {
	/** Initialize the FileX system. */
	//fx_system_initialize();
	printf("fx_system_initialized mock\n");
}

uint8_t g_media_memory_sd_fx_media[512];
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function
 *             with the prototype below.
 *             - void sd_fx_media_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void sd_fx_media_err_callback_internal_mock(void *p_instance, void *p_data);
void sd_fx_media_err_callback_internal_mock(void *p_instance, void *p_data) {
	printf("fx_media_err_callback mock\n");
}

/*ssp_err_t fx_media_init0_format(void) {
	uint32_t fx_ret_val = 0;

	uint32_t sector_size = 512;
	uint32_t sector_count = 50000;
	printf("Inicializacion format fx_media\n");

	return SSP_SUCCESS;
}*/

uint32_t fx_media_init0_open_mock(void) {

	printf("fx_media_init0_open_mock\n");
	return 0;//fx_media_open(&sd_fx_media, (CHAR *) "sd_fx_media",
			//SF_EL_FX_BlockDriver, &g_sf_el_fx_cfg, g_media_memory_sd_fx_media,
			//sizeof(g_media_memory_sd_fx_media));
}

/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void fx_media_init0(void)
 **********************************************************************************************************************/
void fx_media_init0_mock(void) {


	uint32_t err_open = fx_media_init0_open_mock();
	printf("fx_media_init0_mock\n");
	if (err_open != 0) {
		//sd_fx_media_err_callback((void *) &sd_fx_media, &err_open);
		printf("fx_media_init0_Error\n");
	}
}
#define RSPI_TRANSFER_SIZE_1_BYTE (0x52535049)
#define RSPI_SYNERGY_NOT_DEFINED 1
#if (RSPI_SYNERGY_NOT_DEFINED != RSPI_SYNERGY_NOT_DEFINED)
dtc_instance_ctrl_t g_spi0_transfer_tx_ctrl;
uint32_t g_spi0_tx_inter = 0;
transfer_info_t g_spi0_transfer_tx_info[2] =
{
#if (RSPI_TRANSFER_SIZE_1_BYTE == RSPI_SYNERGY_NOT_DEFINED)
	{
		.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_EACH,
		.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
		.size = TRANSFER_SIZE_1_BYTE,
		.mode = TRANSFER_MODE_NORMAL,
		.p_dest = (void *) &g_spi0_tx_inter,
		.p_src = (void const *) NULL,
		.num_blocks = 0,
		.length = 0,
	},
	{
		.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_4_BYTE,
		.mode = TRANSFER_MODE_NORMAL,
		.p_dest = (void *) NULL,
		.p_src = (void const *) &g_spi0_tx_inter,
		.num_blocks = 0,
		.length = 0,
	},
#else
	{
		.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
		.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
		.size =SYNERGY_NOT_DEFINED,
		.mode = TRANSFER_MODE_NORMAL,
		.p_dest = (void *) NULL,
		.p_src = (void const *) NULL,
		.num_blocks = 0,
		.length = 0,
	},
	{
		.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_1_BYTE,
		.mode = TRANSFER_MODE_NORMAL,
		.p_dest =(void *) NULL,
		.p_src = (void const *) &g_spi0_tx_inter,
		.num_blocks = 0,
		.length = 0,
	},
#endif
};

const transfer_cfg_t g_spi0_transfer_tx_cfg =
{
	.p_info = g_spi0_transfer_tx_info,
	.activation_source = SYNERGY_NOT_DEFINED,
	.auto_enable = false,
	.p_callback = NULL,
	.p_context = &g_spi0_transfer_tx,
	.irq_ipl = SYNERGY_NOT_DEFINED
};
/* Instance structure to use this module. */
const transfer_instance_t g_spi0_transfer_tx =
{
	.p_ctrl = &g_spi0_transfer_tx_ctrl,
	.p_cfg = &g_spi0_transfer_tx_cfg,
	.p_api = &g_transfer_on_dtc
};

dtc_instance_ctrl_t g_spi0_transfer_rx_ctrl;
uint32_t g_spi0_rx_inter = 0;
transfer_info_t g_spi0_transfer_rx_info[2] =
{
#if (RSPI_TRANSFER_SIZE_1_BYTE == RSPI_SYNERGY_NOT_DEFINED)
	{
		.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_EACH,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_4_BYTE,
		.mode = TRANSFER_MODE_NORMAL,
		.p_dest = (void *) &g_spi0_rx_inter,
		.p_src = (void const *) NULL,
		.num_blocks = 0,
		.length = 0,
	},
	{
		.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_1_BYTE,
		.mode = TRANSFER_MODE_NORMAL,
		.p_dest = (void *) NULL,
		.p_src = (void const *) &g_spi0_rx_inter,
		.num_blocks = 0,
		.length = 0,
	},
#else
	{
		.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
		.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = SYNERGY_NOT_DEFINED,
		.mode = TRANSFER_MODE_NORMAL,
		.p_dest = (void *) NULL,
		.p_src = (void const *) NULL,
		.num_blocks = 0,
	},
	{
		.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_1_BYTE,
		.mode = TRANSFER_MODE_NORMAL,
		.p_dest = (void *) NULL,
		.p_src = (void const *) &g_spi0_rx_inter,
		.num_blocks = 0,
		.length = 0,
	},
#endif
};

const transfer_cfg_t g_spi0_transfer_rx_cfg =
{
	.p_info = g_spi0_transfer_rx_info,
	.activation_source = SYNERGY_NOT_DEFINED,
	.auto_enable = false,
	.p_callback = NULL,
	.p_context = &g_spi0_transfer_rx,
	.irq_ipl = SYNERGY_NOT_DEFINED
};
/* Instance structure to use this module. */
const transfer_instance_t g_spi0_transfer_rx =
{
	.p_ctrl = &g_spi0_transfer_rx_ctrl,
	.p_cfg = &g_spi0_transfer_rx_cfg,
	.p_api = &g_transfer_on_dtc
};
#endif
#undef RSPI_TRANSFER_SIZE_1_BYTE	
#undef RSPI_SYNERGY_NOT_DEFINED
/*
#if !defined(SSP_SUPPRESS_ISR_g_spi0) && !defined(SSP_SUPPRESS_ISR_SPI0)
SSP_VECTOR_DEFINE_CHAN(spi_rxi_isr, SPI, RXI, 0);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_spi0) && !defined(SSP_SUPPRESS_ISR_SPI0)
SSP_VECTOR_DEFINE_CHAN(spi_txi_isr, SPI, TXI, 0);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_spi0) && !defined(SSP_SUPPRESS_ISR_SPI0)
SSP_VECTOR_DEFINE_CHAN(spi_eri_isr, SPI, ERI, 0);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_spi0) && !defined(SSP_SUPPRESS_ISR_SPI0)
SSP_VECTOR_DEFINE_CHAN(spi_tei_isr, SPI, TEI, 0);
#endif
rspi_instance_ctrl_t g_spi0_ctrl;*/

/** RSPI extended configuration for RSPI HAL driver */
/*
const spi_on_rspi_cfg_t g_spi0_ext_cfg =
		{ .rspi_clksyn = RSPI_OPERATION_SPI,*/
		/* Communication mode is configured by the driver. write calls use TX_ONLY. read and writeRead use FULL_DUPLEX. */
/*		.rspi_comm = RSPI_COMMUNICATION_FULL_DUPLEX, .ssl_polarity.rspi_ssl0 =
				RSPI_SSLP_LOW, .ssl_polarity.rspi_ssl1 = RSPI_SSLP_LOW,
				.ssl_polarity.rspi_ssl2 = RSPI_SSLP_LOW,
				.ssl_polarity.rspi_ssl3 = RSPI_SSLP_LOW,
				.loopback.rspi_loopback1 = RSPI_LOOPBACK1_NORMAL_DATA,
				.loopback.rspi_loopback2 = RSPI_LOOPBACK2_NORMAL_DATA,
				.mosi_idle.rspi_mosi_idle_fixed_val =
						RSPI_MOSI_IDLE_FIXED_VAL_LOW,
				.mosi_idle.rspi_mosi_idle_val_fixing =
						RSPI_MOSI_IDLE_VAL_FIXING_DISABLE, .parity.rspi_parity =
						RSPI_PARITY_STATE_DISABLE, .parity.rspi_parity_mode =
						RSPI_PARITY_MODE_ODD,
				.ssl_select = RSPI_SSL_SELECT_SSL0, .ssl_level_keep =
						RSPI_SSL_LEVEL_KEEP,
				.clock_delay.rspi_clock_delay_count = RSPI_CLOCK_DELAY_COUNT_1,
				.clock_delay.rspi_clock_delay_state =
						RSPI_CLOCK_DELAY_STATE_DISABLE,
				.ssl_neg_delay.rspi_ssl_neg_delay_count =
						RSPI_SSL_NEGATION_DELAY_1,
				.ssl_neg_delay.rspi_ssl_neg_delay_state =
						RSPI_SSL_NEGATION_DELAY_DISABLE,
				.access_delay.rspi_next_access_delay_count =
						RSPI_NEXT_ACCESS_DELAY_COUNT_1,
				.access_delay.rspi_next_access_delay_state =
						RSPI_NEXT_ACCESS_DELAY_STATE_DISABLE, .byte_swap =
						RSPI_BYTE_SWAP_DISABLE, };
*/
/*const spi_cfg_t g_spi0_cfg = { .channel = 0, .operating_mode = SPI_MODE_MASTER,
		.clk_phase = SPI_CLK_PHASE_EDGE_ODD, .clk_polarity =
				SPI_CLK_POLARITY_LOW,
		.mode_fault = SPI_MODE_FAULT_ERROR_DISABLE, .bit_order =
				SPI_BIT_ORDER_MSB_FIRST, .bitrate = 800000, .p_transfer_tx =
				g_spi0_P_TRANSFER_TX, .p_transfer_rx = g_spi0_P_TRANSFER_RX,
		.p_callback = NULL, .p_context = (void *) &g_spi0, .p_extend =
				(void *) &g_spi0_ext_cfg, .rxi_ipl = (4), .txi_ipl = (4),
		.eri_ipl = (4), .tei_ipl = (4), };*/
/* Instance structure to use this module. */
/*const spi_instance_t g_spi0 = { .p_ctrl = &g_spi0_ctrl, .p_cfg = &g_spi0_cfg,
		.p_api = &g_spi_on_rspi };
static TX_MUTEX sf_bus_mutex_spi_bus0;
static TX_EVENT_FLAGS_GROUP sf_bus_eventflag_spi_bus0;
static sf_spi_ctrl_t *p_sf_curr_ctrl_spi_bus0;

sf_spi_bus_t spi_bus0 = { .p_bus_name = (uint8_t *) "spi_bus0", .channel = 0,
		.freq_hz_min = 0, .p_lock_mutex = &sf_bus_mutex_spi_bus0,
		.p_sync_eventflag = &sf_bus_eventflag_spi_bus0, .pp_curr_ctrl =
				(sf_spi_ctrl_t **) &p_sf_curr_ctrl_spi_bus0, .p_lower_lvl_api =
				(spi_api_t *) &g_spi_on_rspi, .device_count = 0, };*/
/***********************************************************************************************************************
 * USB Mass Storage Class Interface Descriptor  for FS mode g_usb_interface_descriptor_storage_0
 **********************************************************************************************************************/
#if defined(__GNUC__)
//static volatile const unsigned char g_usb_interface_descriptor_storage_0_full_speed[] BSP_PLACE_IN_SECTION_V2(".usb_interface_desc_fs") BSP_ALIGN_VARIABLE_V2(1)
#else /* __ICCARM__ */
#pragma section = ".usb_interface_desc_fs" 1
__root static const unsigned char g_usb_interface_descriptor_storage_0_full_speed[] BSP_PLACE_IN_SECTION_V2(".usb_interface_desc_fs")
#endif
//= {
	/***********************************************************************
	 * Mass Storage Class Interface Descriptor                             *
	 ***********************************************************************/
	//0x09, /* 0 bLength */
//	UX_INTERFACE_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
//	0x00, /* 2 bInterfaceNumber */
//	0x00, /* 3 bAlternateSetting  : Alternate for SetInterface Request */
//	0x02, /* 4 bNumEndpoints      : 2 Endpoints for Interface#1 */
//	0x08, /* 5 bInterfaceClass    : Mass Storage Class(0x8) */
//	0x06, /* 6 bInterfaceSubClass : SCSI transparent command set(0x6) */
//	0x50, /* 7 bInterfaceProtocol : BBB(0x50) */
//	0x00, /* 8 iInterface Index */
	/***********************************************************************
	 * Mass Storage Class Endpoint Descriptor (Bulk-Out)  for Interface#0x00 *
	 ***********************************************************************/
//	0x07, /* 0 bLength */
//	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
//	(UX_ENDPOINT_OUT | 1), /* 2 bEndpointAddress */
//	UX_BULK_ENDPOINT, /* 3 bmAttributes  */
//	0x40, /* 4 wMaxPacketSize 64bytes */
//	0x00, /* 5 wMaxPacketSize */
//	0x00, /* 6 bInterval */
	/***********************************************************************
	 * Mass Storage Class Endpoint Descriptor (Bulk-In) for Interface#0x00 *
	 ***********************************************************************/
//	0x07, /* 0 bLength */
//	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
//	(UX_ENDPOINT_IN | 2), /* 2 bEndpointAddress */
//	UX_BULK_ENDPOINT, /* 3 bmAttributes  */
//	0x40, /* 4 wMaxPacketSize 64bytes */
//	0x00, /* 5 wMaxPacketSize */
//	0x00, /* 6 bInterval */
//};

#if defined(UX_DCD_SYNERY_USE_USBHS)
/***********************************************************************************************************************
 * USB Mass Storage Class Interface Descriptor for HS mode g_usb_interface_descriptor_storage_0
 **********************************************************************************************************************/
#if defined(__GNUC__)
static volatile const unsigned char g_usb_interface_descriptor_storage_0_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_interface_desc_hs") BSP_ALIGN_VARIABLE_V2(1)
#else /* __ICCARM__ */
#pragma section = ".usb_interface_desc_hs" 1
__root static const unsigned char g_usb_interface_descriptor_storage_0_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_interface_desc_hs")
#endif
= {
	/***********************************************************************
	 * Mass Storage Class Interface Descriptor                             *
	 ***********************************************************************/
	0x09, /* 0 bLength */
	UX_INTERFACE_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x00, /* 2 bInterfaceNumber */
	0x00, /* 3 bAlternateSetting  : Alternate for SetInterface Request */
	0x02, /* 4 bNumEndpoints      : 2 Endpoints for Interface#1 */
	0x08, /* 5 bInterfaceClass    : Mass Storage Class(0x8) */
	0x06, /* 6 bInterfaceSubClass : SCSI transparent command set(0x6) */
	0x50, /* 7 bInterfaceProtocol : BBB(0x50) */
	0x00, /* 8 iInterface Index */
	/***********************************************************************
	 * Mass Storage Class Endpoint Descriptor (Bulk-Out)  for Interface#0x00 *
	 ***********************************************************************/
	0x07, /* 0 bLength */
	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(UX_ENDPOINT_OUT | 1), /* 2 bEndpointAddress */
	UX_BULK_ENDPOINT, /* 3 bmAttributes  */
	0x00, /* 4 wMaxPacketSize : 512bytes */
	0x02, /* 5 wMaxPacketSize */
	0x00, /* 6 bInterval */
	/***********************************************************************
	 * Mass Storage Class Endpoint Descriptor (Bulk-In) for Interface#0x00 *
	 ***********************************************************************/
	0x07, /* 0 bLength */
	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(UX_ENDPOINT_IN | 2), /* 2 bEndpointAddress */
	UX_BULK_ENDPOINT, /* 3 bmAttributes  */
	0x00, /* 4 wMaxPacketSize : 512bytes */
	0x02, /* 5 wMaxPacketSize */
	0x00, /* 6 bInterval */
};
#endif

/* Size of this USB Interface Descriptor */
#define USB_IFD_SIZE_0x00       (sizeof(g_usb_interface_descriptor_storage_0_full_speed))
/* Number of Interface this USB Interface Descriptor has */
#define USB_IFD_NUM_0x00        (1)
#ifndef USB_IFD_NUM_0
#define USB_IFD_NUM_0  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_0 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_1
#define USB_IFD_NUM_1  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_1 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_2
#define USB_IFD_NUM_2  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_2 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_3
#define USB_IFD_NUM_3  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_3 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_4
#define USB_IFD_NUM_4  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_4 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_5
#define USB_IFD_NUM_5  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_5 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_6
#define USB_IFD_NUM_6  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_6 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_7
#define USB_IFD_NUM_7  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_7 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_8
#define USB_IFD_NUM_8  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_8 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_9
#define USB_IFD_NUM_9  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_9 USB_IFD_SIZE_0x00
#endif 
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#if defined(__ICCARM__)
#define ux_v2_err_callback_WEAK_ATTRIBUTE
#pragma weak ux_v2_err_callback  = ux_v2_err_callback_internal
#elif defined(__GNUC__)
#define ux_v2_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("ux_v2_err_callback_internal_mock")))
#endif
void ux_v2_err_callback(void *p_instance, void *p_data)
ux_v2_err_callback_WEAK_ATTRIBUTE;
/* Static memory pool allocation used by USBX system. */
static char g_ux_pool_memory[18432];
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void ux_v2_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void ux_v2_err_callback_internal_mock(void *p_instance, void *p_data);
void ux_v2_err_callback_internal_mock(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	printf("ux_v2_err_callback_internal_mock\n");
	//SSP_PARAMETER_NOT_USED(p_instance);
//	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	//BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

#ifdef UX_HOST_INITIALIZE
/* Function prototype for the function to notify a USB event from the USBX Host system. */
static UINT ux_system_host_change_function(ULONG event, UX_HOST_CLASS * host_class, VOID * instance);

#ifdef USB_HOST_STORAGE_CLASS_REGISTER

/* A semaphore for synchronizing to the USB media storage mount timing. */
static TX_SEMAPHORE ux_host_storage_semaphore_insertion;

/* Temporary storage place for the pointer to a USBX Host Mass Storage Class Instance. */
static UX_HOST_CLASS_STORAGE * g_ux_new_host_storage_instance;

/* Function prototype for the function to register the USBX Host Class Mass Storage. */
static void ux_host_stack_class_register_storage(void);

/* Function prototype for the function to notify changes happened to the USBX Host Class Mass Storage. */
static UINT ux_system_host_storage_change_function(ULONG event, VOID * instance);

/*******************************************************************************************************************//**
 * @brief      This is the function to register the USBX Host Class Mass Storage.
 **********************************************************************************************************************/
static void ux_host_stack_class_register_storage(void)
{
	UINT status;
	status = ux_host_stack_class_register(_ux_system_host_class_storage_name, ux_host_class_storage_entry);
	if (UX_SUCCESS != status)
	{
		ux_v2_err_callback(NULL, &status);
	}

	/* Create semaphores used for the USB Mass Storage Media Initialization. */
	status = tx_semaphore_create(&ux_host_storage_semaphore_insertion, "ux_host_storage_semaphore_insertion", 0);
	if (TX_SUCCESS != status)
	{
		ux_v2_err_callback(NULL, &status);
	}
}

/*******************************************************************************************************************//**
 * @brief      This is the function to notify changes happened to the USBX Host Class Mass Storage.
 * @param[in]  event      Event code from happened to the USBX Host system.
 * @param[in]  instance   Pointer to a USBX Host class instance, which occurs a event.
 * @retval     USBX error code
 **********************************************************************************************************************/
static UINT ux_system_host_storage_change_function(ULONG event, VOID * instance)
{
	if (UX_DEVICE_INSERTION == event) /* Check if there is a device insertion. */
	{
		g_ux_new_host_storage_instance = instance;

		/* Put the semaphore for a USBX Mass Storage Media insertion. */
		tx_semaphore_put (&ux_host_storage_semaphore_insertion);
	}
	else if (UX_DEVICE_REMOVAL == event) /* Check if there is a device removal. */
	{
		g_ux_new_host_storage_instance = NULL;
	}
	return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      This is the function to get the FileX Media Control Block for a USB Mass Storage device.
 * @param[in]      new_instance     Pointer to a USBX Host Mass Storage Class instance.
 * @param[in/out]  p_storage_media  Pointer of the pointer to save an instance of the USBX Host Mass Storage Media.
 * @param[in/out]  p_fx_media       Pointer of the pointer to save an instance of FileX Media Control Block.
 * @retval     UX_HOST_CLASS_INSTANCE_UNKNOWN  The instance provided was not for a valid Mass Storage device.
 **********************************************************************************************************************/
UINT ux_system_host_storage_fx_media_get(UX_HOST_CLASS_STORAGE * instance, UX_HOST_CLASS_STORAGE_MEDIA ** p_storage_media,
		FX_MEDIA ** p_fx_media)
{
	UINT error = UX_SUCCESS;
	UX_HOST_CLASS * p_host_class;
	UX_HOST_CLASS_STORAGE_MEDIA * p_storage_media_local;
	FX_MEDIA * p_fx_media_local;
	int index;

	/** Get the USBX Host Mass Storage Class. */
	ux_host_stack_class_get(_ux_system_host_class_storage_name, &p_host_class);

	/** Get the pointer to a media attached to the class container for USB Host Mass Storage. */
	p_storage_media_local = (UX_HOST_CLASS_STORAGE_MEDIA *) p_host_class->ux_host_class_media;

	/** Get the pointer to a FileX Media Control Block. */
	for (index = 0; index < UX_HOST_CLASS_STORAGE_MAX_MEDIA; index++)
	{
		p_fx_media_local = &p_storage_media_local->ux_host_class_storage_media;
		if (p_fx_media_local->fx_media_driver_info != instance)
		{
			/* None of FileX Media Control Blocks matched to this USBX Host Mass Storage Instance. */
			p_storage_media_local++;
		}
		else
		{
			/* Found a FileX Media Control Block used for this USBX Host Mass Storage Instance. */
			break;
		}
	}
	if (UX_HOST_CLASS_STORAGE_MAX_MEDIA == index)
	{
		error = UX_HOST_CLASS_INSTANCE_UNKNOWN;
	}
	else
	{
		*p_storage_media = p_storage_media_local;
		*p_fx_media = p_fx_media_local;
	}

	return error;
}
#endif /* USB_HOST_STORAGE_CLASS_REGISTER */

/*******************************************************************************************************************//**
 * @brief      This is the function to notify a USB event from the USBX Host system.
 * @param[in]  event     Event code from happened to the USBX Host system.
 * @param[in]  usb_class Pointer to a USBX Host class, which occurs a event.
 * @param[in]  instance  Pointer to a USBX Host class instance, which occurs a event.
 * @retval     USBX error code
 **********************************************************************************************************************/
static UINT ux_system_host_change_function(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
	UINT status = UX_SUCCESS;
	SSP_PARAMETER_NOT_USED (event);
	SSP_PARAMETER_NOT_USED (host_class);
	SSP_PARAMETER_NOT_USED (instance);

#if !defined(NULL)
	/* Call user function back for USBX Host Class event notification. */
	status = NULL(event, host_class, instance);
	if (UX_SUCCESS != status)
	{
		return status;
	}
#endif

#ifdef USB_HOST_STORAGE_CLASS_REGISTER
	/* Check the class container if it is for a USBX Host Mass Storage class. */
	if (UX_SUCCESS == _ux_utility_memory_compare(_ux_system_host_class_storage_name, host_class, _ux_utility_string_length_get(_ux_system_host_class_storage_name)))
	{
		status = ux_system_host_storage_change_function(event, instance);
	}
#endif
	return status;
}
#endif

#ifdef USB_HOST_HID_CLASS_REGISTER
/* Function prototype to register USBX Host HID Clients to the USBX Host system. */
static void ux_host_class_hid_clients_register(void);
#endif

/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void ux_common_init0(void)
 **********************************************************************************************************************/
void ux_common_init0_mock(void) {
	uint32_t status_ux_init;

	/** Initialize the USBX system. */
	printf("UX common init0 mock\n");

}
#if defined(__ICCARM__)
#define g_sf_el_ux_dcd_fs_0_err_callback_WEAK_ATTRIBUTE
#pragma weak g_sf_el_ux_dcd_fs_0_err_callback  = g_sf_el_ux_dcd_fs_0_err_callback_internal_mock
#elif defined(__GNUC__)
#define g_sf_el_ux_dcd_fs_0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_el_ux_dcd_fs_0_err_callback_internal_mock")))
#endif
void g_sf_el_ux_dcd_fs_0_err_callback(void *p_instance, void *p_data)
g_sf_el_ux_dcd_fs_0_err_callback_WEAK_ATTRIBUTE;
#if (SF_EL_UX_CFG_FS_IRQ_IPL != BSP_IRQ_DISABLED)
/* USBFS ISR vector registering. */
#if !defined(SSP_SUPPRESS_ISR_g_sf_el_ux_dcd_fs_0) && !defined(SSP_SUPPRESS_ISR_USB)
SSP_VECTOR_DEFINE_UNIT(usbfs_int_isr, USB, FS, INT, 0);
#endif
#endif

/* Prototype function for USBX DCD Initializer. */
void ux_dcd_initialize(void);

#undef SYNERGY_NOT_DEFINED
#define SYNERGY_NOT_DEFINED (1)
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_sf_el_ux_dcd_fs_0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_el_ux_dcd_fs_0_err_callback_internal_mock(void *p_instance, void *p_data);
void g_sf_el_ux_dcd_fs_0_err_callback_internal_mock(void *p_instance, void *p_data) {
	printf("ux_dcd_err-callback_internal mock\n");
}
#if ((SYNERGY_NOT_DEFINED != SYNERGY_NOT_DEFINED) && (SYNERGY_NOT_DEFINED != SYNERGY_NOT_DEFINED))
/***********************************************************************************************************************
 * The definition of wrapper interface for USBX Synergy Port DCD Driver to get a transfer module instance.
 **********************************************************************************************************************/
static UINT g_sf_el_ux_dcd_fs_0_initialize_transfer_support(ULONG dcd_io)
{
	UX_DCD_SYNERGY_TRANSFER dcd_transfer;
	dcd_transfer.ux_synergy_transfer_tx = (transfer_instance_t *)&SYNERGY_NOT_DEFINED;
	dcd_transfer.ux_synergy_transfer_rx = (transfer_instance_t *)&SYNERGY_NOT_DEFINED;
	return (UINT)ux_dcd_synergy_initialize_transfer_support(dcd_io, (UX_DCD_SYNERGY_TRANSFER *)&dcd_transfer);
} /* End of function g_sf_el_ux_dcd_fs_0_initialize_transfer_support() */
#endif

/***********************************************************************************************************************
 * Initializes USBX Device Controller Driver.
 **********************************************************************************************************************/
void ux_dcd_initialize(void) {
	int status;
	printf("Ux_dcd_initialize mock\n");
} /* End of function ux_dcd_initialize_mock() */
#if defined(__ICCARM__)
#define ux_device_err_callback_WEAK_ATTRIBUTE
#pragma weak ux_device_err_callback  = ux_device_err_callback_internal
#elif defined(__GNUC__)
#define ux_device_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("ux_device_err_callback_internal_mock")))
#endif
void ux_device_err_callback(void *p_instance, void *p_data)
ux_device_err_callback_WEAK_ATTRIBUTE;
/* Definition for the special linker section for USB */
#if defined(__GNUC__)
extern uint32_t __usb_dev_descriptor_start_fs;
extern uint32_t __usb_descriptor_end_fs;
#if defined(UX_DCD_SYNERY_USE_USBHS)
extern uint32_t __usb_dev_descriptor_start_hs;
extern uint32_t __usb_descriptor_end_hs;
#endif
#endif

/* Counter to calculate number of Interfaces */
static uint8_t g_usbx_number_of_interface_count = 0;

/***********************************************************************************************************************
 * USB Device Descriptor for FS mode
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Configuration Value Auto-calculation for USB Configuration Descriptor
 **********************************************************************************************************************/
#ifndef USB_IFD_NUM_0
#define USB_IFD_NUM_0  (0)
#define USB_IFD_SIZE_0 (0)
#endif
#ifndef USB_IFD_NUM_1
#define USB_IFD_NUM_1  (0)
#define USB_IFD_SIZE_1 (0)
#endif
#ifndef USB_IFD_NUM_2
#define USB_IFD_NUM_2  (0)
#define USB_IFD_SIZE_2 (0)
#endif
#ifndef USB_IFD_NUM_3
#define USB_IFD_NUM_3  (0)
#define USB_IFD_SIZE_3 (0)
#endif
#ifndef USB_IFD_NUM_4
#define USB_IFD_NUM_4  (0)
#define USB_IFD_SIZE_4 (0)
#endif
#ifndef USB_IFD_NUM_5
#define USB_IFD_NUM_5  (0)
#define USB_IFD_SIZE_5 (0)
#endif
#ifndef USB_IFD_NUM_6
#define USB_IFD_NUM_6  (0)
#define USB_IFD_SIZE_6 (0)
#endif
#ifndef USB_IFD_NUM_7
#define USB_IFD_NUM_7  (0)
#define USB_IFD_SIZE_7 (0)
#endif
#ifndef USB_IFD_NUM_8
#define USB_IFD_NUM_8  (0)
#define USB_IFD_SIZE_8 (0)
#endif
#ifndef USB_IFD_NUM_9
#define USB_IFD_NUM_9  (0)
#define USB_IFD_SIZE_9 (0)
#endif

#define USB_NUMBER_OF_INTERFACE USB_IFD_NUM_0 + USB_IFD_NUM_1 + USB_IFD_NUM_2 + USB_IFD_NUM_3 + USB_IFD_NUM_4 + USB_IFD_NUM_5 + USB_IFD_NUM_6 + USB_IFD_NUM_7 + USB_IFD_NUM_8 + USB_IFD_NUM_9
#define USB_TOTAL_LENGTH        USB_IFD_SIZE_0 + USB_IFD_SIZE_1 + USB_IFD_SIZE_2 + USB_IFD_SIZE_3 + USB_IFD_SIZE_4 + USB_IFD_SIZE_5 + USB_IFD_SIZE_6 + USB_IFD_SIZE_7 + USB_IFD_SIZE_8 + USB_IFD_SIZE_9

#if (0 != 0x00)
#define USB_CFG_DESCRIPTOR_bNumInterfaces     (0x00)
#else
#define USB_CFG_DESCRIPTOR_bNumInterfaces     (USB_NUMBER_OF_INTERFACE)
#endif
#if (0 != 0x00)
#define USB_CFG_DESCRIPTOR_wTotalLength       (0x00)
#else
#define USB_CFG_DESCRIPTOR_wTotalLength       (0x09 + USB_TOTAL_LENGTH)
#endif

/***********************************************************************************************************************
 * Consolidated USB Device Descriptor Framework on RAM for IAR build
 **********************************************************************************************************************/
#ifdef __ICCARM__
/* Memory area to complete USB device Descriptor Framework. */
static uint8_t device_framework_full_speed_ram[0x12 + USB_CFG_DESCRIPTOR_wTotalLength];
#if defined(UX_DCD_SYNERY_USE_USBHS)
static uint8_t device_framework_high_speed_ram[0x12 + 0x0a + USB_CFG_DESCRIPTOR_wTotalLength];
#endif
#endif

/***********************************************************************************************************************
 * USB Configuration Descriptor for FS mode
 **********************************************************************************************************************/

#if defined(UX_DCD_SYNERY_USE_USBHS)
/***********************************************************************************************************************
 * USB Configuration Descriptor for HS mode
 **********************************************************************************************************************/
#if defined(__GNUC__)
static volatile const unsigned char ux_cfg_descriptor_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_config_desc_hs") BSP_ALIGN_VARIABLE_V2(1)
#else /* __ICCARM__ */
#pragma section = ".usb_config_desc_hs" 1
__root static const unsigned char ux_cfg_descriptor_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_config_desc_hs")
#endif
= {
	0x09, /* 0 bLength */
	UX_CONFIGURATION_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(uint8_t)(USB_CFG_DESCRIPTOR_wTotalLength), /* 2 wTotalLength : This will be calculated at run-time. */
	(uint8_t)(USB_CFG_DESCRIPTOR_wTotalLength >> 8), /* 3 wTotalLength : This will be calculated at run-time. */
	(uint8_t)(USB_CFG_DESCRIPTOR_bNumInterfaces), /* 4 bNumInterfaces */
	0x01, /* 5 bConfigurationValue : Fixed to 1 since only one configuration is supported. */
	0x00, /* 6 iConfiguration */
	0x80 | (1 << 6) | (0 << 5), /* 7 bmAttributes */
	150, /* 8 bMaxPower */
};
#endif

/***********************************************************************************************************************
 * USB Language Framework (Default setting for US English)
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Function to get size of USB String Descriptor
 **********************************************************************************************************************/
#if (0)
extern const UCHAR NULL[];
static ULONG ux_device_string_descriptor_size_get(void)
{
	ULONG size = 0;
	UCHAR * ptr = (UCHAR *)NULL;
	if(NULL != ptr)
	{
		for(INT i = 0; i < 0; i++)
		{
			ptr = ptr + 3; /* bLength at byte offset 3 */
			/* Counts bLength + Language code(2bytes) + bLength(1byte) */
			size = size + *ptr + 4;
			ptr = ptr + (*ptr) + 1;
		}
	}
	return size;
}
#endif

#if (0)
extern const UCHAR NULL[];
#endif
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void ux_device_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void ux_device_err_callback_internal_mock(void *p_instance, void *p_data);
void ux_device_err_callback_internal_mock(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
//	SSP_PARAMETER_NOT_USED(p_instance);
//	SSP_PARAMETER_NOT_USED(p_data);
	printf("ux_device_err_callback_internal_mock");
	/** An error has occurred. Please check function arguments for more information. */
	//BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void ux_device_init0_mock(void)
 **********************************************************************************************************************/
void ux_device_init0_mock(void) {

	int status_ux_device_init;
	printf("ux_device_init0_mock");

	/** Calculate the size of USBX String Framework. */
	uint32_t string_framework_size;
	char *p_string_framework;

#if(0) /* Check if the number of index in USBX String Framework is greater than zero. */
	{
		p_string_framework = (UCHAR *)NULL;
		string_framework_size = ux_device_string_descriptor_size_get();
	}
#else
	{
		p_string_framework = 0;
		string_framework_size = 0;
	}
#endif

	/** Calculate the size of USB Language Framework. */
	int language_framework_size;
	char *p_language_framework;

#if(0) /* Check if the number of index in USB Language Framework is greater than zero. */
	{
		p_language_framework = (UCHAR *)NULL;
		language_framework_size = 0 * 2;
	}
#else
	{
	//	p_language_framework = (UCHAR *) language_id_framework_default;
		language_framework_size = 2;
	}
#endif

	/** Initialize the USB Device stack. */
//#ifdef __GNUC__
	status_ux_device_init = 0;
	printf("UX_device_init\n");
	/*if (UX_SUCCESS != status_ux_device_init) {
		ux_device_err_callback(NULL, &status_ux_device_init);
	}*/

}
static void g_ux_device_class_storage_setup_mock(void);
/***********************************************************************************************************************
 * USBX Mass Storage Class Parameter Setup Function.
 **********************************************************************************************************************/
//static UX_SLAVE_CLASS_STORAGE_PARAMETER g_ux_device_class_storage_parameter;
void g_ux_device_class_storage_setup_mock(void) {
	int status;


	/* Initializes the device storage class. The class is connected with interface 0 on configuration 1. */

	printf("g_ux_device_class_storage_setup_mock\n");
	status = 0;
	//assert(status == UX_SUCCESS);

	/* Counts up the number of Interfaces. */
	g_usbx_number_of_interface_count = (uint8_t)(
			g_usbx_number_of_interface_count + 1);
}
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void ux_device_class_storage_init0(void)
 **********************************************************************************************************************/
void ux_device_class_storage_init0_mock(void) {
	printf("ux_device_class_storage_init0 mock\n");
	/* Setups USB Mass Storage Class to create the instance (if required) */
	g_ux_device_class_storage_setup_mock();

	/* USB DCD initialization (Only executed once after all the class registration completed). */
	if (g_usbx_number_of_interface_count == USB_NUMBER_OF_INTERFACE) {
		/* Initializes USBX Device Control Driver */
		ux_dcd_initialize();
	}
}
//const elc_instance_t g_elc = { .p_api = &g_elc_on_elc, .p_cfg = NULL };
/* Instance structure to use this module. */
//const fmi_instance_t g_fmi = { .p_api = &g_fmi_on_fmi };
//const ioport_instance_t g_ioport =
//		{ .p_api = &g_ioport_on_ioport, .p_cfg = NULL };
//const cgc_instance_t g_cgc = { .p_api = &g_cgc_on_cgc, .p_cfg = NULL };

void g_common_init_mock(void) {

	printf("g_common_init_mock\n");
	/** Call initialization function if user has selected to do so. */
#if FX_COMMON_INITIALIZE
	fx_common_init0_mock();
#endif
	/** Call initialization function if user has selected to do so. */
#if SF_EL_FX_AUTO_INIT_sd_fx_media
	fx_media_init0();
#endif

	/** Call initialization function if user has selected to do so. */
#if (0)
	ux_common_init0();
#endif
	/** Call initialization function if user has selected to do so. */
#if (0)
	ux_device_init0_mock();
#endif
	/** Call initialization function if user has selected to do so */
#if (0)
	ux_device_class_storage_init0_mock();
#endif
}
