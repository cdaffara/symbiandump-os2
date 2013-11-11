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
// This file contains the implementation of the CRegistryResolveTransaction class.
// 
//

#include "RegistryData.h"
#include "RegistryResolveTransaction.h"
#include "EComMessageIds.h"
//
// CRegistryResolveTransaction class
/**
Constructor for CRegistryResolveTransaction
@param 			aRegistryData Registry data handle.
@param			aExtendedInterfaces The array of extended interfaces for match.
@param			aClientRequest Client's request
@param			aCapability The boolean value for checking the incoming client's capability
*/
CRegistryResolveTransaction::CRegistryResolveTransaction(CRegistryData &aRegistryData,
														 const RExtendedInterfacesArray& aExtendedInterfaces, 
														 const TClientRequest& aClientRequest,TBool aCapability):
	iRegistryData(aRegistryData),
	iExtendedInterfaces(&aExtendedInterfaces),
	iClientRequest(&aClientRequest),
	iCapability(aCapability)
	{
	// Do nothing here
	}

/**
Standardized safe construction which leaves nothing on the cleanup stack.
@param 			aRegistryData Registry data handle.
@param			aExtendedInterfaces The array of extended interfaces for match.
@param			aClientRequest Client's request
@param			aCapability The boolean value for checking the incoming client's capability
@return			The newly created instance.
*/
CRegistryResolveTransaction* CRegistryResolveTransaction::NewL(CRegistryData &aRegistryData,
																const RExtendedInterfacesArray& aExtendedInterfaces, 
														        const TClientRequest& aClientRequest,TBool aCapability)
	{
	CRegistryResolveTransaction* self=new(ELeave) CRegistryResolveTransaction(aRegistryData,aExtendedInterfaces,aClientRequest,aCapability);
	return self;
	}

CRegistryResolveTransaction::~CRegistryResolveTransaction()
	{
	iImplementationInfo.Close();
	}

/**
Returns an array of implementations which satisfy the specified interface, extended interface(s) and 
pass the capability check.
@param			aInterfaceUid The Uid of the interface which the implementations should provide
@return			Array of implementations which satisfy the specified interface
*/
RImplInfoArray& CRegistryResolveTransaction::ListImplementationsL(TUid aInterfaceUid) const
	{
	__ASSERT_DEBUG(iExtendedInterfaces!=NULL, User::Invariant());
	__ASSERT_DEBUG(iClientRequest!=NULL, User::Invariant());
	iImplementationInfo.Reset();
	
	CRegistryData::RImplDataArray implDataList;
	CleanupClosePushL(implDataList);
	iRegistryData.ListImplementationsL(aInterfaceUid,implDataList);
	

	// Do capability check and extended interfaces check
	for (TInt j = 0; j < implDataList.Count(); j++)
		{
		CRegistryData::CImplementationData *currentImplementation = implDataList[j];
		CImplementationInformation* implInfo = currentImplementation->iImplInfo;
		CRegistryData::CDllData *dll = currentImplementation->iParent->iParent;
		
		if(!(iCapability))
			{
			ListImplementationsL(implInfo,dll);
			}
		else
			{
			// Check the capability of the calling client to see
			// if it has the capabilities to view the implementation in the
			// dll in which the implementation resides.
			if(iClientRequest->CheckCapability(dll->iCapSet,*(implInfo)))
				{
				ListImplementationsL(implInfo,dll);			
				}
			}	
  		
		
		}
	CleanupStack::PopAndDestroy(&implDataList);	
	return iImplementationInfo;
	}
	
/**
Does the extended interface checks
@param			aImplInfo pointer to the CImplementationInformation class 
@param			aDll pointer to the CDllData class

*/
void CRegistryResolveTransaction::ListImplementationsL(CImplementationInformation* aImplInfo,CRegistryData::CDllData *aDll) const
	{
	TBool passExtendedInterfaceCheck = ETrue;
	// Filter on the extended interfaces.
	RExtendedInterfacesArray* extendedInterfacesOfThisImp = aImplInfo->GetExtendedInterfaceList();
	// For each extended interface to filter on, see if the extended
	// interface exists in this implementation.
	if (iExtendedInterfaces->Count() > 0 && extendedInterfacesOfThisImp == NULL)
		{
		passExtendedInterfaceCheck = EFalse;
		}
	else
		{
		for (TInt i=0;i<iExtendedInterfaces->Count();i++)
			{
			if (extendedInterfacesOfThisImp->Find((*iExtendedInterfaces)[i]) == KErrNotFound)
				{
				passExtendedInterfaceCheck = EFalse;
				break;								
				}
			}
		}
	if (passExtendedInterfaceCheck)
		{
		aImplInfo->SetVendorId(aDll->iVid);
		// Append the capability check passed found implementation
		User::LeaveIfError(iImplementationInfo.Append(aImplInfo));	
		}
	}
	
