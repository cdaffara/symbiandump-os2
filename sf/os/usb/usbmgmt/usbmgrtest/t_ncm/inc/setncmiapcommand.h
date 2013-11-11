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

#ifndef SETNCMIAPCOMMAND_H
#define SETNCMIAPCOMMAND_H

#include "commandengine.h"
#include <centralrepository.h>

NONSHARABLE_CLASS(CSetNcmIapCommand) : public CNcmCommandBase
/**
Set the Ncm Iap Id into central repository
*/
	{
public:
	enum TSetNcmIapCommandState
	{
		EIapId,			//The command is in state of wait user input iap
		EIapPriority	//The command is in state of wait user input iap index
	};
public:
	static CSetNcmIapCommand* NewL(CUsbNcmConsole& aUsb, TUint aKey);
	~CSetNcmIapCommand();

	//Get the first IAP ID stored in central repository.
	void GetDefaultIapL(TInt& aDefaultIap);

public:
	//From CNcmCommandBase
	void DoCommandL();
	
private:
	CSetNcmIapCommand(CUsbNcmConsole& aUsb, TUint aKey);
	void ConstructL();

private:
	//From CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	//Display all IAPs stored in central repository which are used for Ncm
	void ListAllNcmIapL();
	//Set the Iap Id inputed by user into central repository
	void SetNcmIapL();
	
	//Init work for run this command
	void InitWorkL();
	//Stop work when finish or terminate this command
	void EndWork();
	
private:

	TSetNcmIapCommandState iCommandState;

	//The console used to display iap info and get user input for ncm iap
	CConsoleBase* iConsole;
	//user current input
	RBuf		 iChars;
	//the Iap id user want to store in central repository
	TInt iIapId;
	//the position of the Iap id which will be stored in central repository
	TInt iIndex;
	//central repository handle
	CRepository* iRepository;
	//all Iaps get from central repository
	RArray<TUint32> iRows;

	//followed are used for finding Iap Id stored in central repository
	static const TUint KPartialKey = 0x01010000;
	static const TUint KMask = 0xFFFF0000;
	static const TInt KMaxNumOfChars = 255;
	

	};


_LIT(KSetIapMemo ,"Input the NcmIap you want to set:");
_LIT(KSetIapIndexMemo, "Input the index of iap you want to set:");
_LIT(KErrInput, "Error input. Input again.\n");

const TUid KNcmIapIdRepositoryUID = {0x10286a43};

#endif // SETNCMIAPCOMMAND_H

