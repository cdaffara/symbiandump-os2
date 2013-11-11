/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements the main object of Usbman that manages all the USB Classes
* and the USB Logical Device (via CUsbDeviceStateWatcher).
*
*/

/**
 @file
*/
#include <cusbclasscontrolleriterator.h>
#include <cusbclasscontrollerbase.h>
#include <cusbclasscontrollerplugin.h>
#include <bafl/sysutil.h>
#include <e32svr.h>
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <barsc.h>
#include <barsread.h>
#include <bautils.h>
#ifndef __DUMMY_LDD__
#include <cusbmanextensionplugin.h>
#else
#include <cusbmanextensionplugindummy.h>
#endif
#include <e32property.h> //Publish & Subscribe header
#ifdef USE_DUMMY_CLASS_CONTROLLER
#include "CUsbDummyClassController.h"
#endif
#include "MUsbDeviceNotify.h"
#include "UsbSettings.h"
#include "CUsbServer.h"
#include "UsbUtils.h"
#include "CUsbDevice.h"
#include "CUsbDeviceStateWatcher.h"
#include "cusbchargingporttypewatcher.h"
#include "CPersonality.h"
#include "usbmancenrepmanager.h"
#include "usbmanprivatecrkeys.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbDeviceTraces.h"
#endif


_LIT(KUsbLDDName, "eusbc"); //Name used in call to User::LoadLogicalDevice
_LIT(KUsbLDDFreeName, "Usbc"); //Name used in call to User::FreeLogicalDevice


// Panic category only used in debug builds
#ifdef _DEBUG
_LIT(KUsbDevicePanicCategory, "UsbDevice");
#endif

/**
 * Panic codes for the USB Device Class
 */
enum TUsbDevicePanic
	{
	/** Class called while in an illegal state */
	EBadAsynchronousCall = 0,
	EConfigurationError,
	EResourceFileNotFound,
	/** ConvertUidsL called with an array that is not empty */
	EUidArrayNotEmpty,
	};


CUsbDevice* CUsbDevice::NewL(CUsbServer& aUsbServer)
/**
 * Constructs a CUsbDevice object.
 *
 * @return	A new CUsbDevice object
 */
	{
    OstTraceFunctionEntry0( CUSBDEVICE_NEWL_ENTRY );

	CUsbDevice* r = new (ELeave) CUsbDevice(aUsbServer);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop(r);
	OstTraceFunctionExit0( CUSBDEVICE_NEWL_EXIT );
	return r;
	}


CUsbDevice::~CUsbDevice()
/**
 * Destructor.
 */
	{
    OstTraceFunctionEntry0( CUSBDEVICE_CUSBDEVICE_DES_ENTRY );

	// Cancel any outstanding asynchronous operation.
	Cancel();
	
	delete iUsbClassControllerIterator;
	iSupportedClasses.ResetAndDestroy();
	iSupportedPersonalities.ResetAndDestroy();
	iSupportedClassUids.Close();

	iExtensionPlugins.ResetAndDestroy();
	
	delete iCenRepManager;
	
	delete  iDeviceConfiguration.iManufacturerName;
	delete  iDeviceConfiguration.iProductName;  

	if(iEcom)
		iEcom->Close();
	REComSession::FinalClose();

	// Free any memory allocated to the list of observers. Note that
	// we don't want to call ResetAndDestroy, because we don't own
	// the observers themselves.
	iObservers.Reset();
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
	iChargingObservers.Reset();
	delete iChargerTypeWatcher;
#endif

#ifndef __OVER_DUMMYUSBDI__
#ifndef __WINS__
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_CUSBDEVICE, "CUsbDevice::~CUsbDevice;about to delete device state watcher @ %08x", (TUint32)iDeviceStateWatcher );
	
	delete iDeviceStateWatcher;
	OstTrace0( TRACE_NORMAL, CUSBDEVICE_CUSBDEVICE_DUP1, "CUsbDevice::~CUsbDevice;deleted device state watcher" );

	iLdd.Close();

	OstTrace0( TRACE_NORMAL, CUSBDEVICE_CUSBDEVICE_DUP2, "CUsbDevice::~CUsbDevice;Freeing logical device" );
	
	TInt err = User::FreeLogicalDevice(KUsbLDDFreeName);
	//Putting the LOGTEXT2 inside the if statement prevents a compiler
	//warning about err being unused in UREL builds.
	if(err)
		{
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_CUSBDEVICE_DUP3, "CUsbDevice::~CUsbDevice; User::FreeLogicalDevice returned err=%d", err );
		}
#endif
#endif	

	delete iDefaultSerialNumber;
	OstTraceFunctionExit0( CUSBDEVICE_CUSBDEVICE_DES_EXIT );
	}


CUsbDevice::CUsbDevice(CUsbServer& aUsbServer)
	: CActive(EPriorityStandard)
	, iDeviceState(EUsbDeviceStateUndefined)
	, iServiceState(EUsbServiceIdle)
	, iUsbServer(aUsbServer)
	, iPersonalityCfged(EFalse)
/**
 * Constructor.
 */
	{
	CActiveScheduler::Add(this);
	}


