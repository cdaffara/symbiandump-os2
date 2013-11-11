// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains test classes and their implementations
// to test production class CLoadManager. Where necessary stubs
// are implemented to help in writing test harness using RTest.
// Plugins available to be used in this test
// Interface UID	DLL	UID		Implementation UID	Version		DllFile
// 0x10009DC0		0x10009DB1	0x10009DC3			1		Z:\\..\\EComExample.dll
// 0x10009DB1	0x10009DC4			1		Z:\\..\\EComExample.dll
// 0x10009DB3	0x10009DC3			2		Z:\\..\\EComExample2.dll
// 0x10009DB3	0x10009DC4			2		Z:\\..\\EComExample2.dll
// 0x101F8477	0x101F8478			1		Z:\\..\\EComExample3.dll
// 0x101F8479  0x101F847A			1		Z:\\..\\EComExample4.dll
// 0x101F847B	0x101F847C			1		C:\\..\\EComExample5.dll
// 0x10282331		0x10282330	0x10282332			1		Z:\\..\\EComLoadMgrExample1.dll
// 0x10282330	0x10282334			1		Z:\\..\\EComLoadMgrExample1.dll
// 0x10282335	0x10282333			1		Z:\\..\\EComLoadMgrExample2.dll
// 
//

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <e32uid.h>
#include "LoadManager.h"
#include <ecom/ecomerrorcodes.h>
#include "EComUidCodes.h"
#include "RegistryData.h"
#include <ecom/implementationinformation.h>
#include "../EcomTestUtils/EcomTestUtils.h"
#include "Interface.h"
#include "UnloadPolicy.h"
// ---------------------------------------------------------------------------
// Test and Leave framework

LOCAL_D RTest TheTest(_L("t_loadmanager.exe"));

LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		//DeleteTestPlugin();
		TheTest(EFalse, aLine);
		}
	}

LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		//DeleteTestPlugin();
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::Check((arg), __LINE__)

#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//Leave with info message printed out
LOCAL_C void Leave(TInt aError, TInt aLine)
	{
	RDebug::Print(_L("*** Leave. Error: %d, Line: %d\r\n"), aError, aLine);
	User::Leave(aError);
	}


//Use LEAVE() macro instead of User::Leave() and LEAVE_IF_ERROR() macro instead of
//User::LeaveIfError(). They will print the line number, where the "leave" was called.

#define LEAVE(aError) ::Leave(aError, __LINE__)


// ---------------------------------------------------------------------------

LOCAL_D RFs					TheFs;

LOCAL_D CTrapCleanup* 		TheTrapCleanup 		= NULL;

LOCAL_D CActiveScheduler*	TheActiveScheduler	= NULL;

//Plugins files used for this test program
_LIT(KResourceFileName5, "C:\\resource\\plugins\\EComExample5.RSC");
_LIT(KExampleDllFileName5, "C:\\sys\\bin\\EComExample5.dll");
_LIT(KExampleDllFileName4,"Z:\\sys\\bin\\EComExample4.dll");
_LIT(KExampleDllFileName3,"Z:\\sys\\bin\\EComExample3.dll");
_LIT(KExampleDllFileName2,"Z:\\sys\\bin\\EComExample2.dll");
_LIT(KLoadMgrExample1DllName,"Z:\\sys\\bin\\EComLoadMgrExample1.dll");
_LIT(KLoadMgrExample2DllName,"Z:\\sys\\bin\\EComLoadMgrExample2.dll");

_LIT(KResourceFileName5OnZ, "z:\\RAMOnly\\EComExample5.RSC");
_LIT(KExampleDllFileName10,"Z:\\sys\\bin\\EComExample10.dll");
_LIT(KNULLString,"");

_LIT(KExampleDllFileName12,"C:\\sys\\bin\\EComExample12.dll");
_LIT(KExampleDllFileName12OnZ, "z:\\RAMOnly\\EComExample12.dll");
_LIT(KResourceFileName12, "C:\\resource\\plugins\\EComExample12.RSC");
_LIT(KResourceFileName12OnZ, "z:\\RAMOnly\\EComExample12.RSC");

//Contains files that are part of ROM that must be
//Copied to the RAM for test purposes

_LIT(KExampleDllFileName5OnZ, "z:\\RAMOnly\\EComExample5.dll");


// Used for supressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

const TInt KOneSecond = 1000000;

// Utility clean up function
void CleanupEComArray(TAny* aArray);

//It is used by some test methods which are called two times:
//from normal test and from OOM TheTest.
static void LeaveIfErrNoMemory(TInt aError)
	{
	if(aError == KErrNoMemory)
		{
		LEAVE(aError);
		}
	}

/**
Test State Accessor class for CLoadManager
This class will be used to access the state
of the policy array within CLoadManager
*/
class TLoadManager_StateAccessor
	{
public:
	//Return the number of instance index inside iInstanceInfoList
	TInt InstanceIndexCount(CLoadManager& aLoadManager);
	//Return the number of unload policy inside iAllUnloadPolicies
	TInt UnloadPolicyCount(CLoadManager& aLoadManager);
	//Return a pointer to Instance Index item inside the instance index array
	CInstanceInfo* InstanceIndex(const TInt& aInt,CLoadManager& aLoadManager);
	//Return a pointer to UnloadPolicy item inside the array
	CUnloadPolicy* UnloadPolicy(const TInt& aInt,CLoadManager& aLoadManager);
	//Return a pointer to garbage UnloadPolicy
	CUnloadPolicy* GarbagePolicy(CLoadManager& aLoadManager);
	//Return a count to the number of extended objects within an Extended instance.
	// Must pass in the index of a valid extended instance
	TInt ExtendedInstanceCount(const TInt& aInt,CLoadManager& aLoadManager);
	};

TInt TLoadManager_StateAccessor::InstanceIndexCount(CLoadManager& aLoadManager)
	{
	return aLoadManager.iInstanceInfoList.Count();
	}

TInt TLoadManager_StateAccessor::UnloadPolicyCount(CLoadManager& aLoadManager)
	{
	return aLoadManager.iAllUnloadPolicies.Count();
	}

CInstanceInfo* TLoadManager_StateAccessor::InstanceIndex(const TInt& aInt,CLoadManager& aLoadManager)
	{
	return (aLoadManager.iInstanceInfoList[aInt]);
	}

CUnloadPolicy* TLoadManager_StateAccessor::UnloadPolicy(const TInt& aInt,CLoadManager& aLoadManager)
	{
	return aLoadManager.iAllUnloadPolicies[aInt];
	}
CUnloadPolicy* TLoadManager_StateAccessor::GarbagePolicy(CLoadManager& aLoadManager)
	{
	return aLoadManager.iGarbagePolicy;
	}

TInt TLoadManager_StateAccessor::ExtendedInstanceCount(const TInt& aInt,CLoadManager& aLoadManager)
	{
	CInstanceInfoExtended* instanceExtended = static_cast<CInstanceInfoExtended*>(aLoadManager.iInstanceInfoList[aInt]);
	return instanceExtended->iExtendedObjectInfo.Count();
	}


/**
Test class for object CLoadManager.
The class provides the parameters and behaviour that
allows this class to behave normally under a test scenario.
*/
class CLoadManagerTest : public CBase
	{
public:
	static CLoadManagerTest* NewL();
	virtual inline ~CLoadManagerTest();

	void InstantiationMethodTestL(TUid aDllUid, const TDesC& aDllName, const TUid aIICType);
	void DestroyThisTestL(TUid aDllUid, const TDesC& aDllName, const TUid aIICType);

	void CreateDestroyTestCase1L(TUid aDllUid1,
	  	            			  TUid aImplUid1,
								  const TDesC& aDllName1,
								  TUid aDllUid2,
								  TUid aImplUid2,
								  const TDesC& aDllName2,
								  const TUid aIICType);
	void CreateDestroyTestCase2L(TUid aDllUid,
								   TUid aImplUid1,
								   const TDesC& aDllName,
								   TUid aImplUid2,
								   const TUid aIICType);
	void CreateDestroyTestCase3L();

	void CreateDestroyTestCase4L(TUid aDllUid,TUid aImplUid,const TDesC& aDllName,const TUid aIICType);

	void CreateDestroyTestCase5L(TUid aDllUid1,
	  	            		  			TUid aImplUid1,
							  			const TDesC& aDllName1,
							  			TUid aDllUid2,
							  			TUid aImplUid2,
							  			const TDesC& aDllName2,
							  			TUid aImplUid3,
							  			const TUid aIICType);

	void SpecialOOMTestCaseL();

	void InstantiationMethodTestPlugin1L();
	void InstantiationMethodTestPlugin3L();
	void ImplementationObjectTestL();

	void GetImplementationUidTestL();

	void DestroyThisTestPlugin1L();
	void DestroyThisTestPlugin3L();

	void CreateDestroyTestCase1Plugin1L();
	void CreateDestroyTestCase1Plugin3L();

	void CreateDestroyTestCase2Plugin1L();
	void CreateDestroyTestCase2Plugin3L();

	void CreateDestroyTestCase3Plugin3L();

	void CreateDestroyTestCase4Plugin1L();
	void CreateDestroyTestCase4Plugin3L();

	void CreateDestroyTestCase5Plugin1L();
	void CreateDestroyTestCase5Plugin3L();

	// Extended methods
	void ExtendedMethodTest1Plugin3L();
	void ExtendedMethodTest2Plugin3L();
	void ManuallyReleaseExtendedInterfaceTestL();
	void CreateDestroyTestCase1PluginAndPlugin3L();

private:
	CLoadManagerTest();
	void ConstructL();

public:
	/** The instance of the class under test */
	CLoadManager* iLoadManager;

	/** Unique Id of an interface implementation */
	TUid	iImplementationUid;

	/** LoadManager State Accessor */
	TLoadManager_StateAccessor iStateAccessor;
	};

