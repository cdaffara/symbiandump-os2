
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
// Name        : tstat_captest.cpp
// 
//

#include "tstat.h"
#include <sys/stat.h>
#include <e32err.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LEN  20

// ============================ MEMBER FUNCTIONS ===============================


/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::mkdirprivate
API Tested			: mkdirprivate for capability

-------------------------------------------------------------------------------
*/

TInt CTeststat::mkdirprivate()
	{
    int i;
  	i=mkdir("z:\\private\\10202be9\\temp",S_IWUSR);
  	
	if  (i < 0) 
		{
		
			INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::mkdirsys
API Tested			: mkdirsys

-------------------------------------------------------------------------------
*/

TInt CTeststat::mkdirsys()
	{
	int i;
  	i=mkdir("z:\\sys\\temp",S_IWUSR);
  	
	if  (i < 0) 
		{
		
			INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::lstatsys
API Tested			: lstatsys for capability

-------------------------------------------------------------------------------
*/

TInt CTeststat::lstatsys()
	{
	int i;
 	struct stat buf;
  	i =lstat("z:\\sys\\file7.txt"  , &buf);
  	
	  if  (i < 0) 
		{
		
				INFO_PRINTF2(_L("errno=%d\n"),errno);
         		return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::lstatprivate
API Tested			: lstatprivate for capability

-------------------------------------------------------------------------------
*/

TInt CTeststat::lstatprivate()
	{
	int i;
    struct stat buf;
   	i =lstat("z:\\private\\10202be9\\file7.txt"  , &buf);

	if  (i < 0) 
		{
	        
			INFO_PRINTF2(_L("errno=%d\n"),errno);
          	return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::statsys
API Tested			: statsys for capability

-------------------------------------------------------------------------------
*/

TInt CTeststat::statsys()
	{
	int i;
  	struct stat buf;
    i =stat("z:\\sys\\file8.txt"  , &buf);
    
	if  (i < 0) 
		{
	        
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::statprivate
API Tested			: statprivate for capability

-------------------------------------------------------------------------------
*/

TInt CTeststat::statprivate()
	{
    int i;
  	struct stat buf;
   	i =stat("z:\\private\\10202be9\\file8.txt"  , &buf);
   	
	if  (i < 0) 
		{
			INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::mkfifo
API Tested			: mkfifo for capability
T
-------------------------------------------------------------------------------
*/

TInt CTeststat::mkfifosys()
	{
    int i;
  	const char *path="z:\\sys\\file9.txt";
	i=mkfifo(path,0666 );
	
   	if  (i < 0) 
		{
		
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::mkfifoprivate
API Tested			: mkfifoprivate

-------------------------------------------------------------------------------
*/

TInt CTeststat::mkfifoprivate()
	{
    int i;
  	const char *path="z:\\private\\10202be9\\file9.txt";
	i=mkfifo(path,0666 );
   
	if  (i < 0) 
		{
			INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	
  
/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::chmodsys
API Tested			: chmodsys for capability

-------------------------------------------------------------------------------
*/

TInt CTeststat::chmodsys()
	{
    int i;
  	const char *path="z:\\sys\\file10.txt";
	i = chmod(path, S_IRUSR|S_IRGRP|S_IROTH);
	
	if  (i < 0) 
		{
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTeststat::chmodprivate
API Tested			: chmod private for capability

-------------------------------------------------------------------------------
*/

TInt CTeststat::chmodprivate()
	{
    int i;
  	const char *path="z:\\private\\10202be9\\file10.txt";
	i = chmod(path, S_IRUSR|S_IRGRP|S_IROTH);
	
	if  (i < 0) 
		{
			INFO_PRINTF2(_L("errno=%d\n"),errno);
        	return errno;
		}
	return KErrNone;
	}
	

//ENDFILE
