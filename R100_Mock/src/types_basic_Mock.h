#include <stdbool.h>
#include <stdint.h>


#ifndef TYPES_BASIC_H
#define TYPES_BASIC_H
/******************************************************************************
**Includes
*/

/******************************************************************************
** Defines

/******************************************************************************

*/

#ifndef float_t
#define float_t float
#endif

#ifndef bool_t
#define bool_t bool
#endif

#ifndef int32_t
#define int32_t int
#endif

#ifndef CHAR
#define char_t char
#endif


#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

#define UNUSED(x)                   (void) (x)

// delay implemented by soft in a loop (in fast mode, running at nominal speed)
#define SOFT_FAST_DELAY_1MSEC   15000                           ///< Fast delay 1 msec
#define SOFT_FAST_DELAY_2MSEC   (SOFT_FAST_DELAY_1MSEC * 2)     ///< Fast delay 2 msecs
#define SOFT_FAST_DELAY_10MSEC  (SOFT_FAST_DELAY_1MSEC * 10)    ///< Fast delay 10 msecs
#define SOFT_FAST_DELAY_20MSEC  (SOFT_FAST_DELAY_1MSEC * 20)    ///< Fast delay 20 msecs
#define SOFT_FAST_DELAY_50MSEC  (SOFT_FAST_DELAY_1MSEC * 50)    ///< Fast delay 50 msecs
#define SOFT_FAST_DELAY_100MSEC (SOFT_FAST_DELAY_1MSEC * 100)   ///< Fast delay 100 msecs
#define SOFT_FAST_DELAY_200MSEC (SOFT_FAST_DELAY_1MSEC * 200)   ///< Fast delay 200 msecs
#define SOFT_FAST_DELAY_500MSEC (SOFT_FAST_DELAY_1MSEC * 500)   ///< Fast delay 500 msecs

// delay implemented by soft in a loop (in slow mode, running at low speed)
#define SOFT_SLOW_DELAY_1MSEC   (SOFT_FAST_DELAY_1MSEC / 8)     ///< Slow delay 1 msec
#define SOFT_SLOW_DELAY_2MSEC   (SOFT_SLOW_DELAY_1MSEC * 2)     ///< Slow delay 2 msecs
#define SOFT_SLOW_DELAY_10MSEC  (SOFT_SLOW_DELAY_1MSEC * 10)    ///< Slow delay 10 msecs
#define SOFT_SLOW_DELAY_20MSEC  (SOFT_SLOW_DELAY_1MSEC * 20)    ///< Slow delay 20 msecs
#define SOFT_SLOW_DELAY_50MSEC  (SOFT_SLOW_DELAY_1MSEC * 50)    ///< Slow delay 50 msecs
#define SOFT_SLOW_DELAY_100MSEC (SOFT_SLOW_DELAY_1MSEC * 100)   ///< Slow delay 100 msecs
#define SOFT_SLOW_DELAY_150MSEC (SOFT_SLOW_DELAY_1MSEC * 150)   ///< Slow delay 150 msecs
#define SOFT_SLOW_DELAY_200MSEC (SOFT_SLOW_DELAY_1MSEC * 200)   ///< Slow delay 200 msecs
#define SOFT_SLOW_DELAY_500MSEC (SOFT_SLOW_DELAY_1MSEC * 500)   ///< Slow delay 500 msecs

#define MSECS_PER_TICK          10           ///< number of miliseconds in a single tick

#define TICKS_PER_SECOND        (1000 / MSECS_PER_TICK)         ///< Ticks per second
#define TICKS_PER_MINUTE        ((  60 * 1000) / MSECS_PER_TICK)///< Ticks per minute
#define TICKS_PER_HOUR          ((3600 * 1000) / MSECS_PER_TICK)///< Ticks per hour

#define OSTIME_NO_DELAY         (0)                         ///< No delay
#define OSTIME_10MSEC           (TICKS_PER_SECOND / 100)    ///< 10 msecs
#define OSTIME_20MSEC           (OSTIME_10MSEC * 2)         ///< 20 msecs
#define OSTIME_30MSEC           (OSTIME_10MSEC * 3)         ///< 30 msecs
#define OSTIME_40MSEC           (OSTIME_10MSEC * 4)         ///< 40 msecs
#define OSTIME_50MSEC           (OSTIME_10MSEC * 5)         ///< 50 msecs
#define OSTIME_60MSEC           (OSTIME_10MSEC * 5)         ///< 60 msecs
#define OSTIME_70MSEC           (OSTIME_10MSEC * 5)         ///< 70 secs
#define OSTIME_80MSEC           (OSTIME_10MSEC * 5)         ///< 80 msecs
#define OSTIME_90MSEC           (OSTIME_10MSEC * 5)         ///< 90 msecs
#define OSTIME_100MSEC          (OSTIME_10MSEC * 10)        ///< 100 msecs
#define OSTIME_150MSEC          (OSTIME_10MSEC * 15)        ///< 150 secs
#define OSTIME_200MSEC          (OSTIME_10MSEC * 20)        ///< 200 msecs
#define OSTIME_300MSEC          (OSTIME_10MSEC * 30)        ///< 300 msecs
#define OSTIME_400MSEC          (OSTIME_10MSEC * 40)        ///< 400 msecs
#define OSTIME_500MSEC          (OSTIME_10MSEC * 50)        ///< 500 msecs
#define OSTIME_600MSEC          (OSTIME_10MSEC * 60)        ///< 600 msecs
#define OSTIME_1SEC             (OSTIME_10MSEC * 100)       ///< 1 sec
#define OSTIME_2SEC             (OSTIME_10MSEC * 200)       ///< 2 secs
#define OSTIME_3SEC             (OSTIME_10MSEC * 300)       ///< 3 secs
#define OSTIME_4SEC             (OSTIME_10MSEC * 400)       ///< 4 secs
#define OSTIME_5SEC             (OSTIME_10MSEC * 500)       ///< 5 secs
#define OSTIME_6SEC             (OSTIME_10MSEC * 600)       ///< 6 secs
#define OSTIME_7SEC             (OSTIME_10MSEC * 700)       ///< 7 secs
#define OSTIME_8SEC             (OSTIME_10MSEC * 800)       ///< 8 secs
#define OSTIME_9SEC             (OSTIME_10MSEC * 900)       ///< 9 secs
#define OSTIME_10SEC            (OSTIME_10MSEC * 1000)      ///< 10 secs
#define OSTIME_12SEC            (OSTIME_10MSEC * 1200)      ///< 12 secs
#define OSTIME_14SEC            (OSTIME_10MSEC * 1400)      ///< 14 secs
#define OSTIME_15SEC            (OSTIME_10MSEC * 1500)      ///< 15 secs
#define OSTIME_20SEC            (OSTIME_10MSEC * 2000)      ///< 20 secs
#define OSTIME_30SEC            (OSTIME_10MSEC * 3000)      ///< 30  secs
#define OSTIME_60SEC            (OSTIME_10MSEC * 6000)      ///< 60 secs


#define COMPILE_ASSERT(x) extern int __dummy[(int)x]

/******************************************************************************
** prototypes
*/

#endif  /*TYPES_BASIC_H*/

/*
** $Log$
**
** end of Types_Basic.h
******************************************************************************/