/**
Standardised safe construction which
leaves nothing on the cleanup stack.

@post		CLoadManagerTest is fully constructed and initialised.
*/
CLoadManagerTest* CLoadManagerTest::NewL()
	{
	CLoadManagerTest* self = new(ELeave) CLoadManagerTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Standardized default constructor

@post		CLoadManagerTest is fully constructed.
*/
CLoadManagerTest::CLoadManagerTest()
	{
	// Implementation id for interface 0x10009DC0
	// Implemented in test plugin EComExample4.dll
	iImplementationUid.iUid		= 0x101F847A;
	}

/**
Destructor.

@post		This object is properly destroyed.
*/
CLoadManagerTest::~CLoadManagerTest()
	{
	delete iLoadManager;
	}

/**
Standardized 2nd(Initialization) phase of two phase construction.

@post		CLoadManagerTest is fully constructed.
*/
void CLoadManagerTest::ConstructL()
	{
	iLoadManager  =	CLoadManager::NewL();
	}

/**
The test executes by getting the Instantiation method for
the given ImplementationId
*/
void CLoadManagerTest::InstantiationMethodTestL(TUid aDllUid, const TDesC& aDllName, const TUid aIICType)
	{
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer		= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;

	TEntry entry;
	entry.iName=aDllName;
	entry.iModified=TTime(0);
	entry.iType=TUidType(KDynamicLibraryUid,aIICType,aDllUid);

	TAny* implementationPtr = 0;
	TUid instanceKey;
	TRAPD(err, implementationPtr = iLoadManager->ImplementationObjectL(iImplementationUid, entry,&iInitParams, initParamsFlag,instanceKey));

	::LeaveIfErrNoMemory(err);
	TEST2(err,KErrNone);
	TEST(implementationPtr != 0);

	//Checking the state of the arrays inside CLoadManager and garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager)==1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager)==1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)==NULL);

	//Checking the instance index and UnloadPolicy info
	TEST(iStateAccessor.InstanceIndex(0,*iLoadManager)->ImplementationUid()==iImplementationUid);
	TEST(iStateAccessor.UnloadPolicy(0,*iLoadManager)->DllEntryInformation().GetName().CompareF(aDllName)==0);

	delete (CExampleInterface*)implementationPtr;
	implementationPtr=0;
	iLoadManager->DestroyedThis(instanceKey);
	iLoadManager->ClearGarbage();
	}

/**
Test CLoadManager::ImplementationObjectL with parameters to test old proxy table
*/
void CLoadManagerTest::InstantiationMethodTestPlugin1L()
	{
	// This test tests with Ecom plugin EComExample4.dll
	// EComExample4.dll should be installed on Z: drive
	// The test executes by getting the Instantiation method for the given ImplementationId
	// implementationPtr method should get some valid address.
	// Manually creating the DLL info that in practice will be returned from the server side
	// Set up some init params to be used for testing purpose
	TUid dllUid;
	dllUid.iUid = 0x101F8479;
	iImplementationUid.iUid=0x101F847A;
	InstantiationMethodTestL(dllUid,KExampleDllFileName4,KUidInterfaceImplementationCollection);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3549
@SYMTestCaseDesc	    Calls CLoadManager::ImplementationObjectL with parameters to test new proxy table
@SYMTestPriority 	    High
@SYMTestActions  	    Instantiate implementation for the given ImplementationId with construction parameters
						and check the CLoadManager data structures.
						Destroy the implementation object and again check the CLoadManager data structures.
						(ECom plugin EComExample10.dll will be used for the test.)
@SYMTestExpectedResults Ensure the CLoadManager data structures are as expected after the ImplementationObjectL calls
@SYMEC                  EC43
*/
void CLoadManagerTest::InstantiationMethodTestPlugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3549 "));
	TUid dllUid;
	dllUid.iUid = 0x10009E37;
	iImplementationUid.iUid = 0x10009E38;
	InstantiationMethodTestL(dllUid,KExampleDllFileName10,KUidInterfaceImplementationCollection3);
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3710
@SYMTestCaseDesc	    Tests CLoadManager::ImplementationObjectL with some extreme conditions to improve the
						condition coverage rate.
						1. implementationUid is NULL. The function will leave with KErrNotFound
						2. dllName is NULL. The function will leave with KErrNotFound
						3. second Uid of the PlugIn is neither IIC or IIC3. The function will leave with KErrNotSupported
@SYMTestPriority 	    High
@SYMTestActions  	    Calls CLoadManager::ImplementationObjectL and check the result.
@SYMTestExpectedResults Expected error is return.
@SYMDEF                 DEF111196
*/
void CLoadManagerTest::ImplementationObjectTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3710 "));
	TUid dummyUid;
	dummyUid.iUid = 0;

	TUid dllUid;
	dllUid.iUid = 0x10009E37;

	iImplementationUid.iUid = 0x10009E38;

	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer		= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;

	TEntry entry;
	entry.iName=KExampleDllFileName10;
	entry.iModified=TTime(0);
	entry.iType=TUidType(KDynamicLibraryUid,KUidInterfaceImplementationCollection3,dllUid);

	TUid instanceKey;

	// 1. Calls ImplementationObjectL with implementationUid is NULL. The function will leave with KErrNotFound
	TRAPD(err, TAny* implementationPtr = iLoadManager->ImplementationObjectL(dummyUid, entry,&iInitParams, initParamsFlag,instanceKey));
	TEST2(err,KErrNotFound);

	// 2. Calls ImplementationObjectL with dllName is NULL. The function will leave with KErrNotFound
	entry.iName = KNULLString;
	TRAP(err, TAny* implementationPtr = iLoadManager->ImplementationObjectL(iImplementationUid,entry,&iInitParams, initParamsFlag,instanceKey));
	TEST2(err,KErrNotFound);

	// 3. Calls ImplementationObjectL with second Uid of the PlugIn is neither IIC or IIC3
	entry.iName=KExampleDllFileName10;
	entry.iModified=TTime(0);
	entry.iType=TUidType(KDynamicLibraryUid,dummyUid,dllUid);
	TRAP(err, TAny* implementationPtr = iLoadManager->ImplementationObjectL(iImplementationUid,entry,&iInitParams, initParamsFlag,instanceKey));
	TEST2(err,KErrNotSupported);
	}


/**
Test CLoadManager::DestroyThis(). DestroyThis will actually either remove the UnloadPolicy from the list or decrease
referencecount(to UnloadPolicy) by one.
*/
void CLoadManagerTest::DestroyThisTestL(TUid aDllUid, const TDesC& aDllName, const TUid aIICType)
	{
	// We must call DestroyedThis only after getting a reference to the Implementation
	// so that reference count to this implementaion have value greater than zero.
	// ImplementationObjectL is called to increase the reference count of the test implementation,
	// so that its reference count goes to 1.
	// This test executes by verifing the reference count to this implementation.
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer		= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;

	TEntry entry;
	entry.iName=aDllName;
	entry.iModified=TTime(0);
	entry.iType=TUidType(KDynamicLibraryUid, aIICType, aDllUid);

	TAny* implementationPtr = 0;
	TUid instanceKey;
	TRAPD(err, implementationPtr = iLoadManager->ImplementationObjectL(iImplementationUid, entry, &iInitParams, initParamsFlag,instanceKey));

	::LeaveIfErrNoMemory(err);
	TEST2(err,KErrNone);
	TEST(implementationPtr != 0);

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager)==1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager)==1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)==NULL);

	// The purpose of DestroyedThis is to decrease the reference count of UnloadPolicy or
	// remove the Dll from RegistryData if its on grabagelist and reference count reaches 0
	iLoadManager->DestroyedThis(instanceKey);

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager)==0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager)==0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)!=NULL);

	//delete the instantiation
	delete (CExampleInterface*)implementationPtr;
	implementationPtr=0;
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-3709
@SYMTestCaseDesc		This testcase checks GetImplementationUidL().
@SYMTestPriority			High
@SYMTestActions			Instantiate an implementation.
						Call GetImplementationUidL() and check impl id.
						Call GetImplementationUidL() and check impl id again.
						Destroy plugin.
						Call GetImplementationUidL() and check for error.
@SYMTestExpectedResults	Returns impl uid when valid destructor uid is used otherwise
						leaves with the appropriate error.
