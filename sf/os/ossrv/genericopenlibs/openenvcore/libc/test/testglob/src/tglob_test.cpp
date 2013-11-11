
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
// Name        : tglob.cpp
//



 
#include "tglob.h"
#include <unistd.h>
#include <e32err.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>


#define MAX_LEN  20
_LIT(KFILE1, "C:\\Glob-Dir\\Test1\\tester1.txt");
_LIT(KFILE2, "C:\\Glob-Dir\\Test1\\tester2.txt");

_LIT(KFILE3, "C:\\Glob-Dir\\Test2\\mester1.txt");
_LIT(KFILE4, "C:\\Glob-Dir\\Test2\\mester2.txt");

_LIT(KFILE5, "C:\\Glob-Dir\\Test1\\zester1.txt");
_LIT(KFILE6, "C:\\Glob-Dir\\Test2\\zester1.txt");


// ============================ MEMBER FUNCTIONS ===============================


/*
-------------------------------------------------------------------------------
Function Name		: CTestglob::globappend()
API Tested			: Glob with GLOB_APPEND

-------------------------------------------------------------------------------
*/

	
TInt CTestglob::globappend()
	{
	__UHEAP_MARK;
	int i = 0;
	int err = 0;
	int ferr = 0;
	int gerr1 = 0;
	int gerr2 = 0;
	int terr = KErrNone;
	
	RFile aRfile1,aRfile2,aRfile3,aRfile4;
	
	glob_t globbuf;
	char buf[][35] = {"C:\\Glob-Dir\\Test1\\tester1.txt", 
					  "C:\\Glob-Dir\\Test1\\tester2.txt", 
					  "C:\\Glob-Dir\\Test2\\mester1.txt", 
					  "C:\\Glob-Dir\\Test2\\mester2.txt"};	

	INFO_PRINTF1(_L("globappend"));
	
	INFO_PRINTF1(_L("Searching the path C:\\Glob-Dir\\Test1\\* and Glob-Dir\\Test2\\*"));
	
	do
	{
		ferr = aRfile1.Create(iRfs, KFILE1, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file tester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			break;
		}
		ferr = aRfile2.Create(iRfs, KFILE2, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file tester2.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			aRfile1.Close();
			break;
		}
	
		ferr = aRfile3.Create(iRfs, KFILE3, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file mester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			aRfile1.Close();
			aRfile2.Close();
			break;
		}

		ferr = aRfile4.Create(iRfs, KFILE4, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file mester2.txt\n"));
			terr = KErrGeneral;
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			aRfile1.Close();
			aRfile2.Close();
			aRfile3.Close();
			break;
		}
	
		gerr1 = glob("C:\\Glob-Dir\\Test1\\*.txt", GLOB_MARK, NULL , &globbuf);
		
		if(gerr1)
		{
			INFO_PRINTF1(_L("First Glob operation Failed"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);
			terr = KErrGeneral;
		}
			
		if(gerr1 == 0)
			gerr2 = glob("C:\\Glob-Dir\\Test2\\*.txt",GLOB_MARK | GLOB_APPEND ,NULL ,&globbuf);
		
		if(gerr2 && gerr1 == 0)
		{
			INFO_PRINTF1(_L("Second Glob operation Failed"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);
			terr = KErrGeneral;
		}
			
		while(globbuf.gl_pathc > i && gerr1 == 0 && gerr2 == 0)
		{
			err = strcmp(globbuf.gl_pathv[i],buf[i]);
			if(err)
			{
				INFO_PRINTF1(_L("String Mismatch")) ;
				terr =  KErrGeneral;
	    		break;
			}
		i++;
		}	
		
		globfree(&globbuf);
		aRfile1.Close();
		aRfile2.Close();
		aRfile3.Close();
		aRfile4.Close();
	
	}while(0);
	
	//Clean up. Delete the files. 
	iRfs.Delete(KFILE1);
	iRfs.Delete(KFILE2);
	iRfs.Delete(KFILE3);
	iRfs.Delete(KFILE4);
	
	__UHEAP_MARKEND;
	
	return terr;
	}

	
/*
-------------------------------------------------------------------------------
Function Name		: CTestglob::globbrace()
API Tested			: Glob with GLOB_BRACE flag

-------------------------------------------------------------------------------
*/

TInt CTestglob::globbrace()
	{
	__UHEAP_MARK;
	int i = 0;
	int err = 0;
	int ferr = 0;
	int gerr = 0;
	int terr = KErrNone;
	glob_t globbuf;
	
	RFile aRfile1,aRfile2,aRfile3,aRfile4,aRfile5,aRfile6;
	
	char buf[][35] = {"C:\\Glob-Dir\\Test1\\tester1.txt", 
					  "C:\\Glob-Dir\\Test1\\tester2.txt", 
					  "C:\\Glob-Dir\\Test2\\mester1.txt", 
					  "C:\\Glob-Dir\\Test2\\mester2.txt"
					  };
					  
	INFO_PRINTF1(_L("globbrace"));
	
	INFO_PRINTF1(_L("WILL BE SEARCHING THE PATH C:\\Glob-Dir\\Test1\\te* and C:\\Glob-Dir\\Test1\\me*"));
	
	do
	{
		
		ferr = aRfile1.Create(iRfs, KFILE1, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file tester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			break;
		}
		
		ferr = aRfile2.Create(iRfs, KFILE2, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file tester2.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			aRfile1.Close();
			break;
		}

		ferr = aRfile3.Create(iRfs, KFILE3, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file mester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			aRfile1.Close();
			aRfile2.Close();
			break;
		}

		ferr = aRfile4.Create(iRfs, KFILE4, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file mester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			aRfile1.Close();
			aRfile2.Close();
			aRfile3.Close();
			break;
		}

		ferr = aRfile5.Create(iRfs, KFILE5, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file Test1\\zester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			aRfile1.Close();
			aRfile2.Close();
			aRfile3.Close();
			aRfile4.Close();
			break;
		}

		ferr = aRfile6.Create(iRfs, KFILE6, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file Test2\\zester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			aRfile1.Close();
			aRfile2.Close();
			aRfile3.Close();
			aRfile4.Close();
			aRfile5.Close();
			break;
		}
	
		gerr = glob("C:\\Glob-Dir\\*\\{te*,me*}",GLOB_MARK | GLOB_BRACE ,NULL ,&globbuf);

		if(gerr)
		{
			INFO_PRINTF1(_L("Glob operation Failed"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);
			terr = KErrGeneral;	
		}
		
		else
		{
			while(globbuf.gl_pathc > i)
			{
				err = strcmp(globbuf.gl_pathv[i],buf[i]);
				if(err)
				{
					INFO_PRINTF1(_L("String Mismatch")) ;    
					terr = KErrGeneral;
					break;
				}
				i++;
			}
		}
		
		aRfile1.Close();
		aRfile2.Close();
		aRfile3.Close();
		aRfile4.Close();
		aRfile5.Close();
		aRfile6.Close();
		
		globfree(&globbuf);
	
	}while(0);
	
	iRfs.Delete(KFILE1);
	iRfs.Delete(KFILE2);
	
	iRfs.Delete(KFILE3);
	iRfs.Delete(KFILE4);
	
	iRfs.Delete(KFILE5);
	iRfs.Delete(KFILE6);
	
	
	__UHEAP_MARKEND;
	return terr;
}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestglob::globdooffs()
API Tested			: Glob with GLOB_DOOFFS flag

-------------------------------------------------------------------------------
*/

TInt CTestglob::globdooffs()
	{
	__UHEAP_MARK;
	int i = 0;
	int err = 0;
	int ferr = 0;
	int gerr = 0;
	int terr = KErrNone;
	glob_t globbuf;
	
	RFile aRfile1, aRfile2;
	
	char buf[][35] = {"Hello",
					  "World",
					  "C:\\Glob-Dir\\Test1\\tester1.txt", 
					  "C:\\Glob-Dir\\Test1\\tester2.txt"};
					  
	globbuf.gl_offs = 2;
				  
	INFO_PRINTF1(_L("globdooffs"));
	INFO_PRINTF1(_L("WILL RESERVE 2 SLOTS AT THE STARTING and Search path is C\\*"));
	
	do
	{
		ferr = aRfile1.Create(iRfs, KFILE1, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file tester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			break;
		}

		ferr = aRfile2.Create(iRfs, KFILE2, EFileShareAny);
		if(ferr)
		{
			INFO_PRINTF1(_L("Failed to create the file tester2.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
			aRfile1.Close();
			break;
		}
	
		gerr = glob("C:\\Glob-Dir\\Test1\\*.txt",GLOB_DOOFFS, NULL ,&globbuf);
		
		if(gerr)
		{
			INFO_PRINTF1(_L("Glob operation Failed"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
			terr = KErrGeneral;
		}
			
		else
		{
			globbuf.gl_pathv[0] = "Hello";
    		globbuf.gl_pathv[1] = "World";
    
   		 	while(i < 4)
			{
				err = strcmp(globbuf.gl_pathv[i],buf[i]);
				if(err)
				{
					INFO_PRINTF1(_L("String Mismatch")) ;    
					terr = KErrGeneral;
					break;
				}
				i++;
			}
		}
		
		aRfile1.Close();
		aRfile2.Close();
		globfree(&globbuf);
		
	}while(0);
	
	iRfs.Delete(KFILE1);
	iRfs.Delete(KFILE2);
	
	__UHEAP_MARKEND;
	
	return terr;
	
	}
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestglob::globmark()
API Tested			: Glob with GLOB_MARK flag

-------------------------------------------------------------------------------
*/

TInt CTestglob::globmark()
{
	__UHEAP_MARK;
	
	int i = 0;
	int err = 0;
	int gerr = 0;
	int terr = KErrNone;
	glob_t globbuf;
	char buf[][35] = {"C:\\Glob-Dir\\Test1\\", 
					  "C:\\Glob-Dir\\Test2\\"};
					  
	INFO_PRINTF1(_L("globmark"));
	INFO_PRINTF1(_L("The search path is C:\\Glob-Dir\\*"));
	
	do
	{
		gerr = glob("C:\\Glob-Dir\\*",GLOB_MARK, NULL ,&globbuf);
		if(gerr)
		{
			INFO_PRINTF1(_L("Glob operation Failed"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);
			terr = KErrGeneral;
		}
		
		else
		{
			while(globbuf.gl_pathc > i && gerr == 0)
			{
				err = strcmp(globbuf.gl_pathv[i],buf[i]);
				if(err)
				{
					INFO_PRINTF1(_L("String Mismatch")) ;    
					terr = KErrGeneral;
					break;
				}
				i++;
			}
		}
		globfree(&globbuf);
		
	}while(0);
	
	__UHEAP_MARKEND;
	
	return terr;
	
}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestglob::globnocheck()
API Tested			: Glob to with flag GLOB_NOCHECK

-------------------------------------------------------------------------------
*/

TInt CTestglob::globnocheck()
	{
	__UHEAP_MARK;
	int err = 0;
	int terr = KErrNone;
	int ferr = 0;
	int gerr = 0;
	
	glob_t globbuf;
	
	RFile afile1, afile2;
	
	char buf[35] = "C:\\Glob-Dir\\*\\zz*";
	
	do
	{
		ferr = afile1.Create(iRfs, KFILE5, EFileShareAny);
    	if(ferr)
		{
	    	INFO_PRINTF1(_L("Failed to create the file tester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
	    	terr = KErrGeneral;
	    	break;
		}
		ferr = afile2.Create(iRfs, KFILE6, EFileShareAny);
    	if(ferr)
		{
		    INFO_PRINTF1(_L("Failed to create the file tester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
		    afile2.Close();
	    	terr = KErrGeneral;
	    	break;
		}
	
		INFO_PRINTF1(_L("globnocheck"));
		INFO_PRINTF1(_L("WILL SEARCH C:\\Glob-Dir\\Test1\\zz* FOR THE PATTERN OTHER WISE WILL RETURN THE PATTERN\n"));
	
		gerr = glob("C:\\Glob-Dir\\*\\zz*",GLOB_NOCHECK, NULL ,&globbuf);
	
		if(gerr)
		{
			INFO_PRINTF1(_L("Glob operation Failed"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);
			terr = KErrGeneral;
		}
			
		else
		{
			err = strcmp(globbuf.gl_pathv[0],buf);
			if(!err)
				terr = KErrNone;
			
		}
		afile1.Close();
		afile2.Close();
		
		globfree(&globbuf);
	
	}while(0);
	
	iRfs.Delete(KFILE5);
	iRfs.Delete(KFILE6);
	
	__UHEAP_MARKEND;
	
	return terr;
	}	


/*
-------------------------------------------------------------------------------
Function Name		: CTestglob::globnosort()
API Tested			: Glob with Flag GLOB_NOSORT

-------------------------------------------------------------------------------
*/

TInt CTestglob::globnosort()
	{
	__UHEAP_MARK;
	int i = 0;
	int err = 0;
	int ferr = 0;
	int gerr = 0;
	int terr = KErrNone;
	glob_t globbuf;
	RFile aRfile1,aRfile2,aRfile3,aRfile4;
	
	char buf[][35] = {"C:\\Glob-Dir\\Test2\\mester2.txt", 
					  "C:\\Glob-Dir\\Test2\\mester1.txt", 
					  "C:\\Glob-Dir\\Test1\\tester2.txt", 
					  "C:\\Glob-Dir\\Test1\\tester1.txt"};
	do
	{
	    INFO_PRINTF1(_L("globnosort"));
	    INFO_PRINTF1(_L("WILL BE SEARCHING THE PATH C:\\Glob-Dir\\Test1\\* and C:\\Glob-Dir\\Test1\\*"));
			
	    ferr = aRfile1.Create(iRfs, KFILE1, EFileShareAny);
	    if(ferr)
	    {
	    	INFO_PRINTF1(_L("Failed to create the file tester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
	    	terr = KErrGeneral;
	    	break;
	    }
	    ferr = aRfile2.Create(iRfs, KFILE2, EFileShareAny);
		if(ferr)
	    {
	    	INFO_PRINTF1(_L("Failed to create the file tester2.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
	    	terr = KErrGeneral;
	    	aRfile1.Close();
	    	break;
	    }
	    ferr = aRfile3.Create(iRfs, KFILE3, EFileShareAny);
	    if(ferr)
	    {
	    	INFO_PRINTF1(_L("Failed to create the file mester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
	    	terr = KErrGeneral;
	    	aRfile1.Close();
	    	aRfile2.Close();
	    	break;
	    }
	    ferr = aRfile4.Create(iRfs, KFILE4, EFileShareAny);
		if(ferr)
	    {
	    	INFO_PRINTF1(_L("Failed to create the file mester1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),ferr);
	    	terr = KErrGeneral;
	    	aRfile1.Close();
	    	aRfile2.Close();
	    	aRfile3.Close();
	    	break;
	    }
	    
	    gerr = glob("C:\\Glob-Dir\\*\\*",GLOB_NOSORT , NULL ,&globbuf);
	    
	    if(gerr)
	    {
		    INFO_PRINTF1(_L("Glob operation Failed"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);
		    terr = KErrGeneral;
	    }
	    
		else
		{
	    	while(globbuf.gl_pathc > i)
	    	{
	        	err = strcmp(globbuf.gl_pathv[i],buf[i]);
	        	if(err)
		    	{
		        	INFO_PRINTF1(_L("String Mismatch"));    
		        	terr = KErrGeneral;
		        	break;
		    	}
	    		i++;
	    	}
		}
	    
	   	aRfile1.Close();
	    aRfile2.Close();
	    aRfile3.Close();
	    aRfile4.Close();
	
	    globfree(&globbuf);
	    
	}while(0);
	
	//Assuming these operation will be successful
	//if not should not matter for the test cases.
	
	iRfs.Delete(KFILE1);
	iRfs.Delete(KFILE2);
	iRfs.Delete(KFILE3);
	iRfs.Delete(KFILE4);
	
	__UHEAP_MARKEND;
		
	return terr;
	}






/*
-------------------------------------------------------------------------------
Function Name		: CTestglob::globtilde()
API Tested			: glob(). Creates 2 files in the current working directory
					  then call glod with GLOB_TILDE to search the current working
					  directoy. Match the string. If found all PASS else FAIL  	
-------------------------------------------------------------------------------
*/
//extern int errno;

TInt CTestglob::globtilde()
	{
	__UHEAP_MARK;
	int i = 0;
	int gerr = 0;
	int err = KErrNone;
	glob_t globbuf;
	char *t = NULL;
	const char buf[][50]={
		"\\globdir\\tmp-1.txt",
		"\\globdir\\tmp-2.txt"};
	char *directory = "c:\\";
	chdir (directory);

	do
	{

		int ret = 0;
	 	ret = mkdir("globdir",S_IRWXU);
 	    if(ret && errno != EEXIST)
 	    {
 		    INFO_PRINTF1(_L("Failed to create the directory\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);
 		    err = KErrGeneral;
 		    break;
 	    }
 	
 	    ret = creat("globdir/tmp-1.txt", S_IRWXO);
		if(-1 == ret)
 	    {
 		    INFO_PRINTF1(_L("Failed to create the file tmp-1.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);
 		    rmdir("globdir");
 		    err = KErrGeneral;
 		    break;
 	    }	
 	    ret = creat("globdir/tmp-2.txt", S_IRWXO);
 	    if(-1 == ret)
 	    {
 		    INFO_PRINTF1(_L("Failed to create the file tmp-2.txt\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);
 		    unlink("globdir/tmp-1.txt");
 		    rmdir("globdir");
 		    err = KErrGeneral;
 		    break;
 	    }

	    gerr = glob("~\\globdir\\*",GLOB_MARK | GLOB_TILDE , NULL ,&globbuf);
	    
	    if(gerr)
	    {
		    INFO_PRINTF1(_L("Glob-Tilde Operation Failed\n"));
			INFO_PRINTF2(_L("The reason code for failure is %d\n"),errno);

		    unlink("globdir/tmp-1.txt");
		    unlink("globdir/tmp-2.txt");
		    rmdir("globdir");

		    err = KErrGeneral;	
		    break;
	    }
	
	    while(globbuf.gl_pathc > i)
	    {
	        t = strstr(globbuf.gl_pathv[i],buf[i]);
	        if(!t)
		    {
		        INFO_PRINTF1(_L("String Mismatch")) ;    
		        err = KErrGeneral;
		        break;
		    }
	        i++;
	    }

	    unlink("globdir/tmp-1.txt");
		unlink("globdir/tmp-2.txt");
		rmdir("globdir");

		globfree(&globbuf);
	}while(0);
	
 	__UHEAP_MARKEND;
 	return err;
 }
	
	
	
	
