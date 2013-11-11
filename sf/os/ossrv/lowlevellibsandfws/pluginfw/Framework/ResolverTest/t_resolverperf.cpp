// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <hal.h>
#include <e32test.h>
#include <ecom/ecom.h>
#include <ecom/ecomresolverparams.h>
#include "Interface.h" // for TExampleInterfaceInitParams
#include "EComPatchDataConstantv2.h"
#include "../EcomTestUtils/EcomTestUtils.h"

LOCAL_D RTest test(_L("Custom Resolver Load Time Performance Test"));

_LIT8(KDummyData,"dummy");

// KCExampleInterfaceUid is defined in "Interface.inl" !

// custom resolvers available for testing.
// 200126cd, A0001346 and A0001347 are allocated outside the
// ECOM Uid Allocations.doc
const TUid KDummyResolverUid2  = {0xA0001346};
const TUid KDummyResolverUid3  = {0xA0001347};
const TUid KExampleResolverUid = {0x10009DD0};
const TUid KMyResolverUid      = {0x10009E12};
const TUid KDummyResolverUid1  = {0x200126CD};

const TInt KArraySize = 5;

TUid MyResolvers[KArraySize] = {
	KDummyResolverUid2,
	KDummyResolverUid1,
	KExampleResolverUid,
	KMyResolverUid,
	KDummyResolverUid3
	};

// The custom resolver in RAMOnly dir
_LIT(KDummyRscInC, "c:\\resource\\plugins\\dummycustomresolver1.rsc");
_LIT(KDummyDllInC, "c:\\sys\\bin\\dummycustomresolver1.dll");
_LIT(KDummyRscInZ, "z:\\ramonly\\dummycustomresolver1.rsc");
_LIT(KDummyDllInZ, "z:\\ramonly\\dummycustomresolver1.dll");

// Copies the Plugins to specific folder for testing purpose
LOCAL_C void CopyPluginsL()
	{
	EComTestUtils::FileManCopyFileL(KDummyRscInZ, KDummyRscInC);
	EComTestUtils::FileManCopyFileL(KDummyDllInZ, KDummyDllInC);
	}

// Deleting plugin from the RAM for cleanup purpose
LOCAL_C void DeleteTestPlugin()
	{
	TRAP_IGNORE(EComTestUtils::FileManDeleteFileL(KDummyRscInC));
	TRAP_IGNORE(EComTestUtils::FileManDeleteFileL(KDummyDllInC));
	}

// converts fast counter ticks to milliseconds
LOCAL_D TReal FastCountToMilliseconds(TUint32 aFastCount)
	{
	TInt freqInHz;
	HAL::Get(HAL::EFastCounterFrequency, freqInHz);
	TReal freqInkHz = (TReal)freqInHz / 1000;
	return (TReal)aFastCount / freqInkHz;
	}

// counter wrap around subtraction
LOCAL_D TUint32 ElaspedCounterTicks(TUint32 aStart, TUint32 aEnd)
	{
	if (aEnd >= aStart)
		{
		return (aEnd - aStart);
		}
	else
		{
		return (KMaxTUint32 - aStart + aEnd + 1);
		}
	}

// sleep for 2 minutes and 15 seconds
LOCAL_D void WaitForLazyUnloadPassL(void)
	{
	// Performance results from winscw do not count.
	// Hence sleep only in armv5.
#ifdef __ARMCC__
	const TInt32 KLazyDllUnloadPeriod = 135; // actual is 2 minutes.

	test.Printf(_L("sleep %d s to avoid lazy DLL unload\n"), KLazyDllUnloadPeriod);
	User::After( KLazyDllUnloadPeriod * 1000000 );
	test.Printf(_L("wake up after sleeping %d s\n"), KLazyDllUnloadPeriod);
#endif

	// Wait one second for plugin discovery
#ifdef __X86GCC__
	test.Printf(_L("sleep 1 s \n"));
	User::After(1000000);
	test.Printf(_L("wake up!\n"));
#endif
	}

