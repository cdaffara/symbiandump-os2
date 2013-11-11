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


TInt CTestLibcwchar::wscanf_Test1L()
    {
  
	int res;
	
	wprintf(L"TD ID =1_15_2_2554\r\n");
	
	wprintf(L"Testing scanf for integer\r\n");
	
	wprintf(L"Normal input:Enter a number like 999999999\r\n");
	
	wprintf(L"Expected Result:Converted value=999999999,return value=1\r\n");

	int ret=wscanf(L"%d",&res);
	getwchar();
	

	wprintf(L"Output:Converted value=%d and return value=%d\r\n",res,ret);
	wprintf(L"Boundary value:Enter a number like -999999999\r\n");
	
	ret=wscanf(L"%d",&res);
	getwchar();
	
	wprintf(L"Expected Result:Converted value=-999999999,return value=1\r\n");
	
	wprintf(L"Output:Converted value=%d and return value=%d\r\n",res,ret);
	
	wprintf(L"Bad input:Enter a non-numeral like character-a \r\n");
	
	ret=wscanf(L"%d",&res);
	getwchar();
	
	wprintf(L"\nExpected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);
	

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :wscanf_Test2
//API Tested :wscanf
//TestCase Description:To test whether the function works for conversion 
// specifier ls for wide-char strings.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test2L()
    {
  
	char res[BUF_SIZE];
	wprintf(L"TD ID =1_15_2_2555\r\n");
	
	wprintf(L"Testing scanf for String\r\n");
	
	wprintf(L"Normal input:Enter a string like 'abcdef'\r\n");
	int ret=wscanf(L"%s",res);
	getwchar();

	wprintf(L"Output:Converted value=%s and return value=%d\r\n",res,ret);
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	

	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :wscanf_Test3
//API Tested :wscanf
//TestCase Description:To test whether the function works for conversion 
//specifier f for float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test3L()
    {
	float res;
	
	wprintf(L"TD ID =1_15_2_2556\r\n");
	
	wprintf(L"Testing scanf for float\r\n");
	
	wprintf(L"Normal input:Enter a number like 99.99999\r\n");
	
	wprintf(L"Expected Result:Converted value=99.999999,return value=1\r\n");

	int ret=wscanf(L"%f",&res);
	getwchar();
	
	wprintf(L"Output:Converted value=%f and return value=%d\r\n",res,ret);

	wprintf(L"Boundary value:Enter a number like -99.99999\r\n");
	
	ret=wscanf(L"%f",&res);
	getwchar();
	
	wprintf(L"Expected Result:Converted value=-99.999999,return value=1\r\n");
	
	wprintf(L"Output:Converted value=%f and return value=%d\r\n",res,ret);
	
	wprintf(L"Bad input:Enter a non-numeral like character-a \r\n");
	
	ret=wscanf(L"%f",&res);
	getwchar();
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :wscanf_Test4
//API Tested :wscanf
//TestCase Description:To test whether the function works for conversion specifier 
//e or E for exponential notation of float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test4L()
    {
	float res;
	
	wprintf(L"TD ID =1_15_2_736\r\n");
	
	wprintf(L"Testing scanf for exponential\r\n");
	
	wprintf(L"Normal input:Enter a value like 99.99999\r\n");
	
	wprintf(L"Expected Result:Converted value=9.999999E+01,return value=1\r\n");

	int ret=wscanf(L"%e",&res);
	getwchar();
	
	wprintf(L"Output:Converted value=%e and return value=%d\r\n",res,ret);
	
	wprintf(L"Boundary value:Enter a number like -9999.9999\r\n");
	
	ret=wscanf(L"%e",&res);
	getwchar();
	
	wprintf(L"Expected Result:Converted value=-1.000000E+04,return value=1\r\n");
	
	wprintf(L"Output:Converted value=%e and return value=%d\r\n",res,ret);
	
	wprintf(L"Bad input:Enter a non-numeral like character-a \r\n");
	
	ret=wscanf(L"%e",&res);
	getwchar();
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :wscanf_Test5
//API Tested :wscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c for single characters
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test5L()
    {
  
	char res; 
	
	wprintf(L"TD ID =1_15_2_2558\r\n");
	
	wprintf(L"Testing scanf for Char\r\n");
	
	wprintf(L"Normal input:Enter a character like 'a'\r\n");
	
	wprintf(L"Expected Result:Converted value='a' ,return value=1\r\n");
	 
    int ret=wscanf(L"%c",&res);
    getwchar();
    
    wprintf(L"Output:Converted value=%c and return value=%d\r\n",res,ret);
    
    wprintf(L"Boundary value:Enter aspecial character like '@' \r\n");
	
	ret=wscanf(L"%c",&res);
	getwchar();
	
	wprintf(L"Expected Result:Converted value=@,return value=1\r\n");
	
	wprintf(L"Output:Converted value=%c and return value=%d\r\n",res,ret);
	

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }


// -----------------------------------------------------------------------------
//Function Name :wscanf_Test6
//API Tested :wscanf
//TestCase Description:To test whether the function works for conversion specifier 
//o for octal notation of nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test6L()
    {

	int res;
	wprintf(L"TD ID =1_15_2_2559\r\n");
	
	wprintf(L"Testing scanf for Octal type\r\n");
	
	wprintf(L"Normal input:Enter an octal number like 20\r\n");
	
	wprintf(L"Expected Result:Converted decimal value=16 ,return value=1\r\n");

	int ret=wscanf(L"%o",&res);
	getwchar();
	
	wprintf(L"Output:Converted Octal value is: %o and return value=%d\r\n",res,ret);
	wprintf(L"Output:Converted decimal value: %d and return value=%d\r\n",res,ret);
	
	wprintf(L"Bad input:Enter a non-numeral like character-a \r\n");
	
	ret=wscanf(L"%o",&res);
	getwchar();
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);
	
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }


// -----------------------------------------------------------------------------
//Function Name :wscanf_Test7
//API Tested :wscanf
//TestCase Description:To test whether the function works for conversion specifier 
//x or X for hexadecimal notation.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test7L()
    {
  
	int res;
	wprintf(L"TD ID =1_15_2_2560\r\n");

	wprintf(L"Testing scanf for Hexadecimal type\r\n");
	
	wprintf(L"Normal input:Enter a Hexadecimal number like 14\r\n");
	
	wprintf(L"Expected Result:Converted decimal value=20 ,return value=1\r\n");
	
	int ret=wscanf(L"%x",&res);
	getwchar();
	
	wprintf(L"Output:Converted Hexadecimal value is: %x and return value=%d\r\n",res,ret);
	wprintf(L"Output:Converted decimal value: %d and return value=%d\r\n",res,ret);
	
	wprintf(L"Bad input:Enter a  character other than 0-9 and a-f\r\n");
	
	ret=wscanf(L"%x",&res);
	getwchar();
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);
	
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }


// -----------------------------------------------------------------------------
//Function Name :wscanf_Test8
//API Tested :wscanf
//TestCase Description:To test whether the function works for character matching using 
//the negation ^ operator in the format string with char-matching-[].Eg. 
//wscanf(L"7980231", L"%l[^54321]", &res[0]);-must return 7980 cause these are 
//the characters upto which the grammar [^54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test8L()
    {
  
	wchar_t res[BUF_SIZE];
	
	wprintf(L"TD ID =1_15_2_2582\r\n");
	
	wprintf(L"Testing scanf for Negation operator\r\n");
	
	wprintf(L"Here the conversion automatically ends if the user presses inputs \r\n");
	wprintf(L"Not included in the search grammar\r\n");

	wprintf(L"Normal/Boundary input for grammar [^543]:Enter characters like 'abc1234'\r\n");
	
	wprintf(L"Expected Result:Converted value=abc12 ,return value=1\r\n");
	
	int ret=wscanf(L"%l[^543]", &res[0]);
	getwchar();
	
	wprintf(L"\nOutput:Converted value=%ls and return value=%d\r\n",res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }


// -----------------------------------------------------------------------------
//Function Name :wscanf_Test9
//API Tested :wscanf
//TestCase Description:To test whether the function works for a combination of 
//various data-types and their respective values converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test9L()
    {
	int d;float f;wchar_t res[BUF_SIZE];
	
	wprintf(L"TD ID =1_15_2_2595\r\n");
	
	wprintf(L"Testing scanf for Combination Of types\r\n");
	wprintf(L"Normal input:Enter a combination of different data types values like '999999  9999.9999 abcdef123'\r\n");
	
	wprintf(L"Expected Result:Converted value=999999  10,000.000000 abcdef123 ,return value=3\r\n");

	int ret=wscanf(L"%d%f%ls",&d,&f,res);
	getwchar();
	
	wprintf(L"\nOutput:Converted value=%d  %f  %ls and return value=%d\r\n",d,f,res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :wscanf_Test10
//API Tested :wscanf
//TestCase Description:To test whether the function works for character matching using 
//the [] operator in the format string.Eg. 
//wscanf(L"1227231", L"%l[54321]", &res[0]);-must return 122 cause these are 
//the characters upto which the grammar [54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test10L()
    {
  
	wchar_t res[BUF_SIZE];
	wprintf(L"TD ID =1_15_2_2581\r\n");
	
	wprintf(L"Testing scanf for Simple char-matching\r\n");
	wprintf(L"Normal input:Enter a number like 122\r\n");


	int ret=wscanf(L"%l[54321]", &res[0]);
	getwchar();
	
	wprintf(L"Output:Converted value=%ls and return value=%d\r\n",res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }


// -----------------------------------------------------------------------------
//Function Name :wscanf_Test11
//API Tested :wscanf
//TestCase Description:To test whether the function works for a format string 
//containing %n in it.It behaves as if no input is being consumed.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test11L()
    {
  
	int d1,d2,n1,n2;
	
	wprintf(L"TD ID =1_15_2_2597\r\n");

	wprintf(L"Testing scanf for n-specifier\r\n");
	wprintf(L"Normal input:Enter a number like 999998 and 999999\r\n");
	
	wprintf(L"Expected Result:Converted value=999998  999999,return value=2\r\n");
		
	int ret=wscanf(L"%d%n%n%d", &d1,&n1,&n2,&d2);
	getwchar();
	
	wprintf(L"Output:Converted value=%d %d and return value=%d\r\n",d1,d2,ret);
	
	wprintf(L"Boundary value:Enter a number like -999999999 and -999999999\r\n");
	
	ret=wscanf(L"%d%n%n%d", &d1,&n1,&n2,&d2);
	getwchar();
	
	wprintf(L"Expected Result:Converted value=-999999999  -999999999,return value=2\r\n");
	
	wprintf(L"Output:Converted value= %d  %d and return value=%d\r\n",d1,d2,ret);
	
	wprintf(L"Bad input:Enter a  non-numeral like character-a\r\n");
	
	ret=wscanf(L"%d%n%n%d", &d1,&n1,&n2,&d2);
	getwchar();
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);
	
		
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :wscanf_Test12
//API Tested :wscanf
//TestCase Description:To test whether the format string takes pointer to wchar 
//buffer and proper values are printed to the target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test12L()
    {
	wchar_t dest[BUF_SIZE];
    
	wprintf(L"TD ID =1_15_2_2598\r\n");    
	wprintf(L"Testing scanf for Pointer to Wchar Buf\r\n");
	wprintf(L"Normal input:Enter a value like 'abcdef'\r\n");
	wprintf(L"Expected Result:Converted value= abcdef,return value=1\r\n");
	
	int ret=wscanf(L"%ls",dest);
	getwchar();
	
	wprintf(L"Output:Converted value=%ls and return value=%d\r\n",dest,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }  


// -----------------------------------------------------------------------------
//Function Name :wscanf_Test13
//API Tested :wscanf
//TestCase Description:To test whether the function works for format strings having 
//field width for integer types.Here the number of digits specified in the field 
//width argument is picked up from the input string and converted to an int.(eg %3d)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test13L()
    {
   	
	int res;
	wprintf(L"TD ID =1_15_2_2572\r\n");

	wprintf(L"Testing scanf for Integer Type(width)\r\n");
	wprintf(L"Normal input:Enter a 4 digit number like 9875\r\n");
	wprintf(L"Expected Result:Converted value=987,return value=1\r\n");
	
	int ret=wscanf(L"%3d",&res);
	getwchar();
	
	wprintf(L"Output:Extracted three digit from input number is: %d and return value=%d\r\n",res,ret);
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    } 

// -----------------------------------------------------------------------------
//Function Name :wscanf_Test14
//API Tested :wscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c with specified field width.Here the number of characters mentioned in field 
//width must be picked up and converted to a string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test14L()
    {
  
	char res[BUF_SIZE]; 
	wprintf(L"TD ID =1_15_2_2573\r\n");
	
	wprintf(L"Testing scanf for Character Type\r\n");
	wprintf(L"Normal input:Enter a 6 character string like 'abcdef'\r\n");
	
	wprintf(L"Expected Result:Converted value= abcde,return value=1\r\n");
	
	int ret=wscanf(L"%5c",&res);
	res[5] = '\0';
	getwchar();
	
	wprintf(L"\nOutput:First five characters in input string are: %s and return value=%d\r\n",res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }

// -----------------------------------------------------------------------------
//Function Name :wscanf_Test15
//API Tested :wscanf
//TestCase Description:To test whether the function works for conversion specifier 
//lc.Here the number of characters mentioned in field width must be picked up 
//and converted  to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test15L()
    {
  
	wchar_t res[BUF_SIZE]; 
	 
	wprintf(L"TD ID =1_15_2_2562\r\n");
	 
	wprintf(L"Testing scanf for WChar\r\n");
	wprintf(L"Normal input:Enter a two word string like 'ab'\r\n");
	wprintf(L"Expected Result:Converted value='a',return value=1\r\n"); 
	
    int ret=wscanf(L"%lc",res);
    getwchar();
	
	wprintf(L"\nOutput:Converted WChar character =%s and return value=%d\r\n",res,ret);
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :wscanf_Test16
//API Tested :wscanf
//TestCase Description:To test whether the function works for conversion specifier 
//lc.Here the number of characters mentioned in field width must be picked up 
//and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test16L()
    {
  	
	wchar_t res[BUF_SIZE]; 
	
	wprintf(L"TD ID =1_15_2_2574\r\n");
	
	wprintf(L"Testing scanf for Wide-Character Type\r\n");
	wprintf(L"Normal input:Enter a 6 character string like 'abcdef'\r\n");
 	wprintf(L"Expected Result:Converted value='abcde',return value=1\r\n");
 	
    int ret=wscanf(L"%5lc",res);
    res[5] = L'\0';
    getwchar();
	
	 
	FILE *fp;
	fp = fopen("C:\\test_wscan16.txt","w");
	fwprintf(fp,L"%ls",res);
	fclose(fp);
	
	
	wprintf(L"\nOutput:Converted value=%ls and return value=%d\r\n",res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    } 

// -----------------------------------------------------------------------------
//Function Name :wscanf_Test17
//API Tested :wscanf
//TestCase Description:To test whether the function works * specifier for a combination of 
//various data-types such that the ones with * are ignored and the rest are 
//converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wscanf_Test17L()
    {
	wchar_t res1[BUF_SIZE],res2[BUF_SIZE];
	
	wprintf(L"TD ID =1_15_2_2596\r\n");
	
	wprintf(L"Testing scanf for Ignoring inputr\n");
	wprintf(L"Normal input:Enter a value like '9999999 abcd 999.9999 xyz'\r\n");
	wprintf(L"Expected Result:Converted value= abcd and xyz,return value=2\r\n");

	int ret=wscanf(L"%*d%ls%*f%ls",res1,res2);
	 getwchar();
	
	wprintf(L"Output:Converted values =%ls and %ls and return value=%d\r\n",res1,res2,ret);
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test1
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//d or i for decimal numbers.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test1L()
    {
  	FILE *fp;
    int res;

    fp = fopen("C:\\wchar.txt","w");
    fwprintf(fp,L"23");
    fclose(fp);

    fp=fopen("C:\\wchar.txt","r");
    int ret=fwscanf(fp,L"%d",&res);
    fclose(fp);
	
	INFO_PRINTF2(_L("fwscanf converted %d"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);
	
	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test2
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//						ls for wide-char strings.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test2L()
    {
  
	FILE *fp;
    wchar_t res[BUF_SIZE];

    fp = fopen("C:\\wchar.txt","w");
    fwprintf(fp,L"abcdef");
    fclose(fp);

    fp=fopen("C:\\wchar.txt","r");
    int ret=fwscanf(fp,L"%ls",res);
    fclose(fp);

	
	INFO_PRINTF2(_L("fwscanf converted %s"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);
	
	
	if(ret>=0)
		{
		return KErrNone;
		}
	else 
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test3
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//f for float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test3L()
    {
	FILE *fp;
 	float res;

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"23.3487");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%f",&res);
	fclose(fp);

	
	INFO_PRINTF2(_L("fwscanf converted %f"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test4
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//e or E for exponential notation of float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test4L()
    {
	FILE *fp;
 	float res;

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"23.3487");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%e",&res);
	fclose(fp);

	
	INFO_PRINTF2(_L("fwscanf converted %e"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test5
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c for single characters
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test5L()
    {

	FILE *fp;
	char res; 

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"abcdefgh");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%c",&res);
	fclose(fp);

  
	INFO_PRINTF2(_L("fwscanf converted %c"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test6
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//o for octal notation of nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test6L()
    {

	FILE *fp;
	int res;

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"23");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%#o",&res);
	fclose(fp);

	
	INFO_PRINTF2(_L("fwscanf converted %o"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test7
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//						x or X for hexadecimal notation.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test7L()
    {
    	
	FILE *fp;
	int res;

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"23");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%#x",&res);
	fclose(fp);
  
	
	INFO_PRINTF2(_L("fwscanf converted %x"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test8
//API Tested :fwscanf
//TestCase Description:To test whether the function works for character matching using 
//the negation ^ operator in the format string with char-matching-[].
//Eg. fwscanf(L"7980231", L"%l[^54321]", &res[0]);must return 7890 cause these
// are the characters upto which the grammar [^54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test8L()
    {
  
	FILE *fp;
	wchar_t res[BUF_SIZE];

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"89037231");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%l[^54321]",&res[0]);
	fclose(fp);
  
	

	INFO_PRINTF2(_L("fwscanf converted %s"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test9
//API Tested :fwscanf
//TestCase Description:To test whether the function works for a combination of 
//various data-types and their respective values converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test9L()
    {
	FILE *fp;
	int d;float f;wchar_t res[BUF_SIZE];
	

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"23.326 abcd");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%d%f%ls",&d,&f,res);
	fclose(fp);
  
	INFO_PRINTF4(_L("fwscanf converted %d %f %s"),d,f,res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	
	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test10
//API Tested :fwscanf
//TestCase Description:To test whether the function works for character matching 
//using the [] operator in the format string.Eg. fwscanf(L"1227231", L"%l[54321]", &res[0]);
//must return 122 cause these are the characters upto which the grammar 
//[54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test10L()
    {
  
	FILE *fp;
	wchar_t res[BUF_SIZE];
	

	fp = fopen("C:\\wchar1.txt","w");
	fwprintf(fp,L"1227231");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%l[54321]",&res[0]);
	fclose(fp);
  
	
	INFO_PRINTF2(_L("fwscanf converted %s"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}	
    }


// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test11
//API Tested :fwscanf
//TestCase Description:To test whether the function works for a format string 
//containing %n in it.It behaves as if no input is being consumed.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test11L()
    {
  
	FILE *fp;
	int d1,d2,n1,n2;
	

	fp = fopen("C:\\wchar2.txt","w");
	fwprintf(fp,L"12334",L"2345");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%d%n%n%d",&d1,&n1,&n2,&d2);
	fclose(fp);
  
	INFO_PRINTF3(_L("fwscanf converted %d %d"),d1,d2);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test12
//API Tested :fwscanf
//TestCase Description:To test whether the function works for format strings having 
//field width for integer types.Here the number of digits specified in the 
//field width argument is picked up from the input string and converted to an 
//int.(eg %3d)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test12L()
    {
   	
	FILE *fp;
	int res;
	

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"23456");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%3d",&res);
	fclose(fp);


	INFO_PRINTF2(_L("fwscanf converted %d"),res);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test13
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c with specified field width.Here the number of characters mentioned in 
//field width must be picked up and converted to a string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test13L()
    {
  
	FILE *fp;
	wchar_t res[BUF_SIZE]; 
	

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"abcdefgh");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%5c",&res);
	fclose(fp);
  
	FILE *fp1;
	fp1 = fopen("C:\\test_fw13.txt","w");
	fwprintf(fp1,L"%s",res);
	fclose(fp1);
	
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test14
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//lc.Here the number of characters mentioned in field width must be picked up 
//and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test14L()
    {
  
	FILE *fp;
	wchar_t res[BUF_SIZE]; 
	

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"abcdefgh");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%lc",res);
	fclose(fp);
  
	FILE *fp1;
	fp1 = fopen("C:\\test_fw14.txt","w");
	fwprintf(fp1,L"%s",res);
	fclose(fp1);
	
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test15
//API Tested :fwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//lc. Here the number of characters mentioned in field width must be picked up
//and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test15L()
    {
  	
	FILE *fp;
	wchar_t res[BUF_SIZE]; 
	

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"abcdefgh");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%5lc",res);
	fclose(fp);
  
	FILE *fp1;
	fp1 = fopen("C:\\test_fw15.txt","w");
	fwprintf(fp1,L"%ls",res);
	fclose(fp1);
	
    INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :fwscanf_Test16
//API Tested :fwscanf
//TestCase Description:To test whether the function works * specifier for a combination of 
//various data-types such that the ones with * are ignored and the rest
//are converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwscanf_Test16L()
    {
	FILE *fp;
	wchar_t res1[BUF_SIZE],res2[BUF_SIZE];
	

	fp = fopen("C:\\wchar.txt","w");
	fwprintf(fp,L"23 abcd 45.66 xyz");
	fclose(fp);

	fp=fopen("C:\\wchar.txt","r");
	int ret=fwscanf(fp,L"%*d%ls%*f%ls",res1,res2);
	fclose(fp);
  
  	FILE *fp1;
	fp1 = fopen("C:\\test_fw16.txt","w");
	fwprintf(fp1,L"%ls",res1);
	fclose(fp1);
	fp1 = fopen("C:\\test_fw17.txt","w");
	fwprintf(fp1,L"%ls",res2);
	fclose(fp1);
	
	INFO_PRINTF3(_L("fwscanf converted %ls %ls"),res1,res2);
	INFO_PRINTF2(_L("fwscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }
    
  
// -----------------------------------------------------------------------------
//Function Name :swscanf_Test1
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//d or i for decimal numbers.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test1L()
    {
  
	int res;

	int ret=swscanf(L"23",L"%d",&res);

	INFO_PRINTF2(_L("swscanf converted %d"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);
	

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :swscanf_Test2
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//ls for wide-char strings.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test2L()
    {
  
	char res[BUF_SIZE];

	int ret=swscanf(L"abcd\n",L"%ls",res);

	INFO_PRINTF2(_L("swscanf converted %s"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);
	
	//
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :swscanf_Test3
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//f for float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test3L()
    {
	float res;

	int ret=swscanf(L"23.3487",L"%f",&res);

	INFO_PRINTF2(_L("swscanf converted %f"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :swscanf_Test4
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//e or E for exponential notation of float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test4L()
    {
	float res;

	int ret=swscanf(L"23.3487",L"%e",&res);

	INFO_PRINTF2(_L("swscanf converted %e"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :swscanf_Test5
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c for single characters
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test5L()
    {
  
	 char res; 
	 
     int ret=swscanf(L"abcdefgh",L"%c",&res);

	INFO_PRINTF2(_L("swscanf converted %c"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :swscanf_Test6
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//o for octal notation of nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test6L()
    {

	int res;

	int ret=swscanf(L"23",L"%o",&res);

	INFO_PRINTF2(_L("swscanf converted %d"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :swscanf_Test7
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//x or X for hexadecimal notation.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test7L()
    {
  
	int res;

	int ret=swscanf(L"23",L"%x",&res);

	INFO_PRINTF2(_L("swscanf converted %d"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :swscanf_Test8
//API Tested :swscanf
//TestCase Description:To test whether the function works for character matching using 
//the negation ^ operator in the format string with char-matching-[].
//Eg. swscanf(L"7980231", L"%l[^54321]", &res[0]);
//must return 7890 cause these are the characters upto which the grammar [^54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test8L()
    {
  
	wchar_t res[BUF_SIZE];

	int ret=swscanf(L"89037231", L"%l[^54321]", &res[0]);

	INFO_PRINTF2(_L("swscanf converted %s"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :swscanf_Test9
//API Tested :swscanf
//TestCase Description:To test whether the function works for a combination of 
//various data-types and their respective values converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test9L()
    {
	int d;float f;wchar_t res[BUF_SIZE];

	int ret=swscanf(L"23.326 abcd",L"%d%f%ls",&d,&f,res);

	INFO_PRINTF4(_L("swscanf converted %d %f %s"),d,f,res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :swscanf_Test10
//API Tested :swscanf
//TestCase Description:To test whether the function works for character matching using 
//the [] operator in the format string.Eg. swscanf(L"1227231", L"%l[54321]", &res[0]);
//must return 122 cause these are the characters upto which he grammar 
//[54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test10L()
    {
  
	wchar_t res[BUF_SIZE];

	int ret=swscanf(L"1227231", L"%l[54321]", &res[0]);

	INFO_PRINTF2(_L("swscanf converted %s"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :swscanf_Test11
//API Tested :swscanf
//TestCase Description:To test whether the function works for a format string 
//containing %n in it.It behaves as if no input is being consumed.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test11L()
    {
  
	int d1,d2,n1,n2;

	int ret=swscanf(L"12334", L"%d%n%n%d", &d1,&n1,&n2,&d2);

	INFO_PRINTF3(_L("swscanf converted %d %d"),d1,d2);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :swscanf_Test12
//API Tested :swscanf
//TestCase Description:To test whether the format string takes pointer to wchar 
//buffer and proper values are printed to the target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test12L()
    {
    wchar_t dest[BUF_SIZE];
	wchar_t src[BUF_SIZE];

	char* filename="c:\\Test_FmtIO.txt";
	unsigned int arr[6]={'a','b','c','d','e','f'};
	int count=6;
	CreateTestDataFile(filename,arr,count);
	
	FILE *fp=fopen(filename,"r");
	fgetws(src,5,fp);
	int ret=swscanf(src,L"%ls",dest);
	fclose(fp);

	INFO_PRINTF2(_L("swscanf converted %s"),dest);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }  


// -----------------------------------------------------------------------------
//Function Name :swscanf_Test13
//API Tested :swscanf
//TestCase Description:To test whether the function works for format strings having 
//field width for integer types.Here the number of digits specified in the 
//field width argument is picked up from the input string and converted to an int.(eg %3d)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test13L()
    {
   	
	int res;

	int ret=swscanf(L"23456",L"%3d",&res);

	INFO_PRINTF2(_L("swscanf converted %d"),res);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :swscanf_Test14
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c with specified field width.Here the number of characters mentioned in 
//field width must be picked up and converted to a string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test14L()
    {
  
	char res[BUF_SIZE]; 

    int ret=swscanf(L"abcdefgh",L"%5c",&res);

	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :swscanf_Test15
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//lc.Here the number of characters mentioned in field width must be picked up 
//and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test15L()
    {
  
	 char res[BUF_SIZE]; 

     int ret=swscanf(L"abcdefgh",L"%lc",res);

	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :swscanf_Test16
//API Tested :swscanf
//TestCase Description:To test whether the function works for conversion specifier 
//lc.Here the number of characters mentioned in field width must be picked up 
//and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test16L()
    {
  	
	 char res[BUF_SIZE]; 

     int ret=swscanf(L"abcdefgh",L"%5lc",res);

	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :swscanf_Test17
//API Tested :swscanf
//TestCase Description:To test whether the function works * specifier for a 
//combination of various data-types such that the ones with * are ignored and
//the rest are converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swscanf_Test17L()
    {
  
	wchar_t res1[BUF_SIZE],res2[BUF_SIZE];

	int ret=swscanf(L"23 abcd 45.66 xyz",L"%*d%ls%*f%ls",res1,res2);

	INFO_PRINTF3(_L("swscanf converted %s %s"),res1,res2);
	INFO_PRINTF2(_L("swscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

#ifdef TO_BE_TESTED        


// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test1
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//	d or i for decimal numbers.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test1L()
    {
  
	int res,ret;
	
	wprintf(L"TD ID =1_15_2_2554\r\n");
	
	wprintf(L"Testing vscanf for integer\r\n");
	
	wprintf(L"Normal input:Enter a number like 999999999\r\n");
	
	wprintf(L"Expected Result:Converted value=999999999,return value=1\r\n");
	
	ret=vwscanfTests(L"%d",&res);
	
	wprintf(L"Output:Converted value=%d and return value=%d\r\n",res,ret);
	wprintf(L"Boundary value:Enter a number like -999999999\r\n");
	
	ret=vwscanfTests(L"%d",&res);
	
	wprintf(L"Expected Result:Converted value=-999999999,return value=1\r\n");
	
	wprintf(L"Output:Converted value=%d and return value=%d\r\n",res,ret);
	
	wprintf(L"Bad input:Enter a non-numeral like character-a \r\n");
	
	ret=vwscanfTests(L"%d",&res);
	
	wprintf(L"\nExpected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);
	
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}

    }

// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test2
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion 
// specifier ls for wide-char strings.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test2L()
    {
  
	wchar_t res[BUF_SIZE];
	wprintf(L"TD ID =1_15_2_2555\r\n");
	
	wprintf(L"Testing scanf for String\r\n");
	
	wprintf(L"Normal input:Enter a string like 'abcdef'\r\n");
	
	int ret=vwscanfTests(L"%s",&res);

	wprintf(L"Output:Converted value=%s and return value=%d\r\n",res,ret);
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test3
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//f for float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test3L()
    {
	float res;
	
	wprintf(L"TD ID =1_15_2_2556\r\n");
	
	wprintf(L"Testing scanf for float\r\n");
	
	wprintf(L"Normal input:Enter a number like 99.99999\r\n");
	
	wprintf(L"Expected Result:Converted value=99.999992,return value=1\r\n");

	int ret=vwscanfTests(L"%f",&res);
	
	wprintf(L"Output:Converted value=%f and return value=%d\r\n",res,ret);

	wprintf(L"Boundary value:Enter a number like -99.99999\r\n");
	
	ret=vwscanfTests(L"%f",&res);
	
	wprintf(L"Expected Result:Converted value=-99.999992,return value=1\r\n");
	
	wprintf(L"Output:Converted value=%f and return value=%d\r\n",res,ret);
	
	wprintf(L"Bad input:Enter a non-numeral like character-a \r\n");
	
	ret=vwscanfTests(L"%f",&res);
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }

// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test4
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//e or E for exponential notation of float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test4L()
    {
	float res;
	
	wprintf(L"TD ID =1_15_2_736\r\n");
	
	wprintf(L"Testing scanf for exponential\r\n");
	
	wprintf(L"Normal input:Enter a value like 99.99999\r\n");
	
	wprintf(L"Expected Result:Converted value=9.999999E+01,return value=1\r\n");

	int ret=vwscanfTests(L"%e",&res);
	
	wprintf(L"Output:Converted value=%e and return value=%d\r\n",res,ret);
	
	wprintf(L"Boundary value:Enter a number like -9999.9999\r\n");
	
	
	ret=vwscanfTests(L"%e",&res);
	

	wprintf(L"Output:Converted value=%e and return value=%d\r\n",res,ret);
	
	wprintf(L"Bad input:Enter a non-numeral like character-a \r\n");
	
	ret=vwscanfTests(L"%e",&res);
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test5
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c for single characters
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test5L()
    {
  
	char res; 
	 
	wprintf(L"TD ID =1_15_2_2558\r\n");
	
	wprintf(L"Testing scanf for Char\r\n");
	
	wprintf(L"Normal input:Enter a character like 'a'\r\n");
	
	wprintf(L"Expected Result:Converted value='a' ,return value=1\r\n");	 
    
	int ret=vwscanfTests(L"%c",&res);	
 
    wprintf(L"Output:Converted value=%c and return value=%d\r\n",res,ret);
    
    wprintf(L"Boundary value:Enter aspecial character like '@' \r\n");
	
	ret=vwscanfTests(L"%c",&res);
	
	wprintf(L"Expected Result:Converted value=@,return value=1\r\n");
	
	wprintf(L"Output:Converted value=%c and return value=%d\r\n",res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test6
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//o for octal notation of nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test6L()
    {

	int res;
	
	wprintf(L"TD ID =1_15_2_2559\r\n");
	
	wprintf(L"Testing scanf for Octal type\r\n");
	
	wprintf(L"Normal input:Enter an octal number like 20\r\n");
	
	wprintf(L"Expected Result:Converted decimal value=16 ,return value=1\r\n");

	int ret=vwscanfTests(L"%o",&res);
	
	wprintf(L"Output:Converted Octal value is: %o and return value=%d\r\n",res,ret);
	wprintf(L"Output:Converted value in decimal is: %d and return value=%d\r\n",res,ret);

	wprintf(L"Bad input:Enter a non-numeral like character-a \r\n");
	
	ret=vwscanfTests(L"%o",&res);
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);
	
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }


// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test7
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//x or X for hexadecimal notation.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test7L()
    {
  
	int res;
	
	wprintf(L"TD ID =1_15_2_2560\r\n");

	wprintf(L"Testing scanf for Hexadecimal type\r\n");
	
	wprintf(L"Normal input:Enter a Hexadecimal number like 14\r\n");
	
	wprintf(L"Expected Result:Converted decimal value=20 ,return value=1\r\n");

	int ret=vwscanfTests(L"%x",&res);
	
	wprintf(L"Output:Converted Hexadecimal value is: %x and return value=%d\r\n",res,ret);
	wprintf(L"Output:Converted decimal value is: %d and return value=%d\r\n",res,ret);

	
	wprintf(L"Bad input:Enter a  character other than 0-9 and a-f\r\n");
	
	ret=vwscanfTests(L"%x",&res);
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);
	
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test8
//API Tested :vwscanf
//TestCase Description:To test whether the function works for character matching using 
//the negation ^ operator in the format string with char-matching-[].
//Eg. vwscanf(L"7980231", L"%l[^54321]", &res[0]);
//must return 7980 cause these are the characters upto which 
//the grammar [^54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test8L()
    {
  
	wchar_t res[BUF_SIZE];
	
	wprintf(L"TD ID =1_15_2_2582\r\n");
	
	wprintf(L"Testing scanf for Negation operator\r\n");
	
	wprintf(L"Here the conversion automatically ends if the user presses inputs \r\n");
	wprintf(L"Not included in the search grammar\r\n");

	wprintf(L"Normal/Boundary input for grammar [^543]:Enter characters like 'abc1234'\r\n");
	
	wprintf(L"Expected Result:Converted value=abc12 ,return value=1\r\n");

	int ret=vwscanfTests(L"%l[^543]", &res[0]);
	
	wprintf(L"\nOutput:Converted value=%ls and return value=%d\r\n",res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test9
//API Tested :vwscanf
//TestCase Description:To test whether the function works for a combination of 
//various data-types and their respective values converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test9L()
    {
	int d;float f;wchar_t res[BUF_SIZE];
	
	
	wprintf(L"TD ID =1_15_2_2595\r\n");
	
	wprintf(L"Testing scanf for Combination Of types\r\n");
	wprintf(L"Normal input:Enter a combination of different data types values like '999999  9999.9999 abcdef123'\r\n");
	
	wprintf(L"Expected Result:Converted value=999999  10,000.000000 abcdef123 ,return value=3\r\n");

	int ret=vwscanfTests(L"%d%f%ls",&d,&f,res);
	
	wprintf(L"\nOutput:Converted value=%d  %f  %ls and return value=%d\r\n",d,f,res,ret);
	
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test10
//API Tested :vwscanf
//TestCase Description:To test whether the function works for character 
//matching using the [] operator in the format string.
//Eg.  vwscanf(L"1227231", L"%l[54321]", &res[0]);
//must return 122 cause these are the characters upto which the grammar [54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test10L()
    {
  
	wchar_t res[BUF_SIZE];
	
	wprintf(L"TD ID =1_15_2_2581\r\n");
	
	wprintf(L"Testing scanf for Simple char-matching\r\n");
	wprintf(L"Normal input:Enter a number like 12267\r\n");
	
	wprintf(L"Expected Result:Converted value=1,return value=1\r\n");
	

	int ret=vwscanfTests(L"%l[54321]", &res[0]);
	
	wprintf(L"Output:Converted value=%ls and return value=%d\r\n",res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test11
//API Tested :vwscanf
//TestCase Description:To test whether the function works for a format string 
//containing %n in it.It behaves as if no input is being consumed.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test11L()
    {
  
	int d1,d2,n1,n2;
	
	wprintf(L"TD ID =1_15_2_2597\r\n");

	wprintf(L"Testing scanf for n-specifier\r\n");
	wprintf(L"Normal input:Enter a number like 999998 and 999999\r\n");
	
	wprintf(L"Expected Result:Converted value=999998  999999,return value=2\r\n");

	int ret=vwscanfTests(L"%d%n%n%d", &d1,&n1,&n2,&d2);
	wprintf(L"Output:Converted value=%d %d and return value=%d\r\n",d1,d2,ret);
	
	wprintf(L"Boundary value:Enter a number like -999999999 and -999999999\r\n");
	
	ret=vwscanfTests(L"%d%n%n%d", &d1,&n1,&n2,&d2);
	
	wprintf(L"Expected Result:Converted value=-999999999  -999999999,return value=2\r\n");
	
	wprintf(L"Output:Converted value= %d  %d and return value=%d\r\n",d1,d2,ret);
	
	wprintf(L"Bad input:Enter a  non-numeral like character-a\r\n");
	
	ret=vwscanfTests(L"%d%n%n%d", &d1,&n1,&n2,&d2);
	
	wprintf(L"Expected Result:No conversion occurs,Return value=0\r\n");
	
	wprintf(L"Output:Return value=%d\r\n",ret);
	
		
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test12
//API Tested :vwscanf
//TestCase Description:To test whether the format string takes pointer to wchar 
//buffer and proper values are printed to the target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test12L()
    {
    wchar_t dest[BUF_SIZE];
    
    wprintf(L"TD ID =1_15_2_2598\r\n");    
	wprintf(L"Testing scanf for Pointer to Wchar Buf\r\n");
	wprintf(L"Normal input:Enter a value like 'abcdef'\r\n");
	
	wprintf(L"Expected Result:Converted value= abcdef,return value=1\r\n");

	int ret=vwscanfTests(L"%ls",dest);
	
	wprintf(L"Output:Converted value=%ls and return value=%d\r\n",dest,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }  


// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test13
//API Tested :vwscanf
//TestCase Description:To test whether the function works for format strings having 
//field width for integer types.Here the number of digits specified in the 
//field width argument is picked up from the input string and converted to an
// int.(eg %3d)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test13L()
    {
   	
	int res;
	
	wprintf(L"TD ID =1_15_2_2572\r\n");

	wprintf(L"Testing scanf for Integer Type(width)\r\n");
	wprintf(L"Normal input:Enter a 4 digit number like 9875\r\n");
	
	wprintf(L"Expected Result:Converted value=987,return value=1\r\n");

	int ret=vwscanfTests(L"%3d",&res);
	
	wprintf(L"Output:Extracted three digit from input number is: %d and return value=%d\r\n",res,ret);
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test14
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c with specified field width.Here the number of characters mentioned in field 
//width must be picked up and converted to a string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test14L()
   {
  
	wchar_t res[BUF_SIZE]; 
	
	wprintf(L"TD ID =1_15_2_2573\r\n");
	
	wprintf(L"Testing scanf for Character Type\r\n");
	wprintf(L"Normal input:Enter a 6 character string like 'abcdef'\r\n");
	
	wprintf(L"Expected Result:Converted value= abcde,return value=1\r\n");
	
 	int ret=vwscanfTests(L"%5c",&res);
	wprintf(L"\nOutput:First five characters in input string are: %s and return value=%d\r\n",res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
   }

// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test15
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion 
//specifier lc.Here the number of characters mentioned in field width must be 
//picked up and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test15L()
    {
  
	wchar_t res[BUF_SIZE];
	 
	wprintf(L"TD ID =1_15_2_2562\r\n");
	 
	wprintf(L"Testing scanf for WChar\r\n");
	wprintf(L"Normal input:Enter a two word string like 'ab'\r\n");
	
	wprintf(L"Expected Result:Converted value='a',return value=1\r\n"); 
	 
 	int ret=vwscanfTests(L"%lc",res);

	wprintf(L"\nOutput:Converted WChar character =%s and return value=%d\r\n",res,ret);
  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 
// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test16
//API Tested :vwscanf
//TestCase Description:To test whether the function works for conversion
//specifier lc.Here the number of characters mentioned in field width must be 
//picked up and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwscanf_Test16L()
    {
  	
	 wchar_t res[BUF_SIZE];
	 
	 wprintf(L"TD ID =1_15_2_2574\r\n");
	
	wprintf(L"Testing scanf for Wide-Character Type\r\n");
	wprintf(L"Normal input:Enter a 6 character string like 'abcdef'\r\n");
    
    wprintf(L"Expected Result:Converted value='abcde',return value=1\r\n");
    
	int ret=vwscanfTests(L"%5lc",res);
	res[5] = L'\0';

	wprintf(L"\nOutput:Converted value=%ls and return value=%d\r\n",res,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :vwscanf_Test17
//API Tested :vwscanf
//TestCase Description:To test whether the function works * specifier for a combination of 
//various data-types such that the ones with * are ignored and the rest are 
//converted appropriately.
// -----------------------------------------------------------------------------


TInt CTestLibcwchar::vwscanf_Test17L()
    {
	wchar_t res1[BUF_SIZE],res2[BUF_SIZE];	
	
	wprintf(L"TD ID =1_15_2_2596\r\n");
	
	wprintf(L"Testing scanf for Ignoring inputr\n");
	wprintf(L"Normal input:Enter a value like '9999999 abcd 999.9999 xyz'\r\n");
	
	wprintf(L"Expected Result:Converted value= abcd and xyz,return value=2\r\n");

	int ret=vwscanfTests(L"%*d%ls%*f%ls",res1,res2);	

	wprintf(L"Output:Converted values =%ls and %ls and return value=%d\r\n",res1,res2,ret);

  	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
      
       getwchar();
   	}while(ret!=1 && ret!=0);
   	
	
	if(ret==1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



//Local functions

TInt CTestLibcwchar::vwscanfTests (const wchar_t *fmt, ...)
{
	int ret;
	va_list ap;
	va_start (ap, fmt);

	ret=vwscanf(fmt,ap);
	getwchar();
	
	va_end (ap);
	return ret;
}

#endif //TO_BE_TESTED

// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test1
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//d or i for decimal numbers.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test1L()
    {
  	FILE *fp;
    int res;
    char* filename = "C:\\wchar.txt";
      	
    fp = fopen(filename,"w");
    fwprintf(fp,L"23");
    fclose(fp);

       
	int ret=vfwscanfTests(filename,L"%d",&res);
       
	
	INFO_PRINTF2(_L("vfwscanf converted %d"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);
	
	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test2
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//ls for wide-char strings.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test2L()
    {
  
	FILE *fp;
    wchar_t res[BUF_SIZE];
	char* filename = "C:\\wchar.txt";
	
    fp = fopen(filename,"w");
    fwprintf(fp,L"ab");
    fclose(fp);

    int ret=vfwscanfTests(filename,L"%ls",res);
	
	INFO_PRINTF2(_L("vfwscanf converted %s"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);
	
	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test3
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//f for float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test3L()
    {
	FILE *fp;
 	float res;
	char* filename = "C:\\wchar.txt";
	
	fp = fopen(filename,"w");
	fwprintf(fp,L"23.3487");
	fclose(fp);


	int ret=vfwscanfTests(filename,L"%f",&res);
 
	
	INFO_PRINTF2(_L("vfwscanf converted %f"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test4
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//e or E for exponential notation of float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test4L()
    {
	FILE *fp;
 	float res;
	char* filename = "C:\\wchar.txt";
	
	fp = fopen(filename,"w");
	fwprintf(fp,L"23.3487");
	fclose(fp);

  
	int ret=vfwscanfTests(filename,L"%e",&res);

	INFO_PRINTF2(_L("vfwscanf converted %e"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test5
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c for single characters
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test5L()
    {

	FILE *fp;
	char res; 
	char* filename = "C:\\wchar.txt";
	
	fp = fopen(filename,"w");
  	fwprintf(fp,L"cd");
	fclose(fp);

	int ret=vfwscanfTests(filename,L"%c",&res);

	INFO_PRINTF2(_L("vfwscanf converted %c"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test6
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//o for octal notation of nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test6L()
    {

	FILE *fp;
	int res;
	char* filename = "C:\\wchar.txt";
	
	fp = fopen(filename,"w");
    fwprintf(fp,L"023");
	fclose(fp);

	int ret=vfwscanfTests(filename,L"%#o",&res);



	INFO_PRINTF2(_L("vfwscanf converted %o"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test7
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//x or X for hexadecimal notation.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test7L()
    {
    	
	FILE *fp;
	int res;
	char* filename = "C:\\wchar.txt";
	
	fp = fopen(filename,"w");
	fwprintf(fp,L"0x23");
	fclose(fp);


	int ret=vfwscanfTests(filename,L"%#x",&res);
 
  	INFO_PRINTF2(_L("vfwscanf converted %x"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test8
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for character matching using 
//					   the negation ^ operator in the format string with char-matching-[].
//					   Eg. vfwscanf(L"7980231", L"%l[^54321]", &res[0]);
//					   -must return 7890 cause these are the characters upto which 
//					   the grammar [^54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test8L()
    {
  
	FILE *fp;
	wchar_t res[BUF_SIZE];
	char* filename = "C:\\wchar.txt";
		
	fp = fopen(filename,"w");
	fwprintf(fp,L"89037231");
	fclose(fp);

	int ret=vfwscanfTests(filename,L"%l[^54321]",&res[0]);
  
	INFO_PRINTF2(_L("vfwscanf converted %s"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test9
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for a combination of 
//various data-types and their respective values converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test9L()
    {
	FILE *fp;
	int d;float f;wchar_t res[BUF_SIZE];
	char* filename = "C:\\wchar.txt";

	fp = fopen(filename,"w");
	fwprintf(fp,L"23.326 abcd");
	fclose(fp);


	int ret=vfwscanfTests(filename,L"%d%f%ls",&d,&f,res);
 
  
	INFO_PRINTF4(_L("vfwscanf converted %d %f %s"),d,f,res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test10
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for character matching using 
//					   the [] operator in the format string.
//					   Eg. vfwscanf(L"1227231", L"%l[54321]", &res[0]);
//					   -must return 122 cause these are the characters upto which 
//					   the grammar [54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test10L()
    {
  
	FILE *fp;
	wchar_t res[BUF_SIZE];
	char* filename = "C:\\wchar.txt";

	fp = fopen(filename,"w");
    fwprintf(fp,L"12");
	fclose(fp);

	int ret=vfwscanfTests(filename,L"%l[54321]",&res[0]);

  
	
	INFO_PRINTF2(_L("vfwscanf converted %s"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test11
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for a format string 
//containing %n in it.It behaves as if no input is being consumed.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test11L()
    {
  
	FILE *fp;
	int d1,d2,n1,n2;
	char* filename = "C:\\wchar.txt";

	fp = fopen(filename,"w");
	fwprintf(fp,L"12",L"99");
	fclose(fp);


	int ret=vfwscanfTests(filename,L"%d%n%n%d",&d1,&n1,&n2,&d2);

  
	INFO_PRINTF3(_L("vfwscanf converted %d %d"),d1,d2);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test12
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for format strings having 
//					   field width for integer types.Here the number of digits 
//					   specified in the field width argument is picked up from the 
//					   input string and converted to an int.(eg %3d)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test12L()
    {
   	
	FILE *fp;
	int res;
	char* filename = "C:\\wchar.txt";

	fp = fopen(filename,"w");
	fwprintf(fp,L"2345");
	fclose(fp);

	int ret=vfwscanfTests(filename,L"%3d",&res);
 
	INFO_PRINTF2(_L("vfwscanf converted %d"),res);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test13
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion specifier 
//						c with specified field width.Here the number of characters
//						mentioned in field width must be picked up and converted 
//						to a string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test13L()
    {
  
	FILE *fp;
	wchar_t res[BUF_SIZE]; 
	char* filename = "C:\\wchar.txt";

	fp = fopen(filename,"w");
	fwprintf(fp,L"abcdef");
	fclose(fp);

 
	int ret=vfwscanfTests(filename,L"%5c",&res);

  	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test14
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion 
//specifier lc.Here the number of characters mentioned in field width must be 
//picked up and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test14L()
    {
  
	FILE *fp;
	wchar_t res[BUF_SIZE]; 
	char* filename = "C:\\wchar.txt";

	fp = fopen(filename,"w");
	fwprintf(fp,L"ghij");
	fclose(fp);

  
	int ret=vfwscanfTests(filename,L"%lc",&res);
 
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test15
//API Tested :vfwscanf
//TestCase Description:To test whether the function works for conversion 
//specifier lc.Here the number of characters mentioned in field width must be 
//picked up and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test15L()
    {
  	
	FILE *fp;
	wchar_t res[BUF_SIZE]; 
	char* filename = "C:\\wchar.txt";

	fp = fopen(filename,"w");
	fwprintf(fp,L"abcdef");
	fclose(fp);

	int ret=vfwscanfTests(filename,L"%5lc",&res);
  
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :vfwscanf_Test16
//API Tested :vfwscanf
//TestCase Description:To test whether the function works * specifier for a 
//combination of various data-types such that the ones with * are ignored and 
//the rest are converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwscanf_Test16L()
    {
	FILE *fp;
	wchar_t res1[BUF_SIZE],res2[BUF_SIZE];
	
	char* filename = "C:\\wchar.txt";
	
	fp = fopen(filename,"w");
	fwprintf(fp,L"2345 abcd 45.66 xyz");
	fclose(fp);
	
	int ret=vfwscanfTests(filename,L"%*d%ls%*f%ls",&res1,&res2);
  
	INFO_PRINTF3(_L("vfwscanf converted %s %s"),res1,res2);
	INFO_PRINTF2(_L("vfwscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


//Local functions

TInt CTestLibcwchar::vfwscanfTests(char *filename,const wchar_t *fmt, ...)
	{
	int ret;
	va_list ap;
	va_start (ap, fmt);
	FILE* fp=fopen(filename,"r");

	ret=vfwscanf(fp,fmt,ap);

	fclose(fp); 
	va_end (ap);
	return ret;
	}


// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test1
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion specifier 
//d or i for decimal numbers.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test1L()
    {
  
	int res;

	int ret=vswscanfTests(L"23",L"%d",&res);

	INFO_PRINTF2(_L("vswscanf converted %d"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);
	
	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test2
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion specifier 
//ls for wide-char strings.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test2L()
    {
  
	wchar_t res[BUF_SIZE];

	int ret=vswscanfTests(L"abcd\n",L"%ls",res);

	INFO_PRINTF2(_L("vswscanf converted %s"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);
	
	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test3
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion specifier 
//f for float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test3L()
    {
	float res;

	int ret=vswscanfTests(L"23.3487",L"%f",&res);

	INFO_PRINTF2(_L("vswscanf converted %f"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test4
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion specifier 
//e or E for exponential notation of float nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test4L()
    {
	float res;

	int ret=vswscanfTests(L"23.3487",L"%e",&res);

	INFO_PRINTF2(_L("vswscanf converted %e"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test5
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion specifier 
//c for single characters
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test5L()
    {
  
	 char res; 
	 
     int ret=vswscanfTests(L"abcdefgh",L"%c",&res);

	INFO_PRINTF2(_L("vswscanf converted %c"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test6
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion specifier 
//o for octal notation of nos.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test6L()
    {

	int res;

	int ret=vswscanfTests(L"23",L"%#o",&res);

	INFO_PRINTF2(_L("vswscanf converted %o"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test7
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion specifier 
//x or X for hexadecimal notation.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test7L()
    {
  
	int res;

	int ret=vswscanfTests(L"23",L"%#x",&res);

	INFO_PRINTF2(_L("vswscanf converted %x"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test8
//API Tested :vswscanf
//TestCase Description:To test whether the function works for character matching using 
//					   the negation ^ operator in the format string with char-matching-[].
//					   Eg. vswscanf(L"7980231", L"%l[^54321]", &res[0]);
//					   -must return 7890 cause these are the characters upto which 
//					   the grammar [^54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test8L()
    {
  
	wchar_t res[BUF_SIZE];

	int ret=vswscanfTests(L"89037231", L"%l[^54321]", &res[0]);

	INFO_PRINTF2(_L("vswscanf converted %s"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test9
//API Tested :vswscanf
//TestCase Description:To test whether the function works for a combination of 
//various data-types and their respective values converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test9L()
    {
	int d;float f;wchar_t res[BUF_SIZE];

	int ret=vswscanfTests(L"23.326 abcd",L"%d%f%ls",&d,&f,res);

	INFO_PRINTF4(_L("vswscanf converted %d %f %s"),d,f,res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test10
//API Tested :vswscanf
//TestCase Description:To test whether the function works for character matching using 
//the [] operator in the format string.
//Eg. vswscanf(L"1227231", L"%l[54321]", &res[0]);
//must return 122 cause these are the characters upto which the grammar [54321] holds good.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test10L()
    {
	INFO_PRINTF1(_L("in vswscanf_Test10L"));
	wchar_t res[BUF_SIZE];

	int ret=vswscanfTests(L"1227231", L"%l[54321]", &res[0]);

	INFO_PRINTF2(_L("vswscanf converted %s"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test11
//API Tested :vswscanf
//TestCase Description:To test whether the function works for a format string 
//containing %n in it.It behaves as if no input is being consumed.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test11L()
    {
	INFO_PRINTF1(_L("in vswscanf_Test11L"));
	int d1,d2,n1,n2;

	int ret=vswscanfTests(L"12334",L"%d%n%n%d", &d1,&n1,&n2,&d2);

	INFO_PRINTF3(_L("vswscanf converted %d %d"),d1,d2);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }



// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test12
//API Tested :vswscanf
//TestCase Description:To test whether the format string takes pointer to wchar 
//buffer and proper values are printed to the target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test12L()
    {
    wchar_t dest[BUF_SIZE];
	wchar_t src[BUF_SIZE];

	char* filename="c:\\Test_FmtIO.txt";
	unsigned int arr[6]={'a','b','c','d','e','f'};
	int count=6;
	CreateTestDataFile(filename,arr,count);
	
	FILE *fp=fopen(filename,"r");
	fgetws(src,5,fp);
	int ret=vswscanfTests(src,L"%ls",dest);
	fclose(fp);

	INFO_PRINTF2(_L("vswscanf converted %s"),dest);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }  


// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test13
//API Tested :vswscanf
//TestCase Description:To test whether the function works for format strings having 
//					   field width for integer types.Here the number of digits 
//					   specified in the field width argument is picked up from the 
//					   input string and converted to an int.(eg %3d)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test13L()
    {
   	INFO_PRINTF1(_L("in vswscanf_Test13L"));
	int res;

	int ret=vswscanfTests(L"23456",L"%3d",&res);
	
	INFO_PRINTF2(_L("vswscanf converted %d"),res);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test14
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion specifier 
// c with specified field width.Here the number of characters mentioned in field 
// width must be picked up and converted to a string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test14L()
    {
  
	wchar_t res[BUF_SIZE]; 

    int ret=vswscanfTests(L"abcdefgh",L"%5c",&res);
    
    FILE *fp1;
	fp1 = fopen("C:\\test_vsw_14.txt","w");
	fwprintf(fp1,L"%s",res);
	fclose(fp1);

	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test15
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion specifier 
//lc.Here the number of characters mentioned in field width must be picked up 
//and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test15L()
    {
  
	wchar_t res[BUF_SIZE]; 

    int ret=vswscanfTests(L"abcdefgh",L"%lc",res);
     
    FILE *fp1;
    fp1 = fopen("C:\\test_vsw_15.txt","w");
	fwprintf(fp1,L"%s",res);
	fclose(fp1);

	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    } 

// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test16
//API Tested :vswscanf
//TestCase Description:To test whether the function works for conversion 
//specifier lc.Here the number of characters mentioned in field width must be 
//picked up and converted to a wide-char.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test16L()
    {
  	
	wchar_t res[BUF_SIZE]; 

	int ret=vswscanfTests(L"abcdefgh",L"%5lc",&res);
  
  	FILE *fp1;
	fp1 = fopen("C:\\test_vsw_16.txt","w");
	fwprintf(fp1,L"%ls",res);
	fclose(fp1);

	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	} 

// -----------------------------------------------------------------------------
//Function Name :vswscanf_Test17
//API Tested :vswscanf
//TestCase Description:To test whether the function works * specifier for a
// combination of various data-types such that the ones with * are ignored and
// the rest are converted appropriately.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswscanf_Test17L()
    {
  
	wchar_t res1[BUF_SIZE],res2[BUF_SIZE];

	int ret=vswscanfTests(L"23 abcd 45.66 xyz",L"%*d%ls%*f%ls",res1,res2);
	
	FILE *fp1;
	fp1 = fopen("C:\\test_vsw_17.txt","w");
	fwprintf(fp1,L"%ls",res1);
	fclose(fp1);
	
	fp1 = fopen("C:\\test_vsw_18.txt","w");
	fwprintf(fp1,L"%ls",res2);
	fclose(fp1);

	INFO_PRINTF3(_L("vswscanf converted %s %s"),res1,res2);
	INFO_PRINTF2(_L("vswscanf returned %d"),ret);

	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }


//Local functions

TInt CTestLibcwchar::vswscanfTests (const wchar_t *str,const wchar_t *fmt, ...)
	{
	int ret;
	va_list ap;
	va_start (ap, fmt);

	ret=vswscanf(str,fmt,ap);
	
	va_end (ap);
	return ret;
	}

