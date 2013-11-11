/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32uid.h>
#include <usbman.h>
#include <usb.h>
#include <e32base.h>

#ifdef __USBMAN_NO_PROCESSES__
#include <e32math.h>
#endif

#include <usb/usblogger.h>

#include "rusb.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "RUsbTraces.h"
#endif


static TInt StartServer()
//
// Start the server process or thread
//
	{
	const TUidType serverUid(KNullUid, KNullUid, KUsbmanSvrUid);

#ifdef __USBMAN_NO_PROCESSES__
	//
	// In EKA1 WINS the server is a DLL, the exported entrypoint returns a TInt
	// which represents the real entry-point for the server thread
	//
	RLibrary lib;
	TInt err = lib.Load(KUsbmanImg, serverUid);
	
	if (err != KErrNone)
		{
		return err;
		}

	TLibraryFunction ordinal1 = lib.Lookup(1);
	TThreadFunction serverFunc = reinterpret_cast<TThreadFunction>(ordinal1());

	//
	// To deal with the unique thread (+semaphore!) naming in EPOC, and that we may
	// be trying to restart a server that has just exited we attempt to create a
	// unique thread name for the server.
	// This uses Math::Random() to generate a 32-bit random number for the name
	//
	TName name(KUsbServerName);
	name.AppendNum(Math::Random(),EHex);
	
	RThread server;
	err = server.Create (
		name,
		serverFunc,
		KUsbmanStackSize,
		NULL,
		&lib,
		NULL,
		KUsbmanMinHeapSize,
		KUsbmanMaxHeapSize,
		EOwnerProcess
	);

	lib.Close();	// if successful, server thread has handle to library now
#else
	//
	// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt err = server.Create(KUsbmanImg, KNullDesC, serverUid);
#endif //__USBMAN_NO_PROCESSES__
	
	if (err != KErrNone)
		{
		return err;
		}

	TRequestStatus stat;
	server.Rendezvous(stat);
	
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server

	User::WaitForRequest(stat);		// wait for start or death

	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = (server.ExitType() == EExitPanic) ? KErrServerTerminated : stat.Int();

	server.Close();
	
	OstTrace1( TRACE_NORMAL, RUSB_STARTSERVER, "::StartServer;USB server started successfully: err=%d", err );
	

	return err;
	}




EXPORT_C RUsb::RUsb() 
	: iDeviceStatePkg(0), iServiceStatePkg(0), iMessagePkg(0), 
	  iHostPkg(TDeviceEventInformation())
	{
	OstTraceFunctionEntry0( RUSB_RUSB_CONS_ENTRY );
	OstTraceFunctionExit0( RUSB_RUSB_CONS_EXIT );
	}

EXPORT_C RUsb::~RUsb()
	{
    OstTraceFunctionEntry0( RUSB_RUSB_DES_ENTRY );
	OstTraceFunctionExit0( RUSB_RUSB_DES_EXIT );
	}

EXPORT_C TVersion RUsb::Version() const
	{
	return(TVersion(KUsbSrvMajorVersionNumber,KUsbSrvMinorVersionNumber,KUsbSrvBuildVersionNumber));
	}

EXPORT_C TInt RUsb::Connect()
	{
	OstTraceFunctionEntry0( RUSB_CONNECT_ENTRY );
	TInt retry = 2;
	
	FOREVER
		{
		// Create the session to UsbSrv with 10 asynchronous message slots
		TInt err = CreateSession(KUsbServerName, Version(), 10);

		if ((err != KErrNotFound) && (err != KErrServerTerminated))
			{
			OstTraceFunctionExit0( RUSB_CONNECT_EXIT );
			return err;
			}

		if (--retry == 0)
			{
			OstTraceFunctionExit0( RUSB_CONNECT_EXIT_DUP1 );
			return err;
			}

		err = StartServer();

		if ((err != KErrNone) && (err != KErrAlreadyExists))
			{
			OstTraceFunctionExit0( RUSB_CONNECT_EXIT_DUP2 );
			return err;
			}
		}
	}

EXPORT_C void RUsb::Start(TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( RUSB_START_ENTRY );
	
	SendReceive(EUsbStart, aStatus);
	OstTraceFunctionExit0( RUSB_START_EXIT );
	}

EXPORT_C void RUsb::StartCancel()
	{
	OstTraceFunctionEntry0( RUSB_STARTCANCEL_ENTRY );
	SendReceive(EUsbStartCancel);
	OstTraceFunctionExit0( RUSB_STARTCANCEL_EXIT );
	}

