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


#include <caf/rightsmanager.h>
#include <caf/agentfactory.h>
#include "agentinfo.h"
#include <caf/agentinterface.h>

using namespace ContentAccess;

CRightsManager* CRightsManager::NewL(TUid aUid)
	{
	CRightsManager *self = new (ELeave) CRightsManager;
	CleanupStack::PushL(self);
	self->ConstructL(aUid);
	CleanupStack::Pop(self);
	return self;
	}

CRightsManager::CRightsManager()
	{
	
	}

CRightsManager::~CRightsManager()
	{
	delete iAgentRightsManager;
	delete iAgentFactory;
	REComSession::FinalClose();
	}

void CRightsManager::ConstructL(TUid aUid)
	{	
	// create our own agent factory (de-facto session handle for ECOM)
	iAgentFactory = CAgentFactory::NewL(aUid);

	// Create an instance of the Agent's CAgentData
	iAgentRightsManager = iAgentFactory->CreateRightsManagerL();
	}

EXPORT_C void CRightsManager::ListAllRightsL(RStreamablePtrArray<CRightsInfo>& aArray) const
	{
	iAgentRightsManager->ListAllRightsL(aArray);
	}

EXPORT_C void CRightsManager::ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, const TDesC& aUri) const
	{
	iAgentRightsManager->ListRightsL(aArray, aUri);
	}

EXPORT_C void CRightsManager::ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, TVirtualPathPtr& aVirtualPath) const
	{
	iAgentRightsManager->ListRightsL(aArray, aVirtualPath);
	}

EXPORT_C void CRightsManager::ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, RFile& aFile, const TDesC& aUniqueId) const	
	{
	iAgentRightsManager->ListRightsL(aArray, aFile, aUniqueId);
	}

EXPORT_C void CRightsManager::ListContentL(RStreamablePtrArray<CVirtualPath>& aArray, CRightsInfo& aRightsInfo) const
	{
	iAgentRightsManager->ListContentL(aArray, aRightsInfo);
	}

EXPORT_C MAgentRightsBase* CRightsManager::GetRightsDataL(const CRightsInfo& aRightsInfo) const
	{
	return iAgentRightsManager->GetRightsDataL(aRightsInfo);
	}

EXPORT_C TInt CRightsManager::DeleteRightsObject(const CRightsInfo& aRightsInfo)
	{
	return iAgentRightsManager->DeleteRightsObject(aRightsInfo);
	}

EXPORT_C TInt CRightsManager::DeleteAllRightsObjects(const TVirtualPathPtr& aVirtualPathPtr)
	{
	return iAgentRightsManager->DeleteAllRightsObjects(aVirtualPathPtr);
	}

EXPORT_C TInt CRightsManager::DeleteAllRightsObjects (RFile& aFile, const TDesC& aUniqueId) 
	{
	return iAgentRightsManager->DeleteAllRightsObjects(aFile, aUniqueId);
	}

EXPORT_C TInt CRightsManager::SetProperty(TAgentProperty aProperty, TInt aValue)
	{
	return iAgentRightsManager->SetProperty(aProperty, aValue);
	}

