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

#include <zipfilemember.h>

/**
Retrieve CRC value in a compressed file contained in a CZipFile archive file. 

@return the 32 bit integer contains the CRC value 
*/  
EXPORT_C TUint32 CZipFileMember::CRC32(void) const
	{
	return iCRC32;
	}

/**
Retrieve the size of compressed file contained in a CZipFile archive file.

@return the size of a compressed file 
*/  	
EXPORT_C TUint32 CZipFileMember::CompressedSize(void) const
	{
	return iCompressedSize;
	}

/**
Retrieve the name of a compressed file contained in a CZipFile archive file.

@return the name of a compressed file 
*/  
EXPORT_C const TFileName* CZipFileMember::Name(void) const
	{
	return iName;
	}
	
/**
Retrieve the size of uncompressed file contained in a CZipFile archive file.

@return the size of a uncompressed file 
*/ 
EXPORT_C TUint32 CZipFileMember::UncompressedSize(void) const
	{
	return iUncompressedSize;
	}

/**
Destructor
*/
EXPORT_C CZipFileMember::~CZipFileMember()
	{
	delete iName;
	}