EXPORT_C void RUsb::Stop()
	{
	OstTraceFunctionEntry0( RUSB_STOP_ENTRY );
	SendReceive(EUsbStop);
	OstTraceFunctionExit0( RUSB_STOP_EXIT );
	}

EXPORT_C void RUsb::Stop(TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( RUSB_STOP_TREQUESTSTATUS_ENTRY );
	SendReceive(EUsbStop, aStatus);
	OstTraceFunctionExit0( RUSB_STOP_TREQUESTSTATUS_EXIT );
	}

EXPORT_C void RUsb::StopCancel()
	{
	OstTraceFunctionEntry0( RUSB_STOPCANCEL_ENTRY );
	SendReceive(EUsbStopCancel);
	OstTraceFunctionExit0( RUSB_STOPCANCEL_EXIT );
	}

EXPORT_C TInt RUsb::GetServiceState(TUsbServiceState& aState)
	{
	OstTraceFunctionEntry0( RUSB_GETSERVICESTATE_ENTRY );
	TPckg<TUint32> pkg(aState);
	TInt ret=SendReceive(EUsbGetCurrentState, TIpcArgs(&pkg));
	aState=(TUsbServiceState)pkg();
	OstTraceFunctionExit0( RUSB_GETSERVICESTATE_EXIT );
	return ret;
	}

EXPORT_C TInt RUsb::GetCurrentState(TUsbServiceState& aState)
	{
	OstTraceFunctionEntry0( RUSB_GETCURRENTSTATE_ENTRY );
	return GetServiceState(aState);
	}

EXPORT_C void RUsb::ServiceStateNotification(TUsbServiceState& aState,
	TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( RUSB_SERVICESTATENOTIFICATION_ENTRY );

	iServiceStatePkg.Set((TUint8*)&aState, sizeof(TUint32), sizeof(TUint32));

	SendReceive(EUsbRegisterServiceObserver, TIpcArgs(&iServiceStatePkg), aStatus);
	OstTraceFunctionExit0( RUSB_SERVICESTATENOTIFICATION_EXIT );
	}

EXPORT_C void RUsb::ServiceStateNotificationCancel()
	{
	OstTraceFunctionEntry0( RUSB_SERVICESTATENOTIFICATIONCANCEL_ENTRY );
	SendReceive(EUsbCancelServiceObserver);
	OstTraceFunctionExit0( RUSB_SERVICESTATENOTIFICATIONCANCEL_EXIT );
	}

EXPORT_C TInt RUsb::GetDeviceState(TUsbDeviceState& aState)
	{
	OstTraceFunctionEntry0( RUSB_GETDEVICESTATE_ENTRY );
	TPckg<TUint32> pkg(aState);
	TInt ret=SendReceive(EUsbGetCurrentDeviceState, TIpcArgs(&pkg));
	aState=(TUsbDeviceState)pkg();
	OstTraceFunctionExit0( RUSB_GETDEVICESTATE_EXIT );
	return ret;
	}

EXPORT_C void RUsb::DeviceStateNotification(TUint aEventMask, TUsbDeviceState& aState,
											TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( RUSB_DEVICESTATENOTIFICATION_ENTRY );
	iDeviceStatePkg.Set((TUint8*)&aState, sizeof(TUint32), sizeof(TUint32));

	SendReceive(EUsbRegisterObserver, TIpcArgs(aEventMask, &iDeviceStatePkg), aStatus);
	OstTraceFunctionExit0( RUSB_DEVICESTATENOTIFICATION_EXIT );
	}

EXPORT_C void RUsb::DeviceStateNotificationCancel()
	{
	OstTraceFunctionEntry0( RUSB_DEVICESTATENOTIFICATIONCANCEL_ENTRY );
	SendReceive(EUsbCancelObserver);
	OstTraceFunctionExit0( RUSB_DEVICESTATENOTIFICATIONCANCEL_EXIT );
	}

EXPORT_C void RUsb::StateNotification(TUint aEventMask, TUsbDeviceState& aState, TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( RUSB_STATENOTIFICATION_ENTRY );
	DeviceStateNotification(aEventMask, aState, aStatus);
	OstTraceFunctionExit0( RUSB_STATENOTIFICATION_EXIT );
	}

EXPORT_C void RUsb::StateNotificationCancel()
	{
	OstTraceFunctionEntry0( RUSB_STATENOTIFICATIONCANCEL_ENTRY );
	DeviceStateNotificationCancel();
	OstTraceFunctionExit0( RUSB_STATENOTIFICATIONCANCEL_EXIT );
	}
	
