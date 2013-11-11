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
 @file GlobalShareStep.h
*/
#if (!defined __GLOBALSHARE_STEP_H__)
#define __GLOBALSHARE_STEP_H__
#include <test/testexecutestepbase.h>

// __EDIT_ME__ - Create your own test step definitions
class CGlobalShareStep1 : public CTestStep
	{
public:
	CGlobalShareStep1();
	~CGlobalShareStep1();
	virtual TVerdict doTestStepL();
	};

class CGlobalShareStep2 : public CTestStep
	{
public:
	CGlobalShareStep2();
	~CGlobalShareStep2();
	virtual TVerdict doTestStepL();
	};

/*@{*/
// Literals
_LIT(KGlobalShareStep1,"GlobalShareStep1");
_LIT(KGlobalShareStep2,"GlobalShareStep2");
/*@{*/

#endif