void CUsbDevice::ConstructL()
/**
 * Performs 2nd phase construction of the USB device.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_CONSTRUCTL_ENTRY );
	
	iEcom = &(REComSession::OpenL());

	iUsbClassControllerIterator = new(ELeave) CUsbClassControllerIterator(iSupportedClasses);
	iCenRepManager = CUsbManCenRepManager::NewL(*this);

	iDeviceConfiguration.iManufacturerName   = HBufC::NewL(KUsbStringDescStringMaxSize);
	iDeviceConfiguration.iProductName        = HBufC::NewL(KUsbStringDescStringMaxSize);
#ifndef __OVER_DUMMYUSBDI__	
#ifndef __WINS__
	OstTrace0( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL, "CUsbDevice::ConstructL; About to load LDD" );
#ifndef __DUMMY_LDD__	
	TInt err = User::LoadLogicalDevice(KUsbLDDName);
#else
	TInt err = KErrNone;
#endif
	if (err != KErrNone && err != KErrAlreadyExists)
		{
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP10, "CUsbDevice::ConstructL;err=%d", err );
		User::Leave(err);
		}

	OstTrace0( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP1, "CUsbDevice::ConstructL; About to open LDD" );	
	err = iLdd.Open(0);
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP9, "CUsbDevice::ConstructL;iLdd.Open(0) with error=%d", err );
        User::Leave(err);
	    }
	OstTrace0( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP2, "CUsbDevice::ConstructL; LDD opened" );
	
	
	// hide bus from host while interfaces are being set up
	iLdd.DeviceDisconnectFromHost();

	// Does the USC support cable detection while powered off? If no, then 
	// call PowerUpUdc when RUsb::Start finishes, as is obvious. If yes, we 
	// delay calling PowerUpUdc until both the service state is 'started' and 
	// the device state is not undefined. This is to save power in the UDC 
	// when there's no point it being powered.
	TUsbDeviceCaps devCapsBuf;
	err = iLdd.DeviceCaps(devCapsBuf);
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP11, "CUsbDevice::ConstructL;iLdd.DeviceCaps(devCapsBuf) with error=%d", err );
        User::Leave(err);
	    }
	
	if ( devCapsBuf().iFeatureWord1 & KUsbDevCapsFeatureWord1_CableDetectWithoutPower )
		{
        OstTrace0( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP3, "CUsbDevice::ConstructL: UDC supports cable detect when unpowered" );
		iUdcSupportsCableDetectWhenUnpowered = ETrue;
		}
	else
		{
        OstTrace0( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP4, "CUsbDevice::ConstructL; UDC does not support cable detect when unpowered" );
		}

	TUsbcDeviceState deviceState;
	err = iLdd.DeviceStatus(deviceState);
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP13, "CUsbDevice::ConstructL;iLdd.DeviceStatus(deviceState) with error=%d", err );
        User::Leave(err);
	    }
	SetDeviceState(deviceState);
	OstTrace0( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP5, "CUsbDevice::ConstructL; Got device state" );
	

	iDeviceStateWatcher = CUsbDeviceStateWatcher::NewL(*this, iLdd);
	iDeviceStateWatcher->Start();

#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
	iChargerTypeWatcher = CUsbChargingPortTypeWatcher::NewL(*this, iLdd);
	iChargerTypeWatcher->Start();
#endif	

	// Get hold of the default serial number in the driver
	// This is so it can be put back in place when a device that sets a
	// different serial number (through the P&S key) is stopped
	iDefaultSerialNumber = HBufC16::NewL(KUsbStringDescStringMaxSize);
	TPtr16 serNum = iDefaultSerialNumber->Des();
	err = iLdd.GetSerialNumberStringDescriptor(serNum);
	if (err == KErrNotFound)
		{
		delete iDefaultSerialNumber;
		iDefaultSerialNumber = NULL;
		OstTrace0( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP6, "CUsbDevice::ConstructL; No default serial number" );
		
		}
	else
		{
        if(err < 0)
            {
            OstTrace1( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP12, "CUsbDevice::ConstructL;error=%d", err );
            User::Leave(err);
            }
#ifdef _DEBUG
		OstTraceExt1( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP7, "CUsbDevice::ConstructL;serNum=%S", serNum );
#endif //_DEBUG  
		}

	OstTrace0( TRACE_NORMAL, CUSBDEVICE_CONSTRUCTL_DUP8, "CUsbDevice::ConstructL; UsbDevice::ConstructL() finished" );
	
#endif
#endif
	
#ifndef __OVER_DUMMYUSBDI__
	InstantiateExtensionPluginsL();
#endif
	OstTraceFunctionExit0( CUSBDEVICE_CONSTRUCTL_EXIT );
	}

void CUsbDevice::InstantiateExtensionPluginsL()
	{
    OstTraceFunctionEntry0( CUSBDEVICE_INSTANTIATEEXTENSIONPLUGINSL_ENTRY );
    
	const TUid KUidExtensionPluginInterface = TUid::Uid(KUsbmanExtensionPluginInterfaceUid);
	RImplInfoPtrArray implementations;
	const TEComResolverParams noResolverParams;
	REComSession::ListImplementationsL(KUidExtensionPluginInterface, noResolverParams, KRomOnlyResolverUid, implementations);
	CleanupResetAndDestroyPushL(implementations);
	OstTrace1( TRACE_FLOW, CUSBDEVICE_INSTANTIATEEXTENSIONPLUGINSL, "CUsbDevice::InstantiateExtensionPluginsL;Number of implementations of extension plugin interface: %d", implementations.Count() );
	

	for (TInt i=0; i<implementations.Count(); i++)
		{
#ifndef __DUMMY_LDD__
		CUsbmanExtensionPlugin* plugin = CUsbmanExtensionPlugin::NewL(implementations[i]->ImplementationUid(), *this);
		CleanupStack::PushL(plugin);
		iExtensionPlugins.AppendL(plugin); // transfer ownership to iExtensionPlugins
		CleanupStack::Pop(plugin);
#endif
		OstTrace1( TRACE_NORMAL, CUSBDEVICE_INSTANTIATEEXTENSIONPLUGINSL_DUP1, 
		        "CUsbDevice::InstantiateExtensionPluginsL;Added extension plugin with UID 0x%08x", 
		        implementations[i]->ImplementationUid().iUid );
		
		}

	CleanupStack::PopAndDestroy(&implementations);
	OstTraceFunctionExit0( CUSBDEVICE_INSTANTIATEEXTENSIONPLUGINSL_EXIT );
	}


	
   	
void CUsbDevice::EnumerateClassControllersL()
/**
 * Loads all USB class controllers at startup.
 *
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_ENUMERATECLASSCONTROLLERSL_ENTRY );
	
#ifdef USE_DUMMY_CLASS_CONTROLLER
	//create a TLinearOrder to supply the comparison function, Compare(), to be used  
	//to determine the order to add class controllers
	TLinearOrder<CUsbClassControllerBase> order(CUsbClassControllerBase::Compare);
	
	// For GT171 automated tests, create three instances of the dummy class 
	// controller, which will read their behaviour from an ini file. Do not 
	// make any other class controllers.
	for ( TUint ii = 0 ; ii < 3 ; ii++ )
		{
		AddClassControllerL(CUsbDummyClassController::NewL(*this, ii), order);	
		}
	TInt err = iUsbClassControllerIterator->First();   
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_ENUMERATECLASSCONTROLLERSL_DUP2, "CUsbDevice::EnumerateClassControllersL;iUsbClassControllerIterator->First() with error=%d", err );
        User::Leave(err);
	    }
	    
#else

	// Add a class controller statically.
	// The next line shows how to add a class controller, CUsbExampleClassController,
	// statically

	// AddClassControllerL(CUsbExampleClassController::NewL(*this),order);
	
	// Load class controller plug-ins

	RImplInfoPtrArray implementations;

	const TEComResolverParams noResolverParams;
	REComSession::ListImplementationsL(KUidUsbPlugIns, noResolverParams, KRomOnlyResolverUid, implementations);
  	CleanupResetAndDestroyPushL(implementations);
  	
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_ENUMERATECLASSCONTROLLERSL, "CUsbDevice::EnumerateClassControllersL;Number of implementations to load %d", implementations.Count() );
	
	for (TInt i=0; i<implementations.Count(); i++)
		{
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_ENUMERATECLASSCONTROLLERSL_DUP1, "CUsbDevice::EnumerateClassControllersL;Adding class controller with UID %x", implementations[i]->ImplementationUid().iUid );
        const TUid uid = implementations[i]->ImplementationUid();
		TInt err = iSupportedClassUids.Append(uid);
		if(err < 0)
		    {
            OstTrace1( TRACE_NORMAL, CUSBDEVICE_ENUMERATECLASSCONTROLLERSL_DUP3, "CUsbDevice::EnumerateClassControllersL;iSupportedClassUids.Append(uid) with error=%d", err );
            User::Leave(err);
		    }
		}	
			
	CleanupStack::PopAndDestroy(&implementations);
	
#endif // USE_DUMMY_CLASS_CONTROLLER
	OstTraceFunctionExit0( CUSBDEVICE_ENUMERATECLASSCONTROLLERSL_EXIT );
	}

void CUsbDevice::AddClassControllerL(CUsbClassControllerBase* aClassController, 
									TLinearOrder<CUsbClassControllerBase> aOrder)
/**
 * Adds a USB class controller to the device. The controller will now be
 * managed by this device. Note that the class controller, aClassController, is now
 * owned by this function and can be destroyed by it.  Calling functions do not need to 
 * destroy the class controller. 
 *
 * @param	aClassController	Class to be managed
 * @param   aOrder              Specifies order CUsbClassControllerBase objects are to be
 *                              added
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_ADDCLASSCONTROLLERL_ENTRY );
	
	
	TInt rc = KErrNone;	
	
	if(isPersonalityCfged()) // do not take into account priorities
		{
		rc = iSupportedClasses.Append(aClassController);	
		}
	else
		{
		rc = iSupportedClasses.InsertInOrderAllowRepeats(
		aClassController, aOrder);
		} 
		
	if (rc != KErrNone) 
		{
		// Avoid memory leak by deleting class controller if the append fails.
		delete aClassController;
		OstTrace1( TRACE_NORMAL, CUSBDEVICE_ADDCLASSCONTROLLERL, "CUsbDevice::AddClassControllerL;Leave rc=%d", rc );
		User::Leave(rc);
		}
	OstTraceFunctionExit0( CUSBDEVICE_ADDCLASSCONTROLLERL_EXIT );
	}

void CUsbDevice::RegisterObserverL(MUsbDeviceNotify& aObserver)
/**
 * Register an observer of the device.
 * Presently, the device supports watching state.
 *
 * @param	aObserver	New Observer of the device
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_REGISTEROBSERVERL_ENTRY );
	TInt err = iObservers.Append(&aObserver);
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_REGISTEROBSERVERL, "CUsbDevice::RegisterObserverL;iObservers.Append(&aObserver) with err=%d", err );
        User::Leave(err);
	    }
	OstTraceFunctionExit0( CUSBDEVICE_REGISTEROBSERVERL_EXIT );
	}


void CUsbDevice::DeRegisterObserver(MUsbDeviceNotify& aObserver)
/**
 * De-registers an existing device observer.
 *
 * @param	aObserver	The existing device observer to be de-registered
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_DEREGISTEROBSERVER_ENTRY );

	TInt index = iObservers.Find(&aObserver);

	if (index >= 0)
		iObservers.Remove(index);
	OstTraceFunctionExit0( CUSBDEVICE_DEREGISTEROBSERVER_EXIT );
	}


void CUsbDevice::StartL()
/**
 * Start the USB Device and all its associated USB classes.
 * Reports errors and state changes via observer interface.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_STARTL_ENTRY );

	Cancel();
	SetServiceState(EUsbServiceStarting);

	TRAPD(err, SetDeviceDescriptorL());
	if ( err != KErrNone )
		{
		SetServiceState(EUsbServiceIdle);	
		OstTrace1( TRACE_NORMAL, CUSBDEVICE_STARTL, "CUsbDevice::StartL;Leave with error=%d", err );
		User::Leave(err);		
		}

	iLastError = KErrNone;
	StartCurrentClassController();
	OstTraceFunctionExit0( CUSBDEVICE_STARTL_EXIT );
	}

void CUsbDevice::Stop()
/**
 * Stop the USB device and all its associated USB classes.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_STOP_ENTRY );

	Cancel();
	SetServiceState(EUsbServiceStopping);
	
	iLastError = KErrNone;
	StopCurrentClassController();
	OstTraceFunctionExit0( CUSBDEVICE_STOP_EXIT );
	}

void CUsbDevice::SetServiceState(TUsbServiceState aState)
/**
 * Change the device's state and report the change to the observers.
 *
 * @param	aState	New state that the device is moving to
 */
	{
    OstTraceFunctionEntry0( CUSBDEVICE_SETSERVICESTATE_ENTRY );
    
	OstTraceExt2( TRACE_NORMAL, CUSBDEVICE_SETSERVICESTATE, 
	        "CUsbDevice::SetServiceState;iServiceState=%d;aState=%d", iServiceState, aState );
	

	if (iServiceState != aState)
		{
		// Change state straight away in case any of the clients check it
		TUsbServiceState oldState = iServiceState;
		iServiceState = aState;
		TUint length = iObservers.Count();

		for (TUint i = 0; i < length; i++)
			{
			iObservers[i]->UsbServiceStateChange(LastError(), oldState,
				iServiceState);
			}

		if (iServiceState == EUsbServiceIdle)
			iUsbServer.LaunchShutdownTimerIfNoSessions();
		}
	OstTraceFunctionExit0( CUSBDEVICE_SETSERVICESTATE_EXIT );
	}

