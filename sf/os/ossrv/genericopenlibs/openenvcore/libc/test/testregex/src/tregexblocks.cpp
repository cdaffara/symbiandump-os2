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



#include "tregex.h"
#include <locale.h>
#include "tregexdefines.h"


// -----------------------------------------------------------------------------
// CTestRegex::regcomp_escape
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CTestRegex::regcomp_escape(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp_escape"));
	
	TInt res=KErrGeneral;
	
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"a*\\",REG_EXTENDED);
	
	if(ret==REG_EESCAPE)
		{
		res=KErrNone;
		}
	
	return res;

    }
    
// -----------------------------------------------------------------------------
// CTestRegex::regcomp_badrpt
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestRegex::regcomp_badrpt(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp_badrpt"));
	
	TInt res=KErrGeneral;
	
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"(?",REG_EXTENDED);
	
	if(ret==REG_BADRPT)
		{
		res=KErrNone;
		}
	return res;

    }
    
// -----------------------------------------------------------------------------
// CTestRegex::recomp_eparen
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regcomp_eparen(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regexec_eparen"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"(a*))",REG_EXTENDED|REG_NOTEOL);
		
	if(ret==REG_EPAREN)
		{
		res=KErrNone;
		}
	return res;
	
    }

// -----------------------------------------------------------------------------
// CTestRegex::regcomp_badrpt
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestRegex::regcomp_ebrack(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp_badpat"));
	
	TInt res=KErrGeneral;
	
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[]",REG_EXTENDED);
	
	if(ret==REG_EBRACK)
		{
		res=KErrNone;
		}
	return res;

    }



// -----------------------------------------------------------------------------
// CTestRegex::regcomp_basic
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regcomp_basic(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp_basic"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[a*][b*]",REG_EXTENDED);
	int match=regexec(&re,"aaabb",0,NULL,0);
	
	if(!ret && !match)
		{
		res=KErrNone;
		regfree(&re);
		}
	return res;
	
    }

// -----------------------------------------------------------------------------
// CTestRegex::regcomp_complex
// Tests with complex regular expressions
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TVerdict CTestRegex::regcomp_complex(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp_complex"));
	SetTestStepResult(EPass);
	int ret=0;
	regex_t re;
	char * locale=setlocale(LC_COLLATE,NULL);
	ret=regcomp(&re,IPEXPR,REG_EXTENDED);
	if(!ret)
    regfree(&re);
	else
	SetTestStepResult(EFail);
	ret=regcomp(&re,EMEXPR,REG_EXTENDED);
	if(!ret)
    regfree(&re);
	else
	SetTestStepResult(EFail);
	ret=regcomp(&re,EMEXPR1,REG_EXTENDED);
	if(ret)
	regfree(&re);
	else
	SetTestStepResult(EFail);
	ret=regcomp(&re,EMEXPR2,REG_EXTENDED);
	if(!ret)
    regfree(&re);
	else
	SetTestStepResult(EFail);
	ret=regcomp(&re,EMEXPR3,REG_EXTENDED);
	if(ret)
	regfree(&re);
	else
	SetTestStepResult(EFail);
	ret=regcomp(&re,EMEXPR4,REG_EXTENDED);
	if(!ret)
    regfree(&re);
	else
	SetTestStepResult(EFail);
	ret=regcomp(&re,EMEXPR5,REG_EXTENDED);
	if(ret)
	regfree(&re);
	else
	SetTestStepResult(EFail);
	ret=regcomp(&re,EMEXPR6,REG_EXTENDED);
	if(!ret)
    regfree(&re);
	else
	SetTestStepResult(EFail);
	ret=regcomp(&re,EMEXPR7,REG_EXTENDED);
	if(!ret)
    regfree(&re);
	else
	SetTestStepResult(EFail);
	ret=regcomp(&re,IPEXPR,REG_NEWLINE);
	if(!ret)
    regfree(&re);
	else
	SetTestStepResult(EFail);
	char* locale1=setlocale(LC_CTYPE,"en_US.UTF-8");
	ret=regcomp(&re,EMEXPR7,REG_EXTENDED);
	int match=regexec(&re,"http://www.google.com",0,NULL,0);
	if(!ret && match)
	regfree(&re);
	else
	SetTestStepResult(EFail);
	return TestStepResult();  
	
    }
