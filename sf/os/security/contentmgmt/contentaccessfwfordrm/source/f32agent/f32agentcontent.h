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


/**
 @file
 @internalComponent
 @released
*/

#ifndef __F32AGENTCONTENT_H__
#define __F32AGENTCONTENT_H__

#include <f32file.h>
#include <caf/agentinterface.h>

namespace ContentAccess
	{
	class TVirtualPathPtr;
	class CF32AgentUi;

	/**
	 F32 agent implementation of the CAgentContent class used to browse
	 plaintext files. The browsing will not return any embedded objects since
	 the F32 agent only enables access to the entire file
	 
	 @internalComponent
	 @released
	 */
	class CF32AgentContent : public CAgentContent
		{
	public:
		// Two phase constructor used when the file is opened 
		// using a file name
		static CF32AgentContent* NewL(const TDesC& aURI, TContentShareMode aShareMode);
		
		// Two phase constructor used when the file is opened with 
		// a file handle
		static CF32AgentContent* NewL(RFile& aFile);
		
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		static CF32AgentContent* NewL(const TDesC8& aHeaderData);
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		virtual ~CF32AgentContent();

	public: 
		// From CAgentContent
		virtual TInt OpenContainer(const TDesC& aUniqueId);
		virtual TInt CloseContainer();
		virtual void GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray);
		virtual void GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType);
		virtual TInt Search(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive);
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId);
		virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId);
		virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId);
		virtual TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer);
		virtual void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus);
		virtual void NotifyStatusChange(TEventMask aMask, TRequestStatus& aStatus, const TDesC& aUniqueId);
		virtual TInt CancelNotifyStatusChange(TRequestStatus& aStatus, const TDesC& aUniqueId);
		virtual void RequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId);
		virtual TInt CancelRequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId);
		virtual void DisplayInfoL(TDisplayInfo aInfo, const TDesC& aUniqueId);
		virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue);

	private:
		CF32AgentContent();
		void ConstructL(const TDesC& aURI, TContentShareMode aShareMode);
		void ConstructL(RFile& aFile);
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		void ConstructL(const TDesC8& aHeaderData);
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		
		CF32AgentUi& AgentUiL();		
		void SearchL(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive);

	private:
		HBufC* iURI;
		TContentShareMode iShareMode;
		RFs iFs;
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		RFile64 iFile;
#else
		RFile iFile;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		CF32AgentUi* iAgentUi;
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		HBufC8* iHeaderData;
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		};
} // namespace ContentAccess
#endif // __F32AgentContent_H__
