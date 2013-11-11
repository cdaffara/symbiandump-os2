
// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : tstdio_captest.cpp
// 
//


#include "tstdio.h"
#include <stdio.h>
#include <e32err.h>
#include <sys/stat.h>

#define MAX_LEN  20

// ============================ MEMBER FUNCTIONS ===============================



	

/*--------------------- STREAM-RELATED OPERATIONS ---------------------------*/


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fopensysread
API Tested			: fopensysread

-------------------------------------------------------------------------------
*/

TInt CTestStdio::fopensysread()
	{
	
	FILE *fp;	
	char name[20] = "z:\\sys\\fopen1.txt";
	fp = fopen (name, "r");
	if (fp==NULL) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
   	    return errno;
		}
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fopensyswrite
API Tested			: fopensyswrite

-------------------------------------------------------------------------------
*/

TInt CTestStdio::fopensyswrite()
	{
	FILE *fp;	
	char name[20] = "z:\\sys\\fopen1.txt";
	fp = fopen (name, "w");
	if (fp==NULL)
		{
	
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
   	    return errno;
		}
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fopenprivatewrite
API Tested			: fopenprivatewrite

-------------------------------------------------------------------------------
*/

TInt CTestStdio::fopenprivatewrite()
	{
	FILE *fp;	
	fp = fopen ("z:\\private\\10202be9\\fopen1.txt", "w");
	if (fp==NULL)
		{
		
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
   		return errno;
	   	}
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fopenprivateread
API Tested			: fopenprivateread

-------------------------------------------------------------------------------
*/

TInt CTestStdio::fopenprivateread()
	{
	FILE *fp;	
	fp = fopen ("z:\\private\\10202be9\\fopen1.txt", "r");
	if (fp == NULL) 
		{
		
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
   	    return errno;
		}
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::renamesys
API Tested			: renamesys

-------------------------------------------------------------------------------
*/

TInt CTestStdio::renamesys()
	{
	int i;
	i=rename("z:\\sys\\file1.txt","z:\\sys\\file2.txt");
	
	if  (i == -1) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
   	    return errno;
		}
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::renameprivate
API Tested			: renameprivate

-------------------------------------------------------------------------------
*/

TInt CTestStdio::renameprivate()
	{
    int i;
    i=rename("z:\\private\\10202be9\\file1.txt","z:\\private\\10202be9\\file2.txt");
    
	if  (i < 0) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::removesys
API Tested			:removesys

-------------------------------------------------------------------------------
*/

TInt CTestStdio::removesys()
	{
    int i;
    i=remove("z:\\sys\\file6.txt");
    
    if  (i < 0) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        
        return errno;	    
		}
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::removeprivate
API Tested			: removeprivate

-------------------------------------------------------------------------------
*/

TInt CTestStdio::removeprivate()
	{
    int i;
    i=remove("z:\\private\\10202be9\\file6.txt");
    
    if  (i < 0) 
		{
		
	
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		
		}
	return KErrNone;
	}
	
	

