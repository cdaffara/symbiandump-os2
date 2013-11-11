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

#include <f32file.h>
#include <e32test.h>
#include <bautils.h>
#include <hal.h>
#include <e32std.h>
#include <bsul/bsul.h>
#include "t_clientmessagetestserver.h"

using namespace BSUL;


LOCAL_D RTest				Test (_L("T_ClientMessage"));
LOCAL_D RFs					TheFs;

_LIT(KServerName,"ClientMessageTestServer");
_LIT(KServer2Name,"SecondTestServer");
_LIT(KServerFileName,"T_ClientMessageTestServer.exe");

struct TExitDetails
	{
	TExitCategoryName iCategory;
	TExitType iExitType;
	TInt iReason;
	};

typedef void (*TestFunction)();


/*RClientMessageTestSession client side interface to
 * CClientMessageTestServer server.  This is used to pass
 * messages to the server to testthe Client Message Framework.
 */
class RClientMessageTestSession : public RSessionBase
	{
public:
	TInt Connect();

	TInt TestFunction0(TInt aArg0);
	TInt TestFunction0(TInt aArg0, TInt aArg1);
	TInt TestFunction0(TInt aArg0, TInt aArg1, TInt aArg2);
	TInt TestFunction0(const TDesC8& aData0, const TDesC& aData1);
	TInt TestFunction1(TInt aArg0, TInt aArg1);
	TInt TestFunction2(TInt aArg0, TInt aArg1);
	TInt TestFunction3(TInt aArg0, TInt aArg1);

	TInt TestFunction4(const TDesC8& aData0, const TDesC& aData1);
	TInt TestFunction4(const TDesC& aData0, const TDesC8& aData1);
	TInt TestFunction4(const TDesC8& aData0);
	TInt TestFunction5(const TDesC8& aData0, const TDesC& aData1);
	TInt TestFunction6(const TDesC8& aData0, const TDesC& aData1);
	TInt TestFunction7(const TDesC8& aData0, const TDesC& aData1);
	TInt TestFunction8(const TDesC8& aData0, const TDesC& aData1);

	TInt TestFunction9(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1);
	TInt TestFunction9(TDes& aData0,TInt aOffset0, TDes8& aData1,TInt aOffset1);
	TInt TestFunction10(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1);
	TInt TestFunction11(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1);
	TInt TestFunction12(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1);
	TInt TestFunction13(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1);
	TInt TestFunction14(const TDesC8& aData0,TInt aOffset0, TDes8& aData1);
	TInt TestFunction15(const TDesC& aData0,TInt aOffset0, TDes& aData1);

	TInt TestFunction16(TAny* aData);
	TInt TestFunction17(TAny* aData);

	TInt TestFunction18(TDes8& aData);
	TInt TestFunction18(TDes16& aData);
	TInt TestFunction19(TDes8& aData);

	TInt TestFunction20(TInt aArg0, TInt aArg1);
	TInt TestFunction21(TInt aArg0, TInt aArg1);
	TInt TestFunction22(TInt aArg0, TInt aArg1);
	TInt TestFunction23(TInt aArg0, TInt aArg1);
	TInt TestFunction24(TInt aArg0, TInt aArg1);

	TInt TestFunction25();
	TInt TestFunction26();
	TInt TestFunction27();
	TInt TestFunction28(TInt aArg0);
	TInt TestFunction29(TDes& aData);
	TInt TestFunction30(TDes8& aData);
	TInt TestFunction31(TInt aArg0);
	TInt TestFunction32(TDes8& aData);

	TInt TestInvalidFunction(TInt aFn);


	};

/*RClientMessageTestSession2 client side interface to
 * CClientMessageTestServer2 server.  This is used to pass
 * messages to the server to testthe Client Message Framework.
 */
class RClientMessageTestSession2 : public RSessionBase
	{
public:
	TInt Connect();

	TInt TestFunction0(TInt aArg0, TInt aArg1);
	
	TInt TestFunction1(const TDesC& aData0);

	};

//Global process that is accessible in test code to determine the
//exit reason and category on termination
RProcess serverProcess;

//Launch the server process
static TInt LaunchServer()
	{
	TInt err = serverProcess.Create(KServerFileName,_L(""));

	if(err == KErrNone)
		{
		 TRequestStatus serverStat;

		 serverProcess.SetJustInTime(EFalse);
		 serverProcess.Rendezvous(serverStat);
		 serverProcess.Resume();
		 User::WaitForRequest(serverStat);
		}

	return err;
	}


/* Connect to the CClientMessageTestServer, launching
 * the sevrer process if necessary
 */
TInt RClientMessageTestSession::Connect()
	{
	TInt retry = 2;
	for(;;)
		{
		TInt r = CreateSession(KServerName,TVersion(1,0,0));

		if((r != KErrNotFound)&&(r != KErrServerTerminated))
			{
			return r;
			}

		if(--retry == 0)
			{
			return r;
			}

		r = LaunchServer();
		if((r != KErrNone)&&(r != KErrAlreadyExists))
			{
			return r;
			}
		}
	}

/* These functions are used to send IPC messages to the server.
 * The messages are sent with different parameters to test the
 * ClientMessage framework handling of bad messages
 */