EXPORT_C void RUsb::TryStart(TInt aPersonalityId, TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( RUSB_TRYSTART_ENTRY );

	TIpcArgs ipcArgs(aPersonalityId);
	SendReceive(EUsbTryStart, ipcArgs, aStatus);
	OstTraceFunctionExit0( RUSB_TRYSTART_EXIT );
	}

EXPORT_C void RUsb::TryStop(TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0( RUSB_TRYSTOP_ENTRY );
	SendReceive(EUsbTryStop, aStatus);
	OstTraceFunctionExit0( RUSB_TRYSTOP_EXIT );
	}
	
EXPORT_C TInt RUsb::CancelInterest(TUsbReqType aMessageId)
	{
	OstTraceFunctionEntry0( RUSB_CANCELINTEREST_ENTRY );

	TInt messageId;
	switch (aMessageId)
		{
	case EStart:
		messageId = EUsbStart;
		break;
	case EStop:
		messageId = EUsbStop;
		break;
	case ETryStart:
		messageId = EUsbTryStart;
		break;
	case ETryStop:
		messageId = EUsbTryStop;
		break;
	default:
		return KErrNotSupported;
		}
		
	TIpcArgs ipcArgs(messageId);
	return SendReceive(EUsbCancelInterest, ipcArgs);
	}

EXPORT_C TInt RUsb::GetDescription(TInt aPersonalityId, HBufC*& aLocalizedPersonalityDescriptor)
	{
	OstTraceFunctionEntry0( RUSB_GETDESCRIPTION_ENTRY );
	TInt ret = KErrNone;
	// caller is responsible for freeing up memory allocatd for aLocalizedPersonalityDescriptor
	TRAP(ret, aLocalizedPersonalityDescriptor = HBufC::NewL(KUsbStringDescStringMaxSize));
	if (ret == KErrNone)
		{
		TPtr ptr = aLocalizedPersonalityDescriptor->Des();
		TIpcArgs ipcArgs(0, &ptr);
		ipcArgs.Set(0, aPersonalityId);
		ret = SendReceive(EUsbGetDescription, ipcArgs);
		}
	else
		{
		// just in case caller tries to free the memory before checking the return code
		aLocalizedPersonalityDescriptor = NULL;
		}

	OstTraceFunctionExit0( RUSB_GETDESCRIPTION_EXIT );
	return ret;	
	}
	
EXPORT_C TInt RUsb::GetCurrentPersonalityId(TInt& aPersonalityId)
	{
	OstTraceFunctionEntry0( RUSB_GETCURRENTPERSONALITYID_ENTRY );

	TPckg<TInt> pkg0(aPersonalityId);
	TInt ret = SendReceive(EUsbGetCurrentPersonalityId, TIpcArgs(&pkg0));
	aPersonalityId = static_cast<TInt>(pkg0());
	OstTraceFunctionExit0( RUSB_GETCURRENTPERSONALITYID_EXIT );
	return ret;	
	}

EXPORT_C TInt RUsb::GetSupportedClasses(TInt aPersonalityId, RArray<TUid>& aClassUids)
	{
	OstTraceFunctionEntry0( RUSB_GETSUPPORTEDCLASSES_ENTRY );
	TInt ret = KErrNone;
	HBufC8* buf = NULL;
	// +1 for the actual count of personality ids
	TRAP(ret, buf = HBufC8::NewL((KUsbMaxSupportedClasses + 1)*sizeof (TInt32)));
	if (ret != KErrNone)
		{
		OstTraceFunctionExit0( RUSB_GETSUPPORTEDCLASSES_EXIT );
		return ret;
		}

	TPtr8 ptr8 = buf->Des();
	ret = SendReceive(EUsbGetSupportedClasses, TIpcArgs(aPersonalityId, &ptr8));
		
	if (ret == KErrNone)
		{
		const TInt32* recvedIds = reinterpret_cast<const TInt32*>(buf->Ptr());
		if (!recvedIds)
			{
			delete buf;
			OstTraceFunctionExit0( RUSB_GETSUPPORTEDCLASSES_EXIT_DUP1 );
			return KErrCorrupt;
			}
			
		TInt arraySize = *recvedIds++;
		// Copy received supported class ids to aClassUids
		for (TInt i = 0; i < arraySize; i++)
			{
			if (recvedIds)
				{
				ret = aClassUids.Append(TUid::Uid(*recvedIds++));
				if(ret!=KErrNone)
					{
					//Remove all the ids appended so far (assume the last append failed, because
					//the only reason to fail is if the array couldn't grow to accommodate another
					//element).
					//It would be easier to just reset the array, but we never specified that
					//aClassUids should be an empty array, nor did we specify that this method
					//might empty the array. To maintain exisiting behaviour we should return
					//aClassUids to the state it was in when this method was called.
					TInt last = aClassUids.Count() - 1;
					while(i>0)
						{
						aClassUids.Remove(last);
						i--;
						last--;
						}	
					break;
					}
				}
			else
				{
				ret = KErrCorrupt;
				break;
				}
			}
		}
		
	delete buf;
	OstTraceFunctionExit0( RUSB_GETSUPPORTEDCLASSES_EXIT_DUP2 );
	return ret;
	}
	
