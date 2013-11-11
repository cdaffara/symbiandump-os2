/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



/**
 @file CTestSuiteC.cpp
*/

#include "ctestsuitec.h"

void CTestSuiteC::SetupL()
/**
 * SetupL
 */
	{
	}

void CTestSuiteC::TearDownL()
/**
 * TearDownL
 */
	{
	}

void CTestSuiteC::TestOne()
/**
 * TestOne
 */
	{
	INFO_PRINTF1(_L("Running SuiteC:One"));
	TInt theInt = 0;
	TBool res = iConfig.GetInt(_L("SectionOne"),_L("TheInt"),theInt);
	ASSERT_TRUE( res );
	TBool theBool = EFalse;
	res = iConfig.GetBool(_L("SectionOne"),_L("TheBool"), theBool);
	ASSERT_TRUE( res );
	INFO_PRINTF3(_L("TheInt = %d The Bool = %d"),theInt,theBool);
	}

/*
 *  A test step
 */
void CTestSuiteC::TestTwo()
/**
 * TestTwo
 */
	{
	INFO_PRINTF1(_L("Running SuiteC:Two"));
	TInt theInt = 0;
	TBool res = iConfig.GetInt(_L("SectionOne"),_L("TheInt"),theInt);
	ASSERT_TRUE( res );
	TBool theBool = EFalse;
	res = iConfig.GetBool(_L("SectionOne"),_L("TheBool"), theBool);
	ASSERT_TRUE( res );
	INFO_PRINTF3(_L("TheInt = %d The Bool = %d"),theInt,theBool);
	}

CTestSuite* CTestSuiteC::CreateSuiteL(	const TDesC& aName )
/**
 * CreateSuiteL
 *
 * @param aName - Suite name
 * @return - Suite
 */
	{
	SUB_SUITE;
#ifndef __GCCXML__
 	ADD_TEST_STEP( TestOne );
 	ADD_TEST_STEP( TestTwo );
#endif
#ifdef __GCCXML__
 	ADD_TEST_STEP_CLASS( CTestSuiteC, TestOne );
 	ADD_TEST_STEP_CLASS( CTestSuiteC, TestTwo );
#endif
 	END_SUITE;
	}