void CUsbDevice::SetDeviceState(TUsbcDeviceState aState)
/**
 * The CUsbDevice::SetDeviceState method
 *
 * Change the device's state and report the change to the observers
 *
 * @internalComponent
 * @param	aState	New state that the device is moving to
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_SETDEVICESTATE_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CUSBDEVICE_SETDEVICESTATE, 
	        "CUsbDevice::SetDeviceState;aState=%d;iDeviceState=%d", aState, iDeviceState );

	TUsbDeviceState state;
	switch (aState)
		{
	case EUsbcDeviceStateUndefined:
		state = EUsbDeviceStateUndefined;
		break;
	case EUsbcDeviceStateAttached:
		state = EUsbDeviceStateAttached;
		break;
	case EUsbcDeviceStatePowered:
		state = EUsbDeviceStatePowered;
		break;
	case EUsbcDeviceStateDefault:
		state = EUsbDeviceStateDefault;
		break;
	case EUsbcDeviceStateAddress:
		state = EUsbDeviceStateAddress;
		break;
	case EUsbcDeviceStateConfigured:
		state = EUsbDeviceStateConfigured;
		break;
	case EUsbcDeviceStateSuspended:
		state = EUsbDeviceStateSuspended;
		break;
	default:
		return;
		}

	if (iDeviceState != state)
		{
#ifndef __OVER_DUMMYUSBDI__
#ifndef __WINS__
		if (iDeviceState == EUsbDeviceStateUndefined &&
			iUdcSupportsCableDetectWhenUnpowered &&
			iServiceState == EUsbServiceStarted)
			{
			// We just changed state away from undefined. Hence the cable must
			// now be attached (if it wasn't before). If the UDC supports
			// cable detection when unpowered, NOW is the right time to power
			// it up (so long as usbman is fully started).
			(void)PowerUpAndConnect(); // We don't care about any errors here.
			}
#endif
#endif // __OVER_DUMMYUSBDI__
		// Change state straight away in case any of the clients check it
		TUsbDeviceState oldState = iDeviceState;
		iDeviceState = state;
		TUint length = iObservers.Count();

		for (TUint i = 0; i < length; i++)
			{
			iObservers[i]->UsbDeviceStateChange(LastError(), oldState, iDeviceState);
			}
		}
	OstTraceFunctionExit0( CUSBDEVICE_SETDEVICESTATE_EXIT );
	}

/**
 * Callback called by CDeviceHandler when the USB bus has sucessfully
 * completed a ReEnumeration (restarted all services).
 */
