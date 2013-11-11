// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// e32\drivers\usbc\d_usbc.cpp
// LDD for USB Device driver stack:
// The channel object.
// 
//

/**
 @file d_usbc.cpp
 @internalTechnology
*/

#include <usbc.h>
#include <e32utrace.h>
#include "te_setfilterparameters.h"
#include "te_utracecmds.h"


_LIT(KUsbLddName, "Usbc");

static const TInt KUsbRequestCallbackPriority = 2;


// Quick sanity check on endpoint properties
static TBool ValidateEndpoint(const TUsbcEndpointInfo* aEndpointInfo)
	{
	const TUint dir = aEndpointInfo->iDir;
	const TInt size = aEndpointInfo->iSize;
	if (size <= 0)
		return EFalse;

	switch (aEndpointInfo->iType)
		{
	case KUsbEpTypeControl:
		if (dir != KUsbEpDirBidirect || size > 64)
			return EFalse;
		break;
	case KUsbEpTypeIsochronous:
		if ((dir != KUsbEpDirIn && dir != KUsbEpDirOut) || size > 1024)
			return EFalse;
		break;
	case KUsbEpTypeBulk:
		if ((dir != KUsbEpDirIn && dir != KUsbEpDirOut) || size > 512)
			return EFalse;
		break;
	case KUsbEpTypeInterrupt:
		if ((dir != KUsbEpDirIn && dir != KUsbEpDirOut) || size > 1024)
			return EFalse;
		break;
	default:
		return EFalse;
		}
	return ETrue;
	}


/** Real entry point from the Kernel: return a new driver.
 */
DECLARE_STANDARD_LDD()
	{
	return new DUsbcLogDevice;
	}


/** Create a channel on the device.

	@internalComponent
*/
TInt DUsbcLogDevice::Create(DLogicalChannelBase*& aChannel)
	{

	aChannel = new DLddUsbcChannel;
	return aChannel ? KErrNone : KErrNoMemory;
	}


DUsbcLogDevice::DUsbcLogDevice()
      {
     
	  iParseMask = KDeviceAllowUnit;
	  iUnitsMask = 0xffffffff;								// Leave units decision to the Controller
      iVersion = TVersion(KUsbcMajorVersion, KUsbcMinorVersion, KUsbcBuildVersion);
      }


TInt DUsbcLogDevice::Install()
	{

	// Only proceed if we have the Controller underneath us
	if (!DUsbClientController::UsbcControllerPointer())
		{
		//__KTRACE_OPT(KPANIC, Kern::Printf("LDD Install: USB Controller Not Present"));
		return KErrGeneral;
		}
	return SetName(&KUsbLddName);
	}


//
// Return the USB controller capabilities.
//
void DUsbcLogDevice::GetCaps(TDes8& aDes) const
	{
	TPckgBuf<TCapsDevUsbc> b;
	b().version = iVersion;
	Kern::InfoCopy(aDes, b);
	}


//
// Constructor
//
DLddUsbcChannel::DLddUsbcChannel()
	: iValidInterface(EFalse),
	  iAlternateSettingList(NULL),
	  iCompleteAllCallbackInfo(this, DLddUsbcChannel::EmergencyCompleteDfc, KUsbRequestCallbackPriority),
	  iStatusChangePtr(NULL),
	  iStatusCallbackInfo(this, DLddUsbcChannel::StatusChangeCallback, KUsbRequestCallbackPriority),
	  iEndpointStatusChangePtr(NULL),
	  iEndpointStatusCallbackInfo(this, DLddUsbcChannel::EndpointStatusChangeCallback,
								  KUsbRequestCallbackPriority),
      iOtgFeatureChangePtr(NULL),
      iOtgFeatureCallbackInfo(this, DLddUsbcChannel::OtgFeatureChangeCallback, KUsbRequestCallbackPriority),
	  iBufferBaseEp0(NULL),
	  iBufferSizeEp0(0),
	  iNumberOfEndpoints(0),
	  iHwChunkIN(NULL),
	  iHwChunkOUT(NULL),
	  iHwChunkEp0(NULL),
	  iDeviceState(EUsbcDeviceStateUndefined),
	  iOwnsDeviceControl(EFalse),
	  iAlternateSetting(0),
	  iDeviceStatusNeeded(EFalse),
	  iChannelClosing(EFalse)
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("*** DLddUsbcChannel::DLddUsbcChannel CTOR"));
	iClient = &Kern::CurrentThread();
	iClient->Open();
	for (TInt i = 1; i <= KMaxEndpointsPerClient; i++)
		{
		iEndpoint[i] = NULL;
		}
	for (TInt i = 1; i < KUsbcMaxRequests; i++)
		{
		iRequestStatus[i] = NULL;
		}
	}


DLddUsbcChannel::~DLddUsbcChannel()
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("DLddUsbcChannel::~DLddUsbcChannel()"));
	if (iController)
		{
		iStatusCallbackInfo.Cancel();
		iEndpointStatusCallbackInfo.Cancel();
        iOtgFeatureCallbackInfo.Cancel();
        iCompleteAllCallbackInfo.Cancel();
		AbortInterface();
		DestroyAllInterfaces();
		if (iOwnsDeviceControl)
			{
			iController->ReleaseDeviceControl(this);
			iOwnsDeviceControl = EFalse;
			}
		iController->DeRegisterClient(this);
		DestroyEp0();
		delete iStatusFifo;
		}
	Kern::SafeClose((DObject*&)iClient, NULL);
	}


inline TBool DLddUsbcChannel::ValidEndpoint(TInt aEndpoint)
	{
	return (aEndpoint <= iNumberOfEndpoints && aEndpoint >= 0);
	}


//
// Create channel
//
TInt DLddUsbcChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& aVer)
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("LDD DoCreateL 1 Ver = %02d %02d %02d",
								//	aVer.iMajor, aVer.iMinor, aVer.iBuild));
	if (!Kern::CurrentThreadHasCapability(ECapabilityCommDD,
										  __PLATSEC_DIAGNOSTIC_STRING("Checked by USBC.LDD (USB Driver)")))
		{
		return KErrPermissionDenied;
		}

	iController = DUsbClientController::UsbcControllerPointer();

	if (!iController)
		{
		return KErrGeneral;
		}

    if (!iController->IsActive())
        return KErrDisconnected;

	iStatusFifo = new TUsbcDeviceStatusQueue;
	if (iStatusFifo == NULL)
		{
		return KErrNoMemory;
		}

  	if (!Kern::QueryVersionSupported(TVersion(KUsbcMajorVersion, KUsbcMinorVersion, KUsbcBuildVersion), aVer))
		{
		return KErrNotSupported;
		}

	// set up the correct DFC queue
	SetDfcQ(iController->DfcQ(0));							// sets the channel's dfc queue
    iCompleteAllCallbackInfo.SetDfcQ(iDfcQ);
    iController->RegisterClientCallback(iCompleteAllCallbackInfo);
	iStatusCallbackInfo.SetDfcQ(iDfcQ);						// use the channel's dfcq for this dfc
	iEndpointStatusCallbackInfo.SetDfcQ(iDfcQ);				// use the channel's dfcq for this dfc
    iOtgFeatureCallbackInfo.SetDfcQ(iDfcQ);
	iMsgQ.Receive();										//start up the message q
	iController->RegisterForStatusChange(iStatusCallbackInfo);	// let the ldd start seeing device state changes
	return KErrNone;
	}


void DLddUsbcChannel::HandleMsg(TMessageBase* aMsg)
	{
	TThreadMessage& m = *(TThreadMessage*)aMsg;
	TInt id = m.iValue;
	if (id == (TInt) ECloseMsg)
		{
		iChannelClosing = ETrue;
		m.Complete(KErrNone, EFalse);
		return;
		}
	else if (id == KMaxTInt)
		{
		// Cancel request
		TInt mask = m.Int0();
		TInt b = 1;
		for(TInt reqNo = 0; reqNo <= KUsbcMaxRequests; reqNo++)
			{
			TRequestStatus* pS = iRequestStatus[reqNo];
			if ((mask & b) && (pS != NULL))
				{
				TInt r = DoCancel(reqNo);
				if (r == KErrNone)
					r = KErrCancel;
				Kern::RequestComplete(iClient,iRequestStatus[reqNo], r);
				}
			b <<= 1;
			}
		m.Complete(KErrNone, ETrue);
		return;
		}

	if (!iController->IsActive())
		{
        if (id != RDevUsbcClient::EControlEnableUsbDriver)
		    {
            //__KTRACE_OPT(KUSB, Kern::Printf("DLddUsbcChannel::HandleMsg - Controller is not active"));
            m.Complete(KErrDisconnected, ETrue);
		    return;
		    }
		}
	if (id < 0)
		{
		// DoRequestL
		TRequestStatus* pS = (TRequestStatus*) m.Ptr0();
		DoRequestL(~id, pS, m.Ptr1(), m.Ptr2());
		m.Complete(KErrNone, ETrue);
		}
	else
		{
		// DoControl
		TInt r = DoControl(id, m.Ptr0(), m.Ptr1());
		m.Complete(r, ETrue);
		}
	}


//
// Overriding DObject virtual
//
TInt DLddUsbcChannel::RequestUserHandle(DThread* aThread, TOwnerType /*aType*/)
	{
	__KTRACE_OPT(KUSB, Kern::Printf("DLddUsbcChannel::RequestUserHandle"));
	// The USB client LDD is not designed for a channel to be shared between
	// threads. It saves a pointer to the current thread when it is opened, and
	// uses this to complete any asynchronous requests.
	// It is therefore not acceptable for the handle to be duplicated and used
	// by another thread:
	if (aThread == iClient)
		{
		return KErrNone;
		}
	else
		{
		return KErrAccessDenied;
		}
	}
// Asynchronous requests - overriding pure virtual
//
void DLddUsbcChannel::DoRequestL(TInt aReqNo, TRequestStatus* aStatus, TAny* a1, TAny* a2)
	{
	// Check on request status
	//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL 0x%08x", aReqNo));
	if (aReqNo <= KUsbcMaxRequests)
		{
		TInt r = KErrNone;
		if (iRequestStatus[aReqNo] != NULL)
			{
			DestroyAllInterfaces();
			PanicClientThread(ERequestAlreadyPending);
			}
		else
			{
			TBool needsCompletion;
			iRequestStatus[aReqNo] = aStatus;

			if (aReqNo > KUsbcMaxEpNumber)
				{
				r = DoOtherAsyncReq(aReqNo, a1, a2, needsCompletion);
				}
			else
				{
				r = DoTransferAsyncReq(aReqNo, a1, a2, needsCompletion);
				}

			if (needsCompletion)
				{
				Kern::RequestComplete(iClient, iRequestStatus[aReqNo], r);
				}
			}
		}
	else
		{
		Kern::RequestComplete(iClient, aStatus, KErrNotSupported);
		}
	}


TInt DLddUsbcChannel::DoOtherAsyncReq(TInt aReqNo, TAny* a1, TAny* a2, TBool& aNeedsCompletion)
	{
	// The general assumption is that none of these will complete now.
	// However, those that make this function return something other than
	// KErrNone will get completed by the calling function.
	// So, 1) If you are returning KErrNone but really need to complete because
	//        completion criteria can be met (for example, sufficient data is
	//        available in the buffer) and then set aNeedsCompletion = ETrue.
	//     2) Do NOT complete here AT ALL.
	//
	aNeedsCompletion = EFalse;
	TInt r = KErrNone;

	switch (aReqNo)
		{
	case RDevUsbcClient::ERequestAlternateDeviceStatusNotify:
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlReqDeviceStatusNotify"));
		if (a1 != NULL)
			{
			iDeviceStatusNeeded = ETrue;
			iStatusChangePtr = a1;
			aNeedsCompletion = AlternateDeviceStateTestComplete();
			}
		else
			r = KErrArgument;
		break;
		}
	case RDevUsbcClient::ERequestReEnumerate:
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("ERequestReEnumerate"));
		// If successful, this will complete via the status notification.
		r = iController->ReEnumerate();
		break;
		}
	case RDevUsbcClient::ERequestEndpointStatusNotify:
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("ERequestEndpointStatusNotify"));
		if (a1 != NULL)
			{
			iEndpointStatusChangePtr = a1;
            iController->RegisterForEndpointStatusChange(iEndpointStatusCallbackInfo);
			}
		else
			r = KErrArgument;
		break;
			}
	case RDevUsbcClient::ERequestOtgFeaturesNotify:
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("ERequestOtgFeaturesNotify"));
		if (a1 != NULL)
			{
            iOtgFeatureChangePtr = a1;
            iController->RegisterForOtgFeatureChange(iOtgFeatureCallbackInfo);
            }
		else
			r = KErrArgument;
        break;
        }
    default:
		r = KErrNotSupported;
		}

	aNeedsCompletion = aNeedsCompletion || (r != KErrNone);

	return r;
	}


