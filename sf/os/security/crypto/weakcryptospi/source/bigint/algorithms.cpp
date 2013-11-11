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


#include "words.h"
#include "algorithms.h"

word Add(word *C, const word *A, const word *B, unsigned int N)
{
	assert (N%2 == 0);
	word carry = 0;
	for (unsigned int i = 0; i < N; i+=2)
	{
		dword u = (dword) carry + A[i] + B[i];
		C[i] = LOW_WORD(u);
		u = (dword) HIGH_WORD(u) + A[i+1] + B[i+1];
		C[i+1] = LOW_WORD(u);
		carry = HIGH_WORD(u);
	}
	return carry;
}

word Subtract(word *C, const word *A, const word *B, unsigned int N)
{
	assert (N%2 == 0);
	word borrow=0;
	for (unsigned i = 0; i < N; i+=2)
	{
		dword u = (dword) A[i] - B[i] - borrow;
		C[i] = LOW_WORD(u);
		u = (dword) A[i+1] - B[i+1] - (word)(0-HIGH_WORD(u));
		C[i+1] = LOW_WORD(u);
		borrow = 0-HIGH_WORD(u);
	}
	return borrow;
}

int Compare(const word *A, const word *B, unsigned int N)
{
	while (N--)
		if (A[N] > B[N])
			return 1;
		else if (A[N] < B[N])
			return -1;

	return 0;
}

// It is the job of the calling code to ensure that this won't carry.
// If you aren't sure, use the next version that will tell you if you need to
// grow your integer.
// Having two of these creates ever so slightly more code but avoids having
// ifdefs all over the rest of the code checking the following type stuff which
// causes warnings in certain compilers about unused parameters in release
// builds.  We can't have that can we!
/*
Allows avoid this all over bigint.cpp and primes.cpp
ifdef _DEBUG
	TUint carry = Increment(Ptr(), Size());
	assert(!carry);
else
	Increment(Ptr(), Size())
endif
*/
void IncrementNoCarry(word *A, unsigned int N, word B)
{
	assert(N);
	word t = A[0];
	A[0] = t+B;
	if (A[0] >= t)
		return;
	for (unsigned i=1; i<N; i++)
		if (++A[i])
			return;
	assert(0);
}

word Increment(word *A, unsigned int N, word B)
{
	assert(N);
	word t = A[0];
	A[0] = t+B;
	if (A[0] >= t)
		return 0;
	for (unsigned i=1; i<N; i++)
		if (++A[i])
			return 0;
	return 1;
}

//See commments above about IncrementNoCarry
void DecrementNoCarry(word *A, unsigned int N, word B)
{
	assert(N);
	word t = A[0];
	A[0] = t-B;
	if (A[0] <= t)
		return;
	for (unsigned i=1; i<N; i++)
		if (A[i]--)
			return;
	assert(0);
}

word Decrement(word *A, unsigned int N, word B)
{
	assert(N);
	word t = A[0];
	A[0] = t-B;
	if (A[0] <= t)
		return 0;
	for (unsigned i=1; i<N; i++)
		if (A[i]--)
			return 0;
	return 1;
}

void TwosComplement(word *A, unsigned int N)
{
	Decrement(A, N);
	for (unsigned i=0; i<N; i++)
		A[i] = ~A[i];
}

static word LinearMultiply(word *C, const word *A, word B, unsigned int N)
{
	word carry=0;
	for(unsigned i=0; i<N; i++)
	{
		dword p = (dword)A[i] * B + carry;
		C[i] = LOW_WORD(p);
		carry = HIGH_WORD(p);
	}
	return carry;
}

static void AtomicMultiply(word *C, const word *A, const word *B)
{
/*
	word s;
	dword d;

	if (A1 >= A0)
		if (B0 >= B1)
		{
			s = 0;
			d = (dword)(A1-A0)*(B0-B1);
		}
		else
		{
			s = (A1-A0);
			d = (dword)s*(word)(B0-B1);
		}
	else
		if (B0 > B1)
		{
			s = (B0-B1);
			d = (word)(A1-A0)*(dword)s;
		}
		else
		{
			s = 0;
			d = (dword)(A0-A1)*(B1-B0);
		}
*/
	// this segment is the branchless equivalent of above
	word D[4] = {A[1]-A[0], A[0]-A[1], B[0]-B[1], B[1]-B[0]};
	unsigned int ai = A[1] < A[0];
	unsigned int bi = B[0] < B[1];
	unsigned int di = ai & bi;
	dword d = (dword)D[di]*D[di+2];
	D[1] = D[3] = 0;
	unsigned int si = ai + !bi;
	word s = D[si];

	dword A0B0 = (dword)A[0]*B[0];
	C[0] = LOW_WORD(A0B0);

	dword A1B1 = (dword)A[1]*B[1];
	dword t = (dword) HIGH_WORD(A0B0) + LOW_WORD(A0B0) + LOW_WORD(d) + LOW_WORD(A1B1);
	C[1] = LOW_WORD(t);

	t = A1B1 + HIGH_WORD(t) + HIGH_WORD(A0B0) + HIGH_WORD(d) + HIGH_WORD(A1B1) - s;
	C[2] = LOW_WORD(t);
	C[3] = HIGH_WORD(t);
}

