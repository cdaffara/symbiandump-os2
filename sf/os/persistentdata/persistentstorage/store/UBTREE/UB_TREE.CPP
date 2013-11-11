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

//#pragma message( __FILE__ " : 'TBtreePath' does not track insertion position" )
//#pragma message( __FILE__ " : 'TBtreePath' record of last entry unused" )
//#pragma message( __FILE__ " : 'TBtree' lots of opportunities for on-the-fly integrity checking" )

EXPORT_C void TBtreeToken::ExternalizeL(RWriteStream& aStream) const
/** Externalises a TBtreeToken object to a stream.

@param aStream Stream to which the object should be externalised. */
	{
	aStream<<iFirst;
	aStream<<iRoot;
	aStream.WriteInt8L(iHeight);
	}

EXPORT_C void TBtreeToken::InternalizeL(RReadStream& aStream)
/** Internalises a TBtreeToken object from a stream.

@param aStream Stream from which the object should be internalised. */
	{
	aStream>>iFirst;
	aStream>>iRoot;
	TInt height=aStream.ReadInt8L();
	if (height<0||height>KMaxBtreeHeight)
		__LEAVE(KErrCorrupt);
//
	iHeight=height;
	}

EXPORT_C void TBtreeToken::Clear()
	{
	iFirst=KNullPageRef;
	iRoot=KNullPageRef;
	iHeight=0;
	}

void TBtreePath::Push(TPageRef aRef,TInt aEntry)
	{
	__ASSERT_DEBUG(aEntry==TInt(TUint8(aEntry)),Panic(EBadEntryPos));
	TInt end=--iEnd;
	__ASSERT_DEBUG(end>=0,Panic(EPathUnderflow));
	iNodes[end]=aRef;
	iEntries[end]=TUint8(aEntry);
	}

void TBtreePath::GotoRoot(TBtreeHeight aHeight,TPageRef aRoot)
//
// Set the end of path to height and root-node given.
//
	{
	__ASSERT_DEBUG(aHeight>0&&aHeight<=KMaxBtreeHeight,Panic(EBadTreeHeight));
	iEnd=--aHeight;
	iNodes[aHeight]=aRoot;
	iEntries[aHeight]=0;
	}

EXPORT_C TBtree::TBtree(TBtreeMode aMode)
	: iPages(NULL),iFirst(KNullPageRef),iRoot(KNullPageRef),iHeight(0),iStatus(aMode)
/** Constructor that sets the B-tree mode.

@param aMode B-tree operating mode */
	{}

EXPORT_C TBtree::TBtree(const TBtreeToken& aToken,TBtreeMode aMode)
	: iPages(NULL)
/** Constructor that sets the B-tree mode and initialisation parameters.

@param aToken Parameters with which to initialise B-tree
@param aMode B-tree operating mode */
	{
	Set(aToken,aMode);
	__ASSERT_DEBUG(iHeight>=0&&iHeight<=KMaxBtreeHeight,Panic(EBadTreeHeight));
	}

EXPORT_C void TBtree::Connect(MPagePool* aPool,const MBtreeKey* aKey,const MBtreeLeafOrg* aLeafOrg,const MBtreeIndexOrg* anIndexOrg)
	{
	iPages=aPool;
	iKey=aKey;
	iLeafOrg=aLeafOrg;
	iIndexOrg=anIndexOrg;
	}

EXPORT_C void TBtree::Set(const TBtreeToken& aToken,TBtreeMode aMode)
/** Initialises the B-tree.

@param aToken Parameters with which to initialise the B-tree
@param aMode B-tree operating mode */
	{
	iFirst=aToken.iFirst;
	iRoot=aToken.iRoot;
	iHeight=aToken.iHeight;
	iStatus=aToken.IsBroken()?aMode|EBroken:aMode;
	__ASSERT_DEBUG(iHeight>=0&&iHeight<=KMaxBtreeHeight,Panic(EBadTreeHeight));
	}