TInt RClientMessageTestSession::TestFunction0(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage0,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession::TestFunction0(TInt aArg0)
	{
	return SendReceive(ETestMessage0,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestFunction0(TInt aArg0, TInt aArg1, TInt aArg2)
	{
	return SendReceive(ETestMessage0,TIpcArgs(aArg0,aArg1,aArg2));
	}

TInt RClientMessageTestSession::TestFunction0(const TDesC8& aData0, const TDesC& aData1)
	{
	//Call the function passing invalid arguments
	return SendReceive(ETestMessage0,TIpcArgs(&aData0,&aData1));
	}

TInt RClientMessageTestSession::TestFunction1(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage1,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession::TestFunction2(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage2,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession::TestFunction3(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage3,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession::TestFunction4(const TDesC8& aData0, const TDesC& aData1)
	{
	return SendReceive(ETestMessage4,TIpcArgs(&aData0,&aData1));
	}

TInt RClientMessageTestSession::TestFunction4(const TDesC& aData0, const TDesC8& aData1)
	{
	return SendReceive(ETestMessage4,TIpcArgs(&aData0,&aData1));
	}

TInt RClientMessageTestSession::TestFunction4(const TDesC8& aData0)
	{
	return SendReceive(ETestMessage4,TIpcArgs(&aData0));
	}

TInt RClientMessageTestSession::TestFunction5(const TDesC8& aData0, const TDesC& aData1)
	{
	return SendReceive(ETestMessage5,TIpcArgs(&aData0,&aData1));
	}

TInt RClientMessageTestSession::TestFunction6(const TDesC8& aData0, const TDesC& aData1)
	{
	return SendReceive(ETestMessage6,TIpcArgs(&aData0,&aData1));
	}

TInt RClientMessageTestSession::TestFunction7(const TDesC8& aData0, const TDesC& aData1)
	{
	return SendReceive(ETestMessage7,TIpcArgs(&aData0,&aData1));
	}

TInt RClientMessageTestSession::TestFunction8(const TDesC8& aData0, const TDesC& aData1)
	{
	return SendReceive(ETestMessage8,TIpcArgs(&aData0,&aData1));
	}

TInt RClientMessageTestSession::TestFunction9(TDes& aData0,TInt aOffset0, TDes8& aData1,TInt aOffset1)
	{
	return SendReceive(ETestMessage9,TIpcArgs(&aData0,aOffset0,&aData1,aOffset1));
	}

TInt RClientMessageTestSession::TestFunction9(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1)
	{
	return SendReceive(ETestMessage9,TIpcArgs(&aData0,aOffset0,&aData1,aOffset1));
	}

TInt RClientMessageTestSession::TestFunction10(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1)
	{
	return SendReceive(ETestMessage10,TIpcArgs(&aData0,aOffset0,&aData1,aOffset1));
	}

TInt RClientMessageTestSession::TestFunction11(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1)
	{
	return SendReceive(ETestMessage11,TIpcArgs(&aData0,aOffset0,&aData1,aOffset1));
	}

TInt RClientMessageTestSession::TestFunction12(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1)
	{
	return SendReceive(ETestMessage12,TIpcArgs(&aData0,aOffset0,&aData1,aOffset1));
	}

TInt RClientMessageTestSession::TestFunction13(TDes8& aData0,TInt aOffset0, TDes& aData1,TInt aOffset1)
	{
	return SendReceive(ETestMessage13,TIpcArgs(&aData0,aOffset0,&aData1,aOffset1));
	}

TInt RClientMessageTestSession::TestFunction14(const TDesC8& aData0,TInt aOffset0, TDes8& aData1)
	{
	return SendReceive(ETestMessage14,TIpcArgs(&aData0,aOffset0,&aData1));
	}

TInt RClientMessageTestSession::TestFunction15(const TDesC& aData0,TInt aOffset0, TDes& aData1)
	{
	return SendReceive(ETestMessage15,TIpcArgs(&aData0,aOffset0,&aData1));
	}

TInt RClientMessageTestSession::TestFunction16(TAny* aData)
	{
	return SendReceive(ETestMessage16,TIpcArgs(aData));
	}

TInt RClientMessageTestSession::TestFunction17(TAny* aData)
	{
	return SendReceive(ETestMessage17,TIpcArgs(aData));
	}

TInt RClientMessageTestSession::TestFunction18(TDes8& aData)
	{
	return SendReceive(ETestMessage18,TIpcArgs(&aData));
	}

TInt RClientMessageTestSession::TestFunction18(TDes& aData)
	{
	return SendReceive(ETestMessage18,TIpcArgs(&aData));
	}

TInt RClientMessageTestSession::TestFunction19(TDes8& aData)
	{
	return SendReceive(ETestMessage19,TIpcArgs(&aData));
	}

TInt RClientMessageTestSession::TestFunction20(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage20,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession::TestFunction21(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage21,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession::TestFunction22(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage22,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession::TestFunction23(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage23,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession::TestFunction24(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage24,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession::TestFunction25()
	{
	return SendReceive(ETestMessage25,TIpcArgs());
	}

TInt RClientMessageTestSession::TestFunction26()
	{
	return SendReceive(ETestMessage26,TIpcArgs());
	}

TInt RClientMessageTestSession::TestFunction27()
	{
	return SendReceive(ETestMessage27,TIpcArgs());
	}

TInt RClientMessageTestSession::TestFunction28(TInt aArg0)
	{
	return SendReceive(ETestMessage28,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestFunction29(TDes& aData)
	{
	return SendReceive(ETestMessage29,TIpcArgs(&aData));
	}

TInt RClientMessageTestSession::TestFunction30(TDes8& aData)
	{
	return SendReceive(ETestMessage30,TIpcArgs(&aData));
	}

TInt RClientMessageTestSession::TestFunction31(TInt aArg0)
	{
	return SendReceive(ETestMessage31,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestFunction32(TDes8& aData)
	{
	return SendReceive(ETestMessage32,TIpcArgs(&aData));
	}

TInt RClientMessageTestSession::TestInvalidFunction(TInt aFn)
	{
	return SendReceive(aFn,TIpcArgs());
	}

/* This function is the entry point for a new thread.  It calls the
 * function passed to it as a TAny*.
 */
TInt TestFunctionLauncherL(TAny* aTestFunction)
	{
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt r=KErrNoMemory;
   	if (cleanup)
   		{
   		TestFunction function = (TestFunction)aTestFunction;

__UHEAP_MARK;
 		TRAP(r,function());
__UHEAP_MARKEND;

   		delete cleanup;
   		}
   	return r;
	}

/* This function creates a new thread and passes through the test function
 * to be called.  It also monitors and returns the exit reason for the launched
 * thread.
 */
TExitDetails LaunchTestThreadL(const TDesC& aThreadName, TestFunction aFunction)
	{
	RThread thread;
	TInt err = thread.Create(aThreadName, &TestFunctionLauncherL, KDefaultStackSize, NULL, (TAny*)aFunction);

	if(err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}

	TRequestStatus threadStat;
	thread.Logon(threadStat);

	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);

	thread.Resume();
	User::WaitForRequest(threadStat);

	User::SetJustInTime(jit);

	TExitDetails exitDetails;
	exitDetails.iCategory = thread.ExitCategory();
	exitDetails.iReason = thread.ExitReason();
	exitDetails.iExitType = thread.ExitType();

	return exitDetails;
	}


/*
 This test function accepts 2 integer parameters as defined below
{EParamInt,-10,100},{EParamInt,0,200}
Any calls to this function with parameters outside the ranges
defined above should fail with KErrBadParameter
*/
void TestFunction0AL()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction0( 0, 100);
	Test(err == KErrNone);

	err = session.TestFunction0( 100, 0);
	Test(err == KErrNone);

	err = session.TestFunction0( -5, 10);
	Test(err == KErrNone);

	err = session.TestFunction0( -5, -10);
	Test(err == KErrBadParameter);

	err = session.TestFunction0( -15, 10);
	Test(err == KErrBadParameter);

	err = session.TestFunction0( -10, 200);
	Test(err == KErrNone);

	err = session.TestFunction0( 101, 102);
	Test(err == KErrBadParameter);

	err = session.TestFunction0(11, 56, 43);
	Test(err == KErrNone);

	err = session.TestFunction0( _L8("Des8"), _L("Des"));
	Test(err == KErrBadParameter);

	CleanupStack::PopAndDestroy(&session);
	}


/*
 This test function accepts 2 integer parameters as defined below
{EParamInt,-10,100},{EParamInt,0,200}
In the server, this function tries to access a ptr type from
an int argument. This is a programming error and should result
in the server panicking with ECMPanicWrongParameterType in UDEB
or returning KErrWrongParameterType in UREL
*/
void TestFunction1L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction1(10,100);

#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicWrongParameterType);
#else
	Test(err == KErrWrongParameterType);
#endif
	CleanupStack::PopAndDestroy(&session);
	}

/*
 This test function accepts 1 integer parameter as defined below
{EParamInt,100,10}
As can be seen above, the schema is incorrectly defined as iMin is
greater than iMax. This is a programming error and should result
in the server panicking with ECMPanicBadMessageSchema in UDEB
or returning KErrBadParameter in UREL
*/
void TestFunction2L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//this function is incorrectly defined in the
	//message schema. This should result in the
	//server panicking
	err = session.TestFunction2(10,100);

	//In debug builds the server should panic.
	//In release builds KErrBadParameter should be returned
#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadMessageSchema);
#else
	Test(err == KErrBadParameter);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

/*
 This test function accepts 1 integer parameter as defined below
{EParamInt,-10,-20}
As can be seen above, the schema is incorrectly defined as iMin is
greater than iMax. This is a programming error and should result
in the server panicking with ECMPanicBadMessageSchema in UDEB
or returning KErrBadParameter in UREL
*/
void TestFunction3L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//this function is incorrectly defined in the
	//message schema. This should result in the
	//server panicking
	err = session.TestFunction3(10,100);

	//In debug builds the server should panic.
	//In release builds KErrBadParameter should be returned
#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadMessageSchema);
#else
	Test(err == KErrBadParameter);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

/*
 This test function accepts 2 descriptor parameters as defined below
{EParamDes8Read,2,16},{EParamDes16Read,0,32}
Any calls to this function with parameters outside the constraints
defined above should result in the client being panicked with
KErrBadDescriptor
*/
void TestFunction4AL()
	{
	RClientMessageTestSession session;
	_LIT8(KDes8, "Des8");
	_LIT8(KShortDes8, "De");
	_LIT(KDes16, "Des16");
	_LIT(KLongDes16, "Des16 - A longer version");

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction4(KDes8,KDes16);
	Test(err == KErrNone);

	err = session.TestFunction4(KShortDes8,KLongDes16);
	Test(err == KErrNone);

	CleanupStack::PopAndDestroy(&session);
	}

/*
 This test function accepts 2 descriptor parameters as defined below
{EParamDes8Read,2,16},{EParamDes16Read,0,32}
Any calls to this function with parameters outside the constraints
defined above should result in the client being panicked with
KErrBadDescriptor
*/
void TestFunction4BL()
	{
	RClientMessageTestSession session;
	_LIT8(KDes8, "Des8");
	_LIT(KDes16, "Des16");

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//This should result in the server panicking the client
	err = session.TestFunction4(KDes16,KDes8);

	CleanupStack::PopAndDestroy(&session);
	}

/*
 This test function accepts 2 descriptor parameters as defined below
{EParamDes8Read,2,16},{EParamDes16Read,0,32}
Any calls to this function with parameters outside the constraints
defined above should result in the client being panicked with
KErrBadDescriptor
*/
void TestFunction4CL()
	{
	RClientMessageTestSession session;
	_LIT(KDes16, "Des16");
	_LIT8(KLongDes8, "This is a long Des8");

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//This should result in the server panicing the client
	err = session.TestFunction4(KLongDes8,KDes16);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters as defined below
{EParamDes8Read,2,16},{EParamDes16Read,0,32}
Any calls to this function with parameters outside the constraints
defined above should result in the client being panicked with
KErrBadDescriptor
*/
void TestFunction4DL()
	{
	RClientMessageTestSession session;
	_LIT8(KDes8, "Des8");
	_LIT(KLongDes16, "This is a long Des16 that should cause an issue");

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//This should result in the server panicing the client
	err = session.TestFunction4(KDes8,KLongDes16);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters as defined below
{EParamDes8Read,2,16},{EParamDes16Read,0,32}
Any calls to this function with parameters outside the constraints
defined above should result in the client being panicked with
KErrBadDescriptor
*/
void TestFunction4EL()
	{
	RClientMessageTestSession session;
	_LIT(KDes16, "Des16");
	_LIT8(KEmptyDes8, "");
	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//This should result in the server panicing the client
	//as the function expects a descriptor of minimum length 2
	err = session.TestFunction4(KEmptyDes8,KDes16);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters as defined below
{EParamDes8Read,2,16},{EParamDes16Read,0,32}
Any calls to this function with parameters outside the constraints
defined above should result in the client being panicked with
KErrBadDescriptor
*/
void TestFunction4FL()
	{
	RClientMessageTestSession session;
	_LIT8(KDes8, "Des8");
	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//This should result in the server panicing the client
	//as the function expects 2 descriptor arguments
	err = session.TestFunction4(KDes8);

	CleanupStack::PopAndDestroy(&session);
	}


/*
This test function accepts 2 descriptor parameters as defined below
{EParamDes8Read,2,16},{EParamDes16Read,0,32}
The server attempts to read from the first descriptor but passes
in a buffer that is too small.  This is a programming error and
should result in the server panicking with ECMPanicBadDescriptor in UDEB
and returning KErrNone in UREL
KErrBadDescriptor
*/
void TestFunction5L()
	{
	RClientMessageTestSession session;
	_LIT8(KDes8, "Des8");
	_LIT(KDes16, "Des16");

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction5(KDes8,KDes16);
#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadDescriptor);
#else
	Test(err == KErrNone);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters as defined below
{EParamDes8Read,2,16},{EParamDes16Read,0,32}
Any calls to this function with parameters outside the constraints
defined above should result in the client being panicked with
KErrBadDescriptor
*/
void TestFunction6L()
	{
	RClientMessageTestSession session;
	_LIT8(KDes8, "Des8");
	_LIT(KDes16, "Des16");

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction6(KDes8,KDes16);
	Test(err == KErrArgument);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters as defined below
{EParamDes8Read,2,16},{EParamDes16Read,0,32}
Any calls to this function with parameters outside the constraints
defined above should result in the client being panicked with
KErrBadDescriptor
*/
void TestFunction7L()
	{
	RClientMessageTestSession session;
	_LIT8(KDes8, "Descriptor8");
	_LIT(KDes16, "Descriptor16");

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction7(KDes8,KDes16);
	Test(err == KErrNone);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 1 descriptor parameter as defined below
{EParamDes8Read,0,-16}
As can be seen above, the schema is incorrectly defined as iMax is
negative. This is a programming error and should result
in the server panicking with ECMPanicBadMessageSchema in UDEB
or the client being panicked with KErrBadDescriptor in UREL
*/
void TestFunction8L()
	{
	RClientMessageTestSession session;
	_LIT8(KDes8, "Descriptor8");
	_LIT(KDes16, "Descriptor16");

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//this function is incorrectly defined in the
	//message schema. This should result in the
	//server panicking
	err = session.TestFunction8(KDes8,KDes16);

	//In debug builds the server should panic.
	//In release builds the client will be panicked
#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadMessageSchema);
#endif

	CleanupStack::PopAndDestroy(&session);
	}


/*
This test function accepts 2 descriptor parameters and 2 TInt
parameters as defined below
{EParamDes8,64,64},{EParamInt,0,64},{EParamDes16,128,128},{EParamInt,0,128}
Any calls to this function with descriptor parameters outside the constraints
defined above should result in the client being paniced with
KErrBadDescriptor or KErrOverflow
*/
void TestFunction9AL()
	{
	TBuf8<64> buf8;
	TBuf<128> buf;
	TBuf8<64> buf82;
	TBuf<128> buf2;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction9(buf8,0,buf,0);
	Test(err == KErrNone);
	Test(buf8.Find(_L8("CClientMessageTest")) == 0);
	Test(buf.Find(_L("CClientMessageTest")) == 0);

	err = session.TestFunction9(buf82,7,buf2,73);
	Test(err == KErrNone);
	Test(buf82.Find(_L8("CClientMessageTest")) == 7);
	Test(buf2.Find(_L("CClientMessageTest")) == 73);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters and 2 TInt
parameters as defined below
{EParamDes8,64,64},{EParamInt,0,64},{EParamDes16,128,128},{EParamInt,0,128}
Any calls to this function with descriptor parameters outside the constraints
defined above should result in the client being paniced with
KErrBadDescriptor
*/
void TestFunction9BL()
	{

	TBuf8<32> smallBuf8;
	TBuf<32> smallBuf;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//this should result in a  client panic with KErrBadDescriptor
	err = session.TestFunction9(smallBuf8,0,smallBuf,0);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters and 2 TInt
parameters as defined below
{EParamDes8,64,64},{EParamInt,0,64},{EParamDes16,128,128},{EParamInt,0,128}
The server function attempts to write to the supplied descriptors at the
offsets specified by the two TInt values.  In this test the server will
attempt to write to the first buffer with an offset of 60 which should
result in the client being panicked with KErrOverflow
*/
void TestFunction9CL()
	{

	TBuf8<64> buf8;
	TBuf<128> buf;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//this will attempt to write a descriptor with an offset of 60
	//which should tresult in a client panic KErrOverflow
	err = session.TestFunction9(buf8,60,buf,10);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters and 2 TInt
parameters as defined below
{EParamDes8,64,64},{EParamInt,0,64},{EParamDes16,128,128},{EParamInt,0,128}
The test function below calls this function but passed the 8 and 16 bit
descriptors in the wrong order which should result in the client being
panicked with KErrBadDescriptor
*/
void TestFunction9DL()
	{
	TBuf8<64> buf8;
	TBuf<128> buf;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//this should result in a client panic as the descriptors
	//are passed in in the wrong order
	err = session.TestFunction9(buf,0,buf8,10);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters and 2 TInt
parameters as defined below
{EParamDes8,64,64},{EParamInt,0,64},{EParamDes16,128,128},{EParamInt,0,128}
The server function attempts to write a descriptor that is larger than the
buffer supplied by the client which should result in the client being
panicked with KErrOverflow
*/
void TestFunction10L()
	{

	TBuf8<64> buf8;
	TBuf<128> buf;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//This function tries to write a descriptor
	//that is larger than the declared buffer
	//this should result in a panic
	err = session.TestFunction10(buf8,0,buf,0);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 descriptor parameters and 2 TInt
parameters as defined below
{EParamDes8,64,64},{EParamInt,0,64},{EParamDes16,128,128},{EParamInt,0,128}
The server function attempts to read from a descriptor that is smaller
than the specified minimum length.  This is a programming error and
should result in the server panicking with ECMPanicBadDescriptor in UDEB
and returning KErrNone in UREL
*/
void TestFunction11L()
	{
	_LIT8(KLongDes8,"This descriptor is longer than 32 characters");
	TBuf8<64> buf8(KLongDes8);
	TBuf<128> buf;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//This function tries to read from a descriptor
	//into a buffer that is smaller than the allowed min length
	//this should result in a server panic
	err = session.TestFunction11(buf8,0,buf,0);
#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadDescriptor);
#else
	Test(err == KErrNone);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 1 descriptor parameter as defined below
{EParamDes8,-64,64}
As can be seen above, the schema is incorrectly defined as iMin is
negative. This is a programming error and should result
in the server panicking with ECMPanicBadMessageSchema in UDEB
or returning KErrNone in UREL
*/
void TestFunction12L()
	{

	TBuf8<64> buf8;
	TBuf<128> buf;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//this function is incorrectly defined in the
	//message schema. This should result in the
	//server panicking in debug builds
	//In release builds KErrNone should be returned
	err = session.TestFunction12(buf8,0,buf,0);

#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadMessageSchema);
#else
	Test(err == KErrNone);
#endif
	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 1 descriptor parameter as defined below
{EParamDes8,64,-64}
As can be seen above, the schema is incorrectly defined as iMax is
negative. This is a programming error and should result
in the server panicking with ECMPanicBadMessageSchema in UDEB
or panicking the client with KErrBadDescriptor in UREL
*/
void TestFunction13L()
	{

	TBuf8<64> buf8;
	TBuf<128> buf;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//this function is incorrectly defined in the
	//message schema. This should result in the
	//server panicking in debug builds
	err = session.TestFunction13(buf8,0,buf,0);

#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadMessageSchema);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 8bit descriptor parameters and 1 TInt
parameter as defined below
{EParamDes8Read,0,16},{EParamInt,0,16},{EParamDes8,16,16}
The server function attempts reads from the first descriptor and
then writes into the second descriptor at the offset supplied by
the TInt value.  The test function below checks that the reading
and writing with offset occurs as expected by checking the strings
written by the server.
*/
void TestFunction14L()
	{

	_LIT8(KDes8, "Descriptor8");
	TBuf8<16> buf;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction14(KDes8,0,buf);
	Test(err == KErrNone);
	Test(buf.Find(_L8("Descriptor8")) == 0);

	err = session.TestFunction14(KDes8,3,buf);
	Test(err == KErrNone);
	Test(buf.Find(_L8("criptor8")) == 0);

	err = session.TestFunction14(KDes8,8,buf);
	Test(err == KErrNone);
	Test(buf.Find(_L8("or8")) == 0);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts 2 16bit descriptor parameters and 1 TInt
parameter as defined below
{EParamDes16Read,0,64},{EParamInt,0,64},{EParamDes16,64,64}
The server function attempts reads from the first descriptor and
then writes into the second descriptor at the offset supplied by
the TInt value.  The test function below checks that the reading
and writing with offset occurs as expected by checking the strings
written by the server.
*/
void TestFunction15L()
	{

	_LIT(KDes, "Descriptor");
	TBuf<64> buf;

	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction15(KDes,0,buf);
	Test(err == KErrNone);
	Test(buf.Find(_L("Descriptor")) == 0);

	err = session.TestFunction15(KDes,3,buf);
	Test(err == KErrNone);
	Test(buf.Find(_L("criptor")) == 0);

	err = session.TestFunction15(KDes,8,buf);
	Test(err == KErrNone);
	Test(buf.Find(_L("or")) == 0);

	CleanupStack::PopAndDestroy(&session);
	}


/*
This test function accepts a single TAny* parameter as below
{EParamPtr,0,0}
The server function reads the pointer value.
*/
void TestFunction16L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction16(0);
	Test(err == KErrNone);

	CleanupStack::PopAndDestroy(&session);
	}


/*
This test function accepts a single TAny* parameter as below
{EParamPtr,0,0}
The server function attempts to call CMessageParameterBase::WriteL
which is not defined for a Ptr type. This is a programming error and
should result in the server panicking with ECMPanicWrongParameterType
in UDEB or returning KErrWrongParameterType in UREL
*/
void TestFunction17L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	//This should result in the server panicking with ECMPanicWrongParameterType
	//in UDEB or returning KErrWrongParameterType in UREL
	err = session.TestFunction17(0);
#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicWrongParameterType);
#else
	Test(err == KErrWrongParameterType);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts a single custom TPckg parameter as below
{EParamCustom1,sizeof(CMTestStruct2),sizeof(CMTestStruct2)}.
CMTestStruct2 is defined as follows:

struct CMTestStruct1
	{
	TInt iInt;
	TBuf<32> iDes;
	};

struct CMTestStruct2
	{
	TInt iCount;
	CMTestStruct1* iStruct;
	TCMTestEnum iEnum;
	TBuf<32> iDes;
	};

The EParamCustom1 parameter type is validated using a custom validation
function defined in the testclientmessageschema.cpp.  The validation
function checks that iEnum is in the valid range for TCMTestEnum,
that iCount >= 0 and that iDes.Length() > 0.  If any of these checks
fail the server should return KErrBadParameter.
*/
void TestFunction18AL()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	CMTestStruct1 struct1;
	struct1.iInt = 7;
	struct1.iDes = _L("Test Struct");

	CMTestStruct2 struct2;
	struct2.iCount = 1;
	struct2.iStruct = &struct1;
	struct2.iEnum = ETestVal1;
	struct2.iDes = _L("Test Struct 2");

	TPckg<CMTestStruct2> pData(struct2);

	err = session.TestFunction18(pData);
	Test(err == KErrNone);

	Test(struct2.iCount == 0);

	CMTestStruct1 struct3;
	struct3.iInt = -1;
	struct3.iDes = _L("");

	CMTestStruct2 struct4;
	struct4.iCount = 1;
	struct4.iStruct = &struct3;
	struct4.iEnum = -1;
	struct4.iDes = _L("Test Struct 2");

	TPckg<CMTestStruct2> pData2(struct4);

	//As iEnum is not in the valid range this validation should fail
	err = session.TestFunction18(pData2);
	Test(err == KErrBadParameter);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts a single custom TPckg parameter as below
{EParamCustom1,sizeof(CMTestStruct2),sizeof(CMTestStruct2)}.
CMTestStruct2 is defined above.
The test function below passes in a TPckg<CMTestStruct1> which is an
incorrect type.  The server should return KErrBadDescriptor
*/
void TestFunction18BL()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	CMTestStruct1 struct1;
	struct1.iInt = 0;
	struct1.iDes = _L("Test Struct");

	TPckg<CMTestStruct1> pData(struct1);

	//This should result in a panic
	err = session.TestFunction18(pData);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts a single custom TPckg parameter as below
{EParamCustom1,sizeof(CMTestStruct2),sizeof(CMTestStruct2)}.
CMTestStruct2 is defined above.
The test function below passes in an Empty TBuf the same size as a
CMTestStruct2. As this wont contain any valid data, the server should
panic the client with KErrBadParameter.
*/
void TestFunction18CL()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	TBuf<sizeof(CMTestStruct2)> buf;
	buf.SetLength(sizeof(CMTestStruct2));

	//This should result in a panic
	err = session.TestFunction18(buf);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts a single custom TPckg parameter as below
{EParamCustom2,sizeof(CMTestStruct1),sizeof(CMTestStruct1)}.
CMTestStruct1 is defined as follows:

struct CMTestStruct1
	{
	TInt iInt;
	TBuf<32> iDes;
	};

The EParamCustom1 parameter type is validated using a custom validation
function defined in the testclientmessageschema.cpp.  The validation
function checks that iInt is > 0.  If this check fails the server
should return KErrBadParameter.
The test function below passes the data to the server in a TPckgBuf
*/
void TestFunction19L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	CMTestStruct1 struct1;
	struct1.iInt = 4;
	struct1.iDes = _L("Test Struct");

	TPckgBuf<CMTestStruct1> pData(struct1);

	err = session.TestFunction19(pData);
	Test(err == KErrNone);

	struct1.iInt = -4;
	TPckgBuf<CMTestStruct1> pData2(struct1);

	err = session.TestFunction19(pData2);
	Test(err == KErrBadParameter);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function does not expect any parameters.
The schema defines a single placeholder parameter
{EParamNull,0,0}
but the number of parameters is set to 0
*/
void TestFunction25L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction25();
	Test(err == KErrNone);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function does not expect any parameters.
The schema defines a single placeholder parameter
{EParamNull,0,0}
The schema is incorrectly defined as the number of parameters is
listed as 1 instead of 0.  This is a programming error and should result
in the server panicking with ECMPanicBadMessageSchema in UDEB
or the client being panicked with KErrBadDescriptor in UREL
*/
void TestFunction26L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction26();
	//In debug builds the server should panic.
	//In release builds KErrBadParameter should be returned
#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadMessageSchema);
#else
	Test(err == KErrBadMessageSchema);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function does not expect any parameters.
The schema defines a single placeholder parameter
{453,0,0}
The schema is incorrectly defined as it contains an invalid
parameter type. This is a programming error and should result
in the server panicking with ECMPanicBadMessageSchema in UDEB
or returning KErrBadParameter in UREL
*/
void TestFunction27L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction27();
	//In debug builds the server should panic.
	//In release builds KErrBadMessageSchema should be returned
#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadMessageSchema);
#else
	Test(err == KErrBadMessageSchema);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts a single integer parameter with a
custom validation function.
{EParamCustom3,0,0}
The validation functions checks that the value passed in is a
multiple of 4.
Any calls to this function with a parameter that is not divisible
by 4 should fail with KErrBadParameter
*/
void TestFunction28L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction28(4);
	Test(err == KErrNone);

	err = session.TestFunction28(128);
	Test(err == KErrNone);

	err = session.TestFunction28(73);
	Test(err == KErrBadParameter);

	err = session.TestFunction28(-1);
	Test(err == KErrBadParameter);

	err = session.TestFunction28(44444444);
	Test(err == KErrNone);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts a single descriptor parameter with a
custom validation function.
{EParamCustom4,0,0}
The validation functions checks that the descriptor passed in
contains the string 'Expected Data'
Any calls to this function with a descriptor that is not 'Expected Data'
should fail with KErrBadParameter
*/
void TestFunction29L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	TBuf<32> data = _L("Expected Data");

	err = session.TestFunction29(data);
	Test(err == KErrNone);

	data.SetLength(5);

	err = session.TestFunction29(data);
	Test(err == KErrBadParameter);

	data = _L("expected data");

	err = session.TestFunction29(data);
	Test(err == KErrBadParameter);

	data.Zero();

	err = session.TestFunction29(data);
	Test(err == KErrBadParameter);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts a single custom TPckg parameter as below
{EParamPckg,sizeof(CMTestStruct1),sizeof(CMTestStruct1)}.
CMTestStruct1 is defined as follows:

struct CMTestStruct1
	{
	TInt iInt;
	TBuf<32> iDes;
	};

The EParamCustom1 parameter type is validated using the default validation
function.  This checks that the size of the TPckg passed in is
exactly that defined in the schema. If this check fails the server
should panic the client with KErrBadDescriptor.
*/
void TestFunction30L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	CMTestStruct1 struct1;
	struct1.iInt = 4;
	struct1.iDes = _L("Test Struct");

	TPckg<CMTestStruct1> pData(struct1);

	err = session.TestFunction30(pData);
	Test(err == KErrNone);

	TBuf8<16> data;
	//This should cause a panic
	err = session.TestFunction30(data);

	CleanupStack::PopAndDestroy(&session);
	}

/*
This test function accepts an Intergeer parameter and a TAny* parameter
{EParamInt,EGetIntL,EWrite16L},{EParamPtr,0,0}

The server function checks the value passed in the TInt parameter and calls
an appropriate CMessageParameterBase function on the second parameter.
As none of the functions called are defined for a pointer parameter, the
call defaults to the CMessageParameterBase implementation which panics the
client with ECMPanicWrongParameterType in UDEB or returns
KErrWrongParameterType in UREL
*/
void DoTest31L(TInt aType)
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	err = session.TestFunction31(aType);
#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicWrongParameterType);
#else
	Test(err == KErrWrongParameterType);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

void TestFunction31AL()
	{
	DoTest31L(EGetIntL);
	}

void TestFunction31BL()
	{
	DoTest31L(EGetDes8L);
	}

void TestFunction31CL()
	{
	DoTest31L(EGetDes16L);
	}

void TestFunction31DL()
	{
	DoTest31L(EGetDesLengthL);
	}

void TestFunction31EL()
	{
	DoTest31L(EGetDesMaxLengthL);
	}

void TestFunction31FL()
	{
	DoTest31L(ERead8L);
	}

void TestFunction31GL()
	{
	DoTest31L(ERead16L);
	}

void TestFunction31HL()
	{
	DoTest31L(EWrite8L);
	}

void TestFunction31IL()
	{
	DoTest31L(EWrite16L);
	}
/*
This test function accepts a single custom parameter as below
{EParamCustom5,sizeof(CMTestStruct1),sizeof(CMTestStruct1)}
CMTestStruct1 is defined as follows:

struct CMTestStruct1
	{
	TInt iInt;
	TBuf<32> iDes;
	};

The EParamCustom5 parameter type enum value is  defined as
(0x90000 | EParamPckg).  The upper 16 bits are the index of
the validation function in the array supplied by the server.
The array only contains 5 values so the above definition is
incorrect.  Calling this messge should result in the server
panicking with ECMPanicBadMessageSchema in UDEB or returning
KErrBadMessageSchema in UREL
*/
void TestFunction32L()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	CMTestStruct1 struct1;
	TPckg<CMTestStruct1> pData(struct1);

	err = session.TestFunction32(pData);

#ifdef _DEBUG
	Test(err == KErrServerTerminated);

	TInt type = serverProcess.ExitType();
	Test(type == EExitPanic);

	TInt reason = serverProcess.ExitReason();
	Test(reason == ECMPanicBadMessageSchema);
#else
	Test(err == KErrBadMessageSchema);
#endif

	CleanupStack::PopAndDestroy(&session);
	}

/* Connect to the CClientMessageTestServer, launching
 * the server process if necessary
 */
TInt RClientMessageTestSession2::Connect()
	{
	TInt retry = 2;
	for(;;)
		{
		TInt r = CreateSession(KServer2Name,TVersion(1,0,0));

		if((r != KErrNotFound)&&(r != KErrServerTerminated))
			{
			return r;
			}

		if(--retry == 0)
			{
			return r;
			}

		r = LaunchServer();
		if((r != KErrNone)&&(r != KErrAlreadyExists))
			{
			return r;
			}
		}
	}



/* These functions are used to send IPC messages to the server.
 * The messages are sent with different parameters to test the
 * ClientMessage framework handling of bad messages
 */
TInt RClientMessageTestSession2::TestFunction0(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ETestMessage0,TIpcArgs(aArg0,aArg1));
	}

TInt RClientMessageTestSession2::TestFunction1(const TDesC& aData0)
	{
	return SendReceive(ETestMessage1,TIpcArgs(&aData0));
	}


/**
@SYMTestCaseID SYSLIB-BAFL-CT-4040
@SYMTestCaseDesc Tests CClientMessage handling of EParamInt type parameters
@SYMTestPriority High
@SYMTestActions Call a series of test functions expecting integer parameters.
				Various argument values and types are passed to these functions.
				The Server should only accept messages containing integer
				values that fall within the bounds defined in the message schema.
@SYMTestExpectedResults The Server should return KErrNone if the message is
				accepted or KErrBadParameter if the int parameters are outside
				the bounds defined in the schema.  If the message is incorrectly
				defined in the mssage schema or the server code incorrectly uses
				the parameter type the server should panic.
@SYMDEF         INC117370
*/
void DoIntParameterTestsL()
	{
	TExitDetails exitDetails;

	exitDetails = LaunchTestThreadL(_L("TestFunction0"), &TestFunction0AL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction1"), &TestFunction1L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction2"), &TestFunction2L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction3"), &TestFunction3L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction28"), &TestFunction28L);
	Test(exitDetails.iExitType == EExitKill);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4041
@SYMTestCaseDesc Tests CClientMessage handling of EParamDes8Read and
				EParamDes16Read type parameters
@SYMTestPriority High
@SYMTestActions Call a series of test functions expecting Des*Read parameters.
				Various argument values and types are passed to these functions.
				The Server should only accept messages containing Des*Read
				values that fall within the bounds defined in the message schema.
@SYMTestExpectedResults The Server should return KErrNone if the message is
				accepted or panic the client with KErrBadDescriptor if the
				descriptor parameters are outside the bounds defined in the schema.
				If the message is incorrectly defined in the mssage schema or the
				server code incorrectly uses the parameter type the server should panic.
@SYMDEF         INC117370
*/
void DoDesReadParameterTestsL()
	{
	TExitDetails exitDetails;

	exitDetails = LaunchTestThreadL(_L("TestFunction4A"), &TestFunction4AL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction4B"), &TestFunction4BL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction4C"), &TestFunction4CL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction4D"), &TestFunction4DL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction4E"), &TestFunction4EL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction4F"), &TestFunction4FL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction5"), &TestFunction5L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction6"), &TestFunction6L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction7"), &TestFunction7L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction8"), &TestFunction8L);
#ifdef _DEBUG
	Test(exitDetails.iExitType == EExitKill);
#else
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);
#endif

	exitDetails = LaunchTestThreadL(_L("TestFunction29"), &TestFunction29L);
	Test(exitDetails.iExitType == EExitKill);

	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4042
@SYMTestCaseDesc Tests CClientMessage handling of EParamDes8 and
				EParamDes16 type parameters
@SYMTestPriority High
@SYMTestActions Call a series of test functions expecting Des* parameters.
				Various argument values and types are passed to these functions.
				The Server should only accept messages containing Des*
				values that fall within the bounds defined in the message schema.
@SYMTestExpectedResults The Server should return KErrNone if the message is
				accepted or panic the client with KErrBadDescriptor or KErrOverflow if the
				descriptor parameters are outside the bounds defined in the schema.
				If the message is incorrectly defined in the mssage schema or the
				server code incorrectly uses the parameter type the server should panic.
@SYMDEF         INC117370
*/
void DoDesParameterTestsL()
	{
	TExitDetails exitDetails;
	exitDetails = LaunchTestThreadL(_L("TestFunction9A"), &TestFunction9AL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction9B"), &TestFunction9BL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction9C"), &TestFunction9CL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrOverflow);

	exitDetails = LaunchTestThreadL(_L("TestFunction9D"), &TestFunction9DL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction10"), &TestFunction10L);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrOverflow);

	exitDetails = LaunchTestThreadL(_L("TestFunction11"), &TestFunction11L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction12"), &TestFunction12L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction13"), &TestFunction13L);
#ifdef _DEBUG
	Test(exitDetails.iExitType == EExitKill);
#else
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);
#endif

	exitDetails = LaunchTestThreadL(_L("TestFunction14"), &TestFunction14L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction15"), &TestFunction15L);
	Test(exitDetails.iExitType == EExitKill);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4043
@SYMTestCaseDesc Tests CClientMessage handling of EParamPtr type parameters
@SYMTestPriority High
@SYMTestActions Call test functions accepting ptr parameter types
@SYMTestExpectedResults The Server should return KErrNone if the message is
				accepted. If the message is incorrectly defined in the message
				schema or the server code incorrectly uses the parameter type
				the server should panic
				expected
@SYMDEF         INC117370
*/
void DoPtrParameterTestsL()
	{
	TExitDetails exitDetails;
	exitDetails = LaunchTestThreadL(_L("TestFunction16"), &TestFunction16L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction17"), &TestFunction17L);
	Test(exitDetails.iExitType == EExitKill);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4044
@SYMTestCaseDesc Tests CClientMessage handling of EParamPckg type parameters
@SYMTestPriority High
@SYMTestActions Call a series of test functions expecting Pckg parameters.
				Various argument values and types are passed to these functions.
				The Server should only accept messages containing Pckg
				values that fall within the bounds defined in the message schema.
@SYMTestExpectedResults The Server should return KErrNone if the message is
				accepted, return KErrBadParameter if the packaged parameter is
				outside the bounds described in the schema or panic the client with
				KErrBadDescriptor if the descriptor parameter is outside the bounds
				defined in the schema.
@SYMDEF         INC117370
*/
void DoPckgParameterTestsL()
	{
	TExitDetails exitDetails;
	exitDetails = LaunchTestThreadL(_L("TestFunction18A"), &TestFunction18AL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction18B"), &TestFunction18BL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction18C"), &TestFunction18CL);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction19"), &TestFunction19L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction30"), &TestFunction30L);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);

	exitDetails = LaunchTestThreadL(_L("TestFunction32"), &TestFunction32L);
	Test(exitDetails.iExitType == EExitKill);
	}


/**
@SYMTestCaseID SYSLIB-BAFL-CT-4045
@SYMTestCaseDesc Tests CClientMessage handling of EParamNull type parameters
@SYMTestPriority High
@SYMTestActions Call a series of test functions expecting no parameters.
@SYMTestExpectedResults The Server should return KErrNone if the message is
				accepted. If the message is incorrectly defined in the message
				schema or the server code incorrectly uses the parameter type
				the server should panic.
@SYMDEF         INC117370
*/
void DoNullParameterTestsL()
	{

	TExitDetails exitDetails;
	exitDetails = LaunchTestThreadL(_L("TestFunction25"), &TestFunction25L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction26"), &TestFunction26L);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction27"), &TestFunction27L);
	Test(exitDetails.iExitType == EExitKill);

	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4046
@SYMTestCaseDesc Tests CClientMessage handling of Security policies
@SYMTestPriority High
@SYMTestActions Call several test functions with varying security policies and
				verify that the ClientMessage framework handles each case as
				expected
@SYMTestExpectedResults The server should return KErrNone if the client has
				the required security policy, otherwise return KErrPermissionDenied
@SYMDEF         INC117370
*/
void DoSecurityPolicyTestsL()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();

	CleanupClosePushL(session);

	//This function is incorrectly defined in the message table
	//for this server and so the CClientMessage Object creation will fail
	//This should reult in the server panicking
	RDebug::Print(_L("Testing Always Fail Policy..."));
	err = session.TestFunction20(0,0);
	Test(err == KErrPermissionDenied);

	RDebug::Print(_L("Testing Valid SID Policy..."));
	err = session.TestFunction21(0,0);
	Test(err == KErrNone);

	RDebug::Print(_L("Testing Invalid SID Policy..."));
	err = session.TestFunction22(0,0);
	Test(err == KErrPermissionDenied);

	RDebug::Print(_L("Testing Valid Capability Policy..."));
	err = session.TestFunction23(0,0);
	Test(err == KErrNone);

	RDebug::Print(_L("Testing Invalid Capability Policy..."));
	err = session.TestFunction24(0,0);
	Test(err == KErrPermissionDenied);

	CleanupStack::PopAndDestroy(&session);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4047
@SYMTestCaseDesc Tests CClientMessage handling of undefined messagess
@SYMTestPriority High
@SYMTestActions Call a test function which sends an unknown message request to the
				server.
@SYMTestExpectedResults The ClientMessage framework should return KErrInvalidFunction
@SYMDEF         INC117370
*/
void DoInvalidMessageTestsL()
	{
	RClientMessageTestSession session;

	TInt err = session.Connect();

	CleanupClosePushL(session);

	//This function is not defined in the message table
	//for this server and so the CClientMessage Object creation will fail
	//This should result in the server panicking
	err = session.TestInvalidFunction(45);
	Test(err == KErrInvalidFunction);

	err = session.TestInvalidFunction(5);
	Test(err == KErrInvalidFunction);

	CleanupStack::PopAndDestroy(&session);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4050
@SYMTestCaseDesc Tests CMessageParameterBase default implementation of message access functions
@SYMTestPriority High
@SYMTestActions Calls test functions which signal the server to call one
				of the Message access methods on a EParamPtr parameter type.
				As none of these methods are explicitly defined for a EParamPtr
				type the call will be delegated to the base class implementation
				which will panic the client in UDEB and return an error in UREL
@SYMTestExpectedResults The server should panic the client with ECMPanicWrongParameterType
				in UDEB and return KErrWrongParameterType in UREL
@SYMDEF         INC117370
*/
void DoCMessageParameterBaseTestL()
	{

	TExitDetails exitDetails;
	exitDetails = LaunchTestThreadL(_L("TestFunction31A"), &TestFunction31AL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction31B"), &TestFunction31BL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction31C"), &TestFunction31CL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction31D"), &TestFunction31DL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction31E"), &TestFunction31EL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction31F"), &TestFunction31FL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction31G"), &TestFunction31GL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction31H"), &TestFunction31HL);
	Test(exitDetails.iExitType == EExitKill);

	exitDetails = LaunchTestThreadL(_L("TestFunction31I"), &TestFunction31IL);
	Test(exitDetails.iExitType == EExitKill);

	}


/**
@SYMTestCaseID SYSLIB-BAFL-CT-4048
@SYMTestCaseDesc Tests CClientMessage handling of multiple servers
@SYMTestPriority High
@SYMTestActions Calls test functions which pass the same message to two different
 				servers.  The servers have different message schemas and data is
 				passed to the server to check that the correct schema is used
 				for the given server.
@SYMTestExpectedResults The ClientMessage framework should validate the message
						against the correct schema for the given server
@SYMDEF         INC117370
*/
void DoMultipleServerTestsL()
	{

	RClientMessageTestSession session1;
	RClientMessageTestSession2 session2;

	TInt err = session1.Connect();
	Test(err == KErrNone);
	CleanupClosePushL(session1);

	err = session2.Connect();
	Test(err == KErrNone);
	CleanupClosePushL(session2);

	err = session1.TestFunction0( 0, 150);
	Test(err == KErrNone);

	err = session2.TestFunction0( 0, 150);
	Test(err == KErrBadParameter);

	err = session1.TestFunction0( 0, -50);
	Test(err == KErrBadParameter);

	err = session2.TestFunction0( 0, -50);
	Test(err == KErrNone);

	CleanupStack::PopAndDestroy(2,&session1);

	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4051
@SYMTestCaseDesc Tests CClientMessage allowing disabling of Panic.
@SYMTestPriority Medium
@SYMTestActions Calls test functions which pass a message to a server for which the 
				ESrvFlagDoNotPanicClientOnBadMessageErrors flag is set.Then invokes the client	API with 
				incorrect arguments to create an error scenario of KErrBadDescriptor.
@SYMTestExpectedResults The ClientMessage framework should check for the above flag, and in case 
			its not set simply return an error instead of panicing the client.
@SYMDEF         DEF125501
*/
void DoDisablePanicTestsL()
	{
	
	RClientMessageTestSession2 session;
	_LIT(KDes16, "Descriptor");
	
	TInt err = session.Connect();
	Test(err == KErrNone);

	CleanupClosePushL(session);

	/* According to schema defined, this function accepts TDesC8 only.
	Validation failure occurs.Server should check if flag ESrvFlagDoNotPanicClientOnBadMessageErrors
	is set, and either return with KErrBadDescriptor or Panic the client.
	In this case the above flag is set for CClientMessageTestServer2.
	*/
	err = session.TestFunction1(KDes16);
	
#ifdef _DEBUG
	Test(err == KErrBadDescriptor);

	// Server is not terminated, still alive
	TInt type = serverProcess.ExitType();
	Test(type == EExitPending);
#else
	Test(err == KErrBadDescriptor);
#endif

	CleanupStack::PopAndDestroy(&session);
	
	}


static void DoTestsL()
	{

	TExitDetails exitDetails;

	Test.Title ();
	Test.Start (_L("ClientMessage Tests"));

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4040 Testing Message with Int Parameters "));
	DoIntParameterTestsL();

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4041 Testing Message with Read Only Des Parameters "));
	DoDesReadParameterTestsL();

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4042 Testing Message with Read/Write Des Parameters "));
	DoDesParameterTestsL();

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4043 Testing Message with Ptr Parameters "));
	DoPtrParameterTestsL();

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4044 Testing Message with Pckg Parameters "));
	DoPckgParameterTestsL();

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4045 Testing Message with Null Parameters "));
	DoNullParameterTestsL();

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4046 Testing Security Policy Checking "));
	exitDetails = LaunchTestThreadL(_L("SecurityPolicyTestL"), &DoSecurityPolicyTestsL);
	Test(exitDetails.iExitType == EExitKill);

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4047 Testing Invalid Message "));
	exitDetails = LaunchTestThreadL(_L("InvalidMessageTestL"), &DoInvalidMessageTestsL);
	Test(exitDetails.iExitType == EExitKill);

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4050 Testing CMessageParameterBase Default Implementations "));
	exitDetails = LaunchTestThreadL(_L("CMessageParameterBaseTestL"), &DoCMessageParameterBaseTestL);
	Test(exitDetails.iExitType == EExitKill);

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4048 Testing Multiple Servers "));
	exitDetails = LaunchTestThreadL(_L("MultipleServerTestingL"), &DoMultipleServerTestsL);
	Test(exitDetails.iExitType == EExitKill);

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4051 Testing Disable Client Panic"));
	exitDetails = LaunchTestThreadL(_L("DisablePanicTesting"), &DoDisablePanicTestsL);
	Test(exitDetails.iExitType == EExitKill);
	
	
	Test.Printf(_L("Tests Completed O.K."));

	Test.End();
	Test.Close();

	}


TInt E32Main()
   //
   // Server process entry-point
   //
   	{
   	//
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt r=KErrNoMemory;
   	if (cleanup)
   		{
 		TRAP(r,DoTestsL());
   		delete cleanup;
   		}
   	//
   	return r;
   	}
