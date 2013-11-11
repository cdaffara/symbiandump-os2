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

inline TBtreeToken::TBtreeToken(TPageRef aFirst,TPageRef aRoot,TBtreeHeight aHeight)
	: iFirst(aFirst),iRoot(aRoot),iHeight(aHeight)
	{
	__ASSERT_DEBUG(aHeight<=KMaxBtreeHeight,User::Invariant());
	}

inline void TBtree::MarkIntact()
	{iStatus&=~EBroken;}
inline TBool TBtree::IsRoot(const TBtreePath& aPath) const
	{return aPath.End()+1==iHeight;}
inline const MBtreeNodeOrg* TBtree::NodeOrg(TBool aLeaf) const
	{if (aLeaf) return iLeafOrg; else return iIndexOrg;}

inline const TBtreeInlineLeafOrg::SNode* TBtreeInlineLeafOrg::Node(const TAny* aNode)
	{return (const SNode*)aNode;}
inline TBtreeInlineLeafOrg::SNode* TBtreeInlineLeafOrg::Node(TAny* aNode)
	{return (SNode*)aNode;}
inline const TUint8* TBtreeInlineLeafOrg::Entry(const SNode* aNode,TInt anEntry) const
	{return &aNode->iEntries[anEntry*iEntrySize];}
inline TUint8* TBtreeInlineLeafOrg::Entry(SNode* aNode,TInt anEntry) const
	{return &aNode->iEntries[anEntry*iEntrySize];}

inline const TBtreeInlineIndexOrg::SNode* TBtreeInlineIndexOrg::Node(const TAny* aNode)
	{return (const SNode*)aNode;}
inline TBtreeInlineIndexOrg::SNode* TBtreeInlineIndexOrg::Node(TAny* aNode)
	{return (SNode*)aNode;}
inline const TBtreeInlineIndexOrg::SEntry* TBtreeInlineIndexOrg::Entry(const SNode* aNode,TInt anEntry) const
	{return (const SEntry*)&aNode->iEntries[anEntry*iEntrySize];}
inline TBtreeInlineIndexOrg::SEntry* TBtreeInlineIndexOrg::Entry(SNode* aNode,TInt anEntry) const
	{return (SEntry*)&aNode->iEntries[anEntry*iEntrySize];}
inline TInt TBtreeInlineIndexOrg::KeySize() const
	{return iEntrySize-sizeof(TPageRef);}

