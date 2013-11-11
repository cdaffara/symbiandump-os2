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
#include <caf/cafpanic.h>
#include "f32agentfactory.h"
#include "f32agentcontent.h"
#include "f32agentdata.h"
#include "f32agentmanager.h"


using namespace ContentAccess;

CAgentFactory* CF32AgentFactory::NewL()
	{
	return new (ELeave) CF32AgentFactory();
	}

CAgentContent* CF32AgentFactory::CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode)
	{
	return CF32AgentContent::NewL(aURI, aShareMode);
	}

CAgentContent* CF32AgentFactory::CreateContentBrowserL(RFile& aFile)
	{
	return CF32AgentContent::NewL(aFile);
	}

CAgentData* CF32AgentFactory::CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{		
	return CF32AgentData::NewL(aVirtualPath, aShareMode);
	}

CAgentData* CF32AgentFactory::CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId)
	{
	return CF32AgentData::NewL(aFile, aUniqueId);
	}

CAgentManager*  CF32AgentFactory::CreateManagerL()
	{
	return CF32AgentManager::NewL();
	}

CAgentImportFile* CF32AgentFactory::CreateImporterL(const TDesC8& /* aMimeType */, const CMetaDataArray& /* aArray */, const TDesC& /*aOutputDirectory*/, const TDesC& /* aSuggestedFileName*/)
	{
	// The F32 agent is not used to import files and the code should never
	// reach this point
	User::Panic(KCafPanicString, ECafPanicF32AgentImportNotSupported);
	return NULL;
	}

CAgentImportFile* CF32AgentFactory::CreateImporterL(const TDesC8& /*aMimeType*/, const CMetaDataArray& /*aMetaDataArray*/)
	{
	// The F32 agent is not used to import files and the code should never
	// reach this point
	User::Panic(KCafPanicString, ECafPanicF32AgentImportNotSupported);
	return NULL;
	}

CAgentRightsManager* CF32AgentFactory::CreateRightsManagerL()
	{
	// The F32 agent does not manage any rights or access control to files
	// The code should never reach this point
	User::Panic(KCafPanicString, ECafPanicF32AgentRightManagerNotSupported);
	return NULL;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
CAgentData* CF32AgentFactory::CreateDataConsumerL(const TDesC8& aHeaderData)
	{
	return CF32AgentData::NewL(aHeaderData);
	}

CAgentContent* CF32AgentFactory::CreateContentBrowserL(const TDesC8& aHeaderData)
	{
	return CF32AgentContent::NewL(aHeaderData);
	}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
