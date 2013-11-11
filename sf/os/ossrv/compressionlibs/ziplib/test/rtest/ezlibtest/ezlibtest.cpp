// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32test.h>
#include <bacline.h>

#include <ezcompressor.h>
#include <ezdecompressor.h>
#include <ezfilebuffer.h>
#include <ezlib.h>
#include <hal.h>


_LIT(KSeparator,"----------------------------------------------------------------\n");

const TInt KNumberOfTests = 38;
_LIT(KTest1,"Multistep compression & decompression using buffers smaller than source & destination");
_LIT(KTest2,"Multistep compression & decompression using buffers larger than source & destination");
_LIT(KTest3,"Test Multistep compression when NeedInput provides no input");
_LIT(KTest4,"Test Multistep compression when NeedOutput provides no output");
_LIT(KTest5,"Perform multistep compression 2 with the same CEZCompressor, reseting it\n\
	 after the first compression.");
_LIT(KTest6,"Perform multistep compression 2 with the same CEZCompressor, without reseting it\n\
	 after the first compression.");
_LIT(KTest7,"Performs multistep compression & decompression using a preset dicitonary.");
_LIT(KTest8,"Performs multistep compression with compression level of 0.");
_LIT(KTest9,"Performs multistep compression with compression level of 9.");
_LIT(KTest10,"Performs multistep compression with invalid compression level.");
_LIT(KTest11,"Performs multistep compression with window bits of 8.");
_LIT(KTest12,"Performs multistep compression with an invalid value for window bits.");
_LIT(KTest13,"Performs multistep compression with memory level = 1.");
_LIT(KTest14,"Performs multistep compression with an invalid memory level.");
_LIT(KTest15,"Performs multistep compression using only Huffman encoding.");
_LIT(KTest16,"Performs multistep compression using Filtered strategy.");
_LIT(KTest17,"Performs multistep compression when Alloc fails randomly.");
_LIT(KTest18,"Test Multistep decompression when NeedInput provides no input.");
_LIT(KTest19,"Test Multistep decompression when NeedOutput provides no output.");
_LIT(KTest20,"Test Multistep decompression when compressed stream is corrupt.");
_LIT(KTest21,"Test Multistep decompressor construction when Alloc fails randomly.");
_LIT(KTest22,"Test Multistep decompression when Alloc fails randomly.");
_LIT(KTest23,"Test Multistep decompression when a dictionary is required and not provided.\n\
	 This test should panic.");
_LIT(KTest24,"Test Multistep decompression when a dictionary when the wrong dictionary is provided.");
_LIT(KTest25,"Perform multistep decompression twice with the same CEZDecompressor, reseting it\n\
	 after the first decompression.");
_LIT(KTest26,"Perform multistep decompression twice with the same CEZDecompressor, reseting it\n\
	 after the first decompression.");
_LIT(KTest27,"Perform multistep compression and decompression on unicode data.");
_LIT(KTest28,"Perform multistep compression and decompression on unicode data using a unicode dictionary.");
_LIT(KTest29,"Perform multistep compression and decompression on unicode data using an ascii dictionary.");
_LIT(KTest30,"Test percentage of completion during multistep compression/decompression.");
_LIT(KTest31,"Test single step compression and decompression.");
_LIT(KTest32,"Test single step compression when indaquate output buffer is provided.");
_LIT(KTest33,"Test single step decompression when indaquate output buffer is provided.");
_LIT(KTest34,"Testing graceful exit for zlib 1.1.3 decompression bug. See: INC022729.");
_LIT(KTest35,"Testing EZlib decompression performance with enforced compiler optimisation");
_LIT(KTest36,"Testing correct memory deallocation for CEZFileBufferManager. See: DEF109756.");
_LIT(KTest37,"Test CEZZStream functions through CEZCompressor class.");
_LIT(KTest38,"Test Calls to Zlibapiwrapper.cpp exported API's like inflate, deflate, compress, decompress");

const TPtrC KTest1Des(KTest1);
const TPtrC KTest2Des(KTest2);
const TPtrC KTest3Des(KTest3);
const TPtrC KTest4Des(KTest4);
const TPtrC KTest5Des(KTest5);
const TPtrC KTest6Des(KTest6);
const TPtrC KTest7Des(KTest7);
const TPtrC KTest8Des(KTest8);
const TPtrC KTest9Des(KTest9);
const TPtrC KTest10Des(KTest10);
const TPtrC KTest11Des(KTest11);
const TPtrC KTest12Des(KTest12);
const TPtrC KTest13Des(KTest13);
const TPtrC KTest14Des(KTest14);
const TPtrC KTest15Des(KTest15);
const TPtrC KTest16Des(KTest16);
const TPtrC KTest17Des(KTest17);
const TPtrC KTest18Des(KTest18);
const TPtrC KTest19Des(KTest19);
const TPtrC KTest20Des(KTest20);
const TPtrC KTest21Des(KTest21);
const TPtrC KTest22Des(KTest22);
const TPtrC KTest23Des(KTest23);
const TPtrC KTest24Des(KTest24);
const TPtrC KTest25Des(KTest25);
const TPtrC KTest26Des(KTest26);
const TPtrC KTest27Des(KTest27);
const TPtrC KTest28Des(KTest28);
const TPtrC KTest29Des(KTest29);
const TPtrC KTest30Des(KTest30);
const TPtrC KTest31Des(KTest31);
const TPtrC KTest32Des(KTest32);
const TPtrC KTest33Des(KTest33);
const TPtrC KTest34Des(KTest34);
const TPtrC KTest35Des(KTest35);
const TPtrC KTest36Des(KTest36);
const TPtrC KTest37Des(KTest37);
const TPtrC KTest38Des(KTest38);

void Test1L();
void Test2L();
void Test3();
void Test4();
void Test5L();
void Test6();
void Test7L();
void Test8L();
void Test9L();
void Test10();
void Test11();
void Test12();
void Test13L();
void Test14();
void Test15L();
void Test16L();
void Test17();
void Test18L();
void Test19L();
void Test20L();
void Test21L();
void Test22L();
void Test23();
void Test24L();
void Test25L();
void Test26L();
void Test27L();
void Test28L();
void Test29L();
void Test30L();
void Test31L();
void Test32();
void Test33L();
void Test34L();
void Test35L();
void Test36L();
void TestCEZCompressorMethodsL();
void TestZlibapiwrapperAPIL();

typedef void (*TestPtr)();

struct TTests
	{
	TestPtr iFn;
	const TPtrC *iDescription;
	};

