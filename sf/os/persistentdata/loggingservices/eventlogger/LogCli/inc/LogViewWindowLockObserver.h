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

#ifndef __LOGVIEWWINDOWLOCKOBSERVER_H__
#define __LOGVIEWWINDOWLOCKOBSERVER_H__

// System includes
#include <e32base.h>

// User includes
#include <logwrap.h>
#include "LogCliServShared.h"
#include <logviewchangeobserver.h>

// Classes referenced
class RLogSession;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> MLogViewWindowLockObserver (header)
/////////////////////////////////////////////////////////////////////////////////////////
class MLogViewWindowLockObserver
/**
@internalComponent
*/
	{
public: // FROM MLogViewWindowLockObserver
	virtual void HandleWindowLockStatusChangeL(TLogViewLockStatus aStatus) = 0;
	};


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewWindowLockObserver (header)
/////////////////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CLogViewWindowLockObserver) : public CLogActive
/**
@internalComponent
*/
	{
public:
	CLogViewWindowLockObserver(RLogSession& aSession, TLogViewId aViewId, MLogViewWindowLockObserver& aObserver, TInt aPriority);
	~CLogViewWindowLockObserver();
	void ConstructL();

private: // FROM CLogActive
	void DoRunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	void IssueRequest();

private:
	RLogSession& iSession;
	TLogViewId iViewId; 
	MLogViewWindowLockObserver& iObserver;
	};



#endif
