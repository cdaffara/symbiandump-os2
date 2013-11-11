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
* Description:  
*
*/

#ifndef SUT_LOG_DEF__
#define SUT_LOG_DEF__

#include <SUTFlags.cfg>

#ifdef __ENABLE_LOGGING__

#include <flogger.h>
_LIT(KLogFolder, "sut");
_LIT(KLogFile, "sut_log.txt");

#define SUT_LOG_START(a1) {RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeOverwrite, a1);}
#define SUT_LOG_INFO(a1) {RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, a1);}
#define SUT_LOG_FORMAT(a1, a2) {RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, a1, (a2));}
#define SUT_LOG_FORMAT2(a1, a2, a3) {RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, a1, (a2), (a3));}
#define SUT_LOG_FORMAT3(a1, a2, a3, a4) {RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, a1, (a2), (a3), (a4));}

 
#ifdef __ENABLE_LOGGING_DEBUG__
#define SUT_LOG_DEBUG(a1) {_LIT(temp, a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#define SUT_LOG_DEBUGF(a1, a2) {RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, a1, (a2));}
#else
#define SUT_LOG_DEBUG(a1)
#define SUT_LOG_DEBUGF(a1, a2)
#endif // __ENABLE_LOGGING_DEBUG__

#else
// NO LOGGING

#define SUT_LOG_START(a1) 
#define SUT_LOG_INFO(a1)
#define SUT_LOG_FORMAT(a1, a2) 
#define SUT_LOG_FORMAT2(a1, a2, a3) 
#define SUT_LOG_FORMAT3(a1, a2, a3, a4)
#define SUT_LOG_DEBUG(a1)
#define SUT_LOG_DEBUGF(a1, a2)

#endif  // __ENABLE_LOGGING__

#endif  // SUT_LOG_DEF__
