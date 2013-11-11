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
// LOGSERVEROPERATIONBASE.INL
// 
//

#ifndef __LOGSERVEROPERATIONBASE_INL__
#define __LOGSERVEROPERATIONBASE_INL__


inline RMessage2& CLogServOperationBase::Message()
	{
	return iMessage;
	}

inline void CLogServOperationBase::Cancel()
	{
	Complete(KErrCancel);
	}

inline TBool CLogServOperationBase::HaveMessagePointer() const
	{
	return (!iMessage.IsNull());
	}

inline void CLogServOperationBase::ClearMessagePointer()
	{
	iMessage = RMessage2();
	}

inline TLogOperationType CLogServOperationBase::Type() const
	{
	return iClientServerData.iOperationType;
	}

inline TLogOperationId CLogServOperationBase::Id() const
	{
	return iClientServerData.iOperationId;
	}

inline CLogPackage& CLogServOperationBase::Package()
	{
	return iPackage;
	}

inline MLogServTaskInterface& CLogServOperationBase::TaskInterface() const
	{
	return iTaskInterface;
	}

inline MLogServOperationManager& CLogServOperationBase::OperationManager() const
	{
	return iOperationManager;
	}

inline TLogServSessionId CLogServOperationBase::SessionId()
	{
	return iSessionId;
	}

inline const TLogClientServerData& CLogServOperationBase::ClientServerData() const
	{
	return iClientServerData;
	}

inline void CLogServOperationBase::SetMessageCompletion(const TBool aMessageCompletion)
	{
	iMessageCompletion = aMessageCompletion;
	}

#endif
