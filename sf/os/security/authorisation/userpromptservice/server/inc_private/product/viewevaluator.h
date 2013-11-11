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
* CViewEvaluator - used to evaluate a view (RUpsManagement::CreateView)
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef VIEWEVALUATOR_H
#define VIEWEVALUATOR_H

#include <ups/upstypes.h>
#include <f32file.h>
#include <scs/scsserver.h>
#include "upscommon.h"
#include "upsdbmanager.h"

namespace UserPromptService
{

// forward declarations
class CUpsServer;
class CUpsSession;

class CViewEvaluator : public CAsyncRequest, public MDbHandleClient
/**
	An instance of this object will be created to represent an asynchronous
	request while the UPS evaluates a database view from the RUpsManagement::CreateView
	call.
 */
	{
public:
	static CViewEvaluator* NewLC(CUpsSession* aSession, const RMessage2& aMessage);
	~CViewEvaluator();
	
	void StartEvaluatingView();
private:
	CViewEvaluator(CUpsSession* aSession, const RMessage2& aMessage);
	void ConstructL(const RMessage2& aMessage);

	// implement CAsyncRequest
	virtual void DoCleanup();	// Cancel this request

	// implement CActive
	virtual void DoCancel();
	// implement CActive, override CAsyncRequset
	virtual void RunL();
	virtual TInt RunError(TInt aError);

	// Implement MDbHandleClient
	virtual void DbHandleAboutToBeDeleted();

	inline CUpsSession *UpsSession();
	};

} // End of namespace UserPromptService
#endif
