/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


/** @file
@internalComponent
*/

#include "chargingstates.h"
#include "reenumerator.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "chargingstatesTraces.h"
#endif



// Charging plugin base state

// Empty virtual function implement to give a base of each state class. 
// A concrete state class can overlap them according to actual demand.
void TUsbBatteryChargingPluginStateBase::UsbServiceStateChange(TInt aLastError,
    TUsbServiceState aOldState, TUsbServiceState aNewState)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_USBSERVICESTATECHANGE_ENTRY );
    
    (void)aLastError;
    (void)aOldState;
    (void)aNewState;
    
    // Not use
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_USBSERVICESTATECHANGE_EXIT );
    }

#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
void TUsbBatteryChargingPluginStateBase::PeerDeviceMaxPower(TUint aCurrent)
	{
	(void)aCurrent;
	}

void TUsbBatteryChargingPluginStateBase::UsbChargingPortType(TUint aPortType)
    {
    switch (aPortType)
    	{
    	case EUsbChargingPortTypeDedicatedChargingPort:
		case EUsbChargingPortTypeAcaRidA:
		case EUsbChargingPortTypeAcaRidB:
		case EUsbChargingPortTypeNone:
		case EUsbChargingPortTypeUnsupported:
		    iParent.SetState(EPluginStateIdle);
			break;
		case EUsbChargingPortTypeAcaRidC:
		case EUsbChargingPortTypeStandardDownstreamPort:
		case EUsbChargingPortTypeChargingDownstreamPort:
		case EUsbChargingPortTypeChargingPort:
		default:
			break;
    	}
    }
#endif

void TUsbBatteryChargingPluginStateBase::UsbDeviceStateChange(TInt aLastError,
    TUsbDeviceState aOldState, TUsbDeviceState aNewState)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_USBDEVICESTATECHANGE_ENTRY );
	(void)aLastError;
	(void)aOldState;
    iParent.iDeviceState = aNewState; 
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_USBDEVICESTATECHANGE_EXIT );
    }

  
void TUsbBatteryChargingPluginStateBase::DeviceStateTimeout()
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_DEVICESTATETIMEOUT_ENTRY );
    OstTraceExt3( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_DEVICESTATETIMEOUT, "TUsbBatteryChargingPluginStateBase::DeviceStateTimeout;Time: %u Plugin State = %d, Device State = %d", User::NTickCount(), (TInt32)iParent.iPluginState, (TInt32)iParent.iDeviceState );
    
    iParent.iDeviceReEnumerator->Cancel(); // cancel re-enumeration AO
    
    iParent.SetState(EPluginStateIdle);
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_DEVICESTATETIMEOUT_EXIT );
    }

// For host OTG enabled charging plug-in
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
void TUsbBatteryChargingPluginStateBase::MpsoIdPinStateChanged(TInt aValue)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOIDPINSTATECHANGED_ENTRY );

    OstTrace1( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOIDPINSTATECHANGED, "TUsbBatteryChargingPluginStateBase::MpsoIdPinStateChanged;IdPinState changed => %d", aValue );
    
    // Disable charging here when IdPin is present
    // When IdPin disappears (i.e. the phone becomes B-Device), all necessary step are performed 
    // in UsbDeviceStateChange() method

    iParent.iIdPinState = aValue;
    
    // For all other states besides EPluginStateUserDisabled
    switch(aValue)
        {
        case EUsbBatteryChargingIdPinARole:
            TRAP_IGNORE(iParent.SetInitialConfigurationL());
            iParent.SetState(EPluginStateBEndedCableNotPresent);
			iParent.iAvailableMilliAmps = 0;
			OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOIDPINSTATECHANGED_EXIT );
            return;

        case EUsbBatteryChargingIdPinBRole:
            break;

        default:			
            iParent.SetState(EPluginStateIdle);
            break;
        }
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOIDPINSTATECHANGED_EXIT_DUP1 );
    }

void TUsbBatteryChargingPluginStateBase::MpsoOtgStateChangedL(TUsbOtgState aNewState)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOOTGSTATECHANGEDL_ENTRY );
    
    iParent.iOtgState = aNewState;
    
    // Not use currently
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOOTGSTATECHANGEDL_EXIT );
    }
#endif

void TUsbBatteryChargingPluginStateBase::MpsoVBusStateChanged(TInt aNewState)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOVBUSSTATECHANGED_ENTRY );
    
    if (aNewState == iParent.iVBusState)
        {
        OstTrace1( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOVBUSSTATECHANGED, "TUsbBatteryChargingPluginStateBase::MpsoVBusStateChanged;Receive VBus State Change notification without any state change: aNewState = %d", aNewState );
        OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOVBUSSTATECHANGED_EXIT );
        return;//should not happen??
        }

    OstTraceExt2( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOVBUSSTATECHANGED_DUP1, "TUsbBatteryChargingPluginStateBase::MpsoVBusStateChanged;VBusState changed from %d to %d", iParent.iVBusState, aNewState );
    
    iParent.iVBusState = aNewState;
    if (aNewState == 0) // VBus drop down - we have disconnected from host
        {
		iParent.iAvailableMilliAmps = 0;
        iParent.SetState(EPluginStateIdle);
        }
    // The handling of VBus on will be down in DeviceStateChanged implicitly
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_MPSOVBUSSTATECHANGED_EXIT_DUP1 );
    }


