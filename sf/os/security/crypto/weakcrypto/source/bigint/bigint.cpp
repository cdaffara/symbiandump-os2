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
#include <euserext.h>
#include <securityerr.h>
#include "words.h"
#include "algorithms.h"
#include "windowslider.h"
#include "stackinteger.h"
#include "mont.h"


/**
* Creates a new buffer containing the big-endian binary representation of this
* integer.
*
* Note that it does not support the exporting of negative integers.
*
* @return	The new buffer.
* 
* @leave KErrNegativeExportNotSupported	If this instance is a negative integer.
*
*/
EXPORT_C HBufC8* TInteger::BufferLC() const
	{
	if(IsNegative())
		{
		User::Leave(KErrNegativeExportNotSupported);
		}
	TUint bytes = ByteCount();
	HBufC8* buf = HBufC8::NewMaxLC(bytes);
	TUint8* bufPtr = (TUint8*)(buf->Ptr());
	TUint8* regPtr = (TUint8*)Ptr();

	// we internally store the number little endian, as a string we want it big
	// endian
	for(TUint i=0,j=bytes-1; i<bytes; )
		{
		bufPtr[i++] = regPtr[j--];
		}
	return buf;
	}

EXPORT_C HBufC8* TInteger::BufferWithNoTruncationLC() const
 	{
 	if(IsNegative())
 		{
 		User::Leave(KErrNegativeExportNotSupported);
 		}
 	
 	TUint wordCount = Size();
 	TUint bytes = (wordCount)*WORD_SIZE;
     
  	HBufC8* buf = HBufC8::NewMaxLC(bytes);
 	TUint8* bufPtr = (TUint8*)(buf->Ptr());
	TUint8* regPtr = (TUint8*)Ptr();
	for(TUint i=0,j=bytes-1; i<bytes; )
 		{
 		bufPtr[i++] = regPtr[j--];
 		}
  
	return buf;
	}

/** 
* Gets the number of words required to represent this RInteger.
* 
* @return	The size of the integer in words.
*
*/
EXPORT_C TUint TInteger::WordCount() const
	{
	return CountWords(Ptr(), Size());
	}

/**
* Gets the number of bytes required to represent this RInteger.
* 
* @return	The size of the integer in bytes.
* 
*/
EXPORT_C TUint TInteger::ByteCount() const
	{
	TUint wordCount = WordCount();
	if(wordCount)
		{
		return (wordCount-1)*WORD_SIZE + BytePrecision((Ptr())[wordCount-1]);
		}
	else 
		{
		return 0;
		}
	}

/** 
* Get the number of bits required to represent this RInteger.
* 
* @return	The size of the integer in bits.
* 
*/
EXPORT_C TUint TInteger::BitCount() const
	{
	TUint wordCount = WordCount();
	if(wordCount)
		{
		return (wordCount-1)*WORD_BITS + BitPrecision(Ptr()[wordCount-1]);
		}
	else 
		{
		return 0;
		}
	}


//These 3 declarations instantiate a constant 0, 1, 2 for ease of use and
//quick construction elsewhere in the code.  Note that the functions
//returning references to this static data return const references as you can't
//modify the ROM ;)
//word 0: Size of storage in words
//word 1: Pointer to storage
//word 2: LSW of storage
//word 3: MSW of storage
//Note that the flag bits in word 1 (Ptr()) are zero in the case of a positive
//stack based integer (SignBit == 0, IsHeapBasedBit == 0)
const TUint KBigintZero[4] = {2, (TUint)(KBigintZero+2), 0, 0};
const TUint KBigintOne[4] = {2, (TUint)(KBigintOne+2), 1, 0};
const TUint KBigintTwo[4] = {2, (TUint)(KBigintTwo+2), 2, 0};

/** 
 * Gets the TInteger that represents zero
 *
 * @return	The TInteger representing zero
 */
EXPORT_C const TInteger& TInteger::Zero(void)
	{
	return *reinterpret_cast<const TStackInteger64*>(KBigintZero);
	}

/** 
 * Gets the TInteger that represents one
 *
 * @return	The TInteger representing one
 */
EXPORT_C const TInteger& TInteger::One(void)
	{
	return *reinterpret_cast<const TStackInteger64*>(KBigintOne);
	}
	
/** 
 * Gets the TInteger that represents two
 *
 * @return	The TInteger representing two
 */
EXPORT_C const TInteger& TInteger::Two(void)
	{
	return *reinterpret_cast<const TStackInteger64*>(KBigintTwo);
	}

