/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tzlib.h
* 
*
*/




#ifndef __TZLIB_H__
#define __TZLIB_H__

#include <test/testexecutestepbase.h>
#include <errno.h>
#include <e32std.h>
#include <e32def.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <zlib.h>


#define TESTFILE "c:\\foo.gz"
#define NOFILE   "m:\\xyz.gz"
#define MYFILE "c:\\rit.gz"
#define FILETEST "c:\\hbtools.exe"
#define FILETESTGZ "z:\\tef_ezlib2\\rit1.gz"
#define FILETESTGZLARGE "z:\\tef_ezlib2\\gzipped.gz"
#define FILETESTGZCONCAT "z:\\tef_ezlib2\\concat.gz"

#define FILETESTGZ1 "c:\\ethernetced.xml.gz"
#define PUTFILE "c:\\put.gz"
const char dictionary[] = "hello";
const char hello[] = "hello, hello!";

// string litreals for tzlibcases.cpp
_LIT(Ktest_zlibVersion, "Test_zlibVersion");
_LIT(Ktest_compress01, "Test_compress01");
_LIT(Ktest_deflate01, "Test_deflate01");
_LIT(Ktest_deflate02, "Test_deflate02");
_LIT(Ktest_deflateEnd, "Test_deflateEnd");
_LIT(Ktest_inflate01, "Test_inflate01");
_LIT(Ktest_inflate02, "Test_inflate02");
_LIT(Ktest_inflate03, "Test_inflate03");
_LIT(Ktest_inflate04, "Test_inflate04");
_LIT(Ktest_inflate05, "Test_inflate05");
_LIT(Ktest_inflate06, "Test_inflate06");
_LIT(Ktest_inflateEnd, "Test_inflateEnd");
_LIT(Ktest_deflateSetDictionary01, "Test_deflateSetDictionary01");
_LIT(Ktest_deflateSetDictionary02, "Test_deflateSetDictionary02");
_LIT(Ktest_deflateSetDictionary03, "Test_deflateSetDictionary03");
_LIT(Ktest_deflateSetDictionary04, "Test_deflateSetDictionary04");
_LIT(Ktest_deflateSetDictionary05, "Test_deflateSetDictionary05");
_LIT(Ktest_gzio, "Test_gzio");
_LIT(Ktest_gzdirect, "Test_gzdirect");
_LIT(Ktest_gzdirectnull, "Test_gzdirectnull");
_LIT(Ktest_gzclearerr, "Test_gzclearerr");
_LIT(Ktest_gzclearerr_null, "Test_gzclearerr_null");
_LIT(Ktest_gzerror_streamend, "Test_gzerror_streamend");
_LIT(Ktest_gzungetcnegative, "Test_gzungetcnegative");
_LIT(Ktest_gzseeknegative, "Test_gzseeknegative");
_LIT(Ktest_gzdirecttxt, "Test_gzdirecttxt");
_LIT(KtestGzungetcChain, "TestGzungetcChain");
_LIT(KtestGzseekBack, "TestGzseekBack");
_LIT(KtestGzseekAppend, "TestGzseekAppend");
_LIT(KtestGzseekHugeOffset, "TestGzseekHugeOffset");
_LIT(KtestGzseekNoSize, "TestGzseekNoSize");
_LIT(KtestGzopenLongPath01, "TestGzopenLongPath01");
_LIT(KtestGzseekLongPath01, "TestGzseekLongPath01");
_LIT(KtestGzopenLongPath02, "TestGzopenLongPath02");
_LIT(KtestGzseekMixedFile01, "TestGzseekMixedFile01");
_LIT(KtestGzopenNoMode, "TestGzopenNoMode");
_LIT(KtestGzopenNoPath, "TestGzopenNoPath");
_LIT(KtestGzopenNoPathMode, "TestGzopenNoPathMode");
_LIT(KtestGzseekConcatedFile01, "TestGzseekConcatedFile01");
_LIT(KtestGzopenDiffMode, "TestGzopenDiffMode");
_LIT(KtestGzseekConcatedFile02, "TestGzseekConcatedFile02");
_LIT(KtestGzprintf01, "TestGzprintf01");
_LIT(KtestGzprintf02, "TestGzprintf02");
_LIT(KtestGzflushNull, "TestGzflushNull");
_LIT(KtestGzflushRepeat, "TestGzflushRepeat");
_LIT(KtestGzflushHugeBuf, "TestGzflushHugeBuf");
_LIT(KtestGzrewindNull, "TestGzrewindNull");
_LIT(KtestGzrewindTransparent, "TestGzrewindTransparent");
_LIT(KtestGzerrorErrNoSet, "TestGzerrorErrNoSet");
_LIT(KtestGzgetsBufNull, "TestGzgetsBufNull");
_LIT(KtestGzgetsSmallBuf, "TestGzgetsSmallBuf");


