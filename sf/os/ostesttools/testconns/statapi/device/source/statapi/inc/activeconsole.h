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



#include <e32base.h> 
#include "stat_controller.h"
#include "stat_console.h"

#ifndef LIGHT_MODE
    #include "apgtask.h"
#endif
_LIT( KTxtExampleCode, "STATMAIN" );

class CActiveConsole : public CActive
	{
public:
	 // Construction
	static CActiveConsole* NewL(CConsoleBase*,CStatController*,CStatConsole*,TInt, MNotifyLogMessage *const aMsg);
	 // Destruction
	~CActiveConsole();
	void Start();
	void DoCancel();

private:
	CActiveConsole(CConsoleBase*,CStatController*,CStatConsole*,TInt, MNotifyLogMessage *const aMsg);
	void ConstructL();
	void RunL();
	void PushToBack();

protected:
	CConsoleBase* iConsole;			//A console for reading from
	CStatController *iController;	//Pointer to CStatController object
	CStatConsole *iStatConsole;		//statconsole
	TInt iSessionID;				//CStatController session id, used to close session
	MNotifyLogMessage *iMsg;
	};