EXPORT_C RInteger TInteger::PlusL(const TInteger& aOperand) const
	{
	RInteger sum;
    if (NotNegative())
		{
        if (aOperand.NotNegative())
            sum = PositiveAddL(*this, aOperand);
        else
            sum = PositiveSubtractL(*this, aOperand);
		}
    else
		{
        if (aOperand.NotNegative())
            sum = PositiveSubtractL(aOperand, *this);
        else
			{
            sum = PositiveAddL(*this, aOperand);
			sum.SetSign(TInteger::ENegative);
			}
		}
	return sum;
	}

EXPORT_C RInteger TInteger::MinusL(const TInteger& aOperand) const
	{
	RInteger diff;
    if (NotNegative())
		{
        if (aOperand.NotNegative())
            diff = PositiveSubtractL(*this, aOperand);
        else
            diff = PositiveAddL(*this, aOperand);
		}
    else
		{
        if (aOperand.NotNegative())
			{
            diff = PositiveAddL(*this, aOperand);
			diff.SetSign(TInteger::ENegative);
			}
        else
            diff = PositiveSubtractL(aOperand, *this);
		}
	return diff;
	}

EXPORT_C RInteger TInteger::TimesL(const TInteger& aOperand) const
	{
	RInteger product = PositiveMultiplyL(*this, aOperand);

	if (NotNegative() != aOperand.NotNegative())
		{
		product.Negate();
		}
	return product;
	}

EXPORT_C RInteger TInteger::DividedByL(const TInteger& aOperand) const
	{
	RInteger quotient;
	RInteger remainder;
	DivideL(remainder, quotient, *this, aOperand);
	remainder.Close();
	return quotient;
	}

EXPORT_C RInteger TInteger::DividedByL(TUint aOperand) const
	{
	TUint remainder;
	RInteger quotient;
	DivideL(remainder, quotient, *this, aOperand);
	return quotient;
	}

EXPORT_C RInteger TInteger::ModuloL(const TInteger& aOperand) const
	{
	RInteger remainder;
	RInteger quotient;
	DivideL(remainder, quotient, *this, aOperand);
	quotient.Close();
	return remainder;
	}

EXPORT_C TUint TInteger::ModuloL(TUint aOperand) const
	{
	if(!aOperand)
		{
		User::Leave(KErrDivideByZero);
		}
	return Modulo(*this, aOperand);
	}

EXPORT_C RInteger TInteger::SquaredL() const
	{
	//PositiveMultiplyL optimises for the squaring case already
	//Any number squared is positive, no need for negative handling in TimesL
	return PositiveMultiplyL(*this, *this);
	}

EXPORT_C RInteger TInteger::ExponentiateL(const TInteger& aExponent) const
	{
	//See HAC 14.85

	// 1.1 Precomputation
	// g1 <- g
	// g2 <- g^2
	RInteger g2 = SquaredL();
	CleanupStack::PushL(g2);
	RInteger g1 = RInteger::NewL(*this);
	CleanupStack::PushL(g1);
	TWindowSlider slider(aExponent);

	// 1.2 
	// For i from 1 to (2^(k-1) -1) do g2i+1 <- g2i-1 * g2
	TUint count = (1 << (slider.WindowSize()-1)) - 1; //2^(k-1) -1
	RRArray<RInteger> powerArray(count+1); //+1 because we append g1
	User::LeaveIfError(powerArray.Append(g1));
	CleanupStack::Pop(); //g1
	CleanupClosePushL(powerArray);
	for(TUint k=1; k <= count; k++)
		{
		RInteger g2iplus1 = g2.TimesL(powerArray[k-1]);
		//This append can't fail as the granularity is set high enough
		//plus we've already called Append once which will alloc to the 
		//set granularity
		powerArray.Append(g2iplus1);
		}

	// 2 A <- 1, i <- t
	RInteger A = RInteger::NewL(One());
	CleanupStack::PushL(A);
	TInt i = aExponent.BitCount() - 1;

	// 3 While i>=0 do:
	while( i>=0 )
		{
		// 3.1 If ei == 0 then A <- A^2
		if(!aExponent.Bit(i))
			{
			A *= A;
			i--;
			}
		// 3.2 Find longest bitstring ei,ei-1,...,el s.t. i-l+1<=k and el==1
		// and do:
		// A <- (A^2^(i-l+1)) * g[the index indicated by the bitstring value]
		else
			{
			slider.FindNextWindow(i);
			assert(slider.Length() >= 1);
			for(TUint j=0; j<slider.Length(); j++)
				{
				A *= A;
				}
			A *= powerArray[slider.Value()>>1];
			i -= slider.Length();
			}
		}
	CleanupStack::Pop(&A);
	CleanupStack::PopAndDestroy(2, &g2); //powerArray, g2
	return A;
	}

