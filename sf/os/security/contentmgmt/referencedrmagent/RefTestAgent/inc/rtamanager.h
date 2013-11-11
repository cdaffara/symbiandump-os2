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
* RTA - client Reference Test Server interface
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#ifndef __RTAMANAGER_H__
#define __RTAMANAGER_H__

#include <e32std.h>
#include "rtaclient.h"

class CDir;
namespace ContentAccess
	{
	class TVirtualPathPtr;
	class RAttributeSet;
	class RStringAttributeSet;
	}

namespace ReferenceTestAgent
	{
	class CRightsObject;
	class RRtaManager : public RRtaClient
		{
	public:
		// protected constructor so only derived classes can access it
		IMPORT_C RRtaManager();
		IMPORT_C TInt Connect();
		IMPORT_C TInt DeleteFile(const TDesC& aFileName) const;
		IMPORT_C TInt CopyFile(const TDesC& aSource, const TDesC& aDestination) const;
		IMPORT_C TInt CopyFile(RFile& aSource, const TDesC& aDestination) const;
		IMPORT_C TInt RenameFile(const TDesC& aOldFileName, const TDesC& aNewFileName) const;
		IMPORT_C TInt MkDir(const TDesC& aPath) const;
		IMPORT_C TInt MkDirAll(const TDesC& aPath) const;
		IMPORT_C TInt RenameDir(const TDesC& aOldPath, const TDesC& aNewPath) const;		
		IMPORT_C TInt RmDir(const TDesC& aPath) const;
		IMPORT_C TInt GetDir(const TDesC& aPath, TUint aEntryMask, TUint aEntrySortKey, CDir*& aEntryList) const;
		IMPORT_C TInt GetDir(const TDesC& aPath, TUint aEntryMask, TUint aEntrySortKey, CDir*& aEntryList, CDir*& aFileList) const;
		IMPORT_C TInt GetDir(const TDesC& aPath, TUidType aEntryUid, TUint aEntrySortKey, CDir*& aFileList) const;
		IMPORT_C TInt GetAttribute (const ContentAccess::TVirtualPathPtr& aVirtualPath, TInt aAttribute, TInt &aValue) const;
		IMPORT_C TInt GetAttributeSet (const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::RAttributeSet &aAttributeSet) const;
		IMPORT_C TInt GetAttribute (TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId) const;
		IMPORT_C TInt GetAttributeSet (ContentAccess::RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId) const;	
		IMPORT_C TInt GetStringAttribute (const ContentAccess::TVirtualPathPtr& aVirtualPath, TInt aAttribute, TDes &aValue) const;
		IMPORT_C TInt GetStringAttributeSet (const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::RStringAttributeSet &aStringAttributeSet) const;
		IMPORT_C TInt GetStringAttribute(TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId) const;
		IMPORT_C TInt GetStringAttributeSet(ContentAccess::RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId) const;
		
		IMPORT_C void NotifyStatusChangeL(const TDesC& aURI, ContentAccess::TEventMask aMask) const;
		IMPORT_C TInt CancelNotifyStatusChange(const TDesC& aURI) const;
		IMPORT_C TInt SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue) const;
		IMPORT_C void DisplayInfoL(ContentAccess::TDisplayInfo aInfo, const ContentAccess::TVirtualPathPtr& aVirtualPath) const;
		IMPORT_C void DisplayInfoL(ContentAccess::TDisplayInfo aInfo, RFile& aFile, const TDesC& aUniqueId) const;
		IMPORT_C TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer) const;		 
		IMPORT_C void DisplayManagementInfoL() const;
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        IMPORT_C TInt GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue) const;
        IMPORT_C TInt GetAttributeSet(const TDesC8& aHeaderData, ContentAccess::RAttributeSet& aAttributeSet) const;
        IMPORT_C TInt GetStringAttributeSet(const TDesC8& aHeaderData, ContentAccess::RStringAttributeSet& aAttributeSet) const;
        IMPORT_C TInt GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue) const;
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

	private:
		enum TRetrievalType
			{
			EPathBasedRetrieval,
			EFileHandleBasedRetrieval
			};		
	private:
		void GetDirL(const TDesC& aPath, TUint aEntryMask, TUint aEntrySortKey, CDir*& aEntryList) const;
		void GetDirL(const TDesC& aPath, TUint aEntryMask, TUint aEntrySortKey, CDir*& aEntryList, CDir*& aFileList) const;
		void GetDirL(const TDesC& aPath, TUidType aEntryUid, TUint aEntrySortKey, CDir*& aFileList) const;
		void GetAttributeSetL (const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::RAttributeSet &aAttributeSet) const;
		void GetStringAttributeSetL (const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::RStringAttributeSet &aStringAttributeSet) const;
		void GetAttributeSetL(ContentAccess::RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId) const;
		void GetStringAttributeSetL(ContentAccess::RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId) const;
		void AttributeSetL(const TRetrievalType& aRetrievalType,const ContentAccess::TVirtualPathPtr* aVirtualPath, ...) const;
		void StringAttributeSetL(const TRetrievalType& aRetrievalType,const ContentAccess::TVirtualPathPtr* aVirtualPath, ...) const;
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        void GetAttributeSetL(const TDesC8& aHeaderData, ContentAccess::RAttributeSet& aAttributeSet) const;
        void GetStringAttributeSetL(const TDesC8& aHeaderData, ContentAccess::RStringAttributeSet& aAttributeSet) const;
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		};
	} // namespace
#endif
