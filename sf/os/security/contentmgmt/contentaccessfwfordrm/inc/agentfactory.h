/*
* Copyright (c) 2003 - 2007,2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

@publishedPartner
@released
*/

#ifndef __AGENTFACTORY_H__
#define __AGENTFACTORY_H__

#include <e32base.h>
#include <ecom/ecom.h>
#include <caf/caftypes.h>

namespace ContentAccess
{
	// Products
	 class CAgentContent;
	 class CAgentData;
	 class CAgentImportFile;
	 class CAgentManager;
	 class CAgentRightsManager;

	// Other CAF classes
	class CMetaData;
	class CMetaDataArray;
	class TVirtualPathPtr;


	/** The CAF 2.0 agent interface UID */
	const TUid KCAAgentInterfaceUid = {0x10204740};

	/**
	 Abstract interface handed out by an ECOM agent
	 implementation by REComSession::CreateImplementationL().

	 CAgentFactory defines an abstract factory that is responsible for
	 creating the abstract products: 
	 @li ContentAccess::CAgentContent
	 @li ContentAccess::CAgentData
	 @li ContentAccess::CAgentImportFile
	 @li ContentAccess::CAgentManager
	 @li ContentAccess::CAgentRightsManager
	 
	 Derived classes will hand out concrete product implementations which are 
	 specific for that particular agent.

	 Derived classes effectively represent the ECOM session handle
	 used by CAF classes
	  
	 @see ContentAccess::CF32AgentFactory. This implements 
	 ContentAccess::CAgentFactory to handle unrestricted content.
	 
 	 @publishedPartner
	 @released
	 */
	class CAgentFactory : public CBase
		{
	public:
		/** Create a new instance of a particular agent using its DLL implementation Uid
		@param aUid The Uid of the agent to create an instance of its CAgentFactory
		@return a new CAgentFactory

		@internalComponent
		@released
		*/
		static CAgentFactory* NewL(TUid aUid);
	
	/** Destructor, calls REComSession::DestroyedImplementation(); to destroy 
		the ECOM implementation. When all implementations are destroyed
		ECOM will unload the agent DLL
		*/
		IMPORT_C virtual ~CAgentFactory(); 

		/**
		Factory function creates a CAgentContent object for browsing
		the contents of a file belonging to this agent.
		 
		@param aURI	The location of the file.
		@param aShareMode The share mode used for opening this content
		@return A pointer to the CAgentContent object.
		*/
		virtual CAgentContent* CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode)  = 0;

		/**
		Creates a CAgentContent object for browsing the contents of a file 
		belonging to this agent

		@param aFile An open file handle for reading from the file, the agent must make its own duplicate of the file handle.
		@return A pointer to the CAgentContent object.
		*/
		virtual CAgentContent* CreateContentBrowserL(RFile& aFile) = 0;

		/**
		 Factory function creates a CAgentData object for reading 
		 from a content object
		 
		 @param aVirtualPath The content object to read from
		 @param aShareMode The share mode for opening the file containing the content object
		 @return A pointer to the CAgentData object.
		 */
		 virtual CAgentData* CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)  = 0;

		/**
		 Factory function creates a CAgentData object for reading 
		 from a content object.
		 
		 @param aFile An open file handle used by the agent to read from the file. The agent MUST make its own duplicate of this file handle, either in the same process or a server process
		 @param aUniqueId The agent specific unique ID of object within the file to read.
		 @return A pointer to the CAgentData object.
		 */
		virtual CAgentData* CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId)  = 0;

		/** 
		 Factory function creates an object for performing management 
		 functions with this particular agent

		 CAgentManager objects are stateless so they require no 
		 construction parameters.
		 
		 @return A pointer to a CAgentManager object.
		 */
		virtual CAgentManager*  CreateManagerL()   = 0;
		
		/** 
		 Factory function creates an object that allows the agent to import 
		 or transform content. The agent will supply output files where necessary
		 
		 @param aMimeType The mime type of the file to be imported
		 @param aMetaDataArray Array of metadata associated with the file to import
		 @param aOutputDirectory Preferred directory to store output files
		 @param aSuggestedFileName Suggested filename for the output file(s)
		 @return A pointer to a ContentAccess::CAgentImportFile object.
		 @leave KErrCANotSupported If the agent does not support importing or transforming content
		 */
		virtual CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName) = 0;

		/** 
		 Factory function creates an object that allows the agent to import 
		 or transform content. The caller will supply output files where necessary
		 
		 @param aMimeType The mime type of the file to be imported
		 @param aMetaDataArray Array of metadata associated with the file to import
		 @return A pointer to a ContentAccess::CAgentImportFile object.
		 @leave KErrCANotSupported If the agent does not support importing or transforming content
		 */
		virtual CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray) = 0;
		
		/** 
		Factory function that creates an rights manager object for this agent

		@return A pointer to a ContentAccess::CAgentRightsManager object.
		@leave KErrCANotSupported If the agent does not provide any rights management functions
		*/
		virtual CAgentRightsManager* CreateRightsManagerL() = 0;

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT		
		/**
		Factory function creates a CAgentData object from header data of WMDRM file or stream content
		for reading/decrypting WMDRM data packets.
		 
		@param aHeaderData	Header data of WMDRM file or stream content.
		@return				A pointer to CAgentData object.
		*/
		IMPORT_C virtual CAgentData* CreateDataConsumerL(const TDesC8& aHeaderData);
		
		/**
		Factory function creates a CAgentContent object from header data of WMDRM file or stream content
		for browsing WMDRM content.
		 
		@param aHeaderData	Header data of WMDRM file or stream content.
		@return 			A pointer to CAgentContent object.
		*/
		IMPORT_C virtual CAgentContent* CreateContentBrowserL(const TDesC8& aHeaderData);
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

	private:
		// ECOM session key. This is set by 
		// REComSession::CreateImplementationL() during construction
		TUid iEcomKey;
		};

	} // namespace ContentAccess
#endif // __AGENTFACTORY_H__