EXPORT_C RInteger TInteger::ModularMultiplyL(const TInteger& aA, const TInteger& aB,
	const TInteger& aMod) 
	{
	RInteger product = aA.TimesL(aB);
	CleanupStack::PushL(product);
	RInteger reduced = product.ModuloL(aMod);
	CleanupStack::PopAndDestroy(&product); 
	return reduced;
	}

EXPORT_C RInteger TInteger::ModularExponentiateL(const TInteger& aBase, 
	const TInteger& aExp, const TInteger& aMod) 
	{
	CMontgomeryStructure* mont = CMontgomeryStructure::NewLC(aMod);
	RInteger result = RInteger::NewL(mont->ExponentiateL(aBase, aExp));
	CleanupStack::PopAndDestroy(mont);
	return result;
	}

EXPORT_C RInteger TInteger::GCDL(const TInteger& aOperand) const
	{
	//Binary GCD algorithm -- see HAC 14.4.1
	//with a slight variation -- our g counts shifts rather than actually
	//shifting.  We then do one shift at the end.
	assert(NotNegative());
	assert(aOperand.NotNegative());

	RInteger x = RInteger::NewL(*this);
	CleanupStack::PushL(x);
	RInteger y = RInteger::NewL(aOperand);
	CleanupStack::PushL(y);

	// 1 Ensure x >= y
	if( x < y )
		{
		TClassSwap(x, y);
		}

	TUint g = 0;
	// 2 while x and y even x <- x/2, y <- y/2
	while( x.IsEven() && y.IsEven() )
		{
		x >>= 1;
		y >>= 1;
		++g;
		}
	// 3 while x != 0
	while( x.NotZero() )
		{
		// 3.1 while x even x <- x/2
		while( x.IsEven() )
			{
			x >>= 1;
			}
		// 3.2 while y even y <- y/2
		while( y.IsEven() )
			{
			y >>= 1;
			}
		// 3.3 t <- abs(x-y)/2
		RInteger t = x.MinusL(y);
		t >>= 1;
		t.SetSign(TInteger::EPositive);

		// 3.4 If x>=y then x <- t else y <- t
		if( x >= y )
			{
			x.Set(t);
			}
		else 
			{
			y.Set(t);
			}
		}
	
	// 4 Return (g*y) (equiv to y<<=g as our g was counting shifts not actually
	//shifting)
	y <<= g;
	CleanupStack::Pop(&y);
	CleanupStack::PopAndDestroy(&x); 
	return y;
	}

EXPORT_C RInteger TInteger::InverseModL(const TInteger& aMod) const
	{
	assert(aMod.NotNegative());

	RInteger result;
	if(IsNegative() || *this>=aMod)
		{
		RInteger temp = ModuloL(aMod);
		CleanupClosePushL(temp);
		result = temp.InverseModL(aMod);
		CleanupStack::PopAndDestroy(&temp);
		return result;
		}

	if(aMod.IsEven())
		{
		if( !aMod || IsEven() )
			{
			return RInteger::NewL(Zero());
			}
		if( *this == One() )
			{
			return RInteger::NewL(One());
			}
		RInteger u = aMod.InverseModL(*this); 
		CleanupClosePushL(u);
		if(!u)
			{
			result = RInteger::NewL(Zero());
			}
		else 
			{
			//calculates (aMod*(*this-u)+1)/(*this) 
			result = MinusL(u);
			CleanupClosePushL(result);
			result *= aMod;
			++result;
			result /= *this;
			CleanupStack::Pop(&result); 
			}
		CleanupStack::PopAndDestroy(&u);
		return result;
		}

	result = RInteger::NewEmptyL(aMod.Size());
	CleanupClosePushL(result);
	RInteger workspace = RInteger::NewEmptyL(aMod.Size() * 4);
	TUint k = AlmostInverse(result.Ptr(), workspace.Ptr(), Ptr(), Size(),
		aMod.Ptr(), aMod.Size());
	DivideByPower2Mod(result.Ptr(), result.Ptr(), k, aMod.Ptr(), aMod.Size());
	workspace.Close();
	CleanupStack::Pop(&result);

	return result;
	}