EXPORT_C TInt RUsb::ClassSupported(TInt aPersonalityId, TUid aClassUid, TBool& aSupported)
	{
	OstTraceFunctionEntry0( RUSB_CLASSSUPPORTED_ENTRY );

	TPckg<TInt32>  	pkg2(aSupported);
	TIpcArgs ipcArgs(aPersonalityId, aClassUid.iUid, &pkg2);
	
	TInt ret = SendReceive(EUsbClassSupported, ipcArgs);
	
	if (ret == KErrNone)
		{
		aSupported = static_cast<TBool>(pkg2());		
		}
		
	OstTraceFunctionExit0( RUSB_CLASSSUPPORTED_EXIT );
	return ret;
	}
	
EXPORT_C TInt RUsb::GetPersonalityIds(RArray<TInt>& aPersonalityIds)
	{
	OstTraceFunctionEntry0( RUSB_GETPERSONALITYIDS_ENTRY );
	TInt ret = KErrNone;
	HBufC8* buf = NULL;
	// +1 for the actual count of personality ids
	TRAP(ret, buf = HBufC8::NewL((KUsbMaxSupportedPersonalities + 1)*sizeof (TInt)));
	if (ret != KErrNone)
		{
		OstTraceFunctionExit0( RUSB_GETPERSONALITYIDS_EXIT );
		return ret;
		}

	TPtr8 ptr8 = buf->Des();
	ret = SendReceive(EUsbGetPersonalityIds, TIpcArgs(&ptr8));
		
	if (ret == KErrNone)
		{
		const TInt* recvedIds = reinterpret_cast<const TInt*>(buf->Ptr());
		if (!recvedIds)
			{
			delete buf;
			OstTraceFunctionExit0( RUSB_GETPERSONALITYIDS_EXIT_DUP1 );
			return KErrCorrupt;
			}
			
		TInt arraySize = *recvedIds++;
		// Copy received personality ids to aPersonalityIds
		for (TInt i = 0; i < arraySize; i++)
			{
			if (recvedIds)
				{		
				ret = aPersonalityIds.Append(*recvedIds++);
				
				if(ret!=KErrNone)
					{
					//Remove all the ids appended so far (assume the last append failed, because
					//the only reason to fail is if the array couldn't grow to accommodate another
					//element).
					//It would be easier to just reset the array, but we never specified that
					//aPersonalityIds should be an empty array, nor did we specify that this method
					//might empty the array. To maintain exisiting behaviour we should return
					//aPersonalityIds to the state it was in when this method was called.
					TInt last = aPersonalityIds.Count() - 1;
					while(i>0)
						{
						aPersonalityIds.Remove(last);
						i--;
						last--;
						}	
					break;
					}
				}
			else
				{
				ret = KErrCorrupt;
				break;
				}
			}
		}
		
	delete buf;
	OstTraceFunctionExit0( RUSB_GETPERSONALITYIDS_EXIT_DUP2 );
	return ret;
	}
	
EXPORT_C TInt RUsb::__DbgMarkHeap()
	{
#ifdef _DEBUG
    return SendReceive(EUsbDbgMarkHeap);
#else
    return KErrNone;
#endif
	}

EXPORT_C TInt RUsb::__DbgCheckHeap(TInt aCount)
	{
#ifdef _DEBUG
    return SendReceive(EUsbDbgCheckHeap, TIpcArgs(aCount));
#else
	(void)aCount; // not used for Release builds
    return KErrNone;
#endif
	}

