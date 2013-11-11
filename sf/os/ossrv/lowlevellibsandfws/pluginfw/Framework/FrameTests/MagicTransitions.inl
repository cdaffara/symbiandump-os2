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
 
#include <e32uid.h>

#include "TestUtilities.h"	// For __FILE__LINE__
#include "Interface.h"
#include "MagicTransitions.h"


// __________________________________________________________________________
CExampleInterface_UnitTestContext::CExampleInterface_UnitTestContext(CDataLogger& aDataLogger, 
													MStateAccessor& aAccessor,
													MTransitionObserver& aObserver)
: CUnitTestContext(aDataLogger, aAccessor, aObserver), iExampleInterface(NULL)
	{
	// Do nothing
	}

CExampleInterface_UnitTestContext::~CExampleInterface_UnitTestContext()
	{
	iListImplThread.Close();
	delete iExampleInterface;
	delete iSecondExampleInterface;
	iImplementations.ResetAndDestroy();
	}

//Define the transition ids for all the transitions of the CExampleInterface class
_LIT(KInterfaceNewLTransitionName, "CExampleInterfaceNewLTransition");
_LIT(KInterfaceNewWPLTransitionName, "CExampleInterfaceNewWPLTransition");
_LIT(KInterfaceNewResolveLTransitionName, "CExampleInterfaceNewResolveLTransition");
_LIT(KInterfaceDoubleNewLTransitionName, "CExampleInterfaceDoubleNewLTransition");
_LIT(KInterfaceDestroyTransitionName, "CExampleInterfaceDestroyTransition");
_LIT(KInterfaceDtorTransitionName, "CExampleInterfaceDtorTransition");
_LIT(KInterfaceDoubleDtorTransitionName, "CExampleInterfaceDoubleDtorTransition");
_LIT(KInterfaceListImplementationsLTransitionName, "CExampleInterfaceListImplementationsLTransition");
_LIT(KInterfacePersistLTransitionName, "CExampleInterfacePersistLTransition");
_LIT(KInterfaceAsynchCreateTransitionName, "CExampleInterfaceAsynchCreateTransition");
_LIT(KInterfaceDoMethodLTransitionName, "CExampleInterfaceDoMethodLTransition");
_LIT(KInterfaceFireAndForgetTransitionName,	"CExampleInterfaceFireAndForgetTransition");
_LIT(KInterfaceListImplementationsLNewThreadTransitionName,	"CExampleInterfaceListImplementationsLNewThreadTransition");

// For each class method implement a transition class...
// __________________________________________________________________________
//Construction
CExampleInterfaceNewLTransition::CExampleInterfaceNewLTransition(CUnitTestContext& aUTContext,
																TTransitionValidator& aValidator) 
: CTransitionType(KInterfaceNewLTransitionName, aUTContext, aValidator)
	{
	}

void CExampleInterfaceNewLTransition::TransitMethodL()
	{
	_LIT(KExampleInterfaceNewLTransitMethod, "CExampleInterface::NewL transition");
	Context().DataLogger().LogInformation(KExampleInterfaceNewLTransitMethod);

	// Call the NewL method
	Context().iExampleInterface = CExampleInterface::NewL();
	}

// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceNewLTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
CExampleInterfaceNewWPLTransition::CExampleInterfaceNewWPLTransition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator) 
: CTransitionType(KInterfaceNewWPLTransitionName, aUTContext, aValidator)
	{
	}

void CExampleInterfaceNewWPLTransition::TransitMethodL()
	{
	// Call the NewL method which requires parameters
	const TInt KTestId = 0xBDE;
	_LIT(KTestIdDes,"0xBDE");
	Context().iExampleInterface = CExampleInterface::NewL(KTestId,KTestIdDes());
	}
// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceNewWPLTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
CExampleInterfaceNewResolveLTransition::CExampleInterfaceNewResolveLTransition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator) 
: CTransitionType(KInterfaceNewResolveLTransitionName, aUTContext, aValidator)
	{
	}

void CExampleInterfaceNewResolveLTransition::TransitMethodL()
	{
	// Call the NewL method which requires parameters
	_LIT8(KTestResolveType,"text/*");
	Context().iExampleInterface = CExampleInterface::NewL(KTestResolveType);
	}
// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceNewResolveLTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
//Construction
CExampleInterfaceDoubleNewLTransition::CExampleInterfaceDoubleNewLTransition(CUnitTestContext& aUTContext,
																TTransitionValidator& aValidator) 
