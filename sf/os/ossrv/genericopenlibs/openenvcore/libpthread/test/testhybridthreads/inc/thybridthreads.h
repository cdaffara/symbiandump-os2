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
* Name        : thybridthreads.h
* 
*
*/



#ifndef __THYBRIDTHREADS_H__
#define __THYBRIDTHREADS_H__

#include <test/TestExecuteStepBase.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <e32msgqueue.h>

#define MAXLENGTH 256

_LIT(KTestpthreadjoin, "Testpthreadjoin");
_LIT(KTestpthread_setspecific, "Testpthread_setspecific");

class CTestHybridThreads : public CTestStep
	{
public:
	~CTestHybridThreads(); 
	CTestHybridThreads(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:

	virtual TVerdict Testpthreadjoin (  );
	virtual TVerdict Testpthread_setspecific (  );
	
private:

	// Data
	TInt iParamCnt;
	TInt iBlocked;
	TInt iThreadId;
	};
#endif // __THYBRIDTHREADS_H__
