/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "tdb_blr.h"

const TInt KMaxParamLength=100;

//----------------------Local declaration of thread funcs-------------------

void* asctime_thread_1(void* aParam );
void* asctime_thread_2(void* aParam );
void* localtime_thread_1(void* aParam );
void* localtime_thread_2(void* aParam );
void* gmtime_thread_1(void* aParam );
void* gmtime_thread_2(void* aParam );
void* ctime_thread_1(void* aParam );
void* ctime_thread_2(void* aParam );

CTestDbBlr::~CTestDbBlr() 
	{ 
	}  

CTestDbBlr::CTestDbBlr(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestDbBlr::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iParamCnt=0;
	iRandomizerSeed = 1234567890;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestDbBlr::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestDbBlr::doTestStepL()
	{
	int err;

	if(TestStepName() == Kgetgrgid_pos)
		{
   		INFO_PRINTF1(_L("getgrgid_pos():"));
   		err = getgrgid_pos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetgrgid_neg)
		{
   		INFO_PRINTF1(_L("getgrgid_neg():"));
   		err = getgrgid_neg();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetgrnam_pos)
		{
   		INFO_PRINTF1(_L("getgrnam_pos():"));
   		err = getgrnam_pos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetgrnam_neg)
		{
   		INFO_PRINTF1(_L("getgrnam_neg():"));
   		err = getgrnam_neg();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetgrgid_r_pos)
		{
   		INFO_PRINTF1(_L("getgrgid_r_pos():"));
   		err = getgrgid_r_pos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetgrgid_r_neg)
		{
   		INFO_PRINTF1(_L("getgrgid_r_neg():"));
   		err = getgrgid_r_neg();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetgrnam_r_pos)
		{
   		INFO_PRINTF1(_L("getgrnam_r_pos():"));
   		err = getgrnam_r_pos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetgrnam_r_neg)
		{
   		INFO_PRINTF1(_L("getgrnam_r_neg():"));
   		err = getgrnam_r_neg();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetgrent_combined)
		{
   		INFO_PRINTF1(_L("getgrent_combined():"));
   		err = getgrent_combined();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Ksetgrent_combined1)
		{
   		INFO_PRINTF1(_L("setgrent_combined1():"));
   		err = setgrent_combined1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetpwuid_pos)
		{
   		INFO_PRINTF1(_L("getpwuid_pos():"));
   		err = getpwuid_pos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetpwnam_pos)
		{
   		INFO_PRINTF1(_L("getpwnam_pos():"));
   		err = getpwnam_pos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetpwuid_neg)
		{
   		INFO_PRINTF1(_L("getpwuid_neg():"));
   		err = getpwuid_neg();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetpwnam_neg)
		{
   		INFO_PRINTF1(_L("getpwnam_neg():"));
   		err = getpwnam_neg();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetpwuid_r_pos)
		{
   		INFO_PRINTF1(_L("getpwuid_r_pos():"));
   		err = getpwuid_r_pos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetpwnam_r_pos)
		{
   		INFO_PRINTF1(_L("getpwnam_r_pos():"));
   		err = getpwnam_r_pos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetpwuid_r_neg)
		{
   		INFO_PRINTF1(_L("getpwuid_r_neg():"));
   		err = getpwuid_r_neg();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetpwnam_r_neg)
		{
   		INFO_PRINTF1(_L("getpwnam_r_neg():"));
   		err = getpwnam_r_neg();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kendgrent_test1)
		{
   		INFO_PRINTF1(_L("endgrent_test1():"));
   		err = endgrent_test1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kgetpwent_test1)
		{
   		INFO_PRINTF1(_L("getpwent_test1():"));
   		err = getpwent_test1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Ksetpwent_test1)
		{
   		INFO_PRINTF1(_L("setpwent_test1():"));
   		err = setpwent_test1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kendpwent_test1)
		{
   		INFO_PRINTF1(_L("endpwent_test1():"));
   		err = endpwent_test1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	return TestStepResult(); 

	}
 
// -----------------------------------------------------------------------------
// Utility functions for testing.
// -----------------------------------------------------------------------------

TBool CTestDbBlr::SameString(char *aString1, char *aString2)
	{
	TInt i;
	
	for ( i=0; ( (aString1[i]!=0) && (aString2[i]!=0) && (aString2[i] == aString1[i]) ); i++)	{} // Go through the strings
	
	if ( aString2[i] != aString1[i] )	// If the scanning ended because of a difference between the strings
		{
		return EFalse;
		}
		
	else	// else
		{
		return ETrue;
		}
	
	}
	
void CTestDbBlr::StringToDes(char *aString, TDes &aDescriptor)
	{
	aDescriptor.Zero();
	
	for ( TInt i=0; aString[i]!=0; i++ )
		{
		aDescriptor.Append(aString[i]);
		}
		
	}
	
void CTestDbBlr::RandomizeString(char *aString, TInt aLength)
  {
  TInt16 integer;
  TInt i;
  
  for (i=0; i<aLength; i++)
    {
    Math::Int(integer, Math::FRand(iRandomizerSeed)*255);
    aString[i] = integer;
    }
	aString[i] = 0;
 	}

void CTestDbBlr::ReadStringParam(char* expected)
	{
	_LIT( KaKey, "Param%d" );
     TBuf<8> pNameBuf;
     TPtrC string;
     pNameBuf.Format(KaKey,++iParamCnt);
	 TBool res = GetStringFromConfig(ConfigSection(), pNameBuf, string);
	 if(!res)
	 	{
	 	_LIT(Kerr , "Unable to retrieve string parameter");
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
	
	
void CTestDbBlr::ReadIntParam(TInt &aInt)
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
// Test methods
// -----------------------------------------------------------------------------
//                  

TInt CTestDbBlr::getgrgid_pos()
    {
    //__UHEAP_MARK;
  	// Create temporary variables in stack
  	int gid;
  	TInt ret=KErrGeneral;
  	struct group* gr;
  	char gr_name[KMaxParamLength];
  	int gr_gid;
  	char gr_mem[KMaxParamLength];
  	// Read parameters
  	
    ReadIntParam(gid);
 	ReadStringParam(gr_name);
 	ReadIntParam(gr_gid);
 	ReadStringParam(gr_mem);
    // Do some testing
    gr=getgrgid(gid);

    // Set test result
    if((SameString(gr->gr_name,gr_name)) && (gr->gr_gid==gr_gid) && (SameString(gr->gr_mem[0],gr_mem)))
    {
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    }
    else
    {
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    }
    	
    free(gr->gr_mem);
    free(gr);
   // __UHEAP_MARKEND;
    return ret;
    }  
    
    
TInt CTestDbBlr::getgrgid_neg()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	int gid;
  	TInt ret=KErrGeneral;
  	struct group* gr;
  	// Read parameters
  	
    ReadIntParam(gid);
    // Do some testing
    gr=getgrgid(gid);

    // Set test result
	if(gr==NULL)       	
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    __UHEAP_MARKEND;
    return ret;
    }    
 
 
 
TInt CTestDbBlr::getgrnam_pos()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	struct group* gr;
  	char gr_name[KMaxParamLength];
  	int gr_gid;
  	char gr_mem[KMaxParamLength];
  	// Read parameters
  	
   	ReadStringParam(gr_name);
 	ReadIntParam(gr_gid);
 	ReadStringParam(gr_mem);
    // Do some testing
    gr=getgrnam(gr_name);

    // Set test result
    if((gr->gr_gid==gr_gid) && (SameString(gr->gr_mem[0],gr_mem)))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(gr->gr_mem);
    free(gr);
    __UHEAP_MARKEND;
    return ret;
    }  
    
    
TInt CTestDbBlr::getgrnam_neg()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	char gr_name[KMaxParamLength];
  	struct group* gr;
  	// Read parameters
  	
    ReadStringParam(gr_name);
    // Do some testing
    gr=getgrnam(gr_name);

    // Set test result
	if(gr==NULL)       	
     	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    
    __UHEAP_MARKEND;
    return ret;
    }  


TInt CTestDbBlr::getgrgid_r_pos()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	int gid;
  	struct group* gr=(struct group*)malloc(sizeof(struct group));
  	gr->gr_mem=(char**)malloc(sizeof(char**));
  	char* buffer=(char*)malloc(sizeof(struct group));
  	size_t bufsize=sizeof(buffer);
  	struct group* tempresult;
  	
  	char gr_name[KMaxParamLength];
  	int gr_gid;
  	char gr_mem[KMaxParamLength];
  	// Read parameters
  	
    ReadIntParam(gid);
 	ReadStringParam(gr_name);
 	ReadIntParam(gr_gid);
 	ReadStringParam(gr_mem);
 	
    // Do some testing
    int retval=getgrgid_r(gid,gr,buffer,bufsize,&tempresult);

    // Set test result
    if((SameString(gr->gr_name,gr_name)) && (gr->gr_gid==gr_gid) && (SameString(gr->gr_mem[0],gr_mem)))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(buffer);
    free(gr->gr_mem);
    free(gr);
    __UHEAP_MARKEND;
    return ret;
    }

