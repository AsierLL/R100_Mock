

/**************************************************************************/ 
/*                                                                        */ 
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */ 
/*                                                                        */ 
/*    tx_port.h                                          Synergy/GNU      */ 
/*                                                           5.2          */ 
/*                                                                        */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file contains data type definitions that make the ThreadX      */ 
/*    real-time kernel function identically on a variety of different     */ 
/*    processor architectures.  For example, the size or number of bits   */ 
/*    in an "int" data type vary between microprocessor architectures and */ 
/*    even C compilers for the same microprocessor.  ThreadX does not     */ 
/*    directly use native C data types.  Instead, ThreadX creates its     */ 
/*    own special types that can be mapped to actual data types by this   */ 
/*    file to guarantee consistency in the interface and functionality.   */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  09-25-2015     William E. Lamie         Initial Renesas Synergy/GNU   */ 
/*                                            Support Version 5.0         */ 
/*  03-07-2016     William E. Lamie         Modified comment(s), and      */ 
/*                                            changed CHAR to "char",     */ 
/*                                            resulting in version 5.1    */ 
/*  10-27-2017     William E. Lamie         Modified comment(s), and      */
/*                                            disabled preemption in      */
/*                                            post initialization,        */
/*                                            resulting in version 5.2    */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef TX_PORT_H
#define TX_PORT_H

/* Define compiler library include files.  */

#include <stdlib.h>
#include <string.h>

/* Define basic types for this port.  */

#define VOID                                    void
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef signed int                              INT;
typedef unsigned int                            UINT;
typedef signed long                             LONG;
typedef unsigned long                           ULONG;
typedef signed short                            SHORT;
typedef unsigned short                          USHORT;


/* Define the priority levels for ThreadX.  Legal values range
   from 32 to 1024 and MUST be evenly divisible by 32.  */

#ifndef TX_MAX_PRIORITIES
#define TX_MAX_PRIORITIES                       32
#endif


/* Define the minimum stack for a ThreadX thread on this processor. If the size supplied during
   thread creation is less than this value, the thread create call will return an error.  */

#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK                        200         /* Minimum stack size for this port  */
#endif


/* Define the system timer thread's default stack size and priority.  These are only applicable
   if TX_TIMER_PROCESS_IN_ISR is not defined.  */

#ifndef TX_TIMER_THREAD_STACK_SIZE
#define TX_TIMER_THREAD_STACK_SIZE              1024        /* Default timer thread stack size  */
#endif

#ifndef TX_TIMER_THREAD_PRIORITY    
#define TX_TIMER_THREAD_PRIORITY                0           /* Default timer thread priority    */ 
#endif


/* Define various constants for the ThreadX Synergy GNU port.  */ 

#define TX_INT_DISABLE                          1           /* Disable interrupts               */
#define TX_INT_ENABLE                           0           /* Enable interrupts                */


/* Define the clock source for trace event entry time stamp.  */

#ifndef TX_MISRA_ENABLE
#ifndef TX_TRACE_TIME_SOURCE
#define TX_TRACE_TIME_SOURCE                    *((ULONG *) 0xE0001004)  
#endif
#else
ULONG   _tx_misra_time_stamp_get(VOID);
#define TX_TRACE_TIME_SOURCE                    _tx_misra_time_stamp_get()
#endif

#ifndef TX_TRACE_TIME_MASK
#define TX_TRACE_TIME_MASK                      0xFFFFFFFFUL
#endif


/* Define the port specific options for the _tx_build_options variable. This variable indicates
   how the ThreadX library was built.  */

#define TX_PORT_SPECIFIC_BUILD_OPTIONS          0


/* Define the in-line initialization constant so that modules with in-line
   initialization capabilities can prevent their initialization from being
   a function call.  */

#ifdef TX_MISRA_ENABLE
#define TX_DISABLE_INLINE
#else
#define TX_INLINE_INITIALIZATION
#endif

/* Define prototypes specific to the ThreadX Synergy implementation.  */

VOID  tx_isr_start(ULONG isr_id);
VOID  tx_isr_end(ULONG isr_id);

/* Define whether FPU is being used or not. */
#if ( defined ( __GNUC__ ) && defined (__VFP_FP__) && !defined (__SOFTFP__) ) || \
    ( defined ( __ICCARM__ ) && defined ( __ARMVFP__ ) && (__FPU_PRESENT == 1) )

/* Enable FPU register saving when necessary. */
    
#define TX_VFP_ENABLE
#else
/* FPU not being used. */
#endif

/* Define the location of the vector table */

extern void * __Vectors[];
#define TX_VECTOR_TABLE __Vectors


/* Define the location of the begining of the free RAM  */

extern void * __RAM_segment_used_end__;
#define TX_FREE_MEMORY_START (&__RAM_segment_used_end__)

/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifdef TX_THREAD_INIT
CHAR                            _tx_version_id[] = 
                                    "Copyright (c) 1996-2018 Express Logic Inc. * ThreadX Synergy/GNU Version G5.8sp4 SN: 4154-280-5000 *";
#else
extern  CHAR                    _tx_version_id[100];
#endif

/* Define the TX_THREAD control block extensions for this port. The main reason
   for the multiple macros is so that backward compatibility can be maintained with
   existing ThreadX kernel awareness modules.  */

#ifndef TX_THREAD_EXTENSION_0
#define TX_THREAD_EXTENSION_0
#endif
#ifndef TX_THREAD_EXTENSION_1
#define TX_THREAD_EXTENSION_1
#endif
#ifndef TX_THREAD_EXTENSION_2
#define TX_THREAD_EXTENSION_2                   ULONG       tx_thread_vfp_stack_frame;
#endif
#ifdef TX_ENABLE_EPK_SUPPORT
#include "tx_execution_profile.h"
#endif
#ifndef TX_THREAD_EXTENSION_3
#define TX_THREAD_EXTENSION_3                   int   bsd_errno;
#endif


/* Define the port extensions of the remaining ThreadX objects.  */

#ifndef TX_BLOCK_POOL_EXTENSION
#define TX_BLOCK_POOL_EXTENSION
#endif
#ifndef TX_BYTE_POOL_EXTENSION
#define TX_BYTE_POOL_EXTENSION
#endif
#ifndef TX_EVENT_FLAGS_GROUP_EXTENSION
#define TX_EVENT_FLAGS_GROUP_EXTENSION
#endif
#ifndef TX_MUTEX_EXTENSION
#define TX_MUTEX_EXTENSION
#endif
#ifndef TX_QUEUE_EXTENSION
#define TX_QUEUE_EXTENSION
#endif
#ifndef TX_SEMAPHORE_EXTENSION
#define TX_SEMAPHORE_EXTENSION
#endif
#ifndef TX_TIMER_EXTENSION
#define TX_TIMER_EXTENSION
#endif

/* Define the user extension field of the thread control block.  Nothing
   additional is needed for this port so it is defined as white space.  */

#ifndef TX_THREAD_USER_EXTENSION
#define TX_THREAD_USER_EXTENSION
#endif

#endif