EXPORT_C TBtreeToken TBtree::Token() const
/** Gets an object that encapsulates the TBtree settings. 

That object can then be used to externalise the settings.

@return Encapsulates the TBtree settings. */
	{
	return TBtreeToken(iFirst,iRoot,IsIntact()?iHeight:0);
	}

EXPORT_C TBool TBtree::FirstL(TBtreePos& aPos) const
/** Goes to the first entry in the B-tree.

@param aPos On return, the position of the first entry
@return True if there is a first entry, otherwise false */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	CheckIntactL();
	if (IsEmpty())
		return EFalse;
//
	TBtreePath& path=aPos.iPath;
	GotoRoot(path);
	DescendFirstL(path);
	return ETrue;
	}

EXPORT_C TBool TBtree::LastL(TBtreePos& aPos) const
/** Goes to the last entry in the B-tree.

@param aPos On return, the position of the last entry
@return True if there are any entries, otherwise false */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	CheckIntactL();
	if (IsEmpty())
		return EFalse;
//
	TBtreePath& path=aPos.iPath;
	GotoRoot(path);
	DescendLastL(path);
	return ETrue;
	}

EXPORT_C TBool TBtree::NextL(TBtreePos& aPos) const
/** Gets the position of the entry following the specified entry.

@param aPos On return, the position of the next entry
@return True if there are any more entries, otherwise false */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	TBtreePath& path=aPos.iPath;
	__ASSERT_ALWAYS(path.IsLeaf(),Panic(EInvalidTreePos));
	CheckIntactL();
	if (IsEmpty()||!AscendAtLastL(path))
		return EFalse;
//
	DescendFirstL(path);
	return ETrue;
	}

EXPORT_C TBool TBtree::PreviousL(TBtreePos& aPos) const
/** Gets the position of the entry preceding the specified entry.

@param aPos On return, the position of the preceding entry
@return True if there is a preceding entry, otherwise false */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	TBtreePath& path=aPos.iPath;
	__ASSERT_ALWAYS(path.IsLeaf(),Panic(EInvalidTreePos));
	CheckIntactL();
	if (IsEmpty()||!AscendAtFirstL(path))
		return EFalse;
//
	if (!path.IsLeaf())
		{
		path.Push(ChildNodeL(path));
		DescendLastL(path);
		}
	return ETrue;
	}

EXPORT_C void TBtree::ClearL()
/** Resets the B-tree to have zero-height, and a null root, and deletes all index 
pages. */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	if (IsEmpty())
		return;
	BeginUpdateL();
	DeleteIndexSetL();
	while (iFirst!=KNullPageRef)
		{
		TPageRef link=iLeafOrg->LinkNode(iPages->LockL(iFirst));
		iPages->DeleteL(iFirst);
		iFirst=link;
		}
	EndUpdate();
	}

EXPORT_C TInt TBtree::RepairL()
/** Attempts to repair a broken B-tree.

If the operating mode (TBtreeMode) is set to EBtreeSecure, then the tree can 
be repaired without any loss of data. Otherwise, the tree must be deleted 
entirely using ClearL(), and reconstructed using other means. 

Note that if multiple B-trees share a single store page pool, then reclaiming 
the pool will break all the other B-trees (but the Broken flag will not be 
set automatically). These trees can be repaired by calling MarkBroken() and 
then RepairL(), even if they were not of the EBtreeSecure type.

@return Number of leaves in the tree
@see TBtreeMode */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	if (IsEmpty())
		return 0;
	BeginUpdateL();
	DeleteIndexSetL();