EXPORT_C TInteger& TInteger::operator+=(const TInteger& aOperand)
	{
	this->Set(PlusL(aOperand));
    return *this;
	}

EXPORT_C TInteger& TInteger::operator-=(const TInteger& aOperand)
	{
	this->Set(MinusL(aOperand));
    return *this;
	}

EXPORT_C TInteger& TInteger::operator*=(const TInteger& aOperand)
	{
	this->Set(TimesL(aOperand));
	return *this;
	}

EXPORT_C TInteger& TInteger::operator/=(const TInteger& aOperand)
	{
	this->Set(DividedByL(aOperand));
	return *this;
	}

EXPORT_C TInteger& TInteger::operator%=(const TInteger& aOperand)
	{
	this->Set(ModuloL(aOperand));
	return *this;
	}

EXPORT_C TInteger& TInteger::operator+=(TInt aOperand)
	{
	TStackInteger64 operand(aOperand);
	*this += operand;
	return *this;
	}

EXPORT_C TInteger& TInteger::operator-=(TInt aOperand)
	{
	TStackInteger64 operand(aOperand);
	*this -= operand;
	return *this;
	}

EXPORT_C TInteger& TInteger::operator*=(TInt aOperand)
	{
	TStackInteger64 operand(aOperand);
	*this *= operand;
	return *this;
	}

EXPORT_C TInteger& TInteger::operator/=(TInt aOperand)
	{
	TStackInteger64 operand(aOperand);
	*this /= operand;
	return *this;
	}

EXPORT_C TInteger& TInteger::operator%=(TInt aOperand)
	{
	TStackInteger64 operand(aOperand);
	assert(operand.NotNegative());
	*this %= operand;
	return *this;
	}

EXPORT_C TInteger& TInteger::operator--()
	{
    if (IsNegative())
		{
        if (Increment(Ptr(), Size()))
			{
            CleanGrowL(2*Size());
            (Ptr())[Size()/2]=1;
			}
		}
    else
		{
        if (Decrement(Ptr(), Size()))
			{
			this->CopyL(-1);
			}
		}
    return *this;	
	}

EXPORT_C TInteger& TInteger::operator++()
	{
	if(NotNegative())
		{
		if(Increment(Ptr(), Size()))
			{
			CleanGrowL(2*Size());
			(Ptr())[Size()/2]=1;
			}
		}
	else
		{
		DecrementNoCarry(Ptr(), Size());
		if(WordCount()==0)
			{
			this->CopyL(Zero());
			}
		}
	return *this;
	}

EXPORT_C TInteger& TInteger::operator <<=(TUint aBits)
	{
	const TUint wordCount = WordCount();
	const TUint shiftWords = aBits / WORD_BITS;
	const TUint shiftBits = aBits % WORD_BITS;

	CleanGrowL(wordCount+BitsToWords(aBits));
	ShiftWordsLeftByWords(Ptr(), wordCount + shiftWords, shiftWords);
	ShiftWordsLeftByBits(Ptr()+shiftWords, wordCount + BitsToWords(shiftBits), 
		shiftBits);
	return *this;
	}

EXPORT_C TInteger& TInteger::operator >>=(TUint aBits)
	{
	const TUint wordCount = WordCount();
	const TUint shiftWords = aBits / WORD_BITS;
	const TUint shiftBits = aBits % WORD_BITS;

	ShiftWordsRightByWords(Ptr(), wordCount, shiftWords);
	if(wordCount > shiftWords)
		{
		ShiftWordsRightByBits(Ptr(), wordCount - shiftWords, shiftBits);
		}
	if(IsNegative() && WordCount()==0) // avoid negative 0
		{
		SetSign(EPositive);
		}
	return *this;
	}

EXPORT_C TInt TInteger::UnsignedCompare(const TInteger& aThat) const
	{
	TUint size = WordCount();
	TUint thatSize = aThat.WordCount();

	if( size == thatSize )
		return Compare(Ptr(), aThat.Ptr(), size);
	else
		return size > thatSize ? 1 : -1;
	}

EXPORT_C TInt TInteger::SignedCompare(const TInteger& aThat) const
	{
    if (NotNegative())
		{
        if (aThat.NotNegative())
            return UnsignedCompare(aThat);
        else
            return 1;
		}
    else
		{
        if (aThat.NotNegative())
            return -1;
        else
            return -UnsignedCompare(aThat);
		}
	}

