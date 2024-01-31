/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        thread_defibrillator_hal.c
 * @brief
 *
 * @version     v1
 * @date        06/06/2018
 * @author      ltorres
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */

/******************************************************************************
 ** Includes
 */

#include "thread_defibrillator_hal_Mock.h"

#include "../synergy_gen_Mock/thread_defibrillator_Mock.h"
#include "../thread_defibrillator_entry.h"
#include "../thread_patMon_entry.h"
#include "../thread_core_entry_Mock.h"
#include "../thread_sysMon_entry_Mock.h"
#include "R100_Tables.h"



/******************************************************************************
 ** Macros
 */

/******************************************************************************
 ** Defines
 */

#define ADC_DEF_FS      0x0FFF              ///< ADC full scale (in ADCs for 12 bits resolution)
#define ADC_DEF_MV      3300                ///< ADC full scale (in millivolts)
//#define VC_FS         1815                ///< VC  full scale (in volts)
#define VC_FS           1840                ///< VC full scale (in volts)
#define VC_MIN          10                  ///< VC minimum value (in volts)
#define IH_FS           82500               ///< IH full scale (in mA = 3300 / 0,040 ohms)

#define ADC_DC_ATTN     5878                ///< Attenuation x 1000 in DC_xxx channels


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

static DD_SHOCK_t       dd_shock;           // data dictionary for the shock

static uint32_t         shock_uTime_1;      // identifies the time in the 1st phase (usecs)
static uint32_t         shock_phase;        // identifies the phase of the shock (1 -> 2 -> 3)
static DEFIB_STATE_e    defib_state;        // state of the defibrillator SM
static ERROR_ID_e       defib_error;        // defibrillator error identifier
static uint16_t         vc_sp;              // VC value set point

static uint16_t         charge_sv_vc;       // charge supervisor for VC value
static uint16_t         charge_sv_tick;     // charge supervisor tick
static uint16_t         charge_sv_ramp;     // charge supervisor ramp (in volts/200msecs)

static uint32_t         value_ih;           // IH value in mA (H-Bridge current)
static uint16_t         value_vc;           // VC value in volts
static uint16_t         value_ovch_sp;      // overcharge setpoint ack in ADCs
static uint16_t         value_dc_18V;       // +18DC value in millivolts
static uint16_t         value_dc_main;      // DC_MAIN value in millivolts
static uint32_t         value_audio_acc;    // accumulated AUDIO_IN value in ADCs
static uint32_t         value_audio_idx;    // counter used to normalize the audio energy


/******************************************************************************
 ** Prototypes
 */


/******************************************************************************
 ** Name:    Scan_Analog
 *****************************************************************************/
/**
 ** @brief   scan all analog inputs
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void Scan_Analog_mock (void)
{
   // ssp_err_t       ssp_error;      // ssp error code
    uint16_t        my_adcs;        // conversion value in ADCs
    uint32_t        my_value;       // value in variable units

    //UNUSED(ssp_error);

    // read from ADCs and update the process variables
    printf("Scan_Analog_mock: read from ADCs\n");

    if (Get_Device_Info()->develop_mode == DEVELOP_TEST_RELAY)        ///< special_mode; do not precharge
    {
        static uint32_t value = 0;
        static bool_t aux = false;
        if ((value % 50) ==0)
        {
        	 printf("Scan Analog: Get Device Info develop mode\n");
        }
        value++;
    }
}

/******************************************************************************
 ** Name:    Get_Phase1_Time
 *****************************************************************************/
/**
 ** @brief   get the phase 1 time for a predefined patient impedance
 **          Assume that the impedance table finishes with NULL items
 **
 ** @param   my_zp  patient impedance value in ohms
 **
 ** @return  phase 1 time value (in usecs)
 ******************************************************************************/
static uint32_t Get_Phase1_Time (uint16_t my_zp)
{
    uint32_t    i;

    // get the time required for the connected patient impedance
    for (i=0; energy_table[i].zp_ohms; i++)
    {
        if (my_zp <= energy_table[i].zp_ohms) { break; }
    }
    if (energy_table[i].t1_time == 0)
    {
        i--;
    }
    return ((uint32_t) energy_table[i].t1_time);
}

