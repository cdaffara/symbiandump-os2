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
//

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <ecom/ecom.h>
#include <ecom/ecompanics.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
#include "../EcomTestUtils/EcomTestUtils.h"

//The tests test all the new APIs added to REComSession created for EC43

TUid KCExampleImplUid = {0x10009DC3};

const TInt KOneSecond = 1000000;

LOCAL_D RTest test(_L("t_extendedinterfaces"));
LOCAL_D RFs TheFs;

#define __UNUSED_VAR(var) var = var

// Used for OOM test
#define TEST_OOM_ERR if(err == KErrNoMemory) {User::Leave(err);}


static void ResetAndDestroyArray(TAny* aPtr)
	{
	(static_cast<RImplInfoPtrArray*>(aPtr))->ResetAndDestroy();
	}


/**
Test function for REcomSession::ListImplementationL

@param		aContainInvalidExtIf The bool value identifying if the extended interfaces list contains
			invalid extended interface Uid
@param		aUseDefResolver The bool value identifying if default resolver is used.
@param		aUseResolveParam The bool value identifying if resolve parameter is used.
@param		aNumEntriesInExtIFList The number of entries in the extended interfaces list.
@param		aImplInfoArray A reference to a client owned array which will be filled
			with interface implementation data.
*/
LOCAL_C void ListImplementationTestL(TBool aContainInvalidExtIf,
									 TBool aUseDefResolver,
									 TBool aUseResolveParam,
									 TInt aNumEntriesInExtIFList,
									 RImplInfoPtrArray& aImplInfoArray)
	{
	const TUid KInterfaceUid = {0x10009DBB};
	const TUid KExtendedInterfaceUid1 = {0x10009E44};
	const TUid KExtendedInterfaceUid2 = {0x10009E45};
	const TUid KExtendedInterfaceUid3 = {0x10009E46};
	const TUid KInvalidExtendedInterfaceUid2 = {0x10009E49};
	_LIT8(KExample10ImplementationTest,"Example 10");

	TUid resolverUid = {0x10009DD0};

	// Set up extended interfaces
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);

	if (!aContainInvalidExtIf)
		{
		if (aNumEntriesInExtIFList==1)
			{
			extendedInterfaces.AppendL(KExtendedInterfaceUid1);
			}
		else if (aNumEntriesInExtIFList>1)
			{
			extendedInterfaces.AppendL(KExtendedInterfaceUid2);
			extendedInterfaces.AppendL(KExtendedInterfaceUid3);
			}
		}
	else
		{
		extendedInterfaces.AppendL(KExtendedInterfaceUid1);
		extendedInterfaces.AppendL(KInvalidExtendedInterfaceUid2);
		}

	//Set up resolve parameter
	TEComResolverParams ResolverParams;
	if (aUseResolveParam)
		{
		ResolverParams.SetDataType(KExample10ImplementationTest());
		ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching
		if (aUseDefResolver) //ListImplementationTest2L,ListImplementationTest4L
			{
			REComSession::ListImplementationsL(KInterfaceUid,extendedInterfaces,ResolverParams,aImplInfoArray);
			}
		else //ListImplementationTest3L
			{
			REComSession::ListImplementationsL(KInterfaceUid,extendedInterfaces,ResolverParams,resolverUid,aImplInfoArray);
			}

		}
	else //ListImplementationTest1L, ListImplementationTest5L
		{
		REComSession::ListImplementationsL(KInterfaceUid,extendedInterfaces,aImplInfoArray);
		}
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3597
@SYMTestCaseDesc	    Tests for CImplementationInformation structure.
@SYMTestPriority 	    High
@SYMTestActions  	    Call ListImplementationL method to get a list of implementations with extended interfaces.
						Check that the expected extended interfaces are available in the CImplementationInformation
						data structure via GetExtendedInterfaceList method.
@SYMTestExpectedResults Extended interfaces are available in implementation information structure.
@SYMEC                  EC43
*/
LOCAL_C void ListImplementationTest5L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3597 "));
	// Test Starts...
	RImplInfoPtrArray ifArray;
	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = ETrue;
	TBool useResolveParam = EFalse;
	TInt numEntriesOnExtIFList = 3;

	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	//0x10009E38 is implemented in EcomExample10.dll
	TUid KExample10ImplUid = {0x10009E38};
	TUid KExtendedInterfaceUid1 = {0x10009E44};
	TUid KExtendedInterfaceUid2 = {0x10009E45};
	TUid KExtendedInterfaceUid3 = {0x10009E46};
	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 1);

	//tests extended interfaces
	CImplementationInformation* info = ifArray[0];
	test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
	test (KExample10ImplUid==info->ImplementationUid());

	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArray, &ifArray));

	// Gets extended interfaces list
	RArray<TUid> extInf;
	CleanupClosePushL(extInf);
	info->GetExtendedInterfaceListL(extInf);
	test(extInf[0] == KExtendedInterfaceUid1);
	test(extInf[1] == KExtendedInterfaceUid2);
	test(extInf[2] == KExtendedInterfaceUid3);
	CleanupStack::PopAndDestroy(&extInf);

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	CleanupStack::PopAndDestroy(&ifArray);
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3594
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationL created with an Interface Uid
						and invalid extended Interfaces.
@SYMTestPriority 	    High
@SYMTestActions  	    Call ListImplementationL method supporting interface UID and extended interfaces
						that are invalid.
@SYMTestExpectedResults No implementation is returned.
@SYMEC                  EC43
*/
LOCAL_C void ListImplementationTest4L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3594 "));
	// Test Starts...
	RImplInfoPtrArray ifArray;
	TBool containInvalidExtIf = ETrue;
	TBool useDefResolver = ETrue;
	TBool useResolveParam = ETrue;
	TInt numEntriesOnExtIFList = 2;

	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 0);

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3589
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL for known Interface UID,
						resolver match characteristics, resolver UID and extended interfaces.
@SYMTestPriority 	    High
@SYMTestActions  	    Call ListImplementationL method supporting interface UID, resolver parameters
						resolver UID and extended interfaces.
