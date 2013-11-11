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



#define MAX_LEN 50
#define MIN_LEN 20

#include "teststring.h"
#include <unistd.h> 
#include <errno.h>
#include <string.h>
#include <wchar.h>
#include <stdlib.h>
#include <math.h>

// -----------------------------------------------------------------------------
// Name : TestStrtok_r
// Description: Tests strtok_r() API, which searches for a token in a given string
// 
// -----------------------------------------------------------------------------
//	
TInt CTestString::TestStrtok_r()
	{
	INFO_PRINTF1(_L("Test strtok_r()"));

	TPtrC str1;
	_LIT( KParam1, "Parameter1" );
	TBool res = GetStringFromConfig(ConfigSection(), KParam1, str1);
	if(!res)  
		{
		_LIT(Kerr , "Failed to read parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	TBuf8 <MAX_LEN>strBuf1;
	strBuf1.Copy(str1);
	char *bs = (char*)strBuf1.Ptr();
	TInt len = strBuf1.Length();
	bs[len] = '\0';
	
	_LIT(KParam2,"Parameter2");	
	res = GetStringFromConfig(ConfigSection(), KParam2, str1);
	TBuf8 <MAX_LEN>strBuf2;
	strBuf2.Copy(str1);
	char *ss = (char*)strBuf2.Ptr();
	len = strBuf2.Length();
	ss[len] = '\0';
	INFO_PRINTF2(_L("Token string :%S"), &str1);
	
	char* util = 0;
	char* sptr;
	util = strtok_r( bs, ss, &sptr );
	do 
		{
		TBuf<MAX_LEN> buf;
	 	TInt len = strlen(util);
	 	TInt i;
	 	for (i=0; i<len; i++)
	 		{
	 		buf.Append(util[i]);
	 		}
	 	INFO_PRINTF2(_L("Token = '%S' \n"), &buf);
	 	util = strtok_r( NULL, ss, &sptr );
		}
	while(util != NULL );
	
	return EPass;
	}

// -----------------------------------------------------------------------------
// Name : TestStrnstr
// Description: Tests strnstr() API, The strnstr function locates the first 
// occurrence of a null-terminated string in a big string, where not more than
// len characters specified are searched. 
// -----------------------------------------------------------------------------
//	
TInt CTestString::TestStrnstr()
	{
	INFO_PRINTF1(_L("Test strnstr()"));
	
	TPtrC str1;
	TInt num;
	_LIT( KParam1, "Parameter1" );
	TBool res = GetStringFromConfig(ConfigSection(), KParam1, str1);
	if(!res)  
		{
		_LIT(Kerr , "Failed to read parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	TBuf8 <MAX_LEN>strBuf1;
	strBuf1.Copy(str1);
	char *bs = (char*)strBuf1.Ptr();
	TInt len = strBuf1.Length();
	bs[len] = '\0';
	INFO_PRINTF2(_L("Main string :%S"), &str1);
	
	_LIT(KParam2,"Parameter2");	
	res = GetStringFromConfig(ConfigSection(), KParam2, str1);
	TBuf8 <MAX_LEN>strBuf2;
	strBuf2.Copy(str1);
	char *ss = (char*)strBuf2.Ptr();
	TInt sStrlen = strBuf2.Length();
	ss[sStrlen] = '\0';
	INFO_PRINTF2(_L("Sub string :%S"), &str1);
	
	_LIT(KParam3, "Parameter3");
	res = GetIntFromConfig(ConfigSection(), KParam3, num);
	
	char* ptr = strnstr(bs, ss, num);
	//Substring is an empty string
	if (sStrlen == 0)
		{
		TInt ret = strcmp(ptr,bs);
		if (ret==0)
			{
			return KErrNone;
			}
		else
			{
			return KErrGeneral;
			}
		}
	//Substring is not present in the string	
	if (ptr == NULL)
		{
		INFO_PRINTF1(_L("Sub string not present \n"));
		return KErrNone;
		}
	else
		{
		TBuf<MAX_LEN> buf;
		len = strlen(ptr);
		TInt i;
		for (i=0; i<len; i++)
			{
		 	buf.Append(ptr[i]);
		 	}
		INFO_PRINTF2(_L("String = '%S' \n"), &buf);	
		return KErrNone;
		}
	}

// -----------------------------------------------------------------------------
// Name : TestStrcasestr
// Description: Tests strcasestr() API, The strcasestr function locates a
// substring in a string irrespective of the case
// -----------------------------------------------------------------------------
//	
TInt CTestString::TestStrcasestr()
	{
	INFO_PRINTF1(_L("Test strcasestr()"));
	
	TPtrC str1;

	_LIT( KParam1, "Parameter1" );
	TBool res = GetStringFromConfig(ConfigSection(), KParam1, str1);
	if(!res)  
		{
		_LIT(Kerr , "Failed to read parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	TBuf8 <MAX_LEN>strBuf1;
	strBuf1.Copy(str1);
	char *bs = (char*)strBuf1.Ptr();
	TInt len = strBuf1.Length();
	bs[len] = '\0';
	INFO_PRINTF2(_L("Main string :%S"), &str1);
	
	_LIT(KParam2,"Parameter2");	
	res = GetStringFromConfig(ConfigSection(), KParam2, str1);
	TBuf8 <MAX_LEN>strBuf2;
	strBuf2.Copy(str1);
	char *ss = (char*)strBuf2.Ptr();
	TInt sSlen = strBuf2.Length();
	ss[len] = '\0';
	INFO_PRINTF2(_L("Sub string :%S"), &str1);
	
	char* ptr = strcasestr(bs, ss);
	
	//Search string is empty
	if (sSlen == 0)
		{
		TInt ret = strcmp(ptr,bs);
		if (ret==0)
			{
			return KErrNone;
			}
		else
			{
			return KErrGeneral;
			}
		}
	//Substring is not present
	if (ptr == NULL)
		{
		INFO_PRINTF1(_L("Sub string not present \n"));
		return KErrNone;
		}
	else
		{
		TBuf<MAX_LEN> buf;
		len = strlen(ptr);
		TInt i;
		for (i=0; i<len; i++)
			{
		 	buf.Append(ptr[i]);
		 	}
		INFO_PRINTF2(_L("String = '%S' \n"), &buf);
		return KErrNone;	
		}
		
	}

// -----------------------------------------------------------------------------
// Name : TestMemccpy
// Description: Tests memccpy() API, The memccpy() function copies bytes from
// string src to string dst.
// -----------------------------------------------------------------------------
//
TInt CTestString::TestMemccpy()
	{
	INFO_PRINTF1(_L("Test memccpy()"));
	
	TPtrC str1;

	_LIT( KParam1, "Parameter1" );
	TBool res = GetStringFromConfig(ConfigSection(), KParam1, str1);
	if(!res)  
		{
		_LIT(Kerr , "Failed to read parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	TBuf8 <MAX_LEN>strBuf1;
	strBuf1.Copy(str1);
	char *bs = (char*)strBuf1.Ptr();
	TInt len = strBuf1.Length();
	bs[len] = '\0';
	INFO_PRINTF2(_L("Main string :%S"), &str1);
	
	_LIT( KParam2, "Parameter2" );
	res = GetStringFromConfig(ConfigSection(), KParam2, str1);
	if(!res)  
		{
		_LIT(Kerr , "Failed to read parameter from ini file.") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	TBuf8 <MAX_LEN>strBuf2;
	strBuf2.Copy(str1);
	char *ch = (char*)strBuf2.Ptr();
	len = strBuf2.Length();
	ch[len] = '\0';
	INFO_PRINTF2(_L("Terminating string :%S"), &str1);
	int c = ch[0];
	
	TInt num;
	_LIT(KParam3, "Parameter3");
	res = GetIntFromConfig(ConfigSection(), KParam3, num);
	INFO_PRINTF2(_L("Size of destination area:%d"), num);
		
	char buf[MIN_LEN];
	memset(buf, 0x41, sizeof(buf));
	
	void* ptr = memccpy (buf, bs, c, num);
	
	TBuf<MAX_LEN> buffer;
	len = strlen(buf);
	TInt i;
	for (i=0; i<len; i++)
		{
	 	buffer.Append(buf[i]);
	 	}
	INFO_PRINTF2(_L("String = '%S' \n"), &buffer);
	return EPass ;
	}

// -----------------------------------------------------------------------------
// Name : TestStpcpy
// Description: Tests stpcpy() API, The stpcpy() function copies bytes from
// string src to string dst.
// -----------------------------------------------------------------------------
//	
TInt CTestString::TestStpcpy()
	{
	char buffer[MAX_LEN];
    char *to;
    to = stpcpy (buffer, "hello");
     
    TBuf<MAX_LEN> dispBuf;
	TInt len = strlen(buffer);
	TInt i;
	for (i=0; i<len; i++)
		{
	 	dispBuf.Append(buffer[i]);
	 	}
	INFO_PRINTF2(_L("String = '%S' \n"), &dispBuf);
	
	if (*to == NULL)
		{
		INFO_PRINTF1(_L("Returned pointer is terminating Character\n"));
		}
	return EPass;
	}

// -----------------------------------------------------------------------------
// Name : TestWcsColl
// Description: Tests wcscoll() API, The stpcpy() function copies bytes from
// string src to string dst.
// -----------------------------------------------------------------------------
//
TInt CTestString::TestWcsColl()
	{
	int result;
    wchar_t *wcs1 = L"first";
   	wchar_t *wcs2 = L"first";
   	    
 	result = validate(wcscoll(wcs1, wcs2)==0);
 	if (result==KErrNone)
 		{
 		INFO_PRINTF1(_L("Strings are identical"));	
 		}
 	return result;
	}

// -----------------------------------------------------------------------------
// Name : TstWcsCollUnlikestr1
// Description: Tests wcscoll() API, The stpcpy() function copies bytes from
// string src to string dst.
// -----------------------------------------------------------------------------
//	
TInt CTestString::TstWcsCollUnlikestr1()
	{
	int ret;
    wchar_t *wcs1 = L"first";
   	wchar_t *wcs2 = L"second";
   	    
 	ret = validate(wcscoll(wcs1, wcs2) < 0);
 	if (ret == KErrNone)
 		{
 		INFO_PRINTF1(_L("String1 is less than String2"));
 		}
 	else
 		{
   		INFO_PRINTF1(_L("String1 is greater than String2"));	
   		}
   	return ret;	
	}
	
// -----------------------------------------------------------------------------
// Name : TstWcsCollUnlikestr2
// Description: Tests wcscoll() API, The stpcpy() function copies bytes from
// string src to string dst.
// -----------------------------------------------------------------------------
//	
TInt CTestString::TstWcsCollUnlikestr2()
	{
	int ret;
    wchar_t *wcs1 = L"second";
   	wchar_t *wcs2 = L"first";
   	    
 	ret = validate(wcscoll(wcs1, wcs2) > 0);
 	if (ret == KErrNone)
 		{
 		INFO_PRINTF1(_L("String1 is greater than String2"));
 		}
 	else
 		{
   		INFO_PRINTF1(_L("String1 is less than String2"));	
   		}
   	return ret;	
	}

// -----------------------------------------------------------------------------
// Name : TstWcsxfrm
// Description: Tests wcsxfrm() API, The wcsxfrm() function transforms the
// wide-character string pointed to by a string pointer and places the resulting wide-character 
// string into the array pointed to by ws1.
// -----------------------------------------------------------------------------
//	
TInt CTestString::TstWcsxfrm()
	{
	wchar_t str1[MIN_LEN];
	wchar_t str2[MIN_LEN];
	wchar_t str3[MIN_LEN];
	wchar_t str4[MIN_LEN];
	
	if (mbstowcs(str1, "abcz", MIN_LEN) == (size_t)-1)
		{
		ERR_PRINTF1(_L("Error :mbstowcs()"));
		return KErrGeneral;
		}
	if (mbstowcs(str2, "abcd", MIN_LEN) == (size_t)-1)
		{
		ERR_PRINTF1(_L("Error :mbstowcs()"));
		return KErrGeneral;
		}
	int wcolres = wcscoll(str1, str2);
	//Transform the strings
	int result1 = wcsxfrm(str3, str1, MIN_LEN);
	
	int result2 = wcsxfrm(str4, str2, MIN_LEN);
	
	//Compare the two transformed string
	
	int wcmp = wcscmp(str3,str4);
	
	int ret = validate(wcolres == wcmp);
	if (ret == KErrNone)
		{
		INFO_PRINTF1(_L("wcsxfrm successful"));	
		}
	return ret;	
	}
	
// -----------------------------------------------------------------------------
// Name : TstWcsxfrmErr
// Description: Tests wcsxfrm() API, The wcsxfrm() function transforms the
// wide-character string pointed to by a string pointer and places the resulting wide-character 
// string into the array pointed to by ws1.
// -----------------------------------------------------------------------------
//	
TInt CTestString::TstWcsxfrmErr()
	{
	wchar_t str2[MIN_LEN];
//	wchar_t* str1;
	
	int ret = wcsxfrm(NULL, str2, MIN_LEN);
	
	return EPass;
	}
	
// -----------------------------------------------------------------------------
// Name : TstStrdup
// Description: Tests strdup() API, The strdup() function allocates memory and copies into it 
// the string addressed in the argument1, including the terminating null character
// -----------------------------------------------------------------------------
//	
TInt CTestString::TstStrdup()	
	{
	char* dest;
	
	dest = strdup("Hello");
	
	int ret = validate(strcmp(dest,"Hello")==0);
	
	if (dest == NULL)
		{
		if (errno == ENOMEM)
			{
			ERR_PRINTF1(_L("Insufficient memory"));
			}
		return KErrGeneral;
		}
	else if(ret==KErrNone)
		{
		INFO_PRINTF1(_L("strdup() success"));
		}
	free(dest);
	return ret;
	}
	
// -----------------------------------------------------------------------------
// Name : TstStrndup
// Description: Tests strndup() API, The strndup() function return a malloc'd 
// copy of at most the specified number of bytes of a string 
// -----------------------------------------------------------------------------
//	
TInt CTestString::TstStrndup()	
	{
	char* dest;
	TInt ret;
	TInt len = 5;
	dest = strndup("Hello",len);
	
	if (dest == NULL)
		{
		ERR_PRINTF2(_L("strndup() failure-error no:%d "),errno);
		return KErrGeneral;
		}
	else
		{
		ret = validate(strlen(dest)== len);
		if(ret == KErrNone)
			{
			INFO_PRINTF1(_L("strndup() success"));
			}
		}
	free(dest);
	return ret;
	}

// -----------------------------------------------------------------------------
// Name : TstWcsdup
// Description: Tests wcsdup() API, The wcsdup() function duplicates a 
// wide-character string
// -----------------------------------------------------------------------------
//	
TInt CTestString::TstWcsdup()
	{
	wchar_t* src;
	wchar_t dest[MIN_LEN];
	
	if (mbstowcs(dest, "abcz", MIN_LEN) == (size_t)-1)
		{
		ERR_PRINTF1(_L("Error :mbstowcs()"));
		return KErrGeneral;
		}
	src = wcsdup(dest);
	int r = wcscmp(src,dest);
	int ret = validate(src!= NULL);
	if (ret == KErrNone)
		{
		if (validate(wcscmp(src,dest)!= 0))
			{
			INFO_PRINTF1(_L("wcsdup() success"));
			}
		}
	else
		{
		ERR_PRINTF2(_L("Error :wcsdup()-error no.:"), errno);
		ret = KErrGeneral;
		}
	free(src);		
	return ret;
	}

// -----------------------------------------------------------------------------
// Name : TstStrsep
// Description: Tests strsep() API, The strsep() function duplicates a 
// wide-character string
// -----------------------------------------------------------------------------
//
TInt CTestString::TstStrsep()
	{
    const char string[] = "words spaces -- and, punctuation!";
    const char delimiters[] = " .,;:!-";
    char *running, *temp;
    char *token;
	
	running = strdup(string);
	temp = running;
	if (running == NULL)
		{
		INFO_PRINTF1(_L("strdup failure!"));
		return KErrGeneral;
		}
    token = strsep (&running, delimiters);    /* token => "words" */
   
    TBuf<MAX_LEN> buf;
    TInt len = strlen(token);
    
    for (TInt j =0; j<len;j++)
		{
    	buf.Append(token[j]);
		}
    INFO_PRINTF2(_L("strsep returned: %S"), &buf);
    token = strsep (&running, delimiters);    /* token => "spaces" */
    token = strsep (&running, delimiters);    /* token => "" */
    token = strsep (&running, delimiters);    /* token => "" */
    token = strsep (&running, delimiters);    /* token => "" */
    token = strsep (&running, delimiters);    /* token => "and" */
    token = strsep (&running, delimiters);    /* token => "" */
    token = strsep (&running, delimiters);    /* token => "punctuation" */
    token = strsep (&running, delimiters);    /* token => "" */
    token = strsep (&running, delimiters);    /* token => NULL */
    while (token != NULL)
    	{
    	token = strsep (&running, delimiters);    
    	}
	if (token == NULL)
		{
		INFO_PRINTF1(_L("No more tokens"));
		}
	free(temp);		
	return EPass;
	}
	
   /*
 	*-----------------------------------------------------
	*Test Case Name:Tstldexp()
	*API Name	   :ldexp()
	*Description   :load exponent of a floating point number.
	*Upon successful completion, ldexp() returns a double representing the value x multiplied by 2 raised to the power exp.
	*Note :To cover _copysign()
	*------------------------------------------------------
	*/


TInt CTestString::Tstldexp()
	{
	TPtrC Parameter1,Parameter2;
	TBuf8<100> buf1,buf2;
    _LIT( KParameter1, "Parameter1" );
    TBool res = GetStringFromConfig(ConfigSection(), KParameter1, Parameter1);
    buf1.Copy(Parameter1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    INFO_PRINTF1(_L("For normal input,Api Output is:\r\n"));
    double x = atof(input);
    _LIT( KParameter2, "Parameter2" );
    TBool res1 = GetStringFromConfig(ConfigSection(), KParameter2, Parameter2);
    buf2.Copy(Parameter2);
    char* input1 = (char*) buf2.Ptr();
    input1[buf2.Length()]='\0';
    INFO_PRINTF1(_L("For normal input,Api Output is:\r\n"));
    TInt y = atoi(input1);
	double ret=ldexp(x,y);
	if(ret==0)
		{
		return KErrGeneral;
		}
	return KErrNone;
	}

