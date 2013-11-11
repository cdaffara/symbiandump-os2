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
// Inline entry node organisations
// 
//

#include "UB_STD.H"

EXPORT_C TBtreeInlineLeafOrg::TBtreeInlineLeafOrg()
	{}

EXPORT_C void TBtreeInlineLeafOrg::SetEntrySize(TInt aSize)
	{
	__ASSERT_DEBUG(aSize<=KMaxBtreeKeyLength,Panic(EBadEntrySize));
	iEntrySize=Align4(aSize);
	iMaxEntries=(KPoolPageSize-sizeof(SHeader))/iEntrySize;
	}

EXPORT_C TBool TBtreeInlineLeafOrg::Insert(TAny *aNode,TInt aPos,const TDesC8& anEntry) const
	{
	SNode* const pn=Node(aNode);
	__ASSERT_DEBUG(aPos<=pn->iHead.iCount,Panic(EBadEntryPos));
	if (pn->iHead.iCount==iMaxEntries)
		return EFalse;
	TUint8* pe=Entry(pn,aPos);
	Mem::Copy(pe+iEntrySize,pe,iEntrySize*(pn->iHead.iCount-aPos));
	TInt size=anEntry.Size();
	__ASSERT_ALWAYS(size<=iEntrySize,Panic(EBadEntrySize));
	Mem::Copy(pe,anEntry.Ptr(),size);
	++pn->iHead.iCount;
	return ETrue;
	}

TAny *TBtreeInlineLeafOrg::DoRedistribute(TAny *aLeftNode,TAny *aRightNode,TInt aInsertPos) const
//
// Even out the distibution of entries in LeftNode and RightNode
// if aInsertPos>=0 we want to insert at this cumulative position so take into account for redistribution
// If total entries <=iMaxEntries or >iMaxEntries*2 then return 0
// Otherwise return the node into which insertion should take place
//
	{
	SNode* const pl=Node(aLeftNode);
	SNode* const pr=Node(aRightNode);
	TAny *insertNode=aRightNode;
	TInt lCount=pl->iHead.iCount;
	TInt rCount=pr->iHead.iCount;
	TInt total=lCount+rCount;
	TInt left=(total+1)>>1;
	if (aInsertPos>=0)
		{	// call from Insertoverflow
		__ASSERT_DEBUG(aInsertPos<=total,Panic(EBadEntryPos));
		if (aInsertPos<left)
			{
			insertNode=aLeftNode;
			--left;
			}
		if (total>=iMaxEntries<<1)
			return 0;	// no space
		}
	else
		{ // from Redistribute
		if (total<=iMaxEntries)
			return 0;		// underflow state
		}
	pl->iHead.iCount=left;
	pr->iHead.iCount=total-left;
	if (lCount>left)
		{ // move right
		TInt move=lCount-left;
		Mem::Copy(Entry(pr,move),pr->iEntries,rCount*iEntrySize);
		Mem::Copy(pr->iEntries,Entry(pl,left),move*iEntrySize);
		}
	else if (lCount<left)
		{ // move left
		TInt move=left-lCount;
		Mem::Copy(Entry(pl,lCount),pr->iEntries,move*iEntrySize);
		Mem::Copy(pr->iEntries,Entry(pr,move),(rCount-move)*iEntrySize);
		}
	return insertNode;
	}

EXPORT_C TBool TBtreeInlineLeafOrg::InsertOverflow(TAny *aLeftNode,TAny *aRightNode,TInt aPos,TBool aInsertOnLeft,const TDesC8& anEntry) const
//
// Redistribute keys in aLeftNode and aRightNode to insert a new entry, return success or failure
// We know that the insertion node is full (see code for CBplusTree::InsertAtCurrentL)
// promotion done by manager
//
	{
	__ASSERT_DEBUG(Node(aInsertOnLeft?aLeftNode:aRightNode)->iHead.iCount==iMaxEntries,Panic(EIllegalOverflow));
	if (!aInsertOnLeft)
		aPos+=Node(aLeftNode)->iHead.iCount;
	TAny* iNode=DoRedistribute(aLeftNode,aRightNode,aPos);
	if (!iNode)
		return EFalse;
	if (iNode==aRightNode)
		aPos-=Node(aLeftNode)->iHead.iCount;
	return Insert(iNode,aPos,anEntry);
	}

EXPORT_C void TBtreeInlineLeafOrg::InsertSplit(TAny *aLeftNode,TAny *aRightNode,TInt aPos,const TDesC8& anEntry) const
//
// Split contents of left node into left and right and insert entry
// copy link node from left to right
//
	{
	__ASSERT_DEBUG(Node(aLeftNode)->iHead.iCount==iMaxEntries,Panic(EIllegalSplit));
	__ASSERT_DEBUG(Node(aRightNode)->iHead.iCount==0,Panic(EIllegalSplit));
	TAny* iNode=DoRedistribute(aLeftNode,aRightNode,aPos);
	if (iNode==aRightNode)
		aPos-=Node(aLeftNode)->iHead.iCount;
	Insert(iNode,aPos,anEntry);
	Node(aRightNode)->iHead.iLink=Node(aLeftNode)->iHead.iLink;
	}

