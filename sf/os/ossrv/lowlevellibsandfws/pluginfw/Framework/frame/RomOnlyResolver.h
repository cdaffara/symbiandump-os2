// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __ROMONLYRESOLVER_H__
#define __ROMONLYRESOLVER_H__

#include "DefaultResolver.h"
#include "RegistryData.h"

class TEComResolverParams;

/**
	@internalComponent
	@since 7.0
	Special resolver which controls the identification, (resolution), of which implementation 
	will be used to satisfy an interface implementation instantiation. This version only allows
	plug-ins which are installed in ROM or are updates of ones originally found in ROM.
 */
class CRomOnlyResolver : public CDefaultResolver
{
public:
/**
	@fn				NewL(CRegistryData& aRegistry)
	Intended Usage	: Standardized safe construction which leaves nothing 
					on the cleanup stack.	
	Error Condition	: Cannot fully construct because of memory limitations.	
	@leave			KErrNoMemory
	@since			7.0
	@return			A pointer to the new class
	@post			CRomOnlyResolver is fully constructed, 
					and initialized.
 */
	static CRomOnlyResolver* NewL(MPublicRegistry& aRegistry);

/**
	@fn				~CRomOnlyResolver()
	Intended Usage	: Standard default d'tor	
	Error Condition	: None	
	@since			7.0
 */
	~CRomOnlyResolver();

/**
	@fn				IdentifyImplementationL(TUid aInterfaceUid, 
											const TEComResolverParams& aAdditionalParameters) const
	Intended Usage	:	Request that the resolver identify the most appropriate 
						interface implementation.
	Error Condition	:	
	@since			7.0
	@param			aInterfaceUid The interface for which implementations are requested
	@param			aAdditionalParameters Data to be used to refine the search further
	@return			The Uid of the best fit interface implementation - KNullUid if no match is found
	@pre 			Object is fully constructed and initialized
	@post			Registry contents are not modified but registry keys may be updated
 */
	TUid IdentifyImplementationL(TUid aInterfaceUid, 
								 const TEComResolverParams& aAdditionalParameters)const ;

/**
	@fn				ListAllL(TUid aInterfaceUid, 
							 const TEComResolverParams& aAdditionalParameters) const
	Intended Usage	:	List all the implementations which satisfy the specified interface.
	Error Condition	:	
	@since			7.0
	@param			aInterfaceUid The interface for which implementations are requested
	@param			aAdditionalParameters Data to be used to refine the search further
	@return			Pointer to an array of suitable implementations. Ownership of this array
	is passed to the calling function.
	@pre 			Object is fully constructed and initialized
	@post			Registry contents are not modified but registry keys may be updated
 */
	RImplInfoArray* ListAllL(TUid aInterfaceUid, 
							 const TEComResolverParams& aAdditionalParameters)const ;

private:
/**
	@internalComponent
	@fn				CRomOnlyResolver(CRegistryData& aRegistry)
	Intended Usage	: Standardized default c'tor	
	Error Condition	: None	
	@since			7.0
	@post			CRomOnlyResolver is fully constructed
 */
	explicit CRomOnlyResolver(MPublicRegistry& aRegistry);

};

#endif // __ROMONLYRESOLVER_H__

