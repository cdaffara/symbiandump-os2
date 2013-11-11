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
* ** IMPORTANT ** PublishedPartner API's in this file are published to 3rd party developers via the 
* Symbian website. Changes to these API's should be treated as PublishedAll API changes and the Security TA should be consulted.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <e32base.h>


const TUint KSignMask = 0x1L;
const TUint KHeapBasedMask = 0x2L;
const TUint KPtrMask = 0xFFFFFFFCL;

class RInteger;

/**
 * Abstract base class defining the interface for handling and manipulating big
 * integers.
 *
 * TInteger is capable of representing both negative and positive integers 
 * with an absolute value of less than 2^32^(2^32).  To create an integer 
 * look at RInteger.
 * TInteger defines an interface for the RInteger implementation - it is not
 * intended that TIntegers be copied or assigned from other TIntegers.  On EKA1
 * platforms, this is possible, but it should be avoided.
 * 
 * 
 * @see RInteger
 */
class TInteger
	{
public:
	/** @internalComponent */
	enum TRandomAttribute {EAllBitsRandom=0, ETopBitSet=1, ETop2BitsSet=2};

	IMPORT_C HBufC8* BufferLC() const;
	IMPORT_C TUint WordCount(void) const;
	IMPORT_C TUint ByteCount(void) const;
	IMPORT_C TUint BitCount(void) const;

	IMPORT_C static const TInteger& Zero(void);
	IMPORT_C static const TInteger& One(void);
	IMPORT_C static const TInteger& Two(void);

	IMPORT_C RInteger PlusL(const TInteger& aOperand) const;
	IMPORT_C RInteger MinusL(const TInteger& aOperand) const;
	IMPORT_C RInteger TimesL(const TInteger& aOperand) const;
	IMPORT_C RInteger DividedByL(const TInteger& aOperand) const;
	IMPORT_C RInteger DividedByL(TUint aOperand) const;
	IMPORT_C RInteger ModuloL(const TInteger& aOperand) const;
	IMPORT_C TUint ModuloL(TUint aOperand) const;
	
	IMPORT_C RInteger SquaredL(void) const;
	IMPORT_C RInteger ExponentiateL(const TInteger& aExponent) const;
	IMPORT_C static RInteger ModularMultiplyL(const TInteger& aA, const TInteger& aB,
		const TInteger& aModulus);
	IMPORT_C static RInteger ModularExponentiateL(const TInteger& aBase, 
		const TInteger& aExp, const TInteger& aMod);
	IMPORT_C RInteger GCDL(const TInteger& aOperand) const;
	IMPORT_C RInteger InverseModL(const TInteger& aMod) const;
	
	// These overloaded operator functions leave 
	IMPORT_C TInteger& operator += (const TInteger& aOperand);
	IMPORT_C TInteger& operator -= (const TInteger& aOperand);
	IMPORT_C TInteger& operator *= (const TInteger& aOperand);
	IMPORT_C TInteger& operator /= (const TInteger& aOperand);
	IMPORT_C TInteger& operator %= (const TInteger& aOperand);

	IMPORT_C TInteger& operator += (TInt aOperand);
	IMPORT_C TInteger& operator -= (TInt aOperand);
	IMPORT_C TInteger& operator *= (TInt aOperand);
	IMPORT_C TInteger& operator /= (TInt aOperand);
	IMPORT_C TInteger& operator %= (TInt aOperand);
	IMPORT_C TInteger& operator -- ();
	IMPORT_C TInteger& operator ++ ();

	IMPORT_C TInteger& operator <<= (TUint aBits);
	// End of leaving overloaded operator functions 
	IMPORT_C TInteger& operator >>= (TUint aBits);

	IMPORT_C TInt UnsignedCompare(const TInteger& aThat) const;
	IMPORT_C TInt SignedCompare(const TInteger& aThat) const;
	IMPORT_C TBool operator ! () const;
	inline TBool operator == (const TInteger& aInteger) const;
	inline TBool operator != (const TInteger& aInteger) const;
	inline TBool operator <= (const TInteger& aInteger) const;
	inline TBool operator >= (const TInteger& aInteger) const;
	inline TBool operator < (const TInteger& aInteger) const;
	inline TBool operator > (const TInteger& aInteger) const;
	
	IMPORT_C TInt SignedCompare(TInt aThat) const;
	inline TBool operator == (TInt aInteger) const;
	inline TBool operator != (TInt aInteger) const;
	inline TBool operator <= (TInt aInteger) const;
	inline TBool operator >= (TInt aInteger) const;
	inline TBool operator < (TInt aInteger) const;
	inline TBool operator > (TInt aInteger) const;

	inline TBool IsZero() const {return !*this;}
	inline TBool NotZero() const {return !IsZero();}
	inline TBool IsNegative() const {return Sign() == ENegative;}
	inline TBool NotNegative() const {return !IsNegative();}
	inline TBool IsPositive() const {return NotNegative() && NotZero();}
	inline TBool NotPositive() const {return !IsPositive();}
	inline TBool IsEven() const {return Bit(0) == EFalse;}
	inline TBool IsOdd() const {return Bit(0);}

	IMPORT_C TBool IsPrimeL(void) const;

	IMPORT_C TBool Bit(TUint aBitPos) const;
	IMPORT_C void SetBit(TUint aBitPos);
	IMPORT_C void Negate(void);

	IMPORT_C TInt ConvertToLongL(void) const;

	IMPORT_C void CopyL(const TInteger& aInteger, TBool aAllowShrink=ETrue);
	IMPORT_C void CopyL(const TInt aInteger, TBool aAllowShrink=ETrue);
	IMPORT_C void Set(const RInteger& aInteger);
	IMPORT_C HBufC8* BufferWithNoTruncationLC() const;

protected: //Interface functions to algorthims
	/** @internalComponent */
	RInteger PositiveAddL(const TInteger& aA, const TInteger& aB) const;
	/** @internalComponent */
	RInteger PositiveSubtractL(const TInteger& aA, const TInteger& aB) const;
	/** @internalComponent */
	RInteger PositiveMultiplyL(const TInteger& aA, const TInteger& aB) const;
	/** @internalComponent */
	void PositiveDivideL(RInteger& aRemainder, RInteger& aQuotient, 
		const TInteger& aDividend, const TInteger& aDivisor) const;
	/** @internalComponent */
	void DivideL(RInteger& aRemainder, RInteger& aQuotient, 
		const TInteger& aDividend, const TInteger& aDivisor) const;
	/** @internalComponent */
	void PositiveDivide(TUint& aRemainder, TInteger& aQoutient, 
		const TInteger& aDividend, TUint aDivisor) const;
	/** @internalComponent */
	void DivideL(TUint& aRemainder, RInteger& aQoutient, 
		const TInteger& aDividend, TUint aDivisor) const;
	/** @internalComponent */
	TUint Modulo(const TInteger& aDividend, TUint aDivisor) const;

protected: //Utility functions
	/** @internalComponent */
	TInt ConvertToLong(void) const;
	inline TUint ConvertToUnsignedLong(void) const {return Ptr()[0];}
	/** @internalComponent */
	TBool IsConvertableToLong(void) const;
	/** @internalComponent */
	void RandomizeL(TUint aBits, TRandomAttribute aAttr);
	/** @internalComponent */
	void RandomizeL(const TInteger& aMin, const TInteger& aMax);
	/** @internalComponent */
	void PrimeRandomizeL(TUint aBits, TRandomAttribute aAttr);
	/** @internalComponent */
	TBool SmallPrimeRandomizeL(void);

protected: //Memory Handling
	/** @internalComponent */
	void CreateNewL(TUint aNewSize);
	/** @internalComponent */
	void CleanNewL(TUint aNewSize);
	/** @internalComponent */
	void CleanGrowL(TUint aNewSize);
	/** @internalComponent */
	void CleanResizeL(TUint aNewSize);

protected: //Construction functions
	IMPORT_C TInteger(void);
	/** @internalComponent */
	void Construct(const TDesC8& aValue);
	/** @internalComponent */
	void Construct(const TInteger& aInteger);
	/** @internalComponent */
	void Construct(TInt aInteger);
	/** @internalComponent */
	void Construct(TUint aInteger);
protected: //Construction functions for stack based integers
	/** @internalComponent */
	void ConstructStack(TUint aWords, TUint aInteger);
	/** @internalComponent */
	void ConstructStack(TUint aWords, const TInteger& aInteger);


protected: //Member data
	enum TSign {EPositive=0, ENegative=1};
	TUint iSize;
	TUint iPtr;

protected: //Hackish functions
	/* Memory Layout
	 * Word 0: Size -- Must be power of 2 (algorithms assume this)
	 * Word 1: Pointer to storage location plus 2 flag bits in lsb
	 * Word 1 (bit 0): Sign of the integer (+ve == 0, -ve == 1)
	 * Word 1 (bit 1): Heap based flag bit (stack== 0, heap==1)
	 */
	inline TSign Sign(void) const {return (TSign)(iPtr&KSignMask);}
	inline TUint Size(void) const {return iSize;}
	inline void SetSize(TUint aSize) {iSize = aSize;}
	inline TUint* const Ptr(void) const {return (TUint*)(iPtr&KPtrMask);}
	inline TBool IsStackBased(void) const {return !IsHeapBased();}
	inline TBool IsHeapBased(void) const {return iPtr&KHeapBasedMask;}
	inline void SetPtr(TUint* aPtr) {iPtr &= ~KPtrMask; iPtr |= (TUint)aPtr;}
	inline void SetHeapBased(void) {iPtr |= KHeapBasedMask;}
	inline void SetStackBased(void) {iPtr &= ~KHeapBasedMask;}
	inline void SetSign(TSign aSign) {iPtr &= ~KSignMask; iPtr |= (TUint)aSign;}

private:
	// disable default copy constructor and assignment operator
	TInteger(const TInteger& aInteger);
	TInteger& operator=(const TInteger& aInteger);

	friend class CMontgomeryStructure;
	friend class RInteger; //in order to have access to Size() for an argument
	};