EXPORT_C TBool TInteger::operator!() const
	{
	//Ptr()[0] is just a quick way of weeding out non-zero numbers without
	//doing a full WordCount() == 0.  Very good odds that a non-zero number
	//will have a bit set in the least significant word
	return IsNegative() ? EFalse : (Ptr()[0]==0 && WordCount()==0);
	}

EXPORT_C TInt TInteger::SignedCompare(TInt aInteger) const
	{
	TStackInteger64 temp(aInteger);
	return SignedCompare(temp);
	}

/* TBool IsPrimeL(void) const 
 * and all primality related functions are implemented in primes.cpp */

EXPORT_C TBool TInteger::Bit(TUint aBitPos) const
	{
	if( aBitPos/WORD_BITS >= Size() )
		{
		return 0;
		}
	else 
		{
		return (((Ptr())[aBitPos/WORD_BITS] >> (aBitPos % WORD_BITS)) & 1);
		}
	}

EXPORT_C void TInteger::SetBit(TUint aBitPos) 
	{
	if( aBitPos/WORD_BITS < Size() )
		{
		ArraySetBit(Ptr(), aBitPos);
		}
	}

EXPORT_C void TInteger::Negate() 
	{
	if(!!(*this)) //don't flip sign if *this==0
		{
		SetSign(TSign((~Sign())&KSignMask));
		}
	}

EXPORT_C TInt TInteger::ConvertToLongL(void) const
	{
	if(!IsConvertableToLong())
		{
		User::Leave(KErrTotalLossOfPrecision);
		}
	return ConvertToLong();
	}

EXPORT_C void TInteger::CopyL(const TInteger& aInteger, TBool aAllowShrink)
	{
	if(aAllowShrink)
		{
		CleanResizeL(aInteger.Size());
		}
	else
		{
		CleanGrowL(aInteger.Size());
		}
	Construct(aInteger);
	}

EXPORT_C void TInteger::CopyL(TInt aInteger, TBool aAllowShrink)
	{
	if(aAllowShrink)
		{
		CleanResizeL(2);
		}
	else
		{
		CleanGrowL(2);
		}
	Construct(aInteger);
	}

EXPORT_C void TInteger::Set(const RInteger& aInteger)
	{
	assert(IsHeapBased());
	Mem::FillZ(Ptr(), WordsToBytes(Size()));
	User::Free(Ptr());
	iPtr = aInteger.iPtr;
	iSize = aInteger.iSize;
	}

RInteger TInteger::PositiveAddL(const TInteger &aA, const TInteger& aB) const
	{
	RInteger sum = RInteger::NewEmptyL(CryptoMax(aA.Size(), aB.Size()));
	const word aSize = aA.Size();
	const word bSize = aB.Size();
	const word* const aReg = aA.Ptr();
	const word* const bReg = aB.Ptr();
	word* const sumReg = sum.Ptr();

	word carry;
	if (aSize == bSize)
		carry = Add(sumReg, aReg, bReg, aSize);
	else if (aSize > bSize)
		{
		carry = Add(sumReg, aReg, bReg, bSize);
		CopyWords(sumReg+bSize, aReg+bSize, aSize-bSize);
		carry = Increment(sumReg+bSize, aSize-bSize, carry);
		}
	else
		{
		carry = Add(sumReg, aReg, bReg, aSize);
		CopyWords(sumReg+aSize, bReg+aSize, bSize-aSize);
		carry = Increment(sumReg+aSize, bSize-aSize, carry);
		}

	if (carry)
		{
		CleanupStack::PushL(sum);
		sum.CleanGrowL(2*sum.Size());
		CleanupStack::Pop(&sum);
		sum.Ptr()[sum.Size()/2] = 1;
		}
	sum.SetSign(TInteger::EPositive);
	return sum;
	}

