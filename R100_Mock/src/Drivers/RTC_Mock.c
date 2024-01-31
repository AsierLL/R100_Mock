/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        RTC.c
 * @brief       All functions related to the RTC
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
#include <stdio.h>

#include "../synergy_gen_Mock/hal_data_Mock.h"
#include "../types_basic_Mock.h"
#include "RTC.h"
#include "../Resources/Keypad_Mock.h"

/******************************************************************************
 ** Macros
 */

/******************************************************************************
 ** Defines
 */

// define the VBATT controller address and structure
typedef struct
{
    union
    {
        uint8_t BKRACR; /*!< Backup Register Access Control Register (BKRACR)      */
        struct
        {
            uint8_t BKRACS :3; /*!< Backup Register Access cycle Select            */
        } BKRACR_b; /*!< BitSize                                                        */
    };
    uint8_t RESERVED1[856];
    union
    {
        uint8_t VBTCR1; /*!< VBATT Control Register 1 (VBTCR1)                     */
        struct
        {
            uint8_t BPWSWSTP :1; /*!< Battery Power Supply Switch Stop             */
        } VBTCR1_b; /*!< BitSize                                                        */
    };
    uint8_t RESERVED2[144];
    union
    {
       uint8_t VBTCR2; /*!< VBATT Control Register 2 (VBTCR2)                     */
        struct
        {
            uint8_t :4;
             uint8_t VBTLVDEN  :1; /*!< VBATT Pin Low Voltage Detect Enable         */
             uint8_t :1;
            uint8_t VBTLVDLVL :2; /*!< VBATT Pin Low Voltage Detect Level Select   */
        } VBTCR2_b; /*!< BitSize                                                        */
    };
    union
    {
         uint8_t VBTSR; /*!< VBATT Status Register (VBTSR)                          */
        struct
        {
            uint8_t VBTRDF  :1; /*!< VBATT_R Reset Detect Flag                     */
             uint8_t VBTBLDF :1; /*!< VBATT Battery Low Detect Flag                 */
            uint8_t :2;
             uint8_t VBTRVLD :1; /*!< VBATT_R Valid                                 */
        } VBTSR_b; /*!< BitSize                                                         */
    };
    union
    {
        uint8_t VBTCMPCR; /*!< VBATT Comparator Control register (VBTCMPCR)        */
        struct
        {
            uint8_t VBTCMPE :1; /*!< Battery Power Supply Switch Stop              */
        } VBTCMPCR_b; /*!< BitSize                                                      */
    };

    uint8_t RESERVED3;
    union
    {
         uint8_t VBTLVDICR; /*!< VBATT Pin Low Voltage Detect Interrupt Control Register (VBTLVDICR) */
        struct
        {
             uint8_t VBTLVDIE   :1; /*!< VBATT Pin Low Voltage Detect Interrupt Enable               */
             uint8_t VBTLVDISEL :1; /*!< Pin Low Voltage Detect Interrupt Select                     */
        } VBTLVDICR_b; /*!< BitSize                                                                      */
    };
    uint8_t RESERVED4;
    union
    {
         uint8_t VBTWCTLR; /*!< VBATT Wakeup Control Register (VBTWCTLR)                             */
        struct
        {
             uint8_t VWEN :1; /*!< VBATT Wakeup Enable                                               */
        } VBTWCTLR_b; /*!< BitSize                                                                       */
    };
     uint8_t RESERVED5;
    union
    {
         uint8_t VBTWCH0OTSR; /*!< VBATT Wakeup I/O 0 Output Trigger Select Register (VBTWCH0OTSR)   */
        struct
        {
             uint8_t :1;
             uint8_t CH0VCH1TE  :1; /*!< VBATWIO0 Output VBATWIO1 Trigger Enable                     */
             uint8_t CH0VCH2TE  :1; /*!< VBATWIO0 Output VBATWIO2 Trigger Enable                     */
             uint8_t CH0VRTCTE  :1; /*!< VBATWIO0 Output RTC Periodic Signal Enable                  */
             uint8_t CH0VRTCATE :1; /*!< VBATWIO0 Output RTC Alarm Signal Enable                     */
             uint8_t CH0VAGTUTE :1; /*!< VBATWIO0 Output AGT Underflow Signal Enable                 */
        } VBTWCH0OTSR_b; /*!< BitSize                                                                    */
    };
    union
    {
         uint8_t VBTWCH1OTSR; /*!< VBATT Wakeup I/O 1 Output Trigger Select Register (VBTWCH1OTSR)   */
        struct
        {
             uint8_t CH1VCH0TE  :1; /*!< VBATWIO1 Output VBATWIO0 Trigger Enable                     */
             uint8_t :1;
             uint8_t CH1VCH2TE  :1; /*!< VBATWIO1 Output VBATWIO2 Trigger Enable                     */
             uint8_t CH1VRTCTE  :1; /*!< VBATWIO1 Output RTC Periodic Signal Enable                  */
             uint8_t CH1VRTCATE :1; /*!< VBATWIO1 Output RTC Alarm Signal Enable                     */
             uint8_t CH1VAGTUTE :1; /*!< VBATWIO1 Output AGT underflow Signal Enable                 */
        } VBTWCH1OTSR_b; /*!< BitSize                                                                    */
    };
    union
    {
         uint8_t VBTWCH2OTSR; /*!< VBATT Wakeup I/O 2 Output Trigger Select Register (VBTWCH2OTSR)   */
        struct
        {
            uint8_t CH2VCH0TE  :1; /*!< VBATWIO2 Output VBATWIO0 Trigger Enable                     */
             uint8_t CH2VCH1TE  :1; /*!< VBATWIO2 Output VBATWIO1 Trigger Enable                     */
             uint8_t :1;
            uint8_t CH2VRTCTE  :1; /*!< VBATWIO2 Output RTC Periodic Signal Enable                  */
            uint8_t CH2VRTCATE :1; /*!< VBATWIO2 Output RTC Alarm Signal Enable                     */
             uint8_t CH2VAGTUTE :1; /*!< VBATWIO2 Output AGT underflow Signal Enable                 */
        } VBTWCH2OTSR_b; /*!< BitSize                                                                    */
    };
    union
    {
        uint8_t VBTICTLR; /*!< VBATT Input Control Register (VBTICTLR)                 */
        struct
        {
             uint8_t VCH0INEN :1; /*!< VBATT Wakeup I/O 0 Input Enable                  */
             uint8_t VCH1INEN :1; /*!< VBATT Wakeup I/O 1 Input Enable                  */
             uint8_t VCH2INEN :1; /*!< VBATT Wakeup I/O 2 Input Enable                  */
        } VBTICTLR_b; /*!< BitSize                                                          */
    };
    union
    {
        uint8_t VBTOCTLR; /*!< VBATT Output Control Register (VBTOCTLR)                */
        struct
        {
             uint8_t VCH0OEN   :1; /*!< VBATT Wakeup I/O 0 Output Enable                */
             uint8_t VCH1OEN   :1; /*!< VBATT Wakeup I/O 1 Output Enable                */
             uint8_t VCH2OEN   :1; /*!< VBATT Wakeup I/O 2 Output Enable                */
             uint8_t VOUT0LSEL :1; /*!< VBATT Wakeup I/O 0 Output Level Selection       */
             uint8_t VOUT1LSEL :1; /*!< VBATT Wakeup I/O 1 Output Level Selection       */
             uint8_t VOUT2LSEL :1; /*!< VBATT Wakeup I/O 2 Output Level Selection       */
        } VBTOCTLR_b; /*!< BitSize                                                          */
    };
    union
    {
         uint8_t VBTWTER; /*!< VBATT Wakeup Trigger Source Enable Register (VBTWTER)    */
        struct
        {
             uint8_t VCH0E  :1; /*!< VBATWIO0 Pin Enable                                */
             uint8_t VCH1E  :1; /*!< VBATWIO1 Pin Enable                                */
             uint8_t VCH2E  :1; /*!< VBATWIO2 Pin Enable                                */
             uint8_t VRTCIE :1; /*!< RTC Periodic Signal Enable                         */
             uint8_t VRTCAE :1; /*!< RTC Alarm Signal Enable                            */
             uint8_t VAGTUE :1; /*!< AGT1 underflow Signal Enable                       */
        } VBTWTER_b; /*!< BitSize                                                           */
    };
    union
    {
        uint8_t VBTWEGR; /*!< VBATT Wakeup Trigger Source Edge Register (VBTWEGR)      */
        struct
        {
             uint8_t VCH0EG :1; /*!< VBATWIO0 Wakeup Trigger Source Edge Select         */
             uint8_t VCH1EG :1; /*!< VBATWIO1 Wakeup Trigger Source Edge Select         */
             uint8_t VCH2EG :1; /*!< VBATWIO2 Wakeup Trigger Source Edge Select         */
        } VBTWEGR_b; /*!< BitSize                                                           */
    };
    union
    {
        uint8_t VBTWFR; /*!< VBATT Wakeup Trigger Source Flag Register (VBTWFR)        */
        struct
        {
            uint8_t VCH0F  :1; /*!< VBATWIO0 Wakeup Trigger Flag                       */
             uint8_t VCH1F  :1; /*!< VBATWIO1 Wakeup Trigger Flag                       */
           uint8_t VCH2F  :1; /*!< VBATWIO2 Wakeup Trigger Flag                       */
             uint8_t VRTCIF :1; /*!< RTC Periodic Wakeup Trigger Flag                   */
             uint8_t VRTCAF :1; /*!< RTC Alarm Wakeup Trigger Flag                      */
             uint8_t VAGTUF :1; /*!< AGT1 underflow Wakeup Trigger Flag                 */
        } VBTWFR_b; /*!< BitSize                                                            */
    };
     uint8_t RESERVED6[64];
    union
    {
        uint8_t VBTBKRn[512]; /*!< VBATT Backup Register %s                            */
        struct
        {
           uint8_t VBTBKR :8; /*!< VBTBKR is a 512-byte readable/writeable register
             to store data powered by VBATT.The value of this register is retained even
             when VCC is not powered but VBATT is powered.VBTBKR is initialized
             by VBATT selected voltage power-on-reset.                                      */
        } VBTBKRn_b[512]; /*!< BitSize                                                      */
    };

} R_VBATT_Type;

