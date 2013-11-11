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

#include "fdf.h"
#include <usb/usblogger.h>
#include "utils.h"
#include <usbhost/internal/fdcplugin.hrh>
#include "eventqueue.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "fdfTraces.h"
#endif


_LIT(KDriverUsbhubLddFileName,"usbhubdriver");
_LIT(KDriverUsbdiLddFileName,"usbdi");

_LIT(KPanicCategory, "fdf");

const TUint KVendorSpecificDeviceClassValue = 0xFF;
const TUint KVendorSpecificInterfaceClassValue = 0xFF;
const TUint KMaxSearchKeyLength = 64; 

// Factory function for TInterfaceInfo objects.
CFdf::TInterfaceInfo* CFdf::TInterfaceInfo::NewL(RPointerArray<CFdf::TInterfaceInfo>& aInterfaces)
	{
    OstTraceFunctionEntry0( CFDF_TINTERFACEINFO_NEWL_ENTRY );

	TInterfaceInfo* self = new(ELeave) TInterfaceInfo;
	CleanupStack::PushL(self);
	aInterfaces.AppendL(self);
	CLEANUPSTACK_POP1(self);
	OstTraceFunctionExit0( CFDF_TINTERFACEINFO_NEWL_EXIT );
	return self;
	}


CFdf* CFdf::NewL()
	{
    OstTraceFunctionEntry0( CFDF_NEWL_ENTRY );

	CFdf* self = new(ELeave) CFdf;
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP1(self);
	OstTraceFunctionExit0( CFDF_NEWL_EXIT );
	return self;
	}

CFdf::CFdf()
:	iDevices(_FOFF(CDeviceProxy, iLink)),
	iFunctionDrivers(_FOFF(CFdcProxy, iLink))
	{
    OstTraceFunctionEntry0( CFDF_CFDF_CONS_ENTRY );
	}

void CFdf::ConstructL()
	{
	OstTraceFunctionEntry0( CFDF_CONSTRUCTL_ENTRY );
	
#ifndef __OVER_DUMMYUSBDI__
	// If we're using the DummyUSBDI we don't need the real USBDI.
	TInt err = User::LoadLogicalDevice(KDriverUsbhubLddFileName);
	if ( err != KErrAlreadyExists )
		{
        LEAVEIFERRORL(err,OstTrace1( TRACE_NORMAL, CFDF_CONSTRUCTL, "Error when loading KDriverUsbhubLddFileName OVER DUMMYUSBDI; err=%d", err ););
		}
#endif // __OVER_DUMMYUSBDI__

	TInt errDr=iHubDriver.Open();
	LEAVEIFERRORL(errDr,OstTrace1( TRACE_NORMAL, CFDF_CONSTRUCTL_DUP1, "Error when open iHubDriver; errDr=%d", errDr ););

#ifdef __OVER_DUMMYUSBDI__
	//LEAVEIFERRORL(iHubDriver.StartHost());
	TInt errHo=iHubDriver.StartHost();
	LEAVEIFERRORL(errHo,OstTrace1( TRACE_NORMAL, CFDF_CONSTRUCTL_DUP2, "Fail in iHubDriver.StartHost in dummy; errHo=%d", errHo ););

#endif

	iActiveWaitForBusEvent = CActiveWaitForBusEvent::NewL(iHubDriver, iBusEvent, *this);
	iActiveWaitForBusEvent->Wait();	
		
	CreateFunctionDriverProxiesL();	
	
	iActiveWaitForEComEvent = CActiveWaitForEComEvent::NewL(*this);
	iActiveWaitForEComEvent->Wait();
	
	iEventQueue = CEventQueue::NewL(*this);
	OstTraceFunctionExit0( CFDF_CONSTRUCTL_EXIT );
	}

void CFdf::CreateFunctionDriverProxiesL()
	{
    OstTraceFunctionEntry0( CFDF_CREATEFUNCTIONDRIVERPROXIESL_ENTRY );
	REComSession::ListImplementationsL(TUid::Uid(KFdcEcomInterfaceUid), iImplInfoArray);
	const TUint count = iImplInfoArray.Count();
	OstTrace1( TRACE_NORMAL, CFDF_CREATEFUNCTIONDRIVERPROXIESL, "\tiImplInfoArray.Count() upon FDF creation  = %d", count );
#ifdef _DEBUG
	if ( count == 0 )
		{
        OstTrace0( TRACE_NORMAL, CFDF_CREATEFUNCTIONDRIVERPROXIESL_DUP1, "\tTHERE ARE NO FUNCTION DRIVERS PRESENT IN THE SYSTEM" );
        }
	else
		{
		for (TInt32 kk = 0; kk < count; ++kk)
			OstTraceExt2( TRACE_NORMAL, CFDF_CREATEFUNCTIONDRIVERPROXIESL_DUP2,
			        "FDC implementation UID: 0x%08x Index:%d ", 
			        iImplInfoArray[kk]->ImplementationUid().iUid, kk);
			
		}
#endif

   	for ( TUint i = 0 ; i < count ; ++i )
   		{
   		CFdcProxy* proxy = CFdcProxy::NewL(*this, *iImplInfoArray[i]);
   		
   		// If this proxy is rom based then put it in the first place
   		// this will save time when trying to load the FDC with the rule of 
   		// ROM-based ones have higher priority than installed ones.
   		if (proxy->RomBased())
   			iFunctionDrivers.AddFirst(*proxy);
   		else
   			iFunctionDrivers.AddLast(*proxy);
   		}
	OstTraceFunctionExit0( CFDF_CREATEFUNCTIONDRIVERPROXIESL_EXIT );
	}

