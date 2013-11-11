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

#include "fdfsession.h"
#include "fdfserver.h"
#include <usb/usblogger.h>
#include "utils.h"
#include <ecom/ecom.h>
#include "fdfapi.h"
#include "fdf.h"
#include "event.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "fdfsessionTraces.h"
#endif

#ifdef _DEBUG
_LIT(KPanicCategory, "fdfsession");
#endif

CFdfSession::CFdfSession(CFdf& aFdf, CFdfServer& aServer)
 :	iFdf(aFdf),
	iServer(aServer)
	{
	OstTraceFunctionEntry0( CFDFSESSION_CFDFSESSION_CONS_ENTRY );
	
	OstTraceFunctionExit0( CFDFSESSION_CFDFSESSION_CONS_EXIT );
	}

CFdfSession::~CFdfSession()
	{
	OstTraceFunctionEntry0( CFDFSESSION_CFDFSESSION_DES_ENTRY );
	
	iServer.SessionClosed();
	OstTraceFunctionExit0( CFDFSESSION_CFDFSESSION_DES_EXIT );
	}

void CFdfSession::ServiceL(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_SERVICEL_ENTRY );
	OstTrace1( TRACE_NORMAL, CFDFSESSION_SERVICEL, "aMessage.Function() = %d", aMessage.Function() );
	
	// Switch on the IPC number and call a 'message handler'. Message handlers
	// complete aMessage (either with Complete or Panic), or make a note of
	// the message for later asynchronous completion.
	// Message handlers should not leave- the server does not have an Error
	// function.

	switch ( aMessage.Function() )
		{
	case EUsbFdfSrvEnableDriverLoading:
		EnableDriverLoading(aMessage);
		// This is a sync API- check that the message has been completed.
		// (NB We don't check the converse for async APIs because the message
		// may have been panicked synchronously.)
		if(!(aMessage.Handle() == 0))
		    {
            OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP1, 
                        "Empty aMessage.Handle" );
            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		break;

	case EUsbFdfSrvDisableDriverLoading:
		DisableDriverLoading(aMessage);
		if(!(aMessage.Handle() == 0))
		    {
		    OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP2, 
		              "Empty aMessage.Handle" );
		    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		break;

	case EUsbFdfSrvNotifyDeviceEvent:
		NotifyDeviceEvent(aMessage);
		break;

	case EUsbFdfSrvNotifyDeviceEventCancel:
		NotifyDeviceEventCancel(aMessage);
		if(!(aMessage.Handle() == 0))
		    {
		    OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP3, 
                        "Empty aMessage.Handle" );
		    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		break;

	case EUsbFdfSrvNotifyDevmonEvent:
		NotifyDevmonEvent(aMessage);
		break;

	case EUsbFdfSrvNotifyDevmonEventCancel:
		NotifyDevmonEventCancel(aMessage);
		if(!(aMessage.Handle() == 0))
		    {
		    OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP4, 
		              "Empty aMessage.Handle" );
		    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		break;

	case EUsbFdfSrvGetSingleSupportedLanguageOrNumberOfSupportedLanguages:
		GetSingleSupportedLanguageOrNumberOfSupportedLanguages(aMessage);
		if(!(aMessage.Handle() == 0))
		    {
		    OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP5, 
		               "Empty aMessage.Handle" );
		    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		break;

	case EUsbFdfSrvGetSupportedLanguages:
		GetSupportedLanguages(aMessage);
		if(!(aMessage.Handle() == 0))
		    {
		    OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP6, 
		              "Empty aMessage.Handle" );
		    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		break;

	case EUsbFdfSrvGetManufacturerStringDescriptor:
		GetManufacturerStringDescriptor(aMessage);
		if(!(aMessage.Handle() == 0))
		    {
		    OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP7, 
		              "Empty aMessage.Handle" );
		    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }		
		break;

	case EUsbFdfSrvGetProductStringDescriptor:
		GetProductStringDescriptor(aMessage);
		if(!(aMessage.Handle() == 0))
		    {
		    OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP8, 
		              "Empty aMessage.Handle" );
		    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		break;
		
	case EUsbFdfSrvGetOtgDescriptor:
		GetOtgDeviceDescriptor(aMessage);
		if(!(aMessage.Handle() == 0))
		    {
		    OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP9, 
		              "Empty aMessage.Handle" );
		    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		break;

	// Heap failure testing APIs.
	case EUsbFdfSrvDbgFailNext:
#ifdef _DEBUG
		{
		OstTrace1( TRACE_NORMAL, CFDFSESSION_SERVICEL_DUP10, 
		                        "fail next (simulating failure after %d allocation(s))", 
		                        aMessage.Int0() );
		if ( aMessage.Int0() == 0 )
			{
			__UHEAP_RESET;
			}
		else
			{
			__UHEAP_FAILNEXT(aMessage.Int0());
			}
		}
#endif
		CompleteClient(aMessage, KErrNone);
		break;

	case EUsbFdfSrvDbgAlloc:
		{
		TInt err = KErrNone;
#ifdef _DEBUG
		OstTrace0( TRACE_NORMAL, CFDFSESSION_SERVICEL_DUP11, "allocate on the heap" );
		TInt* x = NULL;
		TRAP(err, x = new(ELeave) TInt);
		delete x;
#endif
		CompleteClient(aMessage, err);
		}
		break;

	default:
		OstTrace0( TRACE_FATAL, CFDFSESSION_SERVICEL_DUP12, 
		           "Invalid message function" );
		aMessage.Panic(KUsbFdfServerName,EBadIpc);
        
		break;
		}
	OstTraceFunctionExit0( CFDFSESSION_SERVICEL_EXIT );
	}

void CFdfSession::CompleteClient(const RMessage2& aMessage, TInt aError)
	{
    OstTrace1( TRACE_NORMAL, CFDFSESSION_COMPLETECLIENT, "completing client message with %d", aError );
 	aMessage.Complete(aError);
	}

void CFdfSession::EnableDriverLoading(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_ENABLEDRIVERLOADING_ENTRY );

	iFdf.EnableDriverLoading();

	CompleteClient(aMessage, KErrNone);
	OstTraceFunctionExit0( CFDFSESSION_ENABLEDRIVERLOADING_EXIT );
	}

void CFdfSession::DisableDriverLoading(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_DISABLEDRIVERLOADING_ENTRY );
	
	iFdf.DisableDriverLoading();

	CompleteClient(aMessage, KErrNone);
	OstTraceFunctionExit0( CFDFSESSION_DISABLEDRIVERLOADING_EXIT );
	}

TBool CFdfSession::NotifyDeviceEventOutstanding() const
	{
	const TBool ret = ( iNotifyDeviceEventMsg.Handle() != 0 );
	OstTrace1( TRACE_NORMAL, CFDFSESSION_NOTIFYDEVICEEVENTOUTSTANDING, 
	        "CFdfSession::NotifyDeviceEventOutstanding returning %d", ret );
	
	return ret;
	}

void CFdfSession::NotifyDeviceEvent(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_NOTIFYDEVICEEVENT_ENTRY );
	
	if ( iNotifyDeviceEventMsg.Handle() )
		{
        OstTrace0( TRACE_FATAL, CFDFSESSION_NOTIFYDEVICEEVENT, 
                "Empty iNotifyDeviceEventMsg" );
		iNotifyDeviceEventMsg.Panic( KUsbFdfServerName, 
		        ENotifyDeviceEventAlreadyOutstanding);
		}
	else
		{
		iNotifyDeviceEventMsg = aMessage;
		TDeviceEvent event;
		if ( iFdf.GetDeviceEvent(event) )
			{
			CompleteDeviceEventNotification(event);
			}
		}
	OstTraceFunctionExit0( CFDFSESSION_NOTIFYDEVICEEVENT_EXIT );
	}

void CFdfSession::NotifyDeviceEventCancel(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_NOTIFYDEVICEEVENTCANCEL_ENTRY );

	if ( iNotifyDeviceEventMsg.Handle() )
		{
		CompleteClient(iNotifyDeviceEventMsg, KErrCancel);
		}
	CompleteClient(aMessage, KErrNone);
	OstTraceFunctionExit0( CFDFSESSION_NOTIFYDEVICEEVENTCANCEL_EXIT );
	}

void CFdfSession::DeviceEvent(const TDeviceEvent& aEvent)
	{
	OstTraceFunctionEntry0( CFDFSESSION_DEVICEEVENT_ENTRY );

	// This function should only be called if there is a request outstanding.
	if(!iNotifyDeviceEventMsg.Handle())
	    {
	    OstTrace0( TRACE_FATAL, CFDFSESSION_DEVICEEVENT, 
	                "iNotifyDeviceEventMsg.Handle" );
	    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }
	CompleteDeviceEventNotification(aEvent);
	OstTraceFunctionExit0( CFDFSESSION_DEVICEEVENT_EXIT );
	}

