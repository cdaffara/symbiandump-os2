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



#ifndef __REFTESTAGENTFACTORY_H__
#define __REFTESTAGENTFACTORY_H__

#include <caf/caf.h>
#include <caf/agentfactory.h>

	class CRefTestAgentFactory : public ContentAccess::CAgentFactory
		{
	public:
		static CAgentFactory* NewL();
		
		// From CAgentFactory
		virtual ContentAccess::CAgentContent* CreateContentBrowserL(const TDesC& aURI, ContentAccess::TContentShareMode aShareMode);	
		virtual ContentAccess::CAgentContent* CreateContentBrowserL(RFile& aFile);
		virtual ContentAccess::CAgentData* CreateDataConsumerL(const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::TContentShareMode aShareMode);
		virtual ContentAccess::CAgentData* CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId);
		virtual ContentAccess::CAgentManager*  CreateManagerL();
		virtual ContentAccess::CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const ContentAccess::CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName);
		virtual ContentAccess::CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const ContentAccess::CMetaDataArray& aMetaDataArray);
		virtual ContentAccess::CAgentRightsManager* CreateRightsManagerL();	
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        virtual ContentAccess::CAgentData* CreateDataConsumerL(const TDesC8& aHeaderData);     
        virtual ContentAccess::CAgentContent* CreateContentBrowserL(const TDesC8& aHeaderData);     
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT   
		};

#endif // __REFTESTAGENTFACTORY_H__
