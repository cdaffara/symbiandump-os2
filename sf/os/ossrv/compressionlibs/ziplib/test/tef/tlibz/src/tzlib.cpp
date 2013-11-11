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
// Name        : tzlib.cpp
// 
//

#include "tzlib.h"

CTestZlib::CTestZlib(const TDesC& aStepName)
	{
	iParamCnt=0;
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestZlib::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTestZlib::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestZlib::doTestStepL()
	{
	int err=KErrNone;

	// BEGIN testcases of tzlib.cpp
	if(TestStepName() == Ktest_zlibVersion)
   		{
   		INFO_PRINTF1(_L("Test_zlibVersion():"));
   		err = Test_zlibVersion();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_compress01)
   		{
   		INFO_PRINTF1(_L("Test_compress01():"));
   		err = Test_compress01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_deflate01)
		{
   		INFO_PRINTF1(_L("Test_deflate01():"));
   		err = Test_deflate01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_deflate02)
		{
   		INFO_PRINTF1(_L("Test_deflate02():"));
   		err = Test_deflate02();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_deflateEnd)
		{
   		INFO_PRINTF1(_L("Test_deflateEnd():"));
   		err = Test_deflateEnd();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_inflate01)
		{
   		INFO_PRINTF1(_L("Test_inflate01():"));
   		err = Test_inflate01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_inflate02)
		{
   		INFO_PRINTF1(_L("Test_inflate02():"));
   		err = Test_inflate02();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_inflate03)
		{
   		INFO_PRINTF1(_L("Test_inflate03():"));
   		err = Test_inflate03();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_inflate04)
		{
   		INFO_PRINTF1(_L("Test_inflate04():"));
   		err = Test_inflate04();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_inflate05)
		{
   		INFO_PRINTF1(_L("Test_inflate05():"));
   		err = Test_inflate05();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_inflate06)
		{
   		INFO_PRINTF1(_L("Test_inflate06():"));
   		err = Test_inflate06();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_inflateEnd)
		{
   		INFO_PRINTF1(_L("Test_inflateEnd():"));
   		err = Test_inflateEnd();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_deflateSetDictionary01)
		{
   		INFO_PRINTF1(_L("Test_deflateSetDictionary01():"));
   		err = Test_deflateSetDictionary01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_deflateSetDictionary02)
		{
   		INFO_PRINTF1(_L("Test_deflateSetDictionary02():"));
   		err = Test_deflateSetDictionary02();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_deflateSetDictionary03)
		{
   		INFO_PRINTF1(_L("Test_deflateSetDictionary03():"));
   		err = Test_deflateSetDictionary03();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_deflateSetDictionary04)
		{
   		INFO_PRINTF1(_L("Test_deflateSetDictionary04():"));
   		err = Test_deflateSetDictionary04();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_deflateSetDictionary05)
		{
   		INFO_PRINTF1(_L("Test_deflateSetDictionary05():"));
   		err = Test_deflateSetDictionary05();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   else if(TestStepName() == Ktest_gzio)
		{
   		INFO_PRINTF1(_L("Test_gzio():"));
   		err = Test_gzio();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_gzdirect)
		{
   		INFO_PRINTF1(_L("Test_gzdirect():"));
   		err = Test_gzdirect();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_gzdirectnull)
		{
   		INFO_PRINTF1(_L("Test_gzdirectnull():"));
   		err = Test_gzdirectnull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_gzclearerr)
		{
   		INFO_PRINTF1(_L("Test_gzclearerr():"));
   		err = Test_gzclearerr();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_gzclearerr_null)
		{
   		INFO_PRINTF1(_L("Test_gzclearerr_null():"));
   		err = Test_gzclearerr_null();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == Ktest_gzerror_streamend)
		{
   		INFO_PRINTF1(_L("Test_gzerror_streamend():"));
   		err = Test_gzerror_streamend();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == Ktest_gzungetcnegative)
		{
   		INFO_PRINTF1(_L("Test_gzungetcnegative():"));
   		err = Test_gzungetcnegative();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_gzseeknegative)
		{
   		INFO_PRINTF1(_L("Test_gzseeknegative():"));
   		err = Test_gzseeknegative();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktest_gzdirecttxt)
		{
   		INFO_PRINTF1(_L("Test_gzdirecttxt():"));
   		err = Test_gzdirecttxt();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzungetcChain)
		{
   		INFO_PRINTF1(_L("TestGzungetcChain():"));
   		err = TestGzungetcChain();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzseekBack)
		{
   		INFO_PRINTF1(_L("TestGzseekBack():"));
   		err = TestGzseekBack();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzseekAppend)
		{
   		INFO_PRINTF1(_L("TestGzseekAppend():"));
   		err = TestGzseekAppend();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzseekHugeOffset)
		{
   		INFO_PRINTF1(_L("TestGzseekHugeOffset():"));
   		err = TestGzseekHugeOffset();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzseekNoSize)
		{
   		INFO_PRINTF1(_L("TestGzseekNoSize():"));
   		err = TestGzseekNoSize();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzopenLongPath01)
		{
   		INFO_PRINTF1(_L("TestGzopenLongPath01():"));
   		err = TestGzopenLongPath01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzseekLongPath01)
		{
   		INFO_PRINTF1(_L("TestGzseekLongPath01():"));
   		err = TestGzseekLongPath01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzopenLongPath02)
		{
   		INFO_PRINTF1(_L("TestGzopenLongPath02():"));
   		err = TestGzopenLongPath02();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzseekMixedFile01)
		{
   		INFO_PRINTF1(_L("TestGzseekMixedFile01():"));
   		err = TestGzseekMixedFile01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzopenNoMode)
		{
   		INFO_PRINTF1(_L("TestGzopenNoMode():"));
   		err = TestGzopenNoMode();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzopenNoPath)
		{
   		INFO_PRINTF1(_L("TestGzopenNoPath():"));
   		err = TestGzopenNoPath();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzopenNoPathMode)
		{
   		INFO_PRINTF1(_L("TestGzopenNoPathMode():"));
   		err = TestGzopenNoPathMode();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzseekConcatedFile01)
		{
   		INFO_PRINTF1(_L("TestGzseekConcatedFile01():"));
   		err = TestGzseekConcatedFile01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzopenDiffMode)
		{
   		INFO_PRINTF1(_L("TestGzopenDiffMode():"));
   		err = TestGzopenDiffMode();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzseekConcatedFile02)
		{
   		INFO_PRINTF1(_L("TestGzseekConcatedFile02():"));
   		err = TestGzseekConcatedFile02();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzprintf01)
		{
   		INFO_PRINTF1(_L("TestGzprintf01():"));
   		err = TestGzprintf01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzprintf02)
		{
   		INFO_PRINTF1(_L("TestGzprintf02():"));
   		err = TestGzprintf02();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzflushNull)
		{
   		INFO_PRINTF1(_L("TestGzflushNull():"));
   		err = TestGzflushNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   else if(TestStepName() == KtestGzflushRepeat)
		{
   		INFO_PRINTF1(_L("TestGzflushRepeat():"));
   		err = TestGzflushRepeat();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzflushHugeBuf)
		{
   		INFO_PRINTF1(_L("TestGzflushHugeBuf():"));
   		err = TestGzflushHugeBuf();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzrewindNull)
		{
   		INFO_PRINTF1(_L("TestGzrewindNull():"));
   		err = TestGzrewindNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzrewindTransparent)
		{
   		INFO_PRINTF1(_L("TestGzrewindTransparent():"));
   		err = TestGzrewindTransparent();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzgetsBufNull)
		{
   		INFO_PRINTF1(_L("TestGzgetsBufNull():"));
   		err = TestGzgetsBufNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzgetsSmallBuf)
		{
   		INFO_PRINTF1(_L("TestGzgetsSmallBuf():"));
   		err = TestGzgetsSmallBuf();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}			// END testcases of tzlib.cpp
   	
   	//BEGIN testcases of tzlib_auto.cpp
   	else if(TestStepName() == KtestDeflateReset)
		{
   		INFO_PRINTF1(_L("TestDeflateReset():"));
   		err = TestDeflateReset();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestDeflateReset_fail)
		{
   		INFO_PRINTF1(_L("TestDeflateReset_fail():"));
   		err = TestDeflateReset_fail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestDeflateInit2_bits)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_bits():"));
   		err = TestDeflateInit2_bits();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestDeflateInit2_level)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_level():"));
   		err = TestDeflateInit2_level();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KtestInflateInit2_bits)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_bits():"));
   		err = TestInflateInit2_bits();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzread)
		{
   		INFO_PRINTF1(_L("TestGzread():"));
   		err = TestGzread();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzread_fail)
		{
   		INFO_PRINTF1(_L("TestGzread_fail():"));
   		err = TestGzread_fail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
   	else if(TestStepName() == KtestUncompress)
		{
   		INFO_PRINTF1(_L("TestUncompress():"));
   		err = TestUncompress();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestUncompressfail)
		{
   		INFO_PRINTF1(_L("TestUncompressfail():"));
   		err = TestUncompressfail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}		
	else if(TestStepName() == KtestInflateSetDictionary)
		{
   		INFO_PRINTF1(_L("TestInflateSetDictionary():"));
   		err = TestInflateSetDictionary();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestInflateSetDictionary_size)
		{
   		INFO_PRINTF1(_L("TestInflateSetDictionary_size():"));
   		err = TestInflateSetDictionary_size();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflateSetDictionary_null)
		{
   		INFO_PRINTF1(_L("TestInflateSetDictionary_null():"));
   		err = TestInflateSetDictionary_null();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzgets)
		{
   		INFO_PRINTF1(_L("TestGzgets():"));
   		err = TestGzgets();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestgzgetsFail)
		{
   		INFO_PRINTF1(_L("TestgzgetsFail():"));
   		err = TestgzgetsFail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestgzgetsopenFail)
		{
   		INFO_PRINTF1(_L("TestgzgetsopenFail():"));
   		err = TestgzgetsopenFail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflate)
		{
   		INFO_PRINTF1(_L("TestInflate():"));
   		err = TestInflate();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestInflate_fail1)
		{
   		INFO_PRINTF1(_L("TestInflate_fail1():"));
   		err = TestInflate_fail1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflate_fail2)
		{
   		INFO_PRINTF1(_L("TestInflate_fail2():"));
   		err = TestInflate_fail2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflate_fail3)
		{
   		INFO_PRINTF1(_L("TestInflate_fail3():"));
   		err = TestInflate_fail3();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestInflateend)
		{
   		INFO_PRINTF1(_L("TestInflateend():"));
   		err = TestInflateend();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflateend_fail)
		{
   		INFO_PRINTF1(_L("TestInflateend_fail():"));
   		err = TestInflateend_fail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflateReset)
		{
   		INFO_PRINTF1(_L("TestInflateReset():"));
   		err = TestInflateReset();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestInflateResetfail1)
		{
   		INFO_PRINTF1(_L("TestInflateResetfail1():"));
   		err = TestInflateResetfail1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflateInit2_)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_():"));
   		err = TestInflateInit2_();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestInflateInit_)
		{
   		INFO_PRINTF1(_L("TestInflateInit_():"));
   		err = TestInflateInit_();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflateInit2_negative)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_negative():"));
   		err = TestInflateInit2_negative();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}		
   	else if(TestStepName() == KtestInflateInit_negative)
		{
   		INFO_PRINTF1(_L("TestInflateInit_negative():"));
   		err = TestInflateInit_negative();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestInflateInit2_versioncheck)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_versioncheck():"));
   		err = TestInflateInit2_versioncheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflateInit_versioncheck)
		{
   		INFO_PRINTF1(_L("TestInflateInit_versioncheck():"));
   		err = TestInflateInit_versioncheck();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestAdlerinit)
		{
   		INFO_PRINTF1(_L("TestAdlerinit():"));
   		err = TestAdlerinit();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestAdler)
		{
   		INFO_PRINTF1(_L("TestAdler():"));
   		err = TestAdler();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestCompress)
		{
   		INFO_PRINTF1(_L("TestCompress():"));
   		err = TestCompress();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestCompress_negative)
		{
   		INFO_PRINTF1(_L("TestCompress_negative():"));
   		err = TestCompress_negative();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestCompress2_positive)
		{
   		INFO_PRINTF1(_L("TestCompress2_positive():"));
   		err = TestCompress2_positive();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestCompress2_negative)
		{
   		INFO_PRINTF1(_L("TestCompress2_negative():"));
   		err = TestCompress2_negative();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestCompressbound)
		{
   		INFO_PRINTF1(_L("TestCompressbound():"));
   		err = TestCompressbound();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestDeflatebound)
		{
   		INFO_PRINTF1(_L("TestDeflatebound():"));
   		err = TestDeflatebound();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflateparams)
		{
   		INFO_PRINTF1(_L("TestDeflateparams():"));
   		err = TestDeflateparams();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflateparamsfail1)
		{
   		INFO_PRINTF1(_L("TestDeflateparamsfail1():"));
   		err = TestDeflateparamsfail1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestDeflateparamsfail2)
		{
   		INFO_PRINTF1(_L("TestDeflateparamsfail2():"));
   		err = TestDeflateparamsfail2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestCrcinit)
		{
   		INFO_PRINTF1(_L("TestCrcinit():"));
   		err = TestCrcinit();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestCrc)
		{
   		INFO_PRINTF1(_L("TestCrc():"));
   		err = TestCrc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGet_crc_table)
		{
   		INFO_PRINTF1(_L("TestGet_crc_table():"));
   		err = TestGet_crc_table();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflateInit_)
		{
   		INFO_PRINTF1(_L("TestDeflateInit_():"));
   		err = TestDeflateInit_();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflateInit_level)
		{
   		INFO_PRINTF1(_L("TestDeflateInit_level():"));
   		err = TestDeflateInit_level();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflateInit2_)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_():"));
   		err = TestDeflateInit2_();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflatefail)
		{
   		INFO_PRINTF1(_L("TestDeflatefail():"));
   		err = TestDeflatefail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflatefail2)
		{
   		INFO_PRINTF1(_L("TestDeflatefail2():"));
   		err = TestDeflatefail2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestZlibversion)
		{
   		INFO_PRINTF1(_L("TestZlibversion():"));
   		err = TestZlibversion();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   else if(TestStepName() == KtestGzputc)
		{
   		INFO_PRINTF1(_L("TestGzputc():"));
   		err = TestGzputc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}		
   	else if(TestStepName() == KtestGzopen)
		{
   		INFO_PRINTF1(_L("TestGzopen():"));
   		err = TestGzopen();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzopenmode)
		{
   		INFO_PRINTF1(_L("TestGzopenmode():"));
   		err = TestGzopenmode();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzopenfail)
		{
   		INFO_PRINTF1(_L("TestGzopenfail():"));
   		err = TestGzopenfail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzputcfail)
		{
   		INFO_PRINTF1(_L("TestGzputcfail():"));
   		err = TestGzputcfail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzputcreturn)
		{
   		INFO_PRINTF1(_L("TestGzputcreturn():"));
   		err = TestGzputcreturn();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzputs)
		{
   		INFO_PRINTF1(_L("TestGzputs():"));
   		err = TestGzputs();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzputsfail)
		{
   		INFO_PRINTF1(_L("TestGzputsfail():"));
   		err = TestGzputsfail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzprintf)
		{
   		INFO_PRINTF1(_L("TestGzprintf():"));
   		err = TestGzprintf();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}		
   	else if(TestStepName() == KtestGzprintf_trying)
		{
   		INFO_PRINTF1(_L("TestGzprintf_trying():"));
   		err = TestGzprintf_trying();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzwrite)
		{
   		INFO_PRINTF1(_L("TestGzwrite():"));
   		err = TestGzwrite();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzwritefail)
		{
   		INFO_PRINTF1(_L("TestGzwritefail():"));
   		err = TestGzwritefail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGztell)
		{
   		INFO_PRINTF1(_L("TestGztell():"));
   		err = TestGztell();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGztell1)
		{
   		INFO_PRINTF1(_L("TestGztell1():"));
   		err = TestGztell1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGztellfail1)
		{
   		INFO_PRINTF1(_L("TestGztellfail1():"));
   		err = TestGztellfail1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestDeflatecopy)
		{
   		INFO_PRINTF1(_L("TestDeflatecopy():"));
   		err = TestDeflatecopy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflatecopyfail)
		{
   		INFO_PRINTF1(_L("TestDeflatecopyfail():"));
   		err = TestDeflatecopyfail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzclose)
		{
   		INFO_PRINTF1(_L("TestGzclose():"));
   		err = TestGzclose();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzclose_fail)
		{
   		INFO_PRINTF1(_L("TestGzclose_fail():"));
   		err = TestGzclose_fail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzeof)
		{
   		INFO_PRINTF1(_L("TestGzeof():"));
   		err = TestGzeof();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzeoffail1)
		{
   		INFO_PRINTF1(_L("TestGzeoffail1():"));
   		err = TestGzeoffail1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzeoffail2)
		{
   		INFO_PRINTF1(_L("TestGzeoffail2():"));
   		err = TestGzeoffail2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}				
   	else if(TestStepName() == KtestGzgetc)
		{
   		INFO_PRINTF1(_L("TestGzgetc():"));
   		err = TestGzgetc();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzflush)
		{
   		INFO_PRINTF1(_L("TestGzflush():"));
   		err = TestGzflush();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzflushsync)
		{
   		INFO_PRINTF1(_L("TestGzflushsync():"));
   		err = TestGzflushsync();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzflushfail)
		{
   		INFO_PRINTF1(_L("TestGzflushfail():"));
   		err = TestGzflushfail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzerror)
		{
   		INFO_PRINTF1(_L("TestGzerror():"));
   		err = TestGzerror();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzerrorfail1)
		{
   		INFO_PRINTF1(_L("TestGzerrorfail1():"));
   		err = TestGzerrorfail1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzgetcfail)
		{
   		INFO_PRINTF1(_L("TestGzgetcfail():"));
   		err = TestGzgetcfail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflateSetDictionary)
		{
   		INFO_PRINTF1(_L("TestDeflateSetDictionary():"));
   		err = TestDeflateSetDictionary();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflateSetDictionary_nodict)
		{
   		INFO_PRINTF1(_L("TestDeflateSetDictionary_nodict():"));
   		err = TestDeflateSetDictionary_nodict();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestDeflateSetDictionary_fail)
		{
   		INFO_PRINTF1(_L("TestDeflateSetDictionary_fail():"));
   		err = TestDeflateSetDictionary_fail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflateend)
		{
   		INFO_PRINTF1(_L("TestDeflateend():"));
   		err = TestDeflateend();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestDeflateendfail1)
		{
   		INFO_PRINTF1(_L("TestDeflateendfail1():"));
   		err = TestDeflateendfail1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestDeflate)
		{
   		INFO_PRINTF1(_L("TestDeflate():"));
   		err = TestDeflate();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzseek)
		{
   		INFO_PRINTF1(_L("TestGzseek():"));
   		err = TestGzseek();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzseekfail1)
		{
   		INFO_PRINTF1(_L("TestGzseekfail1():"));
   		err = TestGzseekfail1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzseekfail2)
		{
   		INFO_PRINTF1(_L("TestGzseekfail2():"));
   		err = TestGzseekfail2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzsetparams)
		{
   		INFO_PRINTF1(_L("TestGzsetparams():"));
   		err = TestGzsetparams();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzsetparams_fail1)
		{
   		INFO_PRINTF1(_L("TestGzsetparams_fail1():"));
   		err = TestGzsetparams_fail1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzsetparams_fail2)
		{
   		INFO_PRINTF1(_L("TestGzsetparams_fail2():"));
   		err = TestGzsetparams_fail2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzsetparams_fail3)
		{
   		INFO_PRINTF1(_L("TestGzsetparams_fail3():"));
   		err = TestGzsetparams_fail3();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzrewind)
		{
   		INFO_PRINTF1(_L("TestGzrewind():"));
   		err = TestGzrewind();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzrewindfail)
		{
   		INFO_PRINTF1(_L("TestGzrewindfail():"));
   		err = TestGzrewindfail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestGzdopen)
		{
   		INFO_PRINTF1(_L("TestGzdopen():"));
   		err = TestGzdopen();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzdopen_fail)
		{
   		INFO_PRINTF1(_L("TestGzdopen_fail():"));
   		err = TestGzdopen_fail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestGzdopen_fail2)
		{
   		INFO_PRINTF1(_L("TestGzdopen_fail2():"));
   		err = TestGzdopen_fail2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestInflateSync)
		{
   		INFO_PRINTF1(_L("TestInflateSync():"));
   		err = TestInflateSync();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestinflateSyncfail)
		{
   		INFO_PRINTF1(_L("TestinflateSyncfail():"));
   		err = TestinflateSyncfail();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestInflateSyncPoint)
		{
   		INFO_PRINTF1(_L("TestInflateSyncPoint():"));
   		err = TestInflateSyncPoint();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestInflateSyncPoint_null)
		{
   		INFO_PRINTF1(_L("TestInflateSyncPoint_null():"));
   		err = TestInflateSyncPoint_null();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KtestZerror)
		{
   		INFO_PRINTF1(_L("TestZerror():"));
   		err = TestZerror();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}	
   	else if(TestStepName() == KtestZerror1)
		{
   		INFO_PRINTF1(_L("TestZerror1():"));
   		err = TestZerror1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}		
   	else if(TestStepName() == KtestZerror2)
		{
   		INFO_PRINTF1(_L("TestZerror2():"));
   		err = TestZerror2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}				//END testcases of tzlib_auto.cpp
   	
   	// BEGIN testcases of tzlibblocks.cpp
   	else if(TestStepName() == KTestDeflateTest01)
		{
   		INFO_PRINTF1(_L("TestDeflateTest01():"));
   		err = TestDeflateTest01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflatecopyDestNull)
		{
   		INFO_PRINTF1(_L("TestDeflatecopyDestNull():"));
   		err = TestDeflatecopyDestNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateCopyStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateCopyStreamStateNull():"));
   		err = TestDeflateCopyStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_WindowBits)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_WindowBits():"));
   		err = TestDeflateInit2_WindowBits();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_StreamSize)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_StreamSize():"));
   		err = TestDeflateInit2_StreamSize();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_MemLevel)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_MemLevel():"));
   		err = TestDeflateInit2_MemLevel();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_Level)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_Level():"));
   		err = TestDeflateInit2_Level();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_Strategy)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_Strategy():"));
   		err = TestDeflateInit2_Strategy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_Version)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_Version():"));
   		err = TestDeflateInit2_Version();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_VersionNull)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_VersionNull():"));
   		err = TestDeflateInit2_VersionNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_StreamNull)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_StreamNull():"));
   		err = TestDeflateInit2_StreamNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateInit2_Version)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_Version():"));
   		err = TestInflateInit2_Version();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateInit2_VersionNull)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_VersionNull():"));
   		err = TestInflateInit2_VersionNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateInit2_WindowBits)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_WindowBits():"));
   		err = TestInflateInit2_WindowBits();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateInit2_StreamNull)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_StreamNull():"));
   		err = TestInflateInit2_StreamNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_HuffmanStrategy)
		{
   		INFO_PRINTF1(_L("TestDeflate_HuffmanStrategy():"));
   		err = TestDeflate_HuffmanStrategy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_AvailInZero)
		{
   		INFO_PRINTF1(_L("TestDeflate_AvailInZero():"));
   		err = TestDeflate_AvailInZero();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzsetparamDefaultCompression)
		{
   		INFO_PRINTF1(_L("TestGzsetparamDefaultCompression():"));
   		err = TestGzsetparamDefaultCompression();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateResetNullStream)
		{
   		INFO_PRINTF1(_L("TestDeflateResetNullStream():"));
   		err = TestDeflateResetNullStream();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateResetStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateResetStreamStateNull():"));
   		err = TestDeflateResetStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_Scenarios)
		{
   		INFO_PRINTF1(_L("TestDeflate_Scenarios():"));
   		err = TestDeflate_Scenarios();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_NullStream)
		{
   		INFO_PRINTF1(_L("TestDeflate_NullStream():"));
   		err = TestDeflate_NullStream();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_StreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflate_StreamStateNull():"));
   		err = TestDeflate_StreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateEndNull)
		{
   		INFO_PRINTF1(_L("TestDeflateEndNull():"));
   		err = TestDeflateEndNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateEndStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateEndStreamStateNull():"));
   		err = TestDeflateEndStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_WindowBits)
		{
   		INFO_PRINTF1(_L("TestDeflate_WindowBits():"));
   		err = TestDeflate_WindowBits();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateBoundStreamNotNull)
		{
   		INFO_PRINTF1(_L("TestDeflateBoundStreamNotNull():"));
   		err = TestDeflateBoundStreamNotNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateBoundStreamNull)
		{
   		INFO_PRINTF1(_L("TestDeflateBoundStreamNull():"));
   		err = TestDeflateBoundStreamNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateBoundStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateBoundStreamStateNull():"));
   		err = TestDeflateBoundStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateSetDictionaryWrap)
		{
   		INFO_PRINTF1(_L("TestDeflateSetDictionaryWrap():"));
   		err = TestDeflateSetDictionaryWrap();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateSetDictionaryLen)
		{
   		INFO_PRINTF1(_L("TestDeflateSetDictionaryLen():"));
   		err = TestDeflateSetDictionaryLen();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateParamsStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateParamsStreamStateNull():"));
   		err = TestDeflateParamsStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateSyncAvailInNull)
		{
   		INFO_PRINTF1(_L("TestInflateSyncAvailInNull():"));
   		err = TestInflateSyncAvailInNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateSyncStreamNull)
		{
   		INFO_PRINTF1(_L("TestInflateSyncStreamNull():"));
   		err = TestInflateSyncStreamNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateSyncStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestInflateSyncStreamStateNull():"));
   		err = TestInflateSyncStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestAdlerScenarios)
		{
   		INFO_PRINTF1(_L("TestAdlerScenarios():"));
   		err = TestAdlerScenarios();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzsetparamsFileNull)
		{
   		INFO_PRINTF1(_L("TestGzsetparamsFileNull():"));
   		err = TestGzsetparamsFileNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateTest01)
		{
   		INFO_PRINTF1(_L("TestDeflateTest01():"));
   		err = TestDeflateTest01();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflatecopyDestNull)
		{
   		INFO_PRINTF1(_L("TestDeflatecopyDestNull():"));
   		err = TestDeflatecopyDestNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateCopyStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateCopyStreamStateNull():"));
   		err = TestDeflateCopyStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_WindowBits)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_WindowBits():"));
   		err = TestDeflateInit2_WindowBits();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_StreamSize)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_StreamSize():"));
   		err = TestDeflateInit2_StreamSize();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_MemLevel)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_MemLevel():"));
   		err = TestDeflateInit2_MemLevel();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_Level)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_Level():"));
   		err = TestDeflateInit2_Level();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_Strategy)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_Strategy():"));
   		err = TestDeflateInit2_Strategy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_Version)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_Version():"));
   		err = TestDeflateInit2_Version();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_VersionNull)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_VersionNull():"));
   		err = TestDeflateInit2_VersionNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateInit2_StreamNull)
		{
   		INFO_PRINTF1(_L("TestDeflateInit2_StreamNull():"));
   		err = TestDeflateInit2_StreamNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateInit2_Version)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_Version():"));
   		err = TestInflateInit2_Version();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateInit2_VersionNull)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_VersionNull():"));
   		err = TestInflateInit2_VersionNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateInit2_WindowBits)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_WindowBits():"));
   		err = TestInflateInit2_WindowBits();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateInit2_StreamNull)
		{
   		INFO_PRINTF1(_L("TestInflateInit2_StreamNull():"));
   		err = TestInflateInit2_StreamNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_HuffmanStrategy)
		{
   		INFO_PRINTF1(_L("TestDeflate_HuffmanStrategy():"));
   		err = TestDeflate_HuffmanStrategy();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_AvailInZero)
		{
   		INFO_PRINTF1(_L("TestDeflate_AvailInZero():"));
   		err = TestDeflate_AvailInZero();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzsetparamDefaultCompression)
		{
   		INFO_PRINTF1(_L("TestGzsetparamDefaultCompression():"));
   		err = TestGzsetparamDefaultCompression();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateResetNullStream)
		{
   		INFO_PRINTF1(_L("TestDeflateResetNullStream():"));
   		err = TestDeflateResetNullStream();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateResetStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateResetStreamStateNull():"));
   		err = TestDeflateResetStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_Scenarios)
		{
   		INFO_PRINTF1(_L("TestDeflate_Scenarios():"));
   		err = TestDeflate_Scenarios();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_NullStream)
		{
   		INFO_PRINTF1(_L("TestDeflate_NullStream():"));
   		err = TestDeflate_NullStream();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_StreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflate_StreamStateNull():"));
   		err = TestDeflate_StreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateEndNull)
		{
   		INFO_PRINTF1(_L("TestDeflateEndNull():"));
   		err = TestDeflateEndNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateEndStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateEndStreamStateNull():"));
   		err = TestDeflateEndStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflate_WindowBits)
		{
   		INFO_PRINTF1(_L("TestDeflate_WindowBits():"));
   		err = TestDeflate_WindowBits();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateBoundStreamNotNull)
		{
   		INFO_PRINTF1(_L("TestDeflateBoundStreamNotNull():"));
   		err = TestDeflateBoundStreamNotNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateBoundStreamNull)
		{
   		INFO_PRINTF1(_L("TestDeflateBoundStreamNull():"));
   		err = TestDeflateBoundStreamNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateBoundStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateBoundStreamStateNull():"));
   		err = TestDeflateBoundStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateSetDictionaryWrap)
		{
   		INFO_PRINTF1(_L("TestDeflateSetDictionaryWrap():"));
   		err = TestDeflateSetDictionaryWrap();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateSetDictionaryLen)
		{
   		INFO_PRINTF1(_L("TestDeflateSetDictionaryLen():"));
   		err = TestDeflateSetDictionaryLen();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateSetDictionaryBadMode)
		{
   		INFO_PRINTF1(_L("TestInflateSetDictionaryBadMode():"));
   		err = TestInflateSetDictionaryBadMode();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateSetDictionaryStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestInflateSetDictionaryStreamStateNull():"));
   		err = TestInflateSetDictionaryStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestDeflateParamsStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestDeflateParamsStreamStateNull():"));
   		err = TestDeflateParamsStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateSyncAvailInNull)
		{
   		INFO_PRINTF1(_L("TestInflateSyncAvailInNull():"));
   		err = TestInflateSyncAvailInNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateSyncStreamNull)
		{
   		INFO_PRINTF1(_L("TestInflateSyncStreamNull():"));
   		err = TestInflateSyncStreamNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateSyncStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestInflateSyncStreamStateNull():"));
   		err = TestInflateSyncStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateSyncPointStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestInflateSyncPointStreamStateNull():"));
   		err = TestInflateSyncPointStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestAdlerScenarios)
		{
   		INFO_PRINTF1(_L("TestAdlerScenarios():"));
   		err = TestAdlerScenarios();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzsetparamsFileNull)
		{
   		INFO_PRINTF1(_L("TestGzsetparamsFileNull():"));
   		err = TestGzsetparamsFileNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzopenWrite)
		{
   		INFO_PRINTF1(_L("TestGzopenWrite():"));
   		err = TestGzopenWrite();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzreadLargeFile)
		{
   		INFO_PRINTF1(_L("TestGzreadLargeFile():"));
   		err = TestGzreadLargeFile();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzopenWriteNoPath)
		{
   		INFO_PRINTF1(_L("TestGzopenWriteNoPath():"));
   		err = TestGzopenWriteNoPath();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzreadLenZero)
		{
   		INFO_PRINTF1(_L("TestGzreadLenZero():"));
   		err = TestGzreadLenZero();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzreadBufZero)
		{
   		INFO_PRINTF1(_L("TestGzreadBufZero():"));
   		err = TestGzreadBufZero();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzreadNonGzFile)
		{
   		INFO_PRINTF1(_L("TestGzreadNonGzFile():"));
   		err = TestGzreadNonGzFile();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzrewindNonGzFile)
		{
   		INFO_PRINTF1(_L("TestGzrewindNonGzFile():"));
   		err = TestGzrewindNonGzFile();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzrewindFileNull)
		{
   		INFO_PRINTF1(_L("TestGzrewindFileNull():"));
   		err = TestGzrewindFileNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestGzflushWithZFinish)
		{
   		INFO_PRINTF1(_L("TestGzflushWithZFinish():"));
   		err = TestGzflushWithZFinish();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestUncompressLenSmall)
		{
   		INFO_PRINTF1(_L("TestUncompressLenSmall():"));
   		err = TestUncompressLenSmall();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestUncompressBufNull)
		{
   		INFO_PRINTF1(_L("TestUncompressBufNull():"));
   		err = TestUncompressBufNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestUncompressLenNull)
		{
   		INFO_PRINTF1(_L("TestUncompressLenNull():"));
   		err = TestUncompressLenNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateScenarios)
		{
   		INFO_PRINTF1(_L("TestInflateScenarios():"));
   		err = TestInflateScenarios();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestInflateStreamStateNull():"));
   		err = TestInflateStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestInflateResetStreamStateNull)
		{
   		INFO_PRINTF1(_L("TestInflateResetStreamStateNull():"));
   		err = TestInflateResetStreamStateNull();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}			//END testcases of tzlibblocks.cpp
	
	// BEGIN testcases of tzlibadvanced.cpp
   	else if(TestStepName() == KTestDeflateTuneL)
		{
   		INFO_PRINTF1(_L("TestDeflateTuneL():"));
   		SetTestStepResult(TestDeflateTuneL());
   		}
   	else if(TestStepName() == KTestDeflateTuneFailL)
		{
   		INFO_PRINTF1(_L("TestDeflateTuneFailL():"));
   		SetTestStepResult(TestDeflateTuneFailL());
   		}
   	else if(TestStepName() == KTestDefInfGZipDefaultHeaderIgnoreL)
		{
   		INFO_PRINTF1(_L("TestDefInfGZipDefaultHeaderIgnoreL():"));
   		SetTestStepResult(TestDefInfGZipDefaultHeaderIgnoreL());
   		}
   	else if(TestStepName() == KTestDefInfGZipDefaultHeaderAutoL)
		{
   		INFO_PRINTF1(_L("TestDefInfGZipDefaultHeaderAutoL():"));
   		SetTestStepResult(TestDefInfGZipDefaultHeaderAutoL());
   		}
   	else if(TestStepName() == KTestDefInfGZipSpecifiedHeaderManualL)
		{
   		INFO_PRINTF1(_L("TestDefInfGZipSpecifiedHeaderManualL():"));
   		SetTestStepResult(TestDefInfGZipSpecifiedHeaderManualL());
   		}
   	else if(TestStepName() == KTestDefInfGZipSpecifiedHeaderAutoL)
		{
   		INFO_PRINTF1(_L("TestDefInfGZipSpecifiedHeaderAutoL():"));
   		SetTestStepResult(TestDefInfGZipSpecifiedHeaderAutoL());
   		}
   	else if(TestStepName() == KTestDefInfZlibHeaderAutoL)
		{
   		INFO_PRINTF1(_L("TestDefInfZlibHeaderAutoL():"));
   		SetTestStepResult(TestDefInfZlibHeaderAutoL());
   		}
   	else if(TestStepName() == KTestDeflateSetHeaderFailsL)
		{
   		INFO_PRINTF1(_L("TestDeflateSetHeaderFailsL():"));
   		SetTestStepResult(TestDeflateSetHeaderFailsL());
   		}
   	else if(TestStepName() == KTestInflateGetHeaderFailsL)
		{
   		INFO_PRINTF1(_L("TestInflateGetHeaderFailsL():"));
   		SetTestStepResult(TestInflateGetHeaderFailsL());
   		}
   	else if(TestStepName() == KTestDefInfRawL)
		{
   		INFO_PRINTF1(_L("TestDefInfRawL():"));
   		SetTestStepResult(TestDefInfRawL());
   		}
   	else if(TestStepName() == KTestDefRawFailsL)
		{
   		INFO_PRINTF1(_L("TestDefRawFailsL():"));
   		SetTestStepResult(TestDefRawFailsL());
   		}
   	else if(TestStepName() == KTestDefInfRawFailsL)
		{
   		INFO_PRINTF1(_L("TestDefInfRawFailsL():"));
   		SetTestStepResult(TestDefInfRawFailsL());
   		}
   	else if(TestStepName() == KTestDeflatePrimeL)
		{
   		INFO_PRINTF1(_L("TestDeflatePrimeL():"));
   		SetTestStepResult(TestDeflatePrimeL());
   		}
   	else if(TestStepName() == KTestDeflatePrimeFailsL)
		{
   		INFO_PRINTF1(_L("TestDeflatePrimeFailsL():"));
   		SetTestStepResult(TestDeflatePrimeFailsL());
   		}
   	else if(TestStepName() == KTestInflatePrimeFailsL)
		{
   		INFO_PRINTF1(_L("TestInflatePrimeFailsL():"));
   		SetTestStepResult(TestInflatePrimeFailsL());
   		}
   	else if(TestStepName() == KTestInflateCopyL)
		{
   		INFO_PRINTF1(_L("TestInflateCopyL():"));
   		SetTestStepResult(TestInflateCopyL());
   		}
   	else if(TestStepName() == KTestInflateCopyFailsParamsL)
		{
   		INFO_PRINTF1(_L("TestInflateCopyFailsParamsL():"));
   		SetTestStepResult(TestInflateCopyFailsParamsL());
   		}
   	else if(TestStepName() == KTestInflateCopyFailsMemL)
		{
   		INFO_PRINTF1(_L("TestInflateCopyFailsMemL():"));
   		SetTestStepResult(TestInflateCopyFailsMemL());
   		}
   	else if(TestStepName() == KTestInflateBackL)
		{
   		INFO_PRINTF1(_L("TestInflateBackL():"));
   		SetTestStepResult(TestInflateBackL());
   		}
   	else if(TestStepName() == KTestInflateBackEndFailsL)
		{
   		INFO_PRINTF1(_L("TestInflateBackEndFailsL():"));
   		SetTestStepResult(TestInflateBackEndFailsL());
   		}
   	else if(TestStepName() == KTestInflateBackFailsL)
		{
   		INFO_PRINTF1(_L("TestInflateBackFailsL():"));
   		SetTestStepResult(TestInflateBackFailsL());
   		}
   	else if(TestStepName() == KTestInflateBackInitFailsParamsL)
		{
   		INFO_PRINTF1(_L("TestInflateBackInitFailsParamsL():"));
   		SetTestStepResult(TestInflateBackInitFailsParamsL());
   		}
   	else if(TestStepName() == KTestInflateBackInitFailsMem)
		{
   		INFO_PRINTF1(_L("TestInflateBackInitFailsMem():"));
   		SetTestStepResult(TestInflateBackInitFailsMem());
   		}
   	else if(TestStepName() == KTestAdler32CombineL)
		{
   		INFO_PRINTF1(_L("TestAdler32CombineL():"));
   		SetTestStepResult(TestAdler32CombineL());
   		}
   	else if(TestStepName() == KTestCrc32CombineL)
		{
   		INFO_PRINTF1(_L("TestCrc32CombineL():"));
   		SetTestStepResult(TestCrc32CombineL());
   		}
   	else if(TestStepName() == KTestZlibCompileFlagsL)
		{
   		INFO_PRINTF1(_L("TestZlibCompileFlagsL():"));
   		SetTestStepResult(TestZlibCompileFlagsL());
   		}
		
   	return TestStepResult(); 
	}
	
 