/******************************************************************************
 ** Name:    GPT_Shock_Callback
 *****************************************************************************/
/**
 ** @brief   callback function at shock end
 **
 ** @param   p_args        pointer to ADC descriptor (unit in use, callback event ...)
 **
 ** @return  none
 ******************************************************************************/
void GPT_Shock_Callback_mock()
{
   // ssp_err_t   ssp_error;      // ssp error code


    //UNUSED(ssp_error);

    // close the L-side controller
    printf("GPT Shock Callback mock\n");


    // read the conversion values and start a new conversion
    Scan_Analog_mock ();

    switch (shock_phase)
    {
        case 1:
            // pass to shock-phase 2
            shock_phase = 2;
            printf("GPT Shock Callback:shock phase 1\n");

            // register the shock DD (dynamic data)
            dd_shock.pulse_vc_start = value_vc;

            break;
        case 2:
            // pass to shock-phase 3
            shock_phase = 3;
            printf("GPT Shock Callback:shock phase 2\n");


            break;
        default:
            // pass to shock-phase 0
            shock_phase = 0;

            printf("GPT Shock Callback:finish shock\n");

            // register the shock DD (dynamic data)
            dd_shock.pulse_vc_middle = value_vc;
            break;
    }
}

/******************************************************************************
 ** Name:    Set_Ovch_Alarm
 *****************************************************************************/
/**
 ** @brief   set the overcharge alarm to a charge voltage
 **
 ** @param   voltage        alarm voltage or voltage to reach (in volts)
 **
 ** @return  none
 ******************************************************************************/
static void Set_Ovch_Alarm_mock (uint16_t voltage)
{
    uint32_t    my_duty;        // duty cycle used to charge the main cap
   // ssp_err_t   ssp_error;      // ssp error code

    //UNUSED(ssp_error);

    // check input parameters ...
    if (voltage > MAX_CHARGE_VOLTAGE) { voltage = MAX_CHARGE_VOLTAGE; }

    ///////////////////////////////////////////////////////
    // program the overcharge alarm ...
    printf("Set Ovch Alarm mock\n");

    // HV_OVCH_SP to set the alarm voltage level
    //   Duty Cycle    --> 0% :: 100%
    //   PWM frequency --> fixed = 50 KHz [20usec]
    my_duty = ((uint32_t) voltage * 100) / VC_FS;
    my_duty = 100 - my_duty;

}

/******************************************************************************
 ** Name:    Defib_Start_Charge
 *****************************************************************************/
/**
 ** @brief   start the charge process of the main cap
 **
 ** @param   voltage        charge voltage or voltage to reach (in volts)
 ** @param   current        set point for charge current (in milliamperes)
 **
 ** @return  none
 ******************************************************************************/
