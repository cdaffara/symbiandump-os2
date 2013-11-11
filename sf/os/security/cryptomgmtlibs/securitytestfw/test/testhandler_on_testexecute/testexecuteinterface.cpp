/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
*/

#include "testexecuteinterface.h"
#include "server.h"


namespace TestExecuteInterface
	{
	class TMain
		{
	public:
		TMain(TScriptTests* aTestTypes, const TDesC& aServerName);
		void MainL();

	private:
		void SignalClientL();

	private:
		TScriptTests* iTestTypes;
		TPtrC iServerName;
		};
}


using namespace TestExecuteInterface;

EXPORT_C TInt TestExecuteInterface::StartWinsEka1Thread(
		TInt (*)(), const TDesC&)
	{
	// EKA2 version or hardware versions do not implement this
	return KErrNone;
	}

EXPORT_C void TestExecuteInterface::StartWithTestExecuteL(TScriptTests theTestTypes[],
														  const TDesC& aServerName)
	{
	TMain(theTestTypes, aServerName).MainL();
	}

TMain::TMain(TScriptTests* aTestTypes, const TDesC& aServerName)
	: iTestTypes(aTestTypes), iServerName(aServerName)
	{
	}

void TMain::MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CServer* server = new CServer (iTestTypes);
	if(server)
		{
		CleanupStack::PushL(server);
		server->StartL(iServerName);
		SignalClientL();
		CleanupStack::Pop(server);
		sched->Start();
		}
	CleanupStack::Pop(sched);
	delete server;
	delete sched;
	}

void TMain::SignalClientL()
	{
	RProcess::Rendezvous(KErrNone);
	}

