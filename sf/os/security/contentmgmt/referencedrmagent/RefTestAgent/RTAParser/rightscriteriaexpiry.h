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

#ifndef __RIGHTSCRITERIAEXPIRY_H__
#define __RIGHTSCRITERIAEXPIRY_H__

#include <f32file.h>
#include <caf/caf.h>
#include "intent.h"

class RReadStream;
class RWriteStream;

namespace ReferenceTestAgent
	{
	/** Interface used to define any implementation of a restriction
	*/
	class CRightsCriteriaExpiry : public MCriteria
		{
	public:
		static CRightsCriteriaExpiry* NewL(TInt aDay, TMonth aMonth, TInt aYear);
		static CRightsCriteriaExpiry* NewL(RReadStream& aStream);
		
		virtual ~CRightsCriteriaExpiry();
		
		virtual TInt Evaluate();
		virtual void ExecuteL();
		virtual TBool IsStateless() const;
		ECriteriaType Type() const;
		virtual MCriteria* CopyL() const;
		virtual TBool Merge(MCriteria& aCriteria);
		virtual void WriteDescription(RFile& aFile);
		
		// Rights must be streamable over IPC
		virtual void ExternalizeL(RWriteStream& aStream);
		
	private:
		CRightsCriteriaExpiry();
		CRightsCriteriaExpiry(TInt aDay, TMonth aMonth, TInt aYear);
		TDateTime& Expiry();
		
		virtual void InternalizeL(RReadStream& aStream);	
		
	private:
		TDateTime iExpire;
		};
	}

#endif

