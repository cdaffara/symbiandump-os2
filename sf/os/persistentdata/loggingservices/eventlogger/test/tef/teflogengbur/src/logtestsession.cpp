// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of test class to send client requests to log engine server
// Identical implementation exists in logeng\ongoing\test\src\test.cpp
// 
//

/**
 @file
 @internalComponent
*/

#include "LogServShared.h"
#include "logtestsession.h"

TInt RLogTestSession::Connect()
//
// Connect to the server
//
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
