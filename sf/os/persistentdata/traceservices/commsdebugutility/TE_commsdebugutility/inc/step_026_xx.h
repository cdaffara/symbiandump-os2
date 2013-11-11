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
// This is the header file for Flogger test 026.xx


#include <comms-infras/commsdebugutility.h>



#if (!defined __STEP_026_XX_H__)
#define __STEP__026_XX_H__

class CFloggerTest026_01 : public CTestStepFlogger
	{
	public:
	CFloggerTest026_01();
	~CFloggerTest026_01();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite(TBool heapTest);
	};



class CFloggerTest026_02 : public CTestStepFlogger
	{
	public:
	CFloggerTest026_02();
	~CFloggerTest026_02();

	virtual enum TVerdict doTestStepL( void );
	};


class CFloggerTest026_03 : public CTestStepFlogger
	{
	public:
	CFloggerTest026_03();
	~CFloggerTest026_03();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL();
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	TInt DoTestWrite();
	};


#endif //(__STEP_026_XX_H__)