TInt CTestDbBlr::getgrgid_r_neg()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	int gid;
  	struct group* gr=(struct group*)malloc(sizeof(struct group));
  	gr->gr_mem=(char**)malloc(sizeof(char**));
  	char* buffer=(char*)malloc(sizeof(struct group));
  	size_t bufsize=sizeof(buffer);
  	struct group* tempresult;
  	
  	// Read parameters
    ReadIntParam(gid);
 	
    // Do some testing
    int retval=getgrgid_r(gid,gr,buffer,bufsize,&tempresult);

    // Set test result
    if(tempresult==NULL)
    		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    
    free(buffer);
    free(gr->gr_mem);
    free(gr);
    __UHEAP_MARKEND;
    return ret;
    }


TInt CTestDbBlr::getgrnam_r_pos()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	struct group* gr=(struct group*)malloc(sizeof(struct group));
  	gr->gr_mem=(char**)malloc(sizeof(char**));
  	char* buffer=(char*)malloc(sizeof(struct group));
  	size_t bufsize=sizeof(buffer);
  	struct group* tempresult;
  	
  	char gr_name[KMaxParamLength];
  	int gr_gid;
  	char gr_mem[KMaxParamLength];
  	// Read parameters
  	
    ReadStringParam(gr_name);
 	ReadIntParam(gr_gid);
 	ReadStringParam(gr_mem);
 	
    // Do some testing
    int retval=getgrnam_r(gr_name,gr,buffer,bufsize,&tempresult);

    // Set test result
    if(gr->gr_gid==gr_gid && (SameString(gr->gr_mem[0],gr_mem)))
   		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    
    free(buffer);
    free(gr->gr_mem);
    free(gr);
    __UHEAP_MARKEND;
    return ret;
    
    }

