/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include "usbhoststack.h"
#include "fdfapi.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "sessionTraces.h"
#endif



/**
Starts the server process.
*/
static TInt StartServer()
	{
    OstTraceFunctionEntry0( _FDF_STARTSERVER_ENTRY );
    
	const TUidType serverUid(KNullUid, KNullUid, KUsbFdfUid);

	//
	// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt err = server.Create(KUsbFdfImg, KNullDesC, serverUid);
	
	OstTrace1( TRACE_NORMAL, _FDF_STARTSERVER, "\terr = %d", err );

	if ( err != KErrNone )
		{
		OstTraceFunctionExit0( _FDF_STARTSERVER_EXIT );
		return err;
		}

	TRequestStatus stat;
	server.Rendezvous(stat);

	if ( stat != KRequestPending )
		{
		OstTrace0( TRACE_NORMAL, _FDF_STARTSERVER_DUP1, "\taborting startup" );
		
		server.Kill(0); 	// abort startup
		}
	else
		{
		OstTrace0( TRACE_NORMAL, _FDF_STARTSERVER_DUP2, "\tresuming" );
		
		server.Resume();	// logon OK - start the server
		}

	User::WaitForRequest(stat); 	// wait for start or death

	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	OstTrace1( TRACE_NORMAL, _FDF_STARTSERVER_DUP3, "\tstat.Int = %d", stat.Int());
	
	err = (server.ExitType() == EExitPanic) ? KErrServerTerminated : stat.Int();

	server.Close();

	OstTrace1( TRACE_NORMAL, _FDF_STARTSERVER_DUP4, "\terr = %d", err );
	
	OstTraceFunctionExit0( _FDF_STARTSERVER_EXIT_DUP1 );
	return err;
	}

EXPORT_C RUsbHostStack::RUsbHostStack()
	// these are all arbitrary initialisations
 :	iDeviceEventPckg(TDeviceEventInformation()),
	iDevmonEventPckg(0)
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_RUSBHOSTSTACK_ENTRY );
    
	OstTrace0( TRACE_NORMAL, RUSBHOSTSTACK_RUSBHOSTSTACK, "*** Search on '***USB HOST STACK' to find device events." );
	    
	}

EXPORT_C TVersion RUsbHostStack::Version() const
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_VERSION_ENTRY );
        
	return(TVersion(	KUsbFdfSrvMajorVersionNumber,
						KUsbFdfSrvMinorVersionNumber,
						KUsbFdfSrvBuildNumber
					)
			);
	}

EXPORT_C TInt RUsbHostStack::Connect()
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_CONNECT_ENTRY );
            
	TInt err = DoConnect();

	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_CONNECT, "\terr = %d", err);
	    
	return err;
	}

/**
Connects the session, starting the server if necessary
@return Error.
*/
TInt RUsbHostStack::DoConnect()
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_DOCONNECT_ENTRY);
    

	TInt retry = 2;

	FOREVER
		{
		// Use message slots from the global pool.
		TInt err = CreateSession(KUsbFdfServerName, Version(), -1);
		OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_DOCONNECT, "\terr = %d", err );
		
		if ((err != KErrNotFound) && (err != KErrServerTerminated))
			{
			OstTrace0( TRACE_NORMAL, RUSBHOSTSTACK_DOCONNECT_DUP1, "\treturning after CreateSession" );
			
			OstTraceFunctionExit0( RUSBHOSTSTACK_DOCONNECT_EXIT);
			return err;
			}

		if (--retry == 0)
			{
			OstTrace0( TRACE_NORMAL, RUSBHOSTSTACK_DOCONNECT_DUP2, "\treturning after running out of retries" );
			
			OstTraceFunctionExit0( RUSBHOSTSTACK_DOCONNECT_EXIT_DUP1 );
			return err;
			}

		err = StartServer();
		OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_DOCONNECT_DUT3, "\terr = %d", err );
		        
		if ((err != KErrNone) && (err != KErrAlreadyExists))
			{
			OstTrace0( TRACE_NORMAL, RUSBHOSTSTACK_DOCONNECT_DUP4, "\treturning after StartServer" );
			            
			OstTraceFunctionExit0( RUSBHOSTSTACK_DOCONNECT_EXIT_DUP2 );
			return err;
			}
		}
	}

EXPORT_C TInt RUsbHostStack::EnableDriverLoading()
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_ENABLEDRIVERLOADING_ENTRY );
    
	TInt ret = SendReceive(EUsbFdfSrvEnableDriverLoading);
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_ENABLEDRIVERLOADING, "\tret = %d", ret );

	OstTraceFunctionExit0( RUSBHOSTSTACK_ENABLEDRIVERLOADING_EXIT );
	return ret;
	}

EXPORT_C void RUsbHostStack::DisableDriverLoading()
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_DISABLEDRIVERLOADING_ENTRY );

	TInt ret = SendReceive(EUsbFdfSrvDisableDriverLoading);
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_DISABLEDRIVERLOADING, "\tret = %d", ret );
	(void)ret;
	OstTraceFunctionExit0( RUSBHOSTSTACK_DISABLEDRIVERLOADING_EXIT );
	}

