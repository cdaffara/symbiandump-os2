// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Public interface for querying support for features on a device, and
// subscribing for notification if any features are added or removed.
// 
//

#ifndef FEATREG_H
#define FEATREG_H

/**
 * Interface for inquiring whether features are supported on the device.
 *
 * The code segment below shows an example of how the QuerySupport() API 
 * should be used in client code that checks feature status. 
 *   
 * @code
 *  
 *  // Single query 
 *  // 
 *  TBool haveUsb = RFeatureRegistry::QuerySupportS(NFeature::KUsb) > 0;
 *  if (haveUsb)
 *    { // ... connect to USB service ...
 *    }  
 *
 *  // or Multiple queries
 *  // 
 *  RFeatureRegistry featReg;
 *  const TBool opened = (featReg.Open() == KErrNone);
 *  iHaveUsb = opened && (featReg.QuerySupport(NFeature::KUsb) > 0);
 *  iHaveBluetooth = opened && (featReg.QuerySupport(NFeature::KBluetooth) > 0);
 *  featReg.Close(); // can always call Close(), even if Open() failed:
 *  // ... proceed to update application menus based on these featue flags
 *  
 * @endcode 
 *  
 * Note the QuerySupport() API can return a negative error code. Clients 
 * calling this API need to decide if this should result in specific error 
 * handling or whether ignoring the error and assuming the feature is not
 * supported is the best policy.
 * 
 * @see RFeatureRegistry::QuerySupportS      
 * @publishedPartner
 * @deprecated
 */
NONSHARABLE_CLASS(RFeatureRegistry)
	{
public:
	/**
	 * Bit assignments in status word for listed features
	 *
 	 * @internalComponent
     */
	enum
		{
		EStatusSupportBit = 1, 
		EStatusUpgradableBit = 2
		};
	inline RFeatureRegistry();
	IMPORT_C TInt Open();
	IMPORT_C TInt QuerySupport(TUid aFeatureUid);
	IMPORT_C TInt QuerySupport(TUid aFeatureUid, TUint32& aInfo);
	IMPORT_C void Close();

	IMPORT_C static TInt QuerySupportS(TUid aFeatureUid);
	IMPORT_C static TInt QuerySupportS(TUid aFeatureUid, TUint32& aInfo);

private:
	class TImpl;
	TImpl* iImpl;
	};

#ifndef SYMBIAN_FEATURE_MANAGER
/**
 * Interface for obtaining notification of changes in the Feature Registry.
 *
 * @internalComponent
 */
 
NONSHARABLE_CLASS(RFeatureRegistryNotify)
	{
public:
	inline RFeatureRegistryNotify();
	IMPORT_C TInt Open();
	IMPORT_C void Subscribe(TRequestStatus &aNotifyStatus);
	IMPORT_C void Cancel();
	IMPORT_C void Close();

private:
	class TImpl;
	TImpl* iImpl;
	};

#endif

#include <featreg.inl> 

#endif
