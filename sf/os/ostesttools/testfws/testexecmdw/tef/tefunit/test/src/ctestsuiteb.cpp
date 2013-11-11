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
 @file CTestSuiteB.cpp
*/

#include "ctestsuiteb.h"
#include "ctestsuitec.h"

void CTestSuiteB::SetupL()
/**
 * SetupL
 */
	{
	}

void CTestSuiteB::TearDownL()
/**
 * TearDownL
 */
	{
	}

void CTestSuiteB::TestOne()
/**
 * TestOne
 */
	{
	INFO_PRINTF1(_L("Running SuiteB:One"));
	TInt theInt = 0;
	ASSERT_EQUALS( theInt, 0 );
	TBool res = iConfig.GetInt(_L("SectionOne"),_L("TheInt"),theInt);
	ASSERT_TRUE( res );
	TBool theBool = EFalse;
	res = iConfig.GetBool(_L("SectionOne"),_L("TheBool"), theBool);
	ASSERT_TRUE( res );
	INFO_PRINTF3(_L("TheInt = %d The Bool = %d"),theInt,theBool);
	}

void CTestSuiteB::TestTwo()
/**
 * TestTwo
 */
	{
	INFO_PRINTF1(_L("Running SuiteB:Two"));
	TInt theInt = 0;
	TBool res = iConfig.GetInt(_L("SectionOne"),_L("TheInt"),theInt);
	ASSERT_TRUE( res );
	TBool theBool = EFalse;
	res = iConfig.GetBool(_L("SectionOne"),_L("TheBool"), theBool);
	ASSERT_TRUE( res );
	INFO_PRINTF3(_L("TheInt = %d The Bool = %d"),theInt,theBool);
	}

CTestSuite* CTestSuiteB::CreateSuiteL(	const TDesC& aName )
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
 	ADD_TEST_STEP_CLASS( CTestSuiteB, TestOne );
 	ADD_TEST_STEP_CLASS( CTestSuiteB, TestTwo );
#endif
 	ADD_TEST_SUITE( CTestSuiteC )
	END_SUITE;
	}
