// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// t_listimplementations.cpp
// 
//

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
#include "../EcomTestUtils/EcomTestUtils.h"

//The test doesn't test all overloaded CreateImplementationL methods,
//because all test ECOM plugins have NewL method with an additional argument.
//Only overloaded CreateImplementationL methods with an additional argument are tested.

// Suicide.dll name to pass when loading it
//_LIT8(KSuicideStr,"suicidal");

/**
 Plugins available to be used in this test
 Interface UID	DLL	UID		Implementation UID	Version		DllFile
 -------------------------------------------------------------------
 0x10009DC0		0x10009DB1	0x10009DC3			1		Z:\\..\\EComExample.dll
 				0x10009DB1	0x10009DC4			1		Z:\\..\\EComExample.dll
 				0x10009DB3	0x10009DC3			2		Z:\\..\\EComExample2.dll
 				0x10009DB3	0x10009DC4			2		Z:\\..\\EComExample2.dll
 				0x101F8477	0x101F8478			1		Z:\\..\\EComExample3.dll
 				0x101F8479  0x101F847A			1		Z:\\..\\EComExample4.dll
 				0x101F847B	0x101F847C			1		C:\\..\\EComExample5.dll

 0x10009DC0		0x101F847D	0x101F847E 			1		Z:\\..\\EComExample6.dll

 0x10009DBB		0x10009DBA	0x10009DBC          1		Z:\\..\\EComExample7.dll

 0x10009DBC		0x10009DBB  0x10009DBD			1		Z:\\..\\EComExample8.dll

 0x01aa500b		0x011f500e  0x01aa500c          2  		Z:\\..\\ecomnullexample.dll

 0x10009DDB		0x10009E4A  0x10009DDC			1		Z:\\..\\EComExample16.dll


*/

TUid KCExampleImplUid = {0x10009DC3};
TUid KCExampleImplUid2 = {0x01aa500c};
TUid KCExampleImplUid3 = {0x10009DC4};
TUid KCExampleImplUid4 = {0x101F8478};
TUid KCExampleImplUid5 = {0x101F847A};
TUid KCExampleImplUid6 = {0x101F847C};
TUid KCExampleImplUid7 = {0x101F847E};
TUid KCExampleImplUid8 = {0x10009DBC};
TUid KCExampleImplUid9 = {0x10009DBD};
TUid KCExampleImplUid16 = {0x10009DDC};

const TInt KOneSecond = 1000000;

LOCAL_D RTest test(_L("t_listimplementations.exe"));
LOCAL_D RFs TheFs;

_LIT(KEComPlugin2OnZ, "z:\\RAMOnly\\EComExample5.dll");

_LIT(KEComPlugin2OnC, "c:\\sys\\bin\\EComExample5.dll");

_LIT(KEComPluginRsc2OnZ, "z:\\RAMOnly\\EComExample5.rsc");
_LIT(KEComPluginRsc2OnC, "c:\\resource\\plugins\\EComExample5.rsc");

_LIT(KEComEx8DllOnZ,		"Z:\\RAMOnly\\EComExample8.dll");
_LIT(KEComEx8DllOnC,		"C:\\sys\\bin\\EComExample8.dll");
_LIT(KEComEx8RscFileOnC,	"C:\\resource\\plugins\\EComExample8.rsc");
_LIT(KEComEx8RscFileOnZ,	"Z:\\RAMOnly\\EComExample8.rsc");

_LIT(KEComEx7DllOnZ,		"Z:\\RAMOnly\\EComExample7.dll");
_LIT(KEComEx7DllOnC,		"C:\\sys\\bin\\EComExample7.dll");
_LIT(KEComEx7RscFileOnC,	"C:\\resource\\plugins\\EComExample7.rsc");
_LIT(KEComEx7RscFileOnZ,	"Z:\\RAMOnly\\EComExample7.rsc");


#define __UNUSED_VAR(var) var = var


class TheTest
	{
public:
	void StartTestL();
	};

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-4007
@SYMTestCaseDesc		Call CreateImplementationL() to load 1 plugin. And
 						Call DestroyedImplementation()with invalid destory id key
 						Call REComSession::FinalClose()
 						Call DestroyedImplementation() to the loaded plugin.
@SYMTestPriority		High
@SYMTestActions			Call CreateImplementationL() to load 1 plugin. And
 						Call DestroyedImplementation()with invalid destory id key
 						Call REComSession::FinalClose()
 						Call DestroyedImplementation() to the loaded plugin.
@SYMTestExpectedResults	Returns success
 						leaves with the appropriate error.
