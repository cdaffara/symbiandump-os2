// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tstdlibblocks.cpp
//



 

#include "tstdlib.h"
#include "sysif.h" //Backend()
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> //MB_LEN_MAX
#include <monetary.h>
#include <locale.h>


#include <test/testexecutestepbase.h>
#include <limits.h>
#include <e32def.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <wctype.h>
#include <inttypes.h>
#include <c32comm.h> 
#include <math.h> 
#include <e32math.h> 
#include <fcntl.h> //O_APPEND, O_RDWR, O_CREAT
#include <sys/stat.h> //S_ISWUSR
#include <sys/syslimits.h> //PATH_MAX
#include <pthread.h> //pthread_create
#include <getopt.h> //getopt, getopt_long
#include <signal.h>
#include <sys/wait.h>

#include <setjmp.h>
#include <iconv.h>
#include <grp.h>
#include <locale.h>
#include <monetary.h>

#define MAX_SIZE 50
#define BUF_SIZE 100
// ============================ GLOBAL FUNCTIONS ===============================
void exitfunc0(void) { }
void exitfunc1(void) { }
void exitfunc2(void) { }
void exitfunc3(void) { }
void exitfunc4(void) { }
void exitfunc5(void) { }
void exitfunc6(void) { }
void exitfunc7(void) { }
void exitfunc8(void) { }
void exitfunc9(void) { }
void exitfunc10(void) { }
void exitfunc11(void) { }
void exitfunc12(void) { }
void exitfunc13(void) { }
void exitfunc14(void) { }
void exitfunc15(void) { }
void exitfunc16(void) { }
void exitfunc17(void) { }
void exitfunc18(void) { }
void exitfunc19(void) { }
void exitfunc20(void) { }
void exitfunc21(void) { }
void exitfunc22(void) { }
void exitfunc23(void) { }
void exitfunc24(void) { }
void exitfunc25(void) { }
void exitfunc26(void) { }
void exitfunc27(void) { }
void exitfunc28(void) { }
void exitfunc29(void) { }
void exitfunc30(void) { }
void exitfunc31(void) { }
void exitfunc32(void) { }

// -----------------------------------------------------------------------------
// ReadStringParam :Reads path name given the buffer
// Returns: KErrNone: On Success
//          KErrGeneral: On Failure                    
// -----------------------------------------------------------------------------
//
void CTestStdlib::ReadStringParam(char* expected)
	{
	_LIT( KaKey, "Param%d" );
   	TBuf<8> pNameBuf;
    TPtrC string;
    pNameBuf.Format(KaKey,++iParamCnt);
	TBool res = GetStringFromConfig(ConfigSection(), pNameBuf, string);
	if(!res)
	 	{
	 	_LIT(Kerr , "Unable to retrieve string parameter") ;
	 	INFO_PRINTF1(Kerr);
	  	}
	TBuf8<256> bufstring;
	bufstring.Copy(string);
	TInt paramLength=string.Length();
	char* text=(char *)(bufstring.Ptr());
 	*(text+paramLength)='\0';
	strcpy(expected,text);
	return;
	}

