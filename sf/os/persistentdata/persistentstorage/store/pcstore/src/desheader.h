// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__DESHEADER_H__)
#define __DESHEADER_H__

#include <pcstore/pcstoredef.h>

namespace PCStore
{
class CStoreWriteStream;
class CStoreReadStream;
class CDes8;
class CDes16;

/**
@internalComponent

Compact stream format for positive integer values in the range 0 to 536,870,911 ((2^29)-1). Values 
in the range 0-127 are stored in a single byte, 128-16383 in two bytes and other values in 4 bytes.

The class provides conversion to and from TInt, and both externalization and internalization functions. 
*/
class TCardinality
	{
public:
	TCardinality(): iCount(0) {};
	TCardinality(TInt aCount);

	void Externalize(CStoreWriteStream& aStream) const;
	void Internalize(CStoreReadStream& aStream);
	operator TInt() const;

private:
	TUint iCount;
	};

/**
@internalComponent

Class for the header information of CDes8 and CDes16.

The header information of CDes8 and CDes16 includes the data width (8 or 16 bits) and length. 
This class forms the header information from the given CDes8 or CDes16 data and stores it into 
a TCardinality object.  It also provides the member functions to externalize / internalize 
itself to / from a stream.
*/
class CDesHeader
	{
public:
	CDesHeader() {}
	CDesHeader(const CDes8& aDes8);
	CDesHeader(const CDes16& aDes16);

	void Externalize(CStoreWriteStream& aStream) const;
	void Internalize(CStoreReadStream& aStream);
	TInt Length() const;

private:
	TCardinality iVal;
	};
}
#endif // !defined(__DESHEADER_H__)
