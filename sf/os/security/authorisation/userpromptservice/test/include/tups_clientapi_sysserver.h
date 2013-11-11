/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file    
 @test
*/

#if !(defined UPS_TEST_CLIENT_H)
#define UPS_TEST_CLIENT_H

#include <e32base.h>
#include <test/tefexportconst.h>
#include <ups/upsclient.h>

enum {
	EUpsTest_ConnectServer, 
	EUpsTest_UseTestService,    // Simulates the use of a service provided by the test system server.
	EUpsTest_OpenSession,       // Opens subsession to test system server. 
	EUpsTest_CloseSession, 		// Closes subsession to test system server. 
	EUpsTest_GetServerSid,  	// returns sid of server. 
	};

class TUpsTestServiceRequestData
	{
	public:
	inline	TUpsTestServiceRequestData();
	TBool 		iOpaquePresent;
	TBuf8<50> 	iOpaqueData;
	TBool 		iMeasureResponceTime;
	TUid  		iServiceUid;
	TBuf<60>	iDestination;
	TBool 		iCancelRequest;
	TBool 		iPlatSecPass;
	};

inline TUpsTestServiceRequestData::TUpsTestServiceRequestData():
iOpaquePresent(0),iOpaqueData(0),iMeasureResponceTime(0),iServiceUid(TUid::Uid(0)),iDestination(0),iCancelRequest(0),iPlatSecPass(0) 
	{
	}
	


class TUpsTestServiceReturn
	{
	public:
	inline		TUpsTestServiceReturn();
	TUpsDecision  	iDecision;
	TInt			iError;
	TBuf<50> 		iErrorMessage;
	TUint32			iRequestDuration;
 	};

inline TUpsTestServiceReturn::TUpsTestServiceReturn():
iDecision(TUpsDecision(EUpsDecNo)),iError(0),iErrorMessage(0)  
	{
	}

class RUpsTestServ : public RSessionBase
 
 	{
public:
	IMPORT_C TInt Connect(const TDesC& aServerName, TBool aDebugMode = EFalse);
	IMPORT_C const TDesC& ServerName() const;
protected:
private:
	RLibrary iLibrary;
	TBuf<KMaxTestExecuteNameLength> iServerName;
	TVersion Version() const;
	};

class RUpsTestSession : public RSubSessionBase
 
	{
public:
	IMPORT_C TInt  					Open(RUpsTestServ& aServ);
	IMPORT_C TInt					UseTestService(TUpsTestServiceRequestData& aTestServiceRequestData,TUpsTestServiceReturn& aServiceCompoundReturn);
	IMPORT_C void  					Close(void);
	IMPORT_C TInt					GetServerSid();
	IMPORT_C void  					ShutdownUPS(void);
protected:

 
private:


	};

#endif
