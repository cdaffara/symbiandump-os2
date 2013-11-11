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
 @file CTestCase.cpp
*/

#include "tefunit.h"

CTestCase::CTestCase(const TTestName& aName)
/**
 * Constructor
 *
 * param - The name of the test
 */
	: CTestBase(aName)
	{
	}

CTestCase::~CTestCase()
/**
 * Destructor
 */
	{
	}

void CTestCase::AcceptL(MVisitor &aVisitor)
/**
 * AcceptL
 *
 * @param aVisitor - Instance of the visitor (runner) to accept
 */
	{
	aVisitor.VisitL(this);
	}
