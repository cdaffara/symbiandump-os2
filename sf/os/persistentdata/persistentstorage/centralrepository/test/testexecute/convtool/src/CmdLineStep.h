// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
 
#ifndef __CMDLINESTEP_H__
#define __CMDLINESTEP_H__

#include <test/testexecutestepbase.h>

_LIT(KCmdLineTestStep, "CmdLineTestStep");


class CCmdLineTestStep : public CTestStep
	{
public:
	CCmdLineTestStep();
	~CCmdLineTestStep() {}
	virtual TVerdict doTestStepL(void);
	void InitialiseServerResourcesL(TInt& aNumPushed);

private:
	void GetCmdLineL();

private:
	TPtrC iCmdLine;
	TPtrC iOutFileName;
	TPtrC iInFileName;
	TBool iExpectPass;
	};

#endif
