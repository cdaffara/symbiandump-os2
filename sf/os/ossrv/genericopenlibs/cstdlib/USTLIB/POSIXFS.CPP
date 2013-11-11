// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PosixFilesystem class
// 
//

#include "SYSIF.H"
#include "FDESC.H"
#include "LTIME.H"
#include "LPOSIX.H"
#include <fcntl.h>
#include <sys/errno.h>

wchar_t * PosixFilesystem::getcwd (RFs& aFs, wchar_t* buf, unsigned long len, int& anErrno)
	{
	TFullName name;
	TInt err = aFs.SessionPath(name);
	if (!err)
		{
		TPtr16 pathdes((TText16 *)buf, len);
		if (pathdes.MaxLength() >= (name.Length() + 1))	//+1 to allow for the null terminator
			{
			pathdes.Copy(name);
			pathdes.ZeroTerminate();
			return buf;
			}
		else
			err = ERANGE;		//out of range
		}
	MapError(err, anErrno);
	return 0;
	}

int PosixFilesystem::chdir (RFs& aFs, const wchar_t* aPath, int& anErrno)
	{
	TParse name;
	TInt err=GetFullPath(name, (const TText16 *)aPath, aFs, NULL);
	if (!err)
		{
		TPtrC path=name.DriveAndPath();
		TUint att=0;
		if (path.Length()==3)	// Problem in F32 - the root directory has no attributes
			att=KEntryAttDir;
		else
			err=aFs.Att(path, att);
		if (!err)
			if (att&KEntryAttDir)
				err=aFs.SetSessionPath(path);
			else
				err=ENOTDIR; 
		}
	return MapError(err,anErrno);
	}

int PosixFilesystem::rmdir (RFs& aFs, const wchar_t* aPath, int& anErrno)
	{
	TParse name;
	TInt err=GetFullPath(name,(const TText16 *)aPath,aFs,NULL);
	if (!err)
		{
		TPtrC path=name.DriveAndPath();
		TUint att=0;
		if (path.Length()==3)
			err=EPERM;	// no, you may not remove the root directory
		else
			err=aFs.Att(path, att);
		if (!err)
			if (att&KEntryAttDir)
				{
				err=aFs.RmDir(path);
				if (err==KErrInUse)
					err=EEXIST;	// i.e. directory not empty
				}
			else
				err=ENOTDIR; 
		}
	return MapError(err,anErrno);
	}



int PosixFilesystem::mkdir (RFs& aFs, const wchar_t* aPath, int perms, int& anErrno)
	{
	TParse name;
	TInt err=GetFullPath(name,(const TText16 *)aPath,aFs,NULL);
	if (!err)
		{
		TPtrC path=name.DriveAndPath();
		err=aFs.MkDir(path);
		if (!err)
			{
			if ((perms&S_IWUSR)==0)
				err=aFs.SetAtt(path,KEntryAttReadOnly,0);
			}
		}
	return MapError(err,anErrno);
	}

int PosixFilesystem::stat (RFs& aFs, const wchar_t* name, struct stat *st, int& anErrno)
	{
	TFullName fullName;
	TInt err=GetFullFile(fullName,(const TText16*)name,aFs);
	if (!err)
		{
		TEntry entry;
		if (fullName.Length()==3)
			{
			entry.iAtt=KEntryAttDir;
			entry.iModified==TTime(0);
			}
		else
			err=aFs.Entry(fullName,entry);
		if (!err)
			{
			st->st_size = entry.iSize;
			st->st_dev = st->st_rdev = (dev_t)TDriveUnit(fullName);
			CFileDesc::MapStat(*st, entry.iModified, entry.iAtt);
			return 0;
			}
		}
	return MapError(err, anErrno);
	}

int PosixFilesystem::chmod (RFs& aFs, const wchar_t* name, int perms, int& anErrno)
	{
	TFullName fullName;
	TInt err=GetFullFile(fullName,(const TText16*)name,aFs);
	if (!err)
		{
		if ((perms&S_IWUSR)==0)
			err=aFs.SetAtt(fullName,KEntryAttReadOnly,0);
		else
			err=aFs.SetAtt(fullName,0,KEntryAttReadOnly);
		}
	return MapError(err, anErrno);
	}

int PosixFilesystem::unlink (RFs& aFs, const wchar_t* name, int& anErrno)
	{
	TFullName fullName;
	TInt err=GetFullFile(fullName, (TText16*)name, aFs);
	if (!err)
		{
		TUint att=0;
		err=aFs.Att(fullName, att);
		if (!err)
			if (att&KEntryAttDir)
				err=EPERM; 
			else
				err=aFs.Delete(fullName);
		}
	return MapError(err, anErrno);
	}

int PosixFilesystem::rename (RFs& aFs, const wchar_t* oldname, const wchar_t* newname, int& anErrno)
	{
	TFileName oldFullName;
	TInt err = GetFullFile(oldFullName,(const TText16 *)oldname,aFs);
	if (!err)
		{
		TFileName newFullName;
		err = GetFullFile(newFullName,(const TText16 *)newname,aFs);
		if (!err)
			{
			// ANSI doesn't require specific handling when newname exists,
			// so we can just use the EPOC32 semantics and insist that
			// newname doesn't currently exist.
			err=aFs.Rename(oldFullName,newFullName);
			}
		}
	return MapError(err, anErrno);
	}


TInt PosixFilesystem::ResolvePath (RFs& aFs, TParse& aResult, const wchar_t* path, TDes* aFilename)
	{
	return GetFullPath(aResult,(const TText16*)path,aFs,aFilename);
	}


#ifdef __WINS__
TInt PosixFilesystem::SetDefaultDir (RFs& /*aFs*/)
	{
	// NB. don't do this on WINS because the executable is
	// something like w:\epoc32\release\wins\deb\mytest.exe (or just mytest.exe or
	// even ./mytest !!)
	return KErrNone;
	}
#else
TInt PosixFilesystem::SetDefaultDir (RFs& aFs)
	{
	TParse parse;
	parse.Set(RProcess().FileName(), NULL, NULL);
#ifdef __SECURE_DATA__
	return aFs.SetSessionToPrivate(TDriveUnit(parse.Drive()));
#else
	return aFs.SetSessionPath(parse.DriveAndPath());
#endif
	}
#endif