@SYMTestExpectedResults Expected implementations are returned.
@SYMEC                  EC43
*/
LOCAL_C void ListImplementationTest3L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3589 "));
	// Test Starts...
	RImplInfoPtrArray ifArray;
	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = EFalse;
	TBool useResolveParam = ETrue;

	//Number of entries in extended interfaces is 3.
	TInt numEntriesOnExtIFList = 3;
	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	TUid KImplementationUid = {0x10009E38};
	TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 1);

	//tests implementation uid
	for (TInt count = 0; count < availCount; ++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
		test(KImplementationUid == info->ImplementationUid());
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	//Number of entries in extended interfaces is 0.
	numEntriesOnExtIFList = 0;
	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 1);

	//tests implementation uid
	for (TInt count = 0; count < availCount; ++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	//Number of entries in extended interfaces is 1.
	numEntriesOnExtIFList = 1;
	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 1);

	//tests implementation uid
	for (TInt count = 0; count < availCount; ++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3588
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL for known Interface UID,
						resolver match characteristics and extended interfaces.
@SYMTestPriority 	    High
@SYMTestActions  	    Call ListImplementationL method supporting interface UID, resolver parameters and extended interfaces.
@SYMTestExpectedResults Expected implementations are returned.
@SYMEC                  EC43
*/
LOCAL_C void ListImplementationTest2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3588 "));
	// Test Starts...
	RImplInfoPtrArray ifArray;
	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = ETrue;
	TBool useResolveParam = ETrue;

	//Number of entries in extended interfaces is 3.
	TInt numEntriesOnExtIFList = 3;
	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	TUid KImplementationUid = {0x10009E38};
	TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 1);

	//tests implementation uid
	for (TInt count = 0; count < availCount; ++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
		test(KImplementationUid==info->ImplementationUid());
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	//Number of entries in extended interfaces is 0.
	numEntriesOnExtIFList = 0;
	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 1);

	//tests implementation uid
	for (TInt count = 0; count < availCount; ++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	//Number of entries in extended interfaces is 1.
	numEntriesOnExtIFList = 1;
	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 1);

	//tests implementation uid
	for (TInt count = 0; count < availCount; ++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3587
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL for known Interface UID and extended interfaces.
@SYMTestPriority 	    High
@SYMTestActions  	    Call ListImplementationL method supporting interface UID and extended interfaces.
@SYMTestExpectedResults Expected implementations are returned.
@SYMEC                  EC43
*/
LOCAL_C void ListImplementationTest1L()
	{
	//
	// Testing a overloaded method for ListImplementationsL
	// ------------------------------------------------------------------
	//
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3587 "));
	RImplInfoPtrArray ifArray;

	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = ETrue;
	TBool useResolveParam = EFalse;

	//Number of entries in extended interfaces is 3.
	TInt numEntriesOnExtIFList = 3;
	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	//0x10009E38 is implemented in EcomExample10.dll
	TUid KExample10ImplUid = {0x10009E38};
	TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 1);

	//tests implementation uid
	const CImplementationInformation* info = ifArray[0];
	test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
	test(KExample10ImplUid == info->ImplementationUid());

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	//Number of entries in extended interfaces is 0.
	numEntriesOnExtIFList = 0;
	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 3);

	//tests implementation uid
	for (TInt count = 0;count < availCount; ++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	//Number of entries in extended interfaces is 1.
	numEntriesOnExtIFList = 1;
	ListImplementationTestL(containInvalidExtIf,useDefResolver,useResolveParam,numEntriesOnExtIFList,ifArray);

	//Checks the result
	availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 2);

	//tests implementation uid
	for (TInt count = 0; count < availCount; ++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("Implementation Uid-%x. %s\n"),info->ImplementationUid(),((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3713
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL and REComSession::ListImplementationL
						with a dummy Interface Uid, dummy implementation Uid or dummy resolver Uid.
@SYMTestPriority 	    High
@SYMTestActions  	    Call CreateImplementationL method with a dummy interface UID
						Call CreateImplementationL method with a dummy implementation UID
						Call CreateImplementationL method with a dummy Resolver UID
						Call ListImplementationL method with a dummy interface UID
						Call ListImplementationL method with a dummy Resolver UID
@SYMTestExpectedResults Leave with KErrAugument and No implementation is created.
@SYMDEF                 DEF111196
*/
LOCAL_C void DummyUidTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3713 "));
	TUid instanceKey;
	TUid dummyUid;
	dummyUid.iUid = 0;
	const TUid KInterfaceUid = {0x10009DBB};
	const TUid KExample10InterfaceUid = {0x10009DBB};
	_LIT8(KExample10ImplementationTest,"Example 10");

	CExampleInterface* dllPtr = NULL;
	// Set up extended interfaces
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);

	//Set up resolve parameter
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KExample10ImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	CExampleInterface::TExampleInterfaceInitParams initParams;
	// Set up some empty initialisation parameters
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// CreateImplementationL with a dummy interface Uid
	TRAPD(err,dllPtr = REINTERPRET_CAST(CExampleInterface*,
				REComSession::CreateImplementationL(dummyUid,
													extendedInterfaces,
				  								    instanceKey,
												    ResolverParams,
												    &initParams)));
	test(err == KErrArgument);
	test(dllPtr == NULL);

	// CreateImplementationL with a dummy implementation Uid
	TRAP(err,dllPtr = REINTERPRET_CAST(CExampleInterface*,
				REComSession::CreateImplementationL(dummyUid,
				  								    instanceKey)));
	test(err == KErrArgument);
	test(dllPtr == NULL);

	// CreateImplementationL with a dummy resolver Uid
	TRAP(err, dllPtr = REINTERPRET_CAST(CExampleInterface*,
						REComSession::CreateImplementationL(KExample10InterfaceUid,
															extendedInterfaces,
														    instanceKey,
														    ResolverParams,
														    dummyUid,
														    &initParams)));
	test(err == KErrArgument);
	test(dllPtr == NULL);

	RImplInfoPtrArray ifArray;

	// ListImplementationL with a dummy interface Uid
	TRAP(err, REComSession::ListImplementationsL(dummyUid,extendedInterfaces,ResolverParams,ifArray));
	test(err == KErrArgument);
	test(ifArray.Count() == 0);

	// ListImplementationL with a dummy resolver Uid
	TRAP(err, REComSession::ListImplementationsL(KInterfaceUid,extendedInterfaces,ResolverParams,dummyUid,ifArray));
	test(err == KErrArgument);
	test(ifArray.Count() == 0);

	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3715
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL with ROMOnly resolver
@SYMTestPriority 	    High
@SYMTestActions  	    Call CreateImplementationL method with ROMOnly resolver
@SYMTestExpectedResults Expected implementation is created. Ensure there is no OOM error.
@SYMDEF                 DEF111196
*/
LOCAL_C void CreateImplementationTest7L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3715 "));
	// test start...
	const TUid KExample10InterfaceUid = {0x10009DBB};
	const TUid KExtendedInterfaceUid1 = {0x10009E44};
	const TUid KExtendedInterfaceUid2 = {0x10009E45};
	const TUid KExtendedInterfaceUid3 = {0x10009E46};
	const TUid KExample10ImplementationUid = {0x10009E38};

	_LIT8(KExample10ImplementationTest,"Example 10");

	// ROMOnly resolver Uid 0x10009D92
	TUid resolverUid = {0x10009D92};

	TUid instanceKey;

	CExampleInterface* dllPtr = NULL;
	// Set up extended interfaces
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	extendedInterfaces.AppendL(KExtendedInterfaceUid1);
	extendedInterfaces.AppendL(KExtendedInterfaceUid2);
	extendedInterfaces.AppendL(KExtendedInterfaceUid3);

	//Set up resolve parameter
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KExample10ImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	dllPtr = REINTERPRET_CAST(CExampleInterface*,
		 	REComSession::CreateImplementationL(KExample10InterfaceUid,
			 									extendedInterfaces,
											    instanceKey,
											    ResolverParams,
											    resolverUid));
	CleanupStack::PopAndDestroy(&extendedInterfaces);

	//checks the implementation created
	test(dllPtr != NULL);
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(instanceKey);

	delete dllPtr;
	}


/**
Test function for REcomSession::CreateImplementationL

@param		aContainInvalidExtIf The bool value identifying if the extended interfaces list contains
			invalid extended interface Uid
@param		aUseDefResolver The bool value identifying if default resolver is used.
@param		aUseInitParam The bool value identifying if initialise parameter is used.
@param		aNumEntriesInExtIFList The number of entries in the extended interfaces list.
@param		aInstanceKey A 32 bit identifer returned by the ECom framework to
			identify this instance to the framework.
@return		A pointer to the implementation created.
*/
LOCAL_C CExampleInterface* CreateImplementationTestL(TBool aContainInvalidExtIf,
									 TBool aUseDefResolver,
									 TBool aUseInitParam,
									 TUid& aInstanceKey)
	{
	const TUid KExample10InterfaceUid = {0x10009DBB};
	const TUid KExtendedInterfaceUid1 = {0x10009E44};
	const TUid KExtendedInterfaceUid2 = {0x10009E45};
	const TUid KExtendedInterfaceUid3 = {0x10009E46};
	const TUid KInvalidExtendedInterfaceUid2 = {0x10009E49};
	_LIT8(KExample10ImplementationTest,"Example 10");

	TUid resolverUid = {0x10009DD0};

	CExampleInterface* dllPtr = NULL;
	// Set up extended interfaces
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);

	if (!aContainInvalidExtIf)
		{
		extendedInterfaces.AppendL(KExtendedInterfaceUid1);
		extendedInterfaces.AppendL(KExtendedInterfaceUid2);
		extendedInterfaces.AppendL(KExtendedInterfaceUid3);
		}
	else
		{
		extendedInterfaces.AppendL(KExtendedInterfaceUid1);
		extendedInterfaces.AppendL(KInvalidExtendedInterfaceUid2);
		}

	//Set up resolve parameter
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KExample10ImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	CExampleInterface::TExampleInterfaceInitParams initParams;
	if  (aUseInitParam) //CreateImplementationTest3L,CreateImplementationTest5L
		{
		// Set up some empty initialisation parameters
		initParams.integer		= 0;
		initParams.descriptor	= NULL;
		if (aUseDefResolver) //CreateImplementationTest3L
			{
			dllPtr = REINTERPRET_CAST(CExampleInterface*,
					 	REComSession::CreateImplementationL(KExample10InterfaceUid,
															extendedInterfaces,
						  								    aInstanceKey,
														    ResolverParams,
														    &initParams));
			}
		else //CreateImplementationTest5L
			{
			dllPtr = REINTERPRET_CAST(CExampleInterface*,
						REComSession::CreateImplementationL(KExample10InterfaceUid,
															extendedInterfaces,
														    aInstanceKey,
														    ResolverParams,
														    resolverUid,
														    &initParams));
			}
		}
	else //CreateImplementationTest2L, CreateImplementationTest4L, CreateImplementationTest6
		{
		if (aUseDefResolver) //CreateImplementationTest2L, CreateImplementationTest6
			{
			dllPtr = REINTERPRET_CAST(CExampleInterface*,
						REComSession::CreateImplementationL(KExample10InterfaceUid,
															extendedInterfaces,
														    aInstanceKey,
														    ResolverParams));
			}
		else //CreateImplementationTest4L
			{
			dllPtr = REINTERPRET_CAST(CExampleInterface*,
					 	REComSession::CreateImplementationL(KExample10InterfaceUid,
						 									extendedInterfaces,
 		   												    aInstanceKey,
														    ResolverParams,
														    resolverUid));
			}
		}
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	return dllPtr;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3708
@SYMTestCaseDesc        Tests for calling DestroyedImplementation() twice.
@SYMTestPriority         High
@SYMTestActions          Call CreateImplementationL method to load a plugin.
                        Delete the plugin by calling DestroyedImplementation() twice.
                        Delete pointer. Call DestroyedImplementation().
                        Cal FinalClose(). Call DestroyedImplementation().
@SYMTestExpectedResults Second call to DestroyedImplementation() should succeed.
@SYMDEF                 DEF111428
*/
LOCAL_C void DestroyedImplementationTestL()
    {
    // Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3708 "));
    TUid instanceKey;
    TBool containInvalidExtIf = EFalse;
    TBool useDefResolver = EFalse;
    TBool useInitParam = ETrue;
    TUid KExample10ImplementationUid = {0x10009E38};

    test.Printf(_L("Loading Plugin..."));

    // Test create implementation by pass in instance key.
    CExampleInterface* dllPtr = CreateImplementationTestL(containInvalidExtIf,useDefResolver,useInitParam,instanceKey);
    test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
    test(KExample10ImplementationUid == dllPtr->ImplId());

    // the destructor calls DestroyedImplementation()
    test.Printf(_L("Calling DestroyedImplementation the 1st time through the plugin destructor..."));
    delete dllPtr;

    test.Printf(_L("Calling DestroyedImplementation a 2nd time directly through REComSession..."));
    REComSession::DestroyedImplementation(instanceKey);

    test.Printf(_L("Calling DestroyedImplementation a 3rd time directly through REComSession but after FinalClose..."));

    REComSession::DestroyedImplementation(instanceKey);
    }

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3595
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an Interface Uid
						and invalid extended Interfaces.
@SYMTestPriority 	    High
@SYMTestActions  	    Call CreateImplementationL method supporting interface UID and extended interfaces
						that are invalid.
@SYMTestExpectedResults No implementation is created.
@SYMEC                  EC43
*/
LOCAL_C void CreateImplementationTest6()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3595 "));
	TUid instanceKey;
	TBool containInvalidExtIf = ETrue;
	TBool useDefResolver = ETrue;
	TBool useInitParam = EFalse;

	test.Printf(_L("Loading Plugin..."));
	TInt err;
	TRAP(err,CExampleInterface* dllPtr = CreateImplementationTestL(containInvalidExtIf,useDefResolver,useInitParam,instanceKey));

	test(KErrNotFound == err);

	test.Printf(_L("Implementation is not found.\n"));
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3593
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an resolver Uid
@SYMTestPriority 	    High
@SYMTestActions  	    Call CreateImplementationL method supporting interface UID, extended interfaces,
						construction parameters, resolver parameters and resolver UID.
@SYMTestExpectedResults Expected implementation is created and no memory leaks.
@SYMEC                  EC43
*/
LOCAL_C void CreateImplementationTest5L()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3593 "));
	TUid instanceKey;
	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = EFalse;
	TBool useInitParam = ETrue;
	TUid KExample10ImplementationUid = {0x10009E38};

	test.Printf(_L("Loading Plugin..."));

	// Test create implementation by pass in instance key.
	CExampleInterface* dllPtr = CreateImplementationTestL(containInvalidExtIf,useDefResolver,useInitParam,instanceKey);
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(instanceKey);
	delete dllPtr;

	// Test create implementation by pass in offset of the instance key.
	dllPtr =  CExampleInterface::NewL8();
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3592
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an resolver Uid
@SYMTestPriority 	    High
@SYMTestActions  	    Call CreateImplementationL method supporting interface UID, extended interfaces,
						resolver parameters and resolver UID.
@SYMTestExpectedResults Expected implementation is created and no memory leaks.
@SYMEC                  EC43
*/
LOCAL_C void CreateImplementationTest4L()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3592 "));
	TUid instanceKey;
	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = EFalse;
	TBool useInitParam = EFalse;
	TUid KExample10ImplementationUid = {0x10009E38};

	test.Printf(_L("Loading Plugin..."));

	// Test create implementation by pass in instance key.
	CExampleInterface* dllPtr = CreateImplementationTestL(containInvalidExtIf,useDefResolver,useInitParam,instanceKey);
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(instanceKey);
	delete dllPtr;

	// Test create implementation by pass in offset of the instance key.
	dllPtr =  CExampleInterface::NewL6();
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3591
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an Interface Uid,
						ExtendedInterface, construction parameters and resolver parameters
@SYMTestPriority 	    High
@SYMTestActions  	    Call CreateImplementationL method supporting interface UID,extended interfaces,
						construction parameters and resolver parameters.
@SYMTestExpectedResults Expected implementation is created and no memory leaks.
@SYMEC                  EC43
*/
LOCAL_C void CreateImplementationTest3L()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3591 "));
	TUid instanceKey;
	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = ETrue;
	TBool useInitParam = ETrue;
	TUid KExample10ImplementationUid = {0x10009E38};

	test.Printf(_L("Loading Plugin..."));

	// Test create implementation by pass in instance key.
	CExampleInterface* dllPtr = CreateImplementationTestL(containInvalidExtIf,useDefResolver,useInitParam,instanceKey);
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(instanceKey);
	delete dllPtr;

	// Test create implementation by pass in offset of the instance key.
	dllPtr =  CExampleInterface::NewL7();
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3590
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an Interface Uid,
						ExtendedInterface and resolver parameters.
@SYMTestPriority 	    High
@SYMTestActions  	    Call CreateImplementationL method supporting interface UID,extended interfaces, and resolver parameters.
@SYMTestExpectedResults Expected implementation is created and no memory leaks.
@SYMEC                  EC43
*/
LOCAL_C void CreateImplementationTest2L()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3590 "));
	TUid instanceKey;
	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = ETrue;
	TBool useInitParam = EFalse;
	TUid KExample10ImplementationUid = {0x10009E38};

	test.Printf(_L("Loading Plugin..."));
	// Test create implementation by pass in instance key.
	CExampleInterface* dllPtr = CreateImplementationTestL(containInvalidExtIf,useDefResolver,useInitParam,instanceKey);
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(instanceKey);
	delete dllPtr;

	// Test create implementation by pass in offset of the instance key.
	dllPtr =  CExampleInterface::NewL5();
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3586
@SYMTestCaseDesc	    Tests for GetExtendedInterfaceL, Manual release
@SYMTestPriority 	    High
@SYMTestActions  	    Create an implementation. Fetch two extended interfaces (one build in, and one
						stand alone and requires release). Manually release	the stand alone extended
						interface. Destroy implementation.
@SYMTestExpectedResults Ensure that manual release releases the stand alone interface correctly.
@SYMEC                  EC43
*/
LOCAL_C void GetExtendedInterfaceTest4L()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3586 "));
	test.Printf(_L("Loading Plugin..."));
	const TUid KExample10ImplementationUid = {0x10009E38};

	CExampleInterface* dllPtr = CExampleInterface::NewL2(KExample10ImplementationUid);
	CleanupStack::PushL(dllPtr);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	// Call extended interface here.
	const TUid KExample10InterfaceUid1 = {0x10009E44};
	const TUid KExample10InterfaceUid2 = {0x10009E45};

    MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid1));
	ext1->DoMethodExtended();

	MExampleInterfaceExtended2* ext2 = static_cast<MExampleInterfaceExtended2*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid2));
	ext2->DoMethodExtended2();

	dllPtr->ManuallyReleaseExtendedInterfaceL(KExample10InterfaceUid2);

	test.Printf(_L("Destroying Plugin..."));
	CleanupStack::Pop(dllPtr);
	delete dllPtr;

	// Test Ends...
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-4001
@SYMTestCaseDesc		Tests GetExtendedInterfaceL with invalid instance keys
@SYMTestPriority		High
@SYMTestActions			Retrieve extended interface using unknown instance key
						Retrieve extended interface using null instance key
