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


#ifndef UNIQUEINSTANCE_H_
#define UNIQUEINSTANCE_H_

#include "UniqueInstanceBase.h"

// A user may only create and destroy the repository. All access is through the
// RUniqueInstance class.
//
// Please note : Deleting the repository is unsafe unless all of its owned
// objects have been destroyed or relinquished. If objects exist in the repository
// (and provided that no RUniqueInstance objects have leaked) it follows that
// there must be RUniqueInstance objects around that still reference these objects.
// These RUniqueInstances will at some point try to access the repository to
// relinquish or destroy these objects, but it will have been deleted! Therefore
// the destructor panics if there are any objects in the repository.
//
// The compare function and copy function must co-operate like this:
// A) aCompare is a total order. That is, for all valid objects of type T t, u and v:
//	i)	aCompare(t, t) == 0									(idempotency)
//	ii)	if aCompare(t, u) < 0 then aCompare(u, t) > 0		(associativity)
//	iii)if aCompare(t, u) < 0 and aCompare(u, v) < 0 then aCompare(t, v) < 0
//															(transitivity)
// B) aCopyL always produces objects that compare equal to the arguments that
//		produced them, that is for all valid objects of type T t:
//	aCompare(t, aCopyL(t, sizeof(T))) == 0			(ignoring the memory leak!)
// If these conditions are not met, the behaviour of the repository is undefined.
// If the type does not own anything and does not have variable length, the
// copy and delete functions can be omitted.
//
// The operation of the repository and its clients is not thread safe.
//
// For aMaxLinks give half the log(base 2) of the expected maximum size of the
// repository. So for a repository of size 256, use 4. For size 65535 use 8. For
// 16777216 use 12.

/**
Store of objects that coalesces repeated instances.

@since App-frameworks6.1
@internalComponent
*/
template <typename T> class CUniqueInstanceRepository
	: public UniqueInstance::CUniqueInstanceRepositoryBase

	{
public:

	// typical implementation might be:
	//	void CompareT(const T* aL, const T* aR)
	//		{
	//		if (*aL == *aR)
	//			return 0;
	//		if (*aL < *aR)
	//			return -1;
	//		return 1;
	//		}
	typedef TInt TCompareFnType(const T*, const T*);

	// typical implementation might be:
	//	void DeleteT(T* aDestroy)
	//		{
	//		delete aDestroy;
	//		}
	typedef void TDeleteFnType(T* aObjectToBeDestroyed);

	// typical implementation might be:
	//	T* CopyT(const T* aSrc, TInt aOffset, TInt)
	//		{
	//		return new(ELeave) T(*aSrc);
	//		}
	typedef T* TCopyFnTypeL(const T* aTToBeCopied, TInt aSizeOfT);

	/**
	 * Creates a new empty repository. aCompare must define a total order on
	 * objects that may be added to the repository. In other words
	 * aCompare(a,b) == 0 iff a and b are equivalent, aCompare(a,b) < 0
	 * iff aCompare(a,b) > 0, aCompare(a,b) < 0 iff aCompare(b,a) < 0 and
	 * aCompare(a,b) < 0 && aCompare(b,c) < 0 implies that aCompare(a,c) < 0.
	 * aDelete(a) deletes a and aCopyL(a) generates a new object such that
	 * aCompare(aCopyL(a),a) == 0.
	 * aMaxLinks concerns the space against speed tradeoff of the
	 * implementation. It should be a number greater than half log2 of the
	 * expected maximum size of the repository.
	 */
	static inline CUniqueInstanceRepository<T>* NewL(TCompareFnType* aCompare,
		TDeleteFnType* aDelete, TCopyFnTypeL* aCopyL, TInt aMaxLinks = 10);
	static inline CUniqueInstanceRepository<T>* NewLC(TCompareFnType* aCompare,
		TDeleteFnType* aDelete, TCopyFnTypeL* aCopyL, TInt aMaxLinks = 10);
	/**
	 * Creates a new empty repository with aDelete(a) simply freeing memory
	 * at a and aCopyL(a) simply bitwise copying the memory occupied by a.
	 * This is suitable only if T has no destructor and only the automatic
	 * copy constructor.
	 */
	static inline CUniqueInstanceRepository<T>* NewL(TCompareFnType* aCompare,
		TInt aMaxLinks = 10);
	static inline CUniqueInstanceRepository<T>* NewLC(TCompareFnType* aCompare,
		TInt aMaxLinks = 10);
private:
	CUniqueInstanceRepository() {}
	};

