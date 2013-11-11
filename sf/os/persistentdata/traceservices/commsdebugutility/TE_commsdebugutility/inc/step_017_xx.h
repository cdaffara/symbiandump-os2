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
// This is the header file for Flogger test 017.xx


#include <comms-infras/commsdebugutility.h>

#if (!defined __STEP_017_XX_H__)
#define __STEP__017_XX_H__

class CFloggerTest017_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest017_01();
	~CFloggerTest017_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoHeapTestWrite();
	private:
	};

class CFloggerTest017_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest017_02();
	~CFloggerTest017_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL( void );
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	private:
	};


class CFloggerTest017_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest017_03();
	~CFloggerTest017_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	
	private:
	};

class CFloggerTest017_04 : public CTestStepFlogger
	{
	public:
	CFloggerTest017_04();
	~CFloggerTest017_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	
	private:
	};

class CFloggerTest017_05 : public CTestStepFlogger
	{
	public:
	CFloggerTest017_05();
	~CFloggerTest017_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	
	private:
	};


class CFloggerTest017_06 : public CTestStepFlogger
	{
	public:
	CFloggerTest017_06();
	~CFloggerTest017_06();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_017_XX_H__)


