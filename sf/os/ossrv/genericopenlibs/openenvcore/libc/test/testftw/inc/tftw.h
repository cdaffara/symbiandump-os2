/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : tftw.h
* 
*
*/



#ifndef __TESTFTW_H__
#define __TESTFTW_H__

// INCLUDE FILES
#include <test/testexecutestepbase.h>
#include <ftw.h>
#include <errno.h>
#include <sys/stat.h>

_LIT(KTestFtw, "TestFtw");
_LIT(KTestFtw2, "TestFtw2");
_LIT(KTestFtw3, "TestFtw3");
_LIT(KTestFtw4, "TestFtw4");
_LIT(KTestFtw5, "TestFtw5");

class CTestftw : public CTestStep
	{
public:
	~CTestftw(); 
	CTestftw(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt TestFtw();
	TInt TestFtw2();
	TInt TestFtw3();
	TInt TestFtw4();
	TInt TestFtw5();
	
	//util function
	void ReadStringParam(char *);
	int CreateDirs();
	
private: //data
	TInt iParamCnt;
	};
#endif 
