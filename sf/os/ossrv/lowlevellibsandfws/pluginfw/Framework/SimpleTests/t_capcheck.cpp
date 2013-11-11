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
//

#include <e32test.h>
#include <e32panic.h>
#include <f32file.h>
#include <bautils.h>

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h"
#include "T_PlatSecInterface.h"
#include "../EcomTestUtils/EcomTestUtils.h"
#include "LoadManager.h"

/**----------------------------Client CAP-----------------------------------------
             Client Capability Set="ReadUserData" "WriteUserData"
*/
/** ---------------------------Plugins used in this test case----------------------
- T_PlatSecECom1.dll in DRIVE Z:
  Details:-Resource file=102026AB.rss
		  -Capability Set="ReadUserData" "WriteUserData"
		  -ImplementationUid=0x102026AA based on InterfaceUid=0x102026A8

- T_PlatSecECom2.dll in DRIVE Z:
  Details:-Resource file=102026AD.rss
  		  -Capability Set="ReadUserData"
  		  -ImplementationUid=0x102026AC based on InterfaceUid=0x102026A9

- T_PlatSecECom3.dll in DRIVE Z:
  Details:-Resource file=102026AF.rss
          -Capability Set="ReaderUserData" "WriteUserData" "AllFiles"
          -ImplementationUid=0x102026AE based on InterfaceUid=0x102026A9

- T_PlatSecECom4.dll in DRIVE C:
  Details:-Resource file=10202666.rss
  		  -Capability Set="ReadUserData" "WriteUserData" "ProtServ"
  		  -ImplementationUid=0x102026B1 based on InterfaceUid=0x102026A8

- T_PlatSecResolver.dll in DRIVE Z:
  Details:-Resource file=10202777.rss
  		  -Capability Set="ReadUserData"
  		  -ImplementationUid=0x10202747 based on InterfaceUid=0x102027E7

- EComExample.dll in Drive Z:
  Details:-Contain two implementation and one Resolver, only interested in the resolver
  		  -Resource file=10009DB1.RSS
  		  -Capability Set="All -Tcb"
  		  -ImplementationUid=0x10009DD0 based on InterfaceUid=0x10009D90
-------------------------------------------------------------------------------------
*/

/** Test cases covered in this test code:
1.	Testing Client API capability filtering in ListImplementation(TUid aInterfaceUid) in Z,C;
2.	Testing Client API capability filtering in CreateImplementation(TUid aImplementationUid) in Z,C;
3.	Testing Client API capability checking in Loading a non-default resolver in the server side
*/

class RCapCheckTest
	{
public:
	static TBool IsPlatSecEnforcementOn();
	//Test case 1
	static void ListImpl_InterfaceL();
	//Test case 2
	static void CreateImpl_Implementation();
	//Test case 3
	static void ListImpl_NonDefaultResolverL();
	//Test case 4
	static void ListImpl_InterfaceWithoutCapCheckL();
	};


/**--------------------Utility Functions--------------------------------------------*/
// Plugins files that need to be copied from Z: to C:
_LIT(KResourceFileNameC,	 "C:\\resource\\plugins\\T_PlatSecECom4.rsc");
_LIT(KExampleDllFileNameC,	 "C:\\sys\\bin\\T_PlatSecECom4.dll");
_LIT(KResourceFileNameZ,	 "Z:\\RAMONLY\\T_PlatSecECom4.rsc");

_LIT(KExampleDllFileNameZ,	 "Z:\\RAMONLY\\T_PlatSecECom4.dll");

const TInt KOneSecond = 1000000;

LOCAL_D RFs	TheFs;

LOCAL_D RTest test(_L("Capability Checking Test"));

// Copy the Plugins to specific folder for testing purpose
LOCAL_C void CopyPluginsL()
	{
	// Copy the dlls and .rsc files on to RAM
	TRAPD(err, EComTestUtils::FileManCopyFileL(KResourceFileNameZ,KResourceFileNameC));
	test(err == KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KExampleDllFileNameZ,KExampleDllFileNameC));
	test(err == KErrNone);
	// Wait, so that ECom server looks for plugins copied from Z: to C drive
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that CDiscoverer::CIdleScanningTimer
	// which is an active object. So the discovering service is asynchronous. We have to
	// wait some time until it finishes. Otherwise ListImplementationsL could fail to find
	// requested implementations.
	User::After(KOneSecond * 3);
	}

// Deleting plugin from the RAM for cleanup purpose
inline LOCAL_C void DeleteTestPlugin()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KResourceFileNameC));
	test(err == KErrNone);

	TRAP(err, EComTestUtils::FileManDeleteFileL(KExampleDllFileNameC));
