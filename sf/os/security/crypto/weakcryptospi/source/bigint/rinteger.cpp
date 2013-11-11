/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <bigint.h>
#include <e32std.h>
#include <random.h>
#include "../common/inlines.h"


/** 
 * Copy constructor
 *
 * This function performs a shallow copy, 
 * i.e. the memory holding the integer is not copied.
 */
EXPORT_C RInteger::RInteger(const RInteger& aInteger)
	{
	*this = aInteger;
	}

/** 
 * Assignment operator
 * 
 * This function performs a shallow copy, 
 * i.e. the memory holding the integer is not copied.
 */
EXPORT_C RInteger& RInteger::operator=(const RInteger& aInteger)
	{
	iSize = aInteger.iSize;
	iPtr = aInteger.iPtr;
	return *this;
	}


/** 
 * Creates a new integer representing 0.
 * 
 * @return	An RInteger by value.
 */
EXPORT_C RInteger RInteger::NewL(void)
	{
	return NewL(TInteger::Zero());
	}

/** 
 * Creates a new integer object representing a specified value.
 * 
 * @param aValue	A descriptor containing the big-endian binary
 * 					representation of the value.
 * @return			An RInteger object representing the value.
 */
EXPORT_C RInteger RInteger::NewL(const TDesC8& aValue)
	{
	RInteger self;
	//Construct zero's memory beyond the size of aValue after construction
	self.CreateNewL(BytesToWords(aValue.Size()));
	self.Construct(aValue);
	return self;
	}

/** 
 * Creates an exact copy of an \c aInteger object.
 * 
 * @param aInteger	The integer you wish to copy
 * @return			An RInteger object representing an exact copy of 
 *					aInteger by value.
 */
EXPORT_C RInteger RInteger::NewL(const TInteger& aInteger)
	{
	RInteger self;
	//don't need to CleanNewL as we'll copy straight from aInteger
	self.CreateNewL(aInteger.Size());
	self.Construct(aInteger);
	return self;
	}

/** 
 * Creates a random integer uniformly distributed over [0, 2^aBits].
 * 
 * @param aBits	The number of bits you wish to randomly select.
 * @param aAttr	Enum specifying whether specific bits in the random number should
 *				be set.  See TRandomAttribute for more information.
 * @return		A random RInteger object in the range specified.
 */
EXPORT_C RInteger RInteger::NewRandomL(TUint aBits, TRandomAttribute aAttr)
	{
	RInteger self;
	self.CleanNewL(BitsToWords(aBits));
	CleanupStack::PushL(self);
	self.RandomizeL(aBits, aAttr);
	CleanupStack::Pop(&self); 
	return self;
	}

/** 
 * Creates a random integer uniformly distributed over [x | min <= x <= max].
 * 
 * @param aMin	The smallest possible value for the random integer (inclusive).
 * @param aMax	The largest possible value for the random integer (inclusive).
 * @return		A random RInteger object in the range specified.
 */
EXPORT_C RInteger RInteger::NewRandomL(const TInteger& aMin,
	const TInteger& aMax)
	{
	RInteger self;
	self.CleanNewL(aMax.Size());
	CleanupStack::PushL(self);
	self.RandomizeL(aMin, aMax);
	CleanupStack::Pop(&self);
	return self;
	}

/** 
 * Finds a random prime integer in the range of [2, 2^aBits].
 *
 * This is done by picking a random integer and using that as a starting point
 * for a sequential search for a prime.  To verify the primality of number, 
 * this algorithm uses a probablistic primality test.  This means that it is
 * possible, although extremely improbable, that the number returned is a pseudoprime.
 *
 * @param aBits	The number of bits you wish to randomly select your prime from.
 * @param aAttr	Enum specifying whether specific bits in the random number should
 *				be set.  See TRandomAttribute for more information.
 * @return		A random RInteger representing a probable prime (with very high
 *				probablity) in the range specified.
 */
EXPORT_C RInteger RInteger::NewPrimeL(TUint aBits, TRandomAttribute aAttr)
	{
	RInteger self;
	self.CleanNewL(BitsToWords(aBits));
	CleanupStack::PushL(self);
	self.PrimeRandomizeL(aBits, aAttr);
	CleanupStack::Pop(&self);
	return self;
	}

/** 
 * Creates a new integer from the value represented by \c aInteger.
 * 
 * @param aInteger	A signed word sized integer.
 * @return			An RInteger representation of aInteger by value.
 */
EXPORT_C RInteger RInteger::NewL(TInt aInteger)
	{
	RInteger self;
	self.CreateNewL(2);
	self.Construct(aInteger);
	return self;
	}

/** 
 * Creates a new integer with a preallocated internal storage of \c aNumWords all
 * initialised to zero.
 *
 * The resulting RInteger object is logically equivalent to RInteger::NewL(0).
 * The only difference is that the internal storage requirements have been 
 * specified to be larger than the default. This is useful if you are about 
 * to perform an operation on this integer, that you know the resulting size
 * requirements of, and wish to avoid a heap resize.
 *
 * @param aNumWords	The number of words for to preallocated and zero fill.
 * @return			An RInteger object representing 0 with a preallocated 
 *					zero-filled internal storage of aNumWords.
 */
EXPORT_C RInteger RInteger::NewEmptyL(TUint aNumWords)
	{
	RInteger self;
	self.CleanNewL(aNumWords);
	//There's no construct as there isn't anything to do
	return self;
	}

/**
 * Creates an RInteger object with no associated internal (heap) storage.
 * 
 * All data members are initialised to zero.  It is safe (although not strictly necessary)
 * to push such an RInteger object onto the CleanupStack.  This is useful, for example, if
 * you want to pass an RInteger object by reference into a function and have it create
 * the representation of the actual integer for you.  
 *
 * Note that performing any operation on such an RInteger object other than the default
 * assignment operator or copy constructor will panic your code.
 * 
 * @return	A stack based class that has no associated internal storage and thus
 *			does not represent any number.
 */
EXPORT_C RInteger::RInteger(void)  
	{
	}

/** 
 * An overloaded TCleanupItem() allowing the RIntegers to be pushed,
 * popped, and destroyed via the CleanupStack like any other CBase derived object.
 */
EXPORT_C RInteger::operator TCleanupItem(void)
	{
	return TCleanupItem(&RInteger::CallClose, this);
	}

/** 
 * Helper function registered with the cleanup stack that just calls Close() for
 * this RInteger object.
 * 
 * @param aPtr	A pointer to the object for which clean-up is to be performed. 
 */
EXPORT_C void RInteger::CallClose(TAny* aPtr)
	{
	((RInteger*)aPtr)->Close();	
	}

/** 
 * Zeros and then frees any memory owned by this RInteger object.
 *  
 * An RInteger object that has been closed can safely fall off the stack.
 */
EXPORT_C void RInteger::Close(void)
	{
	if (iPtr)
		{
		Mem::FillZ(Ptr(), Size()*4);
		User::Free(Ptr());
		iSize = 0;
		iPtr = NULL;
		}
	}

// Method is excluded from coverage due to the problem with BullsEye on ONB.
// Manually verified that this method is functionally covered.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

/** 
 * Creates a new integer from the value represented by \c aInteger.
 *
 * @param aInteger	An unsigned word sized integer.
 * @return			An RInteger representation of aInteger by value.
 */
EXPORT_C RInteger RInteger::NewL(TUint aInteger)
	{
	RInteger self;
	self.CreateNewL(2);
	self.Construct(aInteger);
	return self;
	}
