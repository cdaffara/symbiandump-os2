/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


// [INCLUDE FILES] - do not remove
#include <e32math.h>
#include "widecharclassapiBCTest.h"

#include <stl\_cwchar.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>


// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CwidecharclassapiBCTest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CwidecharclassapiBCTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    widecharclassapiBCTest.cpp file and to widecharclassapiBCTest.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // [test cases entries] - do not remove
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // CwidecharclassapiBCTest::PrintTest. Otherwise the compiler
        // gives errors.
        
        ENTRY( "Arithmetic Functions test", CwidecharclassapiBCTest::ArithmeticTest ),
        ENTRY( "File Manipulation test", CwidecharclassapiBCTest::FileManipulationTest ),
        ENTRY( "Console Operations test", CwidecharclassapiBCTest::ConsoleOperationsTest ),
        ENTRY( "String Operations test", CwidecharclassapiBCTest::StringOperationsTest ),
        ENTRY( "Conversion Operations test", CwidecharclassapiBCTest::ConversionOperationsTest ),
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", CwidecharclassapiBCTest::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( CwidecharclassapiBCTest::PrintTest, ETrue, 1, 3 ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }


int WriteVarPrintfConsole (char *format, ...)
{
  int result;
  va_list args;
  va_start (args, format);
  result = std::vwprintf (L"%d", args);
  va_end (args);
  return result;
}

int WriteVarPrintfFile (FILE * stream, char * format, ...)
{
  int result;
  va_list args;
  va_start (args, format);
  result = std::vfwprintf (stream, L"%d", args);
  va_end (args);
  return result;
}

int WriteVarPrintfString (wchar_t * outputBuf, int sizeBuf, char * format, ...)
{
  int result;
  va_list args;
  va_start (args, format);
  result = std::vswprintf (outputBuf, sizeBuf, L"%d", args);
  va_end (args);
  return result;
}




TInt CwidecharclassapiBCTest::FileManipulationTest( TTestResult& aResult )
    {
  		int testError = 0;
__UHEAP_MARK;

  		
		const  wchar_t *wsList = L"This is Wide Character Class API test.";
		const  wchar_t *wPrintfTestFormat = L"%d";
		
		wchar_t wsListRead[64];
		wchar_t wPrintfTestReadList[64];
		std::wint_t wc = 88;
		int c = 65;


		char fileName[]="c:\\data\\others\\widechartest.txt";
		remove(fileName);
		
		FILE *fp = NULL;
		
		/* opening the output file */
		fp = fopen(fileName,"w");
		if(fp == NULL)
		{
		   testError++;
		}

//Test std::fwide
		int fileOrientation ;
		
		fileOrientation = std::fwide(fp, 0);


//Test std::fwprintf
		int fWritePrResult = std::fwprintf(fp, wPrintfTestFormat, c);
		if(fWritePrResult < 0)
			testError++;
		
//Test std::vfwprintf
		int fWriteVPrResult = WriteVarPrintfFile(fp, "%d", c);
		if(fWriteVPrResult < 0)
			testError++;
		
		
//Test std::fputwc		 
		std::wint_t fWriteWCResult = std::fputwc(wc,fp);
		if(fWriteWCResult == WEOF)
			testError++;
		
//Test std::fputws		 
		int fWriteWSResult = std::fputws(wsList, fp);
		if(fWriteWSResult < 0)
			testError++;
		
	
		fclose(fp);
		
		
		
		int wsListLength = std::wcslen(wsList);
		int wPrintTestListLength = std::wcslen(wPrintfTestFormat);
		

		/* opening the input file */
		fp = fopen(fileName,"r");
		if(fp == NULL)
		{
		   testError++;
		}

//Test std::fwscanf
		int inpC;
		int fReadPrResult = std::fwscanf(fp, wPrintfTestFormat, &inpC);
		if(fReadPrResult < 0)
			testError++;
		fReadPrResult = std::fwscanf(fp, wPrintfTestFormat, &inpC);
		if(fReadPrResult < 0)
			testError++;

		
//Test std::fputwc		 
		std::wint_t fReadWCResult = std::fgetwc(fp);
		if(fReadWCResult == WEOF)
			testError++;
		
//Test std::fgetws		 
		wchar_t * ptrReadWSResult = std::fgetws(wsListRead, wsListLength, fp);
		if(ptrReadWSResult == 0)
			testError++;
		
		
		fclose(fp);
		remove(fileName);
  		
  		
    // Test case passed
__UHEAP_MARKEND;
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
	if(testError)
	{
 	   _LIT( KDescriptionErr, "FileManipulationTest failed" );
 	   aResult.SetResult( KErrGeneral, KDescriptionErr );

 	   // Case was executed
	    return testError;
		
	}
	

    // Case was executed
    return KErrNone;

    }
    
    
