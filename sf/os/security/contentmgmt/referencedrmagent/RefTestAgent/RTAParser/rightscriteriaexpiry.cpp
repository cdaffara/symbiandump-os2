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
#include "rightscriteriaexpiry.h"


using namespace ReferenceTestAgent;


CRightsCriteriaExpiry* CRightsCriteriaExpiry::NewL(TInt aDay, TMonth aMonth, TInt aYear)
	{
	CRightsCriteriaExpiry* self = new (ELeave) CRightsCriteriaExpiry(aDay, aMonth, aYear);
	return self;
	}

CRightsCriteriaExpiry* CRightsCriteriaExpiry::NewL(RReadStream& aStream)
	{
	CRightsCriteriaExpiry* self = new (ELeave) CRightsCriteriaExpiry;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

CRightsCriteriaExpiry::CRightsCriteriaExpiry()
	{
	}

CRightsCriteriaExpiry::CRightsCriteriaExpiry(TInt aDay, TMonth aMonth, TInt aYear) :
iExpire(aYear, aMonth, aDay, 23, 59, 59, 999999)
	{
	}

CRightsCriteriaExpiry::~CRightsCriteriaExpiry()
	{
	}

TInt CRightsCriteriaExpiry::Evaluate()
	{
	TTime expiry(iExpire);
	TTime now;
	now.HomeTime();
	
	if(expiry < now)
		{
		return KErrCANoPermission;
		}
	else	
		{
		return KErrNone;	
		}
	}
	
void CRightsCriteriaExpiry::ExecuteL()
	{
	User::LeaveIfError(Evaluate());	
	}
	

TBool CRightsCriteriaExpiry::IsStateless() const
	{
	return ETrue;	
	}

ECriteriaType CRightsCriteriaExpiry::Type() const
	{
	return ECriteriaExpiry;
	}

MCriteria* CRightsCriteriaExpiry::CopyL() const
	{
	return CRightsCriteriaExpiry::NewL(iExpire.Day(), iExpire.Month(), iExpire.Year());
	}
		
TBool CRightsCriteriaExpiry::Merge(MCriteria& aCriteria)
	{
	CRightsCriteriaExpiry& expiryCriteria = reinterpret_cast<CRightsCriteriaExpiry&>(aCriteria);
	TTime currentExpiry(iExpire);
	TTime mergedExpiry(expiryCriteria.Expiry());
	if(mergedExpiry > currentExpiry)
		{
		iExpire = expiryCriteria.Expiry();
		}
	return ETrue;
	}

TDateTime& CRightsCriteriaExpiry::Expiry()
	{
	return iExpire;
	}

void CRightsCriteriaExpiry::InternalizeL(RReadStream& aStream)
	{
	TPckg <TDateTime> timePckg(iExpire);
	aStream.ReadL(timePckg);
	}
	
void CRightsCriteriaExpiry::ExternalizeL(RWriteStream& aStream)
	{
	TPckg <TDateTime> timePckg(iExpire);
	aStream.WriteL(timePckg);
	}

void CRightsCriteriaExpiry::WriteDescription(RFile& aFile)
	{
	TBuf <20> dateText;
	TBuf8 <20> dateText8;
	TTime expiry(iExpire);
	TRAPD(err, expiry.FormatL(dateText, _L("%1%/1%2%/2%3")));
	
	dateText8.Copy(dateText);
	if(err == KErrNone)
		{
		aFile.Write(_L8("Expiry "));
		aFile.Write(dateText8);
		}
	aFile.Write(_L8("\r\n"));
	}
