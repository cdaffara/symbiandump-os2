/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "tdl.h"
#include <dlfcn.h>
#include <stdlib.h>


#ifdef __EPOC32__
const char* dptr="_Z6divideii"; 
#else 
const char* dptr="?divide@@YAHHH@Z"; 
#endif

#define TEST_VAL(val, exp, text)				  {\
												if ((val==exp)) \
													{ \
													INFO_PRINTF1(text); \
													INFO_PRINTF1(_L("...passed"));\
													} \
												else \
													{ \
													ERR_PRINTF1(text); \
													ERR_PRINTF3(_L("...failed with Actual: %d Expected: %d"),(val), (exp));\
													return EFail;\
													}\
												}

#define TEST_COND(cond, text)				  {\
												if ((cond)) \
													{ \
													INFO_PRINTF1(text); \
													INFO_PRINTF1(_L("...passed"));\
													} \
												else \
													{ \
													ERR_PRINTF1(text); \
													ERR_PRINTF1(_L("...failed"));\
													return EFail;\
													}\
												}
												
CTestDlByName::	CTestDlByName(const TDesC& aStepName)
{
	SetTestStepName(aStepName);		
}

CTestDlByName::~CTestDlByName()
{
	
}

TVerdict CTestDlByName::doTestStepL()
{
	int err;
	if (TestStepName() == KDlSymLkupWithHandle)
   		{
   	   	INFO_PRINTF1(_L("TestSymLkupWithHandle():"));
   	   	err = TestSymLkupWithHandle();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if (TestStepName() == KDlSymLkupWithRTLD_DEFAULT)  	   	
   		{
   	   	INFO_PRINTF1(_L("TestSymLkupWithRTLD_DEFAULT():"));
   	   	err = TestSymLkupWithRTLD_DEFAULT();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if (TestStepName() == KDlLoadByVar)  	   	
   		{
   	   	INFO_PRINTF1(_L("TestLoadByVar():"));
   	   	err = TestLoadByVar();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if (TestStepName() == KDlLoadWithInvalidArg) 	   	
   		{
   	   	INFO_PRINTF1(_L("TestLoadWithInvalidArg():"));
   	   	err = TestLoadWithInvalidArg();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if (TestStepName() == KDlMutexOnOpenHandle) 	   	
   		{
   	   	INFO_PRINTF1(_L("TestMutexOnOpenHandle():"));
   	   	err = TestMutexOnOpenHandle();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	   		
   	else if (TestStepName() == KOpenNonStdDllOrdinalLookup) 	   	
   		{
   	   	INFO_PRINTF1(_L("OpenNonStdDllOrdinalLookup():"));
   	   	err = OpenNonStdDllOrdinalLookup();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if (TestStepName() == KOpenNonStdDllSymbolicLookup) 	   	
   		{
   	   	INFO_PRINTF1(_L("OpenNonStdDllSymbolicLookup():"));
   	   	err = OpenNonStdDllSymbolicLookup();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if (TestStepName() == KOpenNonStdDllSymbolicLookupWithRTLD_DEFAULT) 	   	
   		{
   	   	INFO_PRINTF1(_L("OpenNonStdDllSymbolicLookupWithRTLD_DEFAULT():"));
   	   	err = OpenNonStdDllSymbolicLookupWithRTLD_DEFAULT();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	return TestStepResult(); 		
}

EXPORT_C  int divide(int x, int y)
{
	return (x/y);	
}

TVerdict CTestDlByName::TestSymLkupWithHandle()
{
	typedef int (*fptr)(int, int);
	typedef int (*vptr)(void);
	
	int (*add)(int, int);
	int (*divide)(int, int);
	int (*mult)(int, int);	
	int (*sum)(int, int);
	int (*get)(void);
	
	typedef void *(*DlSym)(void*, const char*);
	
	/*open the dll*/
__UHEAP_MARK;	
	void* handle=dlopen("Z:\\Sys\\Bin\\arithmeticoper1.dll", RTLD_LOCAL|RTLD_LAZY);
	TEST_COND( (handle!=NULL && dlerror()==NULL), _L("dlopen() with RTLD_LOCAL..."));

	/*open the same dll*/
	void* handle1=dlopen("Z:\\Sys\\Bin\\arithmeticoper1.dll", RTLD_LOCAL|RTLD_LAZY);
	TEST_COND( (handle1!=NULL && dlerror()==NULL), _L("dlopen()..."));
	
	TEST_COND((handle==handle1), _L("load the same dll twice..."));

	/*lookup for a valid symbol*/
	
	add= (fptr)dlsym(handle, "add");	
	TEST_COND((add!=NULL && dlerror()==NULL), _L("dlsym() for a valid symbol..."));

	/*lookup for a valid symbol using ordinal*/
	add= (fptr)dlsym(handle, "1");	
	TEST_COND((add!=NULL && dlerror()==NULL), _L("dlsym() using ordinal number..."));

	/*lookup for a valid symbol using invalid ordinal*/
	add= (fptr)dlsym(handle, "100");	
	TEST_COND((add==NULL && dlerror()!=NULL), _L("dlsym() using invalid ordinal number..."));

	/*lookup for a valid symbol using invalid ordinal*/
//	add= (fptr)dlsym(handle, "-1");	
//	TEST_COND((add==NULL && dlerror()!=NULL), _L("dlsym() using invalid ordinal number..."));
		
	/*lookup for a valid symbol in the dependent dll*/
	mult= (fptr)dlsym(handle, "mult");	
	TEST_COND((mult!=NULL && dlerror()==NULL), _L("dlsym() for a symbol in a dependent dll..."));
	
	/*lookup for a symbol in this exe*/
	divide=(fptr) dlsym(handle, dptr);	
	TEST_COND((divide==NULL && dlerror()!=NULL), _L("dlsym() for a global symbol..."));	

	/*lookup for a non-existing symbol*/
	sum=(fptr) dlsym(handle, "sum");	
	TEST_COND((sum==NULL && dlerror()!=NULL), _L("dlsym() for a non-existing symbol..."));	

	/*lookup for a corrupted symbol*/
	sum=(fptr) dlsym(handle, "");	
	TEST_COND((sum==NULL && dlerror()!=NULL), _L("dlsym() for a corrupted symbol..."));
	
	/*check the dependency ordering*/
	get=(vptr) dlsym(handle, "get");	
	TEST_COND((get!=NULL && dlerror()==NULL), _L("dlsym()..."));	

	int val=(*get)();
	TEST_VAL(val, 4, _L("dependency ordering check.."));					

#ifdef __EPOC32__
	/*check the dependency ordering*/
	int* pval=(int*) dlsym(handle, "g_val");	
	TEST_COND((pval!=NULL && dlerror()==NULL), _L("dlsym()..."));
	
	TEST_VAL(*pval, 40, _L("dependency ordering check.."));					
#endif

	/*load an non-std dll*/
	void* handle2=dlopen("z:\\sys\\bin\\dll1.dll", RTLD_GLOBAL|RTLD_LAZY);
	TEST_COND( (handle2!=NULL && dlerror()==NULL), _L("dlopen() the non std dll"));
				
	/*close the handle*/
		
	TInt err=dlclose(handle);
	TEST_VAL(err, 0, _L("dlclose() valid handle..."));				

	/*close the handle*/
	err=dlclose(handle1);
	TEST_VAL(err, 0, _L("dlclose() valid handle..."));	
	
	/*lookup for a valid symbol with invalid handle*/
	add= (fptr)dlsym(handle, "add");	
	TEST_COND((add==NULL && dlerror()!=NULL), _L("dlsym() for a valid symbol with invalid handle..."));
	
	/*close the handle*/
	err=dlclose(handle2);
	TEST_VAL(err, 0, _L("dlclose() valid handle..."));
	
__UHEAP_MARKEND;	

	SetTestStepResult(EPass);
	return TestStepResult(); 						
}


TVerdict CTestDlByName::TestSymLkupWithRTLD_DEFAULT()
{
	typedef int (*fptr)(int, int);
	typedef int (*vptr)(void);
	
	int (*add)(int, int);
	int (*divide)(int, int);
	int (*mult)(int, int);
	int (*get)(void);

	/*open the dll*/
	void* handle=dlopen("Z:\\Sys\\Bin\\arithmeticoper1.dll", RTLD_GLOBAL|RTLD_LAZY);
	TEST_COND( (handle!=NULL && dlerror()==NULL), _L("dlopen() with RTLD_GLOBAL..."));
	
	/*lookup for a valid symbol*/
	add= (fptr)dlsym(RTLD_DEFAULT, "add");	
	TEST_COND((add!=NULL && dlerror()==NULL), _L("dlsym() with RTLD_DEFAULT for a valid symbol..."));	
	
	/*lookup for a symbol in this exe*/
	divide=(fptr) dlsym(RTLD_DEFAULT, dptr);	
	TEST_COND((divide!=NULL && dlerror()==NULL), _L("dlsym()..."));	

	/*lookup for a valid symbol in the dependent dll*/
	mult= (fptr)dlsym(RTLD_DEFAULT, "mult");	
	TEST_COND((mult!=NULL && dlerror()==NULL), _L("dlsym() for a symbol in a dependent dll..."));		

	/*check the load ordering*/
	get=(vptr) dlsym(RTLD_DEFAULT, "get");	
	TEST_COND((get!=NULL && dlerror()==NULL), _L("dlsym()..."));	

	int val=(*get)();
	TEST_VAL(val, 3, _L("load ordering check.."));
	
#ifdef __EPOC32__	
	/*check the load ordering*/
	int* pval=(int*) dlsym(RTLD_DEFAULT, "g_val");	
	TEST_COND((pval!=NULL && dlerror()==NULL), _L("dlsym()..."));	

	INFO_PRINTF2(_L("Address of data: %x"), pval);

	TEST_VAL(*pval, 30, _L("load ordering check.."));	
#endif	

	/*check the unsupported handles like RTLD_NEXT and RTLD_SELF*/
	get=(vptr) dlsym(RTLD_NEXT, "get");	
	TEST_COND((get==NULL && dlerror()!=NULL), _L("dlsym() with unsupported handle, RTLD_NEXT"));	
				
	get=(vptr) dlsym(RTLD_SELF, "get");	
	TEST_COND((get==NULL && dlerror()!=NULL), _L("dlsym() with unsupported handle, RTLD_SELF"));	

	/*close the handle*/
	TInt err=dlclose(handle);
	TEST_VAL(err, 0, _L("dlclose()..."));				

	/*open the global handle*/
	void* handle1=dlopen(0, RTLD_GLOBAL|RTLD_LAZY);
	TEST_COND( (handle1!=NULL && dlerror()==NULL), _L("dlopen() with file as 0"));
		
	/*lookup for a symbol in this exe*/
	divide=(fptr) dlsym(handle1, dptr);	
	TEST_COND((divide!=NULL && dlerror()==NULL), _L("dlsym()..."));

	/*close the handle TODO*/
	err=dlclose(handle1);
	TEST_VAL(err, 0,_L("dlclose()..."));	
		
	SetTestStepResult(EPass);
	return TestStepResult(); 	
}


TVerdict CTestDlByName::TestLoadByVar()
{
	const char* value="Z:\\Sys\\Bin\\";	
	
	/*set the env*/
	int err=setenv("LD_LIBRARY_PATH",value,1);		
	TEST_VAL(err,0, _L("setenv()..."));	
	
	/* use the env to open the dll*/
	void* handle=dlopen("arithmeticoper1.dll", RTLD_GLOBAL|RTLD_LAZY);
	TEST_COND( (handle!=NULL && dlerror()==NULL), _L("dlopen() from valid LD_LIBRARY_PATH .."));	
		
	/*close the handle*/
	err=dlclose(handle);
	TEST_VAL(err, 0, _L("dlclose()..."));				

	unsetenv("LD_LIBRARY_PATH"); 
	
	const char* value1="Z:\\System\\";	
	
	/*set the env*/
	err=setenv("LD_LIBRARY_PATH",value1,1);		
	TEST_VAL(err,0, _L("setenv()..."));	

	/* use the env to open the dll*/
	void* handle1=dlopen("arithmeticoper1.dll", RTLD_GLOBAL|RTLD_LAZY);
	TEST_COND( (handle1==NULL && dlerror()!=NULL), _L("dlopen() from an invalid LD_LIBRARY_PATH.."));	
		
	/*close the handle*/
	err=dlclose(handle1);
	TEST_COND((err!=0), _L("dlclose()..."));				

	unsetenv("LD_LIBRARY_PATH"); 
			
	SetTestStepResult(EPass);
	return TestStepResult(); 		
}

TVerdict CTestDlByName::TestLoadWithInvalidArg()
{
	/*open the non-existing dll*/
	void* handle=dlopen("Z:\\Sys\\Bin\\arithmeticoper5.dll", RTLD_GLOBAL|RTLD_LAZY);
	TEST_COND( (handle==NULL && dlerror()!=NULL), _L("dlopen() the non-existing dll..."));

	/*open the invalid dll*/
	void* handle1=dlopen("Z:\\Sys\\Bin\\arithmeticoper5.dat", RTLD_GLOBAL|RTLD_LAZY);
	TEST_COND( (handle==NULL && dlerror()!=NULL), _L("dlopen() the unsupported file format..."));
		
	/*close the handle*/
	TInt err=dlclose(handle);
	TEST_COND((err!=0),_L("dlclose()..."));				

	/*close the handle*/
	 err=dlclose(handle1);
	TEST_COND((err!=0),_L("dlclose()..."));				
	
	SetTestStepResult(EPass);
	return TestStepResult(); 				
}

void ThreadFun(void)
	{
	/*open the dll*/
	void* handle=dlopen("Z:\\Sys\\Bin\\arithmeticoper1.dll", RTLD_LOCAL|RTLD_LAZY);

	/*close the handle*/
	dlclose(handle);

	return;	
 	}

TInt ThreadFunc(TAny* /*aParam*/)
	{
	// Create the thread's cleanup stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	ThreadFun();
	delete cleanup;
	return KErrNone;
	}
		
TVerdict CTestDlByName::TestMutexOnOpenHandle()
	{
	RThread rthread;	
	TRequestStatus status;
	
	_LIT(KTheKiller, "Killer");
	TBuf<50> threadName(KTheKiller);

	/*open the dll*/
	void* handle=dlopen("Z:\\Sys\\Bin\\arithmeticoper1.dll",  RTLD_LOCAL|RTLD_LAZY);
	TEST_COND( (handle!=NULL && dlerror()==NULL), _L("dlopen() with RTLD_GLOBAL..."));
		
	User::LeaveIfError(rthread.Create(threadName, ThreadFunc, KDefaultStackSize + 0x1000, NULL ,NULL, EOwnerProcess));	

	// Use the appropriate variant call to get the thread exit
	rthread.Logon(status);
	rthread.Resume();		

	User::After(TTimeIntervalMicroSeconds32(50));
	
	/*close the handle*/
	TInt err=dlclose(handle);
	TEST_VAL(err, 0, _L("dlclose()..."));
		
	SetTestStepResult(EPass);
	return TestStepResult(); 	
	}


/*
*	Testcase 	:	OpenNonStdDllOrdinalLookup
*	Desription	:	Opens a non STDDLL, tests ordinal lookup on that DLL
*	Return value:	KErrNone		
**/
TVerdict CTestDlByName::OpenNonStdDllOrdinalLookup()
	{
	void *mylib;
	int bAbs = 0, bStrcmp = 0;
    INFO_PRINTF1(_L("before dlopen\n"));
	mylib = dlopen("z:\\sys\\bin\\dll1.dll", RTLD_GLOBAL);
	
	// The function is abs()
	typedef float (*pAbs)(float);
	pAbs abs;
	
	abs = (pAbs)dlsym(mylib, "1");
	
	if(!abs)
		{
    	INFO_PRINTF1(_L("dlsym failed\n"));
		return (TVerdict) KErrGeneral;
		}
	
	// Invoke the function
	float i = (*abs)(-10);
	
	if(i >= 0)
		{
    	INFO_PRINTF1(_L("dlsym succeeded\n"));
		bAbs = 1;
		}
	
	// strcmp()
	typedef int (*pStrcmp)(const char *, const char *);
	pStrcmp strcmp;
	
	strcmp = (pStrcmp)dlsym(mylib, "2");;
	
	if(!strcmp)
		{
		return (TVerdict) KErrGeneral;
		}
	
	// Invoke strcmp()
	if(!(*strcmp)("string1","string1"))
		{
		bStrcmp = 1;
		}
	
	if(bAbs && bStrcmp)
		{
		dlclose(mylib);
		return (TVerdict) KErrNone;
		}
	else
		{
		return (TVerdict) KErrGeneral;
		}
	}


/*
*	Testcase 	:	OpenNonStdDllSymbolicLookup
*	Desription	:	Opens a non-STDDLL, tests symbolic lookup on that DLL
*	Return value:	KErrNone		
**/
TVerdict CTestDlByName::OpenNonStdDllSymbolicLookup()
	{
	__UHEAP_MARK;
	void *mylib;
	int bAbs = 0, bStrcmp = 0;
	mylib = dlopen("z:\\sys\\bin\\dll1.dll", RTLD_GLOBAL);
	
	// The function is abs()
	typedef float (*pAbs)(float);
	pAbs abs;
	
	abs = (pAbs)dlsym(mylib, "abs");
	
	if((abs == NULL) && (dlerror()!=NULL))
		{
    	INFO_PRINTF1(_L("dlsym failed"));
    	bAbs = 1;
		}
	
	// strcmp()
	typedef int (*pStrcmp)(const char *, const char *);
	pStrcmp strcmp;
	
	strcmp = (pStrcmp)dlsym(mylib, "strcmp");;
	
	if((strcmp == NULL) && (dlerror()!=NULL))
		{
		INFO_PRINTF1(_L("dlsym failed"));
		bStrcmp = 1;
		}
	
	if(bAbs && bStrcmp)
		{
		dlclose(mylib);
		return (TVerdict) KErrNone;
		}
	else
		{
		return (TVerdict) KErrGeneral;
		}
	__UHEAP_MARKEND;
	}


/*
 *	Testcase 	:	OpenNonStdDllSymbolicLookupWithRTLD_DEFAULT
 *	Desription	:	Opens a non-STDDLL, tests symbolic lookup on that DLL using RTLD_DEFAULT
 *	Return value:	KErrNone		
 **/
 TVerdict CTestDlByName::OpenNonStdDllSymbolicLookupWithRTLD_DEFAULT()
	 {
	 __UHEAP_MARK;
	 void *mylib;
	 int bAbs = 0, bStrcmp = 0;
	 mylib = dlopen("z:\\sys\\bin\\dll1.dll", RTLD_GLOBAL);
	 
	 // The function is abs()
	 typedef float (*pAbs)(float);
	 pAbs abs;
	 
	 abs = (pAbs)dlsym(RTLD_DEFAULT, "abs");
		
	 if((abs == NULL) && (dlerror()!=NULL))
		 {
		 INFO_PRINTF1(_L("dlsym failed"));
		 bAbs = 1;
		 }
		
	 // strcmp()
	 typedef int (*pStrcmp)(const char *, const char *);
	 pStrcmp strcmp;
		
	 strcmp = (pStrcmp)dlsym(RTLD_DEFAULT, "strcmp");;
		
	 if((strcmp == NULL) && (dlerror()!=NULL))
		 {
		 INFO_PRINTF1(_L("dlsym failed"));
		 bStrcmp = 1;
		 }
		
	 if(bAbs && bStrcmp)
		 {
		 dlclose(mylib);
		 return (TVerdict) KErrNone;
		 }
	 else
		 {
		 return (TVerdict) KErrGeneral;
		 }
	 __UHEAP_MARKEND;
	 }
