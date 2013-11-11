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
* Description:  Logging macros
*
*/


#ifndef HTIIPCOMMLOGGING_H
#define HTIIPCOMMLOGGING_H

#define LOG_HTI_FLOW

#ifdef __ENABLE_LOGGING__

#include <flogger.h>

_LIT(KLogFolder, "hti");
_LIT(KLogFile, "HtiIPComm_log.txt");
_LIT(KFreeMemFormat, "Total free: %d bytes");
_LIT(KFreeHeapMemFormat, "Heap available: %d bytes, biggest %d bytes");
_LIT(KAllocHeapMemFormat, "Heap alloc: %d bytes, %d cells");

#define HTI_LOG_TEXT(a1) {_LIT(temp, a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#define HTI_LOG_DES(a1) {RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, (a1));}
#define HTI_LOG_FORMAT(a1,a2) {_LIT(temp, a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, temp, (a2));}
#define HTI_LOG_HEX(p,l) {RFileLogger::HexDump(KLogFolder, KLogFile, EFileLoggingModeAppend, NULL, NULL, p, l);}

#define HTI_LOG_FREE_MEM() {TMemoryInfoV1Buf info;UserHal::MemoryInfo(info);RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, KFreeMemFormat, info().iFreeRamInBytes);}
#define HTI_LOG_FREE_HEAP_MEM() {TInt b;TInt free=User::Available(b);RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, KFreeHeapMemFormat, free, b);}
#define HTI_LOG_ALLOC_HEAP_MEM() {TInt b;TInt c = User::AllocSize(b);RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, KAllocHeapMemFormat, b, c);}

//define separately macros to track function flow
#ifdef LOG_HTI_FLOW
_LIT8(KFuncIn, "-=> %S");
_LIT8(KFuncOut, "<=- %S");

#define HTI_LOG_FUNC_IN(a1) {_LIT8(temp, a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, KFuncIn, &temp);}
#define HTI_LOG_FUNC_OUT(a1) {_LIT8(temp, a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, KFuncOut, &temp);}

#else // LOG_HTI_FLOW

#define HTI_LOG_FUNC_IN(a1)
#define HTI_LOG_FUNC_OUT(a1)

#endif // LOG_HTI_FLOW


#else // __ENABLE_LOGGING__

#define HTI_LOG_TEXT(a1)
#define HTI_LOG_DES(a1)
#define HTI_LOG_FORMAT(a1,a2)
#define HTI_LOG_HEX(p,l)

#define HTI_LOG_FREE_MEM()
#define HTI_LOG_FREE_HEAP_MEM()
#define HTI_LOG_ALLOC_HEAP_MEM()

#define HTI_LOG_FUNC_IN(a1)
#define HTI_LOG_FUNC_OUT(a1)

#endif // __ENABLE_LOGGING__

#endif  //HTIIPCOMMLOGGING_H
