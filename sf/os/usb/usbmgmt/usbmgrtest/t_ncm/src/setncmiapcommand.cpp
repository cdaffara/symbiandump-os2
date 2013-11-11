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

#include "setncmiapcommand.h"
#include "ncmtestconsole.h"

_LIT(KSetNcmIapCommandDescription, "Set Ncm Iap ID");

CSetNcmIapCommand* CSetNcmIapCommand::NewL(CUsbNcmConsole& aTestConsole, TUint aKey)
	{
	LOG_STATIC_FUNC_ENTRY
	
	CSetNcmIapCommand* self = new(ELeave) CSetNcmIapCommand(aTestConsole, aKey);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CSetNcmIapCommand::~CSetNcmIapCommand()
	{
	EndWork();
	}

CSetNcmIapCommand::CSetNcmIapCommand(CUsbNcmConsole& aTestConsole, TUint aKey)
	: CNcmCommandBase(EPriorityStandard, aTestConsole, aKey)
	{
	CActiveScheduler::Add(this);
	}

void CSetNcmIapCommand::ConstructL()
	{
	SetDescription(KSetNcmIapCommandDescription());
	}

void CSetNcmIapCommand::DoCommandL()
/**
Main function of set ncm iap. 
It will start a new console to display all Iaps in commsdb and current setting for ncm. 
Then prompt user to input ncm iap id and the index of this iap will be placed in central 
repository. Press 'ESC' for give up this function. After the setting is successful, the 
console will be destroyed and return to main console
*/	
	{
	LOG_FUNC

	if(!IsActive())
		{
		//If the console has existed, close the old console. The work on old console will be 
		//gived up.
		if(iConsole)
			{
			__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Close old console"));
			EndWork();
			}
		InitWorkL();
		
		iConsole->Printf(_L("Press \'ESC\' to exit without change\n"));
		
		//Display all iaps in commsdb
		ListAllNcmIapL();
	
		//wait user input iap id
		iConsole->Printf(KSetIapMemo());
		iCommandState = EIapId;
		iConsole->Read(iStatus);
		SetActive();
		}				
	}

void CSetNcmIapCommand::ListAllNcmIapL()
/**
Display all iaps in commsdb on console and get current config in cental repoistory
*/
	{
	LOG_FUNC
	
	//Display commsdb's iaps
	TInt iapcount = iTestConsole.DrawAvailableIapsL();

	TInt iapId = 0;

	//Get iap config from central repository
	TInt err = iRepository->FindL(KPartialKey, KMask, iRows);
	if(err == KErrNone)
		{
		TUint count = iRows.Count();
		for (TInt row = 0; row < count; ++row)
		    {
		    // Read each IAP ID
		    User::LeaveIfError(iRepository->Get(iRows[row], iapId));
			__FLOG_STATIC2(KSubSys, KLogComponent , _L8("index[%d]: iap = %d"), row, iapId);		    
		    iConsole->Printf(_L("index[%d]: iap = %d\n"), row, iapId);
		    }  
		}
	else
		{
		__FLOG_STATIC1(KSubSys, KLogComponent , _L8("FindL() err[%d]"), err);		    		
		}
	}

void CSetNcmIapCommand::GetDefaultIapL(TInt& aDefaultIap)
/**
Get the default iap id (The first one) stored in central repository. 
@return the default iap id
*/
	{
	LOG_FUNC
	aDefaultIap = 13;
	return ;
	}

void CSetNcmIapCommand::SetNcmIapL()
/**
Set iap id into central repository. The iap id is iIapId. The place of iap id in central
repository is iIndex. iIndex is started from 0. If the iIndex is not exist in central 
repository, iIapId will be placed at last place. 
*/
	{
	LOG_FUNC
	
	TUint count = iRows.Count();
	__FLOG_STATIC1(KSubSys, KLogComponent , _L8("now iap count = %d"), count);

	TInt idx;
	if(iIndex >= count)
		{
		//iIndex is not exist in central repository, create a new entry in central repository
		idx = iRows[count-1]+1;
		User::LeaveIfError(iRepository->Create(idx, iIapId));
		__FLOG_STATIC2(KSubSys, KLogComponent , _L8("add iap: idx = %d, iIapId"), idx, iIapId);		
		}
	else
		{
		//iIndex exists, just set in the new value
		idx = iRows[iIndex];
		User::LeaveIfError(iRepository->Set(idx, iIapId));
		__FLOG_STATIC2(KSubSys, KLogComponent , _L8("set iap: aIdx = %d, iIapId"), idx, iIapId);		
		}

	}

void CSetNcmIapCommand::RunL()
	{
	LOG_FUNC
	__FLOG_STATIC1(KSubSys, KLogComponent , _L8("CSetNcmIapCommand::RunL - iStatus = %d"), iStatus.Int());

	User::LeaveIfError(iStatus.Int());

	TKeyCode code = iConsole->KeyCode();
	switch(code)
		{
		case EKeyEnter:
			{
			iConsole->Printf(_L("\n"));

			//get value
			TInt val;
			TBool typeErr = EFalse;
			if(iChars.Length() != 0)
				{					
				TLex lex(iChars);				
				lex.SkipSpaceAndMark();

				if(KErrNone != lex.Val(val))
					{
					typeErr = ETrue;
					}
				}
			else
				{
				typeErr = ETrue;
				}
			
			if(typeErr)
				{
				//Prompt user input again
				iConsole->Printf(KErrInput());
				iConsole->Printf(KSetIapMemo());
				iChars.SetLength(0);
				__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Input error"));
				break;					
				}
			
			//The input is correct
			__FLOG_STATIC1(KSubSys, KLogComponent , _L8("Get val = %d"), val);
			
			if(iCommandState == EIapId)
				{
				//if the command state is wait iap id, now get the iap id, wait for iap index
				iIapId = val;
				iChars.SetLength(0);
				iConsole->Printf(KSetIapIndexMemo());
				iCommandState = EIapPriority;
				}
			else //(iCommandState == EIapPriority)
				{
				//if the command state is wait iap index, now the index. set into central repoistory.
				//Then destroy the current console and update the value displayed on main console if the
				//setting value is ncm default value.
				iIndex = val;
				SetNcmIapL();
				EndWork();
				if(iIndex == 0)
					{
					iTestConsole.SetLocalIapId(iIapId);
					}
				__FLOG_STATIC2(KSubSys, KLogComponent , _L8("Set Ncm Iap, idx = %d, id = %d"),
						iIndex, iIapId);

				return;
				}				
			}
			break;
		case EKeyEscape:
			//delete this;
			EndWork();
			return;
		case EKeyBackspace:
			if(iChars.Length() > 0)
				{
				iConsole->SetCursorPosRel(TPoint(-1, 0));
				iConsole->ClearToEndOfLine();
				iChars.SetLength(iChars.Length()-1);
				}
			break;
		default:
			iChars.Append(code);
			iConsole->Printf(_L("%c"), code);
			break;
		}
	iConsole->Read(iStatus);
	SetActive();
	
	}

void CSetNcmIapCommand::DoCancel()
	{
	iConsole->ReadCancel();
	}

TInt CSetNcmIapCommand::RunError(TInt aError)
	{
	User::Panic(_L("CSetNcmIapCommand"), aError);
	return aError;
	}

void CSetNcmIapCommand::InitWorkL()
/**
Initialize work. Including start a new console, create buf for getting user input, get repoistory handle.
*/
	{
	LOG_FUNC

	if(!iConsole)
		{		
		iConsole = Console::NewL(_L("Set Ncm IAP Console"), TSize(-1,-1));
		__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Create Console ok!"));

		User::LeaveIfError(iChars.Create(KMaxNumOfChars));
		__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Create iChars ok!"));

		iRepository = CRepository::NewL(KNcmIapIdRepositoryUID);	
		__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Get CRepository ok!"));
		
		iIapId = 1;
		iIndex = 0;
		}
	}

void CSetNcmIapCommand::EndWork()
/**
Finalize work. Including delete console, delete the buf used for getting user input, close repoistory handle.
*/
	{
	LOG_FUNC
	
	if(iConsole)
		{
		__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Destroy console..."));
		Cancel();
		iChars.Close();
		iRows.Close();
		delete iRepository;
		delete iConsole;
		iConsole = NULL;
		__FLOG_STATIC0(KSubSys, KLogComponent , _L8("Destroy console ok!"));
		}
	}

