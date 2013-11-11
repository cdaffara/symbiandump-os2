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
// POSIX systems calls implemented over EPOC32
// 
//

#include "SYSIF.H"
#include <unistd.h>
#include <errno.h>
#include "LPOSIX.H"
#include <utf.h>
#include <networking/dnd_err.h>

GLDEF_C TInt GetFullPath(TParse& aParse, const TText16* upath, RFs& aSession, TDes* aFileName)
//
// Parse a path of the form "[C:][\]AAA\..\.\BBB\xxx" where:
// .  indicates the current directory
// .. indicates move to the parent directory
// An optional "\" at the start of the path indicates the path is not relative to the current path,
// and is implied if the drive specifier is present
// If aFileName is non-NULL then the final component is a filename and should be copied into 
// the aFileName descriptor.
//
	{

	TInt r;
	TBuf<3> drive;
	TFileName nextBit;
	TText16 c=*upath;

	if (c && upath[1]==KDriveDelimiter) 
		{
		// drive name specified
		if (c==L'?')
			drive.Zero();			// use "?:" to mean scan across drives
		else
			{
			drive.Copy(TPtrC16(upath, 2));
			drive.UpperCase();
			}
		upath+=2;
		drive.Append(TChar(KPathDelimiter));	// enforce absoluteness
		}
	else
		{
		// no leading drive specifier
		drive.Zero();
		if (c==KPathDelimiter||c==L'/')
			{
			upath+=1;
			drive.Append(TChar(KPathDelimiter));
			}
		}
	r = aSession.Parse(drive, aParse);

	// upath now looks like a relative pathname, to be added onto
	// aParse a directory at a time. Note that '/' is not allowed in
	// EPOC32 file or directory names, so treat it as an alternative separator

	c=*upath;
	while (c && (r==KErrNone))
		{
		const TText16* ustart=upath;
		do 
			c=*upath++;
		while (c && c!=KPathDelimiter && c!=L'/');

		TInt len=(upath-ustart)-1;		// excludes delimiter
		if (len==0)
			continue;
		if (ustart[0]==L'.')
			{
			if (len==1)
				continue;	// directory . ignored
			if (len==2 && ustart[1]==L'.')
				{
				// directory ..
				(void) aParse.PopDir();	// just stick at root dir, no errors
				continue;
				}
			}
		if (len>=KMaxFileName)
			return ENAMETOOLONG;
		if (c==L'\0' && aFileName!=NULL)
			{
			// it's the trailing filename
			aFileName->Copy(TPtrC16(ustart, len));
			break;
			}
		else	
			{
			// it's a component of the accumulating path
			nextBit.Copy(TPtrC16(ustart, len));
			r = aParse.AddDir(nextBit);
			}
		}
	return(r);
	}


GLDEF_C TInt GetFullFile(TFileName& aName, const TText16* upath, RFs& aSession)
// Use GetFullPath to establish the pathname, then add the filename onto the end
	{
	TParse path;
	TInt err = GetFullPath(path,upath,aSession,&aName);
	if (err!=KErrNone)
		return err;
	// Wildcard drive letter for searching across drives
	if (upath[0]==L'?' && upath[1]==L':')
		{
		TFindFile search(aSession);
		err=search.FindByDir(aName,path.Path());
		if (!err)
			{
			aName=search.File();
			return KErrNone;
			}
		}
	err = path.SetNoWild(path.DriveAndPath(),NULL,&aName);
	if (!err)
		aName = path.FullName();
	return err;
	}


// Set errno from an E32STD.H error code or a STDLIB errno value

static const TUint8 EPOCtoERRNO[43] = {
	ENOENT,   // KErrNotFound=(-1);
	  0,      // KErrGeneral=(-2);
	EINTR,    // KErrCancel=(-3);
	ENOMEM,   // KErrNoMemory=(-4);
	ENOSYS,   // KErrNotSupported=(-5);
	EINVAL,   // KErrArgument=(-6);
	  0,      // KErrTotalLossOfPrecision=(-7);
	  0,      // KErrBadHandle=(-8);
	ERANGE,   // KErrOverflow=(-9);
	ERANGE,   // KErrUnderflow=(-10);
	EEXIST,   // KErrAlreadyExists=(-11);
	ENOENT,   // KErrPathNotFound=(-12);
	EPIPE,    // KErrDied=(-13);
	EACCES,   // KErrInUse=(-14);
	EPIPE,    // KErrServerTerminated=(-15);
	EBUSY,    // KErrServerBusy=(-16);
	  0,      // KErrCompletion=(-17);
	  0,      // KErrNotReady=(-18);
	  0,      // KErrUnknown=(-19);
	  0,      // KErrCorrupt=(-20);
	EACCES,   // KErrAccessDenied=(-21);
	EACCES,   // KErrLocked=(-22);
	  0,      // KErrWrite=(-23);
	ENODEV,   // KErrDisMounted=(-24);
	EPIPE,    // KErrEof=(-25);
	ENOSPC,   // KErrDiskFull=(-26);
	  0,      // KErrBadDriver=(-27);
	EINVAL,   // KErrBadName=(-28);
	ECOMM,    // KErrCommsLineFail=(-29);
	ECOMM,    // KErrCommsFrame=(-30);
	ECOMM,    // KErrCommsOverrun=(-31);
	ECOMM,    // KErrCommsParity=(-32);
    ETIMEDOUT,    // KErrTimedOut=(-33);
    ECONNREFUSED, // KErrCouldNotConnect=(-34);
	  0,      // KErrCouldNotDisconnect=(-35);
	EPIPE,    // KErrDisconnected=(-36);
	  0,      // KErrBadLibraryEntryPoint=(-37);
	  0,      // KErrBadDescriptor=(-38);
	  0,      // KErrAbort=(-39);
	  0,      // KErrTooBig=(-40);
	EDOM,     // KErrDivideByZero=(-41);
	EDOM,     // KErrBadPower=(-42);
	ENOSPC    // KErrDirFull=(-43);
	};


