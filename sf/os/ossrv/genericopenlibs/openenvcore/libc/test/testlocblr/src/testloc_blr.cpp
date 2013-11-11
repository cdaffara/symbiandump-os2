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



#include "testloc_blr.h"
#include <e32math.h>
#include <e32des16.h>
#include <e32std.h>
#include <e32std_private.h>

#define SUPPORTED_LOCALES 104
#define SUPPORTED_CHARSETS 100
#define MAX_SIZE 50
const TInt KMaxParamLength=100;

struct localeStrings
	{
	char localeName[50];
	TInt flag;		
	};

struct charconvStrings
	{
	char charsetName[30];
	};
struct localeStrings locales[SUPPORTED_LOCALES];
struct charconvStrings charsets[SUPPORTED_CHARSETS];


CTestLoc_Blr::~CTestLoc_Blr() 
	{ 
	}  

CTestLoc_Blr::CTestLoc_Blr(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	iParamCnt = 0;
	SetTestStepName(aStepName);		
	}

TVerdict CTestLoc_Blr::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLoc_Blr::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTestLoc_Blr::doTestStepL()
	{
	int err;
	if(TestStepName() == KTestNl_Langinfo_Pass)
		{
   		INFO_PRINTF1(_L("TestNl_Langinfo_Pass():"));
   		err = TestNl_Langinfo_Pass();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   if(TestStepName() == KTestIconv_Pass)
		{
   		INFO_PRINTF1(_L("TestIconv_Pass():"));
   		err = TestIconv_Pass();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	if(TestStepName() == KTestLocaleConv)
		{
   		INFO_PRINTF1(_L("TestLocaleConv():"));
   		err = TestLocaleConv();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	if(TestStepName() == KTestLocale)
		{
   		INFO_PRINTF1(_L("TestSetLocale():"));
   		err = TestSetLocale();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	if(TestStepName() == KTestIconv_Open_Pass)
		{
   		INFO_PRINTF1(_L("TestIconv_Open_Pass():"));
   		err = TestIconv_Open_Pass();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	if(TestStepName() == KTestIconv_Open_Fail)
		{
   		INFO_PRINTF1(_L("TestIconv_Open_Fail():"));
   		err = TestIconv_Open_Fail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	INFO_PRINTF1(_L("End Of test:"));	
   	return TestStepResult(); 

	}

// -----------------------------------------------------------------------------
//Function Name :ReadLangParam
//Description:Reads the LANG parameter from ini file
//param : string to receive the read value
// -----------------------------------------------------------------------------
void CTestLoc_Blr::ReadLangParam(char* aString)
	{
	_LIT( Kparam, "LANG" );
	TPtrC descriptor;
	TInt i=0;
	TBool ret = GetStringFromConfig(ConfigSection(), Kparam, descriptor);
	INFO_PRINTF2(_L("Descriptor:"), &descriptor);
	if ( descriptor == _L("\"\"") )
		{
	  	i = 0;
	  	}
	else
		{
		// If the string is quoted, take only the insides
		if ((descriptor[0] == '\"') && (descriptor[descriptor.Length()-1] == '\"'))
	    	{
	    	for ( i=0; i<descriptor.Length()-2; i++ )
    	  		{
        		aString[i]=descriptor[i+1];
        		}
      		}
   		 // Otherwise,take the whole string
    	else
	      	{
		    for ( i=0; i<descriptor.Length(); i++ )
	    	  	{
	        	aString[i]=descriptor[i];
	        	}
	      	}
    	}
    
	aString[i]='\0';
	INFO_PRINTF1(_L("string read complete:"));
	}

// -----------------------------------------------------------------------------
//Function Name :ReadCodeParam
//Description:Reads the CODE parameter from ini file
//param : string to receive the read value
// -----------------------------------------------------------------------------	
void CTestLoc_Blr::ReadCodeParam(char* aString)
	{
	_LIT( Kparam, "CODE" );
    TPtrC descriptor;
  	TInt i;
	
	TBool ret = GetStringFromConfig(ConfigSection(), Kparam, descriptor);
	if ( descriptor == _L("\"\"") )
		{
	  	i = 0;
	  	}
	else
		{
		// If the string is quoted, take only the insides
		if ((descriptor[0] == '\"') && (descriptor[descriptor.Length()-1] == '\"'))
	    	{
	    	for ( i=0; i<descriptor.Length()-2; i++ )
    	  		{
        		aString[i]=descriptor[i+1];
        		}
      		}
   		 // Otherwise,take the whole string
    	else
	      	{
		    for ( i=0; i<descriptor.Length(); i++ )
	    	  	{
	        	aString[i]=descriptor[i];
	        	}
	      	}
    	}
    
		aString[i]='\0';
	}

// -----------------------------------------------------------------------------
//Function Name :ReadExpStringParam
//Description : Reads the RxpParm parameter from ini file
// -----------------------------------------------------------------------------
void CTestLoc_Blr::ReadExpStringParam(char* aString)
	{
	_LIT( Kparam, "ExpParam%d" );
    TBuf<100> pNameBuf;
  	TPtrC descriptor;
  	TInt i;
	
	pNameBuf.Format(Kparam,iParamCnt);
	TBool ret = GetStringFromConfig(ConfigSection(), pNameBuf, descriptor);
	if ( descriptor == _L("\"\"") )
		{
	  	i = 0;
	  	}
	else
		{
		// If the string is quoted, take only the insides
		if ((descriptor[0] == '\"') && (descriptor[descriptor.Length()-1] == '\"'))
	    	{
	    	for ( i=0; i<descriptor.Length()-2; i++ )
    	  		{
        		aString[i]=descriptor[i+1];
        		}
      		}
   		 // Otherwise,take the whole string
    	else
	      	{
		    for ( i=0; i<descriptor.Length(); i++ )
	    	  	{
	        	aString[i]=descriptor[i];
	        	}
	      	}
    	}
    
		aString[i]='\0';
	}

// -----------------------------------------------------------------------------
//Function Name :ReadStringParam
//Description:Reads string from ini file
//Param : aString is populated with the read string
// -----------------------------------------------------------------------------	
void CTestLoc_Blr::ReadStringParam(char* aString)
	{
	_LIT( Kparam, "Param%d" );
    TBuf<100> pNameBuf;
  	TPtrC descriptor;
  	TInt i;
	
	pNameBuf.Format(Kparam,++iParamCnt);
	TBool ret = GetStringFromConfig(ConfigSection(), pNameBuf, descriptor);
	if ( descriptor == _L("\"\"") )
		{
	  	i = 0;
	  	}
	else
		{
		// If the string is quoted, take only the insides
		if ((descriptor[0] == '\"') && (descriptor[descriptor.Length()-1] == '\"'))
	    	{
	    	for ( i=0; i<descriptor.Length()-2; i++ )
    	  		{
        		aString[i]=descriptor[i+1];
        		}
      		}
   		 // Otherwise,take the whole string
    	else
	      	{
		    for ( i=0; i<descriptor.Length(); i++ )
	    	  	{
	        	aString[i]=descriptor[i];
	        	}
	      	}
    	}
    
		aString[i]='\0';
   	}
   	
// -----------------------------------------------------------------------------
//Function Name : ReadIntParam
//TestCase Description:Reads Int value from ini file
//Param : TInt to receive the read integer 
// -----------------------------------------------------------------------------
void CTestLoc_Blr::ReadIntParam(TInt &aInt)
	{
	_LIT( Kparam, "Param%d" );
    TBuf<8> pNameBuf;
    TPtrC string;
    pNameBuf.Format(Kparam,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aInt);
	}

// -----------------------------------------------------------------------------
//Function Name :SameString
//Description:Compares the two strings passed to it
//
//return: ETrue if the strings match , EFalse otherwise
// -----------------------------------------------------------------------------	
TBool CTestLoc_Blr::SameString(char *aString1, char *aString2)
	{
	TInt i;
	TInt len = strlen(aString1);

	for ( i=0; ( (aString1[i]!=0) && (aString2[i]!=0) && (aString2[i] == aString1[i]) ); i++)	{} // Go through the strings
	
	if ( aString2[i] != aString1[i] )	// If the scanning ended because of a difference between the strings
		{
		return EFalse;
		}
	else	
		{
		return ETrue;
		}
	} 

// -----------------------------------------------------------------------------
//Function Name :TestSetLocaleL
//Description:Tests Locale setting with parameters read from ini file
//return: return value of the operations performed
// -----------------------------------------------------------------------------	
TInt  CTestLoc_Blr::TestSetLocale()
    {
  	// Create temporary variables in stack
  	TInt retVal = KErrGeneral;
  	char localeName[KMaxParamLength];
  	char categoryName[KMaxParamLength];
  	char exptOutput[KMaxParamLength];
  	
  	char* output = NULL;
  	
  	// Read parameters
  	ReadStringParam(categoryName);
    ReadStringParam (localeName);
    ReadStringParam (exptOutput);
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(categoryName);
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(categoryName[j]);
    	}
    	
    TBuf<MAX_SIZE> buf1;
    len = strlen(localeName);
    for (TInt j =0; j<len;j++)
    	{
    	buf1.Append(localeName[j]);
    	}
    	
    INFO_PRINTF1(_L("\r\n"));
    INFO_PRINTF3(_L("Testing %S  for %S category"), &buf,&buf1);
    int	category = LC_ALL;
    if(SameString("LC_ALL",categoryName))
    	{
    	category = LC_ALL;
    	}
    if(SameString("LC_CTYPE",categoryName))
    	{
    	category = LC_CTYPE;
    	}
	if(SameString("LC_COLLATE",categoryName))
		{
		category = LC_COLLATE;
		}
	if(SameString("LC_MONETARY",categoryName))
		{
		category = LC_MONETARY;		 
		}
	if(SameString("LC_NUMERIC",categoryName))
		{
		category = LC_NUMERIC;   
		}
	if(SameString("LC_TIME",categoryName))
		{
		category = LC_TIME;    	
		}
    // Do some testing
    
    
    TInt j = 0;
	TInt flag = 0;
    while(j < SUPPORTED_LOCALES)
    	{
    	if(SameString(locales[j].localeName,localeName) && locales[j].flag)
    		{
    		flag = 1;
			break;	    						
    		}
    	j++;
    	}
    
    
	if(SameString("NULL",localeName))
		{
		output=setlocale(category,NULL);	
		}
	
    else if(SameString("C",localeName) || SameString("POSIX",localeName) || SameString("",localeName) )
    	{
    	output=setlocale(category,localeName);
    	}
    else
    	{
    	if(flag)
    		{
    		output=setlocale(category,localeName);	
    		}
    	else
	    	{
	    	INFO_PRINTF2(_L("%S Locale not Supported :: Test passed.\t\t"), &buf1);
			retVal = KErrNone;
			return KErrNone;
	    	}
		}
    
	if(output!=NULL && SameString(output,exptOutput))
    	{
		INFO_PRINTF1(_L("Test passed."));
		retVal = KErrNone;
    	}
	else if(output==NULL &&  SameString("NULL",exptOutput))
		{
		INFO_PRINTF1(_L("Test passed."));
		retVal = KErrNone;
		}
	else
		{
		ERR_PRINTF1(_L("Test failed."));
		TBuf<MAX_SIZE> buf1;
     	TInt len = strlen(exptOutput);
     	for (TInt j =0; j<len;j++)
     		{
     		buf1.Append(exptOutput[j]);
     		}
		INFO_PRINTF2(_L("API Output = NULL\t Expected Output = %S\n"), &buf1);
		retVal = KErrGeneral;
		}	
    
    // Set test result
    return retVal;
	} 

// -----------------------------------------------------------------------------
//Function Name :TestNl_Langinfo_PassL
//Description:Tests Locale setting of non-local languages with parameters read from ini file
//return: return value of the operations performed
// ---------------------------------------------------------------------------
TInt  CTestLoc_Blr::TestNl_Langinfo_Pass()
    {
     TInt ret = KErrGeneral;
	 // Create temporary variables in stack
	 char localeName[KMaxParamLength];
	 char* locale = NULL;
	  
	 // Read parameters
   	 ReadLangParam(localeName);
   	 TBuf<MAX_SIZE> buf1;
     TInt len = strlen(localeName);
     for (TInt j =0; j<len;j++)
     	{
     	buf1.Append(localeName[j]);
     	}
   	 
   	 INFO_PRINTF2(_L("Testing nl_langinfo() API for %S "), &buf1);	    
    
     TInt j = 0;
	 TInt flag = 0;
     while(j < SUPPORTED_LOCALES)
    	{
    	if(SameString(locales[j].localeName,localeName) && locales[j].flag)
    		{
    		flag = 1;
			break;	    						
    		}
    		j++;
    	}
	    
	if(SameString("C",localeName) || SameString("POSIX",localeName) || SameString("",localeName) )
		{
		locale=setlocale(LC_ALL,localeName);
		}
	
    else
    {
       	j = 0;
    	TInt charsetFlag = 0;
    	char* srcCharset =  strchr(localeName, '.');
    	srcCharset++;
    	while(j < SUPPORTED_CHARSETS)
    		{
	    	if(SameString(charsets[j].charsetName,srcCharset))
	    		{
	    		charsetFlag = 1;
	    		break;
	    		}
	    		j++;
    		}
    	if(flag && charsetFlag)
    		{
    		locale=setlocale(LC_ALL,localeName);	
    		}
    	else
	    	{
	    	INFO_PRINTF1(_L("Test passed."));
	    	
	    	TBuf<MAX_SIZE> buf;
     		len = strlen(srcCharset);
     		for (TInt j =0; j<len;j++)
     			{
     			buf.Append(*(srcCharset+j));
     			}
	    	if(!flag)
	    		{
	    		INFO_PRINTF2(_L("%S Locale is not Supported :: Test passed."), &buf1);
	    		}
	    	if(!charsetFlag)
	    		{
	    		INFO_PRINTF2(_L("%S Charset is not Supported :: Test passed."), &buf);	    	
	    		}
			ret = KErrNone;
			return KErrNone;
	    	}
    	}
    	
    if(locale)
    	{
    	TInt retVal = Langinfo();
    	if(KErrNone == retVal)
	    	{
	    	INFO_PRINTF1(_L("Test passed."));
	    	ret = KErrNone;
	    	}
	    else
	    	{
	    	ERR_PRINTF1(_L("Test failed."));	
	    	ret = KErrGeneral;
	    	}
	   	}
    else
    	{
    	ERR_PRINTF1(_L("Locale setting failed .Test failed."));    
    	ret = KErrGeneral;
    	}
    // Set test result
    return ret;
     }  


TInt CTestLoc_Blr::TestIconv_Open_Pass()
	{
	char srcCharset[KMaxParamLength];
	char dstCharset[KMaxParamLength];
	TInt retVal = KErrGeneral;
		
	ReadStringParam (srcCharset);
	ReadStringParam (dstCharset);
	
	TBuf<MAX_SIZE> buf;
    TInt len = strlen(srcCharset);
    for (TInt j =0; j<len;j++)
   		{
    	buf.Append(srcCharset[j]);
     	}
    
    TBuf<MAX_SIZE> buf1;
    len = strlen(dstCharset);
    for (TInt j =0; j<len;j++)
   		{
    	buf1.Append(dstCharset[j]);
     	}
	INFO_PRINTF3(_L("Conversion from %S to %S"), &buf, &buf1);
	TInt srcFlag = 0;
	TInt dstFlag = 0;
	TInt j = 0;
    while(j < SUPPORTED_CHARSETS)
    	{
    	if(SameString(charsets[j].charsetName,srcCharset))
    		{
    		srcFlag = 1;
    		}
    	if(SameString(charsets[j].charsetName,dstCharset))
    		{
    		dstFlag = 1;
    		}
    	if(srcFlag && dstFlag)
    		{
    		break;
    		}
    	j++;
    	}
	
	if(srcFlag && dstFlag)
		{
		errno = 0;
	
		iconv_t cd = iconv_open(dstCharset, srcCharset);
		if((errno != EINVAL) && (cd != (iconv_t) -1))
			{
			INFO_PRINTF1(_L("Test passed."));	
			retVal =  KErrNone;
			}
		else
			{
			ERR_PRINTF1(_L("Test Failed."));	
			retVal =  KErrGeneral;
			}
		iconv_close(cd);	
		}
	else
		{
		retVal =  KErrNone;
		if(!srcFlag)
			{
			TBuf<MAX_SIZE> buf;
     		TInt len = strlen(srcCharset);
     		for (TInt j =0; j<len;j++)
     			{
     			buf.Append(srcCharset[j]);
     			}
			INFO_PRINTF2(_L("%S Character set not found :Test passed."), &buf);
			}
		if(!dstFlag)
			{
			TBuf<MAX_SIZE> buf;
     		TInt len = strlen(dstCharset);
     		for (TInt j =0; j<len;j++)
     			{
     			buf.Append(dstCharset[j]);
     			}
			INFO_PRINTF2(_L("%S Character set not found :Test passed.\r\n"), &buf);
			}
		}
	return retVal;
	}

TInt CTestLoc_Blr::TestIconv_Open_Fail()
	{
	TInt retVal = KErrGeneral;
	char srcCharset[KMaxParamLength];
	char dstCharset[KMaxParamLength];
	
	ReadStringParam (srcCharset);
	ReadStringParam (dstCharset);
	
	TBuf<MAX_SIZE> buf;
    TInt len = strlen(srcCharset);
    for (TInt j =0; j<len;j++)
   		{
    	buf.Append(srcCharset[j]);
     	}
    
    TBuf<MAX_SIZE> buf1;
    len = strlen(dstCharset);
    for (TInt j =0; j<len;j++)
   		{
    	buf1.Append(dstCharset[j]);
     	}
	INFO_PRINTF3(_L("Conversion from %S to %S"), &buf, &buf1);

	errno = 0;

	iconv_t cd = iconv_open(dstCharset, srcCharset);
	if((errno == EINVAL) && (cd == (iconv_t) -1))
		{
		INFO_PRINTF1(_L("Test Passed"));
		retVal =  KErrNone;
		}
	else
		{
		ERR_PRINTF1(_L("Test Failed"));
		retVal =  KErrGeneral;
		}
	iconv_close(cd);	
	return retVal;
	}


TInt CTestLoc_Blr::TestIconv_Pass()
	{
	INFO_PRINTF1(_L("Reading from ini"));
	TInt retVal = KErrGeneral;
	char srcCharset[KMaxParamLength];
	char dstCharset[KMaxParamLength];
	char inputString[KMaxParamLength];
	char outputString[KMaxParamLength];
	char errorString[KMaxParamLength];
	TInt inputBytes;
	TInt inBytesAfterConversion;
	TInt outputBytes;
	TInt outBytesAfterConversion;
	TInt errNo = 0;
	TInt ret;
	
	ReadStringParam (srcCharset);
	ReadStringParam (dstCharset);
	ReadStringParam (inputString);
	ReadStringParam (outputString);
	ReadIntParam(inputBytes);
	ReadIntParam(inBytesAfterConversion);
	ReadIntParam(outputBytes);
	ReadIntParam(outBytesAfterConversion);
	ReadStringParam(errorString);
	ReadIntParam(ret);
	
	INFO_PRINTF1(_L("Operation 2"));
	if(SameString("0",errorString))
		{
		errNo = 0;
		}
	if(SameString("EINVAL",errorString))
		{
		errNo = EINVAL;
		}
	if(SameString("EILSEQ",errorString))
		{
		errNo = EILSEQ;
		}
	if(SameString("E2BIG",errorString))
		{
		errNo = E2BIG;
		}
	
	char inputStringBeforeConvertion[KMaxParamLength];
	ConvertHexTochar(inputStringBeforeConvertion, inputString);
	
	TBuf<MAX_SIZE> buf;
    TInt len = strlen(srcCharset);
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(srcCharset[j]);
    	}
    	
    TBuf<MAX_SIZE> buf1;
    len = strlen(dstCharset);
    for (TInt j =0; j<len;j++)
    	{
    	buf1.Append(dstCharset[j]);
    	}
	
	INFO_PRINTF3(_L("Conversion from %S to %S"), &buf, &buf1);
	TInt srcFlag = 0;
	TInt dstFlag = 0;
	TInt j = 0;
    while(j < SUPPORTED_CHARSETS)
    	{
    	if(SameString(charsets[j].charsetName,srcCharset))
    		{
    		srcFlag = 1;
    		}
    	if(SameString(charsets[j].charsetName,dstCharset))
    		{
    		dstFlag = 1;
    		}
    	if(srcFlag && dstFlag)
    		{
    		break;
    		}
    	j++;
    	}
    if(srcFlag && dstFlag)
    	{
    	errno = 0;
		iconv_t cd = iconv_open(dstCharset, srcCharset);
		if((errno != EINVAL) && (cd != (iconv_t) -1))
			{
			char* outbuf = outputString;
			const char* inbuf = inputStringBeforeConvertion;
			size_t inbytes = (size_t) inputBytes;
			size_t outbytes = (size_t) outputBytes;
			
			if(SameString("NULL",inputString) && SameString("NOT NULL",outputString))
				{
				retVal = iconv(cd, NULL,&inbytes, &outbuf, &outbytes);	
				}
			else if(SameString("NOT NULL",inputString) && SameString("NULL",outputString))
				{
				retVal = iconv(cd, &inbuf,&inbytes, NULL, &outbytes);	
				}
			else if(SameString("NULL",inputString) && SameString("NULL",outputString))
				{
				retVal = iconv(cd, NULL,&inbytes, NULL, &outbytes);	
				}
			else
				{
				retVal = iconv(cd, &inbuf,&inbytes, &outbuf, &outbytes);	
				}
			
			if((inbytes == inBytesAfterConversion) && 
			   (outbytes == outBytesAfterConversion) &&
			   (errNo == errno) && (ret == retVal ))
				{
				INFO_PRINTF1(_L("Test Passed"));
				retVal = KErrNone;
				}
			else
				{
				ERR_PRINTF1(_L("Test Failed"));
				retVal = KErrGeneral;
				}
			}
		else
			{
			ERR_PRINTF1(_L("Test Failed\r\n"));
			retVal = KErrGeneral;
			}
		iconv_close(cd);
    	}
    else
    	{
    	INFO_PRINTF1(_L("Test passed."));
		retVal =  KErrNone;
		if(!srcFlag)
			{
			INFO_PRINTF2(_L("%S Character set not found :Test passed."), &buf);
			}
		if(!dstFlag)
			{
			INFO_PRINTF2(_L("%S Character set not found :Test passed."), &buf1);
			}
    	}
	return retVal;
	}

TInt  CTestLoc_Blr::TestNl_Langinfo_Fail()
    {
    
	  // Create temporary variables in stack
	  char localeName[KMaxParamLength];
	  TInt ret = KErrGeneral;
	  
	  // Read parameters
    ReadStringParam ( localeName);
    
	char* locale = setlocale(LC_ALL,localeName);

    if(locale)
    	{
    	TInt retVal = Langinfo();
    	if(KErrNone != retVal)
	    	{
	    	INFO_PRINTF1(_L("Test passed."));
	    	ret = KErrNone;
	    	}
	    else
	    	{
	    	ERR_PRINTF1(_L("Test failed."));	
	    	ret = KErrGeneral;
	    	}
	   	}
    else
    	{
    	ERR_PRINTF1(_L("Test failed."));
    	ret = KErrGeneral;
    	}
       // Set test result
    return ret;
   
    }  
    
TInt CTestLoc_Blr::TestLocaleConv()
	{
	TInt ret = KErrGeneral;
	char localeName[KMaxParamLength];
	
	char decimal[KMaxParamLength];
	char decimal_point[KMaxParamLength];
	
	char thousand[KMaxParamLength];
	char thousands_sep[KMaxParamLength];
	
	char group[KMaxParamLength];
	int grouping;
	
	char int_curr[KMaxParamLength];
	char int_curr_symbol[KMaxParamLength];
	
	char currency[KMaxParamLength];
	char currency_symbol[KMaxParamLength];
	
	char mon_decimal[KMaxParamLength];
	char mon_decimal_point[KMaxParamLength];
	
	char mon_thousand[KMaxParamLength];
	char mon_thousands_sep[KMaxParamLength];
	
	char mon_group[KMaxParamLength];
	int mon_grouping;
	
	char pos[KMaxParamLength];
	char positive_sign[KMaxParamLength];
	
	char neg[KMaxParamLength];
	char negative_sign[KMaxParamLength];
	
	char int_fraction[KMaxParamLength];
	int int_frac_digits;
	
	char fraction[KMaxParamLength];
	int frac_digits;
	
	char p_precedes[KMaxParamLength];
	int p_cs_precedes;
	
	char p_sapce[KMaxParamLength];
	int p_sep_by_space;
	
	char p_sign[KMaxParamLength];
	int p_sign_posn;
	
	char n_sign[KMaxParamLength];
	int n_sign_posn;
	
	char int_p_precedes[KMaxParamLength];
	int int_p_cs_precedes;
	
	char int_p_sapce[KMaxParamLength];
	int int_p_sep_by_space;
	
	char int_p_sign[KMaxParamLength];
	int int_p_sign_posn;
	
	char int_n_sign[KMaxParamLength];
	int int_n_sign_posn;

	  // Read parameters
    ReadStringParam ( localeName);

    ReadStringParam ( decimal);
    ReadStringParam ( decimal_point);
    
    ReadStringParam ( thousand);
    ReadStringParam ( thousands_sep);
    
    ReadStringParam ( group);
    ReadIntParam(grouping );
    
    ReadStringParam ( int_curr);
    ReadStringParam ( int_curr_symbol);
    
    ReadStringParam ( currency);
    ReadStringParam ( currency_symbol);
    
    ReadStringParam ( mon_decimal);
    ReadStringParam ( mon_decimal_point);
    
    ReadStringParam ( mon_thousand);
    ReadStringParam ( mon_thousands_sep);
    
    ReadStringParam ( mon_group);
    ReadIntParam(mon_grouping );
    
    ReadStringParam ( pos);
    ReadStringParam ( positive_sign);
    
    ReadStringParam ( neg);
    ReadStringParam ( negative_sign);
    
    ReadStringParam ( int_fraction);
    ReadIntParam(int_frac_digits );
    
    ReadStringParam ( fraction);
    ReadIntParam(frac_digits );
    
    ReadStringParam ( p_precedes);
    ReadIntParam(p_cs_precedes );
    
    ReadStringParam ( p_sapce);
    ReadIntParam(p_sep_by_space );
    
    ReadStringParam ( p_sign);
    ReadIntParam(p_sign_posn );
    
    ReadStringParam ( n_sign);
    ReadIntParam(n_sign_posn );
    
    ReadStringParam ( int_p_precedes);
    ReadIntParam(int_p_cs_precedes );
    
    ReadStringParam ( int_p_sapce);
    ReadIntParam(int_p_sep_by_space );
    
    ReadStringParam ( int_p_sign);
    ReadIntParam(int_p_sign_posn );
    
    ReadStringParam ( int_n_sign);
    ReadIntParam(int_n_sign_posn );
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(localeName);
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(localeName[j]);
    	}
  	INFO_PRINTF2(_L("Testing localeconv for  %S "), &buf);	    
	char* locale = NULL;
  	TInt j = 0;
	TInt flag = 0;
    while(j < SUPPORTED_LOCALES)
    	{
    	if(SameString(locales[j].localeName,localeName) && locales[j].flag)
    		{
    		flag = 1;
			break;	    						
    		}
    	j++;
    	}

       
	if(SameString("C",localeName) || SameString("POSIX",localeName) || SameString("",localeName) )
		{
		locale = setlocale(LC_ALL,localeName);
		}
	
    else
    	{
    	j = 0;
		TInt charsetFlag = 0;
		char* srcCharset =  strchr(localeName, '.');
		srcCharset++;
		while(j < SUPPORTED_CHARSETS)
			{
	    	if(SameString(charsets[j].charsetName,srcCharset))
	    		{
	    		charsetFlag = 1;
	    		break;
	    		}
	    	j++;
			}
	
    	if(flag && charsetFlag)
    		{
    		locale = setlocale(LC_ALL,localeName);	
    		}
    	else
	    	{
	    	INFO_PRINTF1(_L("Test passed."));
	    	TBuf<MAX_SIZE> buf;
    		TInt len = strlen(localeName);
		    for (TInt j =0; j<len;j++)
		    	{
		    	buf.Append(localeName[j]);
		    	}
		    TBuf<MAX_SIZE> buf1;
    		len = strlen(srcCharset);
		    for (TInt j =0; j<len;j++)
		    	{
		    	buf1.Append(srcCharset[j]);
		    	}
			if(!flag)
				{
				INFO_PRINTF2(_L("%S Locale is not Supported :: Test passed."), &buf);
				}
	    	if(!charsetFlag)
	    		{
	    		INFO_PRINTF2(_L("%S Charset is not Supported :: Test passed."), &buf1);	    	
	    		}
			return KErrNone;
	    	}
    	}
  	
  	if(locale)
    	{
    	struct lconv*  result = localeconv();
    	if(result)
    		{
    		TInt retVal = 1;
	    	if(!SameString(decimal_point,result->decimal_point))
	    		{
	    		ERR_PRINTF1(_L("Test Failed."));
    			INFO_PRINTF3(_L("Actual decimal_point =%S\t API returned decimal_point is  = %S\n"), decimal_point,result->decimal_point);
	    		retVal = 0;
	    		}
	    	if(!SameString(thousands_sep, result->thousands_sep))
	    		{
	    		INFO_PRINTF1(_L("Test Failed."));
    			INFO_PRINTF3(_L("Actual thousands_sep =%S\t API returned thousands_sep is  = %S\n"), thousands_sep,result->thousands_sep);
	    		retVal = 0;
	    		}
			if(!SameString(int_curr_symbol, result->int_curr_symbol))
				{
				INFO_PRINTF1(_L("Test Failed."));
    			INFO_PRINTF3(_L("Actual int_curr_symbol =%S\t API returned int_curr_symbol is  = %S\n"), int_curr_symbol,result->int_curr_symbol);
				retVal = 0;
				}
			char temp[100];
    		ConvertHexTochar(temp, currency_symbol);
			if(!SameString(temp, result->currency_symbol))
				{
				INFO_PRINTF1(_L("Test Failed."));
    			INFO_PRINTF3(_L("Actual currency_symbol =%S\t API returned currency_symbol is  = %S\n"), temp,result->currency_symbol);
				retVal = 0;
				}
			if(!SameString(mon_decimal_point, result->mon_decimal_point))
				{
				INFO_PRINTF1(_L("Test Failed."));
    			INFO_PRINTF3(_L("Actual mon_decimal_point =%S\t API returned mon_decimal_point is  = %S\n"), mon_decimal_point,result->mon_decimal_point);
				retVal = 0;
				}
			if(!SameString(mon_thousands_sep, result->mon_thousands_sep))
				{
				INFO_PRINTF1(_L("Test Failed."));
    			INFO_PRINTF3(_L("Actual mon_thousands_sep =%S\t API returned mon_thousands_sep is  = %S\n"), mon_thousands_sep,result->mon_thousands_sep);
				retVal = 0;
				}
			if(!SameString(positive_sign, result->positive_sign))
				{
				retVal = 0;
				}
			if(!SameString(negative_sign, result->negative_sign))
				{
				retVal = 0;
				}
			
			
			if(grouping !=  ((int) result->grouping[0] - 48))
				{
				retVal = 0;	
				}
			if(mon_grouping != ((int) result->mon_grouping[0] - 48))
				{
				retVal = 0;	
				}
			if((char) int_frac_digits != result->int_frac_digits)
				{
				retVal = 0;
				}
			if((char) frac_digits != result->frac_digits)
				{
				retVal = 0;
				}
			if((char) p_cs_precedes != result->p_cs_precedes)
				{
				retVal = 0;
				}
			if((char) p_sep_by_space != result->p_sep_by_space)
				{
				retVal = 0;
				}
			if((char) p_sign_posn != result->p_sign_posn)
				{
				retVal = 0;
				}
			if((char) n_sign_posn != result->n_sign_posn)
				{
				retVal = 0;
				}
			if((char) int_p_cs_precedes != result->int_p_cs_precedes)
				{
				retVal = 0;
				}
			if((char) int_p_sep_by_space != result->int_p_sep_by_space)
				{
				retVal = 0;
				}
			if((char) int_p_sign_posn != result->int_p_sign_posn)
				{
				retVal = 0;
				}
			if((char) int_n_sign_posn != result->int_n_sign_posn)
				{
				retVal = 0;
				}
				        
	     	if(retVal)	
	    		{
	    		INFO_PRINTF1(_L("Test passed."));
	    		ret = KErrNone;
	    		}
	    	else
	    		{
	    		ERR_PRINTF1(_L("Test failed."));
	    		ret = KErrGeneral;
	    		}	
    		}
    	
	    else
	    	{
	    	ERR_PRINTF1(_L("Test failed."));
	    	ret = KErrGeneral;
	    	}
    }
    else
    	{
    	ERR_PRINTF1(_L("Test failed."));
    	INFO_PRINTF2(_L("%S  setting failed\t\t"), localeName);	    
    	ret = KErrGeneral;
    	}
	return ret;
    
	}

// -----------------------------------------------------------------------------
//Function Name :Langinfo
//Description:Tests language info of the locale read from ini file
//return: KErrGeneral if any error therwise KErrNone
// ---------------------------------------------------------------------------
TInt CTestLoc_Blr::Langinfo()
{
	TInt retVal = KErrGeneral;
	char codeset[10];
	char codesetExpected[15];
	
	ReadStringParam ( codeset);
    ReadExpStringParam(codesetExpected);

    retVal = ReadLocaleInformation(codeset, codesetExpected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char abday2[8];
	char abday2Expected[40];
	ReadStringParam (abday2);
    ReadExpStringParam(abday2Expected);
    retVal = ReadLocaleInformation(abday2, abday2Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char abday3[8];
	char abday3Expected[40];
	ReadStringParam ( abday3);
    ReadExpStringParam(abday3Expected);
    retVal = ReadLocaleInformation(abday3, abday3Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char abday4[8];
	char abday4Expected[40];
	ReadStringParam ( abday4);
    ReadExpStringParam(abday4Expected);
    retVal = ReadLocaleInformation(abday4, abday4Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
	
	char abday5[8];
	char abday5Expected[40];
	ReadStringParam ( abday5);
    ReadExpStringParam(abday5Expected);
    retVal = ReadLocaleInformation(abday5, abday5Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char abday6[8];
	char abday6Expected[40];
	ReadStringParam ( abday6);
    ReadExpStringParam(abday6Expected);
    retVal = ReadLocaleInformation(abday6, abday6Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char abday7[8];
	char abday7Expected[40];
	ReadStringParam ( abday7);
    ReadExpStringParam(abday7Expected);
    retVal = ReadLocaleInformation(abday7, abday7Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char abday1[8];
	char abday1Expected[40];
	ReadStringParam ( abday1);
    ReadExpStringParam(abday1Expected);
    retVal = ReadLocaleInformation(abday1, abday1Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char day2[6];
	char day2Expected[100];
	ReadStringParam ( day2);
    ReadExpStringParam(day2Expected);
    retVal = ReadLocaleInformation(day2, day2Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char day3[6];
	char day3Expected[100];
	ReadStringParam ( day3);
    ReadExpStringParam(day3Expected);
    retVal = ReadLocaleInformation(day3, day3Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char day4[6];
	char day4Expected[100];
	ReadStringParam ( day4);
    ReadExpStringParam(day4Expected);
    retVal = ReadLocaleInformation(day4, day4Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char day5[6];
	char day5Expected[100];
	ReadStringParam ( day5);
    ReadExpStringParam(day5Expected);
    retVal = ReadLocaleInformation(day5, day5Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char day6[6];
	char day6Expected[100];
	ReadStringParam ( day6);
    ReadExpStringParam(day6Expected);
    retVal = ReadLocaleInformation(day6, day6Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char day7[6];
	char day7Expected[100];
	ReadStringParam ( day7);
    ReadExpStringParam(day7Expected);
    retVal = ReadLocaleInformation(day7, day7Expected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char day1[6];
	char day1Expected[100];
	ReadStringParam ( day1);
    ReadExpStringParam(day1Expected);
    retVal = ReadLocaleInformation(day1, day1Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth1[8];
	char abmonth1Expected[100];
	ReadStringParam ( abmonth1);
    ReadExpStringParam(abmonth1Expected);
    retVal = ReadLocaleInformation(abmonth1, abmonth1Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth2[8];
	char abmonth2Expected[100];
	ReadStringParam ( abmonth2);
    ReadExpStringParam(abmonth2Expected);
    retVal = ReadLocaleInformation(abmonth2, abmonth2Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth3[8];
	char abmonth3Expected[100];
	ReadStringParam ( abmonth3);
    ReadExpStringParam(abmonth3Expected);
    retVal = ReadLocaleInformation(abmonth3, abmonth3Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth4[8];
	char abmonth4Expected[100];
	ReadStringParam ( abmonth4);
    ReadExpStringParam(abmonth4Expected);
    retVal = ReadLocaleInformation(abmonth4, abmonth4Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth5[8];
	char abmonth5Expected[100];
	ReadStringParam ( abmonth5);
    ReadExpStringParam(abmonth5Expected);
    retVal = ReadLocaleInformation(abmonth5, abmonth5Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth6[8];
	char abmonth6Expected[100];
	ReadStringParam ( abmonth6);
    ReadExpStringParam(abmonth6Expected);
    retVal = ReadLocaleInformation(abmonth6, abmonth6Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth7[8];
	char abmonth7Expected[100];
	ReadStringParam ( abmonth7);
    ReadExpStringParam(abmonth7Expected);
    retVal = ReadLocaleInformation(abmonth7, abmonth7Expected);
    if(retVal == KErrGeneral)
		{	
    	return KErrGeneral;
		}
    
	char abmonth8[8];
	char abmonth8Expected[100];
	ReadStringParam ( abmonth8);
    ReadExpStringParam(abmonth8Expected);
    retVal = ReadLocaleInformation(abmonth8, abmonth8Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth9[8];
	char abmonth9Expected[100];
	ReadStringParam ( abmonth9);
    ReadExpStringParam(abmonth9Expected);
    retVal = ReadLocaleInformation(abmonth9, abmonth9Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth10[8];
	char abmonth10Expected[100];
	ReadStringParam ( abmonth10);
    ReadExpStringParam(abmonth10Expected);
    retVal = ReadLocaleInformation(abmonth10, abmonth10Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth11[8];
	char abmonth11Expected[100];
	ReadStringParam ( abmonth11);
    ReadExpStringParam(abmonth11Expected);
    retVal = ReadLocaleInformation(abmonth11, abmonth11Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth12[8];
	char abmonth12Expected[100];
	ReadStringParam ( abmonth12);
    ReadExpStringParam(abmonth12Expected);
    retVal = ReadLocaleInformation(abmonth12, abmonth12Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month1[6];
	char month1Expected[100];
	ReadStringParam ( month1);
    ReadExpStringParam(month1Expected);
    retVal = ReadLocaleInformation(month1, month1Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month2[6];
	char month2Expected[100];
	ReadStringParam ( month2);
    ReadExpStringParam(month2Expected);
    retVal = ReadLocaleInformation(month2, month2Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month3[6];
	char month3Expected[100];
	ReadStringParam ( month3);
    ReadExpStringParam(month3Expected);
    retVal = ReadLocaleInformation(month3, month3Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month4[6];
	char month4Expected[100];
	ReadStringParam ( month4);
    ReadExpStringParam(month4Expected);
    retVal = ReadLocaleInformation(month4, month4Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month5[6];
	char month5Expected[100];
	ReadStringParam ( month5);
    ReadExpStringParam(month5Expected);
    retVal = ReadLocaleInformation(month5, month5Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month6[6];
	char month6Expected[100];
	ReadStringParam ( month6);
    ReadExpStringParam(month6Expected);
    retVal = ReadLocaleInformation(month6, month6Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
	char month7[6];
	char month7Expected[100];
	ReadStringParam ( month7);
    ReadExpStringParam(month7Expected);
    retVal = ReadLocaleInformation(month7, month7Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month8[6];
	char month8Expected[100];
	ReadStringParam ( month8);
    ReadExpStringParam(month8Expected);
    retVal = ReadLocaleInformation(month8, month8Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month9[6];
	char month9Expected[100];
	ReadStringParam ( month9);
    ReadExpStringParam(month9Expected);
    retVal = ReadLocaleInformation(month9, month9Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
		
	char month10[6];
	char month10Expected[100];
	ReadStringParam ( month10);
    ReadExpStringParam(month10Expected);
    retVal = ReadLocaleInformation(month10, month10Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month11[6];
	char month11Expected[100];
	ReadStringParam ( month11);
    ReadExpStringParam(month11Expected);
    retVal = ReadLocaleInformation(month11, month11Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month12[6];
	char month12Expected[100];
	ReadStringParam ( month12);
    ReadExpStringParam(month12Expected);
    retVal = ReadLocaleInformation(month12, month12Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char am[7];
	char amExpected[20];
	ReadStringParam ( am);
    ReadExpStringParam(amExpected);
    retVal = ReadLocaleInformation(am, amExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char pm[7];
	char pmExpected[20];
	ReadStringParam ( pm);
    ReadExpStringParam(pmExpected);
    retVal = ReadLocaleInformation(pm, pmExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char dfmt[7];
	char dfmtExpected[10];
	ReadStringParam ( dfmt);
    ReadExpStringParam( dfmtExpected);
    retVal = ReadLocaleInformation(dfmt, dfmtExpected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char tfmt[7];
	char tfmtExpected[12];
	ReadStringParam ( tfmt);
    ReadExpStringParam(tfmtExpected);
    retVal = ReadLocaleInformation(tfmt, tfmtExpected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char tfmtAmPm[7];
	char tfmtAmPmExpected[12];
	ReadStringParam ( tfmtAmPm);
    ReadExpStringParam(tfmtAmPmExpected);
    retVal = ReadLocaleInformation(tfmtAmPm, tfmtAmPmExpected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char era[4];
	char eraExpected[2];
	ReadStringParam ( era);
    ReadExpStringParam(eraExpected);
    retVal = ReadLocaleInformation(era, eraExpected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char eratfmt[10];
	char eratfmtExpected[2];
	ReadStringParam ( eratfmt);
    ReadExpStringParam(eratfmtExpected);
    retVal = ReadLocaleInformation(eratfmt, eratfmtExpected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char eradtfmt[10];
	char eradtfmtExpected[2];
	ReadStringParam ( eradtfmt);
    ReadExpStringParam(eradtfmtExpected);
    retVal = ReadLocaleInformation(eradtfmt, eradtfmtExpected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char altdigits[15];
	char altdigitsExpected[2];
	ReadStringParam ( altdigits);
    ReadExpStringParam(altdigitsExpected);
    retVal = ReadLocaleInformation(altdigits, altdigitsExpected);
    if(retVal == KErrGeneral)
    	{
    	return KErrGeneral;
    	}
    
	char radixchar[10];
	char radixcharExpected[2];
	ReadStringParam ( radixchar);
    ReadExpStringParam(radixcharExpected);
    retVal = ReadLocaleInformation(radixchar, radixcharExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
		
	char thousep[10];
	char thousepExpected[2];
	ReadStringParam ( thousep);
    ReadExpStringParam(thousepExpected);
    retVal = ReadLocaleInformation(thousep, thousepExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    return retVal;
	}

void CTestLoc_Blr::ConvertHexTochar(char* dst, char*src)
	{
	int i = 0;
	
	while(*src != '\0')
		{
		if(*src == '\\')
			{
			src++;
			if(*src == 'x')
				{
				src++;
				char temp[2];
				temp[0] = *src;
				src++;
				temp[1] = *src;
				int tenPos = 0;
				int digitPos = 0;
				if(isalpha(temp[0]))
					{
					tenPos = isupper(temp[0])?temp[0] - 'A' + 10: temp[0] - 'a' + 10;
					
					}
				if(isdigit(temp[0]))
					{
					tenPos = temp[0] - '0';
					}
				if(isalpha(temp[1]))
					{
					digitPos = isupper(temp[1])?temp[1] - 'A' + 10: temp[1] - 'a' + 10;
					
					}
				if(isdigit(temp[1]))
					{
					digitPos = temp[1] - '0';
					}
				dst[i++] = tenPos *16 + digitPos;
				}
			}
		else
			{
			dst[i++] = *src;
			}
		src++;
		}
	dst[i++] = '\0';
	}

TInt CTestLoc_Blr::ReadLocaleInformation(char* itemName, char* expectedOuput)
	{
	// Do some testing
	int item = CODESET;	  	         
	if(SameString("CODESET",itemName))
		{
		item = CODESET;
		}
	if(SameString("D_T_FMT",itemName))
		{
		item = D_T_FMT;
		}
	if(SameString("D_FMT",itemName))
		{
		item = D_FMT;
		}
	if(SameString("T_FMT",itemName))
		{
		item = T_FMT;
		}
	if(SameString("T_FMT_AMPM",itemName))
		{
		item = T_FMT_AMPM;
		}
	if(SameString("AM_STR",itemName))
		{
		item = AM_STR;
		}
	if(SameString("PM_STR",itemName))
		{
		item = PM_STR;
		}
	if(SameString("DAY_1",itemName)) 
		{
		item = DAY_1;
		}
	if(SameString("DAY_2",itemName))
		{
		item = DAY_2;	
		}
	if(SameString("DAY_3",itemName))
		{
		item = DAY_3;
		}
	if(SameString("DAY_4",itemName))
		{
		item = DAY_4;
		}
	if(SameString("DAY_5",itemName))
		{
		item = DAY_5;
		}
	if(SameString("DAY_6",itemName))
		{
		item = DAY_6;
		}
	if(SameString("DAY_7",itemName))
		{
		item = DAY_7;
		}
	if(SameString("ABDAY_1",itemName)) 
		{
		item = ABDAY_1;	
		}
	if(SameString("ABDAY_2",itemName)) 
		{
		item = ABDAY_2;
		}
	if(SameString("ABDAY_3",itemName))
		{
		item = ABDAY_3;
		}
	if(SameString("ABDAY_4",itemName)) 
		{
		item = ABDAY_4;
		}
	if(SameString("ABDAY_5",itemName)) 
		{
		item = ABDAY_5;
		}
	if(SameString("ABDAY_6",itemName)) 
		{
		item = ABDAY_6;
		}
	if(SameString("ABDAY_7",itemName))
		{
		item = ABDAY_7;
		}
	if(SameString("MON_1",itemName))
		{
		item = MON_1;
		}
	if(SameString("MON_2",itemName))
		{
		item = MON_2;
		}
	if(SameString("MON_3",itemName))
		{
		item = MON_3;
		}
	if(SameString("MON_4",itemName))
		{
		item = MON_4;
		}
	if(SameString("MON_5",itemName))
		{
		item = MON_5;
		}
	if(SameString("MON_6",itemName))
		{
		item = MON_6;
		}
	if(SameString("MON_7",itemName))
		{
		item = MON_7;
		}
	if(SameString("MON_8",itemName))
		{
		item = MON_8;
		}
	if(SameString("MON_9",itemName))
		{
		item = MON_9;
		}
	if(SameString("MON_10",itemName)) 
		{
		item = MON_10;
		}
	if(SameString("MON_11",itemName)) 
		{
		item = MON_11;
		}
	if(SameString("MON_12",itemName))
		{
		item = MON_12;
		}
	if(SameString("ABMON_1",itemName)) 
		{
		item = ABMON_1;
		}
	if(SameString("ABMON_2",itemName)) 
		{
		item = ABMON_2;
		}
	if(SameString("ABMON_3",itemName)) 
		{
		item = ABMON_3;
		}
	if(SameString("ABMON_4",itemName))
		{
		item = ABMON_4;
		}
	if(SameString("ABMON_5",itemName)) 
		{
		item = ABMON_5;
		}
	if(SameString("ABMON_6",itemName)) 
		{
		item = ABMON_6;
		}
	if(SameString("ABMON_7",itemName)) 
		{
		item = ABMON_7;
		}
	if(SameString("ABMON_8",itemName))
		{
		item = ABMON_8;
		}
	if(SameString("ABMON_9",itemName)) 
		{
		item = ABMON_9;
		}
	if(SameString("ABMON_10",itemName)) 
		{
		item = ABMON_10;
		}
	if(SameString("ABMON_11",itemName)) 
		{
		item = ABMON_11;
		}
	if(SameString("ABMON_12",itemName))
		{
		item = ABMON_12;
		}
	if(SameString("ERA",itemName))
		{
		item = ERA;
		}
	if(SameString("ERA_D_FMT",itemName))
		{
		item = ERA_D_FMT;
		}
	if(SameString("ERA_D_T_FMT",itemName))
		{
		item = ERA_D_T_FMT;
		}
	if(SameString("ERA_T_FMT",itemName))
		{
		item = ERA_T_FMT;
		}
	if(SameString("ALT_DIGITS",itemName))
		{
		item = ALT_DIGITS;
		}
	if(SameString("RADIXCHAR",itemName))
		{
		item = RADIXCHAR;
		}
	if(SameString("THOUSEP",itemName))
		{
		item = THOUSEP;
		}
	
	char temp[100];
    ConvertHexTochar(temp, expectedOuput);
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(itemName);
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(itemName[j]);
    	}
    	
    TBuf<MAX_SIZE> buf1;
    len = strlen(expectedOuput);
    for (TInt j =0; j<len;j++)
    	{
    	buf1.Append( (expectedOuput[j]));	
    	}
    	
    char* result =  nl_langinfo(item);
	
	TBuf<MAX_SIZE> buf3;
    len = strlen(result);
    for (TInt j =0; j<len;j++)
    	{
    	buf3.Append(result[j]);
    	}
	
	if(result != NULL)
		{
		if(SameString(result,temp))	
			{
			INFO_PRINTF1(_L("Test passed."));
    		INFO_PRINTF3(_L("API Output =%S\t Expected Output = %S\n"), &buf1, &buf3);
			return KErrNone;    		
			}
		else
			{
			INFO_PRINTF1(_L("Test Failed."));
    		INFO_PRINTF4(_L("ITEM = %S\t API Output =%S\t Expected Output = %S\n"), &buf, &result,&buf1);
			return KErrGeneral;
			}
		}
	else
		{
		return KErrGeneral;
		}
}

#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
void CTestLoc_Blr::LocateSupportedDLLs()
    {
    TInt j = 0;
       FILE* stream = NULL;
    stream = fopen("C:\\resource\\openc\\locales_new.txt", "r");
    INFO_PRINTF1(_L("LocateSupportedDLLs:"));
    RSessionBase aSessionBase;
    RLoader loader;
         
    char buffer[25];
    char* buf = fgets(buffer, 50, stream);
    _LIT(KRegName, "elocl_reg.");
    _LIT(KColName, "elocl_col.");
    _LIT(KLanName, "elocl_lan.");
    _LIT(KComma, ",");
    while(buf != NULL)
        {
        int  i = 0;
        while(buffer[i] != '=')
            {
            locales[j].localeName[i] = buffer[i];
            i++;                
            }
        locales[j].localeName[i++] = '\0';
        TBuf<50> localeids;
        while(buffer[i] && ('\r' != buffer[i]) && ('\n' != buffer[i]))
        {
            localeids.Append(buffer[i]);  
            i++;
        }
        
        TBuf<30> regDll;
        TBuf<30> lanDll;
        TBuf<30> colDll;
        TBuf<30> token[3];
        regDll.Copy(KRegName);
        colDll.Copy(KColName);
        lanDll.Copy(KLanName);
       
        TLex lex( localeids );
        TChar ch;
        TInt k = 0;
         while((ch = lex.Get()) != 0 )
            {
            while ((ch = lex.Peek()) != ',' && ch != 0) 
                {
                lex.Inc();
                }
            token[k].Copy(lex.MarkedToken());
            k++;
            lex.Inc();
            lex.Mark();
            }
        
        lanDll.Append(token[0]);
        regDll.Append(token[1]);
       colDll.Append(token[2]);
        TInt r = loader.Connect();
        TInt size = KNumLocaleExports * sizeof(TLibraryFunction);
       if(KErrNone == r)
           {
           TLibraryFunction data1[KNumLocaleExports];
           TLibraryFunction data2[KNumLocaleExports];
           TLibraryFunction data3[KNumLocaleExports];
           TPtr8 functionListBuf1((TUint8*) data1, size, size);
           r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&lanDll, &functionListBuf1) );
           locales[j].flag = (KErrNone == r)? 1:0;
           
           TPtr8 functionListBuf2((TUint8*) data2, size, size);
           r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&regDll, &functionListBuf2) );
           locales[j].flag = (KErrNone == r)? 1:0;
          
           TPtr8 functionListBuf3((TUint8*) data3, size, size);
           r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&regDll, &functionListBuf3) );
           locales[j].flag = (KErrNone == r)? 1:0;
                    
           }
        aSessionBase.Close();
        loader.Close();
        
        buffer[0] = '\0';
        buf = fgets(buffer, 50, stream);
        j++;
        }
        fclose(stream);
        INFO_PRINTF1(_L("LocateSupportedDLLs: success"));
    }
#else
void CTestLoc_Blr::LocateSupportedDLLs()
	{
	TInt j = 0;
	FILE* stream = NULL;
	stream = fopen("C:\\locales.txt", "r");
	INFO_PRINTF1(_L("LocateSupportedDLLs:"));
	RSessionBase aSessionBase;
	RLoader loader;
			
	char buffer[25];
	char* buf = fgets(buffer, 50, stream);
	
	while(buf != NULL)
		{
		int  i = 0;
		while(buffer[i] != '=')
			{
			locales[j].localeName[i] = buffer[i];
			i++;				
			}
		locales[j].localeName[i++] = '\0';

		TBuf<20> DllName;
		_LIT(KDllName, "elocl.");
		DllName.Copy(KDllName);
				
		while(buffer[i] && ('\r' != buffer[i]) && ('\n' != buffer[i]))
			{
			DllName.Append(buffer[i]);	
			i++;
			}
		
		TInt r = loader.Connect();
		if(KErrNone == r)
			{
			TLibraryFunction data[KNumLocaleExports];
			TInt size = KNumLocaleExports * sizeof(TLibraryFunction);
			TPtr8 functionListBuf((TUint8*) data, size, size);
			r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&DllName, &functionListBuf) );
			locales[j].flag = (KErrNone == r)? 1:0;
			}
		aSessionBase.Close();
		loader.Close();
		
		buffer[0] = '\0';
		buf = fgets(buffer, 50, stream);
		j++;
		}
		fclose(stream);
		INFO_PRINTF1(_L("LocateSupportedDLLs: success"));
	}
#endif
void CTestLoc_Blr::SupportedCharsets()
	{
	RFs fileSession;
	fileSession.Connect();
	CleanupClosePushL(fileSession);
		
	CCnvCharacterSetConverter* conv = CCnvCharacterSetConverter::NewL() ;
	CleanupStack::PushL(conv);
		
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet> *charSet = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL(fileSession);
	CleanupStack::PushL( charSet );
	
	TInt i = 0;
	TInt j = 0;
	
	TInt count = charSet->Count();
	while(i < count)
		{
		CCnvCharacterSetConverter::SCharacterSet characterSet = charSet->At(i);	
		TUint charSetUID = characterSet.Identifier();
				
		const TBufC<25> charSetName = characterSet.NameIsFileName()?TParsePtrC(characterSet.Name()).Name():characterSet.Name();								
		TBuf8<25> aCharSetname;
		aCharSetname.Copy(charSetName);
	
		strncpy(charsets[j].charsetName, (char*) aCharSetname.Ptr(), aCharSetname.Length());
		charsets[j].charsetName[aCharSetname.Length()] = '\0';
		j++;
		
		HBufC8* stdInterName = NULL;		 
		stdInterName = conv->ConvertCharacterSetIdentifierToStandardNameL(charSetUID, fileSession);					
		
		if(NULL != stdInterName)
			{
			strncpy(charsets[j].charsetName, (char*) stdInterName->Ptr(), stdInterName->Length());
			charsets[j].charsetName[stdInterName->Length()] = '\0';
			j++;
			}
		delete stdInterName;
		stdInterName = NULL;
		i++;
		}
	
	CleanupStack::PopAndDestroy(3); 
	}
