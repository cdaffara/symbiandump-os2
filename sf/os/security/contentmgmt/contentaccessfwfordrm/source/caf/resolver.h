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


#ifndef __RESOLVER_H__
#define __RESOLVER_H__

#include <e32std.h>
#include <ecom/ecom.h>
#include <caf/caftypes.h>

namespace ContentAccess
	{
	const TUid KF32AgentImplUid = { 0x101FC2CF };

	class CAgentInfo;
	
	/**
	Used to identify the Content Access Agent who handles a particular file.
	 
	CAgentResolver builds an array of CAgentInfo objects. Each CAgentInfo object
	holds metadata related to a particular agent implementation.

	This class resolves:
	  - Content to the Agent which manages it
	  - Supplier mime-types to the agent which imports files with that mime type.

	@internalComponent
	@released
	*/
	class CAgentResolver : public CActive
			{
		public:
			/** Constructs a new CAgentResolver and puts it on the cleanup stack
			* @param aDynamicAgentUpdate ETrue if the CAgentResolver should update its list of agents when new agents appear. A CActiveScheduler must already be running when ETrue is supplied
			* @leave KErrCANoAgent If no F32 agent was found
			*/
			IMPORT_C static CAgentResolver* NewLC(const TBool aDynamicAgentUpdate);

			/** Constructs a new CAgentResolver
			* @param aDynamicAgentUpdate ETrue if the CAgentResolver should update its list of agents when new agents appear. A CActiveScheduler must already be running when ETrue is supplied
			* @leave KErrCANoAgent If no F32 agent was found
			*/
			IMPORT_C static CAgentResolver* NewL(const TBool aDynamicAgentUpdate);
			virtual ~CAgentResolver();
	
			/** Finds the agent that handles the given supplier MIME type.
			
			@param aMimeType	MIME type from the supplier application.
			@return				The CAgentInfo object associated with the agent that handles files with this MIME type.
			 
			@leave KErrCANoAgent	If no CA agent supports the given MIME type. 
			*/
			CAgentInfo& ResolveSupplierMimeL(const TDesC8& aMimeType) const;
			 
			/** Finds the agent that handles the given consumer MIME type.
				
			@note	Planning to remove this from the CAF API
			
			@param aMimeType	MIME type of the file
			@return			The CAgentInfo object associated with the agent that handles files with this MIME type.
 			*/
			CAgentInfo& ResolveConsumerMime(const TDesC8& aMimeType) const;
			
			/** Finds the agent that handles a file.
			
			@param aFile Handle of the file to be resolved.
			@return	The CAgentInfo object associated with the agent that will handle this file.
			*/
			CAgentInfo& ResolveFileL(RFile& aFile) const;			

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
			
			/** Finds the agent which handles this piece of content.
			@param aHeaderData	Header data of WMDRM file or stream content.
			@return				CAgentInfo object associated with the agent that will handle WMDRM content.	
			*/
			CAgentInfo& ResolveFileL(const TDesC8& aHeaderData);
			
#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
						
			/** Finds the agent that handles a file and returns the actual URI of the content.
			
			If the aUri is of the form "c:\\private\\agent_name\\" 
			it will be translated to the form "c:\\private\\SID\\" in
			aActualUri. Otherwise the aUri is just copied to aActualUri
			
			@param aUri	Location of file to be resolved.
			@param aActualUri The actual location of the file
			@param aShareMode The file sharing mode to be used with this file
			@return	The CAgentInfo object associated with the agent that will handle this file.
			*/
			CAgentInfo& ResolveFileL(const TDesC& aUri, TDes& aActualUri, TContentShareMode aShareMode = EContentShareReadOnly) const;

			/** Find the agent who owns a directory
			
			This function also maps a path from the agent directory name
			to it's actual name in the file system.
			
			ie. For example if an agent "x" has files stored in a private server directory
			"c:\\private\\12345678\\" 
			
			The file appear as "c:\\private\\x\\" to applications using CAF. This function
			converts "c:\\private\\x\\" back to "c:\\private\\12345678\\"

			@param aPath The path in the form c:\\private\\agent_name\\...
			@param aActualPath The path in the form c:\\private\\SID\\...
			@param aThePrivateDir to ETrue if the directory is just "\\private\\" 
			@return	The Uid of the agent who owns the directory
			*/			
			TUid ResolveDirectory(const TDesC& aPath, TDes& aActualPath, TBool& aThePrivateDir) const;
			
			/** Converts a filename containing the agent SID to the filename containing the agent name.
			@param aFileName The filename in the form "c:\\private\\12345678\\"
			@return A buffer to the new filename in the form "c:\\private\\agent_name\\"
			*/
			HBufC* ConvertAgentFileNameL(const TDesC& aFileName) const;
		
			/** Determines the MIME types of the a file and the mime type of its contents.

			This function should only be used by the CAF recogizer DLL.
			
			@param aName			The file name/URI to be investigated.
			@param aBuffer			A buffer containing bytes from the start of the file.
			@param aFileMimeType	If the file is recognized this will be populated with 
									the mime type of the file.
			@param aContentMimeType	If the file is recognized this will be populated with the mime type of the content object within the file. 

			@return					ETrue if the file is recognized by content access agent
			@return					EFalse if the file is not recognized by a content access agent
			*/
			IMPORT_C TBool DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer, TDes8& aFileMimeType, TDes8& aContentMimeType);
	
			/** Gets the preferred buffer size for recognizing files using the RecognizeFile() function.
			
			This function should only be used by the CAF recogizer DLL.  

			@return	The largest preferred buffer nominated by any agent for recognizing files using the the DoRecognize() function.
			*/
			IMPORT_C TInt PreferredBufferSize();
		
			/** Accessor for the SupplierMimeTypes array.
			*
			* @return	An array of supplier MIME type supported by CAF agents.
			*/
			IMPORT_C const RArray<TPtrC8>& SupplierMimeTypes() const;

			/** Accessor for the ConsumerMimeTypes array.
			*
			* @note	Planning to remove this from the CAF API
			*
			* @return	An array of consumer MIME types supported by CAF agents.
			*/
			IMPORT_C const RArray<TPtrC8>& ConsumerMimeTypes() const;

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT			
			/** This function should only be used by the CAF recogizer DLL.
			
			@param aHeader			Header data of WMDRM content.
			@param aFileMimeType	If the content is recognized this will be populated with 
									the mime type of the content.
			@param aContentMimeType	If the content is recognized this will be populated with the mime type of the content. 

			@return					ETrue if the content is recognized by content access agent
			@return					EFalse if the content is not recognized by a content access agent
			*/
			IMPORT_C TBool DoRecognizeL(const TDesC8& aHeader, TDes8& aFileMimeType, TDes8& aContentMimeType);
#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT	

			/** Find an agent by it's Uid */
			CAgentInfo& AgentInfoL(const TUid& aUid) const;
			
			/** Find an agent by it's name */
			CAgentInfo& AgentInfoL(const TDesC& aAgentName) const;
			
			/** Reference an agent in the iAgentInfos array by index */
			CAgentInfo& AgentInfo(TInt aIndex) const;
			
			/** The number of agents, excluding the F32Agent */
			 TInt AgentInfoCount() const;
			
			/** The default F32 agent */
			TUid DefaultAgentUid() const;

		private:
			// From CActive, used for dynamically updating the list of content access agents
			virtual void DoCancel();
			virtual void RunL();
		
		private:
			CAgentResolver(const TBool aDynamicAgentUpdate);
			
			/** Second phase constructor for CAgentResolver
			*/
			void ConstructL();


			/** Builds a list of all the Content Access Agents. */
			void BuildListOfAgentsL();
			
			/** Add an agent to the list of agents */
			void AddAgentL(const CImplementationInformation& aImplInfo);

			/** Resets the list. */
			void DestroyListOfAgents();

			/** Is this the F32 Agent 
			@param aAgentInfo The agentinfo representing the agent
			@return ETrue if it is the F32Agent, EFalse otherwise
			*/
			TBool IsF32Agent(CAgentInfo& aAgentInfo);

			// Cleanup item
			static void CleanImplArray(TAny* aArray);

		private:
			const TBool iDynamicAgentUpdate;
			REComSession* iEcomSession;

			RArray<TPtrC8> iSupplierMimeTypes;
			RArray<TPtrC8> iConsumerMimeTypes;
	
			// Array of agentInfos (not including F32 agent)
			RPointerArray <CAgentInfo> iAgentInfos;

			// This points to the agent which handles content if it is not
			// recognised by any other agent (this should be the F32Agent)
			CAgentInfo* iDefaultAgent;
			};

	} // namespace ContentAccess
#endif /* __RESOLVER_H__ */

