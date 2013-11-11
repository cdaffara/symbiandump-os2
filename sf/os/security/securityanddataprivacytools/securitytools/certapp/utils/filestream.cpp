/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "filestream_private.h"
#include "logger.h"
#include "stringconv.h"
#include "utils.h"

FileStreamBuf::FileStreamBuf(const std::string &aFileName, bool aWritable, bool aSpecialTextHandling)
	: iWritable(aWritable), iSpecialTextHandling(aSpecialTextHandling)
{
	if(iWritable)
		{
		OpenUtf8FStreamForWrite(iFile, aFileName.c_str());
		}
	else
		{
		OpenUtf8FStreamForRead(iFile, aFileName.c_str());
		}
	if(iFile.fail())
		{
		dbg << Log::Indent() << "Failed to open '" << aFileName << "' for " << ((iWritable)?("output"):("input")) << "!" << Log::Endl();
		FatalError();
		}
	
}

static const char utf8Header[] = 
	{
		0xef, 0xbb, 0xbf
	};

static const char utf16HeaderBig[] = 
	{
		0xfe, 0xff
	};

static const char utf16HeaderLittle[] = 
	{
		0xff, 0xfe
	};

void FileStreamBuf::StripUtf8HeaderIfPresent()
{
	if(iWritable)
		{
		FatalError(); // Coding error
		}

	std::streampos savedPos = iFile.tellg();
	char header[3];
	iFile.read(header, sizeof(header));
	
	if(iFile.good() && memcmp(header, utf8Header, sizeof(utf8Header)) == 0)
		{
		// We read a UTF8 file header so simply return (and thereby skip it).
		return;
		}

	if(iFile.good() && memcmp(header, utf16HeaderBig, sizeof(utf16HeaderBig)) == 0)
		{
		dbg << Log::Indent() << "Input file is Big Endian UTF16 - Only UTF-8 and ASCII are supported" << Log::Endl();
		FatalError();
		}
	if(iFile.good() && memcmp(header, utf16HeaderLittle, sizeof(utf16HeaderLittle)) == 0)
		{
		dbg << Log::Indent() << "Input file is Little Endian UTF16 - Only UTF-8 and ASCII are supported" << Log::Endl();
		FatalError();
		}

	iFile.clear();
	iFile.seekg(savedPos);
}


TInt FileStreamBuf::DoReadL(TAny *aPtr,TInt aMaxLength)
{
BULLSEYE_OFF
	if(iFile.fail())
		{
		dbg << Log::Indent() << "Read error" << Log::Endl();
		FatalError(); // Read on broken stream
		}
BULLSEYE_RESTORE

	iFile.read((char *)aPtr, aMaxLength);
    
	if(iFile.eof())
		{
		dbg << Log::Indent() << "Encountered EOF" << Log::Endl();
		TInt len = iFile.gcount();
		if(iSpecialTextHandling)
			{
			// Add a synthetic NL to the returned data to handle a
			// token immediately before the EOF
			((TUint8 *)aPtr)[len++] = '\n';
			}
		return len;
		}

BULLSEYE_OFF
	if(iFile.fail())
		{
		dbg << Log::Indent() << "Read error" << Log::Endl();
		FatalError(); // error other than EOF
		}
BULLSEYE_RESTORE

	return iFile.gcount();
}

void FileStreamBuf::DoWriteL(const TUint8* aPtr,TInt aLength)
{
	iFile.write((const char *)aPtr, aLength);
BULLSEYE_OFF
	if(iFile.fail())
		{
		FatalError(); // error other than EOF
		}
BULLSEYE_RESTORE
	//#warning "flush all data for debugging"
	//	iFile.flush();
}

TStreamPos FileStreamBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
{
	std::ios_base::seekdir dir;
	switch(aLocation)
		{
		case EStreamBeginning:
			dir = std::ios_base::beg;
			break;
		case EStreamMark:
			dir = std::ios_base::cur;
			break;
		case EStreamEnd:
			dir = std::ios_base::end;
			break;
BULLSEYE_OFF
		default:
			FatalError();
BULLSEYE_RESTORE
		}

	if(aMark == ERead)
		{
		iFile.seekg(anOffset, dir);
BULLSEYE_OFF
		if(iFile.fail()) FatalError(); // error other than EOF
BULLSEYE_RESTORE
		return iFile.tellg();
		}

	if(aMark == EWrite)
		{
		iFile.seekp(anOffset, dir);
BULLSEYE_OFF
		if(iFile.fail()) FatalError(); // error other than EOF
BULLSEYE_RESTORE
		return iFile.tellp();
		}
	
	FatalError();
	return -1;
}

void FileStreamBuf::DoRelease()
{
	iFile.close();
}

FileWriteStream::FileWriteStream(const std::string &aFileName)
	: RWriteStream()
{
	iSnk = new FileStreamBuf(aFileName, true);
}

FileWriteStream::~FileWriteStream()
{
BULLSEYE_OFF
	if(iSnk)
		{
		dbg << Log::Indent() << "forgot to close FileWriteStream" << Log::Endl();
		FatalError();
		}
BULLSEYE_RESTORE
	iSnk = 0;
}



FileReadStream::FileReadStream(const std::string &aFileName, bool aSpecialTextHandling)
	: RReadStream()
{
	iSrc = new FileStreamBuf(aFileName, false, aSpecialTextHandling);
	if(aSpecialTextHandling)
		{
		static_cast<FileStreamBuf *>(iSrc)->StripUtf8HeaderIfPresent();
		}
}


FileReadStream::~FileReadStream()
{
BULLSEYE_OFF
	if(iSrc)
		{
		dbg << Log::Indent() << "forgot to close FileReadStream" << Log::Endl();
		FatalError();
		}
BULLSEYE_RESTORE
	iSrc = 0;
}

// End of file