static word AtomicMultiplyAdd(word *C, const word *A, const word *B)
{
	word D[4] = {A[1]-A[0], A[0]-A[1], B[0]-B[1], B[1]-B[0]};
	unsigned int ai = A[1] < A[0];
	unsigned int bi = B[0] < B[1];
	unsigned int di = ai & bi;
	dword d = (dword)D[di]*D[di+2];
	D[1] = D[3] = 0;
	unsigned int si = ai + !bi;
	word s = D[si];

	dword A0B0 = (dword)A[0]*B[0];
	dword t = A0B0 + C[0];
	C[0] = LOW_WORD(t);

	dword A1B1 = (dword)A[1]*B[1];
	t = (dword) HIGH_WORD(t) + LOW_WORD(A0B0) + LOW_WORD(d) + LOW_WORD(A1B1) + C[1];
	C[1] = LOW_WORD(t);

	t = (dword) HIGH_WORD(t) + LOW_WORD(A1B1) + HIGH_WORD(A0B0) + HIGH_WORD(d) + HIGH_WORD(A1B1) - s + C[2];
	C[2] = LOW_WORD(t);

	t = (dword) HIGH_WORD(t) + HIGH_WORD(A1B1) + C[3];
	C[3] = LOW_WORD(t);
	return HIGH_WORD(t);
}

static inline void AtomicMultiplyBottom(word *C, const word *A, const word *B)
{
	dword t = (dword)A[0]*B[0];
	C[0] = LOW_WORD(t);
	C[1] = HIGH_WORD(t) + A[0]*B[1] + A[1]*B[0];
}

#define MulAcc(x, y)								\
	p = (dword)A[x] * B[y] + c; 					\
	c = LOW_WORD(p);								\
	p = (dword)d + HIGH_WORD(p);					\
	d = LOW_WORD(p);								\
	e += HIGH_WORD(p);

#define SaveMulAcc(s, x, y) 						\
	R[s] = c;										\
	p = (dword)A[x] * B[y] + d; 					\
	c = LOW_WORD(p);								\
	p = (dword)e + HIGH_WORD(p);					\
	d = LOW_WORD(p);								\
	e = HIGH_WORD(p);

#define MulAcc1(x, y)								\
	p = (dword)A[x] * A[y] + c; 					\
	c = LOW_WORD(p);								\
	p = (dword)d + HIGH_WORD(p);					\
	d = LOW_WORD(p);								\
	e += HIGH_WORD(p);

#define SaveMulAcc1(s, x, y) 						\
	R[s] = c;										\
	p = (dword)A[x] * A[y] + d; 					\
	c = LOW_WORD(p);								\
	p = (dword)e + HIGH_WORD(p);					\
	d = LOW_WORD(p);								\
	e = HIGH_WORD(p);

#define SquAcc(x, y)								\
	p = (dword)A[x] * A[y];	\
	p = p + p + c; 					\
	c = LOW_WORD(p);								\
	p = (dword)d + HIGH_WORD(p);					\
	d = LOW_WORD(p);								\
	e += HIGH_WORD(p);

#define SaveSquAcc(s, x, y) 						\
	R[s] = c;										\
	p = (dword)A[x] * A[y];	\
	p = p + p + d; 					\
	c = LOW_WORD(p);								\
	p = (dword)e + HIGH_WORD(p);					\
	d = LOW_WORD(p);								\
	e = HIGH_WORD(p);

// VC60 workaround: MSVC 6.0 has an optimization problem that makes
// (dword)A*B where either A or B has been cast to a dword before
// very expensive. Revisit a CombaSquare4() function when this
// problem is fixed.               