EXPORT_C int MapError(TInt err, int& anErrno)
	{
	if (err==0)
		return err;	// i.e. return 0 without changing errno
	TInt ret=err;
	if (err<KErrNone && err>=KErrDirFull)
		{
		ret=EPOCtoERRNO[-1-err];
		if (ret==0)
			ret=err;	// no sensible translation
		}
	else if ((err == KErrDndNameNotFound) || (err == KErrDndAddrNotFound))
		// KErrDndNameNotFound Returned when no data found for GetByName
		// KErrDndAddrNotFound Returned when no data found for GetByAddr
		{
		ret=ENOENT;
		}
	anErrno=ret;	// KErr* values are negative, so will be distinct anyway
	return -1;
	}



// Directory enumeration

NONSHARABLE_STRUCT(__EPOC32_WDIR) : public CBase	// aka "class __EPOC32_WDIR with everything public"
	{
public:
	__EPOC32_WDIR(struct _reent* aContext) : iContext(aContext) {}
	~__EPOC32_WDIR();

	TInt Open();
	TInt Open(const TDesC& aPath);
	TInt Open(const wchar_t *_path);

	virtual TInt UpdateNarrow();
	struct _reent *iContext;
	HBufC* iPath;
	CDir* iEntries;
	TInt iIndex;		// counting down, 0 means "finished"
	struct wdirent iCurrent;
	TBuf16<KMaxFileName> iCurrentName;
	};

TInt __EPOC32_WDIR::UpdateNarrow()
{
	return -1;		//this is not supported.
}


NONSHARABLE_STRUCT(__EPOC32_DIR) : public __EPOC32_WDIR
	{
public:
	__EPOC32_DIR(struct _reent* aContext) : __EPOC32_WDIR(aContext) {}
	~__EPOC32_DIR() {}

	virtual TInt UpdateNarrow();
	struct dirent iCurrentNarrow;
	TBuf8<KMaxFileName> iCurrentNarrowName;
	};


TInt __EPOC32_DIR::UpdateNarrow()
{
	//update the narrow one
	
	TInt ret = CnvUtfConverter::ConvertFromUnicodeToUtf8(iCurrentNarrowName, iCurrentName);
	if (ret >= 0)
		{
		iCurrentNarrow.d_namlen=(short)iCurrentNarrowName.Length();
		iCurrentNarrow.d_name = (char*)iCurrentNarrowName.PtrZ();
		}
	return ret;
}


__EPOC32_WDIR::~__EPOC32_WDIR()
	{
	delete iPath;
	delete iEntries;
	}

TInt __EPOC32_WDIR::Open(const wchar_t *_path)
	{
	MSystemInterface& sysIf=Interface(iContext);
	TParse name;
	TInt err = sysIf.ResolvePath(name,_path,NULL);
	if (!err)
		err = Open(name.DriveAndPath());
	if (err)
		MapError(err,iContext->_errno);
	return err;
	}

TInt __EPOC32_WDIR::Open(const TDesC& aPath)
	{
	delete iPath;
	iPath=aPath.Alloc();		//allocate space for a hbufc and initialise it to the TDesC contents
	return Open();
	}

TInt __EPOC32_WDIR::Open()
	{
	delete iEntries;
	iEntries=0;
	iIndex=-1;
	if (iPath==0)
		return ENOMEM;
	RFs session;
	TInt err=session.Connect();
	if (err)
		return err;
	err=session.GetDir(*iPath,KEntryAttMaskSupported,ESortByName+EDescending,iEntries);
	session.Close();
	if (!err)
		iIndex=iEntries->Count();
	return err;
	}

