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



#include "tstdio.h"


//******************************Printf Tests******************************************


// -----------------------------------------------------------------------------
//Function Name :printfTest1
//API Tested :printf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest1L(  )
    {
    
	int x,y,ret;
	char ch1='\0';
	char ch2='\0';	
	printf("TD ID = 1_10_2_1683\r\n");
	printf("Testing printf for integer types\r\n");
	_LIT( Kx, "x" );
	TBool res = GetIntFromConfig(ConfigSection(), Kx, x);
	printf("For normal input,Api Output is:\r\n");
    ret=printf("%d",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);
		
	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	

	_LIT( Ky, "y" );
	res = GetIntFromConfig(ConfigSection(), Ky, y);
	
    
    printf("For boundary input,Api Output is:\r\n");
    ret=printf("%d",y);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
    

    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :printfTest2
//API Tested :printf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest2L(  )
    {
    
	int ret;
	float x;
	char ch1='\0';
	char ch2='\0';
	printf("TD ID = 1_10_2_1684\r\n");
	printf("Testing printf for float/double types\r\n");
    TPtrC string1,string2;
    TBuf8<100> buf1;
    
    _LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1);
    buf1.Copy(string1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For normal input,Api Output is:\r\n");
    x=atof(input);
    ret=printf("%f",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	
	buf1.Delete(0,100);
	_LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2 );
		
    buf1.Copy(string2);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For Boundary input:Api Output is:\r\n");
    x=atof(input);
    ret=printf("%f",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
    
    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :printfTest3
//API Tested :printf
  
  
//TestCase Description:To test whether the conversion for exponential type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest3L(  )
    {
    
	int ret;
	float x;
	char ch1='\0';
	char ch2='\0';
	printf("TD ID = 1_10_2_1685\r\n");
	printf("Testing printf for exponential types\r\n");
    TPtrC string1;
    TPtrC string2;
    TBuf8<100> buf1;
	_LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1 );
    buf1.Copy(string1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For normal input,Api Output is:\r\n");
    x=atof(input);
    ret=printf("%e",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	
	buf1.Delete(0,100);
	_LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2 );
    
	buf1.Copy(string2);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For Boundary input:Api Output is:\r\n");
    x=atof(input);
    ret=printf("%e",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
    
    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :printfTest4
//API Tested :printf
  
  
//TestCase Description:To test whether the conversion for character type(%c) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest4L(  )
    {
    
	int ret;
	char ch1='\0';
	printf("TD ID = 1_10_2_1686\r\n");
	printf("Testing printf for character type\r\n");

	TPtrC x;
	
	_LIT( Kx, "x" );
	TBool res = GetStringFromConfig(ConfigSection(), Kx, x);
	printf("For normal input,Api Output is:\r\n");
    ret=printf("%c",(char)x[0]);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
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
//Function Name :printfTest5
//API Tested :printf
  
  
//TestCase Description:To test whether the conversion for string type(%s) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest5L(  )
    {
    
	int ret;
	char ch1='\0';
	printf("TD ID = 1_10_2_1687\r\n");
	printf("Testing printf for string type\r\n");
    TPtrC string;
    TBuf8<100> buf1;
	
	_LIT( Kstring, "string" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring, string);
    
    buf1.Copy(string);
    char* x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    printf("For normal input,Api Output is:\r\n");
    ret=printf("%s",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
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
//Function Name :printfTest6
//API Tested :printf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest6L(  )
    {
    
	int x1,x2,x3,ret;
	char ch1='\0';
	char ch2='\0';
	char ch3='\0';
	printf("TD ID = 1_10_2_1688\r\n");
	printf("Testing printf for octal type\r\n");
	
	_LIT( Kx1, "x1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kx1, x1);
    printf("For normal input,Api Output is:\r\n");
    ret=printf("%o",x1);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);
	
	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	
	_LIT( Kx2, "x2" );
	res = GetIntFromConfig(ConfigSection(), Kx2, x2);
	
    printf("For boundary input,Api Output is:\r\n");
    ret=printf("%o",x2);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);
	
	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
	_LIT( Kx3, "x3" );
	res = GetIntFromConfig(ConfigSection(), Kx3, x3);

    printf("For abnormal input,Api Output is:\r\n");
    ret=printf("%o",x3);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);
	
	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch3=getchar();
		}while(ch3!='1' && ch3!='0');
	
    if(ch1=='1' && ch2=='1' && ch3=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :printfTest7
//API Tested :printf
  
  
//TestCase Description:To test whether the conversion for hexadecimal type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest7L(  )
    {
    
	int ret;
    TPtrC string;
    TBuf8<100> buf1;
    char ch1='\0';
    printf("TD ID = 1_10_2_1689\r\n");
	printf("Testing printf for hexadecimal type\r\n");
   
   	_LIT( Kstring, "string" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring, string );

    buf1.Copy(string);
    char* x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    printf("For normal input,Api Output is:\r\n");
    ret=printf("%x",(int)x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
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
//Function Name :printfTest8
//API Tested :printf
  
  
//TestCase Description:To test whether the conversion for combination of data types
//					   to get the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest8L(  )
    {
    
	int ret,x;
	float y;
	char* z;	
	char ch1='\0';
	printf("TD ID = 1_10_2_1690\r\n");
	printf("Testing printf for combination of types\r\n");
    TPtrC string1,string2;
    
    TBuf8<100> buf1;

	_LIT( Kx, "x" );
	 TBool res = GetIntFromConfig(ConfigSection(), Kx, x);

 
	_LIT( Kstring1, "string1" );
	res = GetStringFromConfig(ConfigSection(), Kstring1, string1 );
    
    buf1.Copy(string1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    y=atof(input);
    buf1.Delete(0,100);
    
    _LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2);
    buf1.Copy(string2);
    z = (char*) buf1.Ptr();
    z[buf1.Length()]='\0';
	ret=printf("For normal input,Api Output is:\r\n");
	
	ret=printf("%s %f %d",z,y,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest9
//API Tested :printf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest9L(  )
    {
    
	int x,ret;
	char ch1='\0';
	printf("TD ID = 1_10_2_1691\r\n");
	char* fmt="%d %n %d";
	printf("Testing printf for n-specifer-pointer to integer type\r\n");

	_LIT( Kx, "x" );
	TBool res = GetIntFromConfig(ConfigSection(), Kx, x);
	
	int *y=&x;
	ret=printf("Normal input:Api Output is:\r\n");
	ret=printf(fmt,x,y);
 
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest10
//API Tested :printf
  
  
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest10L(  )
    {
    
	int ret,x;
	char ch1='\0';
	printf("TD ID = 1_10_2_1692\r\n");
	printf("Testing printf for multiple-percentage in fmt string\r\n");


	_LIT( Kx, "x" );
	TBool res = GetIntFromConfig(ConfigSection(), Kx, x);

	ret=printf("Normal input:Api Output is:\r\n");
	ret=printf("%%%d%%",x);
 
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest11
//API Tested :printf
  
  
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest11L(  )
    {
    
	int ret,x,y;
	char ch1='\0';
	printf("TD ID = 1_10_2_1693\r\n");
	printf("Testing printf for asterix-operator in fmt string\r\n");
	
	_LIT( Kx, "x" );
	TBool res = GetIntFromConfig(ConfigSection(), Kx, x);
	
	_LIT( Ky, "y" );
	res = GetIntFromConfig(ConfigSection(), Ky, y);
	
	ret=printf("Normal input:Api Output is:\r\n");
	ret=printf("%*d",x,y);
 
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest12
//API Tested :printf
  
  
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest12L(  )
    {
    
	int ret;
	float x;
	char ch1='\0';
	char ch2='\0';
	char* fmt="%.f";
	printf("TD ID = 1_10_2_1694\r\n");
	printf("Testing printf for float with precision operator and without the value\r\n");
    TPtrC string1,string2;
    TBuf8<100> buf1;
    
    _LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1 );
    buf1.Copy(string1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For normal input,Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	
	buf1.Delete(0,100);
	
	_LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2 );
	
    buf1.Copy(string2);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For Boundary input:Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
  
    }

// -----------------------------------------------------------------------------
//Function Name :printfTest13
//API Tested :printf,fprintf,sprintf,snprintf
  
  
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest13L(  )
    {
    
	int ret;
	float x;
	char ch1='\0';
	char ch2='\0';
	char* fmt="%.3f";
	printf("TD ID = 1_10_2_1695\r\n");
	printf("Testing printf for float with precision in format string\r\n");
    TPtrC string1,string2;
    TBuf8<100> buf1;

	_LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1 );
    
    buf1.Copy(string1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For normal input,Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	
	buf1.Delete(0,100);
	
	_LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2 );
	
    buf1.Copy(string2);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For Boundary input:Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :printfTest14
//API Tested :printf,fprintf,sprintf,snprintf
  
  
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest14L(  )
    {
    
	int ret;
	char* x;
	char ch1='\0';
	char ch2='\0';	
	char* fmt="%.5s";
	printf("TD ID = 1_10_2_1696\r\n");
	printf("Testing printf Precision with string(1_10_2_1696)\r\n");
    TPtrC string1,string2;
    TBuf8<100> buf1;
    
    _LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1 );
    
    buf1.Copy(string1);
    x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    printf("For normal input:String length>precision,Api Output is:\r\n");
    ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	
	buf1.Delete(0,100);
	
	_LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2 );
	
    buf1.Copy(string2);
    x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    printf("Abnormal input:String length<precision,Api Output is:\r\n");
   	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
    
    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	
    }



// -----------------------------------------------------------------------------
//Function Name :printfTest15
//API Tested :printf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest15L(  )
    {
    
	int ret;
	char* x;
	char ch1='\0';
	char* fmt="%-12s";
	printf("TD ID = 1_10_2_1697\r\n");
	printf("Testing printf for field width with left justification\r\n");
    TPtrC string;
    TBuf8<100> buf1;
	
	_LIT( Kstring, "string" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring, string );
    
    buf1.Copy(string);
    x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    printf("Given are the original and actual outputs:\r\n");
    printf("%s\r\n",x);
    ret=printf(fmt,x);
    printf("|");
    printf("\r\n The pipe char above signifies the end of conversion");
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest16
//API Tested :printf
  
  
//TestCase Description:To test whether right justification is provided for example 
//					   by %10d type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest16L(  )
    {
    
	int ret;
	char* x;
	char ch1='\0';
	char* fmt="%12s";
	printf("TD ID = 1_10_2_1698\r\n");
	printf("Testing printf for field width with right justification\r\n");
    TPtrC string;
    TBuf8<100> buf1;

    _LIT( Kstring, "string" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring, string );
    buf1.Copy(string);
    x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    printf("Given are the original and actual outputs:\r\n");
    printf("%s\r\n",x);
    ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest17
//API Tested :printf
  
  
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest17L(  )
    {

	int ret;
	char *x,*y;
	char ch1='\0';
    char* fmt="%-10s%3s";
    printf("TD ID = 1_10_2_1702\r\n");
	printf("Testing printf for field width with right and left justification\r\n");
    TPtrC string1,string2;
    TBuf8<100> buf1;
    TBuf8<100> buf2;

    _LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1 );
   
    buf1.Copy(string1);
    buf2.Copy(buf1);
    x = (char*)buf2.Ptr();
    x[buf2.Length()]='\0';
    buf1.Delete(0,100);
    
    _LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2 );
    buf1.Copy(string2);
    y = (char*)buf1.Ptr();
    y[buf1.Length()]='\0';
    printf("For normal input:String length>precision,Api Output is:\r\n");
    printf("Original inputs(concatenated) and actual output are given below:\r\n");
    printf("%s%s\r\n",x,y);
    ret=printf(fmt,x,y);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest18
//API Tested :printf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest18L(  )
    {
    
	int ret;
	float x;
	char ch1='\0';
	char ch2='\0';
	char* fmt="%-10.3f";
	printf("TD ID = 1_10_2_1704\r\n");
	printf("printf Field width with left justification test\r\n");
    TPtrC string1,string2;
    TBuf8<100> buf1;
	
	
	_LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1);
    buf1.Copy(string1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For normal input,Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	
	buf1.Delete(0,100);
	_LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2 );
    buf1.Copy(string2);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For Boundary input:Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :printfTest19
//API Tested :printf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest19L(  )
    {
    
	int ret;
	float x;
	char ch1='\0';
	char ch2='\0';
	char* fmt="%10.3f";
	printf("TD ID = 1_10_2_1705\r\n");
	printf("printf Field width with right justification test\r\n");
    TPtrC string1,string2;
    TBuf8<100> buf1;
        
    _LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1 );
    buf1.Copy(string1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For normal input,Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	
	buf1.Delete(0,100);
	
	_LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2 );
	
    buf1.Copy(string2);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For Boundary input:Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :printfTest20
//API Tested :printf
  
  
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest20L(  )
    {
    
   	int ret;
	char* x;
	char ch2='\0';
	char* fmt="%-10.3s";
	printf("TD ID = 1_10_2_1706\r\n");
	printf("printf Field width with justification test for string\r\n");
    TPtrC string;
    TBuf8<100> buf1;
    
    _LIT( Kstring, "string" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring, string);
    buf1.Copy(string);
    x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    printf("Input and acutal output are shown below:\r\n");
    printf("%s\r\n",x);
    ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);
	
	printf("Press 1 to PASS or 0 to FAIL this case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
    if(ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    }

// -----------------------------------------------------------------------------
//Function Name :printfTest21
//API Tested :printf
  
  
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest21L(  )
    {
    
	int x,ret;
	char ch2='\0';
	printf("TD ID = 1_10_2_1707\r\n");
	printf("Testing printf for zero-padding\r\n");
	
	_LIT( Kx, "x" );
	TBool res = GetIntFromConfig(ConfigSection(), Kx, x);

    printf("Input and actual output are shown below:\r\n");
    printf("%d\r\n",x);
    ret=printf("%06d",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);
	
	printf("Press 1 to PASS or 0 to FAIL this case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
    if(ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :printfTest22
//API Tested :printf
  
  
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest22L(  )
    {
    
	int ret;
	float x;
	char ch1='\0';
	char ch2='\0';
	char* fmt="%+f";
	printf("TD ID = 1_10_2_1708\r\n");
	printf("Testing printf for float with positive sign appended\r\n");
    TPtrC string1,string2;
    TBuf8<100> buf1;
    
    
    _LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1);
	buf1.Copy(string1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For normal input(no sign),Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
	    }while(ch1!='1' && ch1!='0');
	
	buf1.Delete(0,100);
	
	_LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2);
	buf1.Copy(string2);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For Abnormal input(with - sign):Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :printfTest23
//API Tested :printf
  
  
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest23L(  )
    {
    
	int ret;
	float x;
	char ch1='\0';
	char* fmt="%    f";
	printf("TD ID = 1_10_2_1710\r\n");
	printf("Testing printf for spaces in format string\r\n");
    TPtrC string;
    TBuf8<100> buf1;
    
    _LIT( Kstring, "string" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring, string);
    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("Given are the original and actual outputs:\r\n");
    x=atof(input);
    printf("%f\r\n",x);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest24
//API Tested :printf
  
  
//TestCase Description:To test whether for x conversions(eg. %#x), the first character
//					   of the output  string  is  made zero (by prefixing a 0 if it was
//					   not zero already)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest24L(  )
    {
    
	int x,ret;
	char ch1='\0';
	printf("TD ID = 1_10_2_1713\r\n");
	printf("Testing printf for hexadecimal conversion with x appended\r\n");
   	
	_LIT( Kx, "x" );
	TBool res = GetIntFromConfig(ConfigSection(), Kx, x);

    printf("For normal input,Api Output is:\r\n");
    ret=printf("%#x",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest25
//API Tested :printf
  
  
//TestCase Description:To test whether for o conversions(eg. %#o), the first character
//					   of the output  string  is  made zero (by prefixing a 0 if it was
//					   not zero already)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest25L(  )
    {
    
	int x,ret;
	char ch1='\0';
	printf("TD ID = 1_10_2_1715\r\n");
	printf("Testing printf for octal conversion with 0 appended\r\n");
	
	_LIT( Kx, "x" );
	TBool res = GetIntFromConfig(ConfigSection(), Kx, x);
    printf("For normal input,Api Output is:\r\n");
    ret=printf("%#o",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :printfTest26
//API Tested :printf,fprintf,sprintf,snprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest26L(  )
    {
    
	int ret;
	float x;
	char ch1='\0';
	char ch2='\0';
	char* fmt="%-010.4f";
	printf("TD ID = 1_10_2_1719\r\n");
	printf("Testing printf for float field width and padding\r\n");
    TPtrC string1,string2;
    TBuf8<100> buf1;
    
    _LIT( Kstring1, "string1" );
	TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1 );

    buf1.Copy(string1);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For normal input(no sign),Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch1=getchar();
		}while(ch1!='1' && ch1!='0');
	
	buf1.Delete(0,100);
	
	_LIT( Kstring2, "string2" );
	res = GetStringFromConfig(ConfigSection(), Kstring2, string2 );

	
    buf1.Copy(string2);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    printf("For Abnormal input(with - sign):Api Output is:\r\n");
    x=atof(input);
	ret=printf(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

	printf("Press 1 to PASS or 0 to FAIL this sub-case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
    if(ch1=='1' && ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }

// -----------------------------------------------------------------------------
//Function Name :printfTest27
//API Tested :printf,fprintf,sprintf,snprintf
  
  
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros  
// -----------------------------------------------------------------------------
  
TInt CTestStdio::printfTest27L(  )
    {
    
	int x,ret;
	char ch1='\0';
	printf("TD ID = 1_10_2_1720\r\n");
	printf("Testing printf for precsion with decimal type\r\n");

	_LIT( Kx, "x" );
	TBool res = GetIntFromConfig(ConfigSection(), Kx, x);
    
    printf("Original input and actual output are shown below\r\n");
    printf("%d\r\n",x);
    ret=printf("%.6d",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
    
    

//****************************************fprintf tests****************************

// -----------------------------------------------------------------------------
//Function Name :fprintfTest1
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------

TInt CTestStdio::fprintfTest1L(  )
    {
    
	int x;
	int ret;
	char* testfile="c:\\printfTest1.txt";

	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);

	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%d",x);
	fclose(fp);
	GetIntFromConfig(ConfigSection(),_L("parameter2"), x);	

	fp=fopen(testfile,"w");
	ret=fprintf(fp,"%d",x);
	fclose(fp);
	
	unlink(testfile);
 
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
//Function Name :fprintfTest2
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest2L(  )
    {
    
	float x;
	int ret;
	char* testfile="c:\\fprintfTest2.txt";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%f",x);
	fclose(fp);

	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	fp=fopen(testfile,"w");
	ret=fprintf(fp,"%f",x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest3
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for exponential type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest3L(  )
    {
    
	float x;
	int ret;
	char* testfile="c:\\fprintfTest3.txt";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%e",x);
	fclose(fp);

	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	fp=fopen(testfile,"w");
	ret=fprintf(fp,"%e",x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest4
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for character type(%c) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest4L(  )
    {
    
	int ret;
	char* testfile="c:\\fprintfTest4.txt";
	
	TPtrC x;
	
	_LIT( Kx, "x" );
	TBool res = GetStringFromConfig(ConfigSection(), Kx, x);
	
	
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%c",(char)x[0]);
	fclose(fp);
	unlink(testfile);
 
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
//Function Name :fprintfTest5
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for string type(%s) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest5L(  )
    {
    
	int ret;
	char* testfile="c:\\fprintfTest5.txt";
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	
    buf1.Copy(string);
    char* x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
	
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%s",x);
	fclose(fp);
	unlink(testfile);
 
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
//Function Name :fprintfTest6
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest6L(  )
    {
    
	int x;
	int ret;
	char* testfile="c:\\fprintfTest6.txt";

GetIntFromConfig(ConfigSection(),_L("parameter1"), x);

	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%o",x);
	fclose(fp);
	
	GetIntFromConfig(ConfigSection(),_L("parameter2"), x);

	fp=fopen(testfile,"w");
	ret=fprintf(fp,"%o",x);
	fclose(fp);
	
GetIntFromConfig(ConfigSection(),_L("parameter3"), x);	

	fp=fopen(testfile,"w");
	ret=fprintf(fp,"%o",x);
	fclose(fp);
	
	unlink(testfile);
 
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
//Function Name :fprintfTest7
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for hexadecimal type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest7L(  )
    {
    
	int ret;
	char* testfile="c:\\fprintfTest7.txt";
	TPtrC string;
    TBuf8<100> buf1;
	
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%x",(int)x);
	fclose(fp);
	
	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    fp=fopen(testfile,"w");
	ret=fprintf(fp,"%x",(int)x);
	fclose(fp);
	
	buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	

    buf1.Copy(string);
    x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    fp=fopen(testfile,"w");
	ret=fprintf(fp,"%x",(int)x);
	fclose(fp);
	
	unlink(testfile);
 
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
//Function Name :fprintfTest8
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for combination of data types
//					   to get the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest8L(  )
    {
    
	int x;
	float y;
	char *z;
	int ret;
	char* testfile="c:\\fprintfTest8.txt";
	TPtrC string;
    TBuf8<100> buf1;
	
	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);	

	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    char* input1 = (char*) buf1.Ptr();
    input1[buf1.Length()]='\0';
    y=atof(input1);
    GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	

    buf1.Copy(string);
    z = (char*) buf1.Ptr();
    z[buf1.Length()]='\0';
    	
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%s %f %d",z,y,x);
	fclose(fp);
	unlink(testfile);
 
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
//Function Name :fprintfTest9
//API Tested :fprintf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest9L(  )
    {
    
	int x;
	int *y=&x;
	int ret;
	char* testfile="c:\\fprintfTest9.txt";
	char* fmt="%d%n%d";
	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);	

	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x,y);
	fclose(fp);
	unlink(testfile);
 
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
//Function Name :fprintfTest10
//API Tested :fprintf
  
  
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest10L(  )
    {
    
	int x;
	int ret;
	char* testfile="c:\\fprintfTest10.txt";

	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);	

	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%%%d%%",x);
	fclose(fp);
	unlink(testfile);
 
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
//Function Name :fprintfTest11
//API Tested :fprintf
  
  
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest11L(  )
    {
    
	int x,y;
	int ret;
	char* testfile="c:\\fprintfTest11.txt";

	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);

	GetIntFromConfig(ConfigSection(),_L("parameter2"), y);

	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%*d",x,y);
	fclose(fp);
	unlink(testfile);
 
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
//Function Name :fprintfTest12
//API Tested :fprintf
  
  
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest12L(  )
    {
    
	float x;
	int ret;
	char* testfile="c:\\fprintfTest12.txt";
	char* fmt="%.f";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

    buf1.Delete(0,100);
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest13
//API Tested :fprintf
  
  
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest13L(  )
    {
    
	float x;
	int ret;
	char* testfile="c:\\fprintfTest13.txt";
	char* fmt="%.3f";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

    buf1.Delete(0,100);
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest14
//API Tested :fprintf
  
  
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest14L(  )
    {
    
	int ret;
	char* testfile="c:\\fprintfTest14.txt";
	char* fmt="%.3s";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,input);
	fclose(fp);

    buf1.Delete(0,100);
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,input);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest15
//API Tested :fprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest15L(  )
    {
    
	int ret;
	char* testfile="c:\\fprintfTest15.txt";
	char* fmt="%-12s";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,input);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest16
//API Tested :fprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %10ls type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest16L(  )
    {
    
	int ret;
	char* testfile="c:\\fprintfTest16.txt";
	char* fmt="%12s";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,input);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest17
//API Tested :fprintf
  
  
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest17L(  )
    {
    
   	int ret;
	char* testfile="c:\\fprintfTest17.txt";
	char* fmt="%-10s%3s";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Delete(0,100);
    buf1.Copy(string);
    char* input1 = (char*) buf1.Ptr();
    input1[buf1.Length()]='\0';    
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,input,input1);
	fclose(fp);
	
	unlink(testfile);
 
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
//Function Name :fprintfTest18
//API Tested :fprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest18L(  )
    {
    
	float x;
	int ret;
	char* testfile="c:\\fprintfTest18.txt";
	char* fmt="%-10.3f";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

    buf1.Delete(0,100);
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest19
//API Tested :fprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest19L(  )
    {
    
	float x;
	int ret;
	char* testfile="c:\\fprintfTest19.txt";
	char* fmt="%10.3f";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

    buf1.Delete(0,100);
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest20
//API Tested :fprintf
  
  
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest20L(  )
    {
    
   	int ret;
	char* testfile="c:\\fprintfTest20.txt";
	char* fmt="%-10.3s";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
   	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,input);
	fclose(fp);
	
	unlink(testfile);
 
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
//Function Name :fprintfTest21
//API Tested :fprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest21L(  )
    {
    
	int x;
	int ret;
	char* testfile="c:\\fprintfTest21.txt";
	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);	

	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%06d",x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest22
//API Tested :fprintf
  
  
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest22L(  )
    {
    
	float x;
	int ret;
	char* testfile="c:\\fprintfTest22.txt";
	char* fmt="%+f";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

    buf1.Delete(0,100);
    GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest23
//API Tested :fprintf
  
  
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest23L(  )
    {
    
	float x;
	int ret;
	char* testfile="c:\\fprintfTest23.txt";
	char* fmt="%      f";
	TPtrC string;
    TBuf8<100> buf1;

	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest24
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for hexadecimal type(%#x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest24L(  )
    {
    
	int x;
	int ret;
	char* testfile="c:\\fprintfTest24.txt";
	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);

	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%#x",x);
	fclose(fp);
	
	unlink(testfile);
 
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
//Function Name :fprintfTest25
//API Tested :fprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%#o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest25L(  )
    {
    
	int x;
	int ret;
	char* testfile="c:\\fprintfTest25.txt";
	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);	

	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%#o",x);
	fclose(fp);
	
	unlink(testfile);
 
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
//Function Name :fprintfTest26
//API Tested :fprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest26L(  )
    {
    
	float x;
	int ret;
	char* testfile="c:\\fprintfTest26.txt";
	char* fmt="%-10.4f";
	TPtrC string;
    TBuf8<100> buf1;
    
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

    buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    
    buf1.Copy(string);
    input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
	x=atof(input);
	fp=fopen(testfile,"w");
	ret=fprintf(fp,fmt,x);
	fclose(fp);

	unlink(testfile);
 
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
//Function Name :fprintfTest27
//API Tested :fprintf
  
  
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros  
// -----------------------------------------------------------------------------
  
TInt CTestStdio::fprintfTest27L(  )
    {
    
	int x;
	int ret;
	char* testfile="c:\\fprintfTest27.txt";
	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);

	FILE *fp=fopen(testfile,"w");
	ret=fprintf(fp,"%.6d",x);
	fclose(fp);

	unlink(testfile);
 
    if(ret<=0)
    	{
    	return KErrGeneral;
		}
    else
		{
        return KErrNone;
		}
    
    }

//*******************************sprintf Tests**************************************

// -----------------------------------------------------------------------------
//Function Name :sprintfTest1
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest1L(  )
    {
    
	int x=345;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%d",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );

    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest2
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------

TInt CTestStdio::sprintfTest2L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest3
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for exponential type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  TInt CTestStdio::sprintfTest3L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%e",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );

    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest4
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for character type(%c) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest4L(  )
    {
    
	char x='z';
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%c",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest5
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for string type(%s) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest5L(  )
    {
    
	char *x="Abcdef";
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest6
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest6L(  )
    {
    
	int x=34;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%o",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest7
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for  hexadecimal type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest7L(  )
    {
    
	int x=34;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%x",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest8
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for combination of data types
//					   to get the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest8L(  )
    {
    
	int x=34;
	float y=(float)67.345;
	char *z="Wxyz";
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%s %f %d",z,y,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest9
//API Tested :sprintf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest9L(  )
    {
    
	int x=345;
	int *y=&x;
	int ret;
	char str[BUF_SIZE];
	char* fmt="%d%n%d";

	ret=sprintf(str,fmt,x,y);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest10
//API Tested :sprintf
  
  
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest10L(  )
    {
    
	int x=345;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%%%d%%",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest11
//API Tested :sprintf
  
  
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest11L(  )
    {
    
	int x=345;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%*d",10,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest12
//API Tested :sprintf
  
  
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest12L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];
	char* fmt="%.f";

	ret=sprintf(str,fmt,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest13
//API Tested :sprintf
  
  
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest13L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%.3f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest14
//API Tested :sprintf
  
  
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest14L(  )
    {
    
	char *x="abcdefghi";
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%.3s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest15
//API Tested :sprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest15L(  )
    {
    
	char *x="abcdef";
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%-12s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest16
//API Tested :sprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %10s type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest16L(  )
    {
    
	char *x="abcdef";
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%12s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest17
//API Tested :sprintf
  
  
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest17L(  )
    {
    
	int x=10,y=20;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%-10d%3d",x,y);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest18
//API Tested :sprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest18L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%-10.3f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest19
//API Tested :sprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest19L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%10.3f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest20
//API Tested :sprintf
  
  
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest20L(  )
    {
    
	char *x="abcdefg";
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%-10.3s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest21
//API Tested :sprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest21L(  )
    {
    
	int x=345;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%06d",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest22
//API Tested :sprintf
  
  
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest22L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%+f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest23
//API Tested :sprintf
  
  
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest23L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];
	char* fmt="%+     f";
		
	ret=sprintf(str,fmt,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest24
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for  hexadecimal type(%#x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest24L(  )
    {
    
	int x=34;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%#x",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest25
//API Tested :sprintf
  
  
//TestCase Description:To test whether the conversion for  octal type(%#o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest25L(  )
    {
    
	int x=34;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%#o",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );

    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest26
//API Tested :sprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest26L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];
	char* fmt="%-010.4f";

	ret=sprintf(str,fmt,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
//Function Name :sprintfTest27
//API Tested :sprintf
  
  
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros  
// -----------------------------------------------------------------------------
  
TInt CTestStdio::sprintfTest27L(  )
    {
    
	int x=90;
	int ret;
	char str[BUF_SIZE];

	ret=sprintf(str,"%.6d",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    return KErrNone;
    
    }


// -----------------------------------------------------------------------------
//Function Name :sprintfTest28
//API Tested :sprintf
  
  
//TestCase Description:To test whether precision for double type eg.%.g%f%e fills up the minimum
//					   number of digits required in the target string with roundoff  
// -----------------------------------------------------------------------------
TInt CTestStdio::sprintfTest28L(  )
    {
    
	int ret;
        TInt len;
        char str[BUF_SIZE];
    
        TPtrC string;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

	TBuf8<200> buf1,buf2,buf3;
	buf1.Copy(string);
	char* format = (char*) buf1.Ptr();
	len=buf1.Length();
	format[len]='\0';

	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

	buf2.Copy(string);
	char * val=(char *)buf2.Ptr();
	len=buf2.Length();
	val[len]='\0';


	GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	

	buf3.Copy(string);
	char * res=(char *)buf3.Ptr();
	len=buf3.Length();
	res[len]='\0';
	MakeSpace(res); 
	
	char arg1[100] = {'%','\0'};
    
        strcat(arg1,format);
	    double a_val=atof(val);

	    	
    	ret=sprintf(str,arg1,a_val);
    	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
    	if(ret<0)
    	return KErrGeneral;
        else
        {
        	if(strcmp(str,res)!=0)
        	{
        		return KErrGeneral;
        	}
        	else
        	   return KErrNone;
        }
        

   
    
    }


// -----------------------------------------------------------------------------
//Function Name :sprintfTest29
//API Tested :sprintf
  
  
//TestCase Description:To test whether precision for floattype eg.%.f fills up the minimum
//					   number of digits required in the target string 
// -----------------------------------------------------------------------------

  
TInt CTestStdio::sprintfTest29L(  )
    {
    
	int ret;
        TInt len;
        char str[BUF_SIZE];
    
        TPtrC string;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

	TBuf8<200> buf1,buf2,buf3; 
	buf1.Copy(string);
	char* format = (char*) buf1.Ptr();
	len=buf1.Length();
	format[len]='\0';

	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

	buf2.Copy(string);
	char * val=(char *)buf2.Ptr();
	len=buf2.Length();
	val[len]='\0';


	GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	

	buf3.Copy(string);
	char * res=(char *)buf3.Ptr();
	len=buf3.Length();
	res[len]='\0';
	char arg1[100] = {'%','\0'};
    
        strcat(arg1,format);
	    float a_val=atof(val);

	    	
    	ret=sprintf(str,arg1,a_val);
    	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
    	if(ret<0)
    	return KErrGeneral;
        else
        {
        	if(strcmp(str,res)!=0)
        	{
        		return KErrGeneral;
        	}
        	else
        	   return KErrNone;
        }
        

    }
    
    
    
/**
*	Testcase		: 	sprintfTestInfL
*	Description		:	1. Tests sprintf functionality for large exponential value
*						2. Tests sprintf functionality for a number > INT_MAX
*	Return values	: 	In both the above cases sprintf fills up the buffer 
*						with Inf, test case returns KErrNone
*/  
TInt CTestStdio::sprintfTestInfL()
    {
    
	int ret;
    TInt len;
    char str[BUF_SIZE];
    
    TPtrC string;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

	TBuf8<200> buf1,buf2,buf3; 
	buf1.Copy(string);
	char* format = (char*) buf1.Ptr();
	len=buf1.Length();
	format[len]='\0';

	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

	buf2.Copy(string);
	char * val=(char *)buf2.Ptr();
	len=buf2.Length();
	val[len]='\0';


	GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	

	buf3.Copy(string);
	char * res=(char *)buf3.Ptr();
	len=buf3.Length();
	res[len]='\0';
	char arg1[100] = {'%','\0'};
    
    strcat(arg1,format);
	float a_val=atof(val);

	    	
    ret=sprintf(str,arg1,a_val);
    INFO_PRINTF2(_L("sprintf returned:%d"),ret );
    if(ret<0)
    	return KErrGeneral;
    else
    	{
        if(strcasecmp(str,res)!=0)
        	{
        		return KErrGeneral;
        	}
        	else
        	   return KErrNone;
        }
    }
    
    
/**
*	Testcase		: 	sprintfTestNanL
*	Description		:	Tests sprintf functionality for a Nan
*	Return values	: 	sprintf fills up the buffer with Nan,
*						Test case returns KErrNOne
*/  
TInt CTestStdio::sprintfTestNanL()
    {
    int ret;
    TInt len;
    char str[BUF_SIZE];
    
    TPtrC string;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

	TBuf8<200> buf1,buf3; 
	buf1.Copy(string);
	char* format = (char*) buf1.Ptr();
	len=buf1.Length();
	format[len]='\0';

	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

	buf3.Copy(string);
	char * res=(char *)buf3.Ptr();
	len=buf3.Length();
	res[len]='\0';
	
	char arg1[100] = {'%','\0'};
    
    strcat(arg1,format);
    double zero=0.0;
	double a_val=zero/zero;		// a_val is a 'Nan'
    		
    ret=sprintf(str,arg1,a_val);
    INFO_PRINTF2(_L("sprintf returned:%d"),ret );
    if(ret<0)
    	return KErrGeneral;
    else
    	{
        if(strcasecmp(str,res)!=0)
        	{
        		return KErrGeneral;
        	}
        	else
        	   return KErrNone;
        }
    }
//************************************snprintf Tests*********************************

// -----------------------------------------------------------------------------
//Function Name :snprintfTest1
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest1L(  )
    {
    
	int x=345;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%d",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest2
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------

TInt CTestStdio::snprintfTest2L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest3
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for exponential type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  TInt CTestStdio::snprintfTest3L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%e",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest4
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for character type(%c) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest4L(  )
    {
    
	char x='z';
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%c",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest5
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for string type(%s) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest5L(  )
    {
    
	char *x="Abcdef";
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest6
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest6L(  )
    {
    
	int x=34;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%o",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest7
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for  hexadecimal type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest7L(  )
    {
    
	int x=34;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%x",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest8
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for combination of data types
//					   to get the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest8L(  )
    {
    
	int x=34;
	float y=(float)67.345;
	char *z="Wxyz";
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%s %f %d",z,y,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest9
//API Tested :snprintf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest9L(  )
    {
    
	int x=345;
	int *y=&x;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%d%n%d",x,y);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );

 
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
//Function Name :snprintfTest10
//API Tested :snprintf
  
  
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest10L(  )
    {
    
	int x=345;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%%%d%%",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest11
//API Tested :snprintf
  
  
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest11L(  )
    {
    
	int x=345;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%*d",10,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
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
//Function Name :snprintfTest12
//API Tested :snprintf
  
  
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest12L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];
	char* fmt="%.f";

	ret=snprintf(str,sizeof(str),fmt,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest13
//API Tested :snprintf
  
  
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest13L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%.3f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest14
//API Tested :snprintf
  
  
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest14L(  )
    {
    
	char *x="abcdefghi";
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%.3s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest15
//API Tested :snprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest15L(  )
    {
    
	char *x="abcdef";
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%-12s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest16
//API Tested :snprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %10s type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest16L(  )
    {
    
	char *x="abcdef";
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%12s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest17
//API Tested :snprintf
  
  
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest17L(  )
    {
    
	int x=10,y=20;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%-10d%3d",x,y);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest18
//API Tested :snprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest18L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%-10.3f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest19
//API Tested :snprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest19L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%10.3f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest20
//API Tested :snprintf
  
  
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest20L(  )
    {
    
	char *x="abcdefg";
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%-10.3s",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest21
//API Tested :snprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest21L(  )
    {
    
	int x=345;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%06d",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest22
//API Tested :snprintf
  
  
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest22L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%+f",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest23
//API Tested :snprintf
  
  
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest23L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];
	char* fmt="%+     f";

	ret=snprintf(str,sizeof(str),fmt,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
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
//Function Name :snprintfTest24
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for  hexadecimal type(%#x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest24L(  )
    {
    
	int x=34;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%#x",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest25
//API Tested :snprintf
  
  
//TestCase Description:To test whether the conversion for  octal type(%#o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest25L(  )
    {
    
	int x=34;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%#o",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest26
//API Tested :snprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest26L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char str[BUF_SIZE];
	char* fmt="%-010.4f";

	ret=snprintf(str,sizeof(str),fmt,x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
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
//Function Name :snprintfTest27
//API Tested :snprintf
  
  
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros  
// -----------------------------------------------------------------------------
  
TInt CTestStdio::snprintfTest27L(  )
    {
    
	int x=90;
	int ret;
	char str[BUF_SIZE];

	ret=snprintf(str,sizeof(str),"%.6d",x);

	INFO_PRINTF2(_L("sprintf returned:%d"),ret );
 
    if(ret<=0)
    	{
    	return KErrGeneral;
		}
    else
		{
        return KErrNone;
		}
    
    }

//************************************Locals*****************************************

TInt CTestStdio::vprintfTests(const char* fmt,...)
	{
	int ret;
	va_list ap;
	va_start(ap,fmt);

	ret=vprintf(fmt,ap);
 
	INFO_PRINTF2(_L("vprintf returned:%d"),ret );

	va_end(ap);
	return ret;
	}

TInt CTestStdio::vfprintfTests( char* filename,const char* fmt,...)
	{
	int ret;
	va_list ap;
	va_start(ap,fmt);

	FILE* fp = fopen(filename,"w");
	ret=vfprintf(fp,fmt,ap);

	INFO_PRINTF2(_L("vprintf returned:%d"),ret );
	
	fclose(fp);

	va_end(ap);
	return ret;
	}


TInt CTestStdio::vsprintfTests( const char* fmt,...)
	{
	int ret;
	char str[BUF_SIZE];
	va_list ap;
	va_start(ap,fmt);

	ret=vsprintf(str,fmt,ap);

	INFO_PRINTF2(_L("vsprintf returned:%d"),ret );
	
	va_end(ap);
	return ret;
	}

TInt CTestStdio::vsnprintfTests( const char* fmt,...)
	{
	int ret;
	char str[BUF_SIZE];
	va_list ap;
	va_start(ap,fmt);

	ret=vsnprintf(str,sizeof(str),fmt,ap);

	INFO_PRINTF2(_L("vsprintf returned:%d"),ret );
	
	va_end(ap);
	return ret;
	}


//************************************vprintf Tests**********************************

// -----------------------------------------------------------------------------
//Function Name :vprintfTest1
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest1L(  )
    {
    
	int x=345;
	
	int ret=vprintfTests("%d",x);

 
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
//Function Name :vprintfTest2
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------

TInt CTestStdio::vprintfTest2L(  )
    {
    
	float x=(float)34.4567;
	
	int ret=vprintfTests("%f",x);

 
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
//Function Name :vprintfTest3
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for exponential type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest3L(  )
    {
    
	float x=(float)34.4567;

	int ret=vprintfTests("%e",x);

 
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
//Function Name :vprintfTest4
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for character type(%c) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest4L(  )
    {
    
	char x='z';
	
	int ret=vprintfTests("%c",x);

 
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
//Function Name :vprintfTest5
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for string type(%s) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest5L(  )
    {
    
	char *x="Abcdef";
	
	int ret=vprintfTests("%s",x);

 
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
//Function Name :vprintfTest6
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest6L(  )
    {
    
	int x=34;
	
	int ret=vprintfTests("%o",x);

 
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
//Function Name :vprintfTest7
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for hexadecimal type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest7L(  )
    {
    
	int x=34;
	
	int ret=vprintfTests("%x",x);

 
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
//Function Name :vprintfTest8
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for combination of data types
//					   to get the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest8L(  )
    {
    
	int ret,x;
	float y;
	char* z;	
	char ch1='\0';
	printf("TD ID = 1_10_2_1827\r\n");
	printf("Testing printf for combination of types\r\n");
    TPtrC string;
    TBuf8<100> buf1;

	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);

	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

    buf1.Copy(string);
    char* input = (char*) buf1.Ptr();
    input[buf1.Length()]='\0';
    y=atof(input);
    buf1.Delete(0,100);
    GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	
 
	buf1.Copy(string);
    z = (char*) buf1.Ptr();
    z[buf1.Length()]='\0';
	ret=printf("For normal input,Api Output is:\r\n");
	
	ret=vprintfTests("%s %f %d",z,y,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :vprintfTest9
//API Tested :vprintf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest9L(  )
    {
    
	int x=345;
	int *y=&x;
	
	int ret=vprintfTests("%d%n%d",x,y);

 
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
//Function Name :vprintfTest10
//API Tested :vprintf
  
  
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest10L(  )
    {
    
	int x=345;
	
	int ret=vprintfTests("%%%d%%",x);

 
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
//Function Name :vprintfTest11
//API Tested :vprintf
  
  
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest11L(  )
    {
    
	int x=345;
	
	int ret=vprintfTests("%*d",10,x);

 
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
//Function Name :vprintfTest12
//API Tested :vprintf
  
  
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest12L(  )
    {
    
	float x=(float)34.4567;
	char* fmt="%.f";

	int ret=vprintfTests(fmt,x);

 
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
//Function Name :vprintfTest13
//API Tested :vprintf
  
  
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest13L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vprintfTests("%.3f",x);
 
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
//Function Name :vprintfTest14
//API Tested :vprintf
  
  
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest14L(  )
    {
    
	char *x="abcdefghi";
	int ret;

	ret=vprintfTests("%.3s",x);
 
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
//Function Name :vprintfTest15
//API Tested :vprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest15L(  )
    {
    
	int ret;
	char* x;
	char ch1='\0';
	char* fmt="%-12s";
	printf("TD ID = 1_10_2_1821\r\n");
	printf("Testing printf for field width with left justification\r\n");
    TPtrC string;
    TBuf8<100> buf1;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    printf("Given are the original and actual outputs:\r\n");
    printf("%s\r\n",x);
    ret=vprintfTests(fmt,x);
    printf("|");
    printf("\r\n The pipe char above signifies the end of conversion");
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :vprintfTest16
//API Tested :vprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %10s type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest16L(  )
    {
    
	int ret;
	char* x;
	char ch1='\0';
	char* fmt="%12s";
	printf("TD ID = 1_10_2_1811\r\n");
	printf("Testing printf for field width with right justification\r\n");
    TPtrC string;
    TBuf8<100> buf1;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    x = (char*) buf1.Ptr();
    x[buf1.Length()]='\0';
    printf("Given are the original and actual outputs:\r\n");
    printf("%s\r\n",x);
    ret=vprintfTests(fmt,x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :vprintfTest17
//API Tested :vprintf
  
  
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest17L(  )
    {
    
	int ret;
	char *x,*y;
	char ch1='\0';
    char* fmt="%-10s%3s";
    printf("TD ID = 1_10_2_1825\r\n");
	printf("Testing printf for field width with right and left justification\r\n");
    TPtrC string;
    TBuf8<100> buf1;
    TBuf8<100> buf2;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

    buf1.Copy(string);
    buf2.Copy(buf1);
    x = (char*)buf2.Ptr();
    x[buf2.Length()]='\0';
    buf1.Delete(0,100);
	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	    

    buf1.Copy(string);
    y = (char*)buf1.Ptr();
    y[buf1.Length()]='\0';
    printf("For normal input:String length>precision,Api Output is:\r\n");
    printf("Original inputs(concatenated) and actual output are given below:\r\n");
    printf("%s%s\r\n",x,y);
    ret=vprintfTests(fmt,x,y);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);

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
//Function Name :vprintfTest18
//API Tested :vprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest18L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vprintfTests("%-10.3f",x);
 
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
//Function Name :vprintfTest19
//API Tested :vprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest19L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vprintfTests("%10.3f",x);
 
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
//Function Name :vprintfTest20
//API Tested :vprintf
  
  
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest20L(  )
    {
    
	char *x="abcdefg";
	int ret;

	ret=vprintfTests("%-10.3s",x);
 
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
//Function Name :vprintfTest21
//API Tested :vprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest21L(  )
    {
    
	int x,ret;
	char ch2='\0';
	printf("TD ID = 1_10_2_1810\r\n");
	printf("Testing printf for zero-padding\r\n");
   
	GetIntFromConfig(ConfigSection(),_L("parameter1"), x);   

    printf("Input and actual output are shown below:\r\n");
    printf("%d\r\n",x);
    ret=printf("%06d",x);
    printf("\r\nApi return value must be equal to the number of chars printed\r\n");
    printf("Api Return value=%d\r\n",ret);
	
	printf("Press 1 to PASS or 0 to FAIL this case:\r\n");
	do
		{
    	ch2=getchar();
		}while(ch2!='1' && ch2!='0');
	
 
    if(ch2=='1')
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
    
    }


// -----------------------------------------------------------------------------
//Function Name :vprintfTest22
//API Tested :vprintf
  
  
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest22L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vprintfTests("%+f",x);
 
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
//Function Name :vprintfTest23
//API Tested :vprintf
  
  
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest23L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* fmt="%+     f";

	ret=vprintfTests(fmt,x);
 
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
//Function Name :vprintfTest24
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for hexadecimal type(%#x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest24L(  )
    {
    
	int x=34;
	
	int ret=vprintfTests("%#x",x);

 
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
//Function Name :vprintfTest25
//API Tested :vprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%#o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest25L(  )
    {
    
	int x=34;
	
	int ret=vprintfTests("%#o",x);

 
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
//Function Name :vprintfTest26
//API Tested :vprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest26L(  )
    {
    
	float x=(float)34.4567;
	char* fmt="%-010.4f";

	int ret=vprintfTests(fmt,x);

 
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
//Function Name :vprintfTest27
//API Tested :vprintf
  
  
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros  
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vprintfTest27L(  )
    {
    
	int x=90;
	

	int ret=vprintfTests("%.6d",x);
 
    if(ret<=0)
    	{
    	return KErrGeneral;
		}
    else
		{
        return KErrNone;
		}
    }

// utility function for parsing cfg parameter

void MakeSpace(char * item)
	{
	int i=0;
	while(item[i])
		{
		if(item[i]=='@') item[i]=' ';
		i++;
		}
	}




/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fprintfbasic
API Tested			: fprintf
TestCase Description: for testing %f %e %a and precision / width /sign /padding
-------------------------------------------------------------------------------
*/

TInt CTestStdio::fprintfbasic()
	{
	INFO_PRINTF1(_L("CTString::fprintfbasic"));

	TInt res=KErrGeneral;
	char ans_buf[100];
	TInt len=0;

	TPtrC string;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	

	TBuf8<200> buf1,buf2,buf3,buf4; 
	buf1.Copy(string);
	char* format = (char*) buf1.Ptr();
	len=buf1.Length();
	format[len]='\0';

	GetStringFromConfig(ConfigSection(),_L("parameter2"), string);	

	buf2.Copy(string);
	char * val=(char *)buf2.Ptr();
	len=buf2.Length();
	val[len]='\0';

	GetStringFromConfig(ConfigSection(),_L("parameter3"), string);	

	buf3.Copy(string);
	char * exp_val=(char *)buf3.Ptr();
	len=buf3.Length();
	exp_val[len]='\0';

	GetStringFromConfig(ConfigSection(),_L("parameter4"), string);	

	buf4.Copy(string);
	char * exp_res=(char *)buf4.Ptr();
	len=buf4.Length();
	exp_res[len]='\0';
	MakeSpace(exp_res); 

	char arg1[100] = {'%','\0'};


	strcat(arg1,format);
	double a_val=atof(val);
	atoi(exp_val);

	char *name = "C:\\fprintf_log.txt";
	FILE * fp=fopen(name,"w+");


	if(fp == NULL)
		{
		return -1;
		}

	fprintf(fp,arg1,a_val);

	if(fseek(fp,0,SEEK_SET)==-1)
		{
		res=KErrGeneral;
		}
	else
	{
 		if(fgets(ans_buf,sizeof(ans_buf),fp)==NULL) 
			{
			res=KErrGeneral;
			}
 		if(!strcmp(ans_buf,exp_res))
			{
			res=KErrNone;
			}
	}

	fclose(fp);
	remove(name);


	return res;
	}

//************************************vsprintf Tests*********************************
// -----------------------------------------------------------------------------
//Function Name :vsprintfTest1
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest1L(  )
    {
    
	int x=345;


	int ret=vsprintfTests("%d",x);

 
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
//Function Name :printfTest2
//API Tested :printf,fprintf,sprintf,snprintf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------

TInt CTestStdio::vsprintfTest2L(  )
    {
	float x=(float)34.4567;
	int ret;

	ret=vsprintfTests("%f",x);

 
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
//Function Name :vsprintfTest3
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the conversion for exponential type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest3L(  )
    {
	float x=(float)34.4567;
	int ret;

	ret=vsprintfTests("%e",x);

 
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
//Function Name :vsprintfTest4
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the conversion for character type(%c) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest4L(  )
    {
    
	char x='z';
	int ret;
	
	ret=vsprintfTests("%c",x);

 
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
//Function Name :vsprintfTest5
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the conversion for string type(%s) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest5L(  )
    {
    
	char *x="Abcdef";
	int ret;
	ret=vsprintfTests("%s",x);

 
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
//Function Name :vsprintfTest6
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest6L(  )
    {
    
	int x=34;
	int ret;
	ret=vsprintfTests("%o",x);

 
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
//Function Name :vsprintfTest7
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the conversion for  hexadecimal  type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest7L(  )
    {
    
	int x=34;
	int ret;
	
	ret=vsprintfTests("%x",x);

 
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
//Function Name :vsprintfTest8
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the conversion for combination of data types
//					   to get the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest8L(  )
    {
    
	int x=34;
	float y=(float)67.345;
	char *z="Wxyz";
	int ret;

	ret=vsprintfTests("%s %f %d",z,y,x);

 
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
//Function Name :vsprintfTest9
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest9L(  )
    {
    
	int x=345;
	int *y=&x;
	int ret; 

	ret=vsprintfTests("%d%n%d",x,y);

 
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
//Function Name :vsprintfTest10
//API Tested :vsprintf
  
  
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest10L(  )
    {
    
	int x=345;
	int ret;
	
	ret=vsprintfTests("%%%d%%",x);

 
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
//Function Name :vsprintfTest11
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest11L(  )
    {
    
	int x=345;
	int ret;
	ret=vsprintfTests("%*d",10,x);

 
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
//Function Name :vsprintfTest12
//API Tested :vsprintf
  
  
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest12L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* fmt="%.f";

	ret=vsprintfTests(fmt,x);

 
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
//Function Name :vsprintfTest13
//API Tested :vsprintf
  
  
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest13L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	
	ret=vsprintfTests("%.3f",x);

 
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
//Function Name :vsprintfTest14
//API Tested :vsprintf
  
  
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest14L(  )
    {
    
	char *x="abcdefghi";
	int ret;

	ret=vsprintfTests("%.3s",x);

 
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
//Function Name :vsprintfTest15
//API Tested :vsprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest15L(  )
    {
    
	char *x="abcdef";
	int ret;
	
	ret=vsprintfTests("%-12s",x);

 
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
//Function Name :vsprintfTest16
//API Tested :vsprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %10s type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest16L(  )
    {
    
	char *x="abcdef";
	int ret;
	
	ret=vsprintfTests("%12s",x);

 
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
//Function Name :vsprintfTest17
//API Tested :vsprintf
  
  
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest17L(  )
    {
    
	int x=10,y=20;
	int ret;

	ret=vsprintfTests("%-10d%3d",x,y);

 
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
//Function Name :vsprintfTest18
//API Tested :vsprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest18L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vsprintfTests("%-10.3f",x);

 
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
//Function Name :vsprintfTest19
//API Tested :vsprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest19L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vsprintfTests("%10.3f",x);

 
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
//Function Name :vsprintfTest20
//API Tested :vsprintf
  
  
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest20L(  )
    {
    
	char *x="abcdefg";
	int ret;

	ret=vsprintfTests("%-10.3s",x);

 
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
//Function Name :vsprintfTest21
//API Tested :vsprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest21L(  )
    {
    
	int x=345;
	int ret;

	ret=vsprintfTests("%06d",x);

 
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
//Function Name :vsprintfTest22
//API Tested :vsprintf
  
  
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest22L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vsprintfTests("%+f",x);

 
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
//Function Name :vsprintfTest23
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest23L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* fmt="%+     f";

	ret=vsprintfTests(fmt,x);

 
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
//Function Name :vsprintfTest24
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the conversion for  hexadecimal  type(%#x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest24L(  )
    {
    
	int x=34;
	int ret;
	
	ret=vsprintfTests("%#x",x);

 
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
//Function Name :vsprintfTest25
//API Tested :vsprintf
  
  
//TestCase Description:To test whether the conversion for  octal  type(%#o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest25L(  )
    {
    
	int x=34;
	int ret;
	
	ret=vsprintfTests("%#o",x);

 
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
//Function Name :vsprintfTest26
//API Tested :vsprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest26L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* fmt="%-010.4f";

	ret=vsprintfTests(fmt,x);

 
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
//Function Name :vsprintfTest27
//API Tested :vsprintf
  
  
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros  
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsprintfTest27L(  )
    {
    
	int x=90;
	int ret;

	ret=vsprintfTests("%.6d",x);

 
    if(ret<=0)
    	{
    	return KErrGeneral;
		}
    else
		{
        return KErrNone;
		}
    
    }


//************************************vsnprintf Tests********************************
// -----------------------------------------------------------------------------
//Function Name :vsnprintfTest1
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest1L(  )
    {
    
	int x=345;
	int ret;

	ret=vsnprintfTests("%d",x);

 
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
//Function Name :vsnprintfTest2
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------

TInt CTestStdio::vsnprintfTest2L(  )
    {
	float x=(float)34.4567;
	int ret;

	ret=vsnprintfTests("%f",x);

 
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
//Function Name :vsnprintfTest3
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for exponential type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest3L(  )
    {
	float x=(float)34.4567;
	int ret;

	ret=vsnprintfTests("%e",x);

 
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
//Function Name :vsnprintfTest4
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for character type(%c) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest4L(  )
    {
    
	char x='z';
	int ret;
	
	ret=vsnprintfTests("%c",x);

 
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
//Function Name :vsnprintfTest5
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for string type(%s) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest5L(  )
    {
    
	char *x="Abcdef";
	int ret;
	ret=vsnprintfTests("%s",x);

 
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
//Function Name :vsnprintfTest6
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest6L(  )
    {
    
	int x=34;
	int ret;
	ret=vsnprintfTests("%o",x);

 
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
//Function Name :vsnprintfTest7
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for  hexadecimal  type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest7L(  )
    {
    
	int x=34;
	int ret;
	
	ret=vsnprintfTests("%x",x);

 
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
//Function Name :vsnprintfTest8
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for combination of data types
//					   to get the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest8L(  )
    {
    
	int x=34;
	float y=(float)67.345;
	char *z="Wxyz";
	int ret;

	ret=vsnprintfTests("%s %f %d",z,y,x);

 
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
//Function Name :vsnprintfTest9
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest9L(  )
    {
    
	int x=345;
	int *y=&x;
	int ret; 

	ret=vsnprintfTests("%d%n%d",x,y);

 
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
//Function Name :vsnprintfTest10
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest10L(  )
    {
    
	int x=345;
	int ret;
	
	ret=vsnprintfTests("%%%d%%",x);

 
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
//Function Name :vsnprintfTest11
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest11L(  )
    {
    
	int x=345;
	int ret;
	ret=vsnprintfTests("%*d",10,x);

 
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
//Function Name :vsnprintfTest12
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest12L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* fmt="%.f";

	ret=vsnprintfTests(fmt,x);

 
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
//Function Name :vsnprintfTest13
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest13L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	
	ret=vsnprintfTests("%.3f",x);

 
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
//Function Name :vsnprintfTest14
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest14L(  )
    {
    
	char *x="abcdefghi";
	int ret;

	ret=vsnprintfTests("%.3s",x);

 
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
//Function Name :vsnprintfTest15
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest15L(  )
    {
    
	char *x="abcdef";
	int ret;
	
	ret=vsnprintfTests("%-12s",x);

 
    if(ret<=0)
    	{
    	return KErrGeneral;
		}
    else
		{
        return KErrNone;
		};
    
    }

// -----------------------------------------------------------------------------
//Function Name :vsnprintfTest16
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %10s type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest16L(  )
    {
    
	char *x="abcdef";
	int ret;
	
	ret=vsnprintfTests("%12s",x);

 
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
//Function Name :vsnprintfTest17
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest17L(  )
    {
    
	int x=10,y=20;
	int ret;

	ret=vsnprintfTests("%-10d%3d",x,y);

 
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
//Function Name :vsnprintfTest18
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest18L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vsnprintfTests("%-10.3f",x);

 
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
//Function Name :vsnprintfTest19
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest19L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vsnprintfTests("%10.3f",x);

 
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
//Function Name :vsnprintfTest20
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest20L(  )
    {
    
	char *x="abcdefg";
	int ret;

	ret=vsnprintfTests("%-10.3s",x);

 
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
//Function Name :vsnprintfTest21
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest21L(  )
    {
    
	int x=345;
	int ret;

	ret=vsnprintfTests("%06d",x);

 
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
//Function Name :vsnprintfTest22
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest22L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	ret=vsnprintfTests("%+f",x);

 
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
//Function Name :vsnprintfTest23
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest23L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* fmt="%+     f";

	ret=vsnprintfTests(fmt,x);

 
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
//Function Name :vsnprintfTest24
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for  hexadecimal  type(%#x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest24L(  )
    {
    
	int x=34;
	int ret;
	
	ret=vsnprintfTests("%#x",x);

 
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
//Function Name :vsnprintfTest25
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether the conversion for  octal  type(%#o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest25L(  )
    {
    
	int x=34;
	int ret;
	
	ret=vsnprintfTests("%#o",x);

 
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
//Function Name :vsnprintfTest26
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest26L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* fmt="%-010.4f";

	ret=vsnprintfTests(fmt,x);

 
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
//Function Name :vsnprintfTest27
//API Tested :vsnprintf
  
  
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros  
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vsnprintfTest27L(  )
    {
    
	int x=90;
	int ret;

	ret=vsnprintfTests("%.6d",x);

 
	if(ret<=0)
    	{
    	return KErrGeneral;
		}
    else
		{
        return KErrNone;
		}
    
    }

//************************************vfprintf Tests*********************************


// -----------------------------------------------------------------------------
//Function Name :vfprintfTest1
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for integer type(%d) gives
//					   the desired results
// -----------------------------------------------------------------------------

TInt CTestStdio::vfprintfTest1L(  )
    {
    
	int x=345;
	int ret;
	char* testfile="c:\\vfprintfTest1.txt";

	ret=vfprintfTests(testfile,"%d",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest2
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for float type(%f) gives
//					   the desired results
// -----------------------------------------------------------------------------
TInt CTestStdio::vfprintfTest2L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* testfile="c:\\vfprintfTest2.txt";

	ret=vfprintfTests(testfile,"%f",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest3
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for exponential type(%e) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest3L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* testfile="c:\\vfprintfTest3.txt";

	ret=vfprintfTests(testfile,"%e",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest4
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for character type(%c) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest4L(  )
    {
    
	char x='z';
	int ret;

	char* testfile="c:\\vfprintfTest4.txt";

	ret=vfprintfTests(testfile,"%c",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest5
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for string type(%s) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest5L(  )
    {
    	char *x="Abcdef";
	int ret;

	char* testfile="c:\\vfprintfTest5.txt";

	ret=vfprintfTests(testfile,"%s",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest6
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest6L(  )
    {
    
	int x=34;
	int ret;
	char* testfile="c:\\vfprintfTest6.txt";

	ret=vfprintfTests(testfile,"%o",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest7
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for hexadecimal type(%x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest7L(  )
    {
    
	int x=34;
	int ret;
	char* testfile="c:\\vfprintfTest7.txt";

	ret=vfprintfTests(testfile,"%x",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest8
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for combination of data types
//					   to get the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest8L(  )
    {
    
	int x=34;
	float y=(float)67.345;
	char *z="Wxyz";
	int ret;
	char* testfile="c:\\vfprintfTest8.txt";

	ret=vfprintfTests(testfile,"%s %f %d",z,y,x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest9
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the format specifier %n works as expected.Here
//					   nothing is expected as input; instead, the number of characters 
//					   consumed thus  far  from  the  input  is stored through the next 
//					   pointer,which must be a pointer to  int
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest9L(  )
    {
    
	int x=345;
	int *y=&x;
	int ret;
	char* testfile="c:\\vfprintfTest9.txt";

	ret=vfprintfTests(testfile,"%d%n%d",x,y);
	unlink(testfile);
 
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
//Function Name :vfprintfTest10
//API Tested :vfprintf
  
  
//TestCase Description:To test whether %% in format string is replaced by a single %
//					   in target string	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest10L(  )
    {
    
	int x=345;
	int ret;

	char* testfile="c:\\vfprintfTest10.txt";

	ret=vfprintfTests(testfile,"%%%d%%",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest11
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the * operator in the format string allows
//					   the target to pick up the next argument in the arg-list
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest11L(  )
    {
    
	int x=345;
	int ret;
	char* testfile="c:\\vfprintfTest11.txt";

	ret=vfprintfTests(testfile,"%*d",10,x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest12
//API Tested :vfprintf
  
  
//TestCase Description:To test whether for a float number as input string,just the 
//					   dot operator without any precision mentioned returns the 
//					   integer part(zero if absent)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest12L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* testfile="c:\\vfprintfTest12.txt";
	char* fmt="%.f";

	ret=vfprintfTests(testfile,fmt,x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest13
//API Tested :vfprintf
  
  
//TestCase Description:To test whether,for a float number as input string,the dot 
//					   operator with any precision mentioned returns the amount of
//					   digits mentioned in the precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest13L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* testfile="c:\\vfprintfTest13.txt";

	ret=vfprintfTests(testfile,"%.3f",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest14
//API Tested :vfprintf
  
  
//TestCase Description:To test whether for a string given as input,any precision 
//					   (%.3s)mentioned picks up as many charcters from it as the
//					   number mentioned as precision.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest14L(  )
    {
    
	char *x="abcdefghi";
	int ret;
	char* testfile="c:\\vfprintfTest14.txt";

	ret=vfprintfTests(testfile,"%.3s",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest15
//API Tested :vfprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %-10ls type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest15L(  )
    {
    
	char *x="abcdef";
	int ret;
	char* testfile="c:\\vfprintfTest15.txt";

	ret=vfprintfTests(testfile,"%-12s",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest16
//API Tested :vfprintf
  
  
//TestCase Description:To test whether left justification is provided for example 
//				       by %10s type of field width
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest16L(  )
    {
    
	char *x="abcdef";
	int ret;
	char* testfile="c:\\vfprintfTest16.txt";

	ret=vfprintfTests(testfile,"%12s",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest17
//API Tested :vfprintf
  
  
//TestCase Description:To test whether justification works even for several
//					   justification strings put together
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest17L(  )
    {
    
	int x=10,y=20;
	int ret;
	char* testfile="c:\\vfprintfTest17.txt";

	ret=vfprintfTests(testfile,"%-10d%3d",x,y);
	unlink(testfile);
 
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
//Function Name :vfprintfTest18
//API Tested :vfprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%-10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest18L(  )
    {
    
	float x=(float)34.4567;
	int ret;

	char* testfile="c:\\vfprintfTest18.txt";

	ret=vfprintfTests(testfile,"%-10.3f",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest19
//API Tested :vfprintf
  
  
//TestCase Description:To test whether for a format string having field width with 
//					   precision(%10.4f),the precision determines the number of 
//					   digits to be copied after the decimal point in the float 
//					   number and the field width gives the alignment of the target string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest19L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* testfile="c:\\vfprintfTest19.txt";

	ret=vfprintfTests(testfile,"%10.3f",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest20
//API Tested :vfprintf
  
  
//TestCase Description:To test whether for a given input string,the target string 
//					   must be generated such that the no. of chars in it equals 
//					   the precision value and the justification is based on the
//					   field width and the flag.
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest20L(  )
    {
    
	char *x="abcdefg";
	int ret;
	char* testfile="c:\\vfprintfTest20.txt";

	ret=vfprintfTests(testfile,"%-10.3s",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest21
//API Tested :vfprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format 
//					   string like %02d pads the output appropriately
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest21L(  )
    {
    
	int x=345;
	int ret;
	char* testfile="c:\\vfprintfTest21.txt";

	ret=vfprintfTests(testfile,"%06d",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest22
//API Tested :vfprintf
  
  
//TestCase Description:To test whether given a + character after the % in the format 
//					   string (eg. %+f),the sign is always copied to target 
//					   (+ sign for positive,even if it is not present in input string)
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest22L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* testfile="c:\\vfprintfTest22.txt";

	ret=vfprintfTests(testfile,"%+f",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest23
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the space(s) after the % in the format specifier 
//					   for numeric type inputs results in a copying of a single space 
//					   preceding the format string
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest23L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* testfile="c:\\vfprintfTest23.txt";
	char* fmt="%+     f";

	ret=vfprintfTests(testfile,fmt,x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest24
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for hexadecimal type(%#x) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest24L(  )
    {
    
	int x=34;
	int ret;
	char* testfile="c:\\vfprintfTest24.txt";

	ret=vfprintfTests(testfile,"%#x",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest25
//API Tested :vfprintf
  
  
//TestCase Description:To test whether the conversion for octal type(%#o) gives
//					   the desired results
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest25L(  )
    {
    
	int x=34;
	int ret;
	char* testfile="c:\\vfprintfTest25.txt";

	ret=vfprintfTests(testfile,"%#o",x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest26
//API Tested :vfprintf
  
  
//TestCase Description:To test whether simple padding with zeros using format string
//					   like %011f first pads the part after the decimal point digits 
//					    with zeros such that not more than 6 digits appear in exponent.	
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest26L(  )
    {
    
	float x=(float)34.4567;
	int ret;
	char* testfile="c:\\vfprintfTest26.txt";
	char* fmt="%-010.4f";

	ret=vfprintfTests(testfile,fmt,x);
	unlink(testfile);
 
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
//Function Name :vfprintfTest27
//API Tested :vfprintf
  
  
//TestCase Description:To test whether precision for integer type eg.%.6d fills up the minimum
//					   number of digits required in the target string with zeros  
// -----------------------------------------------------------------------------
  
TInt CTestStdio::vfprintfTest27L(  )
    {
    
	int x=90;
	int ret;
	char* testfile="c:\\vfprintfTest27.txt";

	ret=vfprintfTests(testfile,"%.6d",x);
	unlink(testfile);
 
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
// CTestStdio::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CTestStdio::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None



 /*
 *Testcase Name:vasprintfTest1()
 *API covered   :vasprintf
 *Description   :write formatted output to a string dynamically allocated with malloc
 *and store the address of the string	
 */
 	
 	
 	
TInt CTestStdio::vasprintfTest1()
	{
	int ret;
	char *pt ="asdf";
	char **ptr =&pt;
	ret=vasprintfTests(ptr,"%s",*ptr);
	if(ret<0)
		{
		return KErrGeneral;
		}
	return KErrNone;
	}

TInt CTestStdio::vasprintfTests(char **ptr,const char* fmt,...)
	{
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=vasprintf(ptr,fmt,ap);
	if(ret<0)
		{
		return KErrGeneral;
		}
 	va_end(ap);
 	free(*ptr);
 	return KErrNone;
 	}
 
 /*
 *Testcase Name:asprintfTest1()
 *API covered   :asprintf
 *Description   :write formatted output to a string dynamically allocated with malloc
 *and store the address of the string	
 */	
 	
 TInt CTestStdio::asprintfTest1()
 	{
 	char *myBuff;
	TInt ret = asprintf( &myBuff, "a string to format, with a number with a value of a string to format, with a number with a value of a string to format, with a number with a value of %d", 10); 
	if(ret!=152)
		{
		INFO_PRINTF1(_L("asprintf returned wrong value\n"));
		return KErrGeneral;
		}
	free(myBuff); 
	INFO_PRINTF1(_L("No panic, No memory Lost\n"));
	return KErrNone;
 	}
 	
//  End of File
