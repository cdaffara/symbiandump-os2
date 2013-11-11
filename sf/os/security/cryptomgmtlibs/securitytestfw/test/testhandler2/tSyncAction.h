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
* T_SYNCACTION.H
*
*/


#ifndef __T_SYNCACTION_H__
#define __T_SYNCACTION_H__

#include "t_testaction.h"

class CSyncAction : public CTestAction
	{
protected:
	IMPORT_C virtual TBool SyncAction() = 0;
	IMPORT_C CSyncAction(CConsoleBase& aConsole, Output& aOut);
	IMPORT_C void ConstructL(const TTestActionSpec& aTestActionSpec);
	IMPORT_C void FailTestNow(TRequestStatus& aStatus);
private:
	IMPORT_C void PerformAction(TRequestStatus& aStatus);	
	};

#endif

