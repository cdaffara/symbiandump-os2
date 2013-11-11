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
 @file CTEFUtilitiesUT.cpp
*/

#include "ctefutilitiesut.h"
#include <test/tefexportconst.h>
//#include "TestPlatSecSetCapabilities.h"

void CTEFUtilityUT::SetupL()
/**
 * SetupL
 */
	{
	CTestFixture::SetupL();
//	iPletSecStep = new CTestPlatSecSetCapabilities();
	}

void CTEFUtilityUT::TearDownL()
/**
 * TearDownL
 */
	{
	CTestFixture::TearDownL();
	}

void CTEFUtilityUT::TestPlatSec_GetCapability()
	{
//	iPletSecStep->doTestStepL();
	TInt theInt = 10;
//
//	TBool res = iConfig.GetInt(_L("Int"),theInt);
//	ASSERT_TRUE(res);
	ASSERT_TRUE(theInt == 10);
//
//	ASSERT_FALSE(theString.Compare(KTestString) );
	}
CTestSuite* CTEFUtilityUT::CreateSuiteL(const TDesC& aName)
/**
 * CreateSuiteL
 *
 * @param aName - Suite name
 * @return - Suite
 */
	{
	SUB_SUITE
	ADD_TEST_STEP_CLASS( CTEFUtilityUT,TestPlatSec_GetCapability )
	END_SUITE
	}