// string literals for tzlib_auto.cpp
_LIT(KtestDeflateReset, "TestDeflateReset");
_LIT(KtestDeflateReset_fail, "TestDeflateReset_fail");
_LIT(KtestDeflateInit2_bits, "TestDeflateInit2_bits");
_LIT(KtestDeflateInit2_level, "TestDeflateInit2_level");
_LIT(KtestInflateInit2_bits, "TestInflateInit2_bits");
_LIT(KtestGzread, "TestGzread");
_LIT(KtestGzread_fail, "TestGzread_fail");
_LIT(KtestUncompress, "TestUncompress");
_LIT(KtestUncompressfail, "TestUncompressfail");
_LIT(KtestInflateSetDictionary, "TestInflateSetDictionary");
_LIT(KtestInflateSetDictionary_size, "TestInflateSetDictionary_size");
_LIT(KtestInflateSetDictionary_null, "TestInflateSetDictionary_null");
_LIT(KtestGzgets, "TestGzgets");
_LIT(KtestgzgetsFail, "TestgzgetsFail");
_LIT(KtestgzgetsopenFail, "TestgzgetsopenFail");
_LIT(KtestInflate, "TestInflate");
_LIT(KtestInflate_fail1, "TestInflate_fail1");
_LIT(KtestInflate_fail2, "TestInflate_fail2");
_LIT(KtestInflate_fail3, "TestInflate_fail3");
_LIT(KtestInflateend, "TestInflateend");
_LIT(KtestInflateend_fail, "TestInflateend_fail");
_LIT(KtestInflateReset, "TestInflateReset");
_LIT(KtestInflateResetfail1, "TestInflateResetfail1");
_LIT(KtestInflateInit2_, "TestInflateInit2_");
_LIT(KtestInflateInit_, "TestInflateInit_");
_LIT(KtestInflateInit2_negative, "TestInflateInit2_negative");
_LIT(KtestInflateInit_negative, "TestInflateInit_negative");
_LIT(KtestInflateInit2_versioncheck, "TestInflateInit2_versioncheck");
_LIT(KtestInflateInit_versioncheck, "TestInflateInit_versioncheck");
_LIT(KtestAdlerinit, "TestAdlerinit");
_LIT(KtestAdler, "TestAdler");
_LIT(KtestCompress, "TestCompress");
_LIT(KtestCompress_negative, "TestCompress_negative");
_LIT(KtestCompress2_positive, "TestCompress2_positive");
_LIT(KtestCompress2_negative, "TestCompress2_negative");
_LIT(KtestCompressbound, "TestCompressbound");
_LIT(KtestDeflatebound, "TestDeflatebound");
_LIT(KtestDeflateparams, "TestDeflateparams");
_LIT(KtestDeflateparamsfail1, "TestDeflateparamsfail1");
_LIT(KtestDeflateparamsfail2, "TestDeflateparamsfail2");
_LIT(KtestCrcinit, "TestCrcinit");
_LIT(KtestCrc, "TestCrc");
_LIT(KtestGet_crc_table, "TestGet_crc_table");
_LIT(KtestDeflateInit_, "TestDeflateInit_");
_LIT(KtestDeflateInit_level, "TestDeflateInit_level");
_LIT(KtestDeflateInit2_, "TestDeflateInit2_");
_LIT(KtestDeflatefail, "TestDeflatefail");
_LIT(KtestDeflatefail2, "TestDeflatefail2");
_LIT(KtestZlibversion, "TestZlibversion");
_LIT(KtestGzputc, "TestGzputc");
_LIT(KtestGzopen, "TestGzopen");
_LIT(KtestGzopenmode, "TestGzopenmode");
_LIT(KtestGzopenfail, "TestGzopenfail");
_LIT(KtestGzputcfail, "TestGzputcfail");
_LIT(KtestGzputcreturn, "TestGzputcreturn");
_LIT(KtestGzputs, "TestGzputs");
_LIT(KtestGzputsfail, "TestGzputsfail");
_LIT(KtestGzprintf, "TestGzprintf");
_LIT(KtestGzprintf_trying, "TestGzprintf_trying");
_LIT(KtestGzwrite, "TestGzwrite");
_LIT(KtestGzwritefail, "TestGzwritefail");
_LIT(KtestGztell, "TestGztell");
_LIT(KtestGztell1, "TestGztell1");
_LIT(KtestGztellfail1, "TestGztellfail1");
_LIT(KtestDeflatecopy, "TestDeflatecopy");
_LIT(KtestDeflatecopyfail, "TestDeflatecopyfail");
_LIT(KtestGzclose, "TestGzclose");
_LIT(KtestGzclose_fail, "TestGzclose_fail");
_LIT(KtestGzeof, "TestGzeof");
_LIT(KtestGzeoffail1, "TestGzeoffail1");
_LIT(KtestGzeoffail2, "TestGzeoffail2");
_LIT(KtestGzgetc, "TestGzgetc");
_LIT(KtestGzflush, "TestGzflush");
_LIT(KtestGzflushsync, "TestGzflushsync");
_LIT(KtestGzflushfail, "TestGzflushfail");
_LIT(KtestGzerror, "TestGzerror");
_LIT(KtestGzerrorfail1, "TestGzerrorfail1");
_LIT(KtestGzgetcfail, "TestGzgetcfail");
_LIT(KtestDeflateSetDictionary, "TestDeflateSetDictionary");
_LIT(KtestDeflateSetDictionary_nodict, "TestDeflateSetDictionary_nodict");
_LIT(KtestDeflateSetDictionary_fail, "TestDeflateSetDictionary_fail");
_LIT(KtestDeflateend, "TestDeflateend");
_LIT(KtestDeflateendfail1, "TestDeflateendfail1");
_LIT(KtestDeflate, "TestDeflate");
_LIT(KtestGzseek, "TestGzseek");
_LIT(KtestGzseekfail1, "TestGzseekfail1");
_LIT(KtestGzseekfail2, "TestGzseekfail2");
_LIT(KtestGzsetparams, "TestGzsetparams");
_LIT(KtestGzsetparams_fail1, "TestGzsetparams_fail1");
_LIT(KtestGzsetparams_fail2, "TestGzsetparams_fail2");
_LIT(KtestGzsetparams_fail3, "TestGzsetparams_fail3");
_LIT(KtestGzrewind, "TestGzrewind");
_LIT(KtestGzrewindfail, "TestGzrewindfail");
_LIT(KtestGzdopen, "TestGzdopen");
_LIT(KtestGzdopen_fail, "TestGzdopen_fail");
_LIT(KtestGzdopen_fail2, "TestGzdopen_fail2");
_LIT(KtestInflateSync, "TestInflateSync");
_LIT(KtestinflateSyncfail, "TestinflateSyncfail");
_LIT(KtestInflateSyncPoint, "TestInflateSyncPoint");
_LIT(KtestInflateSyncPoint_null, "TestInflateSyncPoint_null");
_LIT(KtestZerror, "TestZerror");
_LIT(KtestZerror1, "TestZerror1");
_LIT(KtestZerror2, "TestZerror2");

