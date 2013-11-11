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

#ifndef NCMCLASSCONTROLLER_H
#define NCMCLASSCONTROLLER_H

#include <e32property.h>
#include <cusbclasscontrollerplugin.h>

#include "ncmconnectionmanagerobserver.h"
#include "ncmcommon.h"

#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_ncminternalsrv.h>
#else
#include "ncminternalsrv.h"
#endif // OVERDUMMY_NCMCC

class CNcmConnectionManager;
class CNcmClientManager;

/**
 * NCM Class controller. 
 * It is derived from CUsbClassControllerPlugIn. The USB Manager will control 
 * NCM class through it.
 * And it implements interface MNcmConnectionManagerObserver as well. 
 */
NONSHARABLE_CLASS(CNcmClassController): 
                public CUsbClassControllerPlugIn,
                public MNcmConnectionManagerObserver
	{
public:
	static CNcmClassController* NewL(MUsbClassControllerNotify& aOwner);
	~CNcmClassController();

	// From MNcmConnectionManagerObserver
    void McmoErrorIndication(TInt aError);
    
private:
    /**
     * Two-phases construction: phase 1
     * @param[in]  aOwner, an Interface to talk to the USB server.
     */
	CNcmClassController(MUsbClassControllerNotify& aOwner);
    /**
     * Two-phases construction: phase 2
     */
	void ConstructL();
	
	// From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	// From CUsbClassControllerBase
	void Start(TRequestStatus& aStatus);
	void Stop(TRequestStatus& aStatus);
	void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

	
private:
	void RandomMacAddressL();
	
private: 
    TNcmMacAddress         iHostMacAddress;
	
    CNcmConnectionManager*    iConnectionMan; // Owned.
	
	// Not own.
	TRequestStatus*        iReportStatus;
	
	CNcmClientManager*     iClientMgr; // Owned
	
	RNcmInternalSrv        iNcmInternalSvr;
	
	TBool                  iConnectingToNcmPktDrv;

    TUint                  iDataBufferSize; // Buffer size of data end points
	};

#endif // NCMCLASSCONTROLLER_H
