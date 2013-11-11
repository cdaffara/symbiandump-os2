// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LOGSERVERCLI.H
// 
//

#ifndef __LOGSERVERCLI_H__
#define __LOGSERVERCLI_H__

// System includes
#include <e32std.h>

// User includes
#include <logcli.h>
#include "LogCliServShared.h"

class RLogSession : public RSessionBase
/**
@internalComponent
*/
	{
public:
	/**
	 * Constructor
	 */
	RLogSession();

	/**
	 * Connect to Log Server
	 */
	TInt Connect();

public:

	/**
	 * Asynchronous send to server. Reply received via aStatus
	 */
	void Send(TInt aType, const TIpcArgs& aArgs, TRequestStatus& aStatus) const;

	/**
	 * Synchronous send to server. Reply returned to caller
	 */
	TInt Send(TInt aType, const TIpcArgs& aArgs) const;

	/**
	 * Get the next free operation id
	 */
	inline TLogOperationId AllocateIdOperation();

	/**
	 * Get the next free view id
	 */
	inline TLogViewId AllocateIdView();

private:
	TInt SendWithRetry(TInt aType, const TIpcArgs& aParam) const;
	void SendWithRetryAsync(TInt aType, const TIpcArgs& aParam, TRequestStatus& aStatus) const;

private:
	TLogOperationId iOperationId;
	TLogViewId iViewId;
	};


inline TLogOperationId RLogSession::AllocateIdOperation()
	{
	return iOperationId++;
	}

inline TLogViewId RLogSession::AllocateIdView()
	{
	return iViewId++;
	}


#ifdef LOGGING_ENABLED

#include <f32file.h>
#include <flogger.h>

/**
@internalComponent
*/
_LIT(KLogFileName, "LogEng.txt");
_LIT(KLogFolder,   "LogEng");
_LIT(KTimeFormat, "%02d.%02d:%02d:%06d    ");
_LIT(KTextFormat, "%S");

class Log : public RFileLogger
/**
@internalComponent
*/
	{
public:
	static void New();
	static void Write(const TDesC& aText);
	static void WriteFormat(TRefByValue<const TDesC> aFmt, ...);

private:
	static void PruneLogFile();
    };

/**
@internalComponent
*/
#define LOGNEW							Log::New()
#define LOGTEXT(AAA)					{ _LIT(KString, AAA); Log::Write(KString); }
#define LOGTEXT2(AAA, BBB)				{ _LIT(KString, AAA); Log::WriteFormat(TRefByValue<const TDesC>(KString()), BBB); }
#define LOGTEXT3(AAA, BBB, CCC)			{ _LIT(KString, AAA); Log::WriteFormat(TRefByValue<const TDesC>(KString()), BBB, CCC); }
#define LOGTEXT4(AAA, BBB, CCC, DDD)	{ _LIT(KString, AAA); Log::WriteFormat(TRefByValue<const TDesC>(KString()), BBB, CCC, DDD); }

#else

#define LOGNEW
#define LOGTEXT(AAA)
#define LOGTEXT2(AAA, BBB)
#define LOGTEXT3(AAA, BBB, CCC)
#define LOGTEXT4(AAA, BBB, CCC, DDD)

#endif


#endif

