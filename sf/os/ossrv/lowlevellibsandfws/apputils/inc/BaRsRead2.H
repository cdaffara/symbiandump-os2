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

#ifndef __BARSREAD2_H__
#define __BARSREAD2_H__

#include <e32std.h>
#include <badesca.h>

//Forward declarations
class TResourceReaderImpl;
class CResourceFile;

/** Interprets resource data read from a resource file.

To use an instance of this class, pass the CResourceFile object containing the resource 
data with aResourceId ID to it, by calling OpenL() or OpenLC(). Close the reader
calling Close() after finishing with the resource.

The current position within the buffer is always maintained and any request 
for data is always supplied from the current position. The current position 
is always updated .

Expected behaviour when assignment operator or copy constructor is called:
The class doesn't have assignment operator and copy constructor, so the compiler generated
ones will be used. The buffer used by the source class instance will be shared with the 
destination class instance. However source and destination instances will have their own 
current position pointers, both pointed initially to the same buffer position.

@publishedAll
@released
@see CResourceFile */
class RResourceReader
	{
public:
	IMPORT_C RResourceReader();
	IMPORT_C ~RResourceReader();
	IMPORT_C void OpenL(const CResourceFile* aRscFile, TInt aResourceId);
	IMPORT_C void OpenLC(const CResourceFile* aRscFile, TInt aResourceId);
	IMPORT_C void OpenL(const TDesC8& aRscData);
	IMPORT_C void OpenLC(const TDesC8& aRscData);
	IMPORT_C void Close();

    // Read counted strings into allocated buffer
    IMPORT_C HBufC*   ReadHBufCL();
    IMPORT_C HBufC8*  ReadHBufC8L();
    IMPORT_C HBufC16* ReadHBufC16L();

    // Build pointer from a counted string
    IMPORT_C TPtrC   ReadTPtrCL();
    IMPORT_C TPtrC8  ReadTPtrC8L();
    IMPORT_C TPtrC16 ReadTPtrC16L();

    // Build an array of strings from a resource array
    IMPORT_C CDesCArrayFlat*   ReadDesCArrayL();
    IMPORT_C CDesC8ArrayFlat*  ReadDesC8ArrayL();
    IMPORT_C CDesC16ArrayFlat* ReadDesC16ArrayL();

    IMPORT_C TInt ReadInt8L();
    IMPORT_C TUint ReadUint8L();
    IMPORT_C TInt ReadInt16L();
    IMPORT_C TUint ReadUint16L();
    IMPORT_C TInt ReadInt32L();
    IMPORT_C TUint ReadUint32L();
    IMPORT_C TReal64 ReadReal64L() __SOFTFP;

    IMPORT_C void ReadL(TAny* aPtr,TInt aLength);
    IMPORT_C void RewindL(TInt aLength);
    IMPORT_C void AdvanceL(TInt aLength);
private:
	TResourceReaderImpl* Impl();
	const TResourceReaderImpl* Impl() const;
private:
	//Size of the RResourceReader implementation.
	//It should be 12 because of the BC reasons.
	//That's the size of the implementation class,
	//which implements TResourceReader functionality too.
	enum
		{
		KRsReaderSize = 12
		};
	TUint8 iImpl[KRsReaderSize];
	HBufC8* iRscBuffer;
	};

#endif//__BARSREAD2_H__
