// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __REGISTRYRESOLVETRANSACTION_H__
#define __REGISTRYRESOLVETRANSACTION_H__

#include <e32base.h>
#include <ecom/implementationinformation.h>
//
// Forward declarations

class CRegistryData;

//
// CRegistryResolveTransaction class

/**
@internalComponent
This class facilitates filtering in the CRegistryDataClass. It does this by accepting 
filter parameters when the resolve transaction is created. During the course of the 
resolution these filter parameters will be passed to CRegistryData.
*/
class CRegistryResolveTransaction : public CBase, public MPublicRegistry
	{
public:
	static CRegistryResolveTransaction* NewL(CRegistryData &aRegistryData,
											 const RExtendedInterfacesArray& aExtendedInterfaces, 
											 const TClientRequest& aClientRequest, TBool aCapability);
	virtual ~CRegistryResolveTransaction();
public:   
	RImplInfoArray& ListImplementationsL(TUid aInterfaceUid) const;

private:
	CRegistryResolveTransaction(CRegistryData &aRegistryData,
								const RExtendedInterfacesArray& aExtendedInterfaces, 
								const TClientRequest& aClientRequest,TBool aCapability);
	void ListImplementationsL(CImplementationInformation* implInfo,CRegistryData::CDllData *aDll) const;
private:
	// A reference to the registry data
	CRegistryData &iRegistryData;
    // A pointer to the extended interfaces to filter on
    const RExtendedInterfacesArray* iExtendedInterfaces;
    // A pointer to the client request data for capability check
    const TClientRequest* iClientRequest;
    // Array to hold the filtered results. Must make this mutable so as to not
    // change the ListImplementationsL interface which has been published.
    mutable RImplInfoArray iImplementationInfo;
    // A boolean value for storing the client's capability request 
    // ETrue capability check is done
    // EFalse capability check is not done
    TBool iCapability; 
	};
#endif //__REGISTRYRESOLVETRANSACTION_H__
