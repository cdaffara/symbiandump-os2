// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : twidecharapis.cpp
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



// INCLUDE FILES
#include "twchar.h"

// -----------------------------------------------------------------------------
//Function Name :wcsuprTest1
//API Tested :wcsupr
//TestCase Description: This test code converts the src string which lowercase 
//wide char string to uppercase wide character string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsuprTest1()
    {
    	wchar_t srcWcs[6] = L"abcde";
    	wchar_t *resWcs = L"ABCDE";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsupr(srcWcs);
    	
	   	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :wcsuprTest2
//API Tested :wcsupr
//TestCase Description: This test code converts the src string which is partially
//uppercase and partially lowercase wide char string to uppercase wide character string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsuprTest2()
    {
    	wchar_t srcWcs[6] = L"ABcde";
    	wchar_t *resWcs = L"ABCDE";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsupr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :wcsuprTest3
//API Tested :wcsupr
//TestCase Description: This test code converts the src string which is a uppercase 
//wide char string to uppercase wide character string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsuprTest3()
    {
    	wchar_t srcWcs[6] = L"ABCDE";
    	wchar_t *resWcs = L"ABCDE";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsupr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :wcsuprTest4
//API Tested :wcsupr
//TestCase Description: This test code converts the src string which is a mix of 
//lowercase and uppercase wide char string to uppercase wide character string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsuprTest4()
    {
    	wchar_t srcWcs[6] = L"abCde";
    	wchar_t *resWcs = L"ABCDE";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsupr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsuprTest5
//API Tested :wcsupr
//TestCase Description: This test code converts the src string which lowercase 
//wide chars with numerical chars mixed to uppercase wide character string.where the 
//numerical chars will be returned as it is.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsuprTest5()
    {
    	wchar_t srcWcs[8] = L"ab12cde";
    	wchar_t *resWcs = L"AB12CDE";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsupr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsuprTest6
//API Tested :wcsupr
//TestCase Description: This test code tries to convert the wide char string which
//is a numerical string into uppercase. Since, this is a numerical wide-char string,
//it should return as it is without any changes.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsuprTest6()
    {
    	wchar_t srcWcs[8] = L"12345";
    	wchar_t *resWcs = L"12345";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsupr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcslwrTest1
//API Tested :wcslwr
//TestCase Description: This test code tries to translate the wide-char string into
//lowercase wide char string and it will return a pointer to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcslwrTest1()
    {
    	wchar_t srcWcs[6] = L"ABCDE";
    	wchar_t *resWcs = L"abcde"; 
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcslwr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :wcslwrTest2
//API Tested :wcslwr
//TestCase Description: This test code tries to translate the wide-char string, which 
//is partially uppercase and partially lowercase into lowercase wide char string and 
//it will return a pointer to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcslwrTest2()
    {
    	wchar_t srcWcs[6] = L"ABcde";
    	wchar_t *resWcs = L"abcde";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcslwr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :wcslwrTest3
//API Tested :wcslwr
//TestCase Description: This test code tries to translate the wide-char string, which
//is already in lowercase into lowercase wide-char string and it should return a pointer 
//to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcslwrTest3()
    {
    	wchar_t srcWcs[6] = L"abcde";
    	wchar_t *resWcs = L"abcde";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcslwr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :wcslwrTest4
//API Tested :wcslwr
//TestCase Description: It should convert the srcWcs which is a mix of lowercase
//and uppercase wide-chars to lowercase wide-char string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcslwrTest4()
    {
    	wchar_t srcWcs[6] = L"abCde";
    	wchar_t *resWcs = L"abcde";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcslwr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :wcslwrTest5
//API Tested :wcslwr
//TestCase Description: This test code tries to translate the wide-char string, which
//is a mix of numerical and alphabetical chars into lowercase wide char string and it
//will return a pointer to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcslwrTest5()
    {
    	wchar_t srcWcs[8] = L"AB12CDE";
    	wchar_t *resWcs = L"ab12cde";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcslwr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }    

// -----------------------------------------------------------------------------
//Function Name :wcslwrTest6
//API Tested :wcslwr
//TestCase Description: This test code tries to convert the wide char string which
//is a numerical string into uppercase. Since, this is a numerical wide-char string,
//it should return as it is without any changes.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcslwrTest6()
    {
    	wchar_t srcWcs[8] = L"12345";
    	wchar_t *resWcs = L"12345";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcslwr(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsrevTest1
//API Tested :wcsrev
//TestCase Description: This test code tries to reverse the lowercase wide-char string, 
//return a pointer to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsrevTest1()
    {
    	wchar_t srcWcs[6] = L"abcde";
    	wchar_t *resWcs = L"edcba";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsrev(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :wcsrevTest2
//API Tested :wcsrev
//TestCase Description: This test code tries to reverse the uppercase wide-char string 
// and return a pointer to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsrevTest2()
    {
    	wchar_t srcWcs[6] = L"ABCDE";
    	wchar_t *resWcs = L"EDCBA";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsrev(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :wcsrevTest3
//API Tested :wcsrev
//TestCase Description: This test code tries to reverse the wide-char string 
// and return a pointer to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsrevTest3()
    {
    	wchar_t srcWcs[6] = L"AbcdE";
    	wchar_t *resWcs = L"EdcbA";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsrev(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsrevTest4
//API Tested :wcsrev
//TestCase Description: It should reverse the wide-char string which is a mix of
//lowercase and uppercase wide-char string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsrevTest4()
    {
    	wchar_t srcWcs[6] = L"abCde";
    	wchar_t *resWcs = L"edCba";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsrev(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :wcsrevTest5
//API Tested :wcsrev
//TestCase Description: This test code tries to reverse the wide-char string 
//which is a mix of numerical and alphabetical chars and it will return a pointer 
//to the reversed string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsrevTest5()
    {
    	wchar_t srcWcs[8] = L"AB12CDE";
    	wchar_t *resWcs = L"EDC21BA";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsrev(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }    
 
// -----------------------------------------------------------------------------
//Function Name :wcsrevTest6
//API Tested :wcsrev
//TestCase Description: This test code tries to reverse the numerical wide-char string 
// and return a pointer to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsrevTest6()
    {
    	wchar_t srcWcs[8] = L"12345";
    	wchar_t *resWcs = L"54321";
    	wchar_t *destWcs = NULL;
    	
    	destWcs = wcsrev(srcWcs);
    	
    	if(destWcs != NULL && wcscmp(destWcs, resWcs) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :wcsicmpTest1
//API Tested :wcsicmp
//TestCase Description: This test code tries to compare two wide-char strings by
//converting them into lowercase. It should return 0, as the two wide-char strings
//passed to wcsicmp are same except the case.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsicmpTest1()
    {
    	wchar_t *wcs1 = L"abcdef";
    	wchar_t *wcs2 = L"ABCDEF";
    	TInt	retVal;
    	
    	retVal = wcsicmp(wcs1,wcs2);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsicmpTest2
//API Tested :wcsicmp
//TestCase Description: This test code tries to compare two wide-char strings by
//converting them into lowercase. It should return 0, as the two wide-char strings
//passed to wcsicmp are same except the case.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsicmpTest2()
    {
    	wchar_t *wcs1 = L"ABCDEF";
    	wchar_t *wcs2 = L"abcdef";
    	TInt	retVal;
    	
    	retVal = wcsicmp(wcs1,wcs2);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :wcsicmpTest3
//API Tested :wcsicmp
//TestCase Description: This test code tries to compare two wide-char strings by
//converting them into lowercase. It should return 0, as the two wide-char strings
//passed to wcsicmp are same except the case.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsicmpTest3()
    {
    	wchar_t *wcs1 = L"abcdef";
    	wchar_t *wcs2 = L"AbcdEF";
    	TInt	retVal;
    	
    	retVal = wcsicmp(wcs1,wcs2);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsicmpTest4
//API Tested :wcsicmp
//TestCase Description: This test code tries to compare two wide-char strings by
//converting them into lowercase. Here the wcs1 is in lowercase and wcs2 is a mix 
//lowercase and uppercase wide-char string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsicmpTest4()
    {
    	wchar_t *wcs1 = L"abcdef";
    	wchar_t *wcs2 = L"abCDef";
    	TInt	retVal;
    	
    	retVal = wcsicmp(wcs1,wcs2);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsicmpTest5
//API Tested :wcsicmp
//TestCase Description: This test code tries to compare two wide-char strings by
//converting them into lowercase. It should return 0, as the parameters passed to
//it are equal except the difference in case.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsicmpTest5()
    {
    	wchar_t *wcs1 = L"AbcDef";
    	wchar_t *wcs2 = L"AbcdEF";
    	TInt	retVal;
    	
    	retVal = wcsicmp(wcs1,wcs2);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsicmpTest6
//API Tested :wcsicmp
//TestCase Description: This test code tries to compare two wide-char strings by
//converting them into lowercase. It should return 0,as the argument strings passed 
//to it are equal.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsicmpTest6()
    {
    	wchar_t *wcs1 = L"ABCDEF";
    	wchar_t *wcs2 = L"ABCDEF";
    	TInt	retVal;
    	
    	retVal = wcsicmp(wcs1,wcs2);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsicmpTest7
//API Tested :wcsicmp
//TestCase Description: This test code tries to compare two wide-char strings by
//converting them into lowercase. It should return > 0,as the second char in the
//first parameter is greater than second char in the second parameter.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsicmpTest7()
    {
    	wchar_t *wcs1 = L"hello";
    	wchar_t *wcs2 = L"hallo";
    	TInt	retVal;
    	
    	retVal = wcsicmp(wcs1,wcs2);
    	
    	if(retVal <= 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :wcsicmpTest8
//API Tested :wcsicmp
//TestCase Description: This test code tries to compare two wide-char strings by
//converting them into lowercase. It should return > 0,as the fourth char in the
//first parameter is greater than fourth char(i.e NULL) in the second parameter.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsicmpTest8()
    {
    	wchar_t *wcs1 = L"hello";
    	wchar_t *wcs2 = L"hel";
    	TInt	retVal;
    	
    	retVal = wcsicmp(wcs1,wcs2);
    	
    	if(retVal <= 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsicmpTest9
//API Tested :wcsicmp
//TestCase Description: This test code tries to compare two wide-char strings by
//converting them into lowercase. It should return < 0,as the fourth char(i.e. NULL)
//in the first parameter is less than fourth char in the second parameter.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsicmpTest9()
    {
    	wchar_t *wcs1 = L"hel";
    	wchar_t *wcs2 = L"hello";
    	TInt	retVal;
    	
    	retVal = wcsicmp(wcs1,wcs2);
    	
    	if(retVal >= 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcssetTest1
//API Tested :wcsset
//TestCase Description: This test code tries to set the characters in the wide-char
//string wcs with the wide char specified by wc.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcssetTest1()
    {
    	wchar_t wcs[6] = L"hello";
    	wchar_t wc = L'K';
    	wchar_t *exp = L"KKKKK";
    	wchar_t *res = NULL;
    	
    	res = wcsset(wcs,wc);
    	
    	if(res == NULL || wcscmp(res,exp) != 0 || wcscmp(wcs, exp) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcssetTest2
//API Tested :wcsset
//TestCase Description: This test code tries to set the characters in the wide-char
//string wcs with the wide char specified by wc.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcssetTest2()
    {
    	wchar_t wcs[9] = L"testcase";
    	wchar_t wc = L'M';
    	wchar_t *exp = L"MMMMMMMM";
    	wchar_t *res = NULL;
    	
    	res = wcsset(wcs, wc);
    	
    	if(res == NULL || wcscmp(res,exp) != 0 || wcscmp(wcs, exp) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcssetTest3
//API Tested :wcsset
//TestCase Description: This test code tries to set the characters in the wide-char
//string wcs with the wide char specified by wc. 
// -----------------------------------------------------------------------------
TInt CTestWChar::wcssetTest3()
    {
    	wchar_t wcs[7] = L"123456";
    	wchar_t wc = L'K';
    	wchar_t *exp = L"KKKKKK";
    	wchar_t *res = NULL;
    	
    	res = wcsset(wcs, wc);
    	
    	if(res == NULL || wcscmp(res,exp) != 0 || wcscmp(wcs, exp) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcssetTest4
//API Tested :wcsset
//TestCase Description: This test code tries to set the characters in the wide-char
//string wcs with the wide char specified by wc. here in this case all the chars of 
//wcs will be replaced by wide-char L'1'.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcssetTest4()
    {
    	wchar_t wcs[9] = L"abcd1234";
    	wchar_t wc = L'1';
    	wchar_t *exp = L"11111111";
    	wchar_t *res = NULL;
    	
    	res = wcsset(wcs, wc);
    	
    	if(res == NULL || wcscmp(res,exp) != 0 || wcscmp(wcs, exp) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnsetTest1
//API Tested :wcsnset
//TestCase Description: This test code sets first n chars in wcs with the wide char 
//specified by wc. here it should set only first char of wcs to wc and return a pointer
//to the converted string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnsetTest1()
    {
    	wchar_t wcs[6] = L"hello";
    	wchar_t wc = L'K';
    	wchar_t *exp = L"Kello";
    	wchar_t *res = NULL;
    	TInt n = 1;
    	
    	res = wcsnset(wcs,wc,n);
    	
    	if(res == NULL || wcscmp(res,exp) != 0 || wcscmp(wcs, exp) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnsetTest2
//API Tested :wcsnset
//TestCase Description: This test code sets first n chars in wcs with the wide char 
//specified by wc. here it should return a pointer to string L"ttllo", as the wc 
//passed is L't' and n = 2.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnsetTest2()
    {
    	wchar_t wcs[6] = L"hello";
    	wchar_t wc = L't';
    	wchar_t *exp = L"ttllo";
    	wchar_t *res = NULL;
    	TInt n = 2;
    	
    	res = wcsnset(wcs, wc, n);
    	
    	if(res == NULL || wcscmp(res,exp) != 0 || wcscmp(wcs, exp) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnsetTest3
//API Tested :wcsnset
//TestCase Description: This test code sets first n chars in wcs with the wide char 
//specified by wc. here it should return a pointer to L"cigerking", as wc is L'c'' 
//and n = 1.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnsetTest3()
    {
    	wchar_t wcs[10] = L"tigerking";
    	wchar_t wc = L'c';
    	wchar_t *exp = L"cigerking";
    	wchar_t *res = NULL;
    	TInt n = 1;
    	
    	res = wcsnset(wcs, wc, n);
    	
    	if(res == NULL || wcscmp(res,exp) != 0 || wcscmp(wcs, exp) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnsetTest4
//API Tested :wcsnset
//TestCase Description: This test code sets first n chars in wcs with the wide char 
//specified by wc. here it should return a pointer to L"cccerking", as wc is L'c'' 
//and n = 3.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnsetTest4()
    {
    	wchar_t wcs[10] = L"tigerking";
    	wchar_t wc = L'c';
    	wchar_t *exp = L"cccerking";
    	wchar_t *res = NULL;
    	TInt n = 3;
    	
    	res = wcsnset(wcs, wc, n);
    	
    	if(res == NULL || wcscmp(res,exp) != 0 || wcscmp(wcs, exp) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnsetTest5
//API Tested :wcsnset
//TestCase Description: This test code sets first n chars in wcs with the wide char 
//specified by wc. here it should return a pointer to the converted string and 
//source string passed is also updated with the wc for first n chars.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnsetTest5()
    {
    	wchar_t wcs[11] = L"helloworld";
    	wchar_t wc = L'O';
    	wchar_t *exp = L"OOOOOworld";
    	wchar_t *res = NULL;
    	TInt n = 5;
    	
    	res = wcsnset(wcs,wc,n);
    	
    	if(res == NULL || wcscmp(res,exp) != 0 || wcscmp(wcs, exp) != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :wcsnsetTest6
//API Tested :wcsnset
//TestCase Description: Test the behavior of wcsnset for a NULL pointer.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnsetTest6()
    {
    	if (wcsnset(NULL,L'O',1) == NULL)    	 	
    		return KErrNone;
    	return KErrGeneral;
    }

// -----------------------------------------------------------------------------
//Function Name :wstrtimeTest1
//API Tested :wstrtime
//TestCase Description: This test code tries to get the system time and converts it
//into wide-char string and retuns a pointer to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wstrtimeTest1()
    {
      wchar_t timewcs[20];
    	wchar_t *res = NULL;
    	
    	res = wstrtime(timewcs);
    	
    	if(res == NULL || timewcs == NULL)
    		return KErrGeneral;
    	
    	INFO_PRINTF3(_L("time str res = %s, timewcs = %s"),res, timewcs);
    	
    	return KErrNone;
   
    }

// -----------------------------------------------------------------------------
//Function Name :wstrtimeTest2
//API Tested :wstrtime
//TestCase Description: This test code tries to get the system time and converts it
//into wide-char string. here it should set errno to EFAULT and return NULL as the
//argument passed is NULL.
// -----------------------------------------------------------------------------
TInt CTestWChar::wstrtimeTest2()
    {
    	wchar_t *res = NULL;
    	
    	res = wstrtime(NULL);
    	
    	if(res != NULL || errno != EFAULT)
    		return KErrGeneral;
    	
    	return KErrNone;
   
    }
    
// -----------------------------------------------------------------------------
//Function Name :wstrdateTest1
//API Tested :wstrdate
//TestCase Description: This test code tries to get the system date and converts it
//into wide-char string and retuns a pointer to the same.
// -----------------------------------------------------------------------------
TInt CTestWChar::wstrdateTest1()
    {
     	wchar_t datewcs[20];
    	wchar_t *res = NULL;

    	res = wstrdate(datewcs);

    	if(res == NULL || datewcs == NULL)
    		return KErrGeneral;
    	
    	INFO_PRINTF3(_L("datestr res = %s, datewcs = %s"),res, datewcs);

    	return KErrNone;
   
    }

// -----------------------------------------------------------------------------
//Function Name :wstrdateTest2
//API Tested :wstrdate
//TestCase Description: This test code tries to get the system date and converts it
//into wide-char string. here it should set errno to EFAULT and return NULL as the
//argument passed is NULL.
// -----------------------------------------------------------------------------
TInt CTestWChar::wstrdateTest2()
    {
    	wchar_t *res = NULL;
    	
    	res = wstrdate(NULL);
    	
    	if(res != NULL || errno != EFAULT)
    		return KErrGeneral;
    	
    	return KErrNone;
   
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnicmpTest1
//API Tested :wcsnicmp
//TestCase Description: This test code tries to compare first n chars from two 
//wide-char strings by converting them into lowercase. It should return 0, as 
//the two wide-char strings  passed to wcsnicmp are same except the case and n=5.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnicmpTest1()
    {
    	wchar_t *wcs1 = L"abcdef";
    	wchar_t *wcs2 = L"ABCDEF";
    	int n = 5;
    	TInt	retVal;
    	
    	retVal = wcsnicmp(wcs1,wcs2,n);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnicmpTest2
//API Tested :wcsnicmp
//TestCase Description: This test code tries to compare first n chars from two 
//wide-char strings by converting them into lowercase. It should return 0, as 
//the two wide-char strings passed to wcsnicmp are same except the case.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnicmpTest2()
    {
    	wchar_t *wcs1 = L"ABCDEF";
    	wchar_t *wcs2 = L"abcdef";
    	int n = 5;
    	TInt	retVal;
    	
    	retVal = wcsnicmp(wcs1,wcs2,n);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :wcsnicmpTest3
//API Tested :wcsnicmp
//TestCase Description: This test code tries to compare first n chars from two 
//wide-char strings by converting them into lowercase. It should return 0, as 
//the two wide-char strings passed to wcsnicmp are same except the case.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnicmpTest3()
    {
    	wchar_t *wcs1 = L"abcdef";
    	wchar_t *wcs2 = L"AbcdEF";
    	int n = 8;
    	TInt	retVal;
    	
    	retVal = wcsnicmp(wcs1,wcs2,n);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnicmpTest4
//API Tested :wcsnicmp
//TestCase Description: This test code tries to compare first n chars from two 
//wide-char strings by converting them into lowercase. Here the wcs1 is in 
//lowercase and wcs2 is a mix lowercase and uppercase wide-char string.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnicmpTest4()
    {
    	wchar_t *wcs1 = L"abcdef";
    	wchar_t *wcs2 = L"abCDef";
    	int n = 2;
    	TInt	retVal;
    	
    	retVal = wcsnicmp(wcs1,wcs2,n);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnicmpTest5
//API Tested :wcsnicmp
//TestCase Description: This test code tries to compare first n chars from two 
//wide-char strings by converting them into lowercase. It should return 0,as 
//the argument strings passed to it are equal.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnicmpTest5()
    {
    	wchar_t *wcs1 = L"ABCDEF";
    	wchar_t *wcs2 = L"ABCDEF";
    	int n = 6;    	
    	TInt	retVal;
    	
    	retVal = wcsnicmp(wcs1,wcs2,n);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnicmpTest6
//API Tested :wcsnicmp
//TestCase Description: This test code tries to compare first n chars from two 
//wide-char strings by converting them into lowercase. It should return > 0,as 
//the second char in the first parameter is greater than second char in the 
//second parameter.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnicmpTest6()
    {
    	wchar_t *wcs1 = L"hello";
    	wchar_t *wcs2 = L"hallo";
    	int n = 4;
    	TInt	retVal;
    	
    	retVal = wcsnicmp(wcs1,wcs2,n);
    	
    	if(retVal <= 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :wcsnicmpTest7
//API Tested :wcsnicmp
//TestCase Description: This test code tries to compare first n chars from two 
//wide-char strings by converting them into lowercase. It should return > 0,as 
//the fourth char in the first parameter is greater than fourth char(i.e NULL) 
//in the second parameter.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnicmpTest7()
    {
    	wchar_t *wcs1 = L"hello";
    	wchar_t *wcs2 = L"hel";
    	int n = 5;
    	TInt	retVal;
    	
    	retVal = wcsnicmp(wcs1,wcs2,n);
    	
    	if(retVal <= 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcsnicmpTest8
//API Tested :wcsnicmp
//TestCase Description: This test code tries to compare first n chars from two 
//wide-char strings by converting them into lowercase. It should return < 0,as 
//the fourth char(i.e. NULL) in the first parameter is less than fourth char in 
//the second parameter.
// -----------------------------------------------------------------------------
TInt CTestWChar::wcsnicmpTest8()
    {
    	wchar_t *wcs1 = L"hel";
    	wchar_t *wcs2 = L"hello";
    	int n = 5;
    	TInt	retVal;
    	
    	retVal = wcsnicmp(wcs1,wcs2,n);
    	
    	if(retVal >= 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }

TInt CTestWChar::wcsnicmpTest9()
    {
    	wchar_t *wcs1 = L"abcdef";
    	wchar_t *wcs2 = L"ABCDEF";
    	int n = 0;
    	TInt	retVal;
    	
    	retVal = wcsnicmp(wcs1,wcs2,n);
    	
    	if(retVal != 0)
    		return KErrGeneral;
    	
    	return KErrNone;
    }
    
/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsicollTest1L
	API Tested				:	wcsicoll
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings.	
  --------------------------------------------------------------------------------------------- */
TInt CTestWChar::wcsicollTest1()
{
	wchar_t wcs1[15] = L"Hello world";
	wchar_t wcs2[10][20] = {
					   L"H",
					   L"Hello",
					   L"Hella yaar",
					   L"Hoi wala",
					   L"Joik%%%%%U**@@"}; 	


	// Positive Cases
	INFO_PRINTF1(_L("Positivecases [All Must be Zero]"));
	INFO_PRINTF2(_L(" %d"),wcsicoll(wcs1,wcs1));		//self comparision
	INFO_PRINTF2(_L(" %d"),wcsicoll(wcs1,L"Hello world"));      
	INFO_PRINTF2(_L(" %d"),wcsicoll(L"&&&%%^^@@",L"&&&%%^^@@"));
	INFO_PRINTF2(_L(" %d"),wcsicoll(wcs2[1],wcs2[1])); 
	INFO_PRINTF2(_L(" %d"),wcsicoll(wcs2[4]+5,L"%%%%U**@@"));
	
	// Negative Cases
	INFO_PRINTF1(_L("Negative cases"));
	
	if(	wcsicoll(wcs1,wcs2[4]) != wcs1[0]-wcs2[4][0])					return -1;
	if(	wcsicoll(wcs1,L"%%%%%%%")!= towlower(wcs1[0])-L'%')				return -1;
	if(	wcsicoll(wcs2[4],L"@%@Y") != towlower(wcs2[4][0])-L'@')			return -1;
	if(	wcsicoll(wcs2[4],L"Joik%%%k%U**@@") != towlower(wcs2[4][7])-L'k')	return -1;
	if( wcsicoll(L"sasdhkllllllllk",L"sasdhkllllllllm") != L'k'-L'm')	return -1;
		 
	return KErrNone;
}

/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsicollTest2L
	API Tested				:	wcsicoll
	TestCase Description	:	API is testd with NULL strings as both input strings. here "n"
							`	may take diffarent values.	
  --------------------------------------------------------------------------------------------- */
TInt CTestWChar::wcsicollTest2()
{
		//Null Cases
	INFO_PRINTF1(_L("NULL cases"));
	if(wcsicoll(L"",L""))		return -1;
	if(wcsicoll(L"\0",L""))		return -1;
	if(wcsicoll(L"\0",L"\0"))	return -1;
	
	return KErrNone;
}

/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsicollTest3L
	API Tested				:	wcsicoll
	TestCase Description	:	API is testd with NULL and NOn-NULL combinations input strings. here "n"
							`	may take diffarent values.	
  --------------------------------------------------------------------------------------------- */
TInt CTestWChar::wcsicollTest3()
{
	wchar_t wcs2[10][20] = 	{
							   L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							};
	//NULL and non-NULL combinations..
	INFO_PRINTF1(_L("NULL and non-NULL combinations"));
	if(	wcsicoll(wcs2[4],L"") != towlower(wcs2[4][0]))			return -1;
	if(	wcsicoll(wcs2[4]+5,L"") != towlower(wcs2[4][5]))		return -1;
	if(	wcsicoll(L"",L"@%@Y")!= -L'@')							return -1;
	if(	wcsicoll(L"",L"56dfasdask@#@44") != -L'5')				return -1;
	if(	wcsicoll(L"",L"^") != -L'^')							return -1;

	return KErrNone;
}

/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsncollTest1L
	API Tested				:	wcsncoll
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings.	
  --------------------------------------------------------------------------------------------- */
TInt CTestWChar::wcsncollTest1()
{
	 __UHEAP_MARK;
	 
	wchar_t wcs1[15] = L"Hello world";
	wchar_t wcs2[10][20] = {
					   L"H",
					   L"Hello",
					   L"Hella yaar",
					   L"Hoi wala",
					   L"Joik%%%%%U**@@"}; 	


	// Positive Cases
	INFO_PRINTF1(_L("Positivecases [All Must be Zero]"));
	
	INFO_PRINTF2(_L(" %d"),wcsncoll(wcs1,wcs1,12));
		
	INFO_PRINTF2(_L(" %d"),wcsncoll(wcs1,L"Hello world",5));
	INFO_PRINTF2(_L(" %d"),wcsncoll(L"&&&%%^^@@",L"&&&%%^^@@",3));
	INFO_PRINTF2(_L(" %d"),wcsncoll(wcs2[1],wcs2[1],2));
	INFO_PRINTF2(_L(" %d"),wcsncoll(wcs2[4]+5,L"%%%%U**@@",5));
	

	// Negative Cases
	INFO_PRINTF1(_L("Negative cases"));
	
	if(	wcsncoll(wcs1,wcs2[4],10) != wcs1[0]-wcs2[4][0])					return -1;
	if(	wcsncoll(wcs1,L"%%%%%%%",8)!= wcs1[0]-L'%')							return -1;
	if(	wcsncoll(wcs2[4],L"@%@Y",5) != wcs2[4][0]-L'@')						return -1;
	if(	wcsncoll(wcs2[4],L"Joik%%%k%U**@@",10) != wcs2[4][7]-L'k')			return -1;
	if( wcsncoll(L"sasdhkllllllllk",L"sasdhkllllllllm",16) != L'k'-L'm')	return -1;
		 

	__UHEAP_MARKEND;
	
	return KErrNone;
}

/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsncollTest2L
	API Tested				:	wcsncoll
	TestCase Description	:	API is testd with NULL strings as both input strings. here "n"
							`	may take diffarent values.	
  --------------------------------------------------------------------------------------------- */
TInt CTestWChar::wcsncollTest2()
{
		//Null Cases
	INFO_PRINTF1(_L("NULL cases"));
	if(wcsncoll(L"",L"",2))			return -1;
	if(wcsncoll(L"\0",L"",2))		return -1;
	if(wcsncoll(L"\0",L"\0",2))		return -1;
	
	return KErrNone;
}

/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsncollTest3L
	API Tested				:	wcsncoll
	TestCase Description	:	API is testd with NULL and NOn-NULL combinations input strings. here "n"
							`	may take diffarent values.	
  --------------------------------------------------------------------------------------------- */
TInt CTestWChar::wcsncollTest3()
{
	wchar_t wcs2[10][20] = 	{
							   L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							};
	//NULL and non-NULL combinations..
	INFO_PRINTF1(_L("NULL and non-NULL combinations"));
	if(	wcsncoll(wcs2[4],L"",2) != wcs2[4][0])			return -1;
	if(	wcsncoll(wcs2[4]+5,L"",2) != wcs2[4][5])			return -1;
	if(	wcsncoll(L"",L"@%@Y",2)!= -L'@')							return -1;
	if(	wcsncoll(L"",L"56dfasdask@#@44",5) != -L'5')				return -1;
	if(	wcsncoll(L"",L"^",2) != -L'^')								return -1;

	return KErrNone;
}

/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsnicollTest1L
	API Tested				:	wcsnicoll
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings.	
  --------------------------------------------------------------------------------------------- */
TInt CTestWChar::wcsnicollTest1()
{
	 __UHEAP_MARK;
	 
	wchar_t wcs1[15] = L"Hello world";
	wchar_t wcs2[10][20] = {
					   L"H",
					   L"Hello",
					   L"Hella yaar",
					   L"Hoi wala",
					   L"Joik%%%%%U**@@"}; 	


	// Positive Cases
	INFO_PRINTF1(_L("Positivecases [All Must be Zero]"));
	
	INFO_PRINTF2(_L(" %d"),wcsnicoll(wcs1,wcs1,12));
		
	INFO_PRINTF2(_L(" %d"),wcsnicoll(wcs1,L"Hello world",5));
	INFO_PRINTF2(_L(" %d"),wcsnicoll(L"&&&%%^^@@",L"&&&%%^^@@",3));
	INFO_PRINTF2(_L(" %d"),wcsnicoll(wcs2[1],wcs2[1],2));
	INFO_PRINTF2(_L(" %d"),wcsnicoll(wcs2[4]+5,L"%%%%U**@@",5));
	

	// Negative Cases
	INFO_PRINTF1(_L("Negative cases"));
	
	if(	wcsnicoll(wcs1,wcs2[4],10) != towlower(wcs1[0])-towlower(wcs2[4][0]))					return -1;
	if(	wcsnicoll(wcs1,L"%%%%%%%",8)!= towlower(wcs1[0])-L'%')						return -1;
	if(	wcsnicoll(wcs2[4],L"@%@Y",5) != towlower(wcs2[4][0])-L'@')					return -1;
	if(	wcsnicoll(wcs2[4],L"Joik%%%k%U**@@",10) != towlower(wcs2[4][7])-L'k')		return -1;
	if( wcsnicoll(L"sasdhkllllllllk",L"sasdhkllllllllm",16) != L'k'-L'm')			return -1;
		 

	__UHEAP_MARKEND;
	
	return KErrNone;
}

/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsnicollTest2L
	API Tested				:	wcsnicoll
	TestCase Description	:	API is testd with NULL strings as both input strings. here "n"
							`	may take diffarent values.	
  --------------------------------------------------------------------------------------------- */
TInt CTestWChar::wcsnicollTest2()
{
		//Null Cases
	INFO_PRINTF1(_L("NULL cases"));
	if(wcsnicoll(L"",L"",2))			return -1;
	if(wcsnicoll(L"\0",L"",2))			return -1;
	if(wcsnicoll(L"\0",L"\0",2))		return -1;
	
	return KErrNone;
}

/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsnicollTest3L
	API Tested				:	wcsnicoll
	TestCase Description	:	API is testd with NULL and NOn-NULL combinations input strings. here "n"
							`	may take diffarent values.	
  --------------------------------------------------------------------------------------------- */
TInt CTestWChar::wcsnicollTest3()
{
	wchar_t wcs2[10][20] = 	{
							   L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							};
	//NULL and non-NULL combinations..
	INFO_PRINTF1(_L("NULL and non-NULL combinations"));
	if(	wcsnicoll(wcs2[4],L"",2) != towlower(wcs2[4][0]))			return -1;
	if(	wcsnicoll(wcs2[4]+5,L"",2) != towlower(wcs2[4][5]))			return -1;
	if(	wcsnicoll(L"",L"@%@Y",2)!= -L'@')							return -1;
	if(	wcsnicoll(L"",L"56dfasdask@#@44",5) != -L'5')				return -1;
	if(	wcsnicoll(L"",L"^",2) != -L'^')								return -1;

	return KErrNone;
}

// -----------------------------------------------------------------------------
//Function Name :wtmpnamTest1
//API Tested :wtmpnam
//TestCase Description: To test whether the wtmpnam returns a temporary file name
// -----------------------------------------------------------------------------

TInt CTestWChar::wtmpnamTest1()
    {
    __UHEAP_MARK;
    	
    TInt ret = KErrNone; 

	wchar_t	buf[L_tmpnam];
	wchar_t    rbuf[10];
	
	wchar_t *rval = wtmpnam(buf);
	
	FILE *fp = wfopen(buf, L"w");
	
	if (fp == NULL)
	{
		INFO_PRINTF2(_L("wfopen of file returned by wtmpnam() failed - errno %d "), errno);
		ret = KErrGeneral;
	}
	
	if(fp)
	{
		fwprintf(fp, L"%ls", L"check");
		fclose(fp);
	}
	
	fp = wfopen(buf, L"r");
	
	if(fp)
	{
		fwscanf(fp, L"%ls", rbuf);
		fclose(fp);
	}
	
	INFO_PRINTF2(_L("read from file: %ls"), rbuf);
	INFO_PRINTF2(_L("argument buf: %ls"), buf);
	INFO_PRINTF2(_L("return value: %ls"), rval);
	
	if ( buf != rval || (wcscmp(buf, rval) != 0) || (wcscmp(rbuf, L"check") != 0) )
		ret = KErrGeneral;
	
	__UHEAP_MARKEND;

	return ret;
    
    }
    
// -----------------------------------------------------------------------------
//Function Name :wtmpnamTest2
//API Tested :wtmpnam
//TestCase Description:	To test whether the wtmpnam returns a temporary file name
//						when the input buf is NULL.
// -----------------------------------------------------------------------------
TInt CTestWChar::wtmpnamTest2()
    {
    __UHEAP_MARK;
      
    TInt ret = KErrNone; 	

	wchar_t    rbuf[10];
	
	wchar_t *rval = wtmpnam(NULL);
	
	FILE *fp = wfopen(rval, L"w");
	
	if (fp == NULL)
	{
		INFO_PRINTF2(_L("wfopen of file returned by wtmpnam() failed - errno %d "), errno);
		ret = KErrGeneral;
	}

	if(fp)
	{
		fwprintf(fp, L"%ls", L"check");
		fclose(fp);
	}
	
	fp = wfopen(rval, L"r");
	if(fp)
	{
		fwscanf(fp, L"%ls", rbuf);
		fclose(fp);
	}
	
	INFO_PRINTF2(_L("read from file: %ls"), rbuf);
	
	INFO_PRINTF2(_L("argument buf: %ls"), rval);
		
	if ( wcscmp(rbuf, L"check") != 0 )
		ret = KErrGeneral;
		
	__UHEAP_MARKEND;		

	return ret;
    
    }


//  End of File