RInteger TInteger::PositiveSubtractL(const TInteger &aA, const TInteger& aB) const
	{
	RInteger diff = RInteger::NewEmptyL(CryptoMax(aA.Size(), aB.Size()));
	unsigned aSize = aA.WordCount();
	aSize += aSize%2;
	unsigned bSize = aB.WordCount();
	bSize += bSize%2;
	const word* const aReg = aA.Ptr();
	const word* const bReg = aB.Ptr();
	word* const diffReg = diff.Ptr();

	if (aSize == bSize)
		{
		if (Compare(aReg, bReg, aSize) >= 0)
			{
			Subtract(diffReg, aReg, bReg, aSize);
			diff.SetSign(TInteger::EPositive);
			}
		else
			{
			Subtract(diffReg, bReg, aReg, aSize);
			diff.SetSign(TInteger::ENegative);
			}
		}
	else if (aSize > bSize)
		{
		word borrow = Subtract(diffReg, aReg, bReg, bSize);
		CopyWords(diffReg+bSize, aReg+bSize, aSize-bSize);
		borrow = Decrement(diffReg+bSize, aSize-bSize, borrow);
		assert(!borrow);
		diff.SetSign(TInteger::EPositive);
		}
	else
		{
		word borrow = Subtract(diffReg, bReg, aReg, aSize);
		CopyWords(diffReg+aSize, bReg+aSize, bSize-aSize);
		borrow = Decrement(diffReg+aSize, bSize-aSize, borrow);
		assert(!borrow);
		diff.SetSign(TInteger::ENegative);
		}
	return diff;
	}

RInteger TInteger::PositiveMultiplyL(const TInteger &aA, const TInteger &aB) const
	{
	unsigned aSize = RoundupSize(aA.WordCount());
	unsigned bSize = RoundupSize(aB.WordCount());

	RInteger product = RInteger::NewEmptyL(aSize+bSize);
	CleanupClosePushL(product);

	RInteger workspace = RInteger::NewEmptyL(aSize + bSize);
	AsymmetricMultiply(product.Ptr(), workspace.Ptr(), aA.Ptr(), aSize, aB.Ptr(), 
		bSize);
	workspace.Close();
	CleanupStack::Pop(&product);
	return product;
	}

TUint TInteger::Modulo(const TInteger& aDividend, TUint aDivisor) const
	{
	assert(aDivisor);
	TUint i = aDividend.WordCount();
	TUint remainder = 0;
	while(i--)
		{
		remainder = TUint(MAKE_DWORD(aDividend.Ptr()[i], remainder) % aDivisor);
		}
	return remainder;
	}

void TInteger::PositiveDivide(TUint& aRemainder, TInteger& aQuotient, 
	const TInteger& aDividend, TUint aDivisor) const
	{
	assert(aDivisor);

	TUint i = aDividend.WordCount();
	assert(aQuotient.Size() >= RoundupSize(i));
	assert(aQuotient.Sign() == TInteger::EPositive);
	aRemainder = 0;
	while(i--)
		{
		aQuotient.Ptr()[i] = 
			TUint(MAKE_DWORD(aDividend.Ptr()[i], aRemainder) / aDivisor);
		aRemainder = 
			TUint(MAKE_DWORD(aDividend.Ptr()[i], aRemainder) % aDivisor);
		}
	}

void TInteger::DivideL(TUint& aRemainder, RInteger& aQuotient,
	const TInteger& aDividend, TUint aDivisor) const
	{
	if(!aDivisor)
		{
		User::Leave(KErrDivideByZero);
		}
	
	TUint i = aDividend.WordCount();
	aQuotient.CleanNewL(RoundupSize(i));
	PositiveDivide(aRemainder, aQuotient, aDividend, aDivisor);

	if(aDividend.NotNegative())
		{
		aQuotient.SetSign(TInteger::EPositive);
		}
	else
		{
		aQuotient.SetSign(TInteger::ENegative);
		if(aRemainder)
			{
			--aQuotient;
			aRemainder = aDivisor = aRemainder;
			}
		}
	}

void TInteger::PositiveDivideL(RInteger &aRemainder, RInteger &aQuotient,
	const TInteger &aDividend, const TInteger &aDivisor) const
	{
	unsigned dividendSize = aDividend.WordCount();
	unsigned divisorSize = aDivisor.WordCount();

	if (!divisorSize)
		{
		User::Leave(KErrDivideByZero);
		}

	if (aDividend.UnsignedCompare(aDivisor) == -1)
		{
		aRemainder.CreateNewL(aDividend.Size());
		CleanupStack::PushL(aRemainder);
		aRemainder.CopyL(aDividend); //set remainder to a
		aRemainder.SetSign(TInteger::EPositive);
		aQuotient.CleanNewL(2); //Set quotient to zero
		CleanupStack::Pop(&aRemainder);
		return;
		}

	dividendSize += dividendSize%2;	// round up to next even number
	divisorSize += divisorSize%2;

	aRemainder.CleanNewL(divisorSize);
	CleanupStack::PushL(aRemainder);
	aQuotient.CleanNewL(dividendSize-divisorSize+2);
	CleanupStack::PushL(aQuotient);

	RInteger T = RInteger::NewEmptyL(dividendSize+2*divisorSize+4);
	Divide(aRemainder.Ptr(), aQuotient.Ptr(), T.Ptr(), aDividend.Ptr(), 
		dividendSize, aDivisor.Ptr(), divisorSize);
	T.Close();
	CleanupStack::Pop(2, &aRemainder); //aQuotient, aRemainder
	}

