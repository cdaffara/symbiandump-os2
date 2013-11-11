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


#include <usb/usblogger.h>
#include <usbhostdefs.h>
#include "utils.h"
#include "event.h"
#include "deviceproxy.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "deviceproxyTraces.h"
#endif

#ifdef _DEBUG
#define LOG Log()
_LIT(KPanicCategory,"devproxy");
#else
#define LOG
#endif



CDeviceProxy* CDeviceProxy::NewL(RUsbHubDriver& aHubDriver, TUint aDeviceId)
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_NEWL_ENTRY );
    
	CDeviceProxy* self = new(ELeave) CDeviceProxy(aDeviceId);
	CleanupStack::PushL(self);
	self->ConstructL(aHubDriver);
	CLEANUPSTACK_POP1(self);
	OstTraceFunctionExit0( CDEVICEPROXY_NEWL_EXIT );
	return self;
	}

CDeviceProxy::CDeviceProxy(TUint aDeviceId)
:	iId(aDeviceId)
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_CDEVICEPROXY_CONS_ENTRY );    
	}

void CDeviceProxy::ConstructL(RUsbHubDriver& aHubDriver)
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_CONSTRUCTL_ENTRY );
    
    TInt err=iHandle.Open(aHubDriver, iId);
    if (err<0)
        {
        OstTrace1( TRACE_NORMAL, CDEVICEPROXY_CONSTRUCTL, "handle open with error %d", err );
        User::Leave(err);
        }

	// Pre-allocate objects relating to this device for the event queue.
	iAttachmentEvent = new(ELeave) TDeviceEvent;
	iAttachmentEvent->iInfo.iEventType = EDeviceAttachment;
	iAttachmentEvent->iInfo.iDeviceId = iId;

	iDriverLoadingEvent = new(ELeave) TDeviceEvent;
	iDriverLoadingEvent->iInfo.iEventType = EDriverLoad;
	iDriverLoadingEvent->iInfo.iDeviceId = iId;

	iDetachmentEvent = new(ELeave) TDeviceEvent;
	iDetachmentEvent->iInfo.iEventType = EDeviceDetachment;
	iDetachmentEvent->iInfo.iDeviceId = iId;
	
	ReadStringDescriptorsL();

	LOG;
	OstTraceFunctionExit0( CDEVICEPROXY_CONSTRUCTL_EXIT );
	}

