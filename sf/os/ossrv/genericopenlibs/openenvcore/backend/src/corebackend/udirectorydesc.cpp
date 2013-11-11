// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : UDIRECTORYDESC.CPP
// Part of     : LIBC
// Contains the source for the CDirectoryDesc Class
// Version     : 1.0
//



#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include "fdesc.h"
#include "lposix.h"

/* 
Dirent Structure
*/
struct TDirent
	{
	TUint32 		iEntryNum;
	TUint16 		iRecLen;
	TUint8 			iEntryType;
	TUint8 			iNameLen;
	char 			iEntryName[KMaxFileName+1];
	};

// -----------------------------------------------------------------------------
// CDirectoryDesc::CDirectoryDesc : Constructer
// Constructs Directory Descriptor
// -----------------------------------------------------------------------------
CDirectoryDesc::CDirectoryDesc()
	{
	}

// -----------------------------------------------------------------------------
// CDirectoryDesc::CDirectoryDesc : Constructer
// Destructor for Directory Descriptor
// -----------------------------------------------------------------------------
CDirectoryDesc::~CDirectoryDesc()
	{
	iDir.Close();
	}

// -----------------------------------------------------------------------------
// CDirectoryDesc::Open
// Constructs Directory Descriptor
// -----------------------------------------------------------------------------
TInt CDirectoryDesc::Open(RFs& aSession, const TDesC& aName, int mode, int /*perms*/)
	{
	//Save name of the directory
	iDirName = aName;
	TFileName folderName(aName);
	//Check whether dir name is appended with \ or not
	TPtrC right = folderName.Right(1);
	_LIT(KSlash, "\\");
	if( right != KSlash)
		{
		folderName.Append(KSlash);
		}

	TInt err = KErrDirectoryOpen;
	if( mode == O_RDONLY )
		{
		err = iDir.Open( aSession, folderName, KEntryAttMaskSupported);
		iFcntlFlag = mode & O_ACCMODE;
		}
	return err;
	}

// -----------------------------------------------------------------------------
// CDirectoryDesc::Read : Reading from a Directory
// -----------------------------------------------------------------------------
void CDirectoryDesc::Read(TDes8& aDesc, TRequestStatus& aStatus)
	{
	TInt errorNum = KErrNone;
	const TInt16 KDirentSize = 8;
	TInt readLen = aDesc.MaxLength();
	TUint8* bufPtr = const_cast<TUint8*>(aDesc.Ptr());
	TInt copiedInfo = 0;
	TEntryArray entries;
	errorNum = iDir.Read( entries );
	TDirent direntEntry;
	
	if (errorNum == KErrNone || errorNum == KErrEof)
		{
		errorNum = KErrNone;
		TEntry entry;
		TInt len = 0;
		TInt count = entries.Count();
		TBuf8<KMaxFileName> fileName;
		TInt index = 0;
		TInt copyLen = 0;
		//Loop through each entry and get all the informations
		for (; index<count && copiedInfo<readLen; index++, copiedInfo += copyLen)
			{
			entry = entries[index];
			//Copy File's UID
			TUid fileUID = entry.iType.MostDerived();
			direntEntry.iEntryNum = fileUID.iUid;
			HBufC8 *name;
			if(ConvertUnicodeToUtf8(entry.iName,name,errorNum) == -1)
			{
	           break;
			}
			//Copy entry type and record Length
			fileName.Copy( name->Des() );
			delete name;
			len = fileName.Length();
			direntEntry.iRecLen = KDirentSize + len + 1;
			//Maintaing a four byte boundary.
			direntEntry.iRecLen = Align4(direntEntry.iRecLen);
			direntEntry.iEntryType = 0;
			direntEntry.iNameLen = len;
			//Copy entry name
			Mem::Copy( direntEntry.iEntryName, fileName.PtrZ(), len+1);
			
			//Copy structure on to the buffer
			copyLen = Min(direntEntry.iRecLen, (readLen - copiedInfo));
			Mem::Copy( bufPtr, &direntEntry, copyLen );
			bufPtr += copyLen;
			}
		}
	
	//Set the Length
	aDesc.SetLength( copiedInfo );
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, errorNum);
	}

// -----------------------------------------------------------------------------
// CDirectoryDesc::Write
// Will return with KErrBadHandle
// -----------------------------------------------------------------------------
void CDirectoryDesc::Write(TDes8& /*aDesc*/, TRequestStatus& aStatus)
	{
	TInt errorNum = KErrBadHandle;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, errorNum);
	}

// -----------------------------------------------------------------------------
// CDirectoryDesc::Fcntl
// fcntl Implementation for directory 
// -----------------------------------------------------------------------------
//
TInt CDirectoryDesc::Fcntl(TUint /*anArg*/, TUint aCmd)
	{
	TInt retVal = KErrNotSupported;
	
	if( aCmd == F_GETFL )
		{
		return iFcntlFlag;
		}
	return retVal;
	}

// -----------------------------------------------------------------------------
// CDirectoryDesc::LSeek
// seek Implementation for directory 
// -----------------------------------------------------------------------------
//
TInt CDirectoryDesc::LSeek (off_t& /*offset*/, int /*whence*/)
	{
	// minimal implementation for fd of a directory
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CDirectoryDesc::FStat
// stat Implementation for directory 
// -----------------------------------------------------------------------------
//
TInt CDirectoryDesc::FStat(struct stat *st)
	{
	// set mode as directory
	st->st_mode = S_IFDIR | S_IRUSR;
	st->st_nlink = 1;
	st->st_blksize = 512;
	//More attributes for directrory can be set using RFs::Entry(iDirName, entry);
	return KErrNone;
	}
