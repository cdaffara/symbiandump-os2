// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// e32/drivers/usbcc/chapter9.cpp
// Platform independent layer (PIL) of the USB Device controller driver:
// Processing of USB spec chapter 9 standard requests.
// 
//

/**
 @file chapter9.cpp
 @internalTechnology
*/

#include <usb/usbc.h>

#include "controltransfersm.h"

//#define ENABLE_EXCESSIVE_DEBUG_OUTPUT
//
// === USB Controller member function implementation - PSL API (protected) ========================
//

/** Used to synchronize the Ep0 state machine between the PSL and PIL.
    Accepts a SETUP packet and returns the next Ep0 state.

    @param aSetupBuf The SETUP packet just received by the PSL.
    @return The next Ep0 state.

    @publishedPartner @released
*/
UsbShai::TControlStage DUsbClientController::EnquireEp0NextStage(const TUint8* aSetupBuf) const
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::EnquireEp0NextState()"));

    // This function may be called by the PSL from within an ISR -- so we have
    // to take care what we do here (and also in all functions that get called
    // from here).

    if (SWAP_BYTES_16((reinterpret_cast<const TUint16*>(aSetupBuf)[3])) == 0) // iLength
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  --> EControlTransferStageStatusIn"));
        return UsbShai::EControlTransferStageStatusIn;                            // No-data Control => Status_IN
        }
    else if ((aSetupBuf[0] & KUsbRequestType_DirMask) == KUsbRequestType_DirToDev)
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  --> EControlTransferStageDataOut"));
        return UsbShai::EControlTransferStageDataOut;                            // Control Write => Data_OUT
        }
    else
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  --> EControlTransferStageDataIn"));
        return UsbShai::EControlTransferStageDataIn;                                // Control Read => Data_IN
        }
    }

//
// About iLastError.
// This member is used to remember the last error happend during a
// processXXX likewise function.
//
// Before entry of each ProcessXXX, iLastError will be cleared to KErrNone.
//

// --- The USB Spec Chapter 9 Standard Endpoint Zero Device Requests ---
// Record error happend with iLastError, the value already been set to zero
// before entering ProcessSetupPacket call.
void DUsbClientController::ProcessGetDeviceStatus(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessGetDeviceStatus()"));
    if ( iDeviceState < UsbShai::EUsbPeripheralStateAddress)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }
    else
        {
        // We always assume Device is bus-powered, even though mobile phone almost always
        // has a battry there, because self-powered device can not required more then 100ma current
        // which is not acceptable for Usb charging.
        TBool selfPowered = EFalse;         
        
        /*  FIXME: modify selfPowered to make it compilance with following rule.
           
             1. If current draw exceeds 100mA, the device must report itself 
                as bus-powered during enumeration.
             
             2. In all cases, the GetStatus(DEVICE) call must accurately report 
                whether the device is currently operating on self- or bus-power.
                
             3. A device that is actively drawing more than 100mA from USB must 
                report itself as bus-powered in the GetStatus(DEVICE) call.
             
             4. Peripherals that return "Self-powered" in the GetStatus(DEVICE) 
                call are prohibited from drawing more than 100mA at any time.
        */
        /*   
        TBuf8<KUsbDescSize_Config> config;
        
        if(iDescriptors.GetConfigurationDescriptorTC(&Kern::CurrentThread(),config) == KErrNone)
            {
            TUint8 maxPower = config[8];
            if(maxPower <= 50)
                {
                selfPowered = EFalse;
                }
            }
        */
        
        const TUint16 status = ((selfPowered ? KUsbDevStat_SelfPowered : 0) |
                        (iRmWakeupStatus_Enabled ? KUsbDevStat_RemoteWakeup : 0));
        __KTRACE_OPT(KUSB, Kern::Printf("  Reporting device status: 0x%02x", status));
        *reinterpret_cast<TUint16*>(iEp0_TxBuf) = SWAP_BYTES_16(status);
        if (iConTransferMgr->SetupEndpointZeroWrite(iEp0_TxBuf, sizeof(status)) == KErrNone)
            {
            iEp0WritePending = ETrue;
            }
        else
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Wrong: Write to Ep0 Failed"));
            }
        }
    }

void DUsbClientController::ProcessGetInterfaceStatus(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessGetInterfaceStatus()"));
    if ( iDeviceState < UsbShai::EUsbPeripheralStateConfigured)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }
    else
        {
        if (InterfaceExists(aPacket.iIndex) == EFalse)
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Interface does not exist"));
            iLastError = KErrGeneral;
            }
        else
            {        
            const TUint16 status = 0x0000;                            // as of USB Spec 2.0
            __KTRACE_OPT(KUSB, Kern::Printf("  Reporting interface status: 0x%02x", status));
            *reinterpret_cast<TUint16*>(iEp0_TxBuf) = SWAP_BYTES_16(status);
            if (iConTransferMgr->SetupEndpointZeroWrite(iEp0_TxBuf, sizeof(status)) == KErrNone)
                {
                iEp0WritePending = ETrue;
                }
            else
                {
                __KTRACE_OPT(KUSB, Kern::Printf("  Wrong: Write to Ep0 Failed"));
                }
            }
        }
    }


void DUsbClientController::ProcessGetEndpointStatus(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessGetEndpointStatus()"));
    if (
        ((iDeviceState < UsbShai::EUsbPeripheralStateAddress) ||
         (iDeviceState == UsbShai::EUsbPeripheralStateAddress && (aPacket.iIndex & KUsbEpAddress_Portmask) != 0)))
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }
    else
        {
        if (EndpointExists(aPacket.iIndex) == EFalse)
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Endpoint does not exist"));
            iLastError = KErrGeneral;
            }
        else
            {
            const TInt ep = EpAddr2Idx(aPacket.iIndex);
            const TUint16 status = (iRealEndpoints[ep].iHalt) ?     KUsbEpStat_Halt : 0;
            __KTRACE_OPT(KUSB, Kern::Printf("  Reporting endpoint status 0x%02x for real endpoint %d",
                                            status, ep));
            *reinterpret_cast<TUint16*>(iEp0_TxBuf) = SWAP_BYTES_16(status);
            if (iConTransferMgr->SetupEndpointZeroWrite(iEp0_TxBuf, 2) == KErrNone)
                {
                iEp0WritePending = ETrue;
                }
            else
                {
                __KTRACE_OPT(KUSB, Kern::Printf("  Wrong: Write to Ep0 Failed"));
                }
            }
        }
    }


