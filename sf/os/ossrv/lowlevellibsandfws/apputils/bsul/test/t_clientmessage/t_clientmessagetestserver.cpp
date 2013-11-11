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

#include "t_clientmessagetestserver.h"


_LIT(KServerName,"ClientMessageTestServer");
_LIT(KServer2Name,"SecondTestServer");
extern const BSUL::TClientMessageServerData KServerData;
extern const BSUL::TClientMessageServerData KServer2Data;

/* CClientMessageTestServer is a lightweight Server that is used to test
 * the CClientMessage framework.  Clientside code sends messages to the 
 * server which are processed and the client chceks the expected behaviour.
 * The server code does no testing itself.
 */
CServer2* CClientMessageTestServer::NewLC()
	{
	CClientMessageTestServer* self = new(ELeave) CClientMessageTestServer(CActive::EPriorityStandard);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	
	}

void CClientMessageTestServer::ConstructL()
	{
	BSUL::CClientMessage::InitialiseFrameworkL(KServerData);
	StartL(KServerName);
	}

TInt CClientMessageTestServer::RunError(TInt aError)
	{
	Message().Complete(aError);
	
	ReStart();
	return KErrNone;
	}

CClientMessageTestServer::CClientMessageTestServer(TInt aPriority):
		CServer2(aPriority)
	{	
	}

CSession2* CClientMessageTestServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{

	return new(ELeave)CClientMessageTestSession;
	
	}

/* CClientMessageTestSession is a lightweight Session class processes
 * messages from the client using the CClientMessage framework 
 */

CClientMessageTestSession::~CClientMessageTestSession()
	{
	delete iClientMessage;
	}

void CClientMessageTestSession::CreateL(const CServer2& /*aServer*/)
	{

	}


void CClientMessageTestSession::ServiceL(const RMessage2& aMessage)
	{
	
	iClientMessage = BSUL::CClientMessage::NewL(aMessage);
	
	TRAPD(err, iClientMessage->ValidateL());
	if(err == KErrNone)
		{
		TRAP(err, DoServiceL());
		}

	iClientMessage->CompleteRequestL(err);
	
	delete iClientMessage;
	iClientMessage = NULL;

	}

void CClientMessageTestSession::DoServiceL()
	{
	
	
	switch(iClientMessage->Function())
		{
		case ETestMessage0:
		case ETestMessage21:
		case ETestMessage23:
			{
			TestFunction0L();
			break;
			}
			
		case ETestMessage1:
			{
			TestFunction1L();
			break;
			}
			
		case ETestMessage4:
			{
			TestFunction4L();
			break;
			}
			
		case ETestMessage5:
			{
			TestFunction5L();
			break;
			}
		
		case ETestMessage6:
			{
			TestFunction6L();
			break;
			}
			
		case ETestMessage7:
			{
			TestFunction7L();
			break;
			}
			
		case ETestMessage9:
			{
			TestFunction9L();
			break;
			}
			
		case ETestMessage10:
			{
			TestFunction10L();
			break;
			}
			
		case ETestMessage11:
			{
			TestFunction11L();
			break;
			}
			
		case ETestMessage14:
			{
			TestFunction14L();
			break;
			}
			
		case ETestMessage15:
			{
			TestFunction15L();
			break;
			}
			
		case ETestMessage16:
			{
			TestFunction16L();
			break;
			}
			
		case ETestMessage17:
			{
			TestFunction17L();
			break;
			}
			
		case ETestMessage18:
			{
			TestFunction18L();
			break;
			}
			
		case ETestMessage19:
			{
			TestFunction19L();
			break;
			}
			
		case ETestMessage31:
			{
			TestFunction31L();
			break;
			}
			
		//Just testing the validation of these functions.
		//No action to be taken
		case ETestMessage12:
		case ETestMessage13:
		case ETestMessage25:
		case ETestMessage26:
		case ETestMessage27:
		case ETestMessage28:
		case ETestMessage29:
		case ETestMessage30:
		case ETestMessage32:
			{
			break;
			}
			
		//This should never happen because these functions have been
		//incorrectly defined in the message schema and calling these
		//functions should result in a server panic when creating the
		//CClientMessage object using the message.		
		case ETestMessage2:
		case ETestMessage3:
		case ETestMessage8:
		//These messages should never be called as they have a failing security policy
		//for the client server
		case ETestMessage20:
		case ETestMessage22:
		case ETestMessage24:
			User::Panic(_L("ClientMessageTest"),0);
			break;
			
				
		}
	}


/*The test functions below read from or write to the 
 *clients message using the CCLientMessage framework.
 */

/*
 * Int value tests
 */

