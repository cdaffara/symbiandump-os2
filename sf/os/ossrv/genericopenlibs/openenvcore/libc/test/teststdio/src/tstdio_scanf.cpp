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



// INCLUDE FILES

#include "tstdio.h"

//#define vfscanf __vfscanf

//**********************************scanf Tests******************************************

// -----------------------------------------------------------------------------
//Function Name :scanfTest1
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest1L(  )
    {
    
	int x,ret;
	printf("TD ID =1_10_2_1923\r\n");
	printf("Testing scanf for integer type\r\n");
	
	printf("Normal input:Enter a number like 999999\r\n");
	ret=scanf("%d",&x);
	getchar();
	printf("Expected Result:Converted value=999999,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -999999999\r\n");
	ret=scanf("%d",&x);
	getchar();
	printf("Expected Result:Converted value=-999999999,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=scanf("%d",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
	}




// -----------------------------------------------------------------------------
//Function Name :scanfTest2
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest2L(  )
    {
    
	float x;
	int ret;
	printf("TD ID =1_10_2_1921\r\n");
	printf("Testing scanf for float type\r\n");
	
	printf("Normal input:Enter a number like 99.99999\r\n");
	ret=scanf("%f",&x);
	getchar();
	printf("Expected Result:Converted value=99.999992,return value=1\r\n");
	printf("Output:Converted value=%f and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -9999.9999\r\n");
	ret=scanf("%f",&x);
	getchar();
	printf("Expected Result:Converted value=-10000.000000,return value=1\r\n");
	printf("Output:Converted value=%f and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=scanf("%f",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :scanfTest3
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion for exponential 
//					   type(%e) gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest3L(  )
    {
    
	float x;
	int ret;
	printf("TD ID =1_10_2_1920\r\n");
	printf("Testing scanf for exponential type\r\n");
	
	printf("Normal input:Enter a number like 99.99999\r\n");
	ret=scanf("%e",&x);
	getchar();
	printf("Expected Result:Converted value=9.999999E+01,return value=1\r\n");
	printf("Output:Converted value=%e and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -9999.9999\r\n");
	ret=scanf("%e",&x);
	getchar();
	printf("Expected Result:Converted value=-1.000000E+04,return value=1\r\n");
	printf("Output:Converted value=%e and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=scanf("%e",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :scanfTest4
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion for single character 
//					   type(%c) gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest4L(  )
    {
    
	char x;
	int ret;
	printf("TD ID =1_10_2_1919\r\n");
	printf("Testing scanf for character type\r\n");
	
	printf("Normal input:Enter a single character like 'a'\r\n");
	ret=scanf("%c",&x);
	getchar();
	printf("Expected Result:Converted=a,return value=1\r\n");
	printf("Output:Converted value=%c and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary input:Enter a special character like '@'\r\n");
	ret=scanf("%c",&x);
	getchar();
	printf("Expected Result:Converted=@,return value=1\r\n");
	printf("Output:Converted value=%c and return value=%d\r\n",x,ret);
	getchar();

	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :scanfTest5
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion for string type(%s) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest5L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char ch1='\0';
	printf("TD ID =1_10_2_1925\r\n");
	printf("Testing scanf for string type\r\n");
	
	printf("Normal input:Enter a string like 'abcdef123'\r\n");
	ret=scanf("%s",x);
	getchar();
	printf("Expected Result:Converted=abcdef123,return value=1\r\n");
	printf("Output:Converted value=%s and return value=%d\r\n",x,ret);
	printf("Press 1 to PASS or 0 to FAIL this case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
    
    if(ch1=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :scanfTest6
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest6L(  )
    {
    
	int x,ret;
	printf("TD ID =1_10_2_1924\r\n");
	printf("Testing scanf for octal conversion\r\n");
	
	printf("Normal input:Enter a number like 777777\r\n");
	ret=scanf("%o",&x);
	getchar();
	printf("Expected Result:Converted value=262143,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -777777777\r\n");
	ret=scanf("%o",&x);
	getchar();
	printf("Expected Result:Converted value=-134217727,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=scanf("%o",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :scanfTest7
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion for hexadecimal type(%x) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest7L(  )
    {
    
	int x,ret;
	printf("TD ID =1_10_2_1922\r\n");
	printf("Testing scanf for hexadecimal conversion\r\n");
	
	printf("Normal input:Enter a number like ffffff\r\n");
	ret=scanf("%x",&x);
	getchar();
	printf("Expected Result:Converted value=16777215,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -ffffff\r\n");
	ret=scanf("%x",&x);
	getchar();
	printf("Expected Result:Converted value=-16777215,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a character other than 0-9 and a-f \r\n");
	ret=scanf("%x",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :scanfTest8
//API Tested :scanf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest8L(  )
    {
    
	int dx1,dx2,nx1,nx2;
	int ret;
	printf("TD ID =1_10_2_1932\r\n");
	printf("Testing scanf for n-specifier\r\n");
	printf("Here the specifiers corresponding to percent-n are ignored\r\n");
	printf("and only the ones having the other integer-types are converted\r\n");
	
	printf("Normal input:Enter two integer numbers like 999998 and 999999\r\n");		
	ret=scanf("%d%n%n%d", &dx1,&nx1,&nx2,&dx2); 
	getchar();
	printf("Expected Result:Converted value=999998 and 999999,return value=2\r\n");
	printf("Output:Converted value=%d and %d and d return value=%d\r\n",dx1,dx2,ret);
	getchar();
	
	printf("Boundary value:Enter a number like 999999999 and -999999999\r\n");
	ret=scanf("%d%n%n%d", &dx1,&nx1,&nx2,&dx2); 
	getchar();
	printf("Expected Result:Converted value=999999999and -999999999,return value=2\r\n");
	printf("Output:Converted value=%d and %d  and return value=%d\r\n",dx1,dx2,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a\r\n");
	ret=scanf("%d%n%n%d", &dx1,&nx1,&nx2,&dx2); 
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :scanfTest9
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion occurs for a combination of 
//					   data types.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest9L(  )
    {
    
	int x;
	char str[BUF_SIZE];
	float f;
	int ret;
	printf("TD ID =1_10_2_1930\r\n");
	printf("Testing scanf for multiple conversions-\r\n");
	
	printf("Input with return after each input\r\n");
	printf("Enter an integer(999999) followed by a string(abcdef123)\r\n");
	printf("followed by a float(-9999.9999)\r\n");
	printf("Press enter after each input\r\n");
	ret=scanf("%d%s%f",&x,str,&f);
	getchar();
	printf("Expected Result:Converted value=999999 and -10000.000000 and abcdef123");
	printf("Return value=3\r\n");
	printf("Output:Converted value=%d and %f and %s return value=%d\r\n",x,f,str,ret);
	getchar();
	
	printf("Input with space between each input\r\n");
	printf("Enter an integer(999999) followed by a string(abcdef123)\r\n");
	printf("followed by a float(-9999.9999)\r\n");
	printf("Give space between inputs and enter after the third input\r\n");
	ret=scanf("%d%s%f",&x,str,&f);
	getchar();
	printf("Expected Result:Converted value=999999 and -10000.000000 and abcdef123");
	printf("Return value=3\r\n");
	printf("Output:Converted value=%d and %f and %s return value=%d\r\n",x,f,str,ret);
	getchar();
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :scanfTest10
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion for decimal type(%3d) 
//					   picks up the given no. of digits specified in the field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest10L(  )
    {
    
	int x,ret;
	printf("TD ID =1_10_2_1927\r\n");
	printf("Testing scanf-integer type with field width\r\n");
	
	printf("Normal input:Enter a number like 978\r\n");
	ret=scanf("%3d",&x);
	getchar();
	printf("Expected Result:Converted value=978,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Abnormal input:Enter a number like -99\r\n");
	ret=scanf("%3d",&x);
	getchar();
	printf("Expected Result:Converted value=-99,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=scanf("%3d",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :scanfTest11
//API Tested :scanf
  
  
//TestCase Description:To test whether the conversion for character type(%5c) 
//					   picks up the given no. of chars specified in the field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest11L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	printf("TD ID =1_10_2_1926\r\n");
	printf("Testing scanf-character type with field width\r\n");
	printf("Here the input automatically ends if the user gives more key-presses than \r\n");
	printf("the specified field width\r\n");

	printf("Normal/Boundary input:Enter 5 characters like x45@y\r\n");
	ret=scanf("%5c",x);
	getchar();x[5]='\0';
	printf("Expected Result:Converted =x45@y,return value=1\r\n");
	printf("Output:Converted value=%s and return value=%d\r\n",x,ret);
	getchar();

	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
	    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :scanfTest12
//API Tested :scanf
  
  
//TestCase Description:To test whether the match character format string [] picks 
//					   up the matched characters according to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest12L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	printf("TD ID =1_10_2_1929\r\n");
	printf("Testing scanf for match character operation\r\n");
	printf("Here the conversion automatically ends if the user presses inputs \r\n");
	printf("not included in the search grammar\r\n");

	printf("Normal/Boundary input for grammar [54321]:Enter characters like 5120\r\n");
	ret=scanf("%[54321]",x);
	getchar();
	printf("Expected Result:Converted =512,return value=1\r\n");
	printf("Output:Converted value=%s and return value=%d\r\n",x,ret);
	getchar();

	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :scanfTest13
//API Tested :scanf,fscanf,sscanf
  
  
//TestCase Description:To test whether the match character format string with NOT 
//					   operator [^---] picks up the matched characters according 
//				       to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::scanfTest13L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char ch1='\0';
	printf("TD ID =1_10_2_1928\r\n");
	printf("Testing scanf for match character negation operation\r\n");
	printf("Here the conversion automatically ends if the user presses inputs \r\n");
	printf("not included in the search grammar\r\n");

	printf("Normal/Boundary input for grammar [^543]:Enter characters like 12975\r\n");
	ret=scanf("%[^543]",x);
	getchar();
	printf("Expected Result:Converted =1297,return value=1\r\n");
	printf("Output:Converted value=%s and return value=%d\r\n",x,ret);
	printf("Press 1 to PASS or 0 to FAIL this case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
    
 
    if(ch1=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}    
    }


//**********************************sscanf Tests***************************************

// -----------------------------------------------------------------------------
//Function Name :sscanfTest1
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest1L(  )
    {
    
	int x,ret;
	TPtrC string;
    TBuf8<100> buf1;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%d",&x);
	
	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);		

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%d",&x);
	
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :sscanfTest2
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest2L(  )
    {
    
	float x;
	int ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%f",&x);
	
	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%f",&x);
	
 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest3
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion for float type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest3L(  )
    {
    
	float x;
	int ret;
	TPtrC string;
    TBuf8<100> buf1;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%e",&x);
	
	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%e",&x);

 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest4
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion for single character 
//					   type(%c) gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest4L(  )
    {
    
	char x;
	int ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%c",&x);
	
	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%c",&x);

 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest5
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion for string type(%s) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest5L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	TPtrC string;
    TBuf8<100> buf1;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%s",x);
	
 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest6
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest6L(  )
    {
    
	int x,ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%o",&x);
	
	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%o",&x);

 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :sscanfTest7
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion for hex  type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest7L(  )
    {
    
	int x,ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%x",&x);
	
	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%x",&x);

 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest8
//API Tested :sscanf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest8L(  )
    {
    
	int dx1,dx2,nx1,nx2,ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    _LIT(KBlankSpace," ");
    buf1.Append(KBlankSpace);
    buf1.Append(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%d%n%n%d",&dx1,&nx1,&nx2,&dx2);

	buf1.Delete(0,100);
	
	GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	

    buf1.Copy(string);
    
    GetStringFromConfig(ConfigSection(),_L("parameter4"), string);	

    buf1.Append(KBlankSpace);
    buf1.Append(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%d%n%n%d",&dx1,&nx1,&nx2,&dx2);

 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest9
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion occurs for a combination of 
//					   data types.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest9L(  )
    {
    
	int x;
	char str[BUF_SIZE];
	float f;
	int ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    _LIT(KBlankSpace," ");
    buf1.Append(KBlankSpace);
    buf1.Append(string);
    
    GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	

    buf1.Append(KBlankSpace);
    buf1.Append(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%d%s%f",&x,str,&f);

 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest10
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%3d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest10L(  )
    {
    
	int x,ret;
	TPtrC string;
    TBuf8<100> buf1;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%3d",&x);
	
	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%3d",&x);
	
 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest11
//API Tested :sscanf
  
  
//TestCase Description:To test whether the conversion for string type(%5c) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest11L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%5c",x);
	
 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest12
//API Tested :sscanf
  
  
//TestCase Description:To test whether the match character format string [] picks 
//					   up the matched characters according to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest12L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%[54321]",x);
	
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest13
//API Tested :sscanf
  
  
//TestCase Description:To test whether the match character format string [^--] picks 
//					   up the matched characters according to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest13L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%[^9870]",x);

 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :sscanfTest14
//API Tested :sscanf
  
  
//TestCase Description:To test whether the * format specifier works.Here the type 
//					   with * prefixed must be ignored and other conversions occur
//					   normally.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sscanfTest14L(  )
    {
    
	char x1[BUF_SIZE],x2[BUF_SIZE];
	int ret;
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    _LIT(KBlankSpace," ");
    buf1.Append(KBlankSpace);
    buf1.Append(string);
    GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	

    buf1.Append(KBlankSpace);
    buf1.Append(string);
    GetStringFromConfig(ConfigSection(),_L("parameter4"), string);	

    buf1.Append(KBlankSpace);
    buf1.Append(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	ret=sscanf(input,"%*d%s%*f%s",x1,x2);
	
 
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


//**********************************fscanf Tests***************************************

/// -----------------------------------------------------------------------------
//Function Name :fscanfTest1
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest1L(  )
    {
    
	int x,ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%d",123);
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%d",&x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );
	
	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest2
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest2L(  )
    {
    
	float x;
	int ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%f",123.345);
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%f",&x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest3
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion for float type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest3L(  )
    {
    
	float x;
	int ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%f",123.345);
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%e",&x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest4
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion for single character 
//					   type(%c) gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest4L(  )
    {
    
	char x='a';
	int ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%c",x);
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%c",&x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest5
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion for string type(%s) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest5L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","abcdefg");
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%s",x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

/// -----------------------------------------------------------------------------
//Function Name :fscanfTest6
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest6L(  )
    {
    
	int x,ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%d",123);
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%o",&x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


/// -----------------------------------------------------------------------------
//Function Name :fscanfTest7
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion for hex type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest7L(  )
    {
    
	int x,ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%d",123);
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%x",&x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest8
//API Tested :fscanf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest8L(  )
    {
    
	int dx1,dx2,nx1,nx2,ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","12334 22");
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%d%n%n%d", &dx1,&nx1,&nx2,&dx2); 
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest9
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion occurs for a combination of 
//					   data types.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest9L(  )
    {
    
	int x;
	char str[BUF_SIZE];
	float f;
	int ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","345 zxcvb 454.45453");
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%d%s%f",&x,str,&f);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

/// -----------------------------------------------------------------------------
//Function Name :fscanfTest10
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%3d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest10L(  )
    {
    
	int x,ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%d",13323);
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%3d",&x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest11
//API Tested :fscanf
  
  
//TestCase Description:To test whether the conversion for string type(%5c) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest11L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","abcdesdafg");
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%5c",x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest12
//API Tested :fscanf
  
  
//TestCase Description:To test whether the match character format string [] picks 
//					   up the matched characters according to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest12L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","3226543");
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%[54321]",x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest13
//API Tested :fscanf
  
  
//TestCase Description:To test whether the match character format string [^--] picks 
//					   up the matched characters according to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest13L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","689543");
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%[^54321]",x);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :fscanfTest14
//API Tested :fscanf
  
  
//TestCase Description:To test whether the * format specifier works.Here the type 
//					   with * prefixed must be ignored and other conversions occur
//					   normally.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fscanfTest14L(  )
    {
    
	char x1[BUF_SIZE],x2[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest1.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","23 abcd 45.66 xyz");
	fclose(fp);

	fp=fopen(filename,"r");
	ret=fscanf(fp,"%*d%s%*f%s",x1,x2);
	fclose(fp);
	INFO_PRINTF2(_L("fscanf returned:%d"),ret );


	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


//**********************************vscanf Tests***************************************

// -----------------------------------------------------------------------------
//Function Name :vscanfTest1
//API Tested :vscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest1L(  )
    {
    
	int x,ret;
	printf("TD ID =1_10_2_1980\r\n");
	printf("Testing vscanf for integer type\r\n");
	
	printf("Normal input:Enter a number like 999999\r\n");
	ret=vscanfTests("%d",&x);
	getchar();
	printf("Expected Result:Converted value=999999,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -999999999\r\n");
	ret=vscanfTests("%d",&x);
	getchar();
	printf("Expected Result:Converted value=-999999999,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=vscanfTests("%d",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}

    
    }


// -----------------------------------------------------------------------------
//Function Name :vscanfTest2
//API Tested :vscanf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest2L(  )
    {
    
	float x;
	int ret;
	printf("TD ID =1_10_2_1988\r\n");
	printf("Testing vscanf for float type\r\n");
	
	printf("Normal input:Enter a number like 99.99999\r\n");
	ret=vscanfTests("%f",&x);
	getchar();
	printf("Expected Result:Converted value=99.999992,return value=1\r\n");
	printf("Output:Converted value=%f and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -9999.9999\r\n");
	ret=vscanfTests("%f",&x);
	getchar();
	printf("Expected Result:Converted value=-10000.000000,return value=1\r\n");
	printf("Output:Converted value=%f and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=vscanfTests("%f",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}

    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest3
//API Tested :vscanf
  
  
//TestCase Description:To test whether the conversion for float type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest3L(  )
    {
    
	float x;
	int ret;
	printf("TD ID =1_10_2_1992\r\n");
	printf("Testing vscanf for exponential type\r\n");
	
	printf("Normal input:Enter a number like 99.99999\r\n");
	ret=vscanfTests("%e",&x);
	getchar();
	printf("Expected Result:Converted value=9.999999E+01,return value=1\r\n");
	printf("Output:Converted value=%e and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -9999.9999\r\n");
	ret=vscanfTests("%e",&x);
	getchar();
	printf("Expected Result:Converted value=-1.000000E+04,return value=1\r\n");
	printf("Output:Converted value=%e and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=vscanfTests("%e",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest4
//API Tested :vscanf
  
  
//TestCase Description:To test whether the conversion for single character 
//					   type(%c) gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest4L(  )
    {
    
	char x;
	int ret;
	printf("TD ID =1_10_2_1996\r\n");
	printf("Testing vscanf for character type\r\n");
	
	printf("Normal input:Enter a single character like 'a'\r\n");
	ret=vscanfTests("%c",&x);
	getchar();
	printf("Expected Result:Converted=a,return value=1\r\n");
	printf("Output:Converted value=%c and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary input:Enter a special character like '@'\r\n");
	ret=vscanfTests("%c",&x);
	getchar();
	printf("Expected Result:Converted=@,return value=1\r\n");
	printf("Output:Converted value=%c and return value=%d\r\n",x,ret);
	getchar();

	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest5
//API Tested :vscanf
  
  
//TestCase Description:To test whether the conversion for string type(%s) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest5L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char ch1='\0';
	printf("TD ID =1_10_2_1972\r\n");
	printf("Testing vscanf for string type\r\n");
	
	printf("Normal input:Enter a string like 'abcdef123'\r\n");
	ret=vscanfTests("%s",x);
	getchar();
	printf("Expected Result:Converted=abcdef123,return value=1\r\n");
	printf("Output:Converted value=%s and return value=%d\r\n",x,ret);
	printf("Press 1 to PASS or 0 to FAIL this case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
    
 
    if(ch1=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest6
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest6L(  )
    {
    
	int x,ret;
	printf("TD ID =1_10_2_1976\r\n");
	printf("Testing vscanf for octal conversion\r\n");
	
	printf("Normal input:Enter a number like 777777\r\n");
	ret=vscanfTests("%o",&x);
	getchar();
	printf("Expected Result:Converted value=262143,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -777777777\r\n");
	ret=vscanfTests("%o",&x);
	getchar();
	printf("Expected Result:Converted value=-134217727,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=vscanfTests("%o",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest7
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for hex  type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest7L(  )
    {
    
	int x,ret;
	printf("TD ID =1_10_2_1984\r\n");
	printf("Testing vscanf for hexadecimal conversion\r\n");
	
	printf("Normal input:Enter a number like ffffff\r\n");
	ret=vscanfTests("%x",&x);
	getchar();
	printf("Expected Result:Converted value=16777215,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Boundary value:Enter a number like -ffffff\r\n");
	ret=vscanfTests("%x",&x);
	getchar();
	printf("Expected Result:Converted value=-16777215,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a character other than 0-9 and a-f \r\n");
	ret=vscanfTests("%x",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest8
//API Tested :vscanf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of  characters 
//					   consumed thus  far  from  the  input  is stored  through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest8L(  )
    {
    
	int dx1,dx2,nx1,nx2;
	int ret;
	printf("TD ID =1_10_2_1949\r\n");
	printf("Testing vscanf for n-specifier\r\n");
	printf("Here the specifiers corresponding to percent-n are ignored\r\n");
	printf("and only the ones having the other integer-types are converted\r\n");
	
	printf("Normal input:Enter two integer numbers like 999998 and 999999\r\n");		
	ret=vscanfTests("%d%n%n%d", &dx1,&nx1,&nx2,&dx2); 
	getchar();
	printf("Expected Result:Converted value=999998 and 999999,return value=2\r\n");
	printf("Output:Converted value=%d and %d and d return value=%d\r\n",dx1,dx2,ret);
	getchar();
	
	printf("Boundary value:Enter a number like 999999999 and -999999999\r\n");
	ret=vscanfTests("%d%n%n%d", &dx1,&nx1,&nx2,&dx2); 
	getchar();
	printf("Expected Result:Converted value=999999999and -999999999,return value=2\r\n");
	printf("Output:Converted value=%d and %d  and return value=%d\r\n",dx1,dx2,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral character like a\r\n");
	ret=vscanfTests("%d%n%n%d", &dx1,&nx1,&nx2,&dx2); 
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest9
//API Tested :vscanf
  
  
//TestCase Description:To test whether the conversion occurs for a combination of 
//					   data types.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest9L(  )
    {
    
	int x;
	char str[BUF_SIZE];
	float f;
	int ret;
	printf("TD ID =1_10_2_1960\r\n");
	printf("Testing vscanf for multiple conversions-\r\n");
	
	printf("Input with return after each input\r\n");
	printf("Enter an integer(999999) followed by a string(abcdef123)\r\n");
	printf("followed by a float(-9999.9999)\r\n");
	printf("Press enter after each input\r\n");
	ret=vscanfTests("%d%s%f",&x,str,&f);
	getchar();
	printf("Expected Result:Converted value=999999 and -10000.000000 and abcdef123");
	printf("Return value=3\r\n");
	printf("Output:Converted value=%d and %f and %s return value=%d\r\n",x,f,str,ret);
	getchar();
	
	printf("Input with space between each input\r\n");
	printf("Enter an integer(999999) followed by a string(abcdef123)\r\n");
	printf("followed by a float(-9999.9999)\r\n");
	printf("Give space between inputs and enter after the third input\r\n");
	ret=vscanfTests("%d%s%f",&x,str,&f);
	getchar();
	printf("Expected Result:Converted value=999999 and -10000.000000 and abcdef123");
	printf("Return value=3\r\n");
	printf("Output:Converted value=%d and %f and %s return value=%d\r\n",x,f,str,ret);
	getchar();
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :vscanfTest10
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%3d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest10L(  )
    {
    
	int x,ret;
	printf("TD ID =1_10_2_1953\r\n");
	printf("Testing vscanf-integer type with field width\r\n");
	
	printf("Normal input:Enter a number like 978\r\n");
	ret=vscanfTests("%3d",&x);
	getchar();
	printf("Expected Result:Converted value=978,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Abnormal input:Enter a number like -99\r\n");
	ret=vscanfTests("%3d",&x);
	getchar();
	printf("Expected Result:Converted value=-99,return value=1\r\n");
	printf("Output:Converted value=%d and return value=%d\r\n",x,ret);
	getchar();
	
	printf("Bad input:Enter a non-numeral like character-a \r\n");
	ret=vscanfTests("%3d",&x);
	getchar();
	printf("Expected Result:No conversion occurs,Return value=0\r\n");
	printf("Output:Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest11
//API Tested :vscanf
  
  
//TestCase Description:To test whether the conversion for string type(%5c) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest11L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	printf("TD ID =1_10_2_1957\r\n");
	printf("Testing vscanf-character type with field width\r\n");
	printf("Here the input automatically ends if the user gives more key-presses than \r\n");
	printf("the specified field width\r\n");

	printf("Normal/Boundary input:Enter 5 characters like x45@y\r\n");
	ret=vscanfTests("%5c",x);
	getchar();x[5]='\0';
	printf("Expected Result:Converted =x45@y,return value=1\r\n");
	printf("Output:Converted value=%s and return value=%d\r\n",x,ret);
	getchar();

	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest12
//API Tested :vscanf
  
  
//TestCase Description:To test whether the match character format string [] picks 
//					   up the matched characters according to the specified  grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest12L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	printf("TD ID =1_10_2_1968\r\n");
	printf("Testing vscanf for match character operation\r\n");
	printf("Here the conversion automatically ends if the user presses inputs \r\n");
	printf("not included in the search grammar\r\n");

	printf("Normal/Boundary input for grammar [54321]:Enter characters like 5120\r\n");
	ret=vscanfTests("%[54321]",x);
	getchar();
	printf("Expected Result:Converted =512,return value=1\r\n");
	printf("Output:Converted value=%s and return value=%d\r\n",x,ret);
	getchar();

	printf("Press 1 to PASS or 0 to FAIL this case:");
	scanf("%d",&ret);
 
    
	if(ret==0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vscanfTest13
//API Tested :vscanf
  
  
//TestCase Description:To test whether the match character format string [^--] picks 
//					   up the matched characters according to the specified  grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vscanfTest13L(  )
    {
    
	char x[BUF_SIZE];
	char ch1='\0';
	int ret;
	printf("TD ID =1_10_2_1964\r\n");
	printf("Testing vscanf for match character negation operation\r\n");
	printf("Here the conversion automatically ends if the user presses inputs \r\n");
	printf("not included in the search grammar\r\n");

	printf("Normal/Boundary input for grammar [^543]:Enter characters like 12975\r\n");
	ret=vscanfTests("%[^543]",x);
	getchar();
	printf("Expected Result:Converted =1297,return value=1\r\n");
	printf("Output:Converted value=%s and return value=%d\r\n",x,ret);
	printf("Press 1 to PASS or 0 to FAIL this case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
    
 
    if(ch1=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }



//**********************************vsscanf Tests***************************************

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest1
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest1L(  )
    {
    
	int x,ret;
	ret=vsscanfTests("123","%d",&x);
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );

	
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest2
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest2L(  )
    {
    
	float x;
	int ret;
	ret=vsscanfTests("123.345","%f",&x);
	
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );

 
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest3
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for float type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest3L(  )
    {
    
	float x;
	int ret;
	ret=vsscanfTests("123.345","%e",&x);
	
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );

    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest4
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for single character 
//					   type(%c) gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest4L(  )
    {
    
	char x;
	int ret;
	ret=vsscanfTests("abcd","%c",&x);
	
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest5
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for string type(%s) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest5L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	ret=vsscanfTests("abcd","%s",x);
	
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );

 
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest6
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest6L(  )
    {
    
	int x,ret;
	ret=vsscanfTests("123","%o",&x);
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );
	
 
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest7
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for hex  type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest7L(  )
    {
    
	int x,ret;
	ret=vsscanfTests("123","%x",&x);
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );
 
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest8
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest8L(  )
    {
    
	int dx1,dx2,nx1,nx2,ret;
	ret=vsscanfTests("12334 56","%d%n%n%d", &dx1,&nx1,&nx2,&dx2); 
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );
 
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest9
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion occurs for a combination of 
//					   data types.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest9L(  )
    {
    
	int x;
	char str[BUF_SIZE];
	float f;
	int ret;
	ret=vsscanfTests("345 zxcvb 454.45453","%d%s%f",&x,str,&f);
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );

	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest10
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%3d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest10L(  )
    {
    
	int x,ret;
	ret=vsscanfTests("12343","%3d",&x);
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );

	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest11
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the conversion for string type(%5c) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest11L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	ret=vsscanfTests("abgfhdfcd","%5c",x);
	
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );
 
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest12
//API Tested :vsscanf,fscanf,sscanf
  
  
//TestCase Description:To test whether the match character format string [^] picks 
//					   up the matched characters according to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest12L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	ret=vsscanfTests("5337452","%[54321]",x);
	
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );
 
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest13
//API Tested :vsscanf,fscanf,sscanf
  
  
//TestCase Description:To test whether the match character format string [^--] picks 
//					   up the matched characters according to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest13L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	ret=vsscanfTests("79752","%[^54321]",x);
	
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );
 
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsscanfTest14
//API Tested :vsscanf
  
  
//TestCase Description:To test whether the * format specifier works.Here the type 
//					   with * prefixed must be ignored and other conversions occur
//					   normally.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsscanfTest14L(  )
    {
    
	char x1[BUF_SIZE],x2[BUF_SIZE];
	int ret;
	ret=vsscanfTests("23 abcd 45.66 xyz","%*d%s%*f%s",x1,x2);
	
	INFO_PRINTF2(_L("vsscanf returned:%d"),ret );
 
    
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


//**********************************vfscanf Tests***************************************

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest1
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest1L(  )
    {
    
	int x,ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%d",123);
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%d",&x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);	
	unlink(filename);
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest2
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest2L(  )
    {
    
	float x;
	int ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%f",123.345);
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%f",&x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
	fclose(fp);
	unlink(filename);
 
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :vfscanfTest3
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion for float type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest3L(  )
    {
    
	float x;
	int ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%f",123.345);
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%e",&x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret ); 
	fclose(fp);
	unlink(filename);
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest4
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion for single character 
//					   type(%c) gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest4L(  )
    {
    
	char x='a';
	int ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%c",x);
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%c",&x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :vfscanfTest5
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion for string type(%s) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest5L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","abced");
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%s",x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest6
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest6L(  )
    {
    
	int x,ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%d",123);
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%o",&x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
 	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest7
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion for hex  type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest7L(  )
    {
    
	int x,ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%d",123);
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%x",&x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest8
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest8L(  )
    {
    
	int dx1,dx2,nx1,nx2,ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","12334 56");
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%d%n%n%d", &dx1,&nx1,&nx2,&dx2); 
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest9
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion occurs for a combination of 
//					   data types.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest9L(  )
    {
    
	int x;
	char str[BUF_SIZE];
	float f;
	int ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","345 zxcvb 454.45453");
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%d%s%f",&x,str,&f);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
 	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest10
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion for integer type(%3d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest10L(  )
    {
    
	int x,ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%d",12233);
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%3d",&x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
 	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :vfscanfTest11
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the conversion for string type(%5c) 
//					   gives the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest11L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","abcedfdfd");
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%5c",x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
   	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :vfscanfTest12
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the match character format string [] picks 
//					   up the matched characters according to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest12L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","3225278");
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%[54321]",x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
   	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest13
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the match character format string [^--] picks 
//					   up the matched characters according to the specified grammar
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest13L(  )
    {
    
	char x[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","7894535");
	fclose(fp);
	fp=fopen(filename,"r");

	ret=vfscanfTests(fp,"%[^54321]",x);
	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
    if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :vfscanfTest14
//API Tested :vfscanf
  
  
//TestCase Description:To test whether the * format specifier works.Here the type 
//					   with * prefixed must be ignored and other conversions occur
//					   normally.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfscanfTest14L(  )
    {
    
	char x1[BUF_SIZE],x2[BUF_SIZE];
	int ret;
	char* filename="c:\\ScanfTest.txt";
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%s","23 abcd 45.66 xyz");
	fclose(fp);
	
	fp=fopen(filename,"r");	
	ret=vfscanfTests(fp,"%*d%s%*f%s",x1,x2);	

	INFO_PRINTF2(_L("vfscanf returned:%d"),ret );
 	fclose(fp);
	unlink(filename);
	if(ret<=0)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
    
    }
    


TInt CTestStdio::vscanfTests (const char *fmt, ...)
	{
	int ret;
	va_list ap;
	va_start (ap, fmt);

	ret=vscanf(fmt,ap);
	INFO_PRINTF2(_L("vscanfTests returned:%d"),ret );

	va_end (ap);
	return ret;
	}

TInt CTestStdio::vsscanfTests (const char *str,const char *fmt, ...)
	{
	int ret;
	va_list ap;
	va_start (ap, fmt);

	ret=vsscanf(str,fmt,ap);
	INFO_PRINTF2(_L("vsscanfTests returned:%d"),ret );

	va_end (ap);
	return ret;
	}

TInt CTestStdio::vfscanfTests (FILE* fp,const char *fmt, ...)
	{
	int ret;
	va_list ap;
	va_start (ap, fmt);
	
	ret=vfscanf(fp,fmt,ap);	
	
	INFO_PRINTF2(_L("vfscanfTests returned:%d"),ret );

	va_end (ap);
	return ret;
	}



// -----------------------------------------------------------------------------
// CTestStdio::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CTestStdio::TestwscanfForStr()
	{
/*	char str[BUF_SIZE];
	int ret;
	char ch1='\0';
	INFO_PRINTF1(_L("Testing wscanf for string type\r\n"));
	ret=0;
//	ret=wscanf(_L"%s",str);
	
	INFO_PRINTF2(_L("wscanf returned %d"),ret)*/
	return EPass;
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