static void Defib_Start_Charge_mock(uint16_t voltage, uint16_t current)__attribute__((optimize("-O0")));
static void Defib_Start_Charge_mock(uint16_t voltage, uint16_t current)
{
    //ssp_err_t       ssp_error;      // ssp error code

    uint16_t        my_limit;       // current limit
    int32_t         my_temperature; // device temperature
    uint32_t        batt_percent;   // battery percent

    static const    uint8_t pwm_current [] = {
                                    // charging at Vbatt = 12V --> (725V -- 1430V)
                    70,             //       I total = 2,5 Amp --> (2,1s -- 6,8s ) -- set-point up to 1000
                    75,             //       I total = 3,2 Amp --> (1,8s -- 5,3s ) -- set-point up to 2000
                    80,             //       I total = 4,0 Amp --> (1,5s -- 4,3s ) -- set-point up to 3000
                    85,             //       I total = 4,6 Amp --> (1,4s -- 3,8s ) -- set-point up to 4000
                    90,             //       I total = 5,3 Amp --> (1,2s -- 4,0s ) -- set-point up to 5000 (MAX_CHARGE_CURRENT)
                    0 };            // Table end ...

   // UNUSED(ssp_error);
    printf("Defib_Start_Charge_mock\n");
    // defibrillator operative ???
    if (defib_state == eDEFIB_STATE_IN_ERROR) return;
    if (defib_state == eDEFIB_STATE_OUT_OF_SERVICE) return;

    // adjust the charge current in function of temperature
    my_temperature = Battery_Get_Temperature();

    // temp >= 25ºC     my_limit = max current (5Amp)
    // temp >= 19ºC     my_limit = (4Amp)
    // temp >= 12ºC     my_limit = (3Amp)
    // temp >=  5ºC     my_limit = (2Amp)
    // temp <   5ºC     my_limit = (1Amp)
    if (my_temperature <   5) { my_limit = 1000;               charge_sv_ramp =  9; }
    if (my_temperature >=  5) { my_limit = 2000;               charge_sv_ramp = 12; }
    if (my_temperature >= 12) { my_limit = 3000;               charge_sv_ramp = 15; }
    if (my_temperature >= 19) { my_limit = 4000;               charge_sv_ramp = 18; }
    if (my_temperature >= 25) { my_limit = MAX_CHARGE_CURRENT; charge_sv_ramp = 20; }

    //With low battery change the limits
    batt_percent = Battery_Get_Charge_mock();
    if (batt_percent <   15) { charge_sv_ramp = 5; }

    // limit the voltage and the current to charge the main capacitor ...
    if (voltage > MAX_CHARGE_VOLTAGE) { voltage = MAX_CHARGE_VOLTAGE; }
    if (current > my_limit) { current = my_limit; }

    // set the overcharge alarm !!!
    Set_Ovch_Alarm_mock (voltage);

    // set the process variables
    vc_sp = voltage;

    // assign the state ASAP
    defib_state = eDEFIB_STATE_CHARGING;

    // wait to stabilize the power supply before making a new voltage conversion
   // tx_thread_sleep (OSTIME_10MSEC);
    printf("Sleep\n");
    Sleep(OSTIME_10MSEC);
    Scan_Analog_mock ();

    // force a new conversion to be sure updating voltage values
  //  tx_thread_sleep (OSTIME_10MSEC);
    printf("Sleep\n");
    Sleep(OSTIME_10MSEC);
    Scan_Analog_mock ();

    // open the discharge capacitor relay
    printf("Defib_Start_Charge_mock\n");

    // initialize charge supervisors
    charge_sv_vc   = value_vc;
    charge_sv_tick = 0;

    ///////////////////////////////////////////////////////
    // program the fly-back power supply ...


    // 1.- enable the fly-back charger (CHRG_PSU_EN and CHRG_ONOFF signals set to High)
    {
        #define SUPER_K     100
        uint32_t i_k, j_k, k_k;

        k_k = 0;
        for (i_k=0; i_k<200; i_k++)
        {


//            if (k_k < SUPER_K) k_k += (SUPER_K/100);
            if (k_k < SUPER_K) k_k += (i_k%2);
            if (k_k > SUPER_K) k_k =  SUPER_K;
        }
    }


    // 2.- CHRG_SYNC to set the working frequency externally
    //   PWM fixed             --> 5% ON and 95% OFF
    //   Variable frequency    --> 10 KHz :: 70 KHz  [100us :: 14 us]
    //   Charge speed "golden" --> 50 KHz [20usec]

    // 3.- CHRG_SP for charge current setting
    //   PWM frequency --> used in the GPT for sync purposes (50KHz as "golden")
    //   Duty Cycle    --> 0% :: 100%

//Trace_Arg (TRACE_NEWLINE, "  PWM = %3d", pwm_current[my_duty]);
//  ssp_error = gpt_charger.p_api->dutyCycleSet(gpt_charger.p_ctrl, 70, TIMER_PWM_UNIT_PERCENT, 0x00);
//  ssp_error = gpt_charger.p_api->dutyCycleSet(gpt_charger.p_ctrl, 75, TIMER_PWM_UNIT_PERCENT, 0x00);
//  ssp_error = gpt_charger.p_api->dutyCycleSet(gpt_charger.p_ctrl, 80, TIMER_PWM_UNIT_PERCENT, 0x00);
//  ssp_error = gpt_charger.p_api->dutyCycleSet(gpt_charger.p_ctrl, 85, TIMER_PWM_UNIT_PERCENT, 0x00);
//  ssp_error = gpt_charger.p_api->dutyCycleSet(gpt_charger.p_ctrl, 90, TIMER_PWM_UNIT_PERCENT, 0x00);

    // force a new conversion to be sure updating voltage values
  //  tx_thread_sleep (OSTIME_10MSEC);
    printf("Sleep\n");
    Sleep(OSTIME_10MSEC);
    Scan_Analog_mock ();
}

