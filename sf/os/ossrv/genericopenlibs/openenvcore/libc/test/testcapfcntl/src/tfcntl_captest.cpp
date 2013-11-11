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


 
#include "tfcntl.h"
#include <unistd.h>
#include <e32err.h>
#include <sys/stat.h>
#include <stdio.h>

#define MAX_LEN  20

// ============================ MEMBER FUNCTIONS ===============================


/*
-------------------------------------------------------------------------------
Function Name		: CTestfcntl::opensys
API Tested			: opensys for capability

-------------------------------------------------------------------------------
*/

TInt CTestfcntl::opensys()
	{
	INFO_PRINTF1(_L("opensys"));
    int i;
  	i=open("z:\\sys\\file.txt",O_RDONLY);
  	
	if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}

	
/*
-------------------------------------------------------------------------------
Function Name		: CTestfcntl::openprivate
API Tested			: openprivate

-------------------------------------------------------------------------------
*/

TInt CTestfcntl::openprivate()
	{
    int i;
  	i=open("z:\\private\\10202be9\\file.txt",O_RDONLY);
  	
    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;	
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestfcntl::creatsys
API Tested			: creatsys for capability

-------------------------------------------------------------------------------
*/

TInt CTestfcntl::creatsys()
	{
    int i;
	mode_t mode = S_IWUSR | S_IRGRP | S_IROTH;
	char *filename = "z:\\sys\\file1.txt";
	i = creat(filename,mode);
	
	if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestfcntl::creatprivate
API Tested			: creatprivate

-------------------------------------------------------------------------------
*/

TInt CTestfcntl::creatprivate()
	{
    mode_t mode = S_IWUSR | S_IRGRP | S_IROTH;
    int i;
   	char *filename="z:\\private\\10202be9\\file1.txt";
  	i = creat(filename,mode);

  	if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	
	





























