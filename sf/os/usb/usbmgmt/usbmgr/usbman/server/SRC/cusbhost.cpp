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

#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1
#include <usb/d32usbdescriptors.h>
#else
#include <d32usbdescriptors.h>
#endif
#include <usb/usblogger.h>
#include "cusbhost.h"
#include "CUsbServer.h"
#include "CUsbDevice.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbhostTraces.h"
#endif


CUsbHost* CUsbHost::iInstance = 0;

CUsbHost* CUsbHost::NewL(CUsbServer& aServer)
	{
	if(iInstance == 0)
		{
		iInstance = new (ELeave) CUsbHost(aServer);		
		CleanupStack::PushL(iInstance);		
		iInstance->ConstructL();		
		CleanupStack::Pop(iInstance);
		}	
	return iInstance;
	}

CUsbHost::~CUsbHost()
	{
    OstTraceFunctionEntry0( CUSBHOST_CUSBHOST_DES_ENTRY );

	Stop();

	TInt i =0;
	for(i=0;i<ENumMonitor;i++)
		{
		delete iUsbHostWatcher[i];
		iUsbHostWatcher[i] = NULL;
		}
	iObservers.Close();
	iInstance = 0;
	OstTraceFunctionExit0( CUSBHOST_CUSBHOST_DES_EXIT );
	}

CUsbHost::CUsbHost(CUsbServer& aServer)
	: iUsbServer(aServer)
	{
    OstTraceFunctionEntry0( CUSBHOST_CUSBHOST_CONS_ENTRY );
	OstTraceFunctionExit0( CUSBHOST_CUSBHOST_CONS_EXIT );
	}

void CUsbHost::ConstructL()
	{
    OstTraceFunctionEntry0( CUSBHOST_CONSTRUCTL_ENTRY );

	iUsbHostWatcher[EHostEventMonitor] = 
			CActiveUsbHostEventWatcher::NewL(iUsbHostStack,*this,iHostEventInfo);
	iUsbHostWatcher[EHostMessageMonitor] = 
			CActiveUsbHostMessageWatcher::NewL(iUsbHostStack,*this,iHostMessage);
	OstTraceFunctionExit0( CUSBHOST_CONSTRUCTL_EXIT );
	}
void CUsbHost::StartL()
	{
	OstTraceFunctionEntry0( CUSBHOST_STARTL_ENTRY );

	if(!iHasBeenStarted)
		{
		TInt err = iUsbHostStack.Connect();
		if(err < 0)
		    {
            OstTrace1( TRACE_NORMAL, CUSBHOST_STARTL, "CUsbHost::StartL; iUsbHostStack.Connect() error. Leave error=%d", err );
            User::Leave(err);
		    }

		for(TInt i=0;i<ENumMonitor;i++)
			{
			iUsbHostWatcher[i]->Post();
			}
		iHasBeenStarted = ETrue;
		}
	OstTraceFunctionExit0( CUSBHOST_STARTL_EXIT );
	}

void CUsbHost::Stop()
	{
	OstTraceFunctionEntry0( CUSBHOST_STOP_ENTRY );

	TInt i=0;
	for(i=0;i<ENumMonitor;i++)
		{
		if (iUsbHostWatcher[i])
			{
			iUsbHostWatcher[i]->Cancel();
			}
		}

	iUsbHostStack.Close();

	iHasBeenStarted = EFalse;
	OstTraceFunctionExit0( CUSBHOST_STOP_EXIT );
	}

void CUsbHost::RegisterObserverL(MUsbOtgHostNotifyObserver& aObserver)
	{
	OstTraceFunctionEntry0( CUSBHOST_REGISTEROBSERVERL_ENTRY );

	iObservers.AppendL(&aObserver);
	UpdateNumOfObservers();
	OstTraceFunctionExit0( CUSBHOST_REGISTEROBSERVERL_EXIT );
	}