// -----------------------------------------------------------------------------
// CTestRegex::regexec_basic
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regexec_basic(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regexec_brack"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"b[a*]",REG_EXTENDED);
	int match=regexec(&re,"cc",0,NULL,0);
	
	if(!ret && (match==REG_NOMATCH))
		{
		res=KErrNone;
		regfree(&re);
		}
	return res;
	
    }
	
// -----------------------------------------------------------------------------
// CTestRegex::regexec_icase
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regexec_icase(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regexec_icase"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[a*]",REG_EXTENDED|REG_ICASE);
	int match=regexec(&re,"AA",0,NULL,0);
	
	if(!ret && !match)
		{
		res=KErrNone;
		regfree(&re);
		}
	return res;
	
    }

// -----------------------------------------------------------------------------
// CTestRegex::regexec_notbol
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regexec_notbol(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regexec_notbol"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[a^]",REG_EXTENDED|REG_NOTBOL);
	int match=regexec(&re,"ca",0,NULL,0);
	
	if(!ret && !match) 
		{
		res=KErrNone;
		regfree(&re);
		}
	return res;
	
    }

// -----------------------------------------------------------------------------
// CTestRegex::regexec_noteol
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regexec_range(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regexec_notbol"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[a-c]",REG_EXTENDED|REG_NOTEOL);
	int match=regexec(&re,"ca",0,NULL,0);
	
	if(!ret && !match) 
		{
		res=KErrNone;
		regfree(&re);
		}
	return res;
	
    }
    