@SYMDEF					DEF111789
*/
void CLoadManagerTest::GetImplementationUidTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3709 "));
	// EComExample5.dll (dlluid 0x101F847B) in C:\  with Implementation Uid =0x101F847C
	TUid dllUid;
	dllUid.iUid = 0x101F847B;
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer		= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;

	TEntry entry;
	entry.iName=KExampleDllFileName5;

	entry.iModified=TTime(0);
	entry.iType=TUidType(KDynamicLibraryUid, KUidInterfaceImplementationCollection, dllUid);

	iImplementationUid.iUid=0x101F847C;

	TAny* implementationPtr = 0;
	TUid instanceKey;
	TRAPD(err, implementationPtr = iLoadManager->ImplementationObjectL(iImplementationUid, entry, &iInitParams, initParamsFlag, instanceKey));

	::LeaveIfErrNoMemory(err);
	TEST2(err,KErrNone);
	TEST(implementationPtr != 0);

	// Retrieve impl uid corresponding to detructor key id
	TUid implId = iLoadManager->GetImplementationUidL(instanceKey);
	// Check that the impl uid corresponding to destructor key id is as expected
	TEST(implId == iImplementationUid);

	// Do it again to be sure...
	implId = iLoadManager->GetImplementationUidL(instanceKey);
	// Check that the impl uid corresponding to destructor key id is as expected
	TEST(implId == iImplementationUid);

	iLoadManager->DestroyedThis(instanceKey);

	//delete the instantiation
	delete (CExampleInterface*)implementationPtr;
	implementationPtr=0;

	// Call GetImplementationUidL() again now that the instance has been deleted
	// Expecting a Leave...
	TRAP(err,implId = iLoadManager->GetImplementationUidL(instanceKey));
	TEST2(err, KErrNotFound);
	}

/**
Test executes by getting the CUnloadPolicy* associated to the testing DLL EComExample5.dll.
*/
void CLoadManagerTest::DestroyThisTestPlugin1L()
	{
	// EComExample5.dll (dlluid 0x101F847B) in C:\  with Implementation Uid =0x101F847C
	TUid dllUid;
	dllUid.iUid = 0x101F847B;
	iImplementationUid.iUid=0x101F847C;
	DestroyThisTestL(dllUid,KExampleDllFileName5,KUidInterfaceImplementationCollection);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3550
@SYMTestCaseDesc	    Calls CLoadManager::DestroyedThis with parameters to test new proxy table
@SYMTestPriority 	    High
@SYMTestActions  	    Test executes by getting the CUnloadPolicy* associated to the testing DLL EComExample10.dll.
						DestroyThis will actually either remove the UnloadPolicy from the list or decrease
						referencecount(to UnloadPolicy) by one.
@SYMTestExpectedResults Ensure the CLoadManager data structures are as expected after the ImplementationObjectL calls
@SYMEC                  EC43
*/
void CLoadManagerTest::DestroyThisTestPlugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3550 "));
	// EComExample10.dll
	TUid dllUid;
	dllUid.iUid = 0x10009E37;
	iImplementationUid.iUid = 0x10009E38;
	DestroyThisTestL(dllUid,KExampleDllFileName10,KUidInterfaceImplementationCollection3);
	}

/**
Tests for create and destroy load manager
Test case 1:Creates two implementations from two DIFFERENT DLL Z: C:
Test case 2:Creates two implementations from one SAME DLL
Test case 3:Invalid argument testing for Create Implementation
			Invalid Entry and Implementation uid argument in ImplementationObjectL
Test case 4:Creates two instance of the same implementation Uid from one single DLL
Test case 5:DEF101930: Incorrect policy object returned in
			This testcase checks for the case where 2 implementations
			are created each on separate DLL's then an additional
			implementation is created from one of the already loaded DLL's.
			The impl uid of the newly created impl is lower then the 2
			existing impl uids. The impl uids from the same DLL are such
			that 1 is higher then the uid of the other impl and 1 is lower than
			the other impl.
*/

// Test case 1:Create two implementation from two DIFFERENT DLL Z: C:

void CLoadManagerTest::CreateDestroyTestCase1L(TUid aDllUid1,
													  TUid aImplUid1,
													  const TDesC& aDllName1,
													  TUid aDllUid2,
													  TUid aImplUid2,
													  const TDesC& aDllName2,
													  const TUid aIICType)
	{
	//Some interface instantiation parameters
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer	= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;
	TAny* impl1 = NULL;
	TAny* impl2 = NULL;

	TEntry entry1;
	entry1.iName = aDllName1;
	entry1.iModified = TTime(0);
	entry1.iType = TUidType(KDynamicLibraryUid,aIICType,aDllUid1);

	TEntry entry2;
	entry2.iName = aDllName2;
	entry2.iModified = TTime(0);
	entry2.iType = TUidType(KDynamicLibraryUid,aIICType,aDllUid2);

	TInt err;
	//Creates 1st Implementation
	TUid instanceKey;
	TRAP(err,impl1 = iLoadManager->ImplementationObjectL(aImplUid1,entry1, &iInitParams,initParamsFlag,instanceKey));
	::LeaveIfErrNoMemory(err);
	TEST2(err,KErrNone);

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Creates 2nd Implementation
	TUid instanceKey2;
	TRAP(err,impl2 = iLoadManager->ImplementationObjectL(aImplUid2,entry2, &iInitParams,initParamsFlag,instanceKey2));
	// If impl2 fails here due to KErrNoMemory we need to clear the previous impl1
	// to avoid memory leak
	if (err==KErrNoMemory)
		{
		iLoadManager->DestroyedThis(instanceKey);
		delete (CExampleInterface*)impl1;
		impl1 = NULL;
		::LeaveIfErrNoMemory(err);
		}
	TEST(err == KErrNone);

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 2);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 2);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Now destroy the 2nd Implementation
	iLoadManager->DestroyedThis(instanceKey2);
	delete (CExampleInterface*)impl2;
	impl2 = NULL;

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) != NULL);

	//Now destroy the 1st Implementation;
	iLoadManager->DestroyedThis(instanceKey);
	delete (CExampleInterface*)impl1;
	impl1 = NULL;

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) != NULL);

	iLoadManager->ClearGarbage();

	//Checks the final state of the garbage policy
	//Tests for the new API ClearGarbage()
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);
	}

/**
Plugins used:
- Z:\\..\\EComExample3.dll contains ImplUid=0x101F8478
- C:\\..\\EComExample5.dll contains	ImplUid=0x101F847C

@SYMTestCaseID          SYSLIB-ECOM-CT-0701
@SYMTestCaseDesc	    Tests for create and delete of CLoadManager
@SYMTestPriority 	    High
@SYMTestActions  	    Create two implementation from two DIFFERENT DLL Z: C:
@SYMTestExpectedResults Two UnloadPolicy inside the array because of Two DLL loaded
                        Two Instance indexes because of two Different Implementation Uid's
@SYMREQ                 REQ0000
*/
void CLoadManagerTest::CreateDestroyTestCase1Plugin1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-0701 "));
	//Implementation Uid inside Z:\\..\\EComExample3.dll
	TUid implUid1={0x101F8478};
	//Implementation Uid inside C:\\..\\EComExample5.dll
	TUid implUid2={0x101F847C};
	//EComExample3.dll entry
	TUid dllUid1;
	dllUid1.iUid = 0x101F8477;
	//EComExample5.dll entry
	TUid dllUid2;
	dllUid2.iUid = 0x101F847B;

	CreateDestroyTestCase1L(dllUid1,implUid1,KExampleDllFileName3,dllUid2,implUid2,KExampleDllFileName5,KUidInterfaceImplementationCollection);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3551
@SYMTestCaseDesc	    Tests for create and delete of implementations using PLUGIN3 from two DIFFERENT DLL's
@SYMTestPriority 	    High
@SYMTestActions  	    Create two implementations from two DIFFERENT DLL's using new Dll type PLUGIN3.
						Test executes by getting the CUnloadPolicy* associated to the testing DLL EComExample10.dll.
						DestroyThis will actually either remove the UnloadPolicy from the list or decrease
						referencecount(to UnloadPolicy) by one.
@SYMTestExpectedResults Ensure the CLoadManager data structures are as expected after the ImplementationObjectL calls
						Two UnloadPolicy inside the array because of Two DLL loaded
						Two Instance indexes because of two Different Implementation Uid's
@SYMEC                  EC43
*/
void CLoadManagerTest::CreateDestroyTestCase1Plugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3551 "));
	//Implementation Uid inside Z:\\..\\EComExample10.dll
	TUid implUid1 = {0x10009E38};
	//Implementation Uid inside C:\\..\\EComExample12.dll
	TUid implUid2 = {0x10009E39};
	//EComExample10.dll entry
	TUid dllUid1;
	dllUid1.iUid = 0x10009E37;
	//EComExample12.dll entry
	TUid dllUid2;
	dllUid2.iUid = 0x10009E3E;

	CreateDestroyTestCase1L(dllUid1,implUid1,KExampleDllFileName10,dllUid2,implUid2,KExampleDllFileName12,KUidInterfaceImplementationCollection3);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3552
