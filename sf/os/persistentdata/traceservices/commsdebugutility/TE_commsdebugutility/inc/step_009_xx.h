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
// This is the header file for Flogger test 009.xx


#include <comms-infras/commsdebugutility.h>


#if (!defined __STEP_009_XX_H__)
#define __STEP__009_XX_H__

class CFloggerTest009_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest009_01();
	~CFloggerTest009_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...);
	
	private:
	RFileLogger iFlogger;

	};

class CFloggerTest009_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest009_02();
	~CFloggerTest009_02();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect( void );
	TInt executeStepL( void );
	TInt executeStepL(TBool);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	
	TInt DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...);

	private:
	RFileLogger iFlogger;
	};



class CFloggerTest009_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest009_03();
	~CFloggerTest009_03();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_009_XX_H__)