void CUsbDevice::BusEnumerationCompleted()
	{
	OstTraceFunctionEntry0( CUSBDEVICE_BUSENUMERATIONCOMPLETED_ENTRY );

	// Has the start been cancelled?
	if (iServiceState == EUsbServiceStarting)
		{
		SetServiceState(EUsbServiceStarted);
		}
	else
		{
        OstTrace0( TRACE_NORMAL, CUSBDEVICE_BUSENUMERATIONCOMPLETED, 
                "CUsbDevice::BusEnumerationCompleted;    Start has been cancelled!" );
		}
	OstTraceFunctionExit0( CUSBDEVICE_BUSENUMERATIONCOMPLETED_EXIT );
	}

void CUsbDevice::BusEnumerationFailed(TInt aError)
/**
 * Callback called by CDeviceHandler when the USB bus has
 * completed an ReEnumeration (Restarted all services) with errors
 *
 * @param	aError	Error that has occurred during Re-enumeration
 */
	{
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_BUSENUMERATIONFAILED, "CUsbDevice::BusEnumerationFailed;aError=%d", aError );
	
	iLastError = aError;

	if (iServiceState == EUsbServiceStarting)
		{
		SetServiceState(EUsbServiceStopping);
		StopCurrentClassController();
		}
	else
		{
		OstTrace0( TRACE_NORMAL, CUSBDEVICE_BUSENUMERATIONFAILED_DUP1, 
		        "CUsbDevice::BusEnumerationFailed;    Start has been cancelled!" );		
		}
	}


void CUsbDevice::StartCurrentClassController()
/**
 * Called numerous times to start all the USB classes.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_STARTCURRENTCLASSCONTROLLER_ENTRY );

	iUsbClassControllerIterator->Current()->Start(iStatus);
	SetActive();
	OstTraceFunctionExit0( CUSBDEVICE_STARTCURRENTCLASSCONTROLLER_EXIT );
	}

void CUsbDevice::StopCurrentClassController()
/**
 * Called numerous times to stop all the USB classes.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_STOPCURRENTCLASSCONTROLLER_ENTRY );	

	iUsbClassControllerIterator->Current()->Stop(iStatus);
	SetActive();
	OstTraceFunctionExit0( CUSBDEVICE_STOPCURRENTCLASSCONTROLLER_EXIT );
	}

/**
Utility function to power up the UDC and connect the
device to the host.
*/
TInt CUsbDevice::PowerUpAndConnect()
	{
	OstTraceFunctionEntry0( CUSBDEVICE_POWERUPANDCONNECT_ENTRY );
	
	OstTrace0( TRACE_NORMAL, CUSBDEVICE_POWERUPANDCONNECT, "CUsbDevice::PowerUpAndConnect;Powering up UDC..." );
	
	TInt res = iLdd.PowerUpUdc();
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_POWERUPANDCONNECT_DUP1, 
	        "CUsbDevice::PowerUpAndConnect;PowerUpUdc res = %d", res );
	
	res = iLdd.DeviceConnectToHost();
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_POWERUPANDCONNECT_DUP2, 
	        "CUsbDevice::PowerUpAndConnect;DeviceConnectToHost res = %d", res );
	
	OstTraceFunctionExit0( CUSBDEVICE_POWERUPANDCONNECT_EXIT );
	return res;
	}

void CUsbDevice::RunL()
/**
 * Called when starting or stopping a USB class has completed, successfully or
 * otherwise. Continues with the process of starting or stopping until all
 * classes have been completed.
 */
	{
    OstTraceFunctionEntry0( CUSBDEVICE_RUNL_ENTRY );
    OstTrace1( TRACE_NORMAL, CUSBDEVICE_RUNL, "CUsbDevice::RunL;iStatus.Int()=%d", iStatus.Int() );

    TInt err = iStatus.Int();
    if(err < 0)
        {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_RUNL_DUP4, "CUsbDevice::RunL;iStatus.Int() with error=%d", err );
        User::Leave(err);
        }

	switch (iServiceState)
		{
	case EUsbServiceStarting:
		if (iUsbClassControllerIterator->Next() == KErrNotFound)
			{
#ifndef __OVER_DUMMYUSBDI__
#ifndef __WINS__
			if (!iUdcSupportsCableDetectWhenUnpowered || iDeviceState != EUsbDeviceStateUndefined)
				{
				// We've finished starting the classes. We can just power up the UDC
				// now: there's no need to re-enumerate, because we soft disconnected
				// earlier. This will also do a soft connect.
				OstTrace0( TRACE_NORMAL, CUSBDEVICE_RUNL_DUP1, "CUsbDevice::RunL;Finished starting classes: powering up UDC" );
				

				// It isn't an error if this call fails. This will happen, for example,
				// in the case where there are no USB classes defined.
				(void)PowerUpAndConnect();
				}
#endif
#endif
			// If we're not running on target, we can just go to "started".
			SetServiceState(EUsbServiceStarted);
			}
		else
			{
			StartCurrentClassController();
			}
		break;

	case EUsbServiceStopping:
		if (iUsbClassControllerIterator->Previous() == KErrNotFound)
			{
			// if stopping classes, hide the USB interface from the host
#ifndef __OVER_DUMMYUSBDI__
#ifndef __WINS__
			iLdd.DeviceDisconnectFromHost();

			// Restore the default serial number 
			if (iDefaultSerialNumber)
				{
				TInt res = iLdd.SetSerialNumberStringDescriptor(*iDefaultSerialNumber);
				OstTrace1( TRACE_NORMAL, CUSBDEVICE_RUNL_DUP2, "CUsbDevice::RunL;Restore default serial number res = %d", res );				
				}
			else
				{
				TInt res = iLdd.RemoveSerialNumberStringDescriptor();
				OstTrace1( TRACE_NORMAL, CUSBDEVICE_RUNL_DUP3, "CUsbDevice::RunL;Remove serial number res = %d", res );				
				}

#endif				
#endif			
			SetServiceState(EUsbServiceIdle);
			}
		else
			{
			StopCurrentClassController();
			}
		break;

	default:

        OstTrace1( TRACE_FATAL, CUSBDEVICE_RUNL_DUP5, "CUsbDevice::RunL;Panic reason=%d", EBadAsynchronousCall );
        __ASSERT_DEBUG( EFalse, User::Panic(KUsbDevicePanicCategory, EBadAsynchronousCall) );

		break;
		}
	OstTraceFunctionExit0( CUSBDEVICE_RUNL_EXIT );
	}

