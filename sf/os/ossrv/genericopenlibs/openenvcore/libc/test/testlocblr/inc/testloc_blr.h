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



#ifndef TESTLOC_BLR_H
#define TESTLOC_BLR_H

// INCLUDES
#include <test/testexecutestepbase.h>
#include <e32base.h>
#include <e32std.h>
#include <e32ldr.h>
#include <f32file.h>
#include <locale.h>
#include <langinfo.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <iconv.h>
#include <errno.h>
#include <charconv.h>
#include <f32file.h>

// CONSTANTS
const TInt KNumLocaleExports = 22;

_LIT(KTestLocale , "TestSetLocale");
_LIT(KTestNl_Langinfo_Pass,"TestNl_Langinfo_Pass");
_LIT(KTestIconv_Pass,"TestIconv_Pass");
_LIT(KTestLocaleConv,"TestLocaleConv");
_LIT(KTestIconv_Open_Pass,"TestIconv_Open_Pass");
_LIT(KTestIconv_Open_Fail,"TestIconv_Open_Fail");


class CTestLoc_Blr : public CTestStep
    {
    public:  // Constructors and destructor
        
	    ~CTestLoc_Blr(); 
		CTestLoc_Blr(const TDesC& aStepName);
		TVerdict doTestStepL();
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();
        TInt iParamCnt;
    private:
		// Utility functions for handling parameters etc.
		TBool SameString(char *aString1, char *aString2);
		void ReadIntParam(TInt &aInt);
		void ReadLangParam(char* aString);
		void ReadCodeParam(char* aString);
		void ReadExpCodeParam(char* aString);
		void ReadStringParam(char* aString);
		void ReadExpStringParam(char* aString);
		//Test functions
        TInt TestSetLocale(); 
        TInt Langinfo(); 
        TInt TestNl_Langinfo_Pass();
        TInt TestNl_Langinfo_Fail();
        TInt TestIconv_Open_Pass();
        TInt TestIconv_Open_Fail();
        TInt TestIconv_Pass();
        TInt TestLocaleConv();
        
        TInt ReadLocaleInformation(char* itemName, char* expectedOuput);
        
        void ConvertHexTochar(char* dst, char*src);
        // Utility functions for handling parameters etc.
        void LocateSupportedDLLs();
        void SupportedCharsets();
        
    };

#endif      // 
            
