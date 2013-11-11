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
// This file contains the definition of the CResolver
// class.
// 
//

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef __RESOLVER_H__
#define __RESOLVER_H__

#include <e32base.h>
#include <f32file.h>

#include <ecom/ecom.h>
#include <ecom/publicregistry.h>

class TEComResolverParams;

/**
The UID idnetifying the ECOM Resolver plugin interface.

@publishedAll
@released
*/
const TUid KCustomResolverInterfaceUid = {0x10009D90};

/**
Abstract base class which is used to identify the correct interface implementation
based on criteria supplied by the client.
This base class can be used to write a client specific resolver, however this is not 
required as a default implementation is provided within ECom.

@publishedAll
@released
*/

class CResolver : public CBase
{
public:
	/**
	Intended Usage	:	Request that the resolver identify the most appropriate interface 
						implementation.
	Error Condition	:	Depends on implementation.
	@since			7.0
	@param			aInterfaceUid The interface for which an implementation is requested
	@param			aAdditionalParameters The parameters which must match for an 
					implementation to be suitable
	@return			The unique Id of the implementation which satisfies the specified parameters.
	@pre 			This object is fully constructed.
 	*/
	
	virtual TUid IdentifyImplementationL(TUid aInterfaceUid, 
										 const TEComResolverParams& aAdditionalParameters) const  = 0;

	/**
	Intended Usage	:	List all the implementations which satisfy the specified 
						interface definition and the resolve parameters supplied.
	Error Condition	:	Depends on implementation.
	@since			7.0
	@param			aInterfaceUid The interface for which implementations are requested
	@param			aAdditionalParameters The parameters which must match for an 
					implementation to be suitable
	@return			Pointer to an array of suitable implementations. Ownership of this 
					array is passed to the calling function.
	@pre 			Object is fully constructed and initialized
	@post			Registry contents are not modified but registry keys may be updated
	*/
	
	virtual RImplInfoArray* ListAllL(TUid aInterfaceUid, 
									 const TEComResolverParams& aAdditionalParameters) const = 0;

	
	inline RImplInfoArray& ListAllL(TUid aInterfaceUid)const;

protected:
	
	explicit inline CResolver(MPublicRegistry& aRegistry);

	// Attributes
	/** A reference to the instantiated registry information */
	
	 const MPublicRegistry& iRegistry;
};

/**
@internalAll
Intended Usage	: Standardized default c'tor
Error Condition	: None
@since			7.0
@post			CResolver is fully constructed
*/
CResolver::CResolver(MPublicRegistry& aRegistry) :
CBase(),
iRegistry(aRegistry)
	{
	// Do nothing
	}

/**
Intended Usage	:	List all the implementations which satisfy the specified interface.
Error Condition	:	@see CRegistryData::ListImplementationsL
@since			7.0
@param			aInterfaceUid The interface for which implementations are requested
@return			Array of suitable implementations
@pre 			Object is fully constructed and initialized
@post			Registry contents are not modified but registry keys may be updated
*/
RImplInfoArray& CResolver::ListAllL(TUid aInterfaceUid)const
	{
	return iRegistry.ListImplementationsL(aInterfaceUid);
	}

#endif /* __RESOLVER_H__ */