#define R_VBATT_BASE                        0x4001E0C6UL
#define R_VBATT                             ((R_VBATT_Type *) R_VBATT_BASE)

//#define ACCELERATOR_TEST    (START_TIME_YEAR +  0)            // do not force a test
#define ACCELERATOR_TEST    (START_TIME_YEAR +  5)              // force a test every 5 startups !!!
//#define ACCELERATOR_TEST    (START_TIME_YEAR + 10)              // force a test every 10 startups !!!
//#define ACCELERATOR_TEST    (START_TIME_YEAR + 20)            // force a test every 20 startups !!!
//#define ACCELERATOR_TEST    (START_TIME_YEAR + 40)              // force a test every 40 startups !!!


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

static uint32_t     rtc_test_counter;       // test counter
//static rtc_time_t   rtc_time;               // RTC start time



/******************************************************************************
 ** Prototypes
 */

/******************************************************************************
 ** Name:    rtc_init
 *****************************************************************************/
/**
 ** @brief   Entry point to use RTC timer.
 ** @param   void
 **
 ** @return  void
 ** @todo    pending things to be done
 ******************************************************************************/
void Rtc_Init(void)
{
    // maintain the power-on active (just in case)
    // g_ioport.p_api->pinWrite (RTCIO_PIN, IOPORT_LEVEL_HIGH);
    // g_ioport.p_api->pinWrite (LED_SHOCK, IOPORT_LEVEL_LOW);

    // Initialize RTC

	printf("Rtc_Init\n");

//  rtc_time.tm_year  = ACCELERATOR_TEST;           // DO NOT assign the year. Use as test counter


}