void DUsbClientController::ProcessSetClearDevFeature(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessSetClearDevFeature()"));
    if ( iDeviceState < UsbShai::EUsbPeripheralStateDefault)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        return;
        }

    TInt test_sel = 0;

    if (aPacket.iRequest == KUsbRequest_SetFeature)
        {
        switch (aPacket.iValue)
            {
            case KUsbFeature_RemoteWakeup:
                if ( iDeviceState < UsbShai::EUsbPeripheralStateAddress)
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
                    iLastError = KErrGeneral;                
                    }
                else
                    {
                    iRmWakeupStatus_Enabled = ETrue;
                    }
                break;
                
            case KUsbFeature_TestMode:
                if (!iHighSpeed)
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Request only supported in High-Speed mode"));
                    iLastError = KErrGeneral;
                    }
                else if (LowByte(aPacket.iIndex) != 0)
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Lower byte of wIndex must be zero"));
                    iLastError = KErrGeneral;
                    }
                else
                    {
                    test_sel = HighByte(aPacket.iIndex);
                    if ((test_sel < UsbShai::EUsbTestSelector_Test_J) || (test_sel > UsbShai::EUsbTestSelector_Test_Force_Enable))
                        {
                        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid test selector: %d", test_sel));
                        iLastError = KErrGeneral;
                        }
                    }
                break;
                
            case KUsbFeature_B_HnpEnable:
                if (!iOtgSupport)
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Request only supported on a OTG device"));
                    iLastError = KErrGeneral;
                    }            
                else if (!(iOtgFuncMap & KUsbOtgAttr_HnpSupp))
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Request only valid if OTG device supports HNP"));
                    iLastError = KErrGeneral;
                    }
                else
                    {
                    iOtgFuncMap |= KUsbOtgAttr_B_HnpEnable;
                    OtgFeaturesNotify();
                    }
                break;
                
            case KUsbFeature_A_HnpSupport:
                if (!iOtgSupport)
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Request only supported on a OTG device"));
                    iLastError = KErrGeneral;
                    }
                else if (!(iOtgFuncMap & KUsbOtgAttr_HnpSupp))
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Request only valid if OTG device supports HNP"));
                    iLastError = KErrGeneral;
                    }
                else
                    {
                    iOtgFuncMap |= KUsbOtgAttr_A_HnpSupport;
                    OtgFeaturesNotify();
                    }
                break;
                
            case KUsbFeature_A_AltHnpSupport:
                if (!iOtgSupport)
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Request only supported on a OTG device"));
                    iLastError = KErrGeneral;
                    }
                else if (!(iOtgFuncMap & KUsbOtgAttr_HnpSupp))
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Request only valid if OTG device supports HNP"));
                    iLastError = KErrGeneral;
                    }
                else
                    {
                    iOtgFuncMap |= KUsbOtgAttr_A_AltHnpSupport;
                    OtgFeaturesNotify();
                    }
                break;
                
            default:
                __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Unknown feature requested"));
                iLastError = KErrGeneral;                
            }
        }
    else // KUsbRequest_ClearFeature
        {
        switch (aPacket.iValue)
            {
            case KUsbFeature_RemoteWakeup:
                if ( iDeviceState < UsbShai::EUsbPeripheralStateAddress)
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
                    iLastError = KErrGeneral;
                    }
                else
                    {
                    iRmWakeupStatus_Enabled = EFalse;
                    }
                break;
                
            default:
                __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Unknown feature requested"));
                iLastError = KErrGeneral;
            }
        }
    
    if(iLastError == KErrNone)
        {
        // Sent out status packet if no error found.
        iConTransferMgr->SendEp0ZeroByteStatusPacket();                            // success: zero bytes data during status stage
        
        // 9.4.9: "The transition to test mode of an upstream facing port must not happen until
        // after the status stage of the request."
        if (test_sel)
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Entering HS Test Mode %d", test_sel));
            iController.EnterTestMode((UsbShai::TUsbTestModeSelector)test_sel);
            }
        }
    }


void DUsbClientController::ProcessSetClearIfcFeature(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessSetClearIfcFeature()"));
    if ( iDeviceState < UsbShai::EUsbPeripheralStateConfigured)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }
    else
        {
        // No interface features defined in USB spec, thus
        iLastError = KErrGeneral;
        }
    }


void DUsbClientController::ProcessSetClearEpFeature(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessSetClearEpFeature()"));
    if (
        ((iDeviceState < UsbShai::EUsbPeripheralStateAddress) ||
         (iDeviceState == UsbShai::EUsbPeripheralStateAddress && (aPacket.iIndex & KUsbEpAddress_Portmask) != 0)))
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }
    else if (aPacket.iValue != KUsbFeature_EndpointHalt)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Unknown feature requested"));
        iLastError = KErrGeneral;
        }
    else if (EndpointExists(aPacket.iIndex) == EFalse)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Endpoint does not exist"));
        iLastError = KErrGeneral;
        }
    else
        {
        const TInt ep = EpAddr2Idx(aPacket.iIndex);
        if (iRealEndpoints[ep].iLEndpoint->iInfo.iType == UsbShai::KUsbEpTypeControl ||
            iRealEndpoints[ep].iLEndpoint->iInfo.iType == UsbShai::KUsbEpTypeIsochronous)
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Endpoint is Control or Isochronous"));
            iLastError = KErrGeneral;
            }
        else
            {
            SetClearHaltFeature(ep, aPacket.iRequest);
        
            // success: zero bytes data during status stage
            iConTransferMgr->SendEp0ZeroByteStatusPacket();
            }
        }
    }


void DUsbClientController::ProcessSetAddress(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessSetAddress()"));
    if ( iDeviceState > UsbShai::EUsbPeripheralStateAddress)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }
    else
        {
        const TUint16 addr = aPacket.iValue;
        
        if (addr > 127)
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Bad address value: %d (>127)", addr));
            iLastError = KErrGeneral;
            }
        else if (addr == 0)
            {
            // Enter Default state (from Default or Address)
            NextDeviceState(UsbShai::EUsbPeripheralStateDefault);
            }
        
        __KTRACE_OPT(KUSB, Kern::Printf("  USB address: %d", addr));
        
        // If controller support hw acceleration,call set address first and then status
        if(iControllerProperties.iControllerCaps & UsbShai::KDevCapSetAddressAcceleration)
            {
            iController.SetDeviceAddress(addr);
            }

        // The spec says, under section 9.4.6:
        // "Stages after the initial Setup packet assume the same device address as the Setup packet. The USB
        // device does not change its device address until after the Status stage of this request is completed
        // successfully. Note that this is a difference between this request and all other requests. For all other
        // requests, the operation indicated must be completed before the Status stage."
        // Therefore, here we first send the status packet and only then actually execute the request.
        iConTransferMgr->SendEp0ZeroByteStatusPacket();
        
        // If controller doesn't support hw acceleration, call set address after status
        if((iControllerProperties.iControllerCaps & UsbShai::KDevCapSetAddressAcceleration) == 0)
            {
            iController.SetDeviceAddress(addr);
            }
        }
    }


