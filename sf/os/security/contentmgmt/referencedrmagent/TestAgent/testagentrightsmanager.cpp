/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <caf/rightsinfo.h>
#include "testagentrightsmanager.h"

using namespace ContentAccess;

_LIT(KRightsDescription,"Rights");
_LIT(KRightsUniqueId, "UniqueId");

CTestAgentRightsManager* CTestAgentRightsManager::NewL()
	{
	CTestAgentRightsManager* self=new(ELeave) CTestAgentRightsManager();
	return self;
	}

CTestAgentRightsManager::CTestAgentRightsManager()
	{
	}

CTestAgentRightsManager::~CTestAgentRightsManager()
	{
	}
  
void  CTestAgentRightsManager::ListAllRightsL (RStreamablePtrArray< CRightsInfo > &aArray) const
	{
	TRightsStatus rightsStatus= ERightsStatusNone;
	CRightsInfo *rightsInfo = CRightsInfo::NewL(KRightsDescription(), KRightsUniqueId(), ERightsTypeConsumable , rightsStatus);
	aArray.AppendL(rightsInfo);
	}
	
void  CTestAgentRightsManager::ListRightsL (RStreamablePtrArray< CRightsInfo > &aArray, const TDesC &) const
	{
	TRightsStatus rightsStatus= ERightsStatusNone;
	CRightsInfo *rightsInfo = CRightsInfo::NewL(KRightsDescription(), KRightsUniqueId(), ERightsTypeConsumable , rightsStatus);
	aArray.AppendL(rightsInfo);
	}
	
void  CTestAgentRightsManager::ListRightsL (RStreamablePtrArray< CRightsInfo > &aArray, TVirtualPathPtr& /*aVirtualPath*/) const
	{
	TRightsStatus rightsStatus= ERightsStatusNone;
	CRightsInfo *rightsInfo = CRightsInfo::NewL(KRightsDescription(), KRightsUniqueId(), ERightsTypeConsumable , rightsStatus);
	aArray.AppendL(rightsInfo);
	}
	
void  CTestAgentRightsManager::ListContentL (RStreamablePtrArray< CVirtualPath > &aArray, CRightsInfo &aRightsInfo) const
	{
	CVirtualPath *virtualPath= CVirtualPath::NewL(aRightsInfo.Description());
	aArray.AppendL(virtualPath);
	}
	
MAgentRightsBase *CTestAgentRightsManager::GetRightsDataL (const CRightsInfo& /*aRightsInfo*/) const
	{
	return NULL;
	}
	
TInt CTestAgentRightsManager::DeleteRightsObject (const CRightsInfo &aRightsInfo)
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
	
TInt CTestAgentRightsManager::DeleteAllRightsObjects (const TVirtualPathPtr& /*aVirtualPath*/)
	{
	return KErrNone;
	}

TInt CTestAgentRightsManager::SetProperty(TAgentProperty , TInt)
	{
	return KErrNotSupported;
	}
