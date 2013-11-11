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
 @file CTestCommand.h
*/

#ifndef __TEST_COMMAND__
#define __TEST_COMMAND__

#include <test/testexecuteserverbase.h>
#include "tefunit.h"

class CTestCommand : public CTestStep
	{
public:
	CTestCommand( const TDes& aStepPath, CTestSuite* aSuite );
	~CTestCommand();

	virtual TVerdict doTestStepL();

private:
	CTestSuite*	iSuite;
	TTestPath	iStepPath;
	};

#endif // __TEST_COMMAND__
