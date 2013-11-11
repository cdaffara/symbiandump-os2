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
// All the error codes returned by the Symbian OS wrapper over the zlib 
// library in EZLib component are defined here.
// 
//

/**
 @file
 @publishedAll 
 @released
*/

#ifndef __EZLIBERRORCODES_H__
#define __EZLIBERRORCODES_H__

#include <e32def.h>

//
// Error codes returned from the CEZDecompressor and CEZCompressor classes 
//
/** The Z_STREAM_ERROR zlib error code has been returned by an internal method 
	due to a stream error. */
const TInt KEZlibErrStream				= -11501;
/** The Z_DATA_ERROR zlib error code has been returned by an internal method 
	due to a data error. */
const TInt KEZlibErrData				= -11502;
/** The Z_BUF_ERROR zlib error code has been returned by an internal method 
	due to a buffer error. */
const TInt KEZlibErrBuf					= -11503;
/** The Z_VERSION_ERROR zlib error code has been returned by an internal method 
	due to a version error. */
const TInt KEZlibErrVersion				= -11504;
/** Unexpected ezlib error. */
const TInt KEZlibErrUnexpected			= -11505;
/** Returned by ::DeflateL() if deflation has already occured.  */
const TInt KEZlibErrDeflateTerminated	= -11506;
/** Returned by ::InflateL() if inflation has already occured. */
const TInt KEZlibErrInflateTerminated	= -11507;
/** An error has occured inflating the dictionary. */
const TInt KEZlibErrInflateDictionary	= -11508;

//
// Error codes returned from GZip classes (eg EZGZipFile and CEZGZipToFile)
//
const TInt KEZlibErrNotGZipFile			= -11509;
const TInt KEZlibErrInvalidCompression	= -11510;
const TInt KEZlibErrBadGZipHeader		= -11511;
const TInt KEZlibErrBadGZipTrailer		= -11512;
const TInt KEZlibErrBadGZipCrc			= -11513;


#endif	// __EZLIBERRORCODES_H__