extern "C" {

/**
Resets the position to the beginning of the directory.
@param dp directory
*/
EXPORT_C void wrewinddir (WDIR *dp)
	{
	if (dp==0)
		return;
	(void) dp->Open();	// POSIX doesn't allow for rewind failing
	}

/**
Closes the directory.
@param dp directory to close.
*/
EXPORT_C int wclosedir (WDIR *dp)
	{
	delete dp;
	return 0;
	}

/**
Opens a directory.
@return a pointer to the dir object.
This object describes the directory 
and is used in subsequent operations on the directory
If error, returns NULL.
@param _path path to the directory to be opened.
*/
EXPORT_C DIR *opendir (const char *_path)
	{
	return _opendir_r(_REENT,_path);
	}

/** A wide-character version of opendir()
*/
EXPORT_C WDIR *wopendir (const wchar_t *_path)
	{
	return _wopendir_r(_REENT,_path);
	}

/** A reentrant version of opendir().
*/
EXPORT_C DIR *_opendir_r (struct _reent *r, const char *_path)
	{
	
	wchar_t _widepath[KMaxFileName+1];
	
	if (mbstowcs(_widepath, _path, KMaxFileName) < 0)
		{
		MapError(EILSEQ,r->_errno);
		return 0;
		}
	//coverity[alloc_fn]
	//coverity[assign]

	DIR* dp = new __EPOC32_DIR(r);
	if (dp==0)
		{
		r->_errno=ENOMEM;
		return 0;
		}
	
	//coverity[leave_without_push]

	TInt err = dp->Open(_widepath);
	if (err)
		{
		delete dp;
		return 0;
		}
	return dp;
	}

/** A reentrant version of wopendir().
*/
EXPORT_C WDIR *_wopendir_r (struct _reent *r, const wchar_t *_path)
	{

	//coverity[alloc_fn]
	//coverity[assign]

	WDIR* dp = new __EPOC32_WDIR(r);
	if (dp==0)
		{
		r->_errno=ENOMEM;
		return 0;
		}
	//coverity[leave_without_push]

	TInt err = dp->Open(_path);
	if (err)
		{
		delete dp;
		return 0;
		}
	return dp;
	}

/**
Sets the index position of the directory stream specified by dp 
to the position specified by index.
*/
EXPORT_C void wseekdir(WDIR *dp,off_t index)
	{
	dp->iIndex=index;
	}

EXPORT_C off_t wtelldir(const WDIR *dp)
	{
	return dp->iIndex;
	}

EXPORT_C struct wdirent *wreaddir (WDIR *dp)
	{
	if (dp->iIndex<=0)
		return 0;
	struct wdirent *ep=&dp->iCurrent;
	const TEntry& entry=(*dp->iEntries)[--(dp->iIndex)];
	// in practice, these files must have been created as "X:\something", so they
	// can't really be longer than KMaxFileName-3
	dp->iCurrentName.Copy(entry.iName);
	dp->iCurrentName.ZeroTerminate();
	ep->d_namlen=(short)dp->iCurrentName.Length();
	ep->d_name=(wchar_t*)dp->iCurrentName.PtrZ();
	return ep;
	}

/**
Returns  a  pointer  to a dirent structure representing 
the next  directory  entry  in  the directory  stream  pointed  to be dir.
It returns NULL on reaching the end-of-file or if an error occurred.
@return a  pointer  to a dirent structure, or NULL if an error occurs
or  end-of-file  is reached.
@param dp Points to the directory stream of the directory.
*/
EXPORT_C struct dirent *readdir (DIR *dp)
	{
	return _readdir_r(_REENT, dp);
	}

/** A reentrant version of readdir().
*/
EXPORT_C struct dirent *_readdir_r (struct _reent *r, DIR *dp)
	{
	if (wreaddir(dp))
		{
		if (dp->UpdateNarrow()>=0)
			return &dp->iCurrentNarrow;
		else
			r->_errno = EINVAL;
		}

	return 0;
	}

/**
Sets the position (associated with the directory stream that dirp points to) 
to the beginning of the directory. 
Additionally, it causes the directory stream to refer to the current state of 
the corresponding directory.
@param dp Points to the directory stream of the directory to rewind.
*/
EXPORT_C void rewinddir (DIR *dp)
	{
	wrewinddir(dp);
	}

/**
closes the directory stream that dirp refers to. 
The memory associated with the directory stream is released. 
When this function returns, the value of dirp no longer point to 
an accessible object of type DIR
@return a value of zero. On failure, it returns -1
@param dp Is the directory pointer to close (from opendir()).
*/
EXPORT_C int closedir (DIR *dp)
	{
	return wclosedir(dp);
	}

/**
sets the position of the next readdir() operation on the directory 
stream specified by dp to the position specified by index
@param dp Points to the directory stream of the directory
@param index
*/
EXPORT_C void seekdir(DIR *dp,off_t index)
	{
	wseekdir(dp,index);
	}

/**
Returns the current location associated with the directory stream dir.
@return the current location in the directory stream or -1 if an error occurs. 
@param dp Points to the directory stream of the directory
*/
EXPORT_C off_t telldir(const DIR *dp)
	{
	return wtelldir(dp);
	}

} // extern "C"