TInt CwidecharclassapiBCTest::ConsoleOperationsTest( TTestResult& aResult )
    {
  		int testError = 0;
__UHEAP_MARK;
  		
  		freopen("c:\\data\\others\\myconsole.txt", "w", stdout);
//Test std::putwc 		
  		wchar_t * consoleMessage = L"Enter Character 'v' ";
		fputws(consoleMessage, stdout);
		wint_t putWCResult = std::putwc(L':', stdout);

//Test std::vwprintf
		int c=89;
		int fWriteVPrResult = WriteVarPrintfConsole("%d", c);
		if(fWriteVPrResult < 0)
			testError++;

		ungetwc(L'v', stdin);

//Test std::getwc		
		wint_t getWCResult;
		getWCResult = std::getwc(stdin);


		ungetwc(L'v', stdin);
		getWCResult = std::getwchar();

		fclose(stdout);
		fclose(stderr);
		fclose(stdin);

 		
    // Test case passed
__UHEAP_MARKEND;
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
	if(testError)
	{
 	   // Case was executed
	    return testError;
		
	}
	

    // Case was executed
    return KErrNone;


    }
    
    
TInt CwidecharclassapiBCTest::StringOperationsTest( TTestResult& aResult )
    {
  		int testError = 0;
__UHEAP_MARK;
  		
//Test std::wcscat  		
		wchar_t wcSRC[64] = L"Wide character";
		wchar_t wcDST[64] = L"Class API";
		wchar_t *wcWCSCATResult= std::wcscat(wcDST, (const wchar_t*)wcSRC);

//Test std::wcsncat		
		wchar_t *wcWCSNCATResult= std::wcsncat(wcDST, (const wchar_t*)wcSRC, 4);

		
		wchar_t *wcSTR = L"Wide Character Class API";
		wchar_t wcSearch = L'A';
//Test std::wcschr
		wchar_t* wcWCSCHRResult = std::wcschr(wcSTR, wcSearch);

//Test std::wcsrchr
		wchar_t* wcWCSRCHRResult = std::wcsrchr(wcSTR, wcSearch);

//Test std::wmemchr
		wchar_t* wcWMEMCHRResult = std::wmemchr(wcSTR, wcSearch, 4);


//Test std::wcscmp
		wchar_t *wcS1 = L"ABCDE";
		wchar_t *wcS2 = L"ACCD";
		int intWCSCMPResult = std::wcscmp(wcS1, wcS2);

//Test std::wcsncmp
		int intWCSNCMPResult = std::wcsncmp(wcS1, wcS2, 4);

//Test std::wcscoll
		int intWCSCOLLResult = std::wcscoll(wcS1, wcS2);

//Test std::wmemcmp
		int intWMEMCMPResult = std::wmemcmp(wcS1, wcS2, 4);


//Test std::wcscpy
		wchar_t wcSRC1[64] = L"Wide character";
		wchar_t wcDST1[64] = L"Class API";
		wchar_t *wcWCSCPYResult = std::wcscpy(wcDST1, wcSRC1);

//Test std::wcsncpy
		wchar_t *wcWCSNCPYResult = std::wcsncpy(wcDST1, wcSRC1, 4);

//Test std::wmemcpy
		wchar_t *wcWMEMCPYResult = std::wmemcpy(wcDST1, wcSRC1, 4);

//Test std::wmemmove
		wchar_t *wcWMEMMOVEResult = std::wmemmove(wcDST1, wcSRC1, 4);

//Test std::wmemset
		wchar_t *wcWMEMSETResult = std::wmemset(wcDST1, L'V', 4);

//Test std::wcsxfrm
		size_t lenWCSXFRMResult = std::wcsxfrm(wcDST1, wcSRC1, 15);

//Test std::wcscspn
 		const wchar_t wcSTR1[30] = L"1234567890ABCDE!@#$$";
  		const wchar_t wcSpanset[20] = L"ABCDE!@#$$";
		size_t intWCSCSPNResult = std::wcscspn(wcSTR1, wcSpanset);
	
//Test std::wcsspn	
		size_t intWCSSPNResult = std::wcsspn(wcSTR1, wcSpanset);

//Test std::wcspbrk	
		wchar_t * wcptrWCSSPNResult = std::wcspbrk(wcSTR1, wcSpanset);
	
	
//Test std::wcsftime	
		time_t rawtime;
  		struct tm * timeinfo;
  		wchar_t wcTimeOutput [80];
  		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
  		size_t lenString = std::wcsftime (wcTimeOutput,80,L"Now is %I:%M%p.",timeinfo);
	
//Test std::wcslen	
		size_t lenSTR = std::wcslen(wcSTR);
		
//Test std::wcsstr	
		wchar_t wcSTR2[64] = L"Wide character Class API";
		wchar_t wcSUBSTR[64] = L"Class API";
		wchar_t* wcptrWCSSTR = std::wcsstr(wcSTR2, wcSUBSTR);


		wchar_t wcSTRDest[64];
//Test std::swprintf	
		wchar_t *formatSTR = L"%d";
		int intSampleOutput = 78;
		int returnPrintf = std::swprintf(wcSTRDest, 63, formatSTR, intSampleOutput);
		
//Test std::swscanf	
		int intSampleInput;
		int returnScanf = std::swscanf(wcSTRDest, formatSTR, &intSampleInput);
  		
  		
//Test std::vswprintf
		wchar_t outputSTR[32];
		int c = 76;
		int fWriteVPrResult = WriteVarPrintfString(outputSTR, 32, "%d", c);
		if(fWriteVPrResult < 0)
			testError++;
 		
  		
    // Test case passed
__UHEAP_MARKEND;
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
	if(testError)
	{
 	   // Case was executed
	    return testError;
		
	}
	
    // Case was executed
    return KErrNone;


    }
    
    
