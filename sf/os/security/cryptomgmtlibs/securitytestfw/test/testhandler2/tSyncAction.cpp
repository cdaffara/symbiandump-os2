/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tSyncAction.h"
#include <e32cons.h>




EXPORT_C CSyncAction::CSyncAction(CConsoleBase& aConsole,
								  Output& aOut)
: CTestAction(aConsole,aOut)
	{
	}

EXPORT_C void CSyncAction::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	}

EXPORT_C void CSyncAction::PerformAction(TRequestStatus& aStatus)
	{
	iResult=SyncAction();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iFinished=ETrue;
	}

EXPORT_C void CSyncAction::FailTestNow(TRequestStatus& /*aStatus*/)
	{
	iConsole.Printf(_L("\ntest attempted to call synchronous FailTestNow\n"));
	}