// create the new index set, insert pivots on the end of the index set
	iHeight=1;
	iRoot=iFirst;
	TBtreePath path;
	TBtreePivot pivot;
	TInt count=0;
	TAny* seq=iPages->LockL(iFirst);
	for (;;)
		{
		count+=iLeafOrg->LastEntry(seq);
		TPageRef link=iLeafOrg->LinkNode(seq);
		if (link==KNullPageRef)
			break;
		TAny* next=iPages->LockL(link);
		NewPivot(seq,next,pivot);
		iPages->Unlock(seq);
		seq=next;
		if (iHeight==1)
			{	// first insertion, create a new index set
			iRoot=iFirst;
			NewRootL();
			GotoRoot(path);
			}
		TBtreeHeight end=path.End();
		InsertAtL(path,pivot,link);
		if (path.End()!=end)
			{	// path has been messed up by insertion, set path to last index entry
			GotoRoot(path);
			do path.Push(LastChildNodeL(path)); while (!path.IsLeaf());
			path.Pop();
			}
		else
			path.SetEntry(path.Entry()+1);
		}
	iPages->Unlock(seq);
	EndUpdate();
	return count;
	}

EXPORT_C TBool TBtree::FindL(TBtreePos& aPos,const TAny* aKey,TFind aMode) const
/** Searches for an entry and returns its position. 

@param aPos On return, the position of the entry found
@param aKey Pointer to the key of the entry for which to search
@param aMode Type of search to perform
@return True if search was successful, else false */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	CheckIntactL();
	if (IsEmpty())
		return EFalse;
	TBtreePath& path=aPos.iPath;
	TBool match=SearchL(path,aKey,aMode==ELessEqual||aMode==EGreaterThan);
	switch (aMode)
		{
	case EGreaterThan:
	case EGreaterEqual:	// move on if at end of node
		if (LastEntryL(path)==path.Entry())
			return NextL(aPos);
		return ETrue;
	case ELessThan:
	case ELessEqual:
		return PreviousL(aPos);
	case EEqualTo:
		break;
		}
	return match;
	}

EXPORT_C TBool TBtree::InsertL(TBtreePos& aPos,const TAny* anEntry,TInt aLength,TAllowDuplicates aDup)
/** Inserts an entry into the tree.

@param aPos On return, the position of the entry inserted
@param anEntry Pointer to the entry to insert
@param aLength Length of entry
@param aDup Flag to indicate whether duplicate entries are allowed in the tree
@return True if successful, false if the entry was a duplicate and aDup was 
set to ENoDuplicates */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	CheckIntactL();
	TBtreePath& path=aPos.iPath;
	if (IsEmpty())
		{
		NewTreeL();
		GotoRoot(path);
		}
	else
		{
		if (SearchL(path,iKey->Key(anEntry))&&aDup==ENoDuplicates)
			return EFalse;	// oops a duplicate
		}
	InsertAtL(path,TPtrC8((const TUint8*)anEntry,aLength));
	return ETrue;
	}

EXPORT_C TBool TBtree::DeleteL(const TAny* aKey)
/** Deletes an entry.

@param aKey Pointer to the key of the entry to delete
@return True if successful, false if the entry was not found */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	CheckIntactL();
	TBtreePath path;
	if (IsEmpty()||SearchL(path,aKey)==0)
		return EFalse;		// not found
	DeleteAtL(path);
	return ETrue;
	}

EXPORT_C void TBtree::DeleteAtL(TBtreePos& aPos)
/** Deletes the entry at the specified position

@param aPos Position of the entry to delete */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	TBtreePath& path=aPos.iPath;
	__ASSERT_ALWAYS(path.IsLeaf(),Panic(EInvalidTreePos));
	CheckIntactL();
	DeleteAtL(path);
	}

EXPORT_C void TBtree::ExtractAtL(const TBtreePos& aPos,TAny* anEntry,TInt aMaxLength) const
/** Gets the entry at the specified position.

@param aPos Position of the entry to get
@param anEntry Buffer into which to copy the entry
@param aMaxLength Length of the anEntry buffer. If the entry is longer than 
this, only the first aMaxLength bytes will be copied. */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	const TBtreePath& path=aPos.iPath;
	__ASSERT_ALWAYS(path.IsLeaf(),Panic(EInvalidTreePos));
	CheckIntactL();
	TAny* pp=GetNodeL(path);
	TPtrC8 entry=iLeafOrg->Entry(pp,path.Entry());
	Mem::Copy((TUint8*)anEntry,entry.Ptr(),Min(aMaxLength,entry.Size()));
	iPages->Unlock(pp);
	}