// -----------------------------------------------------------------------------
// CTestRegex::regexec_noteol
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regexec_noteol(  )
    {
	INFO_PRINTF1(_L("CTestRegex::regexec_notbol"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[a$]",REG_EXTENDED|REG_NOTEOL);
	int match=regexec(&re,"ca",0,NULL,0);
	
	if(!ret && !match) 
		{
		res=KErrNone;
		regfree(&re);
		}
	return res;
    }


// -----------------------------------------------------------------------------
// CTestRegex::regexec_noteol
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regfree_basic(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regfree_basic"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"(a*)",REG_EXTENDED|REG_NOTEOL);
	int match=regexec(&re,"aa",0,NULL,0);
	
	if(!ret && !match) 
		{
		res=KErrNone;
		}
	regfree(&re);
	
	return res;
	
    }

// -----------------------------------------------------------------------------
// CTestRegex::regexec_noteol
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regfree_multiple(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regfree_basic"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"(a*)",REG_EXTENDED|REG_NOTEOL);
	int match=regexec(&re,"aa",0,NULL,0);
	
	if(!ret && !match) 
		{
		res=KErrNone;
		}
	regfree(&re);
	
	return res;
	
    }
    
// -----------------------------------------------------------------------------
// CTestRegex::regexec_noteol
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CTestRegex::regerror_basic(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regfree_basic"));
	
	TInt res=KErrGeneral;
	int ret=0;
	char errbuf[5];
	regex_t re;
	
	ret=regcomp(&re,"[]",REG_EXTENDED|REG_NOTEOL);
	int match=regerror(ret,&re,errbuf,sizeof(errbuf));
	
	if(match==28) 
		{
		res=KErrNone;
		}
	regfree(&re);
		
	return res;
	
    }

// -----------------------------------------------------------------------------
// CTestRegex::regcomp1
// To Cover  p_b_symbol() and p_b_coll_elem()
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestRegex::regcomp1(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp1"));
	
	TInt res=KErrGeneral;
	
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[[-*a[*b",REG_EXTENDED|REG_ICASE);
	
	if(ret==REG_EBRACK)
		{
		res=KErrNone;
		}
	regfree(&re);
	return res;

    }
    
// -----------------------------------------------------------------------------
// CTestRegex::regcomp2
// To Cover p_b_eclass() and p_b_coll_elem()
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestRegex::regcomp2(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp2"));
	
	TInt res=KErrGeneral;
	
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[[=*a][*b]",REG_EXTENDED|REG_ICASE);
		
	if(ret==REG_EBRACK)
		{
		res=KErrNone;
		}
	regfree(&re);
	return res;

    }
    
// -----------------------------------------------------------------------------
// CTestRegex::regcomp3
// To Cover for p_b_cclass()
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestRegex::regcomp3(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp3"));
	
	TInt res=KErrGeneral;
	
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[[:*a][*b]",REG_EXTENDED|REG_ICASE);
		
	if(ret==REG_ECTYPE)
		{
		res=KErrNone;
		}
	regfree(&re);
	return res;

    }

// for coverage improvement 

TInt CTestRegex::regcomp_nospec(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp_basic1"));
	
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"a*\\",REG_NOSPEC);
	if(ret==0)
		{
		regfree(&re);
		return KErrNone;
		}
	regfree(&re);
	return KErrGeneral;
	
    }


TInt CTestRegex::regcomp_icase2(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regcomp_basic2"));
	
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"[a*][b*]",REG_ICASE);
	int match=regexec(&re,"aaabb",0,NULL,0);
	
	if(!ret && !match)
		{
		regfree(&re);
		return KErrNone;
		}
	regfree(&re);
	return KErrGeneral;
	
    }
    
TInt CTestRegex::regcomp_newline(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regexec_eparen"));
	
	TInt res=KErrGeneral;
	int ret=0;
	regex_t re;
	
	ret=regcomp(&re,"(.",REG_NEWLINE);
		
	if(ret==0)
		{
		res=KErrNone;
		}
	regfree(&re);
	return res;
	
    }
    /*
    *Testcase Name:regerror_basic1
    *API		  :regerror() + regatoi()
    *Description  :regerror convert regular expression errors to messages.
	*The return value of regerror() is the length of this error message.
	*/

    
 TInt CTestRegex::regerror_basic1(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regfree_basic1"));
	
	int ret;
    TPtrC string;
    char *param=0;
    char errbuf[5];
    TBuf8<256> bufstring;
    
    _LIT( Kret, "Param%d" );
	TBuf<8> pNameBuf1,pNameBuf2;
	pNameBuf1.Format(Kret,++iParamCnt);
    GetIntFromConfig(ConfigSection(), pNameBuf1, ret );
    
   _LIT( Kstring, "Param%d" );
	pNameBuf2.Format(Kstring,++iParamCnt);
	GetStringFromConfig(ConfigSection(), pNameBuf2, string);
	bufstring.Copy(string);
	TInt paramLength=string.Length();
    char* text=(char *)(bufstring.Ptr());
    *(text+paramLength)='\0';
    param=(char *)malloc(sizeof(paramLength+1));
    strcpy(param,text);
	TInt res=KErrGeneral;
	regex_t re;
	re.re_endp=param;
   	int len=regerror(ret,&re,errbuf,sizeof(errbuf));
   	if(len==2)
	   	{
	   	res = KErrNone;
	   	}
	else
		{
		res = KErrGeneral;
		}
		
	free(param);
	regfree(&re);
	return res;	
    }
    
    
   /*
   *--------------------------------------------------------------
   *Testcase Name:regfree_multiple1
   *To cover fast(),slow(),dissect()
   *--------------------------------------------------------------
   */   
    
   TInt CTestRegex::regfree_multiple1(  )
    {

	INFO_PRINTF1(_L("CTestRegex::regfree_multiple1"));
	
	TInt res=KErrGeneral;
	TPtrC string1,string2;
    char *param1=0,*param2=0;
    TBuf8<256> bufstring;
    TBuf<8> pNameBuf1,pNameBuf2;
    
    _LIT( Kstring1, "Param%d" );
	pNameBuf1.Format(Kstring1,++iParamCnt);
	GetStringFromConfig(ConfigSection(), pNameBuf1, string1);
	bufstring.Copy(string1);
	TInt paramLength1=string1.Length();
    char* text1=(char *)(bufstring.Ptr());
    *(text1+paramLength1)='\0';
    param1=(char *)malloc(sizeof(paramLength1+1));
    strcpy(param1,text1);
        
   _LIT( Kstring2, "Param%d" );
	pNameBuf2.Format(Kstring2,++iParamCnt);
	GetStringFromConfig(ConfigSection(), pNameBuf2, string2);
	bufstring.Copy(string2);
	TInt paramLength2=string2.Length();
    char* text2=(char *)(bufstring.Ptr());
    *(text2+paramLength2)='\0';
    param2=(char *)malloc(sizeof(paramLength2+1));
    strcpy(param2,text2);
    
	regex_t re;
	regmatch_t pmatch[2];
	int ret = regcomp(&re,param1,REG_EXTENDED|REG_NOTEOL);
	int match = regexec(&re,param2,2,pmatch,0);
	
	if(!ret && !match) 
		{
		res=KErrNone;
		}
	free(param1);
	free(param2);
	regfree(&re);
	return res;
	}


    