#if defined(__WINS__) || defined (__WINSCW__)
    if (err != KErrNone)
   	    {
        TESTC(test, err, KErrAccessDenied); // DLL File locked under Windows emulator due to it being demand paged
        }
    else
       	TESTC(test, err, KErrNone); // DLL File not locked on Windows
#elif defined(__EPOC32__)
   	TESTC(test, err, KErrNone); // DLL File not locked on target hardware
#endif
    // Do no test at all as its an unexpected platform.
}

TBool RCapCheckTest::IsPlatSecEnforcementOn()
	{
	return PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement);
	}
/**
--------------------------Test case 1---------------------------------------------
Testing Client API capability filtering in ListImplementation(TUid aInterfaceUid);
-Client CapSet listing implemenation with InterfaceUid=0x102026A9
 *Two existing Impl based on that Interface T_PlatSecECom2.dll & T_PlatSecEcom3.dll
  one with lower capability set than the client
 *Expected array count=1 as it wont be able to load T_PlatSecECom2.dll
  when PlatSec is enforced, count=2 otherwise.
-Client Capset listing implementation with InterfaceUid=0x102026A8
 *Test to ensure it pick up plugins in other drive else than Z


@SYMTestCaseID          SYSLIB-ECOM-CT-0771
@SYMTestCaseDesc	    Testing Client API capability filtering in ListImplementation(TUid aInterfaceUid)
@SYMTestPriority 	    High
@SYMTestActions  	    Ensure it picks up plugins in other drive else than Z
                        List of all available implementations which
	                    satisfy this given interface and check for the validity.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void RCapCheckTest::ListImpl_InterfaceL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0771 List Implementation Capability checking\n "));
	// Input parameter
	const TUid interfaceUid = {0x102026A9};
	// Output parameters
	TInt err = KErrNone;
	RImplInfoPtrArray* implementationArray=new (ELeave) RImplInfoPtrArray;

	//Listing implementation from interface=0x102026A9
	TRAP(err, REComSession::ListImplementationsL(interfaceUid, *implementationArray));
	test(KErrNone == err);
	if (RCapCheckTest::IsPlatSecEnforcementOn())
		{
		//Only will return one implementation in T_PlatSecECom3.dll
		test(1 == implementationArray->Count());
		const TUid expectedImplUid = {0x102026AE};
		test(expectedImplUid == (*implementationArray)[0]->ImplementationUid());
		}
	else
		{
		//Return two implementations from T_PlatSecECom2.dll and T_PlatSecECom3.dll
		test(2 == implementationArray->Count());
		}
	implementationArray->ResetAndDestroy();

	//Listing implementation from interface=0x102026A8
	const TUid interfaceUid2 = {0x102026A8};
	TRAP(err, REComSession::ListImplementationsL(interfaceUid2, *implementationArray));
	test(KErrNone == err);
	test(2 == implementationArray->Count());
	implementationArray->ResetAndDestroy();

	delete implementationArray;
	}

/**
--------------------------Test case 2----------------------------------------------------
Testing Client API capability filtering in CreateImplementation(TUid aImplementationUid);
- Client try to create Implementation with different capset
- If Client.CAPS > Plugin.CAPS Expected Result=FAIL
- If Client.CAPS < Plugin.CAPS Expected Result=OK
- If Client.CAPS = Plugin.CAPS Expected Result=OK

@SYMTestCaseID          SYSLIB-ECOM-CT-0772
@SYMTestCaseDesc	    Testing Client API capability filtering in CreateImplementation(TUid aImplementationUid)
@SYMTestPriority 	    High
@SYMTestActions  	    Create Implementation with different capsets
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void RCapCheckTest::CreateImpl_Implementation()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0772 Create Implementation Capability filtering\n "));
	// Output parameters
	TInt ret=KErrNone;
	TUid returnedimplUid;
	TAny* ImplCreation=NULL;

	//Creating Implementation with CAPS="ReadUserData"
	const TUid ImplUid2 = {0x102026AC};
	TRAP(ret,ImplCreation=REComSession::CreateImplementationL(ImplUid2,returnedimplUid));
	if (RCapCheckTest::IsPlatSecEnforcementOn())
		{
		test(KErrPermissionDenied == ret);
		test(NULL == ImplCreation);
		}
	else
		{
		test(KErrNone == ret);
		test(NULL != ImplCreation);
		test(ImplUid2 == returnedimplUid);
		//Cast to CPlatSecInterface2
		CPlatSecInterface2* impl2=reinterpret_cast <CPlatSecInterface2*> (ImplCreation);
		test(ImplUid2 == impl2->ImplId());
		REComSession::DestroyedImplementation(returnedimplUid);

		delete ImplCreation;
		ImplCreation = NULL;
		}

	//Creating Implementation with CAPS="ReadUserData WriteUserData AllFiles"
	const TUid ImplUid3 = {0x102026AE};
	TUid instanceKey;
	TRAP(ret,ImplCreation=REComSession::CreateImplementationL(ImplUid3,instanceKey));
	CInstanceInfoSimple* instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (instanceKey.iUid);
	returnedimplUid = instanceInfo->ImplementationUid();

	test(KErrNone == ret);
	test(NULL != ImplCreation);

	test(ImplUid3 == returnedimplUid);
	//Cast to CPlatSecInterface3
	CPlatSecInterface3* impl3=reinterpret_cast <CPlatSecInterface3*> (ImplCreation);
	test(ImplUid3 == impl3->ImplId());
	REComSession::DestroyedImplementation(instanceKey);

	delete ImplCreation;
	ImplCreation = NULL;

	//Creating Implementation with CAPS="ReadUserData WriteUserData"
	const TUid ImplUid1 = {0x102026AA};
	TRAP(ret,ImplCreation=REComSession::CreateImplementationL(ImplUid1,instanceKey));
	instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (instanceKey.iUid);
	returnedimplUid = instanceInfo->ImplementationUid();
	test(KErrNone == ret);
	test(NULL != ImplCreation);
	test(ImplUid1 == returnedimplUid);
	//Cast to CPlatSecInterface1
	CPlatSecInterface1* impl1=reinterpret_cast <CPlatSecInterface1*> (ImplCreation);
	test(ImplUid1 == impl1->ImplId());
	REComSession::DestroyedImplementation(instanceKey);

	delete ImplCreation;
	ImplCreation = NULL;

	//Creating Implementation in C:\ with CAPS="ReadUserData WriteUserData ProtServ"
	const TUid ImplUid4 = {0x102026B1};
	TRAP(ret,ImplCreation=REComSession::CreateImplementationL(ImplUid4,instanceKey));
	instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (instanceKey.iUid);
	returnedimplUid = instanceInfo->ImplementationUid();
	test(KErrNone == ret);
	test(NULL != ImplCreation);
	test(ImplUid4 == returnedimplUid);
	REComSession::DestroyedImplementation(instanceKey);

	delete ImplCreation;
	ImplCreation = NULL;
	}

/**
--------------------------Test case 3----------------------------------------------------
Testing Client API capability filtering in ListImplementation by specifying a non-default
resolver Uid.  The resolver is loaded in the server side.
If PlatSecEnforcement is ON, the non-default resolver should have at least the "ProtServ"
capability in order to be successfully loaded.
- Testing using a non-default resolver T_PlatSecResolver.dll with CAPS=ReadUserData
- Testing using a non-default resolver EComExample.dll with CAPS=All-Tcb

@SYMTestCaseID          SYSLIB-ECOM-CT-0773
@SYMTestCaseDesc	    Testing Client API capability filtering in CreateImplementation(TUid aImplementationUid)
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for listing all available implementations using a non-default resolver  with CAPS=ReadUserData
                        Tests for listing all available implementations using a non-default resolver with CAPS=All-Tcb
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void RCapCheckTest::ListImpl_NonDefaultResolverL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0773 Capability checking in loading non-default resolver in server side\n "));
	// Input parameters
	const TUid interfaceUid = {0x102026A9};
	const TUid resolverUid	= {0x10202747};
	TEComResolverParams resolverparams;
	_LIT8(KDummyData,"dummy");
	//Set any resolver data type as it will never reach the resolving part
	resolverparams.SetDataType(KDummyData());
	// Output parameters
	TInt err=KErrNone;
	RImplInfoPtrArray* implementationArray=new (ELeave) RImplInfoPtrArray;

	//Listing implementation from interface=0x102026A9 using T_PlatSecResolver.dll
	TRAP(err,REComSession::ListImplementationsL(interfaceUid,resolverparams,resolverUid,*implementationArray));
	if (RCapCheckTest::IsPlatSecEnforcementOn())
		{
		//ensure that no resolver returned here because of lacking the capability ProtServ
		test(KErrPermissionDenied == err);
		}
	else
		{
		//No resolver should be found based on the dummy resolver data
		test(KEComErrNoResolver == err);
		}
	test(0 == implementationArray->Count());

	//Listing implementation from interface=0x10009DD0 using EComExample.dll
	const TUid resolverUid2 = {0x10009DD0};
	TRAP(err,REComSession::ListImplementationsL(interfaceUid,resolverparams,resolverUid2,*implementationArray));
	//ensure that no problem here as the resolver has the capability ProtServ
	test(KErrNone == err);
	if (RCapCheckTest::IsPlatSecEnforcementOn())
		{
		//Only will return one implementation in T_PlatSecECom3.dll
		test(1 == implementationArray->Count());
		const TUid expectedImplUid = {0x102026AE};
		test(expectedImplUid == (*implementationArray)[0]->ImplementationUid());
		}
	else
		{
		//Return two implementations from T_PlatSecECom2.dll and T_PlatSecECom3.dll
		test(2 == implementationArray->Count());
		}
	implementationArray->ResetAndDestroy();

	delete implementationArray;
	implementationArray = NULL;
	}

/**
--------------------------Test case 4---------------------------------------------
Testing Client API without capability filtering in ListImplementation(TUid aInterfaceUid);
-Client CapSet listing implemenation with InterfaceUid=0x102026A9
 *Two existing Impl based on that Interface T_PlatSecECom2.dll & T_PlatSecEcom3.dll
  one with lower capability set than the client
 *Expected array count=2 as it will be able to load T_PlatSecECom2.dll
  when PlatSec is enforced, count=2 otherwise.
-Client Capset listing implementation with InterfaceUid=0x102026A8
 *Test to ensure it pick up plugins in other drive else than Z


@SYMTestCaseID          BASESRVCS-ECOM-T-4026
@SYMTestCaseDesc	    Testing Client API without capability filtering in ListImplementation(TUid aInterfaceUid)
@SYMTestPriority 	    High
@SYMTestActions  	    Ensure it picks up plugins in other drive else than Z
                        List of all available implementations which
	                    satisfy this given interface and check for the validity.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 CR1573,REQ11641 and REQ11793
*/
void RCapCheckTest::ListImpl_InterfaceWithoutCapCheckL()
	{
	test.Next(_L(" @SYMTestCaseID:BASESRVCS-ECOM-T-4026 List Implementation Without Capability checking and trying to Create Implementation\n "));
	// Input parameter
	const TUid interfaceUid = {0x102026A9};
	// Output parameters
	TInt err = KErrNone;
	RImplInfoPtrArray* implementationArray=new (ELeave) RImplInfoPtrArray;
	TBool capability = EFalse;

	//Listing implementation from interface=0x102026A9
	TRAP(err, REComSession::ListImplementationsL(interfaceUid, *implementationArray,capability));
	test(KErrNone == err);
	if (RCapCheckTest::IsPlatSecEnforcementOn())
		{
		TInt count=implementationArray->Count();
		test(2 == implementationArray->Count());
		}
	
	TUid returnedimplUid;
	TAny* ImplCreation=NULL;
	//Creating Implementation with CAPS="ReadUserData"
	const TUid ImplUid2 = {0x102026AC};
	TRAP(err,ImplCreation=REComSession::CreateImplementationL(ImplUid2,returnedimplUid));
	if (RCapCheckTest::IsPlatSecEnforcementOn())
		{
		test(KErrPermissionDenied == err);
		test(NULL == ImplCreation);
		}	
	implementationArray->ResetAndDestroy();

	//Listing implementation from interface=0x102026A8
	const TUid interfaceUid2 = {0x102026A8};
	TRAP(err, REComSession::ListImplementationsL(interfaceUid2, *implementationArray));
	test(KErrNone == err);
	test(2 == implementationArray->Count());
	implementationArray->ResetAndDestroy();

	delete implementationArray;
	}


LOCAL_C void RunTestL()
	{
	__UHEAP_MARK;

	//Test case 1
	RCapCheckTest::ListImpl_InterfaceL();

	//Test case 2
	RCapCheckTest::CreateImpl_Implementation();

	//Test case 3
	RCapCheckTest::ListImpl_NonDefaultResolverL();
	
	//Test case 4
	RCapCheckTest::ListImpl_InterfaceWithoutCapCheckL();

	//We do not want any memory leak here
	REComSession::FinalClose();

	__UHEAP_MARKEND;
	}

GLDEF_C TInt E32Main()
	{
	TInt err=KErrNone;
	__UHEAP_MARK;

	test.Title();
	test.Start(_L("Capability Checking tests."));

	// This test is only applicable for secure ECom.
	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAP(err, CopyPluginsL());
	test(err==KErrNone);

	TRAP(err,RunTestL());
	test(err==KErrNone);

	// Cleanup files. If the cleanup fails that is no problem,
	// as any subsequent tests will replace them. The only downside
	// would be the disk not being tidied
	DeleteTestPlugin();

	delete scheduler;
	delete cleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(0);
	}
