// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_EComClientRequrestPerfTestStep.cpp
// Tests the performance of ECom client calls
// 
//

/**
 @file
 @internalComponent
*/

#include "Te_EComClientRequestPerfTestStep.h"
#include <ecom/ecom.h>
#include "Interface.h"
#include "EComSessionAux.h"
#include "EcomTestUtils.h"
#include <cs_subconparams.h>
#include "EComPerfTestDllTwo.h"

#ifdef __ECOM_SERVER_PERFORMANCE__
// Constants for list interface testing
// There should be a single implementation of this interface in EComPerfTestDllOne.dll
const TUid KUidListImplTestInterface = {0x10009DF3};
// The resolver implementation uid in EComClientRequestPerfTestResolver.dll
const TUid KCustomResolverImplementationUid = {0x10009DF8};
// Constants for create implementation testing
// these are implementations from EComPerfTestDllTwo.dll
const TUid KUidCreateImplementation = {0x10009DF6};
// There should be a single implementation of this interface in EComPerfTestDllTwo.dll
const TUid KUidCreateImplTestInterface = {0x10009DF4};
// Constants for GetExtendedInterface and ListExtendedInterface testing
// these are implementations from EComExample10.dll
const TUid KExample10ImplementationUid = {0x10009E38};
const TUid KExample10ImplementationUid2 = {0x10009E3A};
// Expected data type
_LIT8(KExpectedDataTypeDllOne,"ECom_Performance1");
_LIT8(KExpectedDataTypeDllTwo,"ECom_Performance2-1");
_LIT8(KExpectedDataTypeExample10, "Example 10");
// Extended Interfaces
const TUid KExtendedInterfaceUid1 = {0x10009E44};
const TUid KExtendedInterfaceUid2 = {0x10009E45};
const TUid KExtendedInterfaceUid3 = {0x10009E46};
// Uids for list implementation and create implementation test with extended interfaces. 
const TUid KExample10InterfaceUid = {0x10009DBB}; 

// Start-up urel/armv5 test limits for H2 RAM(non-NAND) hardware configuration. These figures were determined using benchmarks from the ONB&TR.
// Although these limits are passed to CheckPerformance() for non urel/armv5 testing this method
// does not test against them. It is disabled.
// The following constants are test limit allowances that are used for each client request test. The limit
// for each test is determined by adding together these limits as appropriate.
// We do not provide special set of constants for H2 NAND cofiguration because H2 NAND tests are not performed.
const TReal KBasicH2RAM = 5;
const TReal KSecurityInfoRetrievedH2RAM = 5;
const TReal KDefaultResolverH2RAM = 2;
const TReal KCustomResolverH2RAM = 5;
const TReal KDllLoadedH2RAM = 3;
const TReal KFetchExtendedInterfaceH2RAM = 1; 
const TReal KListExtendedInterfacesH2RAM = 1;
const TReal KExtendedInterfacesMatchedH2RAM = 2; 
// Start-up urel/armv5 test limits for H4 RAM(non-NAND) hardware configuration. These constants were determined using benchmarks from the ONB&TR.
const TReal KBasicH4RAM = 2;
const TReal KSecurityInfoRetrievedH4RAM = 3;
const TReal KDefaultResolverH4RAM = 1;
const TReal KCustomResolverH4RAM = 3;
const TReal KDllLoadedH4RAM = 2;
const TReal KFetchExtendedInterfaceH4RAM = 1; 
const TReal KListExtendedInterfacesH4RAM = 1; 
const TReal KExtendedInterfacesMatchedH4RAM = 1; 
// Start-up urel/armv5 test limits for H4 NAND ROM configuration. These constants were determined using benchmarks from the ONB&TR.
const TReal KBasicH4NAND = 3;
const TReal KSecurityInfoRetrievedH4NAND = 9;
const TReal KDefaultResolverH4NAND = 2;
const TReal KCustomResolverH4NAND = 8;
const TReal KDllLoadedH4NAND = 7;
const TReal KFetchExtendedInterfaceH4NAND = 1; 
const TReal KListExtendedInterfacesH4NAND = 1; 
const TReal KExtendedInterfacesMatchedH4NAND = 2; 
// Start-up urel/armv5 test limits for H6 RAM(non-NAND) hardware configuration. These constants were determined using benchmarks from the ONB&TR.
const TReal KBasicH6RAM = 1;
const TReal KSecurityInfoRetrievedH6RAM = 1.5;
const TReal KDefaultResolverH6RAM = 1;
const TReal KCustomResolverH6RAM = 1.5;
const TReal KDllLoadedH6RAM = 1;
const TReal KFetchExtendedInterfaceH6RAM = 1; 
const TReal KListExtendedInterfacesH6RAM = 1; 
const TReal KExtendedInterfacesMatchedH6RAM = 1; 
// Start-up urel/armv5 test limits for H6 NAND ROM configuration. These constants were determined using benchmarks from the ONB&TR.
const TReal KBasicH6NAND = 1.5;
const TReal KSecurityInfoRetrievedH6NAND = 4;
const TReal KDefaultResolverH6NAND = 1;
const TReal KCustomResolverH6NAND = 4;
const TReal KDllLoadedH6NAND = 3;
const TReal KFetchExtendedInterfaceH6NAND = 1; 
const TReal KListExtendedInterfacesH6NAND = 1; 
const TReal KExtendedInterfacesMatchedH6NAND = 1; 
//Specify the number of iterations to do to obtain the average value for API time measurement
const TInt KNumberIteration=10;
#endif // __ECOM_SERVER_PERFORMANCE__

