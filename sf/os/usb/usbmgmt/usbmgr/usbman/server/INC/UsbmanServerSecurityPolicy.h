/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* USBMAN Server Security Policy definitions for Platform security.
*
*/

/**
 @file 
 @internalComponent
*/
 
#if !defined(__USBMANSERVERSECURITYPOLICY_H__)
#define __USBMANSERVERSECURITYPOLICY_H__

#include <usb/usbshared.h>
#include "rusb.h"

/** Usbman Server Security Policy Definition */
const TUint KUsbmanServerRangeCount = 13;

const TInt KUsbmanServerRanges[KUsbmanServerRangeCount] = 
	{
	EUsbStart,                 			/** NetworkControl 		*/
	EUsbRegisterObserver,       		/** none 				*/
	EUsbStartCancel,            		/** NetworkControl 		*/
	EUsbCancelObserver,        			/** none 				*/
	EUsbStopCancel,            			/** NetworkControl 		*/
	EUsbGetCurrentDeviceState, 			/** none 				*/
	EUsbTryStart,              			/** NetworkControl 		*/
	EUsbCancelInterest,        			/** none 				*/
	EUsbSetCtlSessionMode,	   			/** NetworkControl 		*/
	EUsbRegisterMessageObserver, 		/** none 				*/
	EUsbEnableFunctionDriverLoading,	/** NetworkControl 		*/
	EUsbGetSupportedLanguages, 			/** none 				*/
	EUsbGetPersonalityProperty + 1	 				/** fail (to KMaxTInt) 	*/
	};

/** Index numbers into KUsbmanServerElements[] */
const TInt KPolicyNetworkControl = 0;
const TInt KPolicyPass = 1;

/**Mapping IPCs to policy element */
const TUint8 KUsbmanServerElementsIndex[KUsbmanServerRangeCount] = 
	{
	KPolicyNetworkControl, 	/** EUsbStart */
				 			/** EUsbStop  */

	KPolicyPass, 		 	/** EUsbRegisterObserver */
				 			/** EUsbGetCurrentState */

	KPolicyNetworkControl, 	/** EUsbStartCancel */

	KPolicyPass,		 	/** EUsbCancelObserver */

	KPolicyNetworkControl, 	/** EUsbStopCancel */
	KPolicyPass, 		 	/** EUsbGetCurrentDeviceState */
				 			/** EUsbRegisterServiceObserver */
		                 	/** EUsbCancelServiceObserver */
		           	 		/** EUsbDbgMarkHeap */
				 			/** EUsbDbgCheckHeap */
				 			/** EUsbDbgMarkEnd */
				 			/** EUsbDbgFailNext */

	KPolicyNetworkControl,	/** EUsbTryStart */
				 			/** EUsbTryStop */

	KPolicyPass,		 	/** EUsbCancelInterest */
					 		/** EUsbGetCurrentPersonalityId */
					 		/** EUsbGetSupportedClasses */
					 		/** EUsbGetPersonalityIds */
					 		/** EUsbGetDescription */
					 		/** EUsbClassSupported */

	KPolicyNetworkControl,	/** EUsbSetCtlSessionMode */
							/** EUsbBusRequest */
				 			/** EUsbBusRespondSrp */

				 			/** EUsbBusClearError */

				 			/** EUsbBusDrop */

	KPolicyPass,		 	/** EUsbRegisterMessageObserver */
					 		/** EUsbCancelMessageObserver */
					 		/** EUsbRegisterHostObserver */
					 		/** EUsbCancelHostObserver */

	KPolicyNetworkControl,	/** EUsbEnableFunctionDriverLoading */
				 			/** EUsbDisableFunctionDriverLoading */

	KPolicyPass,		 	/** EUsbGetSupportedLanguages */
					 		/** EUsbGetManufacturerStringDescriptor */
					 		/** EUsbGetProductStringDescriptor */
                            /** EUsbGetOtgDescriptor */
				 			/** EUsbDbgAlloc */
							/** EUsbRequestSession */
							/** EUsbGetPersonalityProperty */

	CPolicyServer::ENotSupported, /** EUsbGetPersonalityProperty + 1 to KMaxTInt */

	};

/** Individual policy elements */
const CPolicyServer::TPolicyElement KUsbmanServerElements[] = 
	{
		/** the EFailClient means that the if the check fails the CheckFailed method with return KErrPermissionDenied */
  		{ _INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl), CPolicyServer::EFailClient },
  		{ _INIT_SECURITY_POLICY_PASS },
	};

/** Main policy */
const CPolicyServer::TPolicy KUsbmanServerPolicy = 
	{
	CPolicyServer::EAlwaysPass, /** Specifies all connect attempts should pass */
	KUsbmanServerRangeCount,
	KUsbmanServerRanges,
	KUsbmanServerElementsIndex,
	KUsbmanServerElements,
	};
#endif //__USBMANSERVERSECURITYPOLICY_H__
