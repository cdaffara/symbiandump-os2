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
* sisregistry - server classes
*
*/


/**
 @file 
 @test
 @internalComponent
*/
#ifndef __RTAMANAGERHANDLER_H__
#define __RTAMANAGERHANDLER_H__

#include <e32base.h>
#include "rtamessagehandler.h"
#include "clientserver.h"
#include "RTAserver.h"

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
#include "wmdrmcontentparser.h"     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

namespace ReferenceTestAgent
	{
	class CRefTestAgentArchive;
	class CDrmFileContent;
	
	class CRtaManagerHandler: public CRtaMessageHandler
		{
	public:
		static CRtaManagerHandler* NewL(const CSession2& aSession);
		
		virtual void ServiceL(const RMessage2& aMessage);
		virtual ~CRtaManagerHandler();
	private:
		CRtaManagerHandler(const CSession2& aSession);
		
		void DeleteFileL(const RMessage2& aMessage);
		void CopyFileL(const RMessage2& aMessage);
		void CopyFileHandleL(const RMessage2& aMessage);
		void RenameFileL(const RMessage2& aMessage);
		void MkDirL(const RMessage2& aMessage);
		void MkDirAllL(const RMessage2& aMessage);
		void RenameDirL(const RMessage2& aMessage);
		void RmDirL(const RMessage2& aMessage);
		void GetDir1L(const RMessage2& aMessage);
		void GetDir2L(const RMessage2& aMessage);
		void GetDir3L(const RMessage2& aMessage);
		void GetAttributeL(const RMessage2& aMessage);
		void GetAttributeSetL(const RMessage2& aMessage);
		void GetStringAttributeL(const RMessage2& aMessage);
		void GetStringAttributeSetL(const RMessage2& aMessage);
		void NotifyStatusChangeL(const RMessage2& aMessage); 
		void CancelNotifyStatusChangeL(const RMessage2& aMessage); 	

		void SetPropertyL(const RMessage2& aMessage);
		void DisplayInfoL(const RMessage2& aMessage);
		void DisplayInfo1L(const RMessage2& aMessage);
		void AgentSpecificCommandL(const RMessage2& aMessage);
		void AgentSpecificCommandAsyncL(const RMessage2& aMessage); 
		void DisplayManagementInfoL(const RMessage2& aMessage);
		
		/** Open a content object from within a file using either file path or file handle and the associated unique id. 
		 *	
		 * aUriOrFileSession and aUniqueIdOrFileHandle is the position of the file session and the file 
		 * object in the message passed using IPC when file handle is adopted from the client. 
		 * Otherwise these will represent file path and unique id of the content object respectively.
		 */
		void OpenContentObjectL(const RMessage2& aMessage, TInt aUriOrFileSession, TInt aUniqueIdOrFileHandle);
		void WriteCDirToMessageL(const RMessage2& aMessage, TInt aParam, CDir& aDir);
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
        void GetWMDRMAttributeL(const RMessage2& aMessage);     
        void GetWMDRMStringAttributeL(const RMessage2& aMessage);     
        void GetWMDRMAttributeSetL(const RMessage2& aMessage);     
        void GetWMDRMStringAttributeSetL(const RMessage2& aMessage);     
             
        void OpenWmdrmContentObjectL(const RMessage2& aMessage, TInt aParameter);     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

	private:
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		RFile64 iFile;
#else
		RFile iFile;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

		CRefTestAgentArchive* iArchive;
		
		// this pointer should not be deleted since it points to something owned by iArchive
		CDrmFileContent* iContentObject;
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
        CWmdrmContentParser* iWmdrmContentObject;     
#endif  //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
		};
	
	} //namespace

	
#endif 