// WARNING: KeithR.  05/08/03 This routine doesn't work with gcc on hardware
// either.  I've completely removed it.  It may be worth looking into sometime
// in the future.
/*#ifndef __WINS__
static void CombaSquare4(word *R, const word *A)
{
	dword p;
	word c, d, e;

	p = (dword)A[0] * A[0];
	R[0] = LOW_WORD(p);
	c = HIGH_WORD(p);
	d = e = 0;

	SquAcc(0, 1);

	SaveSquAcc(1, 2, 0);
	MulAcc1(1, 1);

	SaveSquAcc(2, 0, 3);
	SquAcc(1, 2);

	SaveSquAcc(3, 3, 1);
	MulAcc1(2, 2);

	SaveSquAcc(4, 2, 3);

	R[5] = c;
	p = (dword)A[3] * A[3] + d;
	R[6] = LOW_WORD(p);
	R[7] = e + HIGH_WORD(p);
}
#endif */

static void CombaMultiply4(word *R, const word *A, const word *B)
{
	dword p;
	word c, d, e;

	p = (dword)A[0] * B[0];
	R[0] = LOW_WORD(p);
	c = HIGH_WORD(p);
	d = e = 0;

	MulAcc(0, 1);
	MulAcc(1, 0);

	SaveMulAcc(1, 2, 0);
	MulAcc(1, 1);
	MulAcc(0, 2);

	SaveMulAcc(2, 0, 3);
	MulAcc(1, 2);
	MulAcc(2, 1);
	MulAcc(3, 0);

	SaveMulAcc(3, 3, 1);
	MulAcc(2, 2);
	MulAcc(1, 3);

	SaveMulAcc(4, 2, 3);
	MulAcc(3, 2);

	R[5] = c;
	p = (dword)A[3] * B[3] + d;
	R[6] = LOW_WORD(p);
	R[7] = e + HIGH_WORD(p);
}

static void CombaMultiply8(word *R, const word *A, const word *B)
{
	dword p;
	word c, d, e;

	p = (dword)A[0] * B[0];
	R[0] = LOW_WORD(p);
	c = HIGH_WORD(p);
	d = e = 0;

	MulAcc(0, 1);
	MulAcc(1, 0);

	SaveMulAcc(1, 2, 0);
	MulAcc(1, 1);
	MulAcc(0, 2);

	SaveMulAcc(2, 0, 3);
	MulAcc(1, 2);
	MulAcc(2, 1);
	MulAcc(3, 0);

	SaveMulAcc(3, 0, 4);
	MulAcc(1, 3);
	MulAcc(2, 2);
	MulAcc(3, 1);
	MulAcc(4, 0);

	SaveMulAcc(4, 0, 5);
	MulAcc(1, 4);
	MulAcc(2, 3);
	MulAcc(3, 2);
	MulAcc(4, 1);
	MulAcc(5, 0);

	SaveMulAcc(5, 0, 6);
	MulAcc(1, 5);
	MulAcc(2, 4);
	MulAcc(3, 3);
	MulAcc(4, 2);
	MulAcc(5, 1);
	MulAcc(6, 0);

	SaveMulAcc(6, 0, 7);
	MulAcc(1, 6);
	MulAcc(2, 5);
	MulAcc(3, 4);
	MulAcc(4, 3);
	MulAcc(5, 2);
	MulAcc(6, 1);
	MulAcc(7, 0);

	SaveMulAcc(7, 1, 7);
	MulAcc(2, 6);
	MulAcc(3, 5);
	MulAcc(4, 4);
	MulAcc(5, 3);
	MulAcc(6, 2);
	MulAcc(7, 1);

	SaveMulAcc(8, 2, 7);
	MulAcc(3, 6);
	MulAcc(4, 5);
	MulAcc(5, 4);
	MulAcc(6, 3);
	MulAcc(7, 2);

	SaveMulAcc(9, 3, 7);
	MulAcc(4, 6);
	MulAcc(5, 5);
	MulAcc(6, 4);
	MulAcc(7, 3);

	SaveMulAcc(10, 4, 7);
	MulAcc(5, 6);
	MulAcc(6, 5);
	MulAcc(7, 4);

	SaveMulAcc(11, 5, 7);
	MulAcc(6, 6);
	MulAcc(7, 5);

	SaveMulAcc(12, 6, 7);
	MulAcc(7, 6);

	R[13] = c;
	p = (dword)A[7] * B[7] + d;
	R[14] = LOW_WORD(p);
	R[15] = e + HIGH_WORD(p);
}