// -----------------------------------------------------------------------------
// ReadIntParam :Reads an inteeger value given the buffer
// Returns: KErrNone: On Success
//          KErrGeneral: On Failure                    
// -----------------------------------------------------------------------------
//
void CTestStdlib::ReadIntParam(TInt &aInt)
{
	_LIT( Kparam, "Param%d" );
     TBuf<8> pNameBuf;
     TPtrC string;
     pNameBuf.Format(Kparam,++iParamCnt);
	 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aInt);
	 if(!res)
	 	{
	 	_LIT(Kerr , "Unable to retrieve integer parameter") ;
	 	INFO_PRINTF1(Kerr);
	  	}
	 return;
}	
// -----------------------------------------------------------------------------
//Function Name :malloc_Test0
//API Tested :malloc
//TestCase Description:malloc returns -> pointer to unitialized mem locn
// -----------------------------------------------------------------------------
TInt CTestStdlib::malloc_Test0()
    {
    
    
    INFO_PRINTF1(_L("In malloc_Test0L"));

    //--------------------------
	char  *pc = (char *)malloc( sizeof(char) * 3);
	//--------------------------
    INFO_PRINTF1(_L("{Expected: some char}"));

	if (pc == NULL)
		{
		INFO_PRINTF1(_L("malloc failed to allocate memory"));
		return KErrGeneral;
		}
	
	int i = 0;
	for(i = 0; i < 3; i++)
		{
		INFO_PRINTF2(_L(" %c"), pc[i]);
		}

	free(pc);
	
	
	
	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :malloc_Test1
//API Tested :malloc
//TestCase Description:malloc returns -> NULL
// -----------------------------------------------------------------------------
TInt CTestStdlib::malloc_Test1()
    {
    
    
    TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In malloc_Test1L"));

	errno = 0;
    //--------------------------
	char  *pc = (char *)malloc(1024 * 1024);	
    //--------------------------
     
    bool i = (pc == NULL);

	INFO_PRINTF2(_L("{Expected: 1} %d"), (int)i);

	free(pc);

	if (i != true || errno != ENOMEM)
		{
		INFO_PRINTF2(_L("errno was set to - %d"), errno);
		ret = KErrGeneral;
		}
  
  	
  	
	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :malloc_Test2
//API Tested :malloc
//TestCase Description:malloc returns -> NULL
// -----------------------------------------------------------------------------
TInt CTestStdlib::malloc_Test2()
    {
    
    
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In malloc_Test2L"));

    //--------------------------
	char  *pc = (char *)malloc(NULL);	
    //--------------------------
    
    bool i = (pc == NULL);
    

	INFO_PRINTF2(_L("{Expected: 1} %d"), (int)i);

	free(pc);

	if (i != true)
		{
		ret = KErrGeneral;
		}
    
    
	
	return ret;
    }
    
    // -----------------------------------------------------------------------------
//Function Name :calloc_Test0
//API Tested :calloc
//TestCase Description:calloc returns -> pointer to mem locn intialized to zero
// -----------------------------------------------------------------------------
TInt CTestStdlib::calloc_Test0()
    {
    
    
	TInt ret = KErrNone;
    INFO_PRINTF1(_L("In calloc_Test0L"));

    //--------------------------
	int  *pint = (int *)calloc(2, sizeof (int) * 2);
	//--------------------------

	if (pint == NULL)
		{
		INFO_PRINTF1(_L("calloc failed to allocate memory"));
		return KErrNoMemory;
		}

    INFO_PRINTF1(_L("{Expected: zeros}"));

	int i = 0;
	for(i = 0; i < 4; i++)
		{
	   INFO_PRINTF2(_L(" %d"), pint[i]);
		}

	for(i = 0; i < 4; i++)
		{
		if (pint[i] != 0)
			{
			ret = KErrGeneral;
			}
		}
	free(pint);
	
	
	
	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :calloc_Test1
//API Tested :calloc
//TestCase Description:calloc returns -> NULL
// -----------------------------------------------------------------------------
TInt CTestStdlib::calloc_Test1()
    {
    
    
    INFO_PRINTF1(_L("In calloc_Test1L"));

    //--------------------------
	char  *pc = (char *)calloc(3, 1024 * 1024);	
   
    bool i = (pc == NULL);
	//--------------------------

	INFO_PRINTF2(_L("{Expected: 1} %d"), (int)i);

	free(pc);

	
	
	if (i != true)
		{
		return KErrGeneral;
		}
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :calloc_Test2
//API Tested :calloc
//TestCase Description:calloc returns -> NULL
// -----------------------------------------------------------------------------
TInt CTestStdlib::calloc_Test2()
    {
    
    
    INFO_PRINTF1(_L("In calloc_Test2L"));

    //--------------------------
	char  *pc = (char *)calloc(0, 1);	
   
    bool i = (pc == NULL);
	//--------------------------

	INFO_PRINTF2(_L("{Expected: 1} %d"), (int)i);

	free(pc);
	
	
	
	if (i != true)
		{
		return KErrGeneral;
		} 
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :realloc_Test0
//API Tested :realloc
//TestCase Description:realloc returns -> pointer to mem locn with old contents
//unchanged and the new -unintialized.
// -----------------------------------------------------------------------------
TInt CTestStdlib::realloc_Test0()
    {
    
    
    INFO_PRINTF1(_L("In realloc_Test0L"));

    //--------------------------
	int  *pint = (int *)calloc(2, sizeof (int) * 2);

	pint = (int *)realloc(pint, (sizeof (int) * 6) );
	
	if (pint == NULL)
		{
		INFO_PRINTF1(_L("realloc failed to allocate memory"));
		return KErrNoMemory;
		}
	//--------------------------
    INFO_PRINTF1(_L("{Expected: zeros(4) and some uninitialized values}"));

	int i = 0;
	for(i = 0; i < 6; i++)
		{
	   INFO_PRINTF2(_L(" %d"), pint[i]);
		}

	for(i = 0; i < 4; i++)
		{
		if (pint[i] != 0)
			{
			free(pint);
			
			return KErrGeneral;
			}	
		}

	free(pint);
	
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//Function Name :realloc_Test1
//API Tested :realloc
//TestCase Description:realloc returns -> pointer to mem locn with old contents
//unchanged and the new -unintialized.
// -----------------------------------------------------------------------------
TInt CTestStdlib::realloc_Test1()
    {
    
    
    INFO_PRINTF1(_L("In realloc_Test1L"));

    //--------------------------
	int  *pint = (int *)calloc(2, sizeof (int) * 2);

	pint = (int *)realloc(pint, (sizeof (int) ) );
	
	if (pint == NULL)
		{
		INFO_PRINTF1(_L("realloc failed to allocate memory"));
		return KErrNoMemory;
		}
	//--------------------------
    INFO_PRINTF1(_L("{Expected: zeros}"));

	int i = 0;
	for(i = 0; i < 1; i++)
		{
	   INFO_PRINTF2(_L(" %d"), pint[i]);
		}

	for(i = 0; i < 1; i++)
		{
		if (pint[i] != 0)
			{	
			free(pint);
			
			return KErrGeneral;
			}
		}
		
	free(pint);
	
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :realloc_Test2
//API Tested :realloc
//TestCase Description:realloc returns -> pointer that could be freed.
// -----------------------------------------------------------------------------
TInt CTestStdlib::realloc_Test2()
    {
    
    
    INFO_PRINTF1(_L("In realloc_Test2L"));

    //--------------------------
	int  *pint = (int *)calloc(1, sizeof(int) * 3);

	if (pint == NULL)
		{
		INFO_PRINTF1(_L("calloc failed to allocate memory"));
		return KErrNoMemory;
		}

	pint = (int *)realloc(pint, 0);

	//--------------------------
    
	INFO_PRINTF2(_L("{Expected: some int value} %d"), pint[0]);

	free(pint);
	
	
		
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :realloc_Test3
//API Tested :realloc
//TestCase Description:realloc returns -> pointer to mem locn unintialized.
// -----------------------------------------------------------------------------
TInt CTestStdlib::realloc_Test3()
    {
    
    
    INFO_PRINTF1(_L("In realloc_Test3L"));

    //--------------------------
	int  *pint = (int *)realloc(NULL, ( sizeof(int) * 2 ) );
	
	if (pint == NULL)
		{
		INFO_PRINTF1(_L("realloc failed to allocate memory"));
		return KErrNoMemory;
		}
	//--------------------------
    INFO_PRINTF1(_L("{Expected: some int values}"));

	int i = 0;
	for(i = 0; i < 2; i++)
		{
	   INFO_PRINTF2(_L(" %d"), pint[i]);
		}

	free(pint);
	
	
		
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :realloc_Test4
//API Tested :realloc
//TestCase Description:realloc returns -> NULL but leaves the old mem block
//unchanged.
// -----------------------------------------------------------------------------
TInt CTestStdlib::realloc_Test4()
    {
    
    
	TInt ret = KErrNone;

    INFO_PRINTF1(_L("In realloc_Test4L"));

    //--------------------------
	int  *pint = (int *)calloc(1, sizeof(int) * 3);

	if (pint == NULL)
		{
		INFO_PRINTF1(_L("calloc failed to allocate memory"));
		return KErrNoMemory;
		}

	int  *pint_realloc = (int *)realloc(pint, 1024 * 1024);

	bool b = (pint_realloc == NULL);

	//--------------------------
    
	INFO_PRINTF2(_L("{Expected: zeros, 1} %d"), (int)b);
	
	int i = 0;
	for(i = 0; i < 3; i++)
		{
	   INFO_PRINTF2(_L(" %d"), pint[i]);
		}

	for(i = 0; i < 3; i++)
		{
		if (pint[i] != 0)
			{
			ret = KErrGeneral;
			}
		}

	if (b != 1)
		{
		ret = KErrGeneral;
		}

	free(pint);
	
	
		
	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :free_Test0
//API Tested :free
//TestCase Description:free returns -> nothing 
//(check the total available heap space, allocate a chunk of mem check that
//the available heap space is reduced, free the memory and check that the 
//the available heap space is equal to what it was before the memory was
//allocated and freed)
// -----------------------------------------------------------------------------
TInt CTestStdlib::free_Test0()
    {
    INFO_PRINTF1(_L("In free_Test0L"));

    //--------------------------
	RHeap& heap_handle = User::Heap();
	TInt largest_free_block;
	TInt heap_available1 = heap_handle.Available(largest_free_block);

	int  *pint = (int *)malloc(200);

	TInt heap_available2 = heap_handle.Available(largest_free_block);

	if(heap_available2 >= heap_available1)
		{
		INFO_PRINTF3(_L("{heap_avail1 = %d, heap_avail2 = %d}"), 
			           heap_available1, heap_available2);
		INFO_PRINTF1(_L("available memory is not as expected after malloc...\
			          free not tested!"));
        return KErrGeneral;
		}

	free(pint);

	TInt heap_available3 = heap_handle.Available(largest_free_block);
	if(heap_available3 != heap_available1)
		{
		INFO_PRINTF3(_L("{heap_avail1 = %d, heap_avail3 = %d}"), 
			           heap_available1, heap_available3);
		INFO_PRINTF1(_L("available memory is not as expected after free!"));
	    return KErrGeneral;
		}
	
	//--------------------------

	INFO_PRINTF1(_L("{Expected: heapavail1 should be equal to heapavail3}"));
	INFO_PRINTF4(_L("heap_avail1 = %d, heap_avail2 = %d, heap_avail3 = %d"), 
		heap_available1, heap_available2, heap_available3);

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :getenv_Test0
//API Tested :getenv
//TestCase Description:getenv returns -> NULL when the input env variable 
//doesn't exist
// -----------------------------------------------------------------------------
TInt CTestStdlib::getenv_Test0()
    {
       
    INFO_PRINTF1(_L("In getenv_Test0L"));

    //--------------------------
	unsetenv("path");
	char * aenv =  getenv("path");

    bool b = (aenv == NULL);
	//--------------------------

	INFO_PRINTF2(_L("{Expected: 1} %d"), (int)b);

	if (b != true)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :getenv_Test1
//API Tested :getenv
//TestCase Description:getenv returns -> the value of the env variable when 
//it does exist
// -----------------------------------------------------------------------------
TInt CTestStdlib::getenv_Test1()
    {

    INFO_PRINTF1(_L("In getenv_Test1"));

    //--------------------------
	setenv("path", ".;/c:/bin;", 1);

	char *psc = getenv("path");

	char *ps = NULL;

	if (psc != NULL)
		{
		ps = (char *)malloc( strlen(psc) + 1 );

		if (ps == NULL)
			{
			INFO_PRINTF1(_L("malloc failed to allocate memory"));
			return KErrNoMemory;
			}

		strcpy(ps, psc);
		}
    //--------------------------
	INFO_PRINTF1(_L("{Expected: \".;/c:/bin;\"}"));

	int i = 0;
	for (i = 0; ps[i] != '\0'; i++)
		{
	    INFO_PRINTF2(_L(" %c"), ps[i]);
		}

	free(ps);

	
	if ( strcmp(psc, ".;/c:/bin;") != 0 )
    	{
		return KErrGeneral;
		}		
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :setenv_Test0
//API Tested :setenv
//TestCase Description:setenv returns -> 0 (successfully set the non-existing 
//env variable)
// -----------------------------------------------------------------------------
TInt CTestStdlib::setenv_Test0()
    {
    
    INFO_PRINTF1(_L("In setenv_Test0L"));

    //--------------------------
	unsetenv("home");

	int iret = setenv("home", "/home", 0);

	char *psc = getenv("home");

	char *ps = NULL;

	if (psc != NULL)
		{
		ps = (char *)malloc( strlen(psc) + 1 );

	   	if (ps == NULL)
			{
			INFO_PRINTF1(_L("malloc failed to allocate memory"));
			return KErrNoMemory;
			}
		strcpy(ps, psc);
		}
    //--------------------------
	INFO_PRINTF2(_L("{Expected: 0, \"/home\"} %d"), iret);

	int i = 0;
	for (i = 0; ps[i] != '\0'; i++)
		{
		INFO_PRINTF2(_L(" %c"), ps[i]);
		}

	free(ps);
	
	if ( strcmp(psc, "/home") != 0 || iret != 0)
    	{
		return KErrGeneral;
		}	

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :setenv_Test1
//API Tested :setenv
//TestCase Description:setenv returns -> 0 (succesfully set the existing env 
//var by overwriting it)
// -----------------------------------------------------------------------------
TInt CTestStdlib::setenv_Test1()
    {
    
    INFO_PRINTF1(_L("In setenv_Test1L"));

    //--------------------------
	setenv("home", "/home", 1);
 
	int iret2 = setenv("home", "/check", 1);

	char *psc = getenv("home");

	char *ps = NULL;

	if (psc != NULL)
		{
		ps = (char *)malloc( strlen(psc) + 1 );

		if (ps == NULL)
			{
			INFO_PRINTF1(_L("malloc failed to allocate memory"));
			return KErrNoMemory;
			}

		strcpy(ps, psc);
		}
   	//--------------------------    
	INFO_PRINTF2(_L("{Expected: 0, \"/check\"} %d"), iret2);

	int i = 0;
	for (i = 0; ps[i] != '\0'; i++)
		{
		INFO_PRINTF2(_L(" %c"), ps[i]);
		}

	free(ps);
	
	if ( strcmp(psc, "/check") != 0 || iret2 != 0)
    	{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :setenv_Test2
//API Tested :setenv
//TestCase Description:setenv returns -> 0 (without overwriting the existing 
//env var)
// -----------------------------------------------------------------------------
TInt CTestStdlib::setenv_Test2()
    {
    
    INFO_PRINTF1(_L("In setenv_Test2L"));

    //--------------------------
	setenv("home", "/home", 1);
 
	int iret2 = setenv("home", "/check", 0);

	char *psc = getenv("home");

	char *ps = NULL;

	if (psc != NULL)
		{
		ps = (char *)malloc( strlen(psc) + 1 );

		if (ps == NULL)
			{
			INFO_PRINTF1(_L("malloc failed to allocate memory"));
			return KErrNoMemory;
			}

		strcpy(ps, psc);
		}
   	//--------------------------    
	INFO_PRINTF2(_L("{Expected: 0, \"/home\"} %d"), iret2);

	int i = 0;
	for (i = 0; ps[i] != '\0'; i++)
		{
        INFO_PRINTF2(_L(" %c"), ps[i]);
		}

	free(ps);

	if ( strcmp(psc, "/home") != 0 || iret2 != 0)
    	{
		return KErrGeneral;
		}	

	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :setenv_Test3
//API Tested :setenv
//TestCase Description:setenv returns -> -1 (invalid argument)
// -----------------------------------------------------------------------------
TInt CTestStdlib::setenv_Test3()
    {

    TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In setenv_Test3L"));

    //--------------------------
	setenv("home", "/home", 1);
 
 	errno = 0;
	int ret1 = setenv(NULL, "/some", 1);
	if (errno != EINVAL)
 		{
 		INFO_PRINTF1(_L("{setenv1: errno not set to EINVAL"));
 		ret = KErrGeneral;
 		}
	char *psc1 = getenv("home");
	
 	errno = 0;	
	int ret2 = setenv("", "/some", 1);
	if (errno != EINVAL)
 		{
 		INFO_PRINTF1(_L("{setenv2: errno not set to EINVAL"));
 		ret = KErrGeneral;
 		}
 	char *psc2 = getenv("home");
	
	errno = 0;
	int ret3 = setenv("hom=e", "/some", 1);
	if (errno != EINVAL)
 		{
 		INFO_PRINTF1(_L("{setenv3: errno not set to EINVAL"));
 		ret = KErrGeneral;
 		}
	char *psc3 = getenv("home");
	
	//--------------------------    
	INFO_PRINTF2(_L("{Expected: -1, \"/home\"} %d"), ret1);
	
	INFO_PRINTF2(_L("{Expected: -1, \"/home\"} %d"), ret2);
	
	INFO_PRINTF2(_L("{Expected: -1, \"/home\"} %d"), ret3);
	
	if ( strcmp(psc1, "/home") != 0 || 
		 ret1 != -1 ||
		 strcmp(psc2, "/home") != 0 || 
		 ret2 != -1 ||
		 strcmp(psc3, "/home") != 0 || 
		 ret3 != -1
	   )
		{
		ret = KErrGeneral;	
		}

	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :setenv_Test4
//API Tested :putenv
//TestCase Description:setenv returns -> -1 (no sufficient memory)
// -----------------------------------------------------------------------------
TInt CTestStdlib::setenv_Test4()
    {
    
    TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In setenv_Test4L"));
    
#ifdef _DEBUG
    //--------------------------
    unsetenv("noexist");
    
	RHeap *heap_handle = Backend()->Heap();
	heap_handle->__DbgMarkStart();
	heap_handle->__DbgSetAllocFail(RAllocator::ENone, 1);
	heap_handle->__DbgSetAllocFail(RAllocator::EFailNext, 1);
	
	int iret = setenv("noexist", "/check", 1);
	INFO_PRINTF2(_L("setenv errno: %d"), errno);
	if (errno != ENOMEM)
 		{
 		INFO_PRINTF1(_L("{putenv: errno not set to ENOMEM"));
 		ret = KErrGeneral;
 		}

	heap_handle->__DbgSetAllocFail(RAllocator::ENone, 1);
	heap_handle->__DbgMarkEnd(0);
	
	//--------------------------
	INFO_PRINTF2(_L("{Expected: -1} %d"), iret);

	if ( iret != -1 )
		{
    	ret = KErrGeneral;	
		}
	
#else
	INFO_PRINTF1(_L("This test case executes all code only for DEBUG build \
	              as it can simulate memory failure only in DEBUG build"));
#endif //_DEBUG

	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :putenv_Test0
//API Tested :putenv
//TestCase Description:putenv returns -> 0 (successfully set the non-existing 
//env variable)
// -----------------------------------------------------------------------------
TInt CTestStdlib::putenv_Test0()
    {
    
    INFO_PRINTF1(_L("In putenv_Test0L"));

    //--------------------------
	unsetenv("home");

	int iret = putenv("home=/home/xyz");

	char *psc = getenv("home");

	//--------------------------
	INFO_PRINTF2(_L("{Expected: 0, \"/home/xyz\"} %d"), iret);
		
	if ( strcmp(psc, "/home/xyz") != 0 || iret != 0)
    	{
		return KErrGeneral;
		}	
		
	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :putenv_Test1
//API Tested :putenv
//TestCase Description:putenv returns -> 0 (successfully set the existing 
//env variable)
// -----------------------------------------------------------------------------
TInt CTestStdlib::putenv_Test1()
    {

    INFO_PRINTF1(_L("In putenv_Test1L"));
    
    //--------------------------
	int iret1 = setenv("home", "/check", 1);

	int iret = putenv("home=/home/xyz/chk");

	char *psc = getenv("home");

	//--------------------------
	INFO_PRINTF2(_L("{Expected: 0, \"/home/xyz/chk\"} %d"), iret);

	if ( strcmp(psc, "/home/xyz/chk") != 0 || iret != 0)
    	{
		return KErrGeneral;
		}

	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :putenv_Test2
//API Tested :putenv
//TestCase Description:putenv returns -> -1 (invalid argument)
// -----------------------------------------------------------------------------
TInt CTestStdlib::putenv_Test2()
    {

    TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In putenv_Test2L"));

    //--------------------------
	int iret1 = setenv("home", "/check", 1);

	errno = 0;
	int iret = putenv("home/home/xyz/chk");
	if (errno != EINVAL)
 		{
 		INFO_PRINTF1(_L("{putenv: errno not set to EINVAL"));
 		ret = KErrGeneral;
 		}

	char *psc = getenv("home");

	//--------------------------
	INFO_PRINTF2(_L("{Expected: -1, \"/check\"} %d"), iret);

	if ( strcmp(psc, "/check") != 0 || iret != -1)
		{
    	ret = KErrGeneral;	
		}

	return ret;
    }   
  
// -----------------------------------------------------------------------------
//Function Name :putenv_Test3
//API Tested :putenv
//TestCase Description:putenv returns -> -1 (invalid argument)
// -----------------------------------------------------------------------------
TInt CTestStdlib::putenv_Test3()
    {
    
    TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In putenv_Test3L"));
    errno = 0;
    
#ifdef _DEBUG
    //--------------------------
	int iret = setenv("home", "/check", 1);
	errno = 0;
	
	//1st allocation failure
	
	//
	//__UHEAP_RESET;
	//__UHEAP_FAILNEXT(1);

	RHeap *heap_handle = Backend()->Heap();
	heap_handle->__DbgMarkStart();
	heap_handle->__DbgSetAllocFail(RAllocator::ENone, 1);
	heap_handle->__DbgSetAllocFail(RAllocator::EFailNext, 1);
	
	int iret1 = putenv("home=/home/xyz/chk");
	if (errno != ENOMEM)
 		{
 		INFO_PRINTF1(_L("{putenv: errno not set to ENOMEM"));
 		ret = KErrGeneral;
 		}

	errno = 0;
	//2nd allocation failure
	//__UHEAP_FAILNEXT(2);
	heap_handle->__DbgSetAllocFail(RAllocator::ENone, 1);
	heap_handle->__DbgSetAllocFail(RAllocator::EFailNext, 1);

	int iret2 = putenv("home=/home/xyz/chk");
	if (errno != ENOMEM)
	 	{
 		INFO_PRINTF1(_L("{putenv: errno not set to ENOMEM"));
 		ret = KErrGeneral;
 		}
 	
 	heap_handle->__DbgSetAllocFail(RAllocator::ENone, 1);
	heap_handle->__DbgMarkEnd(0);
 	
 	//__UHEAP_RESET;
	//

	char *psc = getenv("home");

	//--------------------------
	INFO_PRINTF3(_L("{Expected: -1, -1, \"/check\"} %d"), iret1, iret2);
		
	if ( strcmp(psc, "/check") != 0 || iret1 != -1 || iret2 != -1 )
		{
    	ret = KErrGeneral;	
		}
	
#else
	INFO_PRINTF1(_L("This test case executes all code only for DEBUG build \
	              as it can simulate memory failure only in DEBUG build"));
	              
#endif //_DEBUG

	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :unsetenv_Test0
//API Tested :unsetenv
//TestCase Description:unsetenv returns -> nothing
// -----------------------------------------------------------------------------
TInt CTestStdlib::unsetenv_Test0()
    {
    
	TInt ret = KErrNone;
    INFO_PRINTF1(_L("In unsetenv_Test0L"));

    //--------------------------
	setenv("home", "/home", 1);

	char  *psc1 = getenv("home");
	
	char  *ps1 = NULL;

	if (psc1 != NULL)
		{
		ps1 = (char *)malloc( strlen(psc1) + 1 );

		if (ps1 == NULL)
			{
			INFO_PRINTF1(_L("malloc failed to allocate memory"));
			return KErrNoMemory;
			}

		strcpy(ps1, psc1);
		}

    if ( strcmp(psc1, "/home") != 0 )
		{
		ret = KErrGeneral;
		}

	unsetenv("home");

    char  *psc2 = getenv("home");

	bool b = (psc2 == NULL);

	if (b != true)
		{
		ret = KErrGeneral;
		}
	//--------------------------

	INFO_PRINTF1(_L("{Expected: \"/home\", 1}"));
	
	int i = 0;
    for(i = 0; ps1[i] != '\0'; i++)
		{
       INFO_PRINTF2(_L(" %c"), ps1[i]);
       	}
	
	INFO_PRINTF2(_L(" %d"), (int)b);
    
	free(ps1);
	
	return ret;
    }
// -----------------------------------------------------------------------------
//Function Name :system_Test0
//API Tested :system
//TestCase Description: system returns -> 1 (the exit code from the called 
//executable) when the input is a valid executable name with a valid path.
// -----------------------------------------------------------------------------
TInt CTestStdlib::system_Test0()
    {
   
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In system_Test0L"));

    //--------------------------
	TPtrC scfgarg;
	int exitcode;
	_LIT( Kscfgarg, "scfgarg" );
     TBool res = GetStringFromConfig(ConfigSection(), Kscfgarg, scfgarg );

	if(!res)
		{
		INFO_PRINTF1(_L("unable to read the parameters from the cfg file!"));
		return KErrGeneral;
		}

	_LIT( Kexitcode, "exitcode" );
	res = GetIntFromConfig(ConfigSection(), Kexitcode, exitcode);
	if(!res)
		{
		INFO_PRINTF1(_L("unable to read the parameters from the cfg file!"));
		return KErrGeneral;
		}

		TBuf8<100> string;
		string.Copy(scfgarg);
		char* command = (char*) string.Ptr();
		command[string.Length()]='\0';
	
		int i = system(command);
		//--------------------------
	
		INFO_PRINTF2(_L("{Expected: 1} %d"), i);

		if (i != exitcode)
			{
			ret = KErrGeneral;
			}
	

	return ret;
	}

// -----------------------------------------------------------------------------
//Function Name :system_Test1
//API Tested :system
//TestCase Description:system returns -> -1 (command not found) when the 
//input command is not supported
// -----------------------------------------------------------------------------
TInt CTestStdlib::system_Test1()
    {
    
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In system_Test1L"));

		int i = system("date");
	
		INFO_PRINTF2(_L("{Expected: -1} %d"), i);

		if (i != -1)
			{
			ret = KErrGeneral;
			}
	

	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :system_Test2
//API Tested :system
//TestCase Description:system returns -> 1 (system is supported) when the
//input is NULL
// -----------------------------------------------------------------------------
TInt CTestStdlib::system_Test2()
    {
    
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In system_Test2L"));

 		int i = system(NULL);
		//--------------------------
	
		INFO_PRINTF2(_L("{Expected: 1} %d"), i);

		if (i != 1)
			{
			ret = KErrGeneral;
			}
	

	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :system_Test3
//API Tested :system
//TestCase Description:system returns -> 1 (exit code of the executable) 
//when the valid input command has leading white spaces.
// -----------------------------------------------------------------------------
TInt CTestStdlib::system_Test3()
    {
    
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In system_Test3L"));

	TPtrC scfgarg;
	int exitcode;
	
	_LIT( Kscfgarg, "scfgarg" );
    TBool res = GetStringFromConfig(ConfigSection(), Kscfgarg, scfgarg );

	if(!res)
		{
		INFO_PRINTF1(_L("unable to read the parameters from the cfg file!"));
		return KErrGeneral;
		}
	
	_LIT( Kexitcode, "exitcode" );
	res = GetIntFromConfig(ConfigSection(), Kexitcode, exitcode);
	if(!res)
		{
		INFO_PRINTF1(_L("unable to read the parameters from the cfg file!"));
		return KErrGeneral;
		}

		TBuf8<100> string;
		string.Copy(scfgarg);

		char command[64] = "   ";
		char* comname = (char*) string.Ptr();
		comname[string.Length()]='\0';

		strcat(command, comname);

		int i = system(command);
		//--------------------------

		INFO_PRINTF2(_L("{Expected: 1} %d"), i);

		if (i != exitcode)
			{
			ret = KErrGeneral;
			}
	

	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :system_Test4
//API Tested :system
//TestCase Description:system returns -> -1 (command not found) when the 
//input command has a tab character.
// -----------------------------------------------------------------------------
TInt CTestStdlib::system_Test4()
    {

    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In system_Test4L"));

 		int i = system("c:\\sys\\bin\\tstdlib_hello\\tworld.exe");
		//--------------------------

		INFO_PRINTF2(_L("{Expected: -1} %d"), i);

		if (i != -1)
			{
			ret = KErrGeneral;
			}
	

	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :system_Test5
//API Tested :system
//TestCase Description:system returns -> -1 (command not found) when the input
//execuatble doesn't exist.
// -----------------------------------------------------------------------------
TInt CTestStdlib::system_Test5()
    {
    
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In system_Test5L"));

 		int i = system("c:\\tstdlib_noexist.exe");
		//--------------------------

		INFO_PRINTF2(_L("{Expected: -1} %d"), i);

		if (i != -1)
			{
			ret = KErrGeneral;
			}
	

	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :system_Test6
//API Tested :system
//TestCase Description:system returns -> -1 (command not found) when the input
//execuatble doesn't exist.
// -----------------------------------------------------------------------------
TInt CTestStdlib::system_Test6()
    {
   
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In system_Test6L"));

 		int i = system("c:\\sys\\bin\\tstdlib_testsystem.exe a b c");
		//--------------------------

		INFO_PRINTF2(_L("{Expected: 4} %d"), i);

		if (i != 4)
			{
			ret = KErrGeneral;
			}
	

	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :abort_Test0
//API Tested :abort
//TestCase Description:abort never returns
//input --> void
// -----------------------------------------------------------------------------
TInt CTestStdlib::abort_Test0()
    {
    
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In abort_Test0L"));

   	int i = system("c:\\sys\\bin\\tstdlib_testabort.exe");

    //--------------------------  
	INFO_PRINTF2(_L("{Expected: 1} %d"), i);

	if (i != 1)
		{
		ret = KErrGeneral;
		}


	return ret;
    }

    
// -----------------------------------------------------------------------------
//Function Name :fopen_Test
//API Tested :fopen
//TestCase Description: If a file is openened with EFileShareReadersOnly, the fopen 
//                      should be able to open it read mode
//
// -----------------------------------------------------------------------------
TInt CTestStdlib::fopen_Test()
    {
    _LIT(KFileName, "C:\\FopenTest.txt");
    RFile file;
    RFs fileSession;
    TInt returnVal;
    TInt ret = KErrGeneral;
    if((returnVal=fileSession.Connect()) < KErrNone)
    	{
    	INFO_PRINTF2(_L("error opening file session, errcode = %d\n"),returnVal);
    	return returnVal;
    	}
    
    if((returnVal=file.Create(fileSession,KFileName,EFileWrite )) < KErrNone)
       	{
       	INFO_PRINTF2(_L("Error opening file, errcode = %d\n"),returnVal);
       	return returnVal;
       	}
    file.Close();
    if((returnVal=file.Open(fileSession,KFileName,EFileShareReadersOnly|EFileRead )) < KErrNone)
    	{
    	INFO_PRINTF2(_L("Error opening file, errcode = %d\n"),returnVal);
    	return returnVal;
    	}    
    
    const char* command = "Z:\\sys\\bin\\fopen_test.exe";
    const char* mode ="r";
    FILE* fp = popen(command, mode);
    if(fp==NULL)
    	{
        ERR_PRINTF2(_L("The errno set is %d\n"),errno);
    	return ret;
    	}
    if(pclose(fp) == -1)
       	{
    	ERR_PRINTF2(_L("The errno set is %d\n"),errno);
    	return ret;
    	}
    file.Close();
    if(unlink("C:\\FopenTest.txt") == -1)
       	{
       	ERR_PRINTF2(_L("The errno set is %d\n"),errno);
    	return ret;
    	}
    ret = KErrNone;
    return ret;
    }


// -----------------------------------------------------------------------------
//Function Name :exit_Test0
//API Tested :exit
//TestCase Description:exit never returns but we are having exit() call in a 
//stand alone application that we call using the API system(). so, we expect
//that the system's return value is same as the exitcode that we used in the 
//application that is made to exit. also, we expect the functions registered
//with atexit to be called in the reverse order of registration before the 
//application exits.
//input --> exitcode is 1 {exit(1)}
// -----------------------------------------------------------------------------
TInt CTestStdlib::exit_Test0()
    {
    
    
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In exit_Test0L"));

   	int i = system("c:\\sys\\bin\\tstdlib_testexit.exe");
	char buf1[64];
	char buf2[64];

	FILE *fp = fopen("c:\\some.txt", "r");
	if(fp)
		{
		fscanf(fp, "%s", buf1);
		fgetc(fp); //skip the space
		fscanf(fp, "%s", buf2);
		}
	
	TBuf8<50> tempString1((TUint8 *)&buf1);
	TBuf8<50> tempString2((TUint8 *)&buf2);
	TBuf<50> displayString1;
	TBuf<50> displayString2;
	
	displayString1.Copy(tempString1);
	displayString2.Copy(tempString2);
	INFO_PRINTF3(_L("{Executing exitfunc3.: %S %S}"), &displayString1, &displayString2);

	strcat(buf1, " ");
	strcat(buf1, buf2);
	if(strcmp(buf1, "Executing exitfunc3.") != 0)
		{
		ret = KErrGeneral;
		}
	
	if(fp)
		{
		fgetc(fp); //skip the newline

		fscanf(fp, "%s", buf1);
		fgetc(fp); //skip the space
		fscanf(fp, "%s", buf2);
		}

	tempString1 = (TUint8 *)buf1;
	tempString2 = (TUint8 *)buf2;
	displayString1.Copy(tempString1);
	displayString2.Copy(tempString2);
	INFO_PRINTF3(_L("{Executing exitfunc2.: %S %S}"), &displayString1, &displayString2);

	strcat(buf1, " ");
	strcat(buf1, buf2);
	if(strcmp(buf1, "Executing exitfunc2.") != 0)
		{
		ret = KErrGeneral;
		}
	
	if(fp)
		{
		fgetc(fp); //skip the newline

		fscanf(fp, "%s", buf1);
		fgetc(fp); //skip the space
		fscanf(fp, "%s", buf2);
		}
	
	tempString1 = (TUint8 *)buf1;
	tempString2 = (TUint8 *)buf2;
	displayString1.Copy(tempString1);
	displayString2.Copy(tempString2);
	INFO_PRINTF3(_L("{Executing exitfunc1.: %S %S}"), &displayString1, &displayString2);

	strcat(buf1, " ");
	strcat(buf1, buf2);
	if(strcmp(buf1, "Executing exitfunc1.") != 0)
		{
		ret = KErrGeneral;
		}
	
	if(fp)
		{
		fclose(fp);	
		}

	if (i != 1)
		{
		ret = KErrGeneral;
		}


	
	
	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :exit_Test1
//API Tested :exit
//TestCase Description:exit never returns but we are having exit() call in a 
//stand alone application that we call using the API system(). so, we expect
//that the system's return value is same as the exitcode that we used in the
//application that is made to exit. we don't have any functions registered with
//atexit for this case.
//input --> exitcode is 3 {exit(3)}
// -----------------------------------------------------------------------------
TInt CTestStdlib::exit_Test1()
    {
    
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In exit_Test1L"));

   	int i = system("c:\\sys\\bin\\tstdlib_testexitec.exe");

    //--------------------------  
	INFO_PRINTF2(_L("{Expected: 3} %d"), i);

	if (i != 3)
		{
		ret = KErrGeneral;
		}

	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :_exitE_Test0
//API Tested :exit
//TestCase Description:exit never returns but we are having exit() call in a 
//stand alone application that we call using the API system(). so, we expect
//that the system's return value is same as the exitcode that we used in the
//application that is made to exit. we don't have any functions registered with
//atexit for this case.
//input --> exitcode is 3 {exit(3)}
// -----------------------------------------------------------------------------
TInt CTestStdlib::_exitE_Test0()
    {
    
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In _exitE_Test0L"));

   	int i = system("c:\\sys\\bin\\tstdlib_test_exitE.exe");

    //--------------------------  
	INFO_PRINTF2(_L("{Expected: 5} %d"), i);

	if (i != 5)
		{
		ret = KErrGeneral;
		}

	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :_exit_Test0
//API Tested :exit
//TestCase Description:exit never returns but we are having exit() call in a 
//stand alone application that we call using the API system(). so, we expect
//that the system's return value is same as the exitcode that we used in the
//application that is made to exit. we don't have any functions registered with
//atexit for this case.
//input --> exitcode is 3 {exit(3)}
// -----------------------------------------------------------------------------
TInt CTestStdlib::_exit_Test0()
    {
    
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In _exit_Test0L"));

   	int i = system("c:\\sys\\bin\\tstdlib_test_exit.exe");

    //--------------------------  
	INFO_PRINTF2(_L("{Expected: 6} %d"), i);

	if (i != 6)
		{
		ret = KErrGeneral;
		}

	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :atexit_Test0
//API Tested :atexit
//TestCase Description:atexit returns non-zero on registration failure
//atexit fails when malloc fails! as the first 32 funs are allocated statically
//and the rest, dynamically.
//input --> function pointer to an exit function.
//This test case executes all code only for DEBUG build as it can simulate
//memory failure only in DEBUG build
// -----------------------------------------------------------------------------
TInt CTestStdlib::atexit_Test0()
    {
    
    TInt ret = KErrNone;
    INFO_PRINTF1(_L("In atexit_Test0L"));

#ifdef _DEBUG
	
	//call some stdlib API to make sure that reent structure will be 
	//created successfully
	char dummy[20];
	sprintf(dummy, "%s", "Dummy");

	vfuncp func_table[] = {
		exitfunc0, exitfunc1, exitfunc2, exitfunc3, exitfunc4,
		exitfunc5, exitfunc6, exitfunc7, exitfunc8, exitfunc9,
		exitfunc10, exitfunc11, exitfunc12, exitfunc13, exitfunc14,
		exitfunc15, exitfunc16, exitfunc17, exitfunc18, exitfunc19,
		exitfunc20, exitfunc21, exitfunc22, exitfunc23, exitfunc24,
		exitfunc25, exitfunc26, exitfunc27, exitfunc28, exitfunc29,
		exitfunc30, exitfunc31, exitfunc32
	};
	
	int i = 0, atexitret1 = 0, atexitret2 = 0;
	int count = 0;
	int numfuncs = sizeof(func_table)/sizeof(vfuncp);

	// loop until no more can be added
	while(count < LEAST_ATEXIT && (atexitret1 = atexit(func_table[3])) >= 0) 
		{
		INFO_PRINTF2(_L("Registered exitfunc%d with atexit()\n"), i);
		count++;
		i = (i+1) % numfuncs;
		}
	INFO_PRINTF2(_L("funcs reg with atexit --> {Expected: 32} %d"), count);
	INFO_PRINTF2(_L("{Expected: zero} %d"), atexitret1);

	if(KErrNone == atexitret1)
		{
		RHeap *heap_handle = Backend()->Heap();

		//Make sure that u will be calling 33rd exit function
		if( (atexitret2 = atexit(func_table[numfuncs])) != -1)
			{
			INFO_PRINTF1(_L("Registration of the 33rd exit function didn't fail"));
			ret = KErrNone;
			}

		heap_handle->__DbgSetAllocFail(RAllocator::ENone, 1);
		heap_handle->__DbgMarkEnd(0);
	
		}
	else
		{
		ERR_PRINTF1(_L("Registration of one of the 32 exit functions failed"));
		ret = KErrGeneral;
		}
	
	INFO_PRINTF2(_L("{Expected: non-zero} %d"), atexitret2);

#else
	INFO_PRINTF1(_L("This test case executes all code only for DEBUG build \
	              as it can simulate memory failure only in DEBUG build"));
#endif //_DEBUG

	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name :atexit_Test1
//API Tested :atexit
//TestCase Description:atexit returns 0 on successful registration
//input --> function pointer to an exit function.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atexit_Test1()
    {
    
    
    TInt ret = KErrNone;

    INFO_PRINTF1(_L("In atexit_Test1L"));

	vfuncp func_table[] = {
		exitfunc0, exitfunc1, exitfunc2, exitfunc3, exitfunc4,
		exitfunc5, exitfunc6, exitfunc7, exitfunc8, exitfunc9,
		exitfunc10, exitfunc11, exitfunc12, exitfunc13, exitfunc14,
		exitfunc15, exitfunc16, exitfunc17, exitfunc18, exitfunc19,
		exitfunc20, exitfunc21, exitfunc22, exitfunc23, exitfunc24,
		exitfunc25, exitfunc26, exitfunc27, exitfunc28, exitfunc29,
		exitfunc30, exitfunc31
	};
	
	int i = 0, atexitret1 = 0;
	int count = 0;
	int numfuncs = sizeof(func_table)/sizeof(vfuncp);

	// loop until no more can be added
	while(count < LEAST_ATEXIT && (atexitret1 = atexit(func_table[i])) >= 0) 
		{
		INFO_PRINTF2(_L("Registered exitfunc%d with atexit()\n"), i);
		count++;
		i = (i+1) % numfuncs;
		}

	INFO_PRINTF2(_L("funcs reg with atexit --> {Expected: 32} %d"), count);

	if (atexitret1 != 0 )
		{
		ret = KErrGeneral;
		}
	
	
	
	return ret;
    }
    
    // -----------------------------------------------------------------------------
//Function Name :abs_neg_max
//API Tested :abs
//TestCase Description: abs(x) returns the absolute value of x.
//      This case finds the absolute value of the maximum negative
//        value in the acceptable range. And compares result with 
//          the expected value(i.e. INT_MAX).
// -----------------------------------------------------------------------------
TInt CTestStdlib::abs_neg_max(  )
    {
    
    
    
    INFO_PRINTF1(_L("Tstdlib: In abs(max_negative_value)"));
  
    int j=abs(INT_MIN+1);
    INFO_PRINTF1(_L("abs(maximum negative value acceptable)"));
    INFO_PRINTF2(_L("abs(INT_MIN+1)-> %d"),j);
    
    
    	
     if(j==INT_MAX)
		{
        return KErrNone;
		}
     else
		{
        return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :abs_good_param
//API Tested :abs
//TD TestCase Number :1_11_2_1213
//TD TestCase Identifier :1213
//TestCase Description: abs(x) returns the absolute value of x.
//      This case finds the absolute value of the some integer
//        value in the acceptable range passed through parameter.
//           And compares result with the expected value which is 
//              also among the parameters.

//Entry Into Source:
// -----------------------------------------------------------------------------
TInt CTestStdlib::abs_good_param(  )
    {
    
    INFO_PRINTF1(_L("Tstdlib: In abs(good_parameters)"));
  
   int intVal1=0, intVal2=0;
    
    _LIT( KintVal1, "intVal1" );
	TBool res1 = GetIntFromConfig(ConfigSection(), KintVal1, intVal1);
    _LIT( KintVal2, "intVal2" );
	TBool res2 = GetIntFromConfig(ConfigSection(), KintVal2, intVal2);
	if (res1 && res2)
		{
         INFO_PRINTF3(_L("Tstdlib: Input->%d Expected-> %d"), intVal1, intVal2);
    
    	
    
        int j=abs(intVal1);
        
    	
    
        INFO_PRINTF3(_L("abs(%d)-> %d"), intVal1, j);
        INFO_PRINTF2(_L("Expected-> %d"), intVal2);

        if(j!=intVal2)
            {
            return KErrGeneral;
            }
        else
            {
            return KErrNone;
            }
		}
    else
		{
        INFO_PRINTF1(_L("Expected Param: <integer-input> <expected-value>"));
        return KErrGeneral;
		}
	}
// -----------------------------------------------------------------------------
//Function Name :abs_pos_max
//API Tested :abs
//TestCase Description: abs(x) returns the absolute value of x
//      This case finds the absolute value of the maximum positive
//        value in the acceptable range using abs().And compares result with 
//          the expected value (i.e. INT_MAX).
// -----------------------------------------------------------------------------
TInt CTestStdlib::abs_pos_max(  )
    {
    
    
    INFO_PRINTF1(_L("Tstdlib: In abs(+ve_maximum)"));
    INFO_PRINTF1(_L("In abs(+ve_maximum)"));


    int j=abs(INT_MAX);
  
    INFO_PRINTF1(_L("abs(maximum positive value acceptable)"));
    INFO_PRINTF2(_L("abs(INT_MAX)-> %d"), j);
    
    
    	
    if(j==INT_MAX)
		{
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :labs_neg_max
//API Tested :labs
//TestCase Description: labs(x) returns the absolute value of x
//      This case finds the absolute value of the maximum negative
//        value in the acceptable range. And compares result with 
//          the expected value (i.e. LONG_MAX).
// -----------------------------------------------------------------------------
TInt CTestStdlib::labs_neg_max(  )
    {
    
    
    INFO_PRINTF1(_L("Tstdlib: In labs(max_negative_value)"));
  
    long int j=labs(LONG_MIN+1);
    INFO_PRINTF1(_L("labs(maximum negative value acceptable)"));
    INFO_PRINTF2(_L("labs(LONG_MIN+1)-> %d"),j);
    
    
    
     if(j==LONG_MAX)
        {
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :labs_good_param
//API Tested :labs
//TestCase Description: labs(x) returns the absolute value of x
//      This case finds the absolute value of the some integer
//        value in the acceptable range passed through parameter.
//           And compares result with the expected value which is 
//              also among the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::labs_good_param(  )
    {
    
    INFO_PRINTF1(_L("Tstdlib: In labs(good_parameters)"));
  
    int intVal1=0, intVal2=0;
    
    

	_LIT( KintVal1, "intVal1" );
	TBool res1 = GetIntFromConfig(ConfigSection(), KintVal1, intVal1);
    _LIT( KintVal2, "intVal2" );
	TBool res2 = GetIntFromConfig(ConfigSection(), KintVal2, intVal2);
	if (res1 && res2)
		{
        INFO_PRINTF3(_L("Tstdlib: Input->%d Expected-> %d"), intVal1, intVal2);
		
    
        long int j=labs(intVal1);
        
		
    
        INFO_PRINTF3(_L("labs(%d)-> %d"), intVal1, j);
        INFO_PRINTF2(_L("Expected-> %d"), intVal2);

        if(j!=intVal2)
            {
            return KErrGeneral;
            }
        else
            {
            return KErrNone;
            }
        }
    else
        {
        INFO_PRINTF1(_L("Expected Param: <integer-input> <expected-value>"));
        return KErrGeneral;
        }
    }

// -----------------------------------------------------------------------------
//Function Name :labs_pos_max
//API Tested :labs
//TestCase Description: labs(x) returns the absolute value of x
//      This case finds the absolute value of the maximum positive
//        value in the acceptable range using abs().And compares result with 
//          the expected value (ie. LONG_MAX).
// -----------------------------------------------------------------------------
TInt CTestStdlib::labs_pos_max(  )
    {
    
       
    INFO_PRINTF1(_L("Tstdlib: In labs(+ve_maximum)") );
    
    long int j=labs(LONG_MAX);
  
    INFO_PRINTF1(_L("labs(maximum positive value acceptable)"));
    INFO_PRINTF2(_L("labs(LONG_MAX)-> %d"), j);
    
    
    
    if(j==LONG_MAX)
        {
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :llabs_pos_max
//API Tested :llabs
//TestCase Description: llabs(x) returns the absolute value of x
//      This case finds the absolute value of the maximum positive
//        value in the acceptable range using abs().And compares result with 
//          the expected value (ie. LLONG_MAX).
// -----------------------------------------------------------------------------
TInt CTestStdlib::llabs_pos_max(  )
    {
    
    
    INFO_PRINTF1(_L("Tstdlib: In llabs(+ve_maximum)") );
    
    long long j=llabs(LLONG_MAX);
  
    INFO_PRINTF1(_L("llabs(maximum positive value acceptable)"));
    INFO_PRINTF2(_L("llabs(LLONG_MAX)-> %ld"), j);
    
    
    
    if(j==LLONG_MAX)
        {
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
    }
  
// -----------------------------------------------------------------------------
//Function Name :llabs_neg_max
//API Tested :llabs
//TestCase Description: llabs(x) returns the absolute value of x
//      This case finds the absolute value of the maximum negative
//        value in the acceptable range. And compares result with 
//          the expected value (i.e. LLONG_MAX).
// -----------------------------------------------------------------------------
TInt CTestStdlib::llabs_neg_max(  )
    {    
    
    
    INFO_PRINTF1(_L("Tstdlib: In labs(max_negative_value)"));
  
    long long j=llabs(LLONG_MIN+1);
    INFO_PRINTF1(_L("llabs(maximum negative value acceptable)"));
    INFO_PRINTF2(_L("llabs(LLONG_MIN+1)-> %ld"),j);
    
    
    
    if(j==LLONG_MAX)
        {
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :llabs_good_param
//API Tested :llabs
//TestCase Description: llabs(x) returns the absolute value of x
//      This case finds the absolute value of the some integer
//        value in the acceptable range passed through parameter.
//           And compares result with the expected value which is 
//              also among the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::llabs_good_param( )
    {
    
    
    
    INFO_PRINTF1(_L("Tstdlib: In llabs(good_parameters)") );
  
 	long long int x = 4294967396;
 	long long int y = 5343546758;
  
  	long long int i = llabs(x);
    long long int j = llabs(-y);
        
    INFO_PRINTF3(_L("Expected-> %ld, %ld"), x, y);

    
    
    if((i != x) || (j != y))
		{
        return KErrGeneral;
        }
	else
        {
        return KErrNone;
        }
    }
    
 // -----------------------------------------------------------------------------
//Function Name :atoi_null_string
//API Tested :atoi
//TestCase Description: 
//     atoi(x) returns the converted value (ascii to int)
//           if the value can be represented.
//      This case finds the conversion of NULL string.
//          And compares result with 0,the expected value.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atoi_null_string(  )
   {
    
    
   INFO_PRINTF1(_L("Tstdlib: In atoi(NULL string)") );

   int  i=atoi("");

   INFO_PRINTF2(_L("atoi(\"\")-> %d"), i);

    
    
   if(i==0)
        {
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
   }


// -----------------------------------------------------------------------------
//Function Name :atoi_good_param
//API Tested :atoi
//TestCase Description: 
//     atoi(x) returns the converted value (ascii to int)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atoi_good_param(  )
    {
      
    INFO_PRINTF1(_L("Tstdlib: In atoi(good_parameters)"));
 
    int intVal = 0;
    TPtrC string;
	
	_LIT( Kstring, "string" );
    TBool res1 = GetStringFromConfig(ConfigSection(), Kstring, string );
    _LIT( KintVal, "intVal" );
	TBool res2 = GetIntFromConfig(ConfigSection(), KintVal, intVal);
    if(res1 && res2 )
		{
		INFO_PRINTF3(_L("Tstdlib: Input-> %S: Expected-> %d"), &string, intVal );
		TBuf8<100> buf;
		buf.Copy(string);
		char* ptr = (char*) buf.Ptr();
		ptr[buf.Length()]='\0';
    
		
    
		int atoiret=atoi(ptr);
    
		
		
		if(atoiret!=intVal)
			{
			INFO_PRINTF3(_L("atoi(\"%S\")-> %d"),&string, atoiret);
			INFO_PRINTF2(_L("%d was the expected output"), intVal);
			return KErrGeneral;    
			}
		else
			{
			INFO_PRINTF3(_L("atoi(\"%S\")-> %d"),&string, atoiret);
			INFO_PRINTF2(_L("Expected -> %d "), intVal);
			return KErrNone;    
			}

		}
    else
        {
        INFO_PRINTF1(_L("Expected Parameters in the configuration file->"));
        INFO_PRINTF1(_L("<input_string> <expected_output>"));
        return KErrGeneral;
        }
    }

// -----------------------------------------------------------------------------
//Function Name :atof_null_string
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double),
//           if the value can be represented.
//      This case finds the conversion of NULL string.
//      And compares result with 0.000000,the expected value.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_null_string(  )
    {
    
    
    INFO_PRINTF1(_L("Tstdlib: In atof(NULL_string)") );
    double  i=atof("");
    
    INFO_PRINTF2(_L("atof(\"\")-> %f"), i);
    
    
    
    if(i==0)
        {
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :atof_pos_floatnum
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "0.76786"and compares the result with the expected 
//        value which is the second member in the structure
//         as in the test case below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_pos_floatnum(  )
    {
    INFO_PRINTF1(_L("Tstdlib: In atof(+ve_float_num_string)"));
     
	struct test_neg_float
        {
        char* float_string;
        double doubleVal;
        };

    struct test_neg_float test[3]=
        {
        
		{"0.76786",0.767860},
        {"1.787800",1.787800},
        {"67.78",67.780000}

        };

	TInt i=0;
    for(i=0;i<3;i++)
		{
		
    
        double ret = atof(test[i].float_string);
            
		
    
        INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
        INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
        if( ret != test[i].doubleVal )
			{
            INFO_PRINTF1(_L("Unexpected return value from atof()"));
            return KErrGeneral;
            }
        
        }
    return KErrNone;
  
    }

// -----------------------------------------------------------------------------
//Function Name :atof_neg_floatnum
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "-0.76786" (i.e negative float no.s)and compares the
//          result with the expected value which is the second 
//          member in the structure as in the testcase below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_neg_floatnum( )
    {
    
    INFO_PRINTF1(_L("Tstdlib:atof(-ve_float_num_string)"));
    struct test_neg_float
        {
        char* float_string;
        double doubleVal;
        };

    struct test_neg_float test[3]=
        {
        {"-0.76786",-0.767860},
        {"-75478.987987",-75478.987987},
        {"-67.78",-67.780000}
        };

        TInt i=0;
	for(i=0;i<3;i++)
		{
		
    
        double ret = atof(test[i].float_string);
            
		
		
        INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
        INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
        if( ret != test[i].doubleVal )
			{
            INFO_PRINTF1(_L("Unexpected return value from atof()"));
            return KErrGeneral;
            }
        
		}
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atof_alpha
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "abcdef" (i.e alphanumeric input)and compares the
//          result with the expected value which is the second 
//          member in the structure as in the testcase below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_alpha( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(alphanumeric_string)"));
    struct test_neg_float
        {
        char* float_string;
        double doubleVal;
        };

    struct test_neg_float test[3]=
        {
        {"fgdsfg",0.000000},
        {"abcd",0.000000},
        {"efgh",0.000000},
        };

	TInt i=0;
    for(i=0;i<3;i++)
        {
		
    
        double ret = atof(test[i].float_string);
            
		
    
        INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
        INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
		if( ret != test[i].doubleVal )
			{
            INFO_PRINTF1(_L("Unexpected return value from atof()"));
            return KErrGeneral;
            }
        
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atof_alpha_floatnum
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "abcdef789.9" (i.e combination of alphanumeric and float no.s)
//          and compares the result with the expected value which is  
//          the second member in the structure as in the testcase below.
// -----------------------------------------------------------------------------


TInt CTestStdlib::atof_alpha_floatnum( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(alpha_floatnum_string)"));
    struct test_neg_float
        {
        char* float_string;
        double doubleVal;
        };

    struct test_neg_float test[3]=
        {
        {"fgdsfg4.56",0.000000},
        {"abcd5656.8",0.000000},
        {"efgh76786.78790",0.000000},
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
		
		double ret = atof(test[i].float_string);
            
		
    
		INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
        INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
        if( ret != test[i].doubleVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atof_pos_floatnum_alpha
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "789.9abcdef" (i.e combination of alphanumeric and float no.s)
//          and compares the result with the expected value which is  
//          the second member in the structure as in the testcase below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_pos_floatnum_alpha( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(+ve_float_num_alpha_string)"));
    struct test_neg_float
        {
		char* float_string;
		double doubleVal;
        };

    struct test_neg_float test[3]=
        {
		{"+54.4ghds",54.4},
		{"75478.987987uff",75478.987987},
		{"67.78gtye",67.780000}
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
		
		
		double ret = atof(test[i].float_string);
            
		
    
		INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
		if( ret != test[i].doubleVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atof_neg_floatnum_alpha
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "-789.9abcdef" (i.e combination of alphanumeric and float no.s)
//          and compares the result with the expected value which is  
//          the second member in the structure as in the testcase below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_neg_floatnum_alpha( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(-ve_float_num_alpha_string)"));
    struct test_neg_float
        {
			char* float_string;
            double doubleVal;
        };

    struct test_neg_float test[3]=
        {
            {"-0.76786fgsd",-0.767860},
            {"-75478.987987hguyh",-75478.987987},
            {"-67.78hihru",-67.780000}
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
		
    
        double ret = atof(test[i].float_string);
            
		
    
		INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
		if( ret != test[i].doubleVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atof_leading_zero
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "000000789.9" (i.e float no. string with leading zeroes)
//          and compares the result with the expected value which is  
//          the second member in the structure as in the testcase below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_leading_zero( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(string_leading zeroes)"));
    struct test_neg_float
        {
			char* float_string;
			double doubleVal;
        };

    struct test_neg_float test[3]=
        {
            {"+000000765.787",765.787000},
            {"+00075478.987987",75478.987987},
            {"000000067.78",67.780000}
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
		
		
		double ret = atof(test[i].float_string);
            
		
    
		INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
		if( ret != test[i].doubleVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    
    }


// -----------------------------------------------------------------------------
//Function Name :atof_neg_leading_zero
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "-000000789.9" (i.e -ve float no. string with leading zeroes)
//          and compares the result with the expected value which is  
//          the second member in the structure as in the testcase below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_neg_leading_zero( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(string_-ve_leading zeroes)"));
    struct test_neg_float
        {
			char* float_string;
			double doubleVal;
        };

    struct test_neg_float test[3]=
        {
            {"-000000765.787",-765.787000},
            {"-00075478.987987",-75478.987987},
            {"-000000067.78",-67.780000}
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
		
    
		double ret = atof(test[i].float_string);
            
		
            
		INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
		if( ret != test[i].doubleVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atof_pos_floatnum_pos_expo
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "8798e3" (i.e +ve float no. string in exponential(+ve) form)
//          and compares the result with the expected value which is  
//          the second member in the structure as in the testcase below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_pos_floatnum_pos_expo( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(+ve_floatnum_+ve_expo)"));
    struct test_neg_float
        {
			char* float_string;
			double doubleVal;
        };

    struct test_neg_float test[3]=
        {
            {"+9e4", 90000.000000},
            {"86778E2",8677800.000000},
            {"67e3",67000.000000}
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
		
    
		double ret = atof(test[i].float_string);
            
		
    
		INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
		if( ret != test[i].doubleVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atof_neg_floatnum_pos_expo
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "-8798e3" (i.e -ve float no. string in exponential(+ve) form)
//          and compares the result with the expected value which is  
// ----------------------------------------------------------------------------
TInt CTestStdlib::atof_neg_floatnum_pos_expo( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(-ve_floatnum_+ve_expo)"));
    struct test_neg_float
        {
			char* float_string;
			double doubleVal;
        };

    struct test_neg_float test[3]=
        {
            {"-9e4", -90000.000000},
            {"-86778E2",-8677800.000000},
            {"-67e3",-67000.000000}
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
		
    
		double ret = atof(test[i].float_string);
            
		
        
		INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
		if( ret != test[i].doubleVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atof_pos_floatnum_neg_expo
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "8798e-3" (i.e +ve float no. string in exponential(-ve) form)
//          and compares the result with the expected value which is  
//          the second member in the structure as in the testcase below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_pos_floatnum_neg_expo( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(+ve_floatnum_-ve_expo)"));
    struct test_neg_float
        {
			char* float_string;
			double doubleVal;
        };

    struct test_neg_float test[3]=
        {
            {"9e-4", 0.000900},
            {"86778E-2",867.780000},
            {"67e-3",0.067000}
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
    
		
    
		double ret = atof(test[i].float_string);
            
		
			
		INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
		if( ret != test[i].doubleVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atof_neg_floatnum_neg_expo
//API Tested :atof
//TestCase Description: 
//     atof(x) returns the converted value (ascii to double)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "-8798e-3" (i.e -ve float no. string in exponential(-ve) form)
//          and compares the result with the expected value which is  
//          the second member in the structure as in the testcase below.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atof_neg_floatnum_neg_expo( )
    {
    
    INFO_PRINTF1(_L("Tstdlib: atof(-ve_floatnum_-ve_expo)"));
    struct test_neg_float
        {
			char* float_string;
			double doubleVal;
        };

    struct test_neg_float test[3]=
        {
            {"-9e-4", -0.000900},
            {"-86778E-2",-867.780000},
            {"-67e-3",-0.067000}
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
		
        
		double ret = atof(test[i].float_string);
    
		
    
		INFO_PRINTF3(_L("atof(\"%lf\")-> %lf"),test[i].doubleVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].doubleVal);
		if( ret != test[i].doubleVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :atol_null_string
//API Tested :atol
//TestCase Description: 
//     atol(x) returns the converted value (ascii to long)
//           if the value can be represented.
//      This case finds the conversion of NULL string.
//      And compares result with 0,the expected value.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atol_null_string(  )
    {
    
    
    INFO_PRINTF1(_L("Tstdlib: In atol(NULL string)") );
 
    int  i=atol("");
    
    INFO_PRINTF2(_L("atol(\"\")-> %d"), i);
    
    
    
    if(i==0)
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
//Function Name :atol_good_param
//API Tested :atoi
//TestCase Description: 
//     atol(x) returns the converted value (ascii to long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atol_good_param(  )
    {
    INFO_PRINTF1(_L("Tstdlib: In atol(good_parameters)") );
 
    int intVal;
    TPtrC string;
	
	_LIT( Kstring, "string" );
    TBool res1 = GetStringFromConfig(ConfigSection(), Kstring, string );
	_LIT( KintVal, "intVal" );
	TBool res2 = GetIntFromConfig(ConfigSection(), KintVal, intVal);

    if(res1 && res2)
        {
        INFO_PRINTF3(_L("Tstdlib: Input-> %S, Expected -> %d"), &string, intVal );

        TBuf8<100> buf;
        buf.Copy(string);


        char* ptr = (char*) buf.Ptr();
        ptr[buf.Length()]='\0';
        
		
        
        long atolret=atol(ptr);

		
		
        if(atolret!=intVal)
            {
            INFO_PRINTF3(_L("atol(\"%S\")-> %d"),&string, atolret);
            INFO_PRINTF2(_L("Expected-> %d") , intVal);
            return KErrGeneral;    
            }
        else
            {
            INFO_PRINTF3(_L("atol(\"%S\")-> %d"),&string, atolret);
            INFO_PRINTF1(_L("No error"));
            return KErrNone;    
            }
      
        }
        else
			{
            INFO_PRINTF1(_L("Expected Parameters in the configuration file-> "));
            INFO_PRINTF1(_L("<input_string> <expected_output_longNum>"));
            return KErrGeneral;    
			}
           
	}

// -----------------------------------------------------------------------------
//Function Name :atollbasic
//API Tested :atoll
//TestCase Description: 
//     atoll(x) returns the converted value (ascii to long long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atollbasic()
	{
	
    
	INFO_PRINTF1(_L("In atollbasicL"));

	long long l = atoll("32677788998");

	INFO_PRINTF2(_L("{Expected: 32677788998 Obtained: %ld}"), l);

	
    
	if(l != 32677788998)
		{
 		return KErrGeneral;
		}

	return KErrNone;
	}
 
// -----------------------------------------------------------------------------
//Function Name :atollspacecheck
//API Tested :atoll
//TestCase Description: 
//     atoll(x) returns the converted value (ascii to long long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// ----------------------------------------------------------------------------- 
TInt CTestStdlib::atollspacecheck()
	{
	
    
	INFO_PRINTF1(_L("In atollspacecheckL"));

	long long l = atoll("       32677788998");

	INFO_PRINTF2(_L("{Expected: 32677788998 Obtained: %ld}"), l);

	
    
	if(l != 32677788998)
 		{
 		return KErrGeneral;
		}

	return KErrNone;
	}
 
// -----------------------------------------------------------------------------
//Function Name :atollsignedsrc
//API Tested :atoll
//TestCase Description: 
//     atoll(x) returns the converted value (ascii to long long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atollsignedsrc()
	{
	
    
	INFO_PRINTF1(_L("In atollsignedsrcL"));

	long long l = atoll("-32677788998");

	INFO_PRINTF2(_L("{Expected: -32677788998 Obtained: %ld}"), l);

	

	if(l != -32677788998)
 		{
 		return KErrGeneral;
		}

	return KErrNone;
	}
 
// -----------------------------------------------------------------------------
//Function Name :atolloctalcheck
//API Tested :atoll
//TestCase Description: 
//     atoll(x) returns the converted value (ascii to long long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atolloctalcheck()
	{
	

	INFO_PRINTF1(_L("In atolloctalcheckL"));

	long long l = atoll("010");

	INFO_PRINTF2(_L("{Expected: 10 Obtained: %ld}"), l);

	
    
	if(l != 10)
 		{
 		return KErrGeneral;
		}

	return KErrNone;
	}
 
// -----------------------------------------------------------------------------
//Function Name :atollhexcheck
//API Tested :atoll
//TestCase Description: 
//     atoll(x) returns the converted value (ascii to long long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atollhexcheck()
	{
	

	INFO_PRINTF1(_L("In atollhexcheckL"));

	long long l = atoll("0xa");

	INFO_PRINTF2(_L("{Expected: 0 Obtained: %ld}"), l);

	

	if(l != 0)
 		{
 		return KErrGeneral;
		}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
//Function Name :atolldigitfirstcheck
//API Tested :atoll
//TestCase Description: 
//     atoll(x) returns the converted value (ascii to long long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atolldigitfirstcheck()
	{
	
    
	INFO_PRINTF1(_L("In atolldigitfirstcheckL"));

	long long l = atoll("6454756356bs");

	INFO_PRINTF2(_L("{Expected: 6454756356 Obtained: %ld}"), l);

	
    
	if(l != 6454756356)
 		{
 		return KErrGeneral;
		}

	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//Function Name :atollalphafirstcheck
//API Tested :atoll
//TestCase Description: 
//     atoll(x) returns the converted value (ascii to long long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atollalphafirstcheck()
	{
	
    
	INFO_PRINTF1(_L("In atollalphafirstcheckL"));

	long long l = atoll("fgsdf64547");

	INFO_PRINTF2(_L("{Expected: 0 Obtained: %ld}"), l);

	
		
	if(l != 0)
 		{
 		return KErrGeneral;
		}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
//Function Name :atollalphacheck
//API Tested :atoll
//TestCase Description: 
//     atoll(x) returns the converted value (ascii to long long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atollalphacheck()
	{
	
    
	INFO_PRINTF1(_L("In atollalphacheckL"));

	long long l = atoll("hsgdfhaff");

	INFO_PRINTF2(_L("{Expected: 0 Obtained: %ld}"), l);

	

	if(l != 0)
 		{
 		return KErrGeneral;
		}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
//Function Name :atollleadingzeroscheck
//API Tested :atoll
//TestCase Description: 
//     atoll(x) returns the converted value (ascii to long long)
//           if the value can be represented.
//      This case finds the conversion of input strings like
//       "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh"..
//          And compares the result with the expected value as passed
//          in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::atollleadingzeroscheck()
	{
	

	INFO_PRINTF1(_L("In atollleadingzeroscheckL"));

	long long l = atoll("-00000005324532657");

	INFO_PRINTF2(_L("{Expected: -5324532657 Obtained: %ld}"), l);

	
    
	if(l != -5324532657)
 		{
 		return KErrGeneral;
		}

	return KErrNone;
	}


// -----------------------------------------------------------------------------
//Function Name :div_good_param
//API Tested :div
//TestCase Description: 
//     div(a,b) divides a by b and  return a structure of type div_t,
//       comprising both the quotient and the remainder(integer type)
//      This case finds the conversion of input integers, example:
//      div(7,2) which should return 3 and 1 respectively in the
//       structure members.And compares the result with the expected 
//        value as passed in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::div_good_param(  )
    {
    
    INFO_PRINTF1(_L("Tstdlib: In div(good_parameters)") );
    int intVal1;
    int intVal2;
    TInt ret1=KErrNone, ret2 = KErrNone;
    div_t j;
	
	_LIT( KintVal1, "intVal1" );
	TBool res1 = GetIntFromConfig(ConfigSection(), KintVal1, intVal1);
	_LIT( KintVal2, "intVal2" );
	TBool res2 = GetIntFromConfig(ConfigSection(), KintVal2, intVal2);
	    
    if( res1 && res2 && ret1==KErrNone && ret2==KErrNone)
		{
        if(intVal2!=0)
			{
			
        
            j=div(intVal1,intVal2);

    		
    	
            INFO_PRINTF3(_L("div( %d, %d )"),intVal1, intVal2);
            INFO_PRINTF3(_L("Result->Quotient=%d Remainder=%d"),j.quot,j.rem);
            int result = (intVal2 * j.quot) + j.rem;
                
            if( result == intVal1 )
	            {
                return KErrNone;
                }
            else
                {
                INFO_PRINTF1(_L("Unexpected output"));
                return KErrGeneral;
                }
			}
		else
			{
			INFO_PRINTF1(_L("This causes an exception"));
			return KErrNone;
			}
		}
        else
			{
            INFO_PRINTF1(_L("Expected input: <Dividend> <Divisor>.."));
            INFO_PRINTF1(_L("<Expected-quotient> <Expected-remainder>"));
            return KErrGeneral;
			}       
    
    }

// -----------------------------------------------------------------------------
//Function Name :ldiv_good_param
//API Tested :ldiv
//TestCase Description: 
//     ldiv(a,b) divides a by b and  return a structure of type ldiv_t,
//       comprising both the quotient and the remainder(all long type). 
//      This case finds the conversion of input integers, example:
//      ldiv(7,2) which should return 3 and 1 respectively in the
//       structure members.And compares the result with the expected 
//        value as passed in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::ldiv_good_param(  )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In ldiv(good_parameters)") );
    
    int intVal1;
    int intVal2;
    
    ldiv_t j;

	_LIT( KintVal1, "intVal1" );
	TBool res1 = GetIntFromConfig(ConfigSection(), KintVal1, intVal1);
	_LIT( KintVal2, "intVal2" );
	TBool res2 = GetIntFromConfig(ConfigSection(), KintVal2, intVal2);
	if(res1 && res2)	 
		{
        if(intVal2!=0)
			{
			
    
            j=ldiv(intVal1,intVal2);
                
			
    
            INFO_PRINTF3(_L("ldiv( %d, %d )"),intVal1, intVal2);
            INFO_PRINTF3(_L("Result->Quotient=%d Remainder=%d"),j.quot,j.rem);
                
            int result = (intVal2 * j.quot) + j.rem;

            if( result == intVal1 )
				{
                return KErrNone;
                }
			else
                {
                INFO_PRINTF1(_L("Unexpected output"));
                return KErrGeneral;
                }
            }
		else
			{
            INFO_PRINTF1(_L("This causes an exception"));
            return KErrNone;
            }
        }
	else
		{
        INFO_PRINTF1(_L("Expected input: <Dividend> <Divisor>.."));
        INFO_PRINTF1(_L("<Expected-quotient> <Expected-remainder>"));
        return KErrGeneral;
        }
        
    }
    
// -----------------------------------------------------------------------------
//Function Name :lldiv_good_param0
//API Tested :lldiv
//TestCase Description: 
//     lldiv(a,b) divides a by b and  return a structure of type lldiv_t,
//       comprising both the quotient and the remainder(all long type). 
//      This case finds the conversion of input integers, example:
//      lldiv(7,2) which should return 3 and 1 respectively in the
//       structure members.And compares the result with the expected 
//        value as passed in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::lldiv_good_param0( )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In lldiv(+ve_dividend_+ve_divisor)") );
    
	long long numer = pow(2, 40);
	long long denom = 3;
	
    
    
	lldiv_t x = lldiv(numer, denom);

    INFO_PRINTF3(_L("Result->Quotient=%ld Remainder=%ld"), x.quot, x.rem);
                
    long long exp_numer = (denom * x.quot) + x.rem;

    
    
    if( exp_numer == numer )
		{
        return KErrNone;
		}
    else
		{
        INFO_PRINTF1(_L("Unexpected output"));
        return KErrGeneral;
		}
    
	}
  
// -----------------------------------------------------------------------------
//Function Name :lldiv_good_param1
//API Tested :lldiv
//TestCase Description: 
//     lldiv(a,b) divides a by b and  return a structure of type lldiv_t,
//       comprising both the quotient and the remainder(all long type). 
//      This case finds the conversion of input integers, example:
//      lldiv(7,2) which should return 3 and 1 respectively in the
//       structure members.And compares the result with the expected 
//        value as passed in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::lldiv_good_param1( )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In lldiv(-ve_dividend_+ve_divisor)") );
    
	long long numer = pow(2, 40);
	long long denom = 3;
	
    
    
	lldiv_t x = lldiv(-numer, denom);

    INFO_PRINTF3(_L("Result->Quotient=%ld Remainder=%ld"), x.quot, x.rem);
                
    long long exp_numer = (denom * x.quot) + x.rem;

    
    
    if( exp_numer == (-numer) )
		{
        return KErrNone;
		}
    else
		{
        INFO_PRINTF1(_L("Unexpected output"));
        return KErrGeneral;
		}
    
	}  

// -----------------------------------------------------------------------------
//Function Name :lldiv_good_param2
//API Tested :lldiv
//TestCase Description: 
//     lldiv(a,b) divides a by b and  return a structure of type lldiv_t,
//       comprising both the quotient and the remainder(all long type). 
//      This case finds the conversion of input integers, example:
//      lldiv(7,2) which should return 3 and 1 respectively in the
//       structure members.And compares the result with the expected 
//        value as passed in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::lldiv_good_param2( )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In lldiv(+ve_dividend_-ve_divisor)") );
    
	long long numer = pow(2, 40);
	long long denom = -3;
	
    
    
	lldiv_t x = lldiv(numer, denom);

    INFO_PRINTF3(_L("Result->Quotient=%ld Remainder=%ld"), x.quot, x.rem);
                
    long long exp_numer = (denom * x.quot) + x.rem;

    
    
    if( exp_numer == (numer) )
		{
        return KErrNone;
		}
    else
		{
        INFO_PRINTF1(_L("Unexpected output"));
        return KErrGeneral;
		}
    
	} 

// -----------------------------------------------------------------------------
//Function Name :lldiv_good_param3
//API Tested :lldiv
//TestCase Description: 
//     lldiv(a,b) divides a by b and  return a structure of type lldiv_t,
//       comprising both the quotient and the remainder(all long type). 
//      This case finds the conversion of input integers, example:
//      lldiv(7,2) which should return 3 and 1 respectively in the
//       structure members.And compares the result with the expected 
//        value as passed in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::lldiv_good_param3( )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In lldiv(-ve_dividend_-ve_divisor)") );
    
	long long numer = pow(2, 40);
	long long denom = -3;
	
    
    
	lldiv_t x = lldiv(-numer, denom);

    INFO_PRINTF3(_L("Result->Quotient=%ld Remainder=%ld"), x.quot, x.rem);
                
    long long exp_numer = (denom * x.quot) + x.rem;

    
    
    if( exp_numer == (-numer) )
		{
        return KErrNone;
		}
    else
		{
        INFO_PRINTF1(_L("Unexpected output"));
        return KErrGeneral;
		}
    
	} 
 
// -----------------------------------------------------------------------------
//Function Name :lldiv_good_param4
//API Tested :lldiv
//TestCase Description: 
//     lldiv(a,b) divides a by b and  return a structure of type lldiv_t,
//       comprising both the quotient and the remainder(all long type). 
//      This case finds the conversion of input integers, example:
//      lldiv(7,2) which should return 3 and 1 respectively in the
//       structure members.And compares the result with the expected 
//        value as passed in the parameters.
// -----------------------------------------------------------------------------
TInt CTestStdlib::lldiv_good_param4( )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In lldiv(zero_dividend)") );
    
	long long numer = 0;
	long long denom = 3;
	INFO_PRINTF1(_L("in lldiv_good_param4L..."));
    
    
	lldiv_t x = lldiv(-numer, denom);

    INFO_PRINTF3(_L("Result->Quotient=%ld Remainder=%ld"), x.quot, x.rem);
                
    long long exp_numer = (denom * x.quot) + x.rem;

    
    
    if( exp_numer == (numer) )
		{
        return KErrNone;
		}
    else
		{
        INFO_PRINTF1(_L("Unexpected output"));
        return KErrGeneral;
		}
    
	} 
 
 
// -----------------------------------------------------------------------------
//Function Name :rand
//API Tested :rand
//TestCase Description: 
// The rand() function shall compute a sequence of pseudo-random integers 
//      in range [0,{RAND_MAX}].This case generates random numbers 20 times
//       and checks whetther there was repeatition.
// -----------------------------------------------------------------------------
TInt CTestStdlib::randL(  )
    {

    INFO_PRINTF1(_L("Tstdlib: In rand()") );

    TInt randArray[20];
    TInt i=0,j,k, flag=0;
    
    
     
    while(i<20)
		{
        randArray[i]=rand();
        INFO_PRINTF2(_L("%d "),randArray[i]);
        i++;
		}
    for(j=0;j<20;j++)
        {
        for(k=0;k<20;k++)
            {
            if((j!=k) && (randArray[j]==randArray[k]))
                {
                INFO_PRINTF1(_L("Repeatition of random no.s within 20 iterations"));
                flag=1;
                break;
                }
            }
        }
        
    
      
    if(flag==0)
		{
        INFO_PRINTF1(_L("No repeatition"));
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}

	}



// -----------------------------------------------------------------------------
//Function Name :srand
//API Tested :srand
//TestCase Description: 
// The srand() function sets the seed for the rand function to compute a 
//      sequence of pseudo-random integers in range [0,{RAND_MAX}].This 
//      case generates random numbers 20 times and checks whetther there
//       was repeatition.
// -----------------------------------------------------------------------------
TInt CTestStdlib::srandL(  )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In srand(seed_val)") );
    
    TInt retVal=KErrNone;
    int seedVal;
    int randVal;
	_LIT( KseedVal, "seedVal" );
	TBool res = GetIntFromConfig(ConfigSection(), KseedVal, seedVal);

    if(res)
		{
    
		
		
        srand(seedVal);
        randVal=rand();
        
		
       
        INFO_PRINTF2(_L("Seed Value given %d"), seedVal);
        INFO_PRINTF2(_L("Random Value returned is %d"), randVal);
            
        if(randVal<seedVal)
            {
            INFO_PRINTF1(_L("rand() returned a value less than the seed."));
            retVal = KErrGeneral;
            }
        }
    else
        {
        INFO_PRINTF1(_L("Expected as parameter: <seed_value>"));
        retVal = KErrGeneral;
        }
	return retVal;
	}

// -----------------------------------------------------------------------------
//Function Name :strtol_null_string
//API Tested :strtol
//TestCase Description: 
//    These functions shall convert the initial portion of the string 
//      pointed to by str to a type long representation.
//      This case finds the conversion of NULL string.
//      And compares result with 0,the expected value.
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtol_null_string(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtol(NULL_string)") );
    
    char *endpt;
    TInt endptr,base;
    

    _LIT( Kendptr, "endptr" );
	TBool res1 = GetIntFromConfig(ConfigSection(), Kendptr, endptr);
	_LIT( Kbase, "base" );
	TBool res2 = GetIntFromConfig(ConfigSection(), Kbase, base);
	
    long converted;

    if(endptr==0)
		{
        endpt=NULL;
		}
    
    
    
    if(res1 && res2)
		{
        converted = strtol("", &endpt,base);
        INFO_PRINTF1(_L("Whatever be the value of argument2 and argument3.."));
        INFO_PRINTF1(_L("if the first argument is :\"\","));
        INFO_PRINTF2(_L("the result always is %d"),converted);
		}
    else
		{
        converted =strtol("", (char **)NULL, 0);
        INFO_PRINTF1(_L("if the first argument is :\"\","));
        INFO_PRINTF2(_L("the result always is %d"),converted);
		}
    
    
    
    if(converted==0)
		{
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
	}

// -----------------------------------------------------------------------------
//Function Name :strtol_good_param
//API Tested :strtol
//TestCase Description: 
//    These functions shall convert the initial portion of the string 
//      pointed to by str to a type long respectively.
//      This case finds the conversion 
//      "321","-354","fghsdf","254jhjh","000073ewg","-0000377jh" "0x123"..
//          And compares result with 0,the expected value.
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtol_good_param(  )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In strtol()") );
    
    TInt endptr,base;
    TInt expected;
    TPtrC string;
    long converted=0;
    char *endpt;
    
    
	_LIT( Kstring, "string" );
	TBool res1 = GetStringFromConfig(ConfigSection(), Kstring, string);
	_LIT( Kendptr, "endptr" );
	TBool res2 = GetIntFromConfig(ConfigSection(), Kendptr, endptr);
	_LIT( Kbase, "base" );
	TBool res3 = GetIntFromConfig(ConfigSection(), Kbase, base);
	_LIT( Kexpected, "expected" );
	TBool res4 = GetIntFromConfig(ConfigSection(), Kexpected, expected);
	if(res1 && res2 && res3 && res4)
		{
        if(endptr==0)
			{
            endpt=NULL;
			}

        TBuf8<100> buf;
        buf.Copy(string);


        char* ptr = (char*) buf.Ptr();
        ptr[buf.Length()]='\0';

		
    
        converted = strtol(ptr,&endpt,base);

		    
    
        if(base<0 || base >36)
			{
            INFO_PRINTF1(_L("Base should be between 0-36 for meaningful output"));
            INFO_PRINTF2(_L(" Error No: %d"), errno);
            if(errno == 0)
				{
                INFO_PRINTF1(_L("No conversion performed"));
				}
            return KErrNone;
			}

        if(converted == LONG_MIN || converted == LONG_MAX)
			{
            INFO_PRINTF1(_L("Return value cannot be represented..."));
            INFO_PRINTF1(_L("....at the destination type"));
            INFO_PRINTF2(_L(" Error No: %d"), errno);
			}
        else if(converted == expected)
			{
            INFO_PRINTF2(_L(" Expected %i"), expected);
	        }
        else
			{
            INFO_PRINTF3(_L("Input: %S Result: %f"), &string, converted );
            INFO_PRINTF2(_L(" Expected %f"), expected);
            INFO_PRINTF1(_L("Unexpected output"));
            return KErrGeneral;
			}
		}
    else
		{
        INFO_PRINTF1(_L("Expected input-> <string> <endptr(0/1)> <base(0-36)>.."));
        INFO_PRINTF1(_L("<expected_value>"));
        INFO_PRINTF1(_L("If endptr is given as 0, the respective argument is set.."));
        INFO_PRINTF1(_L("to NULL,if any other integer specified,it is left unchanged"));
        return KErrGeneral;
		}   
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//Function Name :strtod_null_string
//API Tested :strtod
//TestCase Description: 
//    This function shall convert the initial portion of the string 
//      pointed to by str to a type double representation.
//      This case finds the conversion of NULL string.
//          And compares result with 0,the expected value.
// -----------------------------------------------------------------------------

TInt CTestStdlib::strtod_null_string(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtod(NULL_string)"));
    
    char *endpt;
    TInt endptr;
    
    _LIT( Kendptr, "endptr" );
	TBool res = GetIntFromConfig(ConfigSection(), Kendptr, endptr);
	  
    double converted;

    if(endptr==0)
		{
        endpt=NULL;
		}
    
    
    
    if(res)
		{
        converted = strtod("", &endpt);
        INFO_PRINTF1(_L("Whatever be the value of argument2 .."));
        INFO_PRINTF1(_L("..if the first argument is :\"\"..."));
        INFO_PRINTF2(_L("the result always is %d"), converted);

		}
    else
		{
        converted =strtol("", (char **)NULL, 0);
        INFO_PRINTF1(_L("..if the first argument is :\"\"..."));
        INFO_PRINTF2(_L("the result always is %d"), converted);
		}
    
    
        
    if(converted==0)
		{
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
	}

// -----------------------------------------------------------------------------
//Function Name :strtod_good_param
//API Tested :strtod
//TestCase Description: 
//    These functions shall convert the initial portion of the string 
//      pointed to by str to a type long representation.
//      This case finds the conversion "321.879","-354.87","fghsdf87.89"
//      "254.9jhjh","000073ewg","-0000377.9jh" "0x123"..
//       And compares result with 0,the expected value.
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtod_good_param(  )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In strtod()") );
    
    TInt endptr;
    TInt floatVal1,floatVal2;
    double floatVal;
    TPtrC string;
    double converted;
    char *endpt;
	
	_LIT( Kstring, "string" );
    TBool res1 = GetStringFromConfig(ConfigSection(), Kstring, string );
	_LIT( Kendptr, "endptr" );
	TBool res2 = GetIntFromConfig(ConfigSection(), Kendptr, endptr);
	_LIT( KfloatVal1, "floatVal1" );
	TBool res3 = GetIntFromConfig(ConfigSection(), KfloatVal1, floatVal1);
	_LIT( KfloatVal2, "floatVal2" );
	TBool res4 = GetIntFromConfig(ConfigSection(), KfloatVal2, floatVal2);
	
    if(res1 && res2 && res4 && res3)
		{

        int dividend = 1;
        TInt temp = floatVal2;

        do
			{
            temp /= 10;
            dividend *= 10;
			}while( temp );

        if(floatVal1>=0)
			{
            floatVal=(double) (floatVal1)+(double)((double)floatVal2/dividend);
			}
        else
			{
            floatVal=(double) (floatVal1)-(double)((double)floatVal2/dividend);
			}
        
        if(endptr==0)
			{
            endpt=NULL;
			}

        TBuf8<100> buf;
        buf.Copy(string);

        char* ptr = (char*) buf.Ptr();
        ptr[buf.Length()]='\0';

		
    
        converted = strtod(ptr,&endpt);
    
		
    
        if(converted == floatVal)
			{
            INFO_PRINTF2(_L(" Expected %f"), floatVal);
            return KErrNone;
			}
        else
			{
            INFO_PRINTF3(_L("Input: %S Result: %f"), &string, converted );
            INFO_PRINTF2(_L(" Expected %f"), floatVal);
            INFO_PRINTF1(_L("Unexpected output"));
            return KErrGeneral;
			}
		}
    else
		{
        INFO_PRINTF1(_L("Expected input-> <string> <endptr(0/1)> .."));
        INFO_PRINTF1(_L("<integral_part_expected> <decimal_part_expected>"));
        INFO_PRINTF1(_L("If endptr is  0,the respective arg is set to NULL,.."));
        INFO_PRINTF1(_L("if any other integer specified, it is left unchanged"));
        return KErrGeneral;
		}
	}

// -----------------------------------------------------------------------------
//Function Name :strtod_nan
//API Tested :strtod
//TestCase Description: NaN input to strtod
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtod_nan()
    {
    
    	
    INFO_PRINTF1(_L("In strtod(NAN)"));

	char *endpt1, *endpt2;
    //--------------------------
    double d1 = strtod("NAN", NULL);
    double d2 = strtod("nan(dffgh)rty", &endpt1);
    double d3 = strtod("-NAN123", &endpt2);

   
    INFO_PRINTF4(_L("{Expected: NaN, NaN, NaN} %f %f %f"), d1, d2, d3);

	//--------------------------

	
	
	if (!isnan(d1) || !isnan(d2)| !isnan(d3))
		{
		return KErrGeneral;
		}
	
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :strtod_inf
//API Tested :strtod
//TestCase Description: NaN input to strtod
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtod_inf()
    {
    
    	
    INFO_PRINTF1(_L("In strtod(INFINITY)"));

    //--------------------------
    char *endpt1, *endpt2;
    
    double d1 = strtod("INFINITY123", &endpt1);
    double d2 = strtod("INF", NULL);
    double d3 = strtod("-INFabc", &endpt2);
 	 	
 	INFO_PRINTF4(_L("{Expected: Inf, Inf, -Inf} %f %f %f"), d1, d2, d3);

 	
 	
 		
 	//--------------------------
 	if ( (strcmp(endpt1, "123") != 0) || (strcmp(endpt2, "abc") != 0) )
		{
 		return KErrGeneral;
		}
	
	if ((isinf(d1) != 1) || (isinf(d2) != 1) || (isinf(d3) != -1))
		{
		return KErrGeneral;
		}
	
	return KErrNone;
    }    

// -----------------------------------------------------------------------------
//Function Name :strtod_neg_case
//API Tested :strtod
//TestCase Description: NaN input to strtod
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtod_neg_cases()
    {
    
    	
    INFO_PRINTF1(_L("In strtod(neg_cases)"));

	TInt ret = KErrNone;
	
    //--------------------------
 	char *endpt1, *endpt2, *endpt3, *endpt4, *endpt5, *endpt6;
 	
 	double d1 = strtod("4752364536550er674635jkjk", &endpt1); 	
 	INFO_PRINTF2(_L("{Expected: 4752364536550.0} %f"), d1);
 	//underflow
 	errno = 0;
 	double d2 = strtod("4752364536550478678e-674635jkjk", &endpt2); 	
 	INFO_PRINTF2(_L("{Expected: 0.0} %f"), d2);
 	if (errno != ERANGE)
 		{
 		INFO_PRINTF1(_L("{case2: errno not set to ERANGE"));
 		ret = KErrGeneral;
 		}
 	
 	//overflow (-HUGE_VAL)
 	errno = 0;
 	double d3 = strtod("-1.652364536550478678e+674635jkjk", &endpt3);
 	INFO_PRINTF2(_L("{Expected: -Inf} %f"), d3);
 	if (errno != ERANGE)
	 	{
 		INFO_PRINTF1(_L("{case3: errno not set to ERANGE"));
 		ret = KErrGeneral;
 		}
 	
 	errno = 0;
 	double d4 = strtod("1.652364536550478678e.674635jkjk", &endpt4);
 	INFO_PRINTF2(_L("{Expected: 1.65236453655047867} %f"), d4);
 	
 	double d5 = strtod("1.65236453655047.8678e+674635jkjk", &endpt5);
 	INFO_PRINTF2(_L("{Expected: 1.65236453655047} %f"), d5);
 	
 	//overflow (HUGE_VAL)
 	errno = 0;
 	double d6 = strtod("1.652364536550478678e+674635jkjk", &endpt6);
 	INFO_PRINTF2(_L("{Expected: Inf} %f"), d6);

 	if (errno != ERANGE)
 		{
 		INFO_PRINTF1(_L("{case3: errno not set to ERANGE"));
 		ret = KErrGeneral;
 		}
 	
 	errno = 0;
 	//--------------------------
 	if ( (strcmp(endpt1, "er674635jkjk") != 0) || 
 	     (strcmp(endpt2, "jkjk") != 0) ||
 	     (strcmp(endpt3, "jkjk") != 0) ||
 	     (strcmp(endpt4, "e.674635jkjk") != 0) ||
 	     (strcmp(endpt5, ".8678e+674635jkjk") != 0) ||
 	     (strcmp(endpt6, "jkjk") != 0) )
 		{
 		ret = KErrGeneral;
 		}
	
	if (d1 != 4752364536550.0 || d2 != 0.000000 || d3 != -HUGE_VAL ||
	    d4 != 1.652364536550478678 || d5 != 1.65236453655047 || d6 != HUGE_VAL)
		{
		ret = KErrGeneral;
		}
	
	
	
	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :strtod_misc_case
//API Tested :strtod
//TestCase Description: NaN input to strtod
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtod_misc_cases()
    {
    
    	
    INFO_PRINTF1(_L("In strtod(misc_cases)"));

	TInt ret = KErrNone;
	
    //--------------------------
 	char *endpt2, *endpt3, *endpt4, *endpt5, *endpt6;
 	char *endpt7, *endpt8, *endpt9, *endpt10, *endpt11;
 	
 	double d1 = strtod("-9", NULL); 	
 	INFO_PRINTF2(_L("{Expected: -9.0} %f"), d1); 
             	
 	double d2 = strtod("-INfabc", &endpt2);
 	INFO_PRINTF2(_L("{Expected: -Inf} %f"), d2);
 	 	
 	double d3 = strtod("91+0e876465478299",&endpt3);
 	INFO_PRINTF2(_L("{Expected: 91.0} %f"), d3);
 	
 	double d4 = strtod("1.E788jhhg+e8767743dbn",&endpt4);
 	INFO_PRINTF2(_L("{Expected: Inf} %f"), d4);
 	 	
 	double d5 = strtod("-000e123knvbcyhdr", &endpt5);
 	INFO_PRINTF2(_L("{Expected: -0.0} %f"), d5);
 	
 	double d6 = strtod("0x00e123bhduitri", &endpt6);
 	INFO_PRINTF2(_L("{Expected: 922171.0} %f"), d6);
 	
 	double d7 = strtod("00x12e01klui", &endpt7);
    INFO_PRINTF2(_L("{Expected: 0.0} %f"), d7);
 	
    double d8 = strtod("0000000000000",&endpt8);
    INFO_PRINTF2(_L("{Expected: 0.0} %f"), d8);
 	 	
    double d9 = strtod("-0.000e123nndfuu",&endpt9);
    INFO_PRINTF2(_L("{Expected: -0.0} %f"), d9);
 	
 	double d10 = strtod("0x10p+",&endpt10);
    INFO_PRINTF2(_L("{Expected: 16.0} %f"), d10);
 	
 	double d11 = strtod("0x", NULL);
    INFO_PRINTF2(_L("{Expected: 0.0} %f"), d11);
    
    double d12 = strtod("  -23.45e-+3", &endpt11);
    INFO_PRINTF2(_L("{Expected: 0.0} %f"), d12);
    	 	
 	//--------------------------
 	if ( (strcmp(endpt2, "abc") != 0) || 
 	     (strcmp(endpt3, "+0e876465478299") != 0) ||
 	     (strcmp(endpt4, "jhhg+e8767743dbn") != 0) ||
 	     (strcmp(endpt5, "knvbcyhdr") != 0) ||
 	     (strcmp(endpt6, "hduitri") != 0) ||
 	     (strcmp(endpt7, "x12e01klui") != 0) ||
 	     (strcmp(endpt9, "nndfuu") != 0) ||
 	     (strcmp(endpt10, "p+") != 0) ||
 	     (strcmp(endpt11, "e-+3") != 0)
 	   )
 		{
 		ret = KErrGeneral;
 		}
	
	if (d1 != -9.0 || isinf(d2) != -1 || d3 != 91.0 || isinf(d4) != 1 || 
	    d5 != -0.0 || d6 != 922171.0 || d7 != 0.0 || d8 != 0.0 || d9 != -0.0 ||
	    d10 != 16.0 || d11 != 0.0 || d12 != -23.45
	   )
		{
		ret = KErrGeneral;
		}
	
	
		
	return ret;
    }
        

// -----------------------------------------------------------------------------
//Function Name :strtoul_null_string
//API Tested :strtoul
//TestCase Description: 
//    This function shall convert the initial portion of the string 
//      pointed to by str to a type unsigned long representation.
//      This case finds the conversion of NULL string.
//          And compares result with 0,the expected value.
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtoul_null_string(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtoul(NULL_string)") );
    
    char *endpt;
    TInt endptr, base;
    

    
    _LIT( Kendptr, "endptr" );
	TBool res1 = GetIntFromConfig(ConfigSection(), Kendptr, endptr);
	_LIT( Kbase, "base" );
	TBool res2 = GetIntFromConfig(ConfigSection(), Kbase, base);
	unsigned long converted;

    if(endptr==0)
		{
        endpt=NULL;
		}
    
    

    if(res1 && res2)
        {
        converted = strtoul("", &endpt,base);
        INFO_PRINTF1(_L("Whatever be the value of argument2 and argument3.."));
        INFO_PRINTF1(_L("..if the first argument is :\"\"..."));
        INFO_PRINTF2(_L("...the result always is %d"), converted);

        }
    else
        {
        converted =strtoul("", (char **)NULL, 0);
        INFO_PRINTF1(_L("..if the first argument is :\"\"..."));
        INFO_PRINTF2(_L("...the result always is %d"), converted);
        }
        
    
    
    if(converted==0)
        {
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
	}

// -----------------------------------------------------------------------------
//Function Name :strtoul_good_param
//API Tested :strtod
//    These functions shall convert the initial portion of the string 
//      pointed to by str to a type long representation.
//      This case finds the conversion "321879","-35487","fghsdf8789",
//      "2549jhjh","000073ewg","-00003779jh" "0x123cv"..
//      And compares result with the expected value.
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtoul_good_param(  )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In strtoul()") );
    
    TInt endptr,base;
    TInt expected1;
    
    TPtrC string;
    unsigned long converted;
    char *endpt;
    
    _LIT( Kstring, "string");
    TBool res1 = GetStringFromConfig(ConfigSection(), Kstring, string );
	_LIT( Kendptr, "endptr" );
	TBool res2 = GetIntFromConfig(ConfigSection(), Kendptr, endptr);
	_LIT( Kbase, "base");
	TBool res3 = GetIntFromConfig(ConfigSection(), Kbase, base);
	_LIT( Kexpected1, "expected1");
	TBool res4 = GetIntFromConfig(ConfigSection(), Kexpected1, expected1);

    TUint expected = (TUint) expected1;
    if(res1 && res2 && res3 && res4)
        {
        if(endptr==0)
            {
            endpt=NULL;
            }

        TBuf8<100> buf;
        buf.Copy(string);


        char* ptr = (char*) buf.Ptr();
        ptr[buf.Length()]='\0';

		
    
        converted = strtoul(ptr,&endpt,base);

		
        
        if(base<0 || base >36)
            {
            INFO_PRINTF1(_L("Base should be between 0-36 for meaningful output"));
            INFO_PRINTF2(_L(" Error No: %d"), errno);
            if(errno == 0)
				{
                INFO_PRINTF1(_L("No conversion performed"));
				}
            }

        if(converted == expected)
            {
            INFO_PRINTF2(_L(" Expected %d"), &expected);
            return KErrNone;
            }
        else
            {
            INFO_PRINTF2(_L(" Expected %d"), &expected);
            INFO_PRINTF1(_L("Unexpected output"));
            return KErrGeneral;
            }
        }
    else
        {
        INFO_PRINTF1(_L("Expected input-> <string> <endptr(0/1)> <base>.."));
        INFO_PRINTF1(_L("<expected_value>"));
        INFO_PRINTF1(_L("If endptr is given as 0,the respective argument is set.."));
        INFO_PRINTF1(_L("to NULL, if any other integer specified, it is left unchanged"));
        return KErrGeneral;
        }
    
    
	}

// -----------------------------------------------------------------------------
//Function Name :strtoul_neg_num_alpha
//API Tested :strtod
//TestCase Description: 
//    These functions shall convert the initial portion of the string 
//      pointed to by str to a type long representation.
//      This case finds the conversion of strings like "-35487".
//      And compares result with the expected value as in the 
//      unsigned long member of the structure.
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtoul_neg_num_alpha(  )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In strtoul(-ve_num_alpha)") );

    struct test_neg_long
        {
			char* long_string;
			int base;
			unsigned long longVal;
        };

    struct test_neg_long test[3]=
        {
            {"   -8998", 36, 4294582052u},
            {"-10hguyh",10, 4294967286u},
            {"-98ghjh",18, 4294911911u}
        };

    TInt i=0;
    for(i=0;i<3;i++)
		{
            
		
            
		unsigned long ret = strtoul(test[i].long_string,(char **)NULL,test[i].base);
            
		
            
		INFO_PRINTF3(_L("strtoul(\"%u\")-> %u"),test[i].longVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].longVal);
		if( ret != test[i].longVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :strtoul_neg_leading_zeroes
//API Tested :strtod
//TestCase Description: 
//    These functions shall convert the initial portion of the string 
//      pointed to by str to a type long representation.
//      This case finds the conversion of the strings like
//      "-0000035487","-00003779jh"
//          And compares result with the expected value as in the 
//      unsigned long member of the structure.  
// -----------------------------------------------------------------------------
TInt CTestStdlib::strtoul_neg_leading_zeroes(  )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In strtoul(-ve_leading_zeroes)") );

    struct test_neg_long
		{
			char* long_string;
            int base;
            unsigned long longVal;
        };

    struct test_neg_long test[3]=
        {
            {"   -000008998",36,4294582052u},
            {"-000010",10,4294967286u},
            {"-0000098ghjh",18,4294911911u}
        };

	TInt i=0;
	for(i=0;i<3;i++)
		{
            
		
            
		unsigned long ret = strtoul(test[i].long_string,(char **)NULL,test[i].base);
            
		
            
		INFO_PRINTF3(_L("strtoul(\"%u\")-> %u"),test[i].longVal, ret);
		INFO_PRINTF2(_L("Expected -> %f"),test[i].longVal);
		if( ret != test[i].longVal )
			{
			INFO_PRINTF1(_L("Unexpected return value from atof()"));
			return KErrGeneral;
			}
        
		}
    return KErrNone;

    }
    
int sort_function(const void* a,const void* b)
    {
    int* p1 = (int*) a;
    int val1 = *p1;
    int* p2 = (int*) b;
    int val2 = *p2;
    int x = -1;

    if(val1  > val2 )
        {
        x=1;
        }
    else if(val1 == val2 )
        {
        x=0;
        }
    return x;
    }

// -----------------------------------------------------------------------------
//Function Name :qsort_integers
//API Tested :qsort
//TestCase Description: 
//    The qsort() function shall sort an array of 'nel' objects, the initial 
//      element of which is pointed to by 'base'. The size of each object, in
//       bytes, is specified by the 'width' argument. the function 'compare' is
//       to be written by the user. all these quoted ones are arguments to qsort.
//       respectively.This case sorts a sets of integers as passed by user and 
//        manually makes a check of whether the array is sorted.
// -----------------------------------------------------------------------------
TInt CTestStdlib::qsort_integers(  )
    {
    INFO_PRINTF1(_L("Tstdlib: In quicksort()") );

	int array[100];
    int temp=0;
    int notSorted=0;
    TInt i,j;
    
    _LIT( Kelem1, "elem1" );
    _LIT( Kelem2, "elem2" );
    _LIT( Kelem3, "elem3" );
    _LIT( Kelem4, "elem4" );
    _LIT( Kelem5, "elem5" );
    _LIT( Kelem6, "elem6" );
    
    TBool res1 = GetIntFromConfig(ConfigSection(), Kelem1, array[0]);
    TBool res2 = GetIntFromConfig(ConfigSection(), Kelem2, array[1]);
    TBool res3 = GetIntFromConfig(ConfigSection(), Kelem3, array[2]);
    TBool res4 = GetIntFromConfig(ConfigSection(), Kelem4, array[3]);
    TBool res5 = GetIntFromConfig(ConfigSection(), Kelem5, array[4]);
    TBool res6 = GetIntFromConfig(ConfigSection(), Kelem6, array[5]);
    i=6;
    
    if(i!=0)
        {
        INFO_PRINTF2(_L(" i: %d "),i);

        int save=i;
        for(j=0;j<save;j++)
            {
            INFO_PRINTF2(_L(" %d "),array[j]);
            }
            
		
    
        qsort((void *)array,i,sizeof(array[0]),sort_function);

		
    
        INFO_PRINTF1(_L("Array after sort"));

        for(j=0;j<save;j++)
            {
            INFO_PRINTF2(_L(" %d "),array[j]);
            }

        temp=array[0];

        for(j=1;j<save;j++)
            {
            if(temp>array[j])
                {
                notSorted=1;
                }
            else
                {
                temp=array[j];
                }

            }
            
        if(notSorted)
			{
            return KErrGeneral;
			}

        return KErrNone;

        }
    else
        {
            INFO_PRINTF1(_L("Expected Parameters: <integer-1> <interger-2>.."));
            return KErrGeneral;
        }
    }


int sort_function1( const void *a, const void *b) 
    { 
    return( strcmp((char *)a,(char *)b) ); 
    } 

// -----------------------------------------------------------------------------
//Function Name :qsort_strings
//API Tested :qsort
//TestCase Description: 
//    The qsort() function shall sort an array of 'nel' objects, the initial 
//      element of which is pointed to by 'base'. The size of each object, in
//       bytes, is specified by the 'width' argument. the function 'compare' is
//       to be written by the user. all these quoted ones are arguments to qsort.
//       respectively.This case sorts a sets of strings as passed by user and 
//        manually makes a check of whether the array is sorted.
// -----------------------------------------------------------------------------
TInt CTestStdlib::qsort_strings(  )
    {
    INFO_PRINTF1(_L("Tstdlib: In quicksort(strings)") );
    int retVal=KErrNone;
    int notSorted=0;
    char array[10][50];
    TPtrC string[6];
    int j=0;
    int i;
    
    _LIT( Kelem1, "elem1" );
    _LIT( Kelem2, "elem2" );
    _LIT( Kelem3, "elem3" );
    _LIT( Kelem4, "elem4" );
    _LIT( Kelem5, "elem5" );
    _LIT( Kelem6, "elem6" );
    
    TBool res1 = GetStringFromConfig(ConfigSection(), Kelem1, string[0]);
    TBool res2 = GetStringFromConfig(ConfigSection(), Kelem2, string[1]);
    TBool res3 = GetStringFromConfig(ConfigSection(), Kelem3, string[2]);
    TBool res4 = GetStringFromConfig(ConfigSection(), Kelem4, string[3]);
    TBool res5 = GetStringFromConfig(ConfigSection(), Kelem5, string[4]);
    TBool res6 = GetStringFromConfig(ConfigSection(), Kelem6, string[5]);
    
    for(i=0;i<6;i++)
    {
      	TBuf8<50> buf;
        char* ptr;
        buf.Copy(string[i]);
        ptr = (char*) buf.Ptr();
        ptr[buf.Length()]='\0';

        strcpy(array[i],ptr);
        array[i][buf.Length()]=0;

        if(retVal == KErrNone)
            {
			INFO_PRINTF2(_L("%S"), &string[i]);
            }
        else
            {
            break;
            }
    }
    
    if(i!=0)
        {
        int save=i;
        
		
            
        qsort((void *)array,i,sizeof(array[0]),sort_function1);

		
    	
        INFO_PRINTF1(_L("Array after sorting"));
        for(j=0;j<save;j++)
            {
            TBuf8<50> tempString((TUint8 *)&array[j]);
            TBuf<50> displayString;
            displayString.Copy(tempString);
            INFO_PRINTF2(_L("%S"),&displayString);
            }
        
        for(j=1;j<save;j++)
            {
            if(strcmp(array[j-1],array[j])>1)
                {
                notSorted=1;
                }
            }
        if(notSorted)
            {
            return KErrGeneral;
            }
        
        return KErrNone;
        }

    else
        {
        INFO_PRINTF1(_L("Expected Param: <string-1> <string-2> <string-3>..."));
        return KErrGeneral;
        }

	}



int search_function(const void* a,const void* b)
	{
    return(*(int *)a - *(int *)b);
	}

// -----------------------------------------------------------------------------
//Function Name :binsearch_integers
//API Tested :bsearch
//TestCase Description: 
//    The bsearch() function shall search an array of 'nel' objects, the initial 
//      element of which is pointed to by 'base' for a given 'key'. The size of 
//      each object, in bytes, is specified by the 'width' argument. the function
//       'compare' is to be written by the user. all these quoted ones are arguments 
//       to qsort.respectively.This case sorts a sets of integers as passed by user and 
//        manually makes a check of whether the array has the key or not.
// -----------------------------------------------------------------------------
TInt CTestStdlib::binsearch_integers(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In binsearch()") );

    TInt i,key;
    int array[100],j;
    int* element=NULL;
    int flag=0;
    
    _LIT( Kkey, "key" );
	TBool res = GetIntFromConfig(ConfigSection(), Kkey, key);
	
    if(res)
        {
              
        _LIT( Kelem1, "elem1" );
		TBool res1 = GetIntFromConfig(ConfigSection(), Kelem1, array[0]);
		_LIT( Kelem2, "elem2" );
		TBool res2 = GetIntFromConfig(ConfigSection(), Kelem2, array[1]);
		_LIT( Kelem3, "elem3" );
		TBool res3 = GetIntFromConfig(ConfigSection(), Kelem3, array[2]);
		_LIT( Kelem4, "elem4" );
		TBool res4 = GetIntFromConfig(ConfigSection(), Kelem4, array[3]);
		_LIT( Kelem5, "elem5" );
		TBool res5 = GetIntFromConfig(ConfigSection(), Kelem5, array[4]);
		_LIT( Kelem6, "elem6" );
		TBool res6 = GetIntFromConfig(ConfigSection(), Kelem6, array[5]);
		i=6;
        if(i!=0)
            {
            int save=i;
            for(j=0;j<save;j++)
                {
                INFO_PRINTF2(_L(" %d "),array[j]);
                }

			
    
            element=(int *)bsearch(&key,(void *)array,i,sizeof(array[0]), 
                                                            search_function);
			
            
            if(element!=NULL)
				{
                INFO_PRINTF2(_L(" %d is in the array"),*element);
				}
            else
				{
				INFO_PRINTF2(_L(" Element %d not found "), key);
				}

            for(j=0;j<save;j++)
                {
                if(key==array[j]||!element)
                    {
                    flag=1;
                    }
                }

            if(flag)
                {
                return KErrNone;
                }
            INFO_PRINTF1(_L(" Unexpected behaviour"));
            return KErrGeneral;
            }
        else
            {
			INFO_PRINTF1(_L("Expected Param: <search_key> <array_ele-1>.."));
			INFO_PRINTF1(_L("<array_ele-2> <array_ele-3>...."));
			return KErrGeneral;
            }
        
        }
	else
		{
		INFO_PRINTF1(_L("Expected Parameters: <search_key> <array_ele-1>.."));
		INFO_PRINTF1(_L("<array_ele-2> <array_ele-3>...."));
		return KErrGeneral;
		}
        
	}


int search_function1(const void* a,const void* b)
	{
    return(strcmp((char *)a,(char *) b));
	}

// -----------------------------------------------------------------------------
//Function Name :binsearch_strings
//API Tested :bsearch
//TestCase Description: 
//    The bsearch() function shall search an array of 'nel' objects, the initial 
//      element of which is pointed to by 'base' for a given 'key'. The size of 
//      each object, in bytes, is specified by the 'width' argument. the function
//       'compare' is to be written by the user. all these quoted ones are arguments 
//       to qsort.respectively.This case sorts a sets of strings as passed by user and 
//        manually makes a check of whether the array has the key or not.
// -----------------------------------------------------------------------------
TInt CTestStdlib::binsearch_strings(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In binsearch(strings)") );
    int retVal=KErrNone;
    char array[10][50];
    TPtrC string[6], string_key;
    int j=0;
    int i;
    
    _LIT( Kstring_key, "string_key" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring_key, string_key);
	
    _LIT( Kelem1, "elem1");
    _LIT( Kelem2, "elem2");
    _LIT( Kelem3, "elem3");
    _LIT( Kelem4, "elem4");
    _LIT( Kelem5, "elem5");
    
    
	TBool res1 = GetStringFromConfig(ConfigSection(), Kelem1, string[0]);
    TBool res2 = GetStringFromConfig(ConfigSection(), Kelem2, string[1]);
    TBool res3 = GetStringFromConfig(ConfigSection(), Kelem3, string[2]);
    TBool res4 = GetStringFromConfig(ConfigSection(), Kelem4, string[3]);
    TBool res5 = GetStringFromConfig(ConfigSection(), Kelem5, string[4]);
    
    
    if(res)
        {
        TBuf8<50> buf_key;
        char* ptr_key;
        buf_key.Copy(string_key);
        INFO_PRINTF2(_L("%S"), &string_key);
        ptr_key = (char*) buf_key.Ptr();
        ptr_key[buf_key.Length()]='\0';
		for(i=0;i<5;i++)
			{		
			TBuf8<50> buf;
	        char* ptr;
	        buf.Copy(string[i]);
	        ptr = (char*) buf.Ptr();
	        ptr[buf.Length()]='\0';

	        strcpy(array[i],ptr);
	        array[i][buf.Length()]=0;

	        if(retVal==KErrNone)
	            {
	            INFO_PRINTF2(_L("%S"), &string);
	            }
	        else
	            {
	            break;
	            }
			}       
			
			if(i!=0)
				{
				int save=i;
            
				
            
				char* element=(char *)bsearch((void *)ptr_key,(void *)array,i,
                                           sizeof(array[0]), search_function1);
        
				
    		
				INFO_PRINTF1(_L("Search result:"));

				if(element!=NULL)
					{
					TBuf8<50> tempString((TUint8 *)&*element);
					TBuf<50> ele_key;
					ele_key.Copy(tempString);

					INFO_PRINTF2(_L("\"%S\" is in the array "),&ele_key);
					}
				else
					{
					INFO_PRINTF2(_L("Element \"%S\" not found "),&string_key);
					}


				int flag=0;
				for(j=0;j<save;j++)
					{
					if(strcmp(ptr_key,array[j])||!element)
						{
						flag=1;
						}
					}

				if(flag)
					{
					return KErrNone;
					}

				INFO_PRINTF1(_L(" Unexpected behaviour"));
				return KErrGeneral;
				}
			else
				{
				INFO_PRINTF1(_L("Expected Parameters: <search_key> <array_str-1> .."));
				INFO_PRINTF1(_L("<array_str-2> <array_str-3>...."));
				return KErrGeneral;
				}
        
			}
	else
		{
		INFO_PRINTF1(_L("Expected Parameters: <search_key> <array_str-1> .."));
		INFO_PRINTF1(_L("<array_str-2> <array_str-3>...."));
		return KErrGeneral;
		}
	}

// -----------------------------------------------------------------------------
//Function Name :isatty_Test0
//API Tested :isatty
//TestCase Description: isatty returns -> 1 (stdin, stdout, stderr)
// -----------------------------------------------------------------------------
TInt CTestStdlib::isatty_Test0()
    {
    
    	
    INFO_PRINTF1(_L("In isatty_Test0L"));

    //--------------------------
	int i = isatty(0);	
	int j = isatty(1);	
	int k = isatty(2);	
    //--------------------------
     
    INFO_PRINTF4(_L("{Expected: 1 1 1} %d %d %d"), i, j, k);

	
	
	if (i != 1 || j != 1 || k != 1)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :isatty_Test1
//API Tested :isatty
//TestCase Description: isatty returns -> 0 (data file, pipe fds)
// -----------------------------------------------------------------------------
TInt CTestStdlib::isatty_Test1()
    {
    
    	
    INFO_PRINTF1(_L("In isatty_Test1L"));

    //--------------------------
	int i = isatty(5);	
	
	int fd_file = open("c:\\some.txt", O_APPEND);
	
	int j = isatty(fd_file);	
	
	int fd_pipe[3];
	int p = pipe(fd_pipe);
	
	int k = isatty(fd_pipe[1]);	
    //--------------------------
       
    close(fd_file);          
    close(fd_pipe[0]);       
    close(fd_pipe[1]);
        
    INFO_PRINTF4(_L("{Expected: 0 0 0} %d %d %d"), i, j, k);

	unlink("c:\\some.txt");
	
	
		
	if (i != 0 || j != 0 || k != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :usleep_Test0
//API Tested :usleep
//TestCase Description: To test whether the usleep call gives the desired results
//					    for unsigned long numbers
// -----------------------------------------------------------------------------
TInt CTestStdlib::usleep_Test0(  )
    {
    
    INFO_PRINTF1(_L("In usleep_Test0L"));
    
    int input=0;
    
    _LIT( Kinput, "input" );
	TBool res = GetIntFromConfig(ConfigSection(), Kinput, input);
	
    unsigned long t = (unsigned long)input;
    
    int i = usleep(t);

	INFO_PRINTF2(_L("expected: 0 usleep returned: %d"), i);

    if(i != 0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }
    
// -----------------------------------------------------------------------------
//Function Name :usleep_Test1
//API Tested :usleep
//TestCase Description: To test whether the usleep call gives the desired results
//					    for fractional numbers
// -----------------------------------------------------------------------------
TInt CTestStdlib::usleep_Test1(  )
    {
    
    INFO_PRINTF1(_L("In usleep_Test1L"));
    
    unsigned long t = 1.23331;
    
    int i = usleep(t);

	INFO_PRINTF2(_L("expected: 0 usleep returned: %d"), i);
	
    if(i != 0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}    
    }

// -----------------------------------------------------------------------------
//Function Name :usleep_Test2
//API Tested :usleep
//TestCase Description: To test whether the usleep call gives the desired results
//					    for zero input
// -----------------------------------------------------------------------------
TInt CTestStdlib::usleep_Test2(  )
    {
    
    INFO_PRINTF1(_L("In usleep_Test2L"));
    
    int input=0;
    
    _LIT( Kinput, "input" );
	TBool res = GetIntFromConfig(ConfigSection(), Kinput, input);
	
    unsigned long t = (unsigned long)input;
    
    int i = usleep(t);

	INFO_PRINTF2(_L("expected: 0 usleep returned: %d"), i);
	
	if(i != 0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }
   // -----------------------------------------------------------------------------
//Function Name :getcwd_Test0
//API Tested :getcwd
//TestCase Description: To test whether the getcwd call gives the desired results
//					    for current working directory
// -----------------------------------------------------------------------------
TInt CTestStdlib::getcwd_Test0(  )
    {
    
    	
    INFO_PRINTF1(_L("In getcwd_Test0L"));
    
    TInt ret = KErrNone;
    
    int c = chdir("c:\\");
    
    long size=BUF_SIZE;
	char *buf = (char *)malloc((size_t)size);
	char *ptr = NULL;
	
	if (buf != NULL && c == 0)
		{
		ptr = getcwd(buf, (size_t)size);
		}
	
	if (strcasecmp(ptr, "c:") != 0)
		{
		ret = KErrGeneral;
		}
	
	free(buf);

	
	
	return ret;
    
    }

// -----------------------------------------------------------------------------
//Function Name :getcwd_Test1
//API Tested :getcwd
//TestCase Description: To test whether the getcwd call gives the desired results
//					    after changing current working directory
// -----------------------------------------------------------------------------
TInt CTestStdlib::getcwd_Test1(  )
    {
	
	
    INFO_PRINTF1(_L("In getcwd_Test1L"));
  
    TInt ret = KErrNone;
    
    TPtrC string;
    _LIT( Kstring, "string" );
    TBool res = GetStringFromConfig(ConfigSection(), Kstring, string );

    
    TBuf8<100> buf1;
    buf1.Copy(string);
    char* path = (char*) buf1.Ptr();
    path[buf1.Length()]='\0';
    
	int c = chdir(path);
	long size=BUF_SIZE;
	char *buf = (char *)malloc((size_t)size);
	char *ptr = NULL;
	
	if (buf != NULL && c == 0) 
		{
		ptr = getcwd(buf, (size_t)size);
		}
	if (strcasecmp(ptr, path) != 0)
		{
		ret = KErrGeneral;
		}

	free(buf);
		
	
		
	return ret;
    
    }

// -----------------------------------------------------------------------------
//Function Name :getcwd_Test2
//API Tested :getcwd
//TestCase Description: To test whether the getcwd call sets ERANGE
// -----------------------------------------------------------------------------
TInt CTestStdlib::getcwd_Test2(  )
    {
    
    	
    INFO_PRINTF1(_L("In getcwd_Test2L"));
    
    TInt ret = KErrNone;
    
    int c = chdir("c:\\logs\\");
    
    char *buf = (char *)malloc(2);
	char *ptr = NULL;
	
	errno = 0;
	if (buf != NULL && c == 0)
		{
		ptr = getcwd(buf, 2);
		}
	
	if (ptr != NULL || errno != ERANGE)
		{
		INFO_PRINTF2(_L("errno was set to - %d "), errno);
		ret = KErrGeneral;
		}
	
	free(buf);

	
	
	return ret;
    
    }
  

// -----------------------------------------------------------------------------
//Function Name :getcwd_Test3
//API Tested :getcwd
//TestCase Description: To test whether the getcwd call sets EINVAL
// -----------------------------------------------------------------------------
TInt CTestStdlib::getcwd_Test3(  )
    {
    
    
    INFO_PRINTF1(_L("In getcwd_Test3L"));
    
    TInt ret = KErrNone;

	long size = BUF_SIZE;
	char *buf = (char *)malloc((size_t)size);
	char *ptr = NULL;
	
	errno = 0;
	if (buf != NULL)
		{
		ptr = getcwd(buf, 0);
		}
	
	if (ptr != NULL || errno != EINVAL)
		{
		INFO_PRINTF2(_L("errno was set to - %d "), errno);
		ret = KErrGeneral;
		}
	
	free(buf);

	
	
	return ret;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name			:	tmpfile_Test0
//API Tested 			:	tmpfile
//TestCase Description	:	To test functionality of tmpfile  
// -----------------------------------------------------------------------------
TInt CTestStdlib::tmpfile_Test0()
    {    
    INFO_PRINTF1(_L("In tmpfile_Test0"));
    TInt ret = KErrNone; 
    char buf[2000];

	FILE* fp = tmpfile();
	
	if(fp)
		{
		int chars = fprintf(fp, "%s", "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello");
		if(chars != 2000)
			{
			INFO_PRINTF1(_L("error in fprintf"));
			fclose(fp);
			return KErrGeneral;
			}
			
		fflush(fp);
	
		fseek(fp, -2000, SEEK_CUR); //beg of the file
	
		fscanf(fp, "%s", buf);
		if(strlen(buf) != 2000)
			{
			INFO_PRINTF1(_L("error in fscanf"));
			fclose(fp);
			return KErrGeneral;
			}
		fflush(fp);
		
	if ( (strcmp(buf, "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello") != 0)||fp==NULL)
			{
			INFO_PRINTF1(_L("strcmp failed, hence test case failed"));	
			fclose(fp);
			return KErrGeneral;
			}
		fclose(fp);
		}
	else
		{
		INFO_PRINTF1(_L("tmpfile() returned NULL, test case failed"));
		ret = KErrGeneral;
		}
	
	return ret;
    }
      
// -----------------------------------------------------------------------------
//Function Name :tmpfile_Test1
//API Tested :getcwd
//TestCase Description: To test whether the tmpfile  creates a temp file 
// -----------------------------------------------------------------------------
TInt CTestStdlib::tmpfile_Test1(  )
    {    	
    INFO_PRINTF1(_L("In tmpfile_Test1"));
    TInt ret = KErrNone; 	
    char buf[10];
	
	FILE* fp = tmpfile();
	
	if(fp)
		{
		int bytes = fprintf(fp, "%s", "world");
		if(bytes != 5)
			{
			INFO_PRINTF1(_L("fprintf failed"));
			fclose(fp);
			return KErrGeneral;
			}
		fflush(fp);
	
		fseek(fp, 0, SEEK_SET); //beg of the file
	
		fscanf(fp, "%s", buf);
		fflush(fp);
		if(strlen(buf) != 5)
			{
			INFO_PRINTF1(_L("error in fscanf"));
			fclose(fp);
			return KErrGeneral;
			}

		if(strcmp(buf, "world"))
			{
			INFO_PRINTF1(_L("String read from file does not match the string - world"));
			ret = KErrGeneral;
			}
		fclose(fp);
		}
	else
		{
		INFO_PRINTF1(_L("tmpfile() returned NULL, test case failed"));
		ret = KErrGeneral;
		}

	return ret;
    
    }  
  
// -----------------------------------------------------------------------------
//Function Name :tmpnam_Test0
//API Tested :getcwd
//TestCase Description: To test whether the tmpname returns a temporary file name
// -----------------------------------------------------------------------------
TInt CTestStdlib::tmpnam_Test0(  )
    {
    
    	
    INFO_PRINTF1(_L("In tmpnam_Test0L"));
    
    TInt ret = KErrNone; 
	
	mkdir("c:\\system\\temp", S_IWUSR);

	char	buf[L_tmpnam];
	char    rbuf[10];
	
	char *rval = tmpnam(buf);
	
	FILE *fp = fopen(buf, "w");
	
	if (fp == NULL)
		{
		INFO_PRINTF2(_L("fopen of file returned by tmpnam() failed - errno %d "), errno);
		ret = KErrGeneral;
		}
	
	if(fp)
		{
		fprintf(fp, "%s", "check");
		fclose(fp);
		}
	
	fp = fopen(buf, "r");
	
	if(fp)
		{
		fscanf(fp, "%s", rbuf);
		fclose(fp);
		}
	
	if ( buf != rval || (strcmp(buf, rval) != 0) || (strcmp(rbuf, "check") != 0) )
		{
		ret = KErrGeneral;
		}
	unlink(buf);
	

	return ret;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :tmpnam_Test1
//API Tested :getcwd
//TestCase Description:	To test whether the tmpname returns a temporary file name
//						when the input buf is NULL.
// -----------------------------------------------------------------------------
TInt CTestStdlib::tmpnam_Test1(  )
    {
    
    	
    INFO_PRINTF1(_L("In tmpnam_Test1L"));
    
    TInt ret = KErrNone; 
	
	mkdir("c:\\system\\temp", S_IWUSR);

	char    rbuf[10];
	
	char *rval = tmpnam(NULL);
	
	FILE *fp = fopen(rval, "w");
	
	if (fp == NULL)
		{
		INFO_PRINTF2(_L("fopen of file returned by tmpnam() failed - errno %d "), errno);
		ret = KErrGeneral;
		}

	if(fp)
		{
		fprintf(fp, "%s", "check");
		fclose(fp);
		}
	
	fp = fopen(rval, "r");
	if(fp)
		{
		fscanf(fp, "%s", rbuf);
		fclose(fp);
		}
	
	INFO_PRINTF2(_L("read from file: %s"), rbuf);
		
	if ( strcmp(rbuf, "check") != 0 )
		{	
		ret = KErrGeneral;
		}
	unlink(rval);	
			

	return ret;
    
    }
    // -----------------------------------------------------------------------------
//Function Name :realpath_Test0
//API Tested :realpath
//TestCase Description: To test if realpath sets ENOENT
// -----------------------------------------------------------------------------
TInt CTestStdlib::realpath_Test0(  )
    {
    
    	
    INFO_PRINTF1(_L("In realpath_Test0L"));
    
    TInt ret = KErrNone; 
	
	char	resolvepath[PATH_MAX];
	
	FILE *fp = fopen("c:\\xyz.txt", "w");
	if(!fp)
		{
		INFO_PRINTF1(_L("fopen failed!!"));
		ret = KErrGeneral;
		}
	 
	errno = 0;
	char *rpath = realpath("\\xyz\\some.txt", resolvepath);

	if (errno != ENOENT)
		{
		INFO_PRINTF2(_L("errno was not set to ENOENT - errno %d "), errno);
		ret = KErrGeneral;
		}
	
	if ( rpath != NULL )
		{
		ret = KErrGeneral;
		}
	
	fclose(fp);
	
	

	return ret;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :realpath_Test1
//API Tested :realpath
//TestCase Description: To test if realpath resolves '. and '..'
// -----------------------------------------------------------------------------
TInt CTestStdlib::realpath_Test1(  )
    {
    
    	
    INFO_PRINTF1(_L("In realpath_Test1L"));
    
    TInt ret = KErrNone; 
	
	char	resolvepath[PATH_MAX];
	
	FILE *fp = fopen("c:\\xyz.txt", "w");
	if(!fp)
		{
		INFO_PRINTF1(_L("fopen failed!!"));
		ret = KErrGeneral;
		}
	 
	mkdir("c:\\tmdir", S_IWUSR);
	
	int c = chdir("c:\\");
	if(c == -1)
		{
		INFO_PRINTF1(_L("chdir failed!!"));
		ret = KErrGeneral;
		}
	
	//------------
	char *rpath = realpath("c:\\tmdir\\..\\xyz.txt", resolvepath);

	if ( rpath == NULL || (strcasecmp(rpath, "c:\\xyz.txt") != 0) )
		{
		ret = KErrGeneral;
		}
	
	//------------
	rpath = realpath(".\\tmdir\\..\\xyz.txt", resolvepath);

	if ( rpath == NULL || (strcasecmp(rpath, "c:\\xyz.txt") != 0) )
		{
		ret = KErrGeneral;
		}
	
	//------------
	rpath = realpath("xyz.txt", resolvepath);

	if ( rpath == NULL || (strcasecmp(rpath, "c:\\xyz.txt") != 0) )
		{
		ret = KErrGeneral;
		}
	
	fclose(fp);
	
	rmdir("c:\\tmdir");
	unlink("c:\\xyz.txt");

	
	
	return ret;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :realpath_Test2
//API Tested :realpath
//TestCase Description:To test if realpath resolves symlinks
// -----------------------------------------------------------------------------  
TInt CTestStdlib::realpath_Test2(  )
    {
    
    	
    INFO_PRINTF1(_L("In realpath_Test2L"));
    
    TInt ret = KErrNone; 
	
	char	resolvepath[PATH_MAX];
	
	mkdir("c:\\tdir", S_IWUSR);
	
	FILE *fp = fopen("c:\\tdir\\xyz.txt", "w");
	if(!fp)
		{
		INFO_PRINTF1(_L("fopen failed!!"));
		ret = KErrGeneral;
		}

	fclose(fp);
	
	unlink("c:\\linkname.txt");
		 
	int isymlink = symlink("c:\\tdir\\xyz.txt", "c:\\linkname.txt");
	if (isymlink == -1)
		{
		INFO_PRINTF1(_L("symlink failed!!"));
		ret = KErrGeneral;
		}
	
	char *rpath = realpath("c:\\linkname.txt", resolvepath);

	if ( rpath == NULL || (strcasecmp(rpath, "c:\\tdir\\xyz.txt") != 0) )
		{
		ret = KErrGeneral;
		}
	
	int c = chdir("c:\\");
	if (c == -1)
		{
		INFO_PRINTF1(_L("chdir failed!!"));
		ret = KErrGeneral;
		}
	
	rpath = realpath("linkname.txt", resolvepath);

	if ( rpath == NULL || (strcasecmp(rpath, "c:\\tdir\\xyz.txt") != 0) )
		{
		ret = KErrGeneral;
		}
	
	unlink("c:\\tdir\\xyz.txt");
	rmdir("c:\\tdir");

	
	
	return ret;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :realpath_Test3
//API Tested :realpath
//TestCase Description: To test if realpaths sets EINVAL
// -----------------------------------------------------------------------------
TInt CTestStdlib::realpath_Test3(  )
    {
    
    	
    INFO_PRINTF1(_L("In realpath_Test3L"));
    
    TInt ret = KErrNone; 

	errno = 0;
	char *rpath1 = realpath("c:\\", NULL);

	if ( rpath1 != NULL || errno != EINVAL )
		{
		INFO_PRINTF2(_L("errno was set to - %d "), errno);
		ret = KErrGeneral;
		}
	
	char resolvepath[PATH_MAX];
	
	char *rpath2 = realpath(NULL, resolvepath);

	if ( rpath2 != NULL || errno != EINVAL )
		{
		INFO_PRINTF2(_L("errno was set to - %d "), errno);
		ret = KErrGeneral;
		}

	
	
	return ret;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :realpath_Test4
//API Tested :realpath
//TestCase Description: To test if realpaths sets EACCES
// -----------------------------------------------------------------------------  
TInt CTestStdlib::realpath_Test4(  )
    {
    
    	
    INFO_PRINTF1(_L("In realpath_Test4L"));
    
    TInt ret = KErrNone; 
	
	char	resolvepath[PATH_MAX];
	
	mkdir("c:\\nodir", S_IWUSR);
	
	FILE *fp = fopen("c:\\nodir\\xyz.txt", "w");
	if(!fp)
		{
		INFO_PRINTF1(_L("fopen failed!!"));
		ret = KErrGeneral;
		}

	fclose(fp);
	
	int ichmod = chmod("c:\\nodir", S_IRUSR);
	if(ichmod == -1)
		{
		INFO_PRINTF1(_L("chmod failed!!"));
		ret = KErrGeneral;
		}
	
	char *rpath = realpath("..\\..\\nodir\\xyz.txt", resolvepath);

	chmod("c:\\nodir", S_IWUSR);
	unlink("c:\\nodir\\xyz.txt");
	rmdir("c:\\nodir");
		
	if ( rpath != NULL || errno != EACCES )
		{
		INFO_PRINTF2(_L("errno was set to - %d "), errno);
		ret = KErrGeneral;
		}
	
	
	
	return ret;
    
 }
   
   
//-------------------------------------------------------------------------------
//Function Name		: CTestStdlib::perror_Test0L
//API Tested			: perror
//TestCase Description: The test if perror prints the error messages
//-------------------------------------------------------------------------------

  
TInt CTestStdlib::perror_Test0(  )
    {
    
    	
    INFO_PRINTF1(_L("In perror_Test0L"));
    
    TInt ret = KErrNone; 
    
    chmod("c:\\err.txt", S_IWUSR);
	
	unlink("c:\\err.txt");
	
	int fd = open("c:\\err.txt", O_RDWR | O_CREAT, 0666);
	if(fd == -1)
		{
		INFO_PRINTF1(_L("open failed!!"));
		return KErrGeneral;
		}
	
	close(2); //closing stderr
	int newfd = dup(fd);
	
	errno = ENOENT;
	perror("checking ENOENT");
	
	errno = ENOMEM;
	perror("checking ENOMEM");
	
	int ic1 = close(fd);
	int ic2 = close(newfd);
	if(ic1 == -1 || ic2 == -1)
		{
		INFO_PRINTF1(_L("close failed!!"));
		return KErrGeneral;
		}	
	
	FILE *fp = fopen("c:\\err.txt", "r");
	if(!fp)
		{
		INFO_PRINTF1(_L("fopen failed!!"));
		return KErrGeneral;
		}
	char buf[50];
	
	int iread = fread(buf, 1, 42, fp);
	buf[42] = '\0';
	INFO_PRINTF2(_L("read from file: %s"), buf);
	INFO_PRINTF2(_L("read count: %d"), iread);
	
	if( strcmp(buf, "checking ENOENT: No such file or directory") != 0)
		{
		ret = KErrGeneral;
		}
	
	fgetc(fp); //skip a char
	iread = fread(buf, 1, 39, fp);
	buf[39] = '\0';
	INFO_PRINTF2(_L("read from file: %s"), buf);
	INFO_PRINTF2(_L("read count: %d"), iread);
	
	if( strcmp(buf, "checking ENOMEM: Cannot allocate memory") != 0)
		{
		ret = KErrGeneral;
		}
	
	fclose(fp);
	
	unlink("c:\\err.txt");
	
	
	return ret;
    
    }  
      
TInt CTestStdlib::Testlseek()
	{
	INFO_PRINTF1(_L("Open file for reading!"));
	int fd = open("c:\\data.txt", O_RDONLY|O_CREAT);
	if (fd<0)
		{
		INFO_PRINTF1(_L("fopen failed!!"));
		INFO_PRINTF2(_L("The errno set is %d\n"),errno);
		return KErrGeneral;	
		}
	char buf1[1024];
	TInt ret;
	INFO_PRINTF1(_L("Read the file!"));
	for (int i = 0; i < 1024; ++i) 
		{
		ret =  lseek(fd , 0 ,SEEK_SET) ; //Seek to beginning of file
		int nCnt = read(fd, buf1,1024 );
	
		ret =  lseek(fd , 1024 ,SEEK_CUR) ;
		nCnt = read( fd, buf1, 1024);
	
		ret =  lseek(fd , 0 ,SEEK_SET) ;
		nCnt = read( fd, buf1, 512);
		
		}
	INFO_PRINTF1(_L("read successful!"));
	close(fd);
	unlink("c:\\data.txt");
	return ret;
	}

// -----------------------------------------------------------------------------
//Function Name 		:	mkstemp_Test0
//API Tested 			:	mkstemp
//TestCase Description	: 	To test whether the tmpfile  creates a temp file
// -----------------------------------------------------------------------------
TInt CTestStdlib::mkstemp_Test0(  )
    {
    INFO_PRINTF1(_L("In mkstemp_Test0"));
    TInt ret = KErrNone; 
	char arr[] = "c:\\someXXXXXX";
	char buf[10];
	
	int fd = mkstemp(arr);
	
	if(fd != -1)
		{
		int bytes  = write(fd, "hello", 5);
		if(bytes != 5)
			{
			INFO_PRINTF1(_L("write failed"));
			close(fd);
			return KErrGeneral;
			}
	
		lseek(fd, 0, SEEK_SET); //beg of the file
	
		bytes = read(fd, buf, 5);
		if(bytes != 5)
			{
			INFO_PRINTF1(_L("read failed"));
			close(fd);
			return KErrGeneral;
			}
			
		buf[5] = '\0';
		
		if ((strcmp(buf, "hello") != 0) || fd == -1)
			{
			INFO_PRINTF1(_L("The string read from temp file is not hello"));
			ret = KErrGeneral;
			}
		
		close(fd);
		unlink(arr);
		}

	else
		{
		INFO_PRINTF1(_L("mkstemp returned invalid fd, test case failed"));
		ret = KErrGeneral;
		}
	
	return ret;
    }

// -----------------------------------------------------------------------------
//Function Name 		:	mkstemp_Test1
//API Tested 			:	mkstemp
//TestCase Description	: 	To test that tmpfile  fails to create a temp file when 
//						the template is incorrect
// -----------------------------------------------------------------------------
TInt CTestStdlib::mkstemp_Test1(  )
    {
    INFO_PRINTF1(_L("In mkstemp_Test1"));
    TInt ret = KErrNone; 
	char arr[] = "c:\\someXXXX";
	errno = 0;
	
	int fd = mkstemp(arr);
	
	if(fd != -1 || errno != EINVAL)
		{
		INFO_PRINTF2(_L("errno was set to - %d "), errno);
		ret = KErrGeneral;
		}

	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :confstr_Test0
//API Tested :confstr
//TestCase Description:confstr returns configuration dependant string variable
//_CS_PATH
//input --> void
// -----------------------------------------------------------------------------
TInt CTestStdlib::confstr_Test0()
    {
    
    	
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In confstr_Test0L"));
    
    int n = confstr(_CS_PATH, NULL, 0);
    
  	INFO_PRINTF2(_L("{Expected: 0} %d"), n);
       
    if( n == 0 )
		{
    	ret = KErrGeneral;
		}
                     
    char *buf = (char *)malloc(n);
   
    if ( buf == NULL )
		{
		INFO_PRINTF1(_L("malloc failed!!"));
		return KErrGeneral;
		}
	             
    int len = confstr(_CS_PATH, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	
	free(buf);
    
    
    
	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :confstr_Test1
//API Tested :confstr
//TestCase Description:confstr returns 0 and sets errno to EINVAL
//input --> void
// -----------------------------------------------------------------------------
TInt CTestStdlib::confstr_Test1()
    {
    
    	
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In confstr_Test1L"));
    
    #define _CS_INV 15
    
    int n = 10;
                         
    char *buf = (char *)malloc(n);
   
    if ( buf == NULL )
		{
		INFO_PRINTF1(_L("malloc failed!!"));
		return KErrGeneral;
		}
	
	errno = 0;
	
    int len = confstr(_CS_INV, buf, n);
    
    if (len != 0 || errno != EINVAL)
		{
		INFO_PRINTF2(_L("errno was set to - %d"), errno);
		ret = KErrGeneral;
		}
		
	free(buf);
		
	
		
  	return ret;
    }   
  
  // -----------------------------------------------------------------------------
//Function Name :fpathconf_Test0
//API Tested :fpathconf
//TestCase Description:fpathconf returns configuration option name
//input --> _PC_XXXX_XXXX constants
// -----------------------------------------------------------------------------
TInt CTestStdlib::fpathconf_Test0()
    {
    
    	
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In fpathconf_Test0L"));
    
    int dummy_fd = 100;
 
    int n = fpathconf(dummy_fd, _PC_LINK_MAX);
    
  	INFO_PRINTF2(_L("{Expected: (> _POSIX_LINK_MAX) } %d"), n);
       
    if( n < _POSIX_LINK_MAX )
		{
    	ret = KErrGeneral;
		}
                     
    n = fpathconf(dummy_fd, _PC_NAME_MAX);
    
  	INFO_PRINTF2(_L("{Expected: (> _POSIX_NAME_MAX) } %d"), n);
       
    if( n < _POSIX_NAME_MAX )
		{
    	ret = KErrGeneral;
		}
    
    n = fpathconf(dummy_fd, _PC_PATH_MAX);
    
  	INFO_PRINTF2(_L("{Expected: (> _POSIX_PATH_MAX) } %d"), n);
       
    if( n < _POSIX_PATH_MAX )
    	{
    	ret = KErrGeneral;
		}
    
    n = fpathconf(dummy_fd, _PC_PIPE_BUF);
    
  	INFO_PRINTF2(_L("{Expected: (> _POSIX_PIPE_BUF) } %d"), n);
       
    if( n < _POSIX_PIPE_BUF )
    	{
    	ret = KErrGeneral;
		}
    
    
    
	return ret;
    }
    
    
// -----------------------------------------------------------------------------
//Function Name :fpathconf_Test1
//API Tested :fpathconf
//TestCase Description:fpathconf sets EINVAL when invalid constant is the input
//input --> _PC_XXXX_XXXX constants
// -----------------------------------------------------------------------------
TInt CTestStdlib::fpathconf_Test1()
    {
    
    	
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In fpathconf_Test1L"));
    
    int dummy_fd = 100;
        
    #define _PC_INV_CONST 100
    
    errno = 0;
    
    int n = fpathconf(dummy_fd, _PC_INV_CONST);
    
  	INFO_PRINTF2(_L("{Expected: -1 } %d"), n);
       
    if( n != -1 || errno != EINVAL)
		{
    	INFO_PRINTF2(_L("errno was set to - %d"), errno);
    	ret = KErrGeneral;
		}

    
    
	return ret;
    }
    
    
// -----------------------------------------------------------------------------
//Function Name :somefun
//API Tested :filelocks -it is an internal function to test file locks
//input --> filehandle
// -----------------------------------------------------------------------------       
void* somefun(void* args)
	{

	FILE *fp = (FILE *)args;

	flockfile(fp);

	fputc('a', fp);

	fputc('b', fp);

	fputc('c', fp);

	funlockfile(fp);

	return((void *)NULL);
	}
	
// -----------------------------------------------------------------------------
//Function Name :filelock_Test0
//API Tested :flockfile, funlockfile
//TestCase Description:confstr returns 0 and sets errno to EINVAL
//input --> void
// -----------------------------------------------------------------------------
TInt CTestStdlib::filelock_Test0()
    {
    pthread_self();
    
    
    
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In filelock_Test0L"));
    
    pthread_t obj;
    char buf[10];

	FILE *fp = fopen("c:\\chk.txt", "w");

	if(fp)
		{
        flockfile(fp);
		INFO_PRINTF1(_L("aquired lock in thread 1!"));
		
        fputc('x', fp);
	    INFO_PRINTF1(_L("after writing 'x' from thread 1"));

        int ichk = pthread_create(&obj, NULL, somefun, fp);        
        if(ichk != 0)
			{
        	INFO_PRINTF1(_L("thread creation failure!"));	        	
        	return KErrGeneral;
			}
        
        INFO_PRINTF1(_L("after calling thread 2 from thread 1"));

        fputc('y', fp);
        INFO_PRINTF1(_L("after writing 'y' from thread 1"));
        fputc('z', fp);
        INFO_PRINTF1(_L("after writing 'z' from thread 1"));

        funlockfile(fp);
        INFO_PRINTF1(_L("gave up the lock in thread 1"));
        
        sleep(2);
        fclose(fp);
		}
		
	fp = fopen("c:\\chk.txt", "r");
    
    if(fp)           
    	{
    	fscanf(fp, "%s", buf);
    	fclose(fp);
    	}
	
	INFO_PRINTF2(_L("buf read: %s"), buf);

	if ( (strcmp(buf, "xyzabc") != 0) )
		{
    	ret = KErrGeneral;
		}
	
	unlink("c:\\chk.txt");
  		
  	pthread_join(obj, NULL);
  	
	
	
	return ret;
	
    }
    
// -----------------------------------------------------------------------------
//Function Name :filelock_Test1
//API Tested :flockfile, funlockfile
//TestCase Description:confstr returns 0 and sets errno to EINVAL
//input --> void
// -----------------------------------------------------------------------------
TInt CTestStdlib::filelock_Test1()
    {
    pthread_self();
    
    
    
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In filelock_Test1L"));
    
    pthread_t obj;
    char buf[10];

	FILE *fp = fopen("c:\\chk.txt", "w");

	if(fp)
		{
        flockfile(fp);
		INFO_PRINTF1(_L("1 - aquired lock in thread 1!"));
		
        fputc('x', fp);
        INFO_PRINTF1(_L("after writing 'x' from thread 1"));

        int ichk = pthread_create(&obj, NULL, somefun, fp);
        if(ichk != 0)
			{
        	INFO_PRINTF1(_L("thread creation failure!"));	        	
        	return KErrGeneral;
			}
        
        INFO_PRINTF1(_L("after calling thread 2 from thread 1"));

		funlockfile(fp);
		INFO_PRINTF1(_L("1 - gave up the lock in thread 1"));
		
		sleep(1);
		
		flockfile(fp);
		INFO_PRINTF1(_L("2 - aquired lock in thread 1!"));
		
        fputc('y', fp);
        INFO_PRINTF1(_L("after writing 'y' from thread 1"));
        fputc('z', fp);
        INFO_PRINTF1(_L("after writing 'z' from thread 1"));

        funlockfile(fp);
        INFO_PRINTF1(_L("2 - gave up the lock in thread 1"));
        
        fclose(fp);
		}
		
	fp = fopen("c:\\chk.txt", "r");
    
    if(fp)           
    	{
    	fscanf(fp, "%s", buf);
    	fclose(fp);
    	}
	
	INFO_PRINTF2(_L("buf read: %s"), buf);

	if ( (strcmp(buf, "xabcyz") != 0) )
		{
    	ret = KErrGeneral;
		}
	
	unlink("c:\\chk.txt");
	    
	pthread_join(obj, NULL);
	  	
	
	    
  	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :filelock_Test2
//API Tested :ftrylockfile, flockfile, funlockfile
//TestCase Description:confstr returns 0 and sets errno to EINVAL
//input --> void
// -----------------------------------------------------------------------------
TInt CTestStdlib::filelock_Test2()
    {
    pthread_self();
    
    
    
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In filelock_Test2L"));
    
    pthread_t obj;
    char buf[10];
    int retVal=KErrNone;

	FILE *fp = fopen("c:\\chk.txt", "w");

	if(fp)
		{
		retVal = ftrylockfile(fp);
		if (retVal == 0)
			{
			INFO_PRINTF1(_L("aquired lock in thread 1!"));
		
	        fputc('x', fp);
	        INFO_PRINTF1(_L("after writing 'x' from thread 1"));

	        fputc('y', fp);
	        INFO_PRINTF1(_L("after writing 'y' from thread 1"));
	        
	        int ichk = pthread_create(&obj, NULL, somefun, fp);
	        if(ichk != 0)
        		{
        		INFO_PRINTF1(_L("thread creation failure!"));	        	
        		return KErrGeneral;
        		}
        
	        INFO_PRINTF1(_L("after calling thread 2 from thread 1"));
	        
	        fputc('z', fp);
	        INFO_PRINTF1(_L("after writing 'z' from thread 1"));
	        
	        funlockfile(fp);
	        INFO_PRINTF1(_L("gave up the lock in thread 1"));	       
			}
		else
			{
			INFO_PRINTF1(_L("couldn't acquire the lock in thread 1"));
			}
		
		sleep(2);
		fclose(fp);
		}
		
	fp = fopen("c:\\chk.txt", "r");
    
    if(fp)           
    	{
    	fscanf(fp, "%s", buf);
    	fclose(fp);
    	}
	
	INFO_PRINTF2(_L("buf read: %s"), buf);

	if ( (strcmp(buf, "xyzabc") != 0) || retVal != 0)
		{
    	ret = KErrGeneral;
		}	
	
	unlink("c:\\chk.txt");
	
	pthread_join(obj, NULL);
	
	
	
  	return ret;
    }
          
// -----------------------------------------------------------------------------
//Function Name :somefun
//API Tested :filelocks -it is an internal function to test file locks
//input --> filehandle
// -----------------------------------------------------------------------------       
void* sometryfun(void* args)
	{

	FILE *fp = (FILE *)args;

	flockfile(fp);
	fputc('a', fp);
	fputc('b', fp);
	fputc('c', fp);

	sleep(5);
	
	funlockfile(fp);
	return ((void*)NULL);
	}
	  
// -----------------------------------------------------------------------------
//Function Name :filelock_Test3
//API Tested :ftrylockfile, flockfile, funlockfile
//TestCase Description:confstr returns 0 and sets errno to EINVAL
//input --> void
// -----------------------------------------------------------------------------
TInt CTestStdlib::filelock_Test3()
    {
   	pthread_self();
   	
   	
   	
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In filelock_Test3L"));
    
    pthread_t obj;
    char buf[10];
    int retVal=KErrNone;

	FILE *fp = fopen("c:\\chk1.txt", "w");

	if(fp)
		{
        int ichk = pthread_create(&obj, NULL, sometryfun, fp);
        if(ichk != 0)
			{
        	INFO_PRINTF1(_L("thread creation failure!"));	        	
        	return KErrGeneral;
			}
                
        INFO_PRINTF1(_L("after calling thread 2 from thread 1"));
        
        sleep(1);
        
        retVal = ftrylockfile(fp);
		if (retVal == 0)
			{
			INFO_PRINTF1(_L("aquired lock in thread 1!"));
		
	        fputc('x', fp);
	        INFO_PRINTF1(_L("after writing 'x' from thread 1"));

	        fputc('y', fp);
	        INFO_PRINTF1(_L("after writing 'y' from thread 1"));
	        fputc('z', fp);
	        INFO_PRINTF1(_L("after writing 'z' from thread 1"));

	        funlockfile(fp);
	        INFO_PRINTF1(_L("gave up the lock in thread 1"));
			}
		else
			{
			INFO_PRINTF1(_L("couldn't acquire the lock in thread 1"));
			}
		
		sleep(5);
		
        fclose(fp);
		}
		
	fp = fopen("c:\\chk1.txt", "r");
    
    if(fp)           
    	{
    	fscanf(fp, "%s", buf);
    	fclose(fp);
    	}
	
	INFO_PRINTF2(_L("buf read: %s"), buf);

	if ( (strcmp(buf, "abc") != 0) || retVal == 0 )
		{
    	ret = KErrGeneral;
		}
	
	unlink("c:\\chk1.txt");
	
	pthread_join(obj, NULL);
	
	
	
  	return ret;
    }      
  
  // -----------------------------------------------------------------------------
//Function Name :getopt1
//API Tested :getopt
//TestCase Description:To test getopt  for all the options in optstr
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest1(  )
    {       
    
    
    optind = 1;
    opterr = 1;
    
    int argc = 6;
      
    char *argv[] = {"getopt","-a","hi","-b","-c","hello" }; 
      
    TInt ret = KErrNone; 
      
    char *buf = ( char* ) malloc ( 20 );
      
    char *buf1 = buf;
          
    int ch;
     
    INFO_PRINTF1(_L ("options expected  option 'a', option 'b', option 'c' \n "));
          
    ch = getopt ( argc, argv, "a:bc");
     
    while ( ch != -1 )
		{
     	
     	switch ( ch )
     		{
     		case 'a': 
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'a' \n "));
     			break;
     	
     		case 'b':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'b' \n "));
     			break;
     		
     		case 'c':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'c' \n "));
     			break;
     		
     		case '?':
     		
     			sprintf(buf1++,"%c",ch);
     	    	INFO_PRINTF1(_L ("an option not in optstring or missing arg..\n "));
     			break;  
     			
     		case ':':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("missing argument or the 1st charcter of optstring is : \n "));
     			break;   			
     		}//end switch
     	
     		ch = getopt ( argc, argv, "a:bc");
     	
		} //end while
     
	buf1 = '\0';
     
    if( ( strcmp( buf, "abc" )) != 0 )
		{
        ret = KErrGeneral;
		}
    
    INFO_PRINTF1(_L ("non option ARGV elements expected: argv[5] == hello \n "));
	
	if ( optind < argc ) 
		{
        	
		INFO_PRINTF1(_L ("non option ARGV elements actual:\n "));
        	
		while ( optind < argc )	
        	{
        	INFO_PRINTF2(_L ("argv[%d]=="),optind);  
       		optind++;
			}
		}
        
	if( ( strcmp ( argv[5], "hello")) != 0 )  
		{
    	ret = KErrGeneral;
		}
     free( buf );
     
     
     
     return ret; 
      
    }
    
      
// -----------------------------------------------------------------------------
//Function Name :getopt2
//API Tested :getopt
//TestCase Description:To test getopt for missing argument 
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest2(  )
    {
    
    
    optind = 1;
    opterr = 1;
    int argc = 2;
      
	char *argv[] = { "getopt", "-a" }; //optstring  "a:b", returns ? ...shell returns ( option requires an argument )
    
    TInt ret = KErrNone; 
      
    char *buf = ( char* ) malloc ( 20 );
      
    char *buf1 = buf;
                 
    int ch;
     
    INFO_PRINTF1(_L (" expected output : an option not in optstring or missing arg.. \n "));
     
    ch = getopt ( argc, argv, "a:bc");
     
    while ( ch != -1 )
		{
     	
     	switch ( ch )
     		{
     		case 'a': 
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'a' \n "));
     			break;
     	
     		case 'b':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'b' \n "));
     			break;
     		
     		case 'c':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'c' \n "));
     			break;
     		
     		case '?':
     		
     			sprintf(buf1++,"%c",ch);
     	    	INFO_PRINTF1(_L (" an option not in optstring or missing arg..\n "));
     			break;  
     			
     		case ':':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("missing argument or the 1st charcter of optstring is : \n "));
     			break;   			
     		}//end switch
     	
     		ch = getopt ( argc, argv, "a:bc");
     	
		} //end while
     
     buf1 = '\0';
     
     if( ( strcmp( buf, "?" )) != 0 )
		{
        ret = KErrGeneral;
		}
	if ( optind < argc ) 
        {
        INFO_PRINTF1(_L ("non option ARGV elements actual:\n "));
        while ( optind < argc )	
        	{
        	INFO_PRINTF2(_L ("argv[%d]=="),optind);  
			}
        }
	free( buf );
	
	
	
    return ret; 

    }

           
      
      
// -----------------------------------------------------------------------------
//Function Name :getopt3
//API Tested :getopt
//TestCase Description:To test getopt for an option which is not there in optstr 
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest3(  )
    {       
    
    
    optind = 1;
    opterr = 1;
    int argc = 2;
      
	char *argv[] = { "getopt", "-g" }; //optstring  "a:b", returns ? ...shell returns ( option requires an argument )
    
    INFO_PRINTF1(_L (" expected output : an option not in optstring or missing arg.. \n "));
            
    TInt ret = KErrNone; 
      
    char *buf = ( char* ) malloc ( 20 );
      
    char *buf1 = buf;
        
    int ch;
               
    ch = getopt ( argc, argv, "a:bc");
     
    while ( ch != -1 )
		{
     	
     	switch ( ch )
     		{
     		case 'a': 
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'a' \n "));
     			break;
     	
     		case 'b':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'b' \n "));
     			break;
     		
     		case 'c':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'c' \n "));
     			break;
     		
     		case '?':
     		
     			sprintf(buf1++,"%c",ch);
     	    	INFO_PRINTF1(_L (" an option not in optstring or missing arg..\n "));
     			break;  
     			
     		case ':':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("missing argument or the 1st charcter of optstring is : \n "));
     			break;   			
     		}//end switch
     	
     		ch = getopt ( argc, argv, "a:bc");
     	
		} //end while
     
     buf1 = '\0';
     
     if( ( strcmp( buf, "?" )) != 0 )
		{
        ret = KErrGeneral;
		}

	 if ( optind < argc ) 
        {
        INFO_PRINTF1(_L ("non option ARGV elements actual:\n "));
        while ( optind < argc )	
        	{
        	INFO_PRINTF2(_L ("argv[%d]=="),optind);  
        	}
        }
 
     free ( buf );
     
     
     
     return ret; 
      
   }
        
          
// -----------------------------------------------------------------------------
//Function Name :getopt4
//API Tested :getopt
//TestCase Description:To test getopt if the options are after the nonoption argument 
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest4(  )
    {       

    optind = 1;
    opterr = 1;
    int argc = 7;
      
     char *argv[] = {"getopt","-a","hi","-b","hello","-c","nokia" }; 
          
    int ch;
     
    TInt ret = KErrNone; 
      
    char *buf = ( char* ) malloc ( 20 );
      
    char *buf1 = buf;
          
	INFO_PRINTF1(_L ("options expected  option 'a', option 'b'\n "));
          
    ch = getopt ( argc, argv, "a:bc");
     
    while ( ch != -1 )
		{
     	
     	switch ( ch )
     		{
     		case 'a': 
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'a' \n "));
     			break;
     	
     		case 'b':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'b' \n "));
     			break;
     		
     		case 'c':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'c' \n "));
     			break;
     		
     		case '?':
     		
     			sprintf(buf1++,"%c",ch);
     	    	INFO_PRINTF1(_L ("an option not in optstring or missing arg..\n "));
     			break;  
     			
     		case ':':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("missing argument or the 1st charcter of optstring is : \n "));
     			break;   			
     		}//end switch
     	
     		ch = getopt ( argc, argv, "a:bc");
     	
		} //end while
     
     buf1 = '\0';
     
     if( ( strcmp( buf, "ab" )) != 0 )
		{
        ret = KErrGeneral;
		}
	INFO_PRINTF1(_L ("non option ARGV elements expected: argv[4] == hello,argv[5]==-c,argv[6] ==nokia \n "));
    
	if ( optind < argc ) 
        {
        INFO_PRINTF1(_L ("non option ARGV elements actual:\n "));
        while ( optind < argc )	
        	{
        	INFO_PRINTF2(_L ("argv[%d]=="),optind);  
        	optind++;
        	}
		}
 
    if( (strcmp ( argv[4], "hello") ) || ( strcmp( argv[5],"-c")) || ( strcmp( argv[6], "nokia")) )  
		{
		ret = KErrGeneral;
		}
    free(buf);
    return ret; 
    }    
    
    
// -----------------------------------------------------------------------------
//Function Name :getopt5
//API Tested :getopt
//TestCase Description:To test getopt for missing argument and(&&)optstring contains 1st character as :
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest5(  )
    {       
    
    
    optind = 1;
    opterr = 1;
    int argc = 2;
      
	char *argv[] = { "getopt", "-a" }; //optstring  "a:b", returns ? ...shell returns ( option requires an argument )
    
	int ch;
     
    TInt ret = KErrNone; 
      
    char *buf = ( char* ) malloc ( 20 );
      
    char *buf1 = buf;
    
	ch = getopt ( argc, argv, ":a:bc");
     
    while ( ch != -1 )
		{
     	
     	switch ( ch )
     		{
     		case 'a':
     		
     			sprintf(buf1++,"%c",ch); 
     			INFO_PRINTF1(_L ("option 'a' \n "));
     			break;
     	
     		case 'b':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'b' \n "));
     			break;
     		
     		case 'c':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'c' \n "));
     			break;
     		
     		case '?':
     		
     			sprintf(buf1++,"%c",ch);
     	    	INFO_PRINTF1(_L (" an option not in optstring or missing arg..\n "));
     			break;  
     			
     		case ':':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("missing argument and the 1st charcter of optstring is : \n "));
     			break;   			
     		}//end switch
     	
     		ch = getopt ( argc, argv, ":a:bc");
     	
		} //end while
     
     buf1 = '\0';
     
     if( ( strcmp( buf, ":" )) != 0 )
		{
		ret = KErrGeneral;
		}
	if ( optind < argc ) 
        {
        INFO_PRINTF1(_L ("non option ARGV elements actual:\n "));
        while ( optind < argc )	
        	{
        	INFO_PRINTF2(_L ("argv[%d]=="),optind);  
        	}
        }
 
    free ( buf ); 
     
    
     
    return ret; 
    }

   
// -----------------------------------------------------------------------------
//Function Name :getopt6
//API Tested :getopt
//TestCase Description:To test getopt to evaluate single set of arguments multiple times :
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest6(  )
    {       
	
    optind = 1;
    opterr = 1;
    
    int argc = 3;
      
	char *argv[] = { "getopt", "-a", "-b" }; //optstring  "a:b", returns ? ...shell returns ( option requires an argument )
    int ch;
    TInt ret = KErrNone; 
    char *buf = ( char* ) malloc ( 20 );
    char *buf1 = buf;
    int flag = 0;
    ch = getopt ( argc, argv, "abc");
     
    while ( ch != -1 )
		{
     	switch ( ch )
     		{
     		case 'a':
     		
     			sprintf(buf1++,"%c",ch); 
     			INFO_PRINTF1(_L ("option 'a' \n "));
     			
     			if ( flag == 0 )
     				{
     				optind = 1;
     				optreset = 1;
     				flag = 1;
     				}
     			
     			break;
     	
     		case 'b':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'b' \n "));
     			break;
     		
     		case 'c':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("option 'c' \n "));
     			break;
     		
     		case '?':
     		
     			sprintf(buf1++,"%c",ch);
     	    	INFO_PRINTF1(_L (" an option not in optstring or missing arg..\n "));
     			break;  
     			
     		case ':':
     		
     			sprintf(buf1++,"%c",ch);
     			INFO_PRINTF1(_L ("missing argument and the 1st charcter of optstring is : \n "));
     			break;   			
     		}//end switch
     	
     		ch = getopt ( argc, argv, "abc");
     	
		} //end while
     
     buf1 = '\0';
     
     if( ( strcmp( buf, "aab" )) != 0 )
		{
        ret = KErrGeneral;
		}
     
	 if ( optind < argc ) 
        {
        INFO_PRINTF1(_L ("non option ARGV elements actual:\n "));
        while ( optind < argc )	
        	{
        	INFO_PRINTF2(_L ("argv[%d]=="),optind);  
        	}
        }
        
    INFO_PRINTF2(_L ("optopt == %c "),optopt); 
 
    free ( buf ); 
     
    
     
    return ret; 
    }
    
// -----------------------------------------------------------------------------
//Function Name :getopt_long1
//API Tested :getopt_long
//TestCase Description:To test getopt_long  for all the long options
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest_long1(  )
    {       
     
     
	optind = 1;
	opterr = 1;
	int c;
        
	int argc = 4;
      
	char *argv[] = { "getopt", "--add","hi", "hello" };
	 	
	TInt ret = KErrNone; 
      
	int digit_optind = 0;
	INFO_PRINTF1(_L ("expected : option add non option ARGV elements : hello \n "));
        
	while( 1 )
		{
		int this_option_optind = optind ? optind : 1 ;
		int option_index = 0;
		static const struct option long_options[] = 
			{
        	{"add", 1, 0, 0 },
			{"append",0,0, 0},
			{"delete",1,0,0 },
			{"verbose",0,0,0 },
			{"create",1,0,'a'},
			{"file",1,0,0 },
			{0,0,0,0 }
    		};

		c = getopt_long( argc, argv, "abc:d:012", long_options, &option_index );
		if ( c== -1 )
			{
			break;
			}
		switch( c )
			{
			case 0:
		   		if( ( strcmp(long_options[option_index].name , "add") ) || ( strcmp ( optarg, "hi")))
					{
            		ret = KErrGeneral;
					}
				break;

			case '0':
   			case '1':
   			case '2':
				if( digit_optind != 0 && digit_optind != this_option_optind )
					{
          			INFO_PRINTF1(_L ("digits occur in two different argv elements \n"));
					}
        		digit_optind = this_option_optind;
        		INFO_PRINTF2(_L("option %c\n"),c );
        		break;

			case 'a':
				INFO_PRINTF1(_L ("option 'a' \n"));
				break;

  			case '?':
				break;
			} //end of switch

		} //end of while

		if( optind < argc ) 
			{
			INFO_PRINTF1(_L ("non-option ARGV elements:"));
			while ( optind < argc )
				{
                optind++;                	
				}
        	}
        if( strcmp( argv[3], "hello")) 
			{	
        	ret = KErrGeneral;
			}
        	
	
	
	return ret;

	}//end of function


// -----------------------------------------------------------------------------
//Function Name :getopt_long2
//API Tested :getopt_long
//TD Test case ID:1_14_2_1455
//TestCase Description:To test getopt_long  for all the long options
//Entry Into Source:\src\LPOSIX\SYSCALLS.CPP
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest_long2(  )
    {       
     
     
	optind = 1;
	opterr = 1;
	int c;
        
	int argc = 4;
      
	char *argv[] = { "getopt", "--create","hi", "nokia" };

	int digit_optind = 0;
   		
	TInt ret = KErrNone; 

	INFO_PRINTF1(_L ("expected : option a, non option ARGV elements: hi, nokia \n "));
   		
	while( 1 )
		{
		int this_option_optind = optind ? optind : 1 ;
		int option_index = 0;
		static const struct option long_options[] = 
			{

			{"add", 1, 0, 0 },
			{"append",0,0, 0},
        	{"delete",1,0,0 },
        	{"verbose",0,0,0 },
        	{"create",0,0,'a'},
        	{"file",1,0,0 },
			{0,0,0,0 }

    		};

		c = getopt_long( argc, argv, "abc:d:012", long_options, &option_index );

		if ( c== -1 )
			{
			break;
			}

   		switch( c )
	   		{

     		case 0:

        		INFO_PRINTF2(_L("option %s\n"), long_options[option_index].name );
        		if( optarg )
					{
            	    INFO_PRINTF2(_L("with arg %s\n"), optarg );
					}
            		
        		 break;

    		case '0':
    		case '1':
    		case '2':

        			if( digit_optind != 0 && digit_optind != this_option_optind )
						{
		           	    INFO_PRINTF1(_L ("digits occur in two different argv elements \n"));
						}
        	   	    digit_optind = this_option_optind;
        			
        			INFO_PRINTF2(_L("option %c\n"),c );
        			
				break;

  			case 'a':
  			
  					INFO_PRINTF1(_L ("option 'a' \n"));
        			break;

  			case '?':

       				INFO_PRINTF2(_L("?? getopt returned character code 0%o"),c );
       				
        			break;

  			} //end of switch

		} //end of while
	if( optind < argc ) 
		{
		INFO_PRINTF1(_L ("non-option ARGV elements:"));
		while ( optind < argc )
			{
			TBuf<MAX_SIZE> buf;
    		TInt len = strlen(argv[optind]);
    
    		for (TInt j =0; j<len;j++)
    			{
    			buf.Append(argv[optind][j]);
				}	
              // INFO_PRINTF2(_L("%s\n"),argv[ optind++]);
            INFO_PRINTF2(_L("%s\n"),&buf);
            optind++;
			}
		}
    if( strcmp( argv[2], "hi") || strcmp( argv[3], "nokia" )) 
		{
       	ret = KErrGeneral;
		}
	
	return ret;

	}//end of function
           
 
// -----------------------------------------------------------------------------
//Function Name :getopt_long3
//API Tested :getopt_long
//TestCase Description:To test getopt_long  for short options
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest_long3(  )
    {       
     
     
     	optind = 1;
     	opterr = 1;
         int ch;
        
        int argc = 5;
        
        TInt ret = KErrNone; 
        
        char *buf = ( char* ) malloc ( 20 );
      
     	char *buf1 = buf;
      
	 	char *argv[] = { "getopt", "-a","-c", "nokia","india" };

   		int digit_optind = 0;
   		
   		INFO_PRINTF1(_L ("expected : option 'a', option 'c' non option ARGV elements : india \n "));


   		while( 1 )

   		{

     		int this_option_optind = optind ? optind : 1 ;

     		int option_index = 0;

     		static const struct option long_options[] = {

        				{"add", 1, 0, 0 },
        				{"append",0,0, 0},
        				{"delete",1,0,0 },
        				{"verbose",0,0,0 },
        				{"create",0,0,'a'},
        				{"file",1,0,0 },
        				{0,0,0,0 }
    		};

    	ch = getopt_long( argc, argv, "abc:d:012", long_options, &option_index );

    	if ( ch== -1 )
        	break;

   		switch( ch )

   		{

     		case 0:

        		INFO_PRINTF2(_L("option %s\n"), long_options[option_index].name );
        		
        	
       			 if( optarg )
            	
            	    INFO_PRINTF2(_L("with arg %s\n"), optarg );
            		
        		 break;

    		case '0':
    		case '1':
    		case '2':

        			if( digit_optind != 0 && digit_optind != this_option_optind )

            	    
            	    INFO_PRINTF1(_L ("digits occur in two different argv elements \n"));
        	
            	    digit_optind = this_option_optind;
        			
        			INFO_PRINTF2(_L("option %c\n"),ch );
        			
        			 break;

  			case 'a':
					sprintf(buf1++,"%c",ch);
        			INFO_PRINTF1(_L ("option 'a' \n"));
        			break;
        			
        	case 'b':
					sprintf(buf1++,"%c",ch);
        			INFO_PRINTF1(_L ("option 'b' \n"));
        			break;
		    
		    case 'c':
					sprintf(buf1++,"%c",ch);
        			INFO_PRINTF1(_L ("option 'c' \n"));
        			break;
		

  			case '?':
					sprintf(buf1++,"%c",ch);
       				INFO_PRINTF2(_L("?? getopt returned character code 0%o"),ch );
       				
        			break;

  			} //end of switch

		} //end of while
		
		buf1 ='\0';
		
		if( ( strcmp( buf, "ac" )) != 0 )
     
         	     ret = KErrGeneral;

		if( optind < argc ) 

			{
				
			INFO_PRINTF1(_L ("non-option ARGV elements:"));
				
			while ( optind < argc )
				{
				TBuf<MAX_SIZE> buf;
    			TInt len = strlen(argv[optind]);
    
    			for (TInt j =0; j<len;j++)
	    			{
	    			buf.Append(argv[optind][j]);
					}	
              	// INFO_PRINTF2(_L("%s\n"),argv[ optind++]);
            	INFO_PRINTF2(_L("%s\n"),&buf);
            	optind++;
				}
                           	                	
        	}

		if( ( strcmp ( argv[4], "india")) != 0 )  
      
      		ret = KErrGeneral;
 
 	free(buf);
 	
	
	
	return ret;

	}//end of function          
           
 
// -----------------------------------------------------------------------------
//Function Name :getopt_long4
//API Tested :getopt_long
//TD Test case ID:1_14_2_1457
//TestCase Description:To test getopt_long  for digits
//Entry Into Source:\src\LPOSIX\SYSCALLS.CPP
// -----------------------------------------------------------------------------
  
TInt CTestStdlib::getoptTest_long4(  )
    {       
    
     
	optind = 1;
	opterr = 1;
	int ch;
        
	int argc = 4;
        
	TInt ret = KErrNone; 
      
	char *argv[] = { "getopt", "-1", "-2","nokia" };
	 	
	char *buf = ( char* ) malloc ( 20 );
      
	char *buf1 = buf;
      
	int digit_optind = 0;

	INFO_PRINTF1(_L ("expected : option 1, option 2  \n "));
   	
	while( 1 )

		{
		int this_option_optind = optind ? optind : 1 ;
		int option_index = 0;
		static const struct option long_options[] = 
			{
   				{"add", 1, 0, 0 },
   				{"append",0,0, 0},
   				{"delete",1,0,0 },
   				{"verbose",0,0,0 },
   				{"create",0,0,'a'},
   				{"file",1,0,0 },
   				{0,0,0,0 }
    		};

    	ch = getopt_long( argc, argv, "abc:d:012", long_options, &option_index );

    	if ( ch== -1 )
			{
        	break;
			}

   		switch( ch )
			{
			case 0:
				INFO_PRINTF2(_L("option %s\n"), long_options[option_index].name );
				if( optarg )
					{
            	    INFO_PRINTF2(_L("with arg %s\n"), optarg );
					}
            		
				break;

    		case '0':
    		case '1':
    		case '2':
    		case '3':
				sprintf(buf1++,"%c",ch);

        		if( digit_optind != 0 && digit_optind != this_option_optind )
					{
            	    INFO_PRINTF1(_L ("digits occur in two different argv elements \n"));
					}
				digit_optind = this_option_optind;
        			
        		INFO_PRINTF2(_L("option %c\n"),ch );
        		break;

  			case 'a':
				INFO_PRINTF1(_L ("option 'a' \n"));
        		break;
        			
        	case 'b':
				INFO_PRINTF1(_L ("option 'b' \n"));
        		break;
		    
		    case 'c':
				INFO_PRINTF1(_L ("option 'c' \n"));
        		break;
			
			case '?':
				INFO_PRINTF2(_L("?? getopt returned character code 0%o"),ch );
       			break;

  			} //end of switch

		} //end of while
		
	buf1 = '\0';
	if( ( strcmp( buf, "12" )) != 0 )
		{
        ret = KErrGeneral;
		}

	if( optind < argc ) 
		{
		INFO_PRINTF1(_L ("non-option ARGV elements:"));
		}
	while ( optind < argc )
		{
		TBuf<MAX_SIZE> buf;
    	TInt len = strlen(argv[optind]);
    
    	for (TInt j =0; j<len;j++)
    		{
    		buf.Append(argv[optind][j]);
			}	
              // INFO_PRINTF2(_L("%s\n"),argv[ optind++]);
        INFO_PRINTF2(_L("%s\n"),&buf);
        optind++;
		}
        	
	if( ( strcmp ( argv[3], "nokia")) != 0 )
		{
  		ret = KErrGeneral;
		}
 
	free( buf );
	
	

	return ret;

	}

TInt CTestStdlib::Testsigemptyset()
	{
	

	int ret=0;
	sigset_t sigset;
	INFO_PRINTF1(_L("In Testsigemptyset"));

	ret=sigemptyset(&sigset);
	INFO_PRINTF2(_L("sigemptyset returned %d"),ret);
	
	
    
	return ret;
	}

TInt CTestStdlib::strfmon1(  )
    {    
	
	TInt res = KErrNone;
   	INFO_PRINTF1(_L("Tstdlib: In strfmon_test\n"));    
    char string[31];
	if (setlocale(LC_MONETARY, "en_US.ISO-8859-1") == NULL)
		{
		INFO_PRINTF1(_L("error in setlocale. locale not changed\n"));
		INFO_PRINTF2(_L("The errno set is %d\n"),errno);
		return KErrGeneral;
		}
	ssize_t ret = strfmon(string, sizeof(string), "[%^=*#6n][%=*#6i]",1234.567, 1234.567);
	if (strcmp(string, "[$**1234.57][**1,234.57]") == 0)
		{
		INFO_PRINTF1(_L("strfmon successful \n"));
		INFO_PRINTF2(_L("%s\n"), string);
		}
	else
		{
		return KErrGeneral;
		}
	ret = strfmon(string, sizeof(string), "%=*#5n",123.45);
	if (strcmp(string, "$***123.45") == 0)
		{
		INFO_PRINTF1(_L("strfmon successful \n"));
		INFO_PRINTF2(_L("%s\n"), string);
		}
	else
		{
		return KErrGeneral;
		}

	return res;
	}//end of function 
// -----------------------------------------------------------------------------
//Function Name :TestRealPath5
//API Tested :realpath
//TestCase Description: To test if realpath with different input file names
// -----------------------------------------------------------------------------
TInt CTestStdlib::TestRealPath5()
    {
    
    	
    INFO_PRINTF1(_L("RealPathTest5"));
    
    TInt ret = KErrNone;
    TInt expdRet = KErrNone;
	
	char	resolvepath[PATH_MAX];

	char file1[512];
	ReadStringParam(file1);

	if(chdir("c:\\") < 0 )  
  		{
     	ERR_PRINTF1(_L("Failed to change working directory"));
     	return KErrGeneral;
  		}
  		
  	rmdir(file1);
	if(mkdir(file1 , 0666) < 0 )  
 	  	{
      	ERR_PRINTF1(_L("Directory creation failed"));
      	return KErrGeneral;      
  		}
	

	char file2[512];
	ReadStringParam(file2);
	FILE *fp = fopen(file2, "w");
	if(!fp)
		{
		INFO_PRINTF1(_L("fopen failed!!"));
		ret = KErrGeneral;
		}
	fclose(fp);


	char file3[512];
	ReadStringParam(file3);	
	char *rpath = realpath(file3, resolvepath);

	ReadStringParam(file3);
	if ( rpath == NULL || strcmp(rpath, file3) != 0)
		{
		ret = KErrGeneral;
		}
	ReadIntParam(expdRet);
	if (expdRet == ret)	
		{
		ret = KErrNone;	
		}
	else
		{
		ret = KErrGeneral;	
		}
	unlink(file2);  
	rmdir(file1);
 		
	
	return ret;
    
    }

TInt CTestStdlib::Testlseek1()
	{
	TInt x;
	struct stat buf1  ;

	x=open("c:\\testlseek.txt",O_CREAT|O_APPEND);
	if(x==-1)
		{
		INFO_PRINTF1(_L("open failed!!"));
		return KErrGeneral;
		}
	lseek(x,20,SEEK_SET);
	stat("c:\\testlseek.txt",&buf1);
	close(x);
	
	_LIT(KTestLseek,"The size of the file after lseek %d ");
	INFO_PRINTF2(KTestLseek,buf1.st_size);
	if(buf1.st_size == 0)
		{
		INFO_PRINTF1(_L("lseek passed!!"));
		unlink("c:\\testlseek.txt");
		return KErrNone;
		}
	INFO_PRINTF1(_L("lseek failed!!"));
	unlink("c:\\testlseek.txt");
	return KErrGeneral;
	}

TInt CTestStdlib::Testlseek2()
	{
	TInt x;
	struct stat buf1  ;

	x=open("c:\\testlseek.txt",O_CREAT|O_APPEND|O_RDWR);
	if(x==-1)
		{
		INFO_PRINTF1(_L("open failed!!"));
		return KErrGeneral;
		}
	write(x,"tallam",7);
	lseek(x,20,SEEK_SET);
	write(x,"revanthkumar",13);
	stat("c:\\testlseek.txt",&buf1);
	close(x);
	
	_LIT(KTestLseek,"The size of the file after lseek %d ");
	INFO_PRINTF2(KTestLseek,buf1.st_size);
	if(buf1.st_size == 33)
		{
		INFO_PRINTF1(_L("lseek passed!!"));
		unlink("c:\\testlseek.txt");
		return KErrNone;
		}
	INFO_PRINTF1(_L("lseek failed!!"));
	unlink("c:\\testlseek.txt");
	return KErrGeneral;
	}

TInt CTestStdlib::Testlseek3()
	{
	int testFD = -1;
	FILE *fp;
	char *filename = "C:\\lseek_sample.txt"; 
	char *cmd = "Z:\\sys\\bin\\popen_testwrite.exe";
	testFD = open(filename, O_CREAT | O_RDWR );
	if(testFD == -1)
			{
			INFO_PRINTF1(_L("open failed!!"));
			return KErrGeneral;
			}
	write(testFD,"hi",2);
	int seek_1 = lseek(testFD,0,SEEK_END);
	fp = popen(cmd,"r");
	if(fp == NULL)
			{
			INFO_PRINTF2(_L("popen failed!! %d"),errno);
			return KErrGeneral;
			}
	wait(NULL);
	pclose(fp);
	int seek_2 = lseek(testFD,0,SEEK_END);
	close(testFD);
	if(seek_2 == (seek_1+5))
		{
		INFO_PRINTF1(_L("lseek passed!!"));
		return KErrNone;
		}
	INFO_PRINTF1(_L("lseek failed!!"));
	return KErrGeneral;
	}

TInt CTestStdlib::getoptTest_long5(  )
    {       
     
     
	optind = 1;
	opterr = 1;
	int c;
        
	int argc = 4;
      
	char *argv[] = { "getopt", "--add","hi",NULL };
	TInt ret = KErrGeneral; 
      
	int this_option_optind = optind ? optind : 1 ;
	int option_index = 0;
	static const struct option long_options[] = 
		{
       	{"add", 1, 0, 1 },
		{"append",0,0, 0},
		{"delete",1,0,0 },
		{"verbose",0,0,0 },
		{"create",1,0,'a'},
		{"file",1,0,0 },
		{0,0,0,0 }
   		};

	c = getopt_long( argc, argv, "abc:d:012", long_options, &option_index );

	if(c==1)// returns "val" if flag is "NULL" 
		{
		ret = KErrNone;
		}
       	
	

	return ret;

	}//end of function 
		
/*
*Test name		:setjmpTest
*API			:setjmp
*Description	:set jump point for a non-local goto.
*If the return is from a direct invocation, setjmp() shall return 0.
*If the return is from a call to longjmp(), setjmp() shall return a non-zero value.
*/


TInt CTestStdlib::setjmpTest()
	{
	jmp_buf jmpbuffer;
    static	bool done = true;
	TInt ret = setjmp(jmpbuffer);
	if (!ret)
		{
		longjmp(jmpbuffer, 0);
		}
	else
		{
		if(done)
			{
			done = false;
			longjmp(jmpbuffer, 1);
			}
		 }
	return KErrNone;
	}


/*
*Test Name		:abortTest	
*API			:abort		
*Description	:generate an abnormal process abort and it does not return any value.
*If myfile.txt does not exist, a message is printed and abort is called.
*/

TInt CTestStdlib::abortTest()
	{
	const char* command = "Z:\\sys\\bin\\abort_test.exe";
	const char* mode ="r";
	FILE* fp = popen(command, mode);
	if(fp==NULL)
		{
		INFO_PRINTF2(_L("The errno set is %d\n"),errno);
		return KErrGeneral;
		}
	pclose(fp);
	return KErrNone;
	 }
	
	
/*
*Test Name 		:chownTest
*API			:chown
*Descripetion	:change owner and group of a file and and on success returns non negative integer.
*/


TInt CTestStdlib::chownTest()
	{
	TInt err;
	FILE* pFile;
	char name[15]= "c:\\myfile.txt";
	pFile = fopen (name,"w+");
	if (pFile == NULL)
		{
		INFO_PRINTF1(_L("file opening error"));
		}
	uid_t own = 13;
	gid_t grp = 23;
	err = chown(name,own,grp);
		if(err <0)
		{
		return KErrGeneral;
		}
	fclose(pFile);
	unlink(name);
	return KErrNone;
	}
	
	
/*
*Test Name 		:iconvTestL
*API			:iconv
*Description	:The iconv utility converts the characters  or  sequences  of
*characters  in  file from one code set to another and writes the results to standard output. 
*If an error occurs iconv() returns (size_t)-1 
*/
	
	
TInt CTestStdlib::iconvTest()
	{
	iconv_t cd = NULL;
    char utf8[12] = "abcd";
    char iso[12];
    const char *inbuf;
    char *outbuf;
    size_t err;
    size_t inbytes = 4;
    size_t outbytes = 4;
    inbuf = (const char*) utf8;
    outbuf = (char *) iso;
    cd = iconv_open("UTF-8", "ISO-8859-1");
    if(cd == (iconv_t) -1)
       {
       INFO_PRINTF1(_L("Unbale to create a conversion descriptor0"));
       return KErrGeneral;
       }
    err=iconv (cd, &inbuf, &inbytes, &outbuf, &outbytes);
    if(err == (size_t)-1)
       {
       INFO_PRINTF1(_L("Unbale to create a conversion descriptor0"));
       return KErrGeneral;
       }
    iconv_close(cd);
    return KErrNone;
	}


/*
*Testcase Name	:getgrentTest()
*API			:getgrent(),setgrent() and endgrent() 
*Description	:get the group database entry and The getgrent() function returns a pointer to a structure containing the broken-out fields of an entry in the group database. 
*/


TInt CTestStdlib::setgrentTest()
	{
	struct group *name;
	INFO_PRINTF1(_L("call to  getgrent\n"));
	setgrent(); 
	name = (struct group*)getgrent();
	if(name == NULL)
		{
		return KErrGeneral;
		}
	free(name->gr_mem);
	free(name);
	endgrent();
	return KErrNone;
	}
	
	
	
/*
*Testcase Name :wcreatTest
*API			:wcreat
*Description	:it creates a file  in current working directory(if file exists then it is truncated).
*Returns a non-negative value upon success.
*/


TInt CTestStdlib::wcreatTest()
	{
	TInt fd = 0;
	fd = wcreat(L"Example.txt" ,0666);
	if(fd < 0 )  
		{
		INFO_PRINTF1(_L("File creation failed \n"));
		return KErrGeneral;
		}
	return KErrNone;
	}

/*
* Testcase Name : atof_locale
* API			: atof
* Description	: Tests the behaviour(w.r.t the decimal point) in locale different from the system locale
* Returns a zero on success.
*/
TInt CTestStdlib::atof_locale()
	{
	TInt err = KErrNone;
	
	struct inout
		{
		char *lName;   //Locale Name
		char *inStr;  //Input String
		double outVal; //Expected output
		};
   
	const int testDataSize = 2;
	struct inout testData[testDataSize] =
	{
			{"en_US.ISO-8859-1", "1.5", 1.5},
			{"nl_NL.ISO-8859-1", "1,5", 1.5}
	};
	
	//Save the current locale and restore it after the test
	char currLocale[50];
	strcpy(currLocale, setlocale(LC_ALL, NULL) );
	
	for(int i = 0; i<testDataSize; i++)
		{
		if( setlocale(LC_ALL, testData[i].lName) == NULL )
			{
			ERR_PRINTF1(_L("Unable to set the locale"));
			err = KErrGeneral;
			break;
			}
		
		if(testData[i].outVal != atof(testData[i].inStr))
			{
			ERR_PRINTF1(_L("atof() failed"));
			err = KErrGeneral;
			break;
			}
		}
	
	setlocale(LC_ALL, currLocale);
	return err;
	}

/*
* Testcase Name : strtod_locale
* API			: strtod
* Description	: Tests the behaviour(w.r.t the decimal point) in locale different from the system locale
* Returns a zero on success.
*/
TInt CTestStdlib::strtod_locale()
	{
	TInt err = KErrNone;
	
	struct inout
		{
		char *lName;   //Locale Name
		char *inStr;  //Input String
		double outVal; //Expected output
		char *endPtr; //Tail
		};
   
	const int testDataSize = 5;
	struct inout testData[] =
	{
			{"en_US.ISO-8859-1", "1.5", 1.5, ""},
			{"en_US.ISO-8859-1", "0X1.F", 1.9375, ""},
			{"nl_NL.ISO-8859-1", "1,5", 1.5, ""},
			{"nl_NL.ISO-8859-1", "0x1,F", 1.9375, ""},
			{"nl_NL.ISO-8859-1", "1,5ghtd", 1.5, "ghtd"}
	};
	
	//Save the current locale and restore it after the test
	char currLocale[50];
    strcpy(currLocale, setlocale(LC_ALL, NULL) );
	
	for(int i = 0; i<testDataSize; i++)
		{
		if( setlocale(LC_ALL, testData[i].lName) == NULL )
			{
			ERR_PRINTF1(_L("Unable to set the locale"));
			err = KErrGeneral;
			break;
			}
		
		char *endPtr;
		if(testData[i].outVal != strtod(testData[i].inStr, &endPtr))
			{
			ERR_PRINTF1(_L("strtod() failed"));
			err = KErrGeneral;
			break;
			}
		
		if(strcmp(endPtr, testData[i].endPtr) != 0)
			{
			ERR_PRINTF1(_L("Getting tail failed"));
			err = KErrGeneral;
			break;			
			}
		}
	
	setlocale(LC_ALL, currLocale);
	return err;
	}

TInt CTestStdlib::binsearch_integers1(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In binsearch1()") );

    TInt key;
    int array[100];
    int* element=NULL;
    
    _LIT( Kkey, "key" );
	TBool res = GetIntFromConfig(ConfigSection(), Kkey, key);
	
	_LIT( Kelem1, "elem1" );
	TBool res1 = GetIntFromConfig(ConfigSection(), Kelem1, array[0]);
	_LIT( Kelem2, "elem2" );
	TBool res2 = GetIntFromConfig(ConfigSection(), Kelem2, array[1]);
	_LIT( Kelem3, "elem3" );
	TBool res3 = GetIntFromConfig(ConfigSection(), Kelem3, array[2]);
	_LIT( Kelem4, "elem4" );
	TBool res4 = GetIntFromConfig(ConfigSection(), Kelem4, array[3]);
	_LIT( Kelem5, "elem5" );
	TBool res5 = GetIntFromConfig(ConfigSection(), Kelem5, array[4]);
	_LIT( Kelem6, "elem6" );
	TBool res6 = GetIntFromConfig(ConfigSection(), Kelem6, array[5]);

    element=(int *)bsearch(&key,(void *)array,0,sizeof(array[0]),search_function); 
            
    if(element==NULL)
		{
        INFO_PRINTF1(_L(" SUCCESS"));
        return KErrNone;
		}
	else
		{
		INFO_PRINTF2(_L(" Failure "), key);
		return KErrGeneral;
		}        
	}

TInt CTestStdlib::calloc_Test3()
    {
    INFO_PRINTF1(_L("In calloc_Test3L"));
	char  *pc = (char *)calloc(10, 0);	
   
    bool i = (pc != NULL);
	INFO_PRINTF2(_L("{Expected: 1} %d"), (int)i);

	free(pc);
	if (i != true)
		{
		return KErrGeneral;
		} 
    
	return KErrNone;
    }

TInt CTestStdlib::calloc_Test4()
    {
    INFO_PRINTF1(_L("In calloc_Test4L"));
    size_t  nelem = (size_t)-10;
	char  *pc = (char *)calloc(nelem,10);	
   
    bool i = (pc == NULL);
	INFO_PRINTF2(_L("{Expected: 1} %d"), (int)i);

	free(pc);
	if (i != true)
		{
		return KErrGeneral;
		} 
    
	return KErrNone;
    }
   
TInt CTestStdlib::getenv_Test2()
    {

    INFO_PRINTF1(_L("In getenv_Test2"));
	char *psc = getenv(NULL);

	if (psc == NULL)
		{
		INFO_PRINTF1(_L("Success"));
		return KErrNone;
		}
	else 
		{
		INFO_PRINTF1(_L("Failure"));
		return KErrGeneral;
		}
	}

TInt CTestStdlib::reallocf_Test0()
    {
    INFO_PRINTF1(_L("In reallocf_Test0L"));
	int  *pint = (int *)calloc(2, sizeof (int) * 2);

	pint = (int *)reallocf(pint, (sizeof (int) * 6) );
	
	if (pint == NULL)
		{
		INFO_PRINTF1(_L("realloc failed to allocate memory"));
		return KErrNoMemory;
		}
    INFO_PRINTF1(_L("{Expected: zeros(4) and some uninitialized values}"));

	int i = 0;
	for(i = 0; i < 6; i++)
		{
	   INFO_PRINTF2(_L(" %d"), pint[i]);
		}

	for(i = 0; i < 4; i++)
		{
		if (pint[i] != 0)
			{
			free(pint);
			return KErrGeneral;
			}	
		}

	free(pint);
	return KErrNone;
	}

void regis1()
	{
	}
TInt CTestStdlib::atexit_Test2()
    {
    
    TInt ret = KErrNone;
    INFO_PRINTF1(_L("In atexit_Test2L"));
	
	int a = atexit(regis1);
	if(a==0)
		{
		ret = KErrNone;
		}
	else 
		{
		ret = KErrGeneral;
		}
	return ret;
    }

TInt CTestStdlib::strtol_space_string(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtol(space_string)") );
    
    char *endpt;
    long converted;

	converted = strtol(" 32445", &endpt,10);
	
	if(converted == 32445)
		{
		return KErrNone;	
		}
	else 
		{

		return KErrGeneral;
		}
	}
TInt CTestStdlib::strtol_extreme_string(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtol(extreme_string)") );
    
    char *endpt;
    long converted;

	converted = strtol(" 32445", &endpt,1);
	
	if(errno == EINVAL && converted == 0)
		{
		return KErrNone;	
		}
	else 
		{
		return KErrGeneral;
		}
	}
TInt CTestStdlib::strtol_extreme_string1(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtol(extreme_string1)") );
    
    char *endpt;
    long converted;

	converted = strtol(" 32445", &endpt,40);
	
	if(errno == EINVAL && converted == 0)
		{
		return KErrNone;	
		}
	else 
		{
		return KErrGeneral;
		}
	}
TInt CTestStdlib::strtoll_extreme_string(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtoll(extreme_string)") );
    
    char *endpt;
    long converted;

	converted = strtoll(" 32445", &endpt,1);
	
	if(errno == EINVAL && converted == 0)
		{
		return KErrNone;	
		}
	else 
		{
		return KErrGeneral;
		}
	}
TInt CTestStdlib::strtoll_extreme_string1(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtoll(extreme_string1)") );
    
    char *endpt;
    long converted;

	converted = strtoll(" 32445", &endpt,40);
	
	if(errno == EINVAL && converted == 0)
		{
		return KErrNone;	
		}
	else 
		{
		return KErrGeneral;
		}
	}
TInt CTestStdlib::strtoll_good_param(  )
	{
    
    INFO_PRINTF1(_L("Tstdlib: In strtol()") );
    
    TInt endptr,base;
    TInt expected;
    TPtrC string;
    long converted=0;
    char *endpt;
    
    
	_LIT( Kstring, "string" );
	TBool res1 = GetStringFromConfig(ConfigSection(), Kstring, string);
	_LIT( Kendptr, "endptr" );
	TBool res2 = GetIntFromConfig(ConfigSection(), Kendptr, endptr);
	_LIT( Kbase, "base" );
	TBool res3 = GetIntFromConfig(ConfigSection(), Kbase, base);
	_LIT( Kexpected, "expected" );
	TBool res4 = GetIntFromConfig(ConfigSection(), Kexpected, expected);
	if(res1 && res2 && res3 && res4)
		{
        if(endptr==0)
			{
            endpt=NULL;
			}

        TBuf8<100>buf;
        buf.Copy(string);


        char* ptr = (char*) buf.Ptr();
        ptr[buf.Length()]='\0';
        converted = strtoll(ptr,&endpt,base);

        if(base<0 || base >36)
			{
            INFO_PRINTF1(_L("Base should be between 0-36 for meaningful output"));
            INFO_PRINTF2(_L(" Error No: %d"), errno);
            if(errno == 0)
				{
                INFO_PRINTF1(_L("No conversion performed"));
				}
            return KErrNone;
			}

        if(converted == LONG_MIN || converted == LONG_MAX)
			{
            INFO_PRINTF1(_L("Return value cannot be represented..."));
            INFO_PRINTF1(_L("....at the destination type"));
            INFO_PRINTF2(_L(" Error No: %d"), errno);
			}
        else if(converted == expected)
			{
            INFO_PRINTF2(_L(" Expected %i"), expected);
	        }
        else
			{
            INFO_PRINTF3(_L("Input: %S Result: %f"), &string, converted );
            INFO_PRINTF2(_L(" Expected %f"), expected);
            INFO_PRINTF1(_L("Unexpected output"));
            return KErrGeneral;
			}
		}
    else
		{
        INFO_PRINTF1(_L("Expected input-<string<endptr(0/1)<base(0-36)>.."));
        INFO_PRINTF1(_L("<expected_value>"));
        return KErrGeneral;
		}   
	return KErrNone;
	}
TInt CTestStdlib::strtoul_extreme_string(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtoul(extreme_string)") );
    
    char *endpt;
    long converted;

	converted = strtoul(" 32445", &endpt,1);
	

	if(errno == EINVAL && converted == 0)
		{
		return KErrNone;	
		}
	else 
		{
		return KErrGeneral;
		}
	}
TInt CTestStdlib::strtoul_extreme_string1(  )
	{
    INFO_PRINTF1(_L("Tstdlib: In strtoul(extreme_string1)") );
    
    char *endpt;
    long converted;

	converted = strtoul(" 32445", &endpt,40);
	if(errno == EINVAL && converted == 0)
		{
		return KErrNone;	
		}
	else 
		{
		return KErrGeneral;
		}

	}

TInt CTestStdlib::qsort_integers1(  )
    {
    INFO_PRINTF1(_L("Tstdlib: In quicksort()") );

	int array[100];
    int temp=0;
    int notSorted=0;
    TInt i,j;
    
    _LIT( Kelem1, "elem1" );
    _LIT( Kelem2, "elem2" );
    _LIT( Kelem3, "elem3" );
    _LIT( Kelem4, "elem4" );
    _LIT( Kelem5, "elem5" );
    _LIT( Kelem6, "elem6" );
    _LIT( Kelem7, "elem7" );
	_LIT( Kelem8, "elem8" );

    TBool res1 = GetIntFromConfig(ConfigSection(), Kelem1, array[0]);
    TBool res2 = GetIntFromConfig(ConfigSection(), Kelem2, array[1]);
    TBool res3 = GetIntFromConfig(ConfigSection(), Kelem3, array[2]);
    TBool res4 = GetIntFromConfig(ConfigSection(), Kelem4, array[3]);
    TBool res5 = GetIntFromConfig(ConfigSection(), Kelem5, array[4]);
    TBool res6 = GetIntFromConfig(ConfigSection(), Kelem6, array[5]);
    TBool res7 = GetIntFromConfig(ConfigSection(), Kelem7, array[6]);
    TBool res8 = GetIntFromConfig(ConfigSection(), Kelem8, array[7]);
    i=8;
    
    if(i!=0)
        {
        INFO_PRINTF2(_L(" i: %d "),i);

        int save=i;
        for(j=0;j<save;j++)
            {
            INFO_PRINTF2(_L(" %d "),array[j]);
            }
            
        qsort((void *)array,i,sizeof(array[0]),sort_function);

        INFO_PRINTF1(_L("Array after sort"));

        for(j=0;j<save;j++)
            {
            INFO_PRINTF2(_L(" %d "),array[j]);
            }

        temp=array[0];

        for(j=1;j<save;j++)
            {
            if(temp>array[j])
                {
                notSorted=1;
                }
            else
                {
                temp=array[j];
                }

            }
            
        if(notSorted)
			{
            return KErrGeneral;
			}

        return KErrNone;

        }
    else
        {
            INFO_PRINTF1(_L("Expected Parameters: <integer-1<interger-2>.."));
            return KErrGeneral;
        }
    }

TInt CTestStdlib::qsort_integers2(  )
    {
    INFO_PRINTF1(_L("Tstdlib: In quicksort()") );

	int array[100];
    int temp=0;
    int notSorted=0;
    TInt i,j;
    
    _LIT( Kelem1, "elem1" );
    _LIT( Kelem2, "elem2" );
    _LIT( Kelem3, "elem3" );
    _LIT( Kelem4, "elem4" );
    _LIT( Kelem5, "elem5" );
    _LIT( Kelem6, "elem6" );
    _LIT( Kelem7, "elem7" );

    TBool res1 = GetIntFromConfig(ConfigSection(), Kelem1, array[0]);
    TBool res2 = GetIntFromConfig(ConfigSection(), Kelem2, array[1]);
    TBool res3 = GetIntFromConfig(ConfigSection(), Kelem3, array[2]);
    TBool res4 = GetIntFromConfig(ConfigSection(), Kelem4, array[3]);
    TBool res5 = GetIntFromConfig(ConfigSection(), Kelem5, array[4]);
    TBool res6 = GetIntFromConfig(ConfigSection(), Kelem6, array[5]);
    TBool res7 = GetIntFromConfig(ConfigSection(), Kelem7, array[6]);
    i=7;
    
    if(i!=0)
        {
        INFO_PRINTF2(_L(" i: %d "),i);

        int save=i;

        for(j=0;j<save;j++)
            {
            INFO_PRINTF2(_L(" %d "),array[j]);
            }
            
        qsort((void *)array,i,sizeof(array[0]),sort_function);
        INFO_PRINTF1(_L("Array after sort"));

        for(j=0;j<save;j++)
            {
            INFO_PRINTF2(_L(" %d "),array[j]);
            }

        temp=array[0];

        for(j=1;j<save;j++)
            {
            if(temp>array[j])
                {
                notSorted=1;
                }
            else
                {
                temp=array[j];
                }

            }
            
        if(notSorted)
			{
            return KErrGeneral;
			}

        return KErrNone;

        }
    else
        {
            INFO_PRINTF1(_L("Expected Parameters: <integer-1<interger-2>.."));
            return KErrGeneral;
        }
    }

TInt CTestStdlib::qsort_integers3(  )
    {
    INFO_PRINTF1(_L("Tstdlib: In quicksort()") );

	int array[100];
    int temp=0;
    int notSorted=0;
    TInt i,j;
    
    _LIT( Kelem1, "elem1" );
    _LIT( Kelem2, "elem2" );
    _LIT( Kelem3, "elem3" );
    _LIT( Kelem4, "elem4" );
    _LIT( Kelem5, "elem5" );
    _LIT( Kelem6, "elem6" );
    _LIT( Kelem7, "elem7" );
    _LIT( Kelem8, "elem8" );
    _LIT( Kelem9, "elem9" );
    _LIT( Kelem10, "elem10" );
    _LIT( Kelem11, "elem11" );
    _LIT( Kelem12, "elem12" );
    _LIT( Kelem13, "elem13" );
    _LIT( Kelem14, "elem14" );
    _LIT( Kelem15, "elem15" );
    _LIT( Kelem16, "elem16" );
    _LIT( Kelem17, "elem17" );
    _LIT( Kelem18, "elem18" );
    _LIT( Kelem19, "elem19" );
    _LIT( Kelem20, "elem20" );
    _LIT( Kelem21, "elem21" );
    _LIT( Kelem22, "elem22" );
    _LIT( Kelem23, "elem23" );
    _LIT( Kelem24, "elem24" );
    _LIT( Kelem25, "elem25" );
    _LIT( Kelem26, "elem26" );
    _LIT( Kelem27, "elem27" );
    _LIT( Kelem28, "elem28" );
    _LIT( Kelem29, "elem29" );
    _LIT( Kelem30, "elem30" );
    _LIT( Kelem31, "elem31" );
    _LIT( Kelem32, "elem32" );
    _LIT( Kelem33, "elem33" );
    _LIT( Kelem34, "elem34" );
    _LIT( Kelem35, "elem35" );
    _LIT( Kelem36, "elem36" );
    _LIT( Kelem37, "elem37" );
    _LIT( Kelem38, "elem38" );
    _LIT( Kelem39, "elem39" );
    _LIT( Kelem40, "elem40" );
    _LIT( Kelem41, "elem41" );

    TBool res1 = GetIntFromConfig(ConfigSection(), Kelem1, array[0]);
    TBool res2 = GetIntFromConfig(ConfigSection(), Kelem2, array[1]);
    TBool res3 = GetIntFromConfig(ConfigSection(), Kelem3, array[2]);
    TBool res4 = GetIntFromConfig(ConfigSection(), Kelem4, array[3]);
    TBool res5 = GetIntFromConfig(ConfigSection(), Kelem5, array[4]);
    TBool res6 = GetIntFromConfig(ConfigSection(), Kelem6, array[5]);
    TBool res7 = GetIntFromConfig(ConfigSection(), Kelem7, array[6]);
    TBool res8 = GetIntFromConfig(ConfigSection(), Kelem8, array[7]);
    TBool res9 = GetIntFromConfig(ConfigSection(), Kelem9, array[8]);
    TBool res10 = GetIntFromConfig(ConfigSection(), Kelem10, array[9]);
    TBool res11 = GetIntFromConfig(ConfigSection(), Kelem11, array[10]);
    TBool res12 = GetIntFromConfig(ConfigSection(), Kelem12, array[11]);
    TBool res13 = GetIntFromConfig(ConfigSection(), Kelem13, array[12]);
    TBool res14 = GetIntFromConfig(ConfigSection(), Kelem14, array[13]);
    TBool res15 = GetIntFromConfig(ConfigSection(), Kelem15, array[14]);
    TBool res16 = GetIntFromConfig(ConfigSection(), Kelem16, array[15]);
    TBool res17 = GetIntFromConfig(ConfigSection(), Kelem17, array[16]);
    TBool res18 = GetIntFromConfig(ConfigSection(), Kelem18, array[17]);
    TBool res19 = GetIntFromConfig(ConfigSection(), Kelem19, array[18]);
    TBool res20 = GetIntFromConfig(ConfigSection(), Kelem20, array[19]);
    TBool res21 = GetIntFromConfig(ConfigSection(), Kelem21, array[20]);

    TBool res22 = GetIntFromConfig(ConfigSection(), Kelem22, array[21]);
    TBool res23 = GetIntFromConfig(ConfigSection(), Kelem23, array[22]);
    TBool res24 = GetIntFromConfig(ConfigSection(), Kelem24, array[23]);
    TBool res25 = GetIntFromConfig(ConfigSection(), Kelem25, array[24]);
    TBool res26 = GetIntFromConfig(ConfigSection(), Kelem26, array[25]);
    TBool res27 = GetIntFromConfig(ConfigSection(), Kelem27, array[26]);
    TBool res28 = GetIntFromConfig(ConfigSection(), Kelem28, array[27]);
    TBool res29 = GetIntFromConfig(ConfigSection(), Kelem29, array[28]);
    TBool res30 = GetIntFromConfig(ConfigSection(), Kelem30, array[29]);
    TBool res31 = GetIntFromConfig(ConfigSection(), Kelem31, array[30]);
    TBool res32 = GetIntFromConfig(ConfigSection(), Kelem32, array[31]);
    TBool res33 = GetIntFromConfig(ConfigSection(), Kelem33, array[32]);
    TBool res34 = GetIntFromConfig(ConfigSection(), Kelem34, array[33]);
    TBool res35 = GetIntFromConfig(ConfigSection(), Kelem35, array[34]);
    TBool res36 = GetIntFromConfig(ConfigSection(), Kelem36, array[35]);
    TBool res37 = GetIntFromConfig(ConfigSection(), Kelem37, array[36]);
    TBool res38 = GetIntFromConfig(ConfigSection(), Kelem38, array[37]);
    TBool res39 = GetIntFromConfig(ConfigSection(), Kelem39, array[38]);
    TBool res40 = GetIntFromConfig(ConfigSection(), Kelem40, array[38]);
    TBool res41 = GetIntFromConfig(ConfigSection(), Kelem41, array[40]);
    
    i=41;
    
    if(i!=0)
        {
        INFO_PRINTF2(_L(" i: %d "),i);

        int save=i;
        for(j=0;j<save;j++)
            {
            INFO_PRINTF2(_L(" %d "),array[j]);
            }
            
        qsort((void *)array,i,sizeof(array[0]),sort_function);
        INFO_PRINTF1(_L("Array after sort"));

        for(j=0;j<save;j++)
            {
            INFO_PRINTF2(_L(" %d "),array[j]);
            }

        temp=array[0];

        for(j=1;j<save;j++)
            {
            if(temp>array[j])
                {
                notSorted=1;
                }
            else
                {
                temp=array[j];
                }

            }
            
        if(notSorted)
			{
            return KErrGeneral;
			}

        return KErrNone;

        }
    else
        {
            INFO_PRINTF1(_L("Expected Parameters: <integer-1<interger-2>.."));
            return KErrGeneral;
        }
    }
    
TInt CTestStdlib::strtoullbasic()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoull("900000",&endp,0);
	if(i==900000)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::strtoullneg()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoull("-900000",&endp,0);
	if(i==-900000)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::strtoullpos()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoull("+900000",&endp,1);
	if(i==0)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::strtoullbase()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoull("900000",&endp,40);
	if(i==0)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::strtoulpos()
	{
	TInt res=KErrGeneral;
	char *endp;
	long long i=strtoul("+900000",&endp,16);
	if(i==9437184)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::strtoimaxbasic()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoimax("+12",&endp,0);
	if(i==12)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::strtoimaxbasic1()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoimax("12",&endp,1);
	if(i==0)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::strtoimaxbasic2()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoimax("12",&endp,40);
	if(i==0)
		{
		res=KErrNone;
		}

	return res;
	}
TInt CTestStdlib::strtoumaxbasic()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoumax("+12",&endp,0);
	if(i==12)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::strtoumaxbasic1()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoumax("12",&endp,1);
	if(i==0)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::strtoumaxbasic2()
	{
	TInt res=KErrGeneral;
	char *endp;
	intmax_t i=strtoumax("12",&endp,40);
	if(i==0)
		{
		res=KErrNone;
		}
	return res;
	}
TInt CTestStdlib::confstr_Test2()
    {
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In confstr_Test2L"));
    
    int n=36;
    
    char *buf = (char *)malloc(n);
    
    int len = confstr(_CS_POSIX_V6_ILP32_OFF32_CFLAGS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	
	len = confstr(_CS_POSIX_V6_ILP32_OFF32_LDFLAGS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
		
	len = confstr(_CS_POSIX_V6_ILP32_OFF32_LIBS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	
	len = confstr(_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	
	len = confstr(_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	
	len = confstr(_CS_POSIX_V6_LPBIG_OFFBIG_LIBS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	
	n = 1;
	len = confstr(_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	
	len = confstr(_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	
	len = confstr(_CS_POSIX_V6_ILP32_OFFBIG_LIBS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	n=36;
	len = confstr(_CS_POSIX_V6_LP64_OFF64_CFLAGS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}

	len = confstr(_CS_POSIX_V6_LP64_OFF64_LDFLAGS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
	
	len = confstr(_CS_POSIX_V6_LP64_OFF64_LIBS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
		
	n=23;
	len = confstr(_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS, buf, n);
                    
    INFO_PRINTF2(_L("PATH in buffer: %s"), buf);
    INFO_PRINTF2(_L("length: %d"), len);
      
    if( len != n )
		{
        ret = KErrGeneral;
		}
		
	
	free(buf);
	return ret;
    }
TInt CTestStdlib::tmpfile_fseek(  )
    {
    FILE* fp = tmpfile();
    int ret = KErrNone;
    char buf[10];
 
 	if(fp)
 		{
     	fprintf(fp, "%s", "hello");
     	fflush(fp);
     	fseek(fp, -2, SEEK_END); 
     	int z = fscanf(fp, "%s", buf);
     	fflush(fp);
     	fclose(fp);
     	if(strcmp(buf,"lo") != 0) 
     		{
 			INFO_PRINTF1(_L("Failed to seek to the specified position"));
			ret = KErrGeneral;
 			}
 		}
 	else
 		{
 		INFO_PRINTF2(_L("Failed to open the tmpfile with errno: %d"), errno);
		ret = KErrGeneral;
 		}  
 	return ret;
    }
TInt CTestStdlib::testSymLink()
    {
    int ret = KErrNone;
    const char* file = "C:\\file.dat";
    const char* hlnk = "c:\\hard.lnk";
    const char* hlnk2 = "c:\\\\hard.lnk";
    
    const char* data = "1234567890";
    const char* data1 = "abcdefghij";
    const char* data2 = "ABCDEFGHIJ";
    FILE* fp = fopen(file, "w");
    fwrite(data, 1, 10, fp);
    fclose(fp);
    
    ret = link(file, hlnk);
    if (ret)
     {
     printf("create hard link fail :%d\n", ret);
     return ret;
     }
    
    fp = fopen(hlnk, "r+");
    if( fp )
     fwrite(data1, 1, 10, fp);
    fclose(fp);
    
    fp = fopen(hlnk2, "r+");
    if( fp )
     fwrite(data2, 1, 10, fp);
    fclose(fp);
    return ret;
    }
