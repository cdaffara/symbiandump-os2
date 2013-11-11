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
* CUpdateEvaluator used to process RUpsManagement::UpdateDecisionL
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef UPDATEEVALUATOR_H
#define UPDATEEVALUATOR_H

#include <ups/upstypes.h>
#include <scs/scsserver.h>
#include "upscommon.h"
#include "upsdbmanager.h"

namespace UserPromptService
{

// forward declarations
class CUpsServer;
class CUpsSession;

class CUpdateEvaluator : public CAsyncRequest, public MDbHandleClient
/**
	An instance of this object will be used to update an existing entry.

	This consists of looking up the existing entry, by recordId, and then changing just the
	result field.
*/
	{
public:
	static CUpdateEvaluator* NewLC(CUpsSession* aSession, const RMessage2& aMessage);
	~CUpdateEvaluator();
	
	void StartUpdate();
private:
	CUpdateEvaluator(CUpsSession* aSession, const RMessage2& aMessage);
	void ConstructL(const RMessage2& aMessage);

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

private:
	TBool iNewResult;

	RUpsDbHandleSlave iUpdateDbHandle;
	CDecisionView *iUpdateView;
	};

} // End of namespace UserPromptServer
#endif
