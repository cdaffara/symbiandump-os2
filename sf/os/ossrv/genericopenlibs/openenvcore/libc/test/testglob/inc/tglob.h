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
#include <glob.h>

_LIT(KDIRPATH1, "C:\\Glob-Dir\\Test1\\");
_LIT(KDIRPATH2, "C:\\Glob-Dir\\Test2\\");
_LIT(KBASEDIR, "C:\\Glob-Dir\\");

// MACROS
//#define ?macro ?macro_def
#define BUF_SIZE 100

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TInt bufSize = 200;


//utility functions
void MakeSpace(char *);


//literals for glob operations
_LIT(Kglobappend,"globappend");
_LIT(Kglobbrace,	"globbrace");
_LIT(Kglobdooffs,"globdooffs");
_LIT(Kglobmark, 	"globmark");
_LIT(Kglobnocheck,"globnocheck");
_LIT(Kglobnosort, "globnosort");
_LIT(Kglobtilde, "globtilde");

class CTestglob : public CTestStep
	{
public:
	~CTestglob(); 
	CTestglob(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	
	//Glob Test cases
	TInt globappend();
    TInt globbrace();
	TInt globdooffs();
	TInt globmark();
	TInt globnocheck();
	TInt globnosort();
	TInt globtilde();
	RFs iRfs;
	};
#endif 

