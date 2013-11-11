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
// This is the header file for Flogger test 012.xx


#include <comms-infras/commsdebugutility.h>

#if (!defined __STEP_012_XX_H__)
#define __STEP__012_XX_H__

class CFloggerTest012_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest012_01();
	~CFloggerTest012_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoHeapTestWrite();

	};


class CFloggerTest012_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest012_02();
	~CFloggerTest012_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL( void );
	TInt executeStepL(TBool);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	private:
	RFileLogger iFlogger;
	};

class CFloggerTest012_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest012_03();
	~CFloggerTest012_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL( void );
	TInt executeStepL( TBool );
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	};


class CFloggerTest012_04 : public CTestStepFlogger
	{
public:
	CFloggerTest012_04();
	~CFloggerTest012_04();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_012_XX_H__)


