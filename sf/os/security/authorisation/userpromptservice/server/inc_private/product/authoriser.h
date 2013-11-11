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
* Server-side classes which are used to implement the UPS server.
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef AUTHORISER_H
#define AUTHORISER_H

#include <ups/upstypes.h>
#include <scs/scsserver.h>
#include "upscommon.h"
#include "pluginmanager.h"
#include <ups/policy.h>

namespace UserPromptService
{

// forward declarations
class CUpsSession;
class CUpsSubsession;
class CPolicyCache;
class CPromptRequest;
class CDialogCreator;
class CPolicyEvaluator;
class CFingerprint;
class CClientEntity;

class CAuthoriser : public CAsyncRequest
/**
	An instance of this object would be created to represent an asynchronous
	request while the UPS generates its response, which may include prompting
	the user.
 */
	{
public:
	static CAuthoriser* NewL(RPolicyCacheCountedHandle &aPolicyCache, CUpsSession* aSession, CUpsSubsession* aSubsession, TBool aServerCheckOk,
							  TThreadId &aClientTid, TProcessId &aClientPId,
							  const RMessage2& aMessage, const TServiceId& aServiceId, 
							  RBuf &aDestination, RBuf8 &aOpaqueData);
	virtual ~CAuthoriser();

	void Wakeup();

	void ClearedToDisplayL();
	
private:
	CAuthoriser(RPolicyCacheCountedHandle &aPolicyCache, CUpsSession* aSession, CUpsSubsession* aSubsession, const RMessage2& aMessage);
	void ConstructL(TBool aServerCheckOk, TThreadId &aClientTid, TProcessId &aClientPId, const TServiceId& aServiceId, RBuf &aDestination, RBuf8 &aOpaqueData);

	// implement CAsyncRequest
	virtual void DoCleanup();	// Cancel this request

	// implement CActive
	virtual void DoCancel();
	// implement CActive, override CAsyncRequset
	virtual void RunL();

	void CompleteSelf();

private:
	inline CUpsServer *UpsServer();

	enum TAuthoriserState 
	{
		ECheckPolicy,
		ECreatingFingerprints,
		ECheckDatabase,
		EPreparingDialog,
		EExecutingDialog
	};

	TAuthoriserState iState;

	enum TAuthoriserEvent
	{
		EInternalRequestComplete,
		EClearedToDisplayDialog,
		ECancel
	};

	void ProcessEventL(TAuthoriserEvent aEvent);

	void CheckPolicyStateL(TAuthoriserEvent aEvent);
	void CreatingFingerprintsStateL(TAuthoriserEvent aEvent);
	void CheckDatabaseStateL(TAuthoriserEvent aEvent);
	void PreparingDialogStateL(TAuthoriserEvent aEvent);
	void ExecutingDialogStateL(TAuthoriserEvent aEvent);

	void LookupPolicyL();
	void CreateFingerprintsL();
	void LookupFingerprintsL();
	void PrepareDialogL();
	void ExecuteDialogL();
	void ProcessDialogResultL();

	TUpsDecision MapCPolicyTOptions2TUpsDecision(CPolicy::TOptions aOptions);
	void HandleDbErrorL(TInt aError);

	CPromptRequest* iPromptRequest;

	RPolicyCacheCountedHandle iPolicyCache;
	const CPolicy* iPolicy; ///< NOT owned by us, owned by policy cache

	CPlugin<CPolicyEvaluator>* iPolicyEvaluator;
	RPointerArray<CFingerprint> iFingerprints;
	const CClientEntity* iClientEntity;
	const TAny* iDialogCreatorParams; ///< NOT owned by us, owned by policy evaluator
	
	CPlugin<CDialogCreator>* iDialogCreator;

	const CFingerprint* iDialogFingerprint;
	TUint iDialogEvaluatorInfo;
	CPolicy::TOptions iDialogSelectedOption;

	TBool iPromptForced;
	TUint32 iPromptForcedRecordId;
	};



//
// Inline functions
//
inline CUpsServer *CAuthoriser::UpsServer()
	{
	return static_cast<CUpsServer *>(
			&static_cast<CUpsSession*>(iSession)->iServer);
	}

class CAuthoriserFifo
/**
	Class used by the UpsServer to maintain a list of requests/CAuthoriser objects waiting to display a dialog and a list of
	requests/objects which are currently being reprocessed.
*/
	{
public:
	static CAuthoriserFifo *NewL();
	~CAuthoriserFifo();
	void PushL(CAuthoriser *aAuthoriser);
	CAuthoriser *PopL();
	void RemoveL(CAuthoriser *aAuthoriser);
	TBool IsEmpty() const;
	void Compress();
private:
	RPointerArray<CAuthoriser> iPtrArray;
	};


} // End of namespace UserPromptService


#endif
