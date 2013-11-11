// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "UT_STD.H"

// Class CDbRestrictStage
CDbRestrictStage::CDbRestrictStage(TDbTextComparison aComparison)
	: iTextOp(TTextOps::Ops(aComparison))
	{}

CDbRestrictStage::~CDbRestrictStage()
	{
	delete iRestriction;
	}

TInt CDbRestrictStage::CountL()
	{
	return KDbUndefinedCount;
	}

TBool CDbRestrictStage::FilterRowL(TDbRecordId aRecordId)
	{
	CDbDataStage::ReadRowL(aRecordId);
	return Restriction().EvaluateL(iTextOp);
	}

CDbRestrictStage::TGoto CDbRestrictStage::GotoL(TInt& aWork,TDbPosition aPosition,TDbRecordId& aRecordId)
	{
	TGoto go=CDbDataStage::GotoL(aWork,aPosition,aRecordId);
	if (go==ESuccess)
		{
		switch (aPosition)
			{
		case EDbFirst:
			aPosition=EDbNext;
			break;
		case EDbLast:
			aPosition=EDbPrevious;
			break;
		default:
			break;
			}
		do
			{
			TBool inSet=FilterRowL(aRecordId);
			aWork-=EWorkToRead;
			if (inSet)
				break;
			} while ((go=CDbDataStage::GotoL(aWork,aPosition,aRecordId))==ESuccess);
		}
	return go;
	}

TBool CDbRestrictStage::GotoL(TDbRecordId aRecordId)
//
// If the record exists, check the restriction
//
	{
	return CDbDataStage::GotoL(aRecordId) ? FilterRowL(aRecordId) : EFalse;
	}

CDbRestrictStage::TDelete CDbRestrictStage::DeleteRowL(TDbRecordId& aRecordId,TSynch aSynch)
//
// If trying to synchronise, and we find EDeletedAtNext, then we must check it is really in the rowset
//
	{
	TDelete r=CDbDataStage::DeleteRowL(aRecordId,aSynch);
	if (aSynch==ESynch && r==EDeletedAtNext && !FilterRowL(aRecordId))
		r=EDeletedInLimbo;		// not a member, this cursor is now "in limbo"
	return r;
	}