@SYMDEF					PDEF116654
*/
 LOCAL_C void Test2PDEF116654L()
 	{
 	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4007 Test2PDEF116654L "));

 	// Set up for heap leak checking
 	__UHEAP_MARK;

 	TInt err1;
 	// and leaking thread handles
 	TInt startProcessHandleCount;
 	TInt startThreadHandleCount;
 	TInt endProcessHandleCount;
 	TInt endThreadHandleCount;

 	TInt failAt = 1;
 	__UNUSED_VAR(failAt);

 	// Test Starts...L188
 	RThread thisThread;
 	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

 	CExampleInterface* dllPtr1 = NULL;

 	// Set up some empty initialisation parameters
 	CExampleInterface::TExampleInterfaceInitParams initParams;
 	initParams.integer		= 0;
 	initParams.descriptor	= NULL;

 	TUid iDtor_ID_Key1;

 	test.Printf(_L("Loading 2 Plugins..."));
 	TRAP(err1,dllPtr1 = REINTERPRET_CAST(CExampleInterface*,
 			REComSession::CreateImplementationL(KCExampleImplUid,
 							   					iDtor_ID_Key1,
 							   					&initParams)));
 	//give a false iDtor_ID_Key
 	REComSession::DestroyedImplementation(KCExampleImplUid3);

 	REComSession::FinalClose();

 	// release handles
 	if(err1 == KErrNone)
 		{
 		REComSession::DestroyedImplementation(iDtor_ID_Key1);
 		delete dllPtr1;
 		REComSession::FinalClose();
 		}

 	// check that no handles have leaked
 	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

 	test(startThreadHandleCount == endThreadHandleCount);
 	test.Printf(_L("Great ! No handle mismatch."));

 	// Test Ends...
 	__UHEAP_MARKEND;
 	}

 /**
@SYMTestCaseID			SYSLIB-ECOM-UT-4008
@SYMTestCaseDesc		if CreateImplementationL() leaves, DestroyedImplementation() still works.
						Prove iDoNotCloseThis of EComSession is no use any more.
@SYMTestPriority		High
@SYMTestActions			Call CreateImplementationL() to load 1st plugin success.
						Call CreateImplementationL() to load 2nd plugin leaves.
						Call DestroyedImplementation() to destroy 1st plugin.
@SYMTestExpectedResults	Returns success
						leaves with the appropriate error.
@SYMDEF					PDEF116654

*/
LOCAL_C void Test1PDEF116654L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4008 Test1DEF115714L "));

	// Set up for heap leak checking
	__UHEAP_MARK;

	TInt err1, err2;
	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	// Test Starts...L188
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	CExampleInterface* dllPtr1 = NULL;
	CExampleInterface* dllPtr2 = NULL;
	TUid iDtor_ID_Key1;
	TUid iDtor_ID_Key2;

	TUid resolverUid = {0x10009DD0};

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	test.Printf(_L("Loading 2 Plugins..."));
	TRAP(err1,dllPtr1 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(KCExampleInterfaceUid,
															   iDtor_ID_Key1,
															   &initParams,
															   ResolverParams,
															   resolverUid)));


	TRAP(err2,dllPtr2 = REINTERPRET_CAST(CExampleInterface*,
		REComSession::CreateImplementationL(KCExampleImplUid16,
						   					iDtor_ID_Key2,
						   					&initParams)));

	// release handles
	if(err1 == KErrNone)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key1);
		delete dllPtr1;
		REComSession::FinalClose();
		}
	if(err2 == KErrNone)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key2);
		delete dllPtr2;
		REComSession::FinalClose();
		}

	REComSession::DestroyedImplementation(iDtor_ID_Key2);
	// check that no handles have leaked
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...
	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID			SYSLIB-ECOM-UT-4005
@SYMTestCaseDesc		Call CreateImplementationL() to load plugins.
						Call GetImplementationUidL() to retrieve implementation uid given the destructor key.
						Creating 100 Plugins using the same ImplementationUid...
						List implementation uid from interface uid
						Call DestroyedImplementation()
@SYMTestPriority			High
@SYMTestActions			Call CreateImplementationL() to load no.0 to no.4 plugins.
						Call GetImplementationUidL() and check impl id.
						Call DestroyedImplementation() to destroy the no.0 to no.1 Plugins
						Call CreateImplementationL() to load no.5 to no.8 Plugins
						Call GetImplementationUidL() and check impl id.
						Call CreateImplementationL() to load 100 plugins
						Call GetImplementationUidL() and check impl ids.
						Call ListImplementationsL()
						Call DestroyedImplementation() to destroy the no.2 to no.4 Plugins
						Call GetImplementationUidL() and check impl id.
						Call DestroyedImplementation() to destroy the no.5 to no.8 Plugins
@SYMTestExpectedResults	Returns success
						leaves with the appropriate error.
@SYMDEF					PDEF116654

*/

