/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "TestEComResolver.h"

CTestEComResolver* CTestEComResolver::NewL(MPublicRegistry& aRegistry)
	{
	return new(ELeave) CTestEComResolver(aRegistry);
	}

CTestEComResolver::~CTestEComResolver()
	{
	if(iImplementationInfoArray)
		{
		iImplementationInfoArray->Reset();
		delete iImplementationInfoArray;
		}
	}

CTestEComResolver::CTestEComResolver(MPublicRegistry& aRegistry)
:	CResolver(aRegistry)
	{
	// Do nothing here
	}

TUid CTestEComResolver::IdentifyImplementationL(
	TUid						aInterfaceUid,
	const TEComResolverParams&	aAdditionalParameters
) const
	{
	RImplInfoArray&	fullList = iRegistry.ListImplementationsL(aInterfaceUid);

	// Loop through the implementations matching on type
	TUid			ret = KNullUid;
	const TDesC8&	matchType = aAdditionalParameters.DataType();
	const TBool		useWildcards = aAdditionalParameters.IsWildcardMatch();
	const TInt		count = fullList.Count();
	for(TInt index = 0; (index<count) && (ret==KNullUid); ++index)
		{
		CImplementationInformation*	impData = fullList[index];
		// As soon as we get a match on the datatype then return uid of the 
		// implementation found.
		if (Match(impData->DataType(), matchType, useWildcards))
			{
			ret=impData->ImplementationUid();
			}
		}

	return ret;
	}

RImplInfoArray* CTestEComResolver::ListAllL(
	TUid						aInterfaceUid,
	const TEComResolverParams&	aAdditionalParameters
) const
	{
	// Use the member var to create the array so that we get proper cleanup behaviour
	iImplementationInfoArray = new(ELeave) RImplInfoArray;
	RImplInfoArray*	retList = iImplementationInfoArray;

	RImplInfoArray&	fullList = iRegistry.ListImplementationsL(aInterfaceUid);

	const TDesC8&	matchType = aAdditionalParameters.DataType();
	const TBool		useWildcards = aAdditionalParameters.IsWildcardMatch();
	const TInt		numImps = fullList.Count();
	for(TInt index = 0; index < numImps; ++index)
		{
		CImplementationInformation*	impData = fullList[index];
		if(Match(impData->DataType(), matchType, useWildcards))
			{
			User::LeaveIfError(retList->Append(impData));
			}
		}

	// Reset the member variable because we are passing ownership back
	iImplementationInfoArray = NULL;
	return retList;
	}

TBool CTestEComResolver::Match(
	const TDesC8&	aImplementationType, 
	const TDesC8&	aMatchType, 
	TBool			aUseWildcards
) const
	{
	_LIT8(dataSeparator, "||");
	const TInt	separatorLength = dataSeparator().Length();

	// Look for the section separator marker '||'
	TPtrC8	remainingData = aImplementationType;
	TInt	separatorPos = remainingData.Find(dataSeparator);
	TInt	matchPos=KErrNotFound;
	do 
		{
		TPtrC8	dataSection;
		if (separatorPos==KErrNotFound)
			{
			dataSection.Set(remainingData);
			}
		else
			{
			dataSection.Set(remainingData.Left(separatorPos));
			remainingData.Set(remainingData.Mid(separatorPos + separatorLength));
			}
		if(aUseWildcards)
			{
			matchPos=dataSection.Match(aMatchType);
			}
		else
			{
			matchPos=dataSection.Compare(aMatchType);
			}
		}
	while ((separatorPos!=KErrNotFound) && (matchPos==KErrNotFound));

	return matchPos!=KErrNotFound;
	}
