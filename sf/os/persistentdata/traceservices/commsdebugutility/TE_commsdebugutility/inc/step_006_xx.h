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
// This is the header file for Flogger test 006.xx


#include <comms-infras/commsdebugutility.h>


#if (!defined __STEP_006_XX_H__)
#define __STEP__006_XX_H__

class CFloggerTest006_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest006_01();
	~CFloggerTest006_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect();
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...);
	
	private:
	RFileLogger iFlogger;

	};

class CFloggerTest006_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest006_02();
	~CFloggerTest006_02();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestConnect( void );
	TInt executeStepL( void );
	TInt executeStepL(TBool);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...);
	
	private:
	RFileLogger iFlogger;

	};



class CFloggerTest006_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest006_03();
	~CFloggerTest006_03();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_006_XX_H__)