LOCAL_C void Test20L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4005 Test20L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	TFixedArray<TUid,9> test19LPlugIns;
	test19LPlugIns[0] = KCExampleImplUid;
	test19LPlugIns[1] = KCExampleImplUid3;
	test19LPlugIns[2] = KCExampleImplUid4;
	test19LPlugIns[3] = KCExampleImplUid5;
	test19LPlugIns[4] = KCExampleImplUid6;
	test19LPlugIns[5] = KCExampleImplUid7;
	test19LPlugIns[6] = KCExampleImplUid8;
	test19LPlugIns[7] = KCExampleImplUid9;
	test19LPlugIns[8] = KCExampleImplUid2;

	TFixedArray<CExampleInterface*,9> dllPtr;
	TFixedArray<CExampleInterface*,100> dllPtr1;
	TUid implUid;

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	TFixedArray<TUid,9> iDtor_ID_Key;
	TFixedArray<TUid,100> iDtor_ID_Key1;

	test.Printf(_L("Loading 5 Plugins..."));
	for(TInt i=0; i<5; i++)
		{
		dllPtr[i] = REINTERPRET_CAST(CExampleInterface*,
				REComSession::CreateImplementationL(test19LPlugIns[i],
						   iDtor_ID_Key[i],
						   &initParams));
		test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr[i]->ImplId(), test19LPlugIns[i]);
		test(test19LPlugIns[i]==dllPtr[i]->ImplId());
		}

	test.Printf(_L("Getting implementation uid from detructor key..."));
	for(TInt i=0; i<5; i++)
		{
		implUid = REComSession::GetImplementationUidL(iDtor_ID_Key[i]);
		// checking returned impl uid. For current implementation of Load
		// manager the returned uid is the same as the destructor key. This
		// will change with the implementation of EC043. The destructor key
		// will become an arbitrary value.
		test(implUid == test19LPlugIns[i]);
		}
	test.Printf(_L("Destroying the first 2 Plugins..."));
	for(TInt i=0; i<2; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key[i]);
		delete dllPtr[i];
		}

	test.Printf(_L("Loading another 4 Plugins..."));
	for(TInt i=5; i<8; i++)
		{
		dllPtr[i] = REINTERPRET_CAST(CExampleInterface*,
				REComSession::CreateImplementationL(test19LPlugIns[i],
						   iDtor_ID_Key[i],
						   &initParams));
		test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr[i]->ImplId(), test19LPlugIns[i]);
		test(test19LPlugIns[i]==dllPtr[i]->ImplId());
		}

	test.Printf(_L("Creating 100 Plugins using the same ImplementationUid..."));
	for(TInt i=0; i<100; i++)
		{
		dllPtr1[i] = REINTERPRET_CAST(CExampleInterface*,
			REComSession::CreateImplementationL(test19LPlugIns[8],
					   iDtor_ID_Key1[i],
					   NULL));
		test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr1[i]->ImplId(), test19LPlugIns[8]);
		test(dllPtr1[i]->ImplId()==test19LPlugIns[8]);
		}

	dllPtr[8] = REINTERPRET_CAST(CExampleInterface*,
			REComSession::CreateImplementationL(test19LPlugIns[8],
					   iDtor_ID_Key[8],
					   NULL));
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr[8]->ImplId(), test19LPlugIns[8]);
	test(test19LPlugIns[8]==dllPtr[8]->ImplId());

	test.Printf(_L("Getting implementation uid from detructor key..."));
	for(TInt i=5; i<=8; i++)
		{
		implUid = REComSession::GetImplementationUidL(iDtor_ID_Key[i]);
		// checking returned impl uid. For current implementation of Load
		// manager the returned uid is the same as the destructor key. This
		// will change with the implementation of EC043. The destructor key
		// will become an arbitrary value.
		test(implUid == test19LPlugIns[i]);
		}
	test.Printf(_L("List implementation uid from interface uid..."));
	RImplInfoPtrArray ifArray;
	TUid ifUid = {0x10009DC0};
	_LIT8(KImplementationTest,"text/wml");
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParamsList;
	ResolverParamsList.SetDataType(KImplementationTest());
	ResolverParamsList.SetGenericMatch(ETrue);	// Allow wildcard matching
	REComSession::ListImplementationsL(
			ifUid,
			ResolverParamsList,
			ifArray);
	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 2);
	ifArray.ResetAndDestroy();

	test.Printf(_L("Destroying the 2-4 Plugins..."));
	for(TInt i=2; i<5; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key[i]);
		delete dllPtr[i];
		}

	test.Printf(_L("Destroying 100 Plugins using the same ImplementationUid..."));
	for(TInt i=0; i<100; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key1[i]);
		delete dllPtr1[i];
		}

	test.Printf(_L("Getting implementation uid from detructor key..."));
	for(TInt i=5; i<9; i++)
		{
		implUid = REComSession::GetImplementationUidL(iDtor_ID_Key[i]);
		// checking returned impl uid. For current implementation of Load
		// manager the returned uid is the same as the destructor key. This
		// will change with the implementation of EC043. The destructor key
		// will become an arbitrary value.
		test(implUid == test19LPlugIns[i]);
		}

	test.Printf(_L("Destroying the 5-9 Plugins..."));
	for(TInt i=5; i<9; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key[i]);
		delete dllPtr[i];
		}
	REComSession::FinalClose();
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}
/**
@SYMTestCaseID			SYSLIB-ECOM-UT-4004
@SYMTestCaseDesc		Call CreateImplementationL() to load >8 (9)plugins.
						Call GetImplementationUidL() to retrieve implementation uid given the destructor key.
						List implementation uid from interface uid
						Creating 100 Plugins using the same ImplementationUid
						Call DestroyedImplementation()
						Destroying 100 Plugins using the same ImplementationUid
@SYMTestPriority			High
@SYMTestActions			Call CreateImplementationL() to load 9 plugins.
						Call GetImplementationUidL() and check impl id.
						Call ListImplementationsL()
						Call CreateImplementationL() to load 100 plugins.
						Call DestroyedImplementation() to destroy plugins.
@SYMTestExpectedResults	Returns success
						leaves with the appropriate error.
@SYMDEF					PDEF116654

*/
LOCAL_C void Test19L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4004 Test19L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	TFixedArray<TUid,9> test19LPlugIns;
	test19LPlugIns[0] = KCExampleImplUid;
	test19LPlugIns[1] = KCExampleImplUid3;
	test19LPlugIns[2] = KCExampleImplUid4;
	test19LPlugIns[3] = KCExampleImplUid5;
	test19LPlugIns[4] = KCExampleImplUid6;
	test19LPlugIns[5] = KCExampleImplUid7;
	test19LPlugIns[6] = KCExampleImplUid8;
	test19LPlugIns[7] = KCExampleImplUid9;
	test19LPlugIns[8] = KCExampleImplUid2;

	TFixedArray<CExampleInterface*,9> dllPtr;
	TFixedArray<CExampleInterface*,100> dllPtr1;
	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// Set up the interface find for the default resolver.
	TFixedArray<TUid,9> iDtor_ID_Key;
	TFixedArray<TUid,100> iDtor_ID_Key1;
	test.Printf(_L("Loading Plugin..."));
	for(TInt i=0; i<8; i++)
		{
		dllPtr[i] = REINTERPRET_CAST(CExampleInterface*,
				REComSession::CreateImplementationL(test19LPlugIns[i],
						   iDtor_ID_Key[i],
						   &initParams));
		test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr[i]->ImplId(), test19LPlugIns[i]);
		test(test19LPlugIns[i]==dllPtr[i]->ImplId());
		}

	dllPtr[8] = REINTERPRET_CAST(CExampleInterface*,
			REComSession::CreateImplementationL(test19LPlugIns[8],
					   iDtor_ID_Key[8],
					   NULL));
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr[8]->ImplId(), test19LPlugIns[8]);
	test(test19LPlugIns[8]==dllPtr[8]->ImplId());

	test.Printf(_L("Getting implementation uid from detructor key..."));
	for(TInt i=0; i<=8; i++)
		{
		TUid implUid = REComSession::GetImplementationUidL(iDtor_ID_Key[i]);
		// checking returned impl uid. For current implementation of Load
		// manager the returned uid is the same as the destructor key. This
		// will change with the implementation of EC043. The destructor key
		// will become an arbitrary value.
		test(implUid == test19LPlugIns[i]);
		}

	test.Printf(_L("List implementation uid from interface uid..."));
	RImplInfoPtrArray ifArray;
	TUid ifUid = {0x10009DC0};
	_LIT8(KImplementationTest,"text/wml");
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParamsList;
	ResolverParamsList.SetDataType(KImplementationTest());
	ResolverParamsList.SetGenericMatch(ETrue);	// Allow wildcard matching
	REComSession::ListImplementationsL(
			ifUid,
			ResolverParamsList,
			ifArray);
	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 2);
	ifArray.ResetAndDestroy();

	test.Printf(_L("Creating 100 Plugins using the same ImplementationUid..."));
	for(TInt i=0; i<100; i++)
		{
		dllPtr1[i] = REINTERPRET_CAST(CExampleInterface*,
			REComSession::CreateImplementationL(test19LPlugIns[8],
					   iDtor_ID_Key1[i],
					   NULL));
		test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr1[i]->ImplId(), test19LPlugIns[8]);
		test(dllPtr1[i]->ImplId()==test19LPlugIns[8]);
		}


	test.Printf(_L("Destroying Plugin..."));
	for(TInt i=0; i<9; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key[i]);
		delete dllPtr[i];
		REComSession::FinalClose();
		}

	test.Printf(_L("Destroying 100 Plugins using the same ImplementationUid..."));
	for(TInt i=0; i<100; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key1[i]);
		delete dllPtr1[i];
		REComSession::FinalClose();
		}
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}