// string literals for tzlibblocks.cpp
_LIT(KTestDeflateTest01, "TestDeflateTest01");
_LIT(KTestDeflatecopyDestNull, "TestDeflatecopyDestNull");
_LIT(KTestDeflateCopyStreamStateNull, "TestDeflateCopyStreamStateNull");
_LIT(KTestDeflateInit2_WindowBits, "TestDeflateInit2_WindowBits");
_LIT(KTestDeflateInit2_StreamSize, "TestDeflateInit2_StreamSize");
_LIT(KTestDeflateInit2_MemLevel, "TestDeflateInit2_MemLevel");
_LIT(KTestDeflateInit2_Level, "TestDeflateInit2_Level");
_LIT(KTestDeflateInit2_Strategy, "TestDeflateInit2_Strategy");
_LIT(KTestDeflateInit2_Version, "TestDeflateInit2_Version");
_LIT(KTestDeflateInit2_VersionNull, "TestDeflateInit2_VersionNull");
_LIT(KTestDeflateInit2_StreamNull, "TestDeflateInit2_StreamNull");
_LIT(KTestInflateInit2_Version, "TestInflateInit2_Version");
_LIT(KTestInflateInit2_VersionNull, "TestInflateInit2_VersionNull");
_LIT(KTestInflateInit2_WindowBits, "TestInflateInit2_WindowBits");
_LIT(KTestInflateInit2_StreamNull, "TestInflateInit2_StreamNull");
_LIT(KTestDeflate_HuffmanStrategy, "TestDeflate_HuffmanStrategy");
_LIT(KTestDeflate_AvailInZero, "TestDeflate_AvailInZero");
_LIT(KTestGzsetparamDefaultCompression, "TestGzsetparamDefaultCompression");
_LIT(KTestDeflateResetNullStream, "TestDeflateResetNullStream");
_LIT(KTestDeflateResetStreamStateNull, "TestDeflateResetStreamStateNull");
_LIT(KTestDeflate_Scenarios, "TestDeflate_Scenarios");
_LIT(KTestDeflate_NullStream, "TestDeflate_NullStream");
_LIT(KTestDeflate_StreamStateNull, "TestDeflate_StreamStateNull");
_LIT(KTestDeflateEndNull, "TestDeflateEndNull");
_LIT(KTestDeflateEndStreamStateNull, "TestDeflateEndStreamStateNull");
_LIT(KTestDeflate_WindowBits, "TestDeflate_WindowBits");
_LIT(KTestDeflateBoundStreamNotNull, "TestDeflateBoundStreamNotNull");
_LIT(KTestDeflateBoundStreamNull, "TestDeflateBoundStreamNull");
_LIT(KTestDeflateBoundStreamStateNull, "TestDeflateBoundStreamStateNull");
_LIT(KTestDeflateSetDictionaryWrap, "TestDeflateSetDictionaryWrap");
_LIT(KTestDeflateSetDictionaryLen, "TestDeflateSetDictionaryLen");
_LIT(KTestInflateSetDictionaryBadMode, "TestInflateSetDictionaryBadMode");
_LIT(KTestInflateSetDictionaryStreamStateNull, "TestInflateSetDictionaryStreamStateNull");
_LIT(KTestDeflateParamsStreamStateNull, "TestDeflateParamsStreamStateNull");
_LIT(KTestInflateSyncAvailInNull, "TestInflateSyncAvailInNull");
_LIT(KTestInflateSyncStreamNull, "TestInflateSyncStreamNull");
_LIT(KTestInflateSyncStreamStateNull, "TestInflateSyncStreamStateNull");
_LIT(KTestInflateSyncPointStreamStateNull, "TestInflateSyncPointStreamStateNull");
_LIT(KTestAdlerScenarios, "TestAdlerScenarios");
_LIT(KTestGzsetparamsFileNull, "TestGzsetparamsFileNull");
_LIT(KTestGzopenWrite, "TestGzopenWrite");
_LIT(KTestGzreadLargeFile, "TestGzreadLargeFile");
_LIT(KTestGzopenWriteNoPath, "TestGzopenWriteNoPath");
_LIT(KTestGzreadLenZero, "TestGzreadLenZero");
_LIT(KTestGzreadBufZero, "TestGzreadBufZero");
_LIT(KTestGzreadNonGzFile, "TestGzreadNonGzFile");
_LIT(KTestGzrewindNonGzFile, "TestGzrewindNonGzFile");
_LIT(KTestGzrewindFileNull, "TestGzrewindFileNull");
_LIT(KTestGzflushWithZFinish, "TestGzflushWithZFinish");
_LIT(KTestUncompressLenSmall, "TestUncompressLenSmall");
_LIT(KTestUncompressBufNull, "TestUncompressBufNull");
_LIT(KTestUncompressLenNull, "TestUncompressLenNull");
_LIT(KTestInflateScenarios, "TestInflateScenarios");
_LIT(KTestInflateStreamStateNull, "TestInflateStreamStateNull");
_LIT(KTestInflateResetStreamStateNull, "TestInflateResetStreamStateNull");

