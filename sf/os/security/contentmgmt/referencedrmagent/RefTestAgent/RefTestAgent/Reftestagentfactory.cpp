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


#include <caf/caf.h>
#include "Reftestagentfactory.h"
#include "Reftestagentcontent.h"
#include "Reftestagentdata.h"
#include "Reftestagentmanager.h"
#include "Reftestagentimportfile.h"
#include "Reftestagentrightsmanager.h"

using namespace ContentAccess;

CAgentFactory* CRefTestAgentFactory::NewL()
	{
	return new (ELeave) CRefTestAgentFactory();
	}

CAgentContent* CRefTestAgentFactory::CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode)
	{
	return CRefTestAgentContent::NewL(aURI, aShareMode);
	}

CAgentContent* CRefTestAgentFactory::CreateContentBrowserL(RFile& aFile)
	{
	return CRefTestAgentContent::NewL(aFile);
	}

CAgentData* CRefTestAgentFactory::CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	return CRefTestAgentData::NewL(aVirtualPath, aShareMode);
	}

CAgentData* CRefTestAgentFactory::CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId)
	{
	return CRefTestAgentData::NewL(aFile, aUniqueId);
	}

CAgentManager*  CRefTestAgentFactory::CreateManagerL()
	{
	return CRefTestAgentManager::NewL();
	}

CAgentImportFile* CRefTestAgentFactory::CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName)
	{
	return CRefTestAgentImportFile::NewL(aMimeType, aMetaDataArray, aOutputDirectory, aSuggestedFileName);
	}

CAgentImportFile* CRefTestAgentFactory::CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray)
	{
	return CRefTestAgentImportFile::NewL(aMimeType, aMetaDataArray);
	}

CAgentRightsManager* CRefTestAgentFactory::CreateRightsManagerL()
	{
	return CRefTestAgentRightsManager::NewL();
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
CAgentData* CRefTestAgentFactory::CreateDataConsumerL(const TDesC8& aHeaderData)     
    {     
    return CRefTestAgentData::NewL(aHeaderData);     
    }     
      
CAgentContent* CRefTestAgentFactory::CreateContentBrowserL(const TDesC8& aHeaderData)     
    {     
    return CRefTestAgentContent::NewL(aHeaderData);     
    }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
