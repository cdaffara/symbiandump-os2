/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "CServerOOMTestRunner.h"
#include "t_output.h"

_LIT(KTFileTokens, "t_filetokens");

CServerOOMTestRunner::CServerOOMTestRunner(Output& aOut) :
	COOMTestRunnerBase(aOut)
	{
	}

CServerOOMTestRunner::~CServerOOMTestRunner()
	{
	iClient.Close();
	}

void CServerOOMTestRunner::StartOOMTestL()
	{
	User::LeaveIfError(iClient.Connect(EFileKeyStore));
	SendRequest(EStartOOMTest);
	}

void CServerOOMTestRunner::IncHeapFailPoint()
	{
	SendRequest(EIncHeapFailPoint);
	}

void CServerOOMTestRunner::ResetHeapFail()
	{
	SendRequest(EResetHeapFail);
	}

TInt CServerOOMTestRunner::AllocCount()
	{
	return SendRequest(EAllocCount);
	}

void CServerOOMTestRunner::EndOOMTestL()
	{
	iClient.Close();
	}

TInt CServerOOMTestRunner::SendRequest(TFSTokenMessages aFunction)
	{
	TInt r = iClient.SendRequest(aFunction, TIpcArgs());
	__ASSERT_ALWAYS(r >= 0, User::Panic(KTFileTokens, 1));
	return r;
	}
