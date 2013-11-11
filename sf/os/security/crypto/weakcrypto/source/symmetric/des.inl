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


#ifndef __DES_INL__
#define __DES_INL__

inline void IPerm(TUint32& left, TUint32& right)
	{
	TUint32 work;

	right = rotlFixed(right, 4U);
	work = (left ^ right) & 0xf0f0f0f0;
	left ^= work;
	right = rotrFixed(right^work, 20U);
	work = (left ^ right) & 0xffff0000;
	left ^= work;
	right = rotrFixed(right^work, 18U);
	work = (left ^ right) & 0x33333333;
	left ^= work;
	right = rotrFixed(right^work, 6U);
	work = (left ^ right) & 0x00ff00ff;
	left ^= work;
	right = rotlFixed(right^work, 9U);
	work = (left ^ right) & 0xaaaaaaaa;
	left = rotlFixed(left^work, 1U);
	right ^= work;
	}

inline void FPerm(TUint32& left, TUint32& right)
	{
	TUint32 work;

	right = rotrFixed(right, 1U);
	work = (left ^ right) & 0xaaaaaaaa;
	right ^= work;
	left = rotrFixed(left^work, 9U);
	work = (left ^ right) & 0x00ff00ff;
	right ^= work;
	left = rotlFixed(left^work, 6U);
	work = (left ^ right) & 0x33333333;
	right ^= work;
	left = rotlFixed(left^work, 18U);
	work = (left ^ right) & 0xffff0000;
	right ^= work;
	left = rotlFixed(left^work, 20U);
	work = (left ^ right) & 0xf0f0f0f0;
	right ^= work;
	left = rotrFixed(left^work, 4U);
	}

inline void ReverseKeySchedule(TUint32* aKey)
	{
	TInt i = 0;
	for (; i<16; i+=2)
		{
		TClassSwap((*(aKey+i)), (*(aKey+(32-2-i))));
		TClassSwap((*(aKey+i+1)), (*(aKey+(32-1-i))));
		}
	}

#endif //__DES_INL__
