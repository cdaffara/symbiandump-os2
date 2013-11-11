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

#include "msfdc.h"
#include "utils.h"
#include <d32usbc.h>
#ifndef __DUMMYFDF__
#include <usbhost/internal/fdcpluginobserver.h>
#include <d32usbdi.h>
#else
#include <dummyfdcpluginobserver.h>
#include <dummyusbinterface.h>
#endif
#include <d32usbdescriptors.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "msfdcTraces.h"
#endif


/**
  NewL function of CMsFdc, allocate the memory that needed for instantiating this object.
 
  @param	aObserver   this is a pointer to the Observer object(FDF), MsFdc will get
                       informations from FDF.
  @return	A pointer to this CMsFdc object
 */
CMsFdc* CMsFdc::NewL(MFdcPluginObserver& aObserver)
	{
	OstTraceFunctionEntry0( CMSFDC_NEWL_ENTRY );
	
	CMsFdc* self = new(ELeave) CMsFdc(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CMSFDC_NEWL_EXIT );
	return self;
	} 

/**
  Destructor of CMsFdc.
 */
CMsFdc::~CMsFdc()
	{
	OstTraceFunctionEntry0( CMSFDC_CMSFDC_DES_ENTRY );
	iMsmmSession.Disconnect();
	OstTrace0( TRACE_NORMAL, CMSFDC_CMSFDC, "Disconnected to MSMM OK" );

	OstTraceFunctionExit0( CMSFDC_CMSFDC_DES_EXIT );
	}
/**
  Constructor of CMsFdc.
 */
CMsFdc::CMsFdc(MFdcPluginObserver& aObserver)
:	CFdcPlugin(aObserver)
	{
	}
/**
  The Second phase construction of CMsFdc.
 */
void CMsFdc::ConstructL()
	{
	OstTraceFunctionEntry0( CMSFDC_CONSTRUCTL_ENTRY );
	
	//Set up the connection with mount manager
	TInt error = iMsmmSession.Connect();
	if ( error )
		{
        OstTrace1( TRACE_NORMAL, CMSFDC_CONSTRUCTL, 
                "Failed to connect to MSMM %d",error );
        User::Leave(error);
		}
	else
		{
        OstTrace0( TRACE_NORMAL, CMSFDC_CONSTRUCTL_DUP1, 
                "Connected to MSMM OK" );
 		}
	OstTraceFunctionExit0( CMSFDC_CONSTRUCTL_EXIT );
	}
/**
  Get called when FDF is trying to load the driver for Mass Storage Device. 
 
  @param	aDevice  	 				Device ID allocated by FDF for the newly inserted device
  @param	aInterfaces					The interface array that contains interfaces to be claimed by this FDC
                                       msfdc just claims the first one in this array.	
  @param	aDeviceDescriptor			The device descriptor of the newly inserted device.
  @param	aConfigurationDescriptor	The configuration descriptor of the newly inserted device.
  @return	Any error that occurred or KErrNone
 */
