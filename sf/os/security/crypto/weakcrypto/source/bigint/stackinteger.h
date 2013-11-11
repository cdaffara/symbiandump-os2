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
* TStackInteger class implementation
*
*/


/**
 @file 
 @internalComponent
*/
 
#ifndef __STACK_INTEGER_H__
#define __STACK_INTEGER_H__

#include "bigint.h"

/** An integer that can be created on the stack.\n\n
 * The resulting integers are
 * fixed in size and any code that tries to modify their memory allocation
 * will panic.  Stack based integers may be modified as long as you can
 * guarantee that the resulting integer will fit in the current amount of
 * allocated memory.  For integers that can dynamically grow see RInteger.
 * If you are not sure which integer you want, you want an RInteger.
 * @internalComponent
 * @released
 * @see RInteger
 */
template <TUint W=2> 
class TStackInteger : public TInteger
	{
public:
	inline TStackInteger<W>(TUint aInteger) {ConstructStack(W, aInteger);}
	inline TStackInteger<W>(const TInteger& aInteger) {ConstructStack(W, aInteger);}
protected:
	TUint32 iBuf[W];
	};

typedef TStackInteger<2> TStackInteger64;
typedef TStackInteger<4> TStackInteger128;
typedef TStackInteger<8> TStackInteger256;
typedef TStackInteger<16> TStackInteger512;
typedef TStackInteger<32> TStackInteger1024;
typedef TStackInteger<64> TStackInteger2048;
//Don't make them any bigger than this :).  The last one is 66 words (264 bytes)

#endif // __STACK_INTEGER_H__