void DUsbClientController::ProcessGetDescriptor(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessGetDescriptor()"));
    if ( iDeviceState < UsbShai::EUsbPeripheralStateDefault)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        return ;
        }

    // Make sure we assume the correct speed
    __ASSERT_DEBUG((iHighSpeed == CurrentlyUsingHighSpeed()), Kern::Fault(KUsbPILPanicCat, __LINE__));

    TInt size = 0;
    const TInt result = iDescriptors.FindDescriptor(HighByte(aPacket.iValue), // Type
                                                    LowByte(aPacket.iValue), // Index
                                                    aPacket.iIndex, // Language ID
                                                    size);

    if ((result != KErrNone) || (size == 0))
        {
        // This doesn't have to be an error - protocol-wise it's OK.
        __KTRACE_OPT(KUSB, Kern::Printf("  Couldn't retrieve descriptor"));
        iLastError = KErrGeneral;
        return;
        }

    __KTRACE_OPT(KUSB, Kern::Printf("  Descriptor found, size: %d (requested: %d)",
                                    size, aPacket.iLength));
    if (size > KUsbcBufSz_Ep0Tx)
        {
        // This should actually not be possible (i.e. we should never get here).
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Ep0_Tx buffer too small"));
        }
    if (size > aPacket.iLength)
        {
        // Send only as much data as requested by the host
        size = aPacket.iLength;
        }

#ifdef ENABLE_EXCESSIVE_DEBUG_OUTPUT
    __KTRACE_OPT(KUSB,
                 Kern::Printf("  Data: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ...",
                              iEp0_TxBuf[0], iEp0_TxBuf[1], iEp0_TxBuf[2], iEp0_TxBuf[3],
                              iEp0_TxBuf[4], iEp0_TxBuf[5], iEp0_TxBuf[6], iEp0_TxBuf[7]));
#endif
    // If we're about to send less bytes than expected by the host AND our number is a
    // multiple of the packet size, in order to indicate the end of the control transfer,
    // we must finally send a zero length data packet (ZLP):
    const TBool zlp = ((size < aPacket.iLength) && (size % iEp0MaxPacketSize == 0));
    if (iConTransferMgr->SetupEndpointZeroWrite(iEp0_TxBuf, size, zlp) == KErrNone)
        {
        iEp0WritePending = ETrue;
        }
    else
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  Wrong: Write to Ep0 Failed"));
        }
    }


void DUsbClientController::ProcessSetDescriptor(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessSetDescriptor()"));
#ifndef USB_SUPPORTS_SET_DESCRIPTOR_REQUEST
    iLastError = KErrGeneral;
    return;
#else
    if ( iDeviceState < UsbShai::EUsbPeripheralStateAddress)
        {
        // Error: Invalid device state!
        iLastError = KErrGeneral;
        }
    else if (aPacket.iLength > KUsbcBufSz_Ep0Rx)
        {
        // Error: Our Rx buffer is too small! (Raise a defect to make it larger)
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Ep0_Rx buffer too small"));
        iLastError = KErrGeneral;
        }
    else
        {
        SetEp0DataOutVars();
        iConTransferMgr->SetupEndpointZeroRead();
        }
#endif
    }

void DUsbClientController::ProcessGetConfiguration(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessGetConfiguration()"));
    if ( iDeviceState < UsbShai::EUsbPeripheralStateAddress)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }
    else if ( iDeviceState == UsbShai::EUsbPeripheralStateAddress && iCurrentConfig != 0)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: DeviceState Address && Config != 0"));
        iLastError = KErrGeneral;
        }
    else if ( iDeviceState == UsbShai::EUsbPeripheralStateConfigured && iCurrentConfig == 0)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: DeviceState Configured && Config == 0"));
        iLastError = KErrGeneral;
        }
    else
        {
        if (aPacket.iLength != 1)                                // "unspecified behavior"
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Warning: wLength != 1 (= %d)", aPacket.iLength));
            }
        __KTRACE_OPT(KUSB, Kern::Printf("  Reporting configuration value %d", iCurrentConfig));
        if (iConTransferMgr->SetupEndpointZeroWrite(&iCurrentConfig, sizeof(iCurrentConfig)) == KErrNone)
            {
            iEp0WritePending = ETrue;
            }
        else
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Wrong: Write to Ep0 Failed"));
            }
        }
    }


/** Changes the device's configuration value, including interface setup and/or
    teardown and state change notification of higher-layer clients.
    May also be called by the PSL in special cases - therefore publishedPartner.

    @param aPacket The received Ep0 SET_CONFIGURATION setup request packet.
    @return KErrGeneral in case of a protocol error, KErrNone otherwise.

    @publishedPartner @released
*/
TInt DUsbClientController::ProcessSetConfiguration(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessSetConfiguration()"));

    // This function may be called by the PSL from within an ISR -- so we have
    // to take care what we do here (and also in all functions that get called
    // from here).
    const TInt value = aPacket.iValue;
    
    if ( iDeviceState < UsbShai::EUsbPeripheralStateAddress)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }    
    else if (value > 1)                                            // we support only one configuration
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Configuration value too large: %d", value));
        iLastError = KErrGeneral;
        }
    else
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  Configuration value: %d", value));
        ChangeConfiguration(value);

        // In 9.4.5 under GET_STATUS we read, that after SET_CONFIGURATION the HALT feature
        // for all endpoints is reset to zero.
        TInt num = 0;
        (TAny) DoForEveryEndpointInUse(&DUsbClientController::ClearHaltFeature, num);
        __KTRACE_OPT(KUSB, Kern::Printf("  Called ClearHaltFeature() for %d endpoints", num));
        // success: zero bytes data during status stage
        iConTransferMgr->SendEp0ZeroByteStatusPacket();
        }
        
    return iLastError;
    }