/******************************************************************************
 ** Name:    Defib_Stop_Charge_mock
 *****************************************************************************/
/**
 ** @brief   stop the charge process and maintain the energy in the main cap
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void Defib_Stop_Charge_mock(void)
{
  //  ssp_err_t   ssp_error;      // ssp error code

    //UNUSED(ssp_error);

    // defibrillator operative ???
    if ((defib_state != eDEFIB_STATE_IN_ERROR) && (defib_state != eDEFIB_STATE_OUT_OF_SERVICE))
    {
        // update the state
        defib_state = eDEFIB_STATE_CHARGED;
    }

    // stop the fly-back control timer
    printf("Defib_Stop_Charge mock\n");
}

/******************************************************************************
 ** Name:    Defib_Start_Discharge_mock
 *****************************************************************************/
/**
 ** @brief   start the complete discharge of the main cap
 **
 ** @param   voltage        final voltage in the main capacitor (in volts)
 **
 ** @return  none
 ******************************************************************************/
static void Defib_Start_Discharge_mock(uint16_t voltage)
{
   // ssp_err_t   ssp_error;      // ssp error code

  //  UNUSED(ssp_error);

    // stop the charge (just in case)
    Defib_Stop_Charge_mock ();

    // connect the shock circuit to the internal resistor ...

    // discharge to zero through the relay
    if ((voltage <= VC_MIN) && (value_vc < 400))
    {
        // close the relay to discharge the main capacitor
    	 printf("Defib_Start_DisCharge mock: to zero\n");
    }
    // discharge through the H-Bridge
    else {
        // set the overcharge alarm !!!
        Set_Ovch_Alarm_mock (voltage);
   	 printf("Defib_Start_DisCharge mock:throug H-Bridge\n");

        // rearm the shock circuit ...

    }

    // set the process variables
    vc_sp = (voltage <= VC_MIN) ? 0 : voltage;

    // defibrillator operative ???
    if ((defib_state != eDEFIB_STATE_IN_ERROR) && (defib_state != eDEFIB_STATE_OUT_OF_SERVICE))
    {
        // update the state
        defib_state = eDEFIB_STATE_DISCHARGING;
    }
}

/******************************************************************************
 ** Name:    Defib_Start_Discharge_FULL
 *****************************************************************************/
/**
 ** @brief   start the complete discharge of the main cap to 0V
 **
 ** @param   voltage        final voltage in the main capacitor (in volts)
 **
 ** @return  none
 ******************************************************************************/
static void Defib_Start_Discharge_FULL_mock(uint16_t voltage)
{
    //ssp_err_t   ssp_error;      // ssp error code

    //UNUSED(ssp_error);

    // stop the charge (just in case)
    Defib_Stop_Charge_mock ();

    // connect the shock circuit to the internal resistor ...
	 printf("Defib_Start_DisCharge_FULL_mock\n");

    // set the process variables
    vc_sp = (voltage <= VC_MIN) ? 0 : voltage;

    // defibrillator operative ???
    if ((defib_state != eDEFIB_STATE_IN_ERROR) && (defib_state != eDEFIB_STATE_OUT_OF_SERVICE))
    {
        // update the state
        defib_state = eDEFIB_STATE_DISCHARGING;
    }
}

/******************************************************************************
 ** Name:    Defib_Stop_Discharge
 *****************************************************************************/
/**
 ** @brief   stop the discharge process of the main cap
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void Defib_Stop_Discharge_mock(void)
{
    //ssp_err_t   ssp_error;      // ssp error code

    //UNUSED(ssp_error);

    // defibrillator operative ???
    if ((defib_state != eDEFIB_STATE_IN_ERROR) && (defib_state != eDEFIB_STATE_OUT_OF_SERVICE))
    {
        // update the state
        defib_state = eDEFIB_STATE_STANDBY;
    }

	 printf("Defib_Stop_DisCharge_mock: to zero\n");
}

/******************************************************************************
 ** Name:    Defib_Get_Shock_DD
 *****************************************************************************/
