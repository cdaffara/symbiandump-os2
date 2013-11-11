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
// The context for the unit tests upon the CTestManager class methods.
// 
//

#include "TestManagerTransitions.h"
#include "TestManagerObserverStub.h"

// ______________________________________________________________________________
//
inline CTestManager_UnitTestContext::CTestManager_UnitTestContext(CDataLogger& aDataLogger,
												MStateAccessor& aStateAccessor,
												MTransitionObserver& aObserver)
: CUnitTestContext(aDataLogger, aStateAccessor, aObserver)
	{
	//Do nothing
	}

inline CTestManager_UnitTestContext::~CTestManager_UnitTestContext()
	{
	delete iTestManager;
	delete iComponentInfo;
	delete iTestManagerObserverStub;
	delete iTestController;
	delete iTester;
	if(iTestListArray)
		{
		iTestListArray->ResetAndDestroy();
		delete iTestListArray;
		}
	iTests.ResetAndDestroy();
	}

MManagerObserver& CTestManager_UnitTestContext::ManagerObserver()
	{
	return *STATIC_CAST(MManagerObserver*, iTestManagerObserverStub);
	}