EXPORT_C TBool TBtreeInlineLeafOrg::Delete(TAny *aNode,TInt aPos) const
//
// Delete the specified entry from the node
// Part of the contract is to garauntee that the entry is deleted, even if undeflowed
//
	{
	SNode* const pn=Node(aNode);
	__ASSERT_DEBUG(aPos<pn->iHead.iCount,Panic(EBadEntryPos));

	--pn->iHead.iCount;
	TUint8* pe=Entry(pn,aPos);
	Mem::Copy(pe,pe+iEntrySize,(pn->iHead.iCount-aPos)*iEntrySize);
	return pn->iHead.iCount>=iMaxEntries>>1;		// underflow criterion
	}

EXPORT_C TBool TBtreeInlineLeafOrg::Redistribute(TAny *aLeftNode,TAny *aRightNode) const
//
// Even out the distibution of entries in LeftNode and RightNode if enough keys
// leaf nodes do not set the pivot, done by manager
//
	{
	return DoRedistribute(aLeftNode,aRightNode)!=0;
	}

EXPORT_C void TBtreeInlineLeafOrg::Concatenate(TAny *aLeftNode,const TAny *aRightNode) const
//
// Join LeftNode and RightNode together in LeftNode, set link
// contract says that it will fit
//
	{
	SNode* const pl=Node(aLeftNode);
	const SNode* const pr=Node(aRightNode);
	TInt rCount=pr->iHead.iCount;
	__ASSERT_DEBUG(pl->iHead.iCount+rCount<=iMaxEntries,Panic(ECannotConcatenate));
	Mem::Copy(Entry(pl,pl->iHead.iCount),pr->iEntries,rCount*iEntrySize);
	pl->iHead.iCount+=rCount;
	pl->iHead.iLink=pr->iHead.iLink;
	}

EXPORT_C TInt TBtreeInlineLeafOrg::LastEntry(const TAny* aNode) const
	{
	return Node(aNode)->iHead.iCount;
	}

EXPORT_C const TAny *TBtreeInlineLeafOrg::EntryPtr(const TAny* aNode,TInt aPos) const
	{
	__ASSERT_DEBUG(aPos<Node(aNode)->iHead.iCount,Panic(EBadEntryPos));
	return Entry(Node(aNode),aPos);
	}

EXPORT_C TPtrC8 TBtreeInlineLeafOrg::Entry(const TAny* aNode,TInt aPos) const
	{
	return TPtrC8((const TUint8*)EntryPtr(aNode,aPos),iEntrySize);
	}

EXPORT_C TPageRef TBtreeInlineLeafOrg::LinkNode(const TAny* aNode) const
	{
	return Node(aNode)->iHead.iLink;
	}

EXPORT_C void TBtreeInlineLeafOrg::SetLinkNode(TAny* aNode,TPageRef aNextNode) const
	{
	Node(aNode)->iHead.iLink=aNextNode;
	}


// class TBtreeInlineIndexOrg

EXPORT_C TBtreeInlineIndexOrg::TBtreeInlineIndexOrg()
	{}

EXPORT_C void TBtreeInlineIndexOrg::SetEntrySize(TInt aSize)
	{
	__ASSERT_DEBUG(aSize<=KMaxBtreeKeyLength,Panic(EBadEntrySize));
	iEntrySize=_FOFF(SEntry,iKey[Align4(aSize)]);
	iMaxEntries=(KPoolPageSize-sizeof(SHeader)-sizeof(TPageRef))/iEntrySize;
	}

EXPORT_C TBool TBtreeInlineIndexOrg::Insert(TAny *aNode,TInt aPos,const TDesC8& anEntry,TPageRef aChild) const
	{
	SNode* const pn=Node(aNode);
	__ASSERT_DEBUG(aPos<=pn->iHead.iCount,Panic(EBadEntryPos));
	if (pn->iHead.iCount==iMaxEntries)
		return EFalse;
	TUint8* pe=Entry(pn,aPos)->iKey;
	Mem::Copy(pe+iEntrySize,pe,iEntrySize*(pn->iHead.iCount-aPos));
	TInt size=anEntry.Size();
	__ASSERT_ALWAYS(size<=KeySize(),Panic(EBadEntrySize));
	*(TPageRef*)Mem::Copy(pe,anEntry.Ptr(),size)=aChild;
	++pn->iHead.iCount;
	return ETrue;
	}

