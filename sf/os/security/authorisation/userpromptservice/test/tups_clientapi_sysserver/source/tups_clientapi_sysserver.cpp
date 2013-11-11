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



#include "tups_clientapi_sysserver.h" 


TVersion RUpsTestServ::Version() const    
	{
	return(TVersion(KTestExecuteMajorVersionNumber,KTestExecuteMinorVersionNumber,KTestExecuteBuildVersionNumber));
	}


EXPORT_C TInt RUpsTestServ::Connect(const TDesC& aServerName, TBool aDebugMode)
/**
 * @param aServerName - Human readable name of the test server
 * @param aDebugMode - Set to true for just in time debugging
 * @return int - Standard error codes
 * Secure version of the API call. Expects the server binary to be
 * ServerXXX.exe
 */
	{
	if(aServerName.Length() > iServerName.MaxLength())
		return KErrTooBig;
	iServerName = aServerName;
	// Assume the server is already running and attempt to create a session
	// 4 message slots
	TInt err = CreateSession(aServerName,Version(),-1);
	if(err == KErrNotFound || err == KErrServerTerminated)
		{
		// Server not running
		// Construct the server binary name
		TBuf<KMaxTestExecuteNameLength> serverFile;
		RProcess server;
		_LIT(KEmpty,"");
		_LIT(KExe,".exe");

		serverFile.Copy(aServerName);
		serverFile.Append(KExe);
		err = server.Create(serverFile,KEmpty);
		if(err != KErrNone)
			return err;
		// Synchronise with the server
		TRequestStatus reqStatus;
		server.Rendezvous(reqStatus);

		// Set just in time debugging for the process
		if(aDebugMode)
			server.SetJustInTime(ETrue);
		else
			server.SetJustInTime(EFalse);

		// Start the test harness
		server.Resume();
		// Server will call the reciprocal static synchronise call
		User::WaitForRequest(reqStatus);
		server.Close();
		if(reqStatus.Int() != KErrNone)
			return reqStatus.Int();
		// Create the root server session
		err = CreateSession(aServerName,Version(),-1);
		}
	return err;
	}

EXPORT_C const TDesC& RUpsTestServ::ServerName() const
/**
 * @return - The human readable server name
 */
	{
	return iServerName;
	}

///////
EXPORT_C TInt RUpsTestSession::Open(RUpsTestServ& aServ)
/**
 * @param aServ - Reference to the root server session
 * @return Standard error codes
 * Synchronously open a server test step session
 */
 	{
	return CreateSubSession(aServ,EUpsTest_OpenSession);
	}  

EXPORT_C void RUpsTestSession::Close() 
	{
	CloseSubSession(EUpsTest_CloseSession);
	}
	
EXPORT_C TInt RUpsTestSession::UseTestService(TUpsTestServiceRequestData& aServiceRequestData,TUpsTestServiceReturn& aServiceCompoundReturn)
/**
 *
 * @return aPanicString  - If the test step panics, this member is set up with the panic string
 */
 	{
 	TInt returnReceived;
 	
 	const TPckgC<TUpsTestServiceRequestData> 	compoundArgs(aServiceRequestData);
	TPckg<TUpsTestServiceReturn> 		compoundReturn(aServiceCompoundReturn);
	TIpcArgs args(&compoundArgs, &compoundReturn);
	returnReceived = SendReceive(EUpsTest_UseTestService, args);

	return returnReceived;
	}   
	
	
EXPORT_C TInt RUpsTestSession::GetServerSid()
/**
 *
 * @return aPanicString  - If the test step panics, this member is set up with the panic string
 */
 	{
 	
 	TPckgBuf<TInt> sidBuf;
 	TInt returnServerSid;
 	TIpcArgs args;
 	args.Set(0,&sidBuf);
 	
	SendReceive(EUpsTest_GetServerSid , args);
	
 	returnServerSid=sidBuf(); 

	return returnServerSid;
	}     
	
	
