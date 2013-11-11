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
// Customized ECOM resolver for XML plugin parsers
// 
//

/**
 @file
 @internalComponent
*/

#include <e32std.h>
#include <ecom/implementationinformation.h>

#include <xml/xmlframeworkerrors.h>
#include <xml/xmlframeworkconstants.h>
#include <xml/matchdata.h>

#include "customresolver.h"

using namespace Xml;

/**
Class constructor.

@param aRegistry Gives the resolver access to a list of implementations of an interface.
*/
CCustomResolver::CCustomResolver(MPublicRegistry& aRegistry)
	:CResolver(aRegistry)
{
	
}

/**
Creates the instance of CCustomResolver class. 

@param aRegistry Gives the resolver access to a list of implementations of an interface.

@leave KErrNoMemory If there is not enough memory to create an object
*/

CCustomResolver* CCustomResolver::NewL(MPublicRegistry& aRegistry)
{
	return new (ELeave) CCustomResolver(aRegistry);
}
	
/**
Object destructor.
*/
CCustomResolver::~CCustomResolver()
{

}

/**
Identifies required parsers. 

@param aInterfaceUid 			Interface UID of required parser
@param aAdditionalParameters	Additional parameters for parser resolver 

@return Parser's Uid or KNullUid if parser isn't found. 

@leave KErrNoMemory 			If there is not enough memory to create 
							necessary objects
								
@leave KErrArgument 			If incorrect or partial information is provided 
							for parser resolution
								
@leave KErrXmlMoreThanOneParserMatched 	
							If Xml framework narrowed down the query 
							to many parsers and a user requested to leave 
							in such case (LeaveOnMany flag set). 
								
*/
TUid CCustomResolver::IdentifyImplementationL(TUid aInterfaceUid, const TEComResolverParams& aAdditionalParameters) const
	
{
	// Unpack user matching criteria.
	// Externalized CMatchData object is stored in a heap based buffer as a stream. 
	// Buffer pointer is stored in data type attribute of aAdditionalParameters
	// Specialized NewL method creates a CMatchData object based on that pointer. 
	CMatchData* criteria = CMatchData::NewL(aAdditionalParameters.DataType());
	CleanupStack::PushL(criteria);
	
	// Verify input data
	// Mime type string is a mandatory field for parser resolution
	if (criteria->MimeType().Length() == 0 )
		User::Leave(KErrArgument);
	
	// This method is designed for plugin parser resolution only
	if (aInterfaceUid != KParserInterfaceUid)
		User::Leave(KErrArgument);
	
	// Use the CResolver method to create an array, so that we get proper cleanup behaviour
	RImplInfoArray& implInfo = CResolver::ListAllL(aInterfaceUid);
	
	// Run the resolution process
	TUid uid = criteria->ResolveL(implInfo);
	
	//clean up
	CleanupStack::PopAndDestroy(criteria);
	implInfo.Close();
	return uid;
}
 
 
/**
The ListAllL method is not supported by this customized resolver.

@leave KErrNotSupported 
*/	
RImplInfoArray* CCustomResolver::ListAllL(TUid /*aInterfaceUid*/, const TEComResolverParams& /*aAdditionalParameters */) const
{
	User::Leave(KErrNotSupported);
	return NULL; 
}

