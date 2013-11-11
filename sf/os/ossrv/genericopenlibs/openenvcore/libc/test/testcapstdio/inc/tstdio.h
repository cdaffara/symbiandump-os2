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
* Name        : tstdio.h
* 
*
*/



 /*
* ==============================================================================
*  Name        : tstdio.h
*  Part of     : teststdio
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

#ifndef __TESTSTDIO_H__
#define __TESTSTDIO_H__

#include <test/testexecutestepbase.h>
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
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
_LIT(Kfopensys1, "fopensysread");
_LIT(Kfopensys2, "fopensyswrite");
_LIT(Kfopenprivate1, "fopenprivateread");
_LIT(Kfopenprivate2, "fopenprivatewrite");
_LIT(Krenamesys, "renamesys");
_LIT(Krenameprivate,"renameprivate");
_LIT(Kremovesys,"removesys");
_LIT(Kremoveprivate,"removeprivate");

class CTestStdio : public CTestStep
	{
public:
	~CTestStdio(); 
	CTestStdio(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	
	
	
	//Captests for file operations
	TInt fopensysread();
    TInt fopensyswrite();
	TInt fopenprivateread();
	TInt fopenprivatewrite();
	TInt renamesys();
	TInt renameprivate();
	TInt removesys();
	TInt removeprivate();

	};
#endif 

