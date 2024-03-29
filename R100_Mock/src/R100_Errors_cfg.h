/*-----------------------------------------------------------------------------*/
/* errors configuration (destinations and actions) header file                  */
/*                                                                             */
/* this file is automatically generated form excel sheet                       */
/*                                                                             */
/* author: egarcia                                                             */
/*-----------------------------------------------------------------------------*/
#ifndef R100_ERRORS_CFG_H
#define R100_ERRORS_CFG_H

#define MAX_ERRORS               256               /// number of errors
/******************************************************************************
** includes
*/

#include "R100_Errors.h"

#include "types_basic_Mock.h"

/******************************************************************************
** typedefs
*/

/// errors configuration structure
typedef struct {
    uint8_t   warning_error;     /// warning (0) or error (1)
    uint8_t   user_indication;   /// user indication No (0) or Yes (1)
    uint8_t   daily_monthly;     /// recoverable in daily (0) or monthly(1)
    uint8_t   led_flashing;      /// led flashing No (0) or Yes (1)
    char_t    text[50];          /// text identifier associated
}ERRORS_CFG_t;

/******************************************************************************
** globals
*/

/// errors configuration table
extern const ERRORS_CFG_t R100_Errors_cfg[MAX_ERRORS];

#endif /* R100_ERRORS_CFG_H*/
/*-----------------------------------------------------------------------------*/