TInt DLddUsbcChannel::DoTransferAsyncReq(TInt aEndpointNum, TAny* a1, TAny* a2, TBool& aNeedsCompletion)
	{
	// The general assumption is that none of these will complete now.
	// however, those that are returning something other than KErrNone will get completed
	// by the calling function.
	// So,	1) if you are returning KErrNone but really need to complete because completion criteria can be met
	//			(for example, sufficient data is available in the buffer) and then set aNeedsCompletion=ETrue..
	//		2) Do NOT complete here AT ALL.
	//
	aNeedsCompletion = EFalse;
	TInt r = KErrNone;
	TUsbcEndpoint* pEndpoint = NULL;
	TUsbcEndpointInfo* pEndpointInfo = NULL;
	TEndpointTransferInfo* pTfr = NULL;

	if (aEndpointNum == 0)
		{
		// ep0 requests
		if (!(iValidInterface || iOwnsDeviceControl))
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL rejected: not configured (Ep0)"));
			r = KErrUsbInterfaceNotReady;
			goto exit;
			}
		}
	else
		{
		// other eps
		if (!(iValidInterface && (iDeviceState == EUsbcDeviceStateConfigured ||
		                          iDeviceState == EUsbcDeviceStateSuspended))
		   )
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL rejected not configured (Ep %d)", aEndpointNum));
			r = KErrUsbInterfaceNotReady;
			goto exit;
			}
		}

	if (!ValidEndpoint(aEndpointNum))
		{
		//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: DoRequestL Read: in error complete"));
		r = KErrUsbEpNotInInterface;
		goto exit;
 		}

	if (a1 == NULL)
		{
		r = KErrArgument;
		goto exit;
		}

	TEndpointTransferInfo transferInfo;
	pTfr = &transferInfo;
	r = Kern::ThreadRawRead(iClient, a1, pTfr, sizeof(TEndpointTransferInfo));
	if (r != KErrNone)
		PanicClientThread(r);
	pEndpoint = iEndpoint[aEndpointNum];
	if (!pEndpoint)
		{
		//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: DoRequestL Read: in error complete"));
		r = KErrUsbEpNotInInterface;
		goto exit;
		}

	pEndpointInfo = pEndpoint->EndpointInfo();
	//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL %d", aEndpointNum));

	switch (pTfr->iTransferType)
		{

	case ETransferTypeReadData:
	case ETransferTypeReadPacket:
	case ETransferTypeReadUntilShort:
	case ETransferTypeReadOneOrMore:
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL Read"));
		if (pEndpoint->iDmaBuffers->RxIsActive())
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("**** ReadReq ep%d RxActive", aEndpointNum));
			}
		else
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("**** ReadReq ep%d RxInActive", aEndpointNum));
			}

		if (pEndpointInfo->iDir != KUsbEpDirOut &&
			pEndpointInfo->iDir != KUsbEpDirBidirect)
			{
			// Trying to do the wrong thing
			//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: DoRequestL Read: in error complete"));
			r = KErrUsbEpBadDirection;
			break;
			}
		// Set the length of data to zero now to catch all cases
		TPtrC8 pZeroDesc(NULL, 0);
		// Set client descriptor length to zero
		r = Kern::ThreadDesWrite(iClient, pTfr->iDes, pZeroDesc, 0, 0, iClient);
		if (r != KErrNone)
			PanicClientThread(r);
		pEndpoint->SetTransferInfo(pTfr);
		if (pEndpoint->iDmaBuffers->IsReaderEmpty())
			{
			pEndpoint->SetClientReadPending(ETrue);
			}
		else
			{
			if (pTfr->iTransferType == ETransferTypeReadPacket)
				{
				//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL Read packet: data available complete"));
				r = pEndpoint->CopyToClient(iClient);
				aNeedsCompletion = ETrue;
				break;
				}
			else if (pTfr->iTransferType == ETransferTypeReadData)
				{
				if (pTfr->iTransferSize <= pEndpoint->RxBytesAvailable())
					{
					//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL Read data: data available complete"));
					r = pEndpoint->CopyToClient(iClient);
					aNeedsCompletion = ETrue;
					break;
					}
				else
					{
					pEndpoint->SetClientReadPending(ETrue);
					}
				}
			else if (pTfr->iTransferType == ETransferTypeReadOneOrMore)
				{
				if (pEndpoint->RxBytesAvailable() > 0)
					{
					//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL Read data: data available complete"));
					r = pEndpoint->CopyToClient(iClient);
					aNeedsCompletion = ETrue;
					break;
					}
				else
					{
					pEndpoint->SetClientReadPending(ETrue);
					}
				}
			else if (pTfr->iTransferType == ETransferTypeReadUntilShort)
				{
				TInt nRx = pEndpoint->RxBytesAvailable();
				TInt maxPacketSize = pEndpoint->EndpointInfo()->iSize;
				if( (pTfr->iTransferSize <= nRx) ||
					(nRx < maxPacketSize) ||
					pEndpoint->iDmaBuffers->ShortPacketExists())
					{
					//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL Read data: data available complete"));
					r = pEndpoint->CopyToClient(iClient);
					aNeedsCompletion = ETrue;
					}
				else
					{
					pEndpoint->SetClientReadPending(ETrue);
					}
				}
			}
		r = pEndpoint->TryToStartRead(EFalse);
		if (r != KErrNone)
			{
			//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: DoRequestL Read: couldn't start read"));
			r = KErrNone;									// Reader full isn't a userside error;
			}
		break;
		}

	case ETransferTypeWrite:
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL Write 1"));
		if (pEndpointInfo->iDir != KUsbEpDirIn &&
			pEndpointInfo->iDir != KUsbEpDirBidirect)
			{
			//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: DoRequestL Write: wrong direction complete"));
			r = KErrUsbEpBadDirection;
			break;
			}
		//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL Write 2"));

		TAny* logicalSrc = pTfr->iDes;
		TInt desLength = Kern::ThreadGetDesLength(iClient, logicalSrc);
		if (desLength < pTfr->iTransferSize)
			{
			//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: DoRequestL Write: user buffer too short"));
			r = KErrUsbTransferSize;
			break;
			}

		//__KTRACE_OPT(KUSB, Kern::Printf("DoRequestL Write 3 length=%d maxlength=%d",
									//	pTfr->iTransferSize, desLength));
		// Zero length writes are acceptable
		pEndpoint->SetClientWritePending(ETrue);
		r = pEndpoint->TryToStartWrite(pTfr);
		if (r != KErrNone)
			{
			//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: DoRequestL Write: couldn't start write"));
			pEndpoint->SetClientWritePending(EFalse);
			}
		break;
		}

	default:
		//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: DoTransferAsyncReq: pTfr->iTransferType = %d not supported",
									//	  pTfr->iTransferType));
		r = KErrNotSupported;
		break;
		}
 exit:
	aNeedsCompletion = aNeedsCompletion || (r != KErrNone);
	return r;
	}


//
// Cancel an outstanding request - overriding pure virtual
//
TInt DLddUsbcChannel::DoCancel(TInt aReqNo)
	{
	TInt r = KErrNone;
	//__KTRACE_OPT(KUSB, Kern::Printf("DoCancel: 0x%x", aReqNo));
	if (aReqNo <= iNumberOfEndpoints)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("DoCancel endpoint: 0x%x", aReqNo));
		iEndpoint[aReqNo]->CancelTransfer(iClient);
		}
	else if (aReqNo == RDevUsbcClient::ERequestAlternateDeviceStatusNotify)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("DoCancel: ERequestAlternateDeviceStatusNotify 0x%x", aReqNo));
		iDeviceStatusNeeded = EFalse;
		iStatusFifo->FlushQueue();
		if (iStatusChangePtr)
			{
			TInt deviceState = iController->GetDeviceStatus();
			r = Kern::ThreadRawWrite(iClient, iStatusChangePtr, &deviceState, sizeof(deviceState), iClient);
			if (r != KErrNone)
				PanicClientThread(r);
			iStatusChangePtr = NULL;
			}
		}
	else if (aReqNo == RDevUsbcClient::ERequestReEnumerate)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("DoCancel ERequestReEnumerate: 0x%x", aReqNo));
		}
	else if (aReqNo == RDevUsbcClient::ERequestEndpointStatusNotify)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("DoCancel ERequestEndpointStatusNotify: 0x%x", aReqNo));
		CancelNotifyEndpointStatus();
		}
	else if (aReqNo == RDevUsbcClient::ERequestOtgFeaturesNotify)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("DoCancel ERequestOtgFeaturesNotify: 0x%x", aReqNo));
		CancelNotifyOtgFeatures();
		}
	else
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("DoCancel Unknown! 0x%x", aReqNo));
		}

	return r;
	}


void DLddUsbcChannel::CancelNotifyEndpointStatus()
	{
	if (iEndpointStatusChangePtr)
		{
		TUint epBitmap = 0;
		for (TInt i = 0; i <= iNumberOfEndpoints; i++)
			{
			TInt v = iController->GetEndpointStatus(this, iEndpoint[i]->RealEpNumber());
			TUint b;
			(v == EEndpointStateStalled) ? b = 1 : b = 0;
			epBitmap |= b << i;
			}
		Kern::ThreadRawWrite(iClient, iEndpointStatusChangePtr, (TUint8*) &epBitmap, sizeof(epBitmap), iClient);
		iEndpointStatusChangePtr = NULL;
		}
	iController->DeRegisterForEndpointStatusChange(this);
	}


void DLddUsbcChannel::CancelNotifyOtgFeatures()
	{
    if (iOtgFeatureChangePtr)
        {
        TUint8 features;
        iController->GetCurrentOtgFeatures(features);
		Kern::ThreadRawWrite(iClient, iOtgFeatureChangePtr, (TUint8*)&features, sizeof(features), iClient);
        iOtgFeatureChangePtr = NULL;
        }
	iController->DeRegisterForOtgFeatureChange(this);
    }


