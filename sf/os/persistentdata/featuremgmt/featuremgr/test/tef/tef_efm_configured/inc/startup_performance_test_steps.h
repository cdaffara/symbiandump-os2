// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EFM test steps for startup performance testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __STARTUPPERFORMANCETESTSTEPS__
#define __STARTUPPERFORMANCETESTSTEPS__

#include <test/testexecutestepbase.h>

_LIT(KStartupPerformanceTest, "StartupPerformanceTest");
class CStartupPerformanceTestStep : public CTestStep
   {
public:
	CStartupPerformanceTestStep();
	virtual TVerdict doTestStepL(void);
   };

#endif
