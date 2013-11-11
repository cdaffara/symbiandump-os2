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

#include "commandengine.h"
#include "ncmtestconsole.h"

CNcmCommandEngine* CNcmCommandEngine::NewL(CUsbNcmConsole& aTestConsole)
/**
Constructs a CNcmCommandEngine object.
  @param  aTestConsole The main console 
*/
	{
	LOG_STATIC_FUNC_ENTRY
	CNcmCommandEngine* self = new(ELeave) CNcmCommandEngine(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CNcmCommandEngine::~CNcmCommandEngine()
	{
	Cancel();
	RemoveAllCommand();
	}

CNcmCommandEngine::CNcmCommandEngine(CUsbNcmConsole& aTestConsole)
	: CActive(EPriorityStandard)
	, iTestConsole(aTestConsole)
	, iLastPage(0), iLastItem(-1), iRemainLine(COMMAND_MAX_COUNT_PER_PAGE)
	{
	CActiveScheduler::Add(this);
	}

void CNcmCommandEngine::ConstructL()
	{
	//wait user select command
	iTestConsole.iConsole->Read(iStatus);
	SetActive();
	}

void CNcmCommandEngine::DoCancel()
	{
	iTestConsole.iConsole->ReadCancel();
	}

void CNcmCommandEngine::RunL()
	{
	LOG_FUNC
	
	User::LeaveIfError(iStatus.Int());
	
	TUint key = iTestConsole.iConsole->KeyCode();

	__FLOG_STATIC1(KSubSys, KLogComponent , _L8("key = %c"), key);
	
	if((key >= '0') && (key <= '9'))
		{
		iTestConsole.ScheduleDraw(key-0x30);
		}
	else
		{
		CNcmCommandBase* command = NULL;
		if(iCommandMap.Find(key))
			{
			command = *(iCommandMap.Find(key));
			if(command)
				{
				command->DoCommandL();
				}
			else
				{
				__FLOG_STATIC0(KSubSys, KLogComponent , _L8("The command in hashmap is NULL"));				
				}
			}						
		else
			{
			CUsbNcmConsoleEvent* event = CUsbNcmConsoleEvent::NewL();
			event->iEvent.AppendFormat(_L("G:Unknown:Key[%c]"), key);
			iTestConsole.NotifyEvent(event);			
			}

		}
	iTestConsole.iConsole->Read(iStatus);
	
	SetActive();
	}

void CNcmCommandEngine::RegisterCommand(CNcmCommandBase* aCommand)
/**
Add command object into command map
  @param aCommand  a command which will be used by user 
*/
	{
	//Assert if aCommand is NULL
	__ASSERT_ALWAYS(aCommand, Panic(ENcmCommandIsNull));
	//Assert if the key exists in command map
	__ASSERT_ALWAYS(!iCommandMap.Find(aCommand->Key()), Panic(ENcmCommandKeyExists));
	
	//add command
	TInt err = iCommandMap.Insert(aCommand->Key(), aCommand);
	if(err != KErrNone)
		{
		User::Panic(_L("RegisterCommand"), err);
		}
	
	//make the command to be displayed in which page, which line
	TInt line = (aCommand->Description().Length() + NUM_CHARACTERS_ON_LINE + 4) / (NUM_CHARACTERS_ON_LINE);
	iRemainLine -= line;
	if(iRemainLine < 0)
		{
		iLastPage ++;
		iRemainLine = COMMAND_MAX_COUNT_PER_PAGE - line;
		iLastItem = 0;
		}
	else
		{
		iLastItem ++;
		}
	iCommandKeys[iLastPage][iLastItem] = aCommand->Key();
	
	}

void CNcmCommandEngine::RemoveAllCommand()
/**
Destroy all commands in command map  
*/
	{
	CNcmCommandBase* currentitem;

	RHashMap<TUint, CNcmCommandBase*>::TIter hashMapIter(iCommandMap);
	TInt count = iCommandMap.Count();
	for(TInt i=0; i< count; i++)
	    {
	    currentitem = *(hashMapIter.NextValue());
	    if(currentitem)
	    	{
	    	hashMapIter.RemoveCurrent();
	    	delete currentitem;
	    	}
	    }
	iCommandMap.Close();
	
	}


void CNcmCommandEngine::PrintHelp(TInt aPage)
/**
Get command help info (command key - command description) 
@param aPage  the command help info of specified page
*/
	{		
	LOG_FUNC
	
	__ASSERT_ALWAYS(((aPage <= COMMAND_MAX_PAGE) && (aPage > 0)), Panic(ENcmArrayBound));
	
	TUint key = iCommandKeys[aPage-1][0];
	if(key == 0)
		{
		//No commands in this page
		iTestConsole.iConsole->Printf(_L("No commands\n"));
		}
	else
		{
		//Display command
		TInt i = 0;
		while((key != 0) && (i < COMMAND_MAX_COUNT_PER_PAGE))
			{
			CNcmCommandBase* command = NULL;
			command = *(iCommandMap.Find(key));
			iTestConsole.iConsole->Printf(_L("%c - %S\n"), command->Key(), &command->Description());
			i++;
			key = iCommandKeys[aPage-1][i];
			}
		}

	}

CNcmCommandBase::CNcmCommandBase(TInt aPriority, CUsbNcmConsole& aConsole, TUint aKey)
	: CActive(aPriority), iTestConsole(aConsole), iKey(aKey)
/**
Constructor
@param aPriority  	Active object's priority
@param aConsole		The main console
@param aKey			command key
*/
	{
	}

CNcmCommandBase::~CNcmCommandBase()
	{	
	}

TUint CNcmCommandBase::Key() const
	{
	return iKey;
	}

void CNcmCommandBase::SetKey(TUint aKey)
	{ 
	iKey = aKey; 
	}


const TDesC& CNcmCommandBase::Description()
	{ 
	return iDescription; 
	}

void CNcmCommandBase::SetDescription(const TDesC& aDescription)
/**
Set command description
@param aDescription  the new description for the command 
*/
	{
	iDescription.SetLength(0);
	iDescription.Copy(aDescription.Left(NUM_CHARACTERS_ON_LINE));
	}
void CNcmCommandBase::RunL()
	{	
	}

void CNcmCommandBase::DoCancel()
	{	
	}
TInt CNcmCommandEngine::RunError(TInt aError)
	{
	User::Panic(_L("CNcmCommandEngine"), aError);
	return aError;
	}
