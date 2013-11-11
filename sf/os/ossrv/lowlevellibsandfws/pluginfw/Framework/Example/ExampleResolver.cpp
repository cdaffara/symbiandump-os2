// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements a non-default resolver CExampleResolver.
// 
//

/**
 @internalComponent
*/

#include <ecom/ecom.h>

#include "ExampleResolver.h"
#include "RegistryData.h"

CExampleResolver* CExampleResolver::NewL(MPublicRegistry& aRegistry)
	{
	return new(ELeave) CExampleResolver(aRegistry);
	}

CExampleResolver::~CExampleResolver()
	{
	if(iImplementationInfoArray)
		{
		iImplementationInfoArray->Reset();
		delete iImplementationInfoArray;
		}
	}

CExampleResolver::CExampleResolver(MPublicRegistry& aRegistry)
: CResolver(aRegistry)
	{
	// Do nothing here
	}

TUid CExampleResolver::IdentifyImplementationL(TUid aInterfaceUid, 
											   const TEComResolverParams& aAdditionalParameters) const
	{
	RImplInfoArray& implementationsInfo = iRegistry.ListImplementationsL(aInterfaceUid);
	TUid found = KNullUid;
	if(implementationsInfo.Count())
		{
		found = Resolve(implementationsInfo, aAdditionalParameters);
		}
	return found;
	}

TUid CExampleResolver::Resolve(const RImplInfoArray& aImplementationsInfo, 
							   const TEComResolverParams& aAdditionalParameters) const
	{
	// Loop through the implementations matching on type
	const TInt count = aImplementationsInfo.Count();
	for(TInt index = 0; index < count; ++index)
		{
		const CImplementationInformation& impData = *aImplementationsInfo[index];
		// As soon as we get a match on the datatype then return uid of the 
		// implementation found.
		if (Match(impData.DataType(),						// The Datatype of this implementation
				  aAdditionalParameters.DataType(),			// The type we are trying to find
				  aAdditionalParameters.IsGenericMatch()))	// If wildcards should be used
			return impData.ImplementationUid();
		}

	return KNullUid;
	}

RImplInfoArray* CExampleResolver::ListAllL(TUid aInterfaceUid, 
										   const TEComResolverParams& aAdditionalParameters) const
	{
	//Simulation for INC054125 - ECOM cleanup stack error
	_LIT8(KCriteriaData, "INC054125");
	const TDesC8& dataType = aAdditionalParameters.DataType();
	if(aInterfaceUid.iUid == 0x10009DC8 && dataType.Compare(KCriteriaData) == 0)
		{
		User::Leave(KErrGeneral);
		}
	// Use the member var to create the array so that we get proper cleanup behaviour
	iImplementationInfoArray = new(ELeave) RImplInfoArray;
	RImplInfoArray* retList = iImplementationInfoArray;

	RImplInfoArray& fullList = iRegistry.ListImplementationsL(aInterfaceUid);

	const TBool useWildcards = aAdditionalParameters.IsGenericMatch();
	const TDesC8& matchType = aAdditionalParameters.DataType();
	const TInt numImps = fullList.Count();
	for(TInt index = 0; index < numImps; ++index)
		{
		if(Match(fullList[index]->DataType(), matchType, useWildcards))
			{
			User::LeaveIfError(retList->Append(fullList[index]));
			}
		}

	// Reset the member variable because we are passing ownership back
	iImplementationInfoArray = NULL;
	return retList;
	}

TBool CExampleResolver::Match(const TDesC8& aImplementationType, 
							  const TDesC8& aMatchType, 
							  TBool aUseWildcards) const
	{
	TInt matchPos = KErrNotFound;

	_LIT8(dataSeparator, "||");
	const TInt separatorLength = dataSeparator().Length();

	// Look for the section separator marker '||'
	TInt separatorPos = aImplementationType.Find(dataSeparator);
	if(separatorPos == KErrNotFound)
		{
		// Match against the whole string
		if(aUseWildcards)
			matchPos = aMatchType.Match(aImplementationType);
		else
			matchPos = aMatchType.Compare(aImplementationType);
		}
	else
		{
		// Find the first section, up to the separator
		TPtrC8 dataSection = aImplementationType.Left(separatorPos);
		TPtrC8 remainingData = aImplementationType.Mid(separatorPos + separatorLength);
		// Match against each section in turn
		while(separatorPos != KErrNotFound)
			{
			// Search this section
			if(aUseWildcards)
				matchPos = aMatchType.Match(dataSection);
			else
				matchPos = aMatchType.Compare(dataSection);

			// If we found it then no need to continue, so return
			if(matchPos != KErrNotFound)
				return ETrue;

			// Move on to the next section
			separatorPos = remainingData.Find(dataSeparator);
			if(separatorPos != KErrNotFound)
				{
				dataSection.Set(remainingData.Left(separatorPos));
				remainingData.Set(remainingData.Mid(separatorPos + separatorLength));
				}
			else
				dataSection.Set(remainingData);
			}

		// Check the final part
		if(aUseWildcards)
			matchPos = aMatchType.Match(dataSection);
		else
			matchPos = aMatchType.Compare(dataSection);

		}
	return matchPos != KErrNotFound;
	}
