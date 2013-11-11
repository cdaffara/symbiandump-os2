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
// This is the header file for Flogger test 024.xx


#include <comms-infras/commsdebugutility.h>



#if (!defined __STEP_024_XX_H__)
#define __STEP__024_XX_H__

class CFloggerTest024_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest024_01();
	~CFloggerTest024_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL(TBool);
	TInt executeStepL();
	TInt DoTestWrite();
	};


class CFloggerTest024_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest024_02();
	~CFloggerTest024_02();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL(TBool);
	TInt executeStepL();
	TInt DoTestWrite();
	};


class CFloggerTest024_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest024_03();
	~CFloggerTest024_03();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL(TBool);
	TInt executeStepL();
	TInt DoTestWrite();
	};


class CFloggerTest024_04 : public CTestStepFlogger
	{
	public:
	CFloggerTest024_04();
	~CFloggerTest024_04();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL(TBool);
	TInt executeStepL();
	TInt DoTestWrite();
	};


class CFloggerTest024_05 : public CTestStepFlogger
	{
	public:
	CFloggerTest024_05();
	~CFloggerTest024_05();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL(TBool);
	TInt executeStepL();
	TInt DoTestWrite();
	};


class CFloggerTest024_06 : public CTestStepFlogger
	{
	public:
	CFloggerTest024_06();
	~CFloggerTest024_06();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL(TBool);
	TInt executeStepL();
	TInt DoTestWrite();
	};


class CFloggerTest024_07 : public CTestStepFlogger
	{
	public:
	CFloggerTest024_07();
	~CFloggerTest024_07();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt connectionTest();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	};


class CFloggerTest024_08 : public CTestStepFlogger
	{
	public:
	CFloggerTest024_08();
	~CFloggerTest024_08();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL(TBool);
	TInt executeStepL();
	TInt DoTestWrite();
	};
	
	


#endif //(__STEP_024_XX_H__)


