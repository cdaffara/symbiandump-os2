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
// This file contains test classes and their implementations that test
// class CDefaultResolver. Demonstrates a simple set of derived class
// implementations using RTest.
// 
//

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "RegistryData.h"
#include "Registrar.h"
#include "RegistrarObserver.h"
#include "EComResolverParams.h"
#include "DefaultResolver.h"
#include "../EcomTestUtils/EcomTestUtils.h"
#include "DriveInfo.h"
#include "RegistryResolveTransaction.h"


#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <startup.hrh>

// Used for suppressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

// Used for OOM test
#define TEST_OOM_ERR if(err == KErrNoMemory) User::Leave(err)

// Interface Uids used within tests
const TUid KCExampleInterfaceUid = {0x10009DC0};
const TUid KCExampleInterfaceImp = {0x10009DC1};
const TInt KOneSecond = 1000000;

// Interface Implementation Uids used for testing
const TInt KUidImplementation1 = 0x10009DC3;
const TInt KUidImplementation2 = 0x10009DC4;

// Dlls copied to RAM for testing purpose
_LIT(KEComExDllOnZ, "z:\\RAMOnly\\EComExample5.dll");

// Contains .rsc files of dlls that be copied to RAM
// for testing purpose
_LIT(KEComExDllOnC, "c:\\sys\\bin\\EComExample5.dll");
_LIT(KEComRscFileOnZ, "z:\\RAMOnly\\EComExample5.rsc");
_LIT(KEComRscFileOnC, "c:\\resource\\plugins\\EComExample5.rsc");

_LIT(KEComExampleDllOnC,		"C:\\sys\\bin\\EComExample.dll");
_LIT(KEComExample2DllOnC,		"C:\\sys\\bin\\EComExample2.dll");
_LIT(KEComExample3DllOnC,		"C:\\sys\\bin\\EComExample3.dll");

_LIT(KEComExampleRscOnC,		"C:\\resource\\plugins\\EComExample.rsc");
_LIT(KEComExample2RscOnC,		"C:\\resource\\plugins\\EComExample2.rsc");
_LIT(KEComExample3RscOnC,		"C:\\resource\\plugins\\EComExample3.rsc");

_LIT(KEComExampleRscOnZ,		"Z:\\RAMOnly\\EComExample.rsc");
_LIT(KEComExample2RscOnZ,		"Z:\\RAMOnly\\EComExample2.rsc");
_LIT(KEComExample3RscOnZ,		"Z:\\RAMOnly\\EComExample3.rsc");


_LIT(KEComExampleDllOnZ,			"Z:\\RAMOnly\\EComExample.dll");
_LIT(KEComExample2DllOnZ,		"Z:\\RAMOnly\\EComExample2.dll");
_LIT(KEComExample3DllOnZ,		"Z:\\RAMOnly\\EComExample3.dll");

// Datatype on implementations that .rsc file contains
_LIT8(KResolveMatchType, "text/wml");
// Datatype to look for
_LIT8(KResolveImplementationType, "text/wml||data");

LOCAL_D CTrapCleanup*		TheTrapCleanup=NULL;

LOCAL_D CActiveScheduler*	TheActiveScheduler=NULL;

LOCAL_D RFs					TheFs;

LOCAL_D RTest				test(_L("t_resolver.exe"));

//LOCAL_D TCapabilitySet		dummycaps;
// Utility clean up function
LOCAL_C void CleanupEComPtrArray(TAny* aArray);

/**
This friend class allows us to access private and protected members of production
code class CDefaultResolver
*/
class TDefaultResolver_StateAccessor
	{
public:
	//Auxiliary methods that provide access to private members
	TBool Match(CDefaultResolver& aResolver,
				const TDesC8& aImplementationType,
				const TDesC8& aMatchType,
				TBool aIsGeneric) const;

	TUid Resolve(CDefaultResolver& aResolver,
				 RImplInfoArray& aImplementationInfo,
				 const TEComResolverParams& aAdditionalParameters) const;
	};

/**
Searches for a match of a data type on an implementation type.
Match returns ETrue if aMatchType is found within aImplementationType
@param		aResolver resolver object on which implementations are matched
@param		aImplementationType The implementation data type to search for a match
@param		aMatchType Search data that identifies/matches to  implementations
@param		aIsGeneric ETrue if wildcard matching is allowed
@return		TBool ETrue if a match is found, EFalse if match is not found
*/
TBool TDefaultResolver_StateAccessor::Match(CDefaultResolver& aResolver,
											const TDesC8& aImplementationType,
											const TDesC8& aMatchType,
											TBool aIsGeneric) const
	{
	return aResolver.Match(aImplementationType, aMatchType, aIsGeneric);
	}