/******************************************************************************
 ** Name:    Rtc_Test_Trigger
 *****************************************************************************/
/**
 ** @brief   Trigger or validate the automatic test
 **          The "TestCounter" is used to accelerate the automatic test
 **          in order to check the test functionality
 **
 ** @param   none
 **
 ** @return  true if must proceed to test (TestCounter = 1) condition
 ******************************************************************************/
bool_t Rtc_Test_Trigger (void)
{
	printf("Rtc Test Trigger\n");
    return false;
    return (rtc_test_counter == ACCELERATOR_TEST);
}

/******************************************************************************
 ** Name:    Rtc_Program_Wakeup
 *****************************************************************************/
/**
 ** @brief   program the wakeup time in the internal RTC (use the alarm to
 **          activate the RTCIC0)
 **
 ** @param   msec --- wakeup time in milliseconds
 **
 ** @return  none
 ******************************************************************************/
void Rtc_Program_Wakeup(uint32_t msec)
{
	printf("Rtc Programm Wakeup\n");
}

/******************************************************************************
 ** Name:    Rtc_Program_Kill
 *****************************************************************************/
/**
 ** @brief   program the wakeup time in the internal RTC and kill the processor
 **
 ** @param   msec --- wakeup time in milliseconds
 **
 ** @return  none
 ******************************************************************************/
void Rtc_Program_Kill (uint32_t msec)
{
    uint8_t             pin_state;      // RTCIO pin state

    Rtc_Program_Wakeup (msec);

    // wait to switch off ALL power supplies
    printf("Rtc_Programm_Kill\n");

}
