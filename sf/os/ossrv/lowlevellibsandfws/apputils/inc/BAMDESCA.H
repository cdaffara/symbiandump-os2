// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Started by DavidW, March 1996
// Mixin descriptor array
// 
//

#if !defined(__BAMDESCA_H__)
#define __BAMDESCA_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif



class MDesC8Array
/** 
Interface class for 8 bit descriptor arrays. 
It should be inherited by classes which implement this protocol.

This protocol is implemented by class CDesC8Array and the concrete class CPtrC8Array. 
@publishedAll
@released
*/
    {
public:
	/** Destructor. */
    virtual ~MDesC8Array() {}
	/** Requirements
	
	Returns the number of descriptor elements in a descriptor array.
	
	@return The number of descriptor elements in a descriptor array. */
    virtual TInt MdcaCount() const =0;

	/** Requirements
	
	Indexes into a descriptor array.
	
	@param aIndex The position of the descriptor element within a descriptor array. 
	The position is relative to zero; i.e. zero implies the first descriptor element 
	in a descriptor array. 
	@return An 8 bit non-modifiable pointer descriptor representing the descriptor 
	element located at position aIndex within a descriptor array. */
    virtual TPtrC8 MdcaPoint(TInt aIndex) const =0;
    };


class MDesC16Array
/**
Interface class for 16 bit descriptor arrays. It should be inherited by classes 
which implement this protocol.
This protocol is implemented by class CDesC16Array and the concrete class 
CPtrC16Array. 
@publishedAll
@released
*/
    {
public:
	/** Destructor. */
    virtual ~MDesC16Array() {}

	/** Requirements
	
	Returns the number of descriptor elements in a descriptor array.
	
	@return The number of descriptor elements in a descriptor array. */
    virtual TInt MdcaCount() const =0;
	/** Requirements
	
	Indexes into a descriptor array.
	
	@param aIndex The position of the descriptor element within a descriptor array. 
	The position is relative to zero; i.e. zero implies the first descriptor element 
	in a descriptor array. 
	@return A 16 bit non-modifiable pointer descriptor representing the descriptor 
	element located at position aIndex within a descriptor array. */
    virtual TPtrC16 MdcaPoint(TInt aIndex) const =0;
    };


// The definitions below are macros rather than typedefs to avoid generating
// warning C4097 every time the generic mixin class is used in a class
// definition.

#if defined(_UNICODE)
/** 
Build independent interface class for descriptor arrays.

A 16 bit build variant is generated for a Unicode build and an 8 bit build 
variant generated for a non-Unicode build.

This build independent type should always be used unless an explicit 8 bit 
or 16 bit build variant is required.

@see MDesC16Array
@see MDesC8Array 
@publishedAll
@released
*/
#define MDesCArray MDesC16Array
//typedef MDesC16Array MDesCArray;
#else
/** 
Build independent interface class for descriptor arrays.

A 16 bit build variant is generated for a Unicode build and an 8 bit build 
variant generated for a non-Unicode build.

This build independent type should always be used unless an explicit 8 bit 
or 16 bit build variant is required.

@see MDesC16Array
@see MDesC8Array
@publishedAll
@released
*/
#define MDesCArray MDesC8Array
//typedef MDesC8Array  MDesCArray;
#endif


#endif // __BAMDESCA_H__