EXPORT_C void RUsbHostStack::NotifyDeviceEvent(TRequestStatus& aStat, TDeviceEventInformation& aDeviceEventInformation)
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_NOTIFYDEVICEEVENT_ENTRY );
    
	TIpcArgs args;
	iDeviceEventPckg.Set((TUint8*)&aDeviceEventInformation, sizeof(TDeviceEventInformation), sizeof(TDeviceEventInformation));
	args.Set(0, &iDeviceEventPckg);

	SendReceive(EUsbFdfSrvNotifyDeviceEvent, args, aStat);
	OstTraceFunctionExit0( RUSBHOSTSTACK_NOTIFYDEVICEEVENT_EXIT );
	}

EXPORT_C void RUsbHostStack::NotifyDeviceEventCancel()
	{
	OstTraceFunctionEntry0( RUSBHOSTSTACK_NOTIFYDEVICEEVENTCANCEL_ENTRY );
	
	TInt ret = SendReceive(EUsbFdfSrvNotifyDeviceEventCancel);
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_NOTIFYDEVICEEVENTCANCEL, "\tret = %d", ret);
	(void)ret;
	OstTraceFunctionExit0( RUSBHOSTSTACK_NOTIFYDEVICEEVENTCANCEL_EXIT );
	}

EXPORT_C void RUsbHostStack::NotifyDevmonEvent(TRequestStatus& aStat, TInt& aEvent)
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_NOTIFYDEVMONEVENT_ENTRY );
  
	TIpcArgs args;
	iDevmonEventPckg.Set((TUint8*)&aEvent, sizeof(TInt), sizeof(TInt));
	args.Set(0, &iDevmonEventPckg);

	SendReceive(EUsbFdfSrvNotifyDevmonEvent, args, aStat);
	OstTraceFunctionExit0( RUSBHOSTSTACK_NOTIFYDEVMONEVENT_EXIT );
	}

EXPORT_C void RUsbHostStack::NotifyDevmonEventCancel()
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_NOTIFYDEVMONEVENTCANCEL_ENTRY );
    
	TInt ret = SendReceive(EUsbFdfSrvNotifyDevmonEventCancel);
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_NOTIFYDEVMONEVENTCANCEL, "\tret = %d", ret);
	(void)ret;
	OstTraceFunctionExit0( RUSBHOSTSTACK_NOTIFYDEVMONEVENTCANCEL_EXIT );
	}

EXPORT_C TInt RUsbHostStack::GetSupportedLanguages(TUint aDeviceId, RArray<TUint>& aLangIds)
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_GETSUPPORTEDLANGUAGES_ENTRY );
    
	aLangIds.Reset();

	TUint singleLangIdOrNumLangs = 0;
	TPckg<TUint> singleLangIdOrNumLangsBuf(singleLangIdOrNumLangs);
	TInt ret = SendReceive(EUsbFdfSrvGetSingleSupportedLanguageOrNumberOfSupportedLanguages, TIpcArgs(aDeviceId, &singleLangIdOrNumLangsBuf));
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_GETSUPPORTEDLANGUAGES, "\tsingleLangIdOrNumLangs = %d", singleLangIdOrNumLangs);
	    
	switch ( ret )
		{
	case KErrNotFound:
		OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_GETSUPPORTEDLANGUAGES_DUP1, "\tThere is no language available or the wrong device id %d was supplied",aDeviceId);
		    
		ret = KErrNotFound;
		break;

	case KErrNone:
		// The buffer is now either empty or contains the single supported language ID
		ret = CopyLangIdsToArray(aLangIds, singleLangIdOrNumLangsBuf);
		break;

	case KErrTooBig:
		{
		// The buffer now contains the number of supported languages (not 0 or 1).
		// Lang IDs are TUints.
		RBuf8 buf;
		ret = buf.Create(singleLangIdOrNumLangs * sizeof(TUint));
		if ( ret == KErrNone )
			{
			ret = SendReceive(EUsbFdfSrvGetSupportedLanguages, TIpcArgs(aDeviceId, &buf));
			if ( ret == KErrNone )
				{
				ret = CopyLangIdsToArray(aLangIds, buf);
				}
			buf.Close();
			}
		}
		break;

	default:
		// Regular failure.
		break;
		}

	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_GETSUPPORTEDLANGUAGES_DUP2, "\tret = %d", ret);
	    
	OstTraceFunctionExit0( RUSBHOSTSTACK_GETSUPPORTEDLANGUAGES_EXIT );
	return ret;
	}