CFdf::~CFdf()
	{
    OstTraceFunctionEntry0( CFDF_CFDF_DES_ENTRY );

	// Mimic the detachment of each attached device.
	TSglQueIter<CDeviceProxy> deviceIter(iDevices);
	deviceIter.SetToFirst();
	CDeviceProxy* device;
	while ( ( device = deviceIter++ ) != NULL )
		{
		const TUint deviceId = device->DeviceId();
		OstTrace1( TRACE_NORMAL, CFDF_CFDF, "mimicking detachment of device with id %d", device );
		TellFdcsOfDeviceDetachment(deviceId);
		iDevices.Remove(*device);
		delete device;
		}

	// Destroy all the FDC proxies. They should each now have no 'attached
	// devices' and no plugin instance.
	TSglQueIter<CFdcProxy> fdcIter(iFunctionDrivers);
	fdcIter.SetToFirst();
	CFdcProxy* fdc;
	while ( ( fdc = fdcIter++ ) != NULL )
		{
		iFunctionDrivers.Remove(*fdc);
		delete fdc;
		}

	delete iActiveWaitForBusEvent;
	
	delete iActiveWaitForEComEvent;

	if ( iHubDriver.Handle() )
		{
		iHubDriver.StopHost(); // NB this has no return value
		}
	iHubDriver.Close();

#ifndef __OVER_DUMMYUSBDI__
	//If we're using the DummyUSBDI the real USBDI isn't loaded.
	TInt err = User::FreeLogicalDevice(KDriverUsbhubLddFileName);
	OstTrace1( TRACE_NORMAL, CFDF_CFDF_DUP1, "FreeLogicalDevice( usbhubdriver ) returned %d", err );
	err = User::FreeLogicalDevice(KDriverUsbdiLddFileName);
	OstTrace1( TRACE_NORMAL, CFDF_CFDF_DUP2, "FreeLogicalDevice( usbdi ) returned %d", err );
#endif // __OVER_DUMMYUSBDI__
	
	delete iEventQueue;

	// This is a worthwhile check to do at this point. If we ever don't clean
	// up iInterfaces at the *right* time, then this will be easier to debug
	// than a memory leak.
	if(!(iInterfaces.Count() == 0))
	    {
        OstTrace1( TRACE_FATAL, CFDF_CFDF_DUP3, "Memory leak from interface;Interface remains %d", iInterfaces.Count() );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	iImplInfoArray.ResetAndDestroy();
	REComSession::FinalClose();
	OstTraceFunctionExit0( CFDF_CFDF_DES_EXIT );
	}

void CFdf::EnableDriverLoading()
	{
    OstTraceFunctionEntry0( CFDF_ENABLEDRIVERLOADING_ENTRY );
	iDriverLoadingEnabled = ETrue;
	}

void CFdf::DisableDriverLoading()
	{
	OstTraceFunctionEntry0( CFDF_DISABLEDRIVERLOADING_ENTRY );
	iDriverLoadingEnabled = EFalse;
	}

void CFdf::SetSession(CFdfSession* aSession)
	{
	OstTraceFunctionEntry0( CFDF_SETSESSION_ENTRY );
	OstTrace1( TRACE_NORMAL, CFDF_SETSESSION, "aSession = 0x%08x", aSession );
	iSession = aSession;
	}

CFdfSession* CFdf::Session()
	{
	return iSession;
	}

TBool CFdf::GetDeviceEvent(TDeviceEvent& aEvent)
	{
	OstTraceFunctionEntry0( CFDF_GETDEVICEEVENT_ENTRY );
	if(!iEventQueue)
	    {
        OstTrace0( TRACE_FATAL, CFDF_GETDEVICEEVENT, "iEventQueue is empty" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	return iEventQueue->GetDeviceEvent(aEvent);
	}

TBool CFdf::GetDevmonEvent(TInt& aEvent)
	{
	OstTraceFunctionEntry0( CFDF_GETDEVMONEVENT_ENTRY );
	
	if(!iEventQueue)
	    {
        OstTrace0( TRACE_FATAL, CFDF_GETDEVMONEVENT, "iEventQueue is empty" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	
	return iEventQueue->GetDevmonEvent(aEvent);
	}
	
	
// An ECom plugin has been installed or removed
void CFdf::EComEventReceived()
	{
	TRAPD(ret, HandleEComEventReceivedL());
	if (ret != KErrNone)
		HandleDevmonEvent(KErrUsbUnableToUpdateFDProxyList);
	
	}

void CFdf::HandleEComEventReceivedL()
	{
	OstTraceFunctionEntry0( CFDF_HANDLEECOMEVENTRECEIVEDL_ENTRY );

	// There is no way to filter ecom notification to only receive ones we are interested in, also there is no way
	// to query ecom as to what has changed. Hence there is no option but to call ListImplementations().
	iImplInfoArray.ResetAndDestroy();		
			
	REComSession::ListImplementationsL(TUid::Uid(KFdcEcomInterfaceUid), iImplInfoArray);	
	TUint implementationsCount = iImplInfoArray.Count();
	OstTrace1( TRACE_NORMAL, CFDF_HANDLEECOMEVENTRECEIVEDL, "iImplInfoArray.Count() after ECom notification= %d", implementationsCount );
	
	if ( implementationsCount == 0 )
		{
        OstTrace0( TRACE_NORMAL, CFDF_HANDLEECOMEVENTRECEIVEDL_DUP1, "THERE ARE NO FUNCTION DRIVERS PRESENT IN THE SYSTEM" );
 		}
		
	TSglQueIter<CFdcProxy> proxiesIterDebug(iFunctionDrivers);
	CFdcProxy* fdcDebug = NULL;		
	while ( ( fdcDebug = proxiesIterDebug++ ) != NULL )
		{
		TUid fdcUid = fdcDebug->ImplUid();
		OstTrace1( TRACE_NORMAL, CFDF_HANDLEECOMEVENTRECEIVEDL_DUP2, "Old FDC Proxy implementation UID: 0x%08x", fdcUid.iUid );
		TInt fdcVersion = fdcDebug->Version();
		OstTrace1( TRACE_NORMAL, CFDF_HANDLEECOMEVENTRECEIVEDL_DUP3, "FDC Proxy version UID: %d", fdcVersion );
		}
	OstTrace0( TRACE_NORMAL, CFDF_HANDLEECOMEVENTRECEIVEDL_DUP4, "------------------------------------------------------------------" );

	for (TInt kk = 0; kk < implementationsCount; ++kk)
		{
		TUid fdcUid2 = iImplInfoArray[kk]->ImplementationUid();
		OstTrace1( TRACE_NORMAL, CFDF_HANDLEECOMEVENTRECEIVEDL_DUP5, "New FDC Proxy implementation UID: 0x%08x", fdcUid2.iUid );
		TInt fdcVersion2 = iImplInfoArray[kk]->Version();
		OstTrace1( TRACE_NORMAL, CFDF_HANDLEECOMEVENTRECEIVEDL_DUP6, "FDC Proxy version UID: %d", fdcVersion2 );
		}

	// See if any relevant FDCs (or upgrades) have been installed or uninstalled:	
	
	// For each FD in the proxy list compare the uid and version with each FD returned by ECom looking
	// for the removal, upgrade or downgrade of an existing FD 	
	TSglQueIter<CFdcProxy> proxiesIter(iFunctionDrivers);
	proxiesIter.SetToFirst();
	CFdcProxy* fdc = NULL;	
	while ( ( fdc = proxiesIter++ ) != NULL )
		{
		TBool fdcRemoved = ETrue;
		for (TInt ii = 0; ii < implementationsCount; ++ii)
			{
			if (fdc->ImplUid() == iImplInfoArray[ii]->ImplementationUid())
				{			
				// We have found an upgrade, downgrade, or duplicate (a duplicate could occur in the situation
				// where an FD has been installed, then a device attached, then the FD uninstalled and re-installed *while*
				// the device is still attached (meaning the FD's proxy is still in the proxy list but will have been marked
				// for deletion when the uninstallation was detected).
				fdcRemoved = EFalse;
				if (fdc->Version() != iImplInfoArray[ii]->Version())
					{
					// We've found an upgrade or a downgrade. Note that the upgrade FD proxy needs adding to the
					// proxy list, however that isn't done here it is done later in the loop that is searching for
					// new FDs. This is to prevent its possible duplicate addition [consider the situation where
					// there is FDv1 and a device is attached, then while still attached FDv2 gets installed (while will
					// result in FDv1 getting marked for deletion), then another device is attached which will use FDv2.
					// Now if FDv3 is installed before any of the devices were detached there will be two proxies in the 
					// proxy list with the same UID but differing version numbers. If FDv3 is added here it will therefore
					// be added twice].
					if (fdc->DeviceCount())
						{
						// The device using the FD is still attached
						fdc->MarkForDeletion();
						}
					else
						{
						iFunctionDrivers.Remove(*fdc);
						delete fdc;
						}					
					}		
				else
					{
					// we've found an FD being installed which is still currently 
					// active in the proxy list
					fdc->UnmarkForDeletion();
					}	
				// Since we found the plugin with the same implementationUid
				// we could simply bail out to stop the looping;
				break;
				}
			}
		if (fdcRemoved)
			{ 
			// An FDC has been uninstalled - if the FDC isn't in use remove it 
			// otherwise mark it for deletion
			if (fdc->DeviceCount())
				fdc->MarkForDeletion();
			else
				{
				iFunctionDrivers.Remove(*fdc); 
				delete fdc;				
				}			
			}
		}
		
		
	// For each FD returned by ECom, search and compare with the FD proxy list 
	// looking for new FDs
	for (TInt ii = 0; ii < implementationsCount; ++ii)
		{
		TBool newFdcFound = ETrue;
		proxiesIter.SetToFirst();
		while ( ( fdc = proxiesIter++ ) != NULL )
			{
			if (fdc->ImplUid() == iImplInfoArray[ii]->ImplementationUid() && fdc->Version() == iImplInfoArray[ii]->Version())
				{
				// No need to create a new proxy if there is one with a matching UID and version.
				newFdcFound = EFalse;
				
				// We break out this loop for efficiency.
				break;
				}
			}	
			
		if (newFdcFound)
			{ 
			// A new or upgrade FDC has been installed onto the device
			CFdcProxy* proxy = CFdcProxy::NewL(*this, *iImplInfoArray[ii]);

			// If this proxy is rom based then put it in the first place
	   		// this will save time when trying to load the FDC with the rule that 
	   		// ROM-based ones have higher priority than installed ones.
			if (proxy->RomBased())
				iFunctionDrivers.AddFirst(*proxy);
			else
				iFunctionDrivers.AddLast(*proxy);			
			}
		}
	OstTraceFunctionExit0( CFDF_HANDLEECOMEVENTRECEIVEDL_EXIT );
	}

// A bus event has occurred.
void CFdf::MbeoBusEvent()
	{
    OstTraceFunctionEntry0( CFDF_MBEOBUSEVENT_ENTRY );
    OstTrace1( TRACE_NORMAL, CFDF_MBEOBUSEVENT, "iBusEvent.iEventType = %d", iBusEvent.iEventType );
    OstTrace1( TRACE_NORMAL, CFDF_MBEOBUSEVENT_DUP1, "iBusEvent.iError = %d", iBusEvent.iError );
    OstTrace1( TRACE_NORMAL, CFDF_MBEOBUSEVENT_DUP2, "iBusEvent.iDeviceHandle = %d", iBusEvent.iDeviceHandle );

	switch ( iBusEvent.iEventType )
		{
		case RUsbHubDriver::TBusEvent::EDeviceAttached:
			if ( !iBusEvent.iError )
				{
				// So far, a successful attachment.
				HandleDeviceAttachment(iBusEvent.iDeviceHandle);
				}
			else
				{
				// It was an attachment failure. Simply tell the event queue.
				if(!iEventQueue)
				    {
                    OstTrace0( TRACE_FATAL, CFDF_MBEOBUSEVENT_DUP3, "Empty iEventQueue" );
                    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
				    }
				iEventQueue->AttachmentFailure(iBusEvent.iError);
				}
			break;
	
		case RUsbHubDriver::TBusEvent::EDeviceRemoved:
			// Device detachments are always 'KErrNone'. If the device was
			// pseudo-detached due to an overcurrent condition (for instance) then
			// the overcurrent condition is indicated through the devmon API (i.e.
			// EDevMonEvent) and the detachment is still 'KErrNone'.
			if(!(iBusEvent.iError == KErrNone))
			    {
                OstTrace0( TRACE_NORMAL, CFDF_MBEOBUSEVENT_DUP4, "iBusEvent error" );
                __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
			    }

			HandleDeviceDetachment(iBusEvent.iDeviceHandle);
			break;
	
		case RUsbHubDriver::TBusEvent::EDevMonEvent:
			HandleDevmonEvent(iBusEvent.iError);
			break;
	
		case RUsbHubDriver::TBusEvent::ENoEvent:
		default:
		break;
		}

	// Only re-post the notification when we've finished examining the
	// TBusEvent from the previous completion. (Otherwise it might get
	// overwritten.)
	iActiveWaitForBusEvent->Wait();
	OstTraceFunctionExit0( CFDF_MBEOBUSEVENT_EXIT );
	}

// This is the central handler for device attachment.
// We deal with device attachments in two phases.
// The first phase is confusingly called device attachment.
// The second phase is driver loading.
void CFdf::HandleDeviceAttachment(TUint aDeviceId)
	{
    OstTraceFunctionEntry0( CFDF_HANDLEDEVICEATTACHMENT_ENTRY );
	// This is filled in by HandleDeviceAttachmentL on success.
	CDeviceProxy* device;
	TRAPD(err, HandleDeviceAttachmentL(aDeviceId, device));
	if ( err )
		{
        OstTrace1( TRACE_NORMAL, CFDF_HANDLEDEVICEATTACHMENT, "err = %d", err );
		// There was an attachment failure, so we just increment the count of
		// attachment failures.
		if(!iEventQueue)
		    {
            OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEATTACHMENT_DUP1, "Empty iEventQueue" );
            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }

		iEventQueue->AttachmentFailure(err);
		// If we failed the attachment phase, we can't try to load drivers for
		// the device.

		}
	else
		{
		// This function always moves the 'driver loading' event from the
		// device proxy created by HandleDeviceAttachmentL to the event queue.
		// This event object is always populated with the correct status and
		// error.
		if(!device)
		    {
            OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEATTACHMENT_DUP2, "Empty device" );
            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }

		DoDriverLoading(*device);
		}

	// Finally, clean up the collection of information on the device's
	// interfaces which was populated (maybe only partly) in
	// HandleDeviceAttachmentL.
	iCurrentDevice = NULL;
	iInterfaces.ResetAndDestroy();
	OstTraceFunctionExit0( CFDF_HANDLEDEVICEATTACHMENT_EXIT );
	}

// This does the 'device attachment' phase of the new device attachment only.
void CFdf::HandleDeviceAttachmentL(TUint aDeviceId, CDeviceProxy*& aDevice)
	{
	OstTraceFunctionEntry0( CFDF_HANDLEDEVICEATTACHMENTL_ENTRY );
	// Create the device proxy
	aDevice = CDeviceProxy::NewL(iHubDriver, aDeviceId);
	CleanupStack::PushL(aDevice);
	iCurrentDevice = aDevice;
	// Get necessary descriptors (for this phase)
	TInt err=aDevice->GetDeviceDescriptor(iDD);
	if (err<0)
	    {
        OstTrace1( TRACE_NORMAL, CFDF_HANDLEDEVICEATTACHMENTL, "GetDeviceDescriptor error=%d", err );
        User::Leave(err);
	    }

	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP1, "iDD.USBBcd = 0x%04x", iDD.USBBcd() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP2, "iDD.DeviceClass = 0x%02x", iDD.DeviceClass() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP3, "iDD.DeviceSubClass = 0x%02x", iDD.DeviceSubClass() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP4, "iDD.DeviceProtocol = 0x%02x", iDD.DeviceProtocol() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP5, "iDD.MaxPacketSize0 = %d", iDD.MaxPacketSize0() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP6, "iDD.VendorId = 0x%04x", iDD.VendorId() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP7, "iDD.ProductId = 0x%04x", iDD.ProductId() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP8, "iDD.DeviceBcd = 0x%04x", iDD.DeviceBcd() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP9, "iDD.ManufacturerIndex = %d", iDD.ManufacturerIndex() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP10, "iDD.ProductIndex = %d", iDD.ProductIndex() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP11, "iDD.SerialNumberIndex = %d", iDD.SerialNumberIndex() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP12, "iDD.NumConfigurations = %d", iDD.NumConfigurations() );
	err=aDevice->GetConfigurationDescriptor(iCD);
	if(err<0)
	    {
        OstTrace1( TRACE_NORMAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP13, "GetConfigurationDescriptor error=%d", err);
	    User::Leave(err);
	    }

	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP14, "iCD.TotalLength = %d", iCD.TotalLength() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP15, "iCD.NumInterfaces = %d", iCD.NumInterfaces() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP16, "iCD.ConfigurationValue = %d", iCD.ConfigurationValue() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP17, "iCD.ConfigurationIndex = %d", iCD.ConfigurationIndex() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP18, "iCD.Attributes = %d", iCD.Attributes() );
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP19, "iCD.MaxPower = %d", iCD.MaxPower() );
	        
	const TUint8 numberOfInterfaces = iCD.NumInterfaces();
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP20, "numberOfInterfaces (field in config descriptor) = %d)", numberOfInterfaces );
	if ( numberOfInterfaces == 0 )
		{
	    OstTrace0( TRACE_NORMAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP21, "Error: Usb Configuration Has No Interfaces");
	    User::Leave(KErrUsbConfigurationHasNoInterfaces);
		}

	// Walk the configuration bundle. Collect information on each interface
	// (its number, class, subclass and protocol). This populates iInterfaces.
	if(!(iInterfaces.Count() == 0))
	    {
        OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP22, "Error: Usb Configuration Has No Interfaces");
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	if(!iCurrentDevice)
	    {
        OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP23, "Empty iCurrentDevice");
        User::Panic(KPanicCategory,__LINE__);
	    }

	ParseL(iCD);

	// Log iInterfaces.
	const TUint interfaceCount = iInterfaces.Count();
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP24, "interfaceCount (parsed from bundle) = %d", interfaceCount );
#ifdef _DEBUG
	OstTrace0( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP25, "Logging iInterfaces:");
	for ( TUint ii = 0 ; ii < interfaceCount ; ++ii )
		{
		const TInterfaceInfo* ifInfo = iInterfaces[ii];
		if(!ifInfo)
		    {
            OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP26, "Empty ifInfo");
            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		
		OstTraceExt5(TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP27, "\t\tiInterfaces[%d]: number %d, interface class 0x%02x subclass 0x%02x protocol 0x%02x",
			ii,
			ifInfo->iNumber,
			ifInfo->iClass,
			ifInfo->iSubclass,
			ifInfo->iProtocol
			);
		}
