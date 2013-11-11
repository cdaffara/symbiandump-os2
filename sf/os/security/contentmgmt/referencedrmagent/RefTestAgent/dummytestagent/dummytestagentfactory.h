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



#ifndef __DUMMYTESTAGENTFACTORY_H__
#define __DUMMYTESTAGENTFACTORY_H__

#include <caf/caf.h>
#include <caf/agentfactory.h>

	class CDummyTestAgentFactory : public ContentAccess::CAgentFactory
		{
	public:
		static CAgentFactory* NewL();
		
		// From CAgentFactory
		ContentAccess::CAgentContent* CreateContentBrowserL(const TDesC& aURI, ContentAccess::TContentShareMode aShareMode);	
		ContentAccess::CAgentContent* CreateContentBrowserL(RFile& aFile);
		ContentAccess::CAgentData* CreateDataConsumerL(const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::TContentShareMode aShareMode);
		ContentAccess::CAgentData* CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId);
		ContentAccess::CAgentManager*  CreateManagerL();
		ContentAccess::CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const ContentAccess::CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName);
		ContentAccess::CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const ContentAccess::CMetaDataArray& aMetaDataArray);
		ContentAccess::CAgentRightsManager* CreateRightsManagerL();	
		};

#endif // __DUMMYTESTAGENTFACTORY_H__