/**
 ** @brief   report the shock dynamic data
 **
 ** @param   none
 **
 ** @return  pointer to the data structure
 ******************************************************************************/
DD_SHOCK_t *Defib_Get_Shock_DD (void)
{
    return &dd_shock;
}

/******************************************************************************
 ** Name:    Defib_Get_ErrorId
 *****************************************************************************/
/**
 ** @brief   report the error ID detected in the defibrillator
 **
 ** @param   none
 **
 ** @return  defibrillator error identifier
 ******************************************************************************/
ERROR_ID_e Defib_Get_ErrorId(void)
{
    return defib_error;
}

/******************************************************************************
 ** Name:    Defib_Get_State
 *****************************************************************************/
/**
 ** @brief   report the current state of the defibrillator
 **
 ** @param   none
 **
 ** @return  defibrillator state
 ******************************************************************************/
DEFIB_STATE_e Defib_Get_State(void)
{
    return defib_state;
}

/******************************************************************************
 ** Name:    Defib_Get_Vc
 *****************************************************************************/
/**
 ** @brief   Read the Vc value from the main capacitor
 **
 ** @param   none
 **
 ** @return  Vc voltage value in volts
 ******************************************************************************/
uint16_t Defib_Get_Vc(void)
{
    return ((value_vc < VC_MIN) ? 0 : value_vc);
}

/******************************************************************************
 ** Name:    Defib_Get_IH
 *****************************************************************************/
/**
 ** @brief   Read the IH value from the current sense circuit
 **
 ** @param   none
 **
 ** @return  IH current value in milliamperes
 ******************************************************************************/
uint32_t Defib_Get_IH(void)
{
    return (value_ih);
}

/******************************************************************************
 ** Name:    Defib_Get_Vbatt
 *****************************************************************************/
/**
 ** @brief   Read the Vbatt value from the main supply voltage
 **
 ** @param   none
 **
 ** @return  Vbattery voltage value in millivolts
 ******************************************************************************/
uint16_t Defib_Get_Vbatt(void)
{
    #define     VB_BATTERY                  12000   // nominal voltage
    #define FILTER_SZ           (100/2)               // 1 second filter (20msec each tick)
    static uint16_t filter_value_dc_main [FILTER_SZ];
    static uint16_t filter_idx = 0;
    static uint16_t first_fill = false;
    uint16_t i;
    uint32_t aux;

    if (Defib_Get_State () == eDEFIB_STATE_CHARGING)
    {
        return VB_BATTERY;
    }

    if (Defib_Get_State () != eDEFIB_STATE_CHARGING)
    {
        filter_value_dc_main [filter_idx++] = value_dc_main;
        if (filter_idx >= FILTER_SZ)
        {
            filter_idx = 0;
            first_fill = true;
        }
    }

    if (first_fill == false)        // buffer is not filled yet
    {
        return value_dc_main;
    }

    aux = 0;
    for (i=0; i<FILTER_SZ; i++)
    {
        aux += filter_value_dc_main[i];
    }

    return ((uint16_t) (aux/FILTER_SZ));
}

/******************************************************************************
 ** Name:    Defib_Get_Vdefib
 *****************************************************************************/
/**
 ** @brief   Read the Vdefib value from the defibrillator supply voltage
 **
 ** @param   none
 **
 ** @return  Vdefib voltage value in millivolts
 ******************************************************************************/
uint16_t Defib_Get_Vdefib(void)
{
#define FILTER_SZ           (100/2)               // 1 second filter (20msec each tick)
    static uint16_t filter_value_18V [FILTER_SZ];
    static uint16_t filter_idx = 0;
    static uint16_t first_fill = false;
    uint16_t i;
    uint32_t aux;

    filter_value_18V [filter_idx++] = value_dc_18V;
    if (filter_idx >= 50)
    {
        filter_idx = 0;
        first_fill = true;
    }

    if (first_fill == false)        // buffer is not filled yet
    {
        return value_dc_18V;
    }

    aux = 0;
    for (i=0; i<FILTER_SZ; i++)
    {
        aux += filter_value_18V[i];
    }

    return ((uint16_t) (aux/FILTER_SZ));
}

