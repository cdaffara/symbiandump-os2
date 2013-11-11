// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UC_STD.H"

EXPORT_C void TPagedSetToken::ExternalizeL(RWriteStream &aStream) const
	{
	__ASSERT_DEBUG(iCount>=0,User::Invariant());
	TBtreeToken::ExternalizeL(aStream);
	aStream.WriteInt32L(iCount);
	}

EXPORT_C void TPagedSetToken::InternalizeL(RReadStream &aStream)
	{
	TBtreeToken::InternalizeL(aStream);
	iCount=aStream.ReadInt32L();
	if (iCount<0)
		__LEAVE(KErrCorrupt);
	}

EXPORT_C void TPagedSetToken::Clear()
	{
	TBtreeToken::Clear();
	iCount=0;
	}

EXPORT_C TPagedSetBase::TPagedSetBase(TInt anEntrySize)
//
// Constructor creating a new set.
//
	: iTree(EBtreeSecure,anEntrySize,anEntrySize),iKey(anEntrySize),iCount(0)
	{}

EXPORT_C TPagedSetBase::TPagedSetBase(const TPagedSetToken &aToken,TInt anEntrySize)
//
// Constructor reinstating an existing set.
//
	: iTree(aToken,EBtreeSecure,anEntrySize,anEntrySize),iKey(anEntrySize),iCount(aToken.iCount)
	{}

EXPORT_C void TPagedSetBase::Connect(MPagePool *aPool)
//
// Hook the tree up to its bits.
//
	{
	iTree.Connect(aPool,&iKey);
	}

EXPORT_C void TPagedSetBase::Set(const TPagedSetToken &aToken)
//
// Set to the state described by aToken.
//
	{
	iTree.Set(aToken,EBtreeSecure);
	iCount=aToken.iCount;
	}

EXPORT_C TPagedSetToken TPagedSetBase::Token() const
//
// Build the streaming token for the set.
//
	{
	return TPagedSetToken(iTree.Token(),iCount);
	}

EXPORT_C TInt TPagedSetBase::RepairL()
//
// Repair a broken set.
//
	{
	TInt count=iTree.RepairL();
	iCount=count;
	return count;
	}

EXPORT_C void TPagedSetBase::ClearL()
//
// Empty the set.
//
	{
	iTree.ClearL();
	iCount=0;
	}

EXPORT_C TBool TPagedSetBase::ContainsL(const TAny *aPtr) const
//
// Return whether the set contains an entry equal to the one pointed to by aPtr.
//
	{
	TBtreePos pos;
	return iTree.FindL(pos,aPtr);
	}

EXPORT_C void TPagedSetBase::InsertL(const TAny *aPtr)
//
// Insert an entry into the set.
//
	{
	TBtreePos pos;
	if (!iTree.InsertL(pos,aPtr))
		__LEAVE(KErrAlreadyExists); // a duplicate
//
	++iCount;
	}

EXPORT_C void TPagedSetBase::DeleteL(const TAny *aPtr)
//
// Delete an entry from the set.
//
	{
	if (!iTree.DeleteL(aPtr))
		__LEAVE(KErrNotFound);
//
	--iCount;
	}

EXPORT_C void TPagedSetBase::InsertAllowDuplicatesL(const TAny *aPtr)
//
// Insert an entry into the set, allow duplicates.
//
	{
	TBtreePos pos;
	__DEBUG(TBool success=)iTree.InsertL(pos,aPtr,EAllowDuplicates);
	__ASSERT_DEBUG(success,User::Invariant());
	++iCount;
	}

EXPORT_C TBool TPagedSetIterBase::ResetL()
	{
	return iTree->ResetL(iMark);
	}

EXPORT_C TBool TPagedSetIterBase::NextL()
	{
	return iTree->NextL(iMark);
	}

EXPORT_C void TPagedSetIterBase::ExtractAtL(TAny* aPtr) const
	{
	iTree->ExtractAtL(iMark,aPtr);
	}

EXPORT_C TBool TPagedSetBiIterBase::FirstL()
	{
	return iTree->FirstL(iPos);
	}

EXPORT_C TBool TPagedSetBiIterBase::LastL()
	{
	return iTree->LastL(iPos);
	}

EXPORT_C TBool TPagedSetBiIterBase::NextL()
	{
	return iTree->NextL(iPos);
	}

EXPORT_C TBool TPagedSetBiIterBase::PreviousL()
	{
	return iTree->PreviousL(iPos);
	}

EXPORT_C void TPagedSetBiIterBase::ExtractAtL(TAny* aPtr) const
	{
	iTree->ExtractAtL(iPos,aPtr);
	}