void TInteger::DivideL(RInteger& aRemainder, RInteger& aQuotient, 
	const TInteger& aDividend, const TInteger& aDivisor) const
    {
    PositiveDivideL(aRemainder, aQuotient, aDividend, aDivisor);

    if (aDividend.IsNegative())
        {
        aQuotient.Negate();
        if (aRemainder.NotZero())
            {
            --aQuotient;
			assert(aRemainder.Size() <= aDivisor.Size());
			Subtract(aRemainder.Ptr(), aDivisor.Ptr(), aRemainder.Ptr(), 
				aRemainder.Size());
            }
        }

    if (aDivisor.IsNegative())
        aQuotient.Negate();
    }

TInt TInteger::ConvertToLong(void) const
	{
	TUint value = ConvertToUnsignedLong();
	return Sign() == EPositive ? value : -(static_cast<TInt>(value));
	}

TBool TInteger::IsConvertableToLong(void) const
	{
	if(WordCount() > 1)
		{
		return EFalse;
		}
	TUint value = (Ptr())[0];
	if(Sign() == EPositive)
		{
		return static_cast<TInt>(value) >= 0;
		}
	else
		{
		return -(static_cast<TInt>(value)) < 0;
		}
	}

void TInteger::RandomizeL(TUint aBits, TRandomAttribute aAttr)
	{
	if(!aBits)
		{
		return;
		}
	const TUint bytes = BitsToBytes(aBits);
	const TUint words = BitsToWords(aBits);
	CleanGrowL(words);
	TPtr8 buf((TUint8*)(Ptr()), bytes, WordsToBytes(Size()));
	TUint bitpos = aBits % BYTE_BITS;
	GenerateRandomBytesL(buf);
	//mask with 0 all bits above the num requested in the most significant byte
	if(bitpos)
		{
		buf[bytes-1] = TUint8( buf[bytes-1] & ((1L << bitpos) - 1) );
		}
	//set most significant (top) bit 
	if(aAttr == ETopBitSet || aAttr == ETop2BitsSet)
		{
		SetBit(aBits-1); //Set bit counts from 0
		assert(BitCount() == aBits);
		assert(Bit(aBits-1));
		}
	//set 2nd bit from top
	if(aAttr == ETop2BitsSet)
		{
		SetBit(aBits-2); //Set bit counts from 0
		assert(BitCount() == aBits);
		assert(Bit(aBits-1));
		assert(Bit(aBits-2));
		}
	}

void TInteger::RandomizeL(const TInteger& aMin, const TInteger& aMax)
	{
	assert(aMax > aMin);
	assert(aMin.NotNegative());
	RInteger range = RInteger::NewL(aMax);
	CleanupStack::PushL(range);
	range -= aMin;
	const TUint bits = range.BitCount();

	//if we find a number < range then aMin+range < aMax 
	do
		{
		RandomizeL(bits, EAllBitsRandom);
		} 
	while(*this > range);

	*this += aMin;
	CleanupStack::PopAndDestroy(&range);
	}

/* void PrimeRandomizeL(TUint aBits, TRandomAttribute aAttr)
 * and all primality related functions are implemented in primes.cpp */

void TInteger::CreateNewL(TUint aNewSize)
	{
	//should only be called on construction
	assert(!iPtr);
	
	TUint newSize = RoundupSize(aNewSize);
	SetPtr((TUint*)User::AllocL(WordsToBytes(newSize)));
	SetSize(newSize);
	SetHeapBased();
	}

void TInteger::CleanNewL(TUint aNewSize)
	{
	CreateNewL(aNewSize);
	Mem::FillZ(Ptr(), WordsToBytes(Size())); //clear integer storage
	}

