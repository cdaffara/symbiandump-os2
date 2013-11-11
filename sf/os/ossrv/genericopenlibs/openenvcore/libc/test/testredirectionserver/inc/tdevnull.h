/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tdevnull.h
*
*/

#ifndef __TDEVNULL_H__
#define __TDEVNULL_H__

#include <test/testexecutestepbase.h>
 

_LIT(Ktest_devnullwrite, "test_devnullwrite");
_LIT(Ktest_devnullread, "test_devnullread");


class CTestDevNull : public CTestStep
	{
public:
	CTestDevNull(const TDesC& aStepName);
	virtual ~CTestDevNull(); 
	virtual TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TVerdict TestDevNullWrite();
	TVerdict TestDevNullRead();
	};
#endif //__TDEVNULL_H__
