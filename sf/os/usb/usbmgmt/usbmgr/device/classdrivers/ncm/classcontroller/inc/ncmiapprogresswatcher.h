/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

/** @file
@internalComponent
*/

#ifndef NCMIAPPROGRESSWATCHER_H
#define NCMIAPPROGRESSWATCHER_H

#include <e32base.h>

#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_essock.h>
#include <usb/testncmcc/dummy_insock.h>
#else
#include <es_sock.h>
#include <in_sock.h> 
#endif // OVERDUMMY_NCMCC

class MNcmIapProgressStateObserver;

/**
 * CNcmIapProgressWatcher will handle IAP state changes
 */
NONSHARABLE_CLASS(CNcmIapProgressWatcher) : public CActive
	{
public:
    /**
     * Constructor
     * @param[in]  aObserver, the observer of IAP progress change. 
     * @param[in]  aConnection, the NCM connection.
     */
    CNcmIapProgressWatcher(MNcmIapProgressStateObserver& aObserver, 
                        RConnection& aConnection);
    
	~CNcmIapProgressWatcher();
	
	/**
	 * Start IAP progress watcher
	 */
	void Start();
	
private:
	// From CActive
	void RunL();
	void DoCancel();

private:
	
	TNifProgressBuf            iProgressInfoBuf;
	MNcmIapProgressStateObserver& iObserver;
	RConnection&               iConnection;
	};

#endif // NCMIAPPROGRESSWATCHER_H