EXPORT_C TBool TBtree::ResetL(TBtreeMark& aMark) const
/** Resets an iterator to the root of the tree.

@param aMark Iterator to set
@return True if successful, false if the B-tree is empty */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	if (IsEmpty())
		return EFalse;
//
	TAny* first=iPages->LockL(iFirst);
	aMark.iLeaf=iFirst;
	aMark.iLink=iLeafOrg->LinkNode(first);
	aMark.iEntry=0;
	aMark.iLast=TUint8(iLeafOrg->LastEntry(first));
	iPages->Unlock(first);
	return ETrue;
	}

EXPORT_C TBool TBtree::NextL(TBtreeMark& aMark) const
/** Advances an iterator to the next entry.

@param aMark Iterator to use. On return, the iterator is advanced to the next 
entry.
@return True if successful, false if there is no next entry */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	if (IsEmpty())
		return EFalse;
//
	++aMark.iEntry;
	if (aMark.iEntry<aMark.iLast)
		return ETrue;
//
	if (aMark.iLink!=KNullPageRef)
		{
		TAny* next=iPages->LockL(aMark.iLink);
		aMark.iLeaf=aMark.iLink;
		aMark.iLink=iLeafOrg->LinkNode(next);
		aMark.iEntry=0;
		aMark.iLast=TUint8(iLeafOrg->LastEntry(next));
		iPages->Unlock(next);
		return ETrue;
		}
//
	return EFalse;
	}

EXPORT_C void TBtree::ExtractAtL(const TBtreeMark& aMark,TAny* anEntry,TInt aMaxLength) const
/** Gets an entry at specified iterator position.

@param aMark Position of the entry to get
@param anEntry Buffer into which to copy the entry
@param aMaxLength Length of anEntry buffer. If the entry is longer than this, 
only the first aMaxLength bytes are copied. */
	{
	__ASSERT_DEBUG(iPages!=0,Panic(ENoConnect));
	TAny* pp=iPages->LockL(aMark.iLeaf);
	TPtrC8 entry=iLeafOrg->Entry(pp,aMark.iEntry);
	Mem::Copy((TUint8*)anEntry,entry.Ptr(),Min(aMaxLength,entry.Size()));
	iPages->Unlock(pp);
	}

TBool TBtree::AscendAtLastL(TBtreePath& aPath) const
//
// While aPath is at the end of its node, ascend.
// If not at end, then move right and return true. Return false if we've run out of tree.
//
	{
	for (;;)
		{
		TInt last=LastEntryL(aPath);
		if (aPath.IsLeaf())
			--last;
		TInt entry=aPath.Entry();
		if (entry<last)
			{
			aPath.SetEntry(entry+1);
			return ETrue;	// have a next entry at this level
			}
		if (IsRoot(aPath))
			return EFalse;		// end of tree
		aPath.Pop();
		}
	}

TBool TBtree::AscendAtFirstL(TBtreePath& aPath) const
//
// While aPath is at the beginning of its node, ascend.
// If not at start, then move left and return true. Return false if we've run out of tree.
//
	{
	for (;;)
		{
		TInt entry=aPath.Entry();
		if (entry>0)
			{
			aPath.SetEntry(entry-1);
			return ETrue;		// have a previous entry at this level
			}
		if (IsRoot(aPath))
			return EFalse;		// beginning of sequence
		aPath.Pop();
		}
	}

void TBtree::DescendFirstL(TBtreePath& aPath) const
//
// Descend the tree to next entry on the bottom level.
//
	{
	while (!aPath.IsLeaf())
		aPath.Push(ChildNodeL(aPath));
	}