#endif
	
	// Check that the config's NumInterfaces is the same as the actual number
	// of interface descriptors we found. We rely on this later on.
	if ( numberOfInterfaces != interfaceCount )
		{
        OstTrace0( TRACE_NORMAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP28, "KErrUsbInterfaceCountMismatch");
		User::Leave(KErrUsbInterfaceCountMismatch);
		}

	// Check that each interface number in iInterfaces is unique.
	if ( interfaceCount > 1 )
		{
		for ( TUint ii = 0 ; ii < interfaceCount ; ++ii )
			{
			const TInterfaceInfo* lhs = iInterfaces[ii];
			if(!lhs)
			    {
                OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP29, "Empty lhs");
                __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
			    }

			for ( TUint jj = ii+1 ; jj < interfaceCount ; ++jj )
				{
				const TInterfaceInfo* rhs = iInterfaces[jj];
				if(!rhs)
				    {
                    OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP30, "Empty rhs");
                    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
				    }

				if ( lhs->iNumber == rhs->iNumber )
					{
                    OstTrace1( TRACE_NORMAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP31, "KErrUsbDuplicateInterfaceNumbers; iNumber=%d", lhs->iNumber);
					User::Leave(KErrUsbDuplicateInterfaceNumbers);
					}
				}
			}
		}

#ifndef __OVER_DUMMYUSBDI__
	// If we're using the DummyUSBDI we don't need the real USBDI.
	// Load USBDI when attached devices goes from 0 to 1
	if (iDevices.IsEmpty())
		{
		TInt err = User::LoadLogicalDevice(KDriverUsbdiLddFileName);
		if ( err != KErrAlreadyExists )
			{
			LEAVEIFERRORL(err,OstTrace1( TRACE_NORMAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP32, "Load Logical Device error; error=%d", err););
			}
		}
#endif // __OVER_DUMMYUSBDI__
	
	// Now we know we've succeeded with a device attachment, remove the device
	// proxy from the cleanup stack and put it on the TSglQue.
	CLEANUPSTACK_POP1(aDevice);
	iDevices.AddLast(*aDevice);
	// Also put an event on the event queue.
	TDeviceEvent* const attachmentEvent = aDevice->GetAttachmentEventObject();
	
	if(!attachmentEvent)
	    {
        OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP33, "Empty attachmentEvent");
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	attachmentEvent->iInfo.iVid = iDD.VendorId();
	attachmentEvent->iInfo.iPid = iDD.ProductId();
	attachmentEvent->iInfo.iError = KErrNone;
	if(!iEventQueue)
	    {
        OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEATTACHMENTL_DUP34, "Empty iEventQueue");
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	iEventQueue->AddDeviceEvent(*attachmentEvent);
	OstTrace1( TRACE_DUMP, CFDF_HANDLEDEVICEATTACHMENTL_DUP35, "***USB HOST STACK: SUCCESSFUL ATTACHMENT OF DEVICE (id %d)", aDeviceId );
	
	OstTraceFunctionExit0( CFDF_HANDLEDEVICEATTACHMENTL_EXIT );
	}