/** Check create test is within allowed limit.
@return True means test pass. False means fail.
*/
LOCAL_D TBool CheckCreatePerf(TReal aMilliseconds)
	{
	// These thresholds have 33% margin, i.e. when the limit is 320 ms,
	// the expected result is about 240 ms.
	const TReal KH2DpNandThreshold = 420.0;
	const TReal KH2NandThreshold = 420.0;
	const TReal KH2RamThreshold = 400.0;

	const TReal KH4DpNandThreshold = 200.0;
	const TReal KH4NandThreshold = 200.0;
	const TReal KH4RamThreshold = 180.0;
	const TReal KH4MMCThreshold = 225.0; 
	//INFO: 'KH4MMCThreshold' is the threshold in case of WDP enabled configuration on H4.
    //       This was arrived at by running the test on a ROM built without the USE_DATA_PAGING defined.
	//       The time taken by this operation in the above configuration was used to set this threshold

	const TReal KH6DpNandThreshold = 150.0;
	const TReal KH6NandThreshold = 150.0;
	const TReal KH6RamThreshold = 140.0;


	TReal threshold = 0.0;
	switch (EComTestUtils::GetHardwareConfiguration())
		{
		case EPlatformH2RAM:
			threshold = KH2RamThreshold;
			break;
		case EPlatformH2NAND:
			threshold = KH2NandThreshold;
			break;
		case EPlatformH2NANDDP:
			threshold = KH2DpNandThreshold;
			break;
		case EPlatformH4RAM:
			threshold = KH4RamThreshold;
			break;
		case EPlatformH4MMC:
			threshold = KH4MMCThreshold;
			break;
		case EPlatformH4NAND:
			threshold = KH4NandThreshold;
			break;
		case EPlatformH4NANDDP:
			threshold = KH4DpNandThreshold;
			break;
		case EPlatformH6RAM:
			threshold = KH6RamThreshold;
			break;
		case EPlatformH6NAND:
			threshold = KH6NandThreshold;
			break;
		case EPlatformH6NANDDP:
			threshold = KH6DpNandThreshold;
			break;
		default:
			// Ignore results on winscw and whatever unknown platform.
			test.Printf(_L("custom resolver create perf test: %f ms\n"), aMilliseconds);
			return ETrue;
		}

		test.Printf(_L("custom resolver create perf test: %f ms (limit %f)\n"), aMilliseconds, threshold);
#ifdef _DEBUG
		// the thresholds are for urel only
		return ETrue;
#else
		return (threshold > aMilliseconds);
#endif
	}

/** Check list test is within allowed limit.
@return True means test pass. False means fail.
*/
LOCAL_D TBool CheckListPerf(TReal aMilliseconds)
	{
	// These thresholds have 33% margin, i.e. when the limit is 300 ms,
	// the expected result is about 200 ms.
	const TReal KH2DpNandThreshold = 360.0;
	const TReal KH2NandThreshold = 360.0;
	const TReal KH2RamThreshold = 320.0;

	const TReal KH4DpNandThreshold = 150.0;
	const TReal KH4NandThreshold = 150.0;
	const TReal KH4RamThreshold = 140.0;
	const TReal KH4MMCThreshold = 140.0;
	//INFO: 'KH4MMCThreshold' is the threshold in case of WDP enabled configuration on H4.
    //       This was arrived at by running the test on a ROM built without the USE_DATA_PAGING defined.
	//       The time taken by this operation in the above configuration was used to set this threshold

	const TReal KH6DpNandThreshold = 150.0;
	const TReal KH6NandThreshold = 150.0;
	const TReal KH6RamThreshold = 140.0;


	TReal threshold = 0.0;
	switch (EComTestUtils::GetHardwareConfiguration())
		{
		case EPlatformH2RAM:
			threshold = KH2RamThreshold;
			break;
		case EPlatformH2NAND:
			threshold = KH2NandThreshold;
			break;
		case EPlatformH2NANDDP:
			threshold = KH2DpNandThreshold;
			break;
		case EPlatformH4RAM:
			threshold = KH4RamThreshold;
			break;
		case EPlatformH4MMC:
			threshold = KH4MMCThreshold;
			break;
		case EPlatformH4NAND:
			threshold = KH4NandThreshold;
			break;
		case EPlatformH4NANDDP:
			threshold = KH4DpNandThreshold;
			break;
		case EPlatformH6RAM:
			threshold = KH6RamThreshold;
			break;
		case EPlatformH6NAND:
			threshold = KH6NandThreshold;
			break;
		case EPlatformH6NANDDP:
			threshold = KH6DpNandThreshold;
			break;
		default:
			// Ignore results on winscw and whatever unknown platform.
			test.Printf(_L("custom resolver list perf test: %f ms\n"), aMilliseconds);
			return ETrue;
		}

		test.Printf(_L("custom resolver list perf test: %f ms (limit %f)\n"), aMilliseconds, threshold);
#ifdef _DEBUG
		// the thresholds are for urel only
		return ETrue;
#else
		return (threshold > aMilliseconds);
#endif
	}