@SYMTestCaseDesc	    Tests for create and delete of implementations using PLUGIN1 and PLUGIN3
@SYMTestPriority 	    High
@SYMTestActions  	    Create two implementations from two DIFFERENT DLL's one of PLUGIN1, the other Dll
						of type PLUGIN3. Test executes by getting the CUnloadPolicy* associated to the
						testing DLL EComExample10.dll. DestroyThis will actually either remove the
						UnloadPolicy from the list or decrease 	referencecount(to UnloadPolicy) by one.
						Plugins used:
						- Z:\\..\\EComExample3.dll contains ImplUid=0x101F8478 - PLUGIN type dll
						- Z:\\..\\EComExample10.dll contains	ImplUid=0x10009E38 - PLUGIN3 type dll
@SYMTestExpectedResults Ensure the CLoadManager data structures are as expected after the ImplementationObjectL calls
						Two UnloadPolicy inside the array because of Two DLL loaded
						Two Instance indexes because of two Different Implementations
@SYMEC                  EC43
*/
void CLoadManagerTest::CreateDestroyTestCase1PluginAndPlugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3552 "));
		//Implementation Uid inside Z:\\..\\EComExample3.dll
	TUid ImplUid1 = {0x101F8478};
	//Implementation Uid inside Z:\\..\\EComExample10.dll
	TUid ImplUid2 = {0x10009E38};
	//Some interface instantiation parameters
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer	= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;
	TAny* impl1 = NULL;
	TAny* impl2 = NULL;

	//EComExample3.dll entry
	TUid dllUid1;
	dllUid1.iUid = 0x101F8477;

	TEntry entry1;
	entry1.iName = KExampleDllFileName3;
	entry1.iModified = TTime(0);
	entry1.iType = TUidType(KDynamicLibraryUid,KUidInterfaceImplementationCollection,dllUid1);

	//EComExample10.dll entry
	TUid dllUid2;
	dllUid2.iUid = 0x10009E37;

	TEntry entry2;
	entry2.iName = KExampleDllFileName10;
	entry2.iModified = TTime(0);
	entry2.iType = TUidType(KDynamicLibraryUid,KUidInterfaceImplementationCollection3,dllUid2);

	TInt err;
	//Creates Implementation with Uid=0x101F8478
	TUid instanceKey;
	TRAP(err,impl1 = iLoadManager->ImplementationObjectL(ImplUid1,entry1, &iInitParams,initParamsFlag,instanceKey));
	::LeaveIfErrNoMemory(err);
	TEST2(err,KErrNone);

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Creates 2nd Implementation with Uid=0x10009E38
	TUid instanceKey2;
	TRAP(err,impl2 = iLoadManager->ImplementationObjectL(ImplUid2,entry2, &iInitParams,initParamsFlag,instanceKey2));
	// If impl2 fails here due to KErrNoMemory we need to clear the previous impl1
	// to avoid memory leak
	if (err==KErrNoMemory)
		{
		iLoadManager->DestroyedThis(instanceKey);
		delete (CExampleInterface*)impl1;
		impl1 = NULL;
		::LeaveIfErrNoMemory(err);
		}
	TEST(err == KErrNone);

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 2);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 2);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Now destroy the 2nd Implementation
	iLoadManager->DestroyedThis(instanceKey2);
	delete (CExampleInterface*)impl2;
	impl2 = NULL;

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) != NULL);

	//Now destroy the 1st Implementation;
	iLoadManager->DestroyedThis(instanceKey);
	delete (CExampleInterface*)impl1;
	impl1 = NULL;

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) != NULL);

	iLoadManager->ClearGarbage();

	//Checks the final state of the garbage policy
	//Tests for the new API ClearGarbage()
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);
	}


// Test case 2:Create two implementation from one SAME DLL

void CLoadManagerTest::CreateDestroyTestCase2L(TUid aDllUid,
											   TUid aImplUid1,
											   const TDesC& aDllName,
											   TUid aImplUid2,
											   const TUid aIICType)
	{
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer	= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;
	TAny* impl1 = NULL;
	TAny* impl2 = NULL;

	TEntry entry;
	entry.iName = aDllName;
	entry.iModified = TTime(0);
	entry.iType = TUidType(KDynamicLibraryUid,aIICType,aDllUid);

	TInt err;
	//Creates Implementation 1
	TUid instanceKey;
	TRAP(err,impl1 = iLoadManager->ImplementationObjectL(aImplUid1,entry, &iInitParams,initParamsFlag,instanceKey));
	::LeaveIfErrNoMemory(err);
	TEST(err==KErrNone);
	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	TAny* impl4 = NULL;
	//Recreates another Implementation 1
	TUid instanceKey2;
	TRAP(err,impl4 = iLoadManager->ImplementationObjectL(aImplUid1,entry, &iInitParams,initParamsFlag,instanceKey2));
	if (err == KErrNoMemory)
		{
		iLoadManager->DestroyedThis(instanceKey);
		delete (CExampleInterface*)impl1;
		impl1 = NULL;
		::LeaveIfErrNoMemory(err);
		}
	TEST2(err,KErrNone);
	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 2);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Creates Implementation 2
	TUid instanceKey3;
	TRAP(err,impl2 = iLoadManager->ImplementationObjectL(aImplUid2,entry, &iInitParams,initParamsFlag,instanceKey3));
	// If impl2 fails here due to KErrNoMemory we need to clear the previous impl1
	// to avoid memory leak
	if (err == KErrNoMemory)
		{
		iLoadManager->DestroyedThis(instanceKey);
		delete (CExampleInterface*)impl1;
		impl1 = NULL;
		iLoadManager->DestroyedThis(instanceKey2);
		delete (CExampleInterface*)impl4;
		impl4 = NULL;
		::LeaveIfErrNoMemory(err);
		}

	TEST2(err,KErrNone);

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 3);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Creates another Implementation 2
	TAny* impl3 = NULL;
	TUid instanceKey4;
	TRAP(err,impl3 = iLoadManager->ImplementationObjectL(aImplUid2,entry, &iInitParams,initParamsFlag,instanceKey4));
	//Checks the state of the arrays inside CLoadManager and the garbage policy
	if (err == KErrNoMemory)
		{
		//cleanup the previously loaded implementation before leaving
		iLoadManager->DestroyedThis(instanceKey);
		delete (CExampleInterface*)impl1;
		impl1 = NULL;
		iLoadManager->DestroyedThis(instanceKey2);
		delete (CExampleInterface*)impl4;
		impl4 = NULL;
		iLoadManager->DestroyedThis(instanceKey3);
		delete (CExampleInterface*)impl2;
		impl2 = NULL;
		::LeaveIfErrNoMemory(err);
		}
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 4);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Destroy Implementation 1
	iLoadManager->DestroyedThis(instanceKey);
	delete (CExampleInterface*)impl1;
	impl1 = NULL;
	iLoadManager->DestroyedThis(instanceKey2);
	delete (CExampleInterface*)impl4;
	impl4 = NULL;

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 2);
	//Note here after destroying Impl1, the DLL is still in used, so no garbage policy
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Destroy Implementation 2
	iLoadManager->DestroyedThis(instanceKey3);
	delete (CExampleInterface*)impl2;
	impl2 = NULL;

	//Destroy 2nd Implementation 2
	iLoadManager->DestroyedThis(instanceKey4);
	delete (CExampleInterface*)impl3;
	impl3 = NULL;

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) != NULL);

	iLoadManager->ClearGarbage();

	//Checks the final state of the garbage policy
	//Tests for the new API ClearGarbage()
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);
	}


/**
Plugins used:
- Z:\\..\\EComExample2.dll contains ImplUid 0x10009DC3 & 0x10009DC4

@SYMTestCaseID          SYSLIB-ECOM-CT-0702
@SYMTestCaseDesc	    Tests for create and delete of CLoadManager
@SYMTestPriority 	    High
@SYMTestActions  	    Create two implementation from the SAME DLL
@SYMTestExpectedResults One UnloadPolicy since both Implementation is based on SAME DLL
                        Two Instance indexes because of two Different Implementations
@SYMREQ                 REQ0000
*/
void CLoadManagerTest::CreateDestroyTestCase2Plugin1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-0702 "));
	TUid implUid1={0x10009DC3};
	TUid implUid2={0x10009DC4};

	TUid dllUid;
	dllUid.iUid = 0x10009DB3;

	CreateDestroyTestCase2L(dllUid,implUid1,KExampleDllFileName2,implUid2,KUidInterfaceImplementationCollection);

	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3553
@SYMTestCaseDesc	    Tests for create and delete of implementations using PLUGIN3 from the SAME DLL
@SYMTestPriority 	    High
@SYMTestActions  	    Create two implementation from the SAME DLL using new Dll type PLUGIN3.
						Test executes by getting the CUnloadPolicy* associated to the testing DLL EComExample10.dll.
						DestroyThis will actually either remove the UnloadPolicy from the list or decrease
						referencecount(to UnloadPolicy) by one.
@SYMTestExpectedResults Ensure the CLoadManager data structures are as expected after the ImplementationObjectL calls
						One UnloadPolicy since both Implementation is based on SAME DLL
                        Two Instance indexes because of two Different Implementations
@SYMEC                  EC43
*/
void CLoadManagerTest::CreateDestroyTestCase2Plugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3553 "));
	TUid implUid1 = {0x10009E38};
	TUid implUid2 = {0x10009E3A};

	TUid dllUid;
	dllUid.iUid = 0x10009E37;

	CreateDestroyTestCase2L(dllUid,implUid1,KExampleDllFileName10,implUid2,KUidInterfaceImplementationCollection3);
	}