// string literals for tzlibadvanced.cpp
_LIT(KTestDeflateTuneL, "TestDeflateTuneL");
_LIT(KTestDeflateTuneFailL, "TestDeflateTuneFailL");
_LIT(KTestDefInfGZipDefaultHeaderIgnoreL, "TestDefInfGZipDefaultHeaderIgnoreL");
_LIT(KTestDefInfGZipDefaultHeaderAutoL, "TestDefInfGZipDefaultHeaderAutoL");
_LIT(KTestDefInfGZipSpecifiedHeaderManualL, "TestDefInfGZipSpecifiedHeaderManualL");
_LIT(KTestDefInfGZipSpecifiedHeaderAutoL, "TestDefInfGZipSpecifiedHeaderAutoL");
_LIT(KTestDefInfZlibHeaderAutoL, "TestDefInfZlibHeaderAutoL");
_LIT(KTestDeflateSetHeaderFailsL, "TestDeflateSetHeaderFailsL");
_LIT(KTestInflateGetHeaderFailsL, "TestInflateGetHeaderFailsL");
_LIT(KTestDefInfRawL, "TestDefInfRawL");
_LIT(KTestDefRawFailsL, "TestDefRawFailsL");
_LIT(KTestDefInfRawFailsL, "TestDefInfRawFailsL");
_LIT(KTestDeflatePrimeL, "TestDeflatePrimeL");
_LIT(KTestDeflatePrimeFailsL, "TestDeflatePrimeFailsL");
_LIT(KTestInflatePrimeFailsL, "TestInflatePrimeFailsL");
_LIT(KTestInflateCopyL, "TestInflateCopyL");
_LIT(KTestInflateCopyFailsParamsL, "TestInflateCopyFailsParamsL");
_LIT(KTestInflateCopyFailsMemL, "TestInflateCopyFailsMemL");
_LIT(KTestInflateBackL, "TestInflateBackL");
_LIT(KTestInflateBackEndFailsL, "TestInflateBackEndFailsL");
_LIT(KTestInflateBackFailsL, "TestInflateBackFailsL");
_LIT(KTestInflateBackInitFailsParamsL, "TestInflateBackInitFailsParamsL");
_LIT(KTestInflateBackInitFailsMem, "TestInflateBackInitFailsMem");
_LIT(KTestAdler32CombineL, "TestAdler32CombineL");
_LIT(KTestCrc32CombineL, "TestCrc32CombineL");
_LIT(KTestZlibCompileFlagsL, "TestZlibCompileFlagsL");

