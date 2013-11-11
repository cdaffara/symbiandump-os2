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
#include "testagentfactory.h"
#include "testagentcontent.h"
#include "testagentdata.h"
#include "testagentmanager.h"
#include "testagentimportfile.h"
#include "testagentrightsmanager.h"

using namespace ContentAccess;

CAgentFactory* CTestAgentFactory::NewL()
	{
	return new (ELeave) CTestAgentFactory();
	}

CAgentContent* CTestAgentFactory::CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode)
	{
	return CTestAgentContent::NewL(aURI, aShareMode);
	}

CAgentContent* CTestAgentFactory::CreateContentBrowserL(RFile& aFile)
	{
	return CTestAgentContent::NewL(aFile);
	}

CAgentData* CTestAgentFactory::CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	return CTestAgentData::NewL(aVirtualPath, aShareMode);
	}

CAgentData* CTestAgentFactory::CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId)
	{
	return CTestAgentData::NewL(aFile, aUniqueId);
	}

CAgentManager*  CTestAgentFactory::CreateManagerL()
	{
	return CTestAgentManager::NewL();
	}

CAgentImportFile* CTestAgentFactory::CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName)
	{
	return CTestAgentImportFile::NewL(aMimeType, aMetaDataArray, aOutputDirectory, aSuggestedFileName);
	}

CAgentImportFile* CTestAgentFactory::CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray)
	{
	return CTestAgentImportFile::NewL(aMimeType, aMetaDataArray);
	}

CAgentRightsManager* CTestAgentFactory::CreateRightsManagerL()
	{
	return CTestAgentRightsManager::NewL();
	}