TUsbBatteryChargingPluginStateBase::TUsbBatteryChargingPluginStateBase (
        CUsbBatteryChargingPlugin& aParentStateMachine ): 
        iParent(aParentStateMachine)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_TUSBBATTERYCHARGINGPLUGINSTATEBASE_CONS_ENTRY );
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEBASE_TUSBBATTERYCHARGINGPLUGINSTATEBASE_CONS_EXIT );
    }

        
// Charging plugin idle state

TUsbBatteryChargingPluginStateIdle::TUsbBatteryChargingPluginStateIdle (
        CUsbBatteryChargingPlugin& aParentStateMachine ) :
    TUsbBatteryChargingPluginStateBase(aParentStateMachine)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLE_TUSBBATTERYCHARGINGPLUGINSTATEIDLE_CONS_ENTRY );
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLE_TUSBBATTERYCHARGINGPLUGINSTATEIDLE_CONS_EXIT );
    };

void TUsbBatteryChargingPluginStateIdle::UsbDeviceStateChange(
        TInt aLastError, TUsbDeviceState aOldState, TUsbDeviceState aNewState)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLE_USBDEVICESTATECHANGE_ENTRY );

    OstTraceExt3( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLE_USBDEVICESTATECHANGE, "TUsbBatteryChargingPluginStateIdle::UsbDeviceStateChange;aLastError=%d;aOldState=%d;aNewState=%d", aLastError, aOldState, aNewState );
    (void)aLastError;
    (void)aOldState;
    iParent.iDeviceState = aNewState;
    iParent.LogStateText(aNewState);

    switch (iParent.iDeviceState)
        {
        case EUsbDeviceStateAddress:
            {
            if (iParent.IsUsbChargingPossible())
                {
                iParent.NegotiateChargingCurrent();
                }
            }
            break;
        }
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLE_USBDEVICESTATECHANGE_EXIT );
    }
    
    
// Charging plugin current negotiating state

TUsbBatteryChargingPluginStateCurrentNegotiating::TUsbBatteryChargingPluginStateCurrentNegotiating (
        CUsbBatteryChargingPlugin& aParentStateMachine ) :
    TUsbBatteryChargingPluginStateBase(aParentStateMachine)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_CONS_ENTRY );
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_CONS_EXIT );
    };
    
void TUsbBatteryChargingPluginStateCurrentNegotiating::UsbDeviceStateChange(
        TInt aLastError, TUsbDeviceState aOldState, TUsbDeviceState aNewState)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_USBDEVICESTATECHANGE_ENTRY );

    OstTraceExt3( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_USBDEVICESTATECHANGE, "TUsbBatteryChargingPluginStateCurrentNegotiating::UsbDeviceStateChange;aLastError=%d;aOldState=%d;aNewState=%d", aLastError, aOldState, aNewState );
    (void)aLastError;
    (void)aOldState;
    iParent.iDeviceState = aNewState;
    iParent.LogStateText(aNewState);
    
    switch(iParent.iDeviceState)
        {
        case EUsbDeviceStateConfigured:
            if (iParent.IsUsbChargingPossible())
                {
				iParent.iAvailableMilliAmps = iParent.iRequestedCurrentValue;			 
                iParent.iDeviceStateTimer->Cancel();                
                OstTrace1( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_USBDEVICESTATECHANGE_DUP1, "TUsbBatteryChargingPluginStateCurrentNegotiating::UsbDeviceStateChange;iParent.iAvailableMilliAmps=%d", iParent.iAvailableMilliAmps );
                
                if(0 != iParent.iRequestedCurrentValue)
                    {
                    iParent.SetState(EPluginStateIdleNegotiated);
                    OstTrace1( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_USBDEVICESTATECHANGE_DUP2, "TUsbBatteryChargingPluginStateCurrentNegotiating::UsbDeviceStateChange;PluginState => EPluginStateCharging(%d)", iParent.iPluginState );
                    }
                else
                    {
                    // Host can only accept 0 charging current
                    // No way to do charging
                    iParent.SetState(EPluginStateNoValidCurrent);
                    OstTrace1( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_USBDEVICESTATECHANGE_DUP3, "TUsbBatteryChargingPluginStateCurrentNegotiating::UsbDeviceStateChange;No more current value to try, iPluginState turned to %d", iParent.iPluginState );
                    }
                }
            break;
        
        // If no configured received, there must be a timeout
        // caught by the iDeviceStateTimer, and it will try next value or send state to  
        // EPluginStateNoValidCurrent, so don't worry that we omit something important :-)
            
            
        default:
            break;
        }
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_USBDEVICESTATECHANGE_EXIT );
    }

