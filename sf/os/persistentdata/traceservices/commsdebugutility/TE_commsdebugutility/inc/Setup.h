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
// This is the header file for setting of the flogger.ini file


#if (!defined __SETUP_H__)
#define __SETUP_H__
#include "teststepcomsdbg.h"


class CFloggerTest_Setup : public CTestStepFlogger
	{
	public:
	CFloggerTest_Setup();
	~CFloggerTest_Setup();
	
	virtual enum TVerdict doTestStepL( void );
	};

class CFloggerTest_InitializeIniFile : public CTestStepFlogger
	{
	public:
	CFloggerTest_InitializeIniFile();
	~CFloggerTest_InitializeIniFile();
	
	virtual enum TVerdict doTestStepL( void );
	};


class CFloggerTest_Restore : public CTestStepFlogger
	{
	public:
	CFloggerTest_Restore();
	~CFloggerTest_Restore();
	
	virtual enum TVerdict doTestStepL( void );
	};


#endif //(__SETUP_H__)