const TTests testTable[] = {
	{Test1L,&KTest1Des},
	{Test2L,&KTest2Des},
	{Test3,&KTest3Des},
	{Test4,&KTest4Des},
	{Test5L,&KTest5Des},
	{Test6,&KTest6Des},
	{Test7L,&KTest7Des},
	{Test8L,&KTest8Des},
	{Test9L,&KTest9Des},
	{Test10,&KTest10Des},
	{Test11,&KTest11Des},
	{Test12,&KTest12Des},
	{Test13L,&KTest13Des},
	{Test14,&KTest14Des},
	{Test15L,&KTest15Des},
	{Test16L,&KTest16Des},
	{Test17,&KTest17Des},
	{Test18L,&KTest18Des},
	{Test19L,&KTest19Des},
	{Test20L,&KTest20Des},
	{Test21L,&KTest21Des},
	{Test22L,&KTest22Des},
	{Test23,&KTest23Des},
	{Test24L,&KTest24Des},
	{Test25L,&KTest25Des},
	{Test26L,&KTest26Des},
	{Test27L,&KTest27Des},
	{Test28L,&KTest28Des},
	{Test29L,&KTest29Des},
	{Test30L,&KTest30Des},
	{Test31L,&KTest31Des},
	{Test32,&KTest32Des},
	{Test33L,&KTest33Des},
	{Test34L,&KTest34Des},
	{Test35L,&KTest35Des},
	{Test36L,&KTest36Des},
	{TestCEZCompressorMethodsL, &KTest37Des},
	{TestZlibapiwrapperAPIL, &KTest38Des}
	};

struct TTestFlags
	{
	enum 
		{ 
		FakeNoInput = 16,
		FakeNoOutput = 32,
		Dictionary = 64,
		AllocFail = 128,
		Corrupt = 256,
		AllocFailInflate = 512,
		Unicode = 1024,
		Percent = 2048
		};


	TTestFlags() : iFlags(0), iBufferSize(0) { Defaults(); }
	TTestFlags(TInt aFlags, TInt aBufferSize) : iFlags(aFlags), iBufferSize(aBufferSize) { Defaults(); }
	void Defaults();

	TInt iFlags;
	TInt iBufferSize;
	TPtrC8 iDictionary;
	TInt iLevel;
	TInt iWindowBits;
	TInt iMemLevel;
	CEZCompressor::TStrategy iStrategy;
	};


void TTestFlags::Defaults()
	{
	iLevel = CEZCompressor::EDefaultCompression;
	iWindowBits = CEZCompressor::EMaxWBits;
	iMemLevel = CEZCompressor::EDefMemLevel;
	iStrategy = CEZCompressor::EDefaultStrategy;
	}
	
TReal FastCountToMilliseconds(TInt aFastCount)
   {
   TInt freqInHz;
   HAL::Get(HAL::EFastCounterFrequency, freqInHz);
   TReal freqInkHz = freqInHz / 1000;
   return (TReal)aFastCount / freqInkHz;
   }
    

static RTest Test(_L("EZLIB"));

static TUint8 original[] = "compression is a constant source of depression and depression leads to multiple regressions in succession";
static TUint8 dictionary[] = "constant source multiple leads compression ";
static TUint8 badDictionary[] = "I'm not the dictionary";
static TUint8 compressed[256];
static TInt compressedSize; // length of compressed data in compressed.
static TUint8 uncompressed[256];
static TText output[256];
static TText originalU[256];
static TText dictionaryU[256];

static void RunTestL();

static void CompressBufferL(const TTestFlags &aFlags, RTest &aTest=Test);
static void DecompressBufferL(const TTestFlags &aFlags, RTest &aTest=Test);
static void DblDecompressBufferL(TInt aBufferSize, TBool aReset);
static void CompressDecompressL(const TTestFlags &aFlags);
static void DblCompressBufferL(TInt aBufferSize, TBool aReset = EFalse);

class CBufferManager : public CBase, public MEZBufferManager
	{
public:
	~CBufferManager();

	static CBufferManager *NewLC(TUint8 *aInput, TInt aInputLength, TUint8 *aOutput, TInt aOutputLength, TInt aBufferSize);
	static CBufferManager *NewL(TUint8 *aInput, TInt aInputLength, TUint8 *aOutput, TInt aOutputLength, TInt aBufferSize);

	void InitializeL(CEZZStream &aZStream);
	void NeedInputL(CEZZStream &aZStream);
	void NeedOutputL(CEZZStream &aZStream);
	void FinalizeL(CEZZStream &aZStream);

	void FakeNoInput() { iFakeNoInput = ETrue; }
	void FakeNoOutput() { iFakeNoOutput = ETrue; }

private:
	CBufferManager(TUint8 *aInput, TInt aInputLength, TUint8 *aOutput, TInt aOutputLength);
	void ConstructL(TInt aBufferSize);

private:
	TUint8 *iInput, *iOutput;
	TUint8 *iInputBuffer, *iOutputBuffer;
	TInt iBufferSize;
	TInt iOutputLength, iInputLength;
	TPtr8 iInputDescriptor;
	TPtr8 iOutputDescriptor;
	TBool iFakeNoInput;
	TBool iFakeNoOutput;
	};

CBufferManager::CBufferManager(TUint8 *aInput, TInt aInputLength, TUint8 *aOutput, TInt aOutputLength)
	:	iInput(aInput), 
		iOutput(aOutput), 
		iOutputLength(aOutputLength), 
		iInputLength(aInputLength),
		iInputDescriptor(NULL,0), 
		iOutputDescriptor(NULL,0), 
		iFakeNoInput(EFalse), 
		iFakeNoOutput(EFalse)
	{

	}

CBufferManager::~CBufferManager()
	{
	delete[] iInputBuffer;
	delete[] iOutputBuffer;
	}

CBufferManager *CBufferManager::NewLC(TUint8 *aInput, TInt aInputLength, TUint8 *aOutput, TInt aOutputLength, TInt aBufferSize)
	{
	CBufferManager *bm = new (ELeave) CBufferManager(aInput,aInputLength,aOutput,aOutputLength);
	CleanupStack::PushL(bm);
	bm->ConstructL(aBufferSize);
	return bm;
	}

CBufferManager *CBufferManager::NewL(TUint8 *aInput, TInt aInputLength, TUint8 *aOutput, TInt aOutputLength, TInt aBufferSize)
	{
	CBufferManager *bm = new (ELeave) CBufferManager(aInput,aInputLength,aOutput,aOutputLength);
	CleanupStack::PushL(bm);
	bm->ConstructL(aBufferSize);
	CleanupStack::Pop();
	return bm;
	}

void CBufferManager::ConstructL(TInt aBufferSize)
	{
	iBufferSize = aBufferSize;
	iInputBuffer = new (ELeave) TUint8[iBufferSize];
	iOutputBuffer = new (ELeave) TUint8[iBufferSize];

	}
	
void CBufferManager::InitializeL(CEZZStream &aZStream)
	{
	TInt remaining = iInputLength;
	if (remaining > iBufferSize)
		remaining = iBufferSize;
	Mem::Copy(iInputBuffer,iInput,remaining);
	iInputDescriptor.Set(iInputBuffer,remaining,iBufferSize);
	aZStream.SetInput(iInputDescriptor);
	iOutputDescriptor.Set(iOutputBuffer,0,iBufferSize);
	aZStream.SetOutput(iOutputDescriptor);
	iInput += remaining;
	}

