/******************************************************************************
 * Name      : R100_S3A7                                                      *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : GNU ARM Embedded                                               *
 * Target    : Reanibex Serie 100                                             *
 ******************************************************************************/

/*!
 * @file        init_code.c
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

/* This include file is need only for the 'temporary' fix to insure that the Ioport reference counter is zeroed before it
 * gets referenced. Ioport init is currently called before the C Runtime initialization takes place.
 * It will be removed when a more complete solution for this problem is added.
 */




//#include "types_basic.h"
#include "init_code_Mock.h"
#include "Resources/Keypad_Mock.h"

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

/***********************************************************************************************************************
 * Function Name: R_BSP_WarmStart
 * Description  : This function is called at various points during the startup process. This function is declared as a
 *                weak symbol higher up in this file because it is meant to be overridden by a user implemented version.
 *                One of the main uses for this function is to call functional safety code during the startup process.
 *                To use this function just copy this function into your own code and modify it to meet your needs.
 * Arguments    : event -
 *                    Where at in the start up process the code is currently at
 * Return Value : none
 ***********************************************************************************************************************/
void R_BSP_WarmStart_mock()
{
    printf("R_BSP_WarmStart mock\n");
}