//Test case 3:Invalid argument testing for Create Implementation
// - Invalid Entry and Implementation uid argument in ImplementationObjectL

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0703
@SYMTestCaseDesc	    Tests for create CLoadManager
@SYMTestPriority 	    High
@SYMTestActions  	    Passing invalid Entry and Implementation uid argument in ImplementationObjectL
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CLoadManagerTest::CreateDestroyTestCase3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0703 "));
	TUid invalidUid={0x10007777};

	//Try supplying invalid implementaion Uid and invalid entry argument
	TUid dllUid;
	dllUid.iUid = 0;

	TEntry invalidentry;
	invalidentry.iName=_L("Z:\\system\\LIBS\\PLUGINS\\EComExample9.dll");
	invalidentry.iModified=TTime(0);
	invalidentry.iType=TUidType(KDynamicLibraryUid,KUidInterfaceImplementationCollection,dllUid);

	TBool initParamsFlag = ETrue;

	TUid instanceKey;
	TRAPD(err,iLoadManager->ImplementationObjectL(invalidUid,invalidentry, NULL,initParamsFlag,instanceKey));
	::LeaveIfErrNoMemory(err);
	//It will return KErrNotFound as the DLL is invalid
	TEST2(err,KErrNotFound);

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager)==0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager)==0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)==NULL);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3554
@SYMTestCaseDesc	    Invalid argument testing for Create Implementation using new DLL type PLUGIN3
@SYMTestPriority 	    High
@SYMTestActions  	    Passing invalid Entry and Implementation uid argument in ImplementationObjectL
@SYMTestExpectedResults KErrNotFound will be returned as the DLL is invalid
@SYMEC                  EC43
*/
void CLoadManagerTest::CreateDestroyTestCase3Plugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3554 "));
	// this implementation(0x10009E48) is implemented in EComExample14Upgraded.dll, not in EComExample10.dll
	TUid implementationUid = {0x10009E48};

	//Try supplying invalid implementaion Uid and invalid entry argument
	TUid dllUid;
	dllUid.iUid = 0;

	// Invalid entry for EComExample14Upgraded.dll, because it is not under Z:\\system\\LIBS\\PLUGINS
	TEntry invalidentry;
	invalidentry.iName = _L("Z:\\system\\LIBS\\PLUGINS\\EComExample14Upgraded.dll");
	invalidentry.iModified = TTime(0);
	invalidentry.iType = TUidType(KDynamicLibraryUid,KUidInterfaceImplementationCollection3,dllUid);

	TUid dllUid1;
	dllUid1.iUid = 0x10009E37;
	// Valid entry for EComExample10.dll
	TEntry entry;
	entry.iName = KExampleDllFileName10;
	entry.iModified = TTime(0);
	entry.iType = TUidType(KDynamicLibraryUid,KUidInterfaceImplementationCollection3,dllUid1);
	TBool initParamsFlag = ETrue;

	// Test for valid implementation Uid with invalid entry.
	TUid instanceKey;
	TRAPD(err,iLoadManager->ImplementationObjectL(implementationUid,invalidentry, NULL,initParamsFlag,instanceKey));
	::LeaveIfErrNoMemory(err);
	//Returns KErrNotFound as the DLL is invalid
	TEST2(err,KErrNotFound);
	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager)==0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager)==0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)==NULL);

	// Tests for invalid implementation Uid with valid entry.
	TRAP(err,iLoadManager->ImplementationObjectL(implementationUid,entry, NULL,initParamsFlag,instanceKey));
	::LeaveIfErrNoMemory(err);
	//Returns KErrNotFound as the implementation Uid is not within the DLL
	TEST2(err,KErrNotFound);

	//Checks the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) != NULL);
	}

//Test case 4:Create two instance of the same implementation Uid from one single DLL

void CLoadManagerTest::CreateDestroyTestCase4L(TUid aDllUid,
											   TUid aImplUid,
											   const TDesC& aDllName,
											   const TUid aIICType)
	{
	TInt err;
	TEntry entry;
	entry.iName=aDllName;
	entry.iModified=TTime(0);
	entry.iType=TUidType(KDynamicLibraryUid,aIICType,aDllUid);

	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer		= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;
	TAny* impl1=NULL;
	TAny* impl2=NULL;

	//Create 1st instance of implementation
	TUid instanceKey;
	TRAP(err,impl1=iLoadManager->ImplementationObjectL(aImplUid,entry, &iInitParams,initParamsFlag,instanceKey));
	::LeaveIfErrNoMemory(err);
	TEST2(err,KErrNone);

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager)==1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager)==1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)==NULL);

	//Create 2nd instance of the same implementation
	TUid instanceKey2;
	TRAP(err,impl2=iLoadManager->ImplementationObjectL(aImplUid,entry, &iInitParams,initParamsFlag,instanceKey2));
	// If impl2 fails here due to KErrNoMemory we need to clear the previous impl1
	// to avoid memory leak
	if (err==KErrNoMemory)
		{
		iLoadManager->DestroyedThis(instanceKey);
		delete (CExampleInterface*)impl1;
		impl1=NULL;
		::LeaveIfErrNoMemory(err);
		}

	TEST(err==KErrNone);

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager)==2);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager)==1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)==NULL);

	//Destroyed the 2nd instance of implementation
	iLoadManager->DestroyedThis(instanceKey2);
	delete (CExampleInterface*)impl2;
	impl2=NULL;

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager)==1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager)==1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)==NULL);

	//Destroyed the 1st instance of implementation
	iLoadManager->DestroyedThis(instanceKey);
	delete (CExampleInterface*)impl1;
	impl1=NULL;

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager)==0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager)==0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)!=NULL);

	iLoadManager->ClearGarbage();
	//Checking the final state of the garbage policy
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager)==NULL);
	}


/**
Plugins used:
- Z:\\..\\EComExample2.dll contains ImplUid 0x10009DC3 & 0x10009DC4

@SYMTestCaseID          SYSLIB-ECOM-CT-0704
@SYMTestCaseDesc	    Tests for create and delete of CLoadManager
@SYMTestPriority 	    High
@SYMTestActions  	    Create two instance of the same implementation Uid from one single DLL
@SYMTestExpectedResults CLoadManager only maintains a single UnloadPolicy and one policy
						mapping index
@SYMREQ                 REQ0000
*/
void CLoadManagerTest::CreateDestroyTestCase4Plugin1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0704 "));
	TUid implUid={0x10009DC3};

	TUid dllUid;
	dllUid.iUid = 0x10009DB3;

	CreateDestroyTestCase4L(dllUid,implUid,KExampleDllFileName2,KUidInterfaceImplementationCollection);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3555
@SYMTestCaseDesc	    Create two instance of the same implementation Uid from one single DLL using PLUGIN3
						Plugins used:
						- Z:\\..\\EComExample10.dll contains ImplUid 0x10009E38 & 0x10009E3A
@SYMTestPriority 	    High
@SYMTestActions  	    Create two instance of the same implementation Uid from one single DLL
@SYMTestExpectedResults Ensure the CLoadManager data structures are as expected after the ImplementationObjectL calls
						CLoadManager only maintains a single UnloadPolicy and one instance
@SYMEC                  EC43
*/
void CLoadManagerTest::CreateDestroyTestCase4Plugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3555 "));
	TUid implUid={0x10009E38};

	TUid dllUid;
	dllUid.iUid = 0x10009E37;

	CreateDestroyTestCase4L(dllUid,implUid,KExampleDllFileName10,KUidInterfaceImplementationCollection3);
	}


// Test case 5: DEF101930: Incorrect policy object returned in


