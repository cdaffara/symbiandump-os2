/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tunistd.h
* 
*
*/



 /*
* ==============================================================================
*  Name        : tunistd.h
*  Part of     : testunistd
*
*  Description : ?Description
*  Version:   0.5
*
*  Copyright (C) 2002 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing,  adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
*
* ============================================================================
*/

#ifndef __TESTUNISTD_H__
#define __TESTUNISTD_H__

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
_LIT(Kaccesssys, "accesssys");
_LIT(Kaccessprivate, "accessprivate");
_LIT(Krmdirsys, "rmdirsys");
_LIT(Krmdirprivate, "rmdirprivate");
_LIT(Kunlinksys, "unlinksys");
_LIT(Kunlinkprivate,"unlinkprivate");
_LIT(Kreadlinksys,"readlinksys");
_LIT(Kreadlinkprivate,"readlinkprivate");
_LIT(Ksymlinksys,"symlinksys");
_LIT(Ksymlinkprivate,"symlinkprivate");
_LIT(Klinksys,"linksys");
_LIT(Klinkprivate,"linkprivate");
_LIT(Kchdirsys,"chdirsys");
_LIT(Kchdirprivate,"chdirprivate");

class CTestunistd : public CTestStep
	{
public:
	~CTestunistd(); 
	CTestunistd(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	
	
	
	//Captests for file operations
	TInt accesssys();
    TInt accessprivate();
	TInt rmdirsys();
	TInt rmdirprivate();
	TInt unlinksys();
	TInt unlinkprivate();
	TInt readlinksys();
	TInt readlinkprivate();
	TInt symlinksys();
	TInt symlinkprivate();
	TInt linksys();
	TInt linkprivate();
	TInt chdirsys();
	TInt chdirprivate();
	
	
	
	};
#endif 