void TBtree::DescendLastL(TBtreePath& aPath) const
//
// Descend the tree to the last entry on the bottom level.
//
	{
	while (!aPath.IsLeaf())
		aPath.Push(LastChildNodeL(aPath));
	aPath.SetEntry(LastEntryL(aPath)-1);
	}

TBool TBtree::SearchL(TBtreePath& aPath,const TAny* aKey,TBool aAfter) const
//
// Search the B-tree for an entry. Return true if an exact match is found at base level.
//
	{
	__ASSERT_DEBUG(!IsEmpty(),Panic(ENoTree));
	TInt entry;
	GotoRoot(aPath);
	while (!aPath.IsLeaf())
		{
		TAny* pp=GetNodeL(aPath);
		iIndexOrg->Search(pp,aKey,*iKey,aAfter,entry);
		aPath.SetEntry(entry);
		aPath.Push(iIndexOrg->ChildNode(pp,entry));
		iPages->Unlock(pp);
		}
	TAny* pp=GetNodeL(aPath);
	TBool found=iLeafOrg->Search(pp,aKey,*iKey,aAfter,entry);
	aPath.SetEntry(entry);
	iPages->Unlock(pp);
	return found;
	}

void TBtree::InsertAtL(TBtreePath& aPath,const TDesC8& anEntry)
//
// Insert anEntry at the current entry in aPath.
//
	{
	TBtreePivot pivot;
	TPageRef promote;
	BeginUpdateL();
	if (InsertAtL(aPath,anEntry,KNullPageRef,pivot,promote))
		InsertAtL(aPath,pivot,promote);
	EndUpdate();
	}

void TBtree::NewPivot(const TAny* aLeftNode,const TAny* aRightNode,TBtreePivot& aPivot) const
//
// Get the key organisation to generate a new pivot between the given leaf nodes.
//
	{
	const TAny* left=iLeafOrg->EntryPtr(aLeftNode,iLeafOrg->LastEntry(aLeftNode)-1);
	const TAny* right=iLeafOrg->EntryPtr(aRightNode,0);
	iKey->Between(iKey->Key(left),iKey->Key(right),aPivot);
	__DEBUG(TInt cleft=iKey->Compare(iKey->Key(left),aPivot.Ptr()));
	__DEBUG(TInt cright=iKey->Compare(iKey->Key(right),aPivot.Ptr()));
	__ASSERT_DEBUG((cleft<=0&&cright>0)||(cleft==0&&cright==0),Panic(EIllegalPivot));
	}

void TBtree::ReplacePivotL(TBtreePath& aPath,TAny* aNode,TBtreePivot& aPivot)
//
// Replace current pivot entry in aPath with aPivot.
//
	{
	TInt entry=aPath.Entry();
	// update the pivot in the parent
	if (iIndexOrg->Update(aNode,entry,aPivot))
		iPages->Unlock(aNode,EPageDirty);
	else
		{	// have to delete the old pivot and insert new one
		TPageRef child=iIndexOrg->ChildNode(aNode,entry+1);
		iIndexOrg->Delete(aNode,entry);
		iPages->Unlock(aNode,EPageDirty);
		InsertAtL(aPath,aPivot,child);
		}
	}

void TBtree::InsertAtL(TBtreePath& aPath,TBtreePivot& aPivot,TPageRef aChild)
	{
	TBtreePivot togglePivot;
	for (;;)
		{
		if (!InsertAtL(aPath,aPivot,aChild,togglePivot,aChild))
			break;
		if (!InsertAtL(aPath,togglePivot,aChild,aPivot,aChild))
			break;
		}
	}

