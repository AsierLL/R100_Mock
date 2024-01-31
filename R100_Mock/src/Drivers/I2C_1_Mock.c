/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        I2C_1.c
 * @brief       All functions related to the I2C-1
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

// include the i2C semaphore to access to different devices
#include "I2C_1_Mock.h"

#include "../synergy_gen_Mock/thread_sysMon_Mock.h"
#include "../synergy_gen_Mock/hal_data_Mock.h"

/******************************************************************************
 ** Macros
 */

/******************************************************************************
 ** Defines
 */

// I/O ports
#define     I2C1_SCL            IOPORT_PORT_04_PIN_00
#define     I2C1_SDA            IOPORT_PORT_04_PIN_01

//#define     I2C1_CLK_PULSE      { g_ioport.p_api->pinWrite (I2C1_SCL, IOPORT_LEVEL_HIGH); \\
		                          g_ioport.p_api->pinWrite (I2C1_SCL, IOPORT_LEVEL_LOW); }



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
 ** Name:    I2C1_Init
 *****************************************************************************/
/**
 ** @brief   Entry point to use I2C-2.
 **
 ******************************************************************************/
void I2C1_Init(void)
{
	printf("I2C1 Init\n");
}

/******************************************************************************
 ** Name:    I2C1_StartCondition
 *****************************************************************************/
/**
 ** @brief   generate the START condition in the i2C line
 **
 ******************************************************************************/
static void I2C1_StartCondition (void)
{
    printf("I2C1 StartCondition\n");
}

/******************************************************************************
 ** Name:    I2C1_StopCondition
 *****************************************************************************/
/**
 ** @brief   generate the STOP condition in the i2C line
 **
 ******************************************************************************/
static void I2C1_StopCondition (void)
{
	printf("I2C1 StopCondition\n");
}

/******************************************************************************
 ** Name:    I2C1_TX_Byte
 *****************************************************************************/
/**
 ** @brief   Function to send a data byte to the i2C device
 **
 ** @param   data --- byte to send
 ** @return  operation error code (0 if no error)
 **
 ******************************************************************************/
static uint8_t I2C1_TX_Byte(uint8_t data)
{
    uint8_t i, mask;

    // send the data
    mask = 0x80;
    for (i=0; i<8; i++)
    {
         printf("I2C1 TX Byte: %u\n", data);
    }

    // receive ACK from slave.

    //MOCK version:
    printf("ACK receive from slave\n");


    // return the error code in function of the slave acknowledge.

    //MOCK version: always OK
    return mask;
}

/******************************************************************************
 ** Name:    I2C1_RX_Byte
 *****************************************************************************/
/**
 ** @brief   Function to get a data byte from the i2C device
 **
 ** @param   ack_flag   if the operation must ACK the slave or not
 ** @return  read data
 **
 ******************************************************************************/
static uint8_t I2C1_RX_Byte (bool_t ack_flag)
{
    uint32_t i, my_data;
    uint8_t  mask;

    // read the data
    my_data = 0x00;
    printf("I2C1 RX Byte: %u\n", my_data);



    // return the read data.
    //Mock version: Always 0x00
    return ((uint8_t) my_data);
}

/******************************************************************************
 ** Name:    I2C1_ReadByte
 *****************************************************************************/
/**
 ** @brief   Function to read a byte data from a specific address
 **
 ** @param   addr   --- slave device address
 ** @param   pData  --- Pointer to memory to store the read data
 **
 ******************************************************************************/
void I2C1_ReadByte(uint8_t addr, uint8_t *pData)
{
    uint8_t error_code;     // error code in the i2C communication

    // generates the start condition
    I2C1_StartCondition ();

    // write the address with READ condition
    error_code = I2C1_TX_Byte (addr | 0x01);
    if (!error_code)
    {
        *pData = I2C1_RX_Byte (FALSE);
    }

    // generates the stop condition
    I2C1_StopCondition ();
}

/******************************************************************************
 ** Name:    I2C1_Read
 *****************************************************************************/
/**
 ** @brief   Function to read data from a specific address
 **
 ** @param   addr   --- slave device address
 ** @param   data   --- Pointer to memory to store the read data
 ** @param   nBytes --- data size in bytes
 **
 ******************************************************************************/