/**
Specialization for the odd semantics of TDes. Copy/Delete/Compare functions
are provided free.

@since App-frameworks6.1
@internalComponent
*/
template <> class CUniqueInstanceRepository<TDes>
	: public UniqueInstance::CUniqueInstanceRepositoryBase

	{
	CUniqueInstanceRepository() {}
	static void* DesCopyL(void*, TInt);
	static TInt DesCompare(void*, void*);
	static void DesDelete(void*);
public:
	static inline CUniqueInstanceRepository<TDes>* NewL(TInt aMaxLinks = 10);
	static inline CUniqueInstanceRepository<TDes>* NewLC(TInt aMaxLinks = 10);
	};

/**
Client of a CUniqueInstanceRepository.
Holds a single object, which may be shared.

@since App-frameworks6.1
@internalComponent
*/
template <typename T> class RUniqueInstance

	{
public:
	inline explicit RUniqueInstance(CUniqueInstanceRepository<T>& aRepository)
		: iImpl(aRepository) {}
	/**
	 * Destructor. In debug builds this asserts that no object is owned.
	 */
	inline ~RUniqueInstance() {}

	/**
	 * Registers the argument as a unique instance, to be referenced by this
	 * RUniqueInstance. Any previously owned object is destroyed.
	 */
	inline void TakeL(T* aToAdd);
	/**
	 * Makes a copy of the argument, as a unique instance. The argument is
	 * still owned by the caller
	 */
	inline void TakeCopyL(const T* aToCopy);
	/**
	 * Returns a pointer to the referenced object without passing ownership
	 */
	inline const T* Peek() const;
	/**
	 * Makes another instance of the same object: both have ownership
	 */
	inline void CopyTo(RUniqueInstance<T>& aOther) const;
	/**
	 * Pass ownership of the owned object to another RUniqueInstance.
	 * This object may not be referenced through this RUniqueInstance any
	 * more, only through aOther.
	 */
	inline void MoveTo(RUniqueInstance<T>& aOther);
	/**
	 * Relinquishes ownership of the object to the caller. The object is
	 * not destroyed. It may not be referenced through this RUniqueInstance
	 * any more. The pointer returned may be different from that passed
	 * to TakeL() or returned from Peek()
	 */
	inline T* DropL();
	/**
	 * Releases the owned object.
	 */
	inline void Close();

private:
	UniqueInstance::RInstanceImpl iImpl;
	};

/**
Specialization of RUniqueInstance for the odd semantics of TDes.
Any descriptor that needs ownership to be passed around is an HBufC.
Any time a copy or view over a descriptor is required, a const TDesC
is used. Please note the different signature of NewL and NewLC. This is
because copy, delete and compare functions are provided for free.

@since App-frameworks6.1
@internalComponent
*/
TEMPLATE_SPECIALIZATION class RUniqueInstance<TDes>

	{
public:
	inline explicit RUniqueInstance(CUniqueInstanceRepository<TDes>& aRepository)
		: iImpl(aRepository) {}
	inline ~RUniqueInstance() {}
	inline void TakeL(HBufC* aToAdd);
	inline void TakeCopyL(const TDesC* aToCopy);
	inline const TDesC* Peek() const;
	inline void CopyTo(RUniqueInstance<TDes>& aOther) const;
	inline void MoveTo(RUniqueInstance<TDes>& aOther);
	inline HBufC* DropL();
	inline void Close();

private:
	UniqueInstance::RInstanceImpl iImpl;
	};

template <typename T>
inline CUniqueInstanceRepository<T>* CUniqueInstanceRepository<T>::NewLC(
	typename CUniqueInstanceRepository<T>::TCompareFnType* aCompare,
	typename CUniqueInstanceRepository<T>::TDeleteFnType* aDelete,
	typename CUniqueInstanceRepository<T>::TCopyFnTypeL* aCopyL, TInt aMaxLinks)
	{
	CUniqueInstanceRepository<T>* that = new(ELeave) CUniqueInstanceRepository<T>;
	CleanupStack::PushL(that);
	that->ConstructL(reinterpret_cast<UniqueInstance::TCompareFn*>(aCompare),
		reinterpret_cast<UniqueInstance::TDeleteFn*>(aDelete),
		reinterpret_cast<UniqueInstance::TCopyFnL*>(aCopyL), aMaxLinks,
		sizeof(T));
	return that;
	}

