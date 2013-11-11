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
// This is the header file for Flogger test 025.xx


#include <comms-infras/commsdebugutility.h>


#if (!defined __STEP_025_XX_H__)
#define __STEP__025_XX_H__



class CFloggerTest025_BuildTestUdeb1 : public CTestStepFlogger
	{
	public:
	CFloggerTest025_BuildTestUdeb1();
	~CFloggerTest025_BuildTestUdeb1();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};

/* Removed - see cpp file
class CFloggerTest025_BuildTestUdeb2 : public CTestStepFlogger
	{
	public:
	CFloggerTest025_BuildTestUdeb2();
	~CFloggerTest025_BuildTestUdeb2();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};
*/

class CFloggerTest025_BuildTestUdeb3 : public CTestStepFlogger
	{
	public:
	CFloggerTest025_BuildTestUdeb3();
	~CFloggerTest025_BuildTestUdeb3();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};
	
	
class CFloggerTest025_BuildTestUdeb4 : public CTestStepFlogger
	{
	public:
	CFloggerTest025_BuildTestUdeb4();
	~CFloggerTest025_BuildTestUdeb4();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};
	
	


class CFloggerTest025_BuildTestUrel1 : public CTestStepFlogger
	{
	public:
	CFloggerTest025_BuildTestUrel1();
	~CFloggerTest025_BuildTestUrel1();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};

/* Removed - see cpp file
class CFloggerTest025_BuildTestUrel2 : public CTestStepFlogger
	{
	public:
	CFloggerTest025_BuildTestUrel2();
	~CFloggerTest025_BuildTestUrel2();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};
*/

class CFloggerTest025_BuildTestUrel3 : public CTestStepFlogger
	{
	public:
	CFloggerTest025_BuildTestUrel3();
	~CFloggerTest025_BuildTestUrel3();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};
	
	
class CFloggerTest025_BuildTestUrel4 : public CTestStepFlogger
	{
	public:
	CFloggerTest025_BuildTestUrel4();
	~CFloggerTest025_BuildTestUrel4();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL(TBool);
	TInt executeStepL();
	
	};
#endif //(__STEP_025_XX_H__)


