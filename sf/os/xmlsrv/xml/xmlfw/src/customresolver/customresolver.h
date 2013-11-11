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

#ifndef XMLCUSTOMRESOLVER_H
#define XMLCUSTOMRESOLVER_H

#include <e32def.h>

#include <ecom/resolver.h>
#include <ecom/publicregistry.h>
#include <ecom/ecomresolverparams.h>

namespace Xml
{
class CMatchData;

/**
Implements the functionality of XML customized resovler.
This is used as a plugin resolver for ECOM. 

Plugin resource file: 10273861.rss.

@internalComponent
*/
class CCustomResolver: public CResolver
{

public:
	static CCustomResolver* NewL(MPublicRegistry& aRegistry);
	~CCustomResolver();

public:
	// CResolver derivied methods
	virtual TUid IdentifyImplementationL(TUid aInterfaceUid, const TEComResolverParams& aAdditionalParameters) const;
	virtual RImplInfoArray* ListAllL(TUid aInterfaceUid, const TEComResolverParams& aAdditionalParameters) const;

private:
	CCustomResolver(MPublicRegistry& aRegistry);
};

}

#endif // XMLCUSTOMRESOLVER_H


