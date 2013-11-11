// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tlocale.cpp
//




#include "tlocale.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestLocale::~CTestLocale() 
	{ 
	}  

CTestLocale::CTestLocale(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestLocale::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestLocale::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestLocale::doTestStepL()
	{
	TVerdict err=EFail;
	if(TestStepName() == Ksynclocale)
   		{
   	   	INFO_PRINTF1(_L("Ksynclocale():"));
   	   	err = synclocale();
   	   	}
   	else if(TestStepName() == Ksynclocalehybrid)
   		{
   	   	INFO_PRINTF1(_L("Ksynclocalehybrid():"));
   	   	err = synclocalehybrid();
   	   	}
   	else if(TestStepName() == Kstrcoll_locale)
   		{
   	   	INFO_PRINTF1(_L("Kstrcoll_locale():"));
   	   	err = strcoll_locale();
   	   	}
   	else if(TestStepName() == Kstrcoll_synclocale)
   		{
   	   	INFO_PRINTF1(_L("Kstrcoll_synclocale():"));
   	   	err = strcoll_synclocale();
   	   	}
   	else if(TestStepName() == Kclocale)
   		{
   	   	INFO_PRINTF1(_L("Kclocale():"));
   	   	err = clocale();
   	   	}
    else if(TestStepName()== KTestisascii)
	   	{
	   	INFO_PRINTF1(_L("Testisascii:"));
	   	err = Testisascii();
	   	}
   	else if(TestStepName() == KTesttoascii)
	   	{
	   	INFO_PRINTF1(_L("Testtoascii():"));
	   	err = Testtoascii();
	   	}
	else if(TestStepName() == KTestmblen0)
		{
		INFO_PRINTF1(_L("Testmblen0():"));
		err = Testmblen0();
		}
	else if(TestStepName() == KTestmblen1)
		{
		INFO_PRINTF1(_L("Testmblen1():"));
		err = Testmblen1();
		}
	else if(TestStepName() == KTestmblen2)
		{
		INFO_PRINTF1(_L("Testmblen2():"));
		err = Testmblen2();
		}
	else if(TestStepName() == KTestmblen3)
		{
		INFO_PRINTF1(_L("Testmblen3():"));
		err = Testmblen3();
		}
	else if(TestStepName() == KTestmblen4)
		{
		INFO_PRINTF1(_L("Testmblen4():"));
		err = Testmblen4();
		}
	else if(TestStepName() == KTestwctomb0)
		{
		INFO_PRINTF1(_L("Testwctomb0():"));
		err = Testwctomb0();
		}
	else if(TestStepName() == KTestwctomb1)
		{
		INFO_PRINTF1(_L("Testwctomb1():"));
		err = Testwctomb1();
		}
	else if(TestStepName() == KTestwctomb2)
		{
		INFO_PRINTF1(_L("Testwctomb2():"));
		err = Testwctomb2();
		}
	else if(TestStepName() == KTestwctomb3)
		{
		INFO_PRINTF1(_L("Testwctomb3():"));
		err = Testwctomb3();
		}
	else if(TestStepName() == KTestmbtowc0)
		{
		INFO_PRINTF1(_L("Testmbtowc0():"));
		err = Testmbtowc0();
		}
	else if(TestStepName() == KTestmbtowc1)
		{
		INFO_PRINTF1(_L("Testmbtowc1():"));
		err = Testmbtowc1();
		}
	else if(TestStepName() == KTestmbstowcs)
		{
		INFO_PRINTF1(_L("Testmbstowcs():"));
		err = Testmbstowcs();
		}
	else if(TestStepName() == KTestwcstombs)
		{
		INFO_PRINTF1(_L("Testwcstombs():"));
		err = Testwcstombs();
		}
  	else if(TestStepName() == KTestwcrtomb0)
		{
		INFO_PRINTF1(_L("Testwcrtomb0():"));
		err = Testwcrtomb0();
		}
	else if(TestStepName() == KTestwcrtomb1)
		{
		INFO_PRINTF1(_L("Testwcrtomb1():"));
		err = Testwcrtomb1();
		}
	else if(TestStepName() == KTestwcrtomb_NULL)
		{
		INFO_PRINTF1(_L("Testwcrtomb_NULL():"));
		err = Testwcrtomb_NULL();
		}
	else if(TestStepName() == KTestwcrtomb_Neg1)
		{
		INFO_PRINTF1(_L("Testwcrtomb_Neg1():"));
		err = Testwcrtomb_Neg1();
		}
	else if(TestStepName() == KTestwcrtomb_Neg2)
		{
		INFO_PRINTF1(_L("Testwcrtomb_Neg2():"));
		err = Testwcrtomb_Neg2();
		}
	else if(TestStepName() == KTestwcscoll)
		{
		INFO_PRINTF1(_L("Testwcscoll():"));
		err = Testwcscoll();
		}
	else if(TestStepName() == KTestwcsicoll)
		{
		INFO_PRINTF1(_L("Testwcsicoll():"));
		err = Testwcsicoll();
		}
	else if(TestStepName() == KTestwcslwr)
		{
		INFO_PRINTF1(_L("Testwcslwr():"));
		err = Testwcslwr();
		}
	else if(TestStepName() == KTestwcsupr)
		{
		INFO_PRINTF1(_L("Testwcsupr():"));
		err = Testwcsupr();
		}
	else if(TestStepName() == KTestwctrans)
		{
		INFO_PRINTF1(_L("Testwctrans():"));
		err = Testwctrans();
		}
	else if(TestStepName() == KTestwctrans_Neg)
		{
		INFO_PRINTF1(_L("Testwctrans_Neg():"));
		err = Testwctrans_Neg();
		}
	else if(TestStepName() == KTestwctype)
		{
		INFO_PRINTF1(_L("Testwctype():"));
		err = Testwctype();
		}
	else if(TestStepName() == KTestwctype_Neg)
		{
		INFO_PRINTF1(_L("Testwctype_Neg():"));
		err = Testwctype_Neg();
		}
	else if(TestStepName() == KTestwcstod)
		{
		INFO_PRINTF1(_L("Testwcstod():"));
		err = Testwcstod();
		}
	else if(TestStepName() == KTestwcstod_Neg)
		{
		INFO_PRINTF1(_L("Testwcstod_Neg():"));
		err = Testwcstod_Neg();
		}
	else if(TestStepName() == KTestwctob)
		{
		INFO_PRINTF1(_L("Testwctob():"));
		err = Testwctob();
		}
	else if(TestStepName() == KTestwctob_Neg)
		{
		INFO_PRINTF1(_L("Testwctob_Neg():"));
		err = Testwctob_Neg();
		}
	else if(TestStepName() == KTestwcswidth)
		{
		INFO_PRINTF1(_L("Testwcswidth():"));
		err = Testwcswidth();
		}
	else if(TestStepName() == KTestwcswidth_Neg)
		{
		INFO_PRINTF1(_L("Testwcswidth_Neg():"));
		err = Testwcswidth_Neg();
		}
	else if(TestStepName() == KTestwcwidth)
		{
		INFO_PRINTF1(_L("Testwcwidth():"));
		err = Testwcwidth();
		}
	else if(TestStepName() == KTestmbrlen0)
		{
		INFO_PRINTF1(_L("Testmbrlen0():"));
		err = Testmbrlen0();
		}
	else if(TestStepName() == KTestmbrlen1)
		{
		INFO_PRINTF1(_L("Testmbrlen1():"));
		err = Testmbrlen1();
		}
	else if(TestStepName() == KTestmbrtowc)
		{
		INFO_PRINTF1(_L("Testmbrtowc():"));
		err = Testmbrtowc();
		}
	else if(TestStepName() == KTestmbrtowc_Neg)
		{
		INFO_PRINTF1(_L("Testmbrtowc_Neg():"));
		err = Testmbrtowc_Neg();
		}
	else if(TestStepName() == KTestmbsrtowcs)
		{
		INFO_PRINTF1(_L("Testmbsrtowcs():"));
		err = Testmbsrtowcs();
		}
	else if(TestStepName() == KTestmbsrtowcs1)
		{
		INFO_PRINTF1(_L("Testmbsrtowcs1():"));
		err = Testmbsrtowcs1();
		}
	else if(TestStepName() == KTestmbsinit)
		{
		INFO_PRINTF1(_L("Testmbsinit():"));
		err = Testmbsinit();
		}
	else if(TestStepName() == KTestwcsrtombs)
		{
		INFO_PRINTF1(_L("Testwcsrtombs():"));
		err = Testwcsrtombs();
		}
	else if(TestStepName() == KTestsetlocale)
		{
		INFO_PRINTF1(_L("Testsetlocale():"));
		err = Testsetlocale();
		}
	else if(TestStepName() == KTestctypeApi)
		{
		INFO_PRINTF1(_L("TestctypeApi():"));
		err = TestctypeApi();
		}

	else if(TestStepName() == Ktestnewlocale)
        {
        INFO_PRINTF1(_L("testnewlocale():"));

        err = testnewlocale();
		}

        

   
   	return err;
	}
	
 

