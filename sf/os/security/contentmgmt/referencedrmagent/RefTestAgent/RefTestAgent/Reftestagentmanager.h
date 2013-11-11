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


#ifndef __REFTESTAGENTMANAGER_H__
#define __REFTESTAGENTMANAGER_H__

#include <caf/caf.h>
#include <caf/agentinterface.h>

#include "rtamanager.h"

class RHTTPHeaders;
class RStringPool;

class CRefTestAgentManager : public ContentAccess::CAgentManager
		{
	public:
		static CRefTestAgentManager* NewL();
		static CRefTestAgentManager* NewLC();

		~CRefTestAgentManager(void);

	public:
		// From CAgentManagerBase
		virtual TInt DeleteFile(const TDesC &aFileName);
		virtual TInt CopyFile(const TDesC& aSource, const TDesC& aDestination);
		virtual TInt CopyFile(RFile& aSource, const TDesC& aDestination);
		virtual TInt RenameFile(const TDesC& aSource, const TDesC& aDestination);
		virtual TInt MkDir(const TDesC& aPath);
		virtual TInt MkDirAll(const TDesC& aPath);
		virtual TInt RmDir(const TDesC& aPath);
		virtual TInt GetDir(const TDesC& aName,TUint aEntryAttMask,TUint aEntrySortKey, CDir*& aEntryList) const;
		virtual TInt GetDir(const TDesC& aName,TUint aEntryAttMask,TUint aEntrySortKey, CDir*& aEntryList,CDir*& aDirList) const;
		virtual TInt GetDir(const TDesC& aName,const TUidType& aEntryUid,TUint aEntrySortKey, CDir*& aFileList) const;
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId);
		virtual TInt GetAttributeSet(ContentAccess::RAttributeSet& aAttributeSet, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		virtual TInt GetAttributeSet(ContentAccess::RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId);	
		virtual TInt GetStringAttributeSet(ContentAccess::RStringAttributeSet& aAttributeSet, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		virtual TInt GetStringAttributeSet(ContentAccess::RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId);
		virtual void NotifyStatusChange(const TDesC& aURI, ContentAccess::TEventMask aMask, TRequestStatus& aStatus);
		virtual TInt CancelNotifyStatusChange(const TDesC& aURI, TRequestStatus& aStatus);
		virtual TInt SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue);
		virtual void DisplayInfoL(ContentAccess::TDisplayInfo aInfo, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		virtual void DisplayInfoL(ContentAccess::TDisplayInfo aInfo, RFile& aFile, const TDesC& aUniqueId);
		
		// From CAgentManager
		virtual TBool IsRecognizedL(const TDesC& aURI, ContentAccess::TContentShareMode aShareMode) const;
		virtual TBool IsRecognizedL(RFile& aFile) const;
		virtual TBool RecognizeFileL(const TDesC& aFileName, const TDesC8& aBuffer, TDes8& aFileMimeType, TDes8& aContentMimeType) const;
		
		virtual TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer);
		virtual void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus);
		virtual void DisplayManagementInfoL();
		virtual void PrepareHTTPRequestHeaders(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const;
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        virtual TBool IsRecognizedL(const TDesC8& aHeaderData) const;     
        virtual TBool RecognizeContentL(const TDesC8& aHeaderData, TDes8& aFileMimeType, TDes8& aContentMimeType) const;     
        virtual TInt GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue);     
        virtual TInt GetAttributeSet(const TDesC8& aHeaderData, ContentAccess::RAttributeSet& aAttributeSet);     
        virtual TInt GetStringAttributeSet(const TDesC8& aHeaderData, ContentAccess::RStringAttributeSet& aAttributeSet);     
        virtual TInt GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue);     
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

	private:
		CRefTestAgentManager();
		void ConstructL();
		void DeleteRightsByKeyL(const TDesC &aFileName);
		
	private:
		ReferenceTestAgent::RRtaManager iManagerSession;
		};

#endif // __REFTESTAGENTMANAGER_H__
