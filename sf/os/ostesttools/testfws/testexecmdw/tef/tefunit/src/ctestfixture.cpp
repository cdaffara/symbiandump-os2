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
 @file CTestFixture.cpp
*/

#include "tefunit.h"

CTestFixture::~CTestFixture()
/**
 * Destructor
 */
	{
	}

void CTestFixture::SetupL()
/**
 * SetupL
 */
	{
	}

void CTestFixture::TearDownL()
/**
 * TearDownL
 */
	{
	}

void CTestFixture::SetConfigL(CTestConfig& aConfig)
/**
 * SetConfigL
 *
 * @param aConfig - Test config to be used by the test cases
 */
	{
	iConfig = aConfig;
	}

void CTestFixture::SetLoggerL( CTestExecuteLogger& aLogger )
/**
 * SetLoggerL
 *
 * @param aLogger - Logger ot be used by the test cases
 */
	{
	iLogger = aLogger;
	}