@SYMTestExpectedResults	Ensure that correct leaves occur.
@SYMDEF					DEF111553
*/
LOCAL_C void GetExtendedInterfaceTest5L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-4001 "));
	// Test Starts...
	TUid instanceKey;
	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = EFalse;
	TBool useInitParam = ETrue;

	// get valid instance key by creating implementation
	CExampleInterface* dllPtr = CreateImplementationTestL(containInvalidExtIf,useDefResolver,useInitParam,instanceKey);

	// remove implementation
	REComSession::DestroyedImplementation(instanceKey);
	delete dllPtr;

	// Attempt to fetch extended interface with instance key that is no longer valid but
	// passes the ASSERT test
	const TUid KExtendedInterfaceUid = {0x10009E44};
	TRAPD(err, REComSession::GetExtendedInterfaceL(instanceKey,KExtendedInterfaceUid));
	test(err = KErrNotFound);

	// Attempt to fetch extended interface with null instance key
	TRAP(err, REComSession::GetExtendedInterfaceL(KNullUid,KExtendedInterfaceUid));
	test(err = KErrArgument);

	// Test Ends...
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3581
@SYMTestCaseDesc	    Tests for REComSession::ListExtendedInterfaces with a valid Implementation UID.
						Tests also for non-existent Implementation UID and PLUGIN1 implementation UID.
						Does some create and destruction of	implementations before and after calling
						ListExtendedInterfaces to mimic typical	usage.