void DUsbClientController::ProcessGetInterface(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessGetInterface()"));
    
    const TInt number = aPacket.iIndex;
    
    if ( iDeviceState < UsbShai::EUsbPeripheralStateConfigured)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }
    else if (iCurrentConfig == 0)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Device not configured"));
        iLastError = KErrGeneral;
        }    
    else if (!InterfaceExists(number))
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Bad interface index: %d", number));
        iLastError = KErrGeneral;
        }
    else
        {
        // Send alternate setting code of iCurrentInterface of Interface(set) <number> of the current
        // config (iCurrentConfig).
        const TUint8 setting = InterfaceNumber2InterfacePointer(number)->iCurrentInterface;
        __KTRACE_OPT(KUSB, Kern::Printf("  Reporting interface setting %d", setting));
        if (iConTransferMgr->SetupEndpointZeroWrite(&setting, 1) == KErrNone)
            {
            iEp0WritePending = ETrue;
            }
        else
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Wrong: Write to Ep0 Failed"));
            }
        }
    }


void DUsbClientController::ProcessSetInterface(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessSetInterface()"));
    
    const TInt number = aPacket.iIndex;
    
    if ( iDeviceState < UsbShai::EUsbPeripheralStateConfigured)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }
    else if (iCurrentConfig == 0)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Device not configured"));
        iLastError = KErrGeneral;
        }    
    else if (!InterfaceExists(number))
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Bad interface index: %d", number));
        iLastError = KErrGeneral;
        }
    else
        {
        const TInt setting = aPacket.iValue;
        TUsbcInterfaceSet* const ifcset_ptr = InterfaceNumber2InterfacePointer(number);
        RPointerArray<TUsbcInterface>& ifcs = ifcset_ptr->iInterfaces;
        if (setting >= ifcs.Count())
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Alt Setting >= bNumAltSettings: %d", setting));
            iLastError = KErrGeneral;
            }
        else
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Interface setting:: %d", setting));
            // Set iCurrentInterface of Interface(set) <number> of the current config
            // (iCurrentConfig) to alternate setting <setting>.
            ChangeInterface(ifcs[setting]);
            // In 9.4.5 under GET_STATUS we read, that after SET_INTERFACE the HALT feature
            // for all endpoints (of the now current interface setting) is reset to zero.
            RPointerArray<TUsbcLogicalEndpoint>& eps = ifcset_ptr->CurrentInterface()->iEndpoints;
            const TInt num_eps = eps.Count();
            for (TInt i = 0; i < num_eps; i++)
                {
                const TInt ep_num = EpAddr2Idx(eps[i]->iPEndpoint->iEndpointAddr);
                (TAny) ClearHaltFeature(ep_num);
                }
            // success: zero bytes data during status stage
            iConTransferMgr->SendEp0ZeroByteStatusPacket();    
            }
        }
    }


void DUsbClientController::ProcessSynchFrame(const TUsbcSetup& aPacket)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProcessSynchFrame()"));
    
    const TInt ep = aPacket.iIndex;
    
    if ( iDeviceState < UsbShai::EUsbPeripheralStateConfigured)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
        iLastError = KErrGeneral;
        }    
    else if (EndpointExists(ep) == EFalse)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Endpoint does not exist"));
        iLastError = KErrGeneral;
        }
    else if (iRealEndpoints[EpAddr2Idx(ep)].iLEndpoint->iInfo.iType != UsbShai::KUsbEpTypeIsochronous)
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Endpoint is not isochronous"));
        iLastError = KErrGeneral;
        }
    else 
        {
        // We always send 0:
        *reinterpret_cast<TUint16*>(iEp0_TxBuf) = 0x00;
        if (iConTransferMgr->SetupEndpointZeroWrite(iEp0_TxBuf, 2) == KErrNone)
            {
            iEp0WritePending = ETrue;
            }
        else
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Wrong: Write to Ep0 Failed"));
            }
        }
    }


#ifdef USB_SUPPORTS_SET_DESCRIPTOR_REQUEST
void DUsbClientController::ProceedSetDescriptor()
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ProceedSetDescriptor()"));
    // iEp0DataReceived already reflects the current buffer state
    if (iEp0DataReceived < iConTransferMgr->PktParser().DataLength())
        {
        // Not yet all data received => proceed
        return;
        }
    if (iEp0DataReceived > iConTransferMgr->PktParser().DataLength())
        {
        // Error: more data received than expected
        // but we don't care...
        }
    const TUint8 type = HighByte(iConTransferMgr->PktParser().Value());
    if (type == KUsbDescType_String)
        {
        // set/add new string descriptor
        }
    else
        {
        // set/add new ordinary descriptor
        }
    TUint8 index = LowByte(iConTransferMgr->PktParser().Value());
    TUint16 langid = iConTransferMgr->PktParser().Index();
    TUint16 length_total = iConTransferMgr->PktParser().DataLength();

    iConTransferMgr->SendEp0ZeroByteStatusPacket();
    }
#endif


// --- Secondary (Helper) Functions

void DUsbClientController::SetClearHaltFeature(TInt aRealEndpoint, TUint8 aRequest)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::SetClearHaltFeature()"));
    if (aRequest == KUsbRequest_SetFeature)
        {
        if (iRealEndpoints[aRealEndpoint].iHalt)
            {
            // (This condition is not really an error)
            __KTRACE_OPT(KUSB, Kern::Printf("  Warning: HALT feature already set"));
            return;
            }
        __KTRACE_OPT(KUSB, Kern::Printf("  setting HALT feature for real endpoint %d",
                                        aRealEndpoint));
        iController.StallEndpoint(aRealEndpoint);
        iRealEndpoints[aRealEndpoint].iHalt = ETrue;
        }
    else                                                    // KUsbRequest_ClearFeature
        {
        if (iRealEndpoints[aRealEndpoint].iHalt == EFalse)
            {
            // In this case, before we return, the data toggles are reset to DATA0.
            __KTRACE_OPT(KUSB, Kern::Printf("  Warning: HALT feature already cleared"));
            iController.ResetDataToggle(aRealEndpoint);
            return;
            }
        __KTRACE_OPT(KUSB, Kern::Printf("  clearing HALT feature for real endpoint %d",
                                        aRealEndpoint));
        iController.ResetDataToggle(aRealEndpoint);
        iController.ClearStallEndpoint(aRealEndpoint);
        iRealEndpoints[aRealEndpoint].iHalt = EFalse;
        }
    EpStatusNotify(aRealEndpoint);                            // only called if actually something changed
    }


TInt DUsbClientController::ClearHaltFeature(TInt aRealEndpoint)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ClearHaltFeature()"));
    if (iRealEndpoints[aRealEndpoint].iHalt != EFalse)
        {
        iController.ClearStallEndpoint(aRealEndpoint);
        iRealEndpoints[aRealEndpoint].iHalt = EFalse;
        }
    return KErrNone;
    }