static void CombaMultiplyBottom4(word *R, const word *A, const word *B)
{
	dword p;
	word c, d, e;

	p = (dword)A[0] * B[0];
	R[0] = LOW_WORD(p);
	c = HIGH_WORD(p);
	d = e = 0;

	MulAcc(0, 1);
	MulAcc(1, 0);

	SaveMulAcc(1, 2, 0);
	MulAcc(1, 1);
	MulAcc(0, 2);

	R[2] = c;
	R[3] = d + A[0] * B[3] + A[1] * B[2] + A[2] * B[1] + A[3] * B[0];
}

static void CombaMultiplyBottom8(word *R, const word *A, const word *B)
{
	dword p;
	word c, d, e;

	p = (dword)A[0] * B[0];
	R[0] = LOW_WORD(p);
	c = HIGH_WORD(p);
	d = e = 0;

	MulAcc(0, 1);
	MulAcc(1, 0);

	SaveMulAcc(1, 2, 0);
	MulAcc(1, 1);
	MulAcc(0, 2);

	SaveMulAcc(2, 0, 3);
	MulAcc(1, 2);
	MulAcc(2, 1);
	MulAcc(3, 0);

	SaveMulAcc(3, 0, 4);
	MulAcc(1, 3);
	MulAcc(2, 2);
	MulAcc(3, 1);
	MulAcc(4, 0);

	SaveMulAcc(4, 0, 5);
	MulAcc(1, 4);
	MulAcc(2, 3);
	MulAcc(3, 2);
	MulAcc(4, 1);
	MulAcc(5, 0);

	SaveMulAcc(5, 0, 6);
	MulAcc(1, 5);
	MulAcc(2, 4);
	MulAcc(3, 3);
	MulAcc(4, 2);
	MulAcc(5, 1);
	MulAcc(6, 0);

	R[6] = c;
	R[7] = d + A[0] * B[7] + A[1] * B[6] + A[2] * B[5] + A[3] * B[4] +
				A[4] * B[3] + A[5] * B[2] + A[6] * B[1] + A[7] * B[0];
}

#undef MulAcc
#undef SaveMulAcc
static void AtomicInverseModPower2(word *C, word A0, word A1)
{
	assert(A0%2==1);

	dword A=MAKE_DWORD(A0, A1), R=A0%8;

	for (unsigned i=3; i<2*WORD_BITS; i*=2)
		R = R*(2-R*A);

	assert(R*A==1);

	C[0] = LOW_WORD(R);
	C[1] = HIGH_WORD(R);
}
// ********************************************************

#define A0		A
#define A1		(A+N2)
#define B0		B
#define B1		(B+N2)

#define T0		T
#define T1		(T+N2)
#define T2		(T+N)
#define T3		(T+N+N2)

#define R0		R
#define R1		(R+N2)
#define R2		(R+N)
#define R3		(R+N+N2)

// R[2*N] - result = A*B
// T[2*N] - temporary work space
// A[N] --- multiplier
// B[N] --- multiplicant

void RecursiveMultiply(word *R, word *T, const word *A, const word *B, unsigned int N)
{
	assert(N>=2 && N%2==0);

	if (N==2)
		AtomicMultiply(R, A, B);
	else if (N==4)
		CombaMultiply4(R, A, B);
	else if (N==8)
		CombaMultiply8(R, A, B);
	else
	{
		const unsigned int N2 = N/2;
		int carry;

		int aComp = Compare(A0, A1, N2);
		int bComp = Compare(B0, B1, N2);

		switch (2*aComp + aComp + bComp)
		{
		case -4:
			Subtract(R0, A1, A0, N2);
			Subtract(R1, B0, B1, N2);
			RecursiveMultiply(T0, T2, R0, R1, N2);
			Subtract(T1, T1, R0, N2);
			carry = -1;
			break;
		case -2:
			Subtract(R0, A1, A0, N2);
			Subtract(R1, B0, B1, N2);
			RecursiveMultiply(T0, T2, R0, R1, N2);
			carry = 0;
			break;
		case 2:
			Subtract(R0, A0, A1, N2);
			Subtract(R1, B1, B0, N2);
			RecursiveMultiply(T0, T2, R0, R1, N2);
			carry = 0;
			break;
		case 4:
			Subtract(R0, A1, A0, N2);
			Subtract(R1, B0, B1, N2);
			RecursiveMultiply(T0, T2, R0, R1, N2);
			Subtract(T1, T1, R1, N2);
			carry = -1;
			break;
		default:
			SetWords(T0, 0, N);
			carry = 0;
		}

		RecursiveMultiply(R0, T2, A0, B0, N2);
		RecursiveMultiply(R2, T2, A1, B1, N2);

		// now T[01] holds (A1-A0)*(B0-B1), R[01] holds A0*B0, R[23] holds A1*B1

		carry += Add(T0, T0, R0, N);
		carry += Add(T0, T0, R2, N);
		carry += Add(R1, R1, T0, N);

		assert (carry >= 0 && carry <= 2);
		Increment(R3, N2, carry);
	}
}

