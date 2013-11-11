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

#ifndef FDF_H
#define FDF_H

#include <e32base.h>
#include "activewaitforbusevent.h"
#include "activewaitforecomevent.h"
#include <ecom/ecom.h>
#include "deviceproxy.h"
#include "fdcproxy.h"

class CFdfSession;
class CEventQueue;

NONSHARABLE_CLASS(CFdf) : public CBase, public MBusEventObserver, public MEComEventObserver
	{
private: // types used internally
	struct TInterfaceInfo
		{
		// This odd NewL either: leaves safely, or passes ownership of the new
		// TInterfaceInfo to aInterfaces.
		static TInterfaceInfo* NewL(RPointerArray<TInterfaceInfo>& aInterfaces);

		TUint8 iNumber;
		TUint8 iClass;
		TUint8 iSubclass;
		TUint8 iProtocol;

		TBool iClaimed;
		};
		
		
	enum TDeviceSearchKeys
		{
		EVendorProductDevice						= 0,
		EVendorProduct								= 1,
		EVendorDevicesubclassDeviceprotocol			= 2,
		EVendorDevicesubclass						= 3,
		EDeviceclassDevicesubclassDeviceprotocol 	= 4,
		EDeviceclassDevicesubclass					= 5,
		EMaxDeviceSearchKey							= 6
		};		
		
		
	enum TInterfaceSearchKeys
		{
		EVendorProductDeviceConfigurationvalueInterfacenumber	= 0,
		EVendorProductConfigurationValueInterfacenumber			= 1,
		EVendorInterfacesubclassInterfaceprotocol				= 2,
		EVendorInterfacesubclass								= 3,
		EInterfaceclassInterfacesubclassInterfaceprotocol		= 4,
		EInterfaceclassInterfacesubclass						= 5,
		EMaxInterfaceSearchKey									= 6,
		};

public:
	static CFdf* NewL();
	~CFdf();

public: // called by CFdfSession
	TBool GetDeviceEvent(TDeviceEvent& aEvent);
	TBool GetDevmonEvent(TInt& aEvent);

public: // called by CFdfServer
	void SetSession(CFdfSession* aSession);

public: // called by event queue
	CFdfSession* Session();

public: // invoked indirectly by USBMAN
	void EnableDriverLoading();
	void DisableDriverLoading();

public: // called by CFdcProxy
	TUint32 TokenForInterface(TUint8 aInterface); 
	void GetSerialNumberStringDescriptorL(TUint aDeviceId, TUint32 aLangId, TName& aString) const;

public: // called by CFdcProxy and CFdfSession
	void GetManufacturerStringDescriptorL(TUint aDeviceId, TUint32 aLangId, TName& aString) const;
	void GetProductStringDescriptorL(TUint aDeviceId, TUint32 aLangId, TName& aString) const;
	const RArray<TUint>& GetSupportedLanguagesL(TUint aDeviceId) const;
	void GetOtgDeviceDescriptorL(TInt aDeviceId, TOtgDescriptor& aDescriptor) const;
	void GetConfigurationDescriptorL(TInt aDeviceId, TUsbConfigurationDescriptor& aDescriptor) const;	
	
private:
	CFdf();
	void ConstructL();

private: 
	void MbeoBusEvent();		// from MBusEventObserver
	void EComEventReceived();	// from MEComEventObserver
	void HandleEComEventReceivedL();
	
private: // utility
	void CreateFunctionDriverProxiesL();
	CDeviceProxy* DeviceProxyL(TUint aDeviceId) const;
	void TellFdcsOfDeviceDetachment(TUint aDeviceId);
	void ParseL(TUsbGenericDescriptor& aDesc);

	// Top-level handlers of MBusEventObserver events (calls to MbeoBusEvent)
	void HandleDeviceAttachment(TUint aDeviceId);
	void HandleDeviceDetachment(TUint aDeviceId);
	void HandleDevmonEvent(TInt aEvent);

	// Second-level handler of device attachment. Performs the first step:
	// *actual* device attachment at the FDF level.
	void HandleDeviceAttachmentL(TUint aDeviceId, CDeviceProxy*& aDevice);

	// Second-level handler of device attachment. Performs the second step:
	// driver loading.
	void DoDriverLoading(CDeviceProxy& aDevice);
	void DoDriverLoadingL(CDeviceProxy& aDevice);

	// Utilities for driver loading.

	void FindDriversForInterfacesUsingSpecificKeyL(CDeviceProxy& aDevice,
													TInt& aCollectedErr,
													TBool& aAnySuccess,			
													RArray<TUint>& aInterfacesNumberArray, 
													TInterfaceSearchKeys aKey);											

	void FindDriverForInterfaceUsingSpecificKey(CDeviceProxy& aDevice,
						TInt& aCollectedErr,
						TBool& aAnySuccess,
						RArray<TUint>& aInterfacesGivenToFdc,
						const TDesC8& aSearchKey);
	TBool SearchForADeviceFunctionDriverL(CDeviceProxy& aDevice, TBool& aAnySuccess, TInt& aCollectedErr);
	void SearchForInterfaceFunctionDriversL(CDeviceProxy& aDevice, TBool& aAnySuccess, TInt& aCollectedErr);					
	void FormatDeviceSearchKey(TDes8& aSearchKey, TDeviceSearchKeys aSearchKeys);
	void FormatInterfaceSearchKey(TDes8& aSearchKey, TInterfaceSearchKeys aSearchKeys,  const TInterfaceInfo& aIfInfo);
	TUint UnclaimedInterfaceCount() const;
	void AppendInterfaceNumberToArrayL(CDeviceProxy& aDevice, RArray<TUint>& aArray, TUint aInterfaceNo) const;
	void SetFailureStatus(TInt aUnclaimedInterfaces, TInt aInterfaceCount, TBool aAnySuccess, TBool aCollectedErr, CDeviceProxy& aDevice);
	TBool FindMultipleFDs(const TDesC8& aSearchKey,TSglQueIter<CFdcProxy>& aFdcIter);
	void  RebuildUnClaimedInterfacesArrayL(CDeviceProxy& aDevice, RArray<TUint>& aArray, TUint aOffset = 0);
private: // unowned
	CFdfSession* iSession;

private: // owned
	TUsbDeviceDescriptor 			iDD;
	TUsbConfigurationDescriptor 	iCD;
	TBool 							iDriverLoadingEnabled;
	CEventQueue* 					iEventQueue;

	TBool 							iDeviceDetachedTooEarly;

	RImplInfoPtrArray 				iImplInfoArray;

	// Our session on (and data from) the USBDI thunk.
	RUsbHubDriver 					iHubDriver;
	CActiveWaitForBusEvent* 		iActiveWaitForBusEvent;
	CActiveWaitForEComEvent*		iActiveWaitForEComEvent;
	RUsbHubDriver::TBusEvent 		iBusEvent;

	// Our collections of attached devices and of Function Driver Controllers.
	TSglQue<CDeviceProxy> 			iDevices;
	TSglQue<CFdcProxy> 				iFunctionDrivers;

	// Used while offering interfaces to FDCs.
	RPointerArray<TInterfaceInfo>	iInterfaces; 	
	CDeviceProxy* 					iCurrentDevice;	// iCurrentDevice is not owned by CFdf
	};

#endif // FDF_H
