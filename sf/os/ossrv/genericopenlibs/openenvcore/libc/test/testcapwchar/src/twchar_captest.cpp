
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
// Name        : twchar_captest.cpp
// 
//


#include "twchar.h"
#include <wchar.h>
#include <e32err.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LEN  20

// ============================ MEMBER FUNCTIONS ===============================


/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wfopensys()
API Tested			: fopensys for capability

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wfopensys()
	{
	INFO_PRINTF1(_L("wfopensys"));
   
    FILE *fp;
    wchar_t* name=L"z:\\sys\\wfile.txt";
    fp = wfopen (name, L"r");
    if (fp == NULL)
		{
        	
			INFO_PRINTF2(_L("errno=%d\n"),errno);
      		return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wfopenprivate
API Tested			: wfopenprivate 

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wfopenprivate()
	{
	INFO_PRINTF1(_L("wfopenprivate"));
    FILE *fp;
    wchar_t* name=L"z:\\private\\10202be9\\wfile.txt";
    fp = wfopen (name, L"r");
    if (fp == NULL)
    	{
			INFO_PRINTF2(_L("errno=%d\n"),errno);
      		return errno;
		}
		return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wmkdirsys()
API Tested			: wmkdirsys for capability
-------------------------------------------------------------------------------
*/

TInt CTestwchar::wmkdirsys()
	{
	int i;
 	i = wmkdir(L"z:\\sys\\wtemp", S_IRWXU); 
    
    if  (i < 0) 
		{
	     
			INFO_PRINTF2(_L("errno=%d\n"),errno);
        	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wmkdirprivate()
API Tested			: wmkdirprivate

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wmkdirprivate()
	{
    int i;
  	i = wmkdir(L"z:\\private\\10202be9\\wtemp", S_IRWXU); 
  	
  	if  (i < 0) 
		{
	        
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::waccesssys
API Tested			: waccesssys

-------------------------------------------------------------------------------
*/

TInt CTestwchar::waccesssys()
	{
    int i;
    i=waccess(L"z:\\sys\\wfile.txt",F_OK);

	if  (i < 0) 
		{

			INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::waccessprivate()
API Tested			: waccessprivate

-------------------------------------------------------------------------------
*/

TInt CTestwchar::waccessprivate()
	{
	int i;
  	i=waccess(L"z:\\private\\10202be9\\wfile.txt",F_OK);

	if  (i < 0) 
		{
	        
			INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wchdirsys()
API Tested			: wchdirsys

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wchdirsys()
	{
	int i;
    i = wchdir(L"z:\\sys\\wtemp1");

	if  (i < 0) 
		{
		
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar:: wchdirprivate()
API Tested			: wchdirprivate

-------------------------------------------------------------------------------
*/

TInt CTestwchar:: wchdirprivate()
	{
    int i;
  	i = wchdir(L"z:\\private\\10202be9\\wtemp1");

	if  (i < 0) 
		{
	        
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
          	return errno;
		}
	return KErrNone;
	}
	
  
/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wchmodsys()
API Tested			: wchmodsys

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wchmodsys()
	{
	int i;
	i =wchmod(L"z:\\sys\\wfile.txt", S_IWUSR);
	
	if  (i < 0) 
		{
	        
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wchmodprivate()
API Tested			: wchmodprivate

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wchmodprivate()
	{
	int i;
 	i =wchmod(L"z:\\private\\10202be9\\wfile.txt", S_IWUSR);
	
	if  (i < 0) 
		{
	        
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wrenamesys()
API Tested			: wrenamesys

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wrenamesys()
	{
    int i;
    i=wrename(L"z:\\sys\\wfile1.txt" , L"z:\\sys\\wfile2.txt");

	if  (i < 0) 
		{
	        
			INFO_PRINTF2(_L("errno=%d\n"),errno);
      		return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar:: wrenameprivate()
API Tested			: wrenameprivate

-------------------------------------------------------------------------------
*/

TInt CTestwchar:: wrenameprivate()
	{
	int i;
	i=wrename(L"z:\\private\\10202be9\\wfile1.txt" , L"z:\\private\\10202be9\\wfile2.txt");
	
	if  (i < 0) 
		{
	        
			INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wopensys()
API Tested			: wopensys

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wopensys()
	{
	int i;
  	i=wopen(L"z:\\sys\\wfile.txt",O_RDONLY);
  	
  	if  (i < 0) 
		{
	        
			INFO_PRINTF2(_L("errno=%d\n"),errno);
        	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wopenprivate()
API Tested			: wopenprivate

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wopenprivate()
	{
    int i;
  	i=wopen(L"z:\\private\\10202be9\\wfile.txt",O_RDONLY);
  	
  	if  (i < 0) 
		{
	        
			INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wunlinksys()
API Tested			: wunlinksys

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wunlinksys()
	{
    int i;
  	i=wunlink(L"z:\\sys\\wfile3.txt");
  	
  	if  (i < 0) 
		{
	        
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wunlinkprivate()
API Tested			: wunlinkprivate
-------------------------------------------------------------------------------
*/

TInt CTestwchar::wunlinkprivate()
	{
    int i;
  	i=wunlink(L"z:\\private\\10202be9\\wfile3.txt");

	if  (i < 0) 
		{
	        
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		}
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wstatsys()
API Tested			: wstatsys

-------------------------------------------------------------------------------
*/

TInt CTestwchar::wstatsys()
	{
    int i;
    struct stat buf;
   	i= wstat(L"z:\\sys\\wfile.txt"  , &buf) ;

	if  (i < 0) 
		{
	        
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar:: wstatprivate()
API Tested			: wstatprivate

-------------------------------------------------------------------------------
*/

TInt CTestwchar:: wstatprivate()
	{
    struct stat buf;
   	if(wstat(L"z:\\private\\10202be9\\wfile.txt"  , &buf) < 0 )
   		{
   	
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar:: wrmdirsys()
API Tested			:  wrmdirsys

-------------------------------------------------------------------------------
*/

TInt CTestwchar::  wrmdirsys()
	{
	int i;
	i = wrmdir(L"z:\\sys\\wtemp1");
 	if  (i < 0) 
		{
	        
			INFO_PRINTF2(_L("errno=%d\n"),errno);
          	return errno;
		}
	return KErrNone;
	}

	
/*
-------------------------------------------------------------------------------
Function Name		: CTestwchar::wrmdirprivate()
API Tested			: wrmdirprivate

-------------------------------------------------------------------------------
*/

TInt CTestwchar::  wrmdirprivate()
	{
	int i;
 	i = wrmdir(L"z:\\private\\10202be9\\wtemp1");

	if  (i < 0) 
		{
	        
		    INFO_PRINTF2(_L("errno=%d\n"),errno);
            return errno;
		}
	return KErrNone;
	}

