@SYMTestPriority 	    High
@SYMTestActions  	    List the extended interfaces. Check expected extended interfaces are returned.
						Create an implementation. Ensure that Listing of the extended interface
						still works after the create.
						Delete the implementation. List extended interfaces again.
						List the extended interfaces with a PLUGIN1 implementation UID.
@SYMTestExpectedResults Ensure all ListExtendedInterfaces calls return with expected extended interfaces.
@SYMEC                  EC43
@SYMDEF					DEF111196
*/
LOCAL_C void ListExtendedInterfaceTestL()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3581 "));
	const TUid KExample10ImplementationUid = {0x10009E38};
	const TUid KExample10ImplementationUid2 = {0x10009E3A};
	const TUid KExample10ImplementationUid3 = {0x10009E3B};

	const TUid KExample10InterfaceUid1 = {0x10009E44};
	const TUid KExample10InterfaceUid2 = {0x10009E45};
	const TUid KExample10InterfaceUid3 = {0x10009E46};

	const TUid KImpUidPLUGIN1 = {0x10009DC3};

	RArray<TUid> ifInfo;
	CleanupClosePushL(ifInfo);

	REComSession::ListExtendedInterfacesL(KExample10ImplementationUid,ifInfo);
	test(ifInfo.Count() == 3);

	// Checks that expected interfaces are in list.
	test(ifInfo[0] == KExample10InterfaceUid1);
	test(ifInfo[1] == KExample10InterfaceUid2);
	test(ifInfo[2] == KExample10InterfaceUid3);
	ifInfo.Reset();

	// Creates an implementation
	// Set up some empty initialisation parameters
	test.Printf(_L("Loading Plugin..."));

	CExampleInterface* dllPtr = CExampleInterface::NewL2(KExample10ImplementationUid);
	CleanupStack::PushL(dllPtr);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	// Calls extended interface here.
	MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid1));
	ext1->DoMethodExtended();

	MExampleInterfaceExtended2* ext2 = static_cast<MExampleInterfaceExtended2*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid2));
	ext2->DoMethodExtended2();

	// Checks that ListExtendedInterfaces works after a Create Implementation
	REComSession::ListExtendedInterfacesL(KExample10ImplementationUid,ifInfo);
	test(ifInfo.Count() == 3);

	// Checks that expected interfaces are in list.
	test(ifInfo[0] == KExample10InterfaceUid1);
	test(ifInfo[1] == KExample10InterfaceUid2);
	test(ifInfo[2] == KExample10InterfaceUid3);
	ifInfo.Reset();

	test.Printf(_L("Destroying Plugin..."));
	CleanupStack::Pop(dllPtr);

	delete dllPtr;

	// Test fetch of extended interfaces for an implementation that has no extended interfaces
	REComSession::ListExtendedInterfacesL(KExample10ImplementationUid2,ifInfo);
	test(ifInfo.Count() == 0);
	ifInfo.Reset();

	// Test fetch of extended interfaces for an implementation that does not exist
	TRAPD(err,REComSession::ListExtendedInterfacesL(KNullUid,ifInfo));
	TEST_OOM_ERR;
	test(err == KErrNotFound);
	ifInfo.Reset();

	// Test fetch of extended interfaces for an implementation that has one extended interface
	REComSession::ListExtendedInterfacesL(KExample10ImplementationUid3,ifInfo);
	test(ifInfo.Count() == 1);
	ifInfo.Reset();

	// Test fetch of extended interfaces for an implementation with PLUGIN1 IIC that has no extended interfaces
	REComSession::ListExtendedInterfacesL(KImpUidPLUGIN1,ifInfo);
	test(ifInfo.Count() == 0);
	ifInfo.Reset();

	// Close array, finished with it.
	CleanupStack::PopAndDestroy(&ifInfo);
	// Test Ends...
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3596
@SYMTestCaseDesc	    Try to load a PLUGIN3 DLL with a version one resource file. Also try to load a PLUGIN1 DLL
						with a version three resource file. Tests UID mismatch specified in the DLL .mmp file and
						the resource type in the resource file.
