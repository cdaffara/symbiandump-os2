/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef MSMMCLIENT_H
#define MSMMCLIENT_H

#include <e32std.h>
#include "msmm_pub_def.h"

/**
RMsmmSession is the interface for MFDC to use the MSMM.
*/
NONSHARABLE_CLASS(RMsmmSession) : public RSessionBase
{
public: 
    //  RMsmmSession Implementaion
    IMPORT_C TInt Connect();
    IMPORT_C TInt Disconnect();
    IMPORT_C TVersion Version() const;
    
    /** Add USB Mass Storage Function to the MSMM synchronously.
    @param aDevice Usb Mass Storage device description.
    @param aInterfaceNumber The Usb Mass Storage interface number.
    @param aInterfaceToken The Usb Mass Storage interface token.
    @return Error code of IPC. 
    */
    IMPORT_C TInt AddFunction(const TUSBMSDeviceDescription& aDevice,
            TUint8 aInterfaceNumber,
            TUint32 aInterfaceToken);

    /** Remove USB Mass Storage device from the MSMM synchronously.
    @param aDevice Usb Mass Storage device description.
    @return Error code of IPC.
    */
    IMPORT_C TInt RemoveDevice(TUint aDevice);

    // Support for server-side out-of-memory testing. In release, these just
    // return KErrNone.
    IMPORT_C TInt __DbgFailNext(TInt aCount);
    IMPORT_C TInt __DbgAlloc();
    
    
private:
    // RMsmmSession data member
    TPckgBuf<TUSBMSDeviceDescription> iDevicePkg;
    TUint8 iInterfaceNumber;
    TUint32 iInterfaceToken;
};

#endif // MSMMCLIENT_H

// End of file