/**
@SYMTestCaseID			SYSLIB-ECOM-CT-3708-0001
@SYMTestCaseDesc		Call CreateImplementationL() to load a plugin.
						Call GetImplementationUidL() at various places to retrieve
						implementation uid given the destructor key.
@SYMTestPriority			High
@SYMTestActions			Call CreateImplementationL() to load a plugin.
						Call GetImplementationUidL() and check impl id.
						Call GetImplementationUidL() and check impl id again.
						Destroy plugin.
						Call GetImplementationUidL() and check for error.
						Call FinalClose()
						Call GetImplementationUidL() and check for error.
						Call GetImplementationUidL() with null value and check for error.
@SYMTestExpectedResults	Returns impl uid when valid destructor uid is used otherwise
						leaves with the appropriate error.
@SYMDEF					DEF111789
*/
LOCAL_C void Test18L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3708-0001 Test18L "));

	//
	// Testing GetImplmentationUidL
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
	TUid instanceKey;
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	TUid resolverUid = {0x10009DD0};

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	test.Printf(_L("Loading Plugin..."));
	CExampleInterface* dllPtr = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(KCExampleInterfaceUid,
																instanceKey,
																&initParams,
																ResolverParams,
																resolverUid));

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Getting implementation uid from detructor key..."));
	TUid implUid = REComSession::GetImplementationUidL(instanceKey);
	// checking returned impl uid. For current implementation of Load
	// manager the returned uid is the same as the destructor key. This
	// will change with the implementation of EC043. The destructor key
	// will become an arbitrary value.
	test(implUid == KCExampleImplUid);

	// now test again...
	implUid = REComSession::GetImplementationUidL(instanceKey);
	test(implUid == KCExampleImplUid);

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(instanceKey);
	delete dllPtr;

	// test getImplementationUidL() after instance deleted
	// Expecting a leave
	TRAPD(err, implUid = REComSession::GetImplementationUidL(instanceKey));
	test(err == KErrNotFound);

	REComSession::FinalClose(); // Don't want leaks outside the test

	// test getImplementationUidL() with null uid
	TRAP(err, implUid = REComSession::GetImplementationUidL(KNullUid));
	test(err == KErrArgument);

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-1665
@SYMTestCaseDesc	    Call ListImplementationsL using the interface UID to get the implementations.
						go through the list to get the VID.
@SYMTestPriority 	    High
@SYMTestActions  	    Checks that the VID of the implementation match the VID of the DLL
						which the implementation belongs to.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ6111