void CUsbDevice::DoCancel()
/**
 * Standard active object cancellation function. If we're starting or stopping
 * a USB class, cancels it. If we're not, then we shouldn't be active and hence
 * this function being called is a programming error.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_DOCANCEL_ENTRY );

	switch (iServiceState)
		{
	case EUsbServiceStarting:
	case EUsbServiceStopping:
		iUsbClassControllerIterator->Current()->Cancel();
		break;

	default:

        OstTrace1( TRACE_FATAL, CUSBDEVICE_DOCANCEL, "CUsbDevice::DoCancel;Panic reason=%d", EBadAsynchronousCall );
        __ASSERT_DEBUG( EFalse, User::Panic(KUsbDevicePanicCategory, EBadAsynchronousCall ) );
		break;
		}
	OstTraceFunctionExit0( CUSBDEVICE_DOCANCEL_EXIT );
	}

TInt CUsbDevice::RunError(TInt aError)
/**
 * Standard active object RunError function. Handles errors which occur when
 * starting and stopping the USB class objects.
 *
 * @param aError The error which occurred
 * @return Always KErrNone, to avoid an active scheduler panic
 */
	{
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_RUNERROR, "CUsbDevice::RunError;aError=%d", aError );
	

	iLastError = aError;

	switch (iServiceState)
		{
	case EUsbServiceStarting:
	case EUsbServiceStarted:
		// An error has happened while we're either started or starting, so
		// we have to stop all the classes which were successfully started.
		if ((iUsbClassControllerIterator->Current()->State() ==
				EUsbServiceIdle) &&
			(iUsbClassControllerIterator->Previous() == KErrNotFound))
			{
			SetServiceState(EUsbServiceIdle);
			}
		else
			{
			SetServiceState(EUsbServiceStopping);
			StopCurrentClassController();
			}
		break;

	case EUsbServiceStopping:
		// Argh, we've got problems. Let's stop as many classes as we can.
		if (iUsbClassControllerIterator->Previous() == KErrNotFound)
			SetServiceState(EUsbServiceIdle);
		else
			StopCurrentClassController();
		break;

	default:
        OstTrace1( TRACE_FATAL, CUSBDEVICE_RUNERROR_DUP1, "CUsbDevice::RunError;Panic reason=%d", EBadAsynchronousCall );
        __ASSERT_DEBUG( EFalse, User::Panic(KUsbDevicePanicCategory, EBadAsynchronousCall ) );
		break;
		}

	return KErrNone;
	}

CUsbClassControllerIterator* CUsbDevice::UccnGetClassControllerIteratorL()
/**
 * Function used by USB classes to get an iterator over the set of classes
 * owned by this device. Note that the caller takes ownership of the iterator
 * which this function returns.
 *
 * @return A new iterator
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_UCCNGETCLASSCONTROLLERITERATORL_ENTRY );

	return new (ELeave) CUsbClassControllerIterator(iSupportedClasses);
	}

void CUsbDevice::UccnError(TInt aError)
/**
 * Function called by USB classes to notify the device of a fatal error. In
 * this situation, we should just stop all the classes we can.
 *
 * @param aError The error that's occurred
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_UCCNERROR_ENTRY );

	RunError(aError);
	OstTraceFunctionExit0( CUSBDEVICE_UCCNERROR_EXIT );
	}


#ifdef _DEBUG
void CUsbDevice::PrintDescriptor(CUsbDevice::TUsbDeviceDescriptor& aDeviceDescriptor)
	{
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR, "CUsbDevice::PrintDescriptor;iLength=%d", aDeviceDescriptor.iLength );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP1, "CUsbDevice::PrintDescriptor;iDescriptorType=%d", aDeviceDescriptor.iDescriptorType );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP2, "CUsbDevice::PrintDescriptor;iBcdUsb=0x%04x", aDeviceDescriptor.iBcdUsb );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP3, "CUsbDevice::PrintDescriptor;iDeviceClass=0x%02x", aDeviceDescriptor.iDeviceClass );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP4, "CUsbDevice::PrintDescriptor;iDeviceSubClass=0x%02x", aDeviceDescriptor.iDeviceSubClass );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP5, "CUsbDevice::PrintDescriptor;iDeviceProtocol=0x%02x", aDeviceDescriptor.iDeviceProtocol );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP6, "CUsbDevice::PrintDescriptor;iMaxPacketSize=0x%02x", aDeviceDescriptor.iMaxPacketSize );
	
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP7, "CUsbDevice::PrintDescriptor;iIdVendor=0x%04x", aDeviceDescriptor.iIdVendor );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP8, "CUsbDevice::PrintDescriptor;iProductId=0x%04x", aDeviceDescriptor.iProductId );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP9, "CUsbDevice::PrintDescriptor;iBcdDevice=0x%04x", aDeviceDescriptor.iBcdDevice );
	
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP10, "CUsbDevice::PrintDescriptor;iManufacturer=0x%04x", aDeviceDescriptor.iManufacturer );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP11, "CUsbDevice::PrintDescriptor;iSerialNumber=0x%04x", aDeviceDescriptor.iSerialNumber );
	OstTrace1( TRACE_DUMP, CUSBDEVICE_PRINTDESCRIPTOR_DUP12, "CUsbDevice::PrintDescriptor;iNumConfigurations=0x%04x", aDeviceDescriptor.iNumConfigurations );	
	}
#endif
//
void CUsbDevice::SetDeviceDescriptorL()
/**
 * Modifies the USB device descriptor.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_SETDEVICEDESCRIPTORL_ENTRY );

#if !defined(__OVER_DUMMYUSBDI__) && !defined(__WINS__)

	TInt desSize = 0;
	iLdd.GetDeviceDescriptorSize(desSize);
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_SETDEVICEDESCRIPTORL, "CUsbDevice::SetDeviceDescriptorL;UDeviceDescriptorSize = %d", desSize );
	HBufC8* deviceBuf = HBufC8::NewLC(desSize);
	TPtr8   devicePtr = deviceBuf->Des();
	devicePtr.SetLength(0);

	TInt ret = iLdd.GetDeviceDescriptor(devicePtr);

	if (ret != KErrNone)
		{
		OstTrace1( TRACE_NORMAL, CUSBDEVICE_SETDEVICEDESCRIPTORL_DUP1, "CUsbDevice::SetDeviceDescriptorL;Unable to fetch device descriptor. Error: %d", ret );
		User::Leave(ret);
		}

	TUsbDeviceDescriptor* deviceDescriptor = reinterpret_cast<TUsbDeviceDescriptor*>(
		const_cast<TUint8*>(devicePtr.Ptr()));


#else

	// Create an empty descriptor to allow the settings
	// to be read in from the resource file
	TUsbDeviceDescriptor descriptor;
	TUsbDeviceDescriptor* deviceDescriptor = &descriptor;
	
#endif // __OVER_DUMMYUSBDI__ && _WINS_

	if (iPersonalityCfged)
		{
		SetUsbDeviceSettingsFromPersonalityL(*deviceDescriptor);		
		}
	else
		{
        OstTrace0( TRACE_NORMAL, CUSBDEVICE_SETDEVICEDESCRIPTORL_DUP3, 
                "CUsbDevice::SetDeviceDescriptorL;USB configuration is not read" );
        User::Leave(KErrNotFound);
		}
	
#ifndef __OVER_DUMMYUSBDI__
#ifndef __WINS__
	ret = iLdd.SetDeviceDescriptor(devicePtr);

	if (ret != KErrNone)
		{
		OstTrace1( TRACE_NORMAL, CUSBDEVICE_SETDEVICEDESCRIPTORL_DUP2, "CUsbDevice::SetDeviceDescriptorL;Unable to set device descriptor. Error: %d", ret );
		User::Leave(ret);
		}

	CleanupStack::PopAndDestroy(deviceBuf);

#endif
#endif // __OVER_DUMMYUSBDI__
	OstTraceFunctionExit0( CUSBDEVICE_SETDEVICEDESCRIPTORL_EXIT );
	}

void CUsbDevice::SetUsbDeviceSettingsDefaultsL(CUsbDevice::TUsbDeviceDescriptor& aDeviceDescriptor)
/**
 * Set the device settings defaults, as per the non-resource
 * version of the USB manager
 *
 * @param aDeviceDescriptor The device descriptor for the USB device
 */
	{
	aDeviceDescriptor.iDeviceClass		= KUsbDefaultDeviceClass;
	aDeviceDescriptor.iDeviceSubClass	= KUsbDefaultDeviceSubClass;
	aDeviceDescriptor.iDeviceProtocol	= KUsbDefaultDeviceProtocol;
	aDeviceDescriptor.iIdVendor			= KUsbDefaultVendorId;
	aDeviceDescriptor.iProductId		= KUsbDefaultProductId;
	}

