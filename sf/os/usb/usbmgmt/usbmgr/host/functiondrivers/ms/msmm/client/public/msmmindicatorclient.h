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

#ifndef MSMMINDICATORCLIENT_H
#define MSMMINDICATORCLIENT_H

#include <e32std.h>

/**
RHostMassStorage is the interface for Mass Storage Indicator UI to use the MSMM.
*/
NONSHARABLE_CLASS(RHostMassStorage) : public RSessionBase
{
public: 
    //  RHostMassStorage Implementation API to add a session to the running MSMM server
    IMPORT_C TInt Connect();
    IMPORT_C TInt Disconnect();
    IMPORT_C TVersion Version() const;
    
    /** Dismount USB drives from File System asynchronously. The function will return immediately to the UI with complete status
        The result of dismounting USB drives will be notified to the MSMM Plugin via CMsmmPolicyPluginBase
        @return Error code of IPC.
    */
    IMPORT_C TInt EjectUsbDrives();    
    
};

#endif // MSMMINDICATORCLIENT_H

// End of file