TInt RUsbHostStack::CopyLangIdsToArray(RArray<TUint>& aLangIds, const TDesC8& aBuffer)
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_COPYLANGIDSTOARRAY_ENTRY );
    
	ASSERT(!(aBuffer.Size() % 4));
	const TUint numLangs = aBuffer.Size() / 4;
	
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_COPYLANGIDSTOARRAY, "\tnumLangs = %d", numLangs );
	
	TInt ret = KErrNone;
	const TUint* ptr = reinterpret_cast<const TUint*>(aBuffer.Ptr());
	for ( TUint ii = 0 ; ii < numLangs ; ++ii )
		{
		ret = aLangIds.Append(*ptr++); // increments by sizeof(TUint)
		if ( ret )
			{
			aLangIds.Reset();
			break;
			}
		}

	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_COPYLANGIDSTOARRAY_DUP1, "\tret = %d", ret );
	    
	OstTraceFunctionExit0( RUSBHOSTSTACK_COPYLANGIDSTOARRAY_EXIT );
	return ret;
	}

EXPORT_C TInt RUsbHostStack::GetManufacturerStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
	OstTraceFunctionEntry0( RUSBHOSTSTACK_GETMANUFACTURERSTRINGDESCRIPTOR_ENTRY );
	
	TInt ret = SendReceive(EUsbFdfSrvGetManufacturerStringDescriptor, TIpcArgs(aDeviceId, aLangId, &aString));
	if ( !ret )
		{
        OstTraceExt1( TRACE_NORMAL, RUSBHOSTSTACK_GETMANUFACTURERSTRINGDESCRIPTOR, "RUsbHostStack::GetManufacturerStringDescriptor;aString=%S", aString );
        
        }
	
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_GETMANUFACTURERSTRINGDESCRIPTOR_DUP1, "\tret = %d", ret );
	        
	OstTraceFunctionExit0( RUSBHOSTSTACK_GETMANUFACTURERSTRINGDESCRIPTOR_EXIT );
	return ret;
	}

EXPORT_C TInt RUsbHostStack::GetProductStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_GETPRODUCTSTRINGDESCRIPTOR_ENTRY );
    
	TInt ret = SendReceive(EUsbFdfSrvGetProductStringDescriptor, TIpcArgs(aDeviceId, aLangId, &aString));
	if ( !ret )
		{
		OstTraceExt1( TRACE_NORMAL, RUSBHOSTSTACK_GETPRODUCTSTRINGDESCRIPTOR, "\taString = \"%S\"", aString );
	
		
		}
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_GETPRODUCTSTRINGDESCRIPTOR_DUP1, "\tret = %d", ret );
	
	        
	OstTraceFunctionExit0( RUSBHOSTSTACK_GETPRODUCTSTRINGDESCRIPTOR_EXIT );
	return ret;
	}

EXPORT_C TInt RUsbHostStack::GetOtgDescriptor(TUint aDeviceId, TOtgDescriptor& aDescriptor)
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_GETOTGDESCRIPTOR_ENTRY );
    

	TPckg<TOtgDescriptor> otgDescriptorPckg(aDescriptor);
	
	TIpcArgs args;
	args.Set(0, aDeviceId);
	args.Set(1, &otgDescriptorPckg);

	TInt ret = SendReceive(EUsbFdfSrvGetOtgDescriptor, args);
	if ( !ret )
		{
        OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_GETOTGDESCRIPTOR, "\taDescriptor.iDeviceId = %d", aDescriptor.iDeviceId );
        OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_GETOTGDESCRIPTOR_DUP1, "\taDescriptor.iAttributes = %d", aDescriptor.iAttributes );
		}
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_GETOTGDESCRIPTOR_DUP2, "\tret = %d", ret );
	        
	OstTraceFunctionExit0( RUSBHOSTSTACK_GETOTGDESCRIPTOR_EXIT );
	return ret;
	}


EXPORT_C TInt RUsbHostStack::GetConfigurationDescriptor(TUint aDeviceId, TConfigurationDescriptor& aDescriptor)
	{
    OstTraceFunctionEntry0( RUSBHOSTSTACK_GETCONFIGRATIONDESCRIPTOR_ENTRY );
    

	TPckg<TConfigurationDescriptor> configDescriptorPckg(aDescriptor);
	
	TIpcArgs args;
	args.Set(0, aDeviceId);
	args.Set(1, &configDescriptorPckg);

	TInt ret = SendReceive(EUsbFdfSrvGetConfigurationDescriptor, args);
	OstTrace1( TRACE_NORMAL, RUSBHOSTSTACK_GETCONFIGRATIONDESCRIPTOR_DUP2, "\tret = %d", ret );
	        
	OstTraceFunctionExit0( RUSBHOSTSTACK_GETCONFIGRATIONDESCRIPTOR_EXIT );
	return ret;
	}


EXPORT_C TInt RUsbHostStack::__DbgFailNext(TInt aCount)
	{
#ifdef _DEBUG
	return SendReceive(EUsbFdfSrvDbgFailNext, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

EXPORT_C TInt RUsbHostStack::__DbgAlloc()
	{
#ifdef _DEBUG
	return SendReceive(EUsbFdfSrvDbgAlloc);
#else
	return KErrNone;
#endif
	}
