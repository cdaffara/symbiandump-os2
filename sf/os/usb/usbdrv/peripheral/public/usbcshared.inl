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
// e32\include\drivers\usbcshared.inl
// Kernel side definitions for the USB Device driver stack (PIL + LDD).
// 
//

/**
 @file usbcshared.inl
 @internalTechnology
*/

#ifndef __USBCSHARED_INL__
#define __USBCSHARED_INL__

//
// --- DUsbClientController (USB PDD) ---
//

// --- Private member functions, used by controller itself ---

const DBase* DUsbClientController::PEndpoint2ClientId(TInt aRealEndpoint) const
	{
	if (iRealEndpoints[aRealEndpoint].iLEndpoint)
		return iRealEndpoints[aRealEndpoint].iLEndpoint->iInterface->iInterfaceSet->iClientId;
	else
		return NULL;
	}


TInt DUsbClientController::PEndpoint2LEndpoint(TInt aRealEndpoint) const
	{
	if (iRealEndpoints[aRealEndpoint].iLEndpoint)
		return iRealEndpoints[aRealEndpoint].iLEndpoint->iLEndpointNum;
	else
		return -1;
	}


const TUsbcConfiguration* DUsbClientController::CurrentConfig() const
	{
	return (iCurrentConfig ? iConfigs[iCurrentConfig - 1] : NULL);
	}


TUsbcConfiguration* DUsbClientController::CurrentConfig()
	{
	return (iCurrentConfig ? iConfigs[iCurrentConfig - 1] : NULL);
	}


TBool DUsbClientController::InterfaceExists(TInt aNumber) const
	{
	const TInt num_ifcsets = iConfigs[0]->iInterfaceSets.Count();
	RPointerArray<TUsbcInterfaceSet>& ifcsets = iConfigs[0]->iInterfaceSets;
	for (TInt i = 0; i < num_ifcsets; i++)
		{
		if (ifcsets[i]->iInterfaceNumber == aNumber)
			{
			return ETrue;
			}
		}
	return EFalse;
	}


TBool DUsbClientController::EndpointExists(TUint aAddress) const
	{
	// Ep0 doesn't have a "logical ep" pointer (there's no virtual endpoint zero);
	// that's why this pointer being non-NULL is not a sufficient criterion for
	// endpoint-existence. (Apart from that, ep0 always exists.)
	const TInt idx = EpAddr2Idx(aAddress);
	return ((idx < iDeviceTotalEndpoints) &&
			((iRealEndpoints[idx].iLEndpoint != NULL) ||
			 ((aAddress & KUsbEpAddress_Portmask) == 0)));
	}

TUint DUsbClientController::EpIdx2Addr(TUint aRealEndpoint) const
	{
	return ((aRealEndpoint << 7) & 0x80) | ((aRealEndpoint >> 1) & 0x0f);
	}


TUint DUsbClientController::EpAddr2Idx(TUint aAddress) const
	{
	return ((aAddress & 0x80) >> 7) | ((aAddress & 0x0f) << 1);
	}


void DUsbClientController::SetEp0DataOutVars(const DBase* aClientId)
	{
	__KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::SetEp0DataOutVars()"));
	//iSetup = aPacket;
	iEp0DataReceived = 0;
	iEp0ClientId = aClientId;
	}


void DUsbClientController::ResetEp0DataOutVars()
	{
	__KTRACE_OPT(KUSB, Kern::Printf("DUsbClientController::ResetEp0DataOutVars()"));
	iEp0DataReceived = 0;
	iEp0ClientId = NULL;
	}


TBool DUsbClientController::IsInTheRequestList(const TUsbcRequestCallback& aCallback)
	{
    	const TInt irq = __SPIN_LOCK_IRQSAVE(iUsbLock);
	TSglQueIter<TUsbcRequestCallback> iter(iEp0ReadRequestCallbacks);
	TUsbcRequestCallback* p;
	while ((p = iter++) != NULL)
		{
		if (p == &aCallback)
			{
			__SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
			return ETrue;
			}
		}
    	__SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
	return EFalse;
	}


TBool DUsbClientController::IsInTheStatusList(const TUsbcStatusCallback& aCallback)
	{
    	const TInt irq = __SPIN_LOCK_IRQSAVE(iUsbLock);
	TSglQueIter<TUsbcStatusCallback> iter(iStatusCallbacks);
	TUsbcStatusCallback* p;
	while ((p = iter++) != NULL)
		{
		if (p == &aCallback)
			{
            		__SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
			return ETrue;
			}
		}
    	__SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
	return EFalse;
	}