*/
LOCAL_C void Test17L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1665 Test17L "));

	//
	// Testing the implementation VID feature using ListImplementationsL
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

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	TUid interfaceUid1={0x10009DBC};
	RImplInfoPtrArray implArray;
	// List the implementation which VID is 0.
	REComSession::ListImplementationsL(interfaceUid1, implArray);

	test(implArray.Count()==1);
	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();
	TVendorId vid = implArray[0]->VendorId();
	// imp. uid
	test(implUid.iUid == 0x10009DBD);
	// version
	test(version == 1);
	// C drive
	test(drive == EDriveC);
	// VID
	test(vid.iId == 0x00000000);
	implArray.ResetAndDestroy();

	TUid interfaceUid2={0x10009DBB};

	// List the implementation which VID is 0x70000001.
	REComSession::ListImplementationsL(interfaceUid2, implArray);
	test(implArray.Count()==4);
	//Check that 0x10009DBC implementation uid returned matched the specs above

	// Find implementation 0x10009DBC, and check validity
	for (TInt i=0; i < implArray.Count(); i++)
	{
		if (implArray[i]->ImplementationUid().iUid == 0x10009DBC)
			{
			TInt version2 = implArray[i]->Version();
			TInt drive2 = implArray[i]->Drive();
			TVendorId vid2 = implArray[i]->VendorId();
			// version
			test(version2 == 1);
			// C drive
			test(drive2 == EDriveC);
			// VID
			test(vid2.iId == 0x70000001);
			break;
			}
	}

	//clean up
	implArray.ResetAndDestroy();
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0666
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL with no resolver parameters.
@SYMTestPriority 	    High
@SYMTestActions  	    Checks that the 6 correct implementations are chosen from plug-ins
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test16L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0666 Test16L "));

	//
	// Testing a overloaded method for ListImplementationsL
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

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	RImplInfoPtrArray ifArray;

	TUid ifUid = {0x10009DC0};
	TUid resolverUid = {0x10009D92}; //RomOnlyResolverUid

	// Set up the interface find for the default resolver.
	TEComResolverParams noCriteriaSet;

	TRAPD(err,REComSession::ListImplementationsL(
			ifUid,
			noCriteriaSet,
			resolverUid,
			ifArray));
	test(err==KErrNone);

	// There should be a total of 6 implementations for this interface
	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 6);

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("%d. %s\n"),count,((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0667
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an resolver Uid
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded.Destroy the created interface implementation to ECOM.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test15L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0667 Test15L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	TUid iDtor_ID_Key;
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	TUid resolverUid = {0x10009DD0};

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	test.Printf(_L("Loading Plugin..."));
	CExampleInterface* dllPtr = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(KCExampleInterfaceUid,
															   iDtor_ID_Key,
															   &initParams,
															   ResolverParams,
															   resolverUid));

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0668
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an resolver Uid
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded.Destroy the created interface implementation to ECOM.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test14L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0668 Test14L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	TUid iDtor_ID_Key;
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	TUid resolverUid = {0x10009DD0};

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	test.Printf(_L("Loading Plugin..."));
	CExampleInterface* dllPtr = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(KCExampleInterfaceUid,
															   iDtor_ID_Key,
															   &initParams,
															   ResolverParams,
															   resolverUid));

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());



	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0669
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an Interface Uid
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded.Destroy the created interface implementation to ECOM.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test13L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0669 Test13L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	TUid iDtor_ID_Key;
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	test.Printf(_L("Loading Plugin..."));
	CExampleInterface* dllPtr = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(KCExampleInterfaceUid,
															   iDtor_ID_Key,
															   &initParams,
															   ResolverParams));

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0670
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an Interface Uid
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded.Destroy the created interface implementation to ECOM.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test12L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0670 Test12L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	TUid iDtor_ID_Key;
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	test.Printf(_L("Loading Plugin..."));
	CExampleInterface* dllPtr = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(KCExampleInterfaceUid,
															   iDtor_ID_Key,
															   &initParams,
															   ResolverParams));

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0671
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an Implementation Uid
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded.Destroy the created interface implementation to ECOM.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test11L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0671 Test11L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	TUid iDtor_ID_Key;
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	test.Printf(_L("Loading Plugin..."));
	CExampleInterface* dllPtr = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(KCExampleImplUid,
															   iDtor_ID_Key,
															   &initParams));

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0672
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded.Destroy the created interface implementation to ECOM.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test10L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0672 Test10L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	TUid iDtor_ID_Key;
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	test.Printf(_L("Loading Plugin..."));

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	CExampleInterface* dllPtr = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(KCExampleImplUid,
															   iDtor_ID_Key,
															   &initParams));

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0673
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL()
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test9L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0673 Test9L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);


	test.Printf(_L("Loading Plugin..."));

	CExampleInterface* dllPtr = CExampleInterface::NewL();

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0674
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with initial parameter passed as an argument
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test8L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0674 Test8L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);


	test.Printf(_L("Loading Plugin..."));

	CExampleInterface* dllPtr = CExampleInterface::NewL(1,KNullDesC);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0675
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with resolver UID
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test7L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0675 Test7L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	test.Printf(_L("Loading Plugin..."));

	CExampleInterface* dllPtr = CExampleInterface::NewL4();

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0676
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with default resolver
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test6L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0676 Test6L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);


	test.Printf(_L("Loading Plugin..."));

	CExampleInterface* dllPtr = CExampleInterface::NewL3();

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0677
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL created with an implementation UID
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test5L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0677 Test5L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);


	test.Printf(_L("Loading Plugin..."));

	CExampleInterface* dllPtr = CExampleInterface::NewL2(KCExampleImplUid);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0678
