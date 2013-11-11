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
 
#include <e32uid.h>

#include "TestUtilities.h"	// For __FILE__LINE__
#include "Interface.h"
#include "SuicideTransitions.h"

// __________________________________________________________________________
CSuicideInterface_UnitTestContext::CSuicideInterface_UnitTestContext(CDataLogger& aDataLogger, 
													MStateAccessor& aAccessor,
													MTransitionObserver& aObserver)
: CUnitTestContext(aDataLogger, aAccessor, aObserver), iExampleInterface(NULL)
	{
	// Do nothing
	}

CSuicideInterface_UnitTestContext::~CSuicideInterface_UnitTestContext()
	{
	delete iExampleInterface;
	}


//Define the transition ids for all the transitions of the CExampleInterface class
_LIT(KInterfaceNewLTransitionName, "CSuicideInterfaceNewLTransition");
_LIT(KInterfaceDtorTransitionName, "CSuicideInterfaceDtorTransition");
_LIT(KInterfaceFireAndForgetTransitionName,	"CSuicideInterfaceFireAndForgetTransition");

_LIT8(KInterfaceResolveMatchStr,"suicidal");

// For each class method implement a transition class...
// __________________________________________________________________________
//Construction
CSuicideInterfaceNewLTransition::CSuicideInterfaceNewLTransition(CUnitTestContext& aUTContext,
																TTransitionValidator& aValidator) 
: CTransitionType(KInterfaceNewLTransitionName, aUTContext, aValidator)
	{
	}

void CSuicideInterfaceNewLTransition::TransitMethodL()
	{
	// Call the NewL method
	Context().iExampleInterface = CExampleInterface::NewL(KInterfaceResolveMatchStr());
	}

// Auto casting
inline CSuicideInterface_UnitTestContext& CSuicideInterfaceNewLTransition::Context()
	{
	return REINTERPRET_CAST(CSuicideInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
//Destruction
inline CSuicideInterfaceDtorTransition::CSuicideInterfaceDtorTransition(CUnitTestContext& aUTContext,
																TTransitionValidator& aValidator)
: CTransition(KInterfaceDtorTransitionName, aUTContext, aValidator)
	{
	}

inline void CSuicideInterfaceDtorTransition::TransitMethodL()
	{
	// Call the d'tor method
	delete Context().iExampleInterface;
	Context().iExampleInterface = NULL;
	REComSession::FinalClose();
	}

// Auto casting
inline CSuicideInterface_UnitTestContext& CSuicideInterfaceDtorTransition::Context()
	{
	return REINTERPRET_CAST(CSuicideInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
//FireAndForget method call
CSuicideInterfaceFireAndForgetTransition::CSuicideInterfaceFireAndForgetTransition(CUnitTestContext& aUTContext,
																					TTransitionValidator& aValidator)
: CTransition(KInterfaceFireAndForgetTransitionName, aUTContext, aValidator)
	{
	}

void CSuicideInterfaceFireAndForgetTransition::TransitMethodL()
	{
	// Execute the creation method, and then remove the references
	// in the context.
	// This technically leaks the class but it is supposed to
	// commit suicide and clean itself up, so its ok.
	User::LeaveIfError(Context().iExampleInterface->FireAndForget());
	Context().iExampleInterface = NULL;
	}

// Auto casting
inline CSuicideInterface_UnitTestContext& CSuicideInterfaceFireAndForgetTransition::Context()
	{
	return REINTERPRET_CAST(CSuicideInterface_UnitTestContext&,iUTContext);
	}