void DUsbClientController::ChangeConfiguration(TUint16 aValue)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ChangeConfiguration()"));
    // New configuration is the same as the old one: 0
    if (iCurrentConfig == 0 && aValue == 0)
        {
        // no-op
        __KTRACE_OPT(KUSB, Kern::Printf("  Configuration: New == Old == 0 --> exiting"));
        return;
        }
    // New configuration is the same as the old one (but not 0)
    if (iCurrentConfig == aValue)
        {
        // no-op
        __KTRACE_OPT(KUSB, Kern::Printf("  Configuration: New == Old == %d --> exiting", aValue));
        return;
        }
    // Device is already configured
    if (iCurrentConfig != 0)
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  Device was configured: %d", iCurrentConfig));
        // Tear down all interface(set)s of the old configuration
        RPointerArray<TUsbcInterfaceSet>& ifcsets = CurrentConfig()->iInterfaceSets;
        for (TInt i = 0; i < ifcsets.Count(); ++i)
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Tearing down InterfaceSet %d", i));
            InterfaceSetTeardown(ifcsets[i]);
            }
        iCurrentConfig = 0;
        // Enter Address state (from Configured)
        if (iDeviceState == UsbShai::EUsbPeripheralStateConfigured)
            NextDeviceState(UsbShai::EUsbPeripheralStateAddress);
        }
    // Device gets a new configuration
    if (aValue != 0)
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  Device gets new configuration..."));
        // Setup all alternate settings 0 of all interfaces
        // (Don't separate the next two lines of code.)
        iCurrentConfig = aValue;
        RPointerArray<TUsbcInterfaceSet>& ifcsets = CurrentConfig()->iInterfaceSets;
        const TInt n = ifcsets.Count();
        for (TInt i = 0; i < n; ++i)
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Setting up InterfaceSet %d", i));
            InterfaceSetup(ifcsets[i]->iInterfaces[0]);
            }
        // Enter Configured state (from Address or Configured)
        NextDeviceState(UsbShai::EUsbPeripheralStateConfigured);
        }
    __KTRACE_OPT(KUSB, Kern::Printf("  New configuration: %d", iCurrentConfig));
    return;
    }


void DUsbClientController::InterfaceSetup(TUsbcInterface* aIfc)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::InterfaceSetup()"));
    const TInt num_eps = aIfc->iEndpoints.Count();
    for (TInt i = 0; i < num_eps; i++)
        {
        // Prepare this endpoint for I/O
        TUsbcLogicalEndpoint* const ep = aIfc->iEndpoints[i];
        // (TUsbcLogicalEndpoint's FS/HS endpoint sizes and interval values got
        //  adjusted in its constructor.)
        if (iHighSpeed)
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Setting Ep info size to %d (HS)", ep->iEpSize_Hs));
            ep->iInfo.iSize = ep->iEpSize_Hs;
            }
        else
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Setting Ep info size to %d (FS)", ep->iEpSize_Fs));
            ep->iInfo.iSize = ep->iEpSize_Fs;
            }
        const TInt idx = EpAddr2Idx(ep->iPEndpoint->iEndpointAddr);
        if (iController.ConfigureEndpoint(idx, ep->iInfo) != KErrNone)
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Endpoint %d configuration failed", idx));
            continue;
            }
        // Should there be a problem with it then we could try resetting the ep
        // data toggle at this point (or before the Configure) as well.
        __KTRACE_OPT(KUSB, Kern::Printf("  Connecting real ep addr 0x%02x & logical ep #%d",
                                        ep->iPEndpoint->iEndpointAddr, ep->iLEndpointNum));
        ep->iPEndpoint->iLEndpoint = ep;
        }
    aIfc->iInterfaceSet->iCurrentInterface = aIfc->iSettingCode;
    return;
    }


void DUsbClientController::InterfaceSetTeardown(TUsbcInterfaceSet* aIfcSet)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::InterfaceSetTeardown()"));
    if (aIfcSet->iInterfaces.Count() == 0)
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  No interfaces exist - returning"));
        return;
        }
    RPointerArray<TUsbcLogicalEndpoint>& eps = aIfcSet->CurrentInterface()->iEndpoints;
    const TInt num_eps = eps.Count();
    for (TInt i = 0; i < num_eps; i++)
        {
        TUsbcLogicalEndpoint* const ep = eps[i];
        const TInt idx = EpAddr2Idx(ep->iPEndpoint->iEndpointAddr);

        CancelTransferRequests(idx);

        if (!ep->iPEndpoint->iLEndpoint)
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  real ep %d not configured: skipping", idx));
            continue;
            }
        if (iController.ResetDataToggle(idx) != KErrNone)
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Endpoint %d data toggle reset failed", idx));
            }
        if (iController.DeConfigureEndpoint(idx) != KErrNone)
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Endpoint %d de-configuration failed", idx));
            }

        __KTRACE_OPT(KUSB, Kern::Printf("  disconnecting real ep & logical ep"));
        ep->iPEndpoint->iLEndpoint = NULL;
        }
    if (aIfcSet->CurrentInterface() != 0)
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  Resetting alternate interface setting to 0"));
        //Add this mutex to protect the interface set data structure
        if (NKern::CurrentContext() == EThread)
            {
            NKern::FMWait(&iMutex);
            }
        
        aIfcSet->iCurrentInterface = 0;
        if (NKern::CurrentContext() == EThread)
            {
            NKern::FMSignal(&iMutex);
            }        
        }
    return;
    }


void DUsbClientController::ChangeInterface(TUsbcInterface* aIfc)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ChangeInterface()"));
    TUsbcInterfaceSet* ifcset = aIfc->iInterfaceSet;
    const TUint8 setting = aIfc->iSettingCode;
    if (ifcset->iCurrentInterface == setting)
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  New Ifc == old Ifc: nothing to do"));
        return;
        }
    __KTRACE_OPT(KUSB, Kern::Printf("  Setting new interface setting #%d", setting));
    InterfaceSetTeardown(ifcset);
    InterfaceSetup(aIfc);
    StatusNotify(static_cast<UsbShai::TUsbPeripheralState>(KUsbAlternateSetting | setting), ifcset->iClientId);
    }