/**
Selects	an appropriate implementation from a list of possibles
@param		aResolver resolver object on which implementations are resolved
@param		aImplementationInfo Information on the potential implementations
@param		aAdditionalParameters The data to match against to determine the
			implementation
@return		The Uid of a suitable implementation
*/
TUid TDefaultResolver_StateAccessor::Resolve(CDefaultResolver& aResolver,
											 RImplInfoArray& aImplementationInfo,
											 const TEComResolverParams& aAdditionalParameters) const
	{
	return aResolver.Resolve(aImplementationInfo, aAdditionalParameters);
	}

/**
The implementation of the abstract Registrar Observer class,
used for recieving notifications of registry changes.
Stub class(for CEComServer) used for the creation of CRegistrar class object.
CEComServer class acts as observer for CRegistrar.
*/
class CTestRegistrarObserver : public MRegistrarObserver // codescanner::missingcclass
	{
public:
	// This function is used by RegistrarObserver (i.e.CEComServer) to notify its
	// clients(REComSession objects) that some change has happened in Registry.
	// Here we have no clients to notify, so no implementaion.
	void Notification( TInt /*aNotification*/ ) {}
	};

/**
Test class encloses necessary members that aid to test CDefaultResolver
*/
class CResolverTest: public CBase
	{
public:
	static CResolverTest* NewL();
	virtual ~CResolverTest();

	void IdentifyImplementationTestL();
	void ListAllTestL();
	void MatchTest();
	void ResolveTestL();

private:
	CResolverTest();
	void ConstructL();

public:
	/** The instance of the class under test */
	CDefaultResolver*	iDefaultResolver;
	/** The data store which is used by the resolver */
	CRegistryData*		iRegistryData;
	/** Friend class pointer used for accessing private members */
	TDefaultResolver_StateAccessor*	iStateAccessor;
	/** An array of potential implementations to resolve between */
	RImplInfoArray		iImplementationData;
	/** ECom example interface Uid */
	TUid				iInterfaceUid;
	/** The Uid returned by IdentifyImplementationL(), used to resolve
	the implementation. */
	TUid				iResolvedImpUid;
	/** Additional parameters used for resolving between implementations */
	TEComResolverParams	iAdditionalParameters;
	/** CRegistrar */
	CRegistrar* iRegistrar;
	/** Registrar observer test class */
	CTestRegistrarObserver* iRegistrarObserver;
	/** CRegistryResolveTransaction */
	CRegistryResolveTransaction* iRegistryResolveTransaction;
	/** ExtendedInterfaces List*/
	RArray<TUid> iExtendedInterfaces;
	/** Client Request*/
	TClientRequest iClientReq;
	};

