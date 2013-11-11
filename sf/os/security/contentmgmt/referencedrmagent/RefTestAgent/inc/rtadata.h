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
 @internalAll
*/

#ifndef __RTADATA_H__
#define __RTADATA_H__

#include <e32std.h>
#include <caf/caf.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <caf/cafplatform.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#include "rtaclient.h"

namespace ReferenceTestAgent
	{
	class RRtaData : public RRtaClient
		{
	public:
		IMPORT_C RRtaData();
		IMPORT_C TInt Open(const TDesC& aFileName, const TDesC& aUniqueId, TUint aMode);
		IMPORT_C TInt Open(RFile& aFile, const TDesC& aUniqueId);
		IMPORT_C TInt Read(TDes8& aBuffer, TInt aLength);
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		IMPORT_C TInt Read(TInt64 aPos, TDes8& aBuffer, TInt aLength);
		IMPORT_C TInt DataSize(TInt64 &aSize); 
		IMPORT_C TInt Seek (TSeek aMode, TInt64 &aPos) const; 
#else
		IMPORT_C TInt Read(TInt aPos, TDes8& aBuffer, TInt aLength);
		IMPORT_C TInt DataSize(TInt &aSize); 
		IMPORT_C TInt Seek (TSeek aMode, TInt &aPos) const; 
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		IMPORT_C void EvaluateIntentL(ContentAccess::TIntent aIntent);
		IMPORT_C void ExecuteIntentL(ContentAccess::TIntent aIntent);
		IMPORT_C TInt GetAttribute (TInt aAttribute, TInt &aValue) const;
		IMPORT_C TInt GetAttributeSet (ContentAccess::RAttributeSet &aAttributeSet) const; 
		IMPORT_C TInt GetStringAttribute (TInt aAttribute, TDes &aValue) const; 
		IMPORT_C TInt GetStringAttributeSet (ContentAccess::RStringAttributeSet &aStringAttributeSet) const; 
		IMPORT_C TInt SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue) const;
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        IMPORT_C TInt Open(const TDesC8& aHeaderData);
        IMPORT_C TInt Read(const TDesC8& aEncryptedInputPacket, TDes8& aDecryptedOutputPacket);
        IMPORT_C TInt GetWmdrmAttribute(TInt aAttribute, TInt &aValue) const;
        IMPORT_C TInt GetWmdrmAttributeSet(ContentAccess::RAttributeSet &aAttributeSet) const;
        IMPORT_C TInt GetWmdrmStringAttribute(TInt aAttribute, TDes &aValue) const;
        IMPORT_C TInt GetWmdrmStringAttributeSet(ContentAccess::RStringAttributeSet &aStringAttributeSet) const;
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		
	private:
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        void GetAttributeSetL (ContentAccess::RAttributeSet &aAttributeSet, TBool aWmdrmFlag = EFalse) const;
        void GetStringAttributeSetL (ContentAccess::RStringAttributeSet &aStringAttributeSet, TBool aWmdrmFlag = EFalse) const;
#else
		void GetAttributeSetL (ContentAccess::RAttributeSet &aAttributeSet) const;
		void GetStringAttributeSetL (ContentAccess::RStringAttributeSet &aStringAttributeSet) const;
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		};
	} // namespace
#endif