TBool TBtree::InsertAtL(TBtreePath& aPath,const TDesC8& anEntry,TPageRef aChild,TBtreePivot& aPivot,TPageRef& aPromote)
//
// Insert entry on this level. Return requirement to propagate with aPromote, aChild and aPath ready.
//
	{
	TBool leaf=aPath.IsLeaf();
	const MBtreeNodeOrg* nodeOrg=NodeOrg(leaf);
	TInt entry=aPath.Entry();
	TAny* pNode=GetNodeL(aPath);

	if (leaf?iLeafOrg->Insert(pNode,entry,anEntry):iIndexOrg->Insert(pNode,entry,anEntry,aChild))
		{
		PutNode(pNode,leaf);
		return EFalse;
		}
	// whatever happens next, we will affect the index set
	MarkBroken();
	// attempt an overflow insertion
	if (!IsRoot(aPath))
		{ // we have an ancestor
		TPageRef node=aPath.Node(); // save this in case we cannot overflow
		aPath.Pop();
		TInt child=aPath.Entry();
		TAny* pParent=GetNodeL(aPath);
		TAny* pSibling;
		if (child<iIndexOrg->LastEntry(pParent))
			{ // try to overflow to the right
			pSibling=iPages->LockL(iIndexOrg->ChildNode(pParent,child+1));
			if (leaf)
				{
				if (iLeafOrg->InsertOverflow(pNode,pSibling,entry,ETrue,anEntry))
					{
					NewPivot(pNode,pSibling,aPivot);
overflowOK:			PutNode(pSibling,leaf);
					PutNode(pNode,leaf);
					aPath.SetEntry(child);
					ReplacePivotL(aPath,pParent,aPivot);
					return EFalse; // completed insertion
					}
				}
			else
				{
				const TPtrC8 pivot=iIndexOrg->Entry(pParent,child);
				if (iIndexOrg->InsertOverflow(pNode,pSibling,entry,ETrue,anEntry,aChild,pivot,aPivot))
					goto overflowOK;
				}
			iPages->Unlock(pSibling);
			}
		if (--child>=0)
			{
			pSibling=iPages->LockL(iIndexOrg->ChildNode(pParent,child));
			if (leaf)
				{
				if (iLeafOrg->InsertOverflow(pSibling,pNode,entry,EFalse,anEntry))
					{
					NewPivot(pSibling,pNode,aPivot);
					goto overflowOK;
					}
				}
			else
				{
				const TPtrC8 pivot=iIndexOrg->Entry(pParent,child);
				if (iIndexOrg->InsertOverflow(pSibling,pNode,entry,EFalse,anEntry,aChild,pivot,aPivot))
					goto overflowOK;
				}
			iPages->Unlock(pSibling);
			}
			// cannot overflow, so...
		iPages->Unlock(pParent);
		aPath.Push(node);
		}

		// do a split insertion
	TAny* pSibling=iPages->AllocL();
	nodeOrg->Init(pSibling);
	if (leaf)
		{
		iLeafOrg->InsertSplit(pNode,pSibling,entry,anEntry);
		NewPivot(pNode,pSibling,aPivot);
		aPromote=iPages->AssignL(pSibling); // non-reclaimable, mustn't lose track of leaves
		iLeafOrg->SetLinkNode(pNode,aPromote); // set up sequencing
		iPages->Unlock(pNode,EPageUpdate);
		}
	else
		{
		iIndexOrg->InsertSplit(pNode,pSibling,entry,anEntry,aChild,aPivot);
		aPromote=iPages->AssignL(pSibling,EPageReclaimable);
		iPages->Unlock(pNode,EPageDirty);
		}
		// propagate insert up the tree
	if (IsRoot(aPath))
		{ // new root node needed
		NewRootL();
		GotoRoot(aPath);
		}
	else
		aPath.Pop();
	return ETrue;
	}

