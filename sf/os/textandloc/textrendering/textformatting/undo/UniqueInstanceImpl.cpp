/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include "UniqueInstanceImpl.h"
#include "AssertFileAndLine.h"
#include <e32math.h>

using namespace UniqueInstance;

namespace UniqueInstance
{
_LIT(KUniqueInstance, "UniqInst");

void DestroyRUniqueInstance(void* aUniqueInstance)
	{
	reinterpret_cast<RInstanceImpl*>(aUniqueInstance)->Close();
	}

/**
 * Iterator that traverses an RSkipList's pointers.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class TSkipListIterator
	{
public:
	TSkipListIterator(TCompareFn* aCompare, void* aMatch,
		RSkipList::TSection** aFirstLink, TInt aNumLinks);
	RSkipList::TSection& operator*();
	TInt Level();
	TBool DecrementLevel();		// returns true if aMatch found
	void SpliceIn(RSkipList::TSection* aSection) const;
	void SpliceOut() const;
private:
	TCompareFn*				iCompare;
	void*					iMatch;
	RSkipList::TSection**	iCurrentLink;
	TInt					iLevel;
	};
}

TSkipListIterator::TSkipListIterator(TCompareFn* aCompare, void* aMatch,
	RSkipList::TSection** aFirstLink, TInt aNumLinks)
	: iCompare(aCompare), iMatch(aMatch),
	iCurrentLink(aFirstLink - 1), iLevel(aNumLinks) {}

RSkipList::TSection& TSkipListIterator::operator*()
	{
	return **iCurrentLink;
	}

TInt TSkipListIterator::Level()
	{
	return iLevel;
	}

TBool TSkipListIterator::DecrementLevel()
	{
	--iLevel;
	++iCurrentLink;
	for(;;)
		{
		if (!*iCurrentLink)
			return EFalse;
		TInt compare = iCompare((*iCurrentLink)->iElement.iObject, iMatch);
		if (0 <= compare)
			return compare == 0;
		iCurrentLink = &(*iCurrentLink)->iLinks[-iLevel];
		}
	}

void TSkipListIterator::SpliceIn(RSkipList::TSection* aSection) const
	{
	aSection->iLinks[-iLevel] = *iCurrentLink;
	*iCurrentLink = aSection;
	}

void TSkipListIterator::SpliceOut() const
	{
	*iCurrentLink = (*iCurrentLink)->iLinks[-iLevel];
	}


///////////////////////
//					 //
//	CRepositoryImpl  //
//					 //
///////////////////////

CRepositoryImpl::CRepositoryImpl(TCompareFn* aCompare,
	TDeleteFn* aDelete, TCopyFnL* aCopyL, TInt aObjectSize)
	: iCompare(aCompare), iDelete(aDelete), iCopyL(aCopyL), iObjectSize(aObjectSize)
	{
	iNullElement.iObject = 0;
	ASSERT(iCompare);
	ASSERT(iDelete);
	ASSERT(iCopyL);
	}

CRepositoryImpl::~CRepositoryImpl()
	{
	iSkipList.Close();
	}

void CRepositoryImpl::ConstructL(TInt aMaxLinks)
	{
	iSkipList.Open(*iCompare, aMaxLinks);
	}

SElement* CRepositoryImpl::NullElement()
	{
	return &iNullElement;
	}

TBool CRepositoryImpl::IsNull(SElement* a) const
	{
	return a == &iNullElement;
	}

void CRepositoryImpl::Test() const
	{
	iSkipList.Test();
	}

SElement* CRepositoryImpl::InsertOrIncL(void* aElt)
	{
	SElement* r = iSkipList.AddExisting(aElt);
	if (!r)
		return iSkipList.AddNewL(aElt);
	iDelete(aElt);
	return r;
	}

SElement* CRepositoryImpl::IncOrCopyL(void* aElt)
	{
	SElement* r = iSkipList.AddExisting(aElt);
	if (r)
		return r;
	void* copy = iCopyL(aElt, iObjectSize);
	CleanupStack::PushL(TCleanupItem(iDelete, copy));
	r = iSkipList.AddNewL(copy);
	CleanupStack::Pop();
	return r;
	}

void CRepositoryImpl::DeleteOrDec(SElement* aNoLongerNeeded)
	{
	if (IsNull(aNoLongerNeeded))
		return;
	if (--(aNoLongerNeeded->iRefCount))
		return;
	void* object = aNoLongerNeeded->iObject;
	iSkipList.Remove(object);
	iDelete(object);
	}

void* CRepositoryImpl::DetatchOrCopyL(SElement* aWritableCopyNeeded)
	{
	void* retval = 0;
	if (!IsNull(aWritableCopyNeeded))
		{
		if (1 < aWritableCopyNeeded->iRefCount)
			{
			retval = iCopyL(aWritableCopyNeeded->iObject, iObjectSize);
			--(aWritableCopyNeeded->iRefCount);
			}
		else
			{
			retval = aWritableCopyNeeded->iObject;
			iSkipList.Remove(aWritableCopyNeeded->iObject);
			}
		}
	return retval;
	}


/////////////////
//			   //
//	RSkipList  //
//			   //
/////////////////

RSkipList::~RSkipList()
	{
	ASSERT(!iSentinel);
	}

void RSkipList::Open(TCompareFn* aCompare, TInt aMaxLinks)
	{
	ASSERT(0 < aMaxLinks);
	iLinkCount = aMaxLinks;
	TSection** sentinelBuffer = reinterpret_cast<TSection**>(
		operator new((aMaxLinks - 1) * sizeof(TSection*) + sizeof(TSection)));
	iSentinel = reinterpret_cast<TSection*>(sentinelBuffer + (iLinkCount - 1));
	iCompare = aCompare;
	for (TInt i = 0; i != iLinkCount; ++i)
		iSentinel->iLinks[-i] = 0;
	// iSentinel will be released in Close(). so,
	// coverity[memory_leak]
	}

void RSkipList::Close()
	{
	ASSERT(IsEmpty());
	if (iSentinel)
		operator delete(FirstLink());
	iSentinel = 0;
	}

SElement* RSkipList::AddExisting(void* aNewElt)
	{
	TSkipListIterator it(iCompare, aNewElt, FirstLink(), iLinkCount);
	while (it.Level())
		{
		if (it.DecrementLevel())
			{
			SElement* e = &(*it).iElement;
			++(e->iRefCount);
			return e;
			}
		}
	return 0;
	}

void* RSkipList::Remove(void* aNoLongerNeeded)
	{
	void* object = 0;
	TSection** toBeDeleted = 0;

	TSkipListIterator it(iCompare, aNoLongerNeeded, FirstLink(), iLinkCount);
	while (it.Level())
		{
		if (it.DecrementLevel())
			{
			if (!toBeDeleted)
				{
				TSection& s = *it;
				toBeDeleted = s.iLinks - it.Level();
				object = s.iElement.iObject;
				}
			it.SpliceOut();
			}
		}

	ASSERT(toBeDeleted);
	operator delete(toBeDeleted);

	return object;
	}

SElement* RSkipList::AddNewL(void* aNewElt)
	{
	TInt numLinks = GenerateNumLinks();
	TSection** currentLink = reinterpret_cast<TSection**>
		( operator new((numLinks - 1) * sizeof(TSection*) + sizeof(TSection), ELeave) );
	TSection* newSection = reinterpret_cast<TSection*>(currentLink + (numLinks - 1));
	newSection->iElement.iObject = aNewElt;
	newSection->iElement.iRefCount = 1;

	TSkipListIterator it(iCompare, aNewElt, FirstLink(), iLinkCount);
#ifdef _DEBUG
	// to ensure allocated memory will be attached to the link list.
	TBool attachedToLinkList = EFalse;
#endif
	while (it.Level())
		{
		it.DecrementLevel();
		if (it.Level() < numLinks)
			{
			it.SpliceIn(newSection);
#ifdef _DEBUG
			attachedToLinkList = ETrue;
#endif
			}
		}
#ifdef _DEBUG
	ASSERT(attachedToLinkList);
#endif
	// The memory currentLink will be attached to the link list,
	// and will be released in Remove(). so,
	// coverity[memory_leak]
	return &newSection->iElement;
	}

TBool RSkipList::IsEmpty() const
	{
	TSection** link = FirstLink();
	for (TInt i = iLinkCount; i; --i, ++link)
		{
		if (*link)
			return EFalse;
		}
	return ETrue;
	}

RSkipList::TSection** RSkipList::FirstLink() const
	{
	ASSERT(iSentinel);
	return &iSentinel->iLinks[ 1 - iLinkCount ];
	}

// 3/4 chance that 1 is returned
// (1/4)^n * (3/4) chance that (n-1) is returned
TInt RSkipList::GenerateNumLinks() const
	{
	TInt32 bits = 0;
	TInt r = 1;

	for (;;)
		{
		if (r == iLinkCount)
			return r;
		if ((r & 7) == 0)
			bits = Math::Random();
		if ((bits & 3) != 0)
			return r;
		++r;
		bits >>= 2;
		}
	}

void RSkipList::TestLinks(TSection* aStart, TSection* aEnd, TInt aLink) const
	{
	while (aStart != aEnd)
		{
		__ASSERT_ALWAYS(aStart, User::Panic(KUniqueInstance, 0));
		TSection* aNext = aStart->iLinks[-aLink];
		if (aLink)
			TestLinks(aStart, aNext, aLink - 1);
		aStart = aNext;
		}
	}

TInt RSkipList::Test() const
	{
	TestLinks(iSentinel, 0, iLinkCount - 1);

	TSection* last = iSentinel->iLinks[0];
	if (!last)
		return 0;
	TInt count = 1;
	for (TSection* p = last->iLinks[0]; p; last = p, p = p->iLinks[0])
		{
		++count;
		__ASSERT_ALWAYS(iCompare(last->iElement.iObject, p->iElement.iObject) < 0, User::Panic(KUniqueInstance, 0));
		}
	return count;
	}

