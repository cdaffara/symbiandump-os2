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

EXPORT_C void MBtreeNodeOrg::Init(TAny* aNode) const
//
// Default node initialisation just zero filling.
//
	{
	Mem::FillZ(aNode,KPoolPageSize);
	}

EXPORT_C TBool MBtreeLeafOrg::Search(const TAny* aNode,const TAny* aKey,const MBtreeKey& aComp,TBool aLast,TInt& aPos) const
//
// Default to using a binary search through the node, using KeyAt and the key's comparison.
// if aLast is 1, aEntry is the first > the key, return true if previous entry is a match
// if aLast is 0, aEntry is the first entry >= the key, return true is current entry is a match
//
	{
	TInt match=EFalse;
	TInt rr=LastEntry(aNode)-1;
	TInt ll=0;
	__ASSERT_DEBUG(rr>=-1,Panic(EBadEntryCount));
	while (ll<=rr)
		{
		TInt mm=(rr+ll)>>1;
		TInt res=aComp.Compare(aKey,aComp.Key(EntryPtr(aNode,mm)));
		if (res==0)
			match=ETrue;
		if (res+aLast<=0)
			rr=mm-1;		// compare < 0 or match and aLast=0
		else
			ll=mm+1;		// compare > 0 or match and aLast=1
		}
	aPos=ll;
	return match;
	}

EXPORT_C TBool MBtreeLeafOrg::InsertOverflow(TAny*,TAny*,TInt,TBool,const TDesC8&) const
//
// For simple implementations allow this to be unimplemented. Report failure
//
	{
	return EFalse;
	}

EXPORT_C TBool MBtreeIndexOrg::Search(const TAny* aNode,const TAny* aKey,const MBtreeKey& aComp,TBool aLast,TInt& aPos) const
//
// Default to using a binary search through the node, using KeyAt and the key's comparison.
// if aLast is 1, aEntry is the first > the key
// if aLast is 0, aEntry is the first entry >= the key
//
	{
	TInt rr=LastEntry(aNode);
	TInt ll=-1;
	__ASSERT_DEBUG(rr>0,Panic(EBadEntryCount));
	while (rr>ll+1)
		{
		TInt mm=(rr+ll)>>1;
		if (aComp.Compare(aKey,EntryPtr(aNode,mm))+aLast<=0)
			rr=mm;		// compare < 0 or match and aLast=0
		else
			ll=mm;		// compare > 0 or match and aLast=1
		}
	aPos=rr;
	return EFalse;		// return not used
	}

EXPORT_C TBool MBtreeIndexOrg::InsertOverflow(TAny*,TAny*,TInt,TBool,const TDesC8&,TPageRef,const TDesC8&,TBtreePivot&) const
//
// For simple implementations allow this to be unimplemented. Report failure.
//
	{
	return EFalse;
	}

EXPORT_C TBool MBtreeIndexOrg::Update(TAny*,TInt,const TDesC8&) const
//
// Allow to be unimplemented. Return failure.
//
	{
	return EFalse;
	}

