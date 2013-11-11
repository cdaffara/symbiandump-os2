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


#include <barsc.h>
#include <barsread.h>
#include <utf.h>

#include "keytool_controller.h"
#include "keytool_view.h"
#include "keytool_commands.h"

#include <keytool.rsg>

_LIT(KKeyToolResources,"Z:\\Resource\\Apps\\keytool.RSC");

CKeyToolController::CKeyToolController(MKeyToolView& aView) : CController(aView)
	{
	}

CKeyToolController::~CKeyToolController()
	{	
	delete iEngine; 
	//iFsSession.Close();
	//iResourceFile.Close();
	}

/*static*/ CKeyToolController*  CKeyToolController::NewLC(MKeyToolView& aView)
	{
	CKeyToolController* self = new (ELeave) CKeyToolController(aView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self; 
	}

/*static*/ CKeyToolController*  CKeyToolController::NewL(MKeyToolView& aView)
	{
	CKeyToolController* self = CKeyToolController::NewLC(aView);
	CleanupStack::Pop(self);
	return self; 
	}

void CKeyToolController::ConstructL()
	{
	User::LeaveIfError(iFsSession.Connect()); // Connect session
 
	TFileName fileName(KKeyToolResources);
	 
	// open file, leave if error
	iResourceFile.OpenL(iFsSession, fileName);

	iResourceFile.ConfirmSignatureL();

	iEngine = CKeyToolEngine::NewL(this);
	}

void CKeyToolController::HandleCommandL(TInt aCommand, CKeyToolParameters* aParam)
	{
	switch (aCommand) 
		{
		case KUsageCommand:
			{
			//iView.DisplayUsage();
			iEngine->DisplayUsageL(aParam);
			}
			break;
		case KListCommand:
			{
			iEngine->ListL(aParam);			
			CActiveScheduler::Start();	
			}
			break;
		case KImportCommand:
			{
			// wildcard support
			if (aParam->iDefault)
				{
				CDir* dirList;
				RFs fs;
				CleanupClosePushL(fs);
				User::LeaveIfError(fs.Connect());
				User::LeaveIfError(fs.GetDir(*aParam->iDefault, KEntryAttNormal, ESortByName, dirList));
				CleanupStack::PushL(dirList);
				if (dirList->Count() == 0)
					{
					DisplayLocalisedMsgL(R_KEYTOOL_ERR_OPENFAIL);				
					User::Leave(KErrNotFound);					
					}
				else
					{
					TParse fp;
					fs.Parse(*aParam->iDefault,fp);	
					TPtrC path = fp.DriveAndPath();
					
					// It's done its job let's take it out of the pic!
					//delete aParam->iDefault;
					
					for (TInt k = 0; k < dirList->Count(); k++)
						{
						HBufC* fullPath = HBufC::NewMaxL(path.Length() + KMaxFileName);
						TPtr fullPathPtr(fullPath->Des());
						fullPathPtr.SetLength(0);
						fullPathPtr.Append(path);
				
						const TEntry& entry = (*dirList)[k];
						fullPathPtr.Append(entry.iName);						
						delete aParam->iDefault;
						aParam->iDefault = NULL;

						aParam->iDefault = fullPath;
						if (dirList->Count() > 1)
							{
							delete aParam->iLabel;
							aParam->iLabel = NULL;
							aParam->iLabel = entry.iName.AllocL();
							}
						
						iEngine->ImportL(aParam);
						CActiveScheduler::Start();
						}
					}
				CleanupStack::PopAndDestroy(dirList);
				CleanupStack::PopAndDestroy(&fs);
				}
			else
				{
				DisplayLocalisedMsgL(R_KEYTOOL_ERR_KEYFILE);
				User::Leave(KErrArgument);				
				}
			}	
			break;	
		case KRemoveCommand:
			{
			iEngine->RemoveL(aParam);
			CActiveScheduler::Start();							
			}	
			break;
		case KListStoresCommand:
			{
			iEngine->ListStoresL(aParam);
			CActiveScheduler::Start();				
			}	
			break;
		case KSetUserCommand:
		case KSetManagerCommand:
		case KSetAllUsersCommand:
		case KRemoveUserCommand:
			{
			iEngine->SetPolicyL(aParam);
			CActiveScheduler::Start();
			iEngine->ListL(aParam);			
			CActiveScheduler::Start();
			}
			break;		
			
#ifdef KEYTOOL
		case KMigrateStore:
			{
			iEngine->MigrateStoreL(aParam);
			CActiveScheduler::Start();
			break;
			}
			
#endif // KEYTOOL
			
		default:
			{
			DisplayLocalisedMsgL(R_KEYTOOL_ERR_COMMAND);
			User::Leave(KErrNotSupported);			
			}
		}
	}