TInt DLddUsbcChannel::DoControl(TInt aFunction, TAny* a1, TAny* a2)
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("DoControl: %d", aFunction));

	TInt r = KErrNone;
	TInt ep, param;
	TUsbcEndpoint* pEndpoint;
	TPtrC8 pZeroDesc(NULL, 0);
	TEndpointDescriptorInfo epInfo;
	TUsbcIfcInfo ifcInfo;
	TCSDescriptorInfo desInfo;
	TUsbcEndpointResource epRes;
	TInt bandwidthPriority;
	
	/****declare trace objects and variables*******/
	CUTraces Testtracer;
	int ret=1;
	TUTrace TestTraceObject(UTracePrimary, UTraceSecondary, schema, context, pc);
	TUTrace NegTraceObject1((TPrimaryFilter)(UTracePrimary+1), (TSecondaryFilter)(UTraceSecondary+1), schema, context, pc);
	TUTrace NegTraceObject2((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary+2), schema, context, pc);
	TUTrace MultTraceObject1((TPrimaryFilter)(UTracePrimary), (TSecondaryFilter)(UTraceSecondary+2),schema, context, pc );
	TUTrace MultTraceObject2((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary),schema, context, pc );
	TUTrace MultTraceObject3((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary+1),schema, context, pc );
	TUTrace MultTraceObject4((TPrimaryFilter)(UTracePrimary+1), (TSecondaryFilter)(UTraceSecondary),schema, context, pc );
	TUTrace MultTraceObject5((TPrimaryFilter)(UTracePrimary+2),schema, context, pc );


	switch (aFunction)
		{
	case RDevUsbcClient::EControlEndpointZeroRequestError:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlEndpointZeroRequestError"));
		
		/****************UTRACE METHOD - ALL BASIC LOGGING*********************************/
		for(int tracetag=0; tracetag!=NumberOfUTraceMacros&&ret;tracetag++)
			{
				ret = 0;
				ret = Testtracer.DoTheTrace(tracetag);
			}
		for(int tracetag=0; tracetag!=NumberOfUTraceMacros&&ret;tracetag++)
			{
				ret = 0;
				ret = Testtracer.DoTheSetTrace(tracetag, TestTraceObject);
			}
		for(int tracetag=0; tracetag!=NumberOfUTraceMacros&&ret;tracetag++)
			{
				ret = 0;
				ret = Testtracer.DoTheStaticTrace(tracetag);
			}
	
			//put in extra logs for negative trace test
	
		NegTraceObject1.Trace();
		NegTraceObject2.Trace();
		
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+1), (TSecondaryFilter)(UTraceSecondary+1), schema, context, pc);
		TestTraceObject.Trace();
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary+2), schema, context, pc);
		TestTraceObject.Trace();
		
		TUTrace::Trace((TPrimaryFilter)(UTracePrimary+1), (TSecondaryFilter)(UTraceSecondary+1), schema, context, pc);
		TUTrace::Trace((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary+2), schema, context, pc);
		

		
		
	//	for(int tracetag=0; tracetag!=NumberOfPrintfMacros&&ret;tracetag++)
	//		{
	//			ret = 0;
	//			ret = Testtracer.DoThePrintf(tracetag);
	//		}
	//	for(int tracetag=0; tracetag!=NumberOfPrintfMacros&&ret;tracetag++)
	//		{
	//			ret = 0;
	//			ret = Testtracer.DoTheStaticPrintf(tracetag);
	//		}
			
	
			
		/****************End of Tracing**************************************/
		r = KErrNone;
		if (iOwnsDeviceControl || (iValidInterface && iDeviceState == EUsbcDeviceStateConfigured))
			{
			iController->Ep0Stall(this);
			}
		else
			{
			if (iDeviceState != EUsbcDeviceStateConfigured)
				r = KErrUsbDeviceNotConfigured;
			else
				r = KErrUsbInterfaceNotReady;
			}
		break;

	case RDevUsbcClient::EControlGetAlternateSetting:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetAlternateSetting"));
		/****************UTRACE METHOD NONstatic 0*********************************/
		Testtracer.DoTheSetTrace(0, TestTraceObject);
	
		if (iValidInterface && iDeviceState == EUsbcDeviceStateConfigured)
			{
			r = iController->GetInterfaceNumber(this, param);
			if (r == KErrNone)
				{
				r = Kern::ThreadRawWrite(iClient, a1, &param, sizeof(param), iClient);
				if (r != KErrNone)
					PanicClientThread(r);
				}
			}
		else
			{
			if (iDeviceState != EUsbcDeviceStateConfigured)
				r = KErrUsbDeviceNotConfigured;
			else
				r = KErrUsbInterfaceNotReady;
			}
		break;

	case RDevUsbcClient::EControlDeviceStatus:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlDeviceStatus"));			
		param = iController->GetDeviceStatus();
		r = Kern::ThreadRawWrite(iClient, a1, &param, sizeof(param), iClient);
		if (r != KErrNone)
			PanicClientThread(r);
		break;

	case RDevUsbcClient::EControlEndpointStatus:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlEndpointStatus"));
		if (iValidInterface && ValidEndpoint((TInt) a1))
			{
			pEndpoint = iEndpoint[(TInt)a1];
			if (pEndpoint == NULL)
				r = KErrNotSupported;
			else
				{
				param = iController->GetEndpointStatus(this, iEndpoint[(TInt)a1]->RealEpNumber());
				r = Kern::ThreadRawWrite(iClient, a2, &param, sizeof(param), iClient);
				if (r != KErrNone)
					PanicClientThread(r);
				}
			}
		else
			{
			if (iDeviceState != EUsbcDeviceStateConfigured)
				r = KErrUsbDeviceNotConfigured;
			else
				r = KErrUsbInterfaceNotReady;
			}
		break;

	case RDevUsbcClient::EControlQueryReceiveBuffer:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlQueryReceiveBuffer"));
		if (iValidInterface && ValidEndpoint((TInt) a1))
			{
			pEndpoint=iEndpoint[(TInt) a1];
			if (pEndpoint == NULL)
				r = KErrNotSupported;
			else if (pEndpoint->EndpointInfo()->iDir != KUsbEpDirIn)
				{
				//__KTRACE_OPT(KUSB, Kern::Printf("  bytes = %d", pEndpoint->RxBytesAvailable()));
				param = pEndpoint->RxBytesAvailable();
				r = Kern::ThreadRawWrite(iClient, a2, &param, sizeof(param), iClient);
				if (r != KErrNone)
					PanicClientThread(r);
				}
			}
		else
			{
			if (iDeviceState != EUsbcDeviceStateConfigured)
				r = KErrUsbDeviceNotConfigured;
			else
				r = KErrUsbInterfaceNotReady;
			}
		break;

	case RDevUsbcClient::EControlEndpointCaps:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlEndpointCaps"));
		r = Kern::ThreadDesWrite(iClient, a1, pZeroDesc, 0, 0, iClient);
		if (r != KErrNone)
			PanicClientThread(r);
		iController->EndpointCaps(this, *((TDes8*) a1));
		break;

	case RDevUsbcClient::EControlDeviceCaps:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlDeviceCaps"));
		r = Kern::ThreadDesWrite(iClient, a1, pZeroDesc, 0, 0, iClient);
		if (r != KErrNone)
			PanicClientThread(r);
		iController->DeviceCaps(this, *((TDes8*) a1));
		break;

	case RDevUsbcClient::EControlSendEp0StatusPacket:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSendEp0StatusPacket"));
		//**********************UTRACE METHOD NONstatic 2***********************/
		Testtracer.DoTheSetTrace(2, TestTraceObject);
				
		iController->SendEp0StatusPacket(this);
		break;

	case RDevUsbcClient::EControlHaltEndpoint:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlHaltEndpoint"));
		if (iValidInterface && ValidEndpoint((TInt) a1))
			{
			r = iController->HaltEndpoint(this, iEndpoint[(TInt)a1]->RealEpNumber());
			}
		else
			{
			if (iDeviceState != EUsbcDeviceStateConfigured)
				r = KErrUsbDeviceNotConfigured;
			else
				r = KErrUsbInterfaceNotReady;
			}
		break;

	case RDevUsbcClient::EControlClearHaltEndpoint:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlClearHaltEndpoint"));
		if (iValidInterface && ValidEndpoint((TInt) a1))
			{
			r = iController->ClearHaltEndpoint(this, iEndpoint[(TInt)a1]->RealEpNumber());
			}
		else
			{
			if (iDeviceState != EUsbcDeviceStateConfigured)
				r = KErrUsbDeviceNotConfigured;
			else
				r = KErrUsbInterfaceNotReady;
			}
		break;

	case RDevUsbcClient::EControlDumpRegisters:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlDumpRegisters"));
		iController->DumpRegisters();
		break;

	case RDevUsbcClient::EControlReleaseDeviceControl:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlReleaseDeviceControl"));
		//**********************UTRACE METHOD NONstatic 3***********************/
		Testtracer.DoTheSetTrace(3, TestTraceObject);

		iController->ReleaseDeviceControl(this);
		iOwnsDeviceControl = EFalse;
		break;

	case RDevUsbcClient::EControlEndpointZeroMaxPacketSizes:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlEndpointZeroMaxPacketSizes"));
		//**********************UTRACE METHOD NONstatic 4***********************/
		Testtracer.DoTheSetTrace(4, TestTraceObject);
	

		r = iController->EndpointZeroMaxPacketSizes();
		break;

	case RDevUsbcClient::EControlSetEndpointZeroMaxPacketSize:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetEndpointZeroMaxPacketSize"));
		//**********************UTRACE METHOD NONstatic 5***********************/
		Testtracer.DoTheSetTrace(5, TestTraceObject);
			
		r = iController->SetEndpointZeroMaxPacketSize(reinterpret_cast<TInt>(a1));
		break;

	case RDevUsbcClient::EControlGetEndpointZeroMaxPacketSize:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetEndpointZeroMaxPacketSize"));
		//**********************UTRACE METHOD NONstatic 6***********************/
		Testtracer.DoTheSetTrace(6, TestTraceObject);
			
		r = iController->Ep0PacketSize();
		break;

	case RDevUsbcClient::EControlGetDeviceDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetDeviceDescriptor"));
	//**********************UTRACE METHOD NONstatic 7***********************/
		Testtracer.DoTheSetTrace(7, TestTraceObject);
			
		r = Kern::ThreadDesWrite(iClient, a1, pZeroDesc, 0, 0, iClient);
		if (r != KErrNone)
			PanicClientThread(r);
		r = iController->GetDeviceDescriptor(iClient, *((TDes8*) a1));
		break;

	case RDevUsbcClient::EControlSetDeviceDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetDeviceDescriptor"));
	//**********************UTRACE METHOD NONstatic 8***********************/
		Testtracer.DoTheSetTrace(8, TestTraceObject);
			
		if (a1 != NULL)
			r = iController->SetDeviceDescriptor(iClient, *((TDes8*) a1));
		else
			r = KErrArgument;
		break;

	case RDevUsbcClient::EControlGetDeviceDescriptorSize:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetDeviceDescriptorSize"));
	//**********************UTRACE METHOD NONstatic 9***********************/
		Testtracer.DoTheSetTrace(9, TestTraceObject);
			
		if (a1 != NULL)
			r = iController->GetDeviceDescriptorSize(iClient, *((TDes8*) a1));
		else
			r = KErrArgument;
		break;

	case RDevUsbcClient::EControlGetConfigurationDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetConfigurationDescriptor"));
	//**********************UTRACE METHOD NONstatic 10***********************/
		Testtracer.DoTheSetTrace(10, TestTraceObject);
			
		r = Kern::ThreadDesWrite(iClient, a1, pZeroDesc, 0 , 0, iClient);
		if (r != KErrNone)
			PanicClientThread(r);
		r = iController->GetConfigurationDescriptor(iClient, *((TDes8*) a1));
		break;

	case RDevUsbcClient::EControlGetConfigurationDescriptorSize:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetConfigurationDescriptorSize"));
	//**********************UTRACE METHOD NONstatic 11***********************/
		Testtracer.DoTheSetTrace(11, TestTraceObject);
			
		if (a1 != NULL)
			{
			r = iController->GetConfigurationDescriptorSize(iClient, *((TDes8*) a1));
			}
		else
			r = KErrArgument;
		break;

	case RDevUsbcClient::EControlSetConfigurationDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetConfigurationDescriptor"));
	//**********************UTRACE METHOD NONstatic 12***********************/
		Testtracer.DoTheSetTrace(12, TestTraceObject);

		r = iController->SetConfigurationDescriptor(iClient, *((TDes8*) a1));
		break;

	case RDevUsbcClient::EControlGetInterfaceDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetInterfaceDescriptor"));
	//**********************UTRACE METHOD NONstatic 13***********************/
		Testtracer.DoTheSetTrace(13, TestTraceObject);

		r = iController->GetInterfaceDescriptor(iClient, this, (TInt) a1, *((TDes8*) a2));
		break;

	case RDevUsbcClient::EControlGetInterfaceDescriptorSize:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetInterfaceDescriptorSize"));
	//**********************UTRACE METHOD NONstatic 14***********************/
		Testtracer.DoTheSetTrace(14, TestTraceObject);

		r = iController->GetInterfaceDescriptorSize(iClient, this, (TInt) a1, *(TDes8*) a2);
		break;

	case RDevUsbcClient::EControlSetInterfaceDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetInterfaceDescriptor"));
	//**********************UTRACE METHOD NONstatic 15***********************/
		Testtracer.DoTheSetTrace(15, TestTraceObject);

		r = iController->SetInterfaceDescriptor(iClient, this, (TInt) a1, *((TDes8*) a2));
		break;

	case RDevUsbcClient::EControlGetEndpointDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetEndpointDescriptor"));
	//**********************UTRACE METHOD NONstatic 16***********************/
		Testtracer.DoTheSetTrace(16, TestTraceObject);

		r = Kern::ThreadRawRead(iClient, a1, &epInfo, sizeof(epInfo));
		if (r != KErrNone)
			PanicClientThread(r);
		ep = EpFromAlternateSetting(epInfo.iSetting, epInfo.iEndpoint);
		r = iController->GetEndpointDescriptor(iClient, this, epInfo.iSetting,
											   ep, *(TDes8*) epInfo.iArg);
		break;

	case RDevUsbcClient::EControlGetEndpointDescriptorSize:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetEndpointDescriptorSize"));
	//**********************UTRACE METHOD NONstatic 17***********************/
		Testtracer.DoTheSetTrace(17, TestTraceObject);

		r = Kern::ThreadRawRead(iClient, a1, &epInfo, sizeof(epInfo));
		if (r != KErrNone)
			PanicClientThread(r);
		ep = EpFromAlternateSetting(epInfo.iSetting, epInfo.iEndpoint);
		r = iController->GetEndpointDescriptorSize(iClient, this, epInfo.iSetting,
												   ep, *(TDes8*) epInfo.iArg);
		break;

	case RDevUsbcClient::EControlSetEndpointDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetEndpointDescriptor"));
		//**********************UTRACE METHOD NONstatic 18***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheSetTrace(18, TestTraceObject);

		r = Kern::ThreadRawRead(iClient, a1, &epInfo, sizeof(epInfo));
		if (r != KErrNone)
			PanicClientThread(r);
		ep = EpFromAlternateSetting(epInfo.iSetting, epInfo.iEndpoint);
		r = iController->SetEndpointDescriptor(iClient, this, epInfo.iSetting,
											   ep, *(TDes8*)epInfo.iArg);
		break;

	case RDevUsbcClient::EControlGetDeviceQualifierDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetDeviceQualifierDescriptor"));
	//**********************UTRACE METHOD NONstatic 19***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheSetTrace(19, TestTraceObject);

		r = Kern::ThreadDesWrite(iClient, a1, pZeroDesc, 0, 0, iClient);
		if (r != KErrNone)
			PanicClientThread(r);
		r = iController->GetDeviceQualifierDescriptor(iClient, *((TDes8*) a1));
		break;

	case RDevUsbcClient::EControlSetDeviceQualifierDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetDeviceQualifierDescriptor"));
	//**********************UTRACE METHOD NONstatic 20***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheSetTrace(20, TestTraceObject);

		if (a1 != NULL)
			r = iController->SetDeviceQualifierDescriptor(iClient, *((TDes8*) a1));
		else
			r = KErrArgument;
		break;

	case RDevUsbcClient::EControlGetOtherSpeedConfigurationDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetOtherSpeedConfigurationDescriptor"));
	//**********************UTRACE METHOD NONstatic 21***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheSetTrace(21, TestTraceObject);

		r = Kern::ThreadDesWrite(iClient, a1, pZeroDesc, 0 , 0, iClient);
		if (r != KErrNone)
			PanicClientThread(r);
		r = iController->GetOtherSpeedConfigurationDescriptor(iClient, *((TDes8*) a1));
		break;

	case RDevUsbcClient::EControlSetOtherSpeedConfigurationDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetOtherSpeedConfigurationDescriptor"));
	//**********************UTRACE METHOD NONstatic 22***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheSetTrace(22, TestTraceObject);

		r = iController->SetOtherSpeedConfigurationDescriptor(iClient, *((TDes8*) a1));
		break;


	case RDevUsbcClient::EControlGetCSInterfaceDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetCSInterfaceDescriptor"));
	//**********************UTRACE METHOD NONstatic 23***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheSetTrace(23, TestTraceObject);

		r = iController->GetCSInterfaceDescriptorBlock(iClient, this, (TInt) a1, *((TDes8*) a2));
		break;

	case RDevUsbcClient::EControlGetCSInterfaceDescriptorSize:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetCSInterfaceDescriptorSize"));
	//**********************UTRACE METHOD STATIC 0***********************/
		Testtracer.DoTheStaticTrace(0);

		r = iController->GetCSInterfaceDescriptorBlockSize(iClient, this, (TInt) a1, *(TDes8*) a2);
		break;

	case RDevUsbcClient::EControlGetCSEndpointDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetCSEndpointDescriptor"));
	//**********************UTRACE METHOD STATIC 1***********************/
		Testtracer.DoTheStaticTrace(1);

		r = Kern::ThreadRawRead(iClient, a1, &epInfo, sizeof(epInfo));
		if (r != KErrNone)
			PanicClientThread(r);
		ep = EpFromAlternateSetting(epInfo.iSetting, epInfo.iEndpoint);
		r = iController->GetCSEndpointDescriptorBlock(iClient, this, epInfo.iSetting,
													  ep, *(TDes8*) epInfo.iArg);
		break;

	case RDevUsbcClient::EControlGetCSEndpointDescriptorSize:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetCSEndpointDescriptorSize"));
		//**********************UTRACE METHOD STATIC 2***********************/
		Testtracer.DoTheStaticTrace(2);
			
		r = Kern::ThreadRawRead(iClient, a1, &epInfo, sizeof(epInfo));
		if (r != KErrNone)
			PanicClientThread(r);
		ep = EpFromAlternateSetting(epInfo.iSetting, epInfo.iEndpoint);
		r = iController->GetCSEndpointDescriptorBlockSize(iClient, this, epInfo.iSetting,
														  ep, *(TDes8*) epInfo.iArg);
		break;

	case RDevUsbcClient::EControlSignalRemoteWakeup:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSignalRemoteWakeup"));
		//**********************UTRACE METHOD STATIC 3***********************/
		Testtracer.DoTheStaticTrace(3);
			
	
		r = iController->SignalRemoteWakeup();
		break;

	case RDevUsbcClient::EControlDeviceDisconnectFromHost:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlDeviceDisconnectFromHost"));
		r = iController->UsbDisconnect();
		break;

	case RDevUsbcClient::EControlDeviceConnectToHost:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlDeviceConnectToHost"));
		r = iController->UsbConnect();
		break;

	case RDevUsbcClient::EControlDevicePowerUpUdc:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlDevicePowerUpUdc"));
	//**********************UTRACE METHOD STATIC 4***********************/
		Testtracer.DoTheStaticTrace(4);
			
		r = iController->PowerUpUdc();
		break;

	case RDevUsbcClient::EControlSetDeviceControl:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetDeviceControl"));
	//**********************UTRACE METHOD STATIC 5***********************/
		Testtracer.DoTheStaticTrace(5);

		r = iController->SetDeviceControl(this);
		if (r == KErrNone)
			{
			iOwnsDeviceControl = ETrue;
			if (iEndpoint[0] == NULL)
				{
				//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetDeviceControl 11"));
				r = SetupEp0();
				if (r != KErrNone)
					{
					//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: SetupEp0() failed"));
					iController->ReleaseDeviceControl(this);
					DestroyEp0();
					iOwnsDeviceControl = EFalse;
					}
				iEndpoint[0]->TryToStartRead(EFalse);
				}
			}
		else
			r = KErrInUse;
		break;

	case RDevUsbcClient::EControlCurrentlyUsingHighSpeed:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlCurrentlyUsingHighSpeed"));
	//**********************UTRACE METHOD STATIC 6***********************/
		Testtracer.DoTheStaticTrace(6);
	
		r = iController->CurrentlyUsingHighSpeed();
		break;

	case RDevUsbcClient::EControlSetInterface:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetInterface"));
		r = Kern::ThreadRawRead(iClient, a2, &ifcInfo, sizeof(ifcInfo));
		if (r != KErrNone)
			PanicClientThread(r);
		bandwidthPriority = ifcInfo.iBandwidthPriority;
		if ((bandwidthPriority & 0xffffff00) ||
			((bandwidthPriority & 0x0f) >= KUsbcDmaBufMaxPriorities) ||
			(((bandwidthPriority >> 4) & 0x0f) >= KUsbcDmaBufMaxPriorities))
			{
			r = KErrArgument;
			}
		else
			{
			r = SetInterface((TInt) a1, &ifcInfo);
			}
		break;

	case RDevUsbcClient::EControlReleaseInterface:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlReleaseInterface"));
	//**********************UTRACE METHOD STATIC 7***********************/
		Testtracer.DoTheStaticTrace(7);
			
		r = iController->ReleaseInterface(this, (TInt) a1);
		if (r == KErrNone)
			{
			DestroyInterface((TUint) a1);
			}
		else
			{
			//__KTRACE_OPT(KPANIC, Kern::Printf("  Error in PIL: LDD interface won't be released."));
			}
		break;

	case RDevUsbcClient::EControlSetCSInterfaceDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetCSInterfaceDescriptor"));
	//**********************UTRACE METHOD STATIC 8***********************/
		Testtracer.DoTheStaticTrace(8);

		r = Kern::ThreadRawRead(iClient, a1, &desInfo, sizeof(desInfo));
		if (r != KErrNone)
			PanicClientThread(r);
		r = iController->SetCSInterfaceDescriptorBlock(iClient, this, desInfo.iSetting,
													   *reinterpret_cast<const TDes8*>(desInfo.iArg),
													   desInfo.iSize);
		break;

	case RDevUsbcClient::EControlSetCSEndpointDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetCSEndpointDescriptor"));
	//**********************UTRACE METHOD STATIC 9***********************/
		Testtracer.DoTheStaticTrace(9);

		r = Kern::ThreadRawRead(iClient, a1, &desInfo, sizeof(desInfo));
		if (r != KErrNone)
			PanicClientThread(r);
		ep = EpFromAlternateSetting(desInfo.iSetting, desInfo.iEndpoint);
		r = iController->SetCSEndpointDescriptorBlock(iClient, this, desInfo.iSetting, ep,
													  *reinterpret_cast<const TDes8*>(desInfo.iArg),
													  desInfo.iSize);
		break;

	case RDevUsbcClient::EControlGetStringDescriptorLangId:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetStringDescriptorLangId"));
		//**********************Trace Multiple logging test***********************/
		Testtracer.DoTheTrace(3);
		NegTraceObject1.Trace(aData, aSize);
		MultTraceObject1.Trace(aData, aSize);
		MultTraceObject2.Trace(aData, aSize);
		NegTraceObject2.Trace(aData, aSize);
		MultTraceObject3.Trace(aData, aSize);
		MultTraceObject4.Trace(aData, aSize);
		MultTraceObject5.Trace(aData, aSize);
		
		
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary), (TSecondaryFilter)(UTraceSecondary), schema, context, pc);
		TestTraceObject.Trace(aData, aSize);	
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+1), (TSecondaryFilter)(UTraceSecondary+1), schema, context, pc);
		TestTraceObject.Trace(aData, aSize);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary), (TSecondaryFilter)(UTraceSecondary+2), schema, context, pc);
		TestTraceObject.Trace(aData, aSize);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary), schema, context, pc);
		TestTraceObject.Trace(aData, aSize);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary+2), schema, context, pc);
		TestTraceObject.Trace(aData, aSize);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary+1), schema, context, pc);
		TestTraceObject.Trace(aData, aSize);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+1), (TSecondaryFilter)(UTraceSecondary), schema, context, pc);
		TestTraceObject.Trace(aData, aSize);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), schema, context, pc);
		TestTraceObject.Trace(aData, aSize);
		
		TUTrace::Trace((TPrimaryFilter)(UTracePrimary), (TSecondaryFilter)(UTraceSecondary), schema, context, pc, aData, aSize);
		TUTrace::Trace((TPrimaryFilter)(UTracePrimary+1), (TSecondaryFilter)(UTraceSecondary+1), schema, context, pc, aData, aSize);
		TUTrace::Trace((TPrimaryFilter)(UTracePrimary), (TSecondaryFilter)(UTraceSecondary+2), schema, context, pc, aData, aSize);
		TUTrace::Trace((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary), schema, context, pc, aData, aSize);
		TUTrace::Trace((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary+2), schema, context, pc, aData, aSize);
		TUTrace::Trace((TPrimaryFilter)(UTracePrimary+2), (TSecondaryFilter)(UTraceSecondary+1), schema, context, pc, aData, aSize);
		TUTrace::Trace((TPrimaryFilter)(UTracePrimary+1), (TSecondaryFilter)(UTraceSecondary), schema, context, pc, aData, aSize);
		TUTrace::TracePrimary((TPrimaryFilter)(UTracePrimary+2), schema, context, pc, aData, aSize);

		
		r = iController->GetStringDescriptorLangId(iClient, *((TDes8*) a1));
		break;

	case RDevUsbcClient::EControlSetStringDescriptorLangId:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetStringDescriptorLangId"));
	//**********************UTRACE METHOD STATIC 10***********************/
		Testtracer.DoTheStaticTrace(10);

		r = iController->SetStringDescriptorLangId(reinterpret_cast<TUint>(a1));
		break;

	case RDevUsbcClient::EControlGetManufacturerStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetManufacturerStringDescriptor"));
	//**********************UTRACE METHOD STATIC 11***********************/
		Testtracer.DoTheStaticTrace(11);

		r = iController->GetManufacturerStringDescriptor(iClient, *((TPtr8*) a1));
		break;

	case RDevUsbcClient::EControlSetManufacturerStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetManufacturerStringDescriptor"));
	//**********************UTRACE METHOD STATIC 12***********************/
		Testtracer.DoTheStaticTrace(12);

		r = iController->SetManufacturerStringDescriptor(iClient, *((TPtr8*) a1));
		break;

	case RDevUsbcClient::EControlRemoveManufacturerStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlRemoveManufacturerStringDescriptor"));
	//**********************UTRACE METHOD STATIC 13***********************/
		Testtracer.DoTheStaticTrace(13);

		r = iController->RemoveManufacturerStringDescriptor();
		break;

	case RDevUsbcClient::EControlGetProductStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetProductStringDescriptor"));
	//**********************UTRACE METHOD STATIC 14***********************/
		Testtracer.DoTheStaticTrace(14);
	
		r = iController->GetProductStringDescriptor(iClient, *((TPtr8*) a1));
		break;

	case RDevUsbcClient::EControlSetProductStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetProductStringDescriptor"));
	//**********************UTRACE METHOD STATIC 15***********************/
		Testtracer.DoTheStaticTrace(15);
		
		r = iController->SetProductStringDescriptor(iClient, *((TPtr8*) a1));
		break;

	case RDevUsbcClient::EControlRemoveProductStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlRemoveProductStringDescriptor"));
	//**********************UTRACE METHOD STATIC 16***********************/
		Testtracer.DoTheStaticTrace(16);

		r = iController->RemoveProductStringDescriptor();
		break;

	case RDevUsbcClient::EControlGetSerialNumberStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetSerialNumberStringDescriptor"));
	//**********************UTRACE METHOD STATIC 17***********************/
		Testtracer.DoTheStaticTrace(17);

		r = iController->GetSerialNumberStringDescriptor(iClient, *((TPtr8*) a1));
		break;

	case RDevUsbcClient::EControlSetSerialNumberStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetSerialNumberStringDescriptor"));
	//**********************UTRACE METHOD STATIC 18***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheStaticTrace(18);

		r = iController->SetSerialNumberStringDescriptor(iClient, *((TPtr8*) a1));
		break;

	case RDevUsbcClient::EControlRemoveSerialNumberStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlRemoveSerialNumberStringDescriptor"));
		//**********************UTRACE METHOD STATIC 19***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheStaticTrace(19);

	
		r = iController->RemoveSerialNumberStringDescriptor();
		break;

	case RDevUsbcClient::EControlGetConfigurationStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetConfigurationStringDescriptor"));
		//**********************UTRACE METHOD NONstatic 1***********************/
		Testtracer.DoTheSetTrace(1, TestTraceObject);
		r = iController->GetConfigurationStringDescriptor(iClient, *((TPtr8*) a1));
		break;

	case RDevUsbcClient::EControlSetConfigurationStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetConfigurationStringDescriptor"));
	//**********************UTRACE METHOD STATIC 21***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheStaticTrace(21);
	
		r = iController->SetConfigurationStringDescriptor(iClient, *((TPtr8*) a1));
		break;

	case RDevUsbcClient::EControlRemoveConfigurationStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlRemoveConfigurationStringDescriptor"));
	//**********************UTRACE METHOD STATIC 22***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheStaticTrace(22);

		
		r = iController->RemoveConfigurationStringDescriptor();
		break;

	case RDevUsbcClient::EControlGetStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlGetStringDescriptor"));
	//**********************UTRACE METHOD STATIC 23***********************/
	//	for(int i=0;i!=UTrace_Count;i++)
	//		Testtracer.DoTheStaticTrace(23);

		r = iController->GetStringDescriptor(iClient, (TUint8) (TInt) a1, *((TPtr8*) a2));
		break;

	case RDevUsbcClient::EControlSetStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlSetStringDescriptor"));
		//**********************UTrace PRINTF METHODS+NEGATIVE TEST***********************/
			Testtracer.DoThePrintf(8);
			Testtracer.DoThePrintf(12);
			Testtracer.DoTheStaticPrintf(8);
			Testtracer.DoTheStaticPrintf(12);
		
		//put in extra logs for negative trace printf test
	
		NegTraceObject1.Printf("c-style \"string\" with number %i!", 9999);
		NegTraceObject2.Printf("c-style \"string\" with number %i!", 9999);
		
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+1), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);
		
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary+1), context, pc, "static string no %i", 9999);
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary+2), context, pc, "static string no %i", 9999);
		

		

	
			
		/****************End of Printf Tracing**************************************/


		r = iController->SetStringDescriptor(iClient, (TUint8) (TInt) a1, *((TPtr8*) a2));
		break;

	case RDevUsbcClient::EControlRemoveStringDescriptor:
		//__KTRACE_OPT(KUSB, Kern::Printf("EControlRemoveStringDescriptor"));
	//**********************Trace Multiple logging test***********************/
		Testtracer.DoThePrintf(8);
		NegTraceObject1.Printf("c-style \"string\" with number %i!", 9999);
		MultTraceObject1.Printf("c-style \"string\" with number %i!", 9999);
		MultTraceObject2.Printf("c-style \"string\" with number %i!", 9999);
		NegTraceObject2.Printf("c-style \"string\" with number %i!", 9999);
		MultTraceObject3.Printf("c-style \"string\" with number %i!", 9999);
		MultTraceObject4.Printf("c-style \"string\" with number %i!", 9999);
		MultTraceObject5.Printf("c-style \"string\" with number %i!", 9999);
		
		
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);;	
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+1), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+1), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);
		TestTraceObject.Set((TPrimaryFilter)(UTracePrimary+2), schema, context, pc);
		TestTraceObject.Printf("c-style \"string\" with number %i!", 9999);
		
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary),  context, pc, "c-style \"string\" with number %i!", 9999);
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary+1), context, pc, "c-style \"string\" with number %i!", 9999);
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary),  context, pc, "c-style \"string\" with number %i!", 9999);
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary+2),  context, pc, "c-style \"string\" with number %i!", 9999);
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary+2),  context, pc, "c-style \"string\" with number %i!", 9999);
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary+2),  context, pc, "c-style \"string\" with number %i!", 9999);
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary+1),  context, pc, "c-style \"string\" with number %i!", 9999);
		TUTrace::PrintfPrimary((TPrimaryFilter)(UTracePrimary+2), context, pc, "c-style \"string\" with number %i!", 9999);

		
		r = iController->RemoveStringDescriptor((TUint8) (TInt) a1);
		break;

	case RDevUsbcClient::EControlAllocateEndpointResource:
	
	
		epRes = (TUsbcEndpointResource)((TInt) a2);
		if (!ValidEndpoint((TInt)a1))
			{
			r = KErrUsbEpNotInInterface;
			}
		else
			{
			r = iController->AllocateEndpointResource(this, iEndpoint[(TInt)a1]->RealEpNumber(), epRes);
			}
		break;

	case RDevUsbcClient::EControlDeAllocateEndpointResource:
		epRes = (TUsbcEndpointResource)((TInt) a2);
		if (!ValidEndpoint((TInt)a1))
			{
			r = KErrUsbEpNotInInterface;
			}
		else
			{
			r = iController->DeAllocateEndpointResource(this, iEndpoint[(TInt)a1]->RealEpNumber(), epRes);
			}
		break;

	case RDevUsbcClient::EControlQueryEndpointResourceUse:
		epRes = (TUsbcEndpointResource)((TInt) a2);
		if (!ValidEndpoint((TInt)a1))
			{
			r = KErrUsbEpNotInInterface;
			}
		else
			{
			r = iController->QueryEndpointResource(this, iEndpoint[(TInt)a1]->RealEpNumber(), epRes);
			}
		break;

	case RDevUsbcClient::EControlSetOtgDescriptor:
		{
		r = iController->SetOtgDescriptor(iClient, *((const TDesC8*)a1));
		}
		break;

	case RDevUsbcClient::EControlGetOtgDescriptor:
		{
		r = iController->GetOtgDescriptor(iClient, *((TDes8*)a1));
		}
		break;

	case RDevUsbcClient::EControlGetOtgFeatures:
		{
		r = iController->GetOtgFeatures(iClient, *((TDes8*)a1));
		}
		break;

	case RDevUsbcClient::EControlEnableUsbDriver:
		{		
		iController->EnableClientStack();
		}
		break;

    case RDevUsbcClient::EControlDisableUsbDriver:
		{
		iController->DisableClientStack();
		}
		break;

    default:
		//__KTRACE_OPT(KUSB, Kern::Printf("Function code not supported"));
		r = KErrNotSupported;
		}

	return r;
	}