void CFdf::DoDriverLoading(CDeviceProxy& aDevice)
	{
    OstTraceFunctionEntry0( CFDF_DODRIVERLOADING_ENTRY );
	// Leaving or returning from DoDriverLoadingL is the trigger to put the
	// 'driver loading' event object on the event queue. It must already have
	// been populated correctly (the actual error code it left with doesn't
	// feed into the driver loading event).
	TRAP_IGNORE(DoDriverLoadingL(aDevice));
	
	TDeviceEvent* const driverLoadingEvent = aDevice.GetDriverLoadingEventObject();
	if(!driverLoadingEvent)
	    {
        OstTrace0( TRACE_FATAL, CFDF_DODRIVERLOADING, "Empty driverLoadingEvent" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	// The driver loading event object says whether driver loading succeeded
	// (all interfaces were claimed without error), partly succeeded (not all
	// interfaces were claimed without error), or failed (no interfaces were
	// claimed without error). This information is intended for USBMAN so it
	// can tell the user, but we also use it now to suspend the device if
	// driver loading failed completely.
	if ( driverLoadingEvent->iInfo.iDriverLoadStatus == EDriverLoadFailure )
		{
		// We can't do anything with error here. Suspending the device is for
		// power-saving reasons and is not critical.
		(void)aDevice.Suspend();
		}
	if(!iEventQueue)
	    {
        OstTrace0( TRACE_FATAL, CFDF_DODRIVERLOADING_DUP1, "Empty iEventQueue" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	iEventQueue->AddDeviceEvent(*driverLoadingEvent);
	OstTraceFunctionExit0( CFDF_DODRIVERLOADING_EXIT );
	}


void CFdf::DoDriverLoadingL(CDeviceProxy& aDevice)
	{
    OstTraceFunctionEntry0( CFDF_DODRIVERLOADINGL_ENTRY );
	// Check whether driver loading is enabled.
	if ( !iDriverLoadingEnabled )
		{
		// Complete driver load failure scenario.
		aDevice.SetDriverLoadingEventData(EDriverLoadFailure, KErrUsbDriverLoadingDisabled);
		
		OstTrace1( TRACE_NORMAL, CFDF_DODRIVERLOADINGL, "Leave with error %d", KErrGeneral );
		User::Leave(KErrGeneral);
		}


	// Set this member up so that when the FDC calls TokenForInterface we call
	// the right proxy object.

	TInt collectedErr = KErrNone;
	TBool anySuccess = EFalse;


	// Now actually try to load the drivers.
	// Device drivers are located based upon descriptor information from the USB device. The first search is
	// based on information from the device descriptor and looks for a driver that matches the whole device; 
	// the second search is based upon locating a driver for each interface within a configuration.
	// The particular keys used in the driver search are defined in the Universal Serial Bus Common Class
	// Specification version 1.0. They are represented by TDeviceSearchKeys and TInterfaceSearchKeys.						
	//
	// First perform a device search by iterating through the keys in TDeviceSearchKeys looking for a matching driver.
	TBool functionDriverFound = SearchForADeviceFunctionDriverL(aDevice, anySuccess, collectedErr);
	
	// When do the parsing against the CD bundle, we already know if there is IAD(Interface Association Descriptor)
	// in the new attached device. Once we finished the device level searching of FDC and we couldn't find any, we
	// break down the loading process
	if (aDevice.HasIADFlag() && !functionDriverFound)
		{
		aDevice.SetDriverLoadingEventData(EDriverLoadFailure, KErrUsbUnsupportedDevice);
		OstTrace0( TRACE_NORMAL, CFDF_DODRIVERLOADINGL_DUP1, "Leave with function Driver not Found" );
		User::Leave(KErrGeneral);
		}
	// If a device FD is found then it is supposed to claim all the interfaces, if it didn't then report
	// a partial success but don't offer unclaimed interfaces to any other FD.
	const TUint interfaceCount = iInterfaces.Count();


	
	// If no device driver was found then next perform an Interface search
	if (!functionDriverFound)
		SearchForInterfaceFunctionDriversL(aDevice, anySuccess, collectedErr);

	// Now worry about the following:
	// (a) are there any unclaimed interfaces remaining?
	// (b) what's in collectedErr?
	// Whether all interfaces were taken, some, or none, collectedErr may have
	// an error in it or KErrNone. We use specific error codes in some cases.			
	TUint unclaimedInterfaces = UnclaimedInterfaceCount();
	OstTrace1( TRACE_DUMP, CFDF_DODRIVERLOADINGL_DUP2, "unclaimedInterfaces = %d", unclaimedInterfaces );
	OstTrace1( TRACE_DUMP, CFDF_DODRIVERLOADINGL_DUP3, "anySuccess = %d", anySuccess );
	OstTrace1( TRACE_DUMP, CFDF_DODRIVERLOADINGL_DUP4, "collectedErr = %d", collectedErr );
	if(!(unclaimedInterfaces <= interfaceCount))
	    {
        OstTrace0( TRACE_FATAL, CFDF_DODRIVERLOADINGL_DUP5, "interface Count error" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }


	if(iDeviceDetachedTooEarly)
		{
        OstTrace0( TRACE_NORMAL, CFDF_DODRIVERLOADINGL_DUP6, "Device has been detached too early!" );
		iDeviceDetachedTooEarly = EFalse;
		// the choice of having the status to be EDriverLoadPartialSuccess
		// was not to clash with trying to suspend the device because
		// of a total failure to load the FD.(because device is detached)
		// even though that a FDC has been created
		// see the :
		// if ( driverLoadingEvent->iInfo.iDriverLoadStatus == EDriverLoadFailure )
		// in function above => void CFdf::DoDriverLoadingL(etc...)
		aDevice.SetDriverLoadingEventData(EDriverLoadPartialSuccess, KErrUsbDeviceDetachedDuringDriverLoading);
		}
	else
		{
		SetFailureStatus(unclaimedInterfaces, interfaceCount, anySuccess, collectedErr, aDevice);
		}// iDeviceDetachedTooEarly

	OstTraceFunctionExit0( CFDF_DODRIVERLOADINGL_EXIT );
	}

// Recursive function, originally called with the configuration descriptor.
// Builds up information on the interface descriptors in the configuration
// bundle.
void CFdf::ParseL(TUsbGenericDescriptor& aDesc)
	{
    OstTraceFunctionEntry0( CFDF_PARSEL_ENTRY );
    OstTrace1( TRACE_DUMP, CFDF_PARSEL, "&aDesc = 0x%08x", &aDesc );
    OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP1, "aDesc.ibDescriptorType = %d", aDesc.ibDescriptorType );
    OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP2, "aDesc.iFirstChild = 0x%08x", aDesc.iFirstChild );
    OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP3, "aDesc.iNextPeer = 0x%08x", aDesc.iNextPeer );
    
    if ( aDesc.ibDescriptorType == EInterface )
		{
		// Add interface information to collection, but only if it's alternate
		// setting 0.
		const TUsbInterfaceDescriptor& ifDesc = static_cast<TUsbInterfaceDescriptor&>(aDesc);
		if ( ifDesc.AlternateSetting() == 0 ) // hard-coded '0' means the default (initial configuration) setting
			{
            OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP4, "ifDesc.InterfaceNumber = %d", ifDesc.InterfaceNumber() );
            OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP5, "ifDesc.NumEndpoints = %d", ifDesc.NumEndpoints() );
            OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP6, "ifDesc.InterfaceClass = 0x%02x", ifDesc.InterfaceClass() );
            OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP7, "ifDesc.InterfaceSubClass = 0x%02x", ifDesc.InterfaceSubClass() );
            OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP8, "ifDesc.InterfaceProtocol = 0x%02x", ifDesc.InterfaceProtocol() );
            OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP9, "ifDesc.Interface = %d", ifDesc.Interface() );
            
			TInterfaceInfo* ifInfo = TInterfaceInfo::NewL(iInterfaces);
			ifInfo->iNumber = ifDesc.InterfaceNumber();
			ifInfo->iClass = ifDesc.InterfaceClass();
			ifInfo->iSubclass = ifDesc.InterfaceSubClass();
			ifInfo->iProtocol = ifDesc.InterfaceProtocol();
			ifInfo->iClaimed = EFalse;
			}
		}
	else if (!iCurrentDevice->HasIADFlag() && aDesc.ibDescriptorType == EInterfaceAssociation)
		{
		// When found a Interface association descriptor, set this flag to ETrue,
		// it is checked later after the device level driverloading.
		iCurrentDevice->SetHasIADFlag();
		}
	else if (aDesc.ibDescriptorType == EOTG)
		{
		// OTG descriptor found
		const TUsbOTGDescriptor& otgDesc = static_cast<TUsbOTGDescriptor&>(aDesc);
		OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP10, "otgDesc.Attributes = %d", otgDesc.Attributes() );
		OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP11, "otgDesc.HNPSupported = %d", otgDesc.HNPSupported() );
		OstTrace1( TRACE_DUMP, CFDF_PARSEL_DUP12, "otgDesc.SRPSupported = %d", otgDesc.SRPSupported() );
		
		iCurrentDevice->SetOtgDescriptorL(otgDesc);
		}

	TUsbGenericDescriptor* const firstChild = aDesc.iFirstChild;
	if ( firstChild )
		{
		ParseL(*firstChild);
		}

	TUsbGenericDescriptor* const nextPeer = aDesc.iNextPeer;
	if ( nextPeer )
		{
		ParseL(*nextPeer);
		}
	OstTraceFunctionExit0( CFDF_PARSEL_EXIT );
	}

