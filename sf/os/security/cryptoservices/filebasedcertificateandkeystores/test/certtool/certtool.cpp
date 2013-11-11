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

#include "keytool_utils.h"
#include "keytool_view_imp.h"
#include "keytool_commands.h"
#include "certtool_controller.h"
#include "keytoolfileview.h"


// Boiler plate 
_LIT(KShortName, "Symbian OS CertTool");
_LIT(KName, "Symbian OS CertStore Manipulation Tool");
_LIT(KCopyright, "Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).  All rights reserved.");

_LIT(KNewLine, "\n");

_LIT(KDone, "Press any key to continue... \n");

// CertTool command line parameters

_LIT(KList, "-list");
_LIT(KListShort, "-l");

_LIT(KListStores, "-liststores");
_LIT(KListStoresShort, "-ls");

_LIT(KImport, "-import");
_LIT(KImportShort, "-i");

_LIT(KPrivate, "-private");


_LIT(KSetApps, "-setapps");
_LIT(KSetAppsShort, "-s");

_LIT(KAddApps, "-addapps");
_LIT(KAddAppsShort, "-a");

_LIT(KApps, "-apps");

_LIT(KRemoveApps, "-removeapps");

_LIT(KRemove, "-remove");
_LIT(KRemoveShort, "-r");

// remove private key also while removing the certificate
// which is applicable iff the key is imported using the certool -private option

_LIT(KRemoveKeyAlso, "-rka");

_LIT(KStore, "-store");

_LIT(KHelp, "-help");
_LIT(KHelpShort, "-h");

// Command parameters
_LIT(KLabel, "-label");

_LIT(KDetails, "-details");
_LIT(KDetailsShort, "-d");

_LIT(KOwnerType, "-owner");
_LIT(KOwnerTypeShort, "-o");

_LIT(KPageWise, "-page");
_LIT(KPageWiseShort, "-p");

_LIT(KUids, "-uids");

_LIT(KDeletable, "-deletable");
_LIT(KDeletableShort, "-del");

const TInt KMaxArgs = 10;

/**
 * Certtool can operate in the following modes.
**/
enum OperationMode {
Interactive,
NonInteractive
};


/**
 * Displays tool name and copy-right informations.
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
		aCmdNum = CertToolDefController::KUsageCommand;
		aCmdCount = KMaxArgs;
		return 1;
		}
	if (aCommand.CompareF(KList) == 0 || aCommand.Compare(KListShort) == 0)
		{
		aCmdNum = CertToolDefController::KListCommand;
		}
	else if (aCommand.CompareF(KListStores) == 0 || aCommand.Compare(KListStoresShort) == 0)
		{
		aCmdNum = CertToolDefController::KListStoresCommand;
		}
	else if	(aCommand.CompareF(KImport) == 0 || aCommand.Compare(KImportShort) == 0)
		{
		aCmdNum = CertToolDefController::KImportCommand;
		}
	else if	(aCommand.CompareF(KRemove) == 0 || aCommand.Compare(KRemoveShort) == 0)
		{
		aCmdNum = CertToolDefController::KRemoveCommand;
		}
	else if	(aCommand.CompareF(KSetApps) == 0 || aCommand.Compare(KSetAppsShort) == 0)
		{
		aCmdNum = CertToolDefController::KSetAppsCommand;
		}
	else if	(aCommand.CompareF(KAddApps) == 0 || aCommand.Compare(KAddAppsShort) == 0)
		{
		aCmdNum = CertToolDefController::KAddAppsCommand;
		}
	else if	(aCommand.CompareF(KRemoveApps) == 0 )
		{
		aCmdNum = CertToolDefController::KRemoveAppsCommand;
		}
	else	
		{
		return 0;
		}

	return 1;
	}
	
/**
 * Returns the mode in which the tool would operate. If the command is invoked 
 * with 2 parameters(certool inputFile outputFile), the tool works in non-interactive
 * mode else the interactive mode is chosen.
 */

LOCAL_D OperationMode ModeOfOperationL(const CCommandLineArguments& aCmdArgs, RFs& aFs, RFile& aFile)
    {
    OperationMode mode = Interactive;
    if (KeyToolUtils::DoesFileExistsL(aFs,aCmdArgs.Arg(1)))
        {
		mode = NonInteractive;
        TInt error = aFile.Open(aFs, aCmdArgs.Arg(1), EFileRead|EFileShareAny);
        aFile.Close();
        
        TInt error1 = aFile.Replace(aFs, aCmdArgs.Arg(2), EFileWrite|EFileShareExclusive);
        // If the input file doesn't exist or not able to create outputfile
        // switch to Interactive mode
        if (error != KErrNone || error1 != KErrNone)
            {
            CleanupStack::PopAndDestroy(&aFile);
            mode = Interactive;
            }   
                    
         }
    return mode;
    }

