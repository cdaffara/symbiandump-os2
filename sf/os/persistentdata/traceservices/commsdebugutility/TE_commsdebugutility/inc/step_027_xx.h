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
// See the test specification for details of what these test cases do.

#include <comms-infras/commsdebugutility.h>


#if (!defined __STEP_027_XX_H__)
#define __STEP__027_XX_H__

class CFloggerTest027_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_01();
	~CFloggerTest027_01();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};


class CFloggerTest027_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_02();
	~CFloggerTest027_02();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	RFileLogger iFlogger;
	
	};

class CFloggerTest027_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_03();
	~CFloggerTest027_03();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};


class CFloggerTest027_04 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_04();
	~CFloggerTest027_04();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};

class CFloggerTest027_05 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_05();
	~CFloggerTest027_05();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};


class CFloggerTest027_06 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_06();
	~CFloggerTest027_06();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};

class CFloggerTest027_07 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_07();
	~CFloggerTest027_07();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};

class CFloggerTest027_08 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_08();
	~CFloggerTest027_08();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};

class CFloggerTest027_09 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_09();
	~CFloggerTest027_09();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};

class CFloggerTest027_10 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_10();
	~CFloggerTest027_10();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};

class CFloggerTest027_11 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_11();
	~CFloggerTest027_11();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};


class CFloggerTest027_12 : public CTestStepFlogger
	{
	public:
	CFloggerTest027_12();
	~CFloggerTest027_12();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};




#endif //(__STEP_027_XX_H__)
