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

#include "UB_STD.H"

EXPORT_C TBtreeFixBase::TBtreeFixBase(TBtreeMode aMode,TInt anEntrySize,TInt aKeySize)
	: TBtree(aMode),iEntrySize(anEntrySize)
	{
	iLeafOrgFix.SetEntrySize(anEntrySize);
	iIndexOrgFix.SetEntrySize(aKeySize);
	}

EXPORT_C TBtreeFixBase::TBtreeFixBase(const TBtreeToken& aToken,TBtreeMode aMode,TInt anEntrySize,TInt aKeySize)
	: TBtree(aToken,aMode),iEntrySize(anEntrySize)
	{
	iLeafOrgFix.SetEntrySize(anEntrySize);
	iIndexOrgFix.SetEntrySize(aKeySize);
	}

EXPORT_C void TBtreeFixBase::Connect(MPagePool *aPool,const MBtreeKey *aKey)
/** Initalises the B-tree with a page pool and key handler.

@param aPool Page pool to provide storage for the B-tree
@param aKey Key handler for the B-tree */
	{
	TBtree::Connect(aPool,aKey,&iLeafOrgFix,&iIndexOrgFix);
	}

EXPORT_C TBool TBtreeFixBase::InsertL(TBtreePos& aPos,const TAny* anEntry,TAllowDuplicates aDup)
/** Inserts an entry into the tree.

@param aPos On return, the position of the entry inserted
@param anEntry Pointer to the entry to insert
@param aDup Flag to indicate whether duplicate entries are allowed in the tree
@return True if successful, false if the entry was a duplicate and aDup was 
set to ENoDuplicates */
	{
	return TBtree::InsertL(aPos,anEntry,iEntrySize,aDup);
	}

EXPORT_C void TBtreeFixBase::ExtractAtL(const TBtreePos& aPos,TAny* anEntry) const
/** Gets the entry at the specified position.

@param aPos Position of the entry to get
@param anEntry Buffer into which to copy the entry. */
	{
	TBtree::ExtractAtL(aPos,anEntry,iEntrySize);
	}

EXPORT_C void TBtreeFixBase::ExtractAtL(const TBtreeMark& aMark,TAny* anEntry) const
/** Gets the entry at the specified iterator position.

@param aMark Position of the entry to get
@param anEntry Buffer into which to copy the entry. */
	{
	TBtree::ExtractAtL(aMark,anEntry,iEntrySize);
	}

