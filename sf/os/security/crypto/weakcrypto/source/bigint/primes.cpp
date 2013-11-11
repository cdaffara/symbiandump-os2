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
#include <securityerr.h>
#include "words.h"
#include "primes.h"
#include "algorithms.h"
#include "mont.h"
#include "stackinteger.h"

static inline void EliminateComposites(TUint* aS, TUint aPrime, TUint aJ, 
	TUint aMaxIndex)
	{
	for(; aJ<aMaxIndex; aJ+=aPrime)
		ArraySetBit(aS, aJ);
	}

static inline TInt FindLeastSignificantZero(TUint aX)
	{
	aX = ~aX;
	int i = 0;
	if( aX << 16 == 0 ) aX>>=16, i+=16;
	if( aX << 24 == 0 ) aX>>=8, i+=8;
	if( aX << 28 == 0 ) aX>>=4, i+=4;
	if( aX << 30 == 0 ) aX>>=2, i+=2;
	if( aX << 31 == 0 ) ++i;
	return i;
	}

static inline TInt FindFirstPrimeCandidate(TUint* aS, TUint aBitLength)
	{
	assert(aBitLength % WORD_BITS == 0);
	TUint i=0;
	//The empty statement at the end of this is stop warnings in all compilers
	for(; aS[i] == KMaxTUint && i<BitsToWords(aBitLength); i++) {;}

	if(i == BitsToWords(aBitLength))
		return -1;
	else
		{
		assert( FindLeastSignificantZero((TUint)(aS[i])) >= 0 );
		assert( FindLeastSignificantZero((TUint)(aS[i])) <= 31 );
		return i*WORD_BITS + FindLeastSignificantZero((TUint32)(aS[i]));
		}
	}

static inline TUint FindSmallestIndex(TUint aPrime, TUint aRemainder)
	{
	TUint& j = aRemainder;
	if(j)
		{
		j = aPrime - aRemainder;
		if( j & 0x1L )
			{
			//if j is odd then this + j is even so we actually want 
			//the next number for which (this + j % p == 0) st this + j is odd
			//that is: this + j + p == 0 mod p
			j += aPrime;
			}
		//Turn j into an index for a bit array representing odd numbers only
		j>>=1;
		}
	return j;
	}

static TBool IsSmallPrime(TUint aK) 
	{
	//This is just a binary search of our small prime table.
	TUint l = 0;
	TUint u = KPrimeTableSize;
	while( u > l )
		{
		TUint m = (l+u)>>1;
		TUint p = KPrimeTable[m];
		if(aK < p)
			u = m;
		else if (aK > p)
			l = m + 1;
		else
			return ETrue;
		}
	return EFalse;
	}

static inline TUint RabinMillerRounds(TUint aBits) 
	{
	//See HAC Table 4.4
	if(aBits > 1300)
		return 2;
	if (aBits > 850)
		return 3;
	if (aBits > 650)
		return 4;
	if (aBits > 550)
		return 5;
	if (aBits > 450)
		return 6;
	if (aBits > 400)
		return 7;
	if (aBits > 350)
		return 8;
	if (aBits > 300)
		return 9;
	if (aBits > 250)
		return 12;
	if (aBits > 200)
		return 15;
	if (aBits > 150)
		return 18;
	if (aBits > 100)
		return 27;
	//All of the above are optimisations on the worst case.  The worst case
	//chance of odd composite integers being declared prime by Rabin-Miller is
	//(1/4)^t where t is the number of rounds.  Thus, t = 40 means that the
	//chance of declaring a composite integer prime is less than 2^(-80).  See
	//HAC Fact 4.25 and most of chapter 4 for more details.
	return 40;
	}