// R[2*N] - result = A*A
// T[2*N] - temporary work space
// A[N] --- number to be squared

void RecursiveSquare(word *R, word *T, const word *A, unsigned int N)
{
	assert(N && N%2==0);

	if (N==2)
		AtomicMultiply(R, A, A);
	else if (N==4)
	{
		// VC60 workaround: MSVC 6.0 has an optimization problem that makes
		// (dword)A*B where either A or B has been cast to a dword before
		// very expensive. Revisit a CombaSquare4() function when this
		// problem is fixed.               

// WARNING: KeithR. 05/08/03 This routine doesn't work with gcc on hardware
// either.  I've completely removed it.  It may be worth looking into sometime
// in the future.  Therefore, we use the CombaMultiply4 on all targets.
//#ifdef __WINS__
		CombaMultiply4(R, A, A);
/*#else
		CombaSquare4(R, A);
#endif*/
	}
	else
	{
		const unsigned int N2 = N/2;

		RecursiveSquare(R0, T2, A0, N2);
		RecursiveSquare(R2, T2, A1, N2);
		RecursiveMultiply(T0, T2, A0, A1, N2);

		word carry = Add(R1, R1, T0, N);
		carry += Add(R1, R1, T0, N);
		Increment(R3, N2, carry);
	}
}
// R[N] - bottom half of A*B
// T[N] - temporary work space
// A[N] - multiplier
// B[N] - multiplicant

void RecursiveMultiplyBottom(word *R, word *T, const word *A, const word *B, unsigned int N)
{
	assert(N>=2 && N%2==0);

	if (N==2)
		AtomicMultiplyBottom(R, A, B);
	else if (N==4)
		CombaMultiplyBottom4(R, A, B);
	else if (N==8)
		CombaMultiplyBottom8(R, A, B);
	else
	{
		const unsigned int N2 = N/2;

		RecursiveMultiply(R, T, A0, B0, N2);
		RecursiveMultiplyBottom(T0, T1, A1, B0, N2);
		Add(R1, R1, T0, N2);
		RecursiveMultiplyBottom(T0, T1, A0, B1, N2);
		Add(R1, R1, T0, N2);
	}
}

// R[N] --- upper half of A*B
// T[2*N] - temporary work space
// L[N] --- lower half of A*B
// A[N] --- multiplier
// B[N] --- multiplicant

void RecursiveMultiplyTop(word *R, word *T, const word *L, const word *A, const word *B, unsigned int N)
{
	assert(N>=2 && N%2==0);

	if (N==2)
	{
		AtomicMultiply(T, A, B);
		((dword *)R)[0] = ((dword *)T)[1];
	}
	else if (N==4)
	{
		CombaMultiply4(T, A, B);
		((dword *)R)[0] = ((dword *)T)[2];
		((dword *)R)[1] = ((dword *)T)[3];
	}
	else
	{
		const unsigned int N2 = N/2;
		int carry;

		int aComp = Compare(A0, A1, N2);
		int bComp = Compare(B0, B1, N2);

		switch (2*aComp + aComp + bComp)
		{
		case -4:
			Subtract(R0, A1, A0, N2);
			Subtract(R1, B0, B1, N2);
			RecursiveMultiply(T0, T2, R0, R1, N2);
			Subtract(T1, T1, R0, N2);
			carry = -1;
			break;
		case -2:
			Subtract(R0, A1, A0, N2);
			Subtract(R1, B0, B1, N2);
			RecursiveMultiply(T0, T2, R0, R1, N2);
			carry = 0;
			break;
		case 2:
			Subtract(R0, A0, A1, N2);
			Subtract(R1, B1, B0, N2);
			RecursiveMultiply(T0, T2, R0, R1, N2);
			carry = 0;
			break;
		case 4:
			Subtract(R0, A1, A0, N2);
			Subtract(R1, B0, B1, N2);
			RecursiveMultiply(T0, T2, R0, R1, N2);
			Subtract(T1, T1, R1, N2);
			carry = -1;
			break;
		default:
			SetWords(T0, 0, N);
			carry = 0;
		}

		RecursiveMultiply(T2, R0, A1, B1, N2);

		// now T[01] holds (A1-A0)*(B0-B1), T[23] holds A1*B1

		CopyWords(R0, L+N2, N2);
		word c2 = Subtract(R0, R0, L, N2);
		c2 += Subtract(R0, R0, T0, N2);
		word t = (Compare(R0, T2, N2) == -1);

		carry += t;
		carry += Increment(R0, N2, c2+t);
		carry += Add(R0, R0, T1, N2);
		carry += Add(R0, R0, T3, N2);

		CopyWords(R1, T3, N2);
		assert (carry >= 0 && carry <= 2);
		Increment(R1, N2, carry);
	}
}

