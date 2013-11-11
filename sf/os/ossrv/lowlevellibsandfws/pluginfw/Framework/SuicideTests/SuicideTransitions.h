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
// CExampleInterface interface.
// 
//

#ifndef __SUICIDETRANSITION_H__
#define __SUICIDETRANSITION_H__

#include <test_bed/transition.h>
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
class CSuicideInterface_UnitTestContext : public CUnitTestContext
	{
public:
	inline CSuicideInterface_UnitTestContext(CDataLogger& aDataLogger, 
							  MStateAccessor& aAccessor,
							  MTransitionObserver& aObserver);
	inline ~CSuicideInterface_UnitTestContext();

	CExampleInterface* iExampleInterface;
	};

/**
	Comments : Transition of the NewL method
 */
class CSuicideInterfaceNewLTransition : public CTransitionType
	{
public:
	inline CSuicideInterfaceNewLTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CSuicideInterface_UnitTestContext& Context();
	};

/**
	Comments : Transition of the destructor method
 */
class CSuicideInterfaceDtorTransition : public CTransition
	{
public:
	inline CSuicideInterfaceDtorTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CSuicideInterface_UnitTestContext& Context();
	};


/**
	Comments : Transition of the FireAndForget method
 */
class CSuicideInterfaceFireAndForgetTransition : public CTransition
	{
public:
	inline CSuicideInterfaceFireAndForgetTransition( CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	inline void TransitMethodL();
	inline CSuicideInterface_UnitTestContext& Context();
	};

#include "SuicideTransitions.inl"

#endif	// __SUICIDETRANSITION_H__