TInt DLddUsbcChannel::SetInterface(TInt aInterfaceNumber, TUsbcIfcInfo* aInfoBuf)
	{
	TUsbcInterfaceInfoBuf ifc_info_buf;
	TUsbcInterfaceInfoBuf* const ifc_info_buf_ptr = aInfoBuf->iInterfaceData;
	const TInt srcLen = Kern::ThreadGetDesLength(iClient, ifc_info_buf_ptr);
	if (srcLen < ifc_info_buf.Length())
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface can't copy"));
		PanicClientThread(EDesOverflow);
		}

	TInt r = Kern::ThreadDesRead(iClient, ifc_info_buf_ptr, ifc_info_buf, 0, KChunkShiftBy0);
	if (r != KErrNone)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface Copy failed reason=%d", r));
		PanicClientThread(r);
		}

	TUsbcEndpointInfo* pEndpointData = ifc_info_buf().iEndpointData;

	// If an alternate interface is being asked for then do nothing,
	// just pass it down to the Controller.
	const TInt num_endpoints = ifc_info_buf().iTotalEndpointsUsed;
	//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface num_endpoints=%d", num_endpoints));

	// [The next 4 variables have to be initialized here because of the goto's that follow.]
	// Default attribute for non-dma enabled buffering:
	TUint32 cacheAttribs = EMapAttrSupRw | EMapAttrCachedMax;
	const TUint32 bandwidthPriority = aInfoBuf->iBandwidthPriority;
	// (IN & OUT eps need to have different cache attribs:
	//  IN will be uncached, OUT will be cached.)
	TInt totalINBufferSize = 0;
	TInt totalOUTBufferSize = 0;

	TInt real_ep_numbers[6] = {-1, -1, -1, -1, -1, -1};

    TUsbcAlternateSettingList* alternateSettingListRec = new TUsbcAlternateSettingList;
	if (!alternateSettingListRec)
		{
		r = KErrNoMemory;
		goto KillAll;
		}

	// chain in this alternate setting
	alternateSettingListRec->iNext = iAlternateSettingList;
	iAlternateSettingList = alternateSettingListRec;
	alternateSettingListRec->iSetting = aInterfaceNumber;
	alternateSettingListRec->iNumberOfEndpoints = num_endpoints;

	// ep0
	if (iEndpoint[0] == NULL)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface 11"));
		r = SetupEp0();
		if (r != KErrNone)
			{
			//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: SetupEp0() failed"));
			goto KillAll;
			}
		}

	// other endpoints
	for (TInt i = 1; i <= num_endpoints; i++, pEndpointData++)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface for ep=%d", i));
		if (!ValidateEndpoint(pEndpointData))
			{
			r = KErrUsbBadEndpoint;
			goto KillAll;
			}
		TUsbcEndpoint* ep = new TUsbcEndpoint(this, iController, pEndpointData, i, bandwidthPriority);
		alternateSettingListRec->iEndpoint[i] = ep;
		if (!ep)
			{
			r = KErrNoMemory;
			goto KillAll;
			}
		if (ep->Construct() != KErrNone)
			{
			r = KErrNoMemory;
			goto KillAll;
			}
		if (pEndpointData->iDir == KUsbEpDirIn)
			{
			totalINBufferSize += ep->BufferTotalSize();
			//__KTRACE_OPT(KUSB, Kern::Printf("IN buffering now %d", totalINBufferSize));
			}
		else if (pEndpointData->iDir == KUsbEpDirOut)
			{
			totalOUTBufferSize += ep->BufferTotalSize();
			//__KTRACE_OPT(KUSB, Kern::Printf("OUT buffering now %d", totalOUTBufferSize));
			}
		//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface for ep=%d rec=0x%08x ep==0x%08x",
									//	i, alternateSettingListRec, ep));
		}

	// See if PIL will accept this interface
	//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface Calling controller"));
	r = iController->SetInterface(this,
								  iClient,
								  aInterfaceNumber,
								  ifc_info_buf().iClass,
								  aInfoBuf->iString,
								  (TInt) ifc_info_buf().iTotalEndpointsUsed,
								  (TUsbcEndpointInfo*) ifc_info_buf().iEndpointData,
								  &real_ep_numbers);

	//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface controller returned %d", r));
	if (r != KErrNone)
		{
		//__KTRACE_OPT(KPANIC, Kern::Printf("SetInterface failed reason=%d", r));
		goto KillAll;
		}

	// Record the 'real' endpoint number used by the PDD in both the Ep and
	// the Req callback:
	for (TInt i = 1; i <= num_endpoints; i++)
		{
		alternateSettingListRec->iEndpoint[i]->SetRealEpNumber(real_ep_numbers[i]);
		}

	if (totalOUTBufferSize != 0)
		{
		// maximally cached always
		//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface setting up OUT buffering size=%d", totalOUTBufferSize));
		iHwChunkOUT = SetupInterfaceMemory(totalOUTBufferSize, iHwChunkOUT, KUsbEpDirOut, cacheAttribs);
		if (iHwChunkOUT == NULL)
			{
			//__KTRACE_OPT(KPANIC, Kern::Printf("SetInterface can't get chunk for OUT buffering size=%d reason=%d",
										//	  totalOUTBufferSize, r));
			r = KErrNoMemory;
			goto KillAll;
			}
		}
	if (totalINBufferSize != 0)
		{
		cacheAttribs = EMapAttrSupRw;
		//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface setting up IN buffering size=%d", totalINBufferSize));
		iHwChunkIN = SetupInterfaceMemory(totalINBufferSize, iHwChunkIN, KUsbEpDirIn, cacheAttribs);
		if (iHwChunkIN == NULL)
			{
			//__KTRACE_OPT(KPANIC, Kern::Printf("SetInterface can't get chunk for IN buffering size=%d reason=%d",
											//  totalOUTBufferSize, r));
			r = KErrNoMemory;
			goto KillAll;
			}
		}
	//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface ready to exit"));

	if (aInterfaceNumber == 0)
		{
		// make sure we're ready to go with the main interface
		iValidInterface = ETrue;
		//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface SelectAlternateSetting"));
		SelectAlternateSetting(0);
		}

	return KErrNone;

 KillAll:
	DestroyAllInterfaces();
	DestroyEp0();
	return r;
	}


