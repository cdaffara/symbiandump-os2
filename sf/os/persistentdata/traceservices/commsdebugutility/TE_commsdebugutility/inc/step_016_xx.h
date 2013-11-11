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
// This is the header file for Flogger test 016.xx


#include <comms-infras/commsdebugutility.h>


#if (!defined __STEP_016_XX_H__)
#define __STEP__016_XX_H__

class CFloggerTest016_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest016_01();
	~CFloggerTest016_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...);
	
	private:
	RFileLogger iFlogger;

	};

class CFloggerTest016_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest016_02();
	~CFloggerTest016_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL( void );
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	
	TInt DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...);

	private:
	RFileLogger iFlogger;
	};


class CFloggerTest016_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest016_03();
	~CFloggerTest016_03();

	virtual enum TVerdict doTestStepL( void );
	};

#endif //(__STEP_016_XX_H__)