// aFunction gets called, successively, with the endpoint index of every ep in-use as its argument.
// (BTW: The declaration "type (class::*name)(params)" makes <name> a "pointer to element function".)
//
TInt DUsbClientController::DoForEveryEndpointInUse(TInt (DUsbClientController::*aFunction)(TInt), TInt& aCount)
    {
    __KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::DoForEveryEndpointInUse()"));
    aCount = 0;
    TUsbcConfiguration* const config = CurrentConfig();
    if (!config)
        {
        __KTRACE_OPT(KUSB, Kern::Printf("  Device is not configured - returning"));
        return KErrNone;
        }
    RPointerArray<TUsbcInterfaceSet>& ifcsets = config->iInterfaceSets;
    const TInt num_ifcsets = ifcsets.Count();
    for (TInt i = 0; i < num_ifcsets; i++)
        {
        RPointerArray<TUsbcLogicalEndpoint>& eps = ifcsets[i]->CurrentInterface()->iEndpoints;
        const TInt num_eps = eps.Count();
        for (TInt j = 0; j < num_eps; j++)
            {
            const TInt ep_num = EpAddr2Idx(eps[j]->iPEndpoint->iEndpointAddr);
            const TInt result = (this->*aFunction)(ep_num);
            ++aCount;
            if (result != KErrNone)
                {
                return result;
                }
            }
        }
    return KErrNone;
    }

// Data Tx is done.
void DUsbClientController::ProcessDataInPacket(TInt aCount,TInt aErrCode)
    {
    // Clear Error Code
    iLastError = KErrNone;
    
    // For tx, no premature end is allowed.
    if(aErrCode != KErrNone)
        {
        // something wrong in hardware, we can do nothing as remedy
        // just stall the endpoint.
        iConTransferMgr->StallEndpoint(KEp0_In);
        iConTransferMgr->SetupEndpointZeroRead();
        
        // set err code to Error general if end point is stalled
        iLastError = KErrGeneral;
        }
    else
        {
        // no longer a write pending
        iEp0WritePending = EFalse;
        
        // If it was a client who set up this transmission, we report to that client
        if (iEp0ClientDataTransmitting)
            {
            iEp0ClientDataTransmitting = EFalse;
            TUsbcRequestCallback* const p = iRequestCallbacks[KEp0_Tx];
            
            if (p)
                {
                __ASSERT_DEBUG((p->iTransferDir == UsbShai::EControllerWrite), Kern::Fault(KUsbPILPanicCat, __LINE__));
                p->iError = aErrCode;
                p->iTxBytes = aCount;
                
                // tell the client that the sending is done
                // later, it is possible that a status out packet comes in,
                // just ignore it.
                ProcessDataTransferDone(*p);
                }
            else
                {
                // we should never goes here
                iConTransferMgr->StallEndpoint(KEp0_In);
                
                // request not found
                iLastError = KErrNotFound;
                }
            }
        // else
        //   it is our own who sending the data, no more action needed
        }
    }

// Status Rx is done    
void DUsbClientController::ProcessStatusOutPacket(TInt aErrCode)
    {
    // Clear Error Code
    iLastError = KErrNone;
    
    // Dangdang, psl saying a status out packet recieved from
    // host, but, we already completed user's writting request, just ignore it.
    
    // any way, receiving this means no write is pending.
    iEp0WritePending = EFalse;
    }
    
// Data Rx is (partial) done
void DUsbClientController::ProcessDataOutPacket(TInt aCount,TInt aErrCode)
    {
    // Clear Error Code
    iLastError = KErrNone;
    
    if (aErrCode != KErrNone && aErrCode != KErrPrematureEnd)
        {
        // something wrong in hardware, we can do nothing as remedy
        // just stall the endpoint.
        iConTransferMgr->StallEndpoint(KEp0_Out);
        iConTransferMgr->SetupEndpointZeroRead();
        
        // set err code to Error general if end point is stalled
        iLastError = KErrGeneral;       
        }
    else
        {
        // Trim aCount with iEp0MaxPacketSize per packet
        if (aCount > iEp0MaxPacketSize)
            {
            aCount = iEp0MaxPacketSize;
            }
               
        iEp0DataReceived += aCount;
        
        if (iEp0ClientId == NULL)
            {
            // it is us( not an app), who owns this transaction
            switch( iConTransferMgr->PktParser().Request())
                {
#ifdef USB_SUPPORTS_SET_DESCRIPTOR_REQUEST                
                case KUsbRequest_SetDescriptor:
                    {
                    memcpy(iEp0_RxCollectionBuf + iEp0DataReceived, iEp0_RxBuf, aCount);                    
                    
                    // Status will be sent in side this function
                    // if we had recieved enough bytes
                    ProceedSetDescriptor();
                    }
                    break;
#endif                    
                default:
                    {
                    iConTransferMgr->StallEndpoint(KEp0_In);
                    ResetEp0DataOutVars();
                    
                    // set err code to Error general if end point is stalled
                    iLastError = KErrGeneral;
                    }
                    break;
                }
            
            if (iEp0DataReceived >= iConTransferMgr->PktParser().DataLength())
                {                
                // all data seems now to be here
                ResetEp0DataOutVars();
                }                
            }
        else
            {
            // it is an application who is requesting this data
            // pass the data on to a client
            
            // it is the client's responsibility of sending a status
            // packet back to host to indicate the whole transfer is
            // done
            
            // Find the client Request callback
            TSglQueIter<TUsbcRequestCallback> iter(iEp0ReadRequestCallbacks);
            TUsbcRequestCallback* p;
            while ((p = iter++) != NULL)
                {
                if (p->Owner() == iEp0ClientId)
                    {
                    memcpy(p->iBufferStart, iEp0_RxBuf, aCount);
                    p->iError = KErrNone;
                    *(p->iPacketSize) = aCount;
                    p->iRxPackets = 1;
                    *(p->iPacketIndex) = 0;
                    break;
                    }
                }
            
            // pass data to client if found one.
            if ( p != NULL)
                {
                ProcessDataTransferDone(*p);
                    
                if (iEp0DataReceived >= iConTransferMgr->PktParser().DataLength())
                    {
                    // all data seems now to be here
                    ResetEp0DataOutVars();
                    }
                
                iLastError = KErrNone;
                
                }
            else
                {
                // that's bad, we found a client is request this data
                // but no matching request callback found.
                
                iEp0_RxExtraCount = aCount;
                //iEp0_RxExtraData = ETrue;
                //iEp0_RxExtraError = aErrCode;
                iEp0DataReceived -= aCount;
                
                // No status packet will be send to host since no client is reading this data,
                // waiting client to send a status packet.
                
                iLastError = KErrNotFound;
                }
            }
        }
    }

// Status Tx is done    
void DUsbClientController::ProcessStatusInPacket(TInt aErrCode)
    {
    // Clear Error Code
    iLastError = KErrNone; 

    // it is time to start a new read
    iEp0WritePending = EFalse;
    }

