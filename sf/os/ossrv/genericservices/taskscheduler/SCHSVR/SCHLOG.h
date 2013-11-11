// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __SCHLOG_H__
#define __SCHLOG_H__

// Classes referenced
class CLogEvent;
class CLogWrapper;

/*
This class manages the logging of error messages to the log engine.
It allows several messages to be able to be logged at the same time
by performing some simple queuing.
*/
NONSHARABLE_CLASS(CSchLogManager) : public CActive
	{
public:
	~CSchLogManager();
	static CSchLogManager* NewL(RFs& aFileSession);
	
private:
	CLogWrapper* GetLogWrapperL();
	CSchLogManager();
	void ConstructL(RFs& aFileSession);
	// From CActive
	void RunL();
	void DoCancel();

public:
	void LogError(const TDesC& aSubject, TInt aError);
	void LogError(TInt aError);

private: 
	RFs *iFileSession;
	CLogEvent* iLogEvent;
	CLogWrapper* iLogWrapper;
  	CArrayPtrFlat<CLogEvent>* iPendingLogEvents;
	};

#endif