DPlatChunkHw* DLddUsbcChannel::SetupInterfaceMemory(TInt aBufferSize, DPlatChunkHw* aHwChunk,
													TUint aDirection, TUint32 aCacheAttribs)
	{
	TUint8* oldBase = NULL;
	if (aHwChunk != NULL)
		oldBase = reinterpret_cast<TUint8*>(aHwChunk->LinearAddress());

	DPlatChunkHw* chunk = ReAllocate(aBufferSize, aHwChunk, aCacheAttribs);
	if (chunk == NULL)
		{
		// lost all interfaces:
		// Tell Controller to release Interface and h/w resources associated with this
		iController->DeRegisterClient(this);
		}
	else
		{
		// Parcel out the memory between endpoints
		TUint8* newBase = reinterpret_cast<TUint8*>(chunk->LinearAddress());
		TBool needsRebase = (newBase != oldBase);
		TUint8* pBuf = newBase;
		TUint8* pBufIf = pBuf;							   // this is where an interface's ep buffering starts
		TUsbcAlternateSettingList* asRec = iAlternateSettingList;
		// the current interface
		//__KTRACE_OPT(KUSB, Kern::Printf("SetupInterfaceMemory rebasing setting=%d", asRec->iSetting));
		RebaseInterfaceMemory(asRec, pBuf, aDirection);
		// now the others if a rebase has occured
		if (needsRebase)
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("SetupInterfaceMemory rebasing "));
			asRec = asRec->iNext;
			while (asRec)
				{
				// Interfaces are not concurrent so they can all start at the same logical address
				//__KTRACE_OPT(KUSB, Kern::Printf("SetupInterfaceMemory rebasing setting=%d", asRec->iSetting));
				pBuf = pBufIf;
				RebaseInterfaceMemory(asRec, pBuf, aDirection);
				asRec = asRec->iNext;
				}
			}
		//__KTRACE_OPT(KUSB, Kern::Printf("SetInterface numberOfEndpoints"));
		}
	return chunk;
	}


TInt DLddUsbcChannel::SetupEp0()
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("SetupEp0 entry %x", this));
	TInt ep0Size = iController->Ep0PacketSize();
	TUsbcEndpointInfo ep0Info = TUsbcEndpointInfo(KUsbEpTypeControl, KUsbEpDirBidirect, ep0Size);
	TUsbcEndpoint* ep0 = new TUsbcEndpoint(this, iController, &ep0Info, 0, 0);
	if (ep0 == NULL)
		{
		return KErrNoMemory;
		}
	// In case we have to return early:
	iEndpoint[0] = ep0;
	TInt r = ep0->Construct();
	if (r != KErrNone)
		{
		return KErrNoMemory;
		}
	TInt bufferSize = ep0->BufferTotalSize();
	TUint32 cacheAttribs = EMapAttrSupRw | EMapAttrCachedMax;
	iHwChunkEp0 = Allocate(bufferSize, cacheAttribs);
	if (iHwChunkEp0 == NULL)
		{
		return KErrNoMemory;
		}
	iBufferSizeEp0 = bufferSize;
	iBufferBaseEp0 = (TUint8*) iHwChunkEp0->LinearAddress();
	ep0->SetBufferBase(iBufferBaseEp0);
	ep0->SetRealEpNumber(0);
	//__KTRACE_OPT(KUSB, Kern::Printf("SetupEp0 60 buffersize=%d", iBufferSizeEp0));
	//__KTRACE_OPT(KUSB, Kern::Printf("SetupEp0 exit bufferbase=0x%08x", iBufferBaseEp0));
	return KErrNone;
	}


