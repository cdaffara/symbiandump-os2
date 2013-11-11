/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Logging for Bluetooth engine.
*
*/


#ifndef __HTIBTENGINELOGGING_H__
#define __HTIBTENGINELOGGING_H__

// INCLUDES
#include <flogger.h>

/******************************************************/
/**     LOG SETUP                                    **/
/******************************************************/
_LIT(KLogFolder, "Hti");
_LIT(KLogFile, "BtSerialEngine.log");

#ifdef __ENABLE_LOGGING__
// Comment out the levels which are marked supported in the build
// (and which are used runtime).
//
#define LOG_LEVEL_ERROR_SUPPORTED
#define LOG_LEVEL_WARN_SUPPORTED
#define LOG_LEVEL_INFO_SUPPORTED
//#define LOG_LEVEL_DEBUG_SUPPORTED
#endif

/******************************************************/
/**     MACRO DEFINITIONS                            **/
/******************************************************/

// Macros:                            Example:                                 Result:
// LOGFMT_E(formatstring, parameter)  LOGFMT_E("Leave in RunL: %d", -6)        <TIME> ERROR: Leave in RunL: -6
// LOG_E(string)                      LOG_E("There was an error")              <TIME> ERROR: There was an error
// LOGFMT_W(formatstring, parameter)  LOGFMT_W("Buffer too small: %d", 12)     <TIME> WARNING: Buffer too small: 12
// LOG_W(string)                      LOG_W("warmingtext")                     <TIME> WARNING: warningtext
// LOGFMT_I(formatstring, parameter)  LOGFMT_I("plugin loaded: %S", &myDescr8) <TIME> INFO: plugin loaded: myplugin
// LOG_I(string)                      LOG_I("abc")                             <TIME> INFO: abc
// LOGFMT_D(formatstring, parameter)  LOGFMT_D("buffer length: %d", 56)        <TIME> DEBUG: buffer length: 56
// LOG_D(string)                      LOG_D("abc")                             <TIME> DEBUG: abc

#define ERRORSTR "ERROR: "
#define WARNSTR  "WARNING: "
#define INFOSTR  "INFO: "
#define DEBUGSTR "DEBUG: "

#ifdef LOG_LEVEL_ERROR_SUPPORTED
    #define LOGFMT_E(a1, a2) {_LIT8(temp, ERRORSTR##a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, temp, a2);}
    #define LOG_E(a1) {_LIT8(temp, ERRORSTR##a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#else
    #define LOGFMT_E(a1, a2) {}
    #define LOG_E(a1) {}
#endif

#ifdef LOG_LEVEL_WARN_SUPPORTED
    #define LOGFMT_W(a1, a2) {_LIT8(temp, WARNSTR##a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, temp, a2);}
    #define LOG_W(a1) {_LIT8(temp, WARNSTR##a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#else
    #define LOGFMT_W(a1, a2) {}
    #define LOG_W(a1) {}
#endif

#ifdef LOG_LEVEL_INFO_SUPPORTED
    #define LOGFMT_I(a1, a2) {_LIT8(temp, INFOSTR##a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, temp, a2);}
    #define LOG_I(a1) {_LIT8(temp, INFOSTR##a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#else
    #define LOGFMT_I(a1, a2) {}
    #define LOG_I(a1) {}
#endif

#ifdef LOG_LEVEL_DEBUG_SUPPORTED
    #define LOGFMT_D(a1, a2) {_LIT8(temp, DEBUGSTR##a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, temp, a2);}
    #define LOG_D(a1) {_LIT8(temp, DEBUGSTR##a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#else
    #define LOGFMT_D(a1, a2) {}
    #define LOG_D(a1) {}
#endif

#endif // __HTIBTENGINELOGGING_H__
