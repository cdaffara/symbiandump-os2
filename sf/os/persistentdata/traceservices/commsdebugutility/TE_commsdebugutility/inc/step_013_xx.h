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
// This is the header file for Flogger test 013.xx


#include <comms-infras/commsdebugutility.h>


#if (!defined __STEP_013_XX_H__)
#define __STEP__013_XX_H__

class CFloggerTest013_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest013_01();
	~CFloggerTest013_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...);

	private:
	};

class CFloggerTest013_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest013_02();
	~CFloggerTest013_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt DoTestWrite();
	TInt DoTestCheckWriteL();
	TInt DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...);

	private:
	
	};



class CFloggerTest013_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest013_03();
	~CFloggerTest013_03();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_013_XX_H__)


