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


#ifndef UNIQUEINSTANCEBASE_H_
#define UNIQUEINSTANCEBASE_H_

#include <e32base.h>

// The namespace UniqueInstance contains definitions that are not useful to the
// user, but are useful to the implementation. There is no need to use any symbols
// within it directly.
namespace UniqueInstance
{

/**
@internalComponent
*/
typedef TInt TCompareFn(void*,void*);

/**
destroy free aObject.
@internalComponent
*/
typedef void TDeleteFn(void* aObjectToBeDeleted);
/** 
Copy aObjectToCopy into a newly allocated buffer, return the new object.
aObjectSize is the size of the object that the repository has been specialised
to work with. If this is not a true size of the object (for example in the case
of HBufC) this value should be ignored.
@internalComponent
*/
typedef void* TCopyFnL(void* aObjectToCopy, TInt aObjectSize);

struct SElement;
class CRepositoryImpl;
class RInstanceImpl;

/**
 * Type-unsafe repository with shared equivalent objects.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CUniqueInstanceRepositoryBase) : public CBase
	{
public:
	static void* DumbCopyL(void* aObjectTocopy, TInt aNumberOfBytes);
	static void DumbDelete(void* aDeleteThis);

	void ConstructL(TCompareFn*, TDeleteFn*, TCopyFnL*,
		TInt aMaxLinks, TInt aObjectSize);
	~CUniqueInstanceRepositoryBase();
	void Test() const;

private:
	friend class RInstanceImpl;
	CRepositoryImpl* iImpl;
	};

/**
 * Type-unsafe repository client.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class RInstanceImpl
	{
public:
	explicit RInstanceImpl(CUniqueInstanceRepositoryBase& aRepository);
	~RInstanceImpl();		// asserts that no object is owned

	void TakeL(void* aObject);
	void TakeCopyL(void* aObject);
	void* Peek() const;
	void CopyTo(RInstanceImpl& aOther) const;
	void MoveTo(RInstanceImpl& aOther);
	void* DropL();
	void Close();

private:
	RInstanceImpl(RInstanceImpl&);					// unimplemented
	RInstanceImpl &operator=(const RInstanceImpl&);	// unimplemented

	SElement* iPtr;
	CRepositoryImpl* iRepository;
	};
}

#endif	// UNIQUEINSTANCEBASE_H_