void TBtree::DeleteAtL(TBtreePath& aPath)
//
// Delete the current entry on the path
//
	{
	__ASSERT_DEBUG(aPath.IsLeaf(),Panic(EInvalidTreePos));
	TBool leaf=ETrue;
	const MBtreeNodeOrg* nodeOrg=iLeafOrg;
	TAny* pNode=GetNodeL(aPath);
	BeginUpdateL();
	TBtreePivot newPivot;
//
	for (;;)
		{
		TPageRef node=aPath.Node();
		TInt entry=aPath.Entry();
		if (nodeOrg->Delete(pNode,entry))
			{ // success, no underflow
			if (leaf&&iLeafOrg->LastEntry(pNode)==entry)
				{ // we deleted the last entry so... replace the pivot to ensure invariant
				TPageRef next=iLeafOrg->LinkNode(pNode);
				if (next!=KNullPageRef)
					{ // replace the pivot
					MarkBroken();
					TAny* pSibling=iPages->LockL(next);
					NewPivot(pNode,pSibling,newPivot);
					iPages->Unlock(pSibling);
					PutNode(pNode,leaf);
						// find dividing pivot which we know is there
					do aPath.Pop(); while (aPath.Entry()==LastEntryL(aPath));
					ReplacePivotL(aPath,GetNodeL(aPath),newPivot);
					break;
					}
				}
			PutNode(pNode,leaf);
			break;
			}
		if (IsRoot(aPath))
			{ // the root! If it has entries then we're done
			if (nodeOrg->LastEntry(pNode)>0)
				PutNode(pNode,leaf);
			else
				{
				if (--iHeight!=0) // get child as root
					iRoot=iIndexOrg->ChildNode(pNode,0);
				else // tree is empty
					iRoot=iFirst=KNullPageRef;
				iPages->DeleteL(node);
				}
			break;
			}
		// will definitely affect the index set
		MarkBroken();
			// block has underflowed.. must try to redistribute or concatenate
		aPath.Pop();
		TAny* pParent=GetNodeL(aPath);
		TAny* pSibling;
		entry=aPath.Entry();
		if (entry>0)
			{
			aPath.SetEntry(--entry);
			pSibling=iPages->LockL(iIndexOrg->ChildNode(pParent,entry)); // on left
			}
		else
			{ // There must be a sibling for non-root nodes!
			__ASSERT_DEBUG(iIndexOrg->LastEntry(pParent)>0,Panic(EBadEntryCount));
			pSibling=pNode;
			node=iIndexOrg->ChildNode(pParent,1); // on right of first child
			pNode=iPages->LockL(node);
			}
			// redistribute between nodes
		if (leaf)
			{
			if (iLeafOrg->Redistribute(pSibling,pNode))
				{
				NewPivot(pSibling,pNode,newPivot);
redistributeOK:	PutNode(pSibling,leaf);
				PutNode(pNode,leaf);
				ReplacePivotL(aPath,pParent,newPivot);
				break;
				}
				// failed so concatenate
			iLeafOrg->Concatenate(pSibling,pNode);
			iPages->UpdateL(pSibling); // must change it here and now: link target is being deleted
			}
		else
			{
			TPtrC8 pivot=iIndexOrg->Entry(pParent,entry);
			if (iIndexOrg->Redistribute(pSibling,pNode,pivot,newPivot))
				goto redistributeOK;
				// failed so concatenate
			iIndexOrg->Concatenate(pSibling,pNode,pivot);
			iPages->Unlock(pSibling,EPageDirty);
			}
		iPages->DeleteL(node);
		leaf=EFalse;
		nodeOrg=iIndexOrg;
		pNode=pParent;
		}
	EndUpdate();
	}

void TBtree::DeleteIndexSetL()
//
// Destroy the index set, handle broken state too
//
	{
	__ASSERT_DEBUG(!IsEmpty(),User::Invariant());
	if (IsIntact())
		{
		TBtreePath path;
		GotoRoot(path);
		if (!path.IsLeaf())
			{
			MarkBroken();
			DeleteIndexNodesL(path,ETrue);
			}
		}
	// delete the leading edge
	while (iRoot!=iFirst)
		{
		TPageRef edge=iIndexOrg->ChildNode(iPages->LockL(iRoot),0);
		iPages->DeleteL(iRoot);
		iRoot=edge;
		}
	}