void CClientMessageTestSession::TestFunction0L()
	{	
	TInt arg0;
	TInt arg1;
	
	//Validate the first parameter
	iClientMessage->ValidateL(0);
	iClientMessage->ValidateL(1);
	
	TRAPD(err,iClientMessage->ValidateL(2));
	
	if(err != KErrArgument)
		{
		User::Leave(KErrGeneral);
		}
		
	//read the data from the clients message
	arg0 = iClientMessage->GetIntL(0);
	arg1 = iClientMessage->GetIntL(1);
	
	TRAP(err,iClientMessage->GetIntL(-1));
	
	if(err != KErrArgument)
		{
		User::Leave(KErrGeneral);
		}
	
	TRAP(err,iClientMessage->GetIntL(34));
	
	if(err != KErrArgument)
		{
		User::Leave(KErrGeneral);
		}
	
	//Just use the variables to remove compiler warnings
	arg0 += arg1;
	}

void CClientMessageTestSession::TestFunction1L()
	{
	TInt arg0;
	const TAny* ptr0;
	
	arg0 = iClientMessage->GetIntL(0);
	//Trying to read this argument for function 1 should fail as function 1
	//has been defined as having 2 int parameters
	ptr0 = iClientMessage->GetPtrL(1);	
	
	//Just use the variables to remove compiler warnings
	if(ptr0!= NULL)
		{
		arg0++;
		}
	}


/*
 * Read Only Descriptor tests
 */

void CClientMessageTestSession::TestFunction4L()
	{
	TBuf8<16> des8;
	TBuf<32> des16;
	
	//Read the data from the clients message
	iClientMessage->ReadL(0,des8);
	iClientMessage->ReadL(1,des16);
	}

void CClientMessageTestSession::TestFunction5L()
	{
	TBuf8<3> des8;
	TBuf<32> des16;
	
	//This buffer is not large enough to hold the client message
	//which has a defined length of 16.  Attempting to read into this should fail
	iClientMessage->ReadL(0,des8);
	iClientMessage->ReadL(1,des16);
	}

void CClientMessageTestSession::TestFunction6L()
	{
	TBuf8<16> des8;
	TBuf<32> des16;
	//This offset is negative. Attempting to read into this should fail
	iClientMessage->ReadL(0,des8,-1);
	iClientMessage->ReadL(1,des16);
	}

void CClientMessageTestSession::TestFunction7L()
	{
	TBuf8<16> des8;
	TBuf<32> des16;
	//This offset is towards the end of the string - should be ok
	//but only a partial string should be returned
	iClientMessage->ReadL(0,des8,7);
	iClientMessage->ReadL(1,des16);
	}

/*
 * Read\Write Descriptor tests
 */

void CClientMessageTestSession::TestFunction9L()
	{
	_LIT8(KDes8,"CClientMessageTest");
	_LIT(KDes,"CClientMessageTest");
	TInt offSet0;
	TInt offSet1;
	
	//Read in the offets
	offSet0 = iClientMessage->GetIntL(1);
	offSet1 = iClientMessage->GetIntL(3);
	
	//Write the data to the clients message at the given offset
	iClientMessage->WriteL(0,KDes8,offSet0);
	iClientMessage->WriteL(2,KDes,offSet1);
	}

void CClientMessageTestSession::TestFunction10L()
	{
	_LIT8(KDes8,"CClientMessageTest. This Descriptor is larger than the maximum allowed descriptor");
	_LIT(KDes,"CClientMessageTest");
	TInt offSet0;
	TInt offSet1;
	
	offSet0 = iClientMessage->GetIntL(1);
	offSet1 = iClientMessage->GetIntL(3);
	
	//Thi descriptor is too large and this should fail
	iClientMessage->WriteL(0,KDes8,offSet0);
	iClientMessage->WriteL(2,KDes,offSet1);
	}

void CClientMessageTestSession::TestFunction11L()
	{
	TBuf8<32> des8;
	TBuf<128> des16;
	
	//This buffer is not large enough to hold the client message
	//which has a defined length of 64.  
	//Attempting to read into this should fail on UDEB
	iClientMessage->ReadL(0,des8);
	iClientMessage->ReadL(2,des16);
	}

void CClientMessageTestSession::TestFunction14L()
	{
	TInt offSet;
	TBuf8<16> des8;
	
	offSet = iClientMessage->GetIntL(1);
	
	//Read the data from the clients descriptor using the offset provided
	TInt length = iClientMessage->GetDesLengthL(0);
	
	//Get a reference to the local copy of the descriptor
	const TDesC8& aDes = iClientMessage->GetDes8L(0);
	
	//Just do a comparison here to sure the variables and avoid compiler warnings
	if((length == aDes.Length()) && (length <= des8.MaxLength()))
		{
		iClientMessage->ReadL(0,des8,offSet);
		}
	
	//Write what was read back to the client
	iClientMessage->WriteL(2,des8);
	}

