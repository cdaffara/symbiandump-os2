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


/**
 @file
 @internalComponent
 @released
*/


#ifndef __F32AGENTFACTORY_H__
#define __F32AGENTFACTORY_H__

#include <caf/agentfactory.h>

namespace ContentAccess
	{
	class CAgentContent;
	class CAgentManager;
	class CAgentData;

	/** 
	 Creates products that allow access to plaintext unprotected files
	 via the Content Access Framework (CAF)
	 
	 The F32 agent is treated as a special case by CAF. It is 
	 the default agent used when no other suitable agent is found
	 for a particular operation.
	 
	 No translation of the content will occur and no access restrictions
	 will be enforced aside from the usual platform security data caging
	 rules.
	 
	 @internalComponent
	 @released
	 */
	class CF32AgentFactory : public CAgentFactory
		{
	public:
		/** Entry point for F32 Agent 
		@return a new F32AgentFactory instance
		*/
		static CAgentFactory* NewL();
		
		// From CAgentFactory
		virtual CAgentContent* CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode);	
		virtual CAgentContent* CreateContentBrowserL(RFile& aFile);
		virtual CAgentData* CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
		virtual CAgentData* CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId);
		virtual CAgentManager*  CreateManagerL();

		/** The F32 Agent cannot be used to receive files
		
		The CAF framework should know this is the F32 agent and never
		attempt to call this method
		
		@panic CAF EF32AgentImportNotSupported 
		*/
		virtual CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName);
		/** The F32 Agent cannot be used to receive files

		The CAF framework should know this is the F32 agent and never
		attempt to call this method

		@panic CAF EF32AgentImportNotSupported 
		*/
		virtual CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray);
		/** The F32 Agent does not manage any rights

		The CAF framework should know this is the F32 agent and never
		attempt to call this method

		@panic CAF EF32AgentRightsManagerNotSupported 
		*/
		virtual CAgentRightsManager* CreateRightsManagerL();
		
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		virtual CAgentData* CreateDataConsumerL(const TDesC8& aHeaderData);
		virtual CAgentContent* CreateContentBrowserL(const TDesC8& aHeaderData);
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT		
		};
} // namespace ContentAccess
#endif // __F32AGENTFACTORY_H__