TInt CTestDbBlr::getgrnam_r_neg()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	char gr_name[KMaxParamLength];
  	struct group* gr=(struct group*)malloc(sizeof(struct group));
  	gr->gr_mem=(char**)malloc(sizeof(char**));
  	char* buffer=(char*)malloc(sizeof(struct group));
  	size_t bufsize=sizeof(buffer);
  	struct group* tempresult;
  	
  	// Read parameters
    ReadStringParam(gr_name);
 	
    // Do some testing
    int retval=getgrnam_r(gr_name,gr,buffer,bufsize,&tempresult);

    // Set test result
    if(tempresult==NULL)
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(buffer);
    free(gr->gr_mem);
    free(gr);
    __UHEAP_MARKEND;
    return ret;
    }



TInt CTestDbBlr::getgrent_combined()
    {
    pthread_self();
    __UHEAP_MARK;
    
  	// Create temporary variables in stack
	TInt ret=KErrGeneral;
	int flag=0;
  	struct group* gr;
  	// Read parameters
    
    // Do some testing
    gr=getgrent();
    if(gr->gr_gid==0)
    	flag++;
    free(gr->gr_mem);
    free(gr);
    gr=getgrent();
    if(gr==NULL)
    	flag++;
    gr=getgrent();
    if(gr==NULL)
    	flag++;
    
    // Set test result
    if(flag==3)
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    __UHEAP_MARKEND;
    return ret;
    }