/******************************************************************************
 ** Name:    Defib_Set_Out_of_Service
 *****************************************************************************/
/**
 ** @brief   set the defibrillator out of service
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
void Defib_Set_Out_of_Service (void)
{
    // set out of service ONLY if is not charging nor discharging the main capacitor
    if ((defib_state == eDEFIB_STATE_STANDBY) ||
        (defib_state == eDEFIB_STATE_CHARGED) ||
        (defib_state == eDEFIB_STATE_AFTER_SHOCK))
    {
        defib_state = eDEFIB_STATE_OUT_OF_SERVICE;
    }
}

/******************************************************************************
** Name:    Defib_Disarm
 *****************************************************************************/
/**
 ** @brief   disarm the capcitor through relay
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
void Defib_Disarm_mock(void)
{
    // discharge to zero using the Relay
    Defib_Start_Discharge_FULL_mock (0);
}


/******************************************************************************
 ** Name:    Defib_Charge
 *****************************************************************************/
/**
 ** @brief   charge the main capacitor to a predefined value applying a hysteresis ...
 **
 ** @param   voltage        charge voltage or voltage to reach (in volts)
 ** @param   current        charge current (in milliamperes)
 **
 ** @return  none
 ******************************************************************************/
void Defib_Charge_mock(uint16_t voltage, uint16_t current)
{
    // check if the defibrillator is in error to ignore the command
    if (defib_state == eDEFIB_STATE_IN_ERROR) return;
    if (defib_state == eDEFIB_STATE_OUT_OF_SERVICE) return;

    // charge to a higher voltage using the flyback power supply
    if (value_vc < voltage)
    {
        if ((voltage - value_vc) > 5) 
        {
            //tx_mutex_get(&usd_mutex, OSTIME_500MSEC);
        	WaitForSingleObject(usd_mutex, INFINITE);
            printf("Mutex Get\n");
            Defib_Start_Charge_mock(voltage, current);
            ReleaseMutex(usd_mutex);
            //tx_mutex_put(&usd_mutex);
            printf("Mutex Put\n");
        }
    }

    if (value_vc > voltage)
    {
        // endurance 500 internal discharge
        if (Get_Device_Info()->develop_mode == DEVELOP_TEST_500_iSHOCK)
        {
            // discharge to zero using the Relay
            Defib_Start_Discharge_FULL_mock (0);
        }
        else
        {
            // discharge to a lower voltage using the H-bridge
            if ((value_vc - voltage) > 5) Defib_Start_Discharge_mock(voltage);
        }
    }
    printf("Defib_Charge_mock\n");
}

/******************************************************************************
 ** Name:    Defib_Shock
 *****************************************************************************/
/**
 ** @brief   shock the patient over a known impedance
 **
 ** @param   zp         patient impedance value in ohms
 **
 ** @return  none
 ******************************************************************************/
void Defib_Shock_mock(uint16_t zp)
{
   // ssp_err_t   ssp_error;              // ssp error code

    //UNUSED(ssp_error);

    // check if the defibrillator is in error or out of service to ignore the command
    if (defib_state == eDEFIB_STATE_IN_ERROR) return;
    if (defib_state == eDEFIB_STATE_OUT_OF_SERVICE) return;

    // assign the SM state
    defib_state = eDEFIB_STATE_SHOCKING;

    // open the discharge capacitor relay and connect the shock circuit ...
	 printf("Defib_Shock_mock\n");

    // start a new conversion
    Scan_Analog_mock ();

    // register the shock DD (dynamic data)
    dd_shock.zp = zp;
    dd_shock.shock_uTime_1 = (uint16_t) shock_uTime_1;

    /////////////////////////////////////////////////////////////////////
    // set PWM for upper semiconductors in H-Bridge ...


    // Do not disturb ... and wait for shock done condition ...
    //tx_thread_sleep (OSTIME_40MSEC);
    printf("Sleep\n");
    Sleep(OSTIME_40MSEC);

    // register the shock DD (dynamic data) and start a new conversion
    Scan_Analog_mock ();
    dd_shock.pulse_vc_end = value_vc;

    // check if the shock has been done properly and assign the SM state
    // MUST discharge at least 100 volts in each phase ...
    if ((dd_shock.pulse_vc_start  > dd_shock.pulse_vc_middle) && ((dd_shock.pulse_vc_start  - dd_shock.pulse_vc_middle) >= 100) &&
        (dd_shock.pulse_vc_middle > dd_shock.pulse_vc_end)    && ((dd_shock.pulse_vc_middle - dd_shock.pulse_vc_end)    >= 100))
    {
        defib_state = eDEFIB_STATE_AFTER_SHOCK;
    }
    else {
        defib_state = eDEFIB_STATE_IN_ERROR;
        defib_error = eERR_DEF_SHOCK;
//        if ((pMisc_Settings->glo_develop_mode == DEVELOP_TEST_2500_eSHOCK) ||
//            (pMisc_Settings->glo_develop_mode == DEVELOP_TEST_500_iSHOCK))
//        {
//            defib_state = eDEFIB_STATE_AFTER_SHOCK;
//        }
    }
    printf("Defib_Shock_mock\n");
//    SSP_CRITICAL_SECTION_DEFINE;        // enables the use of critical section in this function ...
//    SSP_CRITICAL_SECTION_ENTER;
//    ... perform critical operations here
//    SSP_CRITICAL_SECTION_EXIT;
}