EXPORT_C TInt RUsb::__DbgMarkEnd(TInt aCount)
	{
#ifdef _DEBUG
    return SendReceive(EUsbDbgMarkEnd, TIpcArgs(aCount));
#else
	(void)aCount; // not used for Release builds
    return KErrNone;
#endif
	}

EXPORT_C TInt RUsb::__DbgFailNext(TInt aCount)
	{
#ifdef _DEBUG
    return SendReceive(EUsbDbgFailNext, TIpcArgs(aCount));
#else
	(void)aCount; // not used for Release builds
    return KErrNone;
#endif
	}

EXPORT_C TInt RUsb::__DbgAlloc()
	{
#ifdef _DEBUG
    return SendReceive(EUsbDbgAlloc);
#else
    return KErrNone;
#endif
	}

EXPORT_C void panic()
    { 
    OstTrace1( TRACE_FATAL, RUSB_PANIC, "::panic;Panic reason=%d", EUsbPanicRemovedExport );
    User::Panic(KUsbCliPncCat, EUsbPanicRemovedExport);
	}

EXPORT_C TInt RUsb::SetCtlSessionMode(TBool aValue)
	{
	OstTraceFunctionEntry0( RUSB_SETCTLSESSIONMODE_ENTRY );

	TPckg<TBool> pkg(aValue);
	return SendReceive(EUsbSetCtlSessionMode, TIpcArgs(&pkg));
	}

EXPORT_C TInt RUsb::BusRequest()
	{
	OstTraceFunctionEntry0( RUSB_BUSREQUEST_ENTRY );

	return SendReceive(EUsbBusRequest);
	}

EXPORT_C TInt RUsb::BusRespondSrp()
	{
	OstTraceFunctionEntry0( RUSB_BUSRESPONDSRP_ENTRY );

	return SendReceive(EUsbBusRespondSrp);
	}

EXPORT_C TInt RUsb::BusClearError()
	{
	OstTraceFunctionEntry0( RUSB_BUSCLEARERROR_ENTRY );
	return SendReceive(EUsbBusClearError);
	}


EXPORT_C TInt RUsb::BusDrop()
	{
	OstTraceFunctionEntry0( RUSB_BUSDROP_ENTRY );
	return SendReceive(EUsbBusDrop);
	}

EXPORT_C void RUsb::MessageNotification(TRequestStatus& aStatus, TInt& aMessage)
	{
	OstTraceFunctionEntry0( RUSB_MESSAGENOTIFICATION_ENTRY );
	iMessagePkg.Set((TUint8*)&aMessage, sizeof(TInt), sizeof(TInt));

	SendReceive(EUsbRegisterMessageObserver, TIpcArgs(&iMessagePkg), aStatus);
	OstTraceFunctionExit0( RUSB_MESSAGENOTIFICATION_EXIT );
	}

EXPORT_C void RUsb::MessageNotificationCancel()
	{
	OstTraceFunctionEntry0( RUSB_MESSAGENOTIFICATIONCANCEL_ENTRY );
	SendReceive(EUsbCancelMessageObserver);
	OstTraceFunctionExit0( RUSB_MESSAGENOTIFICATIONCANCEL_EXIT );
	}

EXPORT_C void RUsb::HostEventNotification(TRequestStatus& aStatus,
										  TDeviceEventInformation& aDeviceInformation)
	{
	OstTraceFunctionEntry0( RUSB_HOSTEVENTNOTIFICATION_ENTRY );

	iHostPkg.Set((TUint8*)&aDeviceInformation, sizeof(TDeviceEventInformation), sizeof(TDeviceEventInformation));

	SendReceive(EUsbRegisterHostObserver, TIpcArgs(&iHostPkg), aStatus);
	OstTraceFunctionExit0( RUSB_HOSTEVENTNOTIFICATION_EXIT );
	}
	
EXPORT_C void RUsb::HostEventNotificationCancel()
	{
	OstTraceFunctionEntry0( RUSB_HOSTEVENTNOTIFICATIONCANCEL_ENTRY );
	SendReceive(EUsbCancelHostObserver);
	OstTraceFunctionExit0( RUSB_HOSTEVENTNOTIFICATIONCANCEL_EXIT );
	}

EXPORT_C TInt RUsb::EnableFunctionDriverLoading()
	{
	OstTraceFunctionEntry0( RUSB_ENABLEFUNCTIONDRIVERLOADING_ENTRY );

	return SendReceive(EUsbEnableFunctionDriverLoading);
	}