void CDeviceProxy::ReadStringDescriptorsL()
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_READSTRINGDESCRIPTORSL_ENTRY );
    
	// wait 10 ms before reading any string descriptors
	// to avoid IOP issues with some USB devices (e.g. PNY Attache)
	User::After(10000);

	// First read string descriptor 0 (supported languages).
	// For each supported language, read the manufacturer, product and serial
	// number string descriptors (as supported). (These are not cached in
	// USBD.)
	// To look up these string descriptors we need to get the device
	// descriptor. The device descriptor *is* cached in USBD, so we don't
	// remember our own copy, even though it is queried later by the CFdf.

	// '0' is the index of the string descriptor which holds the supported
	// language IDs.
	TBuf8<256> stringBuf;
	TUsbStringDescriptor* stringDesc = NULL;
	
	if(!(iHandle.Handle()))
	    {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_READSTRINGDESCRIPTORSL_DUP1, "Empty handler" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
	    }

	TInt err=iHandle.GetStringDescriptor(stringDesc, stringBuf, 0);
	LEAVEIFERRORL(err, OstTrace1( TRACE_ERROR, CDEVICEPROXY_READSTRINGDESCRIPTORSL_DUP6, 
	"iHandle.GetStringDescriptor error %d", err ););
	CleanupStack::PushL(*stringDesc);

	// Copy the language IDs into our array.
	TUint index = 0;
	TInt16 langId = stringDesc->GetLangId(index);
	while ( langId != KErrNotFound )
		{
        OstTrace1( TRACE_NORMAL, CDEVICEPROXY_READSTRINGDESCRIPTORSL, "\tsupported language: 0x%04x", langId );
        
		iLangIds.AppendL(langId); // stored as TUint
		++index;
		langId = stringDesc->GetLangId(index);
		}

	CleanupStack::PopAndDestroy(stringDesc);

	// Get the actual strings for each supported language.
	TUsbDeviceDescriptor deviceDescriptor;
	if(!(iHandle.Handle()))
	    {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_READSTRINGDESCRIPTORSL_DUP2, "Empty handler" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
	    }
	    
	err=iHandle.GetDeviceDescriptor(deviceDescriptor);
	LEAVEIFERRORL(err, OstTrace1( TRACE_ERROR, CDEVICEPROXY_READSTRINGDESCRIPTORSL_DUP7, 
	"iHandle.GetDeviceDescriptor error %d", err ););
	
	TUint8 manufacturerStringDescriptorIndex = deviceDescriptor.ManufacturerIndex();
	TUint8 productStringDescriptorIndex = deviceDescriptor.ProductIndex();
	TUint8 serialNumberStringDescriptorIndex = deviceDescriptor.SerialNumberIndex();
	PopulateStringDescriptorsL(manufacturerStringDescriptorIndex, iManufacturerStrings);
	PopulateStringDescriptorsL(productStringDescriptorIndex, iProductStrings);
	PopulateStringDescriptorsL(serialNumberStringDescriptorIndex, iSerialNumberStrings);
	if(!(iManufacturerStrings.Count() == iLangIds.Count()))
	    {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_READSTRINGDESCRIPTORSL_DUP3, "iLangIds error" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
	    }
	if(!(iProductStrings.Count() == iLangIds.Count()))
	    {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_READSTRINGDESCRIPTORSL_DUP4, "iLangIds error" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
	    }
    if(!(iSerialNumberStrings.Count() == iLangIds.Count()))
        {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_READSTRINGDESCRIPTORSL_DUP5, "iLangIds error" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
        }
	OstTraceFunctionExit0( CDEVICEPROXY_READSTRINGDESCRIPTORSL_EXIT );
	}

// Populates the given array with the supported language variants of the given
// string. Can only leave with KErrNoMemory, which fails instantiation of the
// CDeviceProxy. (It is legal for instance for manufacturer strings to be
// supported but serial number strings to *not* be.)
void CDeviceProxy::PopulateStringDescriptorsL(TUint8 aStringDescriptorIndex, RArray<TName>& aStringArray)
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_POPULATESTRINGDESCRIPTORSL_ENTRY );
    
	const TUint langCount = iLangIds.Count();
	for ( TUint ii = 0 ; ii < langCount ; ++ii )
		{
		TName string;
		TRAPD(err, GetStringDescriptorFromUsbdL(iLangIds[ii], string, aStringDescriptorIndex));
		if ( err == KErrNotFound)
			{
			// Make sure the string is blanked before storing it.
			string = KNullDesC();
			}
		else
			{            
			LEAVEIFERRORL(err, OstTrace1( TRACE_NORMAL, CDEVICEPROXY_POPULATESTRINGDESCRIPTORSL, "err=%d", err ););
			}

		err=aStringArray.Append(string);
		LEAVEIFERRORL(err, OstTrace0( TRACE_NORMAL, CDEVICEPROXY_POPULATESTRINGDESCRIPTORSL_DUP1, "aStringArray append error"););
		}
	OstTraceFunctionExit0( CDEVICEPROXY_POPULATESTRINGDESCRIPTORSL_EXIT );
	}

