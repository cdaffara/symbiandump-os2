// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_logservsession.h"
#include "LogServShared.h"

TInt RLogTestSession::Connect()
	{
	return CreateSession(KLogServerFullName,TVersion(0,0,0));
	}

void RLogTestSession::Send(TInt aType, TIpcArgs& aParam, TRequestStatus& aStatus)
	{
	SendReceive(aType, aParam, aStatus);
	}

TInt RLogTestSession::Send(TInt aType, TIpcArgs&  aParam)
	{
	return SendReceive(aType, aParam);
	}