void CClientMessageTestSession::TestFunction15L()
	{
	TInt offSet;
	TBuf<16> des;
	
	offSet = iClientMessage->GetIntL(1);
	
	//read from the second descriptor
	TInt length = iClientMessage->GetDesLengthL(2);
	iClientMessage->ReadL(2,des,0);
	
	//Read the data from the clients descriptor using the offset provided
	length = iClientMessage->GetDesLengthL(0);
	
	//Get a reference to the local copy of the descriptor
	const TDesC& aDes = iClientMessage->GetDes16L(0);
	
	//Just do a comparison here to sure the variables and avoid compiler warnings
	if((length == aDes.Length()) && (length <= des.MaxLength()))
		{
		iClientMessage->ReadL(0,des,offSet);
		
		}
	//Write what was read back to the client
	if( length <= iClientMessage->GetDesMaxLengthL(2))
		{
		iClientMessage->WriteL(2,des);
		}
	}

/*
 * Ptr tests
 */

void CClientMessageTestSession::TestFunction16L()
	{
	const TAny* ptr;
	
	ptr = iClientMessage->GetPtrL(0);
	
	//Just use the variables to remove compiler warnings
	if(ptr != NULL)
		{
		ptr = NULL;
		}

	}

void CClientMessageTestSession::TestFunction17L()
	{	
	_LIT8(KDes8,"CClientMessageTest");
	
	//Ptr variables dont havea  WriteL method so this should panic
	iClientMessage->WriteL(0,KDes8);	
	}

/*
 * Pckg tests
 */

void CClientMessageTestSession::TestFunction18L()
	{	
	CMTestStruct2 struct1;
	TPckg<CMTestStruct2> pData(struct1);
	
	TInt length = iClientMessage->GetDesLengthL(0);

	iClientMessage->ReadL(0,pData);

	if(length == sizeof(CMTestStruct2))
		{
		struct1.iCount--;
		
		struct1.iDes = _L("Message Handled");
		
		length = iClientMessage->GetDesMaxLengthL(0);
		
		if(length == sizeof(CMTestStruct2))
			{
			iClientMessage->WriteL(0,pData);
			}
		}
	}

void CClientMessageTestSession::TestFunction19L()
	{
	CMTestStruct1 struct1;
	TPckg<CMTestStruct1> pData(struct1);
	
	iClientMessage->ReadL(0,pData);
	}


void CClientMessageTestSession::TestFunction31L()
	{
	TInt function = iClientMessage->GetIntL(0);
	
	switch(function)
		{
		case EGetIntL:
			{
			function = iClientMessage->GetIntL(1);
			break;
			}
			
		case EGetDes8L:
			{
			const TDesC8& data = iClientMessage->GetDes8L(1);
			break;
			}
			
		case ERead8L:
			{
			TBuf8<16> buf;
			iClientMessage->ReadL(1,buf);
			break;
			}
			
		case EWrite8L:
			{
			iClientMessage->WriteL(1, _L8(""));
			break;
			}
			
		case EGetDes16L:
			{
			const TDesC& data = iClientMessage->GetDes16L(1);
			break;
			}
			
		case ERead16L:
			{
			TBuf<16> buf;
			iClientMessage->ReadL(1,buf);
			break;
			}
			
		case EWrite16L:
			{
			iClientMessage->WriteL(1, _L(""));
			break;
			}
			
		case EGetDesLengthL:
			{
			function = iClientMessage->GetDesLengthL(1);
			break;
			}
			
		case EGetDesMaxLengthL:
			{
			function = iClientMessage->GetDesMaxLengthL(1);
			break;
			}
		}
	}


/* CClientMessageTestServer2 is a lightweight Server that is used to test
 * the CClientMessage framework.  Client side code sends messages to the 
 * server which are processed and the client chceks the expected behaviour.
 * The server code does no testing itself.  The second server is used to 
 * test the ClientMessage Frameworks handling of Tls data.
 */
CServer2* CClientMessageTestServer2::NewLC()
	{
	CClientMessageTestServer2* self = new(ELeave) CClientMessageTestServer2(CActive::EPriorityStandard);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	
	}

void CClientMessageTestServer2::ConstructL()
	{
	StartL(KServer2Name);
	}

TInt CClientMessageTestServer2::RunError(TInt aError)
	{
	Message().Complete(aError);
	
	ReStart();
	return KErrNone;
	}

CClientMessageTestServer2::CClientMessageTestServer2(TInt aPriority):
		CServer2(aPriority)
	{	
	
	}

CSession2* CClientMessageTestServer2::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{

	return new(ELeave)CClientMessageTestSession2;
	
	}

/* CClientMessageTestSession2 is a lightweight Session class processes
 * messages from the client using the CClientMessage framework 
 */

CClientMessageTestSession2::~CClientMessageTestSession2()
	{
	delete iClientMessage;
	}

void CClientMessageTestSession2::CreateL(const CServer2& /*aServer*/)
	{

	}