void CUsbDevice::SetUsbDeviceSettingsFromPersonalityL(CUsbDevice::TUsbDeviceDescriptor& aDeviceDescriptor)
/**
 * Configure the USB device from the current personality.
 *
 * @param aDeviceDescriptor The device descriptor for the USB device
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_SETUSBDEVICESETTINGSFROMPERSONALITYL_ENTRY );

	// First, use the default values
	OstTrace0( TRACE_NORMAL, CUSBDEVICE_SETUSBDEVICESETTINGSFROMPERSONALITYL, "CUsbDevice::SetUsbDeviceSettingsFromPersonalityL;Setting default values for the configuration" );
	SetUsbDeviceSettingsDefaultsL(aDeviceDescriptor);

	// Now try to get the configuration from the current personality
    aDeviceDescriptor.iDeviceClass          = iCurrentPersonality->DeviceClass();
    aDeviceDescriptor.iDeviceSubClass       = iCurrentPersonality->DeviceSubClass();
    aDeviceDescriptor.iDeviceProtocol       = iCurrentPersonality->DeviceProtocol();
    aDeviceDescriptor.iIdVendor             = iDeviceConfiguration.iVendorId;
    aDeviceDescriptor.iProductId            = iCurrentPersonality->ProductId();
    aDeviceDescriptor.iBcdDevice            = iCurrentPersonality->BcdDevice();
    aDeviceDescriptor.iNumConfigurations    = iCurrentPersonality->NumConfigurations();	
	

#ifndef __OVER_DUMMYUSBDI__
#ifndef __WINS__
	TInt err = iLdd.SetManufacturerStringDescriptor(*(iDeviceConfiguration.iManufacturerName));
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_SETUSBDEVICESETTINGSFROMPERSONALITYL_DUP7, "CUsbDevice::SetUsbDeviceSettingsFromPersonalityL;iLdd.SetManufacturerStringDescriptor(*(iCurrentPersonality->Manufacturer())) with error=%d", err );
        User::Leave(err);
	    }  
	err = iLdd.SetProductStringDescriptor(*(iDeviceConfiguration.iProductName));
    if(err < 0)
        {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_SETUSBDEVICESETTINGSFROMPERSONALITYL_DUP6, "CUsbDevice::SetUsbDeviceSettingsFromPersonalityL;iLdd.SetProductStringDescriptor(*(iCurrentPersonality->Product())) with error=%d", err );
        User::Leave(err);
        }


	//Read the published serial number. The key is the UID KUidUsbmanServer = 0x101FE1DB
	TBuf16<KUsbStringDescStringMaxSize> serNum;
	TInt r = RProperty::Get(KUidSystemCategory,0x101FE1DB,serNum);
	if(r==KErrNone)
		{
#ifdef _DEBUG
		OstTraceExt1( TRACE_NORMAL, CUSBDEVICE_SETUSBDEVICESETTINGSFROMPERSONALITYL_DUP1, "CUsbDevice::SetUsbDeviceSettingsFromPersonalityL;Setting published SerialNumber: %S", serNum );
#endif//_DEBUG
		//USB spec doesn't give any constraints on what constitutes a valid serial number.
		//As long as it is a string descriptor it is valid.
		err = iLdd.SetSerialNumberStringDescriptor(serNum);	
		if(err < 0)
		    {
            OstTrace1( TRACE_NORMAL, CUSBDEVICE_SETUSBDEVICESETTINGSFROMPERSONALITYL_DUP3, "CUsbDevice::SetUsbDeviceSettingsFromPersonalityL;iLdd.SetSerialNumberStringDescriptor(serNum) with error=%d", err );
            User::Leave(err);
		    }
		}
#ifdef _DEBUG
	else
		{
		OstTrace0( TRACE_NORMAL, CUSBDEVICE_SETUSBDEVICESETTINGSFROMPERSONALITYL_DUP2, "CUsbDevice::SetUsbDeviceSettingsFromPersonalityL;SerialNumber has not been published" );	
		}
#endif // _DEBUG

#endif
#endif // __OVER_DUMMYUSBDI__


#ifdef _DEBUG
	PrintDescriptor(aDeviceDescriptor);		

#ifndef __OVER_DUMMYUSBDI__
#ifndef __WINS__
	TBuf16<KUsbStringDescStringMaxSize> wideString;
	TInt tmp = iLdd.GetConfigurationStringDescriptor(wideString);	
	if(tmp < 0)
	    {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_SETUSBDEVICESETTINGSFROMPERSONALITYL_DUP5, "CUsbDevice::SetUsbDeviceSettingsFromPersonalityL;iLdd.GetConfigurationStringDescriptor(wideString) with error=%d", tmp );
        User::Leave(tmp);
	    }
	OstTraceExt1( TRACE_NORMAL, CUSBDEVICE_SETUSBDEVICESETTINGSFROMPERSONALITYL_DUP4, "CUsbDevice::SetUsbDeviceSettingsFromPersonalityL;Configuration is:%S", wideString );
#endif
#endif // __OVER_DUMMYUSBDI__

#endif // _DEBUG
	}
	
void CUsbDevice::TryStartL(TInt aPersonalityId)
/**
 * Start all USB classes associated with the personality identified
 * by aPersonalityId. Reports errors and state changes via observer 
 * interface.
 *
 * @param aPersonalityId a personality id
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_TRYSTARTL_ENTRY );
	SetCurrentPersonalityL(aPersonalityId);
	
	SelectClassControllersL();
	SetServiceState(EUsbServiceStarting);

	TRAPD(err, SetDeviceDescriptorL());
	if ( err != KErrNone )
		{
		SetServiceState(EUsbServiceIdle);	
		OstTrace1( TRACE_NORMAL, CUSBDEVICE_TRYSTARTL, "CUsbDevice::TryStartL;leave with error=%d", err );
		User::Leave(err);		
		}

	iLastError = KErrNone;
	StartCurrentClassController();
 	OstTraceFunctionExit0( CUSBDEVICE_TRYSTARTL_EXIT );
 	}
 	
TInt CUsbDevice::CurrentPersonalityId() const
/**
 * @return the current personality id
 */
 	{
	OstTraceFunctionEntry0( CUSBDEVICE_CURRENTPERSONALITYID_ENTRY );
 	return iCurrentPersonality->PersonalityId();
 	}
 	
const RPointerArray<CPersonality>& CUsbDevice::Personalities() const
/**
 * @return a const reference to RPointerArray<CPersonality>
 */
 	{
	OstTraceFunctionEntry0( CUSBDEVICE_PERSONALITIES_ENTRY );
 	return iSupportedPersonalities;
 	} 
 	
