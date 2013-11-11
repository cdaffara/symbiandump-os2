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

// Class TDbRecordId
inline TDbRecordId::TDbRecordId(TUint32 aValue)
	: iValue(aValue)
	{}
inline TDbRecordId& TDbRecordId::operator=(TUint32 aValue)
	{iValue=aValue;return *this;}
inline TBool TDbRecordId::operator==(TDbRecordId anId) const
	{return iValue==anId.iValue;}
inline TBool TDbRecordId::operator!=(TDbRecordId anId) const
	{return iValue!=anId.iValue;}
inline TUint32 TDbRecordId::Value() const
	{return iValue;}

// Class CDbRecordBase
inline TBool CDbRecordBase::OpenL()
	{return iFlags&(EOpen|EBroken) ? iFlags&EBroken : DoRestoreL();}
inline void CDbRecordBase::Abandon()
	{iFlags=0;}
inline TBool CDbRecordBase::IsBroken() const
	{return iFlags&EBroken;}
inline void CDbRecordBase::MarkIntact()
	{iFlags=(iFlags&~EBroken)|EOpen;}

// Class CDbRecordIter
inline CDbRecordBase& CDbRecordIter::Host() const
	{return iHost;}
inline CDbRecordIter::TDeleted CDbRecordIter::DeletedL(TDbPosition aPosition,TDbRecordId aRecordId)
	{return DoDeletedL(aPosition,aRecordId,0);}
inline CDbRecordIter::TDeleted CDbRecordIter::DeletedL(TDbPosition aPosition,TDbRecordId aRecordId,const RDbTableRow& aRow)
	{return DoDeletedL(aPosition,aRecordId,&aRow);}

// Class CDbBlobSpace
inline TInt CDbBlobSpace::InlineLimit() const
	{return iInlineLimit;}
inline void CDbBlobSpace::SetInlineLimit(TInt anInlineLimit)
	{iInlineLimit=anInlineLimit;}
