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



#include "tlibcwchar.h"

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsnlen1L
// Example test method function.
// This test case should rerturn the actual length of the string as the second 
// parameter passed is greater than the actual lenght.
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::testwcsnlen1L(  )
    {
	wchar_t *ws=L"testwcsnlen";
    	
	size_t retval = wcsnlen(ws,14);
		
	INFO_PRINTF2(_L("testwcsnlen1 result is %d"),retval);
		
	if(retval < 14)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsnlen2L
// This test case should return the length of the string as specified by the
// second parameter not the actual length of the string.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsnlen2L(  )
    {
	wchar_t *ws=L"testwcsnlen";
    	
	size_t retval = wcsnlen(ws,6);
		
	INFO_PRINTF2(_L("testwcsnlen1 result is %d"),retval);
		
	if(retval == 6)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }
        
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsnlen3L
// This should return the actual length or the specified length as both actual 
// length and specified length both are same.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsnlen3L(  )
    {
	wchar_t *ws=L"testwcsnlen";
    	
	size_t retval = wcsnlen(ws,11);
		
	INFO_PRINTF2(_L("testwcsnlen1 result is %d"),retval);
		
	if(retval == 11)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsnlen4L
// This should return the length as 0 since it is a null string.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsnlen4L(  )
    {
	wchar_t *ws=L"";
    	
	size_t retval = wcsnlen(ws,6);
		
	INFO_PRINTF2(_L("testwcsnlen1 result is %d"),retval);
		
	if(retval == 0)
		{
		return KErrNone;
		}	
	else
		{
		return KErrGeneral;
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsnlen5L
// this should return the length 0 as the second parameter specified is 0.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsnlen5L(  )
    {
	wchar_t *ws=L"testwcsnlen";
    	
	size_t retval = wcsnlen(ws,0);
	
	INFO_PRINTF2(_L("testwcsnlen1 result is %d"),retval);
		
	if(retval == 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsdup1L
// this should generate the duplicate copy of the string passed to the wcsdup
// and the duplicated string will be stored in destws.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsdup1L(  )
    {
	wchar_t *srcws=L"testwcsdup";
    wchar_t *destws = NULL;
    	
	destws = wcsdup(srcws);
	    
	if(!(wcscmp(srcws,destws)))
		{
		free(destws);
		return KErrNone;
		}
	else
		{
		free(destws);
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsdup1L
// this should allocate a pointer to destws and the duplicated string should be
// a null string.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsdup2L(  )
    {
	wchar_t *srcws=L"";
    wchar_t *destws = NULL;
    	
	destws = wcsdup(srcws);
	    
	if(!(wcscmp(srcws,destws)))
		{
		free(destws);
		return KErrNone;
		}
	else
		{
		free(destws);
		return KErrGeneral;
		}
    }
   
   
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcscasecmp1L
// Example test method function.
// This test case should verify that the two wide char strings specified are 
// same even though the case for the two is different.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcscasecmp1L(  )
    {
	wchar_t *ws1=L"testcasecmp";
	wchar_t *ws2=L"TESTCASECMP";
    
	int retval = wcscasecmp(ws1,ws2);
		
	INFO_PRINTF2(_L("testwcscasecmp1 result is %d"),retval);
		
	if(retval == 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
   
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcscasecmp2L
// Example test method function.
// This test case should verify that the two wide char strings specified are 
// same.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcscasecmp2L(  )
    {
	wchar_t *ws1=L"testcasecmp";
	wchar_t *ws2=L"testcasecmp";
    
	int retval = wcscasecmp(ws1,ws2);
		
	INFO_PRINTF2(_L("testwcscasecmp2 result is %d"),retval);
		
	if(retval == 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcscasecmp3L
// Example test method function.
// This test case should verify that the two wide char strings specified are 
// not same as the second string contains less no. of chars than the first one.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcscasecmp3L(  )
    {
	wchar_t *ws1=L"testcasecmp";
	wchar_t *ws2=L"test";
    	
	int retval = wcscasecmp(ws1,ws2);
		
	INFO_PRINTF2(_L("testwcscasecmp3 result is %d"),retval);
		
	if(retval > 0)
		{
		return KErrNone;
		}
	else
		{
			return KErrGeneral; 
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcscasecmp4L
// Example test method function.
// This test case should verify that the two wide char strings specified are 
// not same as the second string contains less no. of chars than the first one.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcscasecmp4L(  )
    {
	wchar_t *ws1=L"test";
	wchar_t *ws2=L"testcasecmp";
    	
	int retval = wcscasecmp(ws1,ws2);
		
	INFO_PRINTF2(_L("testwcscasecmp4 result is %d"),retval);
		
	if(retval < 0)
		{
		return KErrNone;
		}
	else
		{
			return KErrGeneral; 
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcscasecmp5L
// Example test method function.
// This test case should verify that the two wide char strings specified are 
// not same as one is null string and another is non-null string.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcscasecmp5L(  )
    {
	wchar_t *ws1=L"";
	wchar_t *ws2=L"testcasecmp";
    	
	int retval = wcscasecmp(ws1,ws2);
		
	INFO_PRINTF2(_L("testwcscasecmp4 result is %d"),retval);
		
	if(retval < 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcscasecmp6L
// Example test method function.
// This test case should verify that the two wide char strings specified are 
// same as the two strings are null strings.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcscasecmp6L(  )
    {
	wchar_t *ws1=L"";
	wchar_t *ws2=L"";
    	
	int retval = wcscasecmp(ws1,ws2);
		
	INFO_PRINTF2(_L("testwcscasecmp4 result is %d"),retval);
		
	if(retval == 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
 
 
 
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsncasecmp1L
// Example test method function.
// This test case should verify that the two wide char strings specified are 
// same exccept the case for the two is different.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsncasecmp1L(  )
    {
	wchar_t *ws1=L"testcasecmp";
	wchar_t *ws2=L"TESTCASECMP";
    	
	int retval = wcsncasecmp(ws1,ws2,11);
		
	INFO_PRINTF2(_L("testwcsncasecmp1 result is %d"),retval);
		
	if(retval == 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
   
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsncasecmp2L
// Example test method function.
// This test case should verify that the two wide char strings specified are 
// same.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsncasecmp2L(  )
    {
	wchar_t *ws1=L"test";
	wchar_t *ws2=L"testCasecmp";
    	
	int retval = wcsncasecmp(ws1,ws2,11);
		
	INFO_PRINTF2(_L("testwcsncasecmp2 result is %d"),retval);
		
	if(retval < 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsncasecmp3L
// Example test method function.
// This test case should verify that the two wide char strings specified are 
// not same as the second string contains less no. of chars than the first one.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsncasecmp3L(  )
    {
	wchar_t *ws1=L"testcaseCmp";
	wchar_t *ws2=L"test";
    
	int retval = wcsncasecmp(ws1,ws2,11);
		
	INFO_PRINTF2(_L("testwcsncasecmp3 result is %d"),retval);
	
	if(retval > 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsncasecmp4L
// Example test method function.
// This test case should verify the string comparision operation, the no of chars
// to compare is 0 so, it should return 0.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsncasecmp4L(  )
    {
	wchar_t *ws1=L"testcaseCmp";
	wchar_t *ws2=L"test";
    	
	int retval = wcsncasecmp(ws1,ws2,0);
		
	INFO_PRINTF2(_L("testwcsncasecmp4 result is %d"),retval);
		
	if(retval == 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsncasecmp5L
// Example test method function.
// This test case should verify when two strings are null and the no of chars to
// compare is greater than 0 and the return value should be 0 as the two strings 
// match.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsncasecmp5L(  )
    {
	wchar_t *ws1=L"";
	wchar_t *ws2=L"";
    	
	int retval = wcsncasecmp(ws1,ws2,4);
		
	INFO_PRINTF2(_L("testwcsncasecmp5 result is %d"),retval);
		
	if(retval == 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
        
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsncasecmp6L
// Example test method function.
// This test case should verify the string comparision operation, when one string
// is non-null and another string is null, and it should return a negative value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsncasecmp6L(  )
    {
	wchar_t *ws1=L"testcaseCmp";
	wchar_t *ws2=L"";
    
	int retval = wcsncasecmp(ws1,ws2,5);
		
	INFO_PRINTF2(_L("testwcsncasecmp6 result is %d"),retval);
		
	if(retval > 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
        
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsncasecmp7L
// Example test method function.
// This test case should verify the string comparision operation, when one string
// is part of another string with different case and it should return 0 as the no.
// of chars comparing is length of the string which contains less chars than other.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsncasecmp7L(  )
    {
	wchar_t *ws1=L"testcaseCmp";
	wchar_t *ws2=L"TEST";
    	
	int retval = wcsncasecmp(ws1,ws2,4);
		
	INFO_PRINTF2(_L("testwcsncasecmp7 result is %d"),retval);
		
	if(retval == 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }        
    
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcpcpy1L
// API tested : wcpcpy
// this test case copies a string to dest frm src and returns a pointer to its end.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcpcpy1L(  )
    {
	wchar_t *src = L"testcase";
	wchar_t *dest = NULL;
	wchar_t *destEndPtr = L'\0';		
    	
    dest = (wchar_t *)malloc((wcslen(src)+1)*sizeof(wchar_t));
    
    if(dest != NULL)
		{
		destEndPtr = (wchar_t *)wcpcpy(dest,src);
		}
    else
		{
		return KErrGeneral;
		}
		
	INFO_PRINTF2(_L("testwcpcpy1 result is %d"),destEndPtr);
		
	if(!wcscmp(dest,src))
		{
		free(dest);
		return KErrNone;
		}
	else
		{
		free(dest);
		return KErrGeneral; 
		}
    }
    
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcpcpy2L
// API tested : wcpcpy
// this test case copies a string to dest frm src and returns a pointer to its end
// here src string is NULL.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcpcpy2L(  )
    {
	wchar_t *src = L"";
	wchar_t *dest = NULL;
	wchar_t *destEndPtr = L'\0';		
    	
    dest = (wchar_t *)malloc((wcslen(src)+1)*sizeof(wchar_t));
    	
    if(dest != NULL)
		{
		destEndPtr = (wchar_t *)wcpcpy(dest,src);
		}
    else
		{
    	return KErrGeneral;
		}
		
	INFO_PRINTF2(_L("testwcpcpy2 result is %c"),*destEndPtr);
		
	if(!wcscmp(dest,src))
		{
		free(dest);
		return KErrNone;
		}
	else
		{
		free(dest);
		return KErrGeneral; 
		}
    }
    
 
 
 
 
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcpncpy1L
// API tested : wcpncpy
// this test case copies first n characters of a wide character string frm src 
// to dest and returns a pointer to its end.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcpncpy1L(  )
    {
	wchar_t *src = L"testcase";
	wchar_t *dest = NULL;
	wchar_t *destEndPtr = L'\0';		
    	
    dest = (wchar_t *)malloc((wcslen(src)+1)*sizeof(wchar_t));
    	
    if(dest != NULL)
		{
		destEndPtr = (wchar_t *)wcpncpy(dest,src,9);
		}
    else
		{
    	return KErrGeneral;
		}
		
	INFO_PRINTF2(_L("testwcpncpy1 result is %d"),destEndPtr);
		
	if(!wcscmp(dest,src))
		{
		free(dest);
		return KErrNone;
		}
	else
		{
		free(dest);
		return KErrGeneral; 
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcpcpy2L
// API tested : wcpncpy
// this test case copies first n characters of a wide character string frm src 
// to dest, where n is greater than string length and returns a pointer to its end.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcpncpy2L(  )
    {
	wchar_t *src = L"test";
	wchar_t *dest = L'\0';
	wchar_t *destEndPtr = L'\0';		
    
    dest = (wchar_t *)malloc((9)*sizeof(wchar_t));
    
    if(dest != NULL)
		{
	    destEndPtr = (wchar_t *)wcpncpy(dest,src,9);
		}
    else
		{
    	return KErrGeneral;
		}
		
	INFO_PRINTF2(_L("testwcpncpy2 result is %c"),*destEndPtr);
		
	if(!wcscmp(dest,src))
		{
		free(dest);
		return KErrNone;
		}
	else
		{
		free(dest);
		return KErrGeneral; 
		}
    }
    
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcpcpy3L
// API tested : wcpncpy
// this test case copies first n characters of a wide character string frm src 
// to dest, where n is less than string length and returns a pointer to its end.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcpncpy3L(  )
    {
	wchar_t *src = L"test";
	wchar_t *dest;
	wchar_t *destEndPtr;		
    	
    dest = (wchar_t *)malloc((wcslen(src)+1)*sizeof(wchar_t));
    	
    if(dest != NULL)
		{
		destEndPtr = (wchar_t *)wcpncpy(dest,src,2);
		}
    else
		{
    	return KErrGeneral;
		}
		
	INFO_PRINTF2(_L("testwcpncpy3 result is %c"),*destEndPtr);
		
	if(!wcsncmp(dest,src,2))
		{
		free(dest);
		return KErrNone;
		}
	else
		{
		free(dest);
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcpcpy4L
// API tested : wcpncpy
// this test case copies first n characters of a wide character string frm src 
// to dest, where n is greater than string length and returns a pointer to its end.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcpncpy4L(  )
    {
	wchar_t *src = L"test";
	wchar_t *dest = L'\0';
	wchar_t *destEndPtr = L'\0';		
    
    dest = (wchar_t *)malloc((wcslen(src)+1)*sizeof(wchar_t));
    	
    if(dest != NULL)
		{
		destEndPtr = (wchar_t *)wcpncpy(dest,src,5);
		}
    else
		{	
    	return KErrGeneral;
		}
		
	INFO_PRINTF2(_L("testwcpncpy4 result is %c"),*destEndPtr);
		
	if(!wcscmp(dest,src))
		{
		free(dest);
		return KErrNone;
		}
	else
		{
		free(dest);
		return KErrGeneral; 
		}
    }
 
 
 
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsxfrm1L
// API tested : wcsxfrm
// transforms n wide characters from the src to dest.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsxfrm1L(  )
    {
		wchar_t *src = L"testcase";
		wchar_t *dest = NULL;
		int length;	
    	
    	dest = (wchar_t *)malloc((wcslen(src)+1)*sizeof(wchar_t));
    	
    	if(dest != NULL)
			{
			length = wcsxfrm(dest,src,9);
			}
    	else
			{
    		return KErrGeneral;
			}
		
		INFO_PRINTF2(_L("testwcsxfrm result is %d"),length);
		
		if((!wcsncmp(dest,src,8)) && length == 8)
			{
			free(dest);
			return KErrNone;
			}
		else
			{
			free(dest);
			return KErrGeneral; 
			}
    }
 
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcsxfrm2L
// API tested : wcsxfrm
// returns the length of the transformed string. if n is 0, then dest is is null
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcsxfrm2L(  )
    {
	wchar_t *src = L"testcase";
	int length;	
    	
    length = wcsxfrm(NULL,src,0);
    	
	INFO_PRINTF2(_L("testwcsxfrm result is %d"),length);
		
	if(length == 8)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 



// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul1L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a decimal unsigned long value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul1L(  )
    {
	wchar_t *src = L"478";
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoul1 result is %d"),longVal);
		
	if(longVal == 478)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul2L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a decimal unsigned long value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-decimal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul2L(  )
    {
	wchar_t *src = L"89ksr";
	wchar_t **limitStr = &src;//L"8";
	unsigned long  longVal;	
	
	longVal = wcstoul(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoul2 result is %d"),longVal);
		
	if(longVal == 89 ) //errno == EINVAL )
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul3L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a octal unsigned long value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul3L(  )
    {
	wchar_t *src = L"0456";
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoul3 result is %d"),longVal);
		
	if(longVal == 0456)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul14
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a octal unsigned long value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-octal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul4L(  )
    {
	wchar_t *src = L"04klm";
	wchar_t **limitStr = &src;
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,limitStr,8);
    	
	INFO_PRINTF2(_L("testwcstoul4 result is %d"),longVal);
		
	if(longVal == 04 ) 
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul5L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a hexadecimal unsigned long value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul5L(  )
    {
	wchar_t *src = L"0x4a8";
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoul5 result is %d"),longVal);
		
	if(longVal == 0x4a8)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul6L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a hexadecimal unsigned long value.
// here the src string is mixed string which contains both digits(0-9) letters(a-f)
// and chars.so,this will limit the conversion till a non-hexadecimal char occurs 
// while converting and the remaining string is stored in the limitStr.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul6L(  )
    {
	wchar_t *src = L"0x4agh1";
	wchar_t **limitStr = &src;
	unsigned long  longVal;	
		
    longVal = wcstoul(src,limitStr,16);
    	
	INFO_PRINTF2(_L("testwcstoul6 result is %d"),longVal);
		
	if(longVal == 0x4a ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul7L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a decimal unsigned long value.
// here the src string contains digits whose converted decimal value
// exceedes maximum unsigned long value (ULONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul7L(  )
    {
	wchar_t *src = L"987654321987987987";
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoul7 result is %d"),longVal);
		
	if(longVal == ULONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul8L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a octal unsigned long value.
// here the src string contains digits whose converted octal value
// exceedes maximum unsigned long value (ULONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul8L(  )
    {
	wchar_t *src = L"07654321765765765";
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoul8 result is %d"),longVal);
		
	if(longVal == ULONG_MAX)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul9L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a hexadecimal unsigned long value.
// here the src string contains digits(0-9) and letters (a-f) whose converted 
// hexa-decimal value exceedes maximum unsigned long value (ULONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul9L(  )
    {
	wchar_t *src = L"0x9abcdef9abcdef9abcdef";
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoul9 result is %d"),longVal);
		
	if(longVal == ULONG_MAX)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul10L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a decimal unsigned long value.
// here the src string contains digits which will be converted to decimal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul10L(  )
    {
	wchar_t *src = L"7987";
	unsigned long  longVal;	
    
    longVal = wcstoul(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoul10 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul11L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a octal unsigned long value.
// here the src string contains digits which will be converted to octal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul11L(  )
    {
	wchar_t *src = L"0765";
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoul11 result is %d"),longVal);
		
	if(longVal == 0765)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul12L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a hexadecimal unsigned long value.
// here the src string contains digits which will be converted to hexadecimal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul12L(  )
    {
	wchar_t *src = L"0x9abc";
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoul12 result is %d"),longVal);
		
	if(longVal == 0x9abc)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul13L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a decimal unsigned long value.
// here the src string contains digits with '-' sign which will be converted to 
// decimal value where the base value specified is 10. 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul13L(  )
    {
	wchar_t *src = L"-7987";
	unsigned long  longVal;	
    
    longVal = wcstoul(src,NULL,10);
    
	INFO_PRINTF2(_L("testwcstoul13 result is %d"),longVal);
	
	if(longVal ==(unsigned long) -7987)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul14L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a decimal unsigned long value.
// here the src string contains digits with '+' sign which will be converted to 
// decimal value where the base value specified is 10. 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul14L(  )
    {
	wchar_t *src = L"+7987";
	unsigned long  longVal;	
    	
    longVal = wcstoul(src,NULL,10);
    
	INFO_PRINTF2(_L("testwcstoul14 result is %d"),longVal);
	
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul15L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// this test case should set errno to EINVAL,because the base value specified is
// 38, which is out of range of base values.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul15L(  )
    {
	wchar_t *src = L"478";
	wchar_t **limitStr = &src;
	unsigned long  longVal;	
	
	longVal = wcstoul(src,limitStr,38);
    	
	INFO_PRINTF2(_L("testwcstoul2 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
	   	return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul16L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// when string specified is white-space character string, which is an invalid, so 
// the errno to be set to EINVAL.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul16L(  )
    {
	wchar_t *src = L"   ";
	wchar_t **limitStr = &src;
	unsigned long  longVal;	
		
	longVal = wcstoul(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoul16 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoul17L
// API tested : wcstoul
// convert a wide character string to unsigned long.
// when string specified is alphabetical string, which is an invalid,so 
// the errno to be set to EINVAL.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoul17L(  )
    {
	wchar_t *src = L"ksrtc";
	wchar_t **limitStr = &src;
	unsigned long  longVal;	
	
	longVal = wcstoul(src,limitStr,10);
    
	INFO_PRINTF2(_L("testwcstoul17 result is %d"),longVal);
		
	if(errno == EINVAL )
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 

//wcstoll API test cases :: Start

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll1L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a decimal long long value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll1L(  )
    {
	wchar_t *src = L"478";
	long long  longVal;	
    	
    longVal = wcstoll(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoll1 result is %d"),longVal);
		
	if(longVal == (long long)478)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll2L
// convert a wide character string to long long.
// this test case converts a wide char string to a decimal long long value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-decimal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll2L(  )
    {
	wchar_t *src = L"89ksr";
	wchar_t **limitStr = &src;
	long long  longVal;	
	
	longVal = wcstoll(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoll2 result is %d"),longVal);
		
	if(longVal == 89 ) 
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll3L
// API tested : wcstoll
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a octal unsigned long long value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll3L(  )
    {
	wchar_t *src = L"0456";
	long long  longVal;	
    	
    longVal = wcstoll(src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoll3 result is %d"),longVal);
		
	if(longVal == 0456)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll14
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a octal long long value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-octal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll4L(  )
    {
	wchar_t *src = L"04klm";
	wchar_t **limitStr = &src;
	long long  longVal;	
    	
     longVal = wcstoll(src,limitStr,8);
    	
	INFO_PRINTF2(_L("testwcstoll4 result is %d"),longVal);
		
	if(longVal == 04 ) 
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll5L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a hexadecimal long long value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll5L(  )
    {
	wchar_t *src = L"0x4a8";
	long long  longVal;	
    	
    longVal = wcstoll(src,NULL,16);
    
	INFO_PRINTF2(_L("testwcstoll5 result is %d"),longVal);
		
	if(longVal == 0x4a8)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll6L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a hexadecimal long long value.
// here the src string is mixed string which contains both digits(0-9) letters(a-f)
// and chars.so,this will limit the conversion till a non-hexadecimal char occurs 
// while converting and the remaining string is stored in the limitStr.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll6L(  )
    {
	wchar_t *src = L"0x4agh1";
	wchar_t **limitStr = &src;
	long long  longVal;	
		
    longVal = wcstoll(src,limitStr,16);
    	
	INFO_PRINTF2(_L("testwcstoll6 result is %d"),longVal);
		
	if(longVal == 0x4a ) 
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll7L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a decimal long long value.
// here the src string contains digits whose converted decimal value
// exceedes maximum unsigned long value (ULONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll7L(  )
    {
	wchar_t *src = L"987654321987987987987987987987987987987";
	long long  longVal;	
    	
    longVal = wcstoll(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoll7 result is %d"),longVal);
		
	if(longVal == LLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll8L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a octal long long value.
// here the src string contains digits whose converted octal value
// exceedes maximum long long value (LLONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll8L(  )
    {
	wchar_t *src = L"07654321765765765765765765765765765";
	long long  longVal;	
    	
    longVal = wcstoll(src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoll8 result is %d"),longVal);
		
	if(longVal == LLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll9L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a hexadecimal unsigned long value.
// here the src string contains digits(0-9) and letters (a-f) whose converted 
// hexa-decimal value exceedes maximum long long value (LLONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll9L(  )
    {
	wchar_t *src = L"0x9abcdef9abcdef9abcdef9abcdef9abcdef9abcdef";
	long long  longVal;	
    	
    longVal = wcstoll(src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoll9 result is %d"),longVal);
		
	if(longVal == LLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll10L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a decimal long long value.
// here the src string contains digits which will be converted to decimal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll10L(  )
    {
	wchar_t *src = L"7987";
	long long  longVal;	
    	
    longVal = wcstoll(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoll10 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll11L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a octal long long value.
// here the src string contains digits which will be converted to octal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll11L(  )
    {
	wchar_t *src = L"0765";
	long long  longVal;	
    
    longVal = wcstoll(src,NULL,0);
    
	INFO_PRINTF2(_L("testwcstoll11 result is %d"),longVal);
		
	if(longVal == 0765)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll12L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a hexadecimal long long value.
// here the src string contains digits which will be converted to hexadecimal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll12L(  )
    {
	wchar_t *src = L"0x9abc";
	long long  longVal;	
    	
    longVal = wcstoll(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoll12 result is %d"),longVal);
		
	if(longVal == 0x9abc)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll13L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a decimal long long value.
// here the src string contains digits with '-' sign which will be converted to 
// decimal value where the base value specified is 10. 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll13L(  )
    {
	wchar_t *src = L"-7987";
	long long  longVal;	
    
    longVal = wcstoll(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoll13 result is %d"),longVal);
		
	if(longVal == -7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll14L
// API tested : wcstoll
// convert a wide character string to long long.
// this test case converts a wide char string to a octal long long value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-octal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll14L(  )
    {
	wchar_t *src = L"+7987";
	long long  longVal;	
    	
    longVal = wcstoll(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoll14 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll15L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll15L(  )
    {
	wchar_t *src = L"478";
	wchar_t **limitStr = &src;
	long long  longVal;	
		
	longVal = wcstoll(src,limitStr,38);
    	
	INFO_PRINTF2(_L("testwcstoll2 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll16L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll16L(  )
    {
	wchar_t *src = L"   ";
	wchar_t **limitStr = &src;
	long long  longVal;	
		
	longVal = wcstoll(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoll16 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoll17L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoll17L(  )
    {
	wchar_t *src = L"ksrtc";
	wchar_t **limitStr = &src;
	long long  longVal;
	
	longVal = wcstoll(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoll17 result is %d"),longVal);
		
	if(errno == EINVAL )
		{
		 return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
//wcstoll API test cases :: End



//wcstoull API test cases :: Start

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull1L
// API tested : wcstoull
// convert a wide character string to unsigned long long
// this test case converts a wide char string to a decimal unsigned long long value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull1L(  )
    {
	wchar_t *src = L"478";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoull1 result is %d"),longVal);
	
	if(longVal == 478)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull2L
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a decimal unsigned long long value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-decimal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull2L(  )
    {
	wchar_t *src = L"89ksr";
	wchar_t **limitStr = &src;
	unsigned long long  longVal;	
	
	longVal = wcstoull(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoull2 result is %d"),longVal);
		
	if(longVal == 89 ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull3L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a octal unsigned long long value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull3L(  )
    {
	wchar_t *src = L"0456";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoull3 result is %d"),longVal);
		
	if(longVal == 0456)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull14
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a octal unsigned long long value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-octal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull4L(  )
    {
	wchar_t *src = L"04klm";
	wchar_t **limitStr = &src;
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,limitStr,8);
    	
	INFO_PRINTF2(_L("testwcstoull4 result is %d"),longVal);
		
	if(longVal == 04 ) 
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull5L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a hexadecimal unsigned long long 
// value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull5L(  )
    {
	wchar_t *src = L"0x4a8";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoull5 result is %d"),longVal);
		
	if(longVal == 0x4a8)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull6L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a hexadecimal unsigned long long 
// value here the src string is mixed string which contains both digits(0-9) 
// letters(a-f) and chars.so,this will limit the conversion till a non-hexadecimal 
// char occurs while converting and the remaining string is stored in the limitStr.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull6L(  )
    {
	wchar_t *src = L"0x4agh1";
	wchar_t **limitStr = &src;
	unsigned long long  longVal;	
	 
   	longVal = wcstoull(src,limitStr,16);
    	
	INFO_PRINTF2(_L("testwcstoull6 result is %d"),longVal);
		
	if(longVal == 0x4a ) 
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull7L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a decimal unsigned long long value.
// here the src string contains digits whose converted decimal value
// exceedes maximum unsigned long long value (ULLONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull7L(  )
    {
	wchar_t *src = L"987654321987987987987987987987987987987";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoull7 result is %d"),longVal);
		
	if(longVal == ULLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull8L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a octal unsigned long long value.
// here the src string contains digits whose converted octal value
// exceedes maximum unsigned long long value (ULLONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull8L(  )
    {
	wchar_t *src = L"07654321765765765765765765765765";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoull8 result is %d"),longVal);
		
	if(longVal == ULLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull9L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a hexadecimal unsigned long long
// value.here the src string contains digits(0-9) and letters (a-f) whose converted 
// hexa-decimal value exceedes maximum unsigned long long value (ULLONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull9L(  )
    {
	wchar_t *src = L"0x9abcdef9abcdef9abcdef";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoull9 result is %d"),longVal);
		
	if(longVal == ULLONG_MAX)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull10L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a decimal unsigned long long value.
// here the src string contains digits which will be converted to decimal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull10L(  )
    {
	wchar_t *src = L"7987";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoull10 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull11L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a octal unsigned long long value.
// here the src string contains digits which will be converted to octal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull11L(  )
    {
	wchar_t *src = L"0765";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoull11 result is %d"),longVal);
		
	if(longVal == 0765)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull12L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a hexadecimal unsigned long long 
// value.here the src string contains digits which will be converted to hexadecimal
// value where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull12L(  )
    {
	wchar_t *src = L"0x9abc";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoull12 result is %d"),longVal);
		
	if(longVal == 0x9abc)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull13L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull13L(  )
    {
	wchar_t *src = L"-7987";
	unsigned long long  longVal;	
    
    longVal = wcstoull(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoull13 result is %d"),longVal);
		
	if(longVal == (unsigned long long) -7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull14L
// API tested : wcstoull
// convert a wide character string to unsigned long long.
// this test case converts a wide char string to a octal unsigned long long value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-octal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull14L(  )
    {
	wchar_t *src = L"+7987";
	unsigned long long  longVal;	
    	
    longVal = wcstoull(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoull14 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull15L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull15L(  )
    {
	wchar_t *src = L"478";
	wchar_t **limitStr = &src;
	unsigned long long  longVal;	
		
	longVal = wcstoull(src,limitStr,38);
    	
	INFO_PRINTF2(_L("testwcstoull2 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
    
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull16L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull16L(  )
    {
	wchar_t *src = L"   ";
	wchar_t **limitStr = &src;
	unsigned long long  longVal;	
		
	longVal = wcstoull(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoull16 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoull17L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoull17L(  )
    {
	wchar_t *src = L"ksrtc";
	wchar_t **limitStr = &src;
	unsigned long long  longVal;	
	
	longVal = wcstoull(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoull17 result is %d"),longVal);
	
	if(errno == EINVAL )
		{
		 return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
//wcstoull API test cases :: End



//wcstoq API test cases :: Start

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq1L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a decimal long int value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq1L(  )
    {
	wchar_t *src = L"478";
	long long int  longVal;	
    	
    longVal = wcstoq(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoq1 result is %d"),longVal);
		
	if(longVal == 478)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq2L
// convert a wide character string to long int.
// this test case converts a wide char string to a decimal long int value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-decimal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq2L(  )
    {
	wchar_t *src = L"89ksr";
	wchar_t **limitStr = &src;//L"8";
	long long int  longVal;
	
	longVal = wcstoq(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoq2 result is %d"),longVal);
		
	
	if(longVal == 89 ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq3L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a octal long int value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq3L(  )
    {
	wchar_t *src = L"0456";
	long long int  longVal;	
    	
    longVal = wcstoq(src,NULL,8);
    
	INFO_PRINTF2(_L("testwcstoq3 result is %d"),longVal);
	
	if(longVal == 0456)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq4
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a octal long int value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-octal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq4L(  )
    {
	wchar_t *src = L"04klm";
	wchar_t **limitStr = &src;
	long long int  longVal;	
    	
     longVal = wcstoq(src,limitStr,8);
    	
	INFO_PRINTF2(_L("testwcstoq4 result is %d"),longVal);
		
	if(longVal == 04 ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq5L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a hexadecimal long int value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq5L(  )
    {
	wchar_t *src = L"0x4a8";
	long long int  longVal;	
    	
    longVal = wcstoq(src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoq5 result is %d"),longVal);
		
	if(longVal == 0x4a8)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq6L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a hexadecimal long int value.
// here the src string is mixed string which contains both digits(0-9) letters(a-f)
// and chars.so,this will limit the conversion till a non-hexadecimal char occurs 
// while converting and the remaining string is stored in the limitStr.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq6L(  )
    {
	wchar_t *src = L"0x4agh1";
	wchar_t **limitStr = &src;
	long long int  longVal;	
		
    longVal = wcstoq(src,limitStr,16);
    	
	INFO_PRINTF2(_L("testwcstoq6 result is %d"),longVal);
		
	if(longVal == 0x4a ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq7L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a decimal long int value.
// here the src string contains digits whose converted decimal value
// exceedes maximum long int value (LONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq7L(  )
    {
	wchar_t *src = L"987654321987987987987987987987987987987";
	long long int  longVal;	
    	
    longVal = wcstoq(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoq7 result is %d"),longVal);
		
	if(longVal == LLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq8L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a octal long int value.
// here the src string contains digits whose converted octal value
// exceedes maximum long value (LONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq8L(  )
    {
	wchar_t *src = L"07654321765765765765765765765765";
	long long int  longVal;	
    	
    longVal = wcstoq(src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoq8 result is %d"),longVal);
		
	if(longVal == LLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq9L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a hexadecimal long int value.
// here the src string contains digits(0-9) and letters (a-f) whose converted 
// hexa-decimal value exceedes maximum long int value (LONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq9L(  )
    {
	wchar_t *src = L"0x9abcdef9abcdef9abcdef";
	long long int  longVal;	
    	
    longVal = wcstoq(src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoq9 result is %d"),longVal);
		
	if(longVal == LLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq10L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a decimal long int value.
// here the src string contains digits which will be converted to decimal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq10L(  )
    {
	wchar_t *src = L"7987";
	long long int  longVal;	
    
    longVal = wcstoq(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoq10 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq11L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a octal long int value.
// here the src string contains digits which will be converted to octal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq11L(  )
    {
	wchar_t *src = L"0765";
	long long int  longVal;	
    
    longVal = wcstoq(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoq11 result is %d"),longVal);
	
	if(longVal == 0765)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq12L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a hexadecimal long int value.
// here the src string contains digits which will be converted to hexadecimal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq12L(  )
    {
	wchar_t *src = L"0x9abc";
	long long int  longVal;	
    	
    longVal = wcstoq(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoq12 result is %d"),longVal);
		
	if(longVal == 0x9abc)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq13L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a decimal long int value.
// here the src string contains digits with '-' sign which will be converted to 
// decimal value where the base value specified is 10. 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq13L(  )
    {
	wchar_t *src = L"-7987";
	long long int  longVal;	
    	
    longVal = wcstoq(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoq13 result is %d"),longVal);
		
	if(longVal == -7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq14L
// API tested : wcstoq
// convert a wide character string to long int.
// this test case converts a wide char string to a octal long int value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-octal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq14L(  )
    {
	wchar_t *src = L"+7987";
	long long int  longVal;	
    	
    longVal = wcstoq(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoq14 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq15L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq15L(  )
    {
	wchar_t *src = L"478";
	wchar_t **limitStr = &src;
	long long int  longVal;	
		
	longVal = wcstoq(src,limitStr,38);
    	
	INFO_PRINTF2(_L("testwcstoq2 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq16L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq16L(  )
    {
	wchar_t *src = L"   ";
	wchar_t **limitStr = &src;
	long long int  longVal;	
	
	longVal = wcstoq(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoq16 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoq17L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoq17L(  )
    {
	wchar_t *src = L"ksrtc";
	wchar_t **limitStr = &src;
	long long int  longVal;	
	
	longVal = wcstoq(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoq17 result is %d"),longVal);
		
	if(errno == EINVAL )
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
//wcstoq API test cases :: End




//wcstouq API test cases :: Start

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq1L
// API tested : wcstouq
// convert a wide character string to unsigned long int .
// this test case converts a wide char string to a decimal unsigned long int value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq1L(  )
    {
	wchar_t *src = L"478";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstouq1 result is %d"),longVal);
		
	if(longVal == 478)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq2L
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a decimal unsigned long int value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-decimal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq2L(  )
    {
	wchar_t *src = L"89ksr";
	wchar_t **limitStr = &src;
	unsigned long long int  longVal;	
	
	longVal = wcstouq(src,limitStr,10);
    
	INFO_PRINTF2(_L("testwcstouq2 result is %d"),longVal);
		
	if(longVal == 89 ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq3L
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a octal unsigned long int value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq3L(  )
    {
	wchar_t *src = L"0456";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstouq3 result is %d"),longVal);
		
	if(longVal == 0456)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq4
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a octal unsigned long int value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-octal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq4L(  )
    {
	wchar_t *src = L"04klm";
	wchar_t **limitStr = &src;
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,limitStr,8);
    	
	INFO_PRINTF2(_L("testwcstouq4 result is %d"),longVal);
		
	if(longVal == 04 ) 
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq5L
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a hexadecimal unsigned long int 
// value.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq5L(  )
    {
	wchar_t *src = L"0x4a8";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstouq5 result is %d"),longVal);
	
	if(longVal == 0x4a8)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq6L
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a hexadecimal unsigned long int value.
// here the src string is mixed string which contains both digits(0-9) letters(a-f)
// and chars.so,this will limit the conversion till a non-hexadecimal char occurs 
// while converting and the remaining string is stored in the limitStr.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq6L(  )
    {
	wchar_t *src = L"0x4agh1";
	wchar_t **limitStr = &src;
	unsigned long long int  longVal;	
		
    longVal = wcstouq(src,limitStr,16);
    	
	INFO_PRINTF2(_L("testwcstouq6 result is %d"),longVal);
		
	if(longVal == 0x4a ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq7L
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a decimal unsigned long int value.
// here the src string contains digits whose converted decimal value
// exceedes maximum unsigned long value (ULONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq7L(  )
    {
	wchar_t *src = L"987654321987987987987987987987987987987";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstouq7 result is %d"),longVal);
		
	if(longVal == ULLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq8L
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a octal unsigned long int value.
// here the src string contains digits whose converted octal value
// exceedes maximum unsigned long int value (ULONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq8L(  )
    {
	wchar_t *src = L"07654321765765765765765765765765";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstouq8 result is %d"),longVal);
		
	if(longVal == ULLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq9L
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a hexadecimal unsigned long value.
// here the src string contains digits(0-9) and letters (a-f) whose converted 
// hexa-decimal value exceedes maximum unsigned long int value (ULONG_MAX).
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq9L(  )
    {
	wchar_t *src = L"0x9abcdef9abcdef9abcdef";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstouq9 result is %d"),longVal);
		
	if(longVal == ULLONG_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq10L
// API tested : wcstouq
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a decimal unsigned long value.
// here the src string contains digits which will be converted to decimal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq10L(  )
    {
	wchar_t *src = L"7987";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstouq10 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq11L
// API tested : wcstouq
// convert a wide character string to unsigned long.
// this test case converts a wide char string to a octal unsigned long value.
// here the src string contains digits which will be converted to octal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq11L(  )
    {
	wchar_t *src = L"0765";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,0);
    
	INFO_PRINTF2(_L("testwcstouq11 result is %d"),longVal);
	if(longVal == 0765)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq12L
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a hexadecimal unsigned long value.
// here the src string contains digits which will be converted to hexadecimal value
// where the base value specified is 0. which means based on the string passed 
// based on the first char of the string the API will obtain the base.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq12L(  )
    {
	wchar_t *src = L"0x9abc";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstouq12 result is %d"),longVal);
	
	if(longVal == 0x9abc)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq13L
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a decimal unsigned long value.
// here the src string contains digits with '-' sign which will be converted to 
// decimal value where the base value specified is 10. 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq13L(  )
    {
	wchar_t *src = L"-7987";
	unsigned long longVal;	
    	
    longVal = wcstouq(src,NULL,10);
    
	INFO_PRINTF2(_L("testwcstouq13 result is %d"),longVal);
		
	if(longVal == (unsigned long)-7987)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq14L
// API tested : wcstouq
// convert a wide character string to unsigned long int.
// this test case converts a wide char string to a octal unsigned long int value.
// here the src string is mixed string which contains both digits and chars.so,
// this will limit the conversion till a non-octal char occurs while converting.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq14L(  )
    {
	wchar_t *src = L"+7987";
	unsigned long long int  longVal;	
    	
    longVal = wcstouq(src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstouq14 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq15L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq15L(  )
    {
	wchar_t *src = L"478";
	wchar_t **limitStr = &src;
	unsigned long long int  longVal;	
		
	longVal = wcstouq(src,limitStr,38);
    	
	INFO_PRINTF2(_L("testwcstouq2 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq16L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq16L(  )
    {
	wchar_t *src = L"   ";
	wchar_t **limitStr = &src;
	unsigned long long int  longVal;	
		
	longVal = wcstouq(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstouq16 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstouq17L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstouq17L(  )
    {
	wchar_t *src = L"ksrtc";
	wchar_t **limitStr = &src;//L"8";
	unsigned long long int  longVal;	
	
	longVal = wcstouq(src,limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstouq17 result is %d"),longVal);
		
	if(errno == EINVAL )
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
//wcstouq API test cases :: End



//wcstoumax API test cases :: Start

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax1L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax1L(  )
    {
	wchar_t *src = L"478";
	uintmax_t  longVal;	
    	
    longVal = wcstoumax((__wchar_t *)src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoumax1 result is %d"),longVal);
		
	if(longVal == 478)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax2L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax2L(  )
    {
	wchar_t *src = L"89ksr";
	wchar_t **limitStr = &src;
	uintmax_t  longVal;	
	
	longVal = wcstoumax((__wchar_t *)src,(__wchar_t **)limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoumax2 result is %d"),longVal);
		
	if(longVal == 89 ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax3L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax3L(  )
    {
	wchar_t *src = L"0456";
	uintmax_t  longVal;	
    	
    longVal = wcstoumax((__wchar_t *)src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoumax3 result is %d"),longVal);
		
	if(longVal == 0456)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax4
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax4L(  )
    {
	wchar_t *src = L"04klm";
	wchar_t **limitStr = &src;
	uintmax_t  longVal;	
    	
    longVal = wcstoumax((__wchar_t *)src,(__wchar_t **)limitStr,8);
    	
	INFO_PRINTF2(_L("testwcstoumax4 result is %d"),longVal);
		
	if(longVal == 04 ) 
		{
		return KErrNone;
		}
		else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax5L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax5L(  )
    {
	wchar_t *src = L"0x4a8";
	uintmax_t  longVal;	
    
    longVal = wcstoumax((__wchar_t *)src,NULL,16);
    
	INFO_PRINTF2(_L("testwcstoumax5 result is %d"),longVal);
		
	if(longVal == 0x4a8)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax6L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax6L(  )
    {
	wchar_t *src = L"0x4agh1";
	wchar_t **limitStr = &src;
	uintmax_t  longVal;	
	
    longVal = wcstoumax((__wchar_t *)src,(__wchar_t **)limitStr,16);
    	
	INFO_PRINTF2(_L("testwcstoumax6 result is %d"),longVal);
		
	if(longVal == 0x4a ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax7L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax7L(  )
    {
	wchar_t *src = L"987654321987987987987987987987987987987";
	uintmax_t  longVal;	
    	
    longVal = wcstoumax((__wchar_t *)src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoumax7 result is %d"),longVal);
		
	if(longVal == UINTMAX_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax8L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax8L(  )
    {
	wchar_t *src = L"07654321765765765765765765765765";
	uintmax_t  longVal;	
    
    longVal = wcstoumax((__wchar_t *)src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoumax8 result is %d"),longVal);
		
	if(longVal == UINTMAX_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax9L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax9L(  )
    {
	wchar_t *src = L"0x9abcdef9abcdef9abcdef";
	uintmax_t  longVal;	
    	
    longVal = wcstoumax((__wchar_t *)src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoumax9 result is %d"),longVal);
		
	if(longVal == UINTMAX_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax10L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax10L(  )
    {
	wchar_t *src = L"7987";
	uintmax_t  longVal;	
    	
    longVal = wcstoumax((__wchar_t *)src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoumax10 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax11L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax11L(  )
    {
	wchar_t *src = L"0765";
	uintmax_t  longVal;	
    	
    longVal = wcstoumax((__wchar_t *)src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoumax11 result is %d"),longVal);
		
	if(longVal == 0765)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax12L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax12L(  )
    {
	wchar_t *src = L"0x9abc";
	uintmax_t  longVal;	
    	
    longVal = wcstoumax((__wchar_t *)src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoumax12 result is %d"),longVal);
	if(longVal == 0x9abc)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax13L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax13L(  )
    {
	wchar_t *src = L"-7987";
	uintmax_t  longVal;	
    
    longVal = wcstoumax((__wchar_t *)src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoumax13 result is %d"),longVal);
		
	if(longVal == (uintmax_t) -7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax14L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax14L(  )
    {
	wchar_t *src = L"+7987";
	uintmax_t  longVal;	
    	
    longVal = wcstoumax((__wchar_t *)src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoumax14 result is %d"),longVal);
	
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax15L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax15L(  )
    {
	wchar_t *src = L"478";
	wchar_t **limitStr = &src;//L"8";
	uintmax_t  longVal;	
		
	longVal = wcstoumax((__wchar_t *)src,(__wchar_t **)limitStr,38);
    	
	INFO_PRINTF2(_L("testwcstoumax2 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax16L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax16L(  )
    {
	wchar_t *src = L"   ";
	wchar_t **limitStr = &src;//L"8";
	uintmax_t  longVal;	
		
	longVal = wcstoumax((__wchar_t *)src,(__wchar_t **)limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoumax16 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoumax17L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoumax17L(  )
    {
	wchar_t *src = L"ksrtc";
	wchar_t **limitStr = &src;//L"8";
	uintmax_t  longVal;	
	
	longVal = wcstoumax((__wchar_t *)src,(__wchar_t **)limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoumax17 result is %d"),longVal);
	
	if(errno == EINVAL )
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
//wcstoumax API test cases :: End



//wcstoimax API test cases :: Start

// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax1L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax1L(  )
    {
	wchar_t *src = L"478";
	intmax_t  longVal;	
    
    longVal = wcstoimax((__wchar_t *)src,NULL,10);
    
	INFO_PRINTF2(_L("testwcstoimax1 result is %d"),longVal);
		
	if(longVal == 478)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax2L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax2L(  )
    {
	wchar_t *src = L"89ksr";
	wchar_t **limitStr = &src;
	intmax_t  longVal;	
	
	longVal = wcstoimax((__wchar_t *)src,(__wchar_t **)limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoimax2 result is %d"),longVal);
		
	if(longVal == 89 )
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax3L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax3L(  )
    {
	wchar_t *src = L"0456";
	intmax_t  longVal;	
    	
    longVal = wcstoimax((__wchar_t *)src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoimax3 result is %d"),longVal);
		
	if(longVal == 0456)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax4
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax4L(  )
    {
	wchar_t *src = L"04klm";
	wchar_t **limitStr = &src;
	intmax_t  longVal;	
    	
    longVal = wcstoimax((__wchar_t *)src,(__wchar_t **)limitStr,8);
    	
	INFO_PRINTF2(_L("testwcstoimax4 result is %d"),longVal);
		
	if(longVal == 04 ) 
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax5L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax5L(  )
    {
	wchar_t *src = L"0x4a8";
	intmax_t  longVal;	
    	
    longVal = wcstoimax((__wchar_t *)src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoimax5 result is %d"),longVal);
		
	if(longVal == 0x4a8)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax6L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax6L(  )
    {
	wchar_t *src = L"0x4agh1";
	wchar_t **limitStr = &src;
	intmax_t  longVal;	
		
    longVal = wcstoimax((__wchar_t *)src,(__wchar_t **)limitStr,16);
    	
	INFO_PRINTF2(_L("testwcstoimax6 result is %d"),longVal);
		
	if(longVal == 0x4a ) //errno == EINVAL )
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax7L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax7L(  )
    {
	wchar_t *src = L"987654321987987987987987987987987987987";
	intmax_t  longVal;	
    	
    longVal = wcstoimax((__wchar_t *)src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoimax7 result is %d"),longVal);
		
	if(longVal == INTMAX_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax8L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax8L(  )
    {
	wchar_t *src = L"07654321765765765765765765765765";
	intmax_t  longVal;	
    	
    longVal = wcstoimax((__wchar_t *)src,NULL,8);
    	
	INFO_PRINTF2(_L("testwcstoimax8 result is %d"),longVal);
	
	if(longVal == INTMAX_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax9L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax9L(  )
    {
	wchar_t *src = L"0x9abcdef9abcdef9abcdef";
	intmax_t  longVal;	
    	
    longVal = wcstoimax((__wchar_t *)src,NULL,16);
    	
	INFO_PRINTF2(_L("testwcstoimax9 result is %d"),longVal);
		
	if(longVal == INTMAX_MAX)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax10L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax10L(  )
    {
	wchar_t *src = L"7987";
	intmax_t  longVal;	
    
    longVal = wcstoimax((__wchar_t *)src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoimax10 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax11L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax11L(  )
    {
	wchar_t *src = L"0765";
	intmax_t  longVal;	
    	
    longVal = wcstoimax((__wchar_t *)src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoimax11 result is %d"),longVal);
		
	if(longVal == 0765)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax12L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax12L(  )
    {
	wchar_t *src = L"0x9abc";
	intmax_t  longVal;	
    	
    longVal = wcstoimax((__wchar_t *)src,NULL,0);
    	
	INFO_PRINTF2(_L("testwcstoimax12 result is %d"),longVal);
		
	if(longVal == 0x9abc)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax13L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax13L(  )
    {
	wchar_t *src = L"-7987";
	intmax_t  longVal;	
    	
    longVal = (intmax_t)wcstoimax((__wchar_t *)src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoimax13 result is %d"),longVal);
		
	if(longVal == -7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
// -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax14L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax14L(  )
    {
	wchar_t *src = L"+7987";
	intmax_t  longVal;	
    
    longVal = wcstoimax((__wchar_t *)src,NULL,10);
    	
	INFO_PRINTF2(_L("testwcstoimax14 result is %d"),longVal);
		
	if(longVal == 7987)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax15L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax15L(  )
    {
	wchar_t *src = L"478";
	wchar_t **limitStr = &src;
	intmax_t  longVal;	
		
	longVal = wcstoimax((__wchar_t *)src,(__wchar_t **)limitStr,38);
    	
	INFO_PRINTF2(_L("testwcstoimax2 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
	   	return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
    
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax16L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax16L(  )
    {
	wchar_t *src = L"   ";
	wchar_t **limitStr = &src;
	intmax_t  longVal;	
		
	longVal = wcstoimax((__wchar_t *)src,(__wchar_t **)limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoimax16 result is %d"),longVal);
		
	if(errno == EINVAL)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
 // -----------------------------------------------------------------------------
// CTestLibcwchar::testwcstoimax17L
// Example test method function.
// 
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::testwcstoimax17L(  )
    {
	wchar_t *src = L"ksrtc";
	wchar_t **limitStr = &src;
	intmax_t  longVal;	
	
	longVal = wcstoimax((__wchar_t *)src,(__wchar_t **)limitStr,10);
    	
	INFO_PRINTF2(_L("testwcstoimax17 result is %d"),longVal);
		
	if(errno == EINVAL )
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral; 
		}
    }
 
//wcstoimax API test cases :: End


//mbsnrtowcs API test cases :: Start


// -----------------------------------------------------------------------------
//Function Name :testmbsnrtowcs1
//API Tested :mbsnrtowcs
//TestCase Description:mbsnrtowcs returns -> 4 (number of wide chars that make
//up the wide char string) when the input MB char string is of length 4.
//input --> MB char string "some".
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsnrtowcs1L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "some";
	unsigned int nms,len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
		
	nms = len1 = sizeof(wcbuf)/sizeof(*wcbuf);
		
	int len = mbsnrtowcs(wcbuf,&mbcbuf,nms,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 4 cccc \"some\"} %d %x"), len, wcbuf[5]);

	int i = 0;
	for(i = 0; wcbuf[i] != NULL; i++)
		{
		INFO_PRINTF2(_L(" %c"), wcbuf[i]);
	    }

	if (len != 4 || wcbuf[5] != 0xcccc || wcscmp(wcbuf, L"some") != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
	
    }


// -----------------------------------------------------------------------------
//Function Name :testmbsnrtowcs2
//API Tested :mbsnrtowcs
//TestCase Description:mbsnrtowcs returns -> 3 (number of wide chars that make
//up the wide char string) when the input MB char string is of length 4 but 
//not enough space in dest buffer.
//input --> MB char string "some" and n --> 3
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsnrtowcs2L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "some";
	unsigned int nms,len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
		
	nms = len1 = 3;
		
	int len = mbsnrtowcs(wcbuf, &mbcbuf, nms,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 3 cccc \"som\"} %d %x"), len, wcbuf[3]);

	int i = 0;
	for(i = 0; i < 4; i++)
		{
		INFO_PRINTF2(_L(" %c"), wcbuf[i]);
	    }

	if (len != 3 || wcbuf[3] != 0xcccc || wmemcmp(wcbuf, L"som", 3) != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbsnrtowcs3
//API Tested :mbsnrtowcs
//TestCase Description:mbsnrtowcs returns -> 0 (number of wide chars that make
//up the wide char string) when the source buffer (mbcbuf) is empty.
//input --> MB char string '\0'
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsnrtowcs3L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "";
	unsigned int nms,len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
	
	nms = len1 = 4;
		
	int len = mbsnrtowcs(wcbuf, &mbcbuf,nms,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 0 0} %d %x"), len, wcbuf[0]);
	    
	if (len != 0 || wcbuf[0] != 0)
		{
		return KErrGeneral;
		}
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbsnrtowcs4
//API Tested :mbsnrtowcs
//TestCase Description:mbsnrtowcs returns -> 6 (strlen(mbcbuf) + 1)
//when the dest buffer is NULL.
//input --> MB char string "hello" (5)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsnrtowcs4L()
    {
	const char *mbcbuf = "hello";
	unsigned int nms,len1;
	mbstate_t *ps = NULL;

	nms = len1 = 5;
		
	int len = mbsnrtowcs(NULL, &mbcbuf, nms,len1,ps);
	    
	INFO_PRINTF2(_L("{Expected: 5} %d"), len);
	   
	if (len != 5)
		{	
		return KErrGeneral;
		}
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbsnrtowcs5
//API Tested :mbsnrtowcs
//TestCase Description:mbsnrtowcs returns -> 0 (number of wide chars that make
//up the wide char string) when the dest buffer is of zero length.
//input --> MB char string "hello" and n --> 0
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsnrtowcs5L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "hello";
	unsigned int nms,len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
		
	nms = len1 = 0;
		
	int len = mbsnrtowcs(wcbuf, &mbcbuf, nms,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 0 cccc} %d %x"), len, wcbuf[0]);

	if (len != 0 || wcbuf[0] != 0xcccc)
		{
		return KErrGeneral;
		}
	return KErrNone;
    }

//mbsnrtowcs API test cases :: End


//wcsnrtombs API test cases :: Start
 
// -----------------------------------------------------------------------------
//Function Name :testwcsnrtombs1
//API Tested :wcsnrtombs
//TestCase Description:wcsnrtombs returns -> 4 (number of bytes written to 
//the multibyte char string) when the input wide char string occupies 4 bytes
//after encoding it as a multibyte char string.
//input --> wide char string L"some"
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwcsnrtombs1L()
    {
	const wchar_t *wcbuf = L"some";
	char mbcbuf[64];
	size_t nwc,len1;
	mbstate_t *ps = NULL;
	unsigned long lmask = 0x000000ff;

	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	nwc = len1 = sizeof(mbcbuf);
		
	int len = wcsnrtombs(mbcbuf, &wcbuf, nwc,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 4, cc, \"some\"} %d %x"), len, mbcbuf[5]);
	int i = 0;
	for(i = 0; mbcbuf[i] != NULL; i++)
		{
		INFO_PRINTF2(_L(" %c"), mbcbuf[i]);
	    }
		
	if (len != 4 || (mbcbuf[5] & lmask) != 0xcc || strcmp(mbcbuf, "some") != 0)
		{
		return KErrGeneral;
		}
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testwcsnrtombs2
//API Tested :wcsnrtombs
//TestCase Description:wcsnrtombs returns -> 3 (number of bytes written to the 
//multibyte char string) when there is no enough space in the dest buffer.
//input --> wide char string L"some" and nwc -> 3 len1 ->5
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwcsnrtombs2L()
    {
	const wchar_t *wcbuf = L"some";
	char mbcbuf[64];
	size_t nwc,len1;
	mbstate_t *ps = NULL;
	unsigned long lmask = 0x000000ff;
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));
	nwc = 3;len1 = 5;
		
	int len = wcsnrtombs(mbcbuf, &wcbuf, nwc,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 3, cc, \"som\"} %d %x"), len, mbcbuf[3]);

	int i = 0;
	for(i = 0; i < 4; i++)
		{
		INFO_PRINTF2(_L(" %c"), mbcbuf[i]);
	    }

	if (len != 3 || (mbcbuf[3] & lmask) != 0xcc || memcmp(mbcbuf, "som", 3) != 0)
		{
		return KErrGeneral;
		}
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testwcsnrtombs3
//API Tested :wcsnrtombs
//TestCase Description:wcsnrtombs returns -> 0 (number of bytes written to the
//multibyte char string) when the source buffer (wcbuf) is empty.
//input --> wide char string L'\0'
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwcsnrtombs3L()
    {
	const wchar_t *wcbuf = L"";
	char mbcbuf[64];
	size_t nwc,len1;
	mbstate_t *ps = NULL;
		
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	nwc = 3;len1 = 3;
		
	int len = wcsnrtombs(mbcbuf, &wcbuf, nwc,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 0 0} %d %x"), len, mbcbuf[0]);
	  
	if (len != 0 || mbcbuf[0] != 0)
		{	
		return KErrGeneral;
		}
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testwcsnrtombs4
//API Tested :wcsnrtombs
//TestCase Description:wcsnrtombs return the length of the converted string 
//when the dest buffer is NULL.
//input --> wide char string L"hello" (5)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwcsnrtombs4L()
    {
	const wchar_t *wcbuf = L"hello";
	size_t nwc,len1;
	mbstate_t *ps = NULL;
		
	nwc = len1 = 5;
		
	int len = wcsnrtombs(NULL, &wcbuf,nwc,len1,ps);// sizeof(wcbuf)/sizeof(*wcbuf));              
	    
	INFO_PRINTF2(_L("{Expected: 16} %d"), len);
	    
	if(len !=  5)
		{
		return KErrGeneral;
		}
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testwcsnrtombs5
//API Tested :wcsnrtombs
//TestCase Description:wcsnrtombs returns -> 0 (number of bytes written to the 
//multibyte char string) when the dest buffer is of zero length.
//input --> wide char string L"hello" and n --> 0
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwcsnrtombs5L()
    {
	const wchar_t *wcbuf = L"hello";
	char mbcbuf[64];
	size_t nwc,len1;
	mbstate_t *ps = NULL;
	unsigned long lmask = 0x000000ff;

	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	nwc = len1 = 0;
		
	int len = wcsnrtombs(mbcbuf, &wcbuf, nwc,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 0, cc} %d %x"), len, mbcbuf[0]);

	if (len != 0 || (mbcbuf[0] & lmask) != 0xcc)
		{
		return KErrGeneral;
		}
	return KErrNone;
    }
    
    
//wcsnrtombs API test cases :: End 