const CPersonality* CUsbDevice::GetPersonality(TInt aPersonalityId) const
/**
 * Obtains a handle to the CPersonality object whose id is aPersonalityId
 *
 * @param aPeraonalityId a personality id
 * @return a const pointer to the CPersonality object whose id is aPersonalityId if found
 * or 0 otherwise.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_GETPERSONALITY_ENTRY );
	
	TInt count = iSupportedPersonalities.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iSupportedPersonalities[i]->PersonalityId() == aPersonalityId)
			{
			OstTraceFunctionExit0( CUSBDEVICE_GETPERSONALITY_EXIT );
			return iSupportedPersonalities[i];
			}
		}
	
	OstTraceFunctionExit0( CUSBDEVICE_GETPERSONALITY_EXIT_DUP1 );
	return 0;
	}
	
void CUsbDevice::SetCurrentPersonalityL(TInt aPersonalityId)
/**
 * Sets the current personality to the personality with id aPersonalityId
 */
 	{
	OstTraceFunctionEntry0( CUSBDEVICE_SETCURRENTPERSONALITYL_ENTRY );
	const CPersonality* personality = GetPersonality(aPersonalityId);
	if (!personality)
		{
		OstTrace0( TRACE_NORMAL, CUSBDEVICE_SETCURRENTPERSONALITYL, "CUsbDevice::SetCurrentPersonalityL;Personality id not found" );
		User::Leave(KErrNotFound);
		}
		
	iCurrentPersonality = personality;
 	OstTraceFunctionExit0( CUSBDEVICE_SETCURRENTPERSONALITYL_EXIT );
 	}
	
void CUsbDevice::ValidatePersonalities()
/**
 * Verifies all class controllers associated with each personality are loaded.
 * Leave if validation fails.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_VALIDATEPERSONALITIES_ENTRY );

	TInt personalityCount = iSupportedPersonalities.Count();
	for (TInt i = 0; i < personalityCount; i++)
		{
		const RArray<CPersonalityConfigurations::TUsbClasses>& classes = iSupportedPersonalities[i]->SupportedClasses();
		TInt uidCount = classes.Count();
		for (TInt j = 0; j < uidCount; j++)	
			{
			TInt ccCount = iSupportedClassUids.Count();
			TInt k;
		    OstTrace1( TRACE_NORMAL, CUSBDEVICE_VALIDATEPERSONALITIES_DUP1, "CUsbDevice::ValidatePersonalities;iSupportedClassUids Count = %d", ccCount );
			for (k = 0; k < ccCount; k++)
				{
                OstTraceExt4( TRACE_NORMAL, CUSBDEVICE_VALIDATEPERSONALITIES_DUP2, "CUsbDevice::ValidatePersonalities;iSupportedClassUids %d %x classes %d %x", k, iSupportedClassUids[k].iUid, j, classes[j].iClassUid.iUid );
				if (iSupportedClassUids[k] == classes[j].iClassUid)
					{
					break;
					}
				}
			if (k == ccCount)
				{
				OstTrace1( TRACE_FATAL, CUSBDEVICE_VALIDATEPERSONALITIES, "CUsbDevice::ValidatePersonalities; Validation failed, class %x not supported", classes[j].iClassUid.iUid );
				// Previously leaves here. But this causes that USB doesn't work.
				// Now this function will only be called in debug version and only shows some information.
				// The behaviour is a little different with before but should be ok.
				// After the device image is verified, this error will never happen at run time.
				}					
			}	
		}
	OstTraceFunctionExit0( CUSBDEVICE_VALIDATEPERSONALITIES_EXIT );
	}
/**
Converts text string with UIDs to array of Uint

If there is an error during the conversion, this function will not clean-up,
so there may still be UIDs allocated in the RArray.

@param aStr Reference to a string containing one or more UIDs in hex
@param aUIDs On return array of UIDs parsed from the input string
@panic EUidArrayNotEmpty if the RArray passed in is not empty
*/
void CUsbDevice::ConvertUidsL(const TDesC& aStr, RArray<TUint>& aUidArray)	
	{
    OstTraceFunctionEntry0( CUSBDEVICE_CONVERTUIDSL_ENTRY );
	// Function assumes that aUIDs is empty
#ifdef _DEBUG
    if(aUidArray.Count() != 0)
        {
        OstTrace1( TRACE_FATAL, CUSBDEVICE_CONVERTUIDSL, "CUsbDevice::ConvertUidsL;Panic reason=%d", EUidArrayNotEmpty );
        User::Panic(KUsbDevicePanicCategory, EUidArrayNotEmpty);
        }
#endif

	TLex input(aStr);

	// Scan through string to find UIDs
	// Need to do this at least once, as no UID in the string is an error
	do 
		{
		// Find first hex digit
		while (!(input.Eos() || input.Peek().IsHexDigit()))
			{
			input.Inc();	
			}

		// Convert and add to array
		TUint val;
		TInt err = input.Val(val,EHex);
		if(err < 0)
		    {
            OstTrace1( TRACE_NORMAL, CUSBDEVICE_CONVERTUIDSL_DUP1, "CUsbDevice::ConvertUidsL;input.Val(val,EHex) with error=%d", err );
            User::Leave(err);
		    }
		aUidArray.AppendL(val);
		}
	while (!input.Eos());	
	OstTraceFunctionExit0( CUSBDEVICE_CONVERTUIDSL_EXIT );
	}

void CUsbDevice::ReadPersonalitiesL()
    {    
    OstTraceFunctionEntry0( CUSBDEVICE_READPERSONALITIESL_ENTRY );
    TPtrC16 sysUtilModelName;
    TPtrC16 sysUtilManuName;
    
    iPersonalityCfged = EFalse;
    
    iCenRepManager->ReadDeviceConfigurationL(iDeviceConfiguration);
    
    iCenRepManager->ReadPersonalitiesL(iSupportedPersonalities);
    
    //update info for SetManufacturer & SetProduct
    CDeviceTypeInformation* deviceInfo = SysUtil::GetDeviceTypeInfoL();
    CleanupStack::PushL(deviceInfo);
    TInt gotSysUtilModelName = deviceInfo->GetModelName(sysUtilModelName);
    TInt gotSysUtilManuName = deviceInfo->GetManufacturerName(sysUtilManuName);
    
    //To overlap info 
    if (gotSysUtilManuName == KErrNone)
        {
        iDeviceConfiguration.iManufacturerName->Des().Copy(sysUtilManuName); 
        }
        
    if (gotSysUtilModelName == KErrNone)
        {
        iDeviceConfiguration.iProductName->Des().Copy(sysUtilModelName);
        }
    CleanupStack::PopAndDestroy(deviceInfo);
    iPersonalityCfged = ETrue;
    OstTraceFunctionExit0( CUSBDEVICE_READPERSONALITIESL_EXIT );
    }
	
