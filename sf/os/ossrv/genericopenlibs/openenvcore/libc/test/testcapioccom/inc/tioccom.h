/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


 /*
* ==============================================================================
*  Name        : tioccom.h
*  Part of     : testioccom
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

#ifndef __TESTioccom_H__
#define __TESTioccom_H__

#include <test/testexecutestepbase.h>
#include <sys/ioccom.h> 
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
_LIT(Kioctlsocket, "ioctlsocket");

class CTestioccom : public CTestStep
	{
public:
	~CTestioccom(); 
	CTestioccom(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	
	
	
	//Captests for file operations
	TInt ioctlsocket();
  
	
	
	};
#endif 