// Inline methods for TInteger

inline TBool TInteger::operator == (const TInteger& aInteger) const
	{
	return SignedCompare(aInteger) == 0;
	}

inline TBool TInteger::operator != (const TInteger& aInteger) const
	{
	return SignedCompare(aInteger) != 0;
	}

inline TBool TInteger::operator <= (const TInteger& aInteger) const
	{
	return SignedCompare(aInteger) <= 0;
	}

inline TBool TInteger::operator >= (const TInteger& aInteger) const
	{
	return SignedCompare(aInteger) >= 0;
	}

inline TBool TInteger::operator < (const TInteger& aInteger) const
	{
	return SignedCompare(aInteger) < 0;
	}

TBool TInteger::operator > (const TInteger& aInteger) const
	{
	return SignedCompare(aInteger) > 0;
	}

inline TBool TInteger::operator == (TInt aInteger) const
	{
	return SignedCompare(aInteger) == 0;
	}

inline TBool TInteger::operator != (TInt aInteger) const
	{
	return SignedCompare(aInteger) != 0;
	}

inline TBool TInteger::operator <= (TInt aInteger) const
	{
	return SignedCompare(aInteger) <= 0;
	}

inline TBool TInteger::operator >= (TInt aInteger) const
	{
	return SignedCompare(aInteger) >= 0;
	}