@SYMTestPriority 	    High
@SYMTestActions  	    Create an implementation using the invalid plugin.
@SYMTestExpectedResults The plugin should not have been loaded due to a UID1 mismatch.
@SYMEC                  EC43
*/
LOCAL_C void CreateImplementationTest1()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3596 "));
	TUid instanceKey;

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	test.Printf(_L("Loading Plugin..."));

	// Interface with NULL get and release function pointers
	const TUid KExample12InvalidDllImplementationUid = {0x10009E39};

	TRAPD(err, TAny* dllPtr = REComSession::CreateImplementationL(KExample12InvalidDllImplementationUid,instanceKey,&initParams));

	// The plugin should not have been loaded due to a UID1 mismatch. This is specified as the resource
	// type in the resource file and UID1 in the DLL itself.
	test(err==KErrNotFound);

	// Load a PLUGIN1 DLL with a version three resource file
	const TUid KExample14InvalidDllImplementationUid = {0x10009E48};

	TRAP(err, TAny* dllPtr = REComSession::CreateImplementationL(KExample14InvalidDllImplementationUid,instanceKey,&initParams));

	// The plugin should not have been loaded due to a UID1 mismatch. This is specified as the resource
	// type in the resource file and UID1 in the DLL itself.
	test(err==KErrNotFound);
	// Test Ends...
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3585
@SYMTestCaseDesc	    Get and release extended interfaces in cases in which both get and release function pointers
						do not exist. The tests will be:
						1)	Get/release function pointers are NULL
						2)	Only get function, no release function
						3)	Test with a PLUGIN1 DLL (get/release function pointers should be NULL)