//
// Setup Rx is done
//
void DUsbClientController::ProcessSetupPacket(TInt aCount,TInt aErrCode)
    {
    
    if (aErrCode != KErrNone)
        {
        // something wrong in hardware, we can do nothing as remedy
        // just stall the endpoint.
        iConTransferMgr->StallEndpoint(KEp0_Out);
        iConTransferMgr->SetupEndpointZeroRead();
        
        // set err code to Error general if end point is stalled
        iLastError = KErrGeneral;       
        }
    
    TUsbcSetup packet;
    Buffer2Setup(iEp0_RxBuf, packet);
    
    // Clear Error Code
    iLastError = KErrNone;
    
    // If this is a standard request, we can handle it here
    // not need to bother app layer
    if ((packet.iRequestType & KUsbRequestType_TypeMask) == KUsbRequestType_TypeStd)
        {
        // Fixme: this may not needed any more
        iEp0ReceivedNonStdRequest = EFalse;
        ProcessStandardRequest(aCount,packet);
        }
    else
        {
        // Fixme: This may not needed anymore
        iEp0ReceivedNonStdRequest = ETrue;
        ProcessNonStandardRequest(aCount,packet);
        }
    }

#define USB_PROCESS_REQUEST(request,param) \
    do \
    { \
    Process ## request(param); \
    if (iLastError != KErrNone) \
        { \
        __KTRACE_OPT(KUSB, \
                    Kern::Printf("  ProcessEp0SetupReceived: Stalling Ep0")); \
        iConTransferMgr->StallEndpoint(KEp0_In); \
        } \
    }while(0)
    
    
//
// Standard request
// Please note that:Macro USB_PROCESS_REQUEST(xxx) will stall endpoint
// if anything wrong during the process, in which case status packet is 
// not needed.
//
void DUsbClientController::ProcessStandardRequest(TInt /*aCount*/,const TUsbcSetup& aPacket)
    {
    switch (aPacket.iRequest)
        {
        case KUsbRequest_GetStatus:
            switch (aPacket.iRequestType & KUsbRequestType_DestMask)
                { // Recipient
                case KUsbRequestType_DestDevice:
                    USB_PROCESS_REQUEST(GetDeviceStatus,aPacket);
                    break;
                    
                case KUsbRequestType_DestIfc:
                    USB_PROCESS_REQUEST(GetInterfaceStatus,aPacket);
                    break;
                    
                case KUsbRequestType_DestEp:
                    USB_PROCESS_REQUEST(GetEndpointStatus,aPacket);
                    break;
                    
                default:
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: GET STATUS - Other or Unknown recipient"));
                    __KTRACE_OPT(KPANIC, Kern::Printf("  -> DUsbClientController::ProcessEp0SetupReceived: "
                                                      "Stalling Ep0"));
                    iConTransferMgr->StallEndpoint(KEp0_In);
                    iLastError = KErrGeneral;
                    break;
                }
                break;
        
        case KUsbRequest_ClearFeature:
        case KUsbRequest_SetFeature:
            switch (aPacket.iRequestType & KUsbRequestType_DestMask)
                { // Recipient
                case KUsbRequestType_DestDevice:
                    USB_PROCESS_REQUEST(SetClearDevFeature,aPacket);
                    break;
                case KUsbRequestType_DestIfc:
                    // will 100% stall endpoint
                    USB_PROCESS_REQUEST(SetClearIfcFeature,aPacket);
                    break;
                case KUsbRequestType_DestEp:
                    USB_PROCESS_REQUEST(SetClearEpFeature,aPacket);
                    break;
                default:
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: SET/CLEAR FEATURE - "
                                                      "Other or Unknown recipient"));
                    __KTRACE_OPT(KPANIC, Kern::Printf("  -> Stalling Ep0"));
                    iConTransferMgr->StallEndpoint(KEp0_In);
                    iLastError = KErrGeneral;
                    break;
                }
                break;
                
        case KUsbRequest_SetAddress:
            USB_PROCESS_REQUEST(SetAddress,aPacket);
            break;
            
        case KUsbRequest_GetDescriptor:
            USB_PROCESS_REQUEST(GetDescriptor,aPacket);
            break;
            
        case KUsbRequest_SetDescriptor:
            USB_PROCESS_REQUEST(SetDescriptor,aPacket);
            break;
            
        case KUsbRequest_GetConfig:
            USB_PROCESS_REQUEST(GetConfiguration,aPacket);
            break;
            
        case KUsbRequest_SetConfig:
            USB_PROCESS_REQUEST(SetConfiguration,aPacket);
            break;
            
        case KUsbRequest_GetInterface:
            USB_PROCESS_REQUEST(GetInterface,aPacket);
            break;
            
        case KUsbRequest_SetInterface:
            USB_PROCESS_REQUEST(SetInterface,aPacket);
            break;
            
        case KUsbRequest_SynchFrame:
            USB_PROCESS_REQUEST(SynchFrame,aPacket);
            break;
            
        default:
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Unknown/unsupported Std Setup Request"));
            __KTRACE_OPT(KPANIC, Kern::Printf("  -> Stalling Ep0"));
            iConTransferMgr->StallEndpoint(KEp0_In);
            iLastError = KErrGeneral;
            break;
        }       
    }

#undef USB_PROCESS_REQUEST
    