void CUsbHost::DeregisterObserver(MUsbOtgHostNotifyObserver& aObserver)
	{
	OstTraceFunctionEntry0( CUSBHOST_DEREGISTEROBSERVER_ENTRY );
	TInt index = iObservers.Find(&aObserver);
	if(index == KErrNotFound)
		{
        OstTrace0( TRACE_NORMAL, CUSBHOST_DEREGISTEROBSERVER, "CUsbHost::DeregisterObserver;  Cannot remove observer, not found" );
		}
	else
		{
		iObservers.Remove(index);
		}

	UpdateNumOfObservers();
	OstTraceFunctionExit0( CUSBHOST_DEREGISTEROBSERVER_EXIT );
	}

TInt CUsbHost::GetSupportedLanguages(TUint aDeviceId,RArray<TUint>& aLangIds)
	{
	OstTraceFunctionEntry0( CUSBHOST_GETSUPPORTEDLANGUAGES_ENTRY );
	TInt err = KErrNone;
	if ( iUsbHostStack.Handle() )
		{
		err = iUsbHostStack.GetSupportedLanguages(aDeviceId,aLangIds);
		}
	else
		{
		err = KErrBadHandle;
		}
	OstTraceFunctionExit0( CUSBHOST_GETSUPPORTEDLANGUAGES_EXIT );
	return err;
	}

TInt CUsbHost::GetManufacturerStringDescriptor(TUint aDeviceId,TUint aLangId,TName& aString)
	{
	OstTraceFunctionEntry0( CUSBHOST_GETMANUFACTURERSTRINGDESCRIPTOR_ENTRY );
	TInt err = KErrNone;
	if ( iUsbHostStack.Handle() )
		{
		err = iUsbHostStack.GetManufacturerStringDescriptor(aDeviceId,aLangId,aString);
		}
	else
		{
		err = KErrBadHandle;
		}
	OstTraceFunctionExit0( CUSBHOST_GETMANUFACTURERSTRINGDESCRIPTOR_EXIT );
	return err;
	}

TInt CUsbHost::GetProductStringDescriptor(TUint aDeviceId,TUint aLangId,TName& aString)
	{
	OstTraceFunctionEntry0( CUSBHOST_GETPRODUCTSTRINGDESCRIPTOR_ENTRY );
	TInt err = KErrNone;
	if ( iUsbHostStack.Handle() )
		{
		err = iUsbHostStack.GetProductStringDescriptor(aDeviceId,aLangId,aString);
		}
	else
		{
		err = KErrBadHandle;
		}
	OstTraceFunctionExit0( CUSBHOST_GETPRODUCTSTRINGDESCRIPTOR_EXIT );
	return err;
	}

TInt CUsbHost::GetOtgDescriptor(TUint aDeviceId, TOtgDescriptor& otgDescriptor)
	{
	OstTraceFunctionEntry0( CUSBHOST_GETOTGDESCRIPTOR_ENTRY );
	
	TInt err(KErrNone);
	
	if (iUsbHostStack.Handle())
		{
		err = iUsbHostStack.GetOtgDescriptor(aDeviceId, otgDescriptor);
		}
	else
		{
		err = KErrBadHandle;
		}
	
	OstTraceFunctionExit0( CUSBHOST_GETOTGDESCRIPTOR_EXIT );
	return err;
	}