void CClientMessageTestSession2::ServiceL(const RMessage2& aMessage)
	{
	
	TRAPD(err,iClientMessage = BSUL::CClientMessage::NewL(aMessage));
	
	if(err == BSUL::KErrNotInitialised)
		{
		BSUL::CClientMessage::InitialiseFrameworkL(KServer2Data);
		
		iClientMessage = BSUL::CClientMessage::NewL(aMessage);
		}
	
	TRAP(err, DoServiceL());
	
	iClientMessage->CompleteRequestL(err);
	
	delete iClientMessage;
	iClientMessage = NULL;

	}

void CClientMessageTestSession2::DoServiceL()
	{
	//Just do this to test the ::Message API
	const RMessage2& message = iClientMessage->Message();
	
	switch(message.Function())
		{
		case ETestMessage0:
			{
			TRAPD(err,TestFunction0L());
			if(err != BSUL::KErrNotValidated)
				{
				User::Leave(KErrGeneral);
				}
			iClientMessage->ValidateL();

			TestFunction0L();

			break;
			}
			
		case ETestMessage1:
			{
			TestFunction1L();
			break;
			}
			
		case ETestMessage2:
			{
			iClientMessage->ValidateL();

			TestFunction2L();
			break;
			}
			
		default:
			{
			User::Leave(KErrNotFound);
			break;
			}

		}
	}

/*The test function below reads from the clients message
 * using the CCLientMessage framework.
 */

void CClientMessageTestSession2::TestFunction0L()
	{	
	TInt arg0;
	TInt arg1;
	
	//read the data from the clients message
	arg0 = iClientMessage->GetIntL(0);
	arg1 = iClientMessage->GetIntL(1);
	
	//Just use the variables to remove compiler warnings
	arg0 += arg1;
	}

/*The test function below reads the int parameter from the message
 * The parameter is validated through a custom validation function
 */

void CClientMessageTestSession2::TestFunction1L()
	{	
	TInt arg0;

	iClientMessage->ValidateL(0);
		
	//read the data from the clients message
	arg0 = iClientMessage->GetIntL(0);
	
	arg0++;
	}

/*The test function below reads the descriptor from the clients message
 * The parameter is validated through a custom validation function
 */
void CClientMessageTestSession2::TestFunction2L()
	{	
	TBuf<32> data;
		
	//read the data from the clients message
	iClientMessage->ReadL(0,data,0);

	}



static void DoRunServer1L()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CServer2* server = CClientMessageTestServer::NewLC();
	
	TInt err = User::RenameThread(KServerName);
	
	RProcess().Rendezvous(KErrNone);
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(server);

	CleanupStack::PopAndDestroy(scheduler);	
	}

static TInt RunServer1(TAny*)
	{
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt r=KErrNoMemory;
   	if (cleanup)
   		{
 		TRAP(r,DoRunServer1L());
   		delete cleanup;
   		}
   	return r;
	}

static void DoRunServer2L()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CServer2* server = CClientMessageTestServer2::NewLC();
	
	TInt err = User::RenameThread(KServer2Name);
	
	RProcess().Rendezvous(KErrNone);
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(server);

	CleanupStack::PopAndDestroy(scheduler);	
	}

static TInt RunServer2(TAny*)
	{
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt r=KErrNoMemory;
   	if (cleanup)
   		{
 		TRAP(r,DoRunServer2L());
   		delete cleanup;
   		}
   	return r;
	}
   	
/*Launch 2 servers in seperate threads to allow testing of ClientMessage Frameworks
 * Tls handling
 */

static void RunServersL()
	{
	
	RThread server1Thread;	
	TInt err = server1Thread.Create(_L("Test Server 1"), &RunServer1, KDefaultStackSize, NULL, NULL);
	
	if(err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	
	RThread server2Thread;	
	err = server2Thread.Create(_L("Test Server 2"), &RunServer2, KDefaultStackSize, NULL, NULL);	
	
	if(err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	
	server1Thread.Resume();
	
	TRequestStatus thread1Stat;
	server1Thread.Logon(thread1Stat);

	server2Thread.Resume();
	
	TRequestStatus thread2Stat;
	server2Thread.Logon(thread2Stat);
	
	User::WaitForRequest(thread1Stat,thread2Stat);
	
	if(server1Thread.ExitType() != EExitPending)
		{
		User::Panic(server1Thread.ExitCategory(),server1Thread.ExitReason());
		}
	
	else
		{
		User::Panic(server2Thread.ExitCategory(),server2Thread.ExitReason());
		}
	}

TInt E32Main()
   //
   // Server process entry-point
   //
   	{
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt r=KErrNoMemory;
   	if (cleanup)
   		{
 		TRAP(r,RunServersL());
   		delete cleanup;
   		}
   	return r;
   	}

