void CFdfSession::CompleteDeviceEventNotification(const TDeviceEvent& aEvent)
	{
	OstTraceFunctionEntry0( CFDFSESSION_COMPLETEDEVICEEVENTNOTIFICATION_ENTRY );
	

	TRAPD(err, CompleteDeviceEventNotificationL(aEvent));
	if ( err )
		{
        OstTrace0( TRACE_FATAL, CFDFSESSION_COMPLETEDEVICEEVENTNOTIFICATION, 
                "CompleteDeviceEventNotificationL error" );
		iNotifyDeviceEventMsg.Panic( KUsbFdfServerName, 
		        EBadNotifyDeviceEventData);
		}
	OstTraceFunctionExit0( CFDFSESSION_COMPLETEDEVICEEVENTNOTIFICATION_EXIT );
	}

void CFdfSession::CompleteDeviceEventNotificationL(const TDeviceEvent& aEvent)
	{
    OstTraceFunctionEntry0( CFDFSESSION_COMPLETEDEVICEEVENTNOTIFICATIONL_ENTRY );
	
	// iNotifyDeviceEventMsg has one IPC arg: a TDeviceEventInformation
	if(!iNotifyDeviceEventMsg.Handle())
	    {
	    OstTrace0( TRACE_FATAL, CFDFSESSION_COMPLETEDEVICEEVENTNOTIFICATIONL, 
            "Empty iNotifyDeviceEventMsg.Handle" );
	    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	TPckg<TDeviceEventInformation> info(aEvent.iInfo);
	iNotifyDeviceEventMsg.WriteL(0, info);

	CompleteClient(iNotifyDeviceEventMsg, KErrNone);
	OstTraceFunctionExit0( CFDFSESSION_COMPLETEDEVICEEVENTNOTIFICATIONL_EXIT );
	}

TBool CFdfSession::NotifyDevmonEventOutstanding() const
	{
	const TBool ret = ( iNotifyDevmonEventMsg.Handle() != 0 );
	OstTrace1( TRACE_NORMAL, CFDFSESSION_NOTIFYDEVMONEVENTOUTSTANDING, 
	        "CFdfSession::NotifyDevmonEventOutstanding returning %d", ret );
	return ret;
	}

void CFdfSession::NotifyDevmonEvent(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_NOTIFYDEVMONEVENT_ENTRY );
	
	if ( iNotifyDevmonEventMsg.Handle() )
		{
        OstTrace0( TRACE_FATAL, CFDFSESSION_NOTIFYDEVMONEVENT, 
	              "iNotifyDevmonEventMsg.Handle should empty" );
		iNotifyDevmonEventMsg.Panic( KUsbFdfServerName, 
		        ENotifyDevmonEventAlreadyOutstanding);
		        
		}
	else
		{
		iNotifyDevmonEventMsg = aMessage;
		TInt event;
		if ( iFdf.GetDevmonEvent(event) )
			{
			CompleteDevmonEventNotification(event);
			}
		}
	OstTraceFunctionExit0( CFDFSESSION_NOTIFYDEVMONEVENT_EXIT );
	}

void CFdfSession::NotifyDevmonEventCancel(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_NOTIFYDEVMONEVENTCANCEL_ENTRY );
	

	if ( iNotifyDevmonEventMsg.Handle() )
		{
		CompleteClient(iNotifyDevmonEventMsg, KErrCancel);
		}
	CompleteClient(aMessage, KErrNone);
	OstTraceFunctionExit0( CFDFSESSION_NOTIFYDEVMONEVENTCANCEL_EXIT );
	}

