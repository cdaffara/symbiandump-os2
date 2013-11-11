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


#ifndef __AGENTINFO_H__
#define __AGENTINFO_H__

#include <e32base.h>

class CImplementationInformation;

#include <caf/agent.h>

namespace ContentAccess
	{
	class CAgentFactory;
	class CAgentManager;

	/**
	 Holds information about a Content Access Agent implementation. 
	
	The metadata is supplied in the agents resource file so the information can be 
	obtained without the need to construct the agent.
	
	The AgentFactoryL() function is used by other CAF classes to access the 
	agent's CAgentFactory implementation.
	
	@internalComponent
	@released
	*/
	class CAgentInfo : public CBase
		{
	public:
		/** 
		Constructs a new CAgentInfo object from an ECOM CImplementationInfo.
		
		@param aImplInfo	An ECOM Implementation from REComSession::ListImplementationsL().
		@return				A new instance of a CAgentInfo object.
		*/
		static CAgentInfo* NewLC(const CImplementationInformation& aImplInfo);

		virtual ~CAgentInfo();

		/** 
		Determines whether this agent recognizes the supplier MIME type.
		
		@param aSupplierMime	The supplier MIME type to check.
		@return					ETrue, if the MIME type is a supported supplier MIME type.
		*/
		TBool IsSupportedSupplier(const TDesC8& aSupplierMime) const;

		/** 
		Determines whether this agent recognizes the consumer MIME type.
		
		@note Scheduled to be removed from the CAF API
		recognizer uses MIME types based upon the content type, not the mime type
		of the file itself. There will be no Consumer MIME types, only modified 
		content types.
		 
		@param aConsumerMime	The consumer mime type to check.
		@return					ETrue, if the MIME type is a supported consumer MIME type.
		*/
		TBool IsSupportedConsumer(const TDesC8& aConsumerMime) const;

		/** 
		* Exposes the array of supplier MIME types supported by this agent.
		*
		* @return	The array of supplier MIME types.
		*/
		const RPointerArray<HBufC8>& SupplierMimeTypes() const;

		/** 
		Exposes the array of consumer MIME types supported by this agent.
		
		@note Scheduled to be removed from the CAF API
		recognizer uses MIME types based upon the content type, not the 
		mime type of the file itself. There will be no Consumer mime types, 
		only modified content types.
		
		@return 	A reference to the array of consumer mime types.
		*/
		const RPointerArray<HBufC8>& ConsumerMimeTypes() const;
		
		/** 
		* Gets the preferred buffer size for this agent to recognize 
		* DRM files with the ContentAccess::CAgentResolver::DoRecognize() function.
		*
		* @return	The preferred buffer size.
		*/
		TInt PreferredBufferSize() const;

		/** The private server directory managed by this agent 

		This function returns the directory under C:\\private\\ used by this 
		agent. This is specified in the agents resource file in the opaque_data
		field. 
		
		If the agent does not use a private directory the return value will be
		KNullDesC()

		@return The directory name
		*/
		const TDesC& PrivateDirectoryName() const;
		
		/** The agent 
		
		@return A reference to this agent
		*/
		TAgent& Agent(); 

		/**
		Accessor for agent factory.
		
		Allows the factory to be constructed once and used many times. 
		DOES NOT transfer ownership of the CAgentFactory to the caller. 

		@return The CAgentFactory for this particular agent
		*/
		CAgentFactory& AgentFactoryL();

		/**
		Accessor to the agent manager. 
		
		The agent manager is stateless and this method allows it to be 
		constructed once and used several times. DOES NOT transfer ownership 
		of the CAgentManager to the caller. 

		@return The CAgentManager for this particular agent
		*/
		CAgentManager& AgentManagerL();


	private:
		CAgentInfo();
		void ConstructL(const CImplementationInformation& aImplInfo);

		// Helper function to parse MIME types of the form:
		// <mime1>,<mime2>,<mime3>
		void ParseMimeTypesL(const TDesC8& aBuf, RPointerArray<HBufC8>& aMimeTypes);

		// Helper to add descriptor to RPointerArray
		void AddToArrayL(const TDesC8& aElement, RPointerArray<HBufC8>& aArray);

	private:
		TInt iPreferredBufferSize;
		RPointerArray<HBufC8> iSupplierMimeTypes;
		RPointerArray<HBufC8> iConsumerMimeTypes;

		CAgentFactory* iAgentFactory;
		CAgentManager* iAgentManager;

		// Holds the agent name and Implementation UID
		TAgent iAgent;
		
		// Agent private directory is a SID
		TBuf <KMaxSIDLength> iPrivateDirectoryName;
		};
	}

#endif
