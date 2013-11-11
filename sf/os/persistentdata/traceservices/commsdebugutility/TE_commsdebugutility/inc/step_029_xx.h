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

#include <comms-infras/commsdebugutility.h>



#if (!defined __STEP_029_XX_H__)
#define __STEP_029_XX_H__

class CFloggerTest029_IniFile01 : public CTestStepFlogger
	{
	public:
	CFloggerTest029_IniFile01();
	~CFloggerTest029_IniFile01();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};

class CFloggerTest029_IniFile02 : public CTestStepFlogger
	{
	public:
	CFloggerTest029_IniFile02();
	~CFloggerTest029_IniFile02();

	virtual enum TVerdict doTestStepL( void );	
	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};


class CFloggerTest029_IniFile03 : public CTestStepFlogger
	{
	public:
	CFloggerTest029_IniFile03();
	~CFloggerTest029_IniFile03();

	TInt DoTestCheckWriteL();
	
	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};


	
class CFloggerTest029_IniFile04 : public CTestStepFlogger
	{
	public:
	CFloggerTest029_IniFile04();
	~CFloggerTest029_IniFile04();

	virtual enum TVerdict doTestStepL( void );
	TInt DoTestCheckWriteL(TInt aFileStatus);
	TInt DoTestWrite(TInt aStatus);
	TInt executeStepL(TBool heapTest);
	TInt executeStepL();
	private:
	TBuf8<50> iOrgData;
	RFileLogger iFlogger; 
	};


class CFloggerTest029_IniFile04H : public CTestStepFlogger
	{
	public:
	CFloggerTest029_IniFile04H();
	~CFloggerTest029_IniFile04H();

	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__STEP_029_XX_H__)