CEComClientRequestsPerfTest::CEComClientRequestsPerfTest() : CEComPerfTestBase(KEComClientRequestsPerfTest)
	{
	}

CEComClientRequestsPerfTest::~CEComClientRequestsPerfTest()
	{
	}

#ifdef __ECOM_SERVER_PERFORMANCE__
void CEComClientRequestsPerfTest::WaitForDllUnload()
	{
#if defined(__EPOC32__) && !defined(_DEBUG)
	// Timeout to unload dll when lazy unload is enabled - which it is by default
	// Used only for HW release builds
	const TInt32 KLazyUnloadTimeout = 150000000; // 2.5 minutes

	// Wait for DLL to unload - target release builds only
	User::After(KLazyUnloadTimeout);
#else
	INFO_PRINTF1(_L("*** WARNING: Use results with care no delay for DLL to unload ***\n"));
#endif // #if defined(__EPOC32__) && !defined(_DEBUG)

	}
#endif // __ECOM_SERVER_PERFORMANCE__

/*
 Test cases covered in the test step:
 
 1. Time calls to ListImplementationsL and CreateImplementationL without resolver, with default resolver and 
 with custom resolver under various conditions.
 	1.1 ListImplementationL performance testing without extended interfaces.
 	1.2 CreateImplementationL performance testing without extended interfaces.
 	1.3 ListImplementationL performance testing with extended interfaces.(TestCaseID:SYSLIB-ECOM-CT-3598)
 	1.4 CreateImplementationL performance testing with extended interfaces.(TestCaseID:SYSLIB-ECOM-CT-3599)
 
 2. Time calls to GetExtendedInterfaceL and ListExtendedInterfacesL under various conditions.
 	2.1 ListExtendedInterfacesL performance testing.(TestCaseID:SYSLIB-ECOM-CT-3600)
 	2.2 GetExtendedInterfaceL performance testing.(TestCaseID: SYSLIB-ECOM-CT-3601)

 1.3,1.4,2.1 and 2.2 are test cases for EC43
 @return EPass if the test completed successfully and the performance of the client calls are within the maximum
*/
TVerdict CEComClientRequestsPerfTest::doTestStepL()
	{
#ifdef __ECOM_SERVER_PERFORMANCE__
	RImplInfoPtrArray ifArray;
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	extendedInterfaces.AppendL(KExtendedInterfaceUid1);
	extendedInterfaces.AppendL(KExtendedInterfaceUid2);
	extendedInterfaces.AppendL(KExtendedInterfaceUid3);
	//set configuration-dependent timing limits (for non-NAND and NAND ROM configurations)
	TReal limitBasic = 0;
	TReal limitSecurityInfoRetrieved = 0;
	TReal limitDefaultResolver = 0;
	TReal limitCustomResolver = 0;
	TReal limitDllLoaded = 0;
	TReal limitFetchExtendedInterface = 0;
	TReal limitListExtendedInterfaces = 0;
	TReal limitExtendedInterfaceMatched = 0;
	
	THardwareConfiguration hardware_configuration = EComTestUtils::GetHardwareConfiguration();
	switch (hardware_configuration)
		{
		case EPlatformH2RAM:
	    	limitBasic = KBasicH2RAM;
			limitSecurityInfoRetrieved = KSecurityInfoRetrievedH2RAM;
			limitDefaultResolver = KDefaultResolverH2RAM;
			limitCustomResolver = KCustomResolverH2RAM;
			limitDllLoaded = KDllLoadedH2RAM;
			limitFetchExtendedInterface = KFetchExtendedInterfaceH2RAM;
			limitListExtendedInterfaces = KListExtendedInterfacesH2RAM;
			limitExtendedInterfaceMatched = KExtendedInterfacesMatchedH2RAM;
			INFO_PRINTF1(_L("Hardware configuration: H2 RAM"));
			break;
			
		case EPlatformH2NAND:
			INFO_PRINTF1(_L("Hardware configuration: H2 NAND"));
			INFO_PRINTF1(_L("***Performance testing on H2 NAND is not supported!***"));
			SetTestStepResult(EFail);			
			break;
			
		case EPlatformH2NANDDP:
			INFO_PRINTF1(_L("Hardware configuration: H2 NAND DP"));
			INFO_PRINTF1(_L("***Performance testing on H2 NAND DP is not supported!***"));
			SetTestStepResult(EFail);			
			break;
		
		case EPlatformH4RAM:
			limitBasic = KBasicH4RAM;
			limitSecurityInfoRetrieved = KSecurityInfoRetrievedH4RAM;
			limitDefaultResolver = KDefaultResolverH4RAM;
			limitCustomResolver = KCustomResolverH4RAM;
			limitDllLoaded = KDllLoadedH4RAM;
			limitFetchExtendedInterface = KFetchExtendedInterfaceH4RAM;
			limitListExtendedInterfaces = KListExtendedInterfacesH4RAM;
			limitExtendedInterfaceMatched = KExtendedInterfacesMatchedH4RAM;
			INFO_PRINTF1(_L("Hardware configuration: H4 RAM"));
			break;
		
		case EPlatformH4NAND:		
			limitBasic = KBasicH4NAND;
			limitSecurityInfoRetrieved = KSecurityInfoRetrievedH4NAND;
			limitDefaultResolver = KDefaultResolverH4NAND;
			limitCustomResolver = KCustomResolverH4NAND;
			limitDllLoaded = KDllLoadedH4NAND;
			limitFetchExtendedInterface = KFetchExtendedInterfaceH4NAND;
			limitListExtendedInterfaces = KListExtendedInterfacesH4NAND;
			limitExtendedInterfaceMatched = KExtendedInterfacesMatchedH4NAND;			
			INFO_PRINTF1(_L("Hardware configuration: H4 NAND"));			
			break;
		
		case EPlatformH4NANDDP:	
			INFO_PRINTF1(_L("Hardware configuration: H4 NAND DP"));
			break;
			
		case EPlatformH6RAM:
			limitBasic = KBasicH6RAM;
			limitSecurityInfoRetrieved = KSecurityInfoRetrievedH6RAM;
			limitDefaultResolver = KDefaultResolverH6RAM;
			limitCustomResolver = KCustomResolverH6RAM;
			limitDllLoaded = KDllLoadedH6RAM;
			limitFetchExtendedInterface = KFetchExtendedInterfaceH6RAM;
			limitListExtendedInterfaces = KListExtendedInterfacesH6RAM;
			limitExtendedInterfaceMatched = KExtendedInterfacesMatchedH6RAM;
			INFO_PRINTF1(_L("Hardware configuration: H6 RAM"));
			break;
		
		case EPlatformH6NAND:		
			limitBasic = KBasicH6NAND;
			limitSecurityInfoRetrieved = KSecurityInfoRetrievedH6NAND;
			limitDefaultResolver = KDefaultResolverH6NAND;
			limitCustomResolver = KCustomResolverH6NAND;
			limitDllLoaded = KDllLoadedH6NAND;
			limitFetchExtendedInterface = KFetchExtendedInterfaceH6NAND;
			limitListExtendedInterfaces = KListExtendedInterfacesH6NAND;
			limitExtendedInterfaceMatched = KExtendedInterfacesMatchedH6NAND;			
			INFO_PRINTF1(_L("Hardware configuration: H6 NAND"));			
			break;
		case EPlatformH6NANDDP:	
			INFO_PRINTF1(_L("Hardware configuration: H6 NAND DP"));
			break;
		
		case EPlatformWINSCW:
			INFO_PRINTF1(_L("Hardware configuration: WINSCW"));
			break;
		
		default:
			INFO_PRINTF1(_L("***Unrecognized platform!***"));
			SetTestStepResult(EFail);
			break;		
		}

	//
	// 1.1 ListImplementationL performance testing without extended interfaces.
	//
	TInt totalTime=0;
	INFO_PRINTF1(_L("Time servicing ListImplementationsL requests - each request returns 4 implementations:\n"));
	//
	// Time ListImplementationsL without resolver
	//
	TInt startTime = User::FastCounter();
	REComSession::ListImplementationsL(KUidListImplTestInterface, ifArray);
	TInt endTime = User::FastCounter();
	TEST(ifArray.Count()==4);
	ifArray.ResetAndDestroy();
	TReal netTime = FastCountToMilliseconds(endTime-startTime);
	CheckPerformance(netTime, limitBasic+limitSecurityInfoRetrieved, _L("Call with: no resolver, security info retrieval"));

	//
	// Average Time over KNumberIteration ListImplementationsL without resolver when the DLL security info is cached
	//
	totalTime=0;
	for (TInt i=0;i<KNumberIteration;i++)
		{
		startTime = User::FastCounter();
		REComSession::ListImplementationsL(KUidListImplTestInterface, ifArray);
		endTime = User::FastCounter();
		TEST(ifArray.Count()==4);
		ifArray.ResetAndDestroy();
		totalTime+=(endTime-startTime);
		}
		netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
		CheckPerformance(netTime, limitBasic, _L("Call with: no resolver, security info cached"));

	//
	// Average Time over KNumberIteration ListImplementationsL with default resolver when the DLL security info is cached
	//
	totalTime=0;
	TEComResolverParams params;
	params.SetDataType(KExpectedDataTypeDllOne());
	params.SetGenericMatch(ETrue);
	for (TInt i=0;i<KNumberIteration;i++)	
		{	
		startTime = User::FastCounter();
		REComSession::ListImplementationsL(KUidListImplTestInterface, params, ifArray);
		endTime = User::FastCounter();
		TEST(ifArray.Count()==4);
		ifArray.ResetAndDestroy();
		totalTime+=(endTime-startTime);	
		}
		netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
		CheckPerformance(netTime, limitBasic+limitDefaultResolver, _L("Call with: default resolver, security info cached"));

	//
	// Average Time over KNumberIteration ListImplementationsL with custom resolver when the DLL security check is cached
	//
	totalTime=0;
	for (TInt i=0;i<KNumberIteration;i++)	
		{	
		startTime = User::FastCounter();
		REComSession::ListImplementationsL(KUidListImplTestInterface, params, KCustomResolverImplementationUid, ifArray);
		endTime = User::FastCounter();
		TEST(ifArray.Count()==4);
		ifArray.ResetAndDestroy();
		totalTime+=(endTime-startTime);
		}
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitBasic+limitCustomResolver, _L("Call with: custom resolver, security info cached"));

	//
	// 1.2 CreateImplementationL performance testing without extended interfaces.
	//
	INFO_PRINTF1(_L("Time servicing CreateImplementationL requests:\n"));
	//
	// Time Create Implementation without resolver with retrieval of security info and dll will be loaded
	//
	TUid ecomDestructorId;
	startTime = User::FastCounter();
	CImplementationClassExampleTwo *ecomPtr = (CImplementationClassExampleTwo*)REComSession::CreateImplementationL(KUidCreateImplementation, ecomDestructorId);
	endTime = User::FastCounter();
	REComSession::DestroyedImplementation(ecomDestructorId);
	delete ecomPtr;
	netTime = FastCountToMilliseconds(endTime-startTime);
	CheckPerformance(netTime, limitBasic+limitDllLoaded+limitSecurityInfoRetrieved, _L("Call with: no resolver, dll to be loaded, security info retrieval"));
	REComSession::FinalClose();

	WaitForDllUnload();

	//
	// Time CreateImplementation without resolver when the DLL security info cached and dll will be loaded
	//
	startTime = User::FastCounter();
	ecomPtr = (CImplementationClassExampleTwo*)REComSession::CreateImplementationL(KUidCreateImplementation, ecomDestructorId);
	endTime = User::FastCounter();
	netTime = FastCountToMilliseconds(endTime-startTime);
	CheckPerformance(netTime, limitBasic+limitDllLoaded, _L("Call with: no resolver, dll to be loaded, security info cached"));

	//
	// Average Time over KNumberIteration CreateImplementation without resolver when the DLL security info is cached and dll is already
	// loaded
	//
	totalTime=0;
	CImplementationClassExampleTwo *ecomPtr2=NULL;
	TUid ecomDestructorId2;
	for (TInt i=0;i<KNumberIteration;i++)	
		{	
		startTime = User::FastCounter();
		ecomPtr2 = (CImplementationClassExampleTwo*)REComSession::CreateImplementationL(KUidCreateImplementation, ecomDestructorId2);
		endTime = User::FastCounter();
		totalTime+=endTime-startTime;
		REComSession::DestroyedImplementation(ecomDestructorId2);
		delete ecomPtr2;
		}
	REComSession::DestroyedImplementation(ecomDestructorId);
	delete ecomPtr;
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitBasic, _L("Call with: no resolver, dll already loaded, security info cached"));
	REComSession::FinalClose();

	WaitForDllUnload();

	//
	// Time CreateImplementation with default resolver when the DLL security info cached and dll must be loaded
	//
	params.SetDataType(KExpectedDataTypeDllTwo);
	startTime = User::FastCounter();
	ecomPtr = (CImplementationClassExampleTwo*)REComSession::CreateImplementationL(KUidCreateImplTestInterface, ecomDestructorId, params);
	endTime = User::FastCounter();
	netTime = FastCountToMilliseconds(endTime-startTime);
	CheckPerformance(netTime, limitBasic+limitDefaultResolver+limitDllLoaded, _L("Call with: default resolver, dll to be loaded, security info cached"));

	//
	// Average Time over KNumberIteration CreateImplementation with default resolver when the DLL security info is cached and dll is already
	// loaded
	//
	totalTime=0;
	for (TInt i=0;i<KNumberIteration;i++)	
		{
		startTime = User::FastCounter();
		ecomPtr2 = (CImplementationClassExampleTwo*)REComSession::CreateImplementationL(KUidCreateImplTestInterface, ecomDestructorId2, params);
		endTime = User::FastCounter();
		totalTime+=endTime-startTime;
		REComSession::DestroyedImplementation(ecomDestructorId2);
		delete ecomPtr2;
		}
	REComSession::DestroyedImplementation(ecomDestructorId);
	delete ecomPtr;	
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitBasic+limitDefaultResolver, _L("Call with: default resolver, dll already loaded, security info cached"));
	REComSession::FinalClose();

	WaitForDllUnload();

	//
	// Time CreateImplementation with custom resolver when the DLL security info cached and dll must be loaded
	//
	startTime = User::FastCounter();
	ecomPtr = (CImplementationClassExampleTwo*)REComSession::CreateImplementationL(
				KUidCreateImplTestInterface, ecomDestructorId, params, KCustomResolverImplementationUid);
	endTime = User::FastCounter();
	netTime = FastCountToMilliseconds(endTime-startTime);
	CheckPerformance(netTime, limitBasic+limitCustomResolver+limitDllLoaded, _L("Call with: custom resolver, dll to be loaded, security info cached"));

	//
	// Average Time over KNumberIteration CreateImplementation with default resolver when the DLL security info is cached and dll is already
	// loaded
	//
	totalTime=0;
	for (TInt i=0;i<KNumberIteration;i++)	
		{	
		startTime = User::FastCounter();
		ecomPtr2 = (CImplementationClassExampleTwo*)REComSession::CreateImplementationL(
					KUidCreateImplTestInterface, ecomDestructorId2, params, KCustomResolverImplementationUid);
		endTime = User::FastCounter();
		totalTime+=endTime-startTime;	
		REComSession::DestroyedImplementation(ecomDestructorId2);
		delete ecomPtr2;
		}
	REComSession::DestroyedImplementation(ecomDestructorId);
	delete ecomPtr;	
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitBasic+limitCustomResolver, _L("Call with: custom resolver, dll already loaded, security info cached"));

	//
	// 1.3 ListImplementationL performance testing with extended interfaces.
	//
	totalTime = 0;
	INFO_PRINTF1(_L("Time servicing ListImplementationsL with extended interfaces requests:\n"));
	//
	// Time ListImplementationsL with extended interfaces without resolver
	//
	startTime = User::FastCounter();
	REComSession::ListImplementationsL(KExample10InterfaceUid, extendedInterfaces, ifArray);
	endTime = User::FastCounter();
	TEST(ifArray.Count() == 1);
	ifArray.ResetAndDestroy();
	netTime = FastCountToMilliseconds(endTime-startTime);
	CheckPerformance(netTime, limitBasic+limitSecurityInfoRetrieved+limitExtendedInterfaceMatched, _L("Call with: extended interfaces, no resolver, security info retrieval"));

	//
	// Average Time over KNumberIteration ListImplementationsL with extended interfaces,without resolver when the DLL security info is cached
	//
	totalTime=0;
	for (TInt i = 0; i < KNumberIteration; i++)
		{
		startTime = User::FastCounter();
		REComSession::ListImplementationsL(KExample10InterfaceUid, extendedInterfaces, ifArray);
		endTime = User::FastCounter();
		TEST(ifArray.Count() == 1);
		ifArray.ResetAndDestroy();
		totalTime += (endTime-startTime);
		}
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitBasic+limitExtendedInterfaceMatched, _L("Call with: extended interfaces, no resolver, security info cached"));

	//
	// Average Time over KNumberIteration ListImplementationsL with extended interfaces, with default resolver when the DLL security info is cached
	//
	totalTime = 0;
	params.SetDataType(KExpectedDataTypeExample10());
	params.SetGenericMatch(ETrue);
	for (TInt i = 0; i < KNumberIteration; i++)	
		{	
		startTime = User::FastCounter();
		REComSession::ListImplementationsL(KExample10InterfaceUid, extendedInterfaces, params, ifArray);
		endTime = User::FastCounter();
		TEST(ifArray.Count() == 1);
		ifArray.ResetAndDestroy();
		totalTime += (endTime-startTime);	
		}
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitBasic+limitDefaultResolver+limitExtendedInterfaceMatched, _L("Call with: extended interfaces, default resolver, security info cached"));

	//
	// Average Time over KNumberIteration ListImplementationsL with extended interfaces, with custom resolver when the DLL security check is cached
	//
	totalTime = 0;
	for (TInt i = 0; i < KNumberIteration; i++)	
		{	
		startTime = User::FastCounter();
		REComSession::ListImplementationsL(KExample10InterfaceUid, extendedInterfaces, params, KCustomResolverImplementationUid, ifArray);
		endTime = User::FastCounter();
		TEST(ifArray.Count() == 1);
		ifArray.ResetAndDestroy();
		totalTime += (endTime-startTime);
		}
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitBasic+limitCustomResolver+limitExtendedInterfaceMatched, _L("Call with: extended interfaces, custom resolver, security info cached"));
	
	//
	// 1.4 CreateImplementationL performance testing with extended interfaces.
	//
	CExampleInterface* dllPtr = NULL;
	CExampleInterface* dllPtr2 = NULL;
	totalTime = 0;
	INFO_PRINTF1(_L("Time servicing CreateImplementationsL with extended interfaces requests:\n"));
	//
	// Time CreateImplementation with extended interfaces, default resolver when the DLL security info cached and dll must be loaded
	//
	params.SetDataType(KExpectedDataTypeExample10);
	startTime = User::FastCounter();
	dllPtr = reinterpret_cast<CExampleInterface*>(REComSession::CreateImplementationL(KExample10InterfaceUid, extendedInterfaces, ecomDestructorId, params));
	endTime = User::FastCounter();
	netTime = FastCountToMilliseconds(endTime-startTime);
	CheckPerformance(netTime, limitBasic+limitDefaultResolver+limitDllLoaded+limitExtendedInterfaceMatched, _L("Call with: extended interfaces, default resolver, dll to be loaded, security info cached"));
	
	//
	// Average Time over KNumberIteration CreateImplementation with extended interfaces, default resolver when the DLL security info is cached and dll is already
	// loaded
	//
	totalTime = 0;
	for (TInt i = 0; i < KNumberIteration; i++)	
		{
		startTime = User::FastCounter();
		dllPtr2 = reinterpret_cast<CExampleInterface*>(REComSession::CreateImplementationL(KExample10InterfaceUid, extendedInterfaces, ecomDestructorId2, params));
		endTime = User::FastCounter();
		totalTime += endTime-startTime;
		REComSession::DestroyedImplementation(ecomDestructorId2);
		delete dllPtr2;
		}
	REComSession::DestroyedImplementation(ecomDestructorId);
	delete dllPtr;	
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitBasic+limitDefaultResolver+limitExtendedInterfaceMatched, _L("Call with: extended interfaces, default resolver, dll already loaded, security info cached"));
	REComSession::FinalClose();
	WaitForDllUnload();

	//
	// Time CreateImplementation with extended interfaces, custom resolver when the DLL security info cached and dll must be loaded
	//
	startTime = User::FastCounter();
	dllPtr = reinterpret_cast<CExampleInterface*>(REComSession::CreateImplementationL(
		KExample10InterfaceUid, extendedInterfaces, ecomDestructorId, params, KCustomResolverImplementationUid));
	endTime = User::FastCounter();
	netTime = FastCountToMilliseconds(endTime-startTime);
	CheckPerformance(netTime, limitBasic+limitCustomResolver+limitDllLoaded+limitExtendedInterfaceMatched, _L("Call with: extended interfaces, custom resolver, dll to be loaded, security info cached"));

	//
	// Average Time over KNumberIteration CreateImplementation with extended interfaces, custom resolver when the DLL security info is cached and dll is already
	// loaded
	//
	totalTime = 0;
	for (TInt i = 0; i < KNumberIteration; i++)	
		{	
		startTime = User::FastCounter();
		dllPtr2 = reinterpret_cast<CExampleInterface*>(REComSession::CreateImplementationL(
			KExample10InterfaceUid, extendedInterfaces, ecomDestructorId2, params, KCustomResolverImplementationUid));
		endTime = User::FastCounter();
		totalTime += endTime-startTime;	
		REComSession::DestroyedImplementation(ecomDestructorId2);
		delete dllPtr2;
		}
	REComSession::DestroyedImplementation(ecomDestructorId);
	delete dllPtr;	
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitBasic+limitCustomResolver+limitExtendedInterfaceMatched, _L("Call with: extended interfaces, custom resolver, dll already loaded, security info cached"));
	
	REComSession::FinalClose();
	WaitForDllUnload();
	CleanupStack::PopAndDestroy(&extendedInterfaces);

	//
	// 2.1 ListExtendedInterfacesL performance testing.
	//
	RArray<TUid> ifInfo;
	CleanupClosePushL(ifInfo);
	totalTime = 0;
	for (TInt i = 0; i < KNumberIteration; i++)	
		{
		startTime = User::FastCounter();
		REComSession::ListExtendedInterfacesL(KExample10ImplementationUid,ifInfo);
		endTime = User::FastCounter();
		// Ensure test is valid by testing return parameters
		TEST(ifInfo.Count() == 3);
		totalTime += endTime-startTime;
		ifInfo.Reset();
		}
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitListExtendedInterfaces, _L("List extended interface. Three interfaces returned."));
		
	// Test with no interfaces returned
	totalTime = 0;
	for (TInt i = 0; i < KNumberIteration; i++)	
		{
		startTime = User::FastCounter();
		REComSession::ListExtendedInterfacesL(KExample10ImplementationUid2,ifInfo);
		endTime = User::FastCounter();
		// Ensure test is valid by testing return parameters
		TEST(ifInfo.Count() == 0);
		totalTime += endTime-startTime;
		ifInfo.Reset();
		}
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitListExtendedInterfaces, _L("List extended interface. No interface returned."));
	CleanupStack::PopAndDestroy(&ifInfo);

	//
	// 2.2 GetExtendedInterfaceL performance testing.
	//
	
	TUid instanceKey;
	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;
	dllPtr = REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KExample10ImplementationUid,
															   instanceKey,
															   &initParams));
	
	// Call extended interface here.
	totalTime = 0;
	for (TInt i = 0; i < KNumberIteration; i++)	
		{
		startTime = User::FastCounter();
		MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(REComSession::GetExtendedInterfaceL(instanceKey, KExtendedInterfaceUid1 ));
		endTime = User::FastCounter();
		totalTime += endTime - startTime;
		}
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitFetchExtendedInterface, _L("Get extended interface. Interface is part of main instantiation object."));
	totalTime = 0;
	for (TInt i = 0; i < KNumberIteration; i++)	
		{
		startTime = User::FastCounter();
		MExampleInterfaceExtended2* ext2 = static_cast<MExampleInterfaceExtended2*>(REComSession::GetExtendedInterfaceL(instanceKey, KExtendedInterfaceUid2 ));
		endTime = User::FastCounter();
		totalTime += endTime - startTime;
		}
	netTime = (FastCountToMilliseconds(totalTime))/KNumberIteration;
	CheckPerformance(netTime, limitFetchExtendedInterface, _L("Get extended interface. Interface is separate object."));
	REComSession::DestroyedImplementation(instanceKey);
	delete dllPtr;
	REComSession::FinalClose();
#else
	MacroNotDefinedError();
#endif // __ECOM_SERVER_PERFORMANCE__
	return TestStepResult();
	}