void TUsbBatteryChargingPluginStateCurrentNegotiating::DeviceStateTimeout()
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_DEVICESTATETIMEOUT_ENTRY );
    OstTraceExt3( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_DEVICESTATETIMEOUT, "TUsbBatteryChargingPluginStateCurrentNegotiating::DeviceStateTimeout;Time: %d Plugin State = %d, Device State = %d", User::NTickCount(), (TInt32)iParent.iPluginState, (TInt32)iParent.iDeviceState );
    
    iParent.iDeviceReEnumerator->Cancel(); // cancel re-enumeration AO
    
    if(iParent.iRequestedCurrentValue != 0)
        {
        // If there are more value to try ...
        iParent.NegotiateChargingCurrent();
        }
    else
        {
        // The Host doesn't accept 0ma power request?
        // Assume it will never happens.
        iParent.SetState(EPluginStateNoValidCurrent);
        }
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATECURRENTNEGOTIATING_DEVICESTATETIMEOUT_EXIT );
    }


// Charging plugin negotiated fail state
    
    
TUsbBatteryChargingPluginStateNoValidCurrent::TUsbBatteryChargingPluginStateNoValidCurrent (
        CUsbBatteryChargingPlugin& aParentStateMachine ) :
    TUsbBatteryChargingPluginStateBase(aParentStateMachine)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATENOVALIDCURRENT_TUSBBATTERYCHARGINGPLUGINSTATENOVALIDCURRENT_CONS_ENTRY );
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATENOVALIDCURRENT_TUSBBATTERYCHARGINGPLUGINSTATENOVALIDCURRENT_CONS_EXIT );
    };

    
// Charging plugin idle negotiated state

TUsbBatteryChargingPluginStateIdleNegotiated::TUsbBatteryChargingPluginStateIdleNegotiated (
        CUsbBatteryChargingPlugin& aParentStateMachine ) :
    TUsbBatteryChargingPluginStateBase(aParentStateMachine)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLENEGOTIATED_TUSBBATTERYCHARGINGPLUGINSTATEIDLENEGOTIATED_CONS_ENTRY );
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLENEGOTIATED_TUSBBATTERYCHARGINGPLUGINSTATEIDLENEGOTIATED_CONS_EXIT );
    };

void TUsbBatteryChargingPluginStateIdleNegotiated::UsbDeviceStateChange(
        TInt aLastError, TUsbDeviceState aOldState, TUsbDeviceState aNewState)
    {
    OstTraceFunctionEntry0( REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLENEGOTIATED_USBDEVICESTATECHANGE_ENTRY );

    OstTraceExt3( TRACE_NORMAL, REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLENEGOTIATED_USBDEVICESTATECHANGE, "TUsbBatteryChargingPluginStateIdleNegotiated::UsbDeviceStateChange;aLastError=%d;aOldState=%d;aNewState=%d", aLastError, aOldState, aNewState );
    (void)aLastError;
    (void)aOldState;
    iParent.iDeviceState = aNewState;
    iParent.LogStateText(aNewState);
	if (aNewState != EUsbDeviceStateConfigured && aNewState != EUsbDeviceStateSuspended)
		{
		iParent.iAvailableMilliAmps = 0;
		iParent.SetState(EPluginStateIdle);
		}
    OstTraceFunctionExit0( REF_TUSBBATTERYCHARGINGPLUGINSTATEIDLENEGOTIATED_USBDEVICESTATECHANGE_EXIT );
    }
 
// Charging plugin A-role state
    
TUsbBatteryChargingPluginStateBEndedCableNotPresent::TUsbBatteryChargingPluginStateBEndedCableNotPresent (
        CUsbBatteryChargingPlugin& aParentStateMachine ) :
    TUsbBatteryChargingPluginStateBase(aParentStateMachine)
    {
    OstTraceFunctionEntry0( RES_TUSBBATTERYCHARGINGPLUGINSTATEBENDEDCABLENOTPRESENT_TUSBBATTERYCHARGINGPLUGINSTATEBENDEDCABLENOTPRESENT_CONS_ENTRY );
    OstTraceFunctionExit0( RES_TUSBBATTERYCHARGINGPLUGINSTATEBENDEDCABLENOTPRESENT_TUSBBATTERYCHARGINGPLUGINSTATEBENDEDCABLENOTPRESENT_CONS_EXIT );
    }

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
void TUsbBatteryChargingPluginStateBEndedCableNotPresent::MpsoIdPinStateChanged(TInt aValue)
    {
    iParent.iIdPinState = aValue;
    
    switch(aValue)
        {
        case EUsbBatteryChargingIdPinARole:
            break;
        case EUsbBatteryChargingIdPinBRole:
            iParent.SetState(EPluginStateIdle);
            break;
        default:			
            iParent.SetState(EPluginStateIdle);
            break;
        }
    }
#endif
    