@SYMTestPriority 	    High
@SYMTestActions  	    Create an implementation.
						Try getting and releasing interfaces even though no get and release function pointers exist
						Test with get extended interface function pointer and no release function pointer
@SYMTestExpectedResults NULL should be returned if get and release function pointers do not exist
						Releasing an interface that has no release function pointer should finish with
						no errors.
@SYMEC                  EC43
*/
LOCAL_C void ReleaseExtendedInterfaceTest1L()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3585 "));
	test.Printf(_L("Loading Plugin..."));
	// Interface with NULL get and release function pointers
	const TUid KExample10ImplementationUid = {0x10009E3A};

	CExampleInterface* dllPtr = CExampleInterface::NewL2(KExample10ImplementationUid);
	CleanupStack::PushL(dllPtr);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	// Call extended interface here.
	const TUid KExample10InterfaceUid1 = {0x10009E44};
	const TUid KExample10InterfaceUid2 = {0x10009E45};

	// Try getting and releasing interfaces even though no get and release function pointers exist
	MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid1));
	test(ext1 == NULL);

	MExampleInterfaceExtended2* ext2 = static_cast<MExampleInterfaceExtended2*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid2));
	test(ext2 == NULL);

	dllPtr->ManuallyReleaseExtendedInterfaceL(KExample10InterfaceUid2);

	test.Printf(_L("Destroying Plugin 1..."));
	CleanupStack::Pop(dllPtr);
	delete dllPtr;

	// Test with get extended interface function pointer and no release function pointer
	const TUid KExample10ImplementationUidNoRelease = {0x10009E3B};

	dllPtr = CExampleInterface::NewL2(KExample10ImplementationUidNoRelease);
	CleanupStack::PushL(dllPtr);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUidNoRelease);
	test(KExample10ImplementationUidNoRelease == dllPtr->ImplId());

	// Try releasing interfaces even though no release function pointer exists
	MExampleInterfaceExtended* ext = static_cast<MExampleInterfaceExtended*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid1));
	ext->DoMethodExtended();

	// This should do nothing
	dllPtr->ManuallyReleaseExtendedInterfaceL(KExample10InterfaceUid1);

	test.Printf(_L("Destroying Plugin 2..."));
	CleanupStack::Pop(dllPtr);
	delete dllPtr;

	// PLUGIN1 test. Try getting and releasing interfaces even though no get and release function pointers exist.
	dllPtr = CExampleInterface::NewL2(KCExampleImplUid);
	CleanupStack::PushL(dllPtr);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid == dllPtr->ImplId());

	// Call extended interface here.
	// Try getting and releasing interfaces even though no get and release function pointers exist
	MExampleInterfaceExtended* ext3 = static_cast<MExampleInterfaceExtended*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid1));
	test(ext3 == NULL);

	// Nothing should happen when releasing an extended interface for a PLUGIN1 DLL
	dllPtr->ManuallyReleaseExtendedInterfaceL(KExample10InterfaceUid1);

	test.Printf(_L("Destroying Plugin 3..."));
	CleanupStack::Pop(dllPtr);
	delete dllPtr;
	// Test Ends...
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-4002
@SYMTestCaseDesc		Tests ManuallyReleaseExtendedInterfaceL with invalid instance keys
@SYMTestPriority		High
@SYMTestActions			Manually release extended interface using unknown instance key
						Manually release extended interface using null instance key
@SYMTestExpectedResults	Ensure that correct leaves occur.
@SYMDEF					DEF111553
*/
LOCAL_C void ReleaseExtendedInterfaceTest2L()
	{
	// Test Starts...
	// get valid instance key by creating implementation
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-4002 "));
	const TUid KExample10ImplementationUid = {0x10009E3A};
	TUid instanceKey;
	TAny* impl = REComSession::CreateImplementationL(KExample10ImplementationUid, instanceKey, 0);

	// remove implementation
	REComSession::DestroyedImplementation(instanceKey);
	delete impl;

	// Attempt to manually release extended interface with instance key that is no longer valid but
	// passes the ASSERT test
	const TUid KExample10InterfaceUid1 = {0x10009E44};
	TRAPD(err, REComSession::ManuallyReleaseExtendedInterfaceL(instanceKey,KExample10InterfaceUid1));
	test(err = KErrNotFound);

	// Attempt to manually release extended interface with null instance key
	TRAP(err, REComSession::GetExtendedInterfaceL(KNullUid,KExample10InterfaceUid1));
	test(err = KErrArgument);
	// Test Ends...
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3584
@SYMTestCaseDesc	    Tests for GetExtendedInterfaceL, fetching an invalid extended interface
@SYMTestPriority 	    High
@SYMTestActions  	    Create an implementation. Fetch two extended interfaces (one build in, and one
						stand alone and requires release). Fetch an invalid extended interface. Destroy implementation.
@SYMTestExpectedResults Ensure that the GetExtendedInterfaceL call with the invalid interface returns NULL.
@SYMEC                  EC43
*/
LOCAL_C void GetExtendedInterfaceTest3L()
	{
	//
	// Testing a overloaded method for CreateImplementationL
	// ------------------------------------------------------------------
	//
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3584 "));
	test.Printf(_L("Loading Plugin..."));
	const TUid KExample10ImplementationUid = {0x10009E38};

	CExampleInterface* dllPtr = CExampleInterface::NewL2(KExample10ImplementationUid);
	CleanupStack::PushL(dllPtr);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	// Call extended interface here.
	const TUid KExample10InterfaceUid1 = {0x10009E44};
	const TUid KExample10InterfaceUid2 = {0x10009E45};
	const TUid KExample10InvalidInterfaceUid = {0x10009E49};

    MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid1));
	ext1->DoMethodExtended();

	MExampleInterfaceExtended2* ext2 = static_cast<MExampleInterfaceExtended2*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid2));
	ext2->DoMethodExtended2();

	// Get an invalid extended interface.
	MExampleInterfaceExtended2* ext2invalid = static_cast<MExampleInterfaceExtended2*>(dllPtr->GetExtendedInterfaceL(KExample10InvalidInterfaceUid));
	// No interface should be returned if it is not found.
	test(ext2invalid == NULL);

	test.Printf(_L("Destroying Plugin..."));
	CleanupStack::Pop(dllPtr);
	delete dllPtr;

	// Test Ends...
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3583
@SYMTestCaseDesc	    Tests for GetExtendedInterfaceL, with an invalid instance key.
@SYMTestPriority 	    High
@SYMTestActions  	    Call REComSession::GetExtendedInterfaceL() with an invalid instance key.
						GetExtendedInterfacePanic() and ThreadPanicTest() are used for this test case.