void TBtree::DeleteIndexNodesL(TBtreePath& aPath,TBool aLeadingEdge)
//
// Destroy the children of the node in aPath, then destroy the node itself.
// do not destroy the sequence set or the leading edge
//
	{
	if (aPath.End()>1)
		{	// erase children
		for (TInt ii=LastEntryL(aPath);ii>=0;--ii)
			{
			aPath.Push(ChildNodeL(aPath,ii));
			DeleteIndexNodesL(aPath,aLeadingEdge && ii==0);
			aPath.Pop();
			}
		}
	if (!aLeadingEdge)
		iPages->DeleteL(aPath.Node());
	}

void TBtree::NewTreeL()
//
// Construct the initial node on empty tree.
//
	{
	TAny* first=iPages->AllocL();
	iLeafOrg->Init(first);
	iRoot=iFirst=iPages->AssignL(first); // non-reclaimable, it's a leaf as well as the initial root
	iHeight=1;
	}

void TBtree::NewRootL()
//
// Create a new root node and set its child to the current root.
//
	{
	if (iHeight==KMaxBtreeHeight)
		__LEAVE(KErrOverflow); // tree is max height
	TAny* root=iPages->AllocL();
	iIndexOrg->Init(root);
	iIndexOrg->MakeRoot(root,iRoot);
	iRoot=iPages->AssignL(root); // non-reclaimable, mustn't lose track of successive roots
	++iHeight;
	}

void TBtree::BeginUpdateL()
//
// Prepare to update the tree. Mark it dirty and ensure locked pages are abandoned on failure.
//
	{
	iPages->PushL();
	MarkDirty();
	}

void TBtree::EndUpdate()
//
// Update complete, clear the broken flag and discard page pool acquisition
//
	{
	iPages->Pop();
	MarkIntact();
	}

void TBtree::CheckIntactL() const
	{
	if (IsBroken())
		__LEAVE(KErrCorrupt);
	}

void TBtree::GotoRoot(TBtreePath& aPath) const
//
// Set the path to the root of the tree.
//
	{
	__ASSERT_DEBUG(!IsEmpty(),Panic(ENoTree));
	aPath.GotoRoot(iHeight,iRoot);
	}

TAny* TBtree::GetNodeL(const TBtreePath& aPath) const
	{
	return iPages->LockL(aPath.Node());
	}

void TBtree::PutNode(TAny* aNode,TBool aLeaf) const
	{
	iPages->Unlock(aNode,aLeaf&&(iStatus&ESecure)?EPageUpdate:EPageDirty);
	}

TInt TBtree::LastEntryL(const TBtreePath& aPath) const
	{
//#pragma message( __FILE__ " : 'TBtree::LastEntryL()' should have this information without having to look at the tree" )
	TAny* pp=GetNodeL(aPath);
	TInt cc=NodeOrg(aPath.IsLeaf())->LastEntry(pp);
	iPages->Unlock(pp);
	return cc;
	}

TPageRef TBtree::ChildNodeL(const TBtreePath& aPath) const
//
// Get current child node.
//
	{
	__ASSERT_DEBUG(!aPath.IsLeaf(),Panic(EInvalidTreePos));
	TAny* pp=GetNodeL(aPath);
	TPageRef pg=iIndexOrg->ChildNode(pp,aPath.Entry());
	iPages->Unlock(pp);
	return pg;
	}

TPageRef TBtree::ChildNodeL(TBtreePath& aPath,TInt aChild) const
	{
	aPath.SetEntry(aChild);
	return ChildNodeL(aPath);
	}

TPageRef TBtree::LastChildNodeL(TBtreePath& aPath) const
	{
	__ASSERT_DEBUG(!aPath.IsLeaf(),Panic(EInvalidTreePos));
	TAny* pp=GetNodeL(aPath);
	TInt entry=iIndexOrg->LastEntry(pp);
	aPath.SetEntry(entry);
	TPageRef pg=iIndexOrg->ChildNode(pp,entry);
	iPages->Unlock(pp);
	return pg;
	}

