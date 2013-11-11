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
* Name        : toffsetof.h
* 
*
*/




#ifndef __TESTOFFSETOF_H__
#define __TESTOFFSETOF_H__


// INCLUDES
#include <test/testexecutestepbase.h>
#include <stdio.h>
#include <stddef.h>


_LIT(Koffsetof, "offsetof_test");


class CTestOffsetof : public CTestStep
	{
public:
	~CTestOffsetof(); 
	CTestOffsetof(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
protected:
	TInt offsetof_test();
	};
	
#endif //__TESTOFFSETOF_H__
