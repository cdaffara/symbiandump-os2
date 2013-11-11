/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "dummytestagentfactory.h"

using namespace ContentAccess;

CAgentFactory* CDummyTestAgentFactory::NewL()
	{
	return new (ELeave) CDummyTestAgentFactory();
	}

CAgentContent* CDummyTestAgentFactory::CreateContentBrowserL(const TDesC& /*aURI*/, TContentShareMode /*aShareMode*/)
	{
	return NULL;
	}

CAgentContent* CDummyTestAgentFactory::CreateContentBrowserL(RFile& /*aFile*/)
	{
	return NULL;
	}

CAgentData* CDummyTestAgentFactory::CreateDataConsumerL(const TVirtualPathPtr& /*aVirtualPath*/, TContentShareMode /*aShareMode*/)
	{
	return NULL;
	}

CAgentData* CDummyTestAgentFactory::CreateDataConsumerL(RFile& /*aFile*/, const TDesC& /*aUniqueId*/)
	{
	return NULL;
	}

CAgentManager*  CDummyTestAgentFactory::CreateManagerL()
	{
	User::Leave(KErrNotSupported);
	return NULL; //remove compiler warning
	}

CAgentImportFile* CDummyTestAgentFactory::CreateImporterL(const TDesC8& /*aMimeType*/, const CMetaDataArray& /*aMetaDataArray*/, const TDesC& /*aOutputDirectory*/, const TDesC& /*aSuggestedFileName*/)
	{
	return NULL;
	}

CAgentImportFile* CDummyTestAgentFactory::CreateImporterL(const TDesC8& /*aMimeType*/, const CMetaDataArray& /*aMetaDataArray*/)
	{
	return NULL;
	}

CAgentRightsManager* CDummyTestAgentFactory::CreateRightsManagerL()
	{
	User::Leave(KErrNotSupported);
	return NULL; //remove compiler warning
	}