//
// class- or vendor-specific request    
// we dont send back any status to host in PIL for class- or vendor-specific request
// if no client is waiting, stall the endpoint
// if client is not ready, just record them and waiting for client to read them
// (and, send a status packet)
//
void DUsbClientController::ProcessNonStandardRequest(TInt aCount,const TUsbcSetup& aPacket)
    {
    // Find out which client can handle this request
    const DBase* client = FindNonStandardRequestClient(aPacket.iRequestType & KUsbRequestType_DestMask,aPacket);
        
    // If client is valide
    if (client != NULL)
        {
        // Try to relay aPacket to the real recipient
        TSglQueIter<TUsbcRequestCallback> iter(iEp0ReadRequestCallbacks);
        TUsbcRequestCallback* p;
        
        // Find out the request callback with match the client 
        // returned from last FindNonStandardRequestClient().
        while ((p = iter++) != NULL)
            {
            if (p->Owner() == client)
                {
                __ASSERT_DEBUG((p->iEndpointNum == 0), Kern::Fault(KUsbPILPanicCat, __LINE__));
                __ASSERT_DEBUG((p->iTransferDir == UsbShai::EControllerRead), Kern::Fault(KUsbPILPanicCat, __LINE__));
                __KTRACE_OPT(KUSB, Kern::Printf("  Found Ep0 read request"));
                if (aPacket.iLength != 0)
                    {
                    if ((aPacket.iRequestType & KUsbRequestType_DirMask) == KUsbRequestType_DirToDev)
                        {
                        // Data transfer & direction OUT => there'll be a DATA_OUT stage
                        __KTRACE_OPT(KUSB, Kern::Printf("  Next is DATA_OUT: setting up DataOutVars"));
                        SetEp0DataOutVars(client);
                        }
                    else if ((aPacket.iRequestType & KUsbRequestType_DirMask) == KUsbRequestType_DirToHost)
                        {
                        // For possible later use (ZLP).
                        iEp0_TxNonStdCount = aPacket.iLength;
                        }
                    }
                    
                // Found the request callback, jump out now
                break;                
                }
            }
            
        // if a request callback matching the client is found,
        // complete the request
        if( p != NULL)
            {
            __KTRACE_OPT(KUSB, Kern::Printf("  Ep0 read request completed to client"));
            memcpy(p->iBufferStart, iEp0_RxBuf, aCount);
            p->iError = KErrNone;
            *(p->iPacketSize) = aCount;
            p->iRxPackets = 1;
            *(p->iPacketIndex) = 0;
            ProcessDataTransferDone(*p);            
            }
        else
            {            
            __KTRACE_OPT(KUSB, Kern::Printf("  Ep0 read request not found: setting RxExtra vars (Setup)"));
            iEp0_RxExtraCount = aCount;
            //iEp0_RxExtraData = ETrue;
            //iEp0_RxExtraError = aErrCode;
            iSetupPacketPending = ETrue;
            
            // For setup packet,a zero bytes status is always needed
            iLastError = KErrNotFound;           
            }
        }
    else // if (client == NULL)
        {
        // Pil don't know how to deal with non-standard request, stall endpoint
        __KTRACE_OPT(KPANIC, Kern::Printf("  Ep0 request error: Stalling Ep0"));
        iConTransferMgr->StallEndpoint(KEp0_In);
        iLastError = KErrGeneral;
        }
    }
    
const DBase* DUsbClientController::FindNonStandardRequestClient(TUint8 aPacketTypeDestination,const TUsbcSetup& aPacket)
    {
    const DBase* client = NULL;
    
    switch (aPacketTypeDestination)
        { // Recipient
        case KUsbRequestType_DestDevice:
            {
            client = iEp0DeviceControl;
            }
            break;
            
        case KUsbRequestType_DestIfc:
            {
            //Add this mutex to protect the interface set data structure
            if (NKern::CurrentContext() == EThread)
                {
                NKern::FMWait(&iMutex);
                }
                
            if ( iDeviceState < UsbShai::EUsbPeripheralStateConfigured)
                {
                __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
                }
            else
                {
                const TUsbcInterfaceSet* const ifcset_ptr =
                    InterfaceNumber2InterfacePointer(aPacket.iIndex);
                    
                //In some rare case, ifcset_ptr is not NULL but the ifcset_ptr->iInterfaces.Count() is 0,
                //so panic will happen when excute the following line. so I add the conditon
                //0 != ifcset_ptr->iInterfaces.Count() here.
                if (ifcset_ptr && 0 != ifcset_ptr->iInterfaces.Count())
                    {
                    if (ifcset_ptr->CurrentInterface()->iNoEp0Requests)
                        {
                        __KTRACE_OPT(KUSB, Kern::Printf("  Recipient says: NoEp0RequestsPlease"));
                        }
                    else
                        {
                        client = ifcset_ptr->iClientId;
                        }
                    }
                else
                    {
                    __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Interface 0x%02x does not exist",
                                                      aPacket.iIndex));
                    }
                }
                
            if (NKern::CurrentContext() == EThread)
                {
                NKern::FMSignal(&iMutex);
                }
            }
            break;
                        
        case KUsbRequestType_DestEp:
            {
            //Add this mutex to protect the interface set data structure
            if (NKern::CurrentContext() == EThread)
                {
                NKern::FMWait(&iMutex);
                }
            if ( iDeviceState < UsbShai::EUsbPeripheralStateConfigured)
                {
                __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Invalid device state"));
                }
            else if (EndpointExists(aPacket.iIndex) == EFalse)
                {
                __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Endpoint 0x%02x does not exist",
                                                  aPacket.iIndex));
                }
            else
                {
                const TInt idx = EpAddr2Idx(aPacket.iIndex);
                const TUsbcInterfaceSet* const ifcset_ptr =
                    iRealEndpoints[idx].iLEndpoint->iInterface->iInterfaceSet;
                if (ifcset_ptr->CurrentInterface()->iNoEp0Requests)
                    {
                    __KTRACE_OPT(KUSB, Kern::Printf("  Recipient says: NoEp0RequestsPlease"));
                    }
                else
                    {
                    client = ifcset_ptr->iClientId;
                    }
                }
            if (NKern::CurrentContext() == EThread)
                {
                NKern::FMSignal(&iMutex);
                }
            }
            break;
            
        default:
            {
            __KTRACE_OPT(KPANIC, Kern::Printf("  Error: Other or Unknown recipient"));
            break;
            }
        }
        
    return client;
    }

TInt DUsbClientController::ProcessSetupEndpointZeroRead()
    {
    __KTRACE_OPT(KPANIC, Kern::Printf("DUsbClientController:: Read EP0 Issued"));
    return iController.SetupEndpointZeroRead();
    }
    
TInt DUsbClientController::ProcessSetupEndpointZeroWrite(const TUint8* aBuffer, TInt aLength, TBool aZlpReqd)
    {
    __KTRACE_OPT(KPANIC, Kern::Printf("DUsbClientController:: Write EP0 Issued"));
    return iController.SetupEndpointZeroWrite(aBuffer,aLength,aZlpReqd);
    }
    
TInt DUsbClientController::ProcessSendEp0ZeroByteStatusPacket()
    {
    __KTRACE_OPT(KPANIC, Kern::Printf("DUsbClientController:: Zero Status to EP0 Issued"));
    return iController.SendEp0ZeroByteStatusPacket();
    }
    
TInt DUsbClientController::ProcessStallEndpoint(TInt aRealEndpoint)
    {
    __KTRACE_OPT(KPANIC, Kern::Printf("DUsbClientController:: EP0(%d) Stall Issued",aRealEndpoint));
    return iController.StallEndpoint(aRealEndpoint);
    }

void DUsbClientController::ProcessEp0SetupPacketProceed()
    {
    __KTRACE_OPT(KPANIC, Kern::Printf("DUsbClientController:: Missed setup packet procced"));
    iController.Ep0ReadSetupPktProceed();
    }
    
void DUsbClientController::ProcessEp0DataPacketProceed()
    {
    __KTRACE_OPT(KPANIC, Kern::Printf("DUsbClientController:: Missed data packet procced"));
    iController.Ep0ReadDataPktProceed();
    }
    
// -eof-