inline TBool TInteger::operator < (TInt aInteger) const
	{
	return SignedCompare(aInteger) < 0;
	}

inline TBool TInteger::operator > (TInt aInteger) const
	{
	return SignedCompare(aInteger) > 0;
	}


/** 
 * A TInteger derived class allowing the construction of variable length big integers.
 * See the Cryptography API guide for further information.
 *
 *
 * @see TInteger
 */
class RInteger : public TInteger
	{
public:
	IMPORT_C static RInteger NewL(void);
	IMPORT_C static RInteger NewL(const TDesC8& aValue);
	IMPORT_C static RInteger NewL(const TInteger& aInteger);
	IMPORT_C static RInteger NewL(TInt aInteger);
	IMPORT_C static RInteger NewL(TUint aInteger);
	IMPORT_C static RInteger NewEmptyL(TUint aNumWords);

	IMPORT_C static RInteger NewRandomL(TUint aBits, 
		TRandomAttribute aAttr=EAllBitsRandom);
	IMPORT_C static RInteger NewRandomL(const TInteger& aMin,
		const TInteger& aMax);
	IMPORT_C static RInteger NewPrimeL(TUint aBits, 
		TRandomAttribute aAttr=EAllBitsRandom);

	IMPORT_C RInteger(void);
	IMPORT_C RInteger(const RInteger& aInteger);
	IMPORT_C RInteger& operator=(const RInteger& aInteger);

	IMPORT_C operator TCleanupItem();
	IMPORT_C static void CallClose(TAny* aPtr);
	IMPORT_C void Close(void);
	};

#endif // __BIGINT_H__