void CFdfSession::DevmonEvent(TInt aError)
	{
	OstTraceFunctionEntry0( CFDFSESSION_DEVMONEVENT_ENTRY );
	
	// This function should only be called if there is a request outstanding.
	
	
	if(!iNotifyDevmonEventMsg.Handle())
	    {
	    OstTrace0( TRACE_FATAL, CFDFSESSION_DEVMONEVENT, 
	                "Empty iNotifyDevmonEventMsg.Handle()" );
	    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	CompleteDevmonEventNotification(aError);
	OstTraceFunctionExit0( CFDFSESSION_DEVMONEVENT_EXIT );
	}

void CFdfSession::CompleteDevmonEventNotification(TInt aError)
	{
	OstTraceFunctionEntry0( CFDFSESSION_COMPLETEDEVMONEVENTNOTIFICATION_ENTRY );
	

	TRAPD(err, CompleteDevmonEventNotificationL(aError));
	if ( err )
		{
        OstTrace0( TRACE_FATAL, CFDFSESSION_COMPLETEDEVMONEVENTNOTIFICATION, 
	              "Bad Notify Devmon Event Data" );
		iNotifyDevmonEventMsg.Panic(KUsbFdfServerName, 
		        EBadNotifyDevmonEventData);
		}
	OstTraceFunctionExit0( CFDFSESSION_COMPLETEDEVMONEVENTNOTIFICATION_EXIT );
	}

void CFdfSession::CompleteDevmonEventNotificationL(TInt aEvent)
	{
	OstTraceFunctionEntry0( CFDFSESSION_COMPLETEDEVMONEVENTNOTIFICATIONL_ENTRY );
	
	// iNotifyDevmonEventMsg has the following IPC args:
	// 0- TInt& aError
	if(!iNotifyDevmonEventMsg.Handle())
	    {
	    OstTrace0( TRACE_FATAL, CFDFSESSION_COMPLETEDEVMONEVENTNOTIFICATIONL, 
	              "Empty iNotifyDevmonEventMsg.Handle" );
	    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	TPckg<TInt> event(aEvent);
	iNotifyDevmonEventMsg.WriteL(0, event);

	CompleteClient(iNotifyDevmonEventMsg, KErrNone);
	OstTraceFunctionExit0( CFDFSESSION_COMPLETEDEVMONEVENTNOTIFICATIONL_EXIT );
	}

void CFdfSession::GetSingleSupportedLanguageOrNumberOfSupportedLanguages(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGES_ENTRY );
	
	// To save IPC operations between client and server, we make use of the
	// fact that the majority of devices only support a single language.
	// The client is expected to have a buffer big enough to hold a single
	// TUint.
	// If the device supports 0 languages, the buffer is left empty and the
	// request is completed with KErrNotFound.
	// If the device supports 1 language, the language ID is put in the buffer
	// and the request is completed with KErrNone.
	// If the device supports more than 1 language, the number of languages is
	// put in the buffer, and the request is completed with
	// KErrTooBig. The client then allocates a buffer big enough to hold the
	// supported languages and uses EUsbFdfSrvGetSupportedLanguages to get
	// them all.
	TRAPD(err, GetSingleSupportedLanguageOrNumberOfSupportedLanguagesL(aMessage));
	CompleteClient(aMessage, err);
	OstTraceFunctionExit0( CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGES_EXIT );
	}

void CFdfSession::GetSingleSupportedLanguageOrNumberOfSupportedLanguagesL(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGESL_ENTRY );
	
	const TUint deviceId = aMessage.Int0();
	OstTrace1( TRACE_NORMAL, CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGESL, 
	        "deviceId = %d", deviceId );
	const RArray<TUint>& langIds = iFdf.GetSupportedLanguagesL(deviceId);
	const TUint count = langIds.Count();
	OstTrace1( TRACE_NORMAL, CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGESL_DUP1, 
	        "count = %d", count );
	switch ( count )
		{
	case 0:
		// Nothing to write to the client's address space, complete with
	    OstTrace0( TRACE_NORMAL, CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGESL_DUP2, 
                "Nothing to write to the client's address space" );
		User::Leave(KErrNotFound);
	    break;

	case 1:
		{
		// Write the single supported language to the client, complete with
		// KErrNone (or error of course, if their buffer isn't big enough).
		TPckg<TUint> buf(langIds[0]);
		TInt err=aMessage.Write(1, buf);
		LEAVEIFERRORL(err,OstTrace1( TRACE_ERROR, CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGESL_DUP3, 
                "aMessage.Write with err: %d", err ););
		}
		break;

	default:
		{
		// Write the number of supported languages to the client, complete
		// with KErrTooBig (or error if their buffer wasn't big enough). NB
		// This is the point at which this mechanism depends on
		// RMessagePtr2::WriteL itself not leaving with KErrTooBig!
		TPckg<TUint> buf(count);
		TInt error=aMessage.Write(1, buf);
		
		LEAVEIFERRORL(error, OstTrace1( TRACE_ERROR, CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGESL_DUP4, 
                "aMessage.Write with err: %d", error ););
		OstTrace0( TRACE_ERROR, CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGESL_DUP5, 
                "message error too big" );
		User::Leave(KErrTooBig);
		}
		break;
		}
	OstTraceFunctionExit0( CFDFSESSION_GETSINGLESUPPORTEDLANGUAGEORNUMBEROFSUPPORTEDLANGUAGESL_EXIT );
	}

void CFdfSession::GetSupportedLanguages(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_GETSUPPORTEDLANGUAGES_ENTRY );
	
	TRAPD(err, GetSupportedLanguagesL(aMessage));
	CompleteClient(aMessage, err);
	OstTraceFunctionExit0( CFDFSESSION_GETSUPPORTEDLANGUAGES_EXIT );
	}

