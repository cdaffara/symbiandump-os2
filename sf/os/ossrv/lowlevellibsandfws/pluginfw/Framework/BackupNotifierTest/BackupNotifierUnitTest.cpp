// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The unit test class implementations for the CBackupNotifier class.
// 
//

#include "BackupNotifierUnitTest.h"

// ______________________________________________________________________________
//
_LIT(KBackupNotifierCreateAndDestroyUnitTest,"CBackupNotifier_CreateAndDestroy_UnitTest");

CBackupNotifier_CreateAndDestroy_UnitTest* CBackupNotifier_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CBackupNotifier_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CBackupNotifier_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CBackupNotifier_CreateAndDestroy_UnitTest::RunError(TInt aError)
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

inline CBackupNotifier_CreateAndDestroy_UnitTest::~CBackupNotifier_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CBackupNotifier_CreateAndDestroy_UnitTest::CBackupNotifier_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KBackupNotifierCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CBackupNotifier_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TBackupNotifier_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CBackupNotifier_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iUTContext->iStub = new(ELeave) CNotificationStub(iDataLogger);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TBackupNotifier_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CBackupNotifier_NewL_Transition(*iUTContext,*iCtorValidator));
	iDtorValidator = new(ELeave) TBackupNotifier_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CBackupNotifier_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

