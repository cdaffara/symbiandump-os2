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

#ifndef __RTACONTENT_H__
#define __RTACONTENT_H__

#include <e32std.h>
#include <caf/caf.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <caf/cafplatform.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#include "rtaclient.h"

namespace ReferenceTestAgent
	{
	class RRtaContent : public RRtaClient
		{
	public:
		IMPORT_C RRtaContent();
		IMPORT_C TInt Open(const TDesC& aFileName, TUint aMode);
		IMPORT_C TInt Open(RFile& aFile);
		IMPORT_C TInt OpenContainer(const TDesC& aUniqueId) const;
		IMPORT_C TInt CloseContainer() const;
		IMPORT_C void GetEmbeddedObjectsL(ContentAccess::RStreamablePtrArray< ContentAccess::CEmbeddedObject > &aArray) const;
		IMPORT_C void GetEmbeddedObjectsL (ContentAccess::RStreamablePtrArray< ContentAccess::CEmbeddedObject > &aArray, ContentAccess::TEmbeddedType aType) const; 
		IMPORT_C TInt Search (ContentAccess::RStreamablePtrArray< ContentAccess::CEmbeddedObject > &aArray, const TDesC8 &aMimeType, TBool aRecursive) const;
		IMPORT_C TInt GetAttribute (const TDesC& aUniqueId, TInt aAttribute, TInt &aValue) const;
		IMPORT_C TInt GetAttributeSet (const TDesC& aUniqueId, ContentAccess::RAttributeSet &aAttributeSet) const; 
		IMPORT_C TInt GetStringAttribute (const TDesC& aUniqueId, TInt aAttribute, TDes &aValue) const; 
		IMPORT_C TInt GetStringAttributeSet (const TDesC& aUniqueId, ContentAccess::RStringAttributeSet &aStringAttributeSet) const; 
		
		IMPORT_C TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer) const;		
		IMPORT_C void NotifyStatusChangeL(ContentAccess::TEventMask aMask, const TDesC& aUniqueId) const;
		IMPORT_C TInt CancelNotifyStatusChange(const TDesC& aUniqueId) const;
		IMPORT_C void RequestRightsL(const TDesC& aUniqueId) const;
		IMPORT_C TInt CancelRequestRights(const TDesC& aUniqueId) const;
		IMPORT_C void DisplayInfoL(ContentAccess::TDisplayInfo aInfo, const TDesC& aUniqueId) const;
		IMPORT_C TInt SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue) const;
		
		#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        IMPORT_C TInt Open(const TDesC8& aHeaderData);
        IMPORT_C TInt GetWmdrmAttribute(const TDesC& aUniqueId, TInt aAttribute, TInt &aValue) const;
        IMPORT_C TInt GetWmdrmAttributeSet(const TDesC& aUniqueId, ContentAccess::RAttributeSet &aAttributeSet) const; 
        IMPORT_C TInt GetWmdrmStringAttribute(const TDesC& aUniqueId, TInt aAttribute, TDes &aValue) const; 
        IMPORT_C TInt GetWmdrmStringAttributeSet(const TDesC& aUniqueId, ContentAccess::RStringAttributeSet &aStringAttributeSet) const;
		#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

	private:
		void SearchL (ContentAccess::RStreamablePtrArray< ContentAccess::CEmbeddedObject > &aArray, const TDesC8 &aMimeType, TBool aRecursive) const;
		
		#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
            void GetAttributeSetL(const TDesC& aUniqueId, ContentAccess::RAttributeSet &aAttributeSet, TBool aWmdrmFlag = EFalse) const;
            void GetStringAttributeSetL(const TDesC& aUniqueId, ContentAccess::RStringAttributeSet &aStringAttributeSet, TBool aWmdrmFlag = EFalse) const;
		#else
			void GetAttributeSetL (const TDesC& aUniqueId, ContentAccess::RAttributeSet &aAttributeSet) const;
			void GetStringAttributeSetL (const TDesC& aUniqueId, ContentAccess::RStringAttributeSet &aStringAttributeSet) const;
		#endif
		};
	} // namespace
#endif
