/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef __ULOGGERSYSSTARTTEST_H__
#define __ULOGGERSYSSTARTTEST_H__

#include <test/tefunit.h>

#include "uloggersysstarter.h"
#include "te_sysstartersuitestepbase.h"
	
class CSysStart0Step : public CTe_sysstartSuiteStepBase
	{
public:
	CSysStart0Step();
	~CSysStart0Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepPreambleL(); 
	
private:
	
	// Tests
	void TestNewL(); // This test is fine
	void TestStart(); 
	
	};

_LIT(KSysStart0Step,"sysStart0Step");

#endif // __ULOGGERSYSSTARTTEST_H__
