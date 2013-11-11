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
#include <euserext.h>
#include "algorithms.h"
#include "words.h"
#include "windowslider.h"
#include "mont.h"

CMontgomeryStructure* CMontgomeryStructure::NewL(
	const TInteger& aModulus)
	{
	CMontgomeryStructure* self = CMontgomeryStructure::NewLC(aModulus);
	CleanupStack::Pop(self);
	return self;
	}

CMontgomeryStructure* CMontgomeryStructure::NewLC(
	const TInteger& aModulus)
	{
	CMontgomeryStructure* self = new(ELeave) CMontgomeryStructure;
	CleanupStack::PushL(self);
	self->ConstructL(aModulus);
	return self;
	}

CMontgomeryStructure::~CMontgomeryStructure()
	{
	iModulus.Close();
	iModulusInv.Close();
	iWorkspace.Close();
	iResult.Close();
	}

void CMontgomeryStructure::ConstructL(const TInteger& aModulus)
	{
	User::LeaveIfError(aModulus.IsOdd() ? KErrNone : KErrArgument);
	
	iModulusInv = RInteger::NewEmptyL(aModulus.Size());
	iWorkspace = RInteger::NewEmptyL(5*aModulus.Size());
	iModulus = RInteger::NewL(aModulus);
	iResult = RInteger::NewEmptyL(aModulus.Size());
	RecursiveInverseModPower2(iModulusInv.Ptr(), iWorkspace.Ptr(), 
		iModulus.Ptr(), iModulus.Size());
	}

CMontgomeryStructure::CMontgomeryStructure()
	{
	}

TInteger& CMontgomeryStructure::ConvertInL(TInteger& aInteger) const
	{
	aInteger <<= WordsToBits(iModulus.Size());
	aInteger %= iModulus;
	return aInteger;
	}

TInteger& CMontgomeryStructure::ConvertOutL(TInteger& aInteger) const
	{
	TUint* const T = iWorkspace.Ptr();
	TUint* const R = aInteger.Ptr();
	const TUint N = iModulus.Size();
	User::LeaveIfError((aInteger.Size() <= N) ? KErrNone : KErrArgument);

	CopyWords(T, aInteger.Ptr(), aInteger.Size());
	SetWords(T + aInteger.Size(), 0, 2*N - aInteger.Size());
	MontgomeryReduce(R, T+2*N, T, iModulus.Ptr(), iModulusInv.Ptr(), N);
	return aInteger;
	}

const TInteger& CMontgomeryStructure::ReduceL(
	const TInteger& aInteger) const
	{
	RInteger temp = RInteger::NewL(aInteger);
	CleanupStack::PushL(temp);
	ConvertInL(temp);
	iResult.CopyL(ConvertOutL(temp), EFalse);
	CleanupStack::PopAndDestroy(&temp);
	return iResult;
	}

const TInteger& CMontgomeryStructure::MultiplyL(const TInteger& aA, 
	const TInteger& aB) const
	{
	RInteger a = RInteger::NewL(aA);
	CleanupStack::PushL(a);
	RInteger b = RInteger::NewL(aB);
	CleanupStack::PushL(b);
	DoMultiplyL(iResult, ConvertInL(a), ConvertInL(b));
	ConvertOutL(iResult);
	CleanupStack::PopAndDestroy(&b); 
	CleanupStack::PopAndDestroy(&a); 
	return iResult;
	}

void CMontgomeryStructure::DoMultiplyL(TInteger& aResult, const TInteger& aA,
	const TInteger& aB) const
	{
	User::LeaveIfError((aResult.Size() == iModulus.Size()) ? KErrNone : KErrArgument);
					   
	TUint* const T = iWorkspace.Ptr();
	TUint* const R = aResult.Ptr();
	const TUint N = iModulus.Size();
	const TUint* const aReg = aA.Ptr();
	const TUint* const bReg = aB.Ptr();
	const TUint aSize = aA.Size();
	const TUint bSize = aB.Size();
	User::LeaveIfError((aSize <= N && bSize <= N) ? KErrNone : KErrArgument);

	AsymmetricMultiply(T, T+2*N, aReg, aSize, bReg, bSize);
	SetWords(T+aSize+bSize, 0, 2*N - aSize - bSize);
	MontgomeryReduce(R, T+2*N, T, iModulus.Ptr(), iModulusInv.Ptr(), N);
	}