void CLoadManagerTest::CreateDestroyTestCase5L(TUid aDllUid1,
	  	            		  TUid aImplUid1,
							  const TDesC& aDllName1,
							  TUid aDllUid2,
							  TUid aImplUid2,
							  const TDesC& aDllName2,
							  TUid aImplUid3,
							  const TUid aIICType)
	{
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer		= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;
	TAny* impl1 = NULL;
	TAny* impl2 = NULL;
	TAny* impl3 = NULL;

	TEntry entryExample1;
	entryExample1.iName = aDllName1;
	entryExample1.iModified = TTime(0);
	entryExample1.iType = TUidType(KDynamicLibraryUid,aIICType,aDllUid1);

	TEntry entryExample2;
	entryExample2.iName = aDllName2;
	entryExample2.iModified = TTime(0);
	entryExample2.iType = TUidType(KDynamicLibraryUid,aIICType,aDllUid2);

	TInt err;
	TUid instanceKey2;

	//Create Implementation 2
	TRAP(err,impl2 = iLoadManager->ImplementationObjectL(aImplUid2,entryExample1, &iInitParams,initParamsFlag,instanceKey2));
	::LeaveIfErrNoMemory(err);
	TEST(err == KErrNone);
	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	TUid instanceKey3;
	//Create Implementation 3
	TRAP(err,impl3 = iLoadManager->ImplementationObjectL(aImplUid3,entryExample2, &iInitParams,initParamsFlag,instanceKey3));
	// If impl3 fails here due to KErrNoMemory we need to clear the previous impl2
	// to avoid memory leak
	if (err == KErrNoMemory)
		{
		iLoadManager->DestroyedThis(instanceKey2);
		delete (CExampleInterface*)impl2;
		impl2 = NULL;
		::LeaveIfErrNoMemory(err);
		}
	TEST2(err,KErrNone);

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 2);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 2);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Create Implementation 1
	TUid instanceKey1;
	TRAP(err,impl1 = iLoadManager->ImplementationObjectL(aImplUid1,entryExample1, &iInitParams,initParamsFlag,instanceKey1));
	// If impl1 fails here due to KErrNoMemory we need to clear the previous impl2 and impl3
	// to avoid memory leak
	if (err == KErrNoMemory)
		{
		iLoadManager->DestroyedThis(instanceKey2);
		delete (CExampleInterface*)impl2;
		impl2=NULL;
		iLoadManager->DestroyedThis(instanceKey3);
		delete (CExampleInterface*)impl3;
		impl3=NULL;
		::LeaveIfErrNoMemory(err);
		}
	TEST2(err,KErrNone);

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 3);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 2);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Destroy Implementation 1
	iLoadManager->DestroyedThis(instanceKey1);
	delete (CExampleInterface*)impl1;
	impl1 = NULL;

	//Destroy Implementation 2
	iLoadManager->DestroyedThis(instanceKey2);
	delete (CExampleInterface*)impl2;
	impl2 = NULL;

	//Destroy Implementation 3
	iLoadManager->DestroyedThis(instanceKey3);
	delete (CExampleInterface*)impl3;
	impl3 = NULL;

	//Checking the state of the arrays inside CLoadManager and the garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) != NULL);

	iLoadManager->ClearGarbage();

	//Checking the final state of the garbage policy
	//Test for the new API ClearGarbage()
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	}

/**
Plugins used:
- Z:\\..\\EComLoadMgrExample1.dll contains ImplUid 0x10282332 & 0x10282334
- Z:\\..\\EComLoadMgrExample2.dll contains ImplUid 0x10282333

@SYMTestCaseID			SYSLIB-ECOM-UT-3372
@SYMTestCaseDesc		This testcase checks for the case where 2 implementations
						are created each on separate DLL's then an additional
						implementation is created from one of the already loaded DLL's.
						The impl uid of the newly created impl is lower then the 2
						existing impl uids. The impl uids from the same DLL are such
						that 1 is higher then the uid of the other impl and 1 is lower than
						the other impl.
@SYMTestPriority			High
@SYMTestActions			Load impls 0x10282333 and 0x10282334 first and then load impl
						0x10282332.
@SYMTestExpectedResults	Leave with KErrNotFound must not occur.
						Policy and unload policy counts must be updated
						as described in the test.
@SYMDEF				DEF101930
*/
void CLoadManagerTest::CreateDestroyTestCase5Plugin1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3372 "));
	TUid implUid1={0x10282332};
	TUid implUid2={0x10282334};
	TUid implUid3={0x10282333};

	TUid dllUidExample1;
	dllUidExample1.iUid = 0x10282330;

	TUid dllUidExample2;
	dllUidExample2.iUid = 0x10282335;

	CreateDestroyTestCase5L(dllUidExample1,
							implUid1,
							KLoadMgrExample1DllName,
							dllUidExample2,
							implUid2,
							KLoadMgrExample2DllName,
							implUid3,
							KUidInterfaceImplementationCollection);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3556
@SYMTestCaseDesc	    DEF101930: Incorrect policy object returned in
						This testcase checks for the case where 2 implementations
						are created each on separate DLL's then an additional
						implementation is created from one of the already loaded DLL's.
						The impl uid of the newly created impl is lower then the 2
						existing impl uids. The impl uids from the same DLL are such
						that 1 is higher then the uid of the other impl and 1 is lower than
						the other impl. DEF101930: Incorrect policy object returned in

						Plugins used:
						- Z:\\..\\EComExample10.dll contains ImplUid 0x10009E38 & 0x10009E3A
						- Z:\\..\\EComExample12.dll contains ImplUid 0x10009E39
@SYMTestPriority 	    High
@SYMTestActions  	    Load impls 0x10009E3A and 0x10009E39 first and then load impl 0x10009E38.
@SYMTestExpectedResults Leave with KErrNotFound must not occur.
						Policy and unload policy counts must be updated
						as described in the test.
@SYMEC                  EC43 (tested for PLUGIN3 too now, copied from UT-3372)
*/
void CLoadManagerTest::CreateDestroyTestCase5Plugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3556 "));
	TUid implUid1={0x10009E38};
	TUid implUid2={0x10009E3A};
	TUid implUid3={0x10009E39};

	TUid dllUidExample1;
	dllUidExample1.iUid = 0x10009E37;

	TUid dllUidExample2;
	dllUidExample2.iUid = 0x10009E3E;

	CreateDestroyTestCase5L(dllUidExample1,
							implUid1,
							KExampleDllFileName10,
							dllUidExample2,
							implUid2,
							KExampleDllFileName12,
							implUid3,
							KUidInterfaceImplementationCollection3);
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0705
@SYMTestCaseDesc	    Tests for production class CLoadManager
@SYMTestPriority 	    High
@SYMTestActions  	    Check for null condition while creating a new CLoadManagerTest object.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void CreateDeleteTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0705 CreateDeleteTestL "));

	//
	// Tests the Creating and deletion of CLoadManager
	// ------------------------------------------------------------------
	//
	// Set up for heap leak checking
	__UHEAP_MARK;

	TInt startProcessHandleCount = 0;
	TInt startThreadHandleCount = 0;
	TInt endProcessHandleCount = 0;
	TInt endThreadHandleCount = 0;

	RThread rThread;
	rThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	//CREATE DELETE TEST START//

	CLoadManagerTest* loadManagerTest = CLoadManagerTest::NewL();

	TEST(loadManagerTest != NULL);

	delete loadManagerTest;

	//CREATE DELETE TEST END//

	rThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
	TEST(startThreadHandleCount == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0706
@SYMTestCaseDesc	    OOM test while creating and deleting CLoadManager
@SYMTestPriority 	    High
@SYMTestActions  	    Create and delete CLoadManager object and test that no handles have leaked
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void OOMCreateDeleteTest()
	{
	//
	// Out of memory test
	// ------------------------------------------------------------------
	//
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0706 OOMCreateDeleteTest "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CLoadManagerTest* loadManagerTest = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);

		TRAP(err, loadManagerTest = CLoadManagerTest::NewL());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete loadManagerTest;
		loadManagerTest = NULL;

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	TheTest.Printf(_L("- Succeeded at heap failure rate of %i\n"), failAt);
	TEST(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3557
@SYMTestCaseDesc	   	This testcase tests fetching extended interfaces for ImplUid 0x10009E38.
						One interface will be the part of the main implementation object, the
						second interface will be a separate interface requiring release. Also tests
						that the extended interfaces are automatically destroyed.

						Plugins used:
						- Z:\\..\\EComExample10.dll contains ImplUid 0x10009E38 with extended interface
						0x10009E44, 0x10009E45 and 0x10009E46
@SYMTestPriority 	    High
@SYMTestActions  	    This testcase will test one implementation with two extended interfaces.
						Create implementation, then fetch extended interface 0x10009E44. Secondly,
						fetch 0x10009E45 interface. This will be a separate interface requiring release.
						Destroy the interfaces.
						Attempt fetch of interface again. Expect Leave with KErrNotFound as implementation
						no longer exists.
@SYMTestExpectedResults Extended instance count will be zero for 0x10009E44 extended interface,
						0x10009E45 will have an extended instance count	of one. No memory leaks
						after implementation, including extended interfaces	are automatically
						destroyed.
@SYMEC                  EC43
@SYMDEF					DEF111553
*/
void CLoadManagerTest::ExtendedMethodTest1Plugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3557 "));
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer	= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor = &KDummyText;
	TUid dllUid;
	dllUid.iUid = 0x10009E37;

	TEntry entry;
	entry.iName = KExampleDllFileName10;
	entry.iModified = TTime(0);
	entry.iType = TUidType(KDynamicLibraryUid,KUidInterfaceImplementationCollection3,dllUid);

	iImplementationUid.iUid = 0x10009E38;

	CExampleInterface* implementationPtr = 0;
	TUid instanceKey;
	TRAPD(err, implementationPtr = reinterpret_cast<CExampleInterface*>(iLoadManager->ImplementationObjectL(iImplementationUid, entry,&iInitParams, initParamsFlag,instanceKey)));

	::LeaveIfErrNoMemory(err);
	TEST2(err,KErrNone);
	TEST(implementationPtr != 0);

	CleanupStack::PushL(implementationPtr);

	//Checks the state of the arrays inside CLoadManager and garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Checks the instance index and UnloadPolicy info
	TEST(iStateAccessor.InstanceIndex(0,*iLoadManager)->ImplementationUid() == iImplementationUid);
	TEST(iStateAccessor.UnloadPolicy(0,*iLoadManager)->DllEntryInformation().GetName().CompareF(KExampleDllFileName10) == 0);

	// Call extended interface here.
	const TUid KExample10InterfaceUid1 = {0x10009E44}; // Part of main interface
	const TUid KExample10InterfaceUid2 = {0x10009E45}; // Separate interface

	MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(iLoadManager->GetExtendedInterfaceL(instanceKey, KExample10InterfaceUid1 ));
	// Extended interface part of main interface, so no extended object info is needed to be created.
	TEST2(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager),0);
	ext1->DoMethodExtended();

	MExampleInterfaceExtended2* ext2 = static_cast<MExampleInterfaceExtended2*>(iLoadManager->GetExtendedInterfaceL(instanceKey, KExample10InterfaceUid2 ));
	// Separate extended interface, so there will be an extended object info created to track
	// this extended interface so it can be deleted later when the extended interface is no
	// longer needed.
	TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager)==1);
	ext2->DoMethodExtended2();

	CleanupStack::PopAndDestroy(implementationPtr);
	implementationPtr = 0;
	iLoadManager->DestroyedThis(instanceKey);
	iLoadManager->ClearGarbage();
	//Checks the state of the arrays inside CLoadManager and garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	// call again now that implementation has been removed. Expecting a Leave
	TRAP(err, iLoadManager->GetExtendedInterfaceL(instanceKey, KExample10InterfaceUid2 ));
	TEST2(err, KErrNotFound);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3558
