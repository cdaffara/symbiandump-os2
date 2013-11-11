/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TEST_H__
#define __TEST_H__

#include <test/testexecutestepbase.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <sys/msg.h>
#include <sys/sem.h>



_LIT(KsetjmpTest, "setjmpTest");


class CSetjmpTest : public CTestStep
	{
public:
	~CSetjmpTest(); 
	CSetjmpTest(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TVerdict setjmpTest();
	void CopyTDescToString(const TDesC& aSource, char* aDestination);
	TVerdict CompareToExpectedValue(int gv,int& av, int rv, int vv, int sv);
	static void* threadfunction(void* para);
	jmp_buf                 iJmpBuffer[2];
	TBool                   iCreateThread;
	TBool                   iCreateProcess;
	TBool                   iCallSetjmpTwice;
	TInt                    iLongJmpBufIndex;
	TPtrC                   iWorkerProcessName;
	};
#endif 