CDeviceProxy::~CDeviceProxy()
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_CDEVICEPROXY_DES_ENTRY );
    
	LOG;

	// In the design, the event objects should all have had ownership taken
	// onto the event queue by now. The owner of the device proxy is required
	// to take ownership of these objects before destroying the proxy.
	// However, we might hit the destructor due to an out-of-memory failure
	// during construction, so we can't assert this, and we still have to
	// destroy the objects.
	delete iAttachmentEvent;
	delete iDriverLoadingEvent;
	delete iDetachmentEvent;
	delete iOtgDescriptor;

	iLangIds.Reset();
	iManufacturerStrings.Reset();
	iProductStrings.Reset();
	iSerialNumberStrings.Reset();

	iHandle.Close();
	OstTraceFunctionExit0( CDEVICEPROXY_CDEVICEPROXY_DES_EXIT );
	}

TInt CDeviceProxy::GetDeviceDescriptor(TUsbDeviceDescriptor& aDescriptor)
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_GETDEVICEDESCRIPTOR_ENTRY );
    if(!(iHandle.Handle()))
        {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_GETDEVICEDESCRIPTOR_DUP1, "Empty handler" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
        }
	TInt err = iHandle.GetDeviceDescriptor(aDescriptor);

	OstTrace1( TRACE_NORMAL, CDEVICEPROXY_GETDEVICEDESCRIPTOR, "\terr = %d", err );
	
	OstTraceFunctionExit0( CDEVICEPROXY_GETDEVICEDESCRIPTOR_EXIT );
	return err;
	}

TInt CDeviceProxy::GetConfigurationDescriptor(TUsbConfigurationDescriptor& aDescriptor) const
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_GETCONFIGURATIONDESCRIPTOR_ENTRY );
    if(!(iHandle.Handle()))
        {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_GETCONFIGURATIONDESCRIPTOR, "Empty handler" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
        }
	TInt err = const_cast<RUsbDevice&>(iHandle).GetConfigurationDescriptor(aDescriptor);

	OstTrace1( TRACE_NORMAL, CDEVICEPROXY_GETCONFIGURATIONDESCRIPTOR_DUP1, "\terr = %d", err );
	
	OstTraceFunctionExit0( CDEVICEPROXY_GETCONFIGURATIONDESCRIPTOR_EXIT );
	return err;
	}

TInt CDeviceProxy::GetTokenForInterface(TUint aIndex, TUint32& aToken) const
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_GETTOKENFORINTERFACE_ENTRY );
    if(!(iHandle.Handle()))
        {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_GETTOKENFORINTERFACE_DUP1, "Empty handler" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
        }
    // We shouldn't need to worry about whether the device is suspended or
	// resumed before doing this. This function is only called if we find FDs
	// for the device, in which case we wouldn't have suspended it in the
	// first place.
	TInt err = const_cast<RUsbDevice&>(iHandle).GetTokenForInterface(aIndex, aToken);

	OstTrace1( TRACE_NORMAL, CDEVICEPROXY_GETTOKENFORINTERFACE, "\terr = %d", err );
	
	OstTraceFunctionExit0( CDEVICEPROXY_GETTOKENFORINTERFACE_EXIT );
	return err;
	}

const RArray<TUint>& CDeviceProxy::GetSupportedLanguages() const
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_GETSUPPORTEDLANGUAGES_ENTRY );
    
	return iLangIds;
	}

void CDeviceProxy::GetManufacturerStringDescriptorL(TUint32 aLangId, TName& aString) const
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_GETMANUFACTURERSTRINGDESCRIPTORL_ENTRY );
	
	GetStringDescriptorFromCacheL(aLangId, aString, iManufacturerStrings);
	OstTraceFunctionExit0( CDEVICEPROXY_GETMANUFACTURERSTRINGDESCRIPTORL_EXIT );
	}

void CDeviceProxy::GetProductStringDescriptorL(TUint32 aLangId, TName& aString) const
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_GETPRODUCTSTRINGDESCRIPTORL_ENTRY );
    
	GetStringDescriptorFromCacheL(aLangId, aString, iProductStrings);
	OstTraceFunctionExit0( CDEVICEPROXY_GETPRODUCTSTRINGDESCRIPTORL_EXIT );
	}

