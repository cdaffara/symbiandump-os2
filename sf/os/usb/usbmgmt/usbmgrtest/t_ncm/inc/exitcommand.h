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


#ifndef EXITCOMMAND_H
#define EXITCOMMAND_H

#include "commandengine.h"

NONSHARABLE_CLASS(CExitCommand) : public CNcmCommandBase
/**
Quit from the main console
*/
	{
public:
	static CExitCommand* NewL(CUsbNcmConsole& aUsb, TUint aKey);
	~CExitCommand();

public:
	//From CNcmCommandBase
	void DoCommandL();

private:
	CExitCommand(CUsbNcmConsole& aUsb, TUint aKey);
	void ConstructL();

private:
		
	};

#endif // EXITCOMMAND_H
