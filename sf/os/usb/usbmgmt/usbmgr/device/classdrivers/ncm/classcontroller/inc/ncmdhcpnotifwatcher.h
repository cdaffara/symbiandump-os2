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

#ifndef NCMDHCPNOTIFWATCHER_H
#define NCMDHCPNOTIFWATCHER_H

#include <e32base.h>

#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_essock.h>
#include <usb/testncmcc/dummy_insock.h>
#else
#include <es_sock.h>
#include <in_sock.h> 
#endif // OVERDUMMY_NCMCC

class MNcmDhcpNotificationObserver;
class RNcmInternalSrv;

/*
 * CNcmDhcpNotifWatcher is handler of DHCP provisioning requestion from NCM 
 * internal server.
 */
NONSHARABLE_CLASS(CNcmDhcpNotifWatcher) : public CActive
	{
public:
    /**
     * Constructor
     * @param[in]  aObserver, the observer of DHCP provisioning result. 
     * @param[in]  aNcmInteralSvr, the NCM internal server.
     */
    CNcmDhcpNotifWatcher(MNcmDhcpNotificationObserver& aObserver,
            RNcmInternalSrv& aNcmInteralSvr);
    
	~CNcmDhcpNotifWatcher();
	
	/**
	 * Start the watcher to monitor DHCP provisioning requestion from
	 * NCM internal server.
	 */
	void StartL();
	
private:
	
	// From CActive
	void RunL();
	void DoCancel();

private:
	
	MNcmDhcpNotificationObserver& iObserver;  // DHCP provisioning result observer
	RNcmInternalSrv&           iNcmInteralSvr; // NCM internal server
	};

#endif // NCMDHCPNOTIFWATCHER_H

