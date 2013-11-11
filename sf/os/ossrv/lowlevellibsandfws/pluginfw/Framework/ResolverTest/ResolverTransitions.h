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
// The CResolver test transition class definitions
// 
//

#ifndef __RESOLVERTRANSITIONS_H__
#define __RESOLVERTRANSITIONS_H__

#include "LeakTestTransition.h"

#include <ecom/ecom.h>
#include "RegistryData.h"

#define CTransitionType CLeakTestTransition
// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the CDefaultResolver specific
	parameters and behaviour on the CDefaultResolver
	test class for a transition.
 */
class CDefaultResolver_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CDefaultResolver_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CDefaultResolver class under test.
		@pre 			None.
		@post			CDefaultResolver_UnitTestContext is fully constructed, and initialised.
	*/
	inline CDefaultResolver_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CDefaultResolver_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CDefaultResolver_UnitTestContext is fully constructed.
		@post			CDefaultResolver_UnitTestContext is fully destroyed
		*/
		virtual inline ~CDefaultResolver_UnitTestContext();

	/** The instance of the class under test */
	CDefaultResolver* iDefaultResolver;
	/** The data store which is used by the resolver */
	CRegistryData*	iRegistry;
	/** File server handle */
	RFs		iFs;
	/** ECom example interface Uid */
	TUid	iInterfaceUid;
	/** The Uid returned by IdentifyImplementationL.  Used in post validation. */
	TUid	iResolvedImpUid;
	/** Additional parameters used for resolving between implementations */
	TEComResolverParams		iAdditionalParameters;
	/** The implementation data type to find when resolving */
	HBufC8*			iImplementationType;
	/** The implementation match parameters to use when resolving */
	HBufC8*			iMatchType;
	/** Indicates whether wildcard matching should be used during resolving */
	TBool			iUseWildcards;
	/** The return value from Match() */
	TBool			iMatchResult;
	/** The expected return value from Match().  Used in post validation checking */
	TBool			iExpectedMatchResult;
	/** An array of potential implementations to resolve between */
	RImplInfoArray	iImplementationData;
	}; // CDefaultResolver_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDefaultResolver::NewL method.
 */
class CDefaultResolver_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDefaultResolver_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultResolver_NewL_Transition is fully constructed.
	*/
	CDefaultResolver_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultResolver::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultResolver::NewL)
		@since			7.0
		@return			None
		@pre 			CDefaultResolver_NewL_Transition is fully constructed.
		@post			No change in the CDefaultResolver_NewL_Transition apart
						from iDefaultResolver, which may have changed state.
						(@see CDefaultResolver::NewL post-condition) for iDefaultResolver's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultResolver_UnitTestContext
		@pre 			iUTContext is a valid CDefaultResolver_UnitTestContext.
		@post			No change in the CDefaultResolver_NewL_Transition
	*/
	inline CDefaultResolver_UnitTestContext& Context() const;
	};	// CDefaultResolver_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent 
	Comments : Transition test of the CDefaultResolver::Dtor method.
 */
class CDefaultResolver_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CDefaultResolver_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultResolver_Dtor_Transition is fully constructed.
	*/
	CDefaultResolver_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultResolver::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultResolver::Dtor)
		@since			7.0
		@return			None
		@pre 			CDefaultResolver_Dtor_Transition is fully constructed.
		@post			No change in the CDefaultResolver_Dtor_Transition apart
						from iDefaultResolver, which may have changed state.
						(@see CDefaultResolver::Dtor post-condition) for iDefaultResolver's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultResolver_UnitTestContext
		@pre 			iUTContext is a valid CDefaultResolver_UnitTestContext.
		@post			No change in the CDefaultResolver_Dtor_Transition
	*/
	inline CDefaultResolver_UnitTestContext& Context() const;
	};	// CDefaultResolver_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDefaultResolver::IdentifyImplementationL method.
 */