void CDeviceProxy::GetSerialNumberStringDescriptorL(TUint32 aLangId, TName& aString) const
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_GETSERIALNUMBERSTRINGDESCRIPTORL_ENTRY );
	
	GetStringDescriptorFromCacheL(aLangId, aString, iSerialNumberStrings);
	OstTraceFunctionExit0( CDEVICEPROXY_GETSERIALNUMBERSTRINGDESCRIPTORL_EXIT );
	}

void CDeviceProxy::GetOtgDescriptorL(TOtgDescriptor& aDescriptor) const
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_GETOTGDESCRIPTORL_ENTRY );
	
	if (iOtgDescriptor)
		{
		aDescriptor = *iOtgDescriptor;
		}
	else
		{
        OstTrace1( TRACE_NORMAL, CDEVICEPROXY_GETOTGDESCRIPTORL, "Error=%d", KErrNotSupported );
        User::Leave(KErrNotSupported);
		}
	OstTraceFunctionExit0( CDEVICEPROXY_GETOTGDESCRIPTORL_EXIT );
	}

void CDeviceProxy::SetOtgDescriptorL(const TUsbOTGDescriptor& aDescriptor)
	{
	if (iOtgDescriptor)
		{
		delete iOtgDescriptor;
		iOtgDescriptor = NULL;
		}
	iOtgDescriptor = new (ELeave) TOtgDescriptor();

	iOtgDescriptor->iDeviceId = iId;
	iOtgDescriptor->iAttributes = aDescriptor.Attributes();
	}

// Used during instantiation to read supported strings.
void CDeviceProxy::GetStringDescriptorFromUsbdL(TUint32 aLangId, TName& aString, TUint8 aStringDescriptorIndex) const
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_GETSTRINGDESCRIPTORFROMUSBDL_ENTRY );
	
	OstTraceExt2( TRACE_NORMAL, CDEVICEPROXY_GETSTRINGDESCRIPTORFROMUSBDL, "\taLangId = 0x%d, aStringDescriptorIndex = %d", aLangId, aStringDescriptorIndex );
	
	// If the string is not defined by the device, leave.
	if ( aStringDescriptorIndex == 0 )
		{
        OstTrace1( TRACE_NORMAL, CDEVICEPROXY_GETSTRINGDESCRIPTORFROMUSBDL_DUP1, "err=%d", KErrNotFound );
 		User::Leave(KErrNotFound);
		}

	TBuf8<255> stringBuf;
	TUsbStringDescriptor* stringDesc = NULL;
	if(!(iHandle.Handle()))
	    {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_GETSTRINGDESCRIPTORFROMUSBDL_DUP2, "Empty handler" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
	    }
	TInt err=const_cast<RUsbDevice&>(iHandle).GetStringDescriptor(stringDesc, stringBuf, aStringDescriptorIndex, aLangId);
	if(err<0)
	    {
        OstTrace0( TRACE_NORMAL, CDEVICEPROXY_GETSTRINGDESCRIPTORFROMUSBDL_DUP4, "GetStringDescriptor error" );
        User::Leave(err);
	    }
	stringDesc->StringData(aString);
	stringDesc->DestroyTree();
	delete stringDesc;
	OstTraceExt1( TRACE_NORMAL, CDEVICEPROXY_GETSTRINGDESCRIPTORFROMUSBDL_DUP3, "\taString = \"%S\"", aString );
	OstTraceFunctionExit0( CDEVICEPROXY_GETSTRINGDESCRIPTORFROMUSBDL_EXIT );
	}

