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
// This is the header file for Flogger test 005.xx


#include <comms-infras/commsdebugutility.h>

#if (!defined __STEP_008_XX_H__)
#define __STEP__008_XX_H__

class CFloggerTest008_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest008_01();
	~CFloggerTest008_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	private:
	RFileLogger iFlogger;

	};

class CFloggerTest008_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest008_02();
	~CFloggerTest008_02();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect( void );
	TInt executeStepL( void );
	TInt executeStepL(TBool);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	private:
	RFileLogger iFlogger;

	};



class CFloggerTest008_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest008_03();
	~CFloggerTest008_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt executeStepL(TBool);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestConnect( void );

	private:
	RFileLogger iFlogger;

	};


class CFloggerTest008_04 : public CTestStepFlogger
	{
	public:
	CFloggerTest008_04();
	~CFloggerTest008_04();

	virtual enum TVerdict doTestStepL( void );

	};


#endif //(__STEP_008_XX_H__)