/******************************************************************************
 ** Name:    Defib_Shock_Sync
 *****************************************************************************/
/**
 ** @brief   shock the patient over a known impedance synchronized with
 **          a QRS pulse
 **
 ** @param   zp         patient impedance value in ohms
 **
 ** @return  none
 ******************************************************************************/
void Defib_Shock_Sync_mock (uint16_t zp)
{
    // monitor the active process checking the defibrillator condition
    while (defib_state != eDEFIB_STATE_CHARGED)
    {
        if ((defib_state == eDEFIB_STATE_STANDBY)  ||                   // proceed to charge the capacitor !!!
            (defib_state == eDEFIB_STATE_IN_ERROR) ||                   // the defibrillator must be operative !!!
            (defib_state == eDEFIB_STATE_OUT_OF_SERVICE)) return;       // the defibrillator must be in service !!!

        // execute the defibrillator monitor
        Defib_Monitor_mock ();

        // be polite waiting to finish the current process ...
        //tx_thread_sleep (OSTIME_20MSEC);
        printf("Sleep\n");
        Sleep(OSTIME_20MSEC);
    }

    // wait for a Sync pulse with a timeout ...
    Wait_For_Sync_mock (400);
    Defib_Shock_mock(zp);
    printf("Defib_Shock_Sync_mock\n");
}

/******************************************************************************
 ** Name:    Defib_Monitor
 *****************************************************************************/