// R[NA+NB] - result = A*B
// T[NA+NB] - temporary work space
// A[NA] ---- multiplier
// B[NB] ---- multiplicant

void AsymmetricMultiply(word *R, word *T, const word *A, unsigned int NA, const word *B, unsigned int NB)
{
	if (NA == NB)
	{
		if (A == B)
			RecursiveSquare(R, T, A, NA);
		else
			RecursiveMultiply(R, T, A, B, NA);

		return;
	}

	if (NA > NB)
	{
		TClassSwap(A, B);
		TClassSwap(NA, NB);
		//std::swap(A, B);
		//std::swap(NA, NB);
	}

	assert(NB % NA == 0);
	assert((NB/NA)%2 == 0); 	// NB is an even multiple of NA

	if (NA==2 && !A[1])
	{
		switch (A[0])
		{
		case 0:
			SetWords(R, 0, NB+2);
			return;
		case 1:
			CopyWords(R, B, NB);
			R[NB] = R[NB+1] = 0;
			return;
		default:
			R[NB] = LinearMultiply(R, B, A[0], NB);
			R[NB+1] = 0;
			return;
		}
	}

	RecursiveMultiply(R, T, A, B, NA);
	CopyWords(T+2*NA, R+NA, NA);

	unsigned i;

	for (i=2*NA; i<NB; i+=2*NA)
		RecursiveMultiply(T+NA+i, T, A, B+i, NA);
	for (i=NA; i<NB; i+=2*NA)
		RecursiveMultiply(R+i, T, A, B+i, NA);

	if (Add(R+NA, R+NA, T+2*NA, NB-NA))
		Increment(R+NB, NA);
}
// R[N] ----- result = A inverse mod 2**(WORD_BITS*N)
// T[3*N/2] - temporary work space
// A[N] ----- an odd number as input

void RecursiveInverseModPower2(word *R, word *T, const word *A, unsigned int N)
{
	if (N==2)
		AtomicInverseModPower2(R, A[0], A[1]);
	else
	{
		const unsigned int N2 = N/2;
		RecursiveInverseModPower2(R0, T0, A0, N2);
		T0[0] = 1;
		SetWords(T0+1, 0, N2-1);
		RecursiveMultiplyTop(R1, T1, T0, R0, A0, N2);
		RecursiveMultiplyBottom(T0, T1, R0, A1, N2);
		Add(T0, R1, T0, N2);
		TwosComplement(T0, N2);
		RecursiveMultiplyBottom(R1, T1, R0, T0, N2);
	}
}
#undef A0
#undef A1
#undef B0
#undef B1

#undef T0
#undef T1
#undef T2
#undef T3

#undef R0
#undef R1
#undef R2
#undef R3

// R[N] --- result = X/(2**(WORD_BITS*N)) mod M
// T[3*N] - temporary work space
// X[2*N] - number to be reduced
// M[N] --- modulus
// U[N] --- multiplicative inverse of M mod 2**(WORD_BITS*N)

void MontgomeryReduce(word *R, word *T, const word *X, const word *M, const word *U, unsigned int N)
{
	RecursiveMultiplyBottom(R, T, X, U, N);
	RecursiveMultiplyTop(T, T+N, X, R, M, N);
	if (Subtract(R, X+N, T, N))
	{
#ifdef _DEBUG
		word carry = Add(R, R, M, N);
		assert(carry);
#else
		Add(R, R, M, N);
#endif
	}
}