@SYMTestCaseDesc	    Tests for REComSession::CreateImplementationL()
@SYMTestPriority 	    High
@SYMTestActions  	    Check that the correct plugin is loaded
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test4L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0678 Test4L "));

	//
	// Testing a overloaded method for CreateImplementationL
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
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	test.Printf(_L("Loading Plugin..."));

	CExampleInterface* dllPtr = CExampleInterface::NewL2(KCExampleImplUid);

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid);
	test(KCExampleImplUid==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0679
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL for known Interface UID
@SYMTestPriority 	    High
@SYMTestActions  	    Checks that the correct implementations are chosen from two plug-ins
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test3L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0679 Test3L "));

	//
	// Testing a overloaded method for ListImplementationsL
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

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	RImplInfoPtrArray ifArray;

	TUid ifUid = {0x10009DC0};

	REComSession::ListImplementationsL(
			ifUid,
			ifArray);

	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 7);

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("%d. %s\n"),count,((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0680
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL for know Interface UID and resolver match characteristics
@SYMTestPriority 	    High
@SYMTestActions  	    Checks that the correct implementations are chosen from two plug-ins
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0680 Test2L "));

	//
	// Testing a overloaded method for ListImplementationsL
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

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	RImplInfoPtrArray ifArray;

	TUid ifUid = {0x10009DC0};
	_LIT8(KImplementationTest,"text/wml");

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);	// Allow wildcard matching

	REComSession::ListImplementationsL(
			ifUid,
			ResolverParams,
			ifArray);

	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 2);

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("%d. %s\n"),count,((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0681
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL
@SYMTestPriority 	    High
@SYMTestActions  	    Checks that the correct implementations are chosen from two plug-ins
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void Test1L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0681 Test1L "));

	//
	// Testing a overloaded method for ListImplementationsL
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

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	RImplInfoPtrArray ifArray;

	TUid ifUid = {0x10009DC0};
	TUid resolverUid = {0x10009DD0};
	_LIT8(KImplementationTest,"text/wml");

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);	// Allow wildcard matching

	REComSession::ListImplementationsL(
			ifUid,
			ResolverParams,
			resolverUid,
			ifArray);

	// There should be 6 implementations found but only 2 returned.
	// These 2, are the only two that match the datatype supplied.
	// These 2, are also 2 of a posible 4, i.e. version 2.
	// The version 1 implementations are not part of the reported 6
	// they are superseeded.
	// So the 2 that match are implementation uids 0x10009DC3 & 0x10009DC4
	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 2);

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];
		test.Printf(_L("%d. %s\n"),count,((info->DisplayName()).Ptr()));
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0682
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::ListImplementationsL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM1()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0682 TestOOM1 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	RImplInfoPtrArray ifArray;

	TUid ifUid = {0x10009DC0};
	TUid resolverUid = {0x10009DD0};

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);	// Allow wildcard matching

 	do
		{
		__UHEAP_MARK;

		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, REComSession::ListImplementationsL(
													ifUid,
													ResolverParams,
													resolverUid,
													ifArray));
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);

		if(err == KErrNone)
			{
			// Empty the array of implementations to avoid memory leak
			test.Printf(_L("Destroying List..."));
			ifArray.ResetAndDestroy();
			}

		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);
	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0683
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::ListImplementationsL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM2()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0683 TestOOM2 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	RImplInfoPtrArray ifArray;

	TUid ifUid = {0x10009DC0};

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);	// Allow wildcard matching

	do
		{
		__UHEAP_MARK;

		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, REComSession::ListImplementationsL(
												ifUid,
												ResolverParams,
												ifArray));
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);

		if(err == KErrNone)
			{
			// Empty the array of implementations to avoid memory leak
			test.Printf(_L("Destroying List..."));
			ifArray.ResetAndDestroy();
			}
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);
	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0684
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::ListImplementationsL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM3()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0684 TestOOM3 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	RImplInfoPtrArray ifArray;
	TUid ifUid = {0x10009DC0};

	do
		{
		  __UHEAP_MARK;
		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, REComSession::ListImplementationsL(
								ifUid,
								ifArray));
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		if(err == KErrNone)
			{
			// Empty the array of implementations to avoid memory leak
			test.Printf(_L("Destroying List..."));
			ifArray.ResetAndDestroy();
			}
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);
	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0685
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::ListImplementationsL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM16()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0685 TestOOM16 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	RImplInfoPtrArray ifArray;
	TUid ifUid = {0x10009DC0};

	do
		{
		  __UHEAP_MARK;
		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, REComSession::ListImplementationsL(
								ifUid,
								ifArray));
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		if(err == KErrNone)
			{
			// Empty the array of implementations to avoid memory leak
			test.Printf(_L("Destroying List..."));
			ifArray.ResetAndDestroy();
			}

		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);
	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0686
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::ListImplementationsL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM17()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0686 TestOOM17 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	RImplInfoPtrArray ifArray;
	TUid ifUid = {0x10009DC0};

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, REComSession::ListImplementationsL(
									ifUid,
									ifArray));
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		if(err == KErrNone)
			{
			// Empty the array of implementations to avoid memory leak
			test.Printf(_L("Destroying List..."));
			ifArray.ResetAndDestroy();
			}

		__UHEAP_MARKEND;
	}
	while (err == KErrNoMemory);
	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0687
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::CreateImplementationL created with an implementation UID
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM4()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0687 TestOOM4 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = CExampleInterface::NewL2(KCExampleImplUid));

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0688
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::CreateImplementationL created with implementation UID
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM5()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0688 TestOOM5 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = CExampleInterface::NewL2(KCExampleImplUid));

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0689
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::CreateImplementationL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM6()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0689 TestOOM6 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = CExampleInterface::NewL3());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0690
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::CreateImplementationL created with a resolver UID
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM7()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0690 TestOOM7 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = CExampleInterface::NewL4());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0691
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::CreateImplementationL created with a parameter descriptor
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM8()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0691 TestOOM8 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
	  	// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = CExampleInterface::NewL(1,KNullDesC));

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0692
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::CreateImplementationL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM9()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0692 TestOOM9 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = CExampleInterface::NewL());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0693
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::CreateImplementationL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM10()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0693 TestOOM10 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);
	TUid iDtor_ID_Key = KNullUid;

	CExampleInterface* dllPtr = NULL;

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = REINTERPRET_CAST(CExampleInterface*,
								REComSession::CreateImplementationL(KCExampleImplUid,
																   iDtor_ID_Key,
																   &initParams)));

		__UHEAP_SETFAIL(RHeap::ENone, 0);
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0694
@SYMTestCaseDesc	    Tests for out of memory error for REComSession::CreateImplementationL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM11()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0694 TestOOM11 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);
	TUid iDtor_ID_Key = KNullUid;

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = REINTERPRET_CAST(CExampleInterface*,
								REComSession::CreateImplementationL(KCExampleImplUid,
																   iDtor_ID_Key,
																   &initParams)));

		__UHEAP_SETFAIL(RHeap::ENone, 0);
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}
		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0695
@SYMTestCaseDesc	    Tests for out of memory error while creating REComSession::CreateImplementationL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM12()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0695 TestOOM12 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);
	TUid iDtor_ID_Key = KNullUid;

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = REINTERPRET_CAST(CExampleInterface*,
								REComSession::CreateImplementationL(KCExampleInterfaceUid,
																   iDtor_ID_Key,
																   &initParams,
																   ResolverParams)));

		__UHEAP_SETFAIL(RHeap::ENone, 0);
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}
		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0696
@SYMTestCaseDesc	    Tests for out of memory error while creating REComSession::CreateImplementationL
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM13()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0696 TestOOM13 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);
	TUid iDtor_ID_Key = KNullUid;

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = REINTERPRET_CAST(CExampleInterface*,
								REComSession::CreateImplementationL(KCExampleInterfaceUid,
																   iDtor_ID_Key,
																   &initParams,
																   ResolverParams)));

		__UHEAP_SETFAIL(RHeap::ENone, 0);
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}
		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0697
@SYMTestCaseDesc	    Tests for out of memory error while creating REComSession::CreateImplementationL with a resolver UID
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM14()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0697 TestOOM14 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);
	TUid iDtor_ID_Key = KNullUid;

	TUid resolverUid = {0x10009DD0};

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;
		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = REINTERPRET_CAST(CExampleInterface*,
								REComSession::CreateImplementationL(KCExampleInterfaceUid,
																   iDtor_ID_Key,
																   &initParams,
																   ResolverParams,
																   resolverUid)));

		__UHEAP_SETFAIL(RHeap::ENone, 0);
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}
		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0698
