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

#include "msmmclient.h"
#include <usb/usblogger.h>

#include <e32cmn.h>

#include "srvdef.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "msmmclientTraces.h"
#endif

 
// Costants
const TInt KConnectRetry = 0x2;

//---------------------------------------------------------------------------
//
// NON-MEMBER FUNCTIONS

static TInt StartServer()
    {
    OstTraceFunctionEntry0( MSMMCLIENT_SRC_STARTSERVER_ENTRY );
    
    TInt ret = KErrNone;

    // Create the server process
    const TUidType serverUid(KNullUid, KNullUid, KMsmmServUid);
    RProcess server;
    _LIT(KNullCommand,"");
    ret = server.Create(KMsmmServerBinaryName, KNullCommand, serverUid);

    // Was server process created OK?
    if (KErrNone != ret)
        {
        OstTraceFunctionExit0( MSMMCLIENT_SRC_STARTSERVER_EXIT );
        return ret;
        }

    // Set up Rendezvous so that server thread can signal correct startup
    TRequestStatus serverDiedRequestStatus;
    server.Rendezvous(serverDiedRequestStatus);

    // Status flag should still be pending as we haven't 
    // resumed the server yet!
    if (serverDiedRequestStatus != KRequestPending)
        {
        server.Kill(0); // abort the startup here
        }
    else
        {
        server.Resume(); // start server
        }

    User::WaitForRequest(serverDiedRequestStatus);

    // determine the reason for the server exit
    TInt exitReason = (EExitPanic == server.ExitType()) ? 
            KErrGeneral : serverDiedRequestStatus.Int();
    server.Close();
    OstTraceFunctionExit0( MSMMCLIENT_SRC_STARTSERVER_EXIT_DUP1 );
    return exitReason;
    }

//---------------------------------------------------------------------------
// RMsmmSession
//
// Public member functions

EXPORT_C TInt RMsmmSession::Connect()
    {
    OstTraceFunctionEntry0( RMSMMSESSION_CONNECT_ENTRY );
    
    TInt retry = KConnectRetry; // Attempt connect twice then give up
    TInt ret(KErrNone);
    FOREVER
        {
        ret = CreateSession(KMsmmServerName, Version(), KDefaultMessageSlots);
        if ((KErrNotFound != ret) && (KErrServerTerminated != ret))
            {
            break;
            }

        if ((--retry) == 0)
            {
            break;
            }

        ret = StartServer();
        if ((KErrNone != ret) && (KErrAlreadyExists != ret))
            {
            break;
            }
        }

    if (KErrNone != ret)
        {
        OstTrace1( TRACE_ERROR, RMSMMSESSION_CONNECT, 
                "Underlying error value = %d", ret );
        
        ret = KErrCouldNotConnect;
        }
    
    OstTraceFunctionExit0( RMSMMSESSION_CONNECT_EXIT );
    return ret; 
    }

EXPORT_C TInt RMsmmSession::Disconnect()
    {
    OstTraceFunctionEntry0( RMSMMSESSION_DISCONNECT_ENTRY );
    Close();
    OstTraceFunctionExit0( RMSMMSESSION_DISCONNECT_EXIT );
    return KErrNone;
    }

// Called to provide the version number of the server we require for this API
EXPORT_C TVersion RMsmmSession::Version() const
    {
    OstTraceFunctionEntry0( RMSMMSESSION_VERSION_ENTRY );
    
    return TVersion(KMsmmServMajorVersionNumber,
                    KMsmmServMinorVersionNumber,
                    KMsmmServBuildVersionNumber);
    }

EXPORT_C TInt RMsmmSession::AddFunction(
        const TUSBMSDeviceDescription& aDevice,
        TUint8 aInterfaceNumber, TUint32 aInterfaceToken)
    {
    OstTraceFunctionEntry0( RMSMMSESSION_ADDFUNCTION_ENTRY );
    
    TInt ret(KErrNone);
    
    TIpcArgs usbmsIpcArgs;
    iDevicePkg = aDevice; // Package the device description
    usbmsIpcArgs.Set(0, &iDevicePkg);
    usbmsIpcArgs.Set(1, aInterfaceNumber);
    usbmsIpcArgs.Set(2, aInterfaceToken);

    ret = SendReceive(EHostMsmmServerAddFunction, usbmsIpcArgs);
    
    OstTraceFunctionExit0( RMSMMSESSION_ADDFUNCTION_EXIT );
    return ret;
    }

EXPORT_C TInt RMsmmSession::RemoveDevice(TUint aDevice)
    {
    OstTraceFunctionEntry0( RMSMMSESSION_REMOVEDEVICE_ENTRY );
    
    TInt ret(KErrNone);

    TIpcArgs usbmsIpcArgs(aDevice);

    ret = SendReceive(EHostMsmmServerRemoveDevice, usbmsIpcArgs);
    
    OstTraceFunctionExit0( RMSMMSESSION_REMOVEDEVICE_EXIT );
    return ret;
    }

EXPORT_C TInt RMsmmSession::__DbgFailNext(TInt aCount)
    {
    OstTraceFunctionEntry0( RMSMMSESSION_DBGFAILNEXT_ENTRY );
    
#ifdef _DEBUG
    return SendReceive(EHostMsmmServerDbgFailNext, TIpcArgs(aCount));
#else
    (void)aCount;
    OstTraceFunctionExit0( RMSMMSESSION_DBGFAILNEXT_EXIT );
    return KErrNone;
#endif
    }

EXPORT_C TInt RMsmmSession::__DbgAlloc()
    {
    OstTraceFunctionEntry0( RMSMMSESSION_DBGALLOC_ENTRY );
    
#ifdef _DEBUG
    return SendReceive(EHostMsmmServerDbgAlloc);
#else
    OstTraceFunctionExit0( RMSMMSESSION_DBGALLOC_EXIT );
    return KErrNone;
#endif
    }

// End of file