// Method that uses only one array to hold the unclaimed interface numbers.
void CFdf::FindDriversForInterfacesUsingSpecificKeyL(CDeviceProxy& aDevice,
													TInt& aCollectedErr,
													TBool& aAnySuccess,			
													RArray<TUint>& aInterfacesNumberArray, 
													TInterfaceSearchKeys aKey)	
	{
    OstTraceFunctionEntry0( CFDF_FINDDRIVERSFORINTERFACESUSINGSPECIFICKEYL_ENTRY );
 
	const TUint interfaceCount = iInterfaces.Count();
	for ( TUint ii = 0 ; ii < interfaceCount ; ++ii )
		{
		TInterfaceInfo* ifInfo = iInterfaces[ii];		
		if(!ifInfo)
		    {
            OstTrace0( TRACE_FATAL, CFDF_FINDDRIVERSFORINTERFACESUSINGSPECIFICKEYL, "Empty ifInfo" );
            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		
		if ((ifInfo->iClaimed) ||
			(aKey == EVendorInterfacesubclassInterfaceprotocol && ifInfo->iClass != KVendorSpecificInterfaceClassValue)||	
			(aKey == EVendorInterfacesubclass && ifInfo->iClass != KVendorSpecificInterfaceClassValue) ||
			(aKey == EInterfaceclassInterfacesubclassInterfaceprotocol && ifInfo->iClass == KVendorSpecificInterfaceClassValue) ||
			(aKey == EInterfaceclassInterfacesubclass && ifInfo->iClass == KVendorSpecificInterfaceClassValue))
			{
			// Putting ii+1 as the starting offset is to remove the interface on which the searching have been done.
			RebuildUnClaimedInterfacesArrayL(aDevice, aInterfacesNumberArray, ii+1);
			continue;
			}
		

		TBuf8<KMaxSearchKeyLength> searchKey;
		FormatInterfaceSearchKey(searchKey, aKey, *ifInfo);
		OstTraceExt1( TRACE_NORMAL, CFDF_FINDDRIVERSFORINTERFACESUSINGSPECIFICKEYL_DUP1, "searchKey = \"%S\"", searchKey );
		// RArray<TUint>* array = &aInterfacesNumberArray;

		FindDriverForInterfaceUsingSpecificKey(aDevice, aCollectedErr, aAnySuccess, aInterfacesNumberArray, searchKey);

		// Putting ii+1 as the starting offset is to remove the interface on which
		// the searching have been done.		
		RebuildUnClaimedInterfacesArrayL(aDevice, aInterfacesNumberArray, ii+1);
		}
	OstTraceFunctionExit0( CFDF_FINDDRIVERSFORINTERFACESUSINGSPECIFICKEYL_EXIT );
	}



// Called for one interface, to find a Function Driver on the basis of a

// specific search key.
void CFdf::FindDriverForInterfaceUsingSpecificKey(CDeviceProxy& aDevice,
								   TInt& aCollectedErr,
								   TBool& aAnySuccess,
								   RArray<TUint>& aInterfacesGivenToFdc,
								   const TDesC8& aSearchKey)
	{
    OstTraceFunctionEntry0( CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY, "aSearchKey = \"%S\"", aSearchKey );

	// Find an FDC matching this search key.
	TSglQueIter<CFdcProxy> iter(iFunctionDrivers);
	iter.SetToFirst();
	CFdcProxy* fdc;

	while ( ( fdc = iter++ ) != NULL )
		{
        OstTraceExt1( TRACE_NORMAL, CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_DUP1, "FDC's default_data field = \"%S\"", fdc->DefaultDataField() );

#ifdef _DEBUG
	// having these two together in the debug window is helpful for interactive debugging
	TBuf8<KMaxSearchKeyLength > fd_key;
	fd_key.Append(fdc->DefaultDataField().Ptr(), fdc->DefaultDataField().Length() > KMaxSearchKeyLength ? KMaxSearchKeyLength : fdc->DefaultDataField().Length());
	TBuf8<KMaxSearchKeyLength > searchKey;
	searchKey.Append(aSearchKey.Ptr(), aSearchKey.Length() > KMaxSearchKeyLength ? KMaxSearchKeyLength : aSearchKey.Length());
	TInt version = fdc->Version();
#endif // _DEBUG
		if (aSearchKey.CompareF(fdc->DefaultDataField()) == 0 && !fdc->MarkedForDeletion())
			{
			// If there is more than one matching FD then if all of them are in RAM we simply choose the first one we find.
			// (Similarly if they are all in ROM we choose the first one although this situation should not arise as a device
			// manufacturer should not put two matching FDs into ROM).
			// However if there are matching FDs in ROM and RAM then the one in ROM should be selected in preference to
			// any in RAM. Hence at this point if the matching FD we have found is in RAM then we need to scan the list
			// of FDs to see if there is also a matching one in ROM and if so we'll skip this iteration of the loop.
			
			// Edwin comment
			// Put the searching key and the iterator as the parameter of 
			// searching if more FDCs have the same default_data. The iterator
			// helps to searching from the current FDC since this is the very first
			// suitable FDC we found so fa.
			if (!aDevice.MultipleDriversFlag() && FindMultipleFDs(aSearchKey, iter))
				{
				aDevice.SetMultipleDriversFlag();
				}
			OstTrace1( TRACE_NORMAL, CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_DUP2, "found matching FDC (0x%08x)", fdc );

			const TUint count = aInterfacesGivenToFdc.Count();
			OstTrace1( TRACE_NORMAL, CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_DUP3, "logging aInterfacesGivenToFdc (interfaces being offered to the FDC): count = %d", count );
			for ( TUint ii = 0 ; ii < count ; ++ii )
				{
                OstTraceExt2( TRACE_NORMAL, CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_DUP4, 
                        "index %u: interface number %u", 
                        ii, aInterfacesGivenToFdc[ii] );
			    }
			TInt err = fdc->NewFunction(aDevice.DeviceId(), aInterfacesGivenToFdc, iDD, iCD);
			OstTrace1( TRACE_NORMAL, CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_DUP5, "NewFunction returned %d", err );
			            
			// To correctly determine whether the driver load for the whole
			// configuration was a complete failure, a partial success or a
			// complete success, we need to collect any non-KErrNone error
			// from this, and whether any handovers worked at all.
			if ( err == KErrNone )
				{
                OstTraceExt2(TRACE_NORMAL, CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_DUP6, 
                        "***USB HOST STACK: THE FOLLOWING INTERFACES OF DEVICE %u WERE SUCCESSFULLY PASSED TO FUNCTION DRIVER WITH IMPL UID 0x%08x",
					(TInt32) aDevice.DeviceId(), fdc->ImplUid().iUid);
				// We want to log each interface that's in
				// aInterfacesGivenToFdc AND is marked claimed in iInterfaces.
				for ( TUint ii = 0 ; ii < aInterfacesGivenToFdc.Count() ; ++ii )
					{
					const TUint ifNum = aInterfacesGivenToFdc[ii];
					for ( TUint jj = 0 ; jj < iInterfaces.Count() ; ++jj )
						{
						const TInterfaceInfo* ifInfo = iInterfaces[jj];
						
						if(!ifInfo)
						    {
                            OstTrace0( TRACE_FATAL, CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_DUP7, "Empty ifInfo" );
                            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
						    }
						if (	ifNum == ifInfo->iNumber
							&&	ifInfo->iClaimed
							)
							{
                            OstTrace1( TRACE_NORMAL, CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_DUP8, "***USB HOST STACK: bInterfaceNumber %d", ifNum );
							}
						}
					}
				aAnySuccess = ETrue;
				}
			else
				{
				aCollectedErr = err;
				}
			// We found a matching FDC for this interface- no need to look for more.
			break;
			}
		}
	OstTraceFunctionExit0( CFDF_FINDDRIVERFORINTERFACEUSINGSPECIFICKEY_EXIT );
	}

void CFdf::HandleDeviceDetachment(TUint aDeviceId)
	{
    OstTraceFunctionEntry0( CFDF_HANDLEDEVICEDETACHMENT_ENTRY );
    OstTrace1( TRACE_NORMAL, CFDF_HANDLEDEVICEDETACHMENT, "aDeviceId = %d", aDeviceId );

#ifdef _DEBUG
	TBool found = EFalse;
#endif
	// Find the relevant device proxy. If there isn't one, just drop the
	// notification, assuming that the corresponding attachment failed at the
	// FDF level.
	TSglQueIter<CDeviceProxy> iter(iDevices);
	iter.SetToFirst();
	CDeviceProxy* device;
	while ( ( device = iter++ ) != NULL )
		{
		if ( device->DeviceId() == aDeviceId )
			{
#ifdef _DEBUG
			found = ETrue;
#endif
			OstTrace0( TRACE_NORMAL, CFDF_HANDLEDEVICEDETACHMENT_DUP1, "found matching device proxy" );
			
			iDevices.Remove(*device);
			// Before destroying the device proxy, take the detachment event
			// stored in it for the event queue.
			TDeviceEvent* const detachmentEvent = device->GetDetachmentEventObject();
			if(!detachmentEvent)
			    {
                OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEDETACHMENT_DUP2, "Empty detachmentEvent" );
                __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
			    }
			if(!iEventQueue)
			    {
                OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVICEDETACHMENT_DUP3, "Empty iEventQueue" );
                __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
			    }
			iEventQueue->AddDeviceEvent(*detachmentEvent);
			OstTrace1( TRACE_NORMAL, CFDF_HANDLEDEVICEDETACHMENT_DUP4, "***USB HOST STACK: DETACHMENT OF DEVICE (id %d)", aDeviceId );
			delete device;

			TellFdcsOfDeviceDetachment(aDeviceId);
			
#ifndef __OVER_DUMMYUSBDI__
			// If we're using the DummyUSBDI the real USBDI isn't loaded.
			// Unload USBDI when attached devices goes from 1 to 0
			if (iDevices.IsEmpty())
				{
				TInt err = User::FreeLogicalDevice(KDriverUsbdiLddFileName);
				OstTrace1( TRACE_NORMAL, CFDF_HANDLEDEVICEDETACHMENT_DUP5, "FreeLogicalDevice( usbdi ) returned %d", err );
				}
#endif // __OVER_DUMMYUSBDI__
			
			break;
			}
		}