void I2C1_Read(uint8_t addr, uint8_t *data, uint8_t nBytes)
{
    uint8_t error_code;     // error code in the i2C communication
    uint8_t i;              // global use counter

    //check input parameters ...
    if (!data || !nBytes) return;

    // generates the start condition
    I2C1_StartCondition ();

    // write the address with READ condition
    error_code = I2C1_TX_Byte (addr | 0x01);
    if (!error_code)
    {
        // read data bytes ...
        for (i=0; i<(nBytes-1); i++)
        {
            data[i] = I2C1_RX_Byte (TRUE);
        }

        data[i] = I2C1_RX_Byte (FALSE);
    }
    else {
        memset (data, 0, nBytes);
    }

    // generates the stop condition
    I2C1_StopCondition ();
}

/******************************************************************************
 ** Name:    I2C1_WriteByte
 *****************************************************************************/
/**
 ** @brief   Function to write a data byte at specific address
 **
 ** @param   addr --- slave device address
 ** @param   data --- a byte data to write
 **
 ******************************************************************************/
void I2C1_WriteByte(uint8_t addr, uint8_t data)
{
    uint8_t error_code;     // error code in the i2C communication

    // generates the start condition
    I2C1_StartCondition ();

    // write the address
    error_code = I2C1_TX_Byte (addr);
    if (!error_code)
    {
        error_code = I2C1_TX_Byte (data);
    }

    // generates the stop condition
    I2C1_StopCondition ();
}

/******************************************************************************
 ** Name:    I2C1_Write
 *****************************************************************************/
/**
 ** @brief   Function to write data at specific address
 **
 ** @param   addr   --- slave device address
 ** @param   data   --- data buffer to write
 ** @param   nBytes --- number of data in bytes
 **
 ******************************************************************************/
void I2C1_Write(uint8_t addr, uint8_t *data, uint8_t nBytes)
{
    uint8_t error_code;     // error code in the i2C communication
    uint8_t i;              // global use counter

    // generates the start condition
    I2C1_StartCondition ();

    // write the address and continue with data
    error_code = I2C1_TX_Byte (addr);
    for (i=0; !error_code && (i<nBytes); i++)
    {
        error_code = I2C1_TX_Byte (data[i]);
    }

    // generates the stop condition
    I2C1_StopCondition ();
}

/******************************************************************************
 ** Name:    I2C1_Read_RTC
 *****************************************************************************/
/**
 ** @brief   Function to read the RTC date and time data
 **
 ** @param   data   --- Pointer to memory to store the read data
 ** @param   nBytes --- data size in bytes
 **
 ******************************************************************************/
void    I2C1_Read_RTC (uint8_t *data, uint8_t nBytes)
{
    uint8_t     error_code;     // error code in the i2C communication

    // generates the start condition
    I2C1_StartCondition ();

    // write the I2C address
    error_code = I2C1_TX_Byte (i2C_ADD_RTC);
    if (!error_code)
    {
        error_code = I2C1_TX_Byte (0x00);   // read from address 0x00
        if(!error_code)
        {
            I2C1_Read (i2C_ADD_RTC, data, nBytes);

            // the read function includes the Stop condition ...
            return;
        }
    }

    // generates the stop condition
    I2C1_StopCondition ();
}

/******************************************************************************
 ** Name:    I2C1_Read_Temperature
 *****************************************************************************/
/**
 ** @brief   Function to read the temperature from the temperature sensor
 **
 ** @param   pTemp     --- a temperature data pointer
 **
 ******************************************************************************/
uint8_t    I2C1_Read_Temperature (int8_t *pTemp)
{
    uint8_t     error_code;     // error code in the i2C communication
    uint8_t     my_buffer[4];   // buffer to write a single register

    if (!pTemp) return 0;

    //////////////////////////////
    // command a new conversion
    my_buffer[0] = 0x01;        // select the configuration register
    my_buffer[1] = 0x81;        // One shot mode and maintains shutdown mode

    I2C1_Write (i2C_ADD_TEMPERATURE, my_buffer, 2);

    // generates the start condition
    I2C1_StartCondition ();

    // write the I2C address
    error_code = I2C1_TX_Byte (i2C_ADD_TEMPERATURE);
    if (!error_code)
    {
        error_code = I2C1_TX_Byte (0x00);   // read from address 0x00
        if(!error_code)
        {
            I2C1_StartCondition ();
            // restart condition to read from read data register
            error_code = I2C1_TX_Byte (i2C_ADD_TEMPERATURE | 0x01);
            if(!error_code)
            {
                *pTemp = (int8_t) I2C1_RX_Byte (FALSE);
            }
        }
    }

    // in case of error consider the worst temperature for charging the capacitor
    if (error_code)
    {
        *pTemp = 2;
    }

    // generates the stop condition
    I2C1_StopCondition ();

    return error_code;
}