@SYMTestExpectedResults Ensure that the GetExtendedInterfaceL panics in debug mode and leaves in release
						mode.
@SYMEC                  EC43
*/
LOCAL_C void GetExtendedInterfaceTest2L()
	{
	// Test Starts...
	TUid instanceKey;
	TBool containInvalidExtIf = EFalse;
	TBool useDefResolver = EFalse;
	TBool useInitParam = ETrue;

	CExampleInterface* dllPtr = CreateImplementationTestL(containInvalidExtIf,useDefResolver,useInitParam,instanceKey);
	CleanupStack::PushL(dllPtr);

	const TUid KExtendedInterfaceUid = {0x10009E44};
	TUid invalidInstanceKey;
	invalidInstanceKey.iUid = instanceKey.iUid + 1;

	// Fetches an extended interface with invalid instance key.
	// When passed an invalid instance key, REComSession::GetExtendedInterfaceL() panics
	// with KErrArgument in debug mode, but leaves with KErrNotFound in release mode.
	MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(REComSession::GetExtendedInterfaceL(invalidInstanceKey,KExtendedInterfaceUid));

	REComSession::DestroyedImplementation(instanceKey);
	CleanupStack::PopAndDestroy(dllPtr);

	// Test Ends...
	}

/**
A thread function.
Panics in debug mode.
Returns an error code in release mode.
*/
LOCAL_C TInt GetExtendedInterfacePanic(TAny*)
	{
	__UHEAP_MARK;
	CTrapCleanup* threadcleanup = CTrapCleanup::New();

	// In debug mode, GetExtendedInterfaceTest2L panics with reason KErrArgument.
	// In release mode, it leaves with KErrNotFound.
	TRAPD(err, GetExtendedInterfaceTest2L());

	delete threadcleanup;
	__UHEAP_MARKEND;

	return err;
	}

/**
The test code is used for capturing the PANIC that occurs as a result of calling GetExtendedInterfaceL
with an invalid instance key.
*/
LOCAL_C void ThreadPanicTest(const TDesC& aName,TThreadFunction aFunction)
	{
	test.Next(aName);
	TRequestStatus threadStatus;
	RThread thread;
	TBool jit;
	jit=User::JustInTime();
	User::SetJustInTime(EFalse);

	TInt err=thread.Create(aName,aFunction,KDefaultStackSize*8,KMinHeapSize,0x100000,0);
	test(err==KErrNone);
	thread.Logon(threadStatus)	;
	thread.Resume();

	User::WaitForRequest(threadStatus);

	// Different verifications for debug and release mode.
#ifdef _DEBUG
	test(thread.ExitType()==EExitPanic);
	test(thread.ExitReason()==EEComPanic_InvalidImplementationInstanceKey);
#else
	test(thread.ExitType()==EExitKill);
	test(thread.ExitReason()==KErrNotFound);
#endif

	thread.Close();
	User::SetJustInTime(jit);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3582
@SYMTestCaseDesc	    Tests for GetExtendedInterfaceL, stand alone and built in extended interfaces with
						valid instance keys.
@SYMTestPriority 	    High
@SYMTestActions  	    Create an implementation. Fetch two extended interfaces (one build in, and one
						stand alone and requires release). Call extended interfaces. Destroy implementation.
						Re-create the same implementation again. Fetch extended interface. Destroy implementation.
						Re-create again. Fetch two extended interfaces (both being stand alone and
						require release).
@SYMTestExpectedResults Ensure that Extended interfaces can be created, called and cleaned up when
						implementation is destroyed.
@SYMEC                  EC43
*/
LOCAL_C void GetExtendedInterfaceTest1L()
	{
	// Test Starts...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3582 "));
	test.Printf(_L("Loading Plugin..."));
	const TUid KExample10ImplementationUid = {0x10009E38};

	CExampleInterface* dllPtr = CExampleInterface::NewL2(KExample10ImplementationUid);
	CleanupStack::PushL(dllPtr);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	// Calls extended interface here.
	const TUid KExample10InterfaceUid1 = {0x10009E44};
	const TUid KExample10InterfaceUid2 = {0x10009E45};

	// Fetches an extended interface.
	MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid1));
	// Sanity check by calling extended interface.
	ext1->DoMethodExtended();

	// Fetches an extended interface. Stand alone interface.
	MExampleInterfaceExtended2* ext2 = static_cast<MExampleInterfaceExtended2*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid2));
	// Sanity check by calling extended interface.
	ext2->DoMethodExtended2();

	// Delete implementation, this will automatically destroy the stand alone interface.
	test.Printf(_L("Destroying Plugin..."));
	CleanupStack::Pop(dllPtr);
	delete dllPtr;

	// Test to ensure that a new interface can be re-created immediately after deletion. This
	// also tests that no problems occur when no extended interfaces require releasing yet
	// a release function exists in the plug-in implementation.
	dllPtr = CExampleInterface::NewL2(KExample10ImplementationUid);
	CleanupStack::PushL(dllPtr);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	// Ensure that the extended interface can also be fetched again
	MExampleInterfaceExtended* ext = static_cast<MExampleInterfaceExtended*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid1));
	ext->DoMethodExtended();

	test.Printf(_L("Destroying Plugin..."));
	CleanupStack::Pop(dllPtr);
	delete dllPtr;

	// Re-creates implementation. This time create two extended interfaces that require release
	dllPtr = CExampleInterface::NewL2(KExample10ImplementationUid);
	CleanupStack::PushL(dllPtr);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KExample10ImplementationUid);
	test(KExample10ImplementationUid == dllPtr->ImplId());

	MExampleInterfaceExtended2* extension1 = static_cast<MExampleInterfaceExtended2*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid2));
	extension1->DoMethodExtended2();

	const TUid KExample10InterfaceUid3 = {0x10009E46};

	MExampleInterfaceExtended2* extension2 = static_cast<MExampleInterfaceExtended2*>(dllPtr->GetExtendedInterfaceL(KExample10InterfaceUid3));
	extension2->DoMethodExtended2();

	// Destroy the implementation. This should automatically delete both extended interfaces above
	test.Printf(_L("Destroying Plugin..."));
	CleanupStack::Pop(dllPtr);
	delete dllPtr;
	// Test Ends...
	}


typedef void (*ClassFuncPtrL) (void);

