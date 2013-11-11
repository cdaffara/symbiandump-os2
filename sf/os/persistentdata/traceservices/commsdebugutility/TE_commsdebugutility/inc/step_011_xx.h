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
// This is the header file for Flogger test 011.xx


#include <comms-infras/commsdebugutility.h>



#if (!defined __STEP_011_XX_H__)
#define __STEP__011_XX_H__

class CFloggerTest011_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest011_01();
	~CFloggerTest011_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestWrite(RFileLogger& aLogger);
	TInt DoHeapTestWrite();
	};


class CFloggerTest011_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest011_02();
	~CFloggerTest011_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	private:
	RFileLogger iFlogger;
	};


class CFloggerTest011_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest011_03();
	~CFloggerTest011_03();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_011_XX_H__)