TBtreeInlineIndexOrg::SNode* TBtreeInlineIndexOrg::DoRedistribute(TAny *aLeftNode,TAny *aRightNode,const TDesC8& aPivot,TBtreePivot& aNewPivot,TInt aInsertPos) const
//
// Even out the distibution of entries in LeftNode and RightNode
// if aInsertPos>=0 we want to insert at this cumulative position so take into account for redistribution
// If total entries <=iMaxEntries or >iMaxEntries*2 then return 0
// Otherwise return the node into which insertion should take place
// If insertion should be promoted insert position is beyond end of left node, otherwise
// the new pivot is copied to aNewPivot
//
	{
	SNode* const pl=Node(aLeftNode);
	SNode* const pr=Node(aRightNode);
	SNode *insertNode=pr;
	TInt lCount=pl->iHead.iCount;
	TInt rCount=pr->iHead.iCount;
	TInt total=lCount+rCount+1;			// including pivot entry
	TInt left=total>>1;
	if (aInsertPos>=0)
		{	// call from InsertOverflow
		__ASSERT_DEBUG(aInsertPos<=total,Panic(EBadEntryPos));
		if (total>iMaxEntries<<1)
			return NULL;		// no space to insert
		if (aInsertPos<=left)
			{
			if (aInsertPos<left)
				--left;
			insertNode=pl;
			}
		}
	else
		{ // from Redistribute
		if (total<=iMaxEntries)
			return NULL;		// underflow state
		}
	pl->iHead.iCount=left;
	pr->iHead.iCount=total-left-1;		// pivot not included
	TInt pSize=aPivot.Size();
	__ASSERT_DEBUG(pSize<=KeySize(),Panic(EBadEntrySize));
	if (lCount>left)
		{ // move right
		TInt move=lCount-left;
		Mem::Copy(Entry(pr,move),pr->iEntries,rCount*iEntrySize+sizeof(TPageRef));
		TUint8 *pp=Mem::Copy(pr->iEntries,Entry(pl,left+1),(move-1)*iEntrySize+sizeof(TPageRef));
		Mem::Copy(pp,aPivot.Ptr(),pSize);
		aNewPivot.Copy(Entry(pl,left)->iKey,KeySize());		// new pivot
		}
	else if (lCount<left)
		{ // move left
		TInt move=left-lCount-1;
		TUint8 *pp=Mem::Copy(Entry(pl,lCount)->iKey,aPivot.Ptr(),pSize);
		Mem::Copy(pp,pr->iEntries,move*iEntrySize+sizeof(TPageRef));
		aNewPivot.Copy(Entry(pr,move)->iKey,KeySize());
		Mem::Copy(pr->iEntries,Entry(pr,move+1),(rCount-move-1)*iEntrySize+sizeof(TPageRef));
		}
	else
		{	// should we ever get here?	(lCount==left)
		aNewPivot=aPivot;
		}
	return insertNode;
	}

EXPORT_C TBool TBtreeInlineIndexOrg::InsertOverflow(TAny *aLeftNode,TAny *aRightNode,TInt aPos,TBool aInsertOnLeft,const TDesC8& anEntry,
													 TPageRef aChild,const TDesC8& aPivot,TBtreePivot& aNewPivot) const
//
// Redistribute keys in aNodePtr and aRight to insert a new entry, return success or failure
// We know that aNodePtr is full (see code for Insert)
//
	{
	SNode* const pl=Node(aLeftNode);
	if (!aInsertOnLeft)
		aPos+=pl->iHead.iCount+1;		// cumulative insertion point
	SNode* insert=DoRedistribute(aLeftNode,aRightNode,aPivot,aNewPivot,aPos);
	if (insert==NULL)							// no space
		return EFalse;
	TInt lCount=pl->iHead.iCount;
	if (insert==aRightNode)
		aPos-=lCount+1;					// insert position in right node
	else
		{
		// check for the special situation [aPos]:[aPos-1]
		// in which case re-arrange to promote the new entry
		SNode* pr=Node(aRightNode);
		if (aPos==lCount && pr->iHead.iCount<lCount)
			{
			Insert(pr,0,aNewPivot,Entry(pr,0)->iChild);
			Entry(pr,0)->iChild=aChild;
			aNewPivot=anEntry;
			return ETrue;
			}
		}
	__ASSERT_DEBUG(insert->iHead.iCount<iMaxEntries,User::Invariant());
	Insert(insert,aPos,anEntry,aChild);
	return ETrue;
	}