EXPORT_C void RUsb::DisableFunctionDriverLoading()
	{
	OstTraceFunctionEntry0( RUSB_DISABLEFUNCTIONDRIVERLOADING_ENTRY );
	SendReceive(EUsbDisableFunctionDriverLoading);
	OstTraceFunctionExit0( RUSB_DISABLEFUNCTIONDRIVERLOADING_EXIT );
	}

EXPORT_C TInt RUsb::GetSupportedLanguages(TUint aDeviceId, RArray<TUint>& aLangIds)
	{
	OstTraceFunctionEntry0( RUSB_GETSUPPORTEDLANGUAGES_ENTRY );

	aLangIds.Reset();

	TInt ret = KErrNone;
	HBufC8* buf = NULL;
	// +1 for the actual count of language ids
	TRAP(ret, buf = HBufC8::NewL((KUsbMaxSupportedLanguageIds + 1)*sizeof (TUint)));
	if (ret != KErrNone)
		{
		OstTraceFunctionExit0( RUSB_GETSUPPORTEDLANGUAGES_EXIT );
		return ret;
		}

	TPtr8 ptr8 = buf->Des();
	ret = SendReceive(EUsbGetSupportedLanguages, TIpcArgs(aDeviceId, &ptr8));
		
	if (ret == KErrNone)
		{
		const TUint* recvedIds = reinterpret_cast<const TUint*>(buf->Ptr());
		if (!recvedIds)
			{
			delete buf;
			OstTraceFunctionExit0( RUSB_GETSUPPORTEDLANGUAGES_EXIT_DUP1 );
			return KErrCorrupt;
			}
			
		TInt arraySize = *recvedIds++;
		// Copy received language ids to aLangIds
		for (TInt i = 0; i < arraySize; i++)
			{
			ret = aLangIds.Append(*recvedIds++); // increments by sizeof(TUint)
			if ( ret )
				{
				aLangIds.Reset();
				break;
				}
			}
		}
		
	delete buf;	
	OstTraceFunctionExit0( RUSB_GETSUPPORTEDLANGUAGES_EXIT_DUP2 );
	return ret;
	}
	
EXPORT_C TInt RUsb::GetManufacturerStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
	OstTraceFunctionEntry0( RUSB_GETMANUFACTURERSTRINGDESCRIPTOR_ENTRY );

	return SendReceive(EUsbGetManufacturerStringDescriptor, TIpcArgs(aDeviceId, aLangId, &aString));
	}

EXPORT_C TInt RUsb::GetProductStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
	OstTraceFunctionEntry0( RUSB_GETPRODUCTSTRINGDESCRIPTOR_ENTRY );
	return SendReceive(EUsbGetProductStringDescriptor, TIpcArgs(aDeviceId, aLangId, &aString));
	}

EXPORT_C TInt RUsb::GetOtgDescriptor(TUint aDeviceId, TOtgDescriptor& aDescriptor)
	{
	OstTraceFunctionEntry0( RUSB_GETOTGDESCRIPTOR_ENTRY );
	TPckg<TOtgDescriptor> otgDescPkg(aDescriptor);
	
	TIpcArgs args;
	args.Set(0, aDeviceId);
	args.Set(1, &otgDescPkg);

	return SendReceive(EUsbGetOtgDescriptor, args);
	}


EXPORT_C TInt RUsb::RequestSession()
	{
	OstTraceFunctionEntry0( RUSB_REQUESTSESSION_ENTRY );

	return SendReceive(EUsbRequestSession);
	}

EXPORT_C TInt RUsb::GetDetailedDescription(TInt /*aPersonalityId*/, HBufC*& /*aLocalizedPersonalityDescriptor*/)
	{
	OstTraceFunctionEntry0( RUSB_GETDETAILEDDESCRIPTION_ENTRY );
	//This API has been deprecated
	OstTraceFunctionExit0( RUSB_GETDETAILEDDESCRIPTION_EXIT );
	return KErrNotSupported; 
	}

EXPORT_C TInt RUsb::GetPersonalityProperty(TInt aPersonalityId, TUint32& aProperty)
	{
	OstTraceFunctionEntry0( RUSB_GETPERSONALITYPROPERTY_ENTRY );

	TPckg<TUint32> pkg(aProperty);
	TInt ret = SendReceive(EUsbGetPersonalityProperty, TIpcArgs(aPersonalityId, &pkg));
	if (ret == KErrNone)
		{
		aProperty = static_cast<TUint32>(pkg());
		}
	OstTraceFunctionExit0( RUSB_GETPERSONALITYPROPERTY_EXIT );
	return ret;	
	}

