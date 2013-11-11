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
* CPolicyChangeEvaluator used to process RUpsManagement::NotifyPolicyFilesChanged
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef POLICYCHANGEEVALUATOR_H
#define POLICYCHANGEEVALUATOR_H

#include <ups/upstypes.h>
#include "scs/scsserver.h"
#include "upscommon.h"
#include "upsdbmanager.h"
#include "upspolicycachehandle.h"

namespace UserPromptService
{

// forward declarations
class CUpsServer;
class CUpsSession;

class CPolicyChangeEvaluator : public CAsyncRequest, public MDbHandleClient
/**
	An instance of this object will be used to update an existing entry.

	This consists of looking up the existing entry, by recordId, and then changing just the
	result field.
*/
	{
public:
	static CPolicyChangeEvaluator* NewLC(RPolicyCacheCountedHandle &aPolicyCacheHandle, CUpsSession* aSession, const RMessage2& aMessage);
	~CPolicyChangeEvaluator();
	
	void StartUpdate();
private:
	CPolicyChangeEvaluator(RPolicyCacheCountedHandle &aPolicyCacheHandle, CUpsSession* aSession, const RMessage2& aMessage);

	enum TPolicyChangeState 
	{
		EWaitForPolicyCacheIdle,
		EScanDatabase
	};

	TPolicyChangeState iState;

	enum TPolicyChangeEvent
	{
		EInternalRequestComplete,
		ECancel
	};

	void ProcessEventL(TPolicyChangeEvent aEvent);


	// implement CAsyncRequest
	virtual void DoCleanup();	// Cancel this request

	// implement CActive
	virtual void DoCancel();
	// implement CActive, override CAsyncRequset
	virtual void RunL();
	// implement CActive, override CAsyncRequset
	virtual TInt RunError(TInt aError);

	// Implement MDbHandleClient
	virtual void DbHandleAboutToBeDeleted();

	inline CUpsSession *UpsSession();
	inline CUpsServer *UpsServer();

	void WaitForPolicyCacheIdleStateL(TPolicyChangeEvent aEvent);
	void ScanDatabaseStateL(TPolicyChangeEvent aEvent);

	void BuildServerServicePolicyVersionTableL();
	void DeleteUnneededEntriesL();

private:

	struct TServerServicePolicyVersion
		{
		TUint32 iServerId;
		TInt32 iServiceId;
		TUint16 iMajorVersion;
		};

	static TInt OrderServerServicePolicyVersion(const TServerServicePolicyVersion &aLhs,
									 			 	 const TServerServicePolicyVersion &aRhs);


	RPolicyCacheCountedHandle iPolicyCacheHandle;
	RUpsDbHandleSlave iUpdateDbHandle;
	CDecisionView *iUpdateView;
	RArray<TServerServicePolicyVersion> iSSPVarray;
	};

} // End of namespace UserPromptServer
#endif

