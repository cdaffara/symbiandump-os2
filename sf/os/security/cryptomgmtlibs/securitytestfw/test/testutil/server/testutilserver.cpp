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
* TestUtil - server implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/
 
#include "testutilserver.h"
#include "testutilsession.h"
#include "testutilclientserver.h"

CServer2* CTestUtilServer::NewLC()
	{
	CTestUtilServer* self=new(ELeave) CTestUtilServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CTestUtilServer::~CTestUtilServer()
	{
	iFs.Close();
	delete iFileMan;
	delete iTestUtilSessionCommon;
	}

void CTestUtilServer::ConstructL()
	{
	StartL(KTestUtilServerName);
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();
	iFileMan = CFileMan::NewL(iFs);
	iTestUtilSessionCommon = new (ELeave) CTestUtilSessionCommon();
	}

CSession2* CTestUtilServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	return new(ELeave) CTestUtilSession();
	}

void CTestUtilServer::AddSession()
	{
	++iSessionCount;
	}
	
void CTestUtilServer::DropSession()
	{
	--iSessionCount;
	}

void CTestUtilServer::Send(const TDesC& aMessage)
	{
	iSessionIter.SetToFirst();
	while ((iSessionIter++)!=0)
		{
		iTestUtilSessionCommon->Send(aMessage);
		}
	}
