/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
	@internalComponent
	@file
	Comments : Implements a non-default resolver CPlatSecResolver.
	
 */ 

#include <ecom/ecom.h>
#include "ImplementationProxy.h"
#include "T_SecResolver.h"
#include "RegistryData.h"

CPlatSecResolver* CPlatSecResolver::NewL(MPublicRegistry& aRegistry)
	{
	return new(ELeave) CPlatSecResolver(aRegistry);
	}

CPlatSecResolver::~CPlatSecResolver()
	{
	if(iImplementationInfoArray)
		{
		iImplementationInfoArray->Reset();
		delete iImplementationInfoArray;
		}
	}

CPlatSecResolver::CPlatSecResolver(MPublicRegistry& aRegistry)
: CResolver(aRegistry)
	{
	// Do nothing here
	}

TUid CPlatSecResolver::IdentifyImplementationL(TUid aInterfaceUid, 
											   const TEComResolverParams& aAdditionalParameters)const
											   
	{
	RImplInfoArray& implementationsInfo = iRegistry.ListImplementationsL(aInterfaceUid);
	TUid found = KNullUid;
	if(implementationsInfo.Count())
		{
		found = Resolve(implementationsInfo, aAdditionalParameters);
		}
	return found;
	}

TUid CPlatSecResolver::Resolve(const RImplInfoArray& aImplementationsInfo, 
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

RImplInfoArray* CPlatSecResolver::ListAllL(TUid aInterfaceUid, 
										   const TEComResolverParams& aAdditionalParameters) const
	{
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

TBool CPlatSecResolver::Match(const TDesC8& aImplementationType, 
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

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x102026AC, CPlatSecResolver::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

