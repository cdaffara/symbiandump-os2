// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGSERVOPERATIONBASE_H__
#define __LOGSERVOPERATIONBASE_H__

#include <e32base.h>
#include "LogServDefs.h"
#include "LogCliServShared.h"
#include "logpackage.h"

// Classes referenced
class MLogServTaskInterface;
class MLogServOperationManager;

/**
Base class for LogEng server operations.
Maintains a copy of the original RMessage2 object.

@see MLogServOperationManager
@see MLogServTaskInterface
 
@internalComponent
*/
class CLogServOperationBase : public CBase
	{
public:
	enum TCompletionStatus
		{
		EOperationComplete = 0,
		EOperationCompleteWaitForClient
		};

protected:
	CLogServOperationBase(MLogServTaskInterface& aTaskInterface, 
                          MLogServOperationManager& aOperationManager, 
                          const RMessage2& aMessage, 
                          CLogPackage& aPackage,
                          const TLogClientServerData& aClientServerData,
                          TLogServSessionId aSessionId);

public:
	~CLogServOperationBase();

public: // FROM CLogServOperationBase
	virtual void StartL(TRequestStatus& aStatus)=0;
	virtual void WriteL(const RMessage2& aMessage);
	virtual TCompletionStatus CompleteProcessingL(TInt aFinalCompletionCode);

public:
	void Complete(TInt aCompletionCode);
	inline const TLogClientServerData& ClientServerData() const;
	inline void Cancel();
	inline TBool HaveMessagePointer() const;
	inline void ClearMessagePointer();
	inline TLogOperationId Id() const;
	inline CLogPackage& Package();
	inline MLogServTaskInterface& TaskInterface() const;
	inline MLogServOperationManager& OperationManager() const;
	inline TLogOperationType Type() const;
	inline TLogServSessionId SessionId();
	static TUint QueueOffset();
	inline void SetMessageCompletion(const TBool aMessageCompletion);

protected:
	inline RMessage2& Message();

private:
	MLogServTaskInterface& iTaskInterface;
	MLogServOperationManager& iOperationManager;
	CLogPackage& iPackage;
	RMessage2 iMessage;
	TLogClientServerData iClientServerData;
	TLogServSessionId iSessionId;
	TSglQueLink iServerLink;
	TBool iMessageCompletion;
	};


#include "LogServOperationBase.inl"

#endif
