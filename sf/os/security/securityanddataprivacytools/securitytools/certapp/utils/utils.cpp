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


#include <fstream>
#include "utils.h"
#include "logger.h"

#ifndef __LINUX__
#include <windows.h>
#endif

typedef unsigned char TUint8;
typedef unsigned int TUint;

#ifdef __LINUX__
void OpenUtf8FStreamForRead(std::fstream &aFStream, const char *aUtf8FileName)
{
	aFStream.open(aUtf8FileName, std::ios_base::binary | std::ios_base::in);
}

void OpenUtf8FStreamForWrite(std::fstream &aFStream, const char *aUtf8FileName)
{
	aFStream.open(aUtf8FileName, std::ios_base::binary | std::ios_base::trunc | std::ios_base::out);
}

#else

WCHAR *WindowsUtf8ToUtf16(const char *aUtf8)
{
	int utf8Len = strlen(aUtf8) + 1; // We want to copy the terminator to the new string
	int utf16Space = MultiByteToWideChar(CP_UTF8, 0, aUtf8, utf8Len, 0, 0);
	if(utf16Space <= 0)
		{
		dbg << Log::Indent() << "Failed to convert UTF-8 '" << aUtf8 << "' to UTF16" << Log::Endl();
		FatalError();
		return 0;
		}
	// prog << Log::Indent() << "utf16Space " << utf16Space << Log::Endl();
	WCHAR *utf16 = new WCHAR[utf16Space];
	(void) MultiByteToWideChar(CP_UTF8, 0, aUtf8, utf8Len, utf16, utf16Space);
#if 0
	prog << Log::Indent() << "Raw UTF16 name is (chars " << utf16Space << ")" << Log::Endl();
	for(int i=0; i<utf16Space*2; ++i)
		{
		prog << TUint(((TUint8 *)utf16)[i]) << Log::Endl();
		}
#endif
	return utf16;
}

CHAR *WindowsUtf16ToUtf8(const WCHAR *aUtf16)
{
	int utf16Len = wcslen(aUtf16) + 1; // We want to copy the terminator to the new string
	int utf8Space = WideCharToMultiByte(CP_UTF8, 0, aUtf16, utf16Len, 0, 0, 0, 0);
	if(utf8Space <= 0)
		{
		dbg << Log::Indent() << "Failed to convert UTF16 string to UTF8" << Log::Endl();
		FatalError();
		return 0;
		}
	//prog << Log::Indent() << "utf8Space " << utf8Space << Log::Endl();
	CHAR *utf8 = new CHAR[utf8Space];
	(void) WideCharToMultiByte(CP_UTF8, 0, aUtf16, utf16Len, utf8, utf8Space, 0, 0);

#if 0
	prog << Log::Indent() << "Raw UTF8 name is (chars " << utf8Space << ")" << Log::Endl();
	for(int i=0; i<utf8Space; ++i)
		{
		prog << TUint(((TUint8 *)utf8)[i]) << Log::Endl();
		}
#endif
	return utf8;
}

WCHAR *WindowsRetrieveDosName(const WCHAR *aLongName)
{
	// Convert to a short DOS file name. File MUST already exist!
	// len will be space required for string and terminator
	DWORD len = GetShortPathNameW((WCHAR *)aLongName, 0, 0);
	if( len <= 0 )
		{
		prog << Log::Indent() << "Failed to find existing file and retrieve short name" << Log::Endl();
		return 0;
		}
	
	//prog << Log::Indent() << "space required for short name is " << len << Log::Endl();
	WCHAR *shortName = new WCHAR[len];
	(void) GetShortPathNameW(aLongName, shortName, len);
	return shortName;
}

void OpenUtf8FStreamForRead(std::fstream &aFStream, const char *aUtf8FileName)
{
	// Unfortunately the windows C++ STL library will not open files specified via a UTF-8 filename.
	// The workaround is to convert the UTF-8 name to the short DOS compatible name and open that instead

	// First convert the UTF-8 name to UTF-16 for use with the windows APIs
	WCHAR *utf16Name = WindowsUtf8ToUtf16(aUtf8FileName);

	// Obtain short DOS compatible name for file
	WCHAR *utf16NameDos = WindowsRetrieveDosName(utf16Name);
	if(utf16NameDos == 0)
		{
		aFStream.setstate(std::ios_base::badbit);
		delete [] utf16Name;
		return;
		}

	char *utf8NameDos = WindowsUtf16ToUtf8(utf16NameDos);

	aFStream.open(utf8NameDos, std::ios_base::binary | std::ios_base::in);

	delete [] utf8NameDos;
	delete [] utf16NameDos;
	delete [] utf16Name;
	return;
}

void OpenUtf8FStreamForWrite(std::fstream &aFStream, const char *aUtf8FileName)
{
	// Unfortunately the windows C++ STL library will not open files specified via a UTF-8 filename.
	// The workaround is to make sure the file already exists and then convert the UTF-8 name to the short DOS compatible name 
	// and open that instead

	// First convert the UTF-8 name to UTF-16 for use with the windows APIs
	WCHAR *utf16Name = WindowsUtf8ToUtf16(aUtf8FileName);

	// Make sure the file exists
	HANDLE fh = CreateFileW((WCHAR *)utf16Name, GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0,  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if(fh == INVALID_HANDLE_VALUE)
		{
		dbg << Log::Indent() << "Failed to create file '" << aUtf8FileName << "'" << Log::Endl();
		aFStream.setstate(std::ios_base::badbit);
		delete [] utf16Name;
		return;
		}
	// Close handle
	CloseHandle(fh);


	// Obtain short DOS compatible name for file
	WCHAR *utf16NameDos = WindowsRetrieveDosName(utf16Name);
	if(utf16NameDos == 0)
		{
		aFStream.setstate(std::ios_base::badbit);
		delete [] utf16Name;
		return;
		}

	char *utf8NameDos = WindowsUtf16ToUtf8(utf16NameDos);

	aFStream.open(utf8NameDos, std::ios_base::binary | std::ios_base::trunc | std::ios_base::out);

	delete [] utf8NameDos;
	delete [] utf16NameDos;
	delete [] utf16Name;
	return;
}
#endif

// End of file
