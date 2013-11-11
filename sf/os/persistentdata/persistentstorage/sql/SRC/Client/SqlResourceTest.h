// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLRESOURCETEST_H__
#define __SQLRESOURCETEST_H__

#include "SqlUtil.h"	//TSqlSrvFunction

//Forward declarations
class RSqlDbSession;

/**
TSqlResourceTestData class is used for sending test commands to the SQL server.
If the connection with the server is not established yet, the TSqlResourceTestData
instance will store the test command and send it later, at the moment when the connection
with the server is established.

@internalComponent
*/
NONSHARABLE_CLASS(TSqlResourceTestData)
	{
public:
	static TSqlResourceTestData* Instance();
	static void Release();

	TInt Init(RSqlDbSession& aDbSession);
	TInt Set(TSqlSrvFunction aFunction, TInt aAllocFailType, TInt aRate);

private:
	TSqlResourceTestData();

private:		
	TBool			iRqPending;
	RSqlDbSession*	iDbSession;
	TSqlSrvFunction iFunction;
	TInt			iAllocFailType;
	TInt 			iRate;
	
	};

#endif //__SQLRESOURCETEST_H__
