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


#include <mctkeystore.h>
#include <f32file.h>
#include <barsc.h>
#include <barsread.h>
#include <utf.h>
 
#include "keytool_utils.h"
#include "keytool_commands.h"
#include "keytool_controller.h"

#include <keytool.rsg> 

/*static*/ CKeytoolUsage* CKeytoolUsage::NewLC(CKeyToolController* aController)
	{
	CKeytoolUsage* self = new (ELeave) CKeytoolUsage(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CKeytoolUsage* CKeytoolUsage::NewL(CKeyToolController* aController)
	{
	CKeytoolUsage* self = CKeytoolUsage::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}

CKeytoolUsage::CKeytoolUsage(CKeyToolController* aController) : CKeyToolCommand(aController)
	{
	}

void CKeytoolUsage::ConstructL()
	{
	}
	
CKeytoolUsage::~CKeytoolUsage()
	{
	}

void CKeytoolUsage::RunL()
	{
	
	}
	
void CKeytoolUsage::DoCancel()
	{
	
	}
	
void CKeytoolUsage::DoCommandL(CUnifiedKeyStore& /*aKeyStore*/, CKeyToolParameters* aParam)
	{
	// Depending on parameters show the help!
	if (!aParam->iDefault)
		{
		// Display general usage!
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE2);
#ifdef SYMBIAN_AUTH_SERVER
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE5);
#endif // SYMBIAN_AUTH_SERVER
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE3);		        
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE4);
        return;
		}
		
	if (!aParam->iDefault->Compare(_L("import")) )
		{
		// Display detailed *import* help
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_IMPORT);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_IMPORT2);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_IMPORT3);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_IMPORT4);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_KEYUSAGES);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_KEYACCESSES);	        
		}
	else
	if (!aParam->iDefault->Compare(_L("list")) )
		{
		// Display detailed *import* help
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_LIST);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_LIST2);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_LIST3);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_LIST4);                        
		}
	else
	if (!aParam->iDefault->Compare(_L("liststores")) )
		{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_LISTSTORES);                       
		}
	else
	if (!aParam->iDefault->Compare(_L("page")) )
		{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_PAGE);                       
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_PAGE2);                       
		}
	else
	if (!aParam->iDefault->Compare(_L("remove")) )
		{
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_REMOVE);
		}
	else		
	if (!aParam->iDefault->Compare(_L("usage")) )
		{
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_KEYUSAGE);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_KEYUSAGES);
		}
	else
	if (!aParam->iDefault->Compare(_L("removeuser")) )
		{
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_REMOVEUSER);
		}
	else
	if (!aParam->iDefault->Compare(_L("setuser")) )
		{
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_SETUSER);
		}
	else
	if (!aParam->iDefault->Compare(_L("setmanager")) )
		{
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_SETMANAGER);
        }
	else
	if (!aParam->iDefault->Compare(_L("setallusers")) )
		{
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_SETALLUSERS);
		}
	else
	if (!aParam->iDefault->Compare(_L("noninteractive")) )
		{
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_NONINTERACTIVE);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_NONINTERACTIVE1);	        
		}
	else			
	if (!aParam->iDefault->Compare(_L("access")) )
		{
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_KEYACCESS);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_KEYACCESSES);	        
		}
	
#ifdef KEYTOOL
	else if( !aParam->iDefault->Compare(_L("migratestore")) )
		{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE1);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE2);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE3);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE4);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE5);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE6);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE7);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE8);
		iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE_MIGRATE9);
		}
#endif // KEYTOOL
	else
		{
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE);
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE2);		
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE3);		
        iController->DisplayLocalisedMsgL(R_KEYTOOL_USAGE4);
		}						
	}