TBool DUsbClientController::IsInTheEpStatusList(const TUsbcEndpointStatusCallback& aCallback)
	{
    	const TInt irq = __SPIN_LOCK_IRQSAVE(iUsbLock);
	TSglQueIter<TUsbcEndpointStatusCallback> iter(iEpStatusCallbacks);
	TUsbcEndpointStatusCallback* p;
	while ((p = iter++) != NULL)
		{
		if (p == &aCallback)
			{
		    	__SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
			return ETrue;
			}
		}
    	__SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
	return EFalse;
	}


TBool DUsbClientController::IsInTheOtgFeatureList(const TUsbcOtgFeatureCallback& aCallback)
	{
    	const TInt irq = __SPIN_LOCK_IRQSAVE(iUsbLock);
	TSglQueIter<TUsbcOtgFeatureCallback> iter(iOtgCallbacks);
	TUsbcOtgFeatureCallback* p;
	while ((p = iter++) != NULL)
		{
		if (p == &aCallback)
			{
		    	__SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
			return ETrue;
			}
		}
    	__SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
	return EFalse;
	}

TBool DUsbClientController::IsInTheChargerTypeList(const TUsbcChargerTypeCallback& aCallback)
    {
    const TInt irq = __SPIN_LOCK_IRQSAVE(iUsbLock);
    TSglQueIter<TUsbcChargerTypeCallback> iter(iChargerTypeCallbacks);
    TUsbcChargerTypeCallback* p;
    while ((p = iter++) != NULL)
        {
        if (p == &aCallback)
            {
            __SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
            return ETrue;
            }
        }
    __SPIN_UNLOCK_IRQRESTORE(iUsbLock, irq);
    return EFalse;    
    }

//
// --- Misc classes ---
//

// --- TUsbcClientCallback

/** Constructor.
 */
TUsbcClientCallback::TUsbcClientCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority)
	: iOwner(aOwner),
	  iDfc(aCallback, aOwner, aPriority)
	{}


/** Returns a pointer to the owner of this request.

	@return A pointer to the owner of this request.
*/
DBase* TUsbcClientCallback::Owner() const
	{
	return iOwner;
	}


/** Executes the callback function set by the owner of this request.

	@return KErrNone.
*/
TInt TUsbcClientCallback::DoCallback()
	{
	__ASSERT_DEBUG((NKern::CurrentContext() == EThread), Kern::Fault(KUsbPILPanicCat, __LINE__));
	iDfc.Enque();
	return KErrNone;
	}


/** Cancels the callback function set by the owner of this request.
 */
void TUsbcClientCallback::Cancel()
	{
	iDfc.Cancel();
	}


/** Sets the DFC queue used by the callback function.
	@param aDfcQ DFC queue to be set
 */
void TUsbcClientCallback::SetDfcQ(TDfcQue* aDfcQ)
	{
	iDfc.SetDfcQ(aDfcQ);
	}


// --- TUsbcEndpointStatusCallback

/** Constructor.
 */
TUsbcEndpointStatusCallback::TUsbcEndpointStatusCallback(DBase* aOwner, TDfcFn aCallback,
														 TInt aPriority)
	: iOwner(aOwner),
	  iDfc(aCallback, aOwner, aPriority),
	  iState(0)
	{}


/** Sets the state of this request to aState.

	@param aState The new state to be set.
*/
void TUsbcEndpointStatusCallback::SetState(TUint aState)
	{
	iState = aState;
	}


/** Returns the state value of this request.

	@return The state value of this request.
*/
TUint TUsbcEndpointStatusCallback::State() const
	{
	return iState;
	}


/** Returns a pointer to the owner of this request.

	@return A pointer to the owner of this request.
*/
DBase* TUsbcEndpointStatusCallback::Owner() const
	{
	return iOwner;
	}


/** Executes the callback function set by the owner of this request.

	@return KErrNone.
*/
TInt TUsbcEndpointStatusCallback::DoCallback()
	{
	if (NKern::CurrentContext() == EThread)
		iDfc.Enque();
	else
		iDfc.Add();
	return KErrNone;
	}


/** Cancels the callback function set by the owner of this request.
*/
void TUsbcEndpointStatusCallback::Cancel()
	{
	iDfc.Cancel();
	}


