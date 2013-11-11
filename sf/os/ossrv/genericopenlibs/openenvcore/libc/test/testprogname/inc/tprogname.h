/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tprogname.h
* 
*
*/




#ifndef __TESTPROGNAME_H__
#define __TESTPROGNAME_H__

#include <test/testexecutestepbase.h>

_LIT(KGetProgName, "TGetProgName");
_LIT(KSetProgName, "TSetProgName");

class CTestProgName : public CTestStep
	{
public:
	~CTestProgName(); 
	CTestProgName(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt TGetProgName();
	TInt TSetProgName();
	};
#endif 