TInt CMsFdc::Mfi1NewFunction(TUint aDeviceId,
		const TArray<TUint>& aInterfaces,
		const TUsbDeviceDescriptor& aDeviceDescriptor,
		const TUsbConfigurationDescriptor& aConfigurationDescriptor)
	{
	OstTraceFunctionEntry0( CMSFDC_MFI1NEWFUNCTION_ENTRY );
	 
	// this is the evidence that the message got through.
	OstTrace1( TRACE_NORMAL, CMSFDC_MFI1NEWFUNCTION, 
	        "***** Mass Storage FD notified of device (ID %d) attachment!", aDeviceId );	

	// Mass Storage FDC only claims one interface.
	OstTrace1( TRACE_NORMAL, CMSFDC_MFI1NEWFUNCTION_DUP1, 
	            "***** Mass Storage FD interface to request token is %d", aInterfaces[0] );    
	TUint32 token = Observer().TokenForInterface(aInterfaces[0]);
	OstTrace1( TRACE_NORMAL, CMSFDC_MFI1NEWFUNCTION_DUP2, 
	        "***** Mass Storage FD tokenInterface  %d", token );
	if (token == 0)
		{
        OstTrace0( TRACE_ERROR, CMSFDC_MFI1NEWFUNCTION_DUP3, 
	            "***** Mass Storage FDC device containing this function is removed." );
		OstTraceFunctionExit0( CMSFDC_MFI1NEWFUNCTION_EXIT );
		return KErrGeneral;
		}

	//Get the languages that is supported by this device.
	TUint defaultlangid = 0;
	TRAPD(error, GetDefaultLanguageL(aDeviceId, defaultlangid));

	if (error)
		{
        OstTrace0( TRACE_ERROR, CMSFDC_MFI1NEWFUNCTION_DUP4, 
	                "***** Mass Storage FDC getting language array failed" );
		OstTraceFunctionExit0( CMSFDC_MFI1NEWFUNCTION_EXIT_DUP1 );
		return error;
		}
	
	TUSBMSDeviceDescription* data = NULL;
	TRAP(error, data = new (ELeave) TUSBMSDeviceDescription);
	if (error)
		{
        OstTrace0( TRACE_ERROR, CMSFDC_MFI1NEWFUNCTION_DUP5, 
	                    "***** Mass Storage FDC Memory allocation Failed" );
		OstTraceFunctionExit0( CMSFDC_MFI1NEWFUNCTION_EXIT_DUP2 );
		return error;
		}

	//Get Serial number from string descriptor
	error = Observer().GetSerialNumberStringDescriptor(aDeviceId, defaultlangid, 
			data->iSerialNumber);
	
	if (error)
		{
        OstTrace0( TRACE_ERROR, CMSFDC_MFI1NEWFUNCTION_DUP6, 
                    "***** Mass Storage FDC getting Serial Number Failed" );
		delete data;
		OstTraceFunctionExit0( CMSFDC_MFI1NEWFUNCTION_EXIT_DUP3 );
		return error;
		}
	else
		{
        OstTraceExt1( TRACE_ERROR, CMSFDC_MFI1NEWFUNCTION_DUP7, 
	                    "***** Mass Storage FDC Serial String is %S", data->iSerialNumber );
	    }
	//Get Product string descriptor
	error = Observer().GetProductStringDescriptor(aDeviceId, defaultlangid, data->iProductString);
	

	if (error)
		{
        OstTrace0( TRACE_ERROR, CMSFDC_MFI1NEWFUNCTION_DUP8, 
	                    "***** Mass Storage FDC getting Product string Failed" );
	    delete data;
		OstTraceFunctionExit0( CMSFDC_MFI1NEWFUNCTION_EXIT_DUP4 );
		return error;
		}
	else
		{
        OstTraceExt1( TRACE_NORMAL, CMSFDC_MFI1NEWFUNCTION_DUP9, 
	                        "***** Mass Storage FDC Product String is %S", data->iProductString );
		}

	//Get Manufacturer string descriptor
	error = Observer().GetManufacturerStringDescriptor(aDeviceId, defaultlangid, 
			data->iManufacturerString);
	
	if (error)
		{
        OstTrace0( TRACE_ERROR, CMSFDC_MFI1NEWFUNCTION_DUP10, 
                    "***** Mass Storage FDC getting Manufacturer string Failed" );
		delete data;
		OstTraceFunctionExit0( CMSFDC_MFI1NEWFUNCTION_EXIT_DUP5 );
		return error;
		}
	else
		{
        OstTraceExt1( TRACE_NORMAL, CMSFDC_MFI1NEWFUNCTION_DUP11, 
	                            "***** Mass Storage FDC Manufacturer String is %S", data->iManufacturerString );
		}	
	
	/************************Remote Wakeup Attribute acquiring***********************/
	TUint8 attr = aConfigurationDescriptor.Attributes();

	/************************Protocol ID & Transport ID******************************/
	RUsbInterface interface_ep0;
    TUsbInterfaceDescriptor ifDescriptor;
    error = interface_ep0.Open(token);
    if (error)
    	{
        OstTrace0( TRACE_ERROR, CMSFDC_MFI1NEWFUNCTION_DUP12, 
                        "***** Mass Storage FDC Open interface handle Failed" );
    	delete data;
		OstTraceFunctionExit0( CMSFDC_MFI1NEWFUNCTION_EXIT_DUP6 );
		return error;
    	}
    else
    	{
        OstTrace0( TRACE_NORMAL, CMSFDC_MFI1NEWFUNCTION_DUP13, 
                         "***** Mass Storage FDC Open interface handle OK" );
    	}

    error = interface_ep0.GetInterfaceDescriptor(ifDescriptor);
    if (error)
    	{
        OstTrace0( TRACE_ERROR, CMSFDC_MFI1NEWFUNCTION_DUP14, 
                        "***** Mass Storage FDC get interface descriptor Failed" );
 		interface_ep0.Close();
		delete data;
		OstTraceFunctionExit0( CMSFDC_MFI1NEWFUNCTION_EXIT_DUP7 );
		return error;
    	}
    else
    	{
        OstTrace0( TRACE_NORMAL, CMSFDC_MFI1NEWFUNCTION_DUP15, 
                         "***** Mass Storage FDC get interface descriptor OK" );
    	}
	
	/*********************************************************************************/
	
	//Send informations to Mass Storage Mount Manager
	
	data->iConfigurationNumber   = aDeviceDescriptor.NumConfigurations();
	data->iBcdDevice             = aDeviceDescriptor.DeviceBcd();
	data->iDeviceId              = aDeviceId;
	data->iProductId             = aDeviceDescriptor.ProductId();
	data->iVendorId              = aDeviceDescriptor.VendorId();
	
	/*********************************************************************************/
	data->iProtocolId  = ifDescriptor.InterfaceSubClass();
	data->iTransportId = ifDescriptor.InterfaceProtocol();
	
	data->iRemoteWakeup = attr&0x20;  	// Bit 5 indicates the remote wakeup feature.
	data->iIsOtgClient = 0;				// Put 0 into iIsOtgclient for now.
	/*********************************************************************************/

	//This OTG information may need to be changed when OTG descriptor becomes available.
	data->iOtgInformation        = aDeviceDescriptor.DeviceBcd();
	
	error = iMsmmSession.AddFunction(*data, aInterfaces[0], token);
	
	interface_ep0.Close();
	delete data;
	OstTraceFunctionExit0( CMSFDC_MFI1NEWFUNCTION_EXIT_DUP8 );
	return error;
	}