@SYMTestCaseDesc	   	This testcase tests fetching extended interfaces for ImplUid 0x10009E38.
						One interface will be the part of the main implementation object, the
						second interface will be a separate interface requiring release. Tests
						that manual release successful for interface requiring release. Tests
						no memory leaks, and no double release of manually released interface.

						Plugins used:
						- Z:\\..\\EComExample10.dll contains ImplUid 0x10009E38 with extended interface
						0x10009E44, 0x10009E45 and 0x10009E46
@SYMTestPriority 	    High
@SYMTestActions  	    This testcase will test one implementation with two extended interfaces.
						Create implementation, then fetch extended interface 0x10009E44. Secondly,
						fetch 0x10009E45 interface. This will be a separate interface requiring release.
						Manually release extended interface. Destroy the interfaces.
						Attempt fetch of interface again. Expect Leave with KErrNotFound as implementation
						no longer exists.
@SYMTestExpectedResults Extended instance count will be zero for 0x10009E44 extended interface,
						0x10009E45 will have an extended instance count	of one. After manual
						release, the extended interface count will be zero.	No memory leaks after
						implementation, including extended interfaces are automatically destroyed.
						No double release of extended interface already	manually released.
@SYMEC                  EC43
@SYMDEF					DEF111553
*/
void CLoadManagerTest::ExtendedMethodTest2Plugin3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3558 "));
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer	= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;
	TUid dllUid;
	dllUid.iUid = 0x10009E37;

	TEntry entry;
	entry.iName = KExampleDllFileName10;
	entry.iModified = TTime(0);
	entry.iType = TUidType(KDynamicLibraryUid,KUidInterfaceImplementationCollection3,dllUid);

	iImplementationUid.iUid = 0x10009E38;

	CExampleInterface* implementationPtr = 0;
	TUid instanceKey;
	TRAPD(err, implementationPtr = reinterpret_cast<CExampleInterface*>(iLoadManager->ImplementationObjectL(iImplementationUid, entry,&iInitParams, initParamsFlag,instanceKey)));

	::LeaveIfErrNoMemory(err);
	TEST2(err,KErrNone);
	TEST(implementationPtr != 0);

	CleanupStack::PushL(implementationPtr);

	//Checks the state of the arrays inside CLoadManager and garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 1);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 1);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	//Checks the instance index and UnloadPolicy info
	TEST(iStateAccessor.InstanceIndex(0,*iLoadManager)->ImplementationUid() == iImplementationUid);
	TEST(iStateAccessor.UnloadPolicy(0,*iLoadManager)->DllEntryInformation().GetName().CompareF(KExampleDllFileName10) == 0);

	// Call extended interface here.
	const TUid KExample10InterfaceUid1 = {0x10009E44};
	const TUid KExample10InterfaceUid2 = {0x10009E45}; // separate interface object

	MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(iLoadManager->GetExtendedInterfaceL(instanceKey, KExample10InterfaceUid1 ));
	TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 0); // Does not need to be released, so will not be added to extended object array
	ext1->DoMethodExtended();

	MExampleInterfaceExtended2* ext2 = static_cast<MExampleInterfaceExtended2*>(iLoadManager->GetExtendedInterfaceL(instanceKey, KExample10InterfaceUid2 ));
	TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 1); // Needs to be released
	ext2->DoMethodExtended2();

    iLoadManager->ManuallyReleaseExtendedInterfaceL(instanceKey, KExample10InterfaceUid2);
    TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 0);

	CleanupStack::PopAndDestroy(implementationPtr);
	implementationPtr = 0;
	iLoadManager->DestroyedThis(instanceKey);
	iLoadManager->ClearGarbage();
	//Checks the state of the arrays inside CLoadManager and garbage policy
	TEST(iStateAccessor.InstanceIndexCount(*iLoadManager) == 0);
	TEST(iStateAccessor.UnloadPolicyCount(*iLoadManager) == 0);
	TEST(iStateAccessor.GarbagePolicy(*iLoadManager) == NULL);

	// call again now that implementation has been removed. Expecting a Leave
	TRAP(err, iLoadManager->GetExtendedInterfaceL(instanceKey, KExample10InterfaceUid2 ));
	TEST2(err, KErrNotFound);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3711
@SYMTestCaseDesc	   	This testcase tests ManuallyReleaseExtendedInterfaces function for releasing
						extended interface which require release and releasing extended interface
						which doesn't require release. Also tests for releasing same extended interface twice

						Plugins used:
						- Z:\\..\\EComExample10.dll contains ImplUid 0x10009E38 with extended interface
						0x10009E44, 0x10009E45 and 0x10009E46
@SYMTestPriority 	    High
@SYMTestActions  	    This testcase will do the tests as below:
						1. create implementation 0x10009E38.
						2. Get extended interface 0x10009E44,0x10009E45 and 0x10009E46.
						3. Manually release extended interface 0x10009E44 which does not require release.
						4. Manually release extended interface 0x10009E45 which require manually release.
						5. Again manually release extended interface 0x10009E45 which require manually release.
						6. Manually release extended interface 0x10009E46 which require manually release.
						7. Destroy the interfaces
						8. Attempt fetch manual release again. Expect Leave with KErrNotFound as implementation
						no longer exists.
@SYMTestExpectedResults Extended instance count is set correctly.
@SYMDEF                 DEF111196
@SYMDEF					DEF111553
*/
void CLoadManagerTest::ManuallyReleaseExtendedInterfaceTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3711 "));
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer	= 5;
	TBool initParamsFlag = ETrue;
	_LIT(KDummyText,"Dummy params");
	iInitParams.descriptor	= &KDummyText;
	TUid dllUid;
	dllUid.iUid = 0x10009E37;

	TEntry entry;
	entry.iName = KExampleDllFileName10;
	entry.iModified = TTime(0);
	entry.iType = TUidType(KDynamicLibraryUid,KUidInterfaceImplementationCollection3,dllUid);

	iImplementationUid.iUid = 0x10009E38;

	CExampleInterface* implementationPtr = 0;
	TUid instanceKey;
	TRAPD(err, implementationPtr = reinterpret_cast<CExampleInterface*>(iLoadManager->ImplementationObjectL(iImplementationUid, entry,&iInitParams, initParamsFlag,instanceKey)));

	::LeaveIfErrNoMemory(err);
	TEST2(err,KErrNone);
	TEST(implementationPtr != 0);

	CleanupStack::PushL(implementationPtr);

	const TUid KExample10InterfaceUid1 = {0x10009E44};
	const TUid KExample10InterfaceUid2 = {0x10009E45}; // separate interface object
	const TUid KExample10InterfaceUid3 = {0x10009E46}; // separate interface object

	// Gets extended interface 0x10009E44
	MExampleInterfaceExtended* ext1 = static_cast<MExampleInterfaceExtended*>(iLoadManager->GetExtendedInterfaceL(instanceKey, KExample10InterfaceUid1 ));
	TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 0); // Does not need to be released, so will not be added to extended object array

	// Gets extended interface 0x10009E45
	MExampleInterfaceExtended2* ext2 = static_cast<MExampleInterfaceExtended2*>(iLoadManager->GetExtendedInterfaceL(instanceKey, KExample10InterfaceUid2 ));
	TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 1); // Needs to be released

	// Gets extended interface 0x10009E46
	MExampleInterfaceExtended2* ext3 = static_cast<MExampleInterfaceExtended2*>(iLoadManager->GetExtendedInterfaceL(instanceKey, KExample10InterfaceUid3 ));
	TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 2); // Needs to be released

	// call ManuallyReleaseExtendedInterface for 0x10009E44.
	// Nothing would happen because 0x10009E44 is not require release.
    iLoadManager->ManuallyReleaseExtendedInterfaceL(instanceKey, KExample10InterfaceUid1);
    TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 2);

	// call ManuallyReleaseExtendedInterface for 0x10009E45.
	// extended instance count would be 1
    iLoadManager->ManuallyReleaseExtendedInterfaceL(instanceKey, KExample10InterfaceUid2);
    TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 1);

    // call ManuallyReleaseExtendedInterface again, nothing would happen because 0x10009E45 has been released.
    iLoadManager->ManuallyReleaseExtendedInterfaceL(instanceKey, KExample10InterfaceUid2);
    TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 1);

    // call ManuallyReleaseExtendedInterface for 0x10009E46.
	// extended instance count would be 0
    iLoadManager->ManuallyReleaseExtendedInterfaceL(instanceKey, KExample10InterfaceUid3);
    TEST(iStateAccessor.ExtendedInstanceCount(0,*iLoadManager) == 0);

	CleanupStack::PopAndDestroy(implementationPtr);
	implementationPtr = 0;
	iLoadManager->DestroyedThis(instanceKey);
	iLoadManager->ClearGarbage();

	// call again now that implementation has been removed. Expecting a Leave
	TRAP(err, iLoadManager->ManuallyReleaseExtendedInterfaceL(instanceKey, KExample10InterfaceUid3 ));
	TEST2(err, KErrNotFound);
	}



