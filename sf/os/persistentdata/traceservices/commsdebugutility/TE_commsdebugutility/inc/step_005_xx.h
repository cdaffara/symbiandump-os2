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


#if (!defined __STEP_005_XX_H__)
#define __STEP__005_XX_H__

class CFloggerTest005_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest005_01();
	~CFloggerTest005_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect();
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	private:
	RFileLogger iFlogger;
	};

class CFloggerTest005_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest005_02();
	~CFloggerTest005_02();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect( void );
	TInt executeStepL( void );
	TInt executeStepL(TBool);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	
	private:
	RFileLogger iFlogger;
	};


class CFloggerTest005_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest005_03();
	~CFloggerTest005_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt executeStepL(TBool);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestConnect( void );
	
	private:
	RFileLogger iFlogger;

	};


class CFloggerTest005_04 : public CTestStepFlogger
	{
	public:
	CFloggerTest005_04();
	~CFloggerTest005_04();
	virtual enum TVerdict doTestStepL( void );

	};


#endif //(__STEP_005_XX_H__)