void CBufferManager::NeedInputL(CEZZStream &aZStream)
	{
	TInt remaining;
	if (iFakeNoInput)
		remaining = 0;
	else
		{
		remaining = iInputLength - aZStream.TotalIn();
		if (remaining > iBufferSize)
			remaining = iBufferSize;
		Mem::Copy(iInputBuffer,iInput,remaining);
		}
	iInputDescriptor.Set(iInputBuffer,remaining,iBufferSize);
	aZStream.SetInput(iInputDescriptor);
	iInput += remaining;
	}
	

void CBufferManager::NeedOutputL(CEZZStream &aZStream)
	{
	TInt remaining;
	if (iFakeNoOutput)
		{
		remaining = 0;
		iOutputDescriptor.Set(iOutputBuffer,remaining,remaining);
		}
	else
		{
		remaining = iOutputLength - aZStream.TotalOut();
		if (remaining > iBufferSize)
			remaining = iBufferSize;
		}
	TPtrC8 od = aZStream.OutputDescriptor();
	Mem::Copy(iOutput,iOutputBuffer,od.Size());
	aZStream.SetOutput(iOutputDescriptor);
	iOutput += iBufferSize;
	}

void CBufferManager::FinalizeL(CEZZStream &aZStream)
	{
	TInt copy = aZStream.OutputDescriptor().Size();
	
	if (copy > 0)
		Mem::Copy(iOutput,iOutputBuffer,copy);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-1328
@SYMTestCaseDesc	    Decompression of stream test
@SYMTestPriority 	    High
@SYMTestActions  	    Compresses and decompresses a buffer and tests to see whether the orginal and decompression streams
						are the same.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

static void CompressDecompressL(const TTestFlags &aFlags)
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-1328 "));
	CompressBufferL(aFlags);
	DecompressBufferL(aFlags);
		
	if (aFlags.iFlags & TTestFlags::Unicode)
		{
		TPtr s1(REINTERPRET_CAST(TText *, original),sizeof(original));
		TPtr s2(REINTERPRET_CAST(TText *, uncompressed),sizeof(original));
		Test(s1.Compare(s2) == 0,__LINE__);
		}
	else
		{
		TPtrC8 s1(original);
		TPtrC8 s2(uncompressed);
		Test(s1.Compare(s2) == 0,__LINE__);
		}
	}

// Compresses the data in original and places it into compressed.

//Need to pass in RTest object as this function is called by a seperate thread sometimes
static void CompressBufferL(const TTestFlags &aFlags, RTest &aTest)
	{
	_LIT(KMsg,"\nTesting multi-step compresssion with buffer size %d\n");
	aTest.Printf(KMsg,aFlags.iBufferSize);
	aTest.Printf(KSeparator);
	CBufferManager *bm;
	TInt sourceSize;
	
	if (aFlags.iFlags & TTestFlags::Unicode)
		{
		sourceSize =  sizeof(original) * sizeof(TText);
		bm = CBufferManager::NewLC(REINTERPRET_CAST(TUint8*,originalU),sourceSize,compressed,256,aFlags.iBufferSize);
		}
	else
		{
		sourceSize = sizeof(original);
		bm = CBufferManager::NewLC(original,sourceSize,compressed,256,aFlags.iBufferSize);
		}
		

	if (aFlags.iFlags & TTestFlags::FakeNoInput)
		bm->FakeNoInput();
	if (aFlags.iFlags & TTestFlags::FakeNoOutput)
		bm->FakeNoOutput();
	CEZCompressor *def;

#if defined(_DEBUG) && defined(__WINS__)
	if (aFlags.iFlags & TTestFlags::AllocFail)
		__UHEAP_SETFAIL(RHeap::ETrueRandom,5);
#endif

	if (aFlags.iFlags & TTestFlags::Dictionary)
		def = CEZCompressor::NewLC(*bm,aFlags.iDictionary,aFlags.iLevel,aFlags.iWindowBits,
			aFlags.iMemLevel,aFlags.iStrategy);
	else
		def= CEZCompressor::NewLC(*bm,aFlags.iLevel,aFlags.iWindowBits,aFlags.iMemLevel,
			aFlags.iStrategy);

	while (def->DeflateL())
		{
		if (aFlags.iFlags & TTestFlags::Percent)
			{
			_LIT(KPer,"%d ");
			aTest.Printf(KPer,def->Progress(sourceSize));
			}
		}
	if (aFlags.iFlags & TTestFlags::Percent)
		aTest.Printf(_L("\n"));

	compressedSize = def->TotalOut();
	_LIT(KUnMsg,"Uncompressed Size %d\n");
	_LIT(KComMsg,"Compressed Size %d\n");

	if (aFlags.iFlags & TTestFlags::Corrupt)
		compressed[compressedSize >> 1] = 1;

	aTest.Printf(KUnMsg,def->TotalIn());
	aTest.Printf(KComMsg,compressedSize);
	CleanupStack::PopAndDestroy(2);
	}

// Decompresses data stored in compressed and writes it to uncompressed.

//Need to pass in RTest object as this function is called by a seperate thread sometimes
static void DecompressBufferL(const TTestFlags &aFlags, RTest &aTest)
    {
	_LIT(KMsg,"\nTesting multi-step decompresssion with buffer size %d\n");
	aTest.Printf(KMsg,aFlags.iBufferSize);
	aTest.Printf(KSeparator);
	CBufferManager *bm = CBufferManager::NewLC(compressed,compressedSize,uncompressed,256,aFlags.iBufferSize);
	
	if (aFlags.iFlags & TTestFlags::FakeNoInput)
		bm->FakeNoInput();
	if (aFlags.iFlags & TTestFlags::FakeNoOutput)
		bm->FakeNoOutput();

#if defined(_DEBUG) && defined(__WINS__)
	if (aFlags.iFlags & TTestFlags::AllocFail)
		__UHEAP_SETFAIL(RHeap::ETrueRandom,10);
#endif

	CEZDecompressor *in;
	if (aFlags.iFlags & TTestFlags::Dictionary)
		in = CEZDecompressor::NewLC(*bm,aFlags.iDictionary);
	else
		in = CEZDecompressor::NewLC(*bm);

#if defined(_DEBUG) && defined(__WINS__)
	if (aFlags.iFlags & TTestFlags::AllocFailInflate)
		__UHEAP_SETFAIL(RHeap::ETrueRandom,10);
#endif

	while (in->InflateL())
		{
		if (aFlags.iFlags & TTestFlags::Percent)
			{
			_LIT(KPer,"%d ");
			aTest.Printf(KPer,in->Progress(compressedSize));
			}
		}
	if (aFlags.iFlags & TTestFlags::Percent)
		aTest.Printf(_L("\n"));


	if (aFlags.iFlags & TTestFlags::Unicode)
		{
		TPtrC ptr(REINTERPRET_CAST(TText *,uncompressed),in->TotalOut() / sizeof(TText)); 
		aTest.Printf(ptr);
		aTest.Printf(_L("\n"));
		}
		else
		{
		TPtrC8 ptr8(uncompressed);
		TPtr ptr16(output,256);
		ptr16.Copy(ptr8);
		aTest.Printf(ptr16);
		aTest.Printf(_L("\n"));
		}
	CleanupStack::PopAndDestroy(2);
	}