const TInteger& CMontgomeryStructure::SquareL(const TInteger& aA) const
	{
	RInteger a = RInteger::NewL(aA);
	CleanupStack::PushL(a);
	DoSquareL(iResult, ConvertInL(a));
	ConvertOutL(iResult);
	CleanupStack::PopAndDestroy(&a);
	return iResult;
	}

void CMontgomeryStructure::DoSquareL(TInteger& aResult, const TInteger& aA) const
	{
	User::LeaveIfError((aResult.Size() == iModulus.Size()) ? KErrNone : KErrArgument);
	TUint* const T = iWorkspace.Ptr();
	TUint* const R = aResult.Ptr();
	const TUint N = iModulus.Size();
	const TUint* const aReg = aA.Ptr();
	const TUint aSize = aA.Size();

	User::LeaveIfError((aSize <= N) ? KErrNone : KErrArgument);

	RecursiveSquare(T, T+2*N, aReg, aSize);
	SetWords(T+2*aSize, 0, 2*N-2*aSize);
	MontgomeryReduce(R, T+2*N, T, iModulus.Ptr(), iModulusInv.Ptr(), N);
	}

const TInteger& CMontgomeryStructure::ExponentiateL(
	const TInteger& aBase, const TInteger& aExponent) const
	{
	//See HAC 14.85
	if ((iResult.Size() != iModulus.Size()) ||
		(aBase >= iModulus) ||
		(!aBase.IsPositive()) ||
		(!aExponent.IsPositive()))
		{
		User::Leave(KErrArgument);
		}

	// 1.1 Precomputation
	// g1 <- g
	// g2 <- g^2
	RInteger g2 = RInteger::NewL(aBase);
	CleanupStack::PushL(g2);
	ConvertInL(g2);
	//ConvertIn can shrink g2, because we call DoSquare on g2, g2 must be the same size as the modulus
	g2.CleanGrowL(iModulus.Size());
	RInteger g1 = RInteger::NewL(g2);
	CleanupStack::PushL(g1);
	DoSquareL(g2, g2);

	TWindowSlider slider(aExponent);

	// 1.2 
	// For i from 1 to (2^(k-1) -1) do g2i+1 <- g2i-1 * g2
	TUint count = (1 << (slider.WindowSize()-1)) - 1; //2^(k-1) -1
	RRArray<RInteger> powerArray(count+1); //+1 because we append g1
	User::LeaveIfError(powerArray.Append(g1));
	CleanupStack::Pop(&g1); 
	CleanupClosePushL(powerArray);
	for(TUint k=1; k <= count; k++)
		{
		RInteger gi = RInteger::NewEmptyL(iModulus.Size());
		DoMultiplyL(gi, g2, powerArray[k-1]);
		User::LeaveIfError(powerArray.Append(gi));
		}
	
	// 2 A <- 1, i <- t
	RInteger temp = RInteger::NewL(TInteger::One());
	CleanupStack::PushL(temp);
	ConvertInL(temp);

	RInteger& A = iResult;
	//Set A to one converted in for this modulus without changing the memory size of A (iResult)
	A.CopyL(temp, EFalse); 
	CleanupStack::PopAndDestroy(&temp);

	TInt i = aExponent.BitCount() - 1;

	// 3 While i>=0 do:
	while( i>=0 )
		{
		// 3.1 If ei == 0 then A <- A^2
		if(!aExponent.Bit(i))
			{
			DoSquareL(A, A);
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
				DoSquareL(A, A);
				}
			DoMultiplyL(A, A, powerArray[slider.Value()>>1]);
			i -= slider.Length();
			}
		}
	CleanupStack::PopAndDestroy(2, &g2); //powerArray, g2
	return ConvertOutL(A); // A == iResult
	}
