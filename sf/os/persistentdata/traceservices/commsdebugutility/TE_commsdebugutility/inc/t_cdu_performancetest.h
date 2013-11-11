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

#if (!defined __CDU_PERFORMANCE_TEST_H__)
#define __CDU_PERFORMANCE_TEST_H__

#include "teststepcomsdbg.h"

class CCDUPerformanceTest : public CTestStepFlogger
	{
public:
	CCDUPerformanceTest();
	~CCDUPerformanceTest();

	virtual enum TVerdict doTestStepL( void );
	TInt DoCDUPerformanceTestL();
	TInt executeStepL();
#ifndef __WINS__
private:
	TInt GetLogFileSizeL();
	void DeleteLogFileL();
#endif // 	__WINS__
	};
	
#endif // __CDU_PERFORMANCE_TEST_H__