// do a 3 word by 2 word divide, returns quotient and leaves remainder in A
static word SubatomicDivide(word *A, word B0, word B1)
{
	// assert {A[2],A[1]} < {B1,B0}, so quotient can fit in a word
	assert(A[2] < B1 || (A[2]==B1 && A[1] < B0));

	dword p, u;
	word Q;

	// estimate the quotient: do a 2 word by 1 word divide
	if (B1+1 == 0)
		Q = A[2];
	else
		Q = word(MAKE_DWORD(A[1], A[2]) / (B1+1));

	// now subtract Q*B from A
	p = (dword) B0*Q;
	u = (dword) A[0] - LOW_WORD(p);
	A[0] = LOW_WORD(u);
	u = (dword) A[1] - HIGH_WORD(p) - (word)(0-HIGH_WORD(u)) - (dword)B1*Q;
	A[1] = LOW_WORD(u);
	A[2] += HIGH_WORD(u);

	// Q <= actual quotient, so fix it
	while (A[2] || A[1] > B1 || (A[1]==B1 && A[0]>=B0))
	{
		u = (dword) A[0] - B0;
		A[0] = LOW_WORD(u);
		u = (dword) A[1] - B1 - (word)(0-HIGH_WORD(u));
		A[1] = LOW_WORD(u);
		A[2] += HIGH_WORD(u);
		Q++;
		assert(Q);	// shouldn't overflow
	}

	return Q;
}

// do a 4 word by 2 word divide, returns 2 word quotient in Q0 and Q1
static inline void AtomicDivide(word *Q, const word *A, const word *B)
{
	if (!B[0] && !B[1]) // if divisor is 0, we assume divisor==2**(2*WORD_BITS)
	{
		Q[0] = A[2];
		Q[1] = A[3];
	}
	else
	{
		word T[4];
		T[0] = A[0]; T[1] = A[1]; T[2] = A[2]; T[3] = A[3];
		Q[1] = SubatomicDivide(T+1, B[0], B[1]);
		Q[0] = SubatomicDivide(T, B[0], B[1]);

#ifdef _DEBUG
		// multiply quotient and divisor and add remainder, make sure it equals dividend
		assert(!T[2] && !T[3] && (T[1] < B[1] || (T[1]==B[1] && T[0]<B[0])));
		word P[4];
		AtomicMultiply(P, Q, B);
		Add(P, P, T, 4);
		assert(Mem::Compare((TUint8*)P, 4*WORD_SIZE, (TUint8*)A, 4*WORD_SIZE)==0);
#endif
	}
}

// for use by Divide(), corrects the underestimated quotient {Q1,Q0}
static void CorrectQuotientEstimate(word *R, word *T, word *Q, const word *B, unsigned int N)
{
	assert(N && N%2==0);

	if (Q[1])
	{
		T[N] = T[N+1] = 0;
		unsigned i;
		for (i=0; i<N; i+=4)
			AtomicMultiply(T+i, Q, B+i);
		for (i=2; i<N; i+=4)
			if (AtomicMultiplyAdd(T+i, Q, B+i))
				T[i+5] += (++T[i+4]==0);
	}
	else
	{
		T[N] = LinearMultiply(T, B, Q[0], N);
		T[N+1] = 0;
	}

#ifdef _DEBUG
	word borrow = Subtract(R, R, T, N+2);
	assert(!borrow && !R[N+1]);
#else
	Subtract(R, R, T, N+2);
#endif

	while (R[N] || Compare(R, B, N) >= 0)
	{
		R[N] -= Subtract(R, R, B, N);
		Q[1] += (++Q[0]==0);
		assert(Q[0] || Q[1]); // no overflow
	}
}

// R[NB] -------- remainder = A%B
// Q[NA-NB+2] --- quotient	= A/B
// T[NA+2*NB+4] - temp work space
// A[NA] -------- dividend
// B[NB] -------- divisor