static TBool HasSmallDivisorL(const TInteger& aPossiblePrime)
	{
	assert(aPossiblePrime.IsOdd());
	//Start checking at the first odd prime, whether it is even should have
	//already been checked
	for( TUint i=1; i < KPrimeTableSize; i++ )
		{
		if( aPossiblePrime.ModuloL(KPrimeTable[i]) == 0 )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

static TBool RabinMillerIterationL(const CMontgomeryStructure& aMont, 
	const TInteger& aProbablePrime, const TInteger& aBase)
	{
	//see HAC 4.24
	const TInteger& n = aProbablePrime;
	assert(n > KLastSmallPrimeSquared);
	assert(n.IsOdd());
	assert(aBase > TInteger::One());

	RInteger nminus1 = n.MinusL(TInteger::One());
	CleanupStack::PushL(nminus1);
	assert(aBase < nminus1);

	// 1) find (s | 2^s*r == n-1) where r is odd
	// we want the largest power of 2 that divides n-1
	TUint s=0;
	for(;;s++)
		{
		if(nminus1.Bit(s))
			{
			break;
			}
		}
	// (r = (n-1) / 2^s) which is equiv to (n-1 >>= s)
	RInteger r = RInteger::NewL(nminus1);
	CleanupStack::PushL(r);
	r >>= s;

	//At no point do we own y, aMont owns it
	const TInteger* y = &(aMont.ExponentiateL(aBase, r));

	TBool probablePrime = EFalse;
	
	TUint j=1;
	if( *y == TInteger::One() || *y == nminus1 )
		{
		probablePrime = ETrue;
		}
	else
		{
		for(j=1; j<s; j++)
			{
			y = &(aMont.SquareL(*y));
			if(*y == nminus1)
				{
				probablePrime = ETrue;
				break;
				}
			if(*y == TInteger::One())
				{
				assert(probablePrime == EFalse);
				break;
				}
			}
		}
	CleanupStack::PopAndDestroy(&r);
	CleanupStack::PopAndDestroy(&nminus1);//y,r,nminus1
	return probablePrime;
	}

static TBool RabinMillerTestL(const CMontgomeryStructure& aMont, 
	const TInteger& aProbablePrime, TUint aRounds) 
	{
	const TInteger& n = aProbablePrime;
	assert(n > KLastSmallPrimeSquared);
	
	RInteger nminus2 = n.MinusL(TInteger::Two());
	CleanupStack::PushL(nminus2);

	for(TUint i=0; i<aRounds; i++)
		{
		RInteger base = RInteger::NewRandomL(TInteger::Two(), nminus2);
		CleanupStack::PushL(base);
		if(!RabinMillerIterationL(aMont, n, base))
			{
			CleanupStack::PopAndDestroy(2, &nminus2);//base, nminus2
			return EFalse;
			}
		CleanupStack::PopAndDestroy(&base);
		}
	CleanupStack::PopAndDestroy(&nminus2);
	return ETrue;
	}

static TBool IsStrongProbablePrimeL(const TInteger& aPrime) 
	{
	CMontgomeryStructure* mont = CMontgomeryStructure::NewLC(aPrime);
	//This should be using short circuit evaluation
	TBool probablePrime = RabinMillerIterationL(*mont, aPrime, TInteger::Two())
		&& RabinMillerTestL(*mont, aPrime,RabinMillerRounds(aPrime.BitCount()));
	CleanupStack::PopAndDestroy(mont);
	return probablePrime;
	}

/* In the _vast_ majority of cases this simply checks that your chosen random
 * number is >= KLastSmallPrimeSquared and return EFalse and lets the normal
 * prime generation routines handle the situation.  In the case where it is
 * smaller, it generates a provable prime and returns ETrue.  The algorithm for
 * finding a provable prime < KLastPrimeSquared is not the most efficient in the
 * world, but two points come to mind
 * 1) The two if statements hardly _ever_ evaluate to ETrue in real life.
 * 2) Even when it is, the distribution of primes < KLastPrimeSquared is pretty
 * dense, so you aren't going to have check many.
 * This function is essentially here for two reasons:
 * 1) Ensures that it is possible to generate primes < KLastPrimeSquared (the
 * test code does this)
 * 2) Ensures that if you request a prime of a large bit size that there is an
 * even probability distribution across all integers < 2^aBits
 */
TBool TInteger::SmallPrimeRandomizeL(void)
	{
	TBool foundPrime = EFalse;
	//If the random number we've chosen is less than KLastSmallPrime,
	//testing for primality is easy.
	if(*this <= KLastSmallPrime)
		{
		//If Zero or One, or two, next prime number is two
		if(IsZero() || *this == One() || *this == Two())
			{
			CopyL(TInteger::Two());
			foundPrime = ETrue;
			}
		else
			{
			//Make sure any number we bother testing is at least odd
			SetBit(0);
			//Binary search the small primes.
			while(!IsSmallPrime(ConvertToUnsignedLong()))
				{
				//If not prime, add two and try the next odd number.

				//will never carry as the minimum size of an RInteger is 2
				//words.  Much bigger than KLastSmallPrime on 32bit
				//architectures.
				IncrementNoCarry(Ptr(), Size(), 2);
				}
			assert(IsSmallPrime(ConvertToUnsignedLong()));
			foundPrime = ETrue;
			}
		}
	else if(*this <= KLastSmallPrimeSquared)
		{
		//Make sure any number we bother testing is at least odd
		SetBit(0);

		while(HasSmallDivisorL(*this) && *this <= KLastSmallPrimeSquared)
			{
			//If not prime, add two and try the next odd number.

			//will never carry as the minimum size of an RInteger is 2
			//words.  Much bigger than KLastSmallPrime on 32bit
			//architectures.
			IncrementNoCarry(Ptr(), Size(), 2);
			}
		//If we exited while loop because it had no small divisor then it is
		//prime.  Otherwise, we've exceeded the limit of what we can provably
		//generate.  Therefore the normal prime gen routines will be run on it
		//now.
		if(*this < KLastSmallPrimeSquared)
			{
			foundPrime = ETrue;
			}
		else
			{
			assert(foundPrime == EFalse);
			}
		}
	//This doesn't mean there is no such prime, simply means that the number
	//wasn't less than KSmallPrimeSquared and needs to be handled by the normal
	//prime generation routines.
	return foundPrime;
	}

void TInteger::PrimeRandomizeL(TUint aBits, TRandomAttribute aAttr)
	{
	assert(aBits > 1); 
	
	//"this" is "empty" currently.  Consists of Size() words of 0's.  This is just
	//checking that sign flag is positive as we don't set it later.
	assert(NotNegative());

	//Flag for the whole function saying if we've found a prime
	TBool foundProbablePrime = EFalse;

	//Find 2^aBits + 1 -- any prime we find must be less than this.
	RInteger max = RInteger::NewEmptyL(BitsToWords(aBits)+1);
	CleanupStack::PushL(max);
	max.SetBit(aBits);
	assert(max.BitCount()-1 == aBits);

	// aBits 	| approx number of odd numbers you must try to have a 50% 
	//			chance of finding a prime
	//---------------------------------------------------------
	// 512		| 122		
	// 1024		| 245
	// 2048		| 1023
	//Therefore if we are generating larger than 1024 bit numbers we'll use a
	//bigger bit array to have a better chance of avoiding re-generating it.
	TUint sLength = aBits > 1024 ? 1024 : 512;
	RInteger S = RInteger::NewEmptyL(BitsToWords(sLength));
	CleanupStack::PushL(S);

	while(!foundProbablePrime)
		{
		//Randomly choose aBits
		RandomizeL(aBits, aAttr);

		//If the random number chosen is less than KSmallPrimeSquared, we have a
		//special set of routines.
		if(SmallPrimeRandomizeL())
			{
			foundProbablePrime = ETrue;
			}
		else
			{
			//if it was <= KLastSmallPrimeSquared then it would have been
			//handled by SmallPrimeRandomizeL()
			assert(*this > KLastSmallPrimeSquared);

			//Make sure any number we bother testing is at least odd
			SetBit(0);

			//Ensure that this + 2*sLength < max
			RInteger temp = max.MinusL(*this);
			CleanupStack::PushL(temp);
			++temp;
			temp >>=1;
			if(temp < sLength)
				{
				//if this + 2*sLength >= max then we use a smaller sLength to
				//ensure we don't find a number that is outside of our bounds
				//(and bigger than our allocated memory for this)

				//temp must be less than KMaxTUint as sLength is a TUint 
				sLength = temp.ConvertToUnsignedLong();	
				}
			CleanupStack::PopAndDestroy(&temp);

			//Start at 1 as no point in checking against 2 (all odd numbers)
			for(TUint i=1; i<KPrimeTableSize; i++)
				{
				//no need to call ModuloL as we know KPrimeTable[i] is not 0
				TUint remainder = Modulo(*this, KPrimeTable[i]);
				TUint index = FindSmallestIndex(KPrimeTable[i], remainder);
				EliminateComposites(S.Ptr(), KPrimeTable[i], index, sLength);
				}
			TInt j = FindFirstPrimeCandidate(S.Ptr(), sLength);
			TInt prev = 0;
			for(; j>=0; j=FindFirstPrimeCandidate(S.Ptr(), sLength))
				{
				ArraySetBit(S.Ptr(), j);

				//should never carry as we earlier made sure that 2*j + this < max
				//where max is 1 bit more than we asked for.
				IncrementNoCarry(Ptr(), Size(), 2*(j-prev));

				assert(*this < max);
				assert(!HasSmallDivisorL(*this));

				prev = j;

				if( IsStrongProbablePrimeL(*this) )
					{
					foundProbablePrime = ETrue;
					break;
					}
				}
			//This clears the memory
			S.CopyL(0, EFalse);
			}
		}
	CleanupStack::PopAndDestroy(2, &max);
	}

TBool TInteger::IsPrimeL(void) const
	{
	if( NotPositive() )
		{
		return EFalse;
		}
	else if( IsEven() )
		{
		return *this == Two();
		}
	else if( *this <= KLastSmallPrime )
		{
		assert(KLastSmallPrime < KMaxTUint);
		return IsSmallPrime(this->ConvertToUnsignedLong());
		}
	else if( *this <= KLastSmallPrimeSquared )
		{
		return !HasSmallDivisorL(*this);
		}
	else 
		{
		return !HasSmallDivisorL(*this) && IsStrongProbablePrimeL(*this);
		}
	}