/**
  Get called when FDF unload the function controller of the removed device.
 
  @param	aDeviceId	The device ID that indicates that which device's been removed.
 */
void CMsFdc::Mfi1DeviceDetached(TUint aDeviceId)
	{
	OstTraceFunctionEntry0( CMSFDC_MFI1DEVICEDETACHED_ENTRY );
    // this is the evidence that the message got through.
	OstTrace1( TRACE_NORMAL, CMSFDC_MFI1DEVICEDETACHED, 
	        "***** Mass Storage FD notified of device (ID %d) detachment!", aDeviceId );
	iMsmmSession.RemoveDevice(aDeviceId);

	OstTraceFunctionExit0( CMSFDC_MFI1DEVICEDETACHED_EXIT );
	}

/**
  Convert the pointer of this CMsFdc object to a pointer to TAny
 
  @param	aUid	A UID that indicate the interface that is needed..
  @return	this pointer if aUid equals to the interface uid of CMsFdc or otherwise NULL.
 */
TAny* CMsFdc::GetInterface(TUid aUid)
	{
	OstTraceFunctionEntry0( CMSFDC_GETINTERFACE_ENTRY );
	OstTrace1( TRACE_NORMAL, CMSFDC_GETINTERFACE, "aUid = 0x%08x", aUid.iUid );
	
	TAny* ret = NULL;
	if ( aUid == TUid::Uid(KFdcInterfaceV1) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MFdcInterfaceV1*>(this)
			);
		}
	OstTrace1( TRACE_NORMAL, CMSFDC_GETINTERFACE_DUP1, 
        "ret = [0x%08x]", ret );
	OstTraceFunctionExit0( CMSFDC_GETINTERFACE_EXIT );
	return ret;
	}
/**
  Get the default language ID that is supported by this Mass Storage device.
 
  @param	aDeviceId		Device ID allocated by FDF
  @param	aDefaultLangId	The first Language ID that supported by this device.
  @return	KErrNone is everything is alright or KErrNotFound if the SupportedLanguage of 
  			the device are unavailable.
 */
TInt CMsFdc::GetDefaultLanguageL(TUint aDeviceId, TUint& aDefaultLangId)
{
	const RArray<TUint>& languagearray = Observer().GetSupportedLanguagesL(aDeviceId);
	if (languagearray.Count() <= 0)
		{
		return KErrNotFound; 
		}
	aDefaultLangId = languagearray[0];
	return KErrNone;
}