typedef void (CLoadManagerTest::*ClassFuncPtrL) (void);

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0707
@SYMTestCaseDesc	    Wrapper function to call all test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up CLoadManagerTest's test function.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all test functions

@param		testFuncL pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0707 "));
	TheTest.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CLoadManagerTest* loadManagerTest = CLoadManagerTest::NewL();
	CleanupStack::PushL(loadManagerTest);

	(loadManagerTest->*testFuncL)();

	CleanupStack::PopAndDestroy(loadManagerTest);

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	TEST(startProcessHandleCount == endProcessHandleCount);
	TEST(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0708
@SYMTestCaseDesc	    Function to call all OOM test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up all test functions for OOM test.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all OOM test functions

@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0708 "));
	TheTest.Next(aTestDesc);

	TInt err, tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		CLoadManagerTest* loadManagerTest = CLoadManagerTest::NewL();
		CleanupStack::PushL(loadManagerTest);

		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, (loadManagerTest->*testFuncL)());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		CleanupStack::PopAndDestroy(loadManagerTest);
		loadManagerTest = NULL;

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	TEST(err == KErrNone);
	TheTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

void DoTestL()
	{
	__UHEAP_MARK;

	// Basic tests
	CreateDeleteTestL();
	DoBasicTestL(&CLoadManagerTest::InstantiationMethodTestPlugin1L, _L("InstantiationMethodTestPlugin1L"));
	DoBasicTestL(&CLoadManagerTest::DestroyThisTestPlugin1L, _L("DestroyThisTestPlugin1L"));
	DoBasicTestL(&CLoadManagerTest::GetImplementationUidTestL, _L("GetImplementationUidTestL"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase1Plugin1L,_L("Create Destroy Test case 1"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase2Plugin1L,_L("Create Destroy Test case 2"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase3L,_L("Create Destroy Test case 3"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase4Plugin1L,_L("Create Destroy Test case 4"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase5Plugin1L,_L("Create Destroy Test case 5"));

	DoBasicTestL(&CLoadManagerTest::InstantiationMethodTestPlugin3L, _L("InstantiationMethodTest3L"));
	DoBasicTestL(&CLoadManagerTest::DestroyThisTestPlugin3L, _L("DestroyThisTest3L"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase1Plugin3L,_L("Create Destroy Test case 1,Plugin3"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase2Plugin3L,_L("Create Destroy Test case 2"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase3Plugin3L,_L("Create Destroy Test case 3"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase4Plugin3L,_L("Create Destroy Test case 4"));
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase5Plugin3L,_L("Create Destroy Test case 5"));

	//Create an implementation from a PLUGIN dll and one from a PLUGIN3 dll also.
	DoBasicTestL(&CLoadManagerTest::CreateDestroyTestCase1PluginAndPlugin3L,_L("CreateDestroyTestCase1PluginAndPlugin3L"));

	// Get and Release extended interfaces
	DoBasicTestL(&CLoadManagerTest::ExtendedMethodTest1Plugin3L,_L("ExtendedMethodTest1Plugin3L"));
	DoBasicTestL(&CLoadManagerTest::ExtendedMethodTest2Plugin3L,_L("ExtendedMethodTest2Plugin3L"));
	DoBasicTestL(&CLoadManagerTest::ManuallyReleaseExtendedInterfaceTestL,_L("ManuallyReleaseExtendedInterfaceTestL"));

	DoBasicTestL(&CLoadManagerTest::ImplementationObjectTestL,_L("ImplementationObjectTestL"));


	OOMCreateDeleteTest();

	DoOOMTestL(&CLoadManagerTest::InstantiationMethodTestPlugin1L, _L("OOM InstantiationMethodTestPlugin1L"));
	DoOOMTestL(&CLoadManagerTest::DestroyThisTestPlugin1L, _L("OOM DestroyThisTestPlugin1L"));
	DoOOMTestL(&CLoadManagerTest::GetImplementationUidTestL, _L("OOM GetImplementationUidTestL"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase1Plugin1L,_L("OOM Create Destroy Test case 1"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase2Plugin1L,_L("OOM Create Destroy Test case 2"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase3L,_L("OOM Create Destroy Test case 3"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase4Plugin1L,_L("OOM Create Destroy Test case 4"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase5Plugin1L,_L("OOM Create Destroy Test case 5"));

	DoOOMTestL(&CLoadManagerTest::InstantiationMethodTestPlugin3L, _L("OOM InstantiationMethodTest3L"));
	DoOOMTestL(&CLoadManagerTest::DestroyThisTestPlugin3L, _L("OOM DestroyThisTest3L"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase1Plugin3L,_L("OOM Create Destroy Test case 1,Plugin3"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase2Plugin3L,_L("OOM Create Destroy Test case 2"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase3Plugin3L,_L("OOM Create Destroy Test case 3"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase4Plugin3L,_L("OOM Create Destroy Test case 4"));
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase5Plugin3L,_L("OOM Create Destroy Test case 5"));

	//Create an implementation from a PLUGIN dll and one from a PLUGIN3 dll also.
	DoOOMTestL(&CLoadManagerTest::CreateDestroyTestCase1PluginAndPlugin3L,_L("OOM CreateDestroyTestCase1PluginAndPlugin3L"));

	// Get and Release extended interfaces
	DoOOMTestL(&CLoadManagerTest::ExtendedMethodTest1Plugin3L,_L("OOM ExtendedMethodTest1Plugin3L"));
	DoOOMTestL(&CLoadManagerTest::ExtendedMethodTest2Plugin3L,_L("OOM ExtendedMethodTest2Plugin3L"));
	DoOOMTestL(&CLoadManagerTest::ManuallyReleaseExtendedInterfaceTestL,_L("OOM ManuallyReleaseExtendedInterfaceTestL"));

	__UHEAP_MARKEND;
	}

//Leave if aError < 0 with info message printed out
LOCAL_C void LeaveIfError(TInt aError, TInt aLine)
	{
	if(aError < KErrNone)
		{
		::Leave(aError, aLine);
		}
	}

#define LEAVE_IF_ERROR(aError) ::LeaveIfError(aError, __LINE__)
//
// Copies the Plugins to specific folder for testing purpose
//
LOCAL_C void CopyPlugins()
	{
	// Connect the file server instance
	LEAVE_IF_ERROR(TheFs.Connect());

	TRAPD(err, EComTestUtils::FileManCopyFileL(KResourceFileName5OnZ, KResourceFileName5));
	TEST(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KExampleDllFileName5OnZ, KExampleDllFileName5));
	TEST(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KResourceFileName12OnZ, KResourceFileName12));
	TEST(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KExampleDllFileName12OnZ, KExampleDllFileName12));
	TEST(err==KErrNone);
	}

// Deleting plugin from the RAM for cleanup purpose
LOCAL_C void DeleteTestPlugin()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KResourceFileName5));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KExampleDllFileName5));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KResourceFileName12));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KExampleDllFileName12));
	}

//
//Initialise the Active Scheduler
//
LOCAL_C void SetupL()
	{
	// Construct and install the Active Scheduler. The Active Schedular is needed
	// by components used by this test as they are ActiveObjects.
	TheActiveScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	TheTest.Printf(_L("\n"));
	TheTest.Title();
	TheTest.Start( _L("LoadManager Tests.") );

	TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, SetupL());
	TEST(err == KErrNone);

	CopyPlugins();
	// The reason for the folowing delay is:
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that an active object,
	// which scans plugin directories. So the discovering service is asynchronous.
	// We have to wait some time until it finishes.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);

	//Call the main tess
	TRAP(err, DoTestL());
	TEST2(err,KErrNone);

	// Cleanup files. If the cleanup fails that is no problem,
	// as any subsequent tests will replace them. The only downside
	// would be the disk not being tidied
	DeleteTestPlugin();

	TheFs.Close();

	delete TheActiveScheduler;
	delete TheTrapCleanup;

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
	}
