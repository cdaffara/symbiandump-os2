/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TESTFCNTL_H__
#define __TESTFCNTL_H__

#include <test/testexecutestepbase.h>
#include <unistd.h> 
#include <errno.h>
#include <unistd.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>

#include <pthread.h>
#include <semaphore.h>

#include <stdarg.h>



// MACROS
//#define ?macro ?macro_def
#define BUF_SIZE 100

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TInt bufSize = 200;


//utility functions
void MakeSpace(char *);



//literals for file operations
_LIT(Kopensys, "opensys");
_LIT(Kopenprivate, "openprivate");
_LIT(Kcreatsys, "creatsys");
_LIT(Kcreatprivate, "creatprivate");


class CTestfcntl : public CTestStep
	{
public:
	~CTestfcntl(); 
	CTestfcntl(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	
	
	
	//Captests for file operations
	TInt opensys();
    TInt openprivate();
	TInt creatsys();
	TInt creatprivate();

	
	
	};
#endif 

