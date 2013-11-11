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
// This is the header file for Flogger test 020.xx


#include <comms-infras/commsdebugutility.h>



#if (!defined __STEP_020_XX_H__)
#define __STEP__020_XX_H__

class CFloggerTest020_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest020_01();
	~CFloggerTest020_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt DoTestWrite(TBool heapTest);
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	};


class CFloggerTest020_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest020_02();
	~CFloggerTest020_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	
	private:
	};
	

class CFloggerTest020_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest020_03();
	~CFloggerTest020_03();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_020_XX_H__)