void Divide(word *R, word *Q, word *T, const word *A, unsigned int NA, const word *B, unsigned int NB)
{
	assert(NA && NB && NA%2==0 && NB%2==0);
	assert(B[NB-1] || B[NB-2]);
	assert(NB <= NA);

	// set up temporary work space
	word *const TA=T;
	word *const TB=T+NA+2;
	word *const TP=T+NA+2+NB;

	// copy B into TB and normalize it so that TB has highest bit set to 1
	unsigned shiftWords = (B[NB-1]==0);
	TB[0] = TB[NB-1] = 0;
	CopyWords(TB+shiftWords, B, NB-shiftWords);
	unsigned shiftBits = WORD_BITS - BitPrecision(TB[NB-1]);
	assert(shiftBits < WORD_BITS);
	ShiftWordsLeftByBits(TB, NB, shiftBits);

	// copy A into TA and normalize it
	TA[0] = TA[NA] = TA[NA+1] = 0;
	CopyWords(TA+shiftWords, A, NA);
	ShiftWordsLeftByBits(TA, NA+2, shiftBits);

	if (TA[NA+1]==0 && TA[NA] <= 1)
	{
		Q[NA-NB+1] = Q[NA-NB] = 0;
		while (TA[NA] || Compare(TA+NA-NB, TB, NB) >= 0)
		{
			TA[NA] -= Subtract(TA+NA-NB, TA+NA-NB, TB, NB);
			++Q[NA-NB];
		}
	}
	else
	{
		NA+=2;
		assert(Compare(TA+NA-NB, TB, NB) < 0);
	}

	word BT[2];
	BT[0] = TB[NB-2] + 1;
	BT[1] = TB[NB-1] + (BT[0]==0);

	// start reducing TA mod TB, 2 words at a time
	for (unsigned i=NA-2; i>=NB; i-=2)
	{
		AtomicDivide(Q+i-NB, TA+i-2, BT);
		CorrectQuotientEstimate(TA+i-NB, TP, Q+i-NB, TB, NB);
	}

	// copy TA into R, and denormalize it
	CopyWords(R, TA+shiftWords, NB);
	ShiftWordsRightByBits(R, NB, shiftBits);
}

static inline unsigned int EvenWordCount(const word *X, unsigned int N)
{
	while (N && X[N-2]==0 && X[N-1]==0)
		N-=2;
	return N;
}

// return k
// R[N] --- result = A^(-1) * 2^k mod M
// T[4*N] - temporary work space
// A[NA] -- number to take inverse of
// M[N] --- modulus

unsigned int AlmostInverse(word *R, word *T, const word *A, unsigned int NA, const word *M, unsigned int N)
{
	assert(NA<=N && N && N%2==0);

	word *b = T;
	word *c = T+N;
	word *f = T+2*N;
	word *g = T+3*N;
	unsigned int bcLen=2, fgLen=EvenWordCount(M, N);
	unsigned int k=0, s=0;

	SetWords(T, 0, 3*N);
	b[0]=1;
	CopyWords(f, A, NA);
	CopyWords(g, M, N);

	FOREVER
	{
		word t=f[0];
		while (!t)
		{
			if (EvenWordCount(f, fgLen)==0)
			{
				SetWords(R, 0, N);
				return 0;
			}

			ShiftWordsRightByWords(f, fgLen, 1);
			if (c[bcLen-1]) bcLen+=2;
			assert(bcLen <= N);
			ShiftWordsLeftByWords(c, bcLen, 1);
			k+=WORD_BITS;
			t=f[0];
		}

		unsigned int i=0;
		while (t%2 == 0)
		{
			t>>=1;
			i++;
		}
		k+=i;

		if (t==1 && f[1]==0 && EvenWordCount(f, fgLen)==2)
		{
			if (s%2==0)
				CopyWords(R, b, N);
			else
				Subtract(R, M, b, N);
			return k;
		}

		ShiftWordsRightByBits(f, fgLen, i);
		t=ShiftWordsLeftByBits(c, bcLen, i);
		if (t)
		{
			c[bcLen] = t;
			bcLen+=2;
			assert(bcLen <= N);
		}

		if (f[fgLen-2]==0 && g[fgLen-2]==0 && f[fgLen-1]==0 && g[fgLen-1]==0)
			fgLen-=2;

		if (Compare(f, g, fgLen)==-1)
		{
			TClassSwap<word*>(f,g);
			TClassSwap<word*>(b,c);
			s++;
		}

		Subtract(f, f, g, fgLen);

		if (Add(b, b, c, bcLen))
		{
			b[bcLen] = 1;
			bcLen+=2;
			assert(bcLen <= N);
		}
	}
}

// R[N] - result = A/(2^k) mod M
// A[N] - input
// M[N] - modulus

void DivideByPower2Mod(word *R, const word *A, unsigned int k, const word *M, unsigned int N)
{
	CopyWords(R, A, N);

	while (k--)
	{
		if (R[0]%2==0)
			ShiftWordsRightByBits(R, N, 1);
		else
		{
			word carry = Add(R, R, M, N);
			ShiftWordsRightByBits(R, N, 1);
			R[N-1] += carry<<(WORD_BITS-1);
		}
	}
}

