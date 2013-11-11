/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#ifndef __T_LOGGER_H__
#define __T_LOGGER_H__

// Define logging functionality

#include <flogger.h>

_LIT(KLogDir, "security");
_LIT(KLogFile, "testhandler.txt");
_LIT(KNewline, "\n");

// Log to c:\security\testhandler.txt
#define LOG(X) RFileLogger::Write(KLogDir, KLogFile, EFileLoggingModeAppend, (X))
#define LOG1(X, Y) RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, (X), (Y))
#define LOG2(X, Y, Z) RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, (X), (Y), (Z))

// Print to console and log
#define PRINTANDLOG(X) LOG(X); iConsole->Printf(X); iConsole->Printf(KNewline);
#define PRINTANDLOG1(X, Y) LOG1(X, Y); iConsole->Printf((X), (Y)); iConsole->Printf(KNewline);
#define PRINTANDLOG2(X, Y, Z) LOG2(X, Y, Z); iConsole->Printf((X), (Y), (Z)); iConsole->Printf(KNewline);

#endif
