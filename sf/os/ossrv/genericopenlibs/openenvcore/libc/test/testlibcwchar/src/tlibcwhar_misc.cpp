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
// CTestLibcwchar::wcslen1L
// wcslen test method function.
// This test function checks if the function can find out the lenght of a wide char code
// whose length is greater than 0. 
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcslen1L()
    {
    
   	wchar_t *ws=L"___test   ";
	size_t retval = wcslen(ws);
	
	
	INFO_PRINTF2(_L("wcslen1 result is %d"),retval);
	
	if(retval != 10)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
	}

// -----------------------------------------------------------------------------
// CTestLibcwchar::wcslen2L
// wcslen test method function.
// This test function checks if the API can find out the lenght of a wide char code
// whose length is 0. 
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcslen2L( )
    {
    
    wchar_t *ps=L"";
	size_t retval = wcslen(ps);
	INFO_PRINTF2(_L("wcslen2 result is %d"),retval);
	
    if(retval != 0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemcmp1L
// wmemcmp test method function.checks 
// This test function if the API returns 0 when the two wide 
// strings of same size under comparison are same till the number of the 3rd parameter
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemcmp1L(  )
    {
    
    wchar_t *ws1 = L"test case",*ws2 = L"test case";
	int retval;

	retval = wmemcmp(ws1,ws2,500);
	INFO_PRINTF2(_L("wmemcmp1 result is %d"),retval);
	
	if(retval != 0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemcmp2L
// wmemcmp test method function
// This test function checks if the API returns the differnce between the two wide 
// character strings of same size at the position of mismatch till the number of the 3rd parameter
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemcmp2L(  )
    {
    
    wchar_t *ws1=L"test case" ,*ws2 = L"test casg";
	int retval;

	retval = wmemcmp(ws1,ws2,15);
	INFO_PRINTF2(_L("wmemcmp2 result is %d"),retval);

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
// CTestLibcwchar::wcswidth1L
// wcswidth test method function 
// This test function checks if the API takes the value of n larger than the number 
// of characters in the string, it returns the value required by the entire wide char code
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcswidth1L(  )
    {
       
	wchar_t *ws1 = L"test case";
	int retval;

	retval = wcswidth(ws1,50);

	INFO_PRINTF2(_L("wcswidth1 result is %d"),retval);

	if(retval != 9)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
	}


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcswidth2L
// wcswidth test method function 
// This test function checks if the API takes the value of n equal to  than the number 
// of characters in the string, it returns the value required by the entire wide char code
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcswidth2L(  )
    {
       
	wchar_t *ws = L"test case";
	int retval;
	
	retval = wcswidth(ws,9);

	INFO_PRINTF2(_L("wcswidth2 result is %d"),retval);

	if(retval != 9)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcswidth3L
// wcswidth test method function 
// This test function checks if the API takes the value of n smaller than the number 
// of characters in the string, it returns the value required by the string till n chars 
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcswidth3L(  )
	{	
       
	wchar_t *ws = L"test case";
	int retval;
	
	retval = wcswidth(ws,5);

	INFO_PRINTF2(_L("wcswidth3 result is %d"),retval);

	if(retval != 5)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcswidth4L
// wcswidth test method function 
// This test function checks if the API takes the wide char code as a null string,
// it returns the value as 0.  
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcswidth4L(  )
    {
       
	wchar_t *ws = L"";
	int retval;
	
	retval = wcswidth(ws,5);

	INFO_PRINTF2(_L("wcswidth4 result is %d"),retval);
	
	if(retval != 0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcwidth1L
// wcwidth test method function 
// This test function checks if the API takes any wide char, it retunrns the number of
// column positions required by the wide char
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcwidth1L(  )
    {
       
	wchar_t wc = L'a';
	int retval;
	
	retval = wcwidth(wc);

	INFO_PRINTF2(_L("wcwidth1 result is %d"),retval);
	
	if(retval != 1)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcwidth2L
// wcwidth test method function 
// This test function checks if the API takes null wide char, it retunrns 0
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcwidth2L(  )
    {
       
	wchar_t wc = L'\0';
	int retval;

	retval = wcwidth(wc);

	INFO_PRINTF2(_L("wcwidth2 result is %d"),retval);
	
	if(retval != 0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemchr1L
// wmemchr test method function 
// This test function checks if the API takes any character that is the wide char code 
// and when n is such that the character lies in the range of n, the API retunrs the 
// pointer to the wide char string at the position of the wide char
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemchr1L(  )
    {

    wchar_t ws1[50]=L"abcdefghij",*retval;
	
	retval = wmemchr(ws1,L'c',10);

	INFO_PRINTF1(_L("wmemchr1 result is"));

	for(int i=0;i<9;i++)
		{
		INFO_PRINTF3(_L("retval[%d]=%x"),i,retval[i]);
		if(retval[i] != ws1[i+2])
			{
			return KErrGeneral;
			}
		}
	
	return KErrNone;
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemchr2L
// wmemchr test method function 
// This test function checks if the API takes any character that is not inn the wide char,
// the API retunrs a null pointer
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemchr2L(  )
    {

    wchar_t ws1[50]=L"abcdefghij",*retval;
	
	retval = wmemchr(ws1,L'x',50);

	INFO_PRINTF2(_L("wmemchr2 return is %d"),retval);

	if(retval != NULL)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemcpy1L
// wmemcpy test method function 
// This test function checks if the API copies the string n chacters from ws2 to 
// the string pointed by ws1 when ws1 is larger string than ws2
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemcpy1L(  )
    {

    wchar_t ws1[50]=L"abcdefghij",*retval,ws2[50]=L"test";
	
	retval = wmemcpy(ws1,ws2,6);

	INFO_PRINTF1(_L("wmemcpy1 result is"));

	for(int i=0;i<6;i++)
		{
		INFO_PRINTF7(_L("retval[%d]=%x ws1[%d]=%x ws2[%d]=%x"),i,retval[i],i,ws1[i],i,ws2[i]);
		if(retval[i] != ws2[i] || ws1[i] != ws2[i])
			{
			return KErrGeneral;
			}
		}

	return KErrNone;
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemcpy2L
// wmemcpy test method function 
// This test function checks if the API copies the string n chacters from ws2 to 
// the string pointed by ws1 when ws2 is larger string than ws1
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemcpy2L(  )
    {

    wchar_t ws1[50]=L"abcd",*retval,ws2[50]=L"testcase";
	
	retval = wmemcpy(ws1,ws2,5);

	INFO_PRINTF1(_L("wmemcpy2 result is"));

	for(int i=0;i<5;i++)
		{
		INFO_PRINTF7(_L("retval[%d]=%x ws1[%d]=%x ws2[%d]=%x"),i,retval[i],i,ws1[i],i,ws2[i]);
		if(retval[i] != ws2[i] || ws1[i] != ws2[i])
			{
			return KErrGeneral;
			}
		}

	return KErrNone;
    
    }
    



// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemcpy3L
// wmemcpy test method function 
// This test function checks if the API copies the n characters to an unitialised string
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemcpy3L(  )
    {

    wchar_t ws1[50]=L"abcdegghij",*retval,ws2[50];
	
	retval = wmemcpy(ws2,ws1,7);

	INFO_PRINTF1(_L("wmemcpy3 result is"));

	for(int i=0;i<7;i++)
		{
		INFO_PRINTF7(_L("retval[%d]=%x ws1[%d]=%x ws2[%d]=%x"),i,retval[i],i,ws1[i],i,ws2[i]);
		if(retval[i] != ws2[i] || ws1[i] != ws2[i])
			{
			return KErrGeneral;
			}
		}

	return KErrNone;
    
    }
    
// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemset1L
// wmemset test method function 
// This test function checks if the API sets the charater in ws1 till the value of 
// n<=size of string
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemset1L(  )
    {

    wchar_t ws1[50]=L"abcdefghij",*retval;
	
	retval = wmemset(ws1,L'a',7);

	INFO_PRINTF1(_L("wmemset1 result is"));

	int i;

	for(i=0;i<10;i++)
		{
		INFO_PRINTF5(_L("retval[%d]=%x ws1[%d]=%x"),i,retval[i],i,ws1[i]);
		}

	for(i=0;i<7;i++)
		{
		if(ws1[i] != L'a')
			{
			return KErrGeneral;
			}
		}
	
	return KErrNone;
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemset2L
// wmemset test method function 
// This test function checks if the API sets the charater in ws1 till the value of 
// n > size of string
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemset2L(  )
    {

    wchar_t ws1[50]=L"abcdefghij",*retval;
	
	retval = wmemset(ws1,L'a',21);

	INFO_PRINTF1(_L("wmemset2 result is"));

	for(int i=0;i<21;i++)
		{
		INFO_PRINTF5(_L("retval[%d]=%x ws1[%d]=%x"),i,retval[i],i,ws1[i]);
		if(ws1[i] != L'a')
			{
			return KErrGeneral;
			}
		}

	return KErrNone;
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemset3L
// wmemset test method function 
// This test function checks if the API sets the charater in ws1 when ws1 is unintialized
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemset3L(  )
    {

    wchar_t ws1[50]=L"abcdefghij",*retval,ws2[50];
	
	retval = wmemset(ws2,ws1[2],5);
	
	INFO_PRINTF1(_L("wmemset3 result is"));

	for(int i=0;i<5;i++)
		{
		INFO_PRINTF5(_L("retval[%d]=%x ws2[%d]=%x"),i,retval[i],i,ws2[i]);
		if(ws2[i] != ws1[2])
			{
			return KErrGeneral;
			}
		}

	return KErrNone;
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemmove1L
// wmemmove test method function 
// This test function checks if the API copies n chacters from ws2 to 
// the string pointed by ws1 where n <= length of ws1 and ws1 and ws2 
// do not overlap
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wmemmove1L(  )
    {

    wchar_t ws1[50]=L"abcdefghij",*retval,ws2[5] = L"abc";
		
	retval = wmemmove(ws1,ws2,3);

	INFO_PRINTF1(_L("wmemmove1 result is"));

	int i;

	for(i=0;i<10;i++)
		{
		INFO_PRINTF5(_L("retval[%d]=%x ws1[%d]=%x"),i,retval[i],i,ws1[i]);
		}

	for(i=0;i<3;i++)
		{
		if(ws1[i] != ws2[i])
			{
			return KErrGeneral;
			}
		}

	return KErrNone;
    
    }
    

// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemmove2L
// wmemmove test method function 
// This test function checks if the API copies n chacters from ws2 to 
// the string pointed by ws1 where n > length of ws1 and ws1 and ws2 
// do not overlap
// -----------------------------------------------------------------------------
//


TInt CTestLibcwchar::wmemmove2L(  )
    {
	
	wchar_t ws1[50]=L"abcde",*retval,ws2[50] = L"test case";
	
	
	retval = wmemmove(ws1,ws2,11);

	INFO_PRINTF1(_L("wmemmove2 result is"));

	for(int i=0;i<11;i++)
		{
		INFO_PRINTF5(_L("retval[%d]=%x ws1[%d]=%x"),i,retval[i],i,ws1[i]);
		if(ws1[i] != ws2[i])
			{
			return KErrGeneral;
			}
		}

	return KErrNone;
    
	}



// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemmove3L
// wmemmove test method function 
// This test function checks if the API copies n chacters from ws2 to an uninitailized
// wide char array ws1 and ws1 and ws2 do not overlap
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::wmemmove3L(  )
    {
	
	wchar_t ws1[50],*retval,ws2[50] = L"test case";
	
	retval = wmemmove(ws1,ws2,11);

	INFO_PRINTF1(_L("wmemmove3 result is"));

	for(int i=0;i<11;i++)
		{
		INFO_PRINTF5(_L("retval[%d]=%x ws1[%d]=%x"),i,retval[i],i,ws1[i]);
		if(ws1[i] != ws2[i])
			{
			return KErrGeneral;
			}
		}

	return KErrNone;
 
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wmemmove4L
// Description:wmemmove test method function 
// This test function checks if the API copies n characters from ws2 to ws1 where ws1
// and ws2 overlap.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::wmemmove4L(  )
    {
	
	wchar_t ws1[50]=L"abcdefghij",*retval,*ws2,temp[50];
		
	ws2 = ws1 + 2; // to make the two memory area ws1 and ws2 overlap

	wcscpy(temp,ws2);

	retval = wmemmove(ws1,ws2,7);

	INFO_PRINTF1(_L("wmemmove4 result is"));

	int i;

	for(i=0;i<10;i++)
		{
		INFO_PRINTF5(_L("ws1[%d]=%x retval[%d]=%x"),i,ws1[i],i,retval[i]);
		}

	for(i=0;i<7;i++)
		{
		if(ws1[i] != temp[i] || retval[i] != temp[i])
			{
			return KErrGeneral;
			}
		}

	return KErrNone;

    }

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstol1L
	API Tested				:	Wcstol()
	TestCase Description	:	Test where input wide code character cannot be completely converted to long.
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcstol1L(  )
	{	 

    INFO_PRINTF1(_L("started Wcstol  1")); //was _L8
	wchar_t wcs1[41] = L"123.45abcd";
	wchar_t wcs2[8]=L".45abcd";
	wchar_t *eptr;
	long l;

    errno = 0;
    l = wcstol(wcs1, &eptr, 0);

    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long integer: %d and end pointer is set to the Wchar string: %ls"),wcs1,l, eptr);
          
    if((l== 123L) && !(wcscmp (eptr, wcs2)) && (errno==0))
		{
   		return KErrNone;
		}
    else
		{
     	return KErrGeneral;
		}
  
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstol2L
	API Tested				:	Wcstol()
	TestCase Description	:	Test where input wide code character contains a long which is out of range.
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcstol2L(  )
	{	 
    INFO_PRINTF1(_L("started Wcstol  2")); //was _L8
	wchar_t wcs1[41] = L"2147483649abcd";
	wchar_t *eptr;
	long l , long_max = 2147483647;

    errno = 0;
    l = wcstol(wcs1, &eptr, 0);

    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long integer: %d and end pointer is set to the Wchar string: %ls"),wcs1,l, eptr);
         
    if((l==long_max)&&(errno==ERANGE))
   		{
		return KErrNone;
		}
    else
		{
		return KErrGeneral;
		}
    
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstol3L
	API Tested				:	Wcstol()
	TestCase Description	:	Test where input wide code character which can not be converted to long
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcstol3L(  )
	{	 
	INFO_PRINTF1(_L("started Wcstol  3")); //was _L8
	wchar_t wcs1[41] = L"abcdefg";
	wchar_t *eptr;
	long l;

    l = wcstol(wcs1, &eptr, 0);
   
    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long integer: %d and end pointer is set to the Wchar string: %ls"),wcs1,l, eptr);
         
    if((l == 0L) && (!wcscmp(eptr, wcs1)) )
		{
		return KErrNone;
		}
    else
		{
   		return KErrGeneral;
		}
     
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstol4L
	API Tested				:	Wcstol()
	TestCase Description	:	Test where end pointer is NULL
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcstol4L(  )
	{	 
    INFO_PRINTF1(_L("started Wcstol 4")); //was _L8
	wchar_t wcs1[41] = L"123abcdefg";
	wchar_t **eptr;
	long l;
    eptr = (wchar_t**)NULL;
        
    l = wcstol(wcs1, eptr, 0);
     
  	INFO_PRINTF3(_L("Input Wchar string: %ls is converted to a long integer: %d"),wcs1,l);  
      
        
    if((l == 123L) && (eptr == NULL) )
		{
   	    return KErrNone;
		}
     else
		{
     	return KErrGeneral;    
		}
         
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstol5L
	API Tested				:	Wcstol()
	TestCase Description	:	Test where base is beween 2 to 36
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcstol5L(  )
	{	 
    INFO_PRINTF1(_L("started Wcstol  5")); //was _L8
	wchar_t wcs1[41] = L"453abcdefg";
	wchar_t wcs2[41] = L"abcdefg";
	wchar_t *eptr;
	long l;
   
    l = wcstol(wcs1, &eptr, 6);
      
    if((l == 177L) && (!wcscmp(eptr, wcs2) ))
		{
     	
     	INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long integer:%d and end pointer is set to the Wchar string: %ls"),wcs1,l, eptr);
     	return KErrNone;
		}   	    
     else 
		{ 
     	return KErrGeneral;   
  		}
   
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstol6L
	API Tested				:	Wcstol()
	TestCase Description	:	Test where base is beween 2 to 36 and the number cannot be converted to specified base
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcstol6L(  )
	{	 
    INFO_PRINTF1(_L("started Wcstol  6")); //was _L8
	wchar_t wcs1[41] = L"458abcdefg";
	wchar_t wcs2[41] = L"458abcdefg";
	wchar_t *eptr;
	long l;


    l = wcstol(wcs1, &eptr, 3);

    if((l == 0L) && (!wcscmp(eptr, wcs2) ))
		{
     	INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long integer: %d and end pointer is set to the Wchar string: %ls"),wcs1,l, eptr);
     	return KErrNone;
		}   	    
    else 
		{ 
     	return KErrGeneral;   
  		}
             
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstol7L
	API Tested				:	Wcstol()
	TestCase Description	:	Test where base is beween 2 to 36 and the part ofthe number cannot be converted to specified base
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcstol7L(  )
	{	 
	INFO_PRINTF1(_L("started Wcstol  7")); //was _L8
	wchar_t wcs1[41] = L"4583abcdefg";
	wchar_t wcs2[41] = L"83abcdefg";
	wchar_t *eptr;
	long l;


    l = wcstol(wcs1, &eptr, 6);     
    
    if((l==29L)&&(!wcscmp(eptr, wcs2) ))
		{
     	INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long integer: %d and end pointer is set to the Wchar string: %ls"),wcs1,l, eptr);
     	 
     	return KErrNone;
		}
 
     else 
		{ 
     	return KErrGeneral;   
  		}           
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstol8L
	API Tested				:	Wcstol()
	TestCase Description	:	Test where input wchar string contains hex value
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcstol8L(  )
	{	 
    INFO_PRINTF1(_L("started Wcstol  8")); //was _L8
	wchar_t wcs1[41] = L"0X4aGHJK";
	wchar_t wcs2[41] = L"GHJK";
	wchar_t *eptr;
	long l;


    l = wcstol(wcs1, &eptr, 0);

    if((l==74L)&&!wcscmp(eptr, wcs2) )
		{
     	INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long integer: %d and end pointer is set to the Wchar string: %ls"),wcs1,l, eptr);
     	 
     	return KErrNone;
		}   	    
    else 
		{ 
     	return KErrGeneral;   
  		}
               
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstol9L
	API Tested				:	Wcstol()
	TestCase Description	:	Test where input wchar string contains octal value
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcstol9L(  )
	{	 
    INFO_PRINTF1(_L("started Wcstol  8")); //was _L8
	wchar_t wcs1[41] = L"045aGHJK";
	wchar_t wcs2[41] = L"aGHJK";
	wchar_t *eptr;
	long l;


    l = wcstol(wcs1, &eptr, 0);

     
    if((l==37L)&&!wcscmp(eptr, wcs2) )
		{
     	INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long integer: %d and end pointer is set to the Wchar string: %ls"),wcs1,l, eptr);
     	 
     	return KErrNone;
		}
   	    
     else 
		{ 
     	return KErrGeneral;   
  		}
              
	}


//test cases for API mbsinit : Start

// -----------------------------------------------------------------------------
//Function Name :testmbinit1
//API Tested :mbsinit
//TestCase Description:mbrinit returns -> non-zero if initial state is set.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::testmbsinit1L()
	{
	mbstate_t state;
	    
	memset(&state,0,sizeof(mbstate_t));
	    
	int retVal = mbsinit(&state);
	    
	if(retVal != 0)
		{
		return KErrNone;	
		}
			
	return KErrGeneral;
    
    }

// -----------------------------------------------------------------------------
//Function Name :testmbinit2
//API Tested :mbsinit
//TestCase Description:mbrinit returns -> non-zero if mbstate_t state is a null
// pointer.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::testmbsinit2L()
    {
	    
		int retVal = mbsinit(NULL);
	    
	    if(retVal != 0)
			{
			return KErrNone;	
			}
			
		return KErrGeneral;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :testmbinit3
//API Tested :mbsinit
//TestCase Description:mbrinit returns -> zero if mbstate_t state is not a null
// pointer.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsinit3L()
    {
	    mbstate_t state;
	    
	    int retVal = mbsinit(&state);
	    
	    if(retVal == 0)
			{
			return KErrNone;	
			}
			
		return KErrGeneral;
    
    }

//test cases for API mbsinit : End



//test cases for API wctype : Start

// -----------------------------------------------------------------------------
//Function Name :testwctype1
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype1L()
	{
	wctype_t type;
	  
	type = wctype("alnum");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	 if(iswctype(L'k',type))
		{
	    return KErrNone;	
		}
			
	return KErrGeneral;
    
    }

// -----------------------------------------------------------------------------
//Function Name :testwctype2
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype2L()
    {
	wctype_t type;
	  
	type = wctype("alnum");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype((wint_t)L'6',type))
		{
	    return KErrNone;	
		}
			
	return KErrGeneral;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :testwctype3
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::testwctype3L()
    {
	wctype_t type;
	  
	type = wctype("alpha");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype(L'k',type))
		{
	    return KErrNone;	
		}
			
	return KErrGeneral;
    
    }
 
// -----------------------------------------------------------------------------
//Function Name :testwctype4
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.(negative test case)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype4L()
    {
	wctype_t type;
	  
	type = wctype("alpha");
	  
	if(type == (wctype_t)0)
		{
		return KErrGeneral;
		}
	  
	if(iswctype(L'5',type))
		{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
		
    }

// -----------------------------------------------------------------------------
//Function Name :testwctype5
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype5L()
    {
	wctype_t type;
	  
	type = wctype("blank");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype(L' ',type))
		{
	    return KErrNone;	
		}
			
	return KErrGeneral;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :testwctype6
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype6L()
    {
	wctype_t type;
	  
	type = wctype("blank");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype(L'k',type))
		{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
	
    }


// -----------------------------------------------------------------------------
//Function Name :testwctype7
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype7L()
    {
	wctype_t type;
	  
	type = wctype("digit");
	  
	if(type == (wctype_t)0)
		{
		return KErrGeneral;
		}
	  
	if(iswctype(L'9',type))
		{
	    return KErrNone;	
		}
			
	return KErrGeneral;
    
    }

// -----------------------------------------------------------------------------
//Function Name :testwctype8
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype8L()
    {
	wctype_t type;
	  
	type = wctype("digit");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype(L'g',type))
		{
	  	return KErrGeneral;
		}
	 
	return KErrNone;	
	
    }
    
// -----------------------------------------------------------------------------
//Function Name :testwctype9
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype9L()
    {
	wctype_t type;
	  
	type = wctype("lower");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype((wint_t)L'k',type))
		{
	    return KErrNone;	
		}
			
	return KErrGeneral;
    
    }
 
// -----------------------------------------------------------------------------
//Function Name :testwctype10
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.(negative test case)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype10L()
    {
	wctype_t type;
	  
	type = wctype("lower");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype(L'K',type))
		{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
		
    }

// -----------------------------------------------------------------------------
//Function Name :testwctype11
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype11L()
    {
	wctype_t type;
	  
	type = wctype("punct");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype((wint_t)L'!',type))
		{
	    return KErrNone;	
		}
			
	return KErrGeneral;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :testwctype12
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype12L()
    {
	wctype_t type;
	  
	type = wctype("punct");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype(L'k',type))
		{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
	
    }

// -----------------------------------------------------------------------------
//Function Name :testwctype13
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype13L()
    {
	wctype_t type;
	  
	type = wctype("upper");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype((wint_t)L'K',type))
		{
	  	return KErrNone;
		}

    return KErrGeneral;
    }
 
// -----------------------------------------------------------------------------
//Function Name :testwctype14
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.(negative test case)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype14L()
    {
	wctype_t type;
	  
	type = wctype("upper");
	  
	if(type == (wctype_t)0)
	  	{
	  	return KErrGeneral;
		}
	  
	if(iswctype(L'l',type))
	  	{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
		
    }

// -----------------------------------------------------------------------------
//Function Name :testwctype15
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype15L()
    {
	wctype_t type;
	  
	type = wctype("xdigit");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype('d',type))
		{
	    return KErrNone;	
		}
			
	return KErrGeneral;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :testwctype16
//API Tested :wctype
//TestCase Description:wctype returns -> property descriptor if name is valid
//this prop desc can be used by iswctype function to test the wide char passed
// to it is of type returned by the wctype fun.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype16L()
    {
	wctype_t type;
	  
	type = wctype("xdigit");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	if(iswctype(6,type))
		{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
	
    }

// -----------------------------------------------------------------------------
//Function Name :testwctype17
//API Tested :wctype
//TestCase Description:wctype returns ->0 as property descriptor as the name
//parameter passed is invalid one. 
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctype17L()
    {
	wctype_t type;
	  
	type = wctype("kkkkit");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrNone;
		}
	  
	return KErrGeneral;
	  
    }



//test cases for API wctype : End

//test cases for API wctrans and towctrans : Start

// -----------------------------------------------------------------------------
//Function Name :testwctrans1
//API Tested :wctrans
//TestCase Description:wctrans returns -> property descriptor if name is valid
//this prop desc can be used by towctrans function to test the wide char passed
// to it is translated to the type returned by the wctrans.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctrans1L()
    {
	wctrans_t type;
	  
	type = wctrans("tolower");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	wint_t twc = towctrans(L'K',type);
	  
	if(twc != (wint_t)L'k')
		{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
	
    }


// -----------------------------------------------------------------------------
//Function Name :testwctrans2
//API Tested :wctrans
//TestCase Description:wctrans returns -> property descriptor if name is valid
//this prop desc can be used by towctrans function to test the wide char passed
// to it is translated to the type returned by the wctrans.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctrans2L()
    {
	wctrans_t type;
	  
	type = wctrans("toupper");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	wint_t twc = towctrans(L'k',type);
	  
	if(twc != L'K')
		{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
	
    }
    
// -----------------------------------------------------------------------------
//Function Name :testwctrans3
//API Tested :wctrans
//TestCase Description:wctrans returns -> property descriptor if name is valid
//this prop desc can be used by towctrans function to test the wide char passed
//to it is translated to the type returned by the wctrans.(Negative test case).
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctrans3L()
    {
	wctrans_t type;
	  
	type = wctrans("kkkkkwer");
	  
	if(type == (wctrans_t)0)
		{
	  	return KErrNone;
		}
	  
	return KErrGeneral;
	  
    }
    
// -----------------------------------------------------------------------------
//Function Name :testwctrans4
//API Tested :wctrans
//TestCase Description:wctrans returns -> property descriptor if name is valid
//this prop desc can be used by towctrans function to test the wide char passed
// to it is translated to the type returned by the wctrans.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctrans4L()
    {
	wctrans_t type;
	  
	type = wctrans("tolower");
	  
	if(type == (wctype_t)0)
		{
	  	return KErrGeneral;
		}
	  
	wint_t twc = towctrans(WEOF,type);
	  
	if(twc != WEOF)
		{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
	
    }

// -----------------------------------------------------------------------------
//Function Name :testwctrans5
//API Tested :wctrans
//TestCase Description:wctrans returns -> property descriptor if name is valid
//this prop desc can be used by towctrans function to test the wide char passed
// to it is translated to the type returned by the wctrans.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testwctrans5L()
    {
	wctrans_t type;
	  
	type = wctrans("toupper");
	  
	if(type == (wctrans_t)0)
		{
	  	return KErrGeneral;
		}
	  
	wint_t twc = towctrans(L'M',type);
	  
	if(twc != L'M')
		{
	  	return KErrGeneral;
		}
	  
	return KErrNone;	
	
    }
//  End of File