: CTransitionType(KInterfaceDoubleNewLTransitionName, aUTContext, aValidator)
	{
	}

void CExampleInterfaceDoubleNewLTransition::TransitMethodL()
	{
	_LIT(KExampleInterfaceNewLTransitMethod, "CExampleInterface::NewL transition");
	Context().DataLogger().LogInformation(KExampleInterfaceNewLTransitMethod);

	CExampleInterface* objectTwo = CExampleInterface::NewL();
	CleanupStack::PushL(objectTwo);
	Context().iExampleInterface = CExampleInterface::NewL();
	Context().iSecondExampleInterface = objectTwo;
	CleanupStack::Pop(objectTwo);
	}

// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceDoubleNewLTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
//Destruction
inline CExampleInterfaceDtorTransition::CExampleInterfaceDtorTransition(CUnitTestContext& aUTContext,
																TTransitionValidator& aValidator)
: CTransitionType(KInterfaceDtorTransitionName, aUTContext, aValidator)
	{
	}

inline void CExampleInterfaceDtorTransition::TransitMethodL()
	{
	// Call the d'tor method
	delete Context().iExampleInterface;
	Context().iExampleInterface = NULL;
	REComSession::FinalClose();
	}

// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceDtorTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
//Destruction
inline CExampleInterfaceDoubleDtorTransition::CExampleInterfaceDoubleDtorTransition(CUnitTestContext& aUTContext,
																TTransitionValidator& aValidator)
: CTransitionType(KInterfaceDoubleDtorTransitionName, aUTContext, aValidator)
	{
	}

inline void CExampleInterfaceDoubleDtorTransition::TransitMethodL()
	{
	// Call the d'tor method
	delete Context().iExampleInterface;
	Context().iExampleInterface = NULL;
	delete Context().iSecondExampleInterface;
	Context().iSecondExampleInterface = NULL;
	}

// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceDoubleDtorTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
//Leaving method call
inline CExampleInterfaceDoMethodLTransition::CExampleInterfaceDoMethodLTransition(CUnitTestContext& aUTContext,
																		TTransitionValidator& aValidator)
: CTransitionType(KInterfaceDoMethodLTransitionName, aUTContext, aValidator)
	{
	}

inline void CExampleInterfaceDoMethodLTransition::TransitMethodL()
	{
	// Call the DoMethodL method
	Context().iExampleInterface->DoMethodL();
	}

// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceDoMethodLTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
//ListImplementationsL method call
CExampleInterfaceListImplementationsLTransition::CExampleInterfaceListImplementationsLTransition(CUnitTestContext& aUTContext,
																								TTransitionValidator& aValidator)
: CTransitionType(KInterfaceListImplementationsLTransitionName, aUTContext, aValidator)
	{
	}

void CExampleInterfaceListImplementationsLTransition::TransitMethodL()
	{
	// Call the ListImplementationsL method
	CExampleInterface::ListImplementationsL(Context().iImplementations);
	}

// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceListImplementationsLTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
//ListAllImplementationsL method call
CExampleInterfaceListAllImplementationsLTransition::CExampleInterfaceListAllImplementationsLTransition(CUnitTestContext& aUTContext,
																								TTransitionValidator& aValidator)
: CTransitionType(KInterfaceListImplementationsLTransitionName, aUTContext, aValidator)
	{
	}

void CExampleInterfaceListAllImplementationsLTransition::TransitMethodL()
	{
	// Call the ListImplementationsL method
	CExampleInterface::ListAllImplementationsL(Context().iImplementations);
	}

// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceListAllImplementationsLTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

// __________________________________________________________________________
//ListAllImplementationsL method call
CExampleInterfaceListImplementationsLNewThreadTransition::CExampleInterfaceListImplementationsLNewThreadTransition(CUnitTestContext& aUTContext,
																								TTransitionValidator& aValidator)
: CTransitionType(KInterfaceListImplementationsLNewThreadTransitionName, aUTContext, aValidator)
	{
	}

void CExampleInterfaceListImplementationsLNewThreadTransition::TransitMethodL()
	{
	// Start the new thread which will call the ListImplementationsL method
	Context().iListImplThread.Resume();

	// Stay here until that thread returns
	TExitType exit = EExitPending;
	while( exit == EExitPending)
		{
		exit = Context().iListImplThread.ExitType();
		}
	}

// Auto casting
inline CExampleInterface_UnitTestContext& CExampleInterfaceListImplementationsLNewThreadTransition::Context()
	{
	return REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	}

