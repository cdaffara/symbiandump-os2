// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __BARSREADIMPL_H__
#define __BARSREADIMPL_H__

#include <e32std.h>
#include <badesca.h>
#include "BaAssert.h"

/** Interprets resource data read from a resource file.

To use an instance of this class, pass the buffer containing the resource 
data to it by calling SetBuffer().

The buffer containing the resource data is created by RResourceFile::AllocReadLC()/CResourceFile::AllocReadLC() 
or RResourceFile::AllocReadL()/CResourceFile::AllocReadL() which reads the specified resource into it.

The current position within the buffer is always maintained and any request 
for data is always supplied from the current position. The current position 
is always updated .

Expected behaviour when assignment operator or copy constructor is called:
The class doesn't have assignment operator and copy constructor, so the compiler generated
ones will be used. The buffer used by the source class instance will be shared with the 
destination class instance. However source and destination instances will have their own 
current position pointers, both pointed initially to the same buffer position.

Methods panic/leave behaviour: "L" methods panics/leave behaviour depends on iAssertObj
member which could be initialized calling SetAssertObj. If iAssertObj internal state is 
TBaAssert::EPanic - "L" methods will panic, otherwise they will leave if something goes wrong.
By default "L" methods will panic.

@internalComponent
@see TResourceReaderImpl::SetBuffer()
@see RResourceFile::AllocReadL()
@see RResourceFile::AllocReadLC()
@see CResourceFile::AllocReadL()
@see CResourceFile::AllocReadLC() */
class TResourceReaderImpl
    {
public:
	TResourceReaderImpl();

    void SetBuffer(const TDesC8* aBuffer);
	void ResetBuffer();
    const TAny* Ptr();

    // Read counted strings into allocated buffer
    HBufC8*  ReadHBufC8L();
    HBufC16* ReadHBufC16L();

    // Build pointer from a counted string
    TPtrC8  ReadTPtrC8L();
    TPtrC16 ReadTPtrC16L();

    // Build pointer from a counted string in an array of counted strings,
    // also setting the buffer to be used.
    TPtrC8 ReadTPtrC8L(TInt aIndex,const TDesC8* aBuffer);
    TPtrC16 ReadTPtrC16L(TInt aIndex,const TDesC8* aBuffer);

    // Build an array of strings from a resource array
    CDesC8ArrayFlat*  ReadDesC8ArrayL();
    CDesC16ArrayFlat* ReadDesC16ArrayL();

    TInt ReadInt8L();
    TUint ReadUint8L();
    TInt ReadInt16L();
    TUint ReadUint16L();
    TInt ReadInt32L();
    TUint ReadUint32L();
    TReal64 ReadReal64L() __SOFTFP;

    void ReadL(TAny* aPtr,TInt aLength);
    void RewindL(TInt aLength);
    void AdvanceL(TInt aLength);

	void SetAssertObj(const TBaAssert& iAssertObj);
private:
    void MovePtrL(const TUint8* aPtr);
private:
    const TDesC8* iBuffer;
    const TUint8* iCurrentPtr;
    TBaAssert iAssertObj;
    };

#endif//__BARSREADIMPL_H__
