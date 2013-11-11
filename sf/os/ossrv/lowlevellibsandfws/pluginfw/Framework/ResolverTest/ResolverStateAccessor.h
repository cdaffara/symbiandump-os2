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
// The definition of the accessor class for the CResolver tests.
// 
//

#ifndef __RESOLVERSTATEACCESSOR_H__
#define __RESOLVERSTATEACCESSOR_H__

#include <s32file.h>

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : State accessor for the CDefaultResolver object under test.
 */
class TDefaultResolver_StateAccessor : public MStateAccessor
	{
public:
	/**
		@fn				InvariantTest(TAny* aTestObject)
		Intended Usage	: Checks if CDefaultResolver has violated its invariants.	
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the invariants were valid, error code otherwise.
		@pre 			TDefaultResolver_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
	inline TInt InvariantTest(TAny* aTestObject);

	/**
		@fn				Internalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		@since			7.0
		@param			aTestObject The object to be tested
		@return			KErrNone if the internalize was successful.
		@return			One of the EPOC standard error codes indicating the reason
						for the failure	to load the object state.
		@pre 			TDefaultResolver_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			The class under test is set to the state specified
	 */
	inline virtual TInt Internalize(TAny* aTestObject);

	/**
		@fn				Externalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		@since			7.0
		@param			aTestObject The object to be tested
		@return			KErrNone if the internalize was successful.
		@return			One of the EPOC standard error codes indicating the reason
						for the failure	to externalize the object.
		@pre 			TDefaultResolver_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			Returns an error code dependant on the result of the test
	 */
	inline virtual TInt Externalize(TAny* aTestObject);

	/**
		@fn				InternalizeL(RFileReadStream& aStream, CDefaultResolver* aDefaultResolver)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestream to read from .
		@param			aDefaultResolver A pointer to the CDefaultResolver object under test.
		@return			None.
		@pre 			TDefaultResolver_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
		inline void InternalizeL(RFileReadStream& aStream, CDefaultResolver* aDefaultResolver);

	/**
		@fn				ExternalizeL(RFileWriteStream& aStream, CDefaultResolver* aDefaultResolver)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestream to persist into.
		@param			aDefaultResolver A pointer to the CDefaultResolver object under test.
		@return			None.
		@pre 			TDefaultResolver_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(RFileWriteStream& aStream, CDefaultResolver* aDefaultResolver);

/**
	@fn				TBool CDefaultResolver::Match(const TDesC8& aImplementationType, 
												  const TDesC8& aMatchType, 
												  TBool aUseWildcards) const
	Intended Usage	:	
	Error Condition	:	
	@since			7.0
	@param			aImplementationType " "
	@param			aMatchType " "
	@param			aUseWildcards " "
	@return			TBool ETrue if a match has been found
	@pre 			The object is fully constructed
 */
	TBool Match(CDefaultResolver& aResolver,
				const TDesC8& aImplementationType, 
				const TDesC8& aMatchType, 
				TBool aUseWildcards) const;

/**
	@fn				Resolve(CDefaultResolver& aResolver,
							RImplInfoArray& aImplementationData, 
							const TEComResolverParams& aAdditionalParameters) const
	Intended Usage	:	
	Error Condition	:	
	@since			7.0
	@param			aResolver " "
	@param			aImplementationData " "
	@param			aAdditionalParameters " "
	@return			The Uid of a suitable implementation
	@pre 			The object is fully constructed
 */
	TUid Resolve(CDefaultResolver& aResolver,
				 RImplInfoArray& aImplementationData, 
				 const TEComResolverParams& aAdditionalParameters) const;

private:
	/**
		@fn				InternalizeL(CDefaultResolver* aDefaultResolver)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TDefaultResolver_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	inline void InternalizeL(CDefaultResolver* aDefaultResolver);

	/**
		@fn				ExternalizeL(CDefaultResolver* aDefaultResolver)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TDefaultResolver_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(CDefaultResolver* aDefaultResolver);
	};	// TDefaultResolver_StateAccessor

#include "ResolverStateAccessor.inl"

#endif		// __RESOLVERSTATEACCESSOR_H__
