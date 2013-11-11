/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Active Object for asynchronous scan request.
*
*/


#ifndef WLMSCANREQUEST_H
#define WLMSCANREQUEST_H

#include "rwlmserver.h"

/**
 * Active Object for asynchronous scan request.
 *
 * This class implements an Active Object that is used for handling
 * an asynchronous scan request.
 *
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CWlmScanRequest ) : public CActive
    {

public:

    /**
     * Factory method for creating CWlmScanRequest instance.
     *
     * @param aClient Handle to client-side interface to WLAN engine.
     * @return Pointer to the created instance, NULL if none.
     */
    static CWlmScanRequest* NewL(
        RWLMServer& aClient );

    /**
     * Destructor.
     */
    virtual ~CWlmScanRequest();

    /**
     * Issue a scan request.
     *
     * @since S60 v5.0
     * @param aScanList ScanList used for storing the results.
     * @param aSsid SSID used for scanning.
     * @param aCacheLifetime how old cached results (in seconds) the client is willing to accept.
     * @param aMaxDelay maximum time (in seconds) the client is willing to wait for the scan results.
     * @param aStatus Request status of the client that will be completed
     *                when scan has been completed. NULL if synchronous request.
     * @return KErrNone if an asynchronous request, otherwise the completion
     *         status of the request.
     */
    TInt IssueRequest(
        ScanList& aScanList,
        const TDesC8& aSsid,
        TInt& aCacheLifetime,
        TUint& aMaxDelay,
        TRequestStatus* aStatus );

    /**
     * Return the request status of the client.
     *
     * @since S60 v5.0
     * @return Request status of the client, NULL if none.
     */
    TRequestStatus* ClientStatus(); 

// from base class CActive

    /**
     * From CActive.
     * Called when the request has been completed.
     *
     * @since S60 v5.0
     */       
    void RunL();

    /**
     * From CActive.
     * Called by the framework if RunL leaves.
     *
     * @since S60 v5.0
     * @param aError The error code RunL leaved with.
     * @return KErrNone if leave was handled, an error code otherwise.
     */
    TInt RunError(
        TInt aError );

    /**
     * From CActive.
     * Called by the framework as part of AO's Cancel().
     *
     * @since S60 v5.0
     */
    void DoCancel();

private:

    /**
     * Constructor
     *
     * @param aClient Handle to client-side interface to WLAN engine.
     */
    CWlmScanRequest(
        RWLMServer& aClient );

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Updates the scan list contents after a successful scan.
     *
     * @return KErrNone if success, an error code otherwise.
     */
    TInt UpdateResults();

private: // data

    /**
     * Handle to client-side interface.
     */
    RWLMServer& iClient;

    /**
     * Scan list used for storing the results. Not owned by this pointer.
     */
    ScanList* iScanList;

    /**
     * Descriptor for scan list.
     */
    TPtr8 iScanListPtr;

    /**
     * SSID user for scanning.
     */
    TPckgBuf<TSSID> iSsidBuf;
       
    /**
     * Buffer for storing scan list count and scan list size.
     */
    TPckgBuf<TDynamicScanList> iDynamicScanListBuf;
    
    /**
     * Pointer to cache lifetime parameter. Not owned by this pointer.
     */
    TInt* iCacheLifetime;
    
    /**
     * Pointer to max delay parameter. Not owned by this pointer.
     */
    TUint* iMaxDelay;
    
    /**
     * Buffer for storing scan scheduling parameters
     */
    TPckgBuf<TScanScheduling> iScanSchedulingBuf;

    /**
     * Request status of the client. Not owned by this pointer.
     */
    TRequestStatus* iClientStatus;

    };

#endif // WLMSCANREQUEST_H