void CUsbDevice::SelectClassControllersL()
/**
 * Selects class controllers for the current personality
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_SELECTCLASSCONTROLLERSL_ENTRY );
    const RArray<CPersonalityConfigurations::TUsbClasses>& classes = iCurrentPersonality->SupportedClasses();
	RArray<TUid> classUids;
	CleanupClosePushL( classUids ); 
    TInt classCount = classes.Count();
	for(TInt classIndex = 0; classIndex<classCount; ++classIndex)
	    {
        TUid uid = classes[classIndex].iClassUid;
        classUids.AppendL(uid);
	    }
	
	CreateClassControllersL(classUids);
	
    CleanupStack::PopAndDestroy( &classUids );
	OstTraceFunctionExit0( CUSBDEVICE_SELECTCLASSCONTROLLERSL_EXIT );
	}
#ifdef USE_DUMMY_CLASS_CONTROLLER	
void CUsbDevice::CreateClassControllersL(const RArray<TUid>& /* aClassUids*/)
#else 
void CUsbDevice::CreateClassControllersL(const RArray<TUid>& aClassUids)
#endif
/**
 * Creates a class controller object for each class uid
 *
 * @param aClassUids an array of class uids
 */
 	{
	OstTraceFunctionEntry0( CUSBDEVICE_CREATECLASSCONTROLLERSL_ENTRY );

#ifndef USE_DUMMY_CLASS_CONTROLLER

	//create a TLinearOrder to supply the comparison function, Compare(), to be used  
	//to determine the order to add class controllers
	TLinearOrder<CUsbClassControllerBase> order(CUsbClassControllerBase::Compare);

	TInt count = aClassUids.Count();
	
	// destroy any class controller objects in iSupportedClasses and reset it for reuse
	iSupportedClasses.ResetAndDestroy();
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_CREATECLASSCONTROLLERSL, "CUsbDevice::CreateClassControllersL;aClassUids.Count() = %d", count );
	for (TInt i = 0; i < count; i++)
		{ 
		CUsbClassControllerPlugIn* plugIn = CUsbClassControllerPlugIn::NewL(aClassUids[i], *this);
		AddClassControllerL(reinterpret_cast<CUsbClassControllerBase*>(plugIn), order);
		} 
#endif // USE_DUMMY_CLASS_CONTROLLER	
	TInt err = iUsbClassControllerIterator->First();	
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_CREATECLASSCONTROLLERSL_DUP1, "CUsbDevice::CreateClassControllersL;iUsbClassControllerIterator->First() with error=%d", err );
        User::Leave(err);
	    }
 	}

void CUsbDevice::SetDefaultPersonalityL()
/**
 * Sets default personality. Used for Start request.
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_SETDEFAULTPERSONALITYL_ENTRY );

	TInt smallestId = iSupportedPersonalities[0]->PersonalityId();
 	TInt count = iSupportedPersonalities.Count();
 	for (TInt i = 1; i < count; i++)
 		{
 		if(iSupportedPersonalities[i]->PersonalityId() < smallestId)
 			{
 			smallestId = iSupportedPersonalities[i]->PersonalityId();
 			}
 		}
    
	SetCurrentPersonalityL(smallestId);
	SelectClassControllersL();
	OstTraceFunctionExit0( CUSBDEVICE_SETDEFAULTPERSONALITYL_EXIT );
	}

void CUsbDevice::LoadFallbackClassControllersL()
/**
 * Load class controllers for fallback situation:
 * no personalities are configured.
 * This method inserts all class controllers to 
 * the list from which they will be either all started
 * or stopped
 */
 	{
	OstTraceFunctionEntry0( CUSBDEVICE_LOADFALLBACKCLASSCONTROLLERSL_ENTRY );
 	SetDeviceDescriptorL();
	CreateClassControllersL(iSupportedClassUids);
 	OstTraceFunctionExit0( CUSBDEVICE_LOADFALLBACKCLASSCONTROLLERSL_EXIT );
 	}
 	
RDevUsbcClient& CUsbDevice::MuepoDoDevUsbcClient()
/**
 * Inherited from MUsbmanExtensionPluginObserver - Function used by plugins to
 * retrieve our handle to the LDD
 *
 * @return The LDD handle
 */
	{
	return iLdd;
	}

void CUsbDevice::MuepoDoRegisterStateObserverL(MUsbDeviceNotify& aObserver)
/**
 * Inherited from MUsbmanExtensionPluginObserver - Function used by plugins to
 * register themselves for notifications of device/service state changes.
 *
 * @param aObserver New Observer of the device
 */
	{
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_MUEPODOREGISTERSTATEOBSERVERL, "CUsbDevice::MuepoDoRegisterStateObserverL;aObserver = 0x%08x", &aObserver );
	RegisterObserverL(aObserver);
	}


#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1
void CUsbDevice::MuepoDoRegisterChargingObserverL(MUsbChargingNotify& aObserver)
/**
 * Inherited from MUsbmanExtensionPluginObserver - Function used by plugins to
 * register themselves for notifications of charging changes.
 *
 * @param aObserver New Observer of the device
 */
	{
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_MUEPODOREGISTERCHARGINGOBSERVERL, "CUsbDevice::MuepoDoRegisterChargingObserverL;aObserver = 0x%08x", &aObserver );
	RegisterChargingInfoObserverL(aObserver);
	}


void CUsbDevice::RegisterChargingInfoObserverL(MUsbChargingNotify& aObserver)
/**
 * Register an observer of charging.
 * Presently, the device supports watching state.
 *
 * @param	aObserver	New Observer of the charging
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_REGISTERCHARGINGINFOOBSERVERL_ENTRY );
	TInt err = iChargingObservers.Append(&aObserver);
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CUSBDEVICE_REGISTERCHARGINGINFOOBSERVERL, "CUsbDevice::RegisterObserverL;iObservers.Append(&aObserver) with err=%d", err );
        User::Leave(err);
	    }
	OstTraceFunctionExit0( CUSBDEVICE_REGISTERCHARGINGINFOOBSERVERL_EXIT );
	}


void CUsbDevice::DeRegisterChargingInfoObserver(MUsbChargingNotify& aObserver)
/**
 * De-registers an existing charging observer.
 *
 * @param	aObserver	The existing device observer to be de-registered
 */
	{
	OstTraceFunctionEntry0( CUSBDEVICE_DEREGISTERCHARGINGINFOOBSERVERL_ENTRY );

	TInt index = iChargingObservers.Find(&aObserver);

	if (index >= 0)
		iObservers.Remove(index);
	OstTraceFunctionExit0( CUSBDEVICE_DEREGISTERCHARGINGINFOOBSERVERL_EXIT );
	}


void CUsbDevice::SetChargingPortType(TUsbcChargingPortType aChargingPortType)
	{
	OstTraceFunctionEntry0( CUSBDEVICE_SETCHARGINGPORTTYPE_ENTRY );
	TUint length = iChargingObservers.Count();

	for (TUint i = 0; i < length; i++)
		{
		iChargingObservers[i]->UsbChargingPortType(aChargingPortType);
		}
	OstTraceFunctionExit0( CUSBDEVICE_SETCHARGINGPORTTYPE_EXIT );
	}

void CUsbDevice::PeerDeviceMaxPower(TUint aCurrent)
	{
	OstTraceFunctionEntry0( CUSBDEVICE_PEERDEVICEMAXPOWER_ENTRY );
	OstTrace1( TRACE_NORMAL, CUSBDEVICE_PEERDEVICEMAXPOWER_DUP, "CUsbDevice::PeerDeviceMaxPower with aCurrent=%d", aCurrent);
	TUint length = iChargingObservers.Count();

	for (TUint i = 0; i < length; i++)
		{
		iChargingObservers[i]->PeerDeviceMaxPower(aCurrent);
		}		
	OstTraceFunctionExit0( CUSBDEVICE_PEERDEVICEMAXPOWER_EXIT );
	}

#endif