static void DblCompressBufferL(TInt aBufferSize, TBool aReset)
	{
	CEZCompressor *def = NULL;

	_LIT(KMsg,"\nTesting multi-step compresssion with buffer size %d\n");
	_LIT(KUnMsg,"Uncompressed Size %d\n");
	_LIT(KComMsg,"Compressed Size %d\n");

	Test.Printf(KMsg,aBufferSize);
	Test.Printf(KSeparator,aBufferSize);
	CBufferManager *bm = CBufferManager::NewLC(original,sizeof(original),compressed,256,aBufferSize);
	def = CEZCompressor::NewLC(*bm);
			
	while (def->DeflateL()){}
		
	compressedSize = def->TotalOut();
	Test.Printf(KUnMsg,sizeof(original));
	Test.Printf(KComMsg,compressedSize);

	CleanupStack::Pop();
	CleanupStack::PopAndDestroy();
	CleanupStack::PushL(def);

	if (aReset)
		{
		bm = CBufferManager::NewLC(original,sizeof(original),compressed,256,aBufferSize);
		def->ResetL(*bm);
		}

	Test.Printf(KMsg,aBufferSize);
	Test.Printf(KSeparator,aBufferSize);
	
	while (def->DeflateL()){}
		
	compressedSize = def->TotalOut();
	Test.Printf(KUnMsg,sizeof(original));
	Test.Printf(KComMsg,compressedSize);

	CleanupStack::PopAndDestroy(2);
	}

static void DblDecompressBufferL(TInt aBufferSize, TBool aReset)
	{
	CEZDecompressor *def = NULL;

	_LIT(KMsg,"\nTesting multi-step decompresssion with buffer size %d\n");
	Test.Printf(KMsg,aBufferSize);
	Test.Printf(KSeparator);
	CBufferManager *bm = CBufferManager::NewLC(compressed,compressedSize,uncompressed,256,aBufferSize);
	def = CEZDecompressor::NewLC(*bm);
			
	while (def->InflateL()){}
		
	CleanupStack::Pop();
	CleanupStack::PopAndDestroy();
	CleanupStack::PushL(def);

	if (aReset)
		{
		bm = CBufferManager::NewLC(compressed,compressedSize,uncompressed,256,aBufferSize);
		def->ResetL(*bm);
		}

	Test.Printf(KMsg,aBufferSize);
	Test.Printf(KSeparator,aBufferSize);
	
	while (def->InflateL()){}
		
	TPtrC8 ptr8(uncompressed);
	TPtr ptr16(output,256);
	ptr16.Copy(ptr8);
	Test.Printf(ptr16);
	Test.Printf(_L("\n"));

	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0785
@SYMTestCaseDesc	    Multistep compression & decompression test using buffers smaller than source & destination 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test1L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0785 "));
	TTestFlags flags(0,8);
	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0786
@SYMTestCaseDesc	    Multistep compression & decompression test using buffers larger than source & destination 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 1024
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test2L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0786 "));
	TTestFlags flags(0,1024);
	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0787
@SYMTestCaseDesc	    Multistep compression test
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to TTestFlags::FakeNoInput,buffersize to 8
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test3()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0787 "));
	TTestFlags flags(TTestFlags::FakeNoInput,8);

	TRAPD(err,CompressBufferL(flags));
	Test (err==KErrNone);  // This test should succeed as providing no Input is a valid thing for
						   // NeedInputL to do.
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0788
@SYMTestCaseDesc	    Multistep compression test when NeedOutput provides no output
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to TTestFlags::FakeNoOutput,buffersize to 8
                        Check for buffer error.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test4()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0788 "));
	TTestFlags flags(TTestFlags::FakeNoOutput,8);

	TRAPD(err,CompressBufferL(flags));
	Test (err==KEZlibErrBuf);  
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0789
@SYMTestCaseDesc	    Multistep compression test with the same CEZCompressor,
                        resetting it after the first compression 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the buffersize to 8,reset value to true 
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test5L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0789 "));
	DblCompressBufferL(8,ETrue);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0790
@SYMTestCaseDesc	    Multistep compression test with the same CEZCompressor,
                        without resetting it after the first compression 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the buffersize to 8,reset value to false 
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test6()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0790 "));
	//Calling compress twice without reseting (ie passing in EFalse) causes leave
	TRAPD(err,DblCompressBufferL(8,EFalse));
	Test (err==KEZlibErrDeflateTerminated); 
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0791
@SYMTestCaseDesc	    Multistep compression & decompression test using a preset dictionary 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to TTestFlags::Dictionary,buffersize to 8 
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test7L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0791 "));
	TTestFlags flags(TTestFlags::Dictionary,8);
	flags.iDictionary.Set(dictionary,sizeof(dictionary));

	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0792
@SYMTestCaseDesc	    Multistep compression with compression level of 0 test
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8,compression level to 0  
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test8L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0792 "));
	TTestFlags flags(0,8);
	flags.iLevel = 0;

	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0793
@SYMTestCaseDesc	    Multistep compression test with compression level of 9 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8,compression level to 9  
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test9L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0793 "));
	TTestFlags flags(0,8);
	flags.iLevel = 9;

	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0794
@SYMTestCaseDesc	    Multistep compression test with invalid compression level 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8,compression level to 10.
                        Check for stream error when compression level is greater than 9  
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test10()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0794 "));
	TTestFlags flags(0,8);
	flags.iLevel = 10;

	TRAPD(err, CompressDecompressL(flags));
	//CompressL fails with KEZlibErrStream if level is greater than 9
	Test (err==KEZlibErrStream);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0795
@SYMTestCaseDesc	    Multistep compression test with window bits of 7 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 7
                        Check for stream error if window bits is less than 8
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test11()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0795 "));
	TTestFlags flags(0,7);
	flags.iWindowBits = 7;
	//CompressL fails with KEZlibErrStream if window bits is less than 8
	TRAPD(err, CompressDecompressL(flags));
	Test (err==KEZlibErrStream);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0796
@SYMTestCaseDesc	    Multistep compression test with window bits of 7 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8
                        Check for stream error if window bits is less than 9
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test12()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0796 "));
	TTestFlags flags(0,8);
	flags.iWindowBits = 7;
	//CompressL fails with KEZlibErrStream if window bits is less than 9
	TRAPD(err, CompressDecompressL(flags));
	Test (err==KEZlibErrStream);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0797
@SYMTestCaseDesc	    Multistep compression test with memory level = 1
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test13L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0797 "));
	TTestFlags flags(0,8);
	flags.iMemLevel = 1;

	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0798
@SYMTestCaseDesc	    Multistep compression test with memory level = 10 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8
                        Check for stream error if memory level is greater than 8.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test14()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0798 "));
	TTestFlags flags(0,8);
	flags.iMemLevel = 10;

	//CompressL fails with KEZlibErrStream if mem level greater than 8
	TRAPD(err, CompressDecompressL(flags));
	Test (err==KEZlibErrStream);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0799
@SYMTestCaseDesc	    Multistep compression test using only Huffman encoding 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8 
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test15L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0799 "));
	TTestFlags flags(0,8);
	flags.iStrategy = CEZCompressor::EHuffmanOnly;

	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0800
@SYMTestCaseDesc	    Multistep compression test using Filtered strategy 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test16L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0800 "));
	TTestFlags flags(0,8);
	flags.iStrategy = CEZCompressor::EFiltered;

	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0801
@SYMTestCaseDesc	    Multistep compression test when Alloc fails randomly 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test17()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0801 "));
	TTestFlags flags(TTestFlags::AllocFail,8);

	TRAPD(err,CompressBufferL(flags));
	Test(err == KErrNone||err == KErrNoMemory);  //alloc failure is random so no quarantees of failure
	//Need to reset the heap afterwards to avoid problems for next test.
	__UHEAP_RESET;
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0802
@SYMTestCaseDesc	    Multistep decompression test when NeedInput provides no input 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8
                        Check for data error,when no input is given
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test18L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0802 "));
	TTestFlags flags(0,8);
	CompressBufferL(flags); // make sure we initialize compressed properly

	flags.iFlags |= TTestFlags::FakeNoInput;
	TRAPD(err,DecompressBufferL(flags));
	// CEZDecompressor::InflateL has left with KEZlibErrBuf due to buffer error
	Test (err==KEZlibErrBuf); 
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0803
@SYMTestCaseDesc	    Multistep decompression test when NeedOutput provides no output 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8
                        Check for data error,when no output is given
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test19L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0803 "));
	TTestFlags flags(0,8);
	CompressBufferL(flags); // make sure we initialize compressed properly

	flags.iFlags |= TTestFlags::FakeNoOutput;
	TRAPD(err,DecompressBufferL(flags));
	// CEZDecompressor::InflateL has left with KEZlibErrBuf due to buffer error
	Test (err==KEZlibErrBuf); 
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0804
@SYMTestCaseDesc	    Multistep decompression test when NeedOutput provides no output 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to TTestFlags::Corrupt, buffersize to 8
                        Check for data error
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test20L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0804 "));
	TTestFlags flags(TTestFlags::Corrupt,8);
	CompressBufferL(flags); // make sure we initialize compressed properly
	
	flags.iFlags = 0;

	TRAPD(err,DecompressBufferL(flags));
	// CEZDecompressor::InflateL has left with KEZlibErrData due to buffer error
	Test (err==KEZlibErrData); 
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0805
@SYMTestCaseDesc	    Multistep decompression construction test when Alloc fails randomly 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0, buffersize to 8
                        Check for data error
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test21L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0805 "));
	TTestFlags flags(0,8);
	CompressBufferL(flags); // make sure we initialize compressed properly

	flags.iFlags |= TTestFlags::AllocFail;

	TRAPD(err,DecompressBufferL(flags));
	Test (err==KErrNone||err==KErrNoMemory); //Alloc failure is random so no guarantee it will always fail.
	__UHEAP_RESET;
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0806
@SYMTestCaseDesc	    Multistep decompression test when Alloc fails randomly
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0, buffersize to 8
                        Check for no memory errors.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void Test22L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0806 "));
	TTestFlags flags(0,8);
	CompressBufferL(flags); // make sure we initialize compressed properly

	flags.iFlags |= TTestFlags::AllocFailInflate;

	TRAPD(err,DecompressBufferL(flags));
	Test (err==KErrNone||err==KErrNoMemory); //Alloc failure is random so no guarentee it will always fail.
	__UHEAP_RESET;
	}

