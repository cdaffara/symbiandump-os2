/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* T_TESTSPEC.H
*
*/


#ifndef __T_TESTSPEC_H__
#define __T_TESTSPEC_H__

#include <e32base.h>
#include "t_mtestspec.h"

/**
 * This class represents a series of a test that needs to be run. It should be passed 
 * to a CTestHandler object in order for the tests to be run.
 */
class CTestSpec : public CBase, public MTestSpec
	{
public:
	IMPORT_C static CTestSpec* NewL();
	virtual ~CTestSpec();
	
public: // from MTestSpec
	IMPORT_C virtual TBool GetNextTest(CTestAction*& aAction);

public:
	/**
	 * This function adds a test to the series of tests. The class takes ownership
	 * of the argument.
	 * @param aAction The action to add. This pointer must not be 0.
	 * @return A standard error code.
	 */
	IMPORT_C virtual TInt AddNextTest(CTestAction* aAction);

	/// Free all tests, so that we can do a comparison of the heap count before
	/// any tests were loaded and after they were all run.
	void FreeAllTests();

private:
	CTestSpec();
	//void ConstructL();

private:
	/**
	 * This array contains all the tests to be run. The tests are owned by this class and
	 * are destroyed by the destructor.
	 */
	RPointerArray<CTestAction> iTestActionArray;
	TInt iTestNumber;
	};

#endif

