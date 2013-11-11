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


#ifdef TO_BE_TESTED
// -----------------------------------------------------------------------------
//Function Name :wprintf_Test0
//API Tested :wprintf
//TestCase Description:Basic test of wchar strings getting copied to target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test0L()
    {
  
	wprintf(L"TD ID = 1_15_2_2323\n");
	wprintf(L"Testing wprintf for string type\n");
	wprintf(L"For normal input,Api Output is:");

	int ret=wprintf(L"abcdef");
		wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :wprintf_Test1
//API Tested :wprintf
//TestCase Description:To test whether the correct amount of data is copied to target
//					   string while provided with a pointer to a wchar_t buffer
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test1L()
    {
    
	wchar_t src[BUF_SIZE];
	
	
	wprintf(L"TD ID = 1_15_2_2324\n");
	wprintf(L"Testing wprintf for Pointer to Wchar Buf\n");
	
	wprintf(L"For normal input,Api Output is:");
	
	char* filename="c:\\Test_FmtIO.txt";
	unsigned int arr[6]={'a','b','c','d','e','f'};
	int count=6;
	CreateTestDataFile(filename,arr,count);

	FILE *fp=fopen(filename,"r");
	fgetws(src,5,fp);
	int ret=wprintf(src);

	fclose(fp);

	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test2
//API Tested :wprintf
//TestCase Description:A test for basic data-types like integer,float,string,etc,.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test2L()
    {
   
  
	wprintf(L"TD ID = 1_15_2_565\n");
	wprintf(L"Testing wprintf for Basic Types\n");
	wprintf(L"For normal input,Api Output is:");
	
	int ret=wprintf(L"%s %d","abcdef",10);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test3
//API Tested :wprintf
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	   
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test3L()
    {
  
	wprintf(L"TD ID = 1_15_2_2326\n");
	wprintf(L"Testing wprintf for double dollar\n");	
	wprintf(L"For normal input,Api Output is:");
		
	int ret=wprintf(L"%%%d%%",9999);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test4
//API Tested :wprintf
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test4L()
    {
  
	wprintf(L"TD ID = 1_15_2_2327\n");
	wprintf(L"Testing wprintf Using the asterix specifierr\n");		
	wprintf(L"For normal input,Api Output is:");
		
	int ret=wprintf(L"%*d",6,23);	
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test5
//API Tested :wprintf
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test5L()
    {
  
	wprintf(L"TD ID = 1_15_2_2327\n");
	wprintf(L"Testing wprintf for Simple Dot-Float\n");			
	wprintf(L"For normal input,Api Output is:");
		
	int ret=wprintf(L"%.f",99.99999);	
		
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test6
//API Tested :wprintf
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test6L()
    {
  
	wprintf(L"TD ID = 1_15_2_2328\n");
	wprintf(L"Testing wprintf for Precision-Float\n");			
	wprintf(L"For normal input,Api Output is:");
		
	int ret=wprintf(L"%.3f",66.212313);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test7
//API Tested :wprintf
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test7L()
    {
  
	wprintf(L"TD ID = 1_15_2_2329\n");
	wprintf(L"Testing wprintf for Precision-String\n");			
	wprintf(L"For normal input,Api Output is:");
		 
	int ret=wprintf(L"%.2ls",L"abcdefg");
		
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test8
//API Tested :wprintf
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
	
TInt CTestLibcwchar::wprintf_Test8L()
    {
  
	wprintf(L"TD ID = 1_15_2_2334\n");
	wprintf(L"Testing wprintf for Simple Left Justification\n");			
	wprintf(L"For normal input,Api Output is:");
		  
	int ret=wprintf(L"%-12ls",L"abcefg");
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test9
//API Tested :wprintf
//TestCase Description:To test whether right justification is provided for example 
//					   by %10d type of field width
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test9L()
    {
  
	wprintf(L"TD ID = 1_15_2_2335\n");
	wprintf(L"Testing wprintf for Simple Right Justification\n");		
	wprintf(L"For normal input,Api Output is:");
			 
	int ret=wprintf(L"%12ls",L"abcefg");
		
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test10
//API Tested :wprintf
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test10L()
    {
  
	wprintf(L"TD ID = 1_15_2_2333\n");
	wprintf(L"Testing wprintf for Complex Justification\n");		
	wprintf(L"For normal input,Api Output is:");
			  	
	int ret=wprintf(L"%-10d%3d",10,20);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test11
//API Tested :wprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.3f),the precision determines the number of digits
//					   to be copied after the decimal point in the float number and 
//					   the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test11L()
    {

	wprintf(L"TD ID = 1_15_2_2393\n");
	wprintf(L"Testing wprintf for Right Justified+Precision+Float\n");		
	wprintf(L"For normal input,Api Output is:");
		
	int ret=wprintf(L"%10.3f",21.33345);
		
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test12
//API Tested :wprintf
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test12L()
    {
    
	
	wprintf(L"TD ID = 1_15_2_2393\n");
	wprintf(L"Testing wprintf for Left Justified+Precision+String\n");		
	wprintf(L"For normal input,Api Output is:");
		
	int ret=wprintf(L"%-10.3ls",L"abcefg");
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test13
//API Tested :wprintf
//TestCase Description:To test whether right justification occurs only if the number of 
//				       characters left after truncation above is less than the field width.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test13L()
   {
  
	wprintf(L"TD ID = 1_15_2_2394\n");
	wprintf(L"Testing wprintf for Right Justified+Precision+String\n");		
	wprintf(L"For normal input,Api Output is:");
		 	
	int ret=wprintf(L"%10.3ls",L"abcefg");
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test14
//API Tested :wprintf
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test14L()
    {
  
	wprintf(L"TD ID = 1_15_2_2392\n");
	wprintf(L"Testing wprintf for Simple Padding\n");		
	wprintf(L"For normal input,Api Output is:");
		 	
  int ret=wprintf(L"%010d",234);
  
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test15
//API Tested :wprintf
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test15L()
    {

	wprintf(L"TD ID = 1_15_2_2395\n");
	wprintf(L"Testing wprintf for Plus char with numeric input\n");		
	wprintf(L"For normal input,Api Output is:");
		 	 		
	int ret=wprintf(L"%+f",10.23);
	
	 
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test16
//API Tested :wprintf
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test16L()
    {
    
	
	wprintf(L"TD ID = 1_15_2_2396\n");
	wprintf(L"Testing wprintf with Inserting spaces in input\n");		
	wprintf(L"For normal input,Api Output is:");
		
	int ret=wprintf(L"%         f",12.3);
	 
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test17
//API Tested :wprintf
//TestCase Description:To test whether for o conversions(eg. %#o), the first character
//					   of the output  string  is  made zero (by prefixing a 0 if it was
//					   not zero already)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test17L()
    {
    
	   
	
	wprintf(L"TD ID = 1_15_2_2397\n");
	wprintf(L"Testing wprintf for # with o conversion\n");		
	wprintf(L"For normal input,Api Output is:");
		
	int ret=wprintf(L"%#o",20);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test18
//API Tested :wprintf
//TestCase Description:To test whether for x and X conversions(eg. %#x or %#X),
//					   a non-zero result has the string 0x (or 0X  for X  conversions)
//					   prepended to it.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test18L()
    {
    
	
	wprintf(L"TD ID = 1_15_2_2398\n");
	wprintf(L"Testing wprintf for # with x or X  conversion\n");		   
	wprintf(L"For normal input,Api Output is:");
			
	int ret=wprintf(L"%#x",20);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test19
//API Tested :wprintf
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros 
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test19L()
    {
  
	wprintf(L"TD ID = 1_15_2_2332\n");
	wprintf(L"Testing wprintf for Simple Dot-Int\n");		 
	wprintf(L"For normal input,Api Output is:");
			
	int ret=wprintf(L"%.6d",90);
		
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test20
//API Tested :wprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.3f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test20L()
    {
  
	wprintf(L"TD ID = 1_15_2_2332\n");
	wprintf(L"Testing wprintf for Left Justified+Precision+Float\n");		 
	wprintf(L"For normal input,Api Output is:");
			  
	int ret=wprintf(L"%-10.3f",21.33345);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
	do
  	{
       wprintf(L"\nPress 1 to PASS or 0 to FAIL this case:");
       wscanf(L"%d",&ret);
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
//Function Name :wprintf_Test21
//API Tested :wprintf
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %010.4f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::wprintf_Test21L()
    {
	wprintf(L"TD ID = 1_15_2_2332\n");
	wprintf(L"Testing wprintf for Padding for Float\n");	 
	wprintf(L"For normal input,Api Output is:");
			 
	int ret=wprintf(L"%-010.4f",23.45434);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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

#endif //TO_BE_TESTED

// -----------------------------------------------------------------------------
//Function Name :fwprintf_Test1
//API Tested :fwprintf
//TestCase Description:Basic test of wchar strings getting copied to target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test1L()
    {
  
	FILE *fp;
	char* filename = "C:\\wchar.txt" ;
	
	fp = fopen(filename,"w");
	
	int ret=fwprintf(fp,L"abcdef");

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
	
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
//Function Name :fwprintf_Test2
//API Tested :fwprintf
//TestCase Description:A test for basic data-types like integer,float,string,etc,.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test2L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%s %d","abcdef",123);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
	
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
//Function Name :fwprintf_Test3
//API Tested :fwprintf
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	   
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test3L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	int ret=fwprintf(fp,L"%%%d%%",123);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :fwprintf_Test4
//API Tested :fwprintf
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test4L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		

	int ret=fwprintf(fp,L"%*d",6,23);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);	
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
//Function Name :fwprintf_Test5
//API Tested :fwprintf
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test5L()
    {
    
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%.f",66.23);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
	
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
//Function Name :fwprintf_Test6
//API Tested :fwprintf
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test6L()
    {
   	FILE *fp;
   	char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%.3f",66.212313);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
	
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
//Function Name :fwprintf_Test7
//API Tested :fwprintf
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test7L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%.2ls",L"abcdefg");

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
	
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
//Function Name :fwprintf_Test8
//API Tested :fwprintf
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
	
TInt CTestLibcwchar::fwprintf_Test8L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
		fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%-12ls",L"abcefg");

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
	
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
//Function Name :fwprintf_Test9
//API Tested :fwprintf
//TestCase Description:To test whether right justification is provided for example 
//					   by %10d type of field width
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test9L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%12ls",L"abcefg");

	
	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
	
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
//Function Name :fwprintf_Test10
//API Tested :fwprintf
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test10L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%-10d%3d",10,20);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);

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
//Function Name :fwprintf_Test11
//API Tested :fwprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test11L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	int ret=fwprintf(fp,L"%-10.3f",21.33345);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
	
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
//Function Name :fwprintf_Test12
//API Tested :fwprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of digits
//					   to be copied after the decimal point in the float number and 
//					   the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test12L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%10.3f",21.33345);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);

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
//Function Name :fwprintf_Test13
//API Tested :fwprintf
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test13L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%-10.3ls",L"abcefg");

	
	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :fwprintf_Test14
//API Tested :fwprintf
//TestCase Description:To test whether right justification occurs only if the number of 
//				       characters left after truncation above is less than the field width.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test14L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	int ret=fwprintf(fp,L"%10.3ls",L"abcefg");

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :fwprintf_Test15
//API Tested :fwprintf
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test15L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	
	int ret=fwprintf(fp,L"%010d",234);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :fwprintf_Test16
//API Tested :fwprintf
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test16L()
    {
   	FILE *fp;
   	char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	int ret=fwprintf(fp,L"%+f",10.23);

	
	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :fwprintf_Test17
//API Tested :fwprintf
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test17L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
			
	int ret=fwprintf(fp,L"%   f",12.3);

	
	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :fwprintf_Test18
//API Tested :fwprintf
//TestCase Description:To test whether for o conversions(eg. %#o), the first character
//					   of the output  string  is  made zero (by prefixing a 0 if it was
//					   not zero already)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test18L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	int ret=fwprintf(fp,L"%#o",20);
	
	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :fwprintf_Test19
//API Tested :fwprintf
//TestCase Description:To test whether for x and X conversions(eg. %#x or %#X),
//					   a non-zero result has the string 0x (or 0X  for X  conversions)
//					   prepended to it.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test19L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		
	int ret=fwprintf(fp,L"%#x",20);

	
	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :fwprintf_Test20
//API Tested :fwprintf
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test20L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
		 
	int ret=fwprintf(fp,L"%-010.4f",23.45434);

	
	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :fwprintf_Test21
//API Tested :fwprintf
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros 
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::fwprintf_Test21L()
    {
    FILE *fp;
    char* filename = "C:\\wchar.txt" ;
	fp = fopen(filename,"w");
	
	int ret=fwprintf(fp,L"%.6d",90);

	INFO_PRINTF2(_L("fwprintf returned %d"),ret); //Was L8
	fclose(fp);
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
//Function Name :swprintf_Test0
//API Tested :swprintf
//TestCase Description:Basic test of wchar strings getting copied to target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test0L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"abcdef");

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
		
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
//Function Name :swprintf_Test1
//API Tested :swprintf
//TestCase Description:To test whether the correct amount of data is copied to target
//					   string while provided with a pointer to a wchar_t buffer
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test1L()
    {
    wchar_t dest[BUF_SIZE];
	wchar_t src[BUF_SIZE];

	char* filename="c:\\Test_FmtIO.txt";
	unsigned int arr[6]={'a','b','c','d','e','f'};
	int count=6;
	CreateTestDataFile(filename,arr,count);

	FILE *fp=fopen(filename,"r");
	fgetws(src,5,fp);
	int ret=swprintf(dest,(5 * sizeof(wchar_t)),src);
	fclose(fp);

	INFO_PRINTF2(_L("swprintf converted %ls"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8	

	
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
//Function Name :swprintf_Test2
//API Tested :swprintf
//TestCase Description:A test for basic data-types like integer,float,string,etc,.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test2L()
    {
    wchar_t dest[BUF_SIZE];

	int ret=swprintf(dest,sizeof(dest),L"%s %d","abcdef",123);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
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
//Function Name :swprintf_Test3
//API Tested :swprintf
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	   
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test3L()
    {
    wchar_t dest[BUF_SIZE];

	int ret=swprintf(dest,sizeof(dest),L"%%%d%%",123);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
	
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
//Function Name :swprintf_Test4
//API Tested :swprintf
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test4L()
    {
    wchar_t dest[BUF_SIZE];

	int ret=swprintf(dest,sizeof(dest),L"%*d",6,23);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
	
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
//Function Name :swprintf_Test5
//API Tested :swprintf
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test5L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%.f",66.23);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
	
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
//Function Name :swprintf_Test6
//API Tested :swprintf
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test6L()
    {
    
   	wchar_t dest2[BUF_SIZE];

	int ret=swprintf(dest2,sizeof(dest2),L"%.3f",66.212313);

	INFO_PRINTF2(_L("swprintf converted %s"),dest2); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
	
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
//Function Name :swprintf_Test7
//API Tested :swprintf
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test7L()
    {
    wchar_t dest[BUF_SIZE];

	int ret=swprintf(dest,sizeof(dest),L"%.2ls",L"abcdefg");

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
	
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
//Function Name :swprintf_Test8
//API Tested :swprintf
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
	
TInt CTestLibcwchar::swprintf_Test8L()
    {
    wchar_t dest[BUF_SIZE];

	int ret=swprintf(dest,sizeof(dest),L"%-12ls",L"abcefg");

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8

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
//Function Name :swprintf_Test9
//API Tested :swprintf
//TestCase Description:To test whether right justification is provided for example 
//					   by %10d type of field width
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test9L()
    {
    wchar_t dest[BUF_SIZE];

	int ret=swprintf(dest,sizeof(dest),L"%12ls",L"abcefg");

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8

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
//Function Name :swprintf_Test10
//API Tested :swprintf
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test10L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%-10d%3d",10,20);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8

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
//Function Name :swprintf_Test11
//API Tested :swprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test11L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%-10.3f",21.33345);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8

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
//Function Name :swprintf_Test12
//API Tested :swprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of digits
//					   to be copied after the decimal point in the float number and 
//					   the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test12L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%10.3f",21.33345);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8

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
//Function Name :swprintf_Test13
//API Tested :swprintf
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test13L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%-10.3ls",L"abcefg");

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8

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
//Function Name :swprintf_Test14
//API Tested :swprintf
//TestCase Description:To test whether right justification occurs only if the number of 
//				       characters left after truncation above is less than the field width.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test14L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%10.3ls",L"abcefg");

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
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
//Function Name :swprintf_Test15
//API Tested :swprintf
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test15L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%010d",234);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
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
//Function Name :swprintf_Test16
//API Tested :swprintf
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test16L()
    {
    
    wchar_t dest[BUF_SIZE];
		
	int ret=swprintf(dest,sizeof(dest),L"%+f",10.23);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
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
//Function Name :swprintf_Test17
//API Tested :swprintf
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test17L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%   f",12.3);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
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
//Function Name :swprintf_Test18
//API Tested :swprintf
//TestCase Description:To test whether for o conversions(eg. %#o), the first character
//					   of the output  string  is  made zero (by prefixing a 0 if it was
//					   not zero already)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test18L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%#o",20);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8

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
//Function Name :swprintf_Test19
//API Tested :swprintf
//TestCase Description:To test whether for x and X conversions(eg. %#x or %#X),
//					   a non-zero result has the string 0x (or 0X  for X  conversions)
//					   prepended to it.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test19L()
    {
    wchar_t dest[BUF_SIZE];
		
	int ret=swprintf(dest,sizeof(dest),L"%#x",20);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
	
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
//Function Name :swprintf_Test20
//API Tested :swprintf
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test20L()
    {
    
    wchar_t dest[BUF_SIZE];
		 
	int ret=swprintf(dest,sizeof(dest),L"%-010.4f",23.45434);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8

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
//Function Name :swprintf_Test21
//API Tested :swprintf
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros 
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::swprintf_Test21L()
    {
    wchar_t dest[BUF_SIZE];
	
	int ret=swprintf(dest,sizeof(dest),L"%.6d",90);

	INFO_PRINTF2(_L("swprintf converted %s"),dest); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
	
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
//Function Name :vwprintf_Test0
//API Tested :vwprintf
//TestCase Description:Basic test of wchar strings getting copied to target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test0L()
    {
  
  	int ret;
 	wprintf(L"TD ID = 1_15_2_2463\n");
	wprintf(L"Testing wprintf for Simple String\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%ls",L"abcdef");
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test1
//API Tested :vwprintf
//TestCase Description:To test whether the correct amount of data is copied to target
//					   string while provided with a pointer to a wchar_t buffer
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test1L()
    {
  	wchar_t src[BUF_SIZE];
	int ret;
	
 	wprintf(L"TD ID = 1_15_2_2462\n");
	wprintf(L"Testing wprintf for Pointer to Wchar Buf\n");
	wprintf(L"For normal input,Api Output is:");
	
	char* filename="c:\\Test_FmtIO.txt";
	unsigned int arr[6]={'a','b','c','d','e','f'};
	int count=6;
	CreateTestDataFile(filename,arr,count);

	FILE *fp=fopen(filename,"r");
	fgetws(src,5,fp);
	
	ret=vwprintfTests(src);
	fclose(fp);


	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test2
//API Tested :vwprintf
//TestCase Description:A test for basic data-types like integer,float,string,etc,.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test2L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2458\n");
	wprintf(L"Testing wprintf for Basic Types\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%s %d","abcdef",123);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test3
//API Tested :vwprintf
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	   
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test3L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2441\n");
	wprintf(L"Testing wprintf for double dollar\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%%%d%%",123);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test4
//API Tested :vwprintf
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test4L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2443\n");
	wprintf(L"Testing wprintf Using the asterix specifier\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%*d",6,23);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test5
//API Tested :vwprintf
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test5L()
    {
  	int ret;  
  
  	wprintf(L"TD ID = 1_15_2_2447\n");
	wprintf(L"Testing wprintf for Simple Dot-Float\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%.f",66.23);
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test6
//API Tested :vwprintf
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test6L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2445\n");
	wprintf(L"Testing wprintf for Precision-Float\n");
	wprintf(L"For normal input,Api Output is:");
  
	ret=vwprintfTests(L"%.3f",66.212313);
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test7
//API Tested :vwprintf
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test7L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2446\n");
	wprintf(L"Testing wprintf for Precision-String\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%.2ls",L"abcdefg");
	
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test8
//API Tested :vwprintf
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
	
TInt CTestLibcwchar::vwprintf_Test8L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2450\n");
	wprintf(L"Testing wprintf for Simple Left Justification\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%-12ls",L"abcefg");	
	
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test9
//API Tested :vwprintf
//TestCase Description:To test whether right justification is provided for example 
//					   by %10d type of field width
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test9L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2451\n");
	wprintf(L"Testing wprintf for Simple Right Justification\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%12ls",L"abcefg");
	
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test10
//API Tested :vwprintf
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test10L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2449\n");
	wprintf(L"Testing wprintf for Complex Justification\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%-10d%3d",10,20);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test11
//API Tested :vwprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test11L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2600\n");
	wprintf(L"Testing wprintf for Left Justified+Precision+Float\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%-10.3f",21.33345);
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test12
//API Tested :vwprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of digits
//					   to be copied after the decimal point in the float number and 
//					   the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test12L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2453\n");
	wprintf(L"Testing wprintf for Right Justified+Precision+Float\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%10.3f",21.33345);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test13
//API Tested :vwprintf
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test13L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2452\n");
	wprintf(L"Testing wprintf for Left Justified+Precision+String\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%-10.3ls",L"abcefg");
	
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test14
//API Tested :vwprintf
//TestCase Description:To test whether right justification occurs only if the number of 
//				       characters left after truncation above is less than the field width.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test14L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2454\n");
	wprintf(L"Testing wprintf for Right Justified+Precision+String\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%10.3ls",L"abcefg");
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test15
//API Tested :vwprintf
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test15L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2455\n");
	wprintf(L"Testing wprintf for Simple Padding \n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%010d",234);
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test16
//API Tested :vwprintf
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test16L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2461\n");
	wprintf(L"Testing wprintf for Plus char with numeric input\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%+f",10.23);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test17
//API Tested :vwprintf
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test17L()
    {
  	int ret;
  
  	wprintf(L"TD ID = 1_15_2_2459\n");
	wprintf(L"Testing wprintf for Inserting spaces\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%   f",12.3);
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test18
//API Tested :vwprintf
//TestCase Description:To test whether for o conversions(eg. %#o), the first character
//					   of the output  string  is  made zero (by prefixing a 0 if it was
//					   not zero already)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test18L()
    {
	int ret;
	
	
  	wprintf(L"TD ID = 1_15_2_2456\n");
	wprintf(L"Testing wprintf for # with o conversion\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%#o",20);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test19
//API Tested :vwprintf
//TestCase Description:To test whether for x and X conversions(eg. %#x or %#X),
//					   a non-zero result has the string 0x (or 0X  for X  conversions)
//					   prepended to it.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test19L()
    {
	int ret;
	
		
 	wprintf(L"TD ID = 1_15_2_2458\n");
	wprintf(L"Testing wprintf for # with x or X  conversion\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%#x",20);
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test20
//API Tested :vwprintf
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test20L()
    {
	int ret;
	
	wprintf(L"TD ID = 1_15_2_2599\n");
	wprintf(L"Testing wprintf for Padding for Float\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%-010.4f",23.45434);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed(including spaces).\nApi Return value=%d\n",ret);
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
//Function Name :vwprintf_Test21
//API Tested :vwprintf
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros 
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vwprintf_Test21L()
    {
	int ret;
	
	
	wprintf(L"TD ID = 1_15_2_2448\n");
	wprintf(L"Testing wprintf for Simple Dot-Int\n");
	wprintf(L"For normal input,Api Output is:");
	
	ret=vwprintfTests(L"%.6d",90);
	
	wprintf(L"\nApi return value must be equal to the number of chars printed.\nApi Return value=%d\n",ret);
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

TInt CTestLibcwchar::vwprintfTests(const wchar_t *fmt,...)
	{
	int ret;
	va_list ap;
	va_start(ap,fmt);

	ret=vwprintf(fmt,ap);
 	
 	
	INFO_PRINTF2(_L("vwprintf returned:%d"),ret );

	va_end(ap);
	return ret;
	}

#endif //TO_BE_TESTED

// -----------------------------------------------------------------------------
//Function Name :vfwprintf_Test0
//API Tested :vfwprintf
//TestCase Description:Basic test of wchar strings getting copied to target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test0L()
    {
 
 
	int ret;
	char* testfile="c:\\wchar.txt";

	ret=vfwprintfTests(testfile,L"%ls",L"abcdef");
	
		
	INFO_PRINTF2(_L("vfwprintf wrote to file: %s"),testfile ); //Was L8
	
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test1
//API Tested :vfwprintf
//TestCase Description:A test for basic data-types like integer,float,string,etc,.
// -----------------------------------------------------------------------------
   
TInt CTestLibcwchar::vfwprintf_Test1L()
    {
 
	int ret;
	wchar_t src[BUF_SIZE];
	char* testfile="c:\\wchar.txt";

	char* filename="c:\\Test_FmtIO.txt";
	unsigned int arr[6]={'a','b','c','d','e','f'};
	int count=6;
	CreateTestDataFile(filename,arr,count);

	FILE *fp=fopen(filename,"r");
	fgetws(src,6,fp);
	ret=vfwprintfTests(testfile,src);

	fclose(fp);

	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8	

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
//Function Name :vfwprintf_Test2
//API Tested :vfwprintf
//TestCase Description:A test for basic data-types like integer,float,string,etc,.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test2L()
    {
  	char* testfile="c:\\wchar.txt";
	int ret;
	ret=vfwprintfTests(testfile,L"%s %d","abcdef",234);
	
		
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
		
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
//Function Name :vfwprintf_Test3
//API Tested :vfwprintf
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	   
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test3L()
    {
    char* testfile="c:\\wchar.txt";
	int ret;
	int i = 4567;
	ret=vfwprintfTests(testfile,L"%%%d%%",i);
	
	
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
		
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
//Function Name :vfwprintf_Test4
//API Tested :vfwprintf
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test4L()
    {
    char* testfile="c:\\wchar.txt";
	int ret;
	
	ret=vfwprintfTests(testfile,L"%*d",6,23);
	
		
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
		
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
//Function Name :vfwprintf_Test5
//API Tested :vfwprintf
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test5L()
    {
    char* testfile="c:\\wchar.txt";
	int ret;
	ret=vfwprintfTests(testfile,L"%.f",66.23);
	
		
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test6
//API Tested :vfwprintf
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test6L()
    {
    
  	char* testfile="c:\\wchar.txt";
	int ret;
	ret=vfwprintfTests(testfile,L"%.3f",66.212313);
	
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test7
//API Tested :vfwprintf
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test7L()
    {
 	char* testfile="c:\\wchar.txt";
 	char* str = "abcdefg";
 	int ret;

	ret=vfwprintfTests(testfile,L"%.2s",str);
			

	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test8
//API Tested :vfwprintf
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
	
TInt CTestLibcwchar::vfwprintf_Test8L()
    {
  	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%-12s","abcdefg");
		
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test9
//API Tested :vfwprintf
//TestCase Description:To test whether right justification is provided for example 
//					   by %10d type of field width
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test9L()
    {
  	char* testfile="c:\\wchar.txt";
  	int ret;
  
  	ret=vfwprintfTests(testfile,L"%12s","abcdefg");
				
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test10
//API Tested :vfwprintf
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test10L()
    {
  	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%-10d%3d",10,20);
	
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test11
//API Tested :vfwprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test11L()
    {
  	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%-10.3f",21.33345);
	
	
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test12
//API Tested :vfwprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of digits
//					   to be copied after the decimal point in the float number and 
//					   the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test12L()
    {
    
	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%10.3f",21.33345);
	
	
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
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
//Function Name :vfwprintf_Test13
//API Tested :vfwprintf
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test13L()
    {
  	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%-10.3ls",L"abcefg");
	
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test14
//API Tested :vfwprintf
//TestCase Description:To test whether right justification occurs only if the number of 
//				       characters left after truncation above is less than the field width.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test14L()
    {
 	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%10.3ls",L"abcefg");


	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
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
//Function Name :vfwprintf_Test15
//API Tested :vfwprintf
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test15L()
    {
 	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%010d",234);
	
	
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
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
//Function Name :vfwprintf_Test16
//API Tested :vfwprintf
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test16L()
    {
	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%+f",10.23);
	
	
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	
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
//Function Name :vfwprintf_Test17
//API Tested :vfwprintf
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test17L()
    {
	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%   f",12.3);
		
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
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
//Function Name :vfwprintf_Test18
//API Tested :vfwprintf
//TestCase Description:To test whether for o conversions(eg. %#o), the first character
//					   of the output  string  is  made zero (by prefixing a 0 if it was
//					   not zero already)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar:: vfwprintf_Test18L()
    {
	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%#o",20);
		

	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
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
//Function Name :vfwprintf_Test19
//API Tested :vfwprintf
//TestCase Description:To test whether for x and X conversions(eg. %#x or %#X),
//					   a non-zero result has the string 0x (or 0X  for X  conversions)
//					   prepended to it.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test19L()
    {
 	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%#x",20);
			
	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
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
//Function Name :vfwprintf_Test20
//API Tested :vfwprintf
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test20L()
    {
   	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :vfwprintf_Test21
//API Tested :vfwprintf
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros 
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vfwprintf_Test21L()
    {
 	char* testfile="c:\\wchar.txt";
  	int ret;
  
	ret=vfwprintfTests(testfile,L"%.6d",90);

	INFO_PRINTF2(_L("vfwprintf returned %d"),ret); //Was L8
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

//************************************Locals Functions*****************************************

TInt CTestLibcwchar::vfwprintfTests( char *filename,const wchar_t *fmt,...)
{
	int ret;
	va_list ap;
	va_start(ap,fmt);

	FILE* fp = fopen(filename,"w");
	ret=vfwprintf(fp,fmt,ap);
	fclose(fp);
	va_end(ap);
	return ret;
}

// -----------------------------------------------------------------------------
//Function Name :vswprintf_Test0
//API Tested :vswprintf
//TestCase Description:Basic test of wchar strings getting copied to target string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test0L()
    {
  
  	int ret;
 
	ret=vswprintfTests(L"%ls",L"abcdef");
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
		
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
//Function Name :vswprintf_Test1
//API Tested :vswprintf
//TestCase Description:To test whether the correct amount of data is copied to target
//					   string while provided with a pointer to a wchar_t buffer
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test1L()
    {
  	wchar_t src[BUF_SIZE];
	int ret;
	
	char* filename="c:\\Test_FmtIO.txt";
	unsigned int arr[6]={'a','b','c','d','e','f'};
	int count=6;
	CreateTestDataFile(filename,arr,count);

	FILE *fp=fopen(filename,"r");
	fgetws(src,5,fp);
	
	ret=vswprintfTests(src);
	fclose(fp);

	INFO_PRINTF2(_L("vswprintf returned %d"),ret);	 //Was L8

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
//Function Name :vswprintf_Test2
//API Tested :vswprintf
//TestCase Description:A test for basic data-types like integer,float,string,etc,.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test2L()
    {
  	int ret;
	ret=vswprintfTests(L"%s %d","abcdef",123);
 
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
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
//Function Name :vswprintf_Test3
//API Tested :vswprintf
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	   
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test3L()
    {
  	int ret;
	ret=vswprintfTests(L"%%%d%%",123);
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
		
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
//Function Name :vswprintf_Test4
//API Tested :vswprintf
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test4L()
    {
  	int ret;
	ret=vswprintfTests(L"%*d",6,23);
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
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
//Function Name :vswprintf_Test5
//API Tested :vswprintf
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test5L()
    {
	int ret;
	ret=vswprintfTests(L"%.f",66.23);
	
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
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
//Function Name :vswprintf_Test6
//API Tested :vswprintf
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test6L()
    {
  	int ret;
	ret=vswprintfTests(L"%.3f",66.212313);

	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
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
//Function Name :vswprintf_Test7
//API Tested :vswprintf
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test7L()
    {
  	int ret;
	ret=vswprintfTests(L"%.2ls",L"abcdefg");

	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
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
//Function Name :vswprintf_Test8
//API Tested :vswprintf
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
	
TInt CTestLibcwchar::vswprintf_Test8L()
    {
  	int ret;
	ret=vswprintfTests(L"%-12ls",L"abcefg");	

	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8

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
//Function Name :vswprintf_Test9
//API Tested :vswprintf
//TestCase Description:To test whether right justification is provided for example 
//					   by %10d type of field width
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test9L()
    {
  	int ret;
	ret=vswprintfTests(L"%12ls",L"abcefg");
	

	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8

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
//Function Name :vswprintf_Test10
//API Tested :vswprintf
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test10L()
    {
  	int ret;
	ret=vswprintfTests(L"%-10d%3d",10,20);
		
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8

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
//Function Name :vswprintf_Test11
//API Tested :vswprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test11L()
    {
  	int ret;
	ret=vswprintfTests(L"%-10.3f",21.33345);
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8

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
//Function Name :vswprintf_Test12
//API Tested :vswprintf
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of digits
//					   to be copied after the decimal point in the float number and 
//					   the field width gives the alignment of the target string.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test12L()
    {
  	int ret;
	ret=vswprintfTests(L"%10.3f",21.33345);
	
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8

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
//Function Name :vswprintf_Test13
//API Tested :vswprintf
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test13L()
    {
  	int ret;
	ret=vswprintfTests(L"%-10.3ls",L"abcefg");
	

	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8

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
//Function Name :vswprintf_Test14
//API Tested :vswprintf
//TestCase Description:To test whether right justification occurs only if the number of 
//				       characters left after truncation above is less than the field width.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test14L()
    {
  	int ret;
	ret=vswprintfTests(L"%10.3ls",L"abcefg");
		
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
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
//Function Name :vswprintf_Test15
//API Tested :vswprintf
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test15L()
    {
  	int ret;
	ret=vswprintfTests(L"%010d",234);
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
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
//Function Name :vswprintf_Test16
//API Tested :vswprintf
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test16L()
    {
 	int ret;
	ret=vswprintfTests(L"%+f",10.23);
		
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
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
//Function Name :vswprintf_Test17
//API Tested :vswprintf
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test17L()
    {
  	int ret;
	ret=vswprintfTests(L"%   f",12.3);
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
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
//Function Name :vswprintf_Test18
//API Tested :vswprintf
//TestCase Description:To test whether for o conversions(eg. %#o), the first character
//					   of the output  string  is  made zero (by prefixing a 0 if it was
//					   not zero already)
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test18L()
    {
	int ret;
	ret=vswprintfTests(L"%#o",20);
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8

	
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
//Function Name :vswprintf_Test19
//API Tested :vswprintf
//TestCase Description:To test whether for x and X conversions(eg. %#x or %#X),
//					   a non-zero result has the string 0x (or 0X  for X  conversions)
//					   prepended to it.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test19L()
    {
	int ret;
	ret=vswprintfTests(L"%#x",20);
		
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
	
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
//Function Name :vswprintf_Test20
//API Tested :vswprintf
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test20L()
    {
	int ret;
	ret=vswprintfTests(L"%-010.4f",23.45434);
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8

	
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
//Function Name :vswprintf_Test21
//API Tested :vswprintf
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros 
// -----------------------------------------------------------------------------

TInt CTestLibcwchar::vswprintf_Test21L()
    {
	int ret;
	ret=vswprintfTests(L"%.6d",90);
	
	INFO_PRINTF2(_L("vswprintf returned %d"),ret); //Was L8
	
	
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
   
 TInt CTestLibcwchar::vswprintfTests( const wchar_t *fmt,...)
	{
	int ret;
	wchar_t dest[BUF_SIZE];
	va_list ap;
	va_start(ap,fmt);
	
	ret=vswprintf(dest,sizeof(dest),fmt,ap);
		
	INFO_PRINTF2(_L("vswprintf converted: %s"),dest ); //Was L8
	
	va_end(ap);
	return ret;
	}

TInt CTestLibcwchar::swprintf_Test22L()
    {
        
    const int MAX_CAPS_STRING_LENGTH = 500;

	wchar_t StringBuffer[MAX_CAPS_STRING_LENGTH] = { 0 };

    const wchar_t* const CAPS_FORMAT =
        L"Size: %ix%i\n"
        L"Supported color formats: %ls"
        L"Resolution: %idpi x %idpi\n"
        L"Backlight timeout: %ims\n"     
        L"Screensaver timeout: %ims\n"
        L"Variable brightness: %S\n"
        L"Variable gamma: %S\n"
        L"Variable contrast: %S";

	int SmX = 240;
	int SmY = 320;
	wchar_t colorFormat[] = L"XRGB888";
	int NmX = 128;
	int NmY = 128;
	int BrightnessTimeout = 15000;
	int ScreensaverTimeout = 15000;
    wchar_t GetVariableBrightness[] = L"No" ;
    wchar_t GetVariableGamma[] = L"no";
    wchar_t GetVariableContrast[] = L"no";
	
	int ret =    swprintf( StringBuffer,
                  MAX_CAPS_STRING_LENGTH,
                  CAPS_FORMAT,
                  SmX,
                  SmY,
                  colorFormat,
                  NmX,
                  NmY,
                  BrightnessTimeout,              
		          ScreensaverTimeout,
                  GetVariableBrightness,
                  GetVariableGamma ,
                  GetVariableContrast 
                  );

	INFO_PRINTF2(_L("swprintf converted %S"),StringBuffer); //Was L8
	INFO_PRINTF2(_L("swprintf returned %d"),ret); //Was L8
	
	if(ret>=0)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

