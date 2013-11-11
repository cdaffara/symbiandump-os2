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

#ifndef __RIGHTSOBJECT_H__
#define __RIGHTSOBJECT_H__

#include <f32file.h>
#include <caf/caf.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <caf/cafplatform.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

class RReadStream;
class RWriteStream;

namespace ReferenceTestAgent
	{
	class CIntent;
	
	class CRightsObject : public CBase
		{
	public:
		static CRightsObject* NewL(const TDesC8& aCid, TBool aForwardLock, TBool aPending, TInt aPendingDelay=0);
		IMPORT_C static CRightsObject* NewL(RReadStream& aStream, TBool aHaveDeleteKey);
		virtual ~CRightsObject();

		IMPORT_C const TDesC8& Cid();
		IMPORT_C TInt EvaluateIntent(ContentAccess::TIntent aIntent);
		IMPORT_C void ExecuteIntentL(ContentAccess::TIntent aIntent);
		IMPORT_C void MergeL(CRightsObject& aRightsObject);
		IMPORT_C void WriteDescription(RFile& aFile);
		IMPORT_C void ExternalizeL(RWriteStream& aStream);
		IMPORT_C void SetDeleteKeyL(const TDesC8& aDeleteKey);
		IMPORT_C const TDesC8& DeleteKey();

		void AddIntentL(CIntent* aIntent);
		
	private:
		CRightsObject();
		CRightsObject(TBool aForwardLock, TBool aPending, TInt aPendingDelay);
		void ConstructL(const TDesC8& aCid);
		void InternalizeL(RReadStream& aStream, TBool aHaveDeleteKey);

		TBool Pending();
		TBool ForwardLock();
		TInt PendingDelay();
		const ContentAccess::RStreamablePtrArray <CIntent>& IntentArray() const;
		
	private:
		ContentAccess::RStreamablePtrArray <CIntent> iIntents;
		HBufC8* iCid;
		
		TBool iForwardLock;
		TBool iPending;
		TInt iPendingDelay;
		HBufC8* iDeleteKey;
		};
	}

#endif