void DLddUsbcChannel::RebaseInterfaceMemory(TUsbcAlternateSettingList* aAlternateSettingListRec,
											TUint8* aBase, TUint aDirection)
	{
	TUint8* pBuf = aBase;
	//__KTRACE_OPT(KUSB, Kern::Printf("RebaseInterfaceMemory buffer base rec= 0x%08x", aAlternateSettingListRec));
	for (TInt i = 1; i <= aAlternateSettingListRec->iNumberOfEndpoints; i++)
		{
		TUsbcEndpoint* ep = aAlternateSettingListRec->iEndpoint[i];
		if (ep != NULL && (ep->EndpointInfo()->iDir == aDirection))
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("RebaseInterfaceMemory buffer base for ep%d 0x%08x 0x%08x",
										//	i, pBuf, ep));
			pBuf = ep->SetBufferBase(pBuf);
			}
		else
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("RebaseInterfaceMemory ep%d wrong direction", i));
			}
		}
	}


void DLddUsbcChannel::DestroyAllInterfaces()
	{
	// Removes all interfaces
	TUsbcAlternateSettingList* alternateSettingListRec = iAlternateSettingList;
	while (alternateSettingListRec)
		{
		DestroyEndpoints(alternateSettingListRec);
		iController->ReleaseInterface(this, alternateSettingListRec->iSetting);
		TUsbcAlternateSettingList* alternateSettingListRecNext = alternateSettingListRec->iNext;
		delete alternateSettingListRec;
		alternateSettingListRec = alternateSettingListRecNext;
		}
	iNumberOfEndpoints = 0;
	iAlternateSettingList = NULL;

	ClosePhysicalChunk(iHwChunkIN);
	ClosePhysicalChunk(iHwChunkOUT);

	iValidInterface = EFalse;
	}


void DLddUsbcChannel::DestroyInterface(TUint aInterfaceNumber)
	{
	if (iAlternateSetting == aInterfaceNumber)
		{
		ResetInterface(KErrUsbInterfaceNotReady);
		iValidInterface = EFalse;
		iNumberOfEndpoints = 0;
		for (TInt i = 1; i <= KMaxEndpointsPerClient; i++)
			{
			iEndpoint[i] = NULL;
			}
		}
	TUsbcAlternateSettingList* alternateSettingListRec = iAlternateSettingList;
	TUsbcAlternateSettingList* alternateSettingListRecOld = NULL;
	while (alternateSettingListRec)
		{
		TUsbcAlternateSettingList* alternateSettingListRecNext = alternateSettingListRec->iNext;
		if (alternateSettingListRec->iSetting == aInterfaceNumber)
			{
			// This record is to be deleted
			if (alternateSettingListRecOld == NULL)
				{
				// The record to be deleted is at the list head
				iAlternateSettingList = alternateSettingListRecNext;
				}
			else
				{
				// The record to be deleted is NOT at the list head
				alternateSettingListRecOld->iNext = alternateSettingListRecNext;
				}
			DestroyEndpoints(alternateSettingListRec);
			delete alternateSettingListRec;
			break;
			}
		alternateSettingListRecOld = alternateSettingListRec;
		alternateSettingListRec = alternateSettingListRecNext;
		}

	if (iAlternateSettingList == NULL)
		{
		// if no interfaces left destroy non-ep0 buffering
		ClosePhysicalChunk(iHwChunkIN);
		ClosePhysicalChunk(iHwChunkOUT);
		}
	}


void DLddUsbcChannel::DestroyEp0()
	{
	delete iEndpoint[0];
	iEndpoint[0] = NULL;
	ClosePhysicalChunk(iHwChunkEp0);
	}


void DLddUsbcChannel::DestroyEndpoints(TUsbcAlternateSettingList* aListRec)
	{
	for (TInt i = 1; i < KMaxEndpointsPerClient; i++)
		{
		delete aListRec->iEndpoint[i];
		aListRec->iEndpoint[i] = NULL;
		}
	}


void DLddUsbcChannel::EndpointStatusChangeCallback(TAny* aDLddUsbcChannel)
    {
	//__KTRACE_OPT(KUSB, Kern::Printf("EndpointStatusChangeCallback"));
    DLddUsbcChannel* dUsbc = (DLddUsbcChannel*) aDLddUsbcChannel;
	if (dUsbc->iChannelClosing)
		return;
	TUint endpointState = dUsbc->iEndpointStatusCallbackInfo.State();
	const TInt reqNo = (TInt) RDevUsbcClient::ERequestEndpointStatusNotify;
	if (dUsbc->iRequestStatus[reqNo])
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("EndpointStatusChangeCallback Notify status"));
		DThread* client = dUsbc->iClient;
		// set client descriptor length to zero
		TInt r = Kern::ThreadRawWrite(client, dUsbc->iEndpointStatusChangePtr, &endpointState,
									  sizeof(TUint), client);
		if (r != KErrNone)
			dUsbc->PanicClientThread(r);
		Kern::RequestComplete(dUsbc->iClient, dUsbc->iRequestStatus[reqNo], r);
		dUsbc->iEndpointStatusChangePtr = NULL;
		}
	}


void DLddUsbcChannel::StatusChangeCallback(TAny* aDLddUsbcChannel)
	{
    DLddUsbcChannel* dUsbc = (DLddUsbcChannel*) aDLddUsbcChannel;
	if (dUsbc->iChannelClosing)
		return;

    TUsbcDeviceState deviceState;
    TInt i;
 	for (i = 0;
 		 (i < KUsbcDeviceStateRequests) && ((deviceState = dUsbc->iStatusCallbackInfo.State(i)) != EUsbcNoState);
 		 ++i)
		{
 		//__KTRACE_OPT(KUSB, Kern::Printf("StatusChangeCallBack status=%d", deviceState));
		if (deviceState & KUsbAlternateSetting)
			{
			dUsbc->ProcessAlternateSetting(deviceState);
			}
		else
			{
			dUsbc->ProcessDeviceState(deviceState);
			}
		// Only queue if userside is interested
		if (dUsbc->iDeviceStatusNeeded)
			{
			dUsbc->iStatusFifo->AddStatusToQueue(deviceState);
			const TInt reqNo = (TInt) RDevUsbcClient::ERequestAlternateDeviceStatusNotify;
			if (dUsbc->AlternateDeviceStateTestComplete())
				Kern::RequestComplete(dUsbc->iClient, dUsbc->iRequestStatus[reqNo], KErrNone);
			}
		}
 	// We don't want to be interrupted in the middle of this:
	const TInt irqs = NKern::DisableInterrupts(2);
 	dUsbc->iStatusCallbackInfo.ResetState();
	NKern::RestoreInterrupts(irqs);
	}


void DLddUsbcChannel::OtgFeatureChangeCallback(TAny* aDLddUsbcChannel)
    {
	//__KTRACE_OPT(KUSB, Kern::Printf("OtgFeatureChangeCallback"));
    DLddUsbcChannel* dUsbc = (DLddUsbcChannel*) aDLddUsbcChannel;
	if (dUsbc->iChannelClosing)
		return;

    TUint8 features;
    // No return value check. Assume OTG always supported here
    dUsbc->iController->GetCurrentOtgFeatures(features);

    const TInt reqNo = (TInt) RDevUsbcClient::ERequestOtgFeaturesNotify;
	if (dUsbc->iRequestStatus[reqNo])
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("OtgFeatureChangeCallback Notify status"));
		TInt r = Kern::ThreadRawWrite(dUsbc->iClient, dUsbc->iOtgFeatureChangePtr,
                                      &features, sizeof(TUint8), dUsbc->iClient);
		if (r != KErrNone)
			dUsbc->PanicClientThread(r);
		Kern::RequestComplete(dUsbc->iClient, dUsbc->iRequestStatus[reqNo], r);
		dUsbc->iOtgFeatureChangePtr = NULL;
		}
    }


TInt DLddUsbcChannel::SelectAlternateSetting(TUint aAlternateSetting)
	{
	TInt r = KErrGeneral;									// error code doesn't go userside
	TUsbcAlternateSettingList* alternateSettingListRec = iAlternateSettingList;
	while (alternateSettingListRec)
		{
		if (alternateSettingListRec->iSetting == aAlternateSetting)
			{
			// found the correct interface, now latch in new endpoint set
			for (TInt i = 1; i <= KMaxEndpointsPerClient; i++)
				{
				iEndpoint[i] = NULL;
				}
			iNumberOfEndpoints = alternateSettingListRec->iNumberOfEndpoints;
			r = KErrNone;
			for (TInt i = 1; i <= KMaxEndpointsPerClient; i++)
				{
				iEndpoint[i] = alternateSettingListRec->iEndpoint[i];
				}
			}
		alternateSettingListRec = alternateSettingListRec->iNext;
		}
	return r;
	}


TInt DLddUsbcChannel::EpFromAlternateSetting(TUint aAlternateSetting, TInt aEndpoint)
	{
	TUsbcAlternateSettingList* alternateSettingListRec = iAlternateSettingList;
	while (alternateSettingListRec)
		{
		if (alternateSettingListRec->iSetting == aAlternateSetting)
			{
			if ((aEndpoint <= alternateSettingListRec->iNumberOfEndpoints) &&
				(aEndpoint >= 0))
				{
				return alternateSettingListRec->iEndpoint[aEndpoint]->RealEpNumber();
				}
			else
				{
				//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: aEndpoint %d wrong for aAlternateSetting %d",
											//	  aEndpoint, aAlternateSetting));
				return -1;
				}
			}
		alternateSettingListRec = alternateSettingListRec->iNext;
		}
	//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: no aAlternateSetting %d found", aAlternateSetting));
	return -1;
	}


TInt DLddUsbcChannel::ProcessAlternateSetting(TUint aAlternateSetting)
	{
	ResetInterface(KErrUsbInterfaceChange);					// kill any outstanding transfers
	//__KTRACE_OPT(KUSB, Kern::Printf("ProcessAlternateSetting 0x%08x", aAlternateSetting));
	TUint newSetting = aAlternateSetting&(~KUsbAlternateSetting);
	//__KTRACE_OPT(KUSB, Kern::Printf("ProcessAlternateSetting selecting alternate setting 0x%08x", newSetting));
	TInt r = SelectAlternateSetting(newSetting);
	if (r != KErrNone)
		return r;
	StartEpReads();
	iAlternateSetting = newSetting;
    return KErrNone;
	}


TInt DLddUsbcChannel::ProcessDeviceState(TUsbcDeviceState aDeviceState)
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("ProcessDeviceState(%d -> %d)", iDeviceState, aDeviceState));
	if (iDeviceState == aDeviceState)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("  No state change => nothing to be done."));
		return KErrNone;
		}
	if (iDeviceState == EUsbcDeviceStateSuspended)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("  Coming out of Suspend: old state = %d", iOldDeviceState));
		iDeviceState = iOldDeviceState;
		if (iDeviceState == aDeviceState)
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("  New state same as before Suspend => nothing to be done."));
			return KErrNone;
			}
		}
	TBool renumerateState = (aDeviceState == EUsbcDeviceStateConfigured);
	TBool deconfigured = EFalse;
	TInt cancellationCode = KErrNone;
	if (aDeviceState == EUsbcDeviceStateSuspended)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("  Suspending..."));
		iOldDeviceState = iDeviceState;
		// Put PSL into low power mode here
		}
	else
		{
		deconfigured = (iDeviceState == EUsbcDeviceStateConfigured &&
						aDeviceState != EUsbcDeviceStateConfigured);
		if (iDeviceState == EUsbcDeviceStateConfigured)
			{
			if (aDeviceState == EUsbcDeviceStateUndefined)
				cancellationCode = KErrUsbCableDetached;
			else if (aDeviceState == EUsbcDeviceStateAddress)
				cancellationCode = KErrUsbDeviceNotConfigured;
			else if (aDeviceState == EUsbcDeviceStateDefault)
				cancellationCode = KErrUsbDeviceBusReset;
			else
				cancellationCode = KErrUsbDeviceNotConfigured;
			}
		}
	//__KTRACE_OPT(KUSB, Kern::Printf("  %d --> %d", iDeviceState, aDeviceState));
	iDeviceState = aDeviceState;
	if (iValidInterface || iOwnsDeviceControl)
		{
		// This LDD may not own an interface. It could be some manager reenumerating
		// after its subordinate LDDs have setup their interfaces.
		if (deconfigured)
			{
		    DeConfigure(cancellationCode);
			}
		else if (renumerateState)
			{
			// We are enumerated so let's start a read on every endpoint
			ResetInterface(KErrUsbInterfaceChange);
			// Select main interface & latch in new endpoint set
			SelectAlternateSetting(0);
			// Only after correct ifc setting has been chosen!
			UpdateEndpointSizes();
			// Here we go
			StartEpReads();
			}
		}

	const TInt reqNo = (TInt) RDevUsbcClient::ERequestReEnumerate;
	if (renumerateState && iRequestStatus[reqNo])
		{
		// This lot must be done if we are reenumerated
		Kern::RequestComplete(iClient, iRequestStatus[reqNo], KErrNone);
		}

    return KErrNone;
    }


