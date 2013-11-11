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


#include "keytool_utils.h"
#include "certtool_commands.h"
#include "certtool_controller.h"


/*static*/ CCertToolUsage* CCertToolUsage::NewLC(CCertToolController* aController)
	{
	CCertToolUsage* self = new (ELeave) CCertToolUsage(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CCertToolUsage* CCertToolUsage::NewL(CCertToolController* aController)
	{
	CCertToolUsage* self = CCertToolUsage::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}

CCertToolUsage::CCertToolUsage(CCertToolController* aController) : CCertToolCommand(aController)
	{
	}

void CCertToolUsage::ConstructL()
	{
	}
	
CCertToolUsage::~CCertToolUsage()
	{
	}

void CCertToolUsage::RunL()
	{
	
	}
	
void CCertToolUsage::DoCancel()
	{
	
	}
	
void CCertToolUsage::DoCommandL(CUnifiedCertStore& /*aCertStore*/, CKeyToolParameters* aParam)
	{
	// Depending on parameters show the help!
	if (!aParam->iDefault)
		{
		// Display general usage!
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE2);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE3);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE4);
        return;		
		}

	if (!aParam->iDefault->Compare(_L("import")) )
		{
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_IMPORT);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_IMPORT2);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_IMPORT3);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_IMPORT4);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_IMPORT5);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_IMPORT6);
		}
	else
	if (!aParam->iDefault->Compare(_L("list")) )
		{
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_LIST);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_LIST2);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_LIST3);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_LIST4);
		}
	else
	if (!aParam->iDefault->Compare(_L("liststores")) )
		{
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_LISTSTORES);
		}
	else	
	if (!aParam->iDefault->Compare(_L("remove")) )
		{
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_REMOVE);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_REMOVE2);
		}
	else	
	if (!aParam->iDefault->Compare(_L("setapps")) )
		{
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_SETAPPS);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_SETAPPS2);
		}
	else					   	
	if (!aParam->iDefault->Compare(_L("addapps")) )
		{
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_ADDAPPS);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_ADDAPPS2);	        
		}
	else					   	
	if (!aParam->iDefault->Compare(_L("removeapps")) )
		{
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_REMOVEAPPS);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_REMOVEAPPS2);	        
		}
	else					   	
	if (!aParam->iDefault->Compare(_L("noninteractive")) )
		{
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_NONINTERACTIVE);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_NONINTERACTIVE1);	        
		}
	else
	if ( !aParam->iDefault->Compare( _L("expiry") ) )
		{
//		iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_EXPIRY);
		}
	else
	if ( !aParam->iDefault->Compare( _L("page") ) )
		{
		iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_PAGE);
		iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_PAGE2);
		}
	else
		{
		// Display general usage!
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE2);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE3);
        iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE4);		
		}
	}
