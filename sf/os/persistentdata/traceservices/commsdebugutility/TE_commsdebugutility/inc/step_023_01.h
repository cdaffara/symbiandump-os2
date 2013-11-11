// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This is the header file for Flogger unit test 023.01


#if (!defined __STEP_023_01_H__)
#define __STEP__023_01_H__

class CFloggerTest023_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest023_01();
	~CFloggerTest023_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestWrite();
	TInt executeStepL( TBool );
	TInt executeStepL();
	TInt DoTestCheckWriteL();

	};


class CFloggerTest023_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest023_02();
	~CFloggerTest023_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL( TBool );
	TInt executeStepL( void );
	};

#endif //(__STEP_023_01_H__)


