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
// This is the header file for Flogger test 002.xx


#if (!defined __STEP_002_XX_H__)
#define __STEP__002_XX_H__

class CFloggerTest002_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest002_01();
	~CFloggerTest002_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt executeStepL(TBool);
	};


class CFloggerTest002_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest002_02();
	~CFloggerTest002_02();

	virtual enum TVerdict doTestStepL( void );
	};

#endif //(__STEP_002_XX_H__)