void TInteger::CleanGrowL(TUint aNewSize)
	{
	assert(IsHeapBased());
	TUint newSize = RoundupSize(aNewSize);
	TUint oldSize = Size();
	if(newSize > oldSize)
		{
		TUint* oldPtr = Ptr();
		//1) allocate new memory and set ptr and size
		SetPtr((TUint*)User::AllocL(WordsToBytes(newSize)));
		SetSize(newSize);
		//2) copy old mem to new mem
		Mem::Copy(Ptr(), oldPtr, WordsToBytes(oldSize));
		//3) zero all old memory
		Mem::FillZ(oldPtr, WordsToBytes(oldSize));
		//4) give back old memory
		User::Free(oldPtr);
		//5) zero new memory from end of copy to end of growth
		Mem::FillZ(Ptr() + oldSize, WordsToBytes(newSize-oldSize));
		}
	}

void TInteger::CleanResizeL(TUint aNewSize)
	{
	assert(IsHeapBased());
	TUint newSize = RoundupSize(aNewSize);
	TUint oldSize = Size();
	if(newSize > oldSize)
		{
		CleanGrowL(aNewSize);
		}
	else if(newSize < oldSize)
		{
		TUint* oldPtr = Ptr();
		//1) zero memory above newsize
		Mem::FillZ(oldPtr+WordsToBytes(aNewSize),WordsToBytes(oldSize-newSize));
		//2) ReAlloc cell.  Since our newsize is less than oldsize, it is
		//guarenteed not to move.  Thus this is just freeing part of our old
		//cell to the heap for other uses.
		SetPtr((TUint*)User::ReAllocL(Ptr(), WordsToBytes(newSize)));
		SetSize(newSize);
		}
	}

TInteger::TInteger() : iSize(0), iPtr(0)
	{
	}

void TInteger::Construct(const TDesC8& aValue)
	{
	assert(Size() >= BytesToWords(aValue.Size()));
	if(aValue.Size() > 0)
		{
		//People write numbers with the most significant digits first (big
		//endian) but we store our numbers in little endian.  Hence we need to
		//reverse the string by bytes.

		TUint bytes = aValue.Size();
		TUint8* i = (TUint8*)Ptr();
		TUint8* j = (TUint8*)aValue.Ptr() + bytes;

		//Swap the endianess of the number itself
		// (msb) 01 02 03 04 05 06 (lsb) becomes ->
		// (lsb) 06 05 04 03 02 01 (msb)
		while( j != (TUint8*)aValue.Ptr() )
			{
			*i++ = *--j;
			}
		Mem::FillZ((TUint8*)Ptr() + bytes, WordsToBytes(Size()) - bytes);
		}
	else
		{
		//if size is zero, we zero the whole register
		Mem::FillZ((TUint8*)Ptr(), WordsToBytes(Size()));
		}
	SetSign(EPositive);
	}

void TInteger::Construct(const TInteger& aInteger)
	{
	assert(Size() >= aInteger.Size());
	CopyWords(Ptr(), aInteger.Ptr(), aInteger.Size());
	if(Size() > aInteger.Size())
		{
		Mem::FillZ(Ptr()+aInteger.Size(), WordsToBytes(Size()-aInteger.Size()));
		}
	SetSign(aInteger.Sign());
	}

void TInteger::Construct(TInt aInteger)
	{
	Construct((TUint)aInteger);
	if(aInteger < 0)
		{
		SetSign(ENegative);
		Ptr()[0] = -aInteger;
		}
	}

void TInteger::Construct(TUint aInteger)
	{
	assert(Size() >= 2);
	SetSign(EPositive);
	Ptr()[0] = aInteger;
	Mem::FillZ(Ptr()+1, WordsToBytes(Size()-1));
	}

void TInteger::ConstructStack(TUint aWords, TUint aInteger)
	{
	SetPtr((TUint*)(this)+2);
	//SetStackBased(); //Not strictly needed as stackbased is a 0 at bit 1
	SetSize(aWords);
	assert(Size() >= 2);
	Ptr()[0] = aInteger;
	Mem::FillZ(&(Ptr()[1]), WordsToBytes(Size()-1));
	}

void TInteger::ConstructStack(TUint aWords, const TInteger& aInteger)
	{
	SetPtr((TUint*)(this)+2);
	//SetStackBased(); //Not strictly needed as stackbased is a 0 at bit 1
	SetSize(aWords);
	assert( Size() >= RoundupSize(aInteger.WordCount()) );
	CopyWords(Ptr(), aInteger.Ptr(), aInteger.Size());
	Mem::FillZ(Ptr()+aInteger.Size(), WordsToBytes(Size()-aInteger.Size()));
	}

