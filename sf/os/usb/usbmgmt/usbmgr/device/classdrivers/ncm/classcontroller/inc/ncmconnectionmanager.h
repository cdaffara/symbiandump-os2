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

#ifndef NCMCONNECTIONMANAGER_H
#define NCMCONNECTIONMANAGER_H

#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_essock.h>
#include <usb/testncmcc/dummy_insock.h>
#include <usb/testncmcc/dummy_nifman.h>
#else
#include <es_sock.h>
#include <in_sock.h> 
#include <nifman.h>
#include <commdbconnpref.h>
#endif // OVERDUMMY_NCMCC

#include <e32property.h>
#include "ncmcommon.h"
#include "ncmiapprogressstateobserver.h"
#include "ncmiapconnectionobserver.h"
#include "ncmdhcpnotifobserver.h"
#include "ncmiapreaderobserver.h"


class MNcmConnectionManagerObserver;
class CNcmIapProgressWatcher;
class CNcmConnectionManHelper;
class CNcmDhcpNotifWatcher;
class CNcmIapReader;
class RNcmInternalSrv;

/**
 * CNcmConnectionManager to control ncm networking interface
 * It handles:
 *  1 Searching NCM IAP
 *  2 Start NCM connection
 *  3 IAP progress notification
 *  4 DHCP provisioning request from NCM internal server
 */
NONSHARABLE_CLASS(CNcmConnectionManager) : public CBase,
										public MNcmIapProgressStateObserver,
										public MNcmConnectionObserver,
										public MNcmDhcpNotificationObserver,
										public MNcmIAPReaderObserver
	{
public:
    /**
     * Constructs a CNcmConnectionManager object.
     * @param[in]   aOwner, NCM connection observer
     * @param[in]   aHostMacAddress, the NCM interface MAC address
     * @param[in]   aDataBufferSize, the EndPoint buffer size
     * @param[in]   aNcmInternalSvr, the NCM internal server
     * @return      Ownership of a new CNcmConnectionManager object
     */ 
	static CNcmConnectionManager* NewL(MNcmConnectionManagerObserver& aOwner,
            const TNcmMacAddress& aMacAddress, TUint aDataBufferSize,
            RNcmInternalSrv&   aNcmInternalSvr);
	~CNcmConnectionManager();
	
    /**
     * Start NCM connection
     * @param aStatus complete status for this request.
     */
	void Start(TRequestStatus& aStatus);
	
    /**
     * Cancel starting of NCM connection 
     */
	void StartCancel();
    
    /**
     * Stop NCM connection 
     */
	void Stop();
	
	// From MIapPorgressStateObserver
	virtual void MipsoHandleClassFatalError(TInt aError);
	
	// From MNcmConnectionObserver
	virtual void MicoHandleConnectionComplete(TInt aError);
	
	// From MNcmDhcpNotificationObserver
	virtual void MdnoHandleDhcpNotification(TInt aError);
	
	// From MNcmIAPReaderObserver
	virtual void MicoHandleIAPReadingComplete(TInt aError);

private:
    /**
     * Two-phase construction: phase 1
     * @param[in]   aObserver, NCM connection observer
     * @param[in]   aHostMacAddress, the NCM interface MAC address
     * @param[in]   aDataBufferSize, the EndPoint buffer size
     * @param[in]   aNcmInternalSvr, the NCM internal server
     */ 
	CNcmConnectionManager(MNcmConnectionManagerObserver& aObserver, 
	        const TNcmMacAddress& aHostMacAddress, TUint aDataBufferSize,
            RNcmInternalSrv&   aNcmInternalSvr);
	/**
	 * Two-phase construction: phase 2
	 */
	void ConstructL();
	
	/**
	 * Reset NCM connection manager
	 */
    void Reset();
    
    /*
     * Change default MTU size on NCM connection
     */
    void SetCustomMtuL();
    
    /*
     * Handle NCM connection building completion 
     */
	void HandleConnectionCompleteL();
	
private:
	// Owned.
	CNcmIapProgressWatcher*       iIapPrgrsWatcher;
	CNcmConnectionManHelper*      iConnManHelper;
	CNcmDhcpNotifWatcher*         iDhcpNotifWatcher;
	CNcmIapReader*                iIapReader;

	// Not own.
	TRequestStatus* 					iReportStatus;
	
	MNcmConnectionManagerObserver& 		iConnMgrObserver;
	
	// RSocketServ used for starting NCM IAP and configuring DHCP and NAPT, owned
	RSocketServ							iSocketServ;

	// RConnection used for starting NCM IAP and configuring DHCP server, owned
	RConnection							iConnection;
    
    TCommDbConnPref                     iConnPref;

	RProperty 							iProperty;
	TBool 								iConnComplete;
	
    const TNcmMacAddress&               iHostMacAddress;
    const TUint                         iDataBufferSize;
    RNcmInternalSrv&                    iNcmInternalSvr;
};

#endif // NCMCONNECTIONMANAGER_H