void DLddUsbcChannel::UpdateEndpointSizes()
	{
	// First Ep0.
	iEndpoint[0]->SetMaxPacketSize(iController->Ep0PacketSize());
	// Then the regular ones.
	TInt i = 0;
	while ((++i <= KMaxEndpointsPerClient) && iEndpoint[i])
		{
		const TInt size = iController->EndpointPacketSize(this, iEndpoint[i]->RealEpNumber());
		if (size < 0)
			{
			//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: Packet size < 0 for ep %d", i));
			continue;
			}
		iEndpoint[i]->SetMaxPacketSize(size);
		}
	__ASSERT_DEBUG(i == iNumberOfEndpoints + 1,
				   Kern::Printf("  Error: iNumberOfEndpoints wrong (%d)", iNumberOfEndpoints));
	}


DPlatChunkHw* DLddUsbcChannel::ReAllocate(TInt aBuffersize, DPlatChunkHw* aHwChunk, TUint32 aCacheAttribs)
	{
	DPlatChunkHw* chunk = aHwChunk;
	if ((!chunk) || (chunk->iSize < aBuffersize))
		{
		if (chunk)
			{
			ClosePhysicalChunk(chunk);
			}
		//__KTRACE_OPT(KUSB, Kern::Printf("ReAllocate need to get new chunk"));
		chunk = Allocate(aBuffersize, aCacheAttribs);
		}
	return chunk;
	}


DPlatChunkHw* DLddUsbcChannel::Allocate(TInt aBuffersize, TUint32 aCacheAttribs)
	{
	TUint32 physAddr = 0;
	TUint32 size = Kern::RoundToPageSize(aBuffersize);

	if (Epoc::AllocPhysicalRam(size, physAddr) != KErrNone)
		return NULL;

	DPlatChunkHw* HwChunk;
	if (DPlatChunkHw::New(HwChunk, physAddr, aBuffersize, aCacheAttribs) != KErrNone)
		{
		Epoc::FreePhysicalRam(physAddr, size);
		return NULL;
		}

	return HwChunk;
	}


TInt DLddUsbcChannel::DoRxComplete(TUsbcEndpoint* aTUsbcEndpoint, TInt aEndpoint, TBool aReEntrant)
	{
	TBool completeNow;
	TInt err = aTUsbcEndpoint->CopyToClient(iClient, completeNow);
	if (completeNow)
		{
		aTUsbcEndpoint->SetClientReadPending(EFalse);
		Kern::RequestComplete(iClient,iRequestStatus[aEndpoint], err);
		}
	aTUsbcEndpoint->TryToStartRead(aReEntrant);
	return err;
	}


void DLddUsbcChannel::DoRxCompleteNow(TUsbcEndpoint* aTUsbcEndpoint, TInt aEndpoint)
	{
	aTUsbcEndpoint->SetClientReadPending(EFalse);
	Kern::RequestComplete(iClient,iRequestStatus[aEndpoint], KErrCancel);
	}


void DLddUsbcChannel::DoTxComplete(TUsbcEndpoint* aTUsbcEndpoint, TInt aEndpoint, TInt aError)
	{
	aTUsbcEndpoint->SetClientWritePending(EFalse);
	Kern::RequestComplete(iClient,iRequestStatus[aEndpoint], aError);
	}


TBool DLddUsbcChannel::AlternateDeviceStateTestComplete()
	{
	TBool completeNow = EFalse;
	const TInt reqNo = (TInt) RDevUsbcClient::ERequestAlternateDeviceStatusNotify;
	if (iRequestStatus[reqNo])
		{
		// User req is outstanding
		TUint32 deviceState;
		if (iStatusFifo->GetDeviceQueuedStatus(deviceState) == KErrNone)
			{
			// Device state waiting to be sent userside
			completeNow = ETrue;
			//__KTRACE_OPT(KUSB, Kern::Printf("StatusChangeCallback Notify status"));
			// set client descriptor length to zero
			TInt r = Kern::ThreadRawWrite(iClient, iStatusChangePtr, &deviceState,
										  sizeof(TUint32), iClient);
			if (r != KErrNone)
				PanicClientThread(r);
			iStatusChangePtr = NULL;
			}
		}
	return completeNow;
	}


void DLddUsbcChannel::EmergencyCompleteDfc(TAny* aDLddUsbcChannel)
	{
	((DLddUsbcChannel*) aDLddUsbcChannel)->DoEmergencyComplete();
	}


void DLddUsbcChannel::DeConfigure(TInt aErrorCode)
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("DLddUsbcChannel::DeConfigure()"));
	// Called after deconfiguration. Cancels transfers on all endpoints.
	ResetInterface(aErrorCode);
	// Cancel the endpoint status notify request if it is outstanding.
	const TInt KEpNotReq = RDevUsbcClient::ERequestEndpointStatusNotify;
	if (iRequestStatus[KEpNotReq])
		{
		CancelNotifyEndpointStatus();
		Kern::RequestComplete(iClient, iRequestStatus[KEpNotReq], aErrorCode);
		}
	// We have to reset the alternate setting number when the config goes away.
 	SelectAlternateSetting(0);
	iAlternateSetting = 0;
	}


void DLddUsbcChannel::StartEpReads()
	{
	// Queued after enumeration. Starts reads on all endpoints.
	// The endpoint itself decides if it can do a read
	TInt i;
	for (i = 0; i <= iNumberOfEndpoints; i++)
		{
		// The endpoint itself will decide if it can read
		iEndpoint[i]->TryToStartRead(EFalse);
		}
	}


void DLddUsbcChannel::ResetInterface(TInt aErrorCode)
	{
	// Called after change in alternate setting.  Cancels transfers on all endpoints
	if (iValidInterface || iOwnsDeviceControl)
		{
		for (TInt i = 0; i <= iNumberOfEndpoints; i++)
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("Cancelling transfer ep=%d", i));
			iEndpoint[i]->CancelTransfer(iClient);			// Copies data userside
			iEndpoint[i]->AbortTransfer();					// kills any ldd->pil outstanding transfers
			iEndpoint[i]->iDmaBuffers->Flush();
			if (iRequestStatus[i] != NULL)
				Kern::RequestComplete(iClient, iRequestStatus[i], aErrorCode);
			iEndpoint[i]->SetClientWritePending(EFalse);
			iEndpoint[i]->SetClientReadPending(EFalse);
			}
		}
	}


void DLddUsbcChannel::AbortInterface()
	{
	// Called after when channel is closing
	if (iValidInterface || iOwnsDeviceControl)
		{
		for (TInt i = 0; i <= iNumberOfEndpoints; i++)
			{
			iEndpoint[i]->AbortTransfer();					// kills any ldd->pil outstanding transfers
			}
		}
	}


void DLddUsbcChannel::ClosePhysicalChunk(DPlatChunkHw*& aHwChunk)
	{
	if (aHwChunk)
		{
 		const TPhysAddr addr = aHwChunk->PhysicalAddress();
 		const TInt size = aHwChunk->iSize;
		aHwChunk->Close(NULL);
 		Epoc::FreePhysicalRam(addr, size);
		}
	aHwChunk = NULL;
	}


TInt DLddUsbcChannel::DoEmergencyComplete()
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("TUsbcEndpoint::DoEmergencyComplete"));
	// cancel any pending DFCs
	// complete all client requests
    for (TInt i = 0; i < KUsbcMaxRequests; i++)
        {
        if (iRequestStatus[i])
            {
            //__KTRACE_OPT(KUSB, Kern::Printf("Complete request 0x%x", iRequestStatus[i]));
            Kern::RequestComplete(iClient, iRequestStatus[i], KErrDisconnected);
            }
        }
    iStatusCallbackInfo.Cancel();
    iEndpointStatusCallbackInfo.Cancel();
    iOtgFeatureCallbackInfo.Cancel();
	return KErrNone;
	}


void DLddUsbcChannel::PanicClientThread(TInt aReason)
	{
	Kern::ThreadKill(iClient, EExitPanic, aReason, KUsbLDDKillCat);
	}


// ===============Endpoint====================

// Constructor
TUsbcEndpoint::TUsbcEndpoint(DLddUsbcChannel* aLDD, DUsbClientController* aController,
							 const TUsbcEndpointInfo* aEndpointInfo, TInt aEndpointNum,
							 TInt aBandwidthPriority)
	: iController(aController),
	  iEndpointInfo(aEndpointInfo->iType, aEndpointInfo->iDir, aEndpointInfo->iSize),
	  iClientReadPending(EFalse),
	  iClientWritePending(EFalse),
	  iEndpointNumber(aEndpointNum),
	  iRealEpNumber(-1),
	  iLdd(aLDD),
	  iError(KErrNone),
	  iRequestCallbackInfo(NULL),
	  iBytesTransferred(0),
	  iBandwidthPriority(aBandwidthPriority)
	{
 	ResetTransferInfo();
	//__KTRACE_OPT(KUSB, Kern::Printf("TUsbcEndpoint::TUsbcEndpoint 2"));
	}


TInt TUsbcEndpoint::Construct()
	{
	iDmaBuffers = new TDmaBuf(&iEndpointInfo, iBandwidthPriority);
	if (iDmaBuffers == NULL)
		{
		return KErrNoMemory;
		}
	const TInt r = iDmaBuffers->Construct(&iEndpointInfo);
	if (r != KErrNone)
		{
		return r;
		}
	iRequestCallbackInfo = new TUsbcRequestCallback(iLdd,
													iEndpointNumber,
													TUsbcEndpoint::RequestCallback,
													this,
													iLdd->iDfcQ,
													KUsbRequestCallbackPriority);
	if (iRequestCallbackInfo == NULL)
		{
		return KErrNoMemory;
		}
	return KErrNone;
	}


TUsbcEndpoint::~TUsbcEndpoint()
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("TUsbcEndpoint::~TUsbcEndpoint(%d)", iEndpointNumber));
	AbortTransfer();
	delete iRequestCallbackInfo;
	delete iDmaBuffers;
	}


void TUsbcEndpoint::RequestCallback(TAny* aTUsbcEndpoint)
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("TUsbcEndpoint::RequestCallback"));
	((TUsbcEndpoint*) aTUsbcEndpoint)->EndpointComplete();
	}


void TUsbcEndpoint::SetMaxPacketSize(TInt aSize)
	{
	iEndpointInfo.iSize = aSize;
	iDmaBuffers->SetMaxPacketSize(aSize);
	}


TInt TUsbcEndpoint::EndpointComplete()
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("TUsbcEndpoint::EndpointComplete ep=%d %d",
								//	iEndpointNumber, iRequestCallbackInfo->iEndpointNum));

	if (iLdd->ChannelClosing())
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("We're going home -> completions no longer accepted"));
		return KErrNone;
		}

	TTransferDirection transferDir = iRequestCallbackInfo->iTransferDir;
	TInt error = iRequestCallbackInfo->iError;

	switch (transferDir)
		{

	case EControllerWrite:
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("TUsbcEndpoint::EndpointComplete Write 2"));
		if (!iDmaBuffers->TxIsActive())
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("  TX completion but !iDmaBuffers->TxIsActive()"));
			break;
			}

		iDmaBuffers->TxSetInActive();
		TBool completeNow = EFalse;
		iBytesTransferred += iRequestCallbackInfo->iTxBytes;
		if (iClientWritePending)
			{
			//Complete Outstanding Write if necessary
			iError = error;
			if (iError != KErrNone)
				{
				completeNow = ETrue;
				if (iError == KErrPrematureEnd)				// Previous write could not be completed
					iError = KErrNone;
				}
			else
				{
				if (iBytesTransferred == (TUint32) iTransferInfo.iTransferSize)
					{
					completeNow = ETrue;
					}
				else
					{
					iError = ContinueWrite();
					if (iError != KErrNone)
						completeNow = ETrue;
					}
				}
			if (completeNow)
				{
				TxComplete();
				ResetTransferInfo();
				if (iEndpointNumber == 0)
					{
					iDmaBuffers->Flush();
					TryToStartRead(EFalse);
					}
				}
			}
		break;
		}

	case EControllerRead:
		{
		// The first packet always contains the total #of bytes
		const TInt byteCount = iRequestCallbackInfo->iPacketSize[0];
		const TInt packetCount = iRequestCallbackInfo->iRxPackets;
		iDmaBuffers->ReadXferComplete(byteCount, packetCount, error);

		// We queue the dfc if we can complete the read, i.e. if we are reading a packet,
		// or if we have enough data to satisfy a read data request.
		if (iClientReadPending)
			{
			//Complete outstanding read
			//__KTRACE_OPT(KUSB, Kern::Printf("TUsbcEndpoint::EndpointComplete Read 3 (bytes "
										//	"available=%d)", iDmaBuffers->RxBytesAvailable()));
			TInt bytesReqd = iTransferInfo.iTransferSize - iBytesTransferred;
			TBool completeNow = EFalse;

			if (iTransferInfo.iTransferType == ETransferTypeReadPacket ||
				iTransferInfo.iTransferType == ETransferTypeReadOneOrMore)
				{
				// Always complete on a packet read
				completeNow = ETrue;
				}
			else if (iTransferInfo.iTransferType == ETransferTypeReadData)
				{
				// Complete only if enough data is present
				if (iDmaBuffers->RxBytesAvailable() >= bytesReqd)
					completeNow = ETrue;
				}
			else if (iTransferInfo.iTransferType == ETransferTypeReadUntilShort)
				{
				// Complete if enough data is present or if a short packet has been delivered
				const TInt maxPacketSize = iEndpointInfo.iSize;
				const TInt lastPacketSize = iRequestCallbackInfo->iPacketSize[packetCount - 1];
				if (lastPacketSize < maxPacketSize)
					completeNow = ETrue;
				else if (iDmaBuffers->RxBytesAvailable() >= bytesReqd)
					completeNow = ETrue;
				else
					{
					const TUint type = iEndpointInfo.iType;
					if ((type == KUsbEpTypeBulk) && (lastPacketSize & (maxPacketSize - 1)))
						{
						completeNow = ETrue;
						}
					else if ((type != KUsbEpTypeBulk) &&
							 (lastPacketSize > maxPacketSize) &&
							 (lastPacketSize % maxPacketSize))
						{
						completeNow = ETrue;
						}
					}
				}
			if (completeNow)
				{
				iError = error;
				RxComplete(EFalse);
				iClientReadPending = EFalse;
				}
			}
		iDmaBuffers->RxSetInActive();
		if (error != KErrNone)
			{
			return error;
			}
		if (TryToStartRead(EFalse) != KErrNone)
			{
//			if (iEndpointNumber != 0)
//				Kern::Printf("EndpointComplete couldn't start read on ep=%d", iEndpointNumber);
			}
		break;
		}

	default:
		// shouldn't get here
		break;
		}

	return KErrNone;
	}


