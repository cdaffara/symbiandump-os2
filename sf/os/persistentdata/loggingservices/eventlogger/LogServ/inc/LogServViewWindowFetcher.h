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
//

#ifndef __LOGSERVVIEWWINDOWFETCHER_H__
#define __LOGSERVVIEWWINDOWFETCHER_H__

#include "LogCliServShared.h"

// Classes referenced
class CLogGetEvent;
class CLogServViewBase;
class MLogServDatabaseTransactionInterface;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServViewWindowFetcher (header)
/////////////////////////////////////////////////////////////////////////////////////////
/**
@internalComponent
*/
class CLogServViewWindowFetcher : public CLogActive
	{
public:
	static CLogServViewWindowFetcher* NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	~CLogServViewWindowFetcher();

private:
	CLogServViewWindowFetcher(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	void ConstructL();

public:
	void StartL(TRequestStatus& aStatus, const CLogServViewBase& aView, const TLogTransferWindow& aWindow, const RMessage2& aMessage);

private:
	void DoRunL();
	void DoCancel();
	void DoComplete(TInt& aCompletionCode);

private:
	enum TState
		{
		EStateIdle = 0,
		EStateStarting,
		EStateContinuing
		};

private:
	void ProcessEventL();

private:
	void GetNextEventL(TInt aWindowIndex);
	void CompleteRequest();

private:
	MLogServDatabaseTransactionInterface& iDatabase;
	CLogEvent* iEvent;
	CLogGetEvent* iGetEvent;

	// Transient
	TState iState;
	const CLogServViewBase* iView;
	TLogTransferWindow iWindow;
	const RMessage2* iMessage;
	TInt iWindowIndex;
	CBufBase* iBuffer;
	};

#endif