/**
 ** @brief   executes the defibrillator monitor step to read analog
 **          inputs and check that all processes are running properly
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
void Defib_Monitor_mock(void)
{
   // ssp_err_t           ssp_error;      // ssp error code

    int32_t             vc_inc;         // voltage increment since the last check-point
    static  uint32_t    nFails;         // number of consecutive failures
    static  uint32_t    tout;           // tout counter

    //UNUSED(ssp_error);

    // read from ADCs and update the process variables
    Scan_Analog_mock ();

    // read the over-charge flag
	 printf("Defib_Monitor_mock\n");

    ///////////////////////////////////////////////////////////////////////
    // check the main capacitor voltage value (Vc) when charging ...
    if (defib_state == eDEFIB_STATE_CHARGING)
    {
        // update the tick and the voltage increment
        charge_sv_tick++;
        vc_inc = (int32_t) value_vc - (int32_t) charge_sv_vc;

        // the final voltage has been reach. Stop the charge process
        if ((value_vc >= vc_sp))
        {
            // restart the number of consecutive fails counter
            nFails = 0;
            tout = 0;

            Defib_Stop_Charge_mock();

            // check the dV/dt ramp at the end of charge (fast charge -- higher than 2000V/sec)
            // "charge_sv_tick" is incremented every 20 msecs.
            if (((vc_inc * 50) / charge_sv_tick) > 2000)
            {
                defib_state = eDEFIB_STATE_IN_ERROR;
                defib_error = eERR_DEF_CHARGING_FAST;
            }
        }
        else
        {
            // make some verifications every 200 msecs ...
            if ((charge_sv_tick % 10) == 0)
            {
                // update voltage supervisors ...
                charge_sv_vc = value_vc;

                tout++;

                // Check the charging process (slow charge -- lower than "charge_sv_ramp" V/200msec)
                // For voltage higher than 1300V, check the charger process by time
                if ((vc_inc < charge_sv_ramp) && (charge_sv_vc < 1300))
                {
                    nFails++;
                }
                else {
                    nFails = 0;     // restart the number of consecutive fails counter
                }

                // check the dV/dt ramp (fast charge -- higher than 400V/200msec = 2000V/sec)
                if (vc_inc > 400)
                {
                    defib_state = eDEFIB_STATE_IN_ERROR;
                    defib_error = eERR_DEF_CHARGING_FAST;
                }
            }

            // ignore slow detection at the end of the charge process
            // to avoid false positives due to the "HV_OVCH_FLAG_N" oscillation
            if ((nFails >= 5) && (value_vc > ((vc_sp * 90) / 100)))
            {
                // restart the number of consecutive fails counter
                nFails = 0;
                tout = 0;

                Defib_Stop_Charge_mock();
            }


            // Consecutive fails or timeout of 25 seconds 25000/200 = 125
            if ((nFails >= 5) || (tout >= 125))
            {
                defib_state = eDEFIB_STATE_IN_ERROR;
                defib_error = eERR_DEF_CHARGING_SLOW;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////
    // check the main capacitor voltage value (Vc) when discharging ...
    if (defib_state == eDEFIB_STATE_DISCHARGING)
    {
        // restart the number of consecutive fails counter
        nFails = 0;
        tout = 0;

        if (value_vc < PRE_CHARGE_VOLTAGE/2)
        {
            // stop the discharge through the H-Bridge
            Defib_Stop_Discharge_mock ();

            // close the relay to discharge the main capacitor

        }
        else if ((value_vc <= vc_sp)) {
            // stop the discharge through the H-Bridge and standby with the Vc charged
            Defib_Stop_Discharge_mock ();

            // reassign the process state
            defib_state = eDEFIB_STATE_CHARGED;
        }
    }

    ///////////////////////////////////////////////////////////////////////
    // manage the error or out of service condition ...
    if ((defib_state == eDEFIB_STATE_IN_ERROR) || (defib_state == eDEFIB_STATE_OUT_OF_SERVICE))
    {
        // clear the set-point and stop defibrillator operations
        vc_sp = 0;
        Defib_Stop_Charge_mock();
        Defib_Stop_Discharge_mock();

        // restart the number of consecutive fails counter
        nFails = 0;
        tout = 0;

        // Power Supply Disable for the defibrillator functions (charge, discharge & shock)

    }
}

/******************************************************************************
 ** Name:    Defib_Pulse_Off
 *****************************************************************************/
/**
 ** @brief   restart the defibrillator pulse circuits
 **
 ** @param   none
 **
 ** @return  none
 ******************************************************************************/
static void Defib_Pulse_Off_mock(void)
{
    // set all iGBTs in off
	 printf("Defib_Pulse_Off mock\n");
}

/******************************************************************************
 ** Name:    Defib_Initialize
 *****************************************************************************/
/**
 ** @brief   initializes the defibrillator controller
 **
 ** @param   none
 **
 ** @return  initialization result
 ******************************************************************************/
ERROR_ID_e Defib_Initialize_mock(void)
{
   // ssp_err_t   ssp_error;      // ssp error code

   // UNUSED(ssp_error);

    // initialize the defibrillation circuits
    Defib_Pulse_Off_mock();

	 printf("Defib_Initialize mock\n");

    return eERR_NONE;
}

/******************************************************************************
 ** Name:    Get_Audio_Energy
 *****************************************************************************/
/**
 ** @brief   return the audio energy measured in ADCs
 **
 ** @param   none
 **
 ** @return  normalized audio energy
 ******************************************************************************/
uint32_t Get_Audio_Energy(void)
{
    uint32_t    my_energy;      // accumulated energy value in ADCs

    // calculate the normalized energy
    my_energy = value_audio_idx ? (value_audio_acc / value_audio_idx) : 0;

    // restart the energy integration variables
    value_audio_acc = value_audio_idx = 0;

    return my_energy;
}
