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
// This is the header file for Flogger test 010.xx


#include <comms-infras/commsdebugutility.h>

#if (!defined __STEP_010_XX_H__)
#define __STEP__010_XX_H__

class CFloggerTest010_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest010_01();
	~CFloggerTest010_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	private:
	RFileLogger iFlogger;

	};

class CFloggerTest010_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest010_02();
	~CFloggerTest010_02();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect( void );
	TInt executeStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	private:
	RFileLogger iFlogger;

	};



class CFloggerTest010_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest010_03();
	~CFloggerTest010_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt executeStepL(TBool heapTest);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestConnect( void );

	private:
	RFileLogger iFlogger;
	};

class CFloggerTest010_04 : public CTestStepFlogger
	{
	public:
	CFloggerTest010_04();
	~CFloggerTest010_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt executeStepL(TBool heapTest);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestConnect( void );

	private:
	RFileLogger iFlogger;
	};
class CFloggerTest010_05 : public CTestStepFlogger
	{
	public:
	CFloggerTest010_05();
	~CFloggerTest010_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestConnect( void );

	private:
	RFileLogger iFlogger;
	};

class CFloggerTest010_06 : public CTestStepFlogger
	{
	public:
	CFloggerTest010_06();
	~CFloggerTest010_06();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_010_XX_H__)


