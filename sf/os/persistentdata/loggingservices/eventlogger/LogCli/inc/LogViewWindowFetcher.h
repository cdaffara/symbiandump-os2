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

#ifndef __LOGVIEWWINDOWFETCHER_H__
#define __LOGVIEWWINDOWFETCHER_H__

// System includes
#include <e32base.h>

// User includes
#include <logwrap.h>
#include "LogCliServShared.h"
#include <logviewchangeobserver.h>

// Classes referenced
class RLogSession;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> MLogViewWindowFetcherObserver (header)
/////////////////////////////////////////////////////////////////////////////////////////
class MLogViewWindowFetcherObserver
/**
@internalComponent
*/
	{
public:
	virtual void HandleFetchedWindowItemL(TInt aItemIndex, CLogEvent* aEvent) = 0;
	};


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewWindowFetcher (header)
/////////////////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CLogViewWindowFetcher) : public CActive
/**
@internalComponent
*/
	{
public:
	CLogViewWindowFetcher(RLogSession& aSession, TLogViewId aViewId, MLogViewWindowFetcherObserver& aObserver, TInt aPriority);
	~CLogViewWindowFetcher();
	void ConstructL();

public:

	/**
	 * Fetch the specified window.
	 */
	void PrepareToFetchWindowL(const TLogWindowAndCursor& aRequestedWindow);

	/**
	 * Start the asynchronous fetch
	 */
	void Start(TRequestStatus& aObserver);

	/**
	 * Cancel any fetch operation but without notifying the observer
	 */
	void SilentCancel();

	/**
	 * Access the window & cursor for the requested window
	 */
	inline const TLogWindowAndCursor& RequestedWindow() const;

private:
	void Fetch(const TLogWindow& aWindow);
	void CompleteObserver(TInt aError);

private: // FROM CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	RLogSession& iSession;
	const TLogViewId iViewId;
	MLogViewWindowFetcherObserver& iObserver;

private:
	TLogWindowAndCursor iRequestedWindow;
	//
	TRequestStatus* iObserverRequestStatus;
	CBufBase* iBuffer;
	TPtr8 iBufferPointer;
	TPckgBuf<TLogTransferWindow> iFetchWindowData;
	TPckgBuf<TLogClientServerData> iData;
	TLogWindowAndCursor iWindowReq;
	};


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewWindowFetcher (inline)
/////////////////////////////////////////////////////////////////////////////////////////
inline const TLogWindowAndCursor& CLogViewWindowFetcher::RequestedWindow() const
	{
	return iRequestedWindow;
	}

#endif