EXPORT_C void TBtreeInlineIndexOrg::InsertSplit(TAny *aLeftNode,TAny *aRightNode,TInt aPos,const TDesC8& anEntry,TPageRef aChild,TBtreePivot& aPromote) const
//
// part of the contract is not to use aPromote before anEntry
// We know that aNodePtr is full
// prepare right node and use insert-overflow
//
	{
	__ASSERT_DEBUG(Node(aLeftNode)->iHead.iCount==iMaxEntries,Panic(EIllegalSplit));
	__ASSERT_DEBUG(Node(aRightNode)->iHead.iCount==0,Panic(EIllegalSplit));
	SNode* const pl=Node(aLeftNode);
	SNode* const pr=Node(aRightNode);
	SEntry *pe=Entry(pl,pl->iHead.iCount);
	--pl->iHead.iCount;
	Entry(pr,0)->iChild=pe->iChild;
	TPtrC8 pivot((TUint8*)pe-KeySize(),KeySize());
	InsertOverflow(aLeftNode,aRightNode,aPos,ETrue,anEntry,aChild,pivot,aPromote);
	}

EXPORT_C TBool TBtreeInlineIndexOrg::Update(TAny *aNode,TInt aPos,const TDesC8& anEntry) const
	{
	__ASSERT_DEBUG(anEntry.Size()<=KeySize(),Panic(EBadEntrySize));
	__ASSERT_DEBUG(aPos<Node(aNode)->iHead.iCount,Panic(EBadEntryPos));
	Mem::Copy(Entry(Node(aNode),aPos)->iKey,anEntry.Ptr(),KeySize());
	return ETrue;
	}

EXPORT_C TBool TBtreeInlineIndexOrg::Delete(TAny *aNode,TInt aPos) const
//
// Delete the specified key and following child reference from the node
// Part of the contract is to garauntee that the entry is deleted, even if undeflowed
//
	{
	SNode* const pn=Node(aNode);
	__ASSERT_DEBUG(aPos<pn->iHead.iCount,Panic(EBadEntryPos));

	--pn->iHead.iCount;
	TUint8* pe=Entry(pn,aPos)->iKey;
	Mem::Copy(pe,pe+iEntrySize,(pn->iHead.iCount-aPos)*iEntrySize);
	return pn->iHead.iCount>=iMaxEntries>>1;		// underflow criterion
	}

EXPORT_C TBool TBtreeInlineIndexOrg::Redistribute(TAny *aLeftNode,TAny *aRightNode,const TDesC8& aPivot,TBtreePivot& aNewPivot) const
	{
	return DoRedistribute(aLeftNode,aRightNode,aPivot,aNewPivot)!=NULL;
	}

EXPORT_C void TBtreeInlineIndexOrg::Concatenate(TAny *aLeftNode,const TAny *aRightNode,const TDesC8& aPivot) const
//
// Join LeftNode and RightNode together in LeftNode
// contract says that it will fit
//
	{
	SNode* const pl=Node(aLeftNode);
	const SNode* const pr=Node(aRightNode);
	TInt rCount=pr->iHead.iCount;
	TInt lCount=pl->iHead.iCount;
	__ASSERT_DEBUG(lCount+rCount+1<=iMaxEntries,Panic(ECannotConcatenate));
	TInt pSize=aPivot.Size();
	__ASSERT_DEBUG(pSize<=KeySize(),Panic(EBadEntrySize));
	TUint8* pp=Mem::Copy(Entry(pl,lCount)->iKey,aPivot.Ptr(),pSize);
	Mem::Copy(pp,pr->iEntries,rCount*iEntrySize+sizeof(TPageRef));
	pl->iHead.iCount+=rCount+1;
	}

EXPORT_C void TBtreeInlineIndexOrg::MakeRoot(TAny* aNode,TPageRef aChild) const
	{
	__ASSERT_DEBUG(Node(aNode)->iHead.iCount==0,Panic(ECannotMakeRoot));
	Entry(Node(aNode),0)->iChild=aChild;
	}

EXPORT_C TInt TBtreeInlineIndexOrg::LastEntry(const TAny* aNode) const
	{
	return Node(aNode)->iHead.iCount;
	}

EXPORT_C const TAny *TBtreeInlineIndexOrg::EntryPtr(const TAny* aNode,TInt aPos) const
	{
	__ASSERT_DEBUG(aPos<Node(aNode)->iHead.iCount,Panic(EBadEntryPos));
	return Entry(Node(aNode),aPos)->iKey;
	}

EXPORT_C TPtrC8 TBtreeInlineIndexOrg::Entry(const TAny* aNode,TInt aPos) const
	{
	return TPtrC8((const TUint8*)EntryPtr(aNode,aPos),KeySize());
	}

EXPORT_C TPageRef TBtreeInlineIndexOrg::ChildNode(const TAny* aNode,TInt aPos) const
//
// Entries are always aligned
// 
	{
	__ASSERT_DEBUG(aPos<=Node(aNode)->iHead.iCount,Panic(EBadEntryPos));
	return Entry(Node(aNode),aPos)->iChild;
	}