// Called indirectly by users of this class to query a string descriptor.
void CDeviceProxy::GetStringDescriptorFromCacheL(TUint32 aLangId, TName& aString, const RArray<TName>& aStringArray) const
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_GETSTRINGDESCRIPTORFROMCACHEL_ENTRY );
    
    OstTrace1( TRACE_NORMAL, CDEVICEPROXY_GETSTRINGDESCRIPTORFROMCACHEL_DUP1, "\taLangId = 0x%04x", aLangId );
    
	// If the lang ID is not supported by the device, leave. At the same time
	// find the index of the required string in the given string array.
	const TUint langCount = iLangIds.Count();
	TUint index = 0;
	for ( index = 0 ; index < langCount ; ++index )
		{
		if ( iLangIds[index] == aLangId )
			{
			break;
			}
		}
	if ( index == langCount )
		{
  		OstTrace0( TRACE_NORMAL, CDEVICEPROXY_GETSTRINGDESCRIPTORFROMCACHEL, "CDeviceProxy::GetStringDescriptorFromCacheL" );
		User::Leave(KErrNotFound);
		}

	aString = aStringArray[index];
	OstTraceExt1( TRACE_NORMAL, CDEVICEPROXY_GETSTRINGDESCRIPTORFROMCACHEL_DUP2, "\taString = \"%S\"", aString );
	
	OstTraceFunctionExit0( CDEVICEPROXY_GETSTRINGDESCRIPTORFROMCACHEL_EXIT );
	}

TInt CDeviceProxy::Suspend()
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_SUSPEND_ENTRY );
	if(!(iHandle.Handle()))
	    {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_SUSPEND_DUP1, "Empty handler" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
	    }
	TInt ret = iHandle.Suspend();

	OstTrace1( TRACE_NORMAL, CDEVICEPROXY_SUSPEND, "\tret = %d", ret );
	
	OstTraceFunctionExit0( CDEVICEPROXY_SUSPEND_EXIT );
	return ret;
	}

TUint CDeviceProxy::DeviceId() const
	{
	return iId;
	}

void CDeviceProxy::SetDriverLoadingEventData(TDriverLoadStatus aStatus, TInt aError)
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_SETDRIVERLOADINGEVENTDATA_ENTRY );
	
    OstTraceExt2( TRACE_NORMAL, CDEVICEPROXY_SETDRIVERLOADINGEVENTDATA, "\taStatus = %d, aError = %d", aStatus, aError );
    if(!iDriverLoadingEvent)
        {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_SETDRIVERLOADINGEVENTDATA_DUP1, "Empty Driver Loading Event" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
        }
 	iDriverLoadingEvent->iInfo.iDriverLoadStatus = aStatus;
	iDriverLoadingEvent->iInfo.iError = aError;

	LOG;
	OstTraceFunctionExit0( CDEVICEPROXY_SETDRIVERLOADINGEVENTDATA_EXIT );
	}

TDeviceEvent* CDeviceProxy::GetAttachmentEventObject()
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_GETATTACHMENTEVENTOBJECT_ENTRY );
	
    LOG;
    if(!iAttachmentEvent)
        {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_GETATTACHMENTEVENTOBJECT_DUP1, "Empty Attechment Event" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
        }

	TDeviceEvent* const obj = iAttachmentEvent;
	iAttachmentEvent = NULL;
	OstTrace1( TRACE_NORMAL, CDEVICEPROXY_GETATTACHMENTEVENTOBJECT, "\tobj = 0x%08x", obj );
	
	OstTraceFunctionExit0( CDEVICEPROXY_GETATTACHMENTEVENTOBJECT_EXIT );
	return obj;
	}

TDeviceEvent* CDeviceProxy::GetDriverLoadingEventObject()
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_GETDRIVERLOADINGEVENTOBJECT_ENTRY );
	
    LOG;
    if(!iDriverLoadingEvent)
        {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_GETDRIVERLOADINGEVENTOBJECT, "Empty Driver Loading Event" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
        }

	TDeviceEvent* const obj = iDriverLoadingEvent;
	iDriverLoadingEvent = NULL;
	OstTrace1( TRACE_NORMAL, CDEVICEPROXY_GETDRIVERLOADINGEVENTOBJECT_DUP1, "\tobj = 0x%08x", obj );
	
	OstTraceFunctionExit0( CDEVICEPROXY_GETDRIVERLOADINGEVENTOBJECT_EXIT );
	return obj;
	}

