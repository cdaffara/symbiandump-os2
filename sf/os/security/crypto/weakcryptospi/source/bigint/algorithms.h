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
* Bigint algorithms header file
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__

#include "../common/inlines.h"

word Add(word* C, const word* A, const word* B, unsigned int N);
word Subtract(word* C, const word* A, const word* B, unsigned int N);
int Compare(const word *A, const word *B, unsigned int N);
void IncrementNoCarry(word *A, unsigned int N, word B=1);
word Increment(word *A, unsigned int N, word B=1);
void DecrementNoCarry(word *A, unsigned int N, word B=1);
word Decrement(word *A, unsigned int N, word B=1);
void TwosComplement(word *A, unsigned int N);
void Divide(word* R, word* Q, word* T, const word* A, unsigned int NA,
	const word* B, unsigned int NB);
unsigned int AlmostInverse(word *R, word *T, const word *A, unsigned int NA, 
	const word *M, unsigned int N);
void DivideByPower2Mod(word *R, const word *A, unsigned int k, const word *M, 
	unsigned int N);
void RecursiveInverseModPower2(word *R, word *T, const word *A, unsigned int N);
void AsymmetricMultiply(word *R, word *T, const word *A, unsigned int NA, 
	const word *B, unsigned int NB);
void MontgomeryReduce(word *R, word *T, const word *X, const word *M, 
	const word *U, unsigned int N);
void RecursiveSquare(word *R, word *T, const word *A, unsigned int N);

#endif
