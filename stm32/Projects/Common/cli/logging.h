/*
 * FreeRTOS STM32 Reference Integration
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef LOGGING_H
#define LOGGING_H

/* Standard Include. */
#include <stdio.h>

/* Include header for logging level macros. */
#include "logging_levels.h"

/* Dimensions the arrays into which print messages are created. */

#define dlMAX_PRINT_STRING_LENGTH    2048 - CLI_OUTPUT_EOL_LEN       /* maximum length of any single log line */
#define dlLOGGING_STREAM_LENGTH      4096
#define dlMAX_LOG_LINE_LENGTH        ( dlMAX_PRINT_STRING_LENGTH + CLI_OUTPUT_EOL_LEN )

/* Default logging config */
#if ( !defined( LOGGING_OUTPUT_UART ) && !defined( LOGGING_OUTPUT_ITM ) && !defined( LOGGING_OUTPUT_NONE ) )
#define LOGGING_OUTPUT_UART
#endif

#define LOGGING_TIMEOUT_MS    100

#ifndef LOG_LEVEL
#define LOG_LEVEL             LOG_INFO
#endif

/* Get rid of extra C89 style parentheses generated by core FreeRTOS libraries */

#define REMOVE_PARENS( ... )    STR( OVE __VA_ARGS__ )
#define OVE( ... )              OVE __VA_ARGS__
#define STR( ... )              STR_( __VA_ARGS__ )
#define STR_( ... )             REM ## __VA_ARGS__
#define REMOVE

/* Function declarations */
void vLoggingPrintf( const char * const pcLogLevel,
                     const char * const pcFunctionName,
                     const unsigned long ulLineNumber,
                     const char * const pcFormat,
                     ... );
void vLoggingInit( void );
void vLoggingDeInit( void );
void vDyingGasp( void );
void vInitLoggingEarly( void );

/* task.h cannot be included here because this file is included by FreeRTOSConfig.h */
extern void vTaskSuspendAll( void );

#ifndef __BASE_FILE__
#define __BASE_FILE__    "UNKNOWN"
#endif /* ! __BASE_FILE__ */

/* Get basename of file using gcc builtins. */
#define __NAME_ARG__    ( __builtin_strrchr( __BASE_FILE__, '/' ) ? __builtin_strrchr( __BASE_FILE__, '/' ) + 1 : __BASE_FILE__ )

/* Generic logging macros */
#define SdkLog( level, ... )    do { vLoggingPrintf( level, __NAME_ARG__, __LINE__, __VA_ARGS__ ); } while( 0 )

#define LogAssert( ... )        do { SdkLog( "ASRT", __VA_ARGS__ ); } while( 0 )

#define LogSys( ... )           do { vLoggingPrintf( "SYS", __NAME_ARG__, __LINE__, __VA_ARGS__ ); } while( 0 )

#define LogKernel( ... )        SdkLog( "KRN", __VA_ARGS__ )

#if !defined( LOG_LEVEL ) ||       \
    ( ( LOG_LEVEL != LOG_NONE ) && \
    ( LOG_LEVEL != LOG_ERROR ) &&  \
    ( LOG_LEVEL != LOG_WARN ) &&   \
    ( LOG_LEVEL != LOG_INFO ) &&   \
    ( LOG_LEVEL != LOG_DEBUG ) )

#error "Please define LOG_LEVEL as either LOG_NONE, LOG_ERROR, LOG_WARN, LOG_INFO, or LOG_DEBUG."
#else

#if ( LOG_LEVEL >= LOG_ERROR )
#define LogError( ... )    SdkLog( "ERR", REMOVE_PARENS( __VA_ARGS__ ) )
#else
#define LogError( ... )
#endif

#if ( LOG_LEVEL >= LOG_WARN )
#define LogWarn( ... )    SdkLog( "WRN", REMOVE_PARENS( __VA_ARGS__ ) )
#else
#define LogWarn( ... )
#endif

#if ( LOG_LEVEL >= LOG_INFO )
#define LogInfo( ... )    SdkLog( "INF", REMOVE_PARENS( __VA_ARGS__ ) )
#else
#define LogInfo( ... )
#endif

#if ( LOG_LEVEL >= LOG_DEBUG )
#define LogDebug( ... )    SdkLog( "DBG", REMOVE_PARENS( __VA_ARGS__ ) )
#else
#define LogDebug( ... )
#endif
#endif /* if !defined( LOG_LEVEL ) || ( ( LOG_LEVEL != LOG_NONE ) && ( LOG_LEVEL != LOG_ERROR ) && ( LOG_LEVEL != LOG_WARN ) && ( LOG_LEVEL != LOG_INFO ) && ( LOG_LEVEL != LOG_DEBUG ) ) */

#endif /* LOGGING_H */