/**
Wrapper function to call all test functions
@param		testFuncL pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTest(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	__UHEAP_MARK;
	// find out the number of open handles
	TInt pHCountStart;
	TInt tHCountStart;
	RThread().HandleCount(pHCountStart,tHCountStart);

	TRAPD(err, (*testFuncL)());
	test(KErrNone == err);

	// release handles
	REComSession::FinalClose();

	// check that no handles have leaked
	TInt pHCountEnd;
	TInt tHCountEnd;
	RThread().HandleCount(pHCountEnd,tHCountEnd);

	test(pHCountStart == pHCountEnd);
	test(tHCountStart == tHCountEnd);
	test.Printf(_L("Great ! No handle mismatch."));

	__UHEAP_MARKEND;
	}

/**
Wrapper function to call all OOM test functions
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMTest(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);
	TInt err;
	TInt failAt = 1;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);

		TRAP(err, (*testFuncL)());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// release handles
		REComSession::FinalClose();

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		test.Printf(_L("Great ! No handle mismatch."));

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	test(err == KErrNone);
	test.Printf(_L("- server succeeded at heap failure rate of %i\n"), failAt);
	}


void DoTests()
	{
	//Basic test...
	test.Printf(_L("Basic tests...\n"));

	DoBasicTest(&GetExtendedInterfaceTest1L,_L("GetExtendedInterfaceTest1L"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3583 "));
	ThreadPanicTest(_L("GetExtendedInterface Panic Testing"),GetExtendedInterfacePanic);
	DoBasicTest(&GetExtendedInterfaceTest3L,_L("GetExtendedInterfaceTest3L"));
	DoBasicTest(&GetExtendedInterfaceTest4L,_L("GetExtendedInterfaceTest4L"));
	DoBasicTest(&GetExtendedInterfaceTest5L,_L("GetExtendedInterfaceTest5L"));
	DoBasicTest(&ReleaseExtendedInterfaceTest1L,_L("ReleaseExtendedInterfaceTest1L"));
	DoBasicTest(&ReleaseExtendedInterfaceTest2L,_L("ReleaseExtendedInterfaceTest2L"));
	DoBasicTest(&ListExtendedInterfaceTestL,_L("ListExtendedInterfaceTestL"));

	DoBasicTest(&CreateImplementationTest1,_L("CreateImplementationTest1"));
	DoBasicTest(&CreateImplementationTest2L,_L("CreateImplementationTest2L"));
	DoBasicTest(&CreateImplementationTest3L,_L("CreateImplementationTest3L"));
	DoBasicTest(&CreateImplementationTest4L,_L("CreateImplementationTest4L"));
	DoBasicTest(&CreateImplementationTest5L,_L("CreateImplementationTest5L"));
	DoBasicTest(&CreateImplementationTest6,_L("CreateImplementationTest6"));
	DoBasicTest(&CreateImplementationTest7L,_L("CreateImplementationTest7L"));
	DoBasicTest(&DestroyedImplementationTestL,_L("DestroyedImplementationTestL"));

	DoBasicTest(&ListImplementationTest1L,_L("ListImplementationTest1L"));
	DoBasicTest(&ListImplementationTest2L,_L("ListImplementationTest2L"));
	DoBasicTest(&ListImplementationTest3L,_L("ListImplementationTest3L"));
	DoBasicTest(&ListImplementationTest4L,_L("ListImplementationTest4L"));
	DoBasicTest(&ListImplementationTest5L,_L("ListImplementationTest5L"));

	DoBasicTest(&DummyUidTestL,_L("CreateImplementation and ListImplementation Dummy UID test"));

	// OOM test...
	test.Printf(_L("OOM tests...\n"));

	DoOOMTest(&GetExtendedInterfaceTest1L,_L("OOM GetExtendedInterfaceTest1L"));
	// GetExtendedInterfaceTest2L is a special test case, cannot do OOM testing.
	DoOOMTest(&GetExtendedInterfaceTest3L,_L("OOM GetExtendedInterfaceTest3L"));
	DoOOMTest(&GetExtendedInterfaceTest4L,_L("OOM GetExtendedInterfaceTest4L"));
	DoOOMTest(&GetExtendedInterfaceTest5L,_L("OOM GetExtendedInterfaceTest5L"));
	DoOOMTest(&ReleaseExtendedInterfaceTest1L,_L("OOM ReleaseExtendedInterfaceTest1L"));
	DoOOMTest(&ReleaseExtendedInterfaceTest2L,_L("OOM ReleaseExtendedInterfaceTest2L"));
	DoOOMTest(&ListExtendedInterfaceTestL,_L("OOM ListExtendedInterfaceTestL"));

	// CreateImplementationTest1 is a special test case, cannot do OOM testing.
	DoOOMTest(&CreateImplementationTest2L,_L("OOM CreateImplementationTest2L"));
	DoOOMTest(&CreateImplementationTest3L,_L("OOM CreateImplementationTest3L"));
	DoOOMTest(&CreateImplementationTest4L,_L("OOM CreateImplementationTest4L"));
	DoOOMTest(&CreateImplementationTest5L,_L("OOM CreateImplementationTest5L"));
	// CreateImplementationTest6 is a special test case, cannot do OOM testing.
	DoOOMTest(&CreateImplementationTest7L,_L("OOM CreateImplementationTest7L"));
	DoOOMTest(&DestroyedImplementationTestL,_L("OOM DestroyedImplementationTestL"));

	DoOOMTest(&ListImplementationTest1L,_L("OOM ListImplementationTest1L"));
	DoOOMTest(&ListImplementationTest2L,_L("OOM ListImplementationTest2L"));
	DoOOMTest(&ListImplementationTest3L,_L("OOM ListImplementationTest3L"));
	DoOOMTest(&ListImplementationTest4L,_L("OOM ListImplementationTest4L"));
	DoOOMTest(&ListImplementationTest5L,_L("OOM ListImplementationTest5L"));
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();

	test.Start(_L("ExtendedInterfaces tests."));

	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();

	//The reason for the folowing delay is:
	//ECOM server could be already started. It means that when we copy some
	//ECOM plugins from Z: to C: drive - ECOM server should look for and
	//find the new ECOM plugins. The ECOM server uses for that CDiscoverer::CIdleScanningTimer
	//which is an active object. So the discovering service is asynchronous. We have to
	//wait some time until it finishes. Otherwise ListImplementationsL could fail to find
	//requested implementations.
	User::After(KOneSecond * 5);

	DoTests();

	delete cleanup;

	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(0);
	}
