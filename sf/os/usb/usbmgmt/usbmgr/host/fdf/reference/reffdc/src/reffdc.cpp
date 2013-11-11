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

#include "reffdc.h"
#include <usb/usblogger.h>
#include <usbhost/internal/fdcpluginobserver.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "reffdcTraces.h"
#endif



CRefFdc* CRefFdc::NewL(MFdcPluginObserver& aObserver)
	{
	OstTraceFunctionEntry0( CREFFDC_NEWL_ENTRY );
	
	CRefFdc* self = new(ELeave) CRefFdc(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CREFFDC_NEWL_EXIT );
	return self;
	}

CRefFdc::~CRefFdc()
	{
	OstTraceFunctionEntry0( CREFFDC_CREFFDC_DES_ENTRY );
	OstTraceFunctionExit0( CREFFDC_CREFFDC_DES_EXIT );
	}

CRefFdc::CRefFdc(MFdcPluginObserver& aObserver)
:	CFdcPlugin(aObserver)
	{
	}

void CRefFdc::ConstructL()
	{
    OstTraceFunctionEntry0( CREFFDC_CONSTRUCTL_ENTRY );
    
	
	OstTraceFunctionExit0( CREFFDC_CONSTRUCTL_EXIT );
	}

TInt CRefFdc::Mfi1NewFunction(TUint aDeviceId,
		const TArray<TUint>& aInterfaces,
		const TUsbDeviceDescriptor& aDeviceDescriptor,
		const TUsbConfigurationDescriptor& aConfigurationDescriptor)
	{
	OstTraceFunctionEntry0( CREFFDC_MFI1NEWFUNCTION_ENTRY );
	OstTrace1( TRACE_NORMAL, CREFFDC_MFI1NEWFUNCTION, 
	        "***** Ref FD offered chance to claim one function from device with ID %d", 
	        aDeviceId );
	
	(void)aDeviceId;

	TRAPD(err, NewFunctionL(aDeviceId, aInterfaces, aDeviceDescriptor, aConfigurationDescriptor));

	// If any error is returned, RUsbInterface (etc) handles opened from this 
	// call must be closed.
	OstTrace1( TRACE_NORMAL, CREFFDC_MFI1NEWFUNCTION_DUP1, 
	            "err = %d", err );
	OstTraceFunctionExit0( CREFFDC_MFI1NEWFUNCTION_EXIT );
	return err;
	}

void CRefFdc::NewFunctionL(TUint aDeviceId,
		const TArray<TUint>& aInterfaces,
		const TUsbDeviceDescriptor& /*aDeviceDescriptor*/,
		const TUsbConfigurationDescriptor& /*aConfigurationDescriptor*/)
	{
	OstTraceFunctionEntry0( CREFFDC_NEWFUNCTIONL_ENTRY );
	
	// We are obliged to claim the first interface because it has 
	// interface class/subclass(/protocol) settings matching our default_data 
	// field.
	// We must take further interfaces until we have claimed a single complete 
	// function. We must do this regardless of any error that occurs.
	TUint32 token = Observer().TokenForInterface(aInterfaces[0]);
	// The token may now be used to open a RUsbInterface handle.
	(void)token;

	// aDeviceDescriptor is accurate but currently useless. FDCs may not 
	// 'reject' a device on any basis.

	// aConfigurationDescriptor may be walked to find interface descriptors 
	// matching interface numbers in aInterfaces.

	// Illustrate how string descriptors may be obtained for 
	// subsystem-specific purposes.
	const RArray<TUint>& langIds = Observer().GetSupportedLanguagesL(aDeviceId);
	const TUint langCount = langIds.Count();
	OstTrace1( TRACE_NORMAL, CREFFDC_NEWFUNCTIONL, 
	        "device supports %d language(s):", langCount );
	
	for ( TUint ii = 0 ; ii < langCount ; ++ii )
		{
        OstTrace1( TRACE_NORMAL, CREFFDC_NEWFUNCTIONL_DUP1, 
	            "lang code: 0x%04x", langIds[ii] );
	   
        TName string;
		TInt err = Observer().GetManufacturerStringDescriptor(aDeviceId, langIds[ii], string);
		if ( !err )
			{
            OstTraceExt1( TRACE_NORMAL, CREFFDC_NEWFUNCTIONL_DUP2, 
		                "manufacturer string descriptor = \"%S\"", string );
		    err = Observer().GetProductStringDescriptor(aDeviceId, langIds[ii], string);
			if ( !err )
				{
                OstTraceExt1( TRACE_NORMAL, CREFFDC_NEWFUNCTIONL_DUP3, 
			                        "product string descriptor = \"%S\"", string );
				err = Observer().GetSerialNumberStringDescriptor(aDeviceId, langIds[ii], string);
				if ( !err )
					{
                    OstTraceExt1( TRACE_NORMAL, CREFFDC_NEWFUNCTIONL_DUP4, 
				                   "serial number string descriptor = \"%S\"", string );
					}
				else
					{
                    OstTrace1( TRACE_NORMAL, CREFFDC_NEWFUNCTIONL_DUP5, 
				               "GetSerialNumberStringDescriptor returned %d", err );
					}
				}
			else
				{
                OstTrace1( TRACE_NORMAL, CREFFDC_NEWFUNCTIONL_DUP6, 
			                "GetProductStringDescriptor returned %d", err );
			    }
			}
		else
			{
            OstTrace1( TRACE_NORMAL, CREFFDC_NEWFUNCTIONL_DUP7, 
		                            "GetManufacturerStringDescriptor returned %d", err );
			}
		}
	OstTraceFunctionExit0( CREFFDC_NEWFUNCTIONL_EXIT );
	}

void CRefFdc::Mfi1DeviceDetached(TUint aDeviceId)
	{
	OstTraceFunctionEntry0( CREFFDC_MFI1DEVICEDETACHED_ENTRY );
	OstTrace1( TRACE_NORMAL, CREFFDC_MFI1DEVICEDETACHED, 
	        "***** Ref FD notified of detachment of device with ID %d", aDeviceId );
	(void)aDeviceId;

	// Any RUsbInterface (etc) handles opened as a result of any calls to 
	// MfiNewFunction with this device ID should be closed.
	OstTraceFunctionExit0( CREFFDC_MFI1DEVICEDETACHED_EXIT );
	}

TAny* CRefFdc::GetInterface(TUid aUid)
	{
	OstTraceFunctionEntry0( CREFFDC_GETINTERFACE_ENTRY );
	OstTrace1( TRACE_NORMAL, CREFFDC_GETINTERFACE, 
	        "aUid = 0x%08x", aUid.iUid );
	
	TAny* ret = NULL;
	if ( aUid == TUid::Uid(KFdcInterfaceV1) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MFdcInterfaceV1*>(this)
			);
		}

	OstTrace1( TRACE_NORMAL, CREFFDC_GETINTERFACE_DUP1, 
	            "ret = [0x%08x]", ret );
	    
	OstTraceFunctionExit0( CREFFDC_GETINTERFACE_EXIT );
	return ret;
	}
