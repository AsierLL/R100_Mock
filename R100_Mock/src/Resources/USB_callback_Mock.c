/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        Keypad.c
 * @brief       All functions related to the keyboard and LEDs manage
 *
 * @version     v1
 * @date        16/01/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

/******************************************************************************
 ** Includes
 */

//#include "bsp_api_Mock.h"
#include "../../synergy_Mock/ssp/inc/framework/el/ux_api_Mock.h"

#include "../types_basic_Mock.h"
#include "../synergy_gen_Mock/common_data_Mock.h"

/******************************************************************************
 ** Macros
 */

#define USB_ACCESS_KEY      "ESTEFANYDAT"     // key to unlock the access to uSD through USB port
#define USB_ACCESS_FILE     "estefany.dat"    // filename created to unlock the uSD access

#define uSD_LOCKED          1
#define uSD_UNLOCKED        0



/******************************************************************************
 ** Defines
 */

/******************************************************************************
 ** Typedefs
 */

/******************************************************************************
 ** Constants
 */

/******************************************************************************
 ** Externs
 */

/******************************************************************************
 ** Globals
 */

// uint8_t access_key [12] = USB_ACCESS_KEY;           // access key
extern uint8_t      access_key[12];

/******************************************************************************
 ** Locals
 */

//static  FX_MEDIA   *sd_media = (FX_MEDIA *)NULL;    // handler of the SD media card
static  uint8_t     uSD_state = 3;                  // enable a number of writing operations before lock the device
                                                    // 1 --> locked device
                                                    // 0 --> unlocked device

/******************************************************************************
 ** Prototypes
 */


/******************************************************************************
 ** Name:    ux_device_msc_media_status
 *****************************************************************************/
/**
 ** @brief   USBX media status callback function
 **
 ** @param   storage        instance of the storage class
 ** @param   lun            LUN (logical unit number)
 ** @param   media_id       is not currently used (always zero)
 ** @param   media_status   must be filled out exactly like the media status callback return value
 **
 ** @return  uint           media status error code (see USBX Device Stack User Guide)
 **
 ******************************************************************************/
UINT ux_device_msc_media_status_mock()
{
	printf("Ux_Msc_MediaStatus Alawys ok mock \n");
    return (UX_SUCCESS);
}

/******************************************************************************
 ** Name:    ux_device_msc_media_read
 *****************************************************************************/
/**
 ** @brief   USBX read media callback function
 **
 ** @param   storage        instance of the storage class
 ** @param   lun            LUN (logical unit number)
 ** @param   data_pointer   address of the buffer to be used for reading
 ** @param   number_blocks  the number of sectors to read/write
 ** @param   lba            the sector address to read
 ** @param   media_status   must be filled out exactly like the media status callback return value
 **
 ** @return  uint           UX_SUCCESS or UX_ERROR indicating the operation result
 **
 ******************************************************************************/
UINT ux_device_msc_media_read_mock(void* storage,
                                ULONG  lun,
                                UCHAR* data_pointer,
                                ULONG  number_blocks,
                                ULONG  lba,
                                ULONG* media_status)
{
    UINT    fx_res;
    ULONG   i;

   // UNUSED(storage);
   // UNUSED(lun);

    printf("ux_device_msc_media_read_mock\n");

   // sd_media = &sd_fx_media;
    //fx_media_flush (sd_media);
    /* return the sectors requested by the host */
    for (i=0; i<number_blocks; i++, lba++)
    {
      /*  fx_res = fx_media_read (sd_media, lba, &data_pointer[i*512]);
        if (fx_res != FX_SUCCESS)
        {
            fx_media_flush (sd_media);
            *media_status = UX_ERROR;
            return UX_ERROR;
        }*/
    }

    // successful operation
    *media_status = UX_SUCCESS;
    return (UX_SUCCESS);
}

/******************************************************************************
 ** Name:    ux_device_msc_media_write
 *****************************************************************************/
/**
 ** @brief   USBX write media callback function
 **
 ** @param   storage        instance of the storage class
 ** @param   lun            LUN (logical unit number)
 ** @param   data_pointer   address of the buffer to be used for writing
 ** @param   number_blocks  the number of sectors to read/write
 ** @param   lba            the sector address to read
 ** @param   media_status   must be filled out exactly like the media status callback return value
 **
 ** @return  uint           UX_SUCCESS or UX_ERROR indicating the operation result
 **
 ******************************************************************************/
UINT ux_device_msc_media_write_mock(VOID* storage,
                                ULONG  lun,
                                UCHAR* data_pointer,
                                ULONG  number_blocks,
                                ULONG  lba,
                                ULONG* media_status)
{
    UINT    fx_res;
    ULONG   i;

  //  UNUSED(storage);
  //  UNUSED(lun);
    printf("ux_device_msc_media_write_mock\n");

    // manage the unlock controller for the uSD device
    if (uSD_state > uSD_LOCKED)
    {
        uSD_state--;
        for (i=0; i<(number_blocks * 512); i++)
        {
            if (memcmp (&data_pointer[i], access_key, sizeof(USB_ACCESS_KEY)-1) == 0)
            {
                uSD_state = uSD_UNLOCKED;
                break;
            }
        }
    }
    if (uSD_state == uSD_LOCKED) return UX_ERROR;

    /*sd_media = &sd_fx_media;

    if (lba < sd_media->fx_media_reserved_sectors)
    {
        // successful operation
        *media_status = UX_SUCCESS;
        return (UX_SUCCESS);
    }*/

    /* write the sectors requested by the host */
    for (i=0; i<number_blocks; i++, lba++)
    {
      /*  fx_res = fx_media_write (sd_media, lba, &data_pointer[i*512]);
        if (fx_res != FX_SUCCESS)
        {
            fx_media_flush (sd_media);
            *media_status = UX_ERROR;
            return UX_ERROR;
        }*/
    }

    // ... flush data to physical media
    //fx_res = fx_media_flush (sd_media);
    *media_status = (fx_res == 0) ? UX_SUCCESS : UX_ERROR;

    // return the operation result
    return (*media_status);
}
