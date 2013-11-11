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
* Client-side API via which the system server uses the user prompt service.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef UPSCLIENT_H
#define UPSCLIENT_H


#include <scs/scsclient.h>
#include <ups/upstypes.h>

namespace UserPromptService
	{

class CUpsClientConfig;
class CPolicyChangeWatcher;
class RUpsSession : public RScsClientBase
/**
	The system server uses this class to establish a connection with
	the UPS server.

	Internally it retrieves and caches authorisation policy information for all the
	services supported by the current server SID. This information is used to allow
	the RUpsSubsession to avoid creating a subsession and/or querying the UPS 
	if the request does not require UPS interaction. 

 */
	{
public:
	IMPORT_C RUpsSession();
	IMPORT_C TInt Connect();
	IMPORT_C void Close();

public:
	void NotifyPolicyFileChangedL();

private:
	friend class RUpsSubsession;
	CPolicyChangeWatcher *iPolicyChangeWatcher;
	void RetrieveClientConfigL();
	CUpsClientConfig* iClientConfig;
	};

class RUpsManagement : public RScsClientBase
/**
	This class supports the management API for the UPS.
 */
	{
public:
	IMPORT_C RUpsManagement();
	IMPORT_C TInt Connect();
	IMPORT_C void Close();

	// Needs ReadDeviceData	
	IMPORT_C void CreateView(const CDecisionFilter& aFilter, TRequestStatus &aStatus);
	IMPORT_C CDecisionRecord *NextMatchL();
	IMPORT_C void CancelAndCloseView();

	// Needs WriteDeviceData	
	IMPORT_C void RemoveDecisionsL(const CDecisionFilter& aFilter);
	IMPORT_C void DeleteDatabaseL();

	// Needs AllFiles
	IMPORT_C void UpdateDecision(TUint32 aRecordId, TBool aAllow, TRequestStatus &aStatus);
	IMPORT_C void CancelUpdateDecision();

	/**
		The following three APIs are only for use by the UPS swi observer plugin and are discouraged for general use
	*/
	IMPORT_C void DeleteDecisionsForExeL(const TSecureId& aExeSid);
	IMPORT_C void NotifyPluginsMayHaveChangedL();
	IMPORT_C void NotifyPolicyFilesChanged(TRequestStatus &aStatus);
	IMPORT_C void CancelNotifyPolicyFilesChanged();

private:
	RBuf8 iCreateViewFilterBuf;
	TPckgBuf<TUint32> iMatchLengthBuf;
	};

class RUpsSubsession : public RScsClientSubsessionBase
/**
System servers use this class to establish a connection to the UPS for a specific 
client.\n
Before checking a request with the User Prompt Service the Authorise method 
first checks whether authorisation from the User Prompt Service is required. If
authorisation is not required e.g. the client has the correct capabilities for the
service then Authorise completes immediately.
*/
	{
public:
	IMPORT_C RUpsSubsession();
	IMPORT_C TInt Initialise(RUpsSession& aSession, const RThread& aClient);

	IMPORT_C void Authorise(TBool aServerCheckOk, 
							const TServiceId& aServiceId, const TDesC& aDestination,
							TUpsDecision& aDecision, TRequestStatus& aStatus);
	IMPORT_C void Authorise(TBool aServerCheckOk, 
							const TServiceId& aServiceId, const TDesC& aDestination,
							const TDesC8& aOpaqueData, 
							TUpsDecision& aDecision, TRequestStatus& aStatus);						 
	IMPORT_C void CancelPrompt();

	IMPORT_C void Close();

private:
	TInt CreateSubsession();
	void AuthoriseInternal(TBool aServerCheckOk,
		const TServiceId& aServiceId, const TDesC& aDestination, const TDesC8& aOpaqueData,
		TUpsDecision& aDecision, TRequestStatus& aStatus);
	TInt PreparePrompt(const TServiceId& aServiceId, const TDesC& aDestination, const TDesC8& aOpaqueData);
	void ExecutePrompt(TBool aServerCheckOk, TUpsDecision& aDecision, TRequestStatus& aStatus);
	
private:
	/** Describes the TUpsDecision reference passed into Authorise. */
	TPtr8 iDecPtr;

	TBool iSubsessionCreated;
	TThreadId iClientTid;
	TSecureId iClientSid;
	TProcessId iClientPid;
	RUpsSession* iUpsSession;
	};

} // End of namespace UserPromptService

#endif	// #ifndef UPSCLIENT_H

