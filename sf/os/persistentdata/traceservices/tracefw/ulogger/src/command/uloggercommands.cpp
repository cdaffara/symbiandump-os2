// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// ULogger commands
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#include "uloggercommands.h"
#include "uloggershared.h"
using namespace Ulogger;

CCommand::CCommand(MCommandImpl* aCommandImpl)
: iCommandImpl(aCommandImpl)
	{
	}

EXPORT_C CCommand::~CCommand()
	{
	iCommandsLookup.Close();
	}

EXPORT_C CCommand* CCommand::NewL(MCommandImpl* aCommandImpl)
	{
	CCommand* obj = CCommand::NewLC(aCommandImpl);
	CleanupStack::Pop( obj );
	return obj;
	}

EXPORT_C CCommand* CCommand::NewLC(MCommandImpl* aCommandImpl)
	{
	CCommand* obj = new (ELeave) CCommand(aCommandImpl);
	CleanupStack::PushL( obj );
	obj->ConstructL();
	return obj;
	}

EXPORT_C void CCommand::ConstructL()
	{
	iVerbose = EFalse;
	
	//fill the lookup table
	//two command matters
	iCommandsLookup.Append( TCommandLookup(&KCmdEnable, &KCmdPrimaryFilter, ESetPrimaryFilter) ); // -ef 
	iCommandsLookup.Append( TCommandLookup(&KCmdEnable, &KCmdSecondaryFilter, ESetSecondaryFilter) ); // -es 
	iCommandsLookup.Append( TCommandLookup(&KCmdEnable, &KCmdSecondaryFilterToggle, EEnableSecondaryFiltering) ); // -eS 
	iCommandsLookup.Append( TCommandLookup(&KCmdDisable, &KCmdSecondaryFilterToggle, EDisableSecondaryFiltering) ); // -dS 
	iCommandsLookup.Append( TCommandLookup(&KCmdDisable, &KCmdPluginConfigurations, ERemovePluginSettings) ); // -dc 
	iCommandsLookup.Append( TCommandLookup(&KCmdList, &KCmdOutputPlugin, EGetInstalledPlugins) ); // -lo 
	iCommandsLookup.Append( TCommandLookup(&KCmdList, &KCmdPluginConfigurations, EGetPluginSettings) ); // -lc 
	iCommandsLookup.Append( TCommandLookup(&KCmdList, &KCmdPrimaryFilter, EGetPrimaryFilters) ); // -lf 
	iCommandsLookup.Append( TCommandLookup(&KCmdList, &KCmdSecondaryFilter, EGetSecondaryFilters) ); // -ls 
	iCommandsLookup.Append( TCommandLookup(&KCmdList, &KCmdSecondaryFilterToggle, EGetSecondaryFiltering) ); // -lS 
	iCommandsLookup.Append( TCommandLookup(&KCmdEnable, &KCmdBuffer, EResizeTraceBuffer) ); // -eb 
	iCommandsLookup.Append( TCommandLookup(&KCmdList, &KCmdBuffer, EGetTraceBufferSize) ); // -lb 
	iCommandsLookup.Append( TCommandLookup(&KCmdEnable, &KCmdBufferMode, ESetBufferMode) ); // -em
	iCommandsLookup.Append( TCommandLookup(&KCmdList, &KCmdBufferMode, EGetBufferMode) ); // -lm 
	iCommandsLookup.Append( TCommandLookup(&KCmdEnable, &KCmdNotification, ESetDataNotificationSize) );// -en
	iCommandsLookup.Append( TCommandLookup(&KCmdList, &KCmdNotification, EGetDataNotificationSize) ); // -ln 
	iCommandsLookup.Append( TCommandLookup(&KCmdDisable, &KCmdPrimaryFilter, ERemovePrimaryFilter) ); // -df 
	iCommandsLookup.Append( TCommandLookup(&KCmdDisable, &KCmdSecondaryFilter, ERemoveSecondaryFilter) ); // -ds 
	iCommandsLookup.Append( TCommandLookup(&KCmdEnable, &KCmdPluginConfigurations, ESetPluginSettings) ); // -ec 
	iCommandsLookup.Append( TCommandLookup(&KCmdEnable, &KCmdOutputPlugin, ESetActivePlugin) ); // -eo 
	iCommandsLookup.Append( TCommandLookup(&KCmdEnable, &KCmdInputPlugin, ESetActiveInputPlugin) ); // -ei 
	iCommandsLookup.Append( TCommandLookup(&KCmdDisable, &KCmdInputPlugin, EDeactivateInputPlugin) ); // -di 
	iCommandsLookup.Append( TCommandLookup(&KCmdList, &KCmdInputPlugin, EGetInputPlugins) ); // -li 
	//only main command matters
	iCommandsLookup.Append( TCommandLookup(&KCmdStart, &KNullDesC, EStart) ); // -r 
	iCommandsLookup.Append( TCommandLookup(&KCmdStop, &KNullDesC, EStop) ); // -q 
	iCommandsLookup.Append( TCommandLookup(&KCmdRestart, &KNullDesC, ERestart) ); // -t   	
	iCommandsLookup.Append( TCommandLookup(&KCmdOutputPlugin, &KNullDesC, EGetActivePlugin) ); // -o 
	iCommandsLookup.Append( TCommandLookup(&KCmdInputPlugin, &KNullDesC, EGetActiveInputPlugin) ); // -i 
	iCommandsLookup.Append( TCommandLookup(&KCmdHelp, &KNullDesC, EHelp) ); // -h 
	iCommandsLookup.Append( TCommandLookup(&KCmdMan, &KNullDesC, EManPage) ); // -H 
	iCommandsLookup.Append( TCommandLookup(&KCmdVersion, &KNullDesC, EVersion) ); // -version 
	
	}

