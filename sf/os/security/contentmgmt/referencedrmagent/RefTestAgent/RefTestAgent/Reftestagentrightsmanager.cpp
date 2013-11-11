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
* Testagentrightsmanager.cpp
*
*/


#include <caf/caf.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <caf/cafplatform.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS
#include "Reftestagentrightsmanager.h"

using namespace ContentAccess;

_LIT(KRightsDescription,"Rights");
_LIT(KRightsUniqueId, "UniqueId");

CRefTestAgentRightsManager* CRefTestAgentRightsManager::NewL()
	{
	CRefTestAgentRightsManager* self=new(ELeave) CRefTestAgentRightsManager();
	return self;
	}

CRefTestAgentRightsManager::CRefTestAgentRightsManager()
	{
	}

CRefTestAgentRightsManager::~CRefTestAgentRightsManager()
	{
	}

void  CRefTestAgentRightsManager::ListAllRightsL (RStreamablePtrArray< CRightsInfo > &aArray) const
	{
	TRightsStatus rightsStatus= ERightsStatusNone;
	CRightsInfo *rightsInfo = CRightsInfo::NewL(KRightsDescription(), KRightsUniqueId(), ERightsTypeConsumable , rightsStatus);
	CleanupStack::PushL(rightsInfo);
	aArray.AppendL(rightsInfo);
	CleanupStack::Pop(rightsInfo);
	}
	
void  CRefTestAgentRightsManager::ListRightsL (RStreamablePtrArray< CRightsInfo > &aArray, const TDesC &) const
	{
	TRightsStatus rightsStatus= ERightsStatusNone;
	CRightsInfo *rightsInfo = CRightsInfo::NewL(KRightsDescription(), KRightsUniqueId(), ERightsTypeConsumable , rightsStatus);
	CleanupStack::PushL(rightsInfo);
	aArray.AppendL(rightsInfo);
	CleanupStack::Pop(rightsInfo);
	}
	
void  CRefTestAgentRightsManager::ListRightsL (RStreamablePtrArray< CRightsInfo > &aArray, TVirtualPathPtr &/*aVirtualPath*/) const
	{
	TRightsStatus rightsStatus= ERightsStatusNone;
	CRightsInfo *rightsInfo = CRightsInfo::NewL(KRightsDescription(), KRightsUniqueId(), ERightsTypeConsumable , rightsStatus);
	CleanupStack::PushL(rightsInfo);
	aArray.AppendL(rightsInfo);
	CleanupStack::Pop(rightsInfo);
	}
	
void  CRefTestAgentRightsManager::ListContentL (RStreamablePtrArray< CVirtualPath > &aArray, CRightsInfo &aRightsInfo) const
	{
	CVirtualPath *virtualPath= CVirtualPath::NewL(aRightsInfo.Description());
	CleanupStack::PushL(virtualPath);
	aArray.AppendL(virtualPath);
	CleanupStack::Pop(virtualPath);
	}
	
MAgentRightsBase *CRefTestAgentRightsManager::GetRightsDataL (const CRightsInfo &/*aRightsInfo*/) const
	{
	return NULL;
	}
	
TInt CRefTestAgentRightsManager::DeleteRightsObject (const CRightsInfo &aRightsInfo)
	{
	if(aRightsInfo.RightsType() == ERightsTypeConsumable)
		{
		return KErrAccessDenied;
		}
	else
		{	
		return KErrNone;
		}
	}
	
TInt CRefTestAgentRightsManager::DeleteAllRightsObjects (const TVirtualPathPtr &/*aVirtualPath*/)
	{
	return KErrNone;
	}

TInt CRefTestAgentRightsManager::SetProperty(TAgentProperty /*aProperty*/, TInt /*aValue*/)
	{
	return KErrNotSupported;
	}