/******************************************************************************
 ** Name:    I2C1_Read_Eeprom
 *****************************************************************************/
/**
 ** @brief   Function to read EEPROM data from a specific address
 **
 ** @param   addr   --- EEPROM memory address to read from
 ** @param   data   --- Pointer to memory to store the read data
 ** @param   nBytes --- data size in bytes
 **
 ******************************************************************************/
void    I2C1_Read_Eeprom (uint16_t addr, uint8_t *data, uint8_t nBytes)
{
    uint8_t     error_code;     // error code in the i2C communication
    uint8_t     dev_add;        // device address

    // generates the start condition
    I2C1_StartCondition ();

    // assign the device address with the memory address extension bits
    dev_add = (uint8_t) (i2C_ADD_EEPROM + ((addr >> 7) & 0x06));

    // write the I2C address
    error_code = I2C1_TX_Byte (dev_add);
    if (!error_code)
    {
        error_code = I2C1_TX_Byte ((uint8_t) (addr & 0xFF));   // read from selected address
        if(!error_code)
        {
            I2C1_Read (dev_add, data, nBytes);

            // the read function includes the Stop condition ...
            return;
        }
    }

    // generates the stop condition
    I2C1_StopCondition ();
}

/******************************************************************************
 ** Name:    I2C1_Write_Eeprom
 *****************************************************************************/
/**
 ** @brief   Function to write EEPROM data at specific address
 **
 ** @param   addr   --- EEPROM memory address to write in
 ** @param   data   --- data buffer to write
 ** @param   nBytes --- number of data in bytes
 **
 ******************************************************************************/
void    I2C1_Write_Eeprom (uint16_t addr, uint8_t *data, uint8_t nBytes)
{
    uint8_t     error_code;     // error code in the i2C communication
    uint8_t     dev_add;        // device address

    // generates the start condition
    I2C1_StartCondition ();

    // assign the device address with the memory address extension bits
    dev_add = (uint8_t) (i2C_ADD_EEPROM + ((addr >> 7) & 0x06));

    // write the I2C address
    error_code = I2C1_TX_Byte (dev_add);
    if (!error_code)
    {
        error_code = I2C1_TX_Byte ((uint8_t) (addr & 0xFF));   // read from selected address
        if(!error_code)
        {
            I2C1_Write (dev_add, data, nBytes);

            // the write function includes the Stop condition ...
            return;
        }
    }

    // generates the stop condition
    I2C1_StopCondition ();
}

/******************************************************************************
 ** Name:    I2C1_WriteRegister_ACC
 *****************************************************************************/
/**
 ** @brief   Function to write data byte at specific register address
 **
 ** @param   regAddr   --- address of the device register
 ** @param   pData     --- a byte data to write
 **
 ******************************************************************************/
void I2C1_WriteRegister_ACC(uint8_t regAddress, uint8_t data)
{
    uint8_t error_code;     // error code in the i2C communication

    // generates the start condition
    I2C1_StartCondition ();

    // write the address
    error_code = I2C1_TX_Byte (i2C_ADD_ACCEL);
    if (!error_code)
    {
        error_code = I2C1_TX_Byte (regAddress);
        if (!error_code)
        {
            error_code = I2C1_TX_Byte (data);
        }
    }

    // generates the stop condition
    I2C1_StopCondition ();
}

/******************************************************************************
 ** Name:    I2C1_ReadRegister_ACC
 *****************************************************************************/
/**
 ** @brief   Function to read data byte at specific register address
 **
 ** @param   regAddr   --- address of the device register
 ** @param   pData     --- a byte data to read
 **
 ******************************************************************************/
void I2C1_ReadRegister_ACC(uint8_t regAddress, uint8_t *pData)
{
    uint8_t error_code;     // error code in the i2C communication

    // generates the start condition
    I2C1_StartCondition ();

    // write the I2C address
    error_code = I2C1_TX_Byte (i2C_ADD_ACCEL);
    if (!error_code)
    {
        error_code = I2C1_TX_Byte (regAddress);       // command to set pointer of DS2482 read registers
        if(!error_code)
        {
            I2C1_StartCondition ();
            // restart condition to read from read data register
            error_code = I2C1_TX_Byte (i2C_ADD_ACCEL | 0x01);
            if(!error_code)
            {
                *pData = I2C1_RX_Byte (FALSE);
            }
        }
    }

    // generates the stop condition
    I2C1_StopCondition ();
}