/**
This function parses commands given as a parameter and generates TCommand value.
The function is responsible also for triggering event for MCommandImpl

@param aArgs array of arguments to parse
@return KErrNone if commands were parsed successfully and event was triggered, system wide error code otherwise
 */
EXPORT_C TInt CCommand::HandleCommandL(const RArray<TPtrC>& aArgs)
	{	
	TBool optionsConsumed = EFalse;
	RArray<TPtrC> commands;
	CleanupClosePushL( commands );
	RBuf command;
	CleanupClosePushL(command);
	TInt maxSize=2;
	for(TInt k=0; k<aArgs.Count(); ++k) //calculate max command length
		maxSize += aArgs[k].Length();
	User::LeaveIfError(command.Create(maxSize));
	
	for(TInt i=0; i<aArgs.Count(); ++i)
		{
		TPtrC argPtr(aArgs[i]);
		
		//the 'version' command is the special case
		if(!argPtr.Mid(1).Compare(KCmdVersion))
			{
			TPtrC ptr(argPtr.Mid(1));
			commands.AppendL(ptr);
			continue;
			}
		
		//check if we have to parse combined commands (like -ca)
		//other arguments are added to new array for further processing
		TBuf<1> cmdChecker;
		cmdChecker.Copy( argPtr.Mid(0,1) );
		if(!cmdChecker.Compare(KCmdIndicator) && !optionsConsumed)
			{			
			//if there is option 'v' (verbose), this option shoul be removed
			//desc:
			//command data format: 'command param1 param2 paramN'
			//where:
			//command: ia string with commands letters, for example: 'lf', 'esv'
			//paramN: is parameter string, for example 'uloggerserialplugin'
			//array example: 'ec uloggerserialplugin output_port 3'
			
			for(TInt k=1; k<argPtr.Length(); ++k)
				{
				TPtrC ptr(argPtr.Mid(k,1));
				if(!ptr.Compare(KCmdVerboseMode))
					iVerbose = ETrue;
				else
					command.AppendFormat(_L("%S"), &ptr);
				}
			//put command as a first element in array
			if(commands.Count() > 0)
				{
				commands.Remove(0);
				User::LeaveIfError(commands.Insert(command, 0));
				}
			else
				commands.AppendL(command);
			
			
			optionsConsumed = ETrue;
			}
		else
			commands.AppendL(argPtr);
		}
	
	TInt errCode = PrepareCommandL( commands );	
	CleanupStack::PopAndDestroy(2); //command, commands
	return errCode;
	}


/** This method uses lookup table to translate user readable commands into TCommand value.
@param aArgs Array of arguments.
@return KErrNone if command was preapared correctly and system wide error code otherwise
 */
TInt CCommand::PrepareCommandL(RArray<TPtrC>& aArgs)
	{
	TInt i;
	TInt lookupArraySize = iCommandsLookup.Count();
	
	if(aArgs.Count() == 0)
		return iCommandImpl->DoCommandL(EUnknown, aArgs);
	
	//first elements contains command letters
	TPtrC cmd(aArgs[0]);
	if(cmd.Length() <= KMaxNumberOfLettersInCommand) //we want precise command 
		{
		//check two arguments - 1st pass
		for(i=0; i<lookupArraySize; ++i)
			{
			if(cmd.Length() == 2) 
				if(!iCommandsLookup[i].iMainCommand->Compare(cmd.Mid(0,1)))
					if(!iCommandsLookup[i].iSubCommand->Compare(cmd.Mid(1,1)))
						{
						aArgs.Remove(0);
						return iCommandImpl->DoCommandL(iCommandsLookup[i].iCommand, aArgs);
						}
			}

		//check two arguments - 2nd pass
		for(i=0; i<lookupArraySize; ++i)
			{
			//check one argument
			if(cmd.Length() == 1) 
				if(!iCommandsLookup[i].iMainCommand->Compare(cmd.Mid(0,1)))
					{
					//make sure that none of the 'two letters commands' will not be called
					if(iCommandsLookup[i].iSubCommand->Compare(KNullDesC))
						continue;
					aArgs.Remove(0);
					return iCommandImpl->DoCommandL(iCommandsLookup[i].iCommand, aArgs);
					}
			}
		}
	
	return iCommandImpl->DoCommandL(EUnknown, aArgs);
	}