#ifdef _DEBUG
	if ( !found )
		{
        OstTrace0( TRACE_NORMAL, CFDF_HANDLEDEVICEDETACHMENT_DUP6, "no matching device proxy found" );
		}
#endif
	
	OstTraceFunctionExit0( CFDF_HANDLEDEVICEDETACHMENT_EXIT );
	}

void CFdf::HandleDevmonEvent(TInt aEvent)
	{
	OstTraceFunctionEntry0( CFDF_HANDLEDEVMONEVENT_ENTRY );
	OstTrace1( TRACE_NORMAL, CFDF_HANDLEDEVMONEVENT, "aEvent = %d", aEvent );

	if(!iEventQueue)
	    {
        OstTrace0( TRACE_FATAL, CFDF_HANDLEDEVMONEVENT_DUP1, "Empty iEventQueue" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }
	iEventQueue->AddDevmonEvent(aEvent);
	OstTraceFunctionExit0( CFDF_HANDLEDEVMONEVENT_EXIT );
	}

void CFdf::TellFdcsOfDeviceDetachment(TUint aDeviceId)
	{
	OstTraceFunctionEntry0( CFDF_TELLFDCSOFDEVICEDETACHMENT_ENTRY );
	OstTrace1( TRACE_NORMAL, CFDF_TELLFDCSOFDEVICEDETACHMENT, "aDeviceId = %d", aDeviceId );
	
	TSglQueIter<CFdcProxy> iter(iFunctionDrivers);
	iter.SetToFirst();
	CFdcProxy* fdc;
	while ( ( fdc = iter++ ) != NULL )
		{
		fdc->DeviceDetached(aDeviceId);
		if (fdc->DeviceCount() == 0 && fdc->MarkedForDeletion())
			{ // If the FDC was uninstalled while it was in use then it couldn't be deleted at that point so delete it now
			iFunctionDrivers.Remove(*fdc);
			delete fdc;
			}
		}			
		
	OstTraceFunctionExit0( CFDF_TELLFDCSOFDEVICEDETACHMENT_EXIT );
	}

TUint32 CFdf::TokenForInterface(TUint8 aInterface)
	{
	OstTraceFunctionEntry0( CFDF_TOKENFORINTERFACE_ENTRY );
	OstTrace1( TRACE_NORMAL, CFDF_TOKENFORINTERFACE, "aInterface = %d", aInterface );
	
	TUint32 token = 0;

	// Check that the interface was in the array given to the FD and mark it
	// as claimed.
	TBool found = EFalse;
	const TUint interfaceCount = iInterfaces.Count();
	for ( TUint ii = 0 ; ii < interfaceCount ; ++ii )
		{
		TInterfaceInfo* ifInfo = iInterfaces[ii];
		if(!ifInfo)
		    {
            OstTrace0( TRACE_FATAL, CFDF_TOKENFORINTERFACE_DUP1, "Empty ifInfo" );
            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		if ( ifInfo->iNumber == aInterface )
			{
			found = ETrue;
			// The FDC tried to claim an interface that was already claimed.
			if(ifInfo->iClaimed)
			    {
                OstTrace0( TRACE_FATAL, CFDF_TOKENFORINTERFACE_DUP2, "iClaimed error" );
                User::Panic(KPanicCategory,__LINE__);
			    }
			ifInfo->iClaimed = ETrue;
			break;
			}
		}
	// Could not find interface in the interface array- the FDC tried to claim
	// an interface it had not been offered.
	if(!found)
	    {
        OstTrace0( TRACE_FATAL, CFDF_TOKENFORINTERFACE_DUP3, "not found" );
        User::Panic(KPanicCategory,__LINE__);
	    }

	if(!iCurrentDevice)
	    {
        OstTrace0( TRACE_FATAL, CFDF_TOKENFORINTERFACE_DUP4, "Empty iCurrentDevice" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	// GetTokenForInterface will return error in the following cases:
	// 1/ KErrBadHandle: invalid device handle (the CDeviceProxy asserts that
	// the handle is valid) because the device has been detached while processing
	// may be due to too much current or cable has been removed
	// so FDF will still return a token of 0 and FDF will handle the proper
	// device detachment when it will be able to process the detachment notification
	//
	// 2/ KErrNotFound: interface not found (if this happens, the FDC has
	// misbehaved, and the correct thing to do is to panic)
	// 3/ KErrInUse: we've already requested a token for that interface
	// (ditto)
	// 4/ KErrOverflow: when 0xFFFFFFFF tokens have been requested (this is a
	// realistic built-in limitation of USBD)


	TInt err = iCurrentDevice->GetTokenForInterface(aInterface, token);
	switch(err)
		{
		case KErrBadHandle:
			token = 0;
			iDeviceDetachedTooEarly = ETrue;

		case KErrNone: // Fall through and do nothing
			break;

		default:
			OstTraceExt2( TRACE_FATAL, CFDF_TOKENFORINTERFACE_DUP5, "Unexpected error %d when requesting token for aInterface %d",err,aInterface);
			User::Panic(KPanicCategory,__LINE__);
			break;
		}

	OstTraceExt2( TRACE_NORMAL, CFDF_TOKENFORINTERFACE_DUP6, 
	        "Token for interface %hhu is = %u",aInterface, token);
	
	OstTraceFunctionExit0( CFDF_TOKENFORINTERFACE_EXIT );
	return token;
	}

CDeviceProxy* CFdf::DeviceProxyL(TUint aDeviceId) const
	{
	OstTraceFunctionEntry0( CFDF_DEVICEPROXYL_ENTRY );
	OstTrace1( TRACE_NORMAL, CFDF_DEVICEPROXYL, "aDeviceId = %d", aDeviceId );
	
	TSglQueIter<CDeviceProxy> iter(const_cast<CFdf*>(this)->iDevices);
	iter.SetToFirst();
	CDeviceProxy* device = NULL;
	while ( ( device = iter++ ) != NULL )
		{
		if ( device->DeviceId() == aDeviceId )
			{
            OstTrace1( TRACE_NORMAL, CFDF_DEVICEPROXYL_DUP1, "device = 0x%08x", device );
			OstTraceFunctionExit0( CFDF_DEVICEPROXYL_EXIT );
			return device;
			}
		}
	OstTrace0( TRACE_NORMAL, CFDF_DEVICEPROXYL_DUP2, "KErrNotFound");
	User::Leave(KErrNotFound);
	OstTraceFunctionExit0( CFDF_DEVICEPROXYL_EXIT_DUP1 );
	return NULL; // avoid warning
	}

const RArray<TUint>& CFdf::GetSupportedLanguagesL(TUint aDeviceId) const
	{
	OstTraceFunctionEntry0( CFDF_GETSUPPORTEDLANGUAGESL_ENTRY );
	OstTrace1( TRACE_NORMAL, CFDF_GETSUPPORTEDLANGUAGESL, "aDeviceId = %d", aDeviceId );
	
	CDeviceProxy* deviceProxy = DeviceProxyL(aDeviceId);
	return deviceProxy->GetSupportedLanguages();
	}

void CFdf::GetManufacturerStringDescriptorL(TUint aDeviceId, TUint32 aLangId, TName& aString) const
	{
	OstTraceFunctionEntry0( CFDF_GETMANUFACTURERSTRINGDESCRIPTORL_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CFDF_GETMANUFACTURERSTRINGDESCRIPTORL, 
	        "aDeviceId = %u, aLangId = 0x%08x", (TUint32)aDeviceId, aLangId );
		
	CDeviceProxy* deviceProxy = DeviceProxyL(aDeviceId);
	deviceProxy->GetManufacturerStringDescriptorL(aLangId, aString);
	OstTraceExt1( TRACE_NORMAL, CFDF_GETMANUFACTURERSTRINGDESCRIPTORL_DUP1, "aString = \"%S\"", aString );
	OstTraceFunctionExit0( CFDF_GETMANUFACTURERSTRINGDESCRIPTORL_EXIT );
	}

void CFdf::GetProductStringDescriptorL(TUint aDeviceId, TUint32 aLangId, TName& aString) const
	{
	OstTraceFunctionEntry0( CFDF_GETPRODUCTSTRINGDESCRIPTORL_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CFDF_GETPRODUCTSTRINGDESCRIPTORL, 
	        "aDeviceId = %u, aLangId = 0x%04x", (TUint32)aDeviceId, aLangId );

	CDeviceProxy* deviceProxy = DeviceProxyL(aDeviceId);
	deviceProxy->GetProductStringDescriptorL(aLangId, aString);
	OstTraceExt1( TRACE_NORMAL, CFDF_GETPRODUCTSTRINGDESCRIPTORL_DUP1, "aString = \"%S\"", aString );
	OstTraceFunctionExit0( CFDF_GETPRODUCTSTRINGDESCRIPTORL_EXIT );
	}

void CFdf::GetOtgDeviceDescriptorL(TInt aDeviceId, TOtgDescriptor& aDescriptor) const
	{
	OstTraceFunctionEntry0( CFDF_GETOTGDEVICEDESCRIPTORL_ENTRY );
	
	DeviceProxyL(aDeviceId)->GetOtgDescriptorL(aDescriptor);
	OstTraceFunctionExit0( CFDF_GETOTGDEVICEDESCRIPTORL_EXIT );
	}

void CFdf::GetConfigurationDescriptorL(TInt aDeviceId, TUsbConfigurationDescriptor& aDescriptor) const
	{
	OstTraceFunctionEntry0( CFDF_GETCONFIGRATIONDESCRIPTORL_ENTRY );
	
	DeviceProxyL(aDeviceId)->GetConfigurationDescriptor(aDescriptor);
	OstTraceFunctionExit0( CFDF_GETCONFIGRATIONDESCRIPTORL_EXIT );
	}


void CFdf::GetSerialNumberStringDescriptorL(TUint aDeviceId, TUint32 aLangId, TName& aString) const
	{
	OstTraceFunctionEntry0( CFDF_GETSERIALNUMBERSTRINGDESCRIPTORL_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CFDF_GETSERIALNUMBERSTRINGDESCRIPTORL, 
	        "aDeviceId = %u, aLangId = 0x%08x", (TUint32)aDeviceId, aLangId );

	CDeviceProxy* deviceProxy = DeviceProxyL(aDeviceId);
	deviceProxy->GetSerialNumberStringDescriptorL(aLangId, aString);
	OstTraceExt1( TRACE_NORMAL, CFDF_GETSERIALNUMBERSTRINGDESCRIPTORL_DUP1, "aString = \"%S\"", aString );
	OstTraceFunctionExit0( CFDF_GETSERIALNUMBERSTRINGDESCRIPTORL_EXIT );
	}

void CFdf::SearchForInterfaceFunctionDriversL(CDeviceProxy& aDevice, TBool& aAnySuccess, TInt& aCollectedErr)
	{
	RArray<TUint> interfacesNumberArray;	
	CleanupClosePushL(interfacesNumberArray);
	
	for ( TUint ii = 0 ; ii < iInterfaces.Count() ; ++ii )
		{
		// At this point we have NOT done any interface level searching yet,
		// and all interfaces should in the Unclaimed status,
		// just simply put them all into the interfacesNumberArray.
		TUint interfaceNumber = iInterfaces[ii]->iNumber; 
		AppendInterfaceNumberToArrayL(aDevice, interfacesNumberArray, interfaceNumber);
		}


	for ( TUint key = EVendorProductDeviceConfigurationvalueInterfacenumber ; key < EMaxInterfaceSearchKey ; ++key )
		{
		// Searching for proper FDCs based on different criteria.
		FindDriversForInterfacesUsingSpecificKeyL(aDevice,
												aCollectedErr,
												aAnySuccess,
												interfacesNumberArray,
												(TInterfaceSearchKeys) key);
							
		// If all the interfaces have been claimed by an FD then there is no point searching for other FDs							
		if (UnclaimedInterfaceCount() == 0)
			{
			break;
			}
		else
			{
			// Put all the unclaimed interface numbers into the array again.
			RebuildUnClaimedInterfacesArrayL(aDevice, interfacesNumberArray);
			}
		}
	CleanupStack::PopAndDestroy(&interfacesNumberArray);				
	}
void  CFdf::RebuildUnClaimedInterfacesArrayL(CDeviceProxy& aDevice, RArray<TUint>& aArray, TUint aOffset)
	{
	aArray.Reset();
	for ( TUint ii = aOffset ; ii < iInterfaces.Count() ; ++ii )
		{
			if (!iInterfaces[ii]->iClaimed)
				{
				TUint interfaceNumber = iInterfaces[ii]->iNumber; 
				AppendInterfaceNumberToArrayL(aDevice, aArray, interfaceNumber);
				}
		}
	}

void CFdf::AppendInterfaceNumberToArrayL(CDeviceProxy& aDevice, RArray<TUint>& aArray, TUint aInterfaceNo) const
	{
	TInt err = aArray.Append(aInterfaceNo);
	if ( err )
		{
		aDevice.SetDriverLoadingEventData(EDriverLoadFailure, err);	
		OstTrace1( TRACE_NORMAL, CFDF_APPENDINTERFACENUMBERTOARRAYL, "Leave with error: %d", err );
		User::Leave(err);
		}
	}
	


TBool CFdf::SearchForADeviceFunctionDriverL(CDeviceProxy& aDevice, TBool& aAnySuccess, TInt& aCollectedErr)
	{			
	
	RArray<TUint> interfaces;
	CleanupClosePushL(interfaces);
	
	for (TUint ii = 0; ii < iInterfaces.Count(); ++ii)
		{
		TUint interfaceNumber = iInterfaces[ii]->iNumber; 
		AppendInterfaceNumberToArrayL(aDevice, interfaces, interfaceNumber);
		}
	
	TBool foundFdc = EFalse;		
	for (TUint key = EVendorProductDevice; key < EMaxDeviceSearchKey; ++key)
		{		
		
		if (key == EVendorDevicesubclassDeviceprotocol && iDD.DeviceClass() != KVendorSpecificDeviceClassValue)
			continue;
		if (key == EVendorDevicesubclass && iDD.DeviceClass() != KVendorSpecificDeviceClassValue)
			continue;
		if (key == EDeviceclassDevicesubclassDeviceprotocol && iDD.DeviceClass() == KVendorSpecificDeviceClassValue)
			continue;
		if (key == EDeviceclassDevicesubclass && iDD.DeviceClass() == KVendorSpecificDeviceClassValue)
			continue;			
		
		TBuf8<KMaxSearchKeyLength> searchKeyString;
		FormatDeviceSearchKey(searchKeyString, (TDeviceSearchKeys)key);

		// Find an FDC matching this search key.
		TSglQueIter<CFdcProxy> iter(iFunctionDrivers);
		iter.SetToFirst();
		CFdcProxy* fdc;
		while ( ( fdc = iter++ ) != NULL)
			{
			if (fdc->MarkedForDeletion())
				continue;
			OstTraceExt1( TRACE_NORMAL, CFDF_SEARCHFORADEVICEFUNCTIONDRIVERL, "FDC's default_data field = \"%S\"", fdc->DefaultDataField());
			
	// having these two together in the debug window is helpful for interactive debugging
	TBuf8<KMaxSearchKeyLength> fd_key;
	fd_key.Append(fdc->DefaultDataField().Ptr(), fdc->DefaultDataField().Length() > KMaxSearchKeyLength ? KMaxSearchKeyLength : fdc->DefaultDataField().Length());	
	TBuf8<KMaxSearchKeyLength> search_key;
	search_key.Append(searchKeyString.Ptr(), searchKeyString.Length() > KMaxSearchKeyLength ? KMaxSearchKeyLength : searchKeyString.Length());
	TInt version = fdc->Version();

			if (searchKeyString.CompareF(fdc->DefaultDataField()) == 0)
				{
				
				// If there is more than one matching FD then if all of them are in RAM we simply choose the first one we find.
				// (Similarly if they are all in ROM we choose the first one although this situation should not arise as a device
				// manufacturer should not put two matching FDs into ROM).
				// However if there are matching FDs in ROM and RAM then the one in ROM should be selected in preference to
				// any in RAM. Hence at this point if the matching FD we have found is in RAM then we need to scan the list
				// of FDs to see if there is also a matching one in ROM and if so we'll skip this iteration of the loop.
				//if (!fdc->RomBased() && FindMatchingRomBasedFD(searchKeyString))
				//	continue;
				if (FindMultipleFDs(searchKeyString, iter))
					{
					aDevice.SetMultipleDriversFlag();
					}
				
				foundFdc = ETrue;
				OstTrace1( TRACE_NORMAL, CFDF_SEARCHFORADEVICEFUNCTIONDRIVERL_DUP1, "found matching FDC (0x%08x)", fdc );
				TInt err = fdc->NewFunction(aDevice.DeviceId(), interfaces, iDD, iCD);
				OstTrace1( TRACE_NORMAL, CFDF_SEARCHFORADEVICEFUNCTIONDRIVERL_DUP2, "NewFunction returned %d", err);
				// To correctly determine whether the driver load for the whole
				// configuration was a complete failure, a partial success or a
				// complete success, we need to collect any non-KErrNone error
				// from this, and whether any handovers worked at all.
				if ( err == KErrNone )
					{
					aAnySuccess = ETrue;
					}
				else
					{
					aCollectedErr = err;
					}
				break; 	// We found a matching FDC so no need to look for more.
				}
			}
		if (foundFdc)
			break;
		} // end of for
	CleanupStack::PopAndDestroy(&interfaces);
	return foundFdc;
		
	} // end of function




//
// Search the list of FDs looking for which matches with aSearchKey and is rom based and return true if found.
//
// added for Multiple FDs
TBool CFdf::FindMultipleFDs(const TDesC8& aSearchKey,TSglQueIter<CFdcProxy>& aFdcIter)
	{
	CFdcProxy* fdc;
	while ( ( fdc = aFdcIter++ ) != NULL)
		{
		if (!fdc->MarkedForDeletion() &&  (aSearchKey.CompareF(fdc->DefaultDataField()) == 0))
			return ETrue;
		}
		
	return EFalse;
	}

//
// Format the string aSearchKey according to aSearchKeys to search for Device Functions drivers
//
void CFdf::FormatDeviceSearchKey(TDes8& aSearchKey, TDeviceSearchKeys aDeviceSearchKeys)
	{
	OstTraceFunctionEntry0( CFDF_FORMATDEVICESEARCHKEY_ENTRY );
	
	switch (aDeviceSearchKeys)
		{
		case EVendorProductDevice:
			{
			_LIT8(KTemplateV_P_D, "V0x%04xP0x%04xD0x%04x");
			aSearchKey.Format(KTemplateV_P_D(), iDD.VendorId(), iDD.ProductId(), iDD.DeviceBcd());
			break;
			}
		case EVendorProduct:
			{
			_LIT8(KTemplateV_P, "V0x%04xP0x%04x");
			aSearchKey.Format(KTemplateV_P(), iDD.VendorId(), iDD.ProductId());
			break;
			}
		case EVendorDevicesubclassDeviceprotocol:
			{
			_LIT8(KTemplateV_DSC_DP, "V0x%04xDSC0x%02xDP0x%02x");
			aSearchKey.Format(KTemplateV_DSC_DP(), iDD.VendorId(), iDD.DeviceSubClass(), iDD.DeviceProtocol());
			break;
			}
		case EVendorDevicesubclass:
			{
			_LIT8(KTemplateV_DSC, "V0x%04xDSC0x%02x");
			aSearchKey.Format(KTemplateV_DSC(), iDD.VendorId(), iDD.DeviceSubClass());
			break;
			}
		case EDeviceclassDevicesubclassDeviceprotocol:
			{
			_LIT8(KTemplateDC_DSC_DP, "DC0x%02xDSC0x%02xDP0x%02x");
			aSearchKey.Format(KTemplateDC_DSC_DP(), iDD.DeviceClass(), iDD.DeviceSubClass(), iDD.DeviceProtocol());
			break;
			}
		case EDeviceclassDevicesubclass:
			{
			_LIT8(KTemplateDC_DSC, "DC0x%02xDSC0x%02x");
			aSearchKey.Format(KTemplateDC_DSC(), iDD.DeviceClass(), iDD.DeviceSubClass());
			break;
			}
		default:
			{	
			OstTrace1( TRACE_FATAL, CFDF_FORMATDEVICESEARCHKEY, "Invalid aDeviceSearchKeys=%d", aDeviceSearchKeys );
			__ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
			}		
		}
	OstTraceExt1( TRACE_NORMAL, CFDF_FORMATDEVICESEARCHKEY_DUP1, "aSearchKey = \"%s\"", aSearchKey );
	OstTraceFunctionExit0( CFDF_FORMATDEVICESEARCHKEY_EXIT );
	}
	
	
	
	

//
// Format the string aSearchKey according to aSearchKeys to search for Interface Functions drivers
//	
void CFdf::FormatInterfaceSearchKey(TDes8& aSearchKey, TInterfaceSearchKeys aSearchKeys, const TInterfaceInfo& aIfInfo)
	{
	OstTraceFunctionEntry0( CFDF_FORMATINTERFACESEARCHKEY_ENTRY );
	
	switch (aSearchKeys)
		{
		case EVendorProductDeviceConfigurationvalueInterfacenumber:
			{
			_LIT8(KTemplateV_P_D_CV_IN, "V0x%04xP0x%04xD0x%04xCV0x%02xIN0x%02x");		
			aSearchKey.Format(KTemplateV_P_D_CV_IN(), iDD.VendorId(), iDD.ProductId(), iDD.DeviceBcd(), iCD.ConfigurationValue(), aIfInfo.iNumber);
			break;
			}
		case EVendorProductConfigurationValueInterfacenumber:
			{
			_LIT8(KTemplateV_P_CV_IN, "V0x%04xP0x%04xCV0x%02xIN0x%02x");
			aSearchKey.Format(KTemplateV_P_CV_IN(), iDD.VendorId(), iDD.ProductId(), iCD.ConfigurationValue(), aIfInfo.iNumber);
			break;
			}
		case EVendorInterfacesubclassInterfaceprotocol:
			{
			_LIT8(KTemplateV_ISC_IP, "V0x%04xISC0x%02xIP0x%02x");
			aSearchKey.Format(KTemplateV_ISC_IP(), iDD.VendorId(), aIfInfo.iSubclass, aIfInfo.iProtocol);
			break;
			}
		case EVendorInterfacesubclass:
			{
			_LIT8(KTemplateV_ISC, "V0x%04xISC0x%02x");
			aSearchKey.Format(KTemplateV_ISC(), iDD.VendorId(), aIfInfo.iSubclass);
			break;
			}
		case EInterfaceclassInterfacesubclassInterfaceprotocol:
			{
			_LIT8(KTemplateIC_ISC_IP, "IC0x%02xISC0x%02xIP0x%02x");
			aSearchKey.Format(KTemplateIC_ISC_IP(), aIfInfo.iClass, aIfInfo.iSubclass, aIfInfo.iProtocol);
			break;
			}
		case EInterfaceclassInterfacesubclass:
			{
			_LIT8(KTemplateIC_ISC, "IC0x%02xISC0x%02x");
			aSearchKey.Format(KTemplateIC_ISC(), aIfInfo.iClass, aIfInfo.iSubclass);
			break;
			}
		default:
			{
			OstTrace1( TRACE_FATAL, CFDF_FORMATINTERFACESEARCHKEY, "Invalid aSearchKeys=%d", aSearchKeys );
			__ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
			}
		}
	OstTraceExt1( TRACE_NORMAL, CFDF_FORMATINTERFACESEARCHKEY_DUP1, "aSearchKey = \"%s\"", aSearchKey );
	OstTraceFunctionExit0( CFDF_FORMATINTERFACESEARCHKEY_EXIT );
	}


TUint CFdf::UnclaimedInterfaceCount() const
	{
	OstTraceFunctionEntry0( CFDF_UNCLAIMEDINTERFACECOUNT_ENTRY );
		
	TUint unclaimedInterfaces = 0;
	for ( TUint ii = 0 ; ii < iInterfaces.Count() ; ++ii )
		{
		TInterfaceInfo* ifInfo = iInterfaces[ii];
		
		if(!ifInfo)
		    {
            OstTrace0( TRACE_FATAL, CFDF_UNCLAIMEDINTERFACECOUNT, "Empty ifInfo" );
            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		if ( !ifInfo->iClaimed )
			{
            OstTrace1( TRACE_NORMAL, CFDF_UNCLAIMEDINTERFACECOUNT_DUP1, "unclaimed interface: ifInfo->iNumber = %d", ifInfo->iNumber );
			++unclaimedInterfaces;
			}
		}
	OstTrace1( TRACE_NORMAL, CFDF_UNCLAIMEDINTERFACECOUNT_DUP2, "unclaimedInterfaces = \"%d\"", unclaimedInterfaces );
	OstTraceFunctionExit0( CFDF_UNCLAIMEDINTERFACECOUNT_EXIT );
	return unclaimedInterfaces;
	}
	
	
void CFdf::SetFailureStatus(TInt aUnclaimedInterfaces, TInt aInterfaceCount, TBool aAnySuccess, TBool aCollectedErr, CDeviceProxy& aDevice)
	{
	const TUint KMultipleDriverFound = aDevice.MultipleDriversFlag()?KMultipleDriversFound : 0;
	
	if (aUnclaimedInterfaces)
		{
		if(aUnclaimedInterfaces == aInterfaceCount)
			{
			// complete failure
			aDevice.SetDriverLoadingEventData((TDriverLoadStatus)(EDriverLoadFailure|KMultipleDriverFound), KErrUsbFunctionDriverNotFound);
			}
		else
			{// at that stage because we have unclaimed interfaces it means that
			// depending on anySuccess we have a failure or a partial success
			TDriverLoadStatus status = (aAnySuccess)? EDriverLoadPartialSuccess:EDriverLoadFailure;
			aDevice.SetDriverLoadingEventData((TDriverLoadStatus)(status|KMultipleDriverFound), KErrUsbFunctionDriverNotFound);
			}
		}
	else
		{
		if (aCollectedErr)
			{
			// There were no unclaimed interfaces, but an error was expressed.
			// This is either a partial success or a complete failure scenario.
			TDriverLoadStatus status = aAnySuccess ? EDriverLoadPartialSuccess : EDriverLoadFailure;
			aDevice.SetDriverLoadingEventData((TDriverLoadStatus)(status|KMultipleDriverFound), aCollectedErr);
			}
		else
			{
			// There were no unclaimed interfaces, and no error reported.
			aDevice.SetDriverLoadingEventData((TDriverLoadStatus)(EDriverLoadSuccess|KMultipleDriverFound));
			}
		}
	}	