/**
 * The main parsing logic. Same for interactive and non-interactive modes.
 */
LOCAL_D void ParseAndHandleCommandL(CArrayFixFlat<TPtrC>& aArgs, CCertToolController& aController)
    {
    CKeyToolParameters* params = CKeyToolParameters::NewLC();
            
    TInt command = -1; 
    TInt i = -1;
           
    TInt argsCount = aArgs.Count();
    while (i < (argsCount-1))
        {
        i++;
        if ((aArgs.At(i).CompareF(KDetails)==0)|| (aArgs.At(i).Compare(KDetailsShort)==0))
                    {
                    params->iIsDetailed = ETrue;
                    continue;
                    }
        
                if (aArgs.At(i).CompareF(KPageWise)==0 || (aArgs.At(i).Compare(KPageWiseShort)==0))
                    {
                    i++;
                    params->iPageWise = ETrue;
                    continue;
                    }

                if (aArgs.At(i).Compare(KRemoveKeyAlso)==0)
                    {
                    params->iRemoveKey = ETrue;
                    continue;
                    }

                if (aArgs.At(i).CompareF(KApps)==0)
                    {
                    i++;
                    RArray<TUid> apps;
                    TInt k = 0;
                    for (k = i; k < argsCount; k++)
                        {               
                        if (aArgs.At(k).Find(_L("-")) == KErrNotFound)
                            {
                            TUint uid;
                            if (aArgs.At(k).CompareF(KSWInstall)==0)
                                {
                                uid = swinstalluid;
                                }
                            else
                                {
                                if (aArgs.At(k).CompareF(KSWInstallOCSP)==0)
                                    {
                                    uid = swinstallocspuid;
                                    }
                                else
                                    {
                                    if (aArgs.At(k).CompareF(KMidletInstall)==0)
                                        {
                                        uid = midletinstalluid;
                                        }
                                    else
                                        {
                                        if (aArgs.At(k).CompareF(KTls)==0)
                                            {
                                            uid = tlsuid;
                                            }
                                        else
                                            {
                                            // no more valid apps, break cycle
                                            break;
                                            }
                                        }
                                    }
                                }
                            apps.Append(TUid::Uid(uid));                        
                            }
                        else 
                            {
                            // We parsed all UIDs, break the cycle and go on!
                            break;
                            }
                        }
                    i = k-1;
                    params->iUIDs = apps; // We pass on ownership
                    params->iIsDetailed = ETrue;
                    continue;           
                    }
            
                if (aArgs.At(i).CompareF(KUids)==0)
                    {
                    i++;
                    RArray<TUid> uids;
                    TInt k = 0;
                    for (k = i; k < argsCount; k++)
                        {               
                        if (aArgs.At(k).Left(2) == _L("0x"))
                            {
                            TLex lex(aArgs.At(k).Mid(2));       
                            TUint uid =0;
                            TInt err = lex.Val(uid, EHex);
                            if (err == KErrNone)
                                {
                                params->iUIDs.Append(TUid::Uid(uid));                       
                                }            
                            }
                        else 
                            {
                            // We parsed all UIDs, break the cycle and go on!
                            break;
                            }
                        }
                    i = k-1;
                    params->iIsDetailed = ETrue;
                    continue;           
                    }
                
                TDesC& cmd = aArgs.At(i);
                if (cmd.CompareF(KLabel) == 0 || 
                    cmd.CompareF(KPrivate) == 0 || cmd.CompareF(KStore) == 0 ||
                    cmd.CompareF(KOwnerType) == 0 || cmd.Compare(KOwnerTypeShort) == 0 ||
                    cmd.CompareF(KHelp) == 0 || cmd.Compare(KHelpShort) == 0 ||
                    cmd.CompareF(KDeletable) == 0 || cmd.CompareF(KDeletableShort) == 0)
                    {
                    i++;
                    if (i >= argsCount || aArgs.At(i)[0] == '-')
                        {
                        i = argsCount;
                        command = CertToolDefController::KUsageCommand;
                        }
                    else if (cmd.CompareF(KHelp) == 0 || cmd.Compare(KHelpShort) == 0)
                        {
                        params->iDefault = aArgs.At(i).AllocL();
                        i = argsCount;  
                        }
                    else if (cmd.CompareF(KLabel) == 0)
                        {
                        params->iLabel = aArgs.At(i).AllocL();
                        }
                    else if (cmd.CompareF(KPrivate) == 0)
                        {
                        params->iPrivate = aArgs.At(i).AllocL();
                        }
                    else if (cmd.CompareF(KStore) == 0)
                        {
                        TLex parser(aArgs.At(i));
                        TInt err = parser.Val(params->iCertstoreIndex);
                        params->iIsDetailed = ETrue;
                        }
                    else if (cmd.CompareF(KOwnerType) == 0 || cmd.Compare(KOwnerTypeShort) == 0)
                        {
                        params->iIsDetailed = ETrue;
                        params->iOwnerType = aArgs.At(i).AllocL();
                        }
                    else if (cmd.CompareF(KDeletable) == 0 || cmd.CompareF(KDeletableShort) == 0)
                        {
                        params->iIsDetailed = ETrue;
                        params->iIsDeletable = aArgs.At(i).AllocL();
                        }
                    continue;
                    }
        
                if (VerifyCommand(aArgs.At(i), command, i))
                    {
                    continue;
                    }

                    
                if (i!=0) 
                    {
                    if (aArgs.At(i)[0] == '-')
                        {
                        i = argsCount;
                        command = CertToolDefController::KUsageCommand;
                        continue;
                        }
                    delete params->iDefault;
                    params->iDefault = NULL;
                    params->iDefault = aArgs.At(i).AllocL();
                    params->iIsDetailed = ETrue;            
                    }
                } 
             
             
            if (command != -1)
                {
                TRAP_IGNORE(aController.HandleCommandL(command, params));
                }
            else 
                {
                aController.HandleCommandL(CertToolDefController::KUsageCommand, params);       
                }
            CleanupStack::PopAndDestroy(params);
            }


