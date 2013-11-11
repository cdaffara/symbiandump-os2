/*
* Copyright (c) 1997-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CTEST.H"


#define TELLME "Tell Me The Answer"

test_Data;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1081
@SYMTestCaseDesc	    Tests for multibyte to wide character conversions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for conversions from multibyte to wide character and wide character to multibyte
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
int main()
{


    int      i,x;
    char    *pmbc    = (char *)malloc(MB_CUR_MAX);
    wchar_t  wc      = L'a';
    wchar_t *pwcnull = NULL;
    wchar_t *pwc     = (wchar_t *)malloc( sizeof( wchar_t ) );

	char * narrowstring = (char*)malloc(50);
	wchar_t * pwcstring = (wchar_t *)malloc(sizeof(wchar_t)*50);

	wchar_t greek[] = {0x03a3, 0x03bf, 0x03bc, 0x03b4, 0x03b5, 0x0345, 0x03a9, 0x0392, 0x0395, 0x03a1, 0};
	

	
	test_Title("Wide/Multibyte character conversions.");
	
	test_Next("Convert a NULL wide character to multibyte character");
    i = wctomb( pmbc, L'\0');
    test(1==i);
    test(*pmbc=='\0');


	test_Next("Convert a wide character to multibyte character:");
    i = wctomb(pmbc, wc);
    test(i==1);
    test(*pmbc=='a');

    test_Next("Convert multibyte character back to a wide character");
    i = mbtowc(pwc, pmbc, 20);
    test(i==1);
    test(*pwc==L'a');

    //check this
	test_Next("Attempt to convert when target is NULL");
    i = mbtowc(pwcnull, pmbc, MB_CUR_MAX);
    test(i==1);

    test_Next( "Attempt to convert a NULL pointer to a wide character");
    pmbc = NULL;
    i = mbtowc( pwc, pmbc, MB_CUR_MAX );
    test(i==0);

	test_Next("Convert narrow string to unicode");
	strcpy(narrowstring, TELLME);
	i = mbstowcs(pwcstring, narrowstring, 50);
	test(i==(int)strlen(TELLME));		//should be same
	
	test_Next("Convert unicode string back to UTF8");
	i = wcstombs(narrowstring, pwcstring, 50);
	test(i==(int)strlen(TELLME));		//should be same
	test(strcmp(narrowstring, TELLME) == 0);	

	test_Next("Get length of dest string");
	i = wcstombs(NULL, pwcstring, 50);
//	test(i==strlen(TELLME));		//should be same
//	test(strcmp(narrowstring, TELLME) == 0);	

	
	test_Next("Convert first 10 wide characters");
	strcpy(narrowstring,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	i = wcstombs(narrowstring, pwcstring, 10);
	test(i==10);
	test(memcmp(narrowstring,TELLME,10)==0);


	test_Next("Convert greek characters to unicode and back");
	i = wcstombs(narrowstring, &greek[0], 50);
	x = i;
	i = mbstowcs(pwcstring, narrowstring, 50);
	test(2*i==x);
	test(0==memcmp(pwcstring, greek, sizeof(greek)));

	test_Next("onto mblen");
	i = mblen("abc",3);
	test(i==1);

	test_Next("length of a greek character");
	i = mblen(narrowstring,6);
	test (i==2);

	test_Next("test an empty string");
	i = mblen("",2);
	test(i==0);

	test_Next("test a null pointer");
	i = mblen(NULL,2);
	test(i==0);

	test_Next("test a failure");
	i = mblen(narrowstring,1);
	test(i==-1);
	
	
	free(pmbc);
	free(pwc);
	free(pwcstring);
	free(narrowstring);

	test_Close();
}