/** Check create test in cache miss is within allowed limit.
@return True means test pass. False means fail.
*/
LOCAL_D TBool CheckCacheMissCreatePerf(TReal aMilliseconds)
	{
	// These thresholds have 33% margin, i.e. when the limit is 1200 ms,
	// the expected result is about 900 ms.
	const TReal KH2DpNandThreshold = 2250.0;
	const TReal KH2NandThreshold = 2250.0;
	const TReal KH2RamThreshold = 1000.0;

	const TReal KH4DpNandThreshold = 1350.0;
	const TReal KH4NandThreshold = 1350.0;
	const TReal KH4RamThreshold = 500.0;
	const TReal KH4MMCThreshold = 1350.0;
	//INFO: 'KH4MMCThreshold' is the threshold in case of WDP enabled configuration on H4.
    //       This was arrived at by running the test on a ROM built without the USE_DATA_PAGING defined.
	//       The time taken by this operation in the above configuration was used to set this threshold

	const TReal KH6DpNandThreshold = 1000.0;
	const TReal KH6NandThreshold = 1000.0;
	const TReal KH6RamThreshold = 350.0;

	TReal threshold = 0.0;
	switch (EComTestUtils::GetHardwareConfiguration())
		{
		case EPlatformH2RAM:
			threshold = KH2RamThreshold;
			break;
		case EPlatformH2NAND:
			threshold = KH2NandThreshold;
			break;
		case EPlatformH2NANDDP:
			threshold = KH2DpNandThreshold;
			break;
		case EPlatformH4RAM:
			threshold = KH4RamThreshold;
			break;
		case EPlatformH4MMC:
			threshold = KH4MMCThreshold;
			break;
		case EPlatformH4NAND:
			threshold = KH4NandThreshold;
			break;
		case EPlatformH4NANDDP:
			threshold = KH4DpNandThreshold;
			break;
		case EPlatformH6RAM:
			threshold = KH6RamThreshold;
			break;
		case EPlatformH6NAND:
			threshold = KH6NandThreshold;
			break;
		case EPlatformH6NANDDP:
			threshold = KH6DpNandThreshold;
			break;
		default:
			// Ignore results on winscw and whatever unknown platform.
			test.Printf(_L("Cache miss create perf test: %f ms\n"), aMilliseconds);
			return ETrue;
		}

		test.Printf(_L("Cache miss create perf test: %f ms (limit %f)\n"), aMilliseconds, threshold);
#ifdef _DEBUG
		// the thresholds are for urel only
		return ETrue;
#else
		return (threshold > aMilliseconds);
#endif
	}

/** Check list test in cache miss is within allowed limit.
@return True means test pass. False means fail.
*/
LOCAL_D TBool CheckCacheMissListPerf(TReal aMilliseconds)
	{
	// These thresholds have 33% margin, i.e. when the limit is 1200 ms,
	// the expected result is about 900 ms.
	const TReal KH2DpNandThreshold = 2250.0;
	const TReal KH2NandThreshold = 2250.0;
	const TReal KH2RamThreshold = 1000.0;

	const TReal KH4DpNandThreshold = 1350.0;
	const TReal KH4NandThreshold = 1350.0;
	const TReal KH4RamThreshold = 500.0;
	const TReal KH4MMCThreshold = 1350.0;
	//INFO: 'KH4MMCThreshold' is the threshold in case of WDP enabled configuration on H4.
    //       This was arrived at by running the test on a ROM built without the USE_DATA_PAGING defined.
	//       The time taken by this operation in the above configuration was used to set this threshold

	TReal threshold = 0.0;
	switch (EComTestUtils::GetHardwareConfiguration())
		{
		case EPlatformH2RAM:
			threshold = KH2RamThreshold;
			break;
		case EPlatformH2NAND:
			threshold = KH2NandThreshold;
			break;
		case EPlatformH2NANDDP:
			threshold = KH2DpNandThreshold;
			break;
		case EPlatformH4RAM:
			threshold = KH4RamThreshold;
			break;
		case EPlatformH4MMC:
			threshold = KH4MMCThreshold;
			break;
		case EPlatformH4NAND:
			threshold = KH4NandThreshold;
			break;
		case EPlatformH4NANDDP:
			threshold = KH4DpNandThreshold;
			break;
		default:
			// Ignore results on winscw and whatever unknown platform.
			test.Printf(_L("Cache miss list perf test: %f ms\n"), aMilliseconds);
			return ETrue;
		}

		test.Printf(_L("Cache miss list perf test: %f ms (limit %f)\n"), aMilliseconds, threshold);
#ifdef _DEBUG
		// the thresholds are for urel only
		return ETrue;
#else
		return (threshold > aMilliseconds);
#endif
	}

/** do custom resolver create test
@return total ticks spent during the create request.
*/
LOCAL_D TUint32 DoCreatePerfTestL(const TUid aResolverUid)
	{
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer	= 1;
	initParams.descriptor = NULL;
	TAny* p = NULL;
	TUid instanceKey;
	TEComResolverParams resolverparams;
	//Set any resolver data type as it will never reach the resolving part
	resolverparams.SetDataType(KDummyData);
	TUint32 startTime, endTime;

	startTime = User::FastCounter();
	p = REComSession::CreateImplementationL(KCExampleInterfaceUid,
											instanceKey,
											&initParams,
											resolverparams,
											aResolverUid);
	endTime = User::FastCounter();

	test(p != NULL);
	REComSession::DestroyedImplementation(instanceKey);
	CExampleInterface* impl = reinterpret_cast<CExampleInterface*>(p);
	delete impl;

	return ElaspedCounterTicks(startTime, endTime);
	}