/** Sets the DFC queue used by the callback function.
*/
void TUsbcEndpointStatusCallback::SetDfcQ(TDfcQue* aDfcQ)
	{
	iDfc.SetDfcQ(aDfcQ);
	}


// --- TUsbcStatusCallback

/** Constructor.
 */
TUsbcStatusCallback::TUsbcStatusCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority)
	: iOwner(aOwner),
	  iDfc(aCallback, aOwner, aPriority)
	{
 	ResetState();
	}


/** Sets the state of this request to aState (at the first available position
	in the state value array).

	@param aState The new state to be set.
*/
void TUsbcStatusCallback::SetState(UsbShai::TUsbPeripheralState aState)
	{
	for (TInt i = 0; i < KUsbcDeviceStateRequests; i++)
		{
		if (iState[i] == UsbShai::EUsbPeripheralNoState)
			{
			iState[i] = aState;
			return;
			}
		}
	__KTRACE_OPT(KPANIC, Kern::Printf("  Error: KUsbcDeviceStateRequests too small (%d)!",
									  KUsbcDeviceStateRequests));
	}


/** Returns the state value of this request at a certain index.

	@param aIndex The index to be used for referencing the state array.

	@return The state value of this request at aIndex.
*/
UsbShai::TUsbPeripheralState TUsbcStatusCallback::State(TInt aIndex) const
	{
	if (aIndex >= 0 && aIndex < KUsbcDeviceStateRequests)
		{
		return iState[aIndex];
		}
	else
		{
		__KTRACE_OPT(KPANIC, Kern::Printf("  Error: aIndex too large (%d)!", aIndex));
		return UsbShai::EUsbPeripheralNoState;
		}
	}


/** Resets the entire state value array of this request.
*/
void TUsbcStatusCallback::ResetState()
	{
	for (TInt i = 0; i < KUsbcDeviceStateRequests; ++i)
		{
		iState[i] = UsbShai::EUsbPeripheralNoState;
		}
	}


/** Returns a pointer to the owner of this request.

	@return A pointer to the owner of this request.
*/
DBase* TUsbcStatusCallback::Owner() const
	{
	return iOwner;
	}


/** Executes the callback function set by the owner of this request.

	@return KErrNone.
*/
TInt TUsbcStatusCallback::DoCallback()
	{
	if (NKern::CurrentContext() == EThread)
		iDfc.Enque();
	else
		iDfc.Add();
	return KErrNone;
	}


/** Cancels the callback function set by the owner of this request.
*/
void TUsbcStatusCallback::Cancel()
	{
	iDfc.Cancel();
	}


/** Sets the DFC queue used by the callback function.
*/
void TUsbcStatusCallback::SetDfcQ(TDfcQue* aDfcQ)
	{
	iDfc.SetDfcQ(aDfcQ);
	}

// --- TUsbcRequestCallback

/** Constructor.
 */
TUsbcRequestCallback::TUsbcRequestCallback(const DBase* aOwner, TInt aEndpointNum, TDfcFn aDfcFunc,
										   TAny* aEndpoint, TDfcQue* aDfcQ, TInt aPriority)
	: TUsbPeripheralRequest(-1),
      iEndpointNum(aEndpointNum),
	  iOwner(aOwner),
	  iDfc(aDfcFunc, aEndpoint, aDfcQ, aPriority)
	{
	}


/** Destructor.
 */
TUsbcRequestCallback::~TUsbcRequestCallback()
	{
	__KTRACE_OPT(KUSB, Kern::Printf("TUsbcRequestCallback::~TUsbcRequestCallback()"));
	iDfc.Cancel();
	}

/** Sets the transfer direction for this request.

	@param aTransferDir The new transfer direction.
*/
void TUsbcRequestCallback::SetTransferDirection(UsbShai::TTransferDirection aTransferDir)
	{
	iTransferDir = aTransferDir;
	}


/** Returns a pointer to the owner of this request.

	@return A pointer to the owner of this request.
*/
const DBase* TUsbcRequestCallback::Owner() const
	{
	return iOwner;
	}


/** Executes the callback function set by the owner of this request.

	@return KErrNone.
*/
TInt TUsbcRequestCallback::DoCallback()
	{
	if (NKern::CurrentContext() == NKern::EThread)
		iDfc.Enque();
	else
		iDfc.Add();
	return KErrNone;
	}


/** Cancels the callback function set by the owner of this request.
*/
void TUsbcRequestCallback::Cancel()
	{
	iDfc.Cancel();
	}

