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

#ifndef __LOGCLIENTOBSERVER_H__
#define __LOGCLIENTOBSERVER_H__

// System includes
#include <e32base.h>

// User includes
#include <logwrap.h>

// Classes referenced
class CLogClient;
class MLogClientChangeObserver;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogClientObserver (header)
/////////////////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CLogClientObserver) : public CActive
/**
@internalComponent
*/
	{
public:
	static CLogClientObserver* NewL(CLogClient& aClient, MLogClientChangeObserver& aObserver, TInt aPriority);
	~CLogClientObserver();

private:
	CLogClientObserver(CLogClient& aClient, MLogClientChangeObserver& aObserver, TInt aPriority);

private: // FROM CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	void RequestChanges();
	void RequestChangesCancel();
	void NotifyObserverL();

private:
	CLogClient& iClient;
	MLogClientChangeObserver& iObserver;
	//
	TPckgBuf<TInt> iContextBuf;
	TPckgBuf<TInt> iParamBuf1;
	TPckgBuf<TInt> iParamBuf2;
	TPckgBuf<TInt> iParamBuf3;
	};


#endif