TInt CTestDbBlr::setgrent_combined1()
    {
    pthread_self();
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
	int flag=0;
  	struct group* gr;
  	// Read parameters
    
    // Do some testing
    gr=getgrent();
    if(gr->gr_gid==0)
    	flag++;
    free(gr->gr_mem);
    free(gr);
    setgrent();
    gr=getgrent();
    if(gr->gr_gid==0)
    	flag++;
    free(gr->gr_mem);
    free(gr);
    
    // Set test result
    if(flag==2)
    {
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    __UHEAP_MARKEND;
    
    return ret;
    }

TInt CTestDbBlr::endgrent_test1()
    {
  	// Create temporary variables in stack

  	struct group* gr=NULL;
  	TInt ret=KErrGeneral;
  	// Read parameters
    
    // Do some testing
    gr=getgrent();
    if(gr)
		{
		gr=NULL;
		gr=getgrent();
		if(gr)
			{
			gr=NULL;
			gr=getgrent();
			if(gr)
				{
				endgrent();
				setgrent();
				gr=NULL;
				gr=getgrent();
				if(gr)
					{
					setgrent();
					endgrent();
					gr=getgrent();
					if(gr)  ret = KErrNone;
					}
				}
			}
		}
		
    // Set test result
    if(ret==KErrNone)
	    {
	    INFO_PRINTF1(_L("Test passed."));	
	    }
    else
	    {
	    ERR_PRINTF1(_L("Test failed."));	
	    }
    return ret;
    }

TInt CTestDbBlr::getpwuid_pos()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	int uid;
  	struct passwd* pwd;
  	char pw_nam[KMaxParamLength];
  	char pw_dir[KMaxParamLength];
  	// Read parameters
  	
    ReadIntParam(uid);
    ReadStringParam(pw_nam);
    ReadStringParam(pw_dir);
 	// Do some testing
    pwd=getpwuid(uid);

    // Set test result
    if(SameString(pwd->pw_name,pw_nam))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(pwd->pw_dir);
    free(pwd);
    __UHEAP_MARKEND;
    return ret;
    }

TInt CTestDbBlr::getpwuid_neg()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	struct passwd* pwd;
  	int uid;
  	// Read parameters
  	
    ReadIntParam(uid);
    // Do some testing
    pwd=getpwuid(uid);

    // Set test result
	if(pwd==NULL)       	
       {
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    __UHEAP_MARKEND;
    return ret;
    }



TInt CTestDbBlr::getpwnam_pos()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	int uid;
  	struct passwd* pwd;
  	char pw_nam[KMaxParamLength];
  	char pw_dir[KMaxParamLength];
  	// Read parameters
  	
    ReadStringParam(pw_nam);
    ReadIntParam(uid);
    ReadStringParam(pw_dir);
 	// Do some testing
    pwd=getpwnam(pw_nam);

    // Set test result
    if(pwd->pw_uid==uid)
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(pwd->pw_dir);
    free(pwd);
    __UHEAP_MARKEND;
    return ret;
    } 
    
TInt CTestDbBlr::getpwnam_neg()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	struct passwd* pwd;
  	char pw_nam[KMaxParamLength];
  	// Read parameters
  	
    ReadStringParam(pw_nam);
    // Do some testing
    pwd=getpwnam(pw_nam);

    // Set test result
	if(pwd==NULL)       	
       {
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    __UHEAP_MARKEND;
    return ret;
    }

TInt CTestDbBlr::getpwuid_r_pos()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	int uid;
  	char pw_nam[KMaxParamLength];
  	char pw_dir[KMaxParamLength];
  	struct passwd* pwd=(struct passwd*)malloc(sizeof(struct passwd));
  	char* buffer=(char*)malloc(sizeof(struct passwd));
	struct passwd* tempresult;
	size_t bufsize=sizeof(buffer);
  	// Read parameters
  	
    ReadIntParam(uid);
    ReadStringParam(pw_nam);
    ReadStringParam(pw_dir);

 	// Do some testing
	setpwent();
	int retval=getpwuid_r(uid, pwd, buffer, bufsize, &tempresult);
	endpwent();

    // Set test result
    if((SameString(pwd->pw_name,pw_nam)))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(buffer);
    free(pwd->pw_dir);
    free(pwd);
    __UHEAP_MARKEND;
    return ret;
    }

TInt CTestDbBlr::getpwuid_r_neg()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	int uid;
  	struct passwd* pwd=(struct passwd*)malloc(sizeof(struct passwd));
  	char* buffer=(char*)malloc(sizeof(struct passwd));
	struct passwd* tempresult;
	size_t bufsize=sizeof(buffer);
  	// Read parameters
  	
    ReadIntParam(uid);

 	// Do some testing
	int retval=getpwuid_r(uid, pwd, buffer, bufsize, &tempresult);

    // Set test result
    if(tempresult==NULL)
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(buffer);
    free(pwd);
    __UHEAP_MARKEND;
    return ret;
    }

