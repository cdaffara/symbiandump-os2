/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TESTBACKENDDES_H__
#define __TESTBACKENDDES_H__

#include <test/testexecutestepbase.h>

_LIT(KCLSIDesTest, "CLSIDesTest");

class CTestBackendDes : public CTestStep
	{
public:
	~CTestBackendDes(); 
	CTestBackendDes(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostAmbleL();
private:
	TInt CLSIDesTest(void);
	
	//utility methods
	void ReadIntParam(TInt &aInt);

private: //data
	TInt iParamCnt;
	};
#endif 
