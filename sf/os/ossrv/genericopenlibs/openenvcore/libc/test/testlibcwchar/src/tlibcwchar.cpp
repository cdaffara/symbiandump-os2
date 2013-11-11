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


CTestLibcwchar::~CTestLibcwchar() 
	{ 
	}  

CTestLibcwchar::CTestLibcwchar(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestLibcwchar::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestLibcwchar::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestLibcwchar::doTestStepL()
	{
		int err;

   		if(TestStepName() == KTestfgetwcL)
   			{
   	   		INFO_PRINTF1(_L("TestfgetwcL():"));
   	   		err = TestfgetwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		if(TestStepName() == KTestOnefgetwsL)
   			{
   	   		INFO_PRINTF1(_L("TestOnefgetwsL():"));
   	   		err = TestOnefgetwsL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestTwofgetwsL)
   			{
   	   		INFO_PRINTF1(_L("TestTwofgetwsL():"));
   	   		err = TestTwofgetwsL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestThreefgetwsL)
   			{
   	   		INFO_PRINTF1(_L("TestTwofgetwsL():"));
   	   		err = TestThreefgetwsL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestgetwcL)
   			{
   	   		INFO_PRINTF1(_L("TestgetwcL():"));
   	   		err = TestgetwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		if(TestStepName() == KTestOnefwideL)
   			{
   	   		INFO_PRINTF1(_L("TestOnefwideL():"));
   	   		err = TestOnefwideL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestOnefwideL)
   			{
   	   		INFO_PRINTF1(_L("TestOnefwideL():"));
   	   		err = TestOnefwideL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestTwofwideL)
   			{
   	   		INFO_PRINTF1(_L("TestTwofwideL():"));
   	   		err = TestTwofwideL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestThreefwideL)
   			{
   	   		INFO_PRINTF1(_L("TestThreefwideL():"));
   	   		err = TestThreefwideL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestFourfwideL)
   			{
   	   		INFO_PRINTF1(_L("TestFourfwideL():"));
   	   		err = TestFourfwideL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestOnefputwcL)
   			{
   	   		INFO_PRINTF1(_L("TestOnefputwcL():"));
   	   		err = TestOnefputwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestTwofputwcL)
   			{
   	   		INFO_PRINTF1(_L("TestTwofputwcL():"));
   	   		err = TestTwofputwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestThreefputwcL)
   			{
   	   		INFO_PRINTF1(_L("TestThreefputwcL():"));
   	   		err = TestThreefputwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestFourfputwcL)
   			{
   	   		INFO_PRINTF1(_L("TestFourfputwcL():"));
   	   		err = TestFourfputwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestOneputwcL)
   			{
   	   		INFO_PRINTF1(_L("TestOneputwcL():"));
   	   		err = TestOneputwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestTwoputwcL)
   			{
   	   		INFO_PRINTF1(_L("TestTwoputwcL():"));
   	   		err = TestTwoputwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestThreeputwcL)
   			{
   	   		INFO_PRINTF1(_L("TestThreeputwcL():"));
   	   		err = TestThreeputwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestFourputwcL)
   			{
   	   		INFO_PRINTF1(_L("TestFourputwcL():"));
   	   		err = TestFourputwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestOnefputwsL)
   			{
   	   		INFO_PRINTF1(_L("TestOnefputwsL():"));
   	   		err = TestOnefputwsL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestTwofputwsL)
   			{
   	   		INFO_PRINTF1(_L("TestTwofputwsL():"));
   	   		err = TestTwofputwsL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestThreefputwsL)
   			{
   	   		INFO_PRINTF1(_L("TestThreefputwsL():"));
   	   		err = TestThreefputwsL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestFourfputwsL)
   			{
   	   		INFO_PRINTF1(_L("TestFourfputwsL():"));
   	   		err = TestFourfputwsL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	if(TestStepName() == KTestOneungetwcL)
   			{
   	   		INFO_PRINTF1(_L("TestOneungetwcL():"));
   	   		err = TestOneungetwcL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   		
   	   	if(TestStepName() == KTestOneputwcharL)
   			{
   	   		INFO_PRINTF1(_L("TestOneputwcharL():"));
   	   		err = TestOneputwcharL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	
   	   	if(TestStepName() == KTestOnegetwcharL)
   			{
   	   		INFO_PRINTF1(_L("TestOnegetwcharL():"));
   	   		err = TestOnegetwcharL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	   	   	
   	   	
   	   	if(TestStepName() == Kwcslen1L)
   			{
   	   		INFO_PRINTF1(_L("wcslen1L():"));
   	   		err = wcslen1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwcslen2L)
   			{
   	   		INFO_PRINTF1(_L("wcslen2L():"));
   	   		err = wcslen2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemcmp1L)
   			{
   	   		INFO_PRINTF1(_L("wmemcmp1L():"));
   	   		err = wmemcmp1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemcmp2L)
   			{
   	   		INFO_PRINTF1(_L("wmemcmp2L():"));
   	   		err = wmemcmp2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwcswidth1L)
   			{
   	   		INFO_PRINTF1(_L("wcswidth1L():"));
   	   		err = wcswidth1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwcswidth2L)
   			{
   	   		INFO_PRINTF1(_L("wcswidth2L():"));
   	   		err = wcswidth2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwcswidth3L)
   			{
   	   		INFO_PRINTF1(_L("wcswidth3L():"));
   	   		err = wcswidth3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwcswidth4L)
   			{
   	   		INFO_PRINTF1(_L("wcswidth4L():"));
   	   		err = wcswidth4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwcwidth1L)
   			{
   	   		INFO_PRINTF1(_L("wcwidth1L():"));
   	   		err = wcwidth1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwcwidth2L)
   			{
   	   		INFO_PRINTF1(_L("wcwidth2L():"));
   	   		err = wcwidth2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemchr1L)
   			{
   	   		INFO_PRINTF1(_L("wmemchr1L():"));
   	   		err = wmemchr1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemchr2L)
   			{
   	   		INFO_PRINTF1(_L("wmemchr2L():"));
   	   		err = wmemchr2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemcpy1L)
   			{
   	   		INFO_PRINTF1(_L("wmemcpy1L():"));
   	   		err = wmemcpy1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemcpy2L)
   			{
   	   		INFO_PRINTF1(_L("wmemcpy2L():"));
   	   		err = wmemcpy2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemcpy3L)
   			{
   	   		INFO_PRINTF1(_L("wmemcpy3L():"));
   	   		err = wmemcpy3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemset1L)
   			{
   	   		INFO_PRINTF1(_L("wmemset1L():"));
   	   		err = wmemset1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemset2L)
   			{
   	   		INFO_PRINTF1(_L("wmemset2L():"));
   	   		err = wmemset2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemset3L)
   			{
   	   		INFO_PRINTF1(_L("wmemset3L():"));
   	   		err = wmemset3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemmove1L)
   			{
   	   		INFO_PRINTF1(_L("wmemmove1L():"));
   	   		err = wmemmove1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemmove2L)
   			{
   	   		INFO_PRINTF1(_L("wmemmove2L():"));
   	   		err = wmemmove2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemmove3L)
   			{
   	   		INFO_PRINTF1(_L("wmemmove3L():"));
   	   		err = wmemmove3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Kwmemmove4L)
   			{
   	   		INFO_PRINTF1(_L("wmemmove4L():"));
   	   		err = wmemmove4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == KWcstol1L)
   			{
   	   		INFO_PRINTF1(_L("Wcstol1L():"));
   	   		err = Wcstol1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == KWcstol2L)
   			{
   	   		INFO_PRINTF1(_L("Wcstol2L():"));
   	   		err = Wcstol2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == KWcstol3L)
   			{
   	   		INFO_PRINTF1(_L("Wcstol3L():"));
   	   		err = Wcstol3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == KWcstol4L)
   			{
   	   		INFO_PRINTF1(_L("Wcstol4L():"));
   	   		err = Wcstol4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == KWcstol5L)
   			{
   	   		INFO_PRINTF1(_L("Wcstol5L():"));
   	   		err = Wcstol5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == KWcstol6L)
   			{
   	   		INFO_PRINTF1(_L("Wcstol6L():"));
   	   		err = Wcstol6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == KWcstol7L)
   			{
   	   		INFO_PRINTF1(_L("Wcstol7L():"));
   	   		err = Wcstol7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == KWcstol8L)
   			{
   	   		INFO_PRINTF1(_L("Wcstol8L():"));
   	   		err = Wcstol8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == KWcstol9L)
   			{
   	   		INFO_PRINTF1(_L("Wcstol9L():"));
   	   		err = Wcstol9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestmbsinit1L)
   			{
   	   		INFO_PRINTF1(_L("testmbsinit1L():"));
   	   		err = testmbsinit1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestmbsinit2L)
   			{
   	   		INFO_PRINTF1(_L("testmbsinit2L():"));
   	   		err = testmbsinit2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestmbsinit3L)
   			{
   	   		INFO_PRINTF1(_L("testmbsinit3L():"));
   	   		err = testmbsinit3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype1L)
   			{
   	   		INFO_PRINTF1(_L("testwctype1L():"));
   	   		err = testwctype1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype2L)
   			{
   	   		INFO_PRINTF1(_L("testwctype2L():"));
   	   		err = testwctype2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype3L)
   			{
   	   		INFO_PRINTF1(_L("testwctype3L():"));
   	   		err = testwctype3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype4L)
   			{
   	   		INFO_PRINTF1(_L("testwctype4L():"));
   	   		err = testwctype4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype5L)
   			{
   	   		INFO_PRINTF1(_L("testwctype5L():"));
   	   		err = testwctype5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype6L)
   			{
   	   		INFO_PRINTF1(_L("testwctype6L():"));
   	   		err = testwctype6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype7L)
   			{
   	   		INFO_PRINTF1(_L("testwctype7L():"));
   	   		err = testwctype7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype8L)
   			{
   	   		INFO_PRINTF1(_L("testwctype8L():"));
   	   		err = testwctype8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype9L)
   			{
   	   		INFO_PRINTF1(_L("testwctype9L():"));
   	   		err = testwctype9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype10L)
   			{
   	   		INFO_PRINTF1(_L("testwctype10L():"));
   	   		err = testwctype10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype11L)
   			{
   	   		INFO_PRINTF1(_L("testwctype11L():"));
   	   		err = testwctype11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype12L)
   			{
   	   		INFO_PRINTF1(_L("Ktestwctype12L():"));
   	   		err = testwctype12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype13L)
   			{
   	   		INFO_PRINTF1(_L("testwctype13L():"));
   	   		err = testwctype13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		if(TestStepName() == Ktestwctype14L)
   			{
   	   		INFO_PRINTF1(_L("testwctype14L():"));
   	   		err = testwctype14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype15L)
   			{
   	   		INFO_PRINTF1(_L("testwctype15L():"));
   	   		err = testwctype15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype16L)
   			{
   	   		INFO_PRINTF1(_L("testwctype16L():"));
   	   		err = testwctype16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctype17L)
   			{
   	   		INFO_PRINTF1(_L("testwctype17L():"));
   	   		err = testwctype17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctrans1L)
   			{
   	   		INFO_PRINTF1(_L("testwctrans1L():"));
   	   		err = testwctrans1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctrans2L)
   			{
   	   		INFO_PRINTF1(_L("testwctrans2L():"));
   	   		err = testwctrans2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctrans3L)
   			{
   	   		INFO_PRINTF1(_L("testwctrans3L():"));
   	   		err = testwctrans3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctrans4L)
   			{
   	   		INFO_PRINTF1(_L("testwctrans4L():"));
   	   		err = testwctrans4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
		if(TestStepName() == Ktestwctrans5L)
   			{
   	   		INFO_PRINTF1(_L("testwctrans5L():"));
   	   		err = testwctrans5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
   		if(TestStepName() == KWcsncmp_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncmp_Test_1L():"));
   	   		err = Wcsncmp_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncmp_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncmp_Test_2L():"));
   	   		err = Wcsncmp_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncmp_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncmp_Test_3L():"));
   	   		err = Wcsncmp_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncmp_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncmp_Test_4L():"));
   	   		err = Wcsncmp_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncmp_Test_5L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncmp_Test_5L():"));
   	   		err = Wcsncmp_Test_5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscmp_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcscmp_Test_1L():"));
   	   		err = Wcscmp_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscmp_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcscmp_Test_2L():"));
   	   		err = Wcscmp_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscmp_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcscmp_Test_3L():"));
   	   		err = Wcscmp_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscmp_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcscmp_Test_4L():"));
   	   		err = Wcscmp_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscat_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcscat_Test_1L():"));
   	   		err = Wcscat_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscat_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcscat_Test_2L():"));
   	   		err = Wcscat_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscat_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcscat_Test_3L():"));
   	   		err = Wcscat_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscat_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcscat_Test_4L():"));
   	   		err = Wcscat_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncat_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncat_Test_1L():"));
   	   		err = Wcsncat_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncat_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncat_Test_2L():"));
   	   		err = Wcsncat_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncat_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncat_Test_3L():"));
   	   		err = Wcsncat_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncat_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncat_Test_4L():"));
   	   		err = Wcsncat_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncat_Test_5L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncat_Test_5L():"));
   	   		err = Wcsncat_Test_5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscpy_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcscpy_Test_1L():"));
   	   		err = Wcscpy_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscpy_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcscpy_Test_2L():"));
   	   		err = Wcscpy_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscpy_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcscpy_Test_3L():"));
   	   		err = Wcscpy_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscpy_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcscpy_Test_4L():"));
   	   		err = Wcscpy_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcschr_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcschr_Test_1L():"));
   	   		err = Wcschr_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcschr_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcschr_Test_2L():"));
   	   		err = Wcschr_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcschr_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcschr_Test_3L():"));
   	   		err = Wcschr_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcschr_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcschr_Test_4L():"));
   	   		err = Wcschr_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcschr_Test_5L)
   			{
   	   		INFO_PRINTF1(_L("Wcschr_Test_5L():"));
   	   		err = Wcschr_Test_5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsrchr_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcsrchr_Test_1L():"));
   	   		err = Wcsrchr_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsrchr_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcsrchr_Test_2L():"));
   	   		err = Wcsrchr_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsrchr_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcsrchr_Test_3L():"));
   	   		err = Wcsrchr_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsrchr_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcsrchr_Test_4L():"));
   	   		err = Wcsrchr_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsrchr_Test_5L)
   			{
   	   		INFO_PRINTF1(_L("Wcsrchr_Test_5L():"));
   	   		err = Wcsrchr_Test_5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsspn_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcsspn_Test_1L():"));
   	   		err = Wcsspn_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsspn_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcsspn_Test_2L():"));
   	   		err = Wcsspn_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsspn_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcsspn_Test_3L():"));
   	   		err = Wcsspn_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsspn_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcsspn_Test_4L():"));
   	   		err = Wcsspn_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcspbrk_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcspbrk_Test_1L():"));
   	   		err = Wcspbrk_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcspbrk_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcspbrk_Test_2L():"));
   	   		err = Wcspbrk_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcspbrk_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcspbrk_Test_3L():"));
   	   		err = Wcspbrk_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcspbrk_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcspbrk_Test_4L():"));
   	   		err = Wcspbrk_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscoll_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcscoll_Test_1L():"));
   	   		err = Wcscoll_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscoll_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcscoll_Test_2L():"));
   	   		err = Wcscoll_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscoll_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcscoll_Test_3L():"));
   	   		err = Wcscoll_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscoll_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcscoll_Test_4L():"));
   	   		err = Wcscoll_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscspn_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcscspn_Test_1L():"));
   	   		err = Wcscspn_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscspn_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcscspn_Test_2L():"));
   	   		err = Wcscspn_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscspn_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcscspn_Test_3L():"));
   	   		err = Wcscspn_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcscspn_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcscspn_Test_4L():"));
   	   		err = Wcscspn_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncpy_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncpy_Test_1L():"));
   	   		err = Wcsncpy_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncpy_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncpy_Test_2L():"));
   	   		err = Wcsncpy_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncpy_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncpy_Test_3L():"));
   	   		err = Wcsncpy_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncpy_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncpy_Test_4L():"));
   	   		err = Wcsncpy_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsncpy_Test_5L)
   			{
   	   		INFO_PRINTF1(_L("Wcsncpy_Test_5L():"));
   	   		err = Wcsncpy_Test_5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsstr_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcsstr_Test_1L():"));
   	   		err = Wcsstr_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsstr_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcsstr_Test_2L():"));
   	   		err = Wcsstr_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsstr_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcsstr_Test_3L():"));
   	   		err = Wcsstr_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcsstr_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcsstr_Test_4L():"));
   	   		err = Wcsstr_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcstok1L)
   			{
   	   		INFO_PRINTF1(_L("Wcstok1L():"));
   	   		err = Wcstok1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcstok2L)
   			{
   	   		INFO_PRINTF1(_L("Wcstok2L():"));
   	   		err = Wcstok2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcstok3L)
   			{
   	   		INFO_PRINTF1(_L("Wcstok3L():"));
   	   		err = Wcstok3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcstok4L)
   			{
   	   		INFO_PRINTF1(_L("Wcstok4L():"));
   	   		err = Wcstok4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcswcs_Test_1L)
   			{
   	   		INFO_PRINTF1(_L("Wcswcs_Test_1L():"));
   	   		err = Wcswcs_Test_1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcswcs_Test_2L)
   			{
   	   		INFO_PRINTF1(_L("Wcswcs_Test_2L():"));
   	   		err = Wcswcs_Test_2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcswcs_Test_3L)
   			{
   	   		INFO_PRINTF1(_L("Wcswcs_Test_3L():"));
   	   		err = Wcswcs_Test_3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == KWcswcs_Test_4L)
   			{
   	   		INFO_PRINTF1(_L("Wcswcs_Test_4L():"));
   	   		err = Wcswcs_Test_4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test0L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test0L():"));
   	   		err = wprintf_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test1L():"));
   	   		err = wprintf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test2L():"));
   	   		err = wprintf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test3L():"));
   	   		err = wprintf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test4L():"));
   	   		err = wprintf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test5L():"));
   	   		err = wprintf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test6L():"));
   	   		err = wprintf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test7L():"));
   	   		err = wprintf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test8L():"));
   	   		err = wprintf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test9L():"));
   	   		err = wprintf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test10L():"));
   	   		err = wprintf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test11L():"));
   	   		err = wprintf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test12L():"));
   	   		err = wprintf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test13L():"));
   	   		err = wprintf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test14L():"));
   	   		err = wprintf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test15L():"));
   	   		err = wprintf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test16L():"));
   	   		err = wprintf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test17L():"));
   	   		err = wprintf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test18L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test18L():"));
   	   		err = wprintf_Test18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test19L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test19L():"));
   	   		err = wprintf_Test19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test20L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test20L():"));
   	   		err = wprintf_Test20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwprintf_Test21L)
   			{
   	   		INFO_PRINTF1(_L("wprintf_Test21L():"));
   	   		err = wprintf_Test21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test1L():"));
   	   		err = fwprintf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test2L():"));
   	   		err = fwprintf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test3L():"));
   	   		err = fwprintf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test4L():"));
   	   		err = fwprintf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test5L():"));
   	   		err = fwprintf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test6L():"));
   	   		err = fwprintf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test7L():"));
   	   		err = fwprintf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test8L():"));
   	   		err = fwprintf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test9L():"));
   	   		err = fwprintf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test10L():"));
   	   		err = fwprintf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test11L():"));
   	   		err = fwprintf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test12L():"));
   	   		err = fwprintf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test13L():"));
   	   		err = fwprintf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test14L():"));
   	   		err = fwprintf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test15L():"));
   	   		err = fwprintf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test16L():"));
   	   		err = fwprintf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test17L():"));
   	   		err = fwprintf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test18L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test18L():"));
   	   		err = fwprintf_Test18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test19L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test19L():"));
   	   		err = fwprintf_Test19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test20L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test20L():"));
   	   		err = fwprintf_Test20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwprintf_Test21L)
   			{
   	   		INFO_PRINTF1(_L("fwprintf_Test21L():"));
   	   		err = fwprintf_Test21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test0L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test0L():"));
   	   		err = swprintf_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test1L():"));
   	   		err = swprintf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test2L():"));
   	   		err = swprintf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test3L():"));
   	   		err = swprintf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test4L():"));
   	   		err = swprintf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test5L():"));
   	   		err = swprintf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test6L():"));
   	   		err = swprintf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test7L():"));
   	   		err = swprintf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test8L():"));
   	   		err = swprintf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test9L():"));
   	   		err = swprintf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test10L():"));
   	   		err = swprintf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test11L():"));
   	   		err = swprintf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test12L():"));
   	   		err = swprintf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test13L():"));
   	   		err = swprintf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test14L():"));
   	   		err = swprintf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test15L():"));
   	   		err = swprintf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test16L():"));
   	   		err = swprintf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test17L():"));
   	   		err = swprintf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test18L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test18L():"));
   	   		err = swprintf_Test18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test19L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test19L():"));
   	   		err = swprintf_Test19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test20L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test20L():"));
   	   		err = swprintf_Test20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswprintf_Test21L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test21L():"));
   	   		err = swprintf_Test21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test0L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test0L():"));
   	   		err = vwprintf_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test1L():"));
   	   		err = vwprintf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test2L():"));
   	   		err = vwprintf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test3L():"));
   	   		err = vwprintf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test4L():"));
   	   		err = vwprintf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test5L():"));
   	   		err = vwprintf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test6L():"));
   	   		err = vwprintf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test7L():"));
   	   		err = vwprintf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test8L():"));
   	   		err = vwprintf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test9L():"));
   	   		err = vwprintf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test10L():"));
   	   		err = vwprintf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test11L():"));
   	   		err = vwprintf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test12L():"));
   	   		err = vwprintf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test13L():"));
   	   		err = vwprintf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test14L():"));
   	   		err = vwprintf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test15L():"));
   	   		err = vwprintf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test16L():"));
   	   		err = vwprintf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test17L():"));
   	   		err = vwprintf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test18L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test18L():"));
   	   		err = vwprintf_Test18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test19L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test19L():"));
   	   		err = vwprintf_Test19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test20L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test20L():"));
   	   		err = vwprintf_Test20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwprintf_Test21L)
   			{
   	   		INFO_PRINTF1(_L("vwprintf_Test21L():"));
   	   		err = vwprintf_Test21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test0L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test0L():"));
   	   		err = vfwprintf_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test1L():"));
   	   		err = vfwprintf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test2L():"));
   	   		err = vfwprintf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test3L():"));
   	   		err = vfwprintf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test4L():"));
   	   		err = vfwprintf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test5L():"));
   	   		err = vfwprintf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test6L():"));
   	   		err = vfwprintf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test7L():"));
   	   		err = vfwprintf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test8L():"));
   	   		err = vfwprintf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test9L():"));
   	   		err = vfwprintf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test10L():"));
   	   		err = vfwprintf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test11L():"));
   	   		err = vfwprintf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test12L():"));
   	   		err = vfwprintf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test13L():"));
   	   		err = vfwprintf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test14L():"));
   	   		err = vfwprintf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test15L():"));
   	   		err = vfwprintf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test16L():"));
   	   		err = vfwprintf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test17L():"));
   	   		err = vfwprintf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test18L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test18L():"));
   	   		err = vfwprintf_Test18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test19L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test19L():"));
   	   		err = vfwprintf_Test19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test20L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test20L():"));
   	   		err = vfwprintf_Test20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwprintf_Test21L)
   			{
   	   		INFO_PRINTF1(_L("vfwprintf_Test21L():"));
   	   		err = vfwprintf_Test21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test0L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test0L():"));
   	   		err = vswprintf_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test1L():"));
   	   		err = vswprintf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test2L():"));
   	   		err = vswprintf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test3L():"));
   	   		err = vswprintf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test4L():"));
   	   		err = vswprintf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test5L():"));
   	   		err = vswprintf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test6L():"));
   	   		err = vswprintf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test7L():"));
   	   		err = vswprintf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test8L():"));
   	   		err = vswprintf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test8L():"));
   	   		err = vswprintf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test9L():"));
   	   		err = vswprintf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test10L():"));
   	   		err = vswprintf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test11L():"));
   	   		err = vswprintf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test12L():"));
   	   		err = vswprintf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test13L():"));
   	   		err = vswprintf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test14L():"));
   	   		err = vswprintf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test15L():"));
   	   		err = vswprintf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test16L():"));
   	   		err = vswprintf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test17L():"));
   	   		err = vswprintf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test18L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test18L():"));
   	   		err = vswprintf_Test18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test19L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test19L():"));
   	   		err = vswprintf_Test19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test20L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test20L():"));
   	   		err = vswprintf_Test20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswprintf_Test21L)
   			{
   	   		INFO_PRINTF1(_L("vswprintf_Test21L():"));
   	   		err = vswprintf_Test21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test1L():"));
   	   		err = wscanf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test2L():"));
   	   		err = wscanf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test3L():"));
   	   		err = wscanf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test4L():"));
   	   		err = wscanf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test5L():"));
   	   		err = wscanf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test6L():"));
   	   		err = wscanf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test7L():"));
   	   		err = wscanf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test8L():"));
   	   		err = wscanf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test9L():"));
   	   		err = wscanf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test10L():"));
   	   		err = wscanf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test11L():"));
   	   		err = wscanf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test12L():"));
   	   		err = wscanf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test13L():"));
   	   		err = wscanf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test14L():"));
   	   		err = wscanf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test15L():"));
   	   		err = wscanf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test16L():"));
   	   		err = wscanf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwscanf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("wscanf_Test17L():"));
   	   		err = wscanf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test1L():"));
   	   		err = fwscanf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test2L():"));
   	   		err = fwscanf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test3L():"));
   	   		err = fwscanf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test4L():"));
   	   		err = fwscanf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   if(TestStepName() == Kfwscanf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test5L():"));
   	   		err = fwscanf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test6L():"));
   	   		err = fwscanf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test7L():"));
   	   		err = fwscanf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test8L():"));
   	   		err = fwscanf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test9L():"));
   	   		err = fwscanf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test10L():"));
   	   		err = fwscanf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test11L():"));
   	   		err = fwscanf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test12L():"));
   	   		err = fwscanf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test13L():"));
   	   		err = fwscanf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test14L():"));
   	   		err = fwscanf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test15L():"));
   	   		err = fwscanf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kfwscanf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("fwscanf_Test16L():"));
   	   		err = fwscanf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test1L():"));
   	   		err = swscanf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test2L():"));
   	   		err = swscanf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test3L():"));
   	   		err = swscanf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test4L():"));
   	   		err = swscanf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test5L():"));
   	   		err = swscanf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test6L():"));
   	   		err = swscanf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test7L():"));
   	   		err = swscanf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test8L():"));
   	   		err = swscanf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test9L():"));
   	   		err = swscanf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test10L():"));
   	   		err = swscanf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test11L():"));
   	   		err = swscanf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test12L():"));
   	   		err = swscanf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test13L():"));
   	   		err = swscanf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test14L():"));
   	   		err = swscanf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test15L():"));
   	   		err = swscanf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test16L():"));
   	   		err = swscanf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kswscanf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("swscanf_Test17L():"));
   	   		err = swscanf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test1L():"));
   	   		err = vwscanf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test2L():"));
   	   		err = vwscanf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test3L():"));
   	   		err = vwscanf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test4L():"));
   	   		err = vwscanf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test5L():"));
   	   		err = vwscanf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test6L():"));
   	   		err = vwscanf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test7L():"));
   	   		err = vwscanf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test8L():"));
   	   		err = vwscanf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test9L():"));
   	   		err = vwscanf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test10L():"));
   	   		err = vwscanf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test11L():"));
   	   		err = vwscanf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test12L():"));
   	   		err = vwscanf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test13L():"));
   	   		err = vwscanf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test14L():"));
   	   		err = vwscanf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test15L():"));
   	   		err = vwscanf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test16L():"));
   	   		err = vwscanf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvwscanf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("vwscanf_Test17L():"));
   	   		err = vwscanf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test1L():"));
   	   		err = vfwscanf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test2L():"));
   	   		err = vfwscanf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test3L():"));
   	   		err = vfwscanf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test4L():"));
   	   		err = vfwscanf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test5L():"));
   	   		err = vfwscanf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test6L():"));
   	   		err = vfwscanf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test7L():"));
   	   		err = vfwscanf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test8L():"));
   	   		err = vfwscanf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test9L():"));
   	   		err = vfwscanf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test10L():"));
   	   		err = vfwscanf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test11L():"));
   	   		err = vfwscanf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test12L():"));
   	   		err = vfwscanf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test13L():"));
   	   		err = vfwscanf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test14L():"));
   	   		err = vfwscanf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test15L():"));
   	   		err = vfwscanf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvfwscanf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("vfwscanf_Test16L():"));
   	   		err = vfwscanf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test1L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test1L():"));
   	   		err = vswscanf_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test2L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test2L():"));
   	   		err = vswscanf_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test3L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test3L():"));
   	   		err = vswscanf_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test4L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test4L():"));
   	   		err = vswscanf_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test5L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test5L():"));
   	   		err = vswscanf_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test6L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test6L():"));
   	   		err = vswscanf_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test7L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test7L():"));
   	   		err = vswscanf_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test8L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test8L():"));
   	   		err = vswscanf_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test9L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test9L():"));
   	   		err = vswscanf_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test10L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test10L():"));
   	   		err = vswscanf_Test10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test11L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test11L():"));
   	   		err = vswscanf_Test11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test12L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test12L():"));
   	   		err = vswscanf_Test12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test13L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test13L():"));
   	   		err = vswscanf_Test13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test14L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test14L():"));
   	   		err = vswscanf_Test14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test15L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test15L():"));
   	   		err = vswscanf_Test15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test16L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test16L():"));
   	   		err = vswscanf_Test16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kvswscanf_Test17L)
   			{
   	   		INFO_PRINTF1(_L("vswscanf_Test17L():"));
   	   		err = vswscanf_Test17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test0L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test0L():"));
   	   		err = mblen_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test1L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test1L():"));
   	   		err = mblen_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test2L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test2L():"));
   	   		err = mblen_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test3L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test3L():"));
   	   		err = mblen_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test4L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test4L():"));
   	   		err = mblen_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test5L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test5L():"));
   	   		err = mblen_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test6L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test6L():"));
   	   		err = mblen_Test6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test7L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test7L():"));
   	   		err = mblen_Test7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test8L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test8L():"));
   	   		err = mblen_Test8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmblen_Test9L)
   			{
   	   		INFO_PRINTF1(_L("mblen_Test9L():"));
   	   		err = mblen_Test9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwctomb_Test0L)
   			{
   	   		INFO_PRINTF1(_L("wctomb_Test0L():"));
   	   		err = wctomb_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwctomb_Test1L)
   			{
   	   		INFO_PRINTF1(_L("wctomb_Test1L():"));
   	   		err = wctomb_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwctomb_Test2L)
   			{
   	   		INFO_PRINTF1(_L("wctomb_Test2L():"));
   	   		err = wctomb_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwctomb_Test3L)
   			{
   	   		INFO_PRINTF1(_L("wctomb_Test3L():"));
   	   		err = wctomb_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwctomb_Test4L)
   			{
   	   		INFO_PRINTF1(_L("wctomb_Test4L():"));
   	   		err = wctomb_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwctomb_Test5L)
   			{
   	   		INFO_PRINTF1(_L("wctomb_Test5L():"));
   	   		err = wctomb_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstombs_Test0L)
   			{
   	   		INFO_PRINTF1(_L("wcstombs_Test0L():"));
   	   		err = wcstombs_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstombs_Test1L)
   			{
   	   		INFO_PRINTF1(_L("wcstombs_Test1L():"));
   	   		err = wcstombs_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstombs_Test2L)
   			{
   	   		INFO_PRINTF1(_L("wcstombs_Test2L():"));
   	   		err = wcstombs_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstombs_Test3L)
   			{
   	   		INFO_PRINTF1(_L("wcstombs_Test3L():"));
   	   		err = wcstombs_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstombs_Test4L)
   			{
   	   		INFO_PRINTF1(_L("wcstombs_Test4L():"));
   	   		err = wcstombs_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstombs_Test5L)
   			{
   	   		INFO_PRINTF1(_L("wcstombs_Test5L():"));
   	   		err = wcstombs_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbtowc_Test0L)
   			{
   	   		INFO_PRINTF1(_L("mbtowc_Test0L():"));
   	   		err = mbtowc_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbtowc_Test1L)
   			{
   	   		INFO_PRINTF1(_L("mbtowc_Test1L():"));
   	   		err = mbtowc_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbtowc_Test2L)
   			{
   	   		INFO_PRINTF1(_L("mbtowc_Test2L():"));
   	   		err = mbtowc_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbtowc_Test3L)
   			{
   	   		INFO_PRINTF1(_L("mbtowc_Test3L():"));
   	   		err = mbtowc_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbtowc_Test4L)
   			{
   	   		INFO_PRINTF1(_L("mbtowc_Test4L():"));
   	   		err = mbtowc_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbtowc_Test5L)
   			{
   	   		INFO_PRINTF1(_L("mbtowc_Test5L():"));
   	   		err = mbtowc_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbstowcs_Test0L)
   			{
   	   		INFO_PRINTF1(_L("mbstowcs_Test0L():"));
   	   		err = mbstowcs_Test0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbstowcs_Test1L)
   			{
   	   		INFO_PRINTF1(_L("mbstowcs_Test1L():"));
   	   		err = mbstowcs_Test1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbstowcs_Test2L)
   			{
   	   		INFO_PRINTF1(_L("mbstowcs_Test2L():"));
   	   		err = mbstowcs_Test2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbstowcs_Test3L)
   			{
   	   		INFO_PRINTF1(_L("mbstowcs_Test3L():"));
   	   		err = mbstowcs_Test3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbstowcs_Test4L)
   			{
   	   		INFO_PRINTF1(_L("mbstowcs_Test4L():"));
   	   		err = mbstowcs_Test4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kmbstowcs_Test5L)
   			{
   	   		INFO_PRINTF1(_L("mbstowcs_Test5L():"));
   	   		err = mbstowcs_Test5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcrtomb2400)
   			{
   	   		INFO_PRINTF1(_L("wcrtomb2400():"));
   	   		err = wcrtomb2400();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcrtomb2401)
   			{
   	   		INFO_PRINTF1(_L("wcrtomb2401():"));
   	   		err = wcrtomb2401();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcrtomb2402)
   			{
   	   		INFO_PRINTF1(_L("wcrtomb2402():"));
   	   		err = wcrtomb2402();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcrtomb2403)
   			{
   	   		INFO_PRINTF1(_L("wcrtomb2403():"));
   	   		err = wcrtomb2403();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcrtomb2404)
   			{
   	   		INFO_PRINTF1(_L("wcrtomb2404():"));
   	   		err = wcrtomb2404();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcrtomb2405)
   			{
   	   		INFO_PRINTF1(_L("wcrtomb2405():"));
   	   		err = wcrtomb2405();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcrtomb2406)
   			{
   	   		INFO_PRINTF1(_L("wcrtomb2406():"));
   	   		err = wcrtomb2406();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcrtomb2407)
   			{
   	   		INFO_PRINTF1(_L("wcrtomb2407():"));
   	   		err = wcrtomb2407();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcrtomb2672)
   			{
   	   		INFO_PRINTF1(_L("wcrtomb2672():"));
   	   		err = wcrtomb2672();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2409)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2409():"));
   	   		err = wcsrtombs2409();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2410)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2410():"));
   	   		err = wcsrtombs2410();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2411)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2411():"));
   	   		err = wcsrtombs2411();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2412)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2412():"));
   	   		err = wcsrtombs2412();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2413)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2413():"));
   	   		err = wcsrtombs2413();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2414)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2414():"));
   	   		err = wcsrtombs2414();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2415)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2415():"));
   	   		err = wcsrtombs2415();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2423)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2423():"));
   	   		err = wcsrtombs2423();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2436)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2436():"));
   	   		err = wcsrtombs2436();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2437)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2437():"));
   	   		err = wcsrtombs2437();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2438)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2438():"));
   	   		err = wcsrtombs2438();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2439)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2439():"));
   	   		err = wcsrtombs2439	();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2440)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2440():"));
   	   		err = wcsrtombs2440();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2442)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2442():"));
   	   		err = wcsrtombs2442();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2444)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2444():"));
   	   		err = wcsrtombs2444();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2460)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2460():"));
   	   		err = wcsrtombs2460();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsrtombs2673)
   			{
   	   		INFO_PRINTF1(_L("wcsrtombs2673():"));
   	   		err = wcsrtombs2673();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestbtowc1L)
   			{
   	   		INFO_PRINTF1(_L("testbtowc1L():"));
   	   		err = testbtowc1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestbtowc2L)
   			{
   	   		INFO_PRINTF1(_L("testbtowc2L():"));
   	   		err = testbtowc2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestbtowc3L)
   			{
   	   		INFO_PRINTF1(_L("testbtowc3L():"));
   	   		err = testbtowc3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwctob1L)
   			{
   	   		INFO_PRINTF1(_L("testwctob1L():"));
   	   		err = testwctob1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwctob2L)
   			{
   	   		INFO_PRINTF1(_L("testwctob2L():"));
   	   		err = testwctob2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwctob3L)
   			{
   	   		INFO_PRINTF1(_L("testwctob3L():"));
   	   		err = testwctob3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktesttowlower1L)
   			{
   	   		INFO_PRINTF1(_L("testtowlower1L():"));
   	   		err = testtowlower1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktesttowlower2L)
   			{
   	   		INFO_PRINTF1(_L("testtowlower2L():"));
   	   		err = testtowlower2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktesttowlower3L)
   			{
   	   		INFO_PRINTF1(_L("testtowlower3L():"));
   	   		err = testtowlower3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktesttowupper1L)
   			{
   	   		INFO_PRINTF1(_L("testtowupper1L():"));
   	   		err = testtowupper1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktesttowupper2L)
   			{
   	   		INFO_PRINTF1(_L("testtowupper2L():"));
   	   		err = testtowupper2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktesttowupper3L)
   			{
   	   		INFO_PRINTF1(_L("testtowupper3L():"));
   	   		err = testtowupper3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsrtowcs1L)
   			{
   	   		INFO_PRINTF1(_L("testmbsrtowcs1L():"));
   	   		err = testmbsrtowcs1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsrtowcs2L)
   			{
   	   		INFO_PRINTF1(_L("testmbsrtowcs2L():"));
   	   		err = testmbsrtowcs2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsrtowcs3L)
   			{
   	   		INFO_PRINTF1(_L("testmbsrtowcs3L():"));
   	   		err = testmbsrtowcs3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsrtowcs4L)
   			{
   	   		INFO_PRINTF1(_L("testmbsrtowcs4L():"));
   	   		err = testmbsrtowcs4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsrtowcs5L)
   			{
   	   		INFO_PRINTF1(_L("testmbsrtowcs5L():"));
   	   		err = testmbsrtowcs5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrtowc1L)
   			{
   	   		INFO_PRINTF1(_L("testmbrtowc1L():"));
   	   		err = testmbrtowc1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrtowc2L)
   			{
   	   		INFO_PRINTF1(_L("testmbrtowc2L():"));
   	   		err = testmbrtowc2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrtowc3L)
   			{
   	   		INFO_PRINTF1(_L("testmbrtowc3L():"));
   	   		err = testmbrtowc3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrtowc4L)
   			{
   	   		INFO_PRINTF1(_L("testmbrtowc4L():"));
   	   		err = testmbrtowc4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrtowc5L)
   			{
   	   		INFO_PRINTF1(_L("testmbrtowc5L():"));
   	   		err = testmbrtowc5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrtowc6L)
   			{
   	   		INFO_PRINTF1(_L("testmbrtowc6L():"));
   	   		err = testmbrtowc6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen0L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen0L():"));
   	   		err = testmbrlen0L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen1L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen1L():"));
   	   		err = testmbrlen1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen2L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen2L():"));
   	   		err = testmbrlen2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen3L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen3L():"));
   	   		err = testmbrlen3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen4L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen4L():"));
   	   		err = testmbrlen4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen5L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen5L():"));
   	   		err = testmbrlen5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen6L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen6L():"));
   	   		err = testmbrlen6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen7L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen7L():"));
   	   		err = testmbrlen7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen8L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen8L():"));
   	   		err = testmbrlen8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbrlen9L)
   			{
   	   		INFO_PRINTF1(_L("testmbrlen9L():"));
   	   		err = testmbrlen9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstof1L)
   			{
   	   		INFO_PRINTF1(_L("Kwcstof1L():"));
   	   		err = wcstof1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstof2L)
   			{
   	   		INFO_PRINTF1(_L("wcstof2L():"));
   	   		err = wcstof2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstof3L)
   			{
   	   		INFO_PRINTF1(_L("wcstof3L():"));
   	   		err = wcstof3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstof4L)
   			{
   	   		INFO_PRINTF1(_L("wcstof4L():"));
   	   		err = wcstof4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcstof5L)
   			{
   	   		INFO_PRINTF1(_L("wcstof5L():"));
   	   		err = wcstof5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime1_aL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime1_aL():"));
   	   		err = wcsftime1_aL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime2_AL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime2_AL():"));
   	   		err = wcsftime2_AL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime3_bL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime3_bL():"));
   	   		err = wcsftime3_bL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime4_BL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime4_BL():"));
   	   		err = wcsftime4_BL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime5_cL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime5_cL():"));
   	   		err = wcsftime5_cL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime6_CL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime6_CL():"));
   	   		err = wcsftime6_CL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime7_dL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime7_dL():"));
   	   		err = wcsftime7_dL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime8_DL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime8_DL():"));
   	   		err = wcsftime8_DL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime9_eL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime9_eL():"));
   	   		err = wcsftime9_eL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime10_FL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime10_FL():"));
   	   		err = wcsftime10_FL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime11_hL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime11_hL():"));
   	   		err = wcsftime11_hL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime12_IL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime12_IL():"));
   	   		err = wcsftime12_IL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime13_jL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime13_jL():"));
   	   		err = wcsftime13_jL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime14_mL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime14_mL():"));
   	   		err = wcsftime14_mL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime15_ML)
   			{
   	   		INFO_PRINTF1(_L("wcsftime15_ML():"));
   	   		err = wcsftime15_ML();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime16_nL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime16_nL():"));
   	   		err = wcsftime16_nL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime17_pL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime17_pL():"));
   	   		err = wcsftime17_pL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime18_rL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime18_rL():"));
   	   		err = wcsftime18_rL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime19_RL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime19_RL():"));
   	   		err = wcsftime19_RL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime20_SL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime20_SL():"));
   	   		err = wcsftime20_SL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime21_tL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime21_tL():"));
   	   		err = wcsftime21_tL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime22_TL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime22_TL():"));
   	   		err = wcsftime22_TL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime23_uL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime23_uL():"));
   	   		err = wcsftime23_uL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime25_wL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime25_wL():"));
   	   		err = wcsftime25_wL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime26_xL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime26_xL():"));
   	   		err = wcsftime26_xL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime27_XL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime27_XL():"));
   	   		err = wcsftime27_XL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Kwcsftime28_yL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime28_yL():"));
   	   		err = wcsftime28_yL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcsftime29_YL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime29_YL():"));
   	   		err = wcsftime29_YL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcsftime30L)
   			{
   	   		INFO_PRINTF1(_L("wcsftime30L():"));
   	   		err = wcsftime30L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcsftime_mixL)
   			{
   	   		INFO_PRINTF1(_L("wcsftime_mixL():"));
   	   		err = wcsftime_mixL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcsftime2L)
   			{
   	   		INFO_PRINTF1(_L("wcsftime2L():"));
   	   		err = wcsftime2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcstod1L)
   			{
   	   		INFO_PRINTF1(_L("wcstod1L():"));
   	   		err = wcstod1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcstod2L)
   			{
   	   		INFO_PRINTF1(_L("wcstod2L():"));
   	   		err = wcstod2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcstod3L)
   			{
   	   		INFO_PRINTF1(_L("wcstod3L():"));
   	   		err = wcstod3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcstod4L)
   			{
   	   		INFO_PRINTF1(_L("wcstod4L():"));
   	   		err = wcstod4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcstod5L)
   			{
   	   		INFO_PRINTF1(_L("wcstod5L():"));
   	   		err = wcstod5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcstold1L)
   			{
   	   		INFO_PRINTF1(_L("wcstold1L():"));
   	   		err = wcstold1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcstold2L)
   			{
   	   		INFO_PRINTF1(_L("wcstold2L():"));
   	   		err = wcstold2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcstold3L)
   			{
   	   		INFO_PRINTF1(_L("wcstold3L():"));
   	   		err = wcstold3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	
   	   	if(TestStepName() == Kwcstold4L)
   			{
   	   		INFO_PRINTF1(_L("wcstold4L():"));
   	   		err = wcstold4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	   	   	
   	   	if(TestStepName() == Ktestwcsnlen1L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnlen1L():"));
   	   		err = testwcsnlen1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsnlen2L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnlen2L():"));
   	   		err = testwcsnlen2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsnlen3L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnlen3L():"));
   	   		err = testwcsnlen3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsnlen4L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnlen4L():"));
   	   		err = testwcsnlen4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsnlen5L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnlen5L():"));
   	   		err = testwcsnlen5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsdup1L)
   			{
   	   		INFO_PRINTF1(_L("testwcsdup1L():"));
   	   		err = testwcsdup1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsdup2L)
   			{
   	   		INFO_PRINTF1(_L("testwcsdup2L():"));
   	   		err = testwcsdup2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcscasecmp1L)
   			{
   	   		INFO_PRINTF1(_L("testwcscasecmp1L():"));
   	   		err = testwcscasecmp1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcscasecmp2L)
   			{
   	   		INFO_PRINTF1(_L("testwcscasecmp2L():"));
   	   		err = testwcscasecmp2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcscasecmp3L)
   			{
   	   		INFO_PRINTF1(_L("testwcscasecmp3L():"));
   	   		err = testwcscasecmp3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcscasecmp4L)
   			{
   	   		INFO_PRINTF1(_L("testwcscasecmp4L():"));
   	   		err = testwcscasecmp4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcscasecmp5L)
   			{
   	   		INFO_PRINTF1(_L("testwcscasecmp5L():"));
   	   		err = testwcscasecmp5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcscasecmp6L)
   			{
   	   		INFO_PRINTF1(_L("testwcscasecmp6L():"));
   	   		err = testwcscasecmp6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsncasecmp1L)
   			{
   	   		INFO_PRINTF1(_L("testwcsncasecmp1L():"));
   	   		err = testwcsncasecmp1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsncasecmp2L)
   			{
   	   		INFO_PRINTF1(_L("testwcsncasecmp2L():"));
   	   		err = testwcsncasecmp2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsncasecmp3L)
   			{
   	   		INFO_PRINTF1(_L("testwcsncasecmp3L():"));
   	   		err = testwcsncasecmp3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsncasecmp4L)
   			{
   	   		INFO_PRINTF1(_L("testwcsncasecmp4L():"));
   	   		err = testwcsncasecmp4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsncasecmp5L)
   			{
   	   		INFO_PRINTF1(_L("testwcsncasecmp5L():"));
   	   		err = testwcsncasecmp5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsncasecmp6L)
   			{
   	   		INFO_PRINTF1(_L("testwcsncasecmp6L():"));
   	   		err = testwcsncasecmp6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsncasecmp7L)
   			{
   	   		INFO_PRINTF1(_L("testwcsncasecmp7L():"));
   	   		err = testwcsncasecmp7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcpcpy1L)
   			{
   	   		INFO_PRINTF1(_L("testwcpcpy1L():"));
   	   		err = testwcpcpy1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcpcpy2L)
   			{
   	   		INFO_PRINTF1(_L("testwcpcpy2L():"));
   	   		err = testwcpcpy2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcpncpy1L)
   			{
   	   		INFO_PRINTF1(_L("testwcpncpy1L():"));
   	   		err = testwcpncpy1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcpncpy2L)
   			{
   	   		INFO_PRINTF1(_L("testwcpncpy2L():"));
   	   		err = testwcpncpy2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcpncpy3L)
   			{
   	   		INFO_PRINTF1(_L("testwcpncpy3L():"));
   	   		err = testwcpncpy3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcpncpy4L)
   			{
   	   		INFO_PRINTF1(_L("testwcpncpy4L():"));
   	   		err = testwcpncpy4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsxfrm1L)
   			{
   	   		INFO_PRINTF1(_L("testwcsxfrm1L():"));
   	   		err = testwcsxfrm1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsxfrm2L)
   			{
   	   		INFO_PRINTF1(_L("testwcsxfrm2L():"));
   	   		err = testwcsxfrm2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul1L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul1L():"));
   	   		err = testwcstoul1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul2L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul2L():"));
   	   		err = testwcstoul2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul3L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul3L():"));
   	   		err = testwcstoul3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul4L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul4L():"));
   	   		err = testwcstoul4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul5L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul5L():"));
   	   		err = testwcstoul5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul6L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul6L():"));
   	   		err = testwcstoul6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul7L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul7L():"));
   	   		err = testwcstoul7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul8L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul8L():"));
   	   		err = testwcstoul8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul9L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul9L():"));
   	   		err = testwcstoul9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul10L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul10L():"));
   	   		err = testwcstoul10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul11L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul11L():"));
   	   		err = testwcstoul11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul12L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul12L():"));
   	   		err = testwcstoul12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul13L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul13L():"));
   	   		err = testwcstoul13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul14L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul14L():"));
   	   		err = testwcstoul14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul15L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul15L():"));
   	   		err = testwcstoul15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul16L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul16L():"));
   	   		err = testwcstoul16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoul17L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoul17L():"));
   	   		err = testwcstoul17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll1L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll1L():"));
   	   		err = testwcstoll1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll2L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll2L():"));
   	   		err = testwcstoll2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll3L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll3L():"));
   	   		err = testwcstoll3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll4L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll4L():"));
   	   		err = testwcstoll4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll5L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll5L():"));
   	   		err = testwcstoll5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll6L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll6L():"));
   	   		err = testwcstoll6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll7L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll7L():"));
   	   		err = testwcstoll7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll8L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll8L():"));
   	   		err = testwcstoll8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll9L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll9L():"));
   	   		err = testwcstoll9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll10L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll10L():"));
   	   		err = testwcstoll10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll11L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll11L():"));
   	   		err = testwcstoll11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll12L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll12L():"));
   	   		err = testwcstoll12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll13L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll13L():"));
   	   		err = testwcstoll13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll14L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll14L():"));
   	   		err = testwcstoll14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll15L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll15L():"));
   	   		err = testwcstoll15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll16L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll16L():"));
   	   		err = testwcstoll16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoll17L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoll17L():"));
   	   		err = testwcstoll17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull1L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull1L():"));
   	   		err = testwcstoull1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull2L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull2L():"));
   	   		err = testwcstoull2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull3L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull3L():"));
   	   		err = testwcstoull3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull4L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull4L():"));
   	   		err = testwcstoull4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull5L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull5L():"));
   	   		err = testwcstoull5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull6L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull6L():"));
   	   		err = testwcstoull6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull7L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull7L():"));
   	   		err = testwcstoull7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull8L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull8L():"));
   	   		err = testwcstoull8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull9L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull9L():"));
   	   		err = testwcstoull9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull10L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull10L():"));
   	   		err = testwcstoull10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull11L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull11L():"));
   	   		err = testwcstoull11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull12L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull12L():"));
   	   		err = testwcstoull12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull13L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull13L():"));
   	   		err = testwcstoull13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull14L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull14L():"));
   	   		err = testwcstoull14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull15L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull15L():"));
   	   		err = testwcstoull15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull16L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull16L():"));
   	   		err = testwcstoull16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoull17L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoull17L():"));
   	   		err = testwcstoull17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq1L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq1L():"));
   	   		err = testwcstoq1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq2L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq2L():"));
   	   		err = testwcstoq2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq3L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq3L():"));
   	   		err = testwcstoq3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq4L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq4L():"));
   	   		err = testwcstoq4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq5L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq5L():"));
   	   		err = testwcstoq5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq6L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq6L():"));
   	   		err = testwcstoq6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq7L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq7L():"));
   	   		err = testwcstoq7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq8L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq8L():"));
   	   		err = testwcstoq8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq9L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq9L():"));
   	   		err = testwcstoq9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq10L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq10L():"));
   	   		err = testwcstoq10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq11L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq11L():"));
   	   		err = testwcstoq11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq12L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq12L():"));
   	   		err = testwcstoq12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq13L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq13L():"));
   	   		err = testwcstoq13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq14L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq14L():"));
   	   		err = testwcstoq14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq15L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq15L():"));
   	   		err = testwcstoq15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq16L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq16L():"));
   	   		err = testwcstoq16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoq17L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoq17L():"));
   	   		err = testwcstoq17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq1L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq1L():"));
   	   		err = testwcstouq1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq2L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq2L():"));
   	   		err = testwcstouq2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq3L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq3L():"));
   	   		err = testwcstouq3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq4L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq4L():"));
   	   		err = testwcstouq4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq5L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq5L():"));
   	   		err = testwcstouq5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq6L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq6L():"));
   	   		err = testwcstouq6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq7L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq7L():"));
   	   		err = testwcstouq7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq8L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq8L():"));
   	   		err = testwcstouq8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq9L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq9L():"));
   	   		err = testwcstouq9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq10L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq10L():"));
   	   		err = testwcstouq10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq11L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq11L():"));
   	   		err = testwcstouq11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq12L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq12L():"));
   	   		err = testwcstouq12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq13L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq13L():"));
   	   		err = testwcstouq13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq14L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq14L():"));
   	   		err = testwcstouq14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq15L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq15L():"));
   	   		err = testwcstouq15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq16L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq16L():"));
   	   		err = testwcstouq16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstouq17L)
   			{
   	   		INFO_PRINTF1(_L("testwcstouq17L():"));
   	   		err = testwcstouq17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax1L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax1L():"));
   	   		err = testwcstoumax1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax2L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax2L():"));
   	   		err = testwcstoumax2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax3L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax3L():"));
   	   		err = testwcstoumax3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax4L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax4L():"));
   	   		err = testwcstoumax4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax5L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax5L():"));
   	   		err = testwcstoumax5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax6L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax6L():"));
   	   		err = testwcstoumax6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax7L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax7L():"));
   	   		err = testwcstoumax7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax8L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax8L():"));
   	   		err = testwcstoumax8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax9L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax9L():"));
   	   		err = testwcstoumax9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax10L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax10L():"));
   	   		err = testwcstoumax10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax11L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax11L():"));
   	   		err = testwcstoumax11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax12L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax12L():"));
   	   		err = testwcstoumax12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax13L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax13L():"));
   	   		err = testwcstoumax13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax14L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax14L():"));
   	   		err = testwcstoumax14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax15L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax15L():"));
   	   		err = testwcstoumax15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax16L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax16L():"));
   	   		err = testwcstoumax16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoumax17L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoumax17L():"));
   	   		err = testwcstoumax17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax1L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax1L():"));
   	   		err = testwcstoimax1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax2L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax2L():"));
   	   		err = testwcstoimax2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax3L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax3L():"));
   	   		err = testwcstoimax3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax4L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax4L():"));
   	   		err = testwcstoimax4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax5L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax5L():"));
   	   		err = testwcstoimax5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax6L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax6L():"));
   	   		err = testwcstoimax6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax7L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax7L():"));
   	   		err = testwcstoimax7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax8L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax8L():"));
   	   		err = testwcstoimax8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax9L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax9L():"));
   	   		err = testwcstoimax9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax10L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax10L():"));
   	   		err = testwcstoimax10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax11L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax11L():"));
   	   		err = testwcstoimax11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax12L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax12L():"));
   	   		err = testwcstoimax12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax13L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax13L():"));
   	   		err = testwcstoimax13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax14L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax14L():"));
   	   		err = testwcstoimax14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax15L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax15L():"));
   	   		err = testwcstoimax15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax16L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax16L():"));
   	   		err = testwcstoimax16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcstoimax17L)
   			{
   	   		INFO_PRINTF1(_L("testwcstoimax17L():"));
   	   		err = testwcstoimax17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsnrtowcs1L)
   			{
   	   		INFO_PRINTF1(_L("testmbsnrtowcs1L():"));
   	   		err = testmbsnrtowcs1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsnrtowcs2L)
   			{
   	   		INFO_PRINTF1(_L("testmbsnrtowcs2L():"));
   	   		err = testmbsnrtowcs2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsnrtowcs3L)
   			{
   	   		INFO_PRINTF1(_L("testmbsnrtowcs3L():"));
   	   		err = testmbsnrtowcs3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsnrtowcs4L)
   			{
   	   		INFO_PRINTF1(_L("testmbsnrtowcs4L():"));
   	   		err = testmbsnrtowcs4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestmbsnrtowcs5L)
   			{
   	   		INFO_PRINTF1(_L("testmbsnrtowcs5L():"));
   	   		err = testmbsnrtowcs5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsnrtombs1L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnrtombs1L():"));
   	   		err = testwcsnrtombs1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsnrtombs2L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnrtombs2L():"));
   	   		err = testwcsnrtombs2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsnrtombs3L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnrtombs3L():"));
   	   		err = testwcsnrtombs3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsnrtombs4L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnrtombs4L():"));
   	   		err = testwcsnrtombs4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
   	   	if(TestStepName() == Ktestwcsnrtombs5L)
   			{
   	   		INFO_PRINTF1(_L("testwcsnrtombs5L():"));
   	   		err = testwcsnrtombs5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}	
		if(TestStepName() == Kswprintf_Test22L)
   			{
   	   		INFO_PRINTF1(_L("swprintf_Test22L():"));
   	   		err = swprintf_Test22L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}   	      	
   	   	return TestStepResult(); 

	}
 