TInt CwidecharclassapiBCTest::ConversionOperationsTest( TTestResult& aResult )
    {
  		int testError = 0;
//__UHEAP_MARK;

 		size_t len;
		char *szMBTest = "this is test";
		mbstate_t mbs;
		mbstate_t ps;
 		
//Test std::btowc
		int c = 65;
		std::wint_t wc;
 		/* converting single byte to wide-character */
 		wc = std::btowc(c);
  		

//Test std::wcstod
		 wchar_t wcs1[21] = L"  1.23abcd";
		 wchar_t wcs2[5]=L"abcd";
		 wchar_t *eptr;
		 float d;
		 /* convert wide-char string to double */  
		 d = std::wcstod(wcs1, &eptr);
		 /* compare the result */
		 if(!((d == 1.23F) && !(std::wcscmp (eptr, wcs2))))
		   testError++;
  

//Test std::wcstol
		 wchar_t wcs3[8]=L".23abcd";
		 long longintOutput;
		 /* convert wide-char string to long int */  
		 longintOutput = std::wcstol(wcs1, &eptr, 10);
		 /* compare the result */
		 if(!((longintOutput == 1) && !(std::wcscmp (eptr, wcs3))))
		   testError++;
  
//Test std::wcstoul
		 unsigned long unslongintOutput;
		 /* convert wide-char string to long int */  
		 unslongintOutput = std::wcstoul(wcs1, &eptr, 10);
		 /* compare the result */
		 if(!((unslongintOutput == 1) && !(std::wcscmp (eptr, wcs3))))
		   testError++;
  


//Test std::wcstok
		 const wchar_t *seps = L":";
		 wchar_t *last, *tok, text[] = L"one:twothree";
		              
		 tok = std::wcstok(text, seps, &last);
		 if(!tok)
		   testError++;
		 
		 
//Test std::mbsrtowcs
		wchar_t wideCharStringOutput[64];
		len = std::mbsrtowcs(wideCharStringOutput, (const char**)&szMBTest, 10, &ps);
		/* checking for error */
		if(len < 0)
		{
			testError++;
		}
	
//Test std::wcsrtombs
		char mbOutputString[64];
		wchar_t *wcptrString = wideCharStringOutput;
		size_t lenWCS = std::wcsrtombs(mbOutputString, (const wchar_t**) &wcptrString, len, &ps);


//Test std::wcrtomb
		wchar_t wcWCRTOMB = L'v';
		char mbWCRTOMB[10];
		len = std::wcrtomb(mbWCRTOMB, wcWCRTOMB, &ps);

    // Test case passed
__UHEAP_MARKEND ;


    // Sets test case result and description(Maximum size is KStifMaxResultDes)
	if(testError)
	{
	    return testError;
		
	}
	


    // Case was executed
    return KErrNone;


    }



TInt CwidecharclassapiBCTest::ArithmeticTest( TTestResult& aResult )
    {

  		int testError = 0;
__UHEAP_MARK;
		

//Test std::mbrlen
		char *szMBTest = "this is test";
		int aa = 2;
		mbstate_t mbs;
		memset(&mbs, 0, sizeof(mbs));
//		size_t charlen = std::mbrlen(szMBTest, 6, &mbs); 
		size_t charlen = std::mbrlen((const char*)&aa, 6, &mbs); 

//Test std::mbrtowc
		size_t len;
		wchar_t wcMBCOutput[100];
		char *s = "a";
		size_t n = 1;
		mbstate_t ps;
		/* converting multibyte sequence to a wide-char sequence */
		len = std::mbrtowc(wcMBCOutput,(const char *) s, n, &ps);
		/* checking for error value */
		if(len < 0)
		{
			testError++;
		}


//Test std::mbsinit
		int mbINITOutput = std::mbsinit((const mbstate_t*) &ps);


    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
	if(testError)
	{
 	   _LIT( KDescriptionErr, "ArithmeticTest failed" );
 	   aResult.SetResult( KErrGeneral, KDescriptionErr );

 	   // Case was executed
	    return testError;
		
	}
	
    _LIT( KDescription, "LoopTest passed" );
    aResult.SetResult( KErrNone, KDescription );

__UHEAP_MARK;

// Case was executed
    return KErrNone;

    }



// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
?type ?classname::?member_function(
   ?arg_type arg,
   ?arg_type arg )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
/*
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/
//  [End of File] - do not remove
