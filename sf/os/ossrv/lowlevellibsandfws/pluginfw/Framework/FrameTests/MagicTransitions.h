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
// This file contains the definitions for
// the transition classes for testing the
// CExampleInterface class.
// 
//

#ifndef __MAGICTRANSITION_H__
#define __MAGICTRANSITION_H__

#include <ecom/test_bed/transition.h>
#include "LeakTestTransition.h"
#include "Interface.h"

class CExampleInterface;

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	Comments : The context in which the transitions will run
				That is to say the container in which the transition 
				parameters are passed around
 */
class CExampleInterface_UnitTestContext : public CUnitTestContext
	{
public:
	inline CExampleInterface_UnitTestContext(CDataLogger& aDataLogger, 
							  MStateAccessor& aAccessor,
							  MTransitionObserver& aObserver);
	inline ~CExampleInterface_UnitTestContext();

	CExampleInterface* iExampleInterface;
	CExampleInterface* iSecondExampleInterface;
	RImplInfoPtrArray iImplementations;
	/** An extra thread so that we can use ECom multi-threaded */
	RThread iListImplThread;
	};

/**
	Comments : Transition of the NewL method
 */
class CExampleInterfaceNewLTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceNewLTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

/**
	Comments : Transition of the NewL method with parameters.
 */
class CExampleInterfaceNewWPLTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceNewWPLTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

/**
	Comments : Transition of the NewL method with a match descriptor for resolving.
 */
class CExampleInterfaceNewResolveLTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceNewResolveLTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

/**
	Comments : Special transition which creates two objects
 */
class CExampleInterfaceDoubleNewLTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceDoubleNewLTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

/**
	Comments : Transition of the destructor method
 */
class CExampleInterfaceDtorTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceDtorTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

/**
	Comments : Special destructor transition which destroys two implementations
 */
class CExampleInterfaceDoubleDtorTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceDoubleDtorTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

/**
	Comments : Transition of the DoMethodL method
 */
class CExampleInterfaceDoMethodLTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceDoMethodLTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

/**
	Comments : Transition of the ListImplementationsL method
 */
class CExampleInterfaceListImplementationsLTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceListImplementationsLTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

/**
	Comments : Transition of the ListAllImplementationsL method
 */
class CExampleInterfaceListAllImplementationsLTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceListAllImplementationsLTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

/**
	Comments : Transition of the ListAllImplementationsL method which occurs in a separate thread
 */
class CExampleInterfaceListImplementationsLNewThreadTransition : public CTransitionType
	{
public:
	inline CExampleInterfaceListImplementationsLNewThreadTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CExampleInterface_UnitTestContext& Context();
	};

#include "MagicTransitions.inl"

#endif	// __MAGICLeakTestTransition_H__
