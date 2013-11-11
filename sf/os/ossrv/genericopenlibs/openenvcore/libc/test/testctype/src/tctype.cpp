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



#include "tctype.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestCtype::~CTestCtype() 
	{ 
	}  

CTestCtype::CTestCtype(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestCtype::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestCtype::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestCtype::doTestStepL()
	{
		int err;

   			if(TestStepName() == Kisalpha_atoz)
   			{
   	   		INFO_PRINTF1(_L("isalpha_atoz():"));
   	   		err = isalpha_atoz();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisalpha_num)
   			{
   	   		INFO_PRINTF1(_L("isalpha_num():"));
   	   		err = isalpha_num();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisalpha_punc)
   			{
   	   		INFO_PRINTF1(_L("isalpha_punc():"));
   	   		err = isalpha_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisalpha_escape)
   			{
   	   		INFO_PRINTF1(_L("isalpha_escape():"));
   	   		err = isalpha_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
      
        if(TestStepName() == Kisdigit_digit)
   			{
   	   		INFO_PRINTF1(_L("isdigit_digit():"));
   	   		err = isdigit_digit();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisdigit_punc)
   			{
   	   		INFO_PRINTF1(_L("isdigit_punc():"));
   	   		err = isdigit_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisdigit_escape)
   			{
   	   		INFO_PRINTF1(_L("isdigit_escape():"));
   	   		err = isdigit_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisdigit_alpha)
   			{
   	   		INFO_PRINTF1(_L("isdigit_alpha():"));
   	   		err = isdigit_alpha();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        
        if(TestStepName() == Kisalnum_alpha)
   			{
   	   		INFO_PRINTF1(_L("isalnum_alpha():"));
   	   		err = isalnum_alpha();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisalnum_num)
   			{
   	   		INFO_PRINTF1(_L("isalnum_num():"));
   	   		err = isalnum_num();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisalnum_punc)
   			{
   	   		INFO_PRINTF1(_L("isalnum_punc():"));
   	   		err = isalnum_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisalnum_escape)
   			{
   	   		INFO_PRINTF1(_L("isalnum_escape():"));
   	   		err = isalnum_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        
        if(TestStepName() == Kispunct_punct)
   			{
   	   		INFO_PRINTF1(_L("ispunct_punct():"));
   	   		err = ispunct_punct();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kispunct_alpha_num)
   			{
   	   		INFO_PRINTF1(_L("ispunct_alpha_num():"));
   	   		err = ispunct_alpha_num();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kispunct_cntrl)
   			{
   	   		INFO_PRINTF1(_L("ispunct_cntrl():"));
   	   		err = ispunct_cntrl();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        
        if(TestStepName() == Kisupper_upper)
   			{
   	   		INFO_PRINTF1(_L("isupper_upper():"));
   	   		err = isupper_upper();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisupper_lower)
   			{
   	   		INFO_PRINTF1(_L("isupper_lower():"));
   	   		err = isupper_lower();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisupper_punc)
   			{
   	   		INFO_PRINTF1(_L("isupper_punc():"));
   	   		err = isupper_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisupper_escape)
   			{
   	   		INFO_PRINTF1(_L("isupper_escape():"));
   	   		err = isupper_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else

        if(TestStepName() == Kislower_lower)
   			{
   	   		INFO_PRINTF1(_L("islower_lower():"));
   	   		err = islower_lower();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kislower_upper)
   			{
   	   		INFO_PRINTF1(_L("islower_upper():"));
   	   		err = islower_upper();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kislower_punc)
   			{
   	   		INFO_PRINTF1(_L("islower_punc():"));
   	   		err = islower_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kislower_escape)
   			{
   	   		INFO_PRINTF1(_L("islower_escape():"));
   	   		err = islower_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else

        if(TestStepName() == Kisxdigit_xdigit)
   			{
   	   		INFO_PRINTF1(_L("isxdigit_xdigit():"));
   	   		err = isxdigit_xdigit();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisxdigit_escape)
   			{
   	   		INFO_PRINTF1(_L("isxdigit_escape():"));
   	   		err = isxdigit_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisxdigit_punc)
   			{
   	   		INFO_PRINTF1(_L("isxdigit_punc():"));
   	   		err = isxdigit_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisxdigit_other_alpha)
   			{
   	   		INFO_PRINTF1(_L("isxdigit_other_alpha():"));
   	   		err = isxdigit_other_alpha();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        
        if(TestStepName() == Ktoupper_lower)
   			{
   	   		INFO_PRINTF1(_L("toupper_lower():"));
   	   		err = toupper_lower();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Ktoupper_punc)
   			{
   	   		INFO_PRINTF1(_L("toupper_punc():"));
   	   		err = toupper_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Ktoupper_upper)
   			{
   	   		INFO_PRINTF1(_L("toupper_upper():"));
   	   		err = toupper_upper();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Ktoupper_escape)
   			{
   	   		INFO_PRINTF1(_L("toupper_escape():"));
   	   		err = toupper_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else

        if(TestStepName() == Ktolower_upper)
   			{
   	   		INFO_PRINTF1(_L("tolower_upper():"));
   	   		err = tolower_upper();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Ktolower_punc)
   			{
   	   		INFO_PRINTF1(_L("tolower_punc():"));
   	   		err = tolower_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Ktolower_lower)
   			{
   	   		INFO_PRINTF1(_L("tolower_lower():"));
   	   		err = tolower_lower();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Ktolower_escape)
   			{
   	   		INFO_PRINTF1(_L("tolower_escape():"));
   	   		err = tolower_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        
        if(TestStepName() == Kisspace_space)
   			{
   	   		INFO_PRINTF1(_L("isspace_space():"));
   	   		err = isspace_space();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisspace_punc)
   			{
   	   		INFO_PRINTF1(_L("isspace_punc():"));
   	   		err = isspace_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisspace_alpha)
   			{
   	   		INFO_PRINTF1(_L("isspace_alpha():"));
   	   		err = isspace_alpha();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
        if(TestStepName() == Kisspace_num)
   			{
   	   		INFO_PRINTF1(_L("isspace_num():"));
   	   		err = isspace_num();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else

		if(TestStepName() == Kiscntrl_cntrl)
   			{
   	   		INFO_PRINTF1(_L("iscntrl_cntrl():"));
   	   		err = iscntrl_cntrl();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kiscntrl_other_escape)
   			{
   	   		INFO_PRINTF1(_L("iscntrl_other_escape():"));
   	   		err = iscntrl_other_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kiscntrl_alpha_num)
   			{
   	   		INFO_PRINTF1(_L("iscntrl_alpha_num():"));
   	   		err = iscntrl_alpha_num();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		
		if(TestStepName() == Kisprint_cntrl)
   			{
   	   		INFO_PRINTF1(_L("isprint_cntrl():"));
   	   		err = isprint_cntrl();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kisprint_alpha_num)
   			{
   	   		INFO_PRINTF1(_L("isprint_alpha_num():"));
   	   		err = isprint_alpha_num();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kisprint_xdigit)
   			{
   	   		INFO_PRINTF1(_L("isprint_xdigit():"));
   	   		err = isprint_xdigit();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kisprint_punc)
   			{
   	   		INFO_PRINTF1(_L("isprint_punc():"));
   	   		err = isprint_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else

		if(TestStepName() == Kisgraph_escape)
   			{
   	   		INFO_PRINTF1(_L("isgraph_escape():"));
   	   		err = isgraph_escape();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kisgraph_alpha_num)
   			{
   	   		INFO_PRINTF1(_L("isgraph_alpha_num():"));
   	   		err = isgraph_alpha_num();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kisgraph_xdigit)
   			{
   	   		INFO_PRINTF1(_L("isgraph_xdigit():"));
   	   		err = isgraph_xdigit();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kisgraph_punc)
   			{
   	   		INFO_PRINTF1(_L("isgraph_punc():"));
   	   		err = isgraph_punc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kisgraph_space)
   			{
   	   		INFO_PRINTF1(_L("isgraph_space():"));
   	   		err = isgraph_space();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		
		if(TestStepName() == Kisascii_ascii)
   			{
   	   		INFO_PRINTF1(_L("isascii_ascii():"));
   	   		err = isascii_ascii();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Kisascii_bad_param)
   			{
   	   		INFO_PRINTF1(_L("isascii_bad_param():"));
   	   		err = isascii_bad_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else

		if(TestStepName() == Ktoascii_ascii)
   			{
   	   		INFO_PRINTF1(_L("toascii_ascii():"));
   	   		err = toascii_ascii();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
		if(TestStepName() == Ktoascii_bad)
   			{
   	   		INFO_PRINTF1(_L("toascii_bad():"));
   	   		err = toascii_bad();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	
		return TestStepResult(); 

	}
 

