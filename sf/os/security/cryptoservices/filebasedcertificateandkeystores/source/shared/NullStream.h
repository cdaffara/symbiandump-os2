/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @internalTechnology
*/
 
#ifndef __NullStream_h__
#define __NullStream_h__

#include <s32strm.h>
#include <s32buf.h>

/**
 * Implementation of the MStreamBuf interface that throws away all data written
 * to it.  It does not support reading.
 */

class TNullBuf : public MStreamBuf
	{
public:
	TNullBuf();
	TUint BytesWritten();
private:
	virtual void DoWriteL(const TAny* aPtr,TInt aLength);
private:
	TUint iBytesWritten;
	}; 

/**
 * A write stream that throws away all its input, but keeps track of how many
 * bytes have been written to it.  It is used for determining the amount of
 * memory needed to store externalised objects.
 */

class RNullWriteStream : public RWriteStream
	{
public:
	RNullWriteStream();
	TUint BytesWritten();
private:
	TNullBuf iSink;
	};

#endif