/**
 * Parsing the command for non-interactive mode.
 */

LOCAL_D void ParseCommandInNonInteractiveModeL(RFile& aFile, const CCommandLineArguments& aCmdArgs)
    {
  
    KeyToolUtils::SetFile(&aFile);
    
    CKeytoolFileView* view(0); 
    view = CKeytoolFileView::NewLC(aCmdArgs.Arg(1));
    TInt cmdCount = view->SplitFileInputToArrayL();
    
    //For every command, parse and handle.
    for (TInt j = 0; j < cmdCount; j++)
        {
        CCertToolController* controller = CCertToolController::NewLC(*view);
        
        CArrayFixFlat<TPtrC>* args = view->ReadArrayArgumentsLC(j);
        ParseAndHandleCommandL(*args, *controller);
        
        CleanupStack::PopAndDestroy(2, controller);
        }
        
    
    CleanupStack::PopAndDestroy(view);
    
    }


/**
 * Parsing the command for interactive mode.
 */


LOCAL_D void ParseCommandInInteractiveModeL(CConsoleBase& aConsole, const CCommandLineArguments& aCmdArgs)
    {
    CArrayFixFlat<TPtrC>* args = new (ELeave) CArrayFixFlat<TPtrC> (10);
    CleanupStack::PushL(args);
    CKeytoolConsoleView* view = CKeytoolConsoleView::NewLC(aConsole);
    CCertToolController* controller = CCertToolController::NewLC(*view);

    TInt cmdArgsCount = aCmdArgs.Count();
    
    KeyToolUtils::SetConsole(&aConsole);
    BoilerPlateL(&aConsole);
    
    for (TInt i = 0; i < cmdArgsCount; i++)
        {
        args->AppendL(aCmdArgs.Arg(i));
        }
    
    //Interactive mode can handle only one command at a time.
    ParseAndHandleCommandL(*args, *controller); 
    
    // We are done!
    aConsole.Printf(KNewLine);
    aConsole.Printf(KDone);
    aConsole.Getch(); 
    
    CleanupStack::PopAndDestroy(3, args); // controller, view, args
    
    }

/**
 * Parses the command line and given control to the handler to deal with the request.
 */
LOCAL_D void DoMainL() 
	{

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile file;
	
	CConsoleBase* console = Console::NewL(KShortName, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);
	CCommandLineArguments* cmdArgs = CCommandLineArguments::NewLC();
	TInt cmdArgsCount = cmdArgs->Count();
	
	
	OperationMode currentMode = Interactive; //Interactive by default.
	
	// Determine the mode of operation as either interactive or non-interactive.
	if (cmdArgsCount == 3)
		{
		currentMode = ModeOfOperationL(*cmdArgs, fs, file);
		}
		
	switch(currentMode)
	    {
	    case Interactive:
	        ParseCommandInInteractiveModeL(*console, *cmdArgs);
	        break;
	        
	    case NonInteractive:
	        //file refers to the output file name.
	        CleanupClosePushL(file);
	        ParseCommandInNonInteractiveModeL(file, *cmdArgs);
	        CleanupStack::PopAndDestroy(&file);
	        break;
	    }

	CleanupStack::PopAndDestroy(3, &fs);
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

