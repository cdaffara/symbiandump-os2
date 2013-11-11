/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32cons.h>
#include <bacline.h>
#include <keytool.rsg>

#include "keytool_utils.h"
#include "keytool_defs.h"
#include "keytool_controller.h"
#include "keytool_view_imp.h"
#include "keytool_commands.h"
#include "keytoolfileview.h"


// Boiler plate 
_LIT(KShortName, "Nokia KeyTool");
_LIT(KName, "Nokia KeyStore Manipulation Tool");
_LIT(KCopyright, "Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).  All rights reserved.");

_LIT(KNewLine, "\n");

_LIT(KDone, "Press any key to continue... \n");

// Keytool command line parameters
_LIT(KList, "-list");
_LIT(KListShort, "-l");

_LIT(KListStores, "-liststores");
_LIT(KListStoresShort, "-ls");

_LIT(KImport, "-import");
_LIT(KImportShort, "-i");


_LIT(KRemove, "-remove");
_LIT(KRemoveShort, "-r");

_LIT(KHelp, "-help");
_LIT(KHelpShort, "-h");

_LIT(KSetUser, "-setuser");
_LIT(KSetUserShort, "-s");

_LIT(KSetAllUsers, "-setallusers");
_LIT(KSetAllUsersShort, "-a");

_LIT(KSetManager, "-setmanager");
_LIT(KSetManagerShort, "-m");

_LIT(KRemoveUser, "-removeuser");
_LIT(KRemoveUserShort, "-ru");

// Command parameters
_LIT(KLabel, "-label");
_LIT(KKeyUsage, "-usage");
_LIT(KAccess, "-access");

_LIT(KStore, "-store");

//_LIT(KExpiry, "-expiry"); // Functionality not present at the moment...
_LIT(KDetails, "-details");
_LIT(KDetailsShort, "-d");

_LIT(KPageWise, "-page");
_LIT(KPageWiseShort, "-p");

#ifdef KEYTOOL
_LIT(KMigrateStore, "-migratestore");
_LIT(KNewFileName, "-new");
_LIT(KAuthExpression, "-expr");
_LIT(KFreshness, "-freshness");
#endif // KEYTOOL

const TInt KMaxArgs = 10;

/**
 * Displays tool name and copyright informations.
 */
LOCAL_D void BoilerPlateL(CConsoleBase* console) 
	{
	console->Printf(KNewLine);
	console->Printf(KName);
	console->Printf(KNewLine);	
	console->Printf(KCopyright);
	console->Printf(KNewLine);
	console->Printf(KNewLine);	
	}


LOCAL_D TBool VerifyCommand(const TDesC& aCommand, TInt& aCmdNum, TInt& aCmdCount)
	{
	if ((aCmdNum != -1) && (aCommand[0] == '-'))
		{
		aCmdNum = KeyToolDefController::KUsageCommand;
		aCmdCount = KMaxArgs;
		return 1;
		}
	if (aCommand.CompareF(KList) == 0 || aCommand.Compare(KListShort) == 0)
		{
		aCmdNum = KeyToolDefController::KListCommand;
		}
	else if (aCommand.CompareF(KListStores) == 0 || aCommand.Compare(KListStoresShort) == 0)
		{
		aCmdNum = KeyToolDefController::KListStoresCommand;
		}
	else if	(aCommand.CompareF(KImport) == 0 || aCommand.Compare(KImportShort) == 0)
		{
		aCmdNum = KeyToolDefController::KImportCommand;
		}
	else if	(aCommand.CompareF(KRemove) == 0 || aCommand.Compare(KRemoveShort) == 0)
		{
		aCmdNum = KeyToolDefController::KRemoveCommand;
		}
	else if	(aCommand.CompareF(KSetUser) == 0 || aCommand.Compare(KSetUserShort) == 0)
		{
		aCmdNum = KeyToolDefController::KSetUserCommand;
		}
	else if	(aCommand.CompareF(KSetManager) == 0 || aCommand.Compare(KSetManagerShort) == 0)
		{
		aCmdNum = KeyToolDefController::KSetManagerCommand;
		}
	else if	(aCommand.CompareF(KRemoveUser) == 0 || aCommand.Compare(KRemoveUserShort) == 0)
		{
		aCmdNum = KeyToolDefController::KRemoveUserCommand;
		}
	else if	(aCommand.CompareF(KSetAllUsers) == 0 || aCommand.Compare(KSetAllUsersShort) == 0)
		{
		aCmdNum = KeyToolDefController::KSetAllUsersCommand;
		}
	
#ifdef KEYTOOL
	else if	( aCommand.CompareF(KMigrateStore) == 0 )
		{
		aCmdNum = KeyToolDefController::KMigrateStore;
		}
#endif // KEYTOOL
	
	else	
		{
		return 0;
		}

	return 1;
	}
	
	