@SYMTestCaseDesc	    Tests for out of memory error while creating REComSession::CreateImplementationL with a resolver UID
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and that no handles have leaked.Test for no memory and no error conditions
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOM15()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0698 TestOOM15 "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);
	TUid iDtor_ID_Key = KNullUid;
    TUid resolverUid = {0x10009DD0};

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);		// Allow wildcard matching

	CExampleInterface* dllPtr = NULL;

	do
		{
		__UHEAP_MARK;

		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TRAP(err, dllPtr = REINTERPRET_CAST(CExampleInterface*,
								REComSession::CreateImplementationL(KCExampleInterfaceUid,
																   iDtor_ID_Key,
																   &initParams,
																   ResolverParams,
																   resolverUid)));

		__UHEAP_SETFAIL(RHeap::ENone, 0);
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		delete dllPtr;
		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}
		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		test(pHCountStart == pHCountEnd);
		test(tHCountStart == tHCountEnd);
		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test(err == KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-4006
@SYMTestCaseDesc		Tests for out of memory error to REComSession::CreateImplementationL
@SYMTestPriority			High
@SYMTestActions			Call CreateImplementationL() to load no.0 to no.4 plugins.
						Call GetImplementationUidL() and check impl id.
						Call CreateImplementationL() to load no.5 to met OOM.
						Call DestroyedImplementation() to destroy the no.0 to no.1 Plugins
						Call CreateImplementationL() to load no.5 to no.8 Plugins
						Creating 100 Plugins using the same ImplementationUid
						Call GetImplementationUidL() and check impl id.
						List implementation uid from interface uid
						Destroying 100 Plugins using the same ImplementationUid
						Call DestroyedImplementation() to destroy the no.2 to no.4 Plugins
						Call GetImplementationUidL() and check impl id.
						Call DestroyedImplementation() to destroy the no.5 to no.8 Plugins
@SYMTestExpectedResults	Returns success
						leaves with the appropriate error.
@SYMDEF					PDEF116654

*/
LOCAL_C void TestOOM18()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4006 TestOOM18 "));

	//
	// Testing a overloaded method for CreateImplementationL
	// ------------------------------------------------------------------
	//

	// Set up for heap leak checking
	__UHEAP_MARK;

	TInt i;
	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	// Test Starts...
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	TFixedArray<TUid,9> test19LPlugIns;
	test19LPlugIns[0] = KCExampleImplUid7;
	test19LPlugIns[1] = KCExampleImplUid3;
	test19LPlugIns[2] = KCExampleImplUid4;
	test19LPlugIns[3] = KCExampleImplUid5;
	test19LPlugIns[4] = KCExampleImplUid6;
	test19LPlugIns[5] = KCExampleImplUid;
	test19LPlugIns[6] = KCExampleImplUid8;
	test19LPlugIns[7] = KCExampleImplUid9;
	test19LPlugIns[8] = KCExampleImplUid2;

	TFixedArray<CExampleInterface*,9> dllPtr;
	TFixedArray<CExampleInterface*,100> dllPtr1;

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	TFixedArray<TUid,9> iDtor_ID_Key;
	TFixedArray<TUid,100> iDtor_ID_Key1;
	TUid implUid;
	TInt err;

	test.Printf(_L("Loading 5 Plugins..."));
	for(i=0; i<5; i++)
		{
		dllPtr[i] = REINTERPRET_CAST(CExampleInterface*,
				REComSession::CreateImplementationL(test19LPlugIns[i],
						   iDtor_ID_Key[i],
						   &initParams));
		test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr[i]->ImplId(), test19LPlugIns[i]);
		test(test19LPlugIns[i]==dllPtr[i]->ImplId());
		}

	test.Printf(_L("Getting implementation uid from detructor key..."));
	for(i=0; i<5; i++)
		{
		implUid = REComSession::GetImplementationUidL(iDtor_ID_Key[i]);
		// checking returned impl uid. For current implementation of Load
		// manager the returned uid is the same as the destructor key. This
		// will change with the implementation of EC043. The destructor key
		// will become an arbitrary value.
		test(implUid == test19LPlugIns[i]);
		}
	__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
	TRAP(err,dllPtr[5] = REINTERPRET_CAST(CExampleInterface*,
			REComSession::CreateImplementationL(test19LPlugIns[5],
					   iDtor_ID_Key[5],
					   &initParams)));
	__UHEAP_SETFAIL(RHeap::ENone, 0);
	if(err == KErrNone)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key[5]);
		delete dllPtr[5];
		REComSession::FinalClose();
		}

	test.Printf(_L("Destroying the first 2 Plugins..."));
	for(i=0; i<2; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key[i]);
		delete dllPtr[i];
		REComSession::FinalClose();
		}

	test.Printf(_L("Loading another 4 Plugins..."));
	for(i=5; i<8; i++)
		{
		dllPtr[i] = REINTERPRET_CAST(CExampleInterface*,
				REComSession::CreateImplementationL(test19LPlugIns[i],
						   iDtor_ID_Key[i],
						   &initParams));
		test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr[i]->ImplId(), test19LPlugIns[i]);
		test(test19LPlugIns[i]==dllPtr[i]->ImplId());
		}
	dllPtr[8] = REINTERPRET_CAST(CExampleInterface*,
			REComSession::CreateImplementationL(test19LPlugIns[8],
					   iDtor_ID_Key[8],
					   NULL));
	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr[8]->ImplId(), test19LPlugIns[8]);
	test(test19LPlugIns[8]==dllPtr[8]->ImplId());

	test.Printf(_L("Creating 100 Plugins using the same ImplementationUid..."));
	for(TInt i=0; i<100; i++)
		{
		dllPtr1[i] = REINTERPRET_CAST(CExampleInterface*,
			REComSession::CreateImplementationL(test19LPlugIns[8],
					   iDtor_ID_Key1[i],
					   NULL));
		test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr1[i]->ImplId(), test19LPlugIns[8]);
		test(dllPtr1[i]->ImplId()==test19LPlugIns[8]);
		}
	test.Printf(_L("Getting implementation uid from detructor key..."));
	for(i=5; i<=8; i++)
		{
		implUid = REComSession::GetImplementationUidL(iDtor_ID_Key[i]);
		// checking returned impl uid. For current implementation of Load
		// manager the returned uid is the same as the destructor key. This
		// will change with the implementation of EC043. The destructor key
		// will become an arbitrary value.
		test(implUid == test19LPlugIns[i]);
		}

	test.Printf(_L("List implementation uid from interface uid..."));
	RImplInfoPtrArray ifArray;
	TUid ifUid = {0x10009DC0};
	_LIT8(KImplementationTest,"text/wml");
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParamsList;
	ResolverParamsList.SetDataType(KImplementationTest());
	ResolverParamsList.SetGenericMatch(ETrue);	// Allow wildcard matching
	REComSession::ListImplementationsL(
			ifUid,
			ResolverParamsList,
			ifArray);
	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 2);
	ifArray.ResetAndDestroy();

	test.Printf(_L("Destroying 100 Plugins using the same ImplementationUid..."));
	for(TInt i=0; i<100; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key1[i]);
		delete dllPtr1[i];
		}

	test.Printf(_L("Destroying the 2-4 Plugins..."));
	for(i=2; i<5; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key[i]);
		delete dllPtr[i];
		REComSession::FinalClose();
		}

	test.Printf(_L("Getting implementation uid from detructor key..."));
	for(i=5; i<=8; i++)
		{
		implUid = REComSession::GetImplementationUidL(iDtor_ID_Key[i]);
		// checking returned impl uid. For current implementation of Load
		// manager the returned uid is the same as the destructor key. This
		// will change with the implementation of EC043. The destructor key
		// will become an arbitrary value.
		test(implUid == test19LPlugIns[i]);
		}
	test.Printf(_L("Destroying the 5-9 Plugins..."));
	for(i=5; i<9; i++)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key[i]);
		delete dllPtr[i];
		REComSession::FinalClose();
		}

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;
	}



