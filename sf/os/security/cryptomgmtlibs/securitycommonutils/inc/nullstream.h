/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* A write stream that throws away all its input, but keeps track of how many
* bytes have been written to it.	It is used for determining the amount of
* memory needed to store externalised objects.
*
*/


/**
 @file
 @internalTechnology
 @released
*/
#ifndef __NULLSTREAM_H__
#define __NULLSTREAM_H__
#include <e32base.h>
#include <s32strm.h>
#include <scs/nullbuf.h>

NONSHARABLE_CLASS(RNullWriteStream) : public RWriteStream
	{
public:
	IMPORT_C RNullWriteStream();
	IMPORT_C TUint BytesWritten();
private:
	TNullBuf iSink;
	};

#endif
