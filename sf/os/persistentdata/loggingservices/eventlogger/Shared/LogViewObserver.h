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

#ifndef __LOGVIEWOBSERVER_H__
#define __LOGVIEWOBSERVER_H__

// System includes
#include <e32base.h>

// User includes
#include <logwrap.h>
#include <logview.h>

// Classes referenced
class CLogClient;
class CLogChangeDefinition;
class MLogViewChangeObserverInternal;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewObserver (header)
/////////////////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CLogViewObserver) : public CActive
/**
@internalComponent
*/
	{
public:
	static CLogViewObserver* NewL(CLogView& aView, CLogClient& aClient, MLogViewChangeObserverInternal& aObserver, TLogViewId aViewId, TInt aPriority);
	~CLogViewObserver();

private:
	CLogViewObserver(CLogView& aView, CLogClient& aClient, MLogViewChangeObserverInternal& aObserver, TLogViewId aViewId, TInt aPriority);
	void ConstructL();

public:
	void RequestChanges();

private: // FROM CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	void RequestChangesCancel();
	void FetchChangesL(TInt aBufferSizeRequired);
	void NotifyObserverL();

private:
	CLogClient& iClient;
	MLogViewChangeObserverInternal& iObserver;
	TLogViewId iViewId;
	CLogChangeDefinition* iChanges;
	CLogView& iView;
	};


#endif
