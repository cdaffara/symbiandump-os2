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



#ifndef __TESTCTYPE_H__
#define __TESTCTYPE_H__

#include <test/testexecutestepbase.h>
#include <e32svr.h>
#include <ctype.h>

_LIT(Kisalpha_atoz, "isalpha_atoz");
_LIT(Kisalpha_num, "isalpha_num");
_LIT(Kisalpha_punc, "isalpha_punc");
_LIT(Kisalpha_escape, "isalpha_escape");
      
_LIT(Kisdigit_digit, "isdigit_digit");
_LIT(Kisdigit_punc, "isdigit_punc");
_LIT(Kisdigit_escape, "isdigit_escape");
_LIT(Kisdigit_alpha, "isdigit_alpha");
        
_LIT(Kisalnum_alpha, "isalnum_alpha");
_LIT(Kisalnum_num, "isalnum_num");
_LIT(Kisalnum_punc, "isalnum_punc");
_LIT(Kisalnum_escape, "isalnum_escape");
        
_LIT(Kispunct_punct, "ispunct_punct");
_LIT(Kispunct_alpha_num, "ispunct_alpha_num");
_LIT(Kispunct_cntrl, "ispunct_cntrl");
        
_LIT(Kisupper_upper, "isupper_upper");
_LIT(Kisupper_lower, "isupper_lower");
_LIT(Kisupper_punc, "isupper_punc");
_LIT(Kisupper_escape, "isupper_escape");

_LIT(Kislower_lower, "islower_lower");
_LIT(Kislower_upper, "islower_upper");
_LIT(Kislower_punc, "islower_punc");
_LIT(Kislower_escape, "islower_escape");

_LIT(Kisxdigit_xdigit, "isxdigit_xdigit");
_LIT(Kisxdigit_escape, "isxdigit_escape");
_LIT(Kisxdigit_punc, "isxdigit_punc");
_LIT(Kisxdigit_other_alpha, "isxdigit_other_alpha");
        
_LIT(Ktoupper_lower, "toupper_lower");
_LIT(Ktoupper_punc, "toupper_punc");
_LIT(Ktoupper_upper, "toupper_upper");
_LIT(Ktoupper_escape, "toupper_escape");

_LIT(Ktolower_upper, "tolower_upper");
_LIT(Ktolower_punc, "tolower_punc");
_LIT(Ktolower_lower, "tolower_lower");
_LIT(Ktolower_escape, "tolower_escape");
        
_LIT(Kisspace_space, "isspace_space");
_LIT(Kisspace_punc, "isspace_punc");
_LIT(Kisspace_alpha, "isspace_alpha");
_LIT(Kisspace_num, "isspace_num");

_LIT(Kiscntrl_cntrl, "iscntrl_cntrl");
_LIT(Kiscntrl_other_escape, "iscntrl_other_escape");
_LIT(Kiscntrl_alpha_num, "iscntrl_alpha_num");
		
_LIT(Kisprint_cntrl, "isprint_cntrl");
_LIT(Kisprint_alpha_num, "isprint_alpha_num");
_LIT(Kisprint_xdigit, "isprint_xdigit");
_LIT(Kisprint_punc, "isprint_punc");

_LIT(Kisgraph_escape, "isgraph_escape");
_LIT(Kisgraph_alpha_num, "isgraph_alpha_num");
_LIT(Kisgraph_xdigit, "isgraph_xdigit");
_LIT(Kisgraph_punc, "isgraph_punc");
_LIT(Kisgraph_space, "isgraph_space");
		
_LIT(Kisascii_ascii, "isascii_ascii");
_LIT(Kisascii_bad_param, "isascii_bad_param");

_LIT(Ktoascii_ascii, "toascii_ascii");
_LIT(Ktoascii_bad, "toascii_bad");

class CTestCtype : public CTestStep
	{
public:
	~CTestCtype(); 
	CTestCtype(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	 	TInt isalpha_atoz();
        TInt isalpha_num();
        TInt isalpha_punc();
        TInt isalpha_escape();
        
        TInt isdigit_digit();
        TInt isdigit_punc();
        TInt isdigit_escape();
        TInt isdigit_alpha();
        
        TInt isalnum_alpha();
		TInt isalnum_num();
		TInt isalnum_punc();
		TInt isalnum_escape();

		TInt ispunct_punct();
		TInt ispunct_alpha_num();
		TInt ispunct_cntrl();

		TInt isupper_upper();
		TInt isupper_lower();
		TInt isupper_punc();
		TInt isupper_escape();
		
		TInt islower_lower();
		TInt islower_upper();
		TInt islower_punc();
		TInt islower_escape();
		
		TInt isxdigit_xdigit();
		TInt isxdigit_escape();
		TInt isxdigit_punc();
		TInt isxdigit_other_alpha();
		
		TInt toupper_lower();
		TInt toupper_upper();
		TInt toupper_punc();
		TInt toupper_escape();

		TInt tolower_upper();
		TInt tolower_lower();
		TInt tolower_punc();
		TInt tolower_escape();

		TInt isspace_space();
		TInt isspace_punc();
		TInt isspace_alpha();
		TInt isspace_num();
		
		TInt iscntrl_cntrl();
		TInt iscntrl_other_escape();
		TInt iscntrl_alpha_num();
		
		TInt isprint_cntrl();
		TInt isprint_alpha_num();
		TInt isprint_punc();
		TInt isprint_xdigit();
	
		TInt isgraph_escape();
		TInt isgraph_alpha_num();
		TInt isgraph_punc();
		TInt isgraph_xdigit();
		TInt isgraph_space();
		
		TInt isascii_ascii();
		TInt isascii_bad_param();

		TInt toascii_ascii();
		TInt toascii_bad();
	};
#endif 
