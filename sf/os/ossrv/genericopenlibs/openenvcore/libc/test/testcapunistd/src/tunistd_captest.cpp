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
// Name        : tunistd_captest.cpp
// 
//


#include "tunistd.h"
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#define MAX_LEN  20

// ============================ MEMBER FUNCTIONS ===============================



/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::accessprivate
API Tested			: accessprivate

-------------------------------------------------------------------------------
*/

TInt CTestunistd::accessprivate()
	{
		int i;
  		i=access("z:\\private\\10202be9\\ufile.txt",F_OK);
  		
    	if  (i < 0 ) 
			{
	   		INFO_PRINTF2(_L("errno=%d\n"),errno);
        	return errno;
			}
        return KErrNone;
	}

	
/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::accesssys
API Tested			:accesssys for capability

-------------------------------------------------------------------------------
*/

TInt CTestunistd::accesssys()
	{
	int i;
	i=access("z:\\sys\\ufile.txt",F_OK);
	
    if  (i < 0  ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::rmdirsys
API Tested			: rmdirsys for capability

-------------------------------------------------------------------------------
*/

TInt CTestunistd::rmdirsys()
	{
	int i;
    i=rmdir("z:\\sys\\temp1");

    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}

	
/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::rmdirprivate
API Tested			:rmdirprivate for capability

-------------------------------------------------------------------------------
*/

TInt CTestunistd::rmdirprivate()
	{
	int i;
 	i=rmdir("z:\\private\\10202be9\\temp1");
 	
    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}

	
/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::unlinksys
API Tested			: unlinksys for capability
-------------------------------------------------------------------------------
*/

TInt CTestunistd::unlinksys()
	{
	int i;
 	i=unlink("z:\\sys\\file3.txt");
 	
    if  (i < 0 ) 
     	{
   	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::unlinkprivate
API Tested			: unlinkprivate for capability

-------------------------------------------------------------------------------
*/

TInt CTestunistd::unlinkprivate()
	{
	int i;
  	i=unlink("z:\\private\\10202be9\\file3.txt");
  	
    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::linksys
API Tested			: linksys for capability

-------------------------------------------------------------------------------
*/

TInt CTestunistd::linksys()
	{
	int i;
      char rdbuff[25];
     i= link("z:\\sys\\ufile3.txt","z:\\sys\\Link3");

    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
    i=readlink("z:\\sys\\Link3", rdbuff, (sizeof(char)*25)); 
    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
   	i=unlink("z:\\sys\\Link3");
    if  (i < 0 ) 
     	{
   	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
		

/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::fopensysread
API Tested			: fopen
TestCase Description: Create a file using fopen() using absolute path and write
					  something in to it. Then close it. Open the same file 
					  again in append mode and write something in to it
-------------------------------------------------------------------------------
*/

TInt CTestunistd::linkprivate()
	{
	int i;
  	 char rdbuff[25];
  	i=link("z:\\private\\10202be9\\ufile3.txt","z:\\private\\10202be9\\link2");
    if  (i < 0 ) 
		{
  	 INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
    i=readlink("z:\\sys\\Link2", rdbuff, (sizeof(char)*25)); 
    
    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	i=unlink("z:\\private\\10202be9\\file3.txt");
    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	
  
/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::fopensysread
API Tested			: fopen
TestCase Description: Create a file using fopen() using absolute path and write
					  something in to it. Then close it. Open the same file 
					  again in append mode and write something in to it
-------------------------------------------------------------------------------
*/

TInt CTestunistd::chdirsys()
	{
    int i;
    i= chdir("z:\\sys\\temp1");

    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
   	    return errno;
		}
	return KErrNone;
	}
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::fopensysread
API Tested			: fopen
TestCase Description: Create a file using fopen() using absolute path and write
					  something in to it. Then close it. Open the same file 
					  again in append mode and write something in to it
-------------------------------------------------------------------------------
*/

TInt CTestunistd::chdirprivate()
	{
    int i;
    i=chdir("z:\\private\\10202be9\\temp1") ;
    
    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::fopensysread
API Tested			: fopen
TestCase Description: Create a file using fopen() using absolute path and write
					  something in to it. Then close it. Open the same file 
					  again in append mode and write something in to it
-------------------------------------------------------------------------------
*/

TInt CTestunistd::readlinksys()
	{
	int i;
    char rdbuff[25];
    i=readlink("z:\\sys\\Link1", rdbuff, (sizeof(char)*25)); 
    
    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
   	    return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::fopensysread
API Tested			: fopen
TestCase Description: Create a file using fopen() using absolute path and write
					  something in to it. Then close it. Open the same file 
					  again in append mode and write something in to it
-------------------------------------------------------------------------------
*/

TInt CTestunistd::readlinkprivate()
	{
    int i;
    char rdbuff[25];
    i=readlink("z:\\private\\10202be9\\Link1", rdbuff, (sizeof(char)*25)); 
    
    if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::fopensysread
API Tested			: fopen
TestCase Description: Create a file using fopen() using absolute path and write
					  something in to it. Then close it. Open the same file 
					  again in append mode and write something in to it
-------------------------------------------------------------------------------
*/

TInt CTestunistd::symlinksys()
	{
	int i;
  	i=symlink("z:\\sys\\ufile.txt","z:\\Link2");
  	
    if  (i < 0) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestunistd::fopensysread
API Tested			: fopen
TestCase Description: Create a file using fopen() using absolute path and write
					  something in to it. Then close it. Open the same file 
					  again in append mode and write something in to it
-------------------------------------------------------------------------------
*/

TInt CTestunistd::symlinkprivate()
	{
	
    int i;
    i=symlink("z:\\private\\10202be9\\ufile.txt","z:\\Link2");
    
    if  (i < 0) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	































