// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include <ecom/ecom.h>
#include <ecom/resolver.h>

/**
CExampleResolver implements a dummy custom resolver to test upgrade and
SWI file overwrite.
*/
class CExampleResolver : public CResolver
	{
public:
	static CExampleResolver* NewL(MPublicRegistry& aRegistry);
	~CExampleResolver();

	virtual TUid IdentifyImplementationL(TUid aInterfaceUid, 
										 const TEComResolverParams&) const;
	virtual RImplInfoArray* ListAllL(TUid aInterfaceUid, 
									 const TEComResolverParams&) const;
	virtual TInt GetSignature(void) const;

protected:
	CExampleResolver(MPublicRegistry& aRegistry);
	virtual void VerifyVersionL(const CImplementationInformation* aImplInfo, TBool& aMe) const;
	};