template <typename T>
inline CUniqueInstanceRepository<T>* CUniqueInstanceRepository<T>::NewL(
	typename CUniqueInstanceRepository<T>::TCompareFnType* aComp,
	typename CUniqueInstanceRepository<T>::TDeleteFnType* aDel,
	typename CUniqueInstanceRepository<T>::TCopyFnTypeL* aCopyL, TInt aMaxLinks)
	{
	CUniqueInstanceRepository<T>* that = NewLC(aComp, aDel, aCopyL, aMaxLinks);
	CleanupStack::Pop(that);
	return that;
	}

template <typename T>
inline CUniqueInstanceRepository<T>* CUniqueInstanceRepository<T>::NewLC(
	typename CUniqueInstanceRepository<T>::TCompareFnType* aCompare, TInt aMaxLinks)
	{
	CUniqueInstanceRepository<T>* that = new(ELeave) CUniqueInstanceRepository<T>;
	CleanupStack::PushL(that);
	that->ConstructL(reinterpret_cast<UniqueInstance::TCompareFn*>(aCompare),
		DumbDelete, DumbCopyL, aMaxLinks,
		sizeof(T));
	return that;
	}

template <typename T>
inline CUniqueInstanceRepository<T>* CUniqueInstanceRepository<T>::NewL(
	typename CUniqueInstanceRepository<T>::TCompareFnType* aComp, TInt aMaxLinks)
	{
	CUniqueInstanceRepository<T>* that = NewLC(aComp, aMaxLinks);
	CleanupStack::Pop(that);
	return that;
	}

inline CUniqueInstanceRepository<TDes>* CUniqueInstanceRepository<TDes>::NewLC(
	TInt aMaxLinks)
	{
	CUniqueInstanceRepository<TDes>* that = new(ELeave) CUniqueInstanceRepository<TDes>;
	CleanupStack::PushL(that);
	that->ConstructL(DesCompare, DesDelete, DesCopyL, aMaxLinks, 1);
	return that;
	}

inline CUniqueInstanceRepository<TDes>* CUniqueInstanceRepository<TDes>::NewL(
	TInt aMaxLinks)
	{
	CUniqueInstanceRepository<TDes>* that = NewLC(aMaxLinks);
	CleanupStack::Pop(that);
	return that;
	}

///////////////////////////////
//							 //
//	RUniqueInstance inlines  //
//							 //
///////////////////////////////

template <typename T> inline
void RUniqueInstance<T>::TakeL(T* aToAdd)
	{ iImpl.TakeL(aToAdd); }
template <typename T> inline
void RUniqueInstance<T>::TakeCopyL(const T* aToCopy)
	{ iImpl.TakeCopyL(const_cast<T*>(aToCopy)); }
template <typename T> inline
const T* RUniqueInstance<T>::Peek() const
	{ return reinterpret_cast<const T*>(iImpl.Peek()); }
template <typename T> inline
void RUniqueInstance<T>::CopyTo(RUniqueInstance<T>& aOther) const
	{ iImpl.CopyTo(aOther.iImpl); }
template <typename T> inline
void RUniqueInstance<T>::MoveTo(RUniqueInstance<T>& aOther)
	{ iImpl.MoveTo(aOther.iImpl); }
template <typename T> inline
T* RUniqueInstance<T>::DropL()
	{ return reinterpret_cast<T*>(iImpl.DropL()); }
template <typename T> inline
void RUniqueInstance<T>::Close()
	{ iImpl.Close(); }

inline void RUniqueInstance<TDes>::TakeL(HBufC* aToAdd)
	{ iImpl.TakeL(aToAdd); }
inline void RUniqueInstance<TDes>::TakeCopyL(const TDesC* aToCopy)
	{ iImpl.TakeCopyL(const_cast<TDesC*>(aToCopy)); }
inline const TDesC* RUniqueInstance<TDes>::Peek() const
	{ return reinterpret_cast<HBufC*>(iImpl.Peek()); }
inline void RUniqueInstance<TDes>::CopyTo(RUniqueInstance<TDes>& aOther) const
	{ iImpl.CopyTo(aOther.iImpl); }
inline void RUniqueInstance<TDes>::MoveTo(RUniqueInstance<TDes>& aOther)
	{ iImpl.MoveTo(aOther.iImpl); }
inline HBufC* RUniqueInstance<TDes>::DropL()
	{ return reinterpret_cast<HBufC*>(iImpl.DropL()); }
inline void RUniqueInstance<TDes>::Close()
	{ iImpl.Close(); }

#endif	// UNIQUEINSTANCE_H_