typedef struct s_streamSettings
	{
	bool deflateInit2;
	int level;
	int method;
	int deflateWindowBits;
	int inflateWindowBits;
	int memLevel;
	int strategy;   
	} StreamSettings;

class CTestZlib : public CTestStep
	{
public:
	CTestZlib(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt iParamCnt;
	uLong dictId; /* Adler32 value of the dictionary */
	
	void ReadIntParam(int& );
	void ReadStringParam(char*);
	TInt test_deflate( Byte*, uLong);
	TInt test_dict_deflate( Byte*, uLong);
	TInt test_flush(Byte*, uLong*);
	
	// Testcases of tzlibcases.cpp
	TInt Test_zlibVersion();
	TInt Test_compress01();
	TInt Test_deflate01();
	TInt Test_deflate02();
	TInt Test_deflateEnd();
	TInt Test_inflate01();
	TInt Test_inflate02();
	TInt Test_inflate03();
	TInt Test_inflate04();
	TInt Test_inflate05();
	TInt Test_inflate06();
	TInt Test_inflateEnd();
	TInt Test_gzio();
	TInt Test_deflateSetDictionary01();
	TInt Test_deflateSetDictionary02();
	TInt Test_deflateSetDictionary03();
	TInt Test_deflateSetDictionary04();
	TInt Test_deflateSetDictionary05();
	TInt Test_gzdirect();
	TInt Test_gzdirectnull();
	TInt Test_gzclearerr();
	TInt Test_gzclearerr_null();
	TInt Test_gzerror_streamend();
	TInt Test_gzungetcnegative();
	TInt Test_gzseeknegative();
	TInt Test_gzdirecttxt();
	TInt TestGzungetcChain();
	TInt TestGzseekBack();
	TInt TestGzseekAppend();
	TInt TestGzseekHugeOffset();
	TInt TestGzseekNoSize();
	TInt TestGzopenLongPath01();
	TInt TestGzseekLongPath01();
	TInt TestGzopenLongPath02();
	TInt TestGzseekMixedFile01();
	TInt TestGzopenNoMode();
	TInt TestGzopenNoPath();
	TInt TestGzopenNoPathMode();
	TInt TestGzseekConcatedFile01();
	TInt TestGzopenDiffMode();
	TInt TestGzseekConcatedFile02();
	TInt TestGzprintf01();
	TInt TestGzprintf02();
	TInt TestGzflushNull();
	TInt TestGzflushRepeat();
	TInt TestGzflushHugeBuf();
	TInt TestGzrewindNull();
	TInt TestGzrewindTransparent();
	TInt TestGzerrorErrNoSet();
	TInt TestGzgetsBufNull();
	TInt TestGzgetsSmallBuf();
	
	//Common utility functions used by other testcases
	TInt Test_flush(Byte*, uLong*);
	TInt Test_dict_deflate( Byte * compr,uLong comprLen);
	TInt Test_dict_inflate(Byte*, uLong, Byte*, uLong);
	TInt Test_deflate(Byte *compr, uLong comprLen);
	TInt sec_compress(Byte * compr,uLong  comprLen,Byte *  uncompr, uLong uncomprLen);
	TInt sec_deflate01(Byte * compr, uLong comprLen,TInt flush, TInt compression);
	TInt sec_deflateCopy01(Byte * compr, uLong comprLen,TInt flush, TInt compression,Byte *dest);
	TInt sec_deflateSetDictionary01(Byte * compr, uLong comprLen,TInt flush, TInt compression);
	TInt sec_deflateSetDictionary02(TInt compression);
	TInt sec_deflateSetDictionary03(Byte * compr, uLong comprLen,TInt flush, TInt compression);
	TInt sec_deflateSetDictionary04(Byte * compr, uLong comprLen,TInt flush, TInt compression);
	TInt sec_deflateSetDictionary05(Byte * compr, uLong comprLen,TInt flush, TInt compression);
	TInt sec_deflate02(Byte * compr, uLong comprLen,TInt flush, TInt compression);
	TInt sec_inflate(Byte * compr,uLong  comprLen, Byte *  uncompr, uLong  uncomprLen, TInt flush);
	TInt sec_inflate_large_buf(Byte * compr,uLong  comprLen, Byte *  uncompr, uLong  uncomprLen, TInt flush);
	TInt sec_gzio(const char *fname,  Byte *  uncompr, uLong uncomprLen);
	
	TInt PreDeflateInit(Byte * compr, uLong comprLen,TInt flush, TInt compression);
	
	// Testcases of tzlib_auto.cpp
	TInt TestDeflateReset();
	TInt TestDeflateReset_fail();
	TInt TestDeflateInit2_bits();
	TInt TestDeflateInit2_level();
	TInt TestInflateInit2_bits();
	TInt TestGzread();	
	TInt TestGzread_fail();
	TInt TestUncompress();
	TInt TestUncompressfail();
	TInt TestInflateSetDictionary();
	TInt TestInflateSetDictionary_size();
	TInt TestInflateSetDictionary_null();
	TInt TestGzgets();
	TInt TestgzgetsFail();
	TInt TestgzgetsopenFail();
	TInt TestInflate();
	TInt TestInflate_fail1();
	TInt TestInflate_fail2();
	TInt TestInflate_fail3();
	TInt TestInflateend();
	TInt TestInflateend_fail();
	TInt TestInflateReset();
	TInt TestInflateResetfail1();
	TInt TestInflateInit2_();
	TInt TestInflateInit_();
	TInt TestInflateInit2_negative();
	TInt TestInflateInit_negative();
	TInt TestInflateInit2_versioncheck();
	TInt TestInflateInit_versioncheck();
	TInt TestAdlerinit();
	TInt TestAdler();
	TInt TestCompress();
	TInt TestCompress_negative();
	TInt TestCompress2_positive();
	TInt TestCompress2_negative();
	TInt TestCompressbound();
	TInt TestDeflatebound();
	TInt TestDeflateparams();
	TInt TestDeflateparamsfail1();
	TInt TestDeflateparamsfail2();
	TInt TestCrcinit();
	TInt TestCrc();
	TInt TestGet_crc_table();
	TInt TestDeflateInit_();
	TInt TestDeflateInit_level();
	TInt TestDeflateInit2_();
	TInt TestDeflatefail();
	TInt TestDeflatefail2();
	TInt TestZlibversion();
	TInt TestGzputc();
	TInt TestGzopen();
	TInt TestGzopenmode();
	TInt TestGzopenfail();
	TInt TestGzputcfail();
	TInt TestGzputcreturn();
	TInt TestGzputs();
	TInt TestGzputsfail();
	TInt TestGzprintf();
	TInt TestGzprintf_trying();
	TInt TestGzwrite();
	TInt TestGzwritefail();
	TInt TestGztell();
	TInt TestGztell1();
	TInt TestGztellfail1();
	TInt TestDeflatecopy();
	TInt TestDeflatecopyfail();
	TInt TestGzclose();
	TInt TestGzclose_fail();
	TInt TestGzeof();
	TInt TestGzeoffail1();
	TInt TestGzeoffail2();
	TInt TestGzgetc();
	TInt TestGzflush();
	TInt TestGzflushsync();
	TInt TestGzflushfail();
	TInt TestGzerror();
	TInt TestGzerrorfail1();
	TInt TestGzgetcfail();
	TInt TestDeflateSetDictionary();
	TInt TestDeflateSetDictionary_nodict();
	TInt TestDeflateSetDictionary_fail();
	TInt TestDeflateend();
	TInt TestDeflateendfail1();
	TInt TestDeflate();
	TInt TestGzseek();
	TInt TestGzseekfail1();
	TInt TestGzseekfail2();
	TInt TestGzsetparams();
	TInt TestGzsetparams_fail1();
	TInt TestGzsetparams_fail2();
	TInt TestGzsetparams_fail3();
	TInt TestGzrewind();
	TInt TestGzrewindfail();
	TInt TestGzdopen();
	TInt TestGzdopen_fail();
	TInt TestGzdopen_fail2();
	TInt TestInflateSync();
	TInt TestinflateSyncfail();
	TInt TestInflateSyncPoint();
	TInt TestInflateSyncPoint_null();
	TInt TestZerror();
	TInt TestZerror1();
	TInt TestZerror2();
	
	// Testcases of tzlibblocks.cpp
	TInt TestDeflateTest01();
	TInt TestDeflatecopyDestNull();
	TInt TestDeflateCopyStreamStateNull();
	TInt TestDeflateInit2_WindowBits();
	TInt TestDeflateInit2_StreamSize();
	TInt TestDeflateInit2_MemLevel();
	TInt TestDeflateInit2_Level();
	TInt TestDeflateInit2_Strategy();
	TInt TestDeflateInit2_Version();
	TInt TestDeflateInit2_VersionNull();
	TInt TestDeflateInit2_StreamNull();
	TInt TestInflateInit2_Version();
	TInt TestInflateInit2_VersionNull();
	TInt TestInflateInit2_WindowBits();
	TInt TestInflateInit2_StreamNull();
	TInt TestDeflate_HuffmanStrategy();
	TInt TestDeflate_AvailInZero();
	TInt TestGzsetparamDefaultCompression();
	TInt TestDeflateResetNullStream();
	TInt TestDeflateResetStreamStateNull();
	TInt TestDeflate_Scenarios();
	TInt TestDeflate_NullStream();
	TInt TestDeflate_StreamStateNull();
	TInt TestDeflateEndNull();
	TInt TestDeflateEndStreamStateNull();
	TInt TestDeflate_WindowBits();
	TInt TestDeflateBoundStreamNotNull();
	TInt TestDeflateBoundStreamNull();
	TInt TestDeflateBoundStreamStateNull();
	TInt TestDeflateSetDictionaryWrap();
	TInt TestDeflateSetDictionaryLen();
	TInt TestInflateSetDictionaryBadMode();
	TInt TestInflateSetDictionaryStreamStateNull();
	TInt TestDeflateParamsStreamStateNull();
	TInt TestInflateSyncAvailInNull();
	TInt TestInflateSyncStreamNull();
	TInt TestInflateSyncStreamStateNull();
	TInt TestInflateSyncPointStreamStateNull();
	TInt TestAdlerScenarios();
	TInt TestGzsetparamsFileNull();
	TInt TestGzopenWrite();
	TInt TestGzreadLargeFile();
	TInt TestGzopenWriteNoPath();
	TInt TestGzreadLenZero();
	TInt TestGzreadBufZero();
	TInt TestGzreadNonGzFile();
	TInt TestGzrewindNonGzFile();
	TInt TestGzrewindFileNull();
	TInt TestGzflushWithZFinish();
	TInt TestUncompressLenSmall();
	TInt TestUncompressBufNull();
	TInt TestUncompressLenNull();
	TInt TestInflateScenarios();
	TInt TestInflateStreamStateNull();
	TInt TestInflateResetStreamStateNull();

	void DeflateInitL(z_stream &aStream, const int aLevel, const int expectedResult = Z_OK);
	void DeflateInit2L(z_stream &aStream, const int aLevel, const int aMethod, const int aWindowBits, const int aMemLevel, const int aStrategy, const int expectedResult = Z_OK);
	TInt DeflateCompress(z_stream &aStream, Byte *aInputData, int aInputDataLength, Byte *aCompressedDataBuffer, int aCompressedDataBufferLength);
	static void DeflateEnd(TAny *aStream);
	void InflateInitL(z_stream &aStream, const int expectedResult = Z_OK);
	void InflateInit2L(z_stream &aStream, const int aWindowBits, const int expectedResult = Z_OK);
	void InflateBackInitL(z_stream &aStream, const int aWindowBits, Bytef *aWindow, const int expectedResult = Z_OK);
	TInt InflateDecompress(z_stream &aStream, Byte *aCompressedDataBuffer, int aCompressedDataLength, Byte *aDecompressedDataBuffer, int aDecompressedDataBufferLength);
	static void InflateEnd(TAny *aStream);
	static void InflateBackEnd(TAny *aStream);
	TBool GZipHeadersEqual(const gz_header &header1, const gz_header &header2);
	TBool IsDefaultGZipHeader(const gz_header &header);
	
	void CompressDataL(StreamSettings &aStreamSettings, z_stream &aStream, Byte *aInputData, int aInputDataLength, Byte *aCompressedDataBuffer, int aCompressedDataBufferLength, gz_headerp aHeader = NULL);	
	TVerdict DefInfGZipHeaderL(const TBool aIgnoreHeader, const TBool aAutoDetectHeader, gz_headerp aSpecifiedGZipHeader);
	TVerdict DefInfGZipSpecifiedHeaderL(TBool aIgnoreHeader, TBool aAutoDetectHeader);
	
	TVerdict TestDeflateTuneL();
	TVerdict TestDeflateTuneFailL();
	TVerdict TestDefInfGZipDefaultHeaderIgnoreL();
	TVerdict TestDefInfGZipDefaultHeaderAutoL();
	TVerdict TestDefInfGZipSpecifiedHeaderManualL();
	TVerdict TestDefInfGZipSpecifiedHeaderAutoL();
	TVerdict TestDefInfZlibHeaderAutoL();
	TVerdict TestDeflateSetHeaderFailsL();
	TVerdict TestInflateGetHeaderFailsL();
	TVerdict TestDefInfRawL();
	TVerdict TestDefRawFailsL();
	TVerdict TestDefInfRawFailsL();
	TVerdict TestDeflatePrimeL();
	TVerdict TestDeflatePrimeFailsL();
	TVerdict TestInflatePrimeFailsL();
	TVerdict TestInflateCopyL();
	TVerdict TestInflateCopyFailsParamsL();
	TVerdict TestInflateCopyFailsMemL();
	TVerdict TestInflateBackL();
	TVerdict TestInflateBackEndFailsL();
	TVerdict TestInflateBackFailsL();
	TVerdict TestInflateBackInitFailsParamsL();
	TVerdict TestInflateBackInitFailsMem();
	TVerdict TestAdler32CombineL();
	TVerdict TestCrc32CombineL();
	TVerdict TestZlibCompileFlagsL();
	};
#endif //__TZLIB_H__

