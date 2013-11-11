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
// This is the header file for Flogger test 015.xx


#include <comms-infras/commsdebugutility.h>



#if (!defined __STEP_015_XX_H__)
#define __STEP__015_XX_H__

class CFloggerTest015_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest015_01();
	~CFloggerTest015_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt executeStepL(TBool heapTest);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	};


class CFloggerTest015_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest015_02();
	~CFloggerTest015_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL( void );
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	private:
	RFileLogger iFlogger;
	};

class CFloggerTest015_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest015_03();
	~CFloggerTest015_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL( void );
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();

	};


class CFloggerTest015_04 : public CTestStepFlogger
	{
	public:
	CFloggerTest015_04();
	~CFloggerTest015_04();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_015_XX_H__)