void CUsbHost::NotifyHostEvent(TUint aWatcherId)
	{
    OstTraceFunctionEntry0( CUSBHOST_NOTIFYHOSTEVENT_ENTRY );
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
	ProcessHostEvent(aWatcherId);
#endif

	if(aWatcherId == EHostEventMonitor)
		{
		OstTrace1( TRACE_NORMAL, CUSBHOST_NOTIFYHOSTEVENT, "CUsbHost::NotifyHostEvent;DeviceId=%d", iHostEventInfo.iDeviceId );
		OstTrace1( TRACE_NORMAL, CUSBHOST_NOTIFYHOSTEVENT_DUP1, "CUsbHost::NotifyHostEvent;iEventType=%d", iHostEventInfo.iEventType );
		OstTrace1( TRACE_NORMAL, CUSBHOST_NOTIFYHOSTEVENT_DUP2, "CUsbHost::NotifyHostEvent;TDriverLoadStatus=%d", iHostEventInfo.iDriverLoadStatus );
		OstTrace1( TRACE_NORMAL, CUSBHOST_NOTIFYHOSTEVENT_DUP3, "CUsbHost::NotifyHostEvent;VID=%d", iHostEventInfo.iVid );
		OstTrace1( TRACE_NORMAL, CUSBHOST_NOTIFYHOSTEVENT_DUP4, "CUsbHost::NotifyHostEvent;Pid=%d", iHostEventInfo.iPid );

		for(TUint i=0;i<iNumOfObservers;i++)
			{
			iObservers[i]->UsbHostEvent(iHostEventInfo);
			}
		}
	else
		{
		OstTrace1( TRACE_NORMAL, CUSBHOST_NOTIFYHOSTEVENT_DUP5, "CUsbHost::NotifyHostEvent;Host Message=%d", iHostMessage );
		
		for(TUint i=0;i<iNumOfObservers;i++)
			{
			iObservers[i]->UsbOtgHostMessage(iHostMessage);
			}
		}
	OstTraceFunctionExit0( CUSBHOST_NOTIFYHOSTEVENT_EXIT );
	}

void CUsbHost::UpdateNumOfObservers()
	{
	OstTraceFunctionEntry0( CUSBHOST_UPDATENUMOFOBSERVERS_ENTRY );
	iNumOfObservers = iObservers.Count();
	OstTraceFunctionExit0( CUSBHOST_UPDATENUMOFOBSERVERS_EXIT );
	}

TInt CUsbHost::EnableDriverLoading()
	{
	OstTraceFunctionEntry0( CUSBHOST_ENABLEDRIVERLOADING_ENTRY );
	TInt err = KErrNone;
	if ( iUsbHostStack.Handle() )
		{
		err = iUsbHostStack.EnableDriverLoading();
		}
	else
		{
		err = KErrBadHandle;
		}
	OstTraceFunctionExit0( CUSBHOST_ENABLEDRIVERLOADING_EXIT );
	return err;
	}

void CUsbHost::DisableDriverLoading()
	{
	OstTraceFunctionEntry0( CUSBHOST_DISABLEDRIVERLOADING_ENTRY );
	if ( iUsbHostStack.Handle() )
		{
		iUsbHostStack.DisableDriverLoading();
		}
	OstTraceFunctionExit0( CUSBHOST_DISABLEDRIVERLOADING_EXIT );
	}

#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1

void CUsbHost::ProcessHostEvent(TUint aWatcherId)
	{
	OstTraceFunctionEntry0( CUSBHOST_PROCESSHOSTEVENT_ENTRY );
	TConfigurationDescriptor configDesc;
	const TUint KSuspend_Current = 2; //suspend current is 2mA

	if(aWatcherId == EHostEventMonitor)
		{
		// if driver is loaded successful the device enters configed state and we get the max power from configration.
		if (iHostEventInfo.iEventType == EDriverLoad 
			&& iHostEventInfo.iDriverLoadStatus != EDriverLoadFailure)
			{
			TInt ret = iUsbHostStack.GetConfigurationDescriptor(iHostEventInfo.iDeviceId, configDesc);
			if (ret == KErrNone)
				{
        		TUint currentValue = configDesc.iMaxPower <<1;
				iUsbServer.Device().PeerDeviceMaxPower(currentValue);
				}
			}
		// if driver is loaded with failure the device enters suspend state.		
		else if (iHostEventInfo.iEventType == EDriverLoad && 
			iHostEventInfo.iDriverLoadStatus == EDriverLoadFailure)
			{
			iUsbServer.Device().PeerDeviceMaxPower(KSuspend_Current);
			}
		// if a device is unattached .		
		else if (iHostEventInfo.iEventType == EDeviceDetachment)
			{
			iUsbServer.Device().PeerDeviceMaxPower(0);
			}
		}
	OstTraceFunctionExit0( CUSBHOST_PROCESSHOSTEVENT_EXIT );
	}
#endif

