// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// InuLogger.h
// 
//

#ifndef __INULOGGER_H__
#define __INULOGGER_H__

#include <e32std.h>
#include <Flogger.h>


// HTTP Logging class
//##ModelId=3A914DF60214
class TInuLogger
	{
public: // methods

	// Destructor - closes the log
	//##ModelId=3A914DF60249
	IMPORT_C ~TInuLogger();

	// Create log file in directory KLogsdir\KWapLogsDirName - Note: ingore Drive and Path of aFilename
	//##ModelId=3A914DF6023F
	IMPORT_C void CreateFlogger(const TDesC& aFileName, TInt aShowDate, TInt aShowTime);

	//##ModelId=3A914DF6023C
	IMPORT_C void LogIt(TRefByValue<const TDesC> aFmt, ...);

	// Writes aComment to test log file, logging file and test harness
	//##ModelId=3A914DF60232
	IMPORT_C void WriteComment(const TDesC& aComment);

	// Do a formatted dump of binary data
	//##ModelId=3A914DF6022B
	IMPORT_C void DumpIt(const TDesC8& aData);

	// Do a write of the supplied 8 bit data
	//##ModelId=3A914DF60234
	IMPORT_C void WriteComment(const TDesC8& aData);

private: // attributes

	// Logger handle
	//##ModelId=3A914DF60228
	RFileLogger iLogger;
	};

// By default, logging will always be switched on.  Later, this may be removed, leaving it to individual
// source files to enable logging where needed.
#define _LOGGING

#if defined (_DEBUG) && defined (_LOGGING)

// HTTP Logging macros
#define __DECLARE_LOG		TInuLogger iLogger;
#define __OPEN_LOG(L)		iLogger.CreateFlogger(_L(L), ETrue, ETrue);
#define __OPEN_LOG2(L)		iLogger.CreateFlogger(L, ETrue, ETrue);
#define __LOG(C)			iLogger.WriteComment(C);
#define __LOG1(C, X)		iLogger.LogIt(C, X);
#define __LOG2(C, X, Y)		iLogger.LogIt(C, X, Y);
#define __LOG3(C, X, Y, Z)	iLogger.LogIt(C, X, Y, Z);
#define __DUMPBIN(D)		iLogger.DumpIt(D);
#define __QINFO(aText)		{User::InfoPrint(aText); User::After(1000000);} 

#else if !defined (_DEBUG)

// NULL macros
#define __DECLARE_LOG		
#define __OPEN_LOG(L)		
#define __OPEN_LOG2(L)		
#define __LOG(C)			
#define __LOG1(C, X)		
#define __LOG2(C, X, Y)		
#define __LOG3(C, X, Y, Z)	
#define __DUMPBIN(D)		
#define __QINFO(aText) 
#endif // !_DEBUG

#endif // __InuLOGGER_H__
