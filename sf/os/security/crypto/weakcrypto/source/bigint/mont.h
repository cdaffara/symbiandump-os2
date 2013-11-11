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
* CMontgomeryStructure class implementation
*
*/


/**
 @file 
 @internalComponent
*/
 
#ifndef __MONT_H__
#define __MONT_H__

class TInteger;
class RInteger;

/** A special representation of integers allowing efficient multiplication
 * and exponentiation under special conditions.
 * @internalComponent
 */
class CMontgomeryStructure : public CBase
	{
public:
	static CMontgomeryStructure* NewL(const TInteger& aModulus);
	static CMontgomeryStructure* NewLC(const TInteger& aModulus);
	~CMontgomeryStructure();
	const TInteger& ReduceL(const TInteger& aInteger) const;
	const TInteger& MultiplyL(const TInteger& aA, const TInteger& aB) const;
	const TInteger& ExponentiateL(const TInteger& aBase, const TInteger& aExponent) const;
	const TInteger& SquareL(const TInteger& aA) const;
protected:
	void DoMultiplyL(TInteger& aResult, const TInteger& aA, const TInteger& aB) const;
	void DoSquareL(TInteger& aResult, const TInteger& aA) const;
	TInteger& ConvertInL(TInteger& aInteger) const;
	TInteger& ConvertOutL(TInteger& aInteger) const;
	CMontgomeryStructure();
private:
	void ConstructL(const TInteger& aModulus);
	RInteger iModulus;
	RInteger iModulusInv;
	RInteger iWorkspace;
	mutable RInteger iResult;
	};

#endif // __MONT_H__
