/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include "activeconsole.h"

CActiveConsole* CActiveConsole::NewL(CConsoleBase* aConsole,CStatController *aController,CStatConsole* aStatConsole,TInt aSessionID, MNotifyLogMessage *const aMsg)
{
	CActiveConsole* self=new(ELeave)CActiveConsole(aConsole,aController,aStatConsole,aSessionID,aMsg);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;	
}

CActiveConsole::CActiveConsole(CConsoleBase* aConsole,CStatController *aController,CStatConsole* aStatConsole,TInt aSessionID, MNotifyLogMessage *const aMsg) : CActive(0)
{
	iConsole=aConsole;	
	iController=aController;
	iSessionID=aSessionID;
	iStatConsole=aStatConsole;
	iMsg = aMsg;
}

void CActiveConsole::ConstructL()
{
	iMsg->Msg( _L("ACTIVECONSOLE: Adding self to scheduler.") );
	CActiveScheduler::Add(this);
}

CActiveConsole::~CActiveConsole()
{
	Cancel();
}

//Checks for user input to console.
void CActiveConsole::RunL()
{
	TChar gChar = TChar(iConsole->KeyCode());

	switch (gChar)
	{
		case EKeyEscape:		//escape exits
		case '2':				//2 exits
			iConsole->Printf( _L("Processing escape request.") );
			iMsg->Msg( _L("ACTIVECONSOLE: Processing escape request.") );
			iStatConsole->UserExitRequest();
			iController->StopSession(iSessionID);
			return;
		case '1':				//1 sets system to forground
		case 'b':				//b or B sets system to forground
		case 'B':
			PushToBack();
			break;
		default:
			break;
	}

	if(iStatus!=KErrNone)
	{
		iConsole->Printf(_L("** error reading from active console **"));
		iConsole->Getch();
	}
	iConsole->Read(iStatus);
	SetActive();
}

void CActiveConsole::DoCancel()
{
}

//Initial request
void CActiveConsole::Start()
{
	iConsole->Read(iStatus);
	SetActive();
}

//Push statapi to background by switching to the system window
void CActiveConsole::PushToBack()
{
#ifndef LIGHT_MODE
	RWsSession Ws;
	
	//conect to windows server
	if(Ws.Connect() != KErrNone) {
		return;
	}
	TInt wsHandle = Ws.WsHandle();

	if(0!=wsHandle)
		{
		TApaTaskList taskList(Ws);

		//switch to system
	    TApaTask newTask = taskList.FindApp(KTxtExampleCode);
	 
	    if( newTask.WgId() != KErrNotFound ) 
	         newTask.SendToBackground();

		//clean up RWsSession
		Ws.Close();
		}
	else
		{
		iConsole->Printf(_L("Error: Unable to connect to windows server."));
		}
#endif // ifndef LIGHT_MODE
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