LOCAL_C void TestListImplementationL()
	{
	test.Next(_L("ListImplemenatationsL for custom resolver tests..."));

	Test1L();
	Test2L();
	Test3L();
	Test16L();
	Test17L();
	}

LOCAL_C void TestCreateImplemenatationL()
	{
	test.Next(_L("CreateImplemenatationL tests..."));

	Test4L();
	Test5L();
	Test6L();
	Test7L();
	Test8L();
	Test9L();
	Test10L();
	Test11L();
	Test12L();
	Test13L();
	Test14L();
	Test15L();
	Test18L();
	Test19L();
	Test20L();
	Test1PDEF116654L();
	Test2PDEF116654L();

	}

LOCAL_C void TestOOM()
	{
	test.Next(_L("OOM tests..."));

	TestOOM1();
	TestOOM2();
	TestOOM3();
	TestOOM4();
	TestOOM5();
    TestOOM6();
	TestOOM7();
	TestOOM8();
	TestOOM9();
	TestOOM10();
	TestOOM11();
	TestOOM12();
	TestOOM13();
	TestOOM14();
	TestOOM15();
	TestOOM16();
	TestOOM17();
	TestOOM18();
	}


//

void TestINC053667L()
	{

	test.Next(_L("INC053667 - NTT NULL as construction parameter does not work in ECom"));
	// Set up for heap leak checking
	__UHEAP_MARK;
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	// Test Starts...
	TUid iDtor_ID_Key;
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);
	test.Printf(_L("Loading Plugin..."));

	CExampleInterface* dllPtr = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(KCExampleImplUid2,
															   iDtor_ID_Key,
															   NULL));

	test.Printf(_L("Found dll uid %x, required %x.\n"),dllPtr->ImplId(), KCExampleImplUid2);
	test(KCExampleImplUid2==dllPtr->ImplId());

	test.Printf(_L("Destroying Plugin..."));
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	delete dllPtr;
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;

	}

//INC054125 - ECOM cleanup stack error
static void TestINC054125L()
	{
	test.Next(_L("INC054125-ECOM cleanup stack error"));

	RImplInfoPtrArray ifArray;
	TUid ifUid = {0x10009DC8};//Test ECOM plugin UID
	TUid resolverUid = {0x10009DD0};//Custom resolver UID, implementation in ExampleResolver.cpp,
	                                //EComExample test project.
	TEComResolverParams criteriaSet;
	_LIT8(KCriteriaData, "INC054125");//This string is used by CExampleResolver::ListAllL() impl.
	                                  //where it is detected together with 0x10009DC8 uid and
	                                  //original conditions, under which INC054125 shows itself,
	                                  //are simulated.
	criteriaSet.SetDataType(KCriteriaData);

	TRAPD(err, REComSession::ListImplementationsL(ifUid, criteriaSet, resolverUid, ifArray));
	test(err == KErrGeneral);
	// Empty the array of implementations
	ifArray.ResetAndDestroy();
	REComSession::FinalClose();
	}

void TheTest::StartTestL()
	{
	TestListImplementationL();
	TestCreateImplemenatationL();
	TestOOM();
	TestINC054125L();
	TestINC053667L();
	}

TInt DoTestsL()
	{
	// Should any tests leave past the lowest level???
	TheTest theT;
	theT.StartTestL();
	return KErrNone;
	}

void SetupFiles()
	{
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComPlugin2OnZ, KEComPlugin2OnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComPluginRsc2OnZ, KEComPluginRsc2OnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComEx7DllOnZ, KEComEx7DllOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComEx7RscFileOnZ, KEComEx7RscFileOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComEx8DllOnZ, KEComEx8DllOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComEx8RscFileOnZ, KEComEx8RscFileOnC));
	test(err==KErrNone);

	}

void CleanupFiles()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComPlugin2OnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComPluginRsc2OnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComEx7DllOnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComEx8DllOnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComEx7RscFileOnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComEx8RscFileOnC));
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("ListImplementationsL tests."));

	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	//install the scheduler
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	SetupFiles();
	//The reason for the folowing delay is:
	//ECOM server could be already started. It means that when we copy some
	//ECOM plugins from Z: to C: drive - ECOM server should look for and
	//find the new ECOM plugins. The ECOM server uses for that CDiscoverer::CIdleScanningTimer
	//which is an active object. So the discovering service is asynchronous. We have to
	//wait some time until it finishes. Otherwise ListImplementationsL could fail to find
	//requested implementations.
	User::After(KOneSecond * 5);

	TRAPD(err,DoTestsL());

	CleanupFiles();

	delete scheduler;
	delete cleanup;
	test(err==KErrNone);
	test.Next(_L("/n"));
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(0);
	}
