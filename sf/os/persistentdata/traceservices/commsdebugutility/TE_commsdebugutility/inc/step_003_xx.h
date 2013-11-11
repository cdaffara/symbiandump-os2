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
// This is the header file for Flogger test 003.xx


#include <comms-infras/commsdebugutility.h>


#if (!defined __STEP_003_XX_H__)
#define __STEP__003_XX_H__

class CFloggerTest003_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest003_01();
	~CFloggerTest003_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL();
	TInt executeStepL(TBool heapTest);
	TInt DoTestWrite(TBool heapTest);
	};


class CFloggerTest003_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest003_02();
	~CFloggerTest003_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt executeStepL(TBool);
	TInt DoTestWrite();
	private:
	RFileLogger iFlogger;
	};


class CFloggerTest003_03 : public CTestStepFlogger
{
	public:
	CFloggerTest003_03();
	~CFloggerTest003_03();
	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_003_XX_H__)