/**
Create a CResolverTest object on the CleanupStack
@return			A pointer to the newly created class.
*/
CResolverTest* CResolverTest::NewL()
	{
	CResolverTest* self = new (ELeave) CResolverTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Standardized default constructor
@post		CRegistrarTest is fully constructed.
*/
CResolverTest::CResolverTest()
:	CBase()
	{
	// Interface uid to find implemenations
	iInterfaceUid.iUid = KCExampleInterfaceUid.iUid;
	}

/**
Standardized 2nd(Initialization) phase of two phase construction.
Completes the safe construction of the CResolverTest object
@post		CRegistrarTest is fully constructed.
@leave		KErrNoMemory.
*/
void CResolverTest::ConstructL()
	{
	iStateAccessor = new(ELeave) TDefaultResolver_StateAccessor;
	iRegistryData = CRegistryData::NewL(TheFs);
	iRegistrarObserver=new (ELeave) CTestRegistrarObserver;
	// construct the registry resolve transaction object here
	TBool capability= ETrue;
	iRegistryResolveTransaction = CRegistryResolveTransaction::NewL(*iRegistryData,iExtendedInterfaces,iClientReq,capability);
	iRegistrar=CRegistrar::NewL(*iRegistryData, *iRegistrarObserver, TheFs);
	iRegistrar->ProcessSSAEventL(EStartupStateNonCritical);
	// next the default resolver
	iDefaultResolver = CDefaultResolver::NewL(*iRegistryResolveTransaction);
	}

/**
Standardized default destructor
@post		This object is properly destroyed.
*/
CResolverTest::~CResolverTest()
	{
	delete iStateAccessor;
	delete iDefaultResolver;
	delete iRegistrar;
	delete iRegistrarObserver;
	delete iRegistryData;
	delete iRegistryResolveTransaction;
	iExtendedInterfaces.Close();
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0659
@SYMTestCaseDesc	    Create and delete Resolver object test
@SYMTestPriority 	    High
@SYMTestActions  	    Creates and deletes the resolver object
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void CreateDeleteTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0659 CreateDeleteTestL "));
	//
	// Creates and deletes resolver object
	// ------------------------------------------------------------------
	//
	// Set up for heap leak checking
	__UHEAP_MARK;
	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CResolverTest* theTest=NULL;

	TRAPD(err, theTest = CResolverTest::NewL());
	test(err == KErrNone);
	delete theTest;

	// Check for open handles
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-1324
@SYMTestCaseDesc	    Tests for CDefaultResolver::IdentifyImplementationL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests that the resolver identifies most appropriate
						interface implementation
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CResolverTest::IdentifyImplementationTestL()
	{
	// Tests that the resolver identifies most appropriate
	// interface implementation
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1324 "));
	iAdditionalParameters.SetDataType(KResolveMatchType);
	iAdditionalParameters.SetGenericMatch(ETrue);//Allow wildcard matching
	TRAPD(err, iResolvedImpUid =
		iDefaultResolver->IdentifyImplementationL(iInterfaceUid, iAdditionalParameters));
	TEST_OOM_ERR;
	test(err == KErrNone);
	// Check the Appropriate implementation id that should be identified
	test(iResolvedImpUid.iUid == KUidImplementation1);

	test.Printf(_L("Interface Uid 0x%08x\n"), iInterfaceUid);
	test.Printf(_L("Resolved Implementation Uid = 0x%08x\n"), iResolvedImpUid);

	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-1325
@SYMTestCaseDesc	    Tests for CDefaultResolver::ListAllL() function
@SYMTestPriority 	    High
@SYMTestActions  	    The test executes by sending an interface id and data type to match.
						Prints all the implementation for the interface id
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CResolverTest::ListAllTestL()
	{
	// Tests that ListAll() lists implementations for a specified interface
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1325 "));

	// set datatype to text/wml and enable wildcard match
	iAdditionalParameters.SetDataType(KResolveMatchType);
	iAdditionalParameters.SetGenericMatch(ETrue);
	RImplInfoArray*	implData=NULL;

	/* This should list two implementations given below
	depending on additional parameter and wildcard match
	10009DC3	Ver 2/"text/ wml"/10009DB3
	10009DC4	Ver 2/"text/ *"/10009DB3
	*/
	TRAPD(err, implData = iDefaultResolver->ListAllL(iInterfaceUid,	iAdditionalParameters));

	TEST_OOM_ERR;
	test(err == KErrNone);
	CleanupStack::PushL(TCleanupItem(CleanupEComPtrArray, implData));

	const TInt availCount = implData->Count();
	test(availCount == 2);

	test.Printf(_L("Found %d implementations.\n"),availCount);

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = (*implData)[count];

		TDriveName driveName = info->Drive().Name();
		test.Printf(_L("%d. uid={%x} version=%d on drive %S\n"), count+1,
			info->ImplementationUid(), info->Version(), &driveName);

		switch(info->ImplementationUid().iUid)
			{
			case KUidImplementation1:
				test(info->Version()==2);
				test(info->Drive()==EDriveC);
				break;

			case KUidImplementation2:
				test(info->Version()==2);
				test(info->Drive()==EDriveC);
				break;

			default:
				test.Printf(_L("Mismatched implementation Uid\n"));
				test(EFalse);
			}
		}
	// Empty the array of implementations
	CleanupStack::PopAndDestroy();//ifArray, results in a call to CleanupEComPtrArray

	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-1326
@SYMTestCaseDesc	    Tests for TDefaultResolver_StateAccessor::Match() function
@SYMTestPriority 	    High
@SYMTestActions  	    The test executes by searching for a match of a data type on an implementation type
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CResolverTest::MatchTest()
	{
	// 1.Search using search parameters that result in a Match to a particular
	// Implementation.
	// 2.Search using search parameters that result in a mismatch.
	// 3.Finally, search using search parameters with wildcards that result in a Match

	// Searches for KResolveMatchType (text/wml) on implementations
	// KResolveImplementationType(text/wml||data)
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1326 "));
	TBool matchResult = iStateAccessor->Match(*iDefaultResolver,
											  KResolveImplementationType,
										      KResolveMatchType,
										      EFalse);
	test(matchResult);

	_LIT8(KUnResolveableImplType, "Abc||xyz");
	// Pass data "Abc||xyz" which is not present on implementation type
	// to look for and test for failure
	matchResult = iStateAccessor->Match(*iDefaultResolver,
										KUnResolveableImplType,
										KResolveMatchType,
										EFalse);
	test(!matchResult);

	// Wild card in data type
	_LIT8(KResolveWildImplType, "text*||xyz");

	// Set to enable wild card search and test for success
	matchResult = iStateAccessor->Match(*iDefaultResolver,
										KResolveWildImplType,
										KResolveMatchType,
										ETrue);
	test(matchResult);

	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-1327
@SYMTestCaseDesc	    Tests the TDefaultResolver_StateAccessor::Resolve() function
@SYMTestPriority 	    High
@SYMTestActions  	    The test executes by sending an implementation data with additional parameter
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CResolverTest::ResolveTestL()
	{
	// Resolves a appropriate implementation from a list of possibles

	// Create iImplementationType on heap so that address can be passed to
	// CImplementationInformation::NewL method
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1327 "));
	HBufC8* implementationType = HBufC8::NewL(KResolveImplementationType().Length());
	CleanupStack::PushL(implementationType);
	TPtr8 impPtr = implementationType->Des();
	impPtr = KResolveImplementationType;

	// Interface Uid to create an item of implementation data
	iResolvedImpUid.iUid=KCExampleInterfaceImp.iUid;

	TDriveUnit drive(EDriveC);
	CImplementationInformation* impData = CImplementationInformation::NewL(iResolvedImpUid,
																		  0,
																		  NULL,
																		  implementationType,
																		  NULL,
																		  drive,
																		  EFalse,
																		  EFalse);
	// Pop now before pushing impData since ownership is not with it
	CleanupStack::Pop(implementationType);
	CleanupStack::PushL(impData);
	// Add implementation data so that you can resolve the same
	User::LeaveIfError(iImplementationData.Append(impData));
	CleanupStack::Pop(impData);

	// set datatype to text/wml and enable wildcard match
	iAdditionalParameters.SetDataType(KResolveMatchType);
	iAdditionalParameters.SetGenericMatch(ETrue);

	// Newly resolved id
	TUid resolvedUid;
	TRAPD(err, resolvedUid = iStateAccessor->Resolve(*iDefaultResolver,
													 iImplementationData,
													 iAdditionalParameters));
	TEST_OOM_ERR;
	test(err == KErrNone);
	// confirm newly created interface id
	test(resolvedUid == iResolvedImpUid);

	// Logging and cleaning up the array elements
	while(iImplementationData.Count())
		{
		// Fetch the address at first location and empty it
		CImplementationInformation* impInfo = iImplementationData[0];
		test.Printf(_L("Resolved Uid is 0x%x \n"), impInfo->ImplementationUid());
		iImplementationData.Remove(0);
		delete impInfo;
		}
	iImplementationData.Reset();
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0660
@SYMTestCaseDesc	    Tests for OOM while create and delete test
@SYMTestPriority 	    High
@SYMTestActions  	    Calls the CResolverTest test function
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void OOMCreateDeleteTest()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0660 OOM CreateDeleteTest"));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CResolverTest* resolverTest = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt+=100);

		TRAP(err, resolverTest = CResolverTest::NewL());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete resolverTest;
		resolverTest = NULL;

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		}
	while(err == KErrNoMemory);

	test.Printf(_L("- Succeeded at heap failure rate of %i\n"), failAt);
	test(err == KErrNone);
	}

// Type definition for pointer to member function.
// Used in calling the CResolverTest member function for testing.
typedef void (CResolverTest::*ClassFuncPtrL) (void);

/**
This function is used to force a discovery prior to the tests commencing.
*/
LOCAL_C void ForceDiscovery()
    {
	CTestRegistrarObserver* registrarObserver = new (ELeave) CTestRegistrarObserver;
	CleanupStack::PushL(registrarObserver);
	CRegistryData* registryData = CRegistryData::NewL(TheFs);
	CleanupStack::PushL(registryData);
	CRegistrar* registrar = CRegistrar::NewL(*registryData, *registrarObserver, TheFs);
	CleanupStack::PushL(registrar);
	registrar->ProcessSSAEventL(EStartupStateNonCritical);

	CleanupStack::PopAndDestroy(registrar);
	CleanupStack::PopAndDestroy(registryData);
	CleanupStack::PopAndDestroy(registrarObserver);
    }

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0661
@SYMTestCaseDesc	    Wrapper function which calls other test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls the CResolverTest test function
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all test functions

@param		testFunc pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0661 "));
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CResolverTest* resolverTest = CResolverTest::NewL();
	CleanupStack::PushL(resolverTest);

	(resolverTest->*testFuncL)();

	CleanupStack::PopAndDestroy(resolverTest);

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startProcessHandleCount == endProcessHandleCount);
	test(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0662
@SYMTestCaseDesc	    Function to call all OOM test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls the CResolverTest test function
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
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0662 "));
	test.Next(aTestDesc);

	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		CResolverTest* resolverTest = CResolverTest::NewL();
		CleanupStack::PushL(resolverTest);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, (resolverTest->*testFuncL)());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		CleanupStack::PopAndDestroy(resolverTest);
		resolverTest = NULL;
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	test(err == KErrNone);
	test.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

LOCAL_C void DoTestsL()
	{
	__UHEAP_MARK;

	// Force a discovery to ensure that the ecom.dat file is updated with the data from the plugins
	// required for this test. The plugins for this test are copied by calling CopyPlugins() from E32Main().
	// They are deleted by calling DeleteTestPlugin() also from E32Main().
	ForceDiscovery();

	// Basic tests
	CreateDeleteTestL();
	DoBasicTestL(&CResolverTest::IdentifyImplementationTestL, _L("Identify implementation test"));
	DoBasicTestL(&CResolverTest::ListAllTestL, _L("ListAllTestL"));
	DoBasicTestL(&CResolverTest::MatchTest, _L("MatchTest"));
	DoBasicTestL(&CResolverTest::ResolveTestL, _L("ResolveTestL"));

	// OOM tests
	OOMCreateDeleteTest();
	DoOOMTestL(&CResolverTest::IdentifyImplementationTestL, _L("OOM IdentifyImplementationTestL"));
	DoOOMTestL(&CResolverTest::ListAllTestL, _L("OOM ListAllTestL"));
	DoOOMTestL(&CResolverTest::MatchTest, _L("OOM MatchTest"));
	DoOOMTestL(&CResolverTest::ResolveTestL, _L("OOM ResolveTestL"));

	__UHEAP_MARKEND;
	}

/**
This function is used for cleanup support of locally declared arrays
*/
LOCAL_C void CleanupEComPtrArray(TAny* aArray)
	{

	(static_cast<RImplInfoArray*>(aArray))->Reset();
	delete aArray;// delete here

	}

/**
Copies the Plugins to specific folder for testing purpose
*/
LOCAL_C void CopyPlugins()
    {
	// Copy the dlls and .rsc files on to RAM
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExDllOnZ, KEComExDllOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExampleDllOnZ, KEComExampleDllOnC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample2DllOnZ, KEComExample2DllOnC));
	test(err==KErrNone);
 	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample3DllOnZ, KEComExample3DllOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExampleRscOnZ, KEComExampleRscOnC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample2RscOnZ, KEComExample2RscOnC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample3RscOnZ, KEComExample3RscOnC));
	test(err==KErrNone);
	}

// Deleting plugin from the RAM for cleanup purpose
inline LOCAL_C void DeleteTestPlugin()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComExDllOnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComRscFileOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExampleDllOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample2DllOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample3DllOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExampleRscOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample2RscOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample3RscOnC));
	}

//Initialise the Active Scheduler
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

	test.Printf(_L("\n"));
	test.Title();
	test.Start(_L("Resolver Tests"));

	TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, SetupL());
	test(err == KErrNone);

	// Connect the file server instance
	User::LeaveIfError(TheFs.Connect());

	CopyPlugins();

	// Wait, so that ECom server looks for plugins copied from Z: to C drive
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that CDiscoverer::CIdleScanningTimer
	// which is an active object. So the discovering service is asynchronous. We have to
	// wait some time until it finishes. Otherwise ListImplementationsL could fail to find
	// requested implementations.
	User::After(KOneSecond * 3);

	TRAP(err, DoTestsL());
	test(err == KErrNone);

	// Cleanup files. If the cleanup fails that is no problem,
	// as any subsequent tests will replace them. The only downside
	// would be the disk not being tidied
	DeleteTestPlugin();

	TheFs.Close();
	delete TheTrapCleanup;
	delete TheActiveScheduler;
	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(KErrNone);
	}
