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

#ifndef __INTENT_H__
#define __INTENT_H__

#include <f32file.h>
#include <caf/caf.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <caf/cafplatform.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

class RReadStream;
class RWriteStream;

namespace ReferenceTestAgent
	{
	enum ECriteriaType
			{
			ECriteriaCount,
			ECriteriaExpiry
			};

	/** Interface used to define any implementation of a criteria
	related to the use of content
	*/
	class MCriteria
		{
	public:
		virtual ~MCriteria();
			
		virtual TInt Evaluate() = 0;
		virtual void ExecuteL() = 0;
		virtual TBool IsStateless() const = 0;
		virtual ECriteriaType Type() const = 0;
		virtual MCriteria* CopyL() const = 0;
		virtual TBool Merge(MCriteria& aCriteria) = 0;
		virtual void WriteDescription(RFile& aFile) = 0;
		
		// Rights must be streamable over IPC
		virtual void ExternalizeL(RWriteStream& aStream) = 0;
		virtual void InternalizeL(RReadStream& aStream) = 0;
		};

	
	class CIntent : public CBase
		{
	public:
		static CIntent* NewL(ContentAccess::TIntent aIntent);
		static CIntent* NewL(RReadStream& aStream);
		virtual ~CIntent();

		TInt Evaluate();
		void ExecuteL();
		ContentAccess::TIntent Intent();
		
		void AddCriteriaL(MCriteria* aCriteria);
		void MergeL(CIntent* aIntent);
		CIntent* CopyL() const;
		void ExternalizeL(RWriteStream& aStream) const;

		void WriteDescription(RFile& aFile);
	private:
		CIntent();
		CIntent(ContentAccess::TIntent aIntent);
		void InternalizeL(RReadStream& aStream);
		const RPointerArray <MCriteria>& CriteriaArray() const;
	
	private:
		ContentAccess::TIntent iIntent;
		RPointerArray <MCriteria> iCriteria;
		};
	}

#endif

