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


#ifndef __TESTAGENTMANAGER_H__
#define __TESTAGENTMANAGER_H__

#include <caf/caf.h>
#include "agentinterface.h"


class RHTTPHeaders;
class RStringPool;

/** A simple agent implementing a dummy DRM file format

Several of the functions such as Copy, Rename and GetDir are not supported
by this agent.

 */
class CTestAgentManager : public ContentAccess::CAgentManager
		{
	public:
		static CTestAgentManager* NewL();
		static CTestAgentManager* NewLC();

		~CTestAgentManager(void);

	public:
		// From CAgentManager
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
		virtual TInt GetAttributeSet(ContentAccess::RAttributeSet& aAttributeSet, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		virtual TInt GetStringAttributeSet(ContentAccess::RStringAttributeSet& aAttributeSet, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		virtual void NotifyStatusChange(const TDesC& aURI, ContentAccess::TEventMask aMask, TRequestStatus& aStatus);
		virtual TInt CancelNotifyStatusChange(const TDesC& aURI, TRequestStatus& aStatus);
		virtual TInt SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue);
		virtual void DisplayInfoL(ContentAccess::TDisplayInfo aInfo, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		virtual TBool IsRecognizedL(const TDesC& aURI, ContentAccess::TContentShareMode aShareMode) const;
		virtual TBool IsRecognizedL(RFile& aFile) const;
		virtual TBool RecognizeFileL(const TDesC& aFileName, const TDesC8& aBuffer, TDes8& aFileMimeType, TDes8& aContentMimeType) const;
		virtual TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer);
		virtual void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus);
		virtual void DisplayManagementInfoL();
		virtual void PrepareHTTPRequestHeaders(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const;
	private:
		CTestAgentManager();
		void ConstructL();
		void GetAttributeL(TInt aAttribute, TInt& aValue, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		void GetAttributeSetL(ContentAccess::RAttributeSet& aAttributeSet, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		void GetStringAttributeSetL(ContentAccess::RStringAttributeSet& aAttributeSet, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		void GetStringAttributeL(TInt aAttribute, TDes& aValue, const ContentAccess::TVirtualPathPtr& aVirtualPath);
		
	private:
		RFs iFs;
		CFileMan *iFileMan;
		};

#endif // __TESTAGENTMANAGER_H__
