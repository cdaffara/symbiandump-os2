// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _ZIP_FILE_MEMBER_H_
#define _ZIP_FILE_MEMBER_H_

#include <localtypes.h>

/**
Class encapsulating the representation of a compressed file contained in a 
CZipFile archive file. 

@publishedAll 
@released
*/
class CZipFileMember: public CBase
	{
friend class CZipFile;

public:

    IMPORT_C TUint32       CRC32(void) const;
	IMPORT_C TUint32		  CompressedSize(void) const;
	IMPORT_C const TFileName*	Name(void) const;
	IMPORT_C TUint32       UncompressedSize(void) const;
	IMPORT_C virtual ~CZipFileMember();
	
private:

	/** The name of a compressed file*/
	TFileName*	  iName;
	
	/** The method for compressing file*/ 
	TUint16		  iCompressionMethod;
	
	/** The size of compressed file */
	TUint32       iCompressedSize;
	
	/** The size of uncompressed file*/
	TUint32       iUncompressedSize;
	
	/** CRC with 32 bits length in a compressed file*/
	TUint32		  iCRC32;
	
	TUint32	      iDataOffset;
	};
	
#endif /* !_ZIP_FILE_MEMBER_H_ */

