/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <usb/usblogger.h>
#include <e32cmn.h>

#include "srvdef.h"
#include "msmmindicatorclient.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "msmmindicatorclientTraces.h"
#endif

 

// Costants
const TInt KConnectRetry = 0x2;


//---------------------------------------------------------------------------
// RHostMassStorage
//
// Public member functions

EXPORT_C TInt RHostMassStorage::Connect()
    {
    OstTraceFunctionEntry0( RHOSTMASSSTORAGE_CONNECT_ENTRY );
    
    TInt retry = KConnectRetry; // Attempt to add a session to MSMM Server twice
    TInt ret(KErrNone);
    FOREVER
        {
        ret = CreateSession(KMsmmServerName, Version(), KDefaultMessageSlots);
        // We are not allowed to start the server
        if ((KErrNotFound == ret) || (KErrServerTerminated == ret))
            {
            OstTrace1( TRACE_NORMAL, RHOSTMASSSTORAGE_CONNECT, 
                    "Underlying error value = %d", ret );
            return KErrNotReady;
            }
        if ( KErrNone == ret )
            {
            break;
            }
        if ((--retry) == 0)
            {
            break;
            }
        }    
    OstTraceFunctionExit0( RHOSTMASSSTORAGE_CONNECT_EXIT );
    return ret; 
    }

EXPORT_C TInt RHostMassStorage::Disconnect()
    {
    OstTraceFunctionEntry0( RHOSTMASSSTORAGE_DISCONNECT_ENTRY );
    
    Close();
    OstTraceFunctionExit0( RHOSTMASSSTORAGE_DISCONNECT_EXIT );
    return KErrNone;
    }

/**
 *  Called to validate the version of the server we require for this API
 *  @return TVersion    The version of MSMM Server that supports this API
 */
EXPORT_C TVersion RHostMassStorage::Version() const
    {
    OstTraceFunctionEntry0( RHOSTMASSSTORAGE_VERSION_ENTRY );
    
    return TVersion(KMsmmServMajorVersionNumber,
                    KMsmmServMinorVersionNumber,
                    KMsmmServBuildVersionNumber);
    }

/** 
 * Dismount USB drives from File System asynchronously. The function will return immediately to the UI with complete status
 * The result of dismounting USB drives will be notified to the MSMM Plugin via CMsmmPolicyPluginBase
 * @return Error code of IPC.
*/

EXPORT_C TInt RHostMassStorage::EjectUsbDrives()
    {
    OstTraceFunctionEntry0( RHOSTMASSSTORAGE_EJECTUSBDRIVES_ENTRY );
    
    TInt ret(KErrNone);

    TIpcArgs usbmsIpcArgs;

    ret = Send(EHostMsmmServerEjectUsbDrives, usbmsIpcArgs);
    
    OstTraceFunctionExit0( RHOSTMASSSTORAGE_EJECTUSBDRIVES_EXIT );
    return ret;
    }


// End of file