static void ThreadL()
	{
	RTest test2(_L("EZLIB - second thread"));
	test2.Title();
	_LIT(KStartMsg,"Starting Tests in second thread");
	test2.Start(KStartMsg);

	TTestFlags flags(TTestFlags::Dictionary,8);
	flags.iDictionary.Set(dictionary,sizeof(dictionary));

	TRAPD(err,CompressBufferL(flags, test2));
	test2 (err == KErrNone);
	flags.iFlags = 0;
	//This code leaves (used to panic before the fix for DEF112672)
	DecompressBufferL(flags, test2);
	User::Panic(_L("TestFailure failure"),0);
	test2.End();
	test2.Close();
	}

static TInt TestThread(TAny*)
	{
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (!cleanup)
		return KErrNoMemory;
	TRAPD(r,ThreadL());
	delete cleanup;
	return r;
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0807
@SYMTestCaseDesc	    Multistep decompression test when a dictionary is required and not provided.
@SYMTestPriority 	    High
@SYMTestActions  	    Test for panic and exit reason.
@SYMTestExpectedResults This test should pass without panic.
 						(Expected to panic before the fix for DEF112672)
@SYMREQ                 REQ0000
*/

void Test23()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0807 "));
	__UHEAP_MARK;

	RThread t;
	_LIT(KTestThread,"Panicing thread");
	Test.Printf(_L("launching thread"));
	Test (t.Create(KTestThread,&TestThread,0x2000,0x10000,0x100000,0,EOwnerThread) == KErrNone);
	TRequestStatus s;
	t.Logon(s);
	Test (s.Int()==KRequestPending);
	t.Resume();
	Test.Printf(_L("Awaiting completion"));
	User::WaitForRequest(s);
	User::SetJustInTime(ETrue);	// enable debugger panic handling
	_LIT(KCategory,"zlib");
	Test (t.ExitType()==EExitKill);
	Test (t.ExitCategory()!=KCategory);
	Test (t.ExitReason()==KEZlibErrData);// SetDictionaryL() leaving error code should be KEZlibErrData
	t.Close();

	__UHEAP_MARKEND;

	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0808
@SYMTestCaseDesc	    Multistep decompression test when the wrong dictionary is provided 
@SYMTestPriority 	    High
@SYMTestActions  	    Check for an error during inflating the dictionary
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test24L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0808 "));
	TTestFlags flags(TTestFlags::Dictionary,8);
	flags.iDictionary.Set(dictionary,sizeof(dictionary));

	CompressBufferL(flags);
	
	flags.iDictionary.Set(badDictionary,sizeof(badDictionary));
	
	TRAPD(err,DecompressBufferL(flags));
	// CEZDecompressor::SetDictionaryL() has left due to bad data
	Test (err==KEZlibErrInflateDictionary); 
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0809
@SYMTestCaseDesc	    Multistep decompression test twice with the same CEZDecompressor,
                        reseting it after the first decompression 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8,reset value to TRUE 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test25L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0809 "));
	TTestFlags flags(0,8);
	CompressBufferL(flags); // make sure we initialize compressed properly

	DblDecompressBufferL(8,ETrue);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0810
@SYMTestCaseDesc	    Multistep decompression test twice with the same CEZDecompressor,
                        resetting it after the first decompression
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to 0,buffersize to 8,reset value to FALSE 
                        Check for deflation already occured.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test26L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0810 "));
	TTestFlags flags(0,8);
	CompressBufferL(flags); // make sure we initialize compressed properly

	TRAPD(err,DblDecompressBufferL(8,EFalse));
	//Calling decompress twice without resenting causes leave
	Test (err==KEZlibErrInflateTerminated); 
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0811
@SYMTestCaseDesc	    Multistep compression and decompression test on unicode data 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to TTestFlags::Unicode,buffersize to 8 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test27L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0811 "));
	TPtrC8 ptr8(original);
	TPtr ptr16(originalU,256);
	ptr16.Copy(ptr8);

	TTestFlags flags(TTestFlags::Unicode,8);
	TPtrC8(REINTERPRET_CAST(TUint8 *,originalU),ptr16.Size());
	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0812
@SYMTestCaseDesc	    Multistep compression and decompression test on unicode data using a unicode dictionary
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to TTestFlags::Dictionary|TTestFlags::Unicode,buffersize to 8 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test28L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0812 "));
	TPtrC8 ptr8(original);
	TPtr ptr16(originalU,256);
	ptr16.Copy(ptr8);

	TPtrC8 dptr8(dictionary);
	TPtr dptr16(dictionaryU,256);
	dptr16.Copy(dptr8);


	TTestFlags flags(TTestFlags::Dictionary|TTestFlags::Unicode,8);
	flags.iDictionary.Set(REINTERPRET_CAST(TUint8*, dictionaryU),dptr16.Size());

	TPtrC8(REINTERPRET_CAST(TUint8 *,originalU),ptr16.Size());
	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0813
@SYMTestCaseDesc	    Multistep compression and decompression test on unicode data using an ASCII dictionary 
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to TTestFlags::Dictionary|TTestFlags::Unicode,buffersize to 8 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test29L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0813 "));
	TPtrC8 ptr8(original);
	TPtr ptr16(originalU,256);
	ptr16.Copy(ptr8);

	TTestFlags flags(TTestFlags::Dictionary|TTestFlags::Unicode,8);
	flags.iDictionary.Set(dictionary,sizeof(dictionary));

	TPtrC8(REINTERPRET_CAST(TUint8 *,originalU),ptr16.Size());
	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0814
@SYMTestCaseDesc	    Tests the percentage of completion during multistep compression/decompression
@SYMTestPriority 	    High
@SYMTestActions  	    Set the test flags to TTestFlags::Percent,buffersize to 8 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test30L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0814 "));
	TTestFlags flags(TTestFlags::Percent,8);

	CompressDecompressL(flags);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0815
@SYMTestCaseDesc	    Single step compression and decompression test
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up CEZCompressor::CompressL(),CEZCompressor::DecompressL() test
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test31L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0815 "));
	TPtrC8 originalD(original,sizeof(original));
	TPtr8 compressedD(compressed,0,256);
	TPtr8 uncompressedD(uncompressed,0,256);
	CEZCompressor::CompressL(compressedD,originalD);
	CEZDecompressor::DecompressL(uncompressedD,compressedD);

	Test.Printf(_L("\n"));
	Test.Printf(KSeparator);
	TPtr ptr16(output,256);
	ptr16.Copy(uncompressedD);
	Test.Printf(ptr16);
	Test.Printf(_L("\n"));
	Test(originalD.Compare(uncompressedD) == 0,__LINE__);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0816
@SYMTestCaseDesc	    Single step compression test when inadequate output buffer is provided
@SYMTestPriority 	    High
@SYMTestActions  	    Check for EZlibErrBuf buffer error flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test32()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0816 "));
	TPtrC8 originalD(original,sizeof(original));
	TPtr8 compressedD(compressed,0,1);

	Test.Printf(_L("\n"));
	Test.Printf(KSeparator);

	TRAPD(err,CEZCompressor::CompressL(compressedD,originalD));
	// Buffer error
	Test(err == KEZlibErrBuf);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0817
@SYMTestCaseDesc	    Single step decompression test when inadequate output buffer is provided
@SYMTestPriority 	    High
@SYMTestActions  	    Check for EZlibErrBuf buffer error flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void Test33L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0817 "));
	Test.Printf(_L("\n"));
	Test.Printf(KSeparator);
	TPtrC8 originalD(original,sizeof(original));
	TPtr8 compressedD(compressed,0,256);
	TPtr8 uncompressedD(uncompressed,0,1);
	CEZCompressor::CompressL(compressedD,originalD);
	TRAPD(err,CEZDecompressor::DecompressL(uncompressedD,compressedD));
	// Buffer error
	Test(err == KEZlibErrBuf);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0818
@SYMTestCaseDesc	    Testing graceful exit for zlib 1.1.3 decompression bug
@SYMTestPriority 	    High
@SYMTestActions  	    Check for buffer error after inflation
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Test34L()
    {
	Test.Printf(_L("\n"));
	Test.Printf(KSeparator);	
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0818 "));
	// 00000720.png contains specially crafted data block that exposes bug in 1.1.3 
	// decompression algorithm.  Causes InflateL() to return Z_BUF_ERROR.
	#if !defined(__WINS__)
	_LIT(KInputFile, "z:\\test\\png\\00000720.png");
	#else
	_LIT(KInputFile, "c:\\test\\png\\00000720.png");
	#endif
	_LIT(KTempDirectory, "c:\\test\\temp\\");	
	RFile input, output, tempInput;
	RFs rfs;
	rfs.Connect();
	rfs.MkDirAll(KTempDirectory);	
	
	TInt res = input.Open(rfs, KInputFile, EFileStream | EFileRead | EFileShareAny);
	Test(res==KErrNone); 
	CleanupClosePushL(input);	

	TFileName filename;
	TBuf8<1024> readBuf;
	TInt fileLen; 
	input.Size(fileLen); 
	res = input.Read(8,readBuf,fileLen);
	Test(res==KErrNone); 

	res = tempInput.Temp(rfs, KTempDirectory, filename, EFileStream | EFileWrite | EFileShareExclusive);
	Test(res==KErrNone); 
	CleanupClosePushL(tempInput);
	res = tempInput.Write(readBuf, fileLen-8);  // Create temporary file without the 8 byte PNG header.
	Test(res==KErrNone); 
	
	CEZFileBufferManager *fb = CEZFileBufferManager::NewLC(tempInput, output, 4096);
	CEZDecompressor *inf = CEZDecompressor::NewLC(*fb);
	
	TRAPD(err, while (inf->InflateL()){});
	//CEZDecompressor::InflateL() leaves
	Test (err==KEZlibErrBuf); 

	tempInput.Close();
	User::LeaveIfError(rfs.Delete(filename));
	input.Close();
	CleanupStack::PopAndDestroy(4);
    }

#if !(defined(__WINSCW__) || defined (_DEBUG))	
void DecompressFileL(const TDesC& aFilePath, RFs& aFs, TInt aTimingLimit)
{
   _LIT(KTempDirectory, "c:\\test\\temp\\");      
   aFs.MkDirAll(KTempDirectory);   
    	
   RFile inputFile, tempFile;
   //open the input compressed file
   TInt res = inputFile.Open(aFs, aFilePath, EFileStream | EFileRead | EFileShareExclusive);
   Test(res==KErrNone); 
   CleanupClosePushL(inputFile);         		
   
   //create a temporary file to be used as an output of the decompressor
   TFileName filename;
   res = tempFile.Temp(aFs, KTempDirectory, filename, EFileStream | EFileWrite | EFileShareExclusive);
   Test(res==KErrNone); 
   CleanupClosePushL(tempFile);
   
   //decompress the data from the input file several times            
   TInt startTime = User::FastCounter();
   for (TInt i = 0; i<10; i++)
   {
   		TInt position = 0;
   		inputFile.Seek(ESeekStart,position);		
   		CEZFileBufferManager *fb = CEZFileBufferManager::NewLC(inputFile, tempFile, 4096);
   		//create a decompressor object based on the file buffer manager
   		CEZDecompressor *decompressor = CEZDecompressor::NewLC(*fb);	
	   	TRAP(res, while (decompressor->InflateL()){});   
   		Test(res==KErrNone);     		   		
   		CleanupStack::PopAndDestroy(2);
   }
   TInt endTime = User::FastCounter();

   //close input and temporary file
   CleanupStack::PopAndDestroy(2);      
   //delete temporary file   
   aFs.Delete(filename);
   
   TReal netTime = FastCountToMilliseconds(endTime-startTime); 
   _LIT(KLogLine,"netTime =%f\n");   
   Test.Printf(KLogLine,netTime);   
   
   if (aTimingLimit)
	   {
	   Test(netTime <= aTimingLimit);
	   }
   else
	   {
	   Test.Printf(_L("Timings results are only checked on the RAM configuration of H2 and H4 HRP\n"));	
	   }   
}
#endif

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-3426
@SYMTestCaseDesc        Testing EZlib decompressor performance after enforcing compiler optimisation
@SYMTestPriority        Medium
@SYMTestActions         This test uses several typical pieces of compressed 
						image data as an input and inflates them 10 times. 
						The total duration of this operation is measured and 
						compared against a benchmarking limit. Only performed
						on UREL builds running on H2 and H4 hardware. 
@SYMTestExpectedResults Time spent on decompression should be less than a preset limit.
@SYMREQ                 REQ7363
*/

void Test35L()
   {	
   Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-3426 "));
#if !(defined(__WINSCW__) || defined (_DEBUG))	
   Test.Printf(_L("\n"));
   Test.Printf(KSeparator);   
   _LIT(KInputFile1, "z:\\test\\png\\compressed_data1");//A small-sized picture with
   														//transparent background which
   														//represents typical UI component 
   														//icon.
   	
   _LIT(KInputFile2, "z:\\test\\png\\compressed_data2");//Medium-sized picture with a
   													    //single image typical for a UI 
   													    //theme background image
   													    
   _LIT(KInputFile3, "z:\\test\\png\\compressed_data3");//Medium-sized picture with
   														//multiple small images on it 
   														//typical for a sheet of UI 
   														//component skins
   
   _LIT(KInputFile4, "z:\\test\\png\\compressed_data4");//Large-sized photo image 
   														   
   RFs rfs;
   rfs.Connect();
   CleanupClosePushL(rfs);   
   
   // determine media type of C drive
   TDriveInfo driveInf;
   TInt res = rfs.Drive(driveInf, EDriveC);   
   Test(res==KErrNone);
   
   TInt timingLimit1 = 0;
   TInt timingLimit2 = 0;
   TInt timingLimit3 = 0;
   TInt timingLimit4 = 0;
   // only check benchmarks if running on hardware, with UREL + RAM build (i.e. not NAND)   
   if (driveInf.iType == EMediaRam)
		{
		TInt muid = 0;   
   		res= HAL::Get(HAL::EMachineUid, muid);
      	Test(res==KErrNone);     		   		 
	    switch (muid)
	   		{
	   		case HAL::EMachineUid_OmapH2:
	   			timingLimit1 = 350;
	   			timingLimit2 = 1650;
	   			timingLimit3 = 2900;
	   			timingLimit4 = 2750;
	   			break;
	   		case HAL::EMachineUid_OmapH4:
	   			timingLimit1 = 200;
	   			timingLimit2 = 850;
	   			timingLimit3 = 1400;
	   			timingLimit4 = 1500;
	   			break;
	   		default:
	   			break;
	   		}
   		}
   
   DecompressFileL(KInputFile1, rfs, timingLimit1);
   DecompressFileL(KInputFile2, rfs, timingLimit2);
   DecompressFileL(KInputFile3, rfs, timingLimit3);
   DecompressFileL(KInputFile4, rfs, timingLimit4);   
   CleanupStack::PopAndDestroy();	   
#else
   Test.Printf(_L("This test is only performed on hardware UREL builds\n"));
#endif   
   }
    
/**
@SYMTestCaseID          SYSLIB-EZLIB-UT-3684
@SYMTestCaseDesc	    Test the memory deallocation of the CEZFileBufferManager object to ensure that
						there is no memory leak on success or failure of file compression and decompression
@SYMTestPriority 	    High
@SYMTestActions  	    Compressing and Decompressing a png image using CEZCompressor::InflateL() and
						CEZDecompressor::DeflateL()
@SYMTestExpectedResults Test must not fail and memory must not leak afterdeallocating CEZFileBufferManager 
						object (fb)
@SYMDEF                 DEF109756
*/
void Test36L()
    {
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-3684 "));
    Test.Printf(_L("\n"));
	Test.Printf(KSeparator);		
	//Set the path of input file and output files
	#if !defined(__WINS__)
	_LIT(KInputFile, "z:\\test\\png\\00000720.png");
	#else
	_LIT(KInputFile, "c:\\test\\png\\00000720.png");
	#endif
	_LIT(KOutputFile, "c:\\test\\temp\\00000720.z");
	_LIT(KUncompressedFile, "c:\\test\\temp\\000007201");
	
	RFs rfs;
	rfs.Connect();
	CleanupClosePushL(rfs);
	
	TPtrC inputFile(KInputFile);
	TPtrC outputFile(KOutputFile);
	TPtrC uncompressedFile(KUncompressedFile);

	RFile input;
	RFile output;
	TInt err;
	//Open the PNG file for compressing
	User::LeaveIfError(input.Open(rfs, inputFile,EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(input);
	
	//Create file to store output of file compressing
	err = output.Create(rfs, outputFile,EFileStream | EFileWrite | EFileShareExclusive);
	if (err == KErrAlreadyExists)//Just in case if the output file already exists then open it.
		User::LeaveIfError(output.Open(rfs, outputFile,EFileStream | EFileWrite | EFileShareExclusive));
	else 
		User::LeaveIfError(err);
	CleanupClosePushL(output);

__UHEAP_MARK;
	//Create and assign the CEZFileBufferManager object 'fb' as the compressor's buffer manager
	CEZFileBufferManager *fb = CEZFileBufferManager::NewLC(input,output,16384);
	CEZCompressor *def = CEZCompressor::NewLC(*fb);

	while (def->DeflateL()){/*do nothing*/}

	CleanupStack::PopAndDestroy(def);
	CleanupStack::PopAndDestroy(fb);
__UHEAP_MARKEND;

	CleanupStack::PopAndDestroy(&output);
	CleanupStack::PopAndDestroy(&input);
	//Now open the compressed file for decompressing
	User::LeaveIfError(input.Open(rfs, outputFile,EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(input);
	
	//Create file to store output of decompressing
	err = output.Create(rfs, uncompressedFile,EFileStream | EFileWrite | EFileShareExclusive);
	if (err == KErrAlreadyExists)//Just in case if the output file already exists then open it.
		User::LeaveIfError(output.Open(rfs, uncompressedFile,EFileStream | EFileWrite | EFileShareExclusive));
	else 
		User::LeaveIfError(err);
	
	CleanupClosePushL(output);

__UHEAP_MARK;
	//Create and assign the CEZFileBufferManager object 'fb' as the decompressor's buffer manager
	fb = CEZFileBufferManager::NewLC(input,output,16384);
	CEZDecompressor *inf = CEZDecompressor::NewLC(*fb);

	while (inf->InflateL()){/*do nothing*/}

	CleanupStack::PopAndDestroy(inf);
	CleanupStack::PopAndDestroy(fb);
__UHEAP_MARKEND;
	input.Close();
	output.Close();

	CleanupStack::PopAndDestroy(&output);
	CleanupStack::PopAndDestroy(&input);
	CleanupStack::PopAndDestroy(&rfs);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4298
@SYMTestCaseDesc	    Test CEZZStream functions through CEZCompressor class e.g. 
						Adler32(), AvailIn(), AvailOut()
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Compress stream of data using CEZCompressor::NewLC() & CEZCompressor::DeflateL()
						2.	Call CEZCompressor::Adler32() to get the checksum value of the uncompressed data
						3.	Call CEZCompressor::AvailIn() to get the number of bytes available at the next input byte
						4.	Call CEZCompressor::AvailOut() to get the remaining free space at next output byte target
@SYMTestExpectedResults The test succeeds with no errors i.e. KErrNone 
@SYMDEF                 REQ8024
*/

void TestCEZCompressorMethodsL()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4298 "));
	CBufferManager *bm;
	TInt sourceSize;
	
	sourceSize = sizeof(original);
	bm = CBufferManager::NewLC(original,sourceSize,compressed,256,8);
	CEZCompressor *compressor = CEZCompressor::NewLC(*bm);
	
	TInt32 adler32;
	TInt availIn;
	TInt availOut;
	
	while(compressor->DeflateL()){/*do nothing*/}
	
	adler32 = compressor->Adler32();
	Test(adler32 > 0);
	
	availIn = compressor->AvailIn();
	Test(availIn == 0);
	
	availOut = compressor->AvailOut();
	Test(availOut > 0);
	
	CleanupStack::PopAndDestroy(compressor);
	CleanupStack::PopAndDestroy(bm);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4300
@SYMTestCaseDesc	    TTest Zlibapiwrapper.cpp exported API's like inflate, 
						deflate, compress, decompress 
@SYMTestPriority 	    High
@SYMTestActions  	    Call the exported C APIs of Zlibapiwrapper.cpp 
						Note: This is done to check if the C APIs of zlib.lib 
						are called through ezlib.lib.
@SYMTestExpectedResults All exported C API's should be called (i.e. run a basic scenario
						of compressing some data) & test must not fail 
@SYMDEF                 REQ8024
*/

void TestZlibapiwrapperAPIL()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4300 "));
	z_stream deflateStream;
	const char inputData[] = "inputData!"; // data to compress
    uLong inputDataLength = (uLong)sizeof(inputData) + 1;     
    Byte *compressedDataBuffer; // output buffer
    //The buffer size is kept as 50 but incase the input data is changed..
    //...the buffer size also has to be increased accordingly
    TInt compressedDataBufferLength = 50;

	// Allocate memory for output buffer
    compressedDataBuffer = (Byte*)User::AllocZ(compressedDataBufferLength);
    Test(compressedDataBuffer != Z_NULL);
    CleanupStack::PushL(compressedDataBuffer);
    
     // Initialise the stream
	deflateStream.zalloc = (alloc_func)0;
	deflateStream.zfree = (free_func)0;
	deflateStream.opaque = (voidpf)0;
    
    int level = Z_DEFAULT_COMPRESSION;
   	int err = 0;
	int flush = 0;

    err = deflateInit(&deflateStream, level);

	if(err != Z_OK) 
		{ 
	    deflateEnd(&deflateStream);
		Test(err == Z_OK);
	    }	

	
	// Compress data in the input buffer
	deflateStream.next_in  = (Byte*)inputData;
	deflateStream.next_out = compressedDataBuffer; 
	
    do 
    	{
    	if (deflateStream.total_in < inputDataLength)
    		{
			deflateStream.avail_in = 1; // force small buffer
    		}
    	
    	flush = (deflateStream.total_in == inputDataLength) ?  Z_FINISH : Z_NO_FLUSH;
	
	    // run deflate() on input until output buffer not full, finish
	    // compression if all of source has been read in 
	    do 
	    	{
	    	if (deflateStream.total_out < compressedDataBufferLength)
	    		{
				deflateStream.avail_out = 1; // force small buffer
	    		}

	        err = deflate(&deflateStream, flush); 
	        if(err != Z_OK && err != Z_STREAM_END) 
	    		{ 
	    	    deflateEnd(&deflateStream);
	    		Test(err == Z_OK || err == Z_STREAM_END);
	    	    } 
	    	} while(deflateStream.avail_out == 0 && err == Z_OK);
    	} while(err != Z_STREAM_END); 
    	
    deflateEnd(&deflateStream);
	CleanupStack::PopAndDestroy(compressedDataBuffer);

	}
	
/**
@SYMTestCaseID			SYSLIB-EZLIB-UT-4001
@SYMTestCaseDesc		Test for DEF112672 - Panic ZLIB 11 - when installing the sisx file.
						The test attempts to decompress a buffer with the "Dictionary" flag set
						but with NULL dictionary buffer. The test should not cause an assert in the EZLib library.
@SYMTestPriority		Normal
@SYMTestActions			Test for DEF112672 - Panic ZLIB 11 - when installing the sisx file.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF112672
*/

void DEF112672L()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-UT-4001 "));
	TTestFlags flags(TTestFlags::Dictionary, 8);
	flags.iDictionary.Set(dictionary, sizeof(dictionary));
	CompressBufferL(flags);
	flags.iDictionary.Set(0, 0);
	TRAPD(err, DecompressBufferL(flags));
	Test(err == KEZlibErrData); 
	}

void RunTestL()
	{
	for (TInt testNum=1; testNum<KNumberOfTests; testNum++)
		{
		__UHEAP_MARK;
		_LIT(KTestNum,"\n\nRunning Test %d\n\n");
		TBuf<80> buf;
		buf.Format(KTestNum, testNum);
		Test.Next(buf);
		_LIT(KStrMsg,"%S");
		Test.Printf(KStrMsg, testTable[testNum].iDescription);
		testTable[testNum].iFn();
		__UHEAP_MARKEND;
		}
	DEF112672L();
	}	

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	Test.Title();
	_LIT(KStartMsg,"Starting Tests..");
	Test.Start(KStartMsg);
__UHEAP_MARK;
	TRAPD(err,RunTestL());
	Test (err==KErrNone);
	Test.End();
	Test.Close();
__UHEAP_MARKEND;
	delete cleanup;
	return KErrNone;
	}