void CFdfSession::GetSupportedLanguagesL(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_GETSUPPORTEDLANGUAGESL_ENTRY );
	
	const TUint deviceId = aMessage.Int0();
	OstTrace1( TRACE_NORMAL, CFDFSESSION_GETSUPPORTEDLANGUAGESL, 
	        "deviceId = %d", deviceId );
	const RArray<TUint>& langIds = iFdf.GetSupportedLanguagesL(deviceId);

	const TUint count = langIds.Count();
	OstTrace1( TRACE_NORMAL, CFDFSESSION_GETSUPPORTEDLANGUAGESL_DUP1, 
	        "count = %d", count );
	RBuf8 buf;
	buf.CreateL(count * sizeof(TUint));
	CleanupClosePushL(buf);
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		buf.Append((TUint8*)&(langIds[ii]), sizeof(TUint));
		}

	// Write back to the client.
	TInt err=aMessage.Write(1, buf);
	LEAVEIFERRORL(err,OstTrace1( TRACE_ERROR, CFDFSESSION_GETSUPPORTEDLANGUAGESL_DUP2, 
            "aMessage.Write with error: %d", err ););
	CleanupStack::PopAndDestroy(&buf);
	OstTraceFunctionExit0( CFDFSESSION_GETSUPPORTEDLANGUAGESL_EXIT );
	}

void CFdfSession::GetManufacturerStringDescriptor(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_GETMANUFACTURERSTRINGDESCRIPTOR_ENTRY );
	

	GetStringDescriptor(aMessage, EManufacturer);
	OstTraceFunctionExit0( CFDFSESSION_GETMANUFACTURERSTRINGDESCRIPTOR_EXIT );
	}

void CFdfSession::GetProductStringDescriptor(const RMessage2& aMessage)
	{
    OstTraceFunctionEntry0( CFDFSESSION_GETPRODUCTSTRINGDESCRIPTOR_ENTRY );
    

	GetStringDescriptor(aMessage, EProduct);
	OstTraceFunctionExit0( CFDFSESSION_GETPRODUCTSTRINGDESCRIPTOR_EXIT );
	}

void CFdfSession::GetStringDescriptor(const RMessage2& aMessage, TStringType aStringType)
	{
	OstTraceFunctionEntry0( CFDFSESSION_GETSTRINGDESCRIPTOR_ENTRY );
	
	TRAPD(err, GetStringDescriptorL(aMessage, aStringType));
	CompleteClient(aMessage, err);
	OstTraceFunctionExit0( CFDFSESSION_GETSTRINGDESCRIPTOR_EXIT );
	}

void CFdfSession::GetStringDescriptorL(const RMessage2& aMessage, TStringType aStringType)
	{
	OstTraceFunctionEntry0( CFDFSESSION_GETSTRINGDESCRIPTORL_ENTRY );
	
	if(!(aStringType == EManufacturer || aStringType == EProduct))
	    {
	    OstTrace0( TRACE_FATAL, CFDFSESSION_GETSTRINGDESCRIPTORL, 
	                    "String Type error" );
	    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }
	TName string;
	const TUint deviceId = aMessage.Int0();
	const TUint langId = aMessage.Int1();
	if ( aStringType == EManufacturer )
		{
		iFdf.GetManufacturerStringDescriptorL(deviceId, langId, string);
		}
	else
		{
		iFdf.GetProductStringDescriptorL(deviceId, langId, string);
		}
	OstTraceExt1( TRACE_NORMAL, CFDFSESSION_GETSTRINGDESCRIPTORL_DUP1, 
	        "string = \"%S\"", string );

	TInt err=aMessage.Write(2, string);
	LEAVEIFERRORL(err,OstTrace1( TRACE_ERROR, CFDFSESSION_GETSTRINGDESCRIPTORL_DUP2, 
	        "Message write with err: %d", err ););
	OstTraceFunctionExit0( CFDFSESSION_GETSTRINGDESCRIPTORL_EXIT );
	}

void CFdfSession::GetOtgDeviceDescriptor(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CFDFSESSION_GETOTGDEVICEDESCRIPTOR_ENTRY );
	
	TOtgDescriptor otgDesc;
	const TUint deviceId = aMessage.Int0();
	TRAPD(err, iFdf.GetOtgDeviceDescriptorL(deviceId, otgDesc));	
	if (KErrNone == err)
		{
		TPckg<TOtgDescriptor> buf(otgDesc);
		err = aMessage.Write(1, buf);
		}
	CompleteClient(aMessage, err);
	OstTraceFunctionExit0( CFDFSESSION_GETOTGDEVICEDESCRIPTOR_EXIT );
	}
