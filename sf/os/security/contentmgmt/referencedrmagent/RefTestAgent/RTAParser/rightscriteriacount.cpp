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


#include <s32strm.h>
#include <caf/caf.h>
#include "rightscriteriacount.h"


using namespace ReferenceTestAgent;


CRightsCriteriaCount* CRightsCriteriaCount::NewL(TInt aCount)
	{
	CRightsCriteriaCount* self = new (ELeave) CRightsCriteriaCount(aCount);
	return self;
	}

CRightsCriteriaCount* CRightsCriteriaCount::NewL(RReadStream& aStream)
	{
	CRightsCriteriaCount* self = new (ELeave) CRightsCriteriaCount;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

CRightsCriteriaCount::CRightsCriteriaCount()
	{
	}

CRightsCriteriaCount::CRightsCriteriaCount(TInt aCount) : iCount(aCount)
	{
	}

CRightsCriteriaCount::~CRightsCriteriaCount()
	{
	}

TInt CRightsCriteriaCount::Evaluate()
	{
	TInt err = KErrNone;
	if(iCount == 0)
		{
		err = KErrCANoPermission;
		}
	return err;
	}
	
void CRightsCriteriaCount::ExecuteL()
	{
	if(iCount > 0)
		{
		iCount--;
		}
	else
		{
		User::Leave(KErrCANoPermission);	
		}
	}

TBool CRightsCriteriaCount::IsStateless() const
	{
	return EFalse;	
	}
	
ECriteriaType CRightsCriteriaCount::Type() const
	{
	return ECriteriaCount;
	}
	
TInt CRightsCriteriaCount::Count() const
	{
	return iCount;	
	}
	
MCriteria* CRightsCriteriaCount::CopyL() const
	{
	return CRightsCriteriaCount::NewL(iCount);		
	}

TBool CRightsCriteriaCount::Merge(MCriteria& aCriteria)
	{
	CRightsCriteriaCount& countCriteria = reinterpret_cast<CRightsCriteriaCount&>(aCriteria);
	iCount += countCriteria.Count();
	return ETrue;
	}
	
	
void CRightsCriteriaCount::InternalizeL(RReadStream& aStream)
	{
	iCount = aStream.ReadInt32L();
	}
	
void CRightsCriteriaCount::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteInt32L(iCount);
	}

void CRightsCriteriaCount::WriteDescription(RFile& aFile)
	{
	TBuf8 <10> count;
	count.Num(iCount);
	aFile.Write(_L8("Count = "));
	aFile.Write(count);
	aFile.Write(_L8("\r\n"));
	}
