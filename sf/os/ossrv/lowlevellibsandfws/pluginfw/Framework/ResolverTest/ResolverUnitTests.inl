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
// The implementation of the CResolver unit test classes.
// 
//

#include "ResolverStateAccessor.h"

const TUid KCExampleInterfaceUid			= {0x10009DC0};
const TUid KCExampleInterfaceImp			= {0x10009DC1};

// ______________________________________________________________________________
//
_LIT(KDefaultResolverCreateAndDestroyUnitTest,"CDefaultResolver_CreateAndDestroy_UnitTest");

CDefaultResolver_CreateAndDestroy_UnitTest* CDefaultResolver_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDefaultResolver_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CDefaultResolver_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CDefaultResolver_CreateAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CDefaultResolver_CreateAndDestroy_UnitTest::~CDefaultResolver_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	// delete any validators used
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CDefaultResolver_CreateAndDestroy_UnitTest::CDefaultResolver_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDefaultResolverCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CDefaultResolver_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDefaultResolver_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDefaultResolver_UnitTestContext(iDataLogger, 
															  *iStateAccessor, 
															  *this);

	User::LeaveIfError(iUTContext->iFs.Connect());

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDefaultResolver_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDefaultResolver_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CDefaultResolver_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CDefaultResolver_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CDefaultResolver_CreateAndDestroy_UnitTest::PrepareUnitTestL()
	{
	iUTContext->iRegistry = CRegistryData::NewL(iUTContext->iFs);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultResolverIdentifyImplementationUnitTest,"CDefaultResolver_IdentifyImplementation_UnitTest");

CDefaultResolver_IdentifyImplementation_UnitTest* CDefaultResolver_IdentifyImplementation_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDefaultResolver_IdentifyImplementation_UnitTest* self = 
					new(ELeave) CDefaultResolver_IdentifyImplementation_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CDefaultResolver_IdentifyImplementation_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base

	// Delete all the iImplementationData (which we created)
	while(iUTContext->iImplementationData.Count())
		{
		CImplementationInformation* impInfo = iUTContext->iImplementationData[0];
		iUTContext->iImplementationData.Remove(0);
		delete impInfo;
		}

	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iDefaultValidator;
	delete iMatchValidator;
	delete iIdentifyImplementationValidator;
	delete iDtorValidator;

	return error;
	}

inline CDefaultResolver_IdentifyImplementation_UnitTest::~CDefaultResolver_IdentifyImplementation_UnitTest()
	{
	if(iUTContext)
		{
		// Delete all the iImplementationData (which we created)
		while(iUTContext->iImplementationData.Count())
			{
			CImplementationInformation* impInfo = iUTContext->iImplementationData[0];
			iUTContext->iImplementationData.Remove(0);
			delete impInfo;
			}
		delete iUTContext;
		}

	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iDefaultValidator;
	delete iMatchValidator;
	delete iIdentifyImplementationValidator;
	delete iDtorValidator;
	}

inline CDefaultResolver_IdentifyImplementation_UnitTest::CDefaultResolver_IdentifyImplementation_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDefaultResolverIdentifyImplementationUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CDefaultResolver_IdentifyImplementation_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDefaultResolver_StateAccessor;

	// Construct the Unit test context.
	iUTContext = new(ELeave) CDefaultResolver_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	User::LeaveIfError(iUTContext->iFs.Connect());
	iUTContext->iRegistry = CRegistryData::NewL(iUTContext->iFs);

	// Set up the match type and implementation type for use in the Match transition
	_LIT8(KResolveImplementationType, "DataType2||DataType1");
	_LIT8(KResolveMatchType, "DataType1");

	iUTContext->iImplementationType = HBufC8::NewMaxL(KResolveImplementationType().Length());
	TPtr8 impType = iUTContext->iImplementationType->Des();
	impType = KResolveImplementationType;

	iUTContext->iMatchType = HBufC8::NewMaxL(KResolveMatchType().Length());
	TPtr8 matchType = iUTContext->iMatchType->Des();
	matchType = KResolveMatchType;

	iUTContext->iExpectedMatchResult = ETrue; // Based on iMatchType and iImplementationType

	iUTContext->iInterfaceUid.iUid = KCExampleInterfaceUid.iUid;

	TUid impUid;
	impUid.iUid = KCExampleInterfaceImp.iUid;
	HBufC8* implType = HBufC8::NewMaxL(KResolveImplementationType().Length());
	TPtr8 impPtr = implType->Des();
	impPtr = KResolveImplementationType;
	TDriveUnit drive(EDriveC);
	CImplementationInformation* impData = 
		CImplementationInformation::NewL(impUid, 0, NULL, implType, NULL, drive, EFalse, EFalse);
	CleanupStack::PushL(impData);
	User::LeaveIfError(iUTContext->iImplementationData.Append(impData));
	CleanupStack::Pop(impData);

	iUTContext->iAdditionalParameters.SetDataType(*iUTContext->iMatchType);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDefaultResolver_Ctor_TransitionValidator(*iUTContext);
	iIdentifyImplementationValidator = new(ELeave) TDefaultResolver_IdentifyImplementationL_TransitionValidator(*iUTContext);
	iDefaultValidator = new(ELeave) TDefaultResolver_Default_TransitionValidator(*iUTContext);
	iMatchValidator = new(ELeave) TDefaultResolver_Match_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDefaultResolver_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CDefaultResolver_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CDefaultResolver_Match_Transition(*iUTContext,*iMatchValidator));
	AddTransitionL(new(ELeave)CDefaultResolver_Resolve_Transition(*iUTContext,*iDefaultValidator));
	AddTransitionL(new(ELeave)CDefaultResolver_IdentifyImplementationL_Transition(*iUTContext,*iIdentifyImplementationValidator));
	AddTransitionL(new(ELeave)CDefaultResolver_Dtor_Transition(*iUTContext,*iDtorValidator));

	// Add the permissible error codes for failures
	AddLeaveErrorCodeL(KEComErrNoInterfaceIdentified);
	}

