// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef LOGDYNBUF_H
#define LOGDYNBUF_H

#include <e32base.h>

/**
This class manages a 16-bit resizable buffer and offers functions which can be used for constructing 
SQL statements. 
AppendL(), and AppendNumL() can be used to append data to the end of the buffer. 
RLogDynBuf instance will try to expand the buffer if there is not enough available space for the data to be appended.

The following code fragment shows how RLogDynBuf can be used:
@code
const TInt KGranularity = 128;
RLogDynBuf buf;
buf.CreateLC(KGranularity);
buf.AppendL(_L("some data"));//AppendL() automatically expands the buffer if there is not enough place for the string
buf.AppendNumL(1234);		 //AppendNumL() automatically expands the buffer if there is not enough place for the string
buf.AppendL(_L("more data"));//AppendL() automatically expands the buffer if there is not enough place for the string
......
CleanupStack::PopAndDestroy(buf);
@endcode

@internalComponent
*/
NONSHARABLE_CLASS(RLogDynBuf)
	{
public:
	inline RLogDynBuf();
	void CreateLC(TInt aGranularity);
	void Close();
	inline const TDesC& DesC() const;
	inline void SetLength(TInt aLength);
	inline TInt Length() const;
	void AppendL(const TDesC& aStr);
	
private:
	void DoAllocL(TInt aLen);
	
private:
	TInt				iGranularity;
	RBuf				iBuf;
	
	};

/**
Initializes RLogDynBuf data memebrs with default values.
*/
inline RLogDynBuf::RLogDynBuf() :
	iGranularity(0)
	{
	}

/**
@return Non-modifiable 16-bit descriptor to the data in the buffer.
*/
inline const TDesC& RLogDynBuf::DesC() const
	{
	return iBuf;
	}

/**
Sets the length of the data represented by the buffer to the specified value.

@param aLength The new length of the buffer
*/
inline void RLogDynBuf::SetLength(TInt aLength)
	{
	__ASSERT_DEBUG(aLength >= 0, User::Invariant());
	iBuf.SetLength(aLength);
	}

/**
@return The length of the data in the buffer
*/
inline TInt RLogDynBuf::Length() const
	{
	return iBuf.Length();
	}

#endif//LOGDYNBUF_H
