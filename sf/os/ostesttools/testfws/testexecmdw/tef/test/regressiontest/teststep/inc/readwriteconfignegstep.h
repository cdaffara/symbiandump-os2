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
 @file WriteStringStep.h
*/
#if (!defined __READ_WRITE_CONFIG_NEG_STEP_H__)
#define __READ_WRITE_CONFIG_NEG_STEP_H__
#include <test/testexecutestepbase.h>

class CReadWriteConfigNegStep : public CTestStep
	{
public:
	CReadWriteConfigNegStep();
	~CReadWriteConfigNegStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	TBool TestInt();
	TBool TestBool();
	TBool TestHex();
	TBool TestString();
	};

#endif
