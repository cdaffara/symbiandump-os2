/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
 @internalComponent
 @test
 */

#include "exitcommand.h"
#include "ncmtestconsole.h"

_LIT(KExitCommandDescription, "Exit");

CExitCommand* CExitCommand::NewL(CUsbNcmConsole& aTestConsole, TUint aKey)
	{
	LOG_STATIC_FUNC_ENTRY
	
	CExitCommand* self = new(ELeave) CExitCommand(aTestConsole, aKey);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CExitCommand::~CExitCommand()
	{
	}

CExitCommand::CExitCommand(CUsbNcmConsole& aTestConsole, TUint aKey)
	: CNcmCommandBase(EPriorityStandard, aTestConsole, aKey)
	{
	}

void CExitCommand::ConstructL()
	{
	SetDescription(KExitCommandDescription());
	}

void CExitCommand::DoCommandL()
	{
	LOG_FUNC
	__FLOG_STATIC0(KSubSys, KLogComponent , _L8("CExitCommand::DoCommandL - Stop scheduler"));
	
	//Quit the main console
	iTestConsole.Stop();
	}