void TUsbcEndpoint::TxComplete()
	{
	iLdd->DoTxComplete(this, iEndpointNumber, iError);
	}


TInt TUsbcEndpoint::RxComplete(TBool aReEntrant)
	{
	return iLdd->DoRxComplete(this, iEndpointNumber, aReEntrant);
	}


void TUsbcEndpoint::RxCompleteNow()
	{
	iLdd->DoRxCompleteNow(this, iEndpointNumber);
	}


TInt TUsbcEndpoint::CopyToClient(DThread* aClient)
	{
	TBool completeNow;
	return CopyToClient(aClient, completeNow);
	}


TInt TUsbcEndpoint::CopyToClient(DThread* aClient, TBool& aCompleteNow)
	{
	TInt err;
	TAny* des = iTransferInfo.iDes;
	const TInt length = iTransferInfo.iTransferSize;
	const TBool KReadData = EFalse;
	const TBool KReadUntilShort = ETrue;

	if (iTransferInfo.iTransferType == ETransferTypeReadPacket)
		{
		err = iDmaBuffers->RxCopyPacketToClient(aClient, des, length);
		aCompleteNow = ETrue;
		}
	else if (iTransferInfo.iTransferType == ETransferTypeReadOneOrMore)
		{
		err = iDmaBuffers->RxCopyDataToClient(aClient, des, length, iBytesTransferred,
											  KReadData, aCompleteNow);
		aCompleteNow = ETrue;
		}
	else if (iTransferInfo.iTransferType == ETransferTypeReadUntilShort)
		{
		err = iDmaBuffers->RxCopyDataToClient(aClient, des, length, iBytesTransferred,
											  KReadUntilShort, aCompleteNow);
		}
	else
		{
		err = iDmaBuffers->RxCopyDataToClient(aClient, des, length, iBytesTransferred,
											  KReadData, aCompleteNow);
		}

	if (aCompleteNow)
		{
		ResetTransferInfo();
		SetClientReadPending(EFalse);
		}

	return err;
	}


TInt TUsbcEndpoint::TryToStartRead(TBool aReEntrant)
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("TryToStartRead 1 ep=%d", iEndpointNumber));
	TInt r = KErrNone;
	if (iEndpointInfo.iDir != KUsbEpDirOut &&
		iEndpointInfo.iDir != KUsbEpDirBidirect)
		{
		// Verify ep direction
		//__KTRACE_OPT(KUSB, Kern::Printf("TryToStartRead wrong direction ep=%d", iEndpointNumber));
		return KErrUsbEpBadDirection;
		}

	if (iEndpointNumber == 0)
		{
		// Can't issue an Ep0 read if reader or writer is active
		if (iDmaBuffers->TxIsActive())
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("TryToStartRead ep0 Tx already active FATAL"));
			return KErrUsbEpNotReady;
			}
		if (iDmaBuffers->RxIsActive())
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("TryToStartRead ep0 Rx already active non-FATAL"));
			}
		}

	if (!(iDmaBuffers->RxIsActive()))
		{
		TUint8* bufferAddr;
		TPhysAddr physAddr;
		TUsbcPacketArray* indexArray;
		TUsbcPacketArray* sizeArray;
		TInt length;
		r = iDmaBuffers->RxGetNextXfer(bufferAddr, indexArray, sizeArray, length, physAddr);
		if (r == KErrNone)
			{
			iDmaBuffers->RxSetActive();
			iRequestCallbackInfo->SetRxBufferInfo(bufferAddr, physAddr, indexArray, sizeArray, length);

			//__KTRACE_OPT(KUSB, Kern::Printf("TryToStartRead 2 bufferAddr=0x%08x", bufferAddr));

			r = iController->SetupReadBuffer(*iRequestCallbackInfo);
			if (r != KErrNone)
				{
				iDmaBuffers->RxSetInActive();
				//__KTRACE_OPT(KPANIC, Kern::Printf("  Error: TryToStartRead controller rejects read"));
				}
			}
		else
			{
			if (iClientReadPending)
				{
				// Deadlock, try to resolve it by draining buffer into descriptor
				if (!aReEntrant)
					{
					RxComplete(ETrue);
					}
				else
					{
					// we are stuck, better complete userside otherwise the userside request will hang
					RxCompleteNow();
					}
				}
			}
		}
	return r;
	}


TInt TUsbcEndpoint::TryToStartWrite(TEndpointTransferInfo* pTfr)
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("TryToStartWrite 1 ep=%d", iEndpointNumber));
	if (iEndpointInfo.iDir != KUsbEpDirIn &&
		iEndpointInfo.iDir != KUsbEpDirBidirect)
		{
		// Verify ep direction
		return KErrUsbEpBadDirection;
		}
	if (iEndpointNumber == 0)
		{
		// Can't issue an Ep0 write if unread data is available or writer is active
		if (iDmaBuffers->TxIsActive() || !iDmaBuffers->IsReaderEmpty())
			{
			return KErrUsbEpNotReady;
			}
		if (iDmaBuffers->RxIsActive())
			{
			// if a reader is active then cancel the read
			iDmaBuffers->RxSetInActive();
			iController->CancelReadBuffer(iLdd, iRealEpNumber);
			}
		}
	SetTransferInfo(pTfr);
	ContinueWrite();
	return KErrNone;
	}


TInt TUsbcEndpoint::ContinueWrite()
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("ContinueWrite 2"));
	TUint8* bufferAddr;
	TPhysAddr physAddr;
	TInt bufferLength;
	TInt r = iDmaBuffers->TxGetNextXfer(bufferAddr, bufferLength, physAddr);
	if (r != KErrNone)											// probably already active
		return r;
	//__KTRACE_OPT(KUSB, Kern::Printf("ContinueWrite 3"));
	iDmaBuffers->TxSetActive();
	TBool zlpReqd = EFalse;
	TUint32 transferSize = iTransferInfo.iTransferSize;
	TAny* logicalSrc = iTransferInfo.iDes;
	TInt length = Min(transferSize - iBytesTransferred, (TUint32) bufferLength);
	if (iBytesTransferred+length>=transferSize)
		{
		// only send a zlp if this is the last buffer of the transfer
		zlpReqd = iTransferInfo.iZlpReqd;
		}
	r = iDmaBuffers->TxStoreData(iLdd->Client(), logicalSrc, length, iBytesTransferred);
	iDmaBuffers->TxSetActive();
	iRequestCallbackInfo->SetTxBufferInfo(bufferAddr, physAddr, length);
	iRequestCallbackInfo->iZlpReqd = zlpReqd;
#if 0
	for (TInt i = 0; i < iRequestCallbackInfo->iLength; i++)
		{
		//__KTRACE_OPT(KUSB, Kern::Printf("Buffer[%d] = 0x%02x", i, iRequestCallbackInfo->iBufferStart[i]));
		}
#endif
	r = iController->SetupWriteBuffer(*iRequestCallbackInfo);
	return r;
	}


void TUsbcEndpoint::CancelTransfer(DThread* aThread)
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("CancelTransfer"));
	if (iDmaBuffers != NULL)
		{
		if (iClientWritePending)
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("  (iClientWritePending)"));
			iClientWritePending = EFalse;
			iController->CancelWriteBuffer(iLdd, iRealEpNumber);
			iDmaBuffers->TxSetInActive();
			}
		if (iClientReadPending)
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("  (iClientReadPending)"));
			iClientReadPending = EFalse;
			CopyToClient(aThread);
			}
		}
	}


void TUsbcEndpoint::AbortTransfer()
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("Abort Transfer"));
	if (iDmaBuffers != NULL)
		{
		if (iDmaBuffers->TxIsActive())
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("  (iClientWritePending)"));
			iController->CancelWriteBuffer(iLdd, iRealEpNumber);
			iDmaBuffers->TxSetInActive();
			}
		if (iDmaBuffers->RxIsActive())
			{
			//__KTRACE_OPT(KUSB, Kern::Printf("  (iClientReadPending)"));
			iController->CancelReadBuffer(iLdd, iRealEpNumber);
			iDmaBuffers->RxSetInActive();
			}
		iRequestCallbackInfo->iDfc.Cancel();
		}
	}


TUsbcClientCallback::TUsbcClientCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority)
	: iOwner(aOwner), iDfc(aCallback, aOwner, aPriority)
	{
	}


TUsbcRequestCallback::TUsbcRequestCallback(const DBase* aOwner, TInt aEndpointNum, TDfcFn aDfcFunc,
										   TUsbcEndpoint* aEndpoint, TDfcQue* aDfcQ, TInt aPriority)
	: iEndpointNum(aEndpointNum),
	  iRealEpNum(-1),
	  iOwner(aOwner),
	  iDfc(aDfcFunc, aEndpoint, aDfcQ, aPriority),
	  iTransferDir(EControllerNone),
	  iBufferStart(NULL),
	  iPacketIndex(NULL),									// actually TUint16 (*)[]
	  iPacketSize(NULL),									// actually TUint16 (*)[]
	  iLength(0),
	  iZlpReqd(EFalse),
	  iTxBytes(0),
	  iRxPackets(0),
	  iError(KErrNone)
	{
	}


TUsbcRequestCallback::~TUsbcRequestCallback()
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("TUsbcRequestCallback::~TUsbcRequestCallback()"));
	iDfc.Cancel();
	}


TUsbcStatusCallback::TUsbcStatusCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority)
	: iOwner(aOwner), iDfc(aCallback, aOwner, aPriority)
	{
 	ResetState();
	}


TUsbcEndpointStatusCallback::TUsbcEndpointStatusCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority)
	: iOwner(aOwner),
	  iDfc(aCallback, aOwner, aPriority),
	  iState(0)
	{
	}


TUsbcOtgFeatureCallback::TUsbcOtgFeatureCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority)
	: iOwner(aOwner),
	  iDfc(aCallback, aOwner, aPriority),
	  iValue(0)
	{
	}


TUsbcAlternateSettingList::TUsbcAlternateSettingList()
	: iNext(NULL),
	  iNumberOfEndpoints(0),
	  iSetting(0)
	{
	for (TInt i = 0; i <= KMaxEndpointsPerClient; i++)
		{
		iEndpoint[i] = NULL;
		}
	}


TUsbcAlternateSettingList::~TUsbcAlternateSettingList()
	{
	//__KTRACE_OPT(KUSB, Kern::Printf("TUsbcAlternateSettingList::~TUsbcAlternateSettingList()"));
	for (TInt i = 0; i <= KMaxEndpointsPerClient; i++)
		{
		delete iEndpoint[i];
		}
	}


TUsbcDeviceStatusQueue::TUsbcDeviceStatusQueue()
	{
	FlushQueue();
	}


void TUsbcDeviceStatusQueue::FlushQueue()
	{
	for (TInt i = 0; i < KUsbDeviceStatusQueueDepth; i++)
		{
		iDeviceStatusQueue[i] = KUsbDeviceStatusNull;
		}
	iStatusQueueHead = 0;
	}


void TUsbcDeviceStatusQueue::AddStatusToQueue(TUint32 aDeviceStatus)
	{
	// Only add a new status if it is not a duplicate of the one at the head of the queue
	if (!(iStatusQueueHead != 0 &&
		  iDeviceStatusQueue[iStatusQueueHead - 1] == aDeviceStatus))
		{
		if (iStatusQueueHead == KUsbDeviceStatusQueueDepth)
			{
			// Discard item at tail of queue
			TUint32 status;
			GetDeviceQueuedStatus(status);
			}
		iDeviceStatusQueue[iStatusQueueHead] = aDeviceStatus;
		iStatusQueueHead++;
		}
	}


TInt TUsbcDeviceStatusQueue::GetDeviceQueuedStatus(TUint32& aDeviceStatus)
	{
	TInt r = KErrNone;
	if (iStatusQueueHead <= 0)
		{
		r = KErrGeneral;
		aDeviceStatus = KUsbDeviceStatusNull;
		}
	else
		{
		aDeviceStatus = iDeviceStatusQueue[0];
		for(TInt i = 1; i < KUsbDeviceStatusQueueDepth; i++)
			{
			TUint32 s = iDeviceStatusQueue[i];
			iDeviceStatusQueue[i - 1] = s;
			}
		iStatusQueueHead--;
		iDeviceStatusQueue[KUsbDeviceStatusQueueDepth - 1] = KUsbDeviceStatusNull;
		}
	return r;
	}


//---