// --- TUsbcOtgFeatureCallback

/** Constructor.
 */
TUsbcOtgFeatureCallback::TUsbcOtgFeatureCallback(DBase* aOwner, TDfcFn aCallback,
												 TInt aPriority)
	: iOwner(aOwner),
	  iDfc(aCallback, aOwner, aPriority),
	  iValue(0)
	{}


/** Returns a pointer to the owner of this request.
	@return A pointer to the owner of this request.
*/
DBase* TUsbcOtgFeatureCallback::Owner() const
	{
	return iOwner;
	}


/** Set feature value which is to be notified to client.
	@param OTG feature value to be set
*/
void TUsbcOtgFeatureCallback::SetFeatures(TUint8 aFeatures)
	{
	iValue = aFeatures;
	}


/** Set feature value which is to be notified to client.
	@return Value of OTG features
*/
TUint8 TUsbcOtgFeatureCallback::Features() const
	{
	return iValue;
	}


/** Set DFC queue.
	@param aDfcQ  DFC queue to be set
*/ 
void TUsbcOtgFeatureCallback::SetDfcQ(TDfcQue* aDfcQ)
	{
	iDfc.SetDfcQ(aDfcQ);
	}


/** Executes the callback function set by the owner of this request.
	@return KErrNone.
*/
TInt TUsbcOtgFeatureCallback::DoCallback()
	{
	if (NKern::CurrentContext() == EThread)
		iDfc.Enque();
	else
		iDfc.Add();
	return KErrNone;
	}


/** Cancels the callback function set by the owner of this request.
 */
void TUsbcOtgFeatureCallback::Cancel()
	{
	iDfc.Cancel();
	}


// --- TUsbcChargerTypeCallback

/** Constructor.
 */
TUsbcChargerTypeCallback::TUsbcChargerTypeCallback(DBase* aOwner, TDfcFn aCallback,
                                                 TInt aPriority)
    : iOwner(aOwner),
      iDfc(aCallback, aOwner, aPriority),
      iChargerType(UsbShai::EPortTypeNone),
      iPendingNotify(EFalse)
    {}

/** Set charger type which is to be notified to client.
    @param Charger type to be set
*/
void TUsbcChargerTypeCallback::SetChargerType(TUint aType)
    {
    iChargerType = aType;    
    }


/** Returns a pointer to the owner of this request.
    @return A pointer to the owner of this request.
*/
DBase* TUsbcChargerTypeCallback::Owner() const
    {
    return iOwner;
    }


/** Charger type which is to be notified to client.
    @return Value of Charger Type
*/
TUint TUsbcChargerTypeCallback::ChargerType() const
    {
    return iChargerType;
    }


/** Set whether there is an pending charger type which is to be notified to client.
    @param whether there is an pending charger type
*/
void TUsbcChargerTypeCallback::SetPendingNotify(TBool aPendingNotify) 
    {
    iPendingNotify = aPendingNotify;
    }

/** return whether there is an pending charger type which is to be notified to client.
    @return whether there is an pending charger type
*/
TBool TUsbcChargerTypeCallback::PendingNotify() const
    {
    return iPendingNotify;
    }

/** Executes the callback function set by the owner of this request.
    @return KErrNone.
*/
TInt TUsbcChargerTypeCallback::DoCallback()
    {
    if (NKern::CurrentContext() == NKern::EThread)
        iDfc.Enque();
    else
        iDfc.Add();
    return KErrNone;
    }


/** Cancels the callback function set by the owner of this request.
 */
void TUsbcChargerTypeCallback::Cancel()
    {
    iDfc.Cancel();
    }


/** Set DFC queue.
    @param aDfcQ  DFC queue to be set
*/
void TUsbcChargerTypeCallback::SetDfcQ(TDfcQue* aDfcQ)
    {
    iDfc.SetDfcQ(aDfcQ);
    }


/** Returns a pointer to the currently selected (active) setting of this interface.

	@return A pointer to the currently selected (active) setting of this interface.
*/
const TUsbcInterface* TUsbcInterfaceSet::CurrentInterface() const
	{
	return iInterfaces[iCurrentInterface];
	}


/** Returns a pointer to the currently selected (active) setting of this interface.

	@return A pointer to the currently selected (active) setting of this interface.
*/

TUsbcInterface* TUsbcInterfaceSet::CurrentInterface()
	{
	return iInterfaces[iCurrentInterface];
	}

#endif // __USBCSHARED_INL__



