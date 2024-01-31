/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_audio_hal.c
 * @brief
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
#include "thread_audio_hal_Mock.h"

#include "../synergy_gen_Mock/thread_audio_Mock.h"
//#include "../thread_audio_entry.h"

#include "../types_basic_Mock.h"
#include "../Resources/Trace_Mock.h"
#include "../Drivers/I2C_2_Mock.h"




/******************************************************************************
 ** Macros
 */

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

/******************************************************************************
 ** Locals
 */

/******************************************************************************
 ** Prototypes
 */

/******************************************************************************
 ** Name:    DMA_Shot
 *****************************************************************************/
/**
 ** @brief   program the DMA to transfer audio data samples
 **
 ** @param   pSamples pointer to audio samples
 ** @param   nSamples number of samples to transfer
 **
 ******************************************************************************/
void DMA_Shot_mock (uint16_t *pSamples, uint16_t nSamples)
{
	printf("DMA Shot: %u \n", nSamples);
}

/******************************************************************************
 ** Name:    Audio_init
 *****************************************************************************/
/**
 ** @brief   initializes the audio engine. Assume that all messages are sampled
 **          at 11,025 Ksps with 12 bits resolution
 **
 ******************************************************************************/
void Audio_Init_mock(void)
{
	printf("Open DCA audio\n");
	printf("Open Timer audio\n");

	// Set the mutex's ceiling value to 1 (if necessary)
	printf("Put ceiling value to 1 to audioSemaphore\n");
	printf("Release usd mutex\n");
	ReleaseMutex(&usd_mutex);
	//tx_mutex_put(&usd_mutex);
	printf("Audio HW configuration\n");
	printf("Audio Init finished\n");
}

/******************************************************************************
 ** Name:    Audio_Play
 *****************************************************************************/
/**
 ** @brief   plays a single audio message
 **
 ** @param   pMsg  pointer to the message data samples
 ** @param   size  number of samples to transfer
 **
 ******************************************************************************/
void Audio_Play_Start_mock (const uint16_t *pSamples, uint16_t nSamples)
{
    if (pSamples && nSamples)
    {
        // set the audio player running
        Audio_Mute_Off_mock();

        // program the DMA, the DAC and the timer to play the audio message
        DMA_Shot_mock ((uint16_t *) pSamples, nSamples);
        printf("Audio Play Start: %u\n",nSamples);
    }
}

/******************************************************************************
 ** Name:    Audio_Stop
 *****************************************************************************/
/**
 ** @brief   stops the audio reproduction
 **
 ** @param   pMsg  pointer to the message data
 ** @param   size  number of samples of the audio message
 **
 ******************************************************************************/
void Audio_Stop_mock(void)
{
	printf("disable the DMA used to play audio messages\n");
	printf("completion of the current transfer \n");
	printf("write the half scale into the audio port !!!\n");
	printf("Audio Stop\n");
}

/******************************************************************************
 ** Name:    Audio_Mute
 *****************************************************************************/
/**
 ** @brief   mute the audio engine
 **
 ******************************************************************************/
void Audio_Mute(void)
{

	printf("Audio Mute\n");
}

/******************************************************************************
 ** Name:    Audio_Mute_Off
 *****************************************************************************/
/**
 ** @brief   un-mute the audio engine
 **
 ******************************************************************************/
void Audio_Mute_Off_mock(void)
{

	printf("Audio Mute Off\n");
}

/******************************************************************************
 ** Name:    Audio_Volume_Set
 *****************************************************************************/
/**
 ** @brief   set playing audio volume
 **
 ******************************************************************************/
void Audio_Volume_Set(uint8_t volume)
{
    if (volume > MAX_AUDIO_VOLUME)
    {
        volume = MAX_AUDIO_VOLUME;
    }

    if (volume < MIN_AUDIO_VOLUME)
    {
        volume = MIN_AUDIO_VOLUME;
    }
    // set the audio volume to the requested value ...
    I2C2_WriteByte (i2C_ADD_AUDIO, volume);
}