class CDefaultResolver_IdentifyImplementationL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDefaultResolver_IdentifyImplementationL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultResolver_IdentifyImplementationL_Transition is fully constructed.
	*/
	CDefaultResolver_IdentifyImplementationL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultResolver::IdentifyImplementationL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultResolver::IdentifyImplementationL)
		@since			7.0
		@return			None
		@pre 			CDefaultResolver_IdentifyImplementationL_Transition is fully constructed.
		@post			No change in the CDefaultResolver_IdentifyImplementationL_Transition apart
						from iDefaultResolver, which may have changed state.
						(@see CDefaultResolver::IdentifyImplementationL post-condition) for iDefaultResolver's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultResolver_UnitTestContext
		@pre 			iUTContext is a valid CDefaultResolver_UnitTestContext.
		@post			No change in the CDefaultResolver_IdentifyImplementationL_Transition
	*/
	inline CDefaultResolver_UnitTestContext& Context() const;
	};	// CDefaultResolver_IdentifyImplementationL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDefaultResolver::ListAllL method.
 */
class CDefaultResolver_ListAllL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDefaultResolver_ListAllL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultResolver_ListAllL_Transition is fully constructed.
	*/
	CDefaultResolver_ListAllL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultResolver::ListAllL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultResolver::ListAllL)
		@since			7.0
		@return			None
		@pre 			CDefaultResolver_ListAllL_Transition is fully constructed.
		@post			No change in the CDefaultResolver_ListAllL_Transition apart
						from iDefaultResolver, which may have changed state.
						(@see CDefaultResolver::ListAllL post-condition) for iDefaultResolver's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultResolver_UnitTestContext
		@pre 			iUTContext is a valid CDefaultResolver_UnitTestContext.
		@post			No change in the CDefaultResolver_ListAllL_Transition
	*/
	inline CDefaultResolver_UnitTestContext& Context() const;
	};	// CDefaultResolver_ListAllL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDefaultResolver::Match method.
 */
class CDefaultResolver_Match_Transition : public CTransition
	{
public:
	/**
		@fn				CDefaultResolver_Match_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultResolver_Match_Transition is fully constructed.
	*/
	CDefaultResolver_Match_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultResolver::Match method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultResolver::Match)
		@since			7.0
		@return			None
		@pre 			CDefaultResolver_Match_Transition is fully constructed.
		@post			No change in the CDefaultResolver_Match_Transition apart
						from iDefaultResolver, which may have changed state.
						(@see CDefaultResolver::Match post-condition) for iDefaultResolver's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultResolver_UnitTestContext
		@pre 			iUTContext is a valid CDefaultResolver_UnitTestContext.
		@post			No change in the CDefaultResolver_Match_Transition
	*/
	inline CDefaultResolver_UnitTestContext& Context() const;
	};	// CDefaultResolver_Match_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDefaultResolver::Resolve method.
 */
class CDefaultResolver_Resolve_Transition : public CTransition
	{
public:
	/**
		@fn				CDefaultResolver_Resolve_Transition(CUnitTestContext& aUTContext,
															TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultResolver_Resolve_Transition is fully constructed.
	*/
	CDefaultResolver_Resolve_Transition(CUnitTestContext& aUTContext,
										TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultResolver::ResolveL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultResolver::ResolveL)
		@since			7.0
		@return			None
		@pre 			CDefaultResolver_ResolveL_Transition is fully constructed.
		@post			No change in the CDefaultResolver_ResolveL_Transition apart
						from iDefaultResolver, which may have changed state.
						(@see CDefaultResolver::ResolveL post-condition) for iDefaultResolver's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultResolver_UnitTestContext
		@pre 			iUTContext is a valid CDefaultResolver_UnitTestContext.
		@post			No change in the CDefaultResolver_ResolveL_Transition
	*/
	inline CDefaultResolver_UnitTestContext& Context() const;
	};	// CDefaultResolver_ResolveL_Transition

#include "ResolverTransitions.inl"

#endif		// __RESOLVERTRANSITIONS_H__