/**
 * Parses the command line and given control to the handler to deal with the request.
 */
LOCAL_D void DoMainL() 
	{
	TBool interactiveMode = ETrue;
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	CConsoleBase* console = Console::NewL(KShortName, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);
	CCommandLineArguments* cmdArgs = CCommandLineArguments::NewLC();
	TInt cmdArgsCount = cmdArgs->Count();
	
	RFile file;
	// command: keytool inputfile outputfile
	if (cmdArgsCount == 3)
		{
		if (KeyToolUtils::DoesFileExistsL(fs,cmdArgs->Arg(1)))
			{
			interactiveMode = EFalse;
			TInt error = file.Open(fs, cmdArgs->Arg(1), EFileRead|EFileShareAny);
			file.Close();
			
			TInt error1 = file.Replace(fs, cmdArgs->Arg(2), EFileWrite|EFileShareExclusive);
			CleanupClosePushL(file);
			// If the input file doesn't exist or not able to create outputfile
			// switch to Interactive mode
			if (error != KErrNone || error1 != KErrNone)
				{
				CleanupStack::PopAndDestroy(&file);
				interactiveMode = ETrue;
				}
			}
		}
		
	CKeytoolConsoleView* view = CKeytoolConsoleView::NewLC(*console);
	CKeyToolController* controller = CKeyToolController::NewLC(*view);
	CArrayFixFlat<TPtrC>* args = new (ELeave) CArrayFixFlat<TPtrC> (10);
	CleanupStack::PushL(args);
	CKeytoolFileView* view1 = NULL;

	TInt cmdCount = 0;
	if (interactiveMode)
		{
		KeyToolUtils::SetConsole(console);
		BoilerPlateL(console);
		for (TInt i = 0; i < cmdArgsCount; i++)
			{
			args->AppendL(cmdArgs->Arg(i));
			}
		// In Interactive mode by default we can execute 1 command only.
		cmdCount = 1;
		}
	else
		{
		CleanupStack::PopAndDestroy(3, view); // args, controller, view
		KeyToolUtils::SetFile(&file);
		view1 = CKeytoolFileView::NewLC(cmdArgs->Arg(1));
		cmdCount = view1->SplitFileInputToArrayL();
		}	
		
	for (TInt j = 0; j < cmdCount; j++)
		{
		if (!interactiveMode)
			{
			controller = CKeyToolController::NewLC(*view1);
			args = view1->ReadArrayArgumentsLC(j);
			}
			
		CKeyToolParameters* params = CKeyToolParameters::NewLC();
		
		TInt command = -1; 
		TInt i = -1;

		TInt argsCount = args->Count();
		while (i < (argsCount-1))
		 	{
			i++;

			if ((args->At(i).CompareF(KDetails)==0)|| (args->At(i).Compare(KDetailsShort)==0))
				{
				params->iIsDetailed = ETrue;
				continue;
				}
			
			if (args->At(i).CompareF(KPageWise)==0 || (args->At(i).Compare(KPageWiseShort)==0))
				{
				params->iPageWise = ETrue;
				continue;
				}			
			
			TDesC& commd = args->At(i);
			if (VerifyCommand(commd, command, i))
				{
				if (commd.CompareF(KSetAllUsers) == 0 || commd.Compare(KSetAllUsersShort) == 0 ||
				    commd.CompareF(KRemoveUser) == 0 || commd.Compare(KRemoveUserShort) == 0 ||
				    commd.CompareF(KSetManager) == 0 || commd.Compare(KSetManagerShort) == 0 )
					{
					i++;
					if (args->At(i)[0] == '-' ||  args->At(i).Left(2) == _L("0x"))
						{
						i = argsCount;
						command = KeyToolDefController::KUsageCommand;
						}
					else
						{
						i--;
						}
					}
				if (commd.CompareF(KSetAllUsers) == 0 || commd.Compare(KSetAllUsersShort) == 0)
					{
					params->iPolicy = CKeyToolParameters::ESetAllUsersPolicy;
					params->iIsDetailed = ETrue;
					}
				if (commd.CompareF(KRemoveUser) == 0 || commd.Compare(KRemoveUserShort) == 0)
					{
					params->iPolicy = CKeyToolParameters::ERemoveUserPolicy;
					params->iIsDetailed = ETrue;
					}
				if (commd.CompareF(KSetManager) == 0 || commd.Compare(KSetManagerShort) == 0)
					{
					params->iPolicy = CKeyToolParameters::ESetManagerPolicy;
					params->iIsDetailed = ETrue;
					}
				if (commd.CompareF(KSetUser) == 0 || commd.Compare(KSetUserShort) == 0)
					{
					params->iPolicy = CKeyToolParameters::ESetUserPolicy;
					i++;
					if (i >= argsCount || args->At(i)[0] == '-' ||  args->At(i).Left(2) != _L("0x"))
						{
						i = argsCount;
						command = KeyToolDefController::KUsageCommand;
						}
					else
						{
						TUid app;
						TLex lex(args->At(i).Mid(2));		
						TUint uid =0;
						TInt err = lex.Val(uid, EHex);
						if (err == KErrNone)
							{
							app = TUid::Uid(uid);						
							}			 
						params->iUIDs.Append(app); 
						params->iIsDetailed = ETrue;
						}
					}
				
#ifdef KEYTOOL
				if(commd.CompareF(KMigrateStore) == 0 )
					{
					int mandatoryParams = 2;
					if(argsCount-1 >= ++i)
						{
						params->iOldKeyFile = args->At(i).AllocL();
						}
					else
						{
						controller->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_OLDKEY_ABSENT);
						--mandatoryParams;
						}
					if(argsCount-1 >= ++i)
						{
						params->iPassphrase = args->At(i).AllocL();
						}
					else
						{
						controller->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_PASSPHRASE_ABSENT);
						--mandatoryParams;
						}
					
					if( mandatoryParams != 2 )
						{
						command = KeyToolDefController::KUsageCommand;
						}
					else
						{
						TInt count = args->Count();
						
						while(++i < count )
							{
							TDesC& commd = args->At(i);
							if(commd.CompareF(KNewFileName) == 0 &&	argsCount-1 >= ++i)
								{
								delete params->iNewKeyFile;
								params->iNewKeyFile=NULL;
								params->iNewKeyFile = args->At(i++).AllocL();
								}
							else if(commd.CompareF(KAuthExpression) == 0  &&	argsCount-1 >= ++i )
								{
								params->iAuthExpression = args->At(i++).AllocL();
								}
							else if(commd.CompareF(KFreshness) == 0  &&	argsCount-1 >= ++i )
								{
								TLex lex(args->At(i++));		
								TInt err = lex.Val(params->iFreshness);
								}
							} // while
						} // if(mandatoryParams == 0)
					} // if
				
#endif // KEYTOOL
				continue;
				}

			TDesC& cmd = args->At(i);
				if (cmd.CompareF(KHelp) == 0 || cmd.Compare(KHelpShort) == 0 ||
				    cmd.CompareF(KLabel) == 0 || cmd.CompareF(KKeyUsage) == 0 ||
				    cmd.CompareF(KStore) == 0 || cmd.CompareF(KAccess) == 0)
				{
				i++;
				if (i >= argsCount || args->At(i)[0] == '-')
					{
					i = argsCount;
					command = KeyToolDefController::KUsageCommand;
					}
				else if (cmd.CompareF(KHelp) == 0 || cmd.Compare(KHelpShort) == 0)
					{
					params->iDefault = args->At(i).AllocL();
					i = argsCount;	
					}
				else if (cmd.CompareF(KLabel) == 0)
					{
					params->iLabel = args->At(i).AllocL();
					}
				else if (cmd.CompareF(KKeyUsage) == 0)
					{
					params->iUsage = KeyToolUtils::ParseKeyUsage(args->At(i));
					}
				else if (cmd.CompareF(KStore) == 0)
					{
					TLex parser(args->At(i));
					TInt err = parser.Val(params->iKeystoreIndex);
					params->iIsDetailed = ETrue;
					}
				else if (cmd.CompareF(KAccess) == 0)
					{
					params->iAccess = KeyToolUtils::ParseKeyAccess(args->At(i));
					}
				else
					{
					// no action required	
					}
				continue;
				}
				
			if (i!=0) 
				{
				if (args->At(i)[0] == '-')
					{
					i = argsCount;
					command = KeyToolDefController::KUsageCommand;
					continue;
					}
				delete params->iDefault;
				params->iDefault = NULL;
				params->iDefault = args->At(i).AllocL();			
				}
		 	} 
		 
		 
		if (command != -1)
			{
			TRAP_IGNORE(controller->HandleCommandL(command, params));
			}
		else 
			{
			controller->HandleCommandL(KeyToolDefController::KUsageCommand, params);		
			}
		
		CleanupStack::PopAndDestroy(3, controller); // params, args, controller
		}
	if (interactiveMode)
		{
		CleanupStack::PopAndDestroy(view);
		// We are done!
		console->Printf(KNewLine);
		console->Printf(KDone);
		console->Getch();	
		}
	else
		{
		CleanupStack::PopAndDestroy(2, &file); //view1 and file
		}
	CleanupStack::PopAndDestroy(3, &fs); //cmdArgs, console, fs	
	}
	

GLDEF_C TInt E32Main()         // main function called by E32
   	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); 
	
	TRAP_IGNORE(DoMainL());
	
	delete cleanup; 
	__UHEAP_MARKEND;
	return 0; 
   	}