TDeviceEvent* CDeviceProxy::GetDetachmentEventObject()
	{
	OstTraceFunctionEntry0( CDEVICEPROXY_GETDETACHMENTEVENTOBJECT_ENTRY );
    LOG;

    if(!iDetachmentEvent)
        {
        OstTrace0( TRACE_FATAL, CDEVICEPROXY_GETDETACHMENTEVENTOBJECT, "Empty Detachment Event" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory, __LINE__));
        }
	TDeviceEvent* const obj = iDetachmentEvent;
	iDetachmentEvent = NULL;
	
	OstTrace1( TRACE_NORMAL, CDEVICEPROXY_GETDETACHMENTEVENTOBJECT_DUP1, "\tobj = 0x%08x", obj );
	
	OstTraceFunctionExit0( CDEVICEPROXY_GETDETACHMENTEVENTOBJECT_EXIT );
	return obj;
	}


void CDeviceProxy::Log()
	{
    OstTraceFunctionEntry0( CDEVICEPROXY_LOG_ENTRY );
    
    OstTrace1( TRACE_DUMP, CDEVICEPROXY_LOG, "\tiId = %d", iId );
    OstTrace1( TRACE_DUMP, CDEVICEPROXY_LOG_DUP1, "\tiHandle.Handle() = %d", iHandle.Handle() );
    
    if ( iAttachmentEvent )
		{
        OstTrace0( TRACE_DUMP, CDEVICEPROXY_LOG_DUP2, "\tlogging iAttachmentEvent" );
 		iAttachmentEvent->Log();
		}
	if ( iDriverLoadingEvent )
		{
        OstTrace0( TRACE_DUMP, CDEVICEPROXY_LOG_DUP3, "\tlogging iDriverLoadingEvent" );

		iDriverLoadingEvent->Log();
		}
	if ( iDetachmentEvent )
		{
        OstTrace0( TRACE_DUMP, CDEVICEPROXY_LOG_DUP4, "\tlogging iDetachmentEvent" );
		iDetachmentEvent->Log();
		}
	const TUint langCount = iLangIds.Count();
	const TUint manufacturerCount = iManufacturerStrings.Count();
	const TUint productCount = iProductStrings.Count();
	const TUint serialNumberCount = iSerialNumberStrings.Count();

	// from the code below we can see that some protection have been added
	// if(ii<manufacturerCount) etc...
	// This has been done to protect in case there have been an incomplete construction etc..
	// when logging the data

	OstTrace1( TRACE_DUMP, CDEVICEPROXY_LOG_DUP5, "C\tlangCount = %d", langCount );
	
	for ( TUint ii = 0 ; ii < langCount ; ++ii )
		{
        OstTrace1( TRACE_FLOW, CDEVICEPROXY_LOG_DUP6, "\tlang ID 0x%04x:", iLangIds[ii] );
        
		if(ii<manufacturerCount)
			{
            OstTraceExt1( TRACE_FLOW, CDEVICEPROXY_LOG_DUP7, "\t\tmanufacturer string: \"%S\"", iManufacturerStrings[ii] );
            }
		if(ii<productCount)
			{
            OstTraceExt1( TRACE_FLOW, CDEVICEPROXY_LOG_DUP8, "\t\tproduct string: \"%S\"", iProductStrings[ii]);
		 	}
		if(ii<serialNumberCount)
			{
            OstTraceExt1( TRACE_FLOW, CDEVICEPROXY_LOG_DUP9, "\t\tserial number string: \"%S\"", iSerialNumberStrings[ii]);
			}

		}
	OstTraceFunctionExit0( CDEVICEPROXY_LOG_EXIT );
	}

