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



#ifndef __TESTwchar_H__
#define __TESTwchar_H__

#include <test/testexecutestepbase.h>
#include <wchar.h> 
#include <errno.h>
#include <wchar.h>
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
_LIT(Kwfopensys, "wfopensys");
_LIT(Kwfopenprivate,"wfopenprivate");
_LIT(Kwmkdirsys, "wmkdirsys");
_LIT(Kwmkdirprivate, "wmkdirprivate");
_LIT(Kwaccesssys, "waccesssys");
_LIT(Kwaccessprivate,"waccessprivate");
_LIT(Kwchdirsys,"wchdirsys");
_LIT(Kwchdirprivate,"wchdirprivate");
_LIT(Kwchmodsys,"wchmodsys");
_LIT(Kwchmodprivate,"wchmodprivate");
_LIT(Kwrenamesys,"wrenamesys");
_LIT(Kwrenameprivate,"wrenameprivate");
_LIT(Kwopensys,"wopensys");
_LIT(Kwopenprivate,"wopenprivate");
_LIT(Kwunlinksys,"wunlinksys");
_LIT(Kwunlinkprivate,"wunlinkprivate");
_LIT(Kwstatsys,"wstatsys");
_LIT(Kwstatprivate,"wstatprivate");
_LIT(Kwrmdirsys,"wrmdirsys");
_LIT(Kwrmdirprivate,"wrmdirprivate");

class CTestwchar : public CTestStep
	{
public:
	~CTestwchar(); 
	CTestwchar(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	
	
	
	//Captests for file operations
	TInt wfopensys();
    TInt wfopenprivate();
	TInt wmkdirsys();
	TInt wmkdirprivate();
	TInt waccesssys();
	TInt waccessprivate();
	TInt wchdirsys();
	TInt wchdirprivate();
	TInt wchmodsys();
	TInt wchmodprivate();
	TInt linksys();
	TInt linkprivate();
	TInt wrenamesys();
	TInt wrenameprivate();
	TInt wunlinksys();
	TInt wunlinkprivate();
    TInt wstatsys();
    TInt wstatprivate();
    TInt wrmdirsys();
    TInt wrmdirprivate();	
	TInt wopensys();
	TInt wopenprivate();
	
	};
#endif 

