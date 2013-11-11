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


#ifndef UNIQUEINSTANCEIMPL_H_
#define UNIQUEINSTANCEIMPL_H_

#include "UniqueInstance.h"

namespace UniqueInstance
{

/**
@internalComponent
*/
void DestroyRUniqueInstance(void* runique);
/**
 * Reference-counted object.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
struct SElement
	{
	TInt iRefCount;
	void* iObject;
	};

/**
 * Skip list holding sorted, reference counted objects
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class RSkipList
	{
public:
	struct TSection
		{
		TSection* iLinks[1];
		SElement iElement;
		};

	RSkipList& operator=(const RSkipList&);
	RSkipList(const RSkipList&);

	void TestLinks(TSection* aStart, TSection* aEnd, TInt aLink) const;

	RSkipList() : iSentinel(0), iCompare(0) {}
	~RSkipList();
	void Open(TCompareFn* aCompare, TInt aMaxLinks);
	void Close();

	/**
	 * Adds an element only if it already exists, otherwise returns 0
	 */
	SElement* AddExisting(void* aElt);
	/**
	 * Adds a new element. aNewElt must not already have an equivalent in this
	 * skip list.
	 */
	SElement* AddNewL(void* aNewElt);
	/**
	 * Removes the element, no matter what its reference count is. Return a pointer
	 * to the object, which is now no longer owned.
	 */
	void* Remove(void* aNoLongerNeeded);
	/**
	 * Returns true iff skip list has no elements.
	 */
	TBool IsEmpty() const;
	/**
	 * Runs tests on the integrity of the skip list.
	 * Returns the number of elements in the list.
	 */
	TInt Test() const;

private:
	TSection*	iSentinel;
	TInt		iLinkCount;
	TCompareFn*	iCompare;

	TSection** FirstLink() const;

	TInt GenerateNumLinks() const;
	};
}

/**
 * Implements the unique instance repository behaviour
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(UniqueInstance::CRepositoryImpl) : public CBase
	{
public:
	CRepositoryImpl(TCompareFn* aCompare, TDeleteFn* aDelete, TCopyFnL* aCopyL,
		TInt aObjectSize);
	~CRepositoryImpl();
	void ConstructL(TInt aMaxLinks);

	/**
	 * Adds aElt to the list, passing ownership.
	 */
	SElement*	InsertOrIncL(void* aElt);
	/**
	 * Adds aElt to the list with the caller retaining ownership.
	 */
	SElement*	IncOrCopyL(void* aElt);
	/**
	 * Deletes from the list. aNoLongerNeeded points to an element of the list.
	 */
	void		DeleteOrDec(UniqueInstance::SElement* aNoLongerNeeded);
	/**
	 * Removes from the list, passing ownership back to the caller.
	 */
	void*		DetatchOrCopyL(UniqueInstance::SElement* aWritableCopyNeeded);
	/**
	 * Returns a prototype null element. Ownership is retained.
	 */
	SElement*	NullElement();
	/**
	 * Compares the element pointer passed against the prototype null element.
	 */
	TBool		IsNull(SElement* a) const;
	/**
	 * Runs an integrity check. Panics on failure.
	 */
	void		Test() const;

private:
	TCompareFn*	iCompare;
	TDeleteFn*	iDelete;
	TCopyFnL*	iCopyL;
	TInt		iObjectSize;

	SElement	iNullElement;
	RSkipList	iSkipList;
	};

#endif	// UNIQUEINSTANCEIMPL_H_
