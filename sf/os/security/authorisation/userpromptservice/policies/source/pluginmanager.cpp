/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ecom/ecom.h>
#include "pluginmanager.h"
#include <ups/dialogcreator.h>
#include <ups/policyevaluator.h>

using namespace UserPromptService;

// CPluginManager -----------------------------------------------------------
CPluginManager::CPluginManager()
/**
Constructor
*/
	{		
	}
	
CPluginManager::~CPluginManager()
/**
Destructor
*/
	{
	REComSession::FinalClose();	
	}

EXPORT_C CPluginManager* CPluginManager::NewL()
/**
Factory function for plug-in managers.
@return A pointer to the new plug-in manager.
*/
	{
	CPluginManager* self = new(ELeave) CPluginManager();
	return self;
	}

EXPORT_C void CPluginManager::Unload()
/**
Schedules a deferred call to REComSession::FinalClose. This is
invoked once the last plug-in has been freed.
*/
	{
	if (iPluginCount == 0)
		{
		REComSession::FinalClose();
		iUnload = EFalse;
		}
	else 
		{
		iUnload = ETrue;
		}
	}
	
EXPORT_C void CPluginManager::ReleasePlugin()
/**
Called by CPlugin to decrement the count of active plug-ins. If
Unload has been called then REComSession::FinalClose is invoked when 
the plug-in count reaches zero.
*/
	{
	if (--iPluginCount == 0 && iUnload)
		{
		iUnload = EFalse;
		REComSession::FinalClose();
		}
	}
	
EXPORT_C CPlugin<CPolicyEvaluator>* CPluginManager::CreateEvaluatorL(const TUid& aEvaluatorId)
/**
Instantiates a new CPolicyEvaluator plug-in.
@param aEvaluatorId	The ECOM implementation UID of the policy evaluator to instantiate.
@return				A pointer to the new policy evaluator plug-in.
*/
	{
	CPolicyEvaluator* e = CPolicyEvaluator::NewL(aEvaluatorId);
	CleanupStack::PushL(e);
	CPlugin<CPolicyEvaluator>* p = new(ELeave) CPlugin<CPolicyEvaluator>(this, e);
	CleanupStack::Pop(e);
	++iPluginCount;
	return p;
	}

EXPORT_C CPlugin<CDialogCreator>* CPluginManager::CreateDialogCreatorL(const TUid& aDialogCreatorId)
	{
/**
Instantiates a new CDialogCreator plug-in.
@param	aDialogCreatorId	The ECOM implementation UID of the dialog creator to instantiate.
@return						A pointer to the new dialog creator plug-in.
*/
	CDialogCreator* d = CDialogCreator::NewL(aDialogCreatorId);
	CleanupStack::PushL(d);
	CPlugin<CDialogCreator>* p = new(ELeave) CPlugin<CDialogCreator>(this, d);
	CleanupStack::Pop(d);
	++iPluginCount;
	return p;
	}
