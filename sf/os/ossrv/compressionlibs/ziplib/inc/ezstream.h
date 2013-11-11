// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declaration for CEZStream class.  
// 
//

#ifndef __EZZSTREAM_H__
#define __EZZSTREAM_H__

#include <e32base.h>
#include <ezlib.h> 
#include <ezliberrorcodes.h>

/**
Represents a zip stream

@publishedAll
@released
*/
class CEZZStream : public CBase
	{
public:
/** stream panic values */
	enum 
		{
		EStreamError = -Z_STREAM_ERROR,
		EDataError = -Z_DATA_ERROR,
		EBufError = -Z_BUF_ERROR,
		EVersionError = -Z_VERSION_ERROR,
		EUnexpected
		};
public:
	IMPORT_C void SetInput(const TDesC8& aInputData);
	IMPORT_C void SetOutput(TDes8& aOutputData);

	IMPORT_C TPtrC8 OutputDescriptor() const;
	IMPORT_C TInt TotalOut() const;
	IMPORT_C TInt TotalIn() const;
	IMPORT_C TInt32 Adler32() const;
	IMPORT_C TInt AvailIn() const;
	IMPORT_C TInt AvailOut() const;

	IMPORT_C TInt Progress(TInt aTotalLength) const;
	
protected:
	CEZZStream();

	
protected:
	z_stream iStream;

	// This pointer is reset every time SetOutput is called.  It maintains a pointer to the
	// start of the output buffer which is being written to.  We cannot use iStream.next_out,
	// because zlib will modifify this during its deflate and inflate.
	
	TUint8* iOutputPointer;
	TInt iOutputBufferLength;
	};

#endif
