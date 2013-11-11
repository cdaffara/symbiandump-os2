// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BABITFLAGS_H__
#define __BABITFLAGS_H__

// System includes
#include <e32std.h>

//
// ----> TBitFlagsT (header)
//
template <class T>
class TBitFlagsT
/**
A simple class which manages the process of setting and clearing
flags in an abstract fashion.
@publishedAll
@released
*/
	{
//
public:										// CONSTRUCT
//

	/**
	 * Default constructor - initialize all flags to zero
	 */
	inline TBitFlagsT();

	/**
	 * Initialize the whole flag to a certain value
	 */
	inline TBitFlagsT(T aFlags);

	/**
	 * Copy constructor - initialize this flag object to mirror
	 * that of aFlags.
	 */
	inline TBitFlagsT(const TBitFlagsT& aFlags);

//
public:										// MANIPULATORS
//

	/**
	 * Set all the flags
	 */
	inline void								SetAll();

	/**
	 * Clear all the flags
	 */
	inline void								ClearAll();

	/**
	 * Set a particular flag
	 */
	inline void								Set(TInt aFlagIndex);

	/**
	 * Clear a particular flag
	 */
	inline void								Clear(TInt aFlagIndex);

	/**
	 * Set or clear a particular flag 
	 * 
	 * If aValue is 1, then the flag is set
	 * If aValue is 0, then the flag is cleared
	 */
	inline void								Assign(TInt aFlagIndex, TBool aValue);

	/**
	 * Change the state of a particular flag. If the flag at the specified
	 * index was clear, then it becomes set, otherwise it becomes clear.
	 */
	inline void								Toggle(TInt aFlagIndex);

//
public:										// OPERATORS
//

	/**
	 * Check if a particular flag is set or not
	 *
	 * @return A boolean indicating whether the specified flag is set or clear
	 */
	inline TBool							operator[](TInt aFlagIndex) const;

	/**
	 * Assignment operator - assign specific value to the whole flag, replacing
	 * any existing value.
	 */
	inline TBitFlagsT&						operator=(const TBitFlagsT& aFlags);

	/**
	 * Compare the value of the whole flag with a given value.
	 *
	 * @return A boolean indicating whether the two flags are identical.
	 */
	inline TBool							operator==(const TBitFlagsT& aFlags);

//
public:										// ACCESS
//

	/**
	 * Check if a particular flag is set
	 */
	inline TBool							IsSet(TInt aFlagIndex) const;

	/**
	 * Check if a particular flag is clear
	 */
	inline TBool							IsClear(TInt aFlagIndex) const;

	/**
	 * Access the underlying value of the flag.
	 */
	inline T								Value() const { return iFlags; }

	/**
	 * Assign a new value (directly) to this flag object. Replaces any
	 * existing individual flag settings.
	 */
	inline void								SetValue(T aFlags) { iFlags = aFlags; }

//
private:									// INTERNAL
//

	/**
	 * Generate a mask for a particular flag
	 */
	inline T								FlagMask(TInt aFlagIndex) const;

//
public:										// MEMBER DATA
//
	
	// The underlying object container which represents the flags.
	T										iFlags;
	};

/**
Type definitions
@publishedAll
@released
*/
typedef TBitFlagsT<TUint8> TBitFlags8;
//

/**
Type definitions
@publishedAll
@released
*/
typedef TBitFlagsT<TUint16> TBitFlags16;
//

/**
Type definitions
@publishedAll
@released
*/
typedef TBitFlagsT<TUint32>	TBitFlags32;
//


typedef TBitFlags32 TBitFlags;


//
// ----> TBitFlagsT (inlines)
//
template <class T>
inline TBitFlagsT<T>::TBitFlagsT() : iFlags(T(0)) 
	{}

template <class T>
inline TBitFlagsT<T>::TBitFlagsT(T aFlags) : iFlags(aFlags) 
	{}

template <class T>
inline TBitFlagsT<T>::TBitFlagsT(const TBitFlagsT<T>& aFlags) : iFlags(aFlags.iFlags) 
	{}

template <class T>
inline T TBitFlagsT<T>::FlagMask(TInt aFlagIndex) const
	{ return T(T(1)<<aFlagIndex); }

template <class T>
inline TBool TBitFlagsT<T>::IsSet(TInt aFlagIndex) const
	{ 
	// All out-of-range values should return  false
	if(aFlagIndex <= ((sizeof(T)<<3)-1) )
		{
		return iFlags & FlagMask(aFlagIndex);
		}
	else
		{
		return EFalse;
		}
	 
	}

template <class T>
inline TBool TBitFlagsT<T>::IsClear(TInt aFlagIndex) const
	{ return !IsSet(aFlagIndex); }

template <class T>
inline void TBitFlagsT<T>::Set(TInt aFlagIndex)
	{ iFlags = T(iFlags | FlagMask(aFlagIndex)); }

template <class T>
inline void TBitFlagsT<T>::Clear(TInt aFlagIndex)
	{ iFlags = T(iFlags & ~(FlagMask(aFlagIndex))); }

template <class T>
inline void TBitFlagsT<T>::Assign(TInt aFlagIndex, TBool aVal)
	{ if (aVal) Set(aFlagIndex); else Clear(aFlagIndex); }

template <class T>
inline void TBitFlagsT<T>::Toggle(TInt aFlagIndex)
	{ iFlags = T(iFlags^FlagMask(aFlagIndex)); }

template <class T>
inline TBool TBitFlagsT<T>::operator[](TInt aFlagIndex) const
	{ return IsSet(aFlagIndex); }

template <class T>
inline TBitFlagsT<T>& TBitFlagsT<T>::operator=(const TBitFlagsT<T>& aFlags)
	{ iFlags = aFlags.iFlags; return *this; }

template <class T>
inline TBool TBitFlagsT<T>::operator==(const TBitFlagsT<T>& aFlags)
	{ return iFlags == aFlags.Value(); }

template <class T>
inline void TBitFlagsT<T>::SetAll()
	{ iFlags = T(~(T(0))); }

template <class T>
inline void TBitFlagsT<T>::ClearAll()
	{ iFlags = T(0); }


#endif
