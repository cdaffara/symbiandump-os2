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

/**
 @file
 @internalTechnology
*/

#ifndef NCMINTERNALSRV_H
#define NCMINTERNALSRV_H

#include <e32base.h>

/**
Client-side interface of the NCM internal server for the communication between the NCM Class Controller and the Packet Driver.
*/
NONSHARABLE_CLASS(RNcmInternalSrv) : public RSessionBase
    {
public:
    IMPORT_C RNcmInternalSrv();
    IMPORT_C ~RNcmInternalSrv();

public:
    /**
    Connect the handle to the server.
    Must be called before all other methods (except Version and Close).
    @return Symbian error code
    */
    IMPORT_C TInt Connect();
    /**
    Getter for the version of the server.
    @return Version of the server
    */
    IMPORT_C TVersion Version() const;

public:
    /**
     Transfer the Ldd's handles from NCM Class Controller to the Packet Driver.
     @param[in]   aCommHandle, the Communication Interface handle.
     @param[in]   aCommChunk, the Communication Interface's RChunk handle.
     @param[in]   aDataHandle, the Data Interface handle.
     @param[in]   aDataChunk, the Data Interface's RChunk handle.
     @return      KErrNone, successfully transferred handle to Packet Driver.
                  others, system-wide error code.
    */
    IMPORT_C TInt TransferHandle(RHandleBase& aCommHandle, RHandleBase& aCommChunk, RHandleBase& aDataHandle, RHandleBase& aDataChunk);
    /**
    Set this RConnection's IapId to Packet Driver.
    @param[in]    aIapId, the RConnection's, which loads the Packet Driver, IapId.
    @return       KErrNone, successfully set IapId.
                  others, system-wide error code.
    */
    IMPORT_C TInt SetIapId(TUint aIapId);
    /**
    Set the DHCP Provision result to Packet Driver.
    @param[in]    aResult, the result of DHCP Provision done by Class Controller.
    @return       KErrNone, successfully set the result to Packet Driver.
                  others, system-wide error code.
    */
    IMPORT_C TInt SetDhcpResult(TInt aResult);
    /**
    Register the notification of doing DHCP Provision.
    @param[in,out] aStatus, the request status to be registered.
    */
    IMPORT_C void DhcpProvisionNotify(TRequestStatus& aStatus);
    /**
    Cancel the registration of DHCP Provision.
    */
    IMPORT_C void DhcpProvisionNotifyCancel();
    /**
    Transfer the buffer size of in endpoint of data interface.
    */
    IMPORT_C TInt TransferBufferSize(TUint aSize);

private:
    };

#endif // NCMINTERNALSRV_H
