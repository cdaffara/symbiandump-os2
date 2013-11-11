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
* Name        : tstat.h
* 
*
*/



 /*
* ==============================================================================
*  Name        : tstat.h
*  Part of     : teststat
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

#ifndef __TESTstat_H__
#define __TESTstat_H__

#include <test/testexecutestepbase.h>
#include <sys/stat.h> 
#include <errno.h>
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
_LIT(Kmkdirsys, "mkdirsys");
_LIT(Kmkdirprivate, "mkdirprivate");
_LIT(Klstatsys, "lstatsys");
_LIT(Klstatprivate, "lstatprivate");
_LIT(Kstatsys, "statsys");
_LIT(Kstatprivate,"statprivate");
_LIT(Kmkfifosys,"mkfifosys");
_LIT(Kmkfifoprivate,"mkfifoprivate");
_LIT(Kchmodsys,"chmodsys");
_LIT(Kchmodprivate,"chmodprivate");

class CTeststat : public CTestStep
	{
public:
	~CTeststat(); 
	CTeststat(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	
	
	
	//Captests for file operations
	TInt mkdirsys();
    TInt mkdirprivate();
	TInt lstatsys();
	TInt lstatprivate();
	TInt statsys();
	TInt statprivate();
	TInt mkfifosys();
	TInt mkfifoprivate();
	TInt chmodsys();
	TInt chmodprivate();

	
	
	};
#endif 