TInt CTestDbBlr::getpwnam_r_pos()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	int uid;
  	char pw_nam[KMaxParamLength];
  	char pw_dir[KMaxParamLength];
  	struct passwd* pwd=(struct passwd*)malloc(sizeof(struct passwd));
  	char* buffer=(char*)malloc(sizeof(struct passwd));
	struct passwd* tempresult;
	size_t bufsize=sizeof(buffer);
  	// Read parameters
  	
    ReadStringParam(pw_nam);
    ReadIntParam(uid);
    ReadStringParam(pw_dir);

 	// Do some testing
	int retval=getpwnam_r(pw_nam, pwd, buffer, bufsize, &tempresult);

    // Set test result
    if(pwd->pw_uid==uid)
    {
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(buffer);
    free(pwd->pw_dir);
    free(pwd);
    __UHEAP_MARKEND;
    return ret;
    }

TInt CTestDbBlr::getpwnam_r_neg()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	char pw_nam[KMaxParamLength];
  	struct passwd* pwd=(struct passwd*)malloc(sizeof(struct passwd));
  	char* buffer=(char*)malloc(sizeof(struct passwd));
	struct passwd* tempresult;
	size_t bufsize=sizeof(buffer);
  	// Read parameters
  	
    ReadStringParam(pw_nam);

 	// Do some testing
	int retval=getpwnam_r(pw_nam, pwd, buffer, bufsize, &tempresult);

    // Set test result
    if(tempresult==NULL)
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;	
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(buffer);
    free(pwd);
    __UHEAP_MARKEND;
    return ret;
    }

    
TInt CTestDbBlr::getpwent_test1()
    {
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;

  	struct passwd* pwd=NULL;
  	// Read parameters
    
    // Do some testing
    pwd=getpwent();
    if(!pwd)
    	{
    	ERR_PRINTF1(_L("Test failed."));
    	}
    else
    	{
    	pwd=getpwent();
	    if(!pwd)
	    	{
	    	ERR_PRINTF1(_L("Test failed."));
	    	}
	    else
	    	{
		    pwd=getpwent();
		    if(!pwd)
		    	{
		    	ERR_PRINTF1(_L("Test failed."));
		    	}
		    else
		    	{
		    	// Set test result
  				INFO_PRINTF1(_L("Test passed."));
  				ret = KErrNone;
		    	}
	    	}
    	}
    return ret;
    }


TInt CTestDbBlr::setpwent_test1()
    {
  	// Create temporary variables in stack
	struct passwd* pwd=NULL;
  	TInt ret=KErrGeneral;
  	// Read parameters
    
    // Do some testing
    pwd=getpwent();
    if(pwd)
	    {
	    pwd = NULL;
	    pwd=getpwent();
	    if(pwd)
		    {
		    pwd = NULL;
		    pwd=getpwent();
		    if(pwd)
			    {
			    setpwent();
			    setpwent();
			    pwd = NULL;
			    pwd=getpwent();
			    if(pwd)
				    {
				    setpwent();
				    pwd = NULL;
				    pwd=getpwent();
				    if(pwd)ret= KErrNone;
					}
			    }
		    }
	    }
	// Set test result
    if(ret==KErrNone)	
	    {
	    INFO_PRINTF1(_L("Test passed."));	
	    }
    else
	    {
	    ERR_PRINTF1(_L("Test failed."));		
	    }
    return ret;
    }
    
TInt CTestDbBlr::endpwent_test1()
    {
  	// Create temporary variables in stack

  	struct passwd* pwd=NULL;
  	TInt ret=KErrGeneral;
  	// Read parameters
    
    // Do some testing
    pwd=getpwent();
    if(pwd)
	    {
	    pwd = NULL;
	    pwd=getpwent();
	    if(pwd)
		    {
		    pwd = NULL;
		    pwd=getpwent();
		    if(pwd)
		    	{
			    endpwent();
			    setpwent();
			    pwd=NULL;
			    pwd=getpwent();
			    if(pwd)
				    {
				    setpwent();
				    endpwent();
				    pwd=getpwent();
				    if(pwd)
					    {
						ret = KErrNone;					    	
					    }
				    }
			    }
		    }
	    }
	// Set test result
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("Test passed."));
		}
	else
		{
		ERR_PRINTF1(_L("Test failed."));
		}
    return ret;
	}

