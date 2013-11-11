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
* Description:  Logger library
*
*/


#ifndef __LOGGER_H_____
#define __LOGGER_H_____

//  INCLUDES
#include <e32std.h>

// CONSTANTS
const TInt KLogLevelDebug   = 0;
const TInt KLogLevelInfo    = 10;
const TInt KLogLevelWarning = 20;
const TInt KLogLevelError   = 30;

const TInt KMinimumLogLevel = KLogLevelInfo;


// When __ENABLE_LOGGING__ macro is defined, logging functions are
// compiled to final build.
//
// Example usage: LOGFW(DebugLog(_L("Got an error %d"), 5))
//
#ifdef __ENABLE_LOGGING__

    #define LOGFW(a) a;

    _LIT(KLogFileName, "c:\\HTI_BtComm.log");

    // panics
    _LIT(KLogPanicCategory, "CustNotifyLog");
    static const TInt KPanicFsConnectFailed = 1;
    static const TInt KPanicFileCreateFailed = 2;
    static const TInt KPanicFileWriteFailed = 3;

    const TInt KLogEntryMaxLength = 156;

    // Usage example: DOLOG(_L("Logstring"));
    void DebugLog(TRefByValue<const TDesC> aFmt,...);
    void InfoLog(TRefByValue<const TDesC> aFmt,...);
    void WarnLog(TRefByValue<const TDesC> aFmt,...);
    void ErrLog(TRefByValue<const TDesC> aFmt,...);

    void Log(TInt aLevel, TRefByValue<const TDesC> aFmt, VA_LIST list);

    void LogBytes(const TDesC8& aData, TInt aMaxBytesToLog );

#else  //__ENABLE_LOGGING__

    #define LOGFW(a)

#endif //__ENABLE_LOGGING__

#endif //__LOGGER_H_____

