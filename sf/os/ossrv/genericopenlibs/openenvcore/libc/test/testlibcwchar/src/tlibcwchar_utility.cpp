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


//********************** WCHAR UTILITY TESTCASES ******************************
	
/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncmp_Test_1L
	API Tested				:	wcsncmp()
	TD TestCase Number		:	1_15_2_515
	TD TestCase Identifier	:	515
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings, and "n" may take diffarent values.
								<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncmp_Test_1L(  )
	{
    	 
	wchar_t wcs1[15] = L"Hello world";
	wchar_t wcs2[10][20] = 	{  L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							   }; 	


	// Positive Cases
	INFO_PRINTF1(_L("Positivecases"));

	if(wcsncmp(wcs1,wcs1,wcslen(wcs1)))	
		{
		return -1;
		}
	if(wcsncmp(wcs1,wcs2[0],1))
		{
		return -1;
		}
	if(wcsncmp(wcs1,wcs2[1],5))
		{
		return -1; 
		}
	if(wcsncmp(wcs1,wcs2[2],4))
		{
		return -1;
		}
	if(wcsncmp(wcs1,wcs2[3],1))
		{
		return -1;
		}
	if(wcsncmp(wcs2[4]+5,L"%%",2))
		{
		return -1;
		}

	// Negative Cases
	INFO_PRINTF1(_L("Negative cases"));
	
	if(wcsncmp(wcs1,wcs2[4],10) != wcs1[0]-wcs2[4][0])
		{
		return -1;
		}
	if(wcsncmp(wcs2[4],L"@%@Y",10) != wcs2[4][0]-L'@')
		{
		return -1;
		}
	if(wcsncmp(wcs2[4],L"Joik%%%k%U**@@",10) != wcs2[4][7]-L'k')
		{
		return -1;
		}
	return KErrNone;
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncmp_Test_2L
	API Tested				:	wcsncmp()
	TD TestCase Number		:	1_15_2_516
	TD TestCase Identifier	:	516
	TestCase Description	:	This will test the API for the wide-character NULL strings. Here 
								both inputs chosen as NULL strings. "n" may take diffarent values
								<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcsncmp_Test_2L(  )
	{

	//Null Cases
	INFO_PRINTF1(_L("NULL cases"));
	if(wcsncmp(L"",L"",1))
		{
		return -1;
		}
	if(wcsncmp(L"\0",L"",123))
		{
		return -1;
		}
	if(wcsncmp(L"\0",L"\0",123))
		{
		return -1;
		}
	return KErrNone;
	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncmp_Test_3L
	API Tested				:	wcsncmp()
	TD TestCase Number		:	1_15_2_517
	TD TestCase Identifier	:	517
	TestCase Description	:	API is tested for the combination of NULL and non-NULL as input 
								wide-character strings.
								<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncmp_Test_3L(  )
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
	if(	wcsncmp(wcs2[4],L"",10) != wcs2[4][0])
		{
		return -1;
		}
	if(	wcsncmp(wcs2[4]+5,L"",10)!= wcs2[4][5])
		{
		return -1;
		}
	if( wcsncmp(L"",L"@%@Y",10) != -L'@')
		{
		return -1;
		}
	if(	wcsncmp(L"",L"56dfasdask@#@44",10) != -L'5')
		{
		return -1;
		}
	if(	wcsncmp(L"",L"^",10) != -L'^')
		{
		return -1;
		}
	return KErrNone;

	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncmp_Test_4L
	API Tested				:	wcsncmp()
	TD TestCase Number		:	1_15_2_509
	TD TestCase Identifier	:	509
	TestCase Description	:	This will test the API for the range of values of N, Here API is tested 
								with the maximum and minimum values of N.
								<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncmp_Test_4L(  )
	{

	//Test for size_t and for the large strings
	INFO_PRINTF1(_L("Test for size_t"));
	size_t k_max = (size_t)0xFFFFFFFF;
	size_t k_min = (size_t)0x00000000; 


	if(wcsncmp(L"",L"",k_max))
		{
		return -1;
		}
	if(wcsncmp(L"",L"",k_max))
		{
		return -1;
		}
	if(wcsncmp(L"",L"",k_min))
		{
		return -1;
		}
	if(wcsncmp(L"djfdfdsjfldsjf",L"!!32314fffsa",0))
		{
		return -1;
		}
	if(wcsncmp(L"",L"", (unsigned int)-1))
		{
		return -1;
		}

	return KErrNone;

	}
	
 

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncmp_Test_5L
	API Tested				:	wcsncmp()
	TD TestCase Number		:	1_15_2_520
	TD TestCase Identifier	:	520
	TestCase Description	:	This will test the API with large string as input strings	and 
								n" may take diffarent values. This will read the chars from a file 
								and populate it in to a buffer, These buffers are used for testing.
								<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncmp_Test_5L(  )
	{

	// Test for  16-bit characters //readin from file 
	INFO_PRINTF1(_L(" Test for 16-bit characters"));

	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
									0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));
	
	FILE *fp =fopen(filename , "r");
	if(fp == NULL)
		{
	    INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	
	wchar_t fwcs1[15];
	wchar_t fwcs2[15];
	wchar_t fwcs3[15];
	fgetws(fwcs1,10,fp);
	fclose(fp);
	fp =fopen(filename , "r");
	
	fgetws(fwcs2,10,fp);
	fgetws(fwcs3,10,fp);  	
	 	

	if(	wcsncmp(fwcs1,fwcs2,7))
		{
		return -1;
		}
	if(	wcsncmp(fwcs1,fwcs2,5))
		{
		return -1;
		}
	INFO_PRINTF2(_L("Comparision b/n Two Hindi words [Must be Unmatch] %d"),wcsncmp(fwcs2,fwcs3,10)); 
	
	fclose(fp);
		 
	return KErrNone;
	}




/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscmp_Test_1L
	API Tested				:	wcscmp()
	TD TestCase Number		:	1_15_2_464
	TD TestCase Identifier	:	464
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings,This will test for both positive and negitive 								\cases
								<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscmp_Test_1L(  )
    {
    
    
	 
	wchar_t wcs1[15] = L"Hello world";
	wchar_t wcs2[10][20] = 	{
							   L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							}; 	
	// Positive Cases
	INFO_PRINTF1(_L("Positivecases"));
	if(	wcscmp(wcs1,wcs1))//self comparision
		{
		return -1;
		}
	if(	wcscmp(wcs1,L"Hello world"))
		{
		return -1;
		}
	if( wcscmp(L"&&&%%^^@@",L"&&&%%^^@@"))
		{
		return -1;
		}
	if(	wcscmp(wcs2[1],wcs2[1]))
		{
		return -1;
		}
	if(	wcscmp(wcs2[4]+5,L"%%%%U**@@"))
		{
		return -1;
		}
	
	
	// Negative Cases
	INFO_PRINTF1(_L("Negative cases"));
	
	
	if(	wcscmp(wcs1,wcs2[4]) != wcs1[0]-wcs2[4][0])
		{
		return -1;
		}
	if(	wcscmp(wcs1,L"%%%%%%%") != wcs1[0]-L'%')
		{
		return -1;
		}
	if(	wcscmp(wcs2[4],L"@%@Y") != wcs2[4][0]-L'@')
		{
		return -1;
		}
	if(	wcscmp(wcs2[4],L"Joik%%%k%U**@@") !=wcs2[4][7]-L'k')
		{
		return -1;
		}
	if( wcscmp(L"sasdhkllllllllk",L"sasdhkllllllllm") != L'k'-L'm')
		{
		return -1;
		}
	
		 
	return KErrNone;

	}



/* --------------------------------------------------------------------------------------------
Function Name			:	Wcscmp_Test_2L
API Tested				:	wcscmp()
TD TestCase Number		:	1_15_2_424
TD TestCase Identifier	:	424
TestCase Description	:	API Tested by choosing NULL wide-character string as both input 
						strings.
						<MRT_WCHARSTR.CPP>
--------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscmp_Test_2L(  )
	{

	//Null Cases
	INFO_PRINTF1(_L("NULL cases"));
	if(wcscmp(L"",L""))
		{
		return -1;
		}
	if(wcscmp(L"\0",L""))
		{
		return -1;
		}
	if(wcscmp(L"\0",L"\0"))
		{
		return -1;
		}
	
		 
	return KErrNone;
	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscmp_Test_3L
	API Tested				:	wcscmp()
	TD TestCase Number		:	1_15_2_425
	TD TestCase Identifier	:	425
	TestCase Description	:	API is tested for the combination of NULL and non-NULL as input 
								wide-character strings.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscmp_Test_3L(  )
	{
	 
	wchar_t wcs2[10][20] = 	{
							   L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							};
	int err =0;
	//NULL and non-NULL combinations..
	INFO_PRINTF1(_L("NULL and non-NULL combinations"));
	err = wcscmp(wcs2[4],L"");
	if(	err != wcs2[4][0])
		{
		return -1;
		}
	
	err = wcscmp(wcs2[4]+5,L"");
	if(	err != wcs2[4][5])
		{
		return -1;
		}
		
	err = wcscmp(L"",L"@%@Y");
	if(	err != -L'@')
		{
		return -1;
		}

	err = wcscmp(L"",L"56dfasdask@#@44");
	if(	err != -L'5')
		{
		return -1;
		}
	
	err = wcscmp(L"",L"^");
	if(	err != -L'^')
		{
		return -1;
		}

		 
	return KErrNone;
	}




/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscmp_Test_4L
	API Tested				:	wcscmp()
	TD TestCase Number		:	1_15_2_423
	TD TestCase Identifier	:	423
	TestCase Description	:	API is tested for large strings, here the inputs buffers of huge 
								length
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscmp_Test_4L(  )
	{	 

	//Test for  the large strings
	INFO_PRINTF1(_L("Large string Tests"));
	wchar_t buf[][250] = { L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd",
						L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd+ds4534~!~!~!~!~!~!12351`ds7@#$%^&&_&&*YU&&&&&&(&*&*"
						L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd"
	}; 	




	//INFO_PRINTF2(_L("Must be Zero: %d" ,wcscmp(buf[0],buf[2])); //Was L8
	if(	wcscmp(L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd",L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd"))
		{
		return -1;
		}
	if(	wcscmp(buf[0],buf[1]) != -L'+')
		{
		return -1;
		}

	 
	// Test for  16-bit characters //readin from file 
	INFO_PRINTF1(_L(" Test for 16-bit characters"));
	
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));


	FILE *fp = fopen(filename,"r");

	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	wchar_t fwcs1[15];
	wchar_t fwcs2[15];
	wchar_t fwcs3[15];
	fgetws(fwcs1,10,fp);
	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	fgetws(fwcs2,10,fp);
	fgetws(fwcs3,10,fp);  	
	fclose(fp); 	

	INFO_PRINTF2(_L("1st WC-string[wcscmp]:%s") ,fwcs1);
	INFO_PRINTF2(_L("2st WC-string[wcscmp]:%s") ,fwcs2);

	if(	wcscmp(fwcs1,fwcs2))
		{
		return -1;
		}
	if(	wcscmp(fwcs2,fwcs3) != fwcs2[0]-fwcs3[0])
		{
		return -1;
		}

		 	 
	return KErrNone;
	}




/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscat_Test_1L
	API Tested				:	wcscat()
	TD TestCase Number		:	1_15_2_250
	TD TestCase Identifier	:	250
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings. will check the API for both positive and Negative 
								cases.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscat_Test_1L(  )
	{

	//common strings
	INFO_PRINTF1(_L("Test for common cases"));
	wchar_t *res1,*res2,*res3;

	wchar_t str[][200] = { L"Hello",
						L"Time",
						L"Hio",
						L"012345"

	};

	res1 = wcscat(str[0],L" World");
	res2 = wcscat(str[1],L"\0World");
	res3 = wcscat(str[2],L"890\0abc");
	

	if(	wcscmp(res1,str[0]))
		{
		return -1;
		}
	if(	wcscmp(res2,str[1]))
		{
		return -1;
		}
	if(	wcscmp(res3,str[2]))
		{
		return -1;
		}
	
	INFO_PRINTF2(_L(" Res1[wcscat]:%ls") ,res1);
	INFO_PRINTF2(_L(" Res2[wcscat]:%ls") ,res2);
	INFO_PRINTF2(_L(" Res3[wcscat]:%ls") ,res3);
	
		 
	return KErrNone;
	}
      
	

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscat_Test_2L
	API Tested				:	wcscat()
	TD TestCase Number		:	1_15_2_235
	TD TestCase Identifier	:	235
	TestCase Description	:	API is tested with NULL strings as both input strings.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscat_Test_2L(  )
	{
	//Null cases

	INFO_PRINTF1(_L("Null cases"));  
	wchar_t ne[10] = L"\0";
	wchar_t *nres1 = wcscat(ne,L"");
	wchar_t *nres2 = wcscat(ne,L"\0World");


	INFO_PRINTF2(_L("Expected Null[nres1] ::%ls") ,nres1);
	INFO_PRINTF2(_L("Expected Null[ne] ::%ls") ,ne);
	INFO_PRINTF2(_L("Expected Null[nres2] ::%ls") ,nres2);
	INFO_PRINTF2(_L("Expected Null[ne] ::%ls") ,ne);

		 
	return KErrNone;
	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscat_Test_3L
	API Tested				:	wcscat()
	TD TestCase Number		:	1_15_2_254
	TD TestCase Identifier	:	254
	TestCase Description	:	API is tested for NULL and non-NULL string as input wide-character
								strings.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscat_Test_3L(  )
	{

	//Null and Non-NULL combinations

	wchar_t NonwStr[20] = L"VYKOn"; 
	wchar_t ne[10] = L"\0";
	INFO_PRINTF1(_L("Null and Non-NULL combinations")); 
	wchar_t *res1 = wcscat(NonwStr,L"");
	wchar_t *res2 = wcscat(ne,NonwStr);
	
	INFO_PRINTF2(_L("Res1[wcscat]	:: %ls") ,res1);
	INFO_PRINTF2(_L("str1[wcscat]	:: %ls") ,NonwStr);	
	INFO_PRINTF2(_L("Res2[wcscat]	:: %ls") ,res2);
	INFO_PRINTF2(_L("str2[wcscat]  :: %ls") ,ne);

		 
	return KErrNone;
	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscat_Test_4L
	API Tested				:	wcscat()
	TD TestCase Number		:	1_15_2_712
	TD TestCase Identifier	:	712
	TestCase Description	:	API is tested with the large buffers as input wide-character strings.
								These buffers are populated using a file.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscat_Test_4L(  )
	{
     
	// Reading from file ., Fil inputs

	INFO_PRINTF1(_L(" Test for 16-bit characters"));
	 
	wchar_t fwcs1[15];
	wchar_t fwcs2[15];
	wchar_t fwcs3[15];
	wchar_t fwcs4[15];

	
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));

	FILE *fp = fopen(filename,"r");

	if(fp == NULL)
		{
	    INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	fgetws(fwcs1,6,fp);
	fgetws(fwcs2,4,fp);
	fgetws(fwcs3,8,fp); 
	fgetws(fwcs4,3,fp);
	fclose(fp); 	

	INFO_PRINTF2(_L("1st WC-string[fwcs1]: %ls") ,fwcs1);
	INFO_PRINTF2(_L("2st WC-string[fwcs2]: %ls") ,fwcs2);

	wchar_t *res3 = wcscat(fwcs1,fwcs2); 

	INFO_PRINTF2(_L("Resultant of wcscat(fwcs1,fwcs2): %ls") ,res3);
	if(wcscmp(res3,fwcs1))
		{
		return -1;
		}
	
	INFO_PRINTF2(_L("1st WC-string[fwcs3]:%ls") ,fwcs3);
	INFO_PRINTF2(_L("2st WC-string[fwcs4]:%ls") ,fwcs4);

	wchar_t *res4 = wcscat(fwcs3,fwcs4);

	INFO_PRINTF2(_L("Resultant of wcscat(fwcs3,fwcs4): %ls") ,res4);

	if(wcscmp(res4,fwcs3))
		{
		return -1;
		}
	
	// Buffer concatenation..
	INFO_PRINTF1(_L("_Large Wide string cases_")); //WAS L8
	wchar_t buf[][500] = { 	L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd\
						  	  SKDKDASDLSLdjLADJLADJLASDJLAS$$^%&",
							L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd+ds4534~!~!~!~!~!~!12351`ds7@#$%^&&_&&*YU&&&&&&(&*&*"
							L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd"
	}; 		

	TInt size = wcslen(buf[0]); 
	wchar_t *res1 = wcscat(buf[0],buf[1]);
	
	if(	wcscmp(res1,buf[0]))
		{
		return -1;
		}
	if(	wcslen(buf[0])	!= (size + wcslen(buf[1])))
		{
		return -1;
		}
		
	return KErrNone;


	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncat_Test_1L
	API Tested				:	wcsncat()
	TD TestCase Number		:	1_15_2_502
	TD TestCase Identifier	:	502
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings, and "n" may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncat_Test_1L(  )
	{

	//common strings

	INFO_PRINTF1(_L("Test for common cases"));
	wchar_t *res1,*res2,*res3,*res4;

	wchar_t str[][20] = {	L"Hello",
							L"Time",
							L"Hio",
							L"012345"

	};


	res1 = wcsncat(str[0],L" World",12);
	res2 = wcsncat(str[1],L"\0World",3);
	res3 = wcsncat(str[2],L"890\0abc",1);
	res4 = wcsncat(str[3],L"67890123456789",4);
	

	if(wcscmp(res1,str[0]))
		{
		return -1;
		}
	if(wcscmp(res2,str[1]))
		{
		return -1;
		}
	if(wcscmp(res3,str[2]))
		{
		return -1;
		}
	if(wcscmp(res4,str[3]))
		{
		return -1;
		}

	INFO_PRINTF2(_L("Expected=Helloworld, Res1[wcsncat] :%ls") ,res1);
	INFO_PRINTF2(_L("Expected=Time, Res2[wcsncat] :%ls") ,res2);
	INFO_PRINTF2(_L("Expected=Hio890, Res3[wcsncat] :%ls") ,res3);
	INFO_PRINTF2(_L("Expected=0123456789, Res4[wcsncat] :%ls") ,res4);
	
	
		 
	return KErrNone;
	}



	/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncat_Test_2L
	API Tested				:	wcsncat()
	TD TestCase Number		:	1_15_2_503
	TD TestCase Identifier	:	503
	TestCase Description	:	API is testd with NULL strings as both input strings. here "n"
						`	may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
	--------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncat_Test_2L(  )
	{

	//Null cases

	INFO_PRINTF1(_L("Null cases"));  
	wchar_t ne[10] = L"\0";
	wchar_t *nres1 = wcsncat(ne,L"",130);
	wchar_t *nres2 = wcsncat(ne,L"\0World",12);


	if(	wcscmp(nres1,L"\0"))
		{
		return -1;
		}
	if(	wcscmp(ne,L"\0"))
		{
		return -1;
		}
	if(	wcscmp(nres2,L"\0"))
		{
		return -1;
		}
	if(	wcscmp(ne,L"\0"))
		{
		return -1;
		}

	INFO_PRINTF2(_L("Must be Null: str1[wcsncat]:%ls") ,ne);	
	INFO_PRINTF2(_L("Must be Null: Res2[wcsncat]:%ls") ,nres2);
	INFO_PRINTF2(_L("Must be Null: str2[wcsncat]:%ls") ,ne);

	
		 	 
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncat_Test_3L
	API Tested				:	wcsncat()
	TD TestCase Number		:	1_15_2_504
	TD TestCase Identifier	:	504
	TestCase Description	:	API is tested with the combination of NULL and non-NULL strings
								as input strings.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncat_Test_3L(  )
	{
            
	//Null and Non-NULL combinations

	wchar_t NonwStr[20] = L"VYKOn"; 
	wchar_t ne[10] = L"\0";
	INFO_PRINTF1(_L("Null and Non-NULL combinations")); 
	
	INFO_PRINTF2(_L("Before Null catenation:str1[wcscat]: %ls") ,NonwStr);	
	wchar_t *res1 = wcsncat(NonwStr,L"",4);
	wchar_t *res2 = wcsncat(ne,NonwStr,2);

	if ( wcscmp(NonwStr,res1))
		{
		return -1;
		}
	INFO_PRINTF2(_L("After Null catenation:str1[wcscat]: %ls") ,NonwStr);	// was L8
	INFO_PRINTF2(_L("Res2[wcscat]: %ls") ,res2); // was L8

		 
	return KErrNone;
	}
     

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncat_Test_4L
	API Tested				:	wcsncat()
	TD TestCase Number		:	1_15_2_506
	TD TestCase Identifier	:	506
	TestCase Description	:	API is tested with large input strings. "n" may take diffarent
								values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncat_Test_4L(  )
	{
	 
	// Reading from file ., Fil inputs

	INFO_PRINTF1(_L(" Test for 16-bit characters")); // Was L8
	 
	wchar_t fwcs1[15];
	wchar_t fwcs2[15];
	wchar_t fwcs3[15];
	wchar_t fwcs4[15];

	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));

	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	fgetws(fwcs1,6,fp);
	fgetws(fwcs2,4,fp);
	fgetws(fwcs3,8,fp); 
	fgetws(fwcs4,3,fp);
	fclose(fp); 	

	INFO_PRINTF2(_L("1st WC-string[wcsncat]:%ls") ,fwcs1); //Was L8
	INFO_PRINTF2(_L("2st WC-string[wcsncat]:%ls") ,fwcs2);//Was L8

	wchar_t *res3 = wcsncat(fwcs1,fwcs2,1); 

	if(wcscmp(fwcs1,res3))
		{
		INFO_PRINTF1(_L("Wcsncat  return val is n't same as Destn"));
		return -1;
		}

	INFO_PRINTF2(_L("After wcsncat(fwcs1,fwcs2,1):fwcs::%ls") ,fwcs1); //Was L8


	INFO_PRINTF2(_L("1st WC-string[fwcs3]:%ls") ,fwcs3); //Was L8
	INFO_PRINTF2(_L("2st WC-string[fwcs4]:%ls") ,fwcs4); //Was L8

	wchar_t *res4 = wcsncat(fwcs3,fwcs4,56);

	if(wcscmp(fwcs3,res4))
		{
		INFO_PRINTF1(_L("Wcsncat  return val is n't same as Destn")); //Was L8
		return -1;
		}
	INFO_PRINTF2(_L("wcsncat(fwcs3,fwcs4,56):fwcs3::%ls") ,fwcs3); //Was L8




	// Buffer concatenation..
	INFO_PRINTF1(_L("_Large Wide string cases_")); //Was L8
	wchar_t buf[][500] = { 	L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd\
						  	SKDKDASDLSLdjLADJLADJLASDJLAS$$^%&",
							L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd+ds4534~!~!~!~!~!~!12351`ds7@#$%^&&_&&*YU&&&&&&(&*&*"
							L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd"
	}; 		

	TInt size = wcslen(buf[0]); 
	wchar_t *res1 = wcsncat(buf[0],buf[1],13);
	if(wcscmp(buf[0],res1))
		{
		INFO_PRINTF1(_L("Wcsncat  return val is n't same as Destn")); //Was L8
		return -1;
		}

	if	(wcslen(buf[0]) != (unsigned int)(size + 13))
		{
		return -1;
		}
	
		 
	return KErrNone;


	}


	 

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncat_Test_5L
	API Tested				:	wcsncat()
	TD TestCase Number		:	1_15_2_521
	TD TestCase Identifier	:	521
	TestCase Description	:	THis will test the API with diffarent values of "n",
								includes maximum and minimum values of "n".	
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncat_Test_5L(  )
	{

	//Test for size_t and for the large strings
	INFO_PRINTF1(_L("Test for size_t")); //Was L8
	size_t k_max = (size_t)0xFFFFFFFF;
	size_t k_min = (size_t)0x00000000; 

	  
	wchar_t nul[20] = L"\0";
	wchar_t *res1 = wcsncat(nul,L"",k_max);

	INFO_PRINTF2(_L("Must be NULL [wcsncat]:%ls") ,res1); //Was L8
	 
	wcsncat(nul,L"",k_min);

	INFO_PRINTF2(_L("Expected Null:Res[wcsncat]:%ls") ,res1); //Was L8

	wchar_t wstr[40] = L"EDJFDJFLDJ543543 $#^#$(@)";	

	res1 = wcsncat(wstr,L"!!32314fffsa",0);

	if(wcscmp(wstr,res1))
		{
		INFO_PRINTF1(_L("Not working properly for n = 0"));
		return -1;
		}
	
	fflush(stdin);

		 
	return KErrNone;


	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscpy_Test_1L
	API Tested				:	wcscpy()
	TD TestCase Number		:	1_15_2_465
	TD TestCase Identifier	:	465
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings, 

	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscpy_Test_1L(  )
	{

	wchar_t wcs1[15] = L"Hello world";
	wchar_t wcs2[10][20] = 	{
							   L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							}; 	

	// Positive Cases
	INFO_PRINTF1(_L("Positivecases")); //Was L8
	if(wcscmp( wcscpy(wcs1,wcs1),wcs1))// self copying
		{
		return -1;
		}
	if(wcscmp( wcscpy(wcs1,L"Hello world"),L"Hello world"))
		{
		return -1;
		}
	if(wcscmp( wcscpy(wcs2[0],L"&&&%%^^@@"),L"&&&%%^^@@" ))
		{
		return -1;
		}
	if(wcscmp( wcscpy(wcs2[0],wcs2[4]),wcs2[4]))
		{
		return -1;
		}

	wcscpy(wcs2[4],L"\0%%%%U**@@");

	INFO_PRINTF2(_L("Res[Must be NULL] :%ls") ,wcs2[4]);

		 
	return KErrNone;

	}
	 


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscpy_Test_2L
	API Tested				:	Wcscpy()
	TD TestCase Number		:	1_15_2_467
	TD TestCase Identifier	:	467
	TestCase Description	:	API is testd with NULL strings as both input strings. 
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscpy_Test_2L( )
	{
	//Null Cases
	wchar_t wcs[20] = L"sfdf";
	INFO_PRINTF1(_L("NULL cases")); //Was L8


	if(wcscmp(wcscpy(wcs,L""),L""))
		{
		return -1;
		}
	if(wcscmp(wcscpy(wcs,L"\0"),L""))
		{
		return -1;
		}
	if(wcscmp(wcscpy(wcs,L"\0"),L""))
		{
		return -1;
		}

	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscpy_Test_3L
	API Tested				:	wcscpy()
	TD TestCase Number		:	1_15_2_468
	TD TestCase Identifier	:	468
	TestCase Description	:	API is tested with the combination of NULL and non-NULL strings
								as input strings.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscpy_Test_3L( )
	{

	//NULL and non-NULL combinations..
			
	wchar_t wcs2[10][20] = 	{
							   L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							};
	wchar_t nul[20] = L"";
	wchar_t nul_1[20] = L""; 

	INFO_PRINTF1(_L("NULL and non-NULL combinations")); //Was L8
	if(wcscmp(wcscpy(wcs2[4],L""),wcs2[4]))
		{
		return -1;
		}
	if(wcscmp(wcscpy(wcs2[2],L""),wcs2[2]))
		{
		return -1;
		}
	if(wcscmp(wcscpy(nul,L"@%@Y"),L"@%@Y"))
		{
		return -1;
		}
	if(wcscmp(wcscpy(nul_1,L"56dfasdask@#@44"),L"56dfasdask@#@44"))
		{
		return -1;
		}
	
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscpy_Test_4L
	API Tested				:	wcscpy()
	TD TestCase Number		:	1_15_2_710
	TD TestCase Identifier		:	710
	TestCase Description		:	This will test the API with large string as input strings	and 
							n" may take diffarent values. This will read the chars from a file 
							and populate it in to a buffer, These buffers are used for testing.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscpy_Test_4L( )
	{

	//Test for  16-bit characters //readin from file 

	INFO_PRINTF1(_L(" Test for 16-bit characters")); //Was L8
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));
	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	
	wchar_t fwcs1[15];
	wchar_t fwcs2[15];
	wchar_t fwcs3[15];
	fgetws(fwcs1,10,fp);
	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	fgetws(fwcs2,10,fp);
	fgetws(fwcs3,10,fp);  	
	fclose(fp); 	

	wchar_t n[20] =L"";
	if(wcscmp(wcscpy(fwcs1,fwcs2),fwcs2))
		{
		return -1;
		}
	if(wcscmp(wcscpy(n,fwcs3),fwcs3))
		{
		return -1;
		}
	
	return KErrNone;

	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcschr_Test_1L
	API Tested				:	Wcschr()
	TD TestCase Number		:	1_15_2_402
	TD TestCase Identifier	:	402
	TestCase Description	:	Here the 1st argument is chosen as NULL string and the seach character is
								may be any wide-character.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcschr_Test_1L( ) 
	{
	wchar_t search_char[15] = L"&&&&$%%%%%TTU";
	wchar_t *wstr;
	TInt i ; 

	/*The falling function call must returns a NULL pointer,
	since ,We can't print the NULL pointer using console->printf,.. it is 
	printed here using normal printf stmt, which prints (null)*/

	for(i = 0 ; search_char[i]; i++)
		{
		wstr = wcschr(L"", search_char[i]);
		if(wstr != NULL)
			{
			INFO_PRINTF2(_L("wcschr(): Not returning NULL for the loop:%d"),i);
			return -1;
			}
		INFO_PRINTF2(_L("{Expectd 0}%d"),wstr);
		}

	INFO_PRINTF2(_L("Res[Must be NULL str]: %s"), wcschr(L"",L'\0'));
	INFO_PRINTF2(_L("Res[Must be 0]: %d"),wcschr(L"",L'\n'));

	return KErrNone;
	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcschr_Test_2L
	API Tested				:	wcschr()
	TD TestCase Number		:	1_15_2_302
	TD TestCase Identifier	:	302
	TestCase Description	:	To test,Whether the terminating NULL character is considered as 
								a part of the Widecharacter string or not.

	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcschr_Test_2L( ) 
	{
	wchar_t search_char[20] = L"&&&&$%%%%%dfklkTTU";
	wchar_t *wstr;

	wstr = wcschr(search_char,L'\0');
	if(wcscmp(wstr,L"\0"))
		{
		INFO_PRINTF1(_L("wcschr(): Not returning NULL for the loop"));
		return -1;
		}

	INFO_PRINTF2(_L("Res_2[Must be NULL string]: %ls") ,wstr);

		 
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	wcschr_Test_3L
	API Tested				:	wcschr()
	TD TestCase Number		:	1_15_2_293
	TD TestCase Identifier	:	293
	TestCase Description	:	API is Tested for positive case, where search cahracter is chosen such tat
								it is contained in the given string.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcschr_Test_3L( ) 
	{
	wchar_t wcs1[35] = L"1234567890abcdefghijklmnopqrst";
	wchar_t *wstr;
	TInt i;


	for(i = 0 ; wcs1[i]; i++)
		{
		wstr = wcschr(wcs1, wcs1[i]);
		INFO_PRINTF2(_L("Res :%ls") ,wstr);	

		}

		 
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	wcschr_Test_4L
	API Tested				:	wcschr()
	TD TestCase Number		:	1_15_2_301
	TD TestCase Identifier	:	301
	TestCase Description	:	API is Tested for Negative case, where search cahracter is chosen such tat
								it is not contained in the given string.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcschr_Test_4L( ) 
	{
	wchar_t *c = L"2323213213123123123214353";
	wchar_t *s = L"asd@#@$@#%BCNB";
	wchar_t *wstr;
	TInt i;
	for(i = 0 ; s[i]; i++)
		{
		wstr = wcschr(c, s[i]);
		if(wstr != NULL)
			{
			INFO_PRINTF2(_L("wcschr(): Not returning NULL for the loop:%d"),i);
			return -1;
			}
		INFO_PRINTF2(_L("[Expected 0]:%d"), wstr);
		}
	
		 	
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcschr_Test_5L
	API Tested				:	Wcschr()
	TD TestCase Number		:	1_15_2_713
	TD TestCase Identifier	:	713
	TestCase Description	:	This will test the API with large string as input strings	and 
								n" may take diffarent values. This will read the chars from a file 
								and populate it in to a buffer, These buffers are used for testing.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcschr_Test_5L( ) 
	{

	TInt i;
	
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));
	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	wchar_t fwcs1[60], *wstr;
	 
	fgetws(fwcs1,50,fp);

	INFO_PRINTF2(_L("Input :%ls") ,fwcs1);
    fclose(fp);
	INFO_PRINTF1(_L("positive cases"));
    for(i = 0 ; fwcs1[i]; i++)
		{
		wstr = wcschr(fwcs1, fwcs1[i]);
		INFO_PRINTF2(_L("Res:%ls") ,wstr);	
		}   


	INFO_PRINTF1(_L("Negative cases")); //Was L8
    wchar_t ch = (wchar_t)0xffff;
	for(i=0 ;i < 5 ; i ++)
		{
		wstr = wcschr(fwcs1, ch);
		if(wstr != NULL)
			{
			INFO_PRINTF2(_L("wcschr(): Not returning NULL for the loop:%d"),i);
			return -1;
			}
		INFO_PRINTF2(_L("[Expected 0]:%d"),wstr);
		}   
		
		 	
	return KErrNone;
	}


//WCsrchr()..
/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsrchr_Test_1L
	API Tested				:	Wcsrchr()
	TD TestCase Number		:	1_15_2_535
	TD TestCase Identifier	:	535
	TestCase Description	:	Here the 1st argument is chosen as NULL string and the seach character is
								may be any wide-character.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcsrchr_Test_1L( ) 
	{
	wchar_t search_char[15] = L"&&&&$%%%%%TTU";
	wchar_t *wstr;
    TInt i ; 
	
	/*The falling function call must returns a NULL pointer,
	since ,We can't print the NULL pointer using console->printf,.. it is 
	printed here using normal printf stmt, which prints (null)*/
	
	for(i = 0 ; search_char[i]; i++)
		{
		wstr = wcsrchr(L"", search_char[i]);
		if(wstr != NULL)
			{
			INFO_PRINTF2(_L("wcschr(): Not returning NULL for the loop:%d"),i);
			return -1;
			}
		INFO_PRINTF2(_L("{Expected:(0)}%d"),wstr);
		}


	INFO_PRINTF2(_L("Res[Expected:(0)]:%d"), wcsrchr(L"",L'\0'));
	INFO_PRINTF2(_L(" [Expected:(0)]:%d"),wcsrchr(L"",L'\n')); //Both NULLS
	
		 
	return KErrNone;
	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsrchr_Test_2L
	API Tested				:	Wcsrchr()
	TD TestCase Number		:	1_15_2_537
	TD TestCase Identifier	:	537
	TestCase Description	:	To test,Whether the terminating NULL character is considered as 
								a part of the Widecharacter string or not.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsrchr_Test_2L( ) 
	{
	wchar_t search_char[20] = L"&&&&$%%%%%dfklkTTU";
	wchar_t *wstr;


	wstr = wcsrchr(search_char,L'\0');
	if(wcscmp(wstr,L""))
		{
		INFO_PRINTF1(_L("wcschr(): Not returning NULL"));
		return -1;
		}
	INFO_PRINTF2(_L("Res_1[Must be NULL string]: %ls") ,wstr);	
	wstr = wcsrchr(search_char,L'\0');
	if(wcscmp(wstr,L""))
		{
		INFO_PRINTF1(_L("wcsrchr(): Not returning NULL"));
		return -1;
		}
	INFO_PRINTF2(_L("Res_2[Must be NULL string]: %ls") ,wstr);

		 
	return KErrNone;
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsrchr_Test_3L
	API Tested				:	Wcsrchr()
	TD TestCase Number		:	1_15_2_539
	TD TestCase Identifier	:	539
	TestCase Description	:	API is Tested for positive case, where search cahracter is chosen such tat
								it is contained in the given string.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsrchr_Test_3L( ) 
	{
	wchar_t wcs1[50] = L"1234567890abcdefghijklmnopqrst1QQ2FF3GG3HH4NN";
	wchar_t *wstr;
	TInt i;

  	for(i = 0; wcs1[i]; i++)
		{
		wstr = wcsrchr(wcs1, wcs1[i]);
		INFO_PRINTF2(_L("Res :%ls") ,wstr);	
		}
	
	wstr = wcsrchr(wcs1, wcs1[0]);
	if(wcsncmp(wstr,L"1QQ2FF3",5))
		{
		return -1;
		}
	
		    
    return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsrchr_Test_4L
	API Tested				:	Wcsrchr()
	TD TestCase Number		:	1_15_2_538
	TD TestCase Identifier	:	538
	TestCase Description	:	API is Tested for Negative case, where search character is chosen such tat
								it is not contained in the given string.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsrchr_Test_4L( ) 
	{
	wchar_t *c = L"2323213213123123123214353";
	wchar_t *s = L"asd@#@$@#%BCNB";
	wchar_t *wstr;
	TInt i;
	for(i = 0 ; s[i]; i++)
		{
		wstr = wcsrchr(c, s[i]);
		if(wstr != NULL)
			{
			INFO_PRINTF2(_L("wcsrchr(): Not returning NULL for the loop:%d"),i);
			return -1;
			}
		INFO_PRINTF2(_L("[Expected 0] :%d"), wstr);
		}
	return KErrNone;
	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncmp_Test_5L
	API Tested				:	wcsncmp()
	TD TestCase Number		:	1_15_2_540
	TD TestCase Identifier	:	540
	TestCase Description	:	This will test the API with large string as input strings	and 
								n" may take diffarent values. This will read the chars from a file 
								and populate it in to a buffer, These buffers are used for testing.
								"n" may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsrchr_Test_5L( ) 
	{
	TInt i;
	
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));

	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
		
	wchar_t fwcs1[60], *wstr;
	 
	fgetws(fwcs1,50,fp);

	INFO_PRINTF2(_L("Res_2[] :%ls") ,fwcs1);
    
	INFO_PRINTF1(_L("Negative cases"));
    fclose(fp);
	for(i = 0 ; fwcs1[i]; i++)
		{
		wstr = wcsrchr(fwcs1, fwcs1[i]);
		INFO_PRINTF2(_L("Res :%ls") ,wstr);	
		}   
	
	INFO_PRINTF1(_L("Negative cases"));

    wchar_t ch = (wchar_t)0xffff;
	for(i=0 ; i < 5 ; i ++)
		{
		wstr = wcsrchr(fwcs1, ch);
		if(wstr != NULL)
			{
			INFO_PRINTF2(_L("wcsrchr(): Not returning NULL for the loop:%d"),i);
			return -1;
			}
		INFO_PRINTF2(_L("[Expected 0]%d "),wstr);
		}
	
		 	
	return KErrNone;
	}

// wcsspn Tests



/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsspn_Test_1L
	API Tested				:	wcsspn()
	TD TestCase Number		:	1_15_2_489
	TD TestCase Identifier	:	489
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings, Will test for both the positive and negitive 
								cases.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsspn_Test_1L( ) 
	{
	
	wchar_t wcs1[30] = L"1234567890ABCDE!@#$$";
	
	wchar_t *wcs2[20] = { L"abcdefghijkl",	
						  L":::?>_)+(|{>",
						  L"~*(&IUIJJJ",
						  L"1234567",
						  L":::?>",
						  L"1",
						  L"as1sd2ds3ds48f5fd"
						};

	//Negative cases..
	for(TInt i = 0; i < 3 ; i ++)
		{
		INFO_PRINTF2(_L("{Must be Zero} : %d "), wcsspn(wcs1,wcs2[i]));
		}
	
		
	//Positive cases..
	if(wcslen(wcs1) != wcsspn(wcs1,wcs1))//self WC strings
		{
		return -1;
		}
	INFO_PRINTF2(_L(" {Must be  eq to 7 } : %d "),wcsspn(wcs1,wcs2[3]));
	INFO_PRINTF2(_L(" {Must be  eq to 1 } : %d "),wcsspn(wcs1,wcs2[5]));
    INFO_PRINTF2(_L(" {Must be  eq to 5 } : %d "),wcsspn(wcs2[1],wcs2[4]));
	INFO_PRINTF2(_L(" {Must be  eq to 5 } : %d "),wcsspn(wcs1,wcs2[6])); // May be in ranndom..
	
		 
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsspn_Test_2L
	API Tested				:	wcsspn()
	TD TestCase Number		:	1_15_2_490
	TD TestCase Identifier	:	490
	TestCase Description	:	API is testd with NULL strings as both input strings. here "n"
							`	may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt  CTestLibcwchar:: Wcsspn_Test_2L( ) 
	{
	
	// both NULL cases..
	INFO_PRINTF2(_L("{Must be Zero} : %d "), wcsspn(L"",L""));
	INFO_PRINTF2(_L("{Must be Zero} : %d "), wcsspn(L"",L"\0"));
	 	
	return KErrNone;
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsspn_Test_3L
	API Tested				:	wcsspn()
	TD TestCase Number		:	1_15_2_491
	TD TestCase Identifier	:	491
	TestCase Description	:	API is testd with combination of NULL and NOn-NULL strings as input strings.
								here "n" may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
	
TInt  CTestLibcwchar:: Wcsspn_Test_3L( )
	{
	wchar_t wcs1[30] = L"1234567890ABCDE!@#$$";

	//NULL & Non-NULL cases..
	INFO_PRINTF2(_L("{Must be Zero} : %d "), wcsspn(wcs1,L""));
	INFO_PRINTF2(_L("{Must be Zero} : %d "), wcsspn(L"\0",wcs1));
		 	
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	wcsspn_Test_4L
	API Tested				:	wcsspn()
	TD TestCase Number		:	1_15_2_497
	TD TestCase Identifier	:	497
	TestCase Description	:	This will test the API with large string as input strings	and 
								n" may take diffarent values. This will read the chars from a file 
								and populate it in to a buffer, These buffers are used for testing.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
// Test with Hindi characters
TInt CTestLibcwchar::Wcsspn_Test_4L( ) 
	{
	
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));
	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	 
	wchar_t fwcs1[60];
	 
	fgetws(fwcs1,50,fp);
	fclose(fp);

	INFO_PRINTF2(_L("{Must be 49 } %d"), wcsspn(fwcs1,fwcs1) );
	
	wchar_t *test_str =  fwcs1;
	test_str[37] = L'\0';

	INFO_PRINTF2(_L("{Must be 37 } %d"), wcsspn(fwcs1,test_str) );
	
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcspbrk_Test_1L
	API Tested				:	Wcspbrk()
	TD TestCase Number		:	1_15_2_530
	TD TestCase Identifier	:	515
	TestCase Description	:	API is tested with the combination of NULL and non-NULL strings
								as input strings.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt  CTestLibcwchar::Wcspbrk_Test_1L( )
	{
	
	wchar_t *wcs[50] = {  L"abcdefghijkl",	
						  L":::?>_)+(|{>",
						  L"~*(&IUIJJJ",
						  L"1234567",
						  L":::?>",
						  L"1",
						  L"as1sd2ds3ds48f5fd"
						  };
	
	wchar_t *res1 = wcspbrk(wcs[0], L"");
	if(res1 != NULL)
		{
			INFO_PRINTF1(_L("T1:wcsbrk: Not returning NULL"));
			return -1;
		}

	
	res1 = wcspbrk(wcs[2], L"\0");
	if(res1 != NULL)
		{
			INFO_PRINTF1(_L("T2:wcsbrk: Not returning NULL"));
			return -1;
		}

     // WS1 as NULL
	INFO_PRINTF2(_L("[Expected 0] %d"),  wcspbrk( L"",wcs[1])); 
	INFO_PRINTF2(_L("[Expected 0] %d") , wcspbrk( L"",L"c"));

	// Both as NULL
	INFO_PRINTF2(_L("[Expected 0] %d") , wcspbrk( L"",L""));
	 
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcspbrk_Test_2L
	API Tested				:	Wcspbrk()
	TD TestCase Number		:	1_15_2_532
	TD TestCase Identifier	:	532
	TestCase Description	:	Test that, The function returning pointer to wide-character code[Which is a part of the WS2 in WS1], 
								in case if there is an occurrence in the wide-character string pointed to by ws1 of any 
								wide-character code from the wide-character string pointed to by ws2.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt  CTestLibcwchar::Wcspbrk_Test_2L( )
	{

	wchar_t *wcs[50] = {  L"abcdefghijkl",	
						  L":::?>_)+(|{>",
						  L"~*(&IUIJJJ",
						  L"1234567",
						  L":::?>",
						  L"1",
						  L"as1sd2ds3ds48f5fd"
						  };

	wchar_t *res1 = wcspbrk(wcs[0], wcs[6]);
	INFO_PRINTF2(_L("Result:%ls"),res1);

	res1 = wcspbrk(wcs[1], wcs[4]);
	INFO_PRINTF2(_L("Result:%ls"),res1);

	res1 = wcspbrk(wcs[3], wcs[6]);
	INFO_PRINTF2(_L("Result: %ls"),res1);

	res1 = wcspbrk(wcs[6], wcs[5]);
	INFO_PRINTF2(_L("Result: %ls"),res1);

	res1 = wcspbrk(wcs[2], L"*");
	INFO_PRINTF2(_L("Result:%ls"),res1);

		 
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcspbrk_Test_3L
	API Tested				:	Wcspbrk()
	TD TestCase Number		:	1_15_2_531
	TD TestCase Identifier	:	531
	TestCase Description	:	Test that,The function returning NULL pointer, 
								in case if there is no occurrence in the wide-character string 
								pointed to by ws1 of any wide-character code from the wide-character string pointed to by ws2.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt  CTestLibcwchar::Wcspbrk_Test_3L( )
	{

	wchar_t *wcs[50] = { L"abcdefghijkl",	
						  L":::?>_)+(|{>",
						  L"~*(&IUIJJJ",
						  L"1234567",
						  L":::?>",
						  L"1",
						  L"as1sd2ds3ds48f5fd"
						  };

	wchar_t *res1 = wcspbrk(wcs[0], wcs[1]);
	INFO_PRINTF2(_L(" [Expected 0] %d "), res1);
	
	res1 = wcspbrk(wcs[0], wcs[2]);
	INFO_PRINTF2(_L(" [Expected 0] %d "), res1);

	res1 = wcspbrk(wcs[3], wcs[0]);
	INFO_PRINTF2(_L(" [Expected 0] %d "), res1);
	
	
		 
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcspbrk_Test_4L
	API Tested				:	Wcspbrk()
	TD TestCase Number		:	1_15_2_533
	TD TestCase Identifier	:	533
	TestCase Description	:	This will test the API with large string as input strings	and 
								n" may take diffarent values. This will read the chars from a file 
								and populate it in to a buffer, These buffers are used for testing.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
//Test for 16-bit char string [HINDI]
TInt CTestLibcwchar::Wcspbrk_Test_4L( ) 
	{
	
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));	
	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
		 
	wchar_t fwcs1[60];
	 
	fgetws(fwcs1,50,fp);
	fclose(fp);

	INFO_PRINTF2(_L("Res_2: %ls") ,fwcs1);
    
	wchar_t *res = wcspbrk(fwcs1,fwcs1+2);
	INFO_PRINTF2(_L("Result1: %ls"),res);
	
		 
	return KErrNone;
	}
 


// Wcscoll Tests ..
/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscoll_Test_1L
	API Tested				:	Wcscoll()
	TD TestCase Number		:	1_15_2_472
	TD TestCase Identifier	:	472
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings,
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscoll_Test_1L(  )
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
	INFO_PRINTF2(_L(" %d"),wcscoll(wcs1,wcs1));		//self comparision
	INFO_PRINTF2(_L(" %d"),wcscoll(wcs1,L"Hello world"));      
	INFO_PRINTF2(_L(" %d"),wcscoll(L"&&&%%^^@@",L"&&&%%^^@@"));
	INFO_PRINTF2(_L(" %d"),wcscoll(wcs2[1],wcs2[1])); 
	INFO_PRINTF2(_L(" %d"),wcscoll(wcs2[4]+5,L"%%%%U**@@"));
	
	 
	// Negative Cases
	INFO_PRINTF1(_L("Negative cases"));
	
	if(	wcscoll(wcs1,wcs2[4]) != wcs1[0]-wcs2[4][0])
		{
		return -1;
		}
	if(	wcscoll(wcs1,L"%%%%%%%")!= wcs1[0]-L'%')
		{
		return -1;
		}
	if(	wcscoll(wcs2[4],L"@%@Y") != wcs2[4][0]-L'@')
		{
		return -1;
		}
	if(	wcscoll(wcs2[4],L"Joik%%%k%U**@@") != wcs2[4][7]-L'k')
		{
		return -1;
		}
	if( wcscoll(L"sasdhkllllllllk",L"sasdhkllllllllm") != L'k'-L'm')
		{
		return -1;
		}
	
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscoll_Test_2L
	API Tested				:	wcscoll()
	TD TestCase Number		:	1_15_2_473
	TD TestCase Identifier	:	473
	TestCase Description	:	API is testd with NULL strings as both input strings. here "n"
							`	may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscoll_Test_2L(  )
	{

		//Null Cases
	INFO_PRINTF1(_L("NULL cases"));
	if(wcscoll(L"",L""))
		{
		return -1;
		}
	if(wcscoll(L"\0",L""))
		{
		return -1;
		}
	if(wcscoll(L"\0",L"\0"))
		{
		return -1;
		}
	return KErrNone;
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscoll_Test_3L
	API Tested				:	wcscoll()
	TD TestCase Number		:	1_15_2_474
	TD TestCase Identifier	:	474
	TestCase Description	:	API is testd with NULL and NOn-NULL combinations input strings. here "n"
							`	may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcscoll_Test_3L(  )
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
	if(	wcscoll(wcs2[4],L"") != wcs2[4][0])
		{
		return -1;
		}
	if(	wcscoll(wcs2[4]+5,L"") != wcs2[4][5])
		{
		return -1;
		}
	if(	wcscoll(L"",L"@%@Y")!= -L'@')
		{
		return -1;
		}
	if(	wcscoll(L"",L"56dfasdask@#@44") != -L'5')
		{
		return -1;
		}
	if(	wcscoll(L"",L"^") != -L'^')
		{
		return -1;
		}

	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscoll_Test_3L
	API Tested				:	wcscoll()
	TD TestCase Number		:	1_15_2_709
	TD TestCase Identifier	:	709
	TestCase Description	:	This will test the API with large string as input strings	and 
								n" may take diffarent values. This will read the chars from a file 
								and populate it in to a buffer, These buffers are used for testing.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscoll_Test_4L(  )
	{

		//Test for  the large strings
	INFO_PRINTF1(_L("Large string Tests"));
	wchar_t buf[][250] = { L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd",
						L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd+ds4534~!~!~!~!~!~!12351`ds7@#$%^&&_&&*YU&&&&&&(&*&*"
						L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd"
						}; 	


	//INFO_PRINTF2(_L("Must be Zero: %d" ,wcscmp(buf[0],buf[2])); //Was L8
	if(	wcscoll(L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd",L"A434739847324392%$^%$^$$^}QQ{{|{Q|{|mcxvc cxv cxv%%%%lsdsd"))
		{
		return -1;
		}
	if(	wcscoll(buf[0],buf[1]) != -L'+')
		{
		return -1;
		}

	// Test for  16-bit characters //readin from file 
	INFO_PRINTF1(_L(" Test for 16-bit characters"));
	
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));
	FILE *fp = fopen(filename,"r");

	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	wchar_t fwcs1[15];
	wchar_t fwcs2[15];
	wchar_t fwcs3[15];
	fgetws(fwcs1,10,fp);
	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	fgetws(fwcs2,10,fp);
	fgetws(fwcs3,10,fp);  	
	fclose(fp); 	

	INFO_PRINTF2(_L("1st WC-string[wcscoll]") ,fwcs1);
	INFO_PRINTF2(_L("2st WC-string[wcscoll]") ,fwcs2);

	if(	wcscoll(fwcs1,fwcs2))
		{
		return -1;
		}
	if(	wcscoll(fwcs2,fwcs3) != fwcs2[0]-fwcs3[0])
		{
		return -1;
		}
	return KErrNone;
	}



// wcscspn Tests..

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscspn_Test_1L
	API Tested				:	Wcscspn()
	TD TestCase Number		:	1_15_2_483
	TD TestCase Identifier	:	483
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings,
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcscspn_Test_1L( ) 
	{
	wchar_t wcs1[30] = L"1234567890ABCDE!@#$$";
	
	wchar_t *wcs2[20] = { L"abcdefghijkl",	
						  L":::?>_)+(|{>",
						  L"~*(&IUIJJJ",
						  L"1234567",
						  L":::?>",
						  L"1",
						  L"as1sd2ds3ds48f5fd"
						};

	//Negative cases..
	TInt i;
	for( i = 0; i < 3 ; i ++)
		{
		if( wcslen(wcs1) != wcscspn(wcs1,wcs2[i]))
			{
			INFO_PRINTF2(_L(" Failed in Loop :%d "),i);
			return -1;
			}
		}
	
	//Negative cases..
	INFO_PRINTF2(_L(" {Must be  eq to 0} : %d "),wcscspn(wcs1,wcs1)); // self WC strings
	INFO_PRINTF2(_L(" {Must be  eq to 0} : %d "),wcscspn(wcs1,wcs2[3]));
	INFO_PRINTF2(_L(" {Must be  eq to 0} : %d "),wcscspn(wcs1,wcs2[5]));
    INFO_PRINTF2(_L(" {Must be  eq to 0} : %d "),wcscspn(wcs2[1],wcs2[4]));
	INFO_PRINTF2(_L(" {Must be  eq to 0} : %d "),wcscspn(wcs1,wcs2[6])); // May be in random..

    	
	wchar_t *str;
	for( i = 0; i<5; i ++)
		{
		str = (wchar_t*)wcs1+i;
		if( i != (int)wcscspn(wcs1,str))
			{
			INFO_PRINTF2(_L(" Failed in Loop :%d "),i);
			return -1;
			}
		}
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscspn_Test_2L
	API Tested				:	Wcscspn()
	TD TestCase Number		:	1_15_2_485
	TD TestCase Identifier		:	485
	TestCase Description		:	API is tested with the NULL strings both
							as input strings.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt  CTestLibcwchar:: Wcscspn_Test_2L( ) 
	{

	// both NULL cases..
	
	if(wcscspn(L"",L""))
		{
		return -1;
		}
	INFO_PRINTF2(_L("{Must be Zero} : %d "), wcscspn(L"",L""));
	INFO_PRINTF2(_L("{Must be Zero} : %d "), wcscspn(L"",L"\0"));
	
	return KErrNone;

	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscspn_Test_3L
	API Tested				:	Wcscspn()
	TD TestCase Number		:	1_15_2_486
	TD TestCase Identifier		:	486
	TestCase Description		:	API is tested with the combination of NULL and non-NULL strings
							as input strings.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
	
TInt  CTestLibcwchar:: Wcscspn_Test_3L( ) 
	{
	wchar_t wcs1[30] = L"1234567890ABCDE!@#$$";
	//NULL & Non-NULL cases..
	INFO_PRINTF3(_L("{Expected : %d}  : %d "), wcslen(wcs1),wcscspn(wcs1,L""));
	INFO_PRINTF2(_L("{Expected :Zero} : %d "), wcscspn(L"\0",wcs1));
	
	return KErrNone;

	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcscspn_Test_4L
	API Tested				:	Wcscspn()
	TD TestCase Number		:	1_15_2_711
	TD TestCase Identifier	:	711
	TestCase Description	:	This will test the API with large string as input strings	and 
								n" may take diffarent values. This will read the chars from a file 
								and populate it in to a buffer, These buffers are used for testing.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
// Test with Hindi characters
TInt CTestLibcwchar::Wcscspn_Test_4L( ) 
	{
	
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));

	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	 
	wchar_t fwcs1[60];
	 
	fgetws(fwcs1,50,fp);
	fclose(fp);

	INFO_PRINTF2(_L("Res_2[Must be NULL string]: %ls") ,fwcs1);
    
	
	INFO_PRINTF2(_L("{Must be 0 } %d"), wcscspn(fwcs1,fwcs1));
	
	
	if( wcscspn(fwcs1,L"sdasd%%%") != wcslen(fwcs1) )
		{
		return -1;
		}
	if(	wcscspn(L"$$#^%%%",fwcs1)  != wcslen(L"$$#^%%%"))
		{
		return -1;
		}
	if( wcscspn(fwcs1,L"123@11")   != wcslen(fwcs1))
		{
		return -1;
		}
	return KErrNone;
	}

// WCSNCPY TESTS
/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncpy_Test_1L
	API Tested				:	Wcsncpy()
	TD TestCase Number		:	1_15_2_656
	TD TestCase Identifier	:	656
	TestCase Description	:	This will test the API by giving common wide-character strings 
								as input strings, and "n" may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncpy_Test_1L(  )
	{
	
	wchar_t wcs1[15] = L"Hello world";
	wchar_t wcs2[10][20] = 	{
							   L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							}; 	

	// Positive Cases
	INFO_PRINTF1(_L("Positivecases"));
	if(	wcscmp ( wcsncpy(wcs1,wcs1,wcslen(wcs1)),wcs1))//self copying
		{
		return -1;
		}
	if(	wcscmp ( wcsncpy(wcs1,L"Hello world",wcslen(L"Hello world")),L"Hello world"))
		{
		return -1;
		}

	wchar_t* res1 = wcsncpy(wcs2[0],L"&&&%%^^@@",123 );

	INFO_PRINTF2(_L("{Expected 0} %d "), wcscmp(res1,wcs2[0])); 

	wchar_t *res2 = wcsncpy(wcs1,wcs2[4],4);

	INFO_PRINTF2(_L("{Expected 0} %d "),wcscmp(res2,wcs1));

	INFO_PRINTF2(_L("Res2[wcsncpy]:%ls") ,wcs1);   //  Behaviour is similiar to Posix API

	wcsncpy(wcs2[4],L"\0%%%%U**@@",111);

	INFO_PRINTF2(_L("Res [Must be NULL]: %ls") ,wcs2[4]);

 	/* This is to test whether there , NULL's are
	appending to the source string if the length of
	the ws2 is smaller than given value of "n"*/
	//Step 3 in Test Director

	TInt i;
	wcsncpy(wcs2[0],L"HI",7);
	for(i=2;i<7;i++)
		{
		INFO_PRINTF2(_L("Must be Null %ls"),wcs2[0]+i);
		}
	
	for( i=7;i<11;i++)
		{
		INFO_PRINTF2(_L("May n't be Null %ls"),wcs2[0]+i); //Was L8
		}	
	return KErrNone;

	}
	 

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncpy_Test_2L
	API Tested				:	wcsncpy()
	TD TestCase Number		:	1_15_2_657
	TD TestCase Identifier	:	657
	TestCase Description	:	API is testd with NULL strings as both input strings. here "n"
							`	may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncpy_Test_2L( )
	{
	//Null Cases
	wchar_t wcs[20] = L"sfdf";
	INFO_PRINTF1(_L("NULL cases"));


	if(wcscmp(wcsncpy(wcs,L"",1),L""))
		{
		return -1;
		}
	if(wcscmp(wcsncpy(wcs,L"\0",4),L""))
		{
		return -1;
		}

	wchar_t *n_str = L"XXXX";
	INFO_PRINTF2(_L("Should n't be Zero :%d"),wcscmp(wcsncpy(n_str,L"\0",0),L""));

		 
	return KErrNone;
	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncpy_Test_3L
	API Tested				:	Wcsncpy()
	TD TestCase Number		:	1_15_2_658
	TD TestCase Identifier	:	658
	TestCase Description	:	API is tested with the combination of NULL and non-NULL strings
								as input strings and "n" may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncpy_Test_3L( )
	{

	//NULL and non-NULL combinations..
							
	wchar_t wcs2[10][20] = {
							   L"H",
							   L"Hello",
							   L"Hella yaar",
							   L"Hoi wala",
							   L"Joik%%%%%U**@@"
							};
	wchar_t nul[20] = L"";
	wchar_t *str  = L"Hellp";

	INFO_PRINTF1(_L("NULL and non-NULL combinations"));
	INFO_PRINTF2(_L("must equal to zero %d"),wcscmp(wcsncpy(wcs2[4],L"",10),L""));
	INFO_PRINTF2(_L("must zero[ n=0 case] %d"),wcscmp(wcsncpy(str,L"",0),str));
	INFO_PRINTF2(_L("must equal to zero %d"),wcscmp(wcsncpy(wcs2[2],L"",1),L""));
	

	wcsncpy(nul,L"@%@Y",1);
	INFO_PRINTF2(_L("nul:[wcsncpy]: %ls") ,nul);


	wchar_t ws[4] = L"Hi0"; 

	/* Application must ensure there is an enough room in 
	destination string to 	 copy the source*/
	wcsncpy(ws,L"sdfksaj@&(@&$@",3);
	INFO_PRINTF2(_L("Res=[wcsncpy]:%ls") ,ws);
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncpy_Test_4L
	API Tested				:	Wcsncpy()
	TD TestCase Number		:	1_15_2_660
	TD TestCase Identifier	:	660
	TestCase Description	:	This will test the API with large string as input strings	and 
								n" may take diffarent values. This will read the chars from a file 
								and populate it in to a buffer, These buffers are used for testing.
								"n" may take diffarent values.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsncpy_Test_4L( )
	{

	//Test for  16-bit characters //reading from file 

	INFO_PRINTF1(_L("Test for 16-bit characters")); //Was L8
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));

	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	wchar_t fwcs1[15];
	wchar_t fwcs2[15];
	wchar_t fwcs3[15];
	fgetws(fwcs1,10,fp);
	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	fgetws(fwcs2,10,fp);
	fgetws(fwcs3,25,fp);  	
	fclose(fp); 	

	wchar_t n[20] = L"";
	INFO_PRINTF2(_L("[Must be ZeRO] %d "), wcscmp(wcsncpy(fwcs1,fwcs2,10),fwcs2)); 

	wchar_t *res = wcsncpy(n,fwcs3,20);

	if(wcscmp(res,n))
		{
		return -1;
		}

//	INFO_PRINTF2(_L("Res[wcsncpy]: %ls") ,n); //Was L8
	//after wcslen() has been tested ..
	
	INFO_PRINTF2(_L("fwcs3") ,fwcs3);

	wchar_t nul[4] = L""; 
	wcsncpy(nul,fwcs3,2);

//	INFO_PRINTF2(_L("Resultant of wcsncpy(nul,fwcs3,2)::%ls") ,nul);
	return KErrNone;

	}



/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsncpy_Test_5L
	API Tested				:	Wcsncpy()
	TD TestCase Number		:	1_15_2_662
	TD TestCase Identifier	:	662
	TestCase Description	:	This will test the API for the range of values of N, Here API is tested 
								with the maximum and minimum values of N.	
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */

TInt CTestLibcwchar::Wcsncpy_Test_5L( )
	{
	INFO_PRINTF1(_L("Test for size_t"));
	//size_t k_max = (size_t)0xFFFFFFFF;
	size_t k_min = (size_t)0x00000000; 


	wchar_t nul[20] = L"\0";

	/*Was Crashing.. Even In Linux , gives Seg fault 
	wchar_t *res1 = wcsncpy(nul,L"",655);
	INFO_PRINTF2(_L( _L("Must be NULL [wcsncpy]") ,res1); //Was L8
	*/
	wchar_t *res2 =  wcsncpy(nul,L"",k_min);

	INFO_PRINTF2(_L("Expected NULL[wcsncpy] :%ls") ,res2);

	wchar_t wstr[40] = L"EDJFDJFLDJ543543 $#^#$(@)";	

	res2 = wcsncpy(wstr,L"!!32314fffsa",0);

	//Shouldn't cpy anything to the destination..
	if(wcscmp(wstr,res2))
		{
		return -1;
		}
	return KErrNone;
	}


//wcsstr test..
/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsstr_Test_1L
	API Tested				:	Wcsstr()
	TD TestCase Number		:	1_15_2_663
	TD TestCase Identifier	:	663
	TestCase Description	:	To test that- The function returning WS1 in case if WS2 is NULL.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsstr_Test_1L( )
	{
	wchar_t *buf[20] = { L"fkhsdf%%38",
						 L"rtti123123",
	};

	wchar_t *str  = wcsstr(buf[0],L"");
	wchar_t *str1 = wcsstr(buf[1],L"\0");

	INFO_PRINTF2(_L("[Expected:0]%d"),wcscmp(str,buf[0]));
	INFO_PRINTF2(_L("[Expected:0]%d"),wcscmp(str1,buf[1]));
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsstr_Test_2L
	API Tested				:	Wcsstr()
	TD TestCase Number		:	1_15_2_664
	TD TestCase Identifier	:	664
	TestCase Description	:	Test that,The function returning  pointer to the first occurence 
								of the substring ws2 in the string WS1,   in case if there is an 
								occurrence in the wide-character string pointed to by ws1 of any 
								wide-character code from the wide-character string pointed to by ws2.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsstr_Test_2L( )
	{
	wchar_t *res,w_char[40] = L"LFDJSLLSKJLSJLAKJ(*&#(@!~~@!()( #@";
	TInt i;

	for(i=0; w_char[i];i++)
		{
		res = wcsstr(w_char,w_char+i);
		INFO_PRINTF2(_L("Res Array[wcsstr]:: %ls"),res);
		}

	res = wcsstr(w_char , L"J(*");
	INFO_PRINTF2(_L("Res2[wcsstr]:%ls"),res);
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsstr_Test_3L
	API Tested				:	Wcsstr()
	TD TestCase Number		:	1_15_2_665
	TD TestCase Identifier	:	665
	TestCase Description	:	Test that,The function returning NULL pointer, in case if there
								is no occurrence in the wide-character string pointed to by ws1
								of any wide-character code from the wide-character string pointed 
								to by ws2.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsstr_Test_3L( )
	{
	TInt i;
	wchar_t w_char[40]		=	L"LFDJSLLSKJLSJLAKJ(*&#(@!~~@!()( #@";
	wchar_t *w_char2[30]		=	{	L"343",
								L"oipoipi",
								L"\b\t\a",
								L"YYYYYYY",
								};
	for(i=0 ;i  <4 ; i++)
		{
		INFO_PRINTF2(_L("[Expected 0] %d"), wcsstr(w_char,w_char2[i]));
		}
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcsstr_Test_4L
	API Tested				:	Wcsstr()
	TD TestCase Number		:	1_15_2_667
	TD TestCase Identifier	:	667
	TestCase Description	:	This will test the API with large string as input strings. This will
								read the chars from a file and populate it in to a buffer, These buffers 
								are used for testing.
	<Source File>			:	<MRT_WCHARSTR.CPP>
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcsstr_Test_4L( )
	{
	//Test for  16-bit characters //readin from file 

	INFO_PRINTF1(_L(" Test for 16-bit characters"));
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));
	
	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	wchar_t fwcs1[15];
	wchar_t fwcs2[15];
	wchar_t fwcs3[30];
	fgetws(fwcs1,10,fp);
	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	fgetws(fwcs2,10,fp);
	fgetws(fwcs3,25,fp);  	
	fclose(fp); 	

	  
	wchar_t *res = wcsstr(fwcs1,fwcs2);
	// Since both are same
	if(wcscmp(res,fwcs1))
		{
		return -1;
		}

	INFO_PRINTF2(_L("Input String:: %ls") ,fwcs1);

	res = wcsstr(fwcs1,fwcs1+2);

	INFO_PRINTF2(_L("Res [Should n't be NULL]: %ls") ,res);

	INFO_PRINTF2(_L("fwcs2:%ls") ,fwcs2 + 8);
	INFO_PRINTF2(_L("fwcs3:%ls") ,fwcs3);


	res	=	wcsstr(fwcs3,fwcs3 + 3);

	INFO_PRINTF2(_L("Resultant of wcsstr(fwcs3,fwcs2 + 8): %ls") ,res);
	return KErrNone;
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstok1L
	API Tested				:	Wcstok()
	TD TestCase Number		:	1_15_2_2310
	TD TestCase Identifier	:	2310
	TestCase Description	:	test where the seperator contains all the characters present in the input text.
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcstok1L(  )
	{	 
    
    INFO_PRINTF1(_L("started Wcstok 1")); //Was L8
 	const wchar_t *seps = L"onetwhr";
    wchar_t *last, *tok, text[] = L"onetwothree";
               
	tok = wcstok(text, seps, &last);
    if(tok == NULL)
		{
       	return KErrNone;
		}
    else
		{
    	return KErrGeneral;
		}
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstok2L
	API Tested				:	Wcstok()
	TD TestCase Number		:	1_15_2_2311
	TD TestCase Identifier	:	2311	
	TestCase Description	:	test where the seperator contains no characters which are present in the input text.
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcstok2L(  )
	{	 
    
    INFO_PRINTF1(_L("started Wcstok 2")); //Was L8
 	const wchar_t *seps = L"abcd";
    wchar_t *last, *tok1, *tok2, text[] = L"efghijk";
                    
    tok1 = wcstok(text, seps, &last);
    tok2 = wcstok(NULL, seps, &last);
    if(!(wcscmp (tok1, text)) && (tok2 == NULL))
		{
    	INFO_PRINTF3(_L("token generated : %ls and input text: %ls are same"), tok1, text);
        return KErrNone;
		}
    return KErrGeneral;                
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstok3L
	API Tested				:	Wcstok()
	TD TestCase Number		:	1_15_2_2312
	TD TestCase Identifier	:	2312
	TestCase Description	:	test where the input string contains the characters present in the seperator string.
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcstok3L(  )
	{	 
    
    INFO_PRINTF1(_L("started Wcstok 3")); //Was L8
 	const wchar_t *seps = L"sv";
    wchar_t *last, *tok1,*tok2,*tok3,text[] = L"onestwovthree", one[]=L"one", two[]=L"two", three[] = L"three";

    tok1 = wcstok(text, seps, &last);
    tok2 = wcstok(NULL, seps, &last);
    tok3 = wcstok(NULL, seps, &last);
               
                          
    if(!(wcscmp (tok1, one)) && !(wcscmp (tok2, two)) && !(wcscmp (tok3, three)))
		{
    	INFO_PRINTF1(_L("tokens generated are one, two & three from the input string onestwovthree and seperator string sv"));
        return KErrNone;
		}
   	else
   		{
  		return KErrGeneral;   		
   		}
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstok4L
	API Tested				:	Wcstok()
	TD TestCase Number		:	1_15_2_2313
	TD TestCase Identifier	:	2313
	TestCase Description	:	test where the different seperator strings are there in subsequent calls.
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcstok4L(  )
	{
               
	INFO_PRINTF1(_L("started Wcstok 4"));      //Was L8
    wchar_t WCString1[] = L"?a???b,,,#c";
    wchar_t *ptr;
    wchar_t *pwcs1, *pwcs2, *pwcs3, a[]=L"a", b[]=L"??b",c[]=L"c";

    pwcs1 = wcstok(WCString1, L"?", &ptr);
    /* pwcs1 points to the token L"a"*/
    pwcs2 = wcstok((wchar_t *)NULL, L",", &ptr);
    /* pwcs2 points to the token L"??b"*/
    pwcs3 = wcstok( (wchar_t *)NULL, L"#,", &ptr);
    /* pwcs3 points to the token L"c"*/              
                          
    if(!(wcscmp (pwcs1, a)) && !(wcscmp (pwcs2, b)) && !(wcscmp (pwcs3,c)))
		{
    	INFO_PRINTF4(_L("tokens generated are %ls, %ls and %ls"), pwcs1, pwcs2, pwcs3);            
        return KErrNone;
		}
    else
		{
    	return KErrGeneral;
		}                
	}

//wcswcs test..
/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcswcs_Test_1L
	API Tested				:	Wcswcs()
	TestCase Description	:	To test that- The function returning WS1 in case if WS2 is NULL.
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcswcs_Test_1L( )
	{
	wchar_t *buf[20] = { L"fkhsdf%%38",
						 L"rtti123123",
						};

	wchar_t *str  = wcswcs(buf[0],L"");
	wchar_t *str1 = wcswcs(buf[1],L"\0");

	INFO_PRINTF2(_L("[Expected:0]%d"),wcscmp(str,buf[0]));
	INFO_PRINTF2(_L("[Expected:0]%d"),wcscmp(str1,buf[1]));
	 	
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcswcs_Test_2L
	API Tested				:	Wcswcs()
	TestCase Description	:	Test that,The function returning  pointer to the first occurence 
								of the substring ws2 in the string WS1,   in case if there is an 
								occurrence in the wide-character string pointed to by ws1 of any 
								wide-character code from the wide-character string pointed to by ws2.
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcswcs_Test_2L( )
	{
	wchar_t *res,w_char[40] = L"LFDJSLLSKJLSJLAKJ(*&#(@!~~@!()( #@";
	TInt i;

	for(i=0; w_char[i];i++)
		{
		res = wcswcs(w_char,w_char+i);
		INFO_PRINTF2(_L("Res Array[wcswcs]:: %ls"),res);
		}

	res = wcswcs(w_char , L"J(*");
	INFO_PRINTF2(_L("Res2[wcswcs]:%ls"),res);

	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcswcs_Test_3L
	API Tested				:	Wcswcs()
	TestCase Description	:	Test that,The function returning NULL pointer, in case if there
								is no occurrence in the wide-character string pointed to by ws1
								of any wide-character code from the wide-character string pointed 
								to by ws2.
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcswcs_Test_3L( )
	{
	TInt i;
	wchar_t w_char[40]		=	L"LFDJSLLSKJLSJLAKJ(*&#(@!~~@!()( #@";
	wchar_t *w_char2[30]		=	{	L"343",
								L"oipoipi",
								L"\b\t\a",
								L"YYYYYYY",
								};
	for(i=0 ;i  <4 ; i++)
		{
		INFO_PRINTF2(_L("[Expected 0] %d"), wcswcs(w_char,w_char2[i]));
		}
	return KErrNone;
	}


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcswcs_Test_4L
	API Tested				:	Wcswcs()
	TestCase Description	:	This will test the API with large string as input strings. This will
								read the chars from a file and populate it in to a buffer, These buffers 
								are used for testing.
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::Wcswcs_Test_4L( )
	{
	//Test for  16-bit characters //readin from file 

	INFO_PRINTF1(_L(" Test for 16-bit characters"));
	unsigned int alphabets[10] = {0x0905,0x0906,0x0907,0x0908,0x0000,
						0x0909,0x090a,0x090f,0x090d,0x090e};
	char filename[20] = "c:\\file.wc";
	CreateTestDataFile(filename,alphabets,sizeof(alphabets));
	
	FILE *fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	wchar_t fwcs1[15];
	wchar_t fwcs2[15];
	wchar_t fwcs3[30];
	fgetws(fwcs1,10,fp);
	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}
	fgetws(fwcs2,10,fp);
	fgetws(fwcs3,25,fp);  	
	fclose(fp); 	

	  
	wchar_t *res = wcswcs(fwcs1,fwcs2);
	// Since both are same
	if(wcscmp(res,fwcs1))
		{
		return -1;
		}

	INFO_PRINTF2(_L("Input String:: %ls") ,fwcs1);

	res = wcswcs(fwcs1,fwcs1+2);

	INFO_PRINTF2(_L("Res [Should n't be NULL]: %ls") ,res);

	INFO_PRINTF2(_L("fwcs2:%ls") ,fwcs2 + 8);
	INFO_PRINTF2(_L("fwcs3:%ls") ,fwcs3);


	res	=	wcswcs(fwcs3,fwcs3 + 3);

	INFO_PRINTF2(_L("Resultant of wcswcs(fwcs3,fwcs2 + 8): %ls") ,res);
	return KErrNone;
	}

