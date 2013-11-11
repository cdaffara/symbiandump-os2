// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CClientRequestPerfTestResolver.h
// 
//


#ifndef __CCLIENTREQUESTPERFTESTRESOLVER_H__
#define __CCLIENTREQUESTPERFTESTRESOLVER_H__

#include "Resolver.h"

/**
	@since 9.2
	Controls the identification, (resolution), of which implementation 
	will be used to satisfy an interface implementation instantiation.
 */
class CClientRequestPerfTestResolver : public CResolver
{
public:
	static CClientRequestPerfTestResolver* NewL(MPublicRegistry& aRegistry);
	~CClientRequestPerfTestResolver();
	TUid IdentifyImplementationL(TUid aInterfaceUid, const TEComResolverParams& aAdditionalParameters) const;
	RImplInfoArray* ListAllL(TUid aInterfaceUid, 
							 const TEComResolverParams& aAdditionalParameters)const ;

private:
	explicit CClientRequestPerfTestResolver(MPublicRegistry& aRegistry);
	TUid Resolve(const RImplInfoArray& aImplementationsInfo, 
				 const TEComResolverParams& aAdditionalParameters) const;
	TBool Match(const TDesC8& aImplementationType, 
				const TDesC8& aMatchType, 
				TBool aUseWildcards) const;

private:
	mutable RImplInfoArray* iImplementationInfoArray;
};

#endif // __CCLIENTREQUESTPERFTESTRESOLVER_H__

