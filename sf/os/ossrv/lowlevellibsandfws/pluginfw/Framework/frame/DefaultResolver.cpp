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
// Implements the default CDefaultResolver class.
// 
//

/**
 @file
 @internalComponent
*/

#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>

#include "TestUtilities.h"	// For __FILE__LINE__
#include "DefaultResolver.h"

CDefaultResolver* CDefaultResolver::NewL(MPublicRegistry& aRegistry)
	{
	return new(ELeave) CDefaultResolver(aRegistry);
	}

// Default d'tor

CDefaultResolver::~CDefaultResolver()
	{
	}

// Default c'tor

CDefaultResolver::CDefaultResolver(MPublicRegistry& aRegistry)
: CResolver(aRegistry)
	{
	// Do nothing here
	}


TUid CDefaultResolver::IdentifyImplementationL(TUid aInterfaceUid, 
											   const TEComResolverParams& aAdditionalParameters) const
	{
	RImplInfoArray* implementationsInfo = ListAllL(aInterfaceUid, aAdditionalParameters);
	TUid found = Resolve(*implementationsInfo, aAdditionalParameters);
	implementationsInfo->Reset();
	delete implementationsInfo;
	return found;
	}


TUid CDefaultResolver::Resolve(const RImplInfoArray& aImplementationsInfo, 
							   const TEComResolverParams& aAdditionalParameters) const
	{
	// Place to store the result if we get a wildcard match
	TUid wildMatched = KNullUid;
	TInt wildConfidence = KErrNotFound;
	const TDesC8& matchType = aAdditionalParameters.DataType();

	// Loop through the implementations matching on type
	const TInt count = aImplementationsInfo.Count();
	TBool isGenericMatch=aAdditionalParameters.IsGenericMatch();
	for(TInt index = 0; index < count; ++index)
		{
		const CImplementationInformation& impData = *aImplementationsInfo[index];
		const TDesC8& dataType = impData.DataType();

		// As soon as we get a match on the datatype then return uid of the 
		// implementation found.
		if(Match(dataType,	// The Datatype of this implementation
				 matchType,	// The type we are trying to find
				 EFalse))	// Don't use wildcards first
			{
			// We have got an exact match so return this
			return impData.ImplementationUid();
			}
		else if(isGenericMatch) // If the client wants us to use wildcards
			{
			if(Match(dataType,
					 matchType,
					 ETrue))
				{
				// We have matched using wildcards so work out a confidence value
				TInt confidence = 0;
				TInt length = Min(matchType.Length(), dataType.Length());
				while((matchType[confidence] == dataType[confidence]) && 
					  (++confidence < length))
					{
					}
				if(confidence > wildConfidence)
					{
					wildConfidence = confidence;
					wildMatched = impData.ImplementationUid();
					}
				}
			}
		}

	return wildMatched;
	}

void CloseAndDeleteRArray(TAny* aObject)
	{
	RImplInfoArray* array=reinterpret_cast<RImplInfoArray*>(aObject);
	if (array)
		array->Close();
	delete array;
	}


RImplInfoArray* CDefaultResolver::ListAllL(TUid aInterfaceUid, 
										   const TEComResolverParams& aAdditionalParameters) const
	{
	// Use the member var to create the array so that we get proper cleanup behaviour
	RImplInfoArray* retList = new (ELeave) RImplInfoArray;
	
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRArray,retList));
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
	CleanupStack::Pop();
	return retList;
	}

TBool CDefaultResolver::Match(const TDesC8& aImplementationType, 
							  const TDesC8& aMatchType, 
							  TBool aUseWildcards) const
	{
	// In this function if allowing wildcards then TDesC8::Match is used which returns 
	// the position of the match or KErrNotFound
	// If not allowing wildcards then TDesC8::Compare is used which returns a TInt which
	// indicates if one descriptor is bigger than the other (0 if they are identical)

	TBool gotMatch = ETrue;
	
	if(aMatchType.Length()!=0)
	{
		gotMatch = EFalse;
		
		_LIT8(dataSeparator, "||");
		const TInt separatorLength = dataSeparator().Length();

		// Look for the section separator marker '||'
		TInt separatorPos = aImplementationType.Find(dataSeparator);
		if(separatorPos == KErrNotFound)
			{
			// Match against the whole string
			if(aUseWildcards)
				gotMatch = aMatchType.Match(aImplementationType) != KErrNotFound;
			else
				gotMatch = aMatchType.Compare(aImplementationType) == 0;
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
					gotMatch = aMatchType.Match(dataSection) != KErrNotFound;
				else
					gotMatch = aMatchType.Compare(dataSection) == 0;

				// If we found it then no need to continue, so return
				if(gotMatch)
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
				gotMatch = aMatchType.Match(dataSection) != KErrNotFound;
			else
				gotMatch = aMatchType.Compare(dataSection) == 0;
			}
		}
		return gotMatch;
	}