/** do custom resolver list test
@return total ticks spent during the list request.
*/
LOCAL_D TUint32 DoListPerfTestL(const TUid aResolverUid)
	{
	TEComResolverParams resolverparams;
	//Set any resolver data type as it will never reach the resolving part
	resolverparams.SetDataType(KDummyData);
	RImplInfoPtrArray ifArray;
	TUint32 startTime, endTime;

	startTime = User::FastCounter();
	REComSession::ListImplementationsL(KCExampleInterfaceUid,
									   resolverparams,
									   aResolverUid,
									   ifArray);
	endTime = User::FastCounter();

	test(ifArray.Count() > 0);
	ifArray.ResetAndDestroy();

	return ElaspedCounterTicks(startTime, endTime);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-PT-4009
@SYMTestCaseDesc 	Time how long it takes to do 100 create requests and
					100 list requests involving custom resolvers.
@SYMTestPriority 	High
@SYMTestActions  	Run create and list requests in a loop.
@SYMTestExpectedResults on hw testing, the time must be within prescribed threshold.
@SYMCR CR1182
*/
LOCAL_D void RunResolverPerfTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-PT-4009 Time a number of create and list requests\n "));

	// If ECOM server is not yet running, the first message will take long
	// time because ECOM has to scandir and parse spi file. So do a dummy run
	// to kick start ECOM.
	DoListPerfTestL(MyResolvers[0]);

	const TInt KNumLoops = 100;
	TInt j;

	TUint32 createTicks = 0;
	TUint32 listTicks = 0;
	for (TInt i = 0; i < KNumLoops; i++)
		{
		j = i % KCustomResolverCacheSize;
		createTicks += DoCreatePerfTestL(MyResolvers[j]);
		listTicks += DoListPerfTestL(MyResolvers[j]);
		}

	REComSession::FinalClose();

	TReal createMs = FastCountToMilliseconds(createTicks);
	TBool createTestPass = CheckCreatePerf(createMs);

	TReal listMs = FastCountToMilliseconds(listTicks);
	TBool listTestPass = CheckListPerf(listMs);

	test(createTestPass);
	test(listTestPass);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-PT-4010
@SYMTestCaseDesc 	Verify CR1182 does not slow down list and create
	request significantly if there are a lot of cache misses.
@SYMTestPriority 	High
@SYMTestActions  	Repeatedly do list and create requests with five
	custom resolvers (1 more than max cache size). Time how long it takes.
@SYMTestExpectedResults on hw testing, the time must be within prescribed threshold.
@SYMCR CR1182
*/
LOCAL_D void CacheMissPerfTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-PT-4010 Cache miss performance test\n "));

	test(KArraySize > KCustomResolverCacheSize);

	const TInt KNumLoops = 100;
	TInt i, j;

	TUint32 createTicks = 0;
	for (i = 0; i < KNumLoops; i++)
		{
		j = i % KArraySize;
		createTicks += DoCreatePerfTestL(MyResolvers[j]);
		}

	TUint32 listTicks = 0;
	for (i = 0; i < KNumLoops; i++)
		{
		j = i % KArraySize;
		listTicks += DoListPerfTestL(MyResolvers[j]);
		}

	REComSession::FinalClose();

	TReal createMs = FastCountToMilliseconds(createTicks);
	TBool createTestPass = CheckCacheMissCreatePerf(createMs);

	TReal listMs = FastCountToMilliseconds(listTicks);
	TBool listTestPass = CheckCacheMissListPerf(listMs);

	test(createTestPass);
	test(listTestPass);
	}

LOCAL_C void RunTestL()
	{
	__UHEAP_MARK;
	CopyPluginsL();
	WaitForLazyUnloadPassL();

	RunResolverPerfTestL();
	CacheMissPerfTestL();

	// pause till ecom server flushes the cache.
	// otherwise may affect the next test to run.
	User::After(KCustomResolverCacheTimeout + 1000000);

	DeleteTestPlugin();

	__UHEAP_MARKEND;
	}

GLDEF_C TInt E32Main()
	{
	TInt err=KErrNone;
	__UHEAP_MARK;

	test.Title();
	test.Start(_L("CR1182 Perf. tests."));

	CTrapCleanup* cleanup = CTrapCleanup::New();
	test(cleanup != NULL);

	TRAP(err,RunTestL());
	test(err == KErrNone);

	delete cleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(err);
	}
