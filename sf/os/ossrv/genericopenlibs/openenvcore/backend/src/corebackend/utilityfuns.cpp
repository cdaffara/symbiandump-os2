/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include <errno.h>
#include <utf.h>
#include "sysif.h"
#include "lposix.h"

GLDEF_C TInt GetFullPath(TParse& aParse, const TText16* upath, TDes* aFileName)
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
	TBuf<KMaxDriveName+1> drive;
	TText16 c=*upath;
	TInt PathLen = WcsLen((wchar_t*) upath);

	if(!PathLen) //if null string
		{
		return KErrNotFound;
		}
	else if (PathLen >= KMaxFileName)
		{
		return ENAMETOOLONG;
		}	
	
	if (c && upath[1]==KDriveDelimiter) 
		{
		// drive name specified
		if (c==L'?')
			{
			drive.Zero();	// use "?:" to mean scan across drives
			}
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
	
	TFileName sessionPath;
	r = Backend()->GetSessionPath(sessionPath);
	if(r == KErrNone)
		{
		r = aParse.Set(drive, NULL, &sessionPath);
		}
	
	// Whole path length
	TInt pathLength = drive.Length();

	// upath now looks like a relative pathname, to be added onto
	// aParse a directory at a time. Note that '/' is not allowed in
	// EPOC32 file or directory names, so treat it as an alternative separator
	c=*upath;
	while (c && (r==KErrNone))
		{
		const TText16* ustart=upath;
		do
			{
			c=*upath++;
			}
		while (c && c!=KPathDelimiter && c!=L'/');

		TInt len=(upath-ustart)-1;		// excludes delimiter
		pathLength += len;
		if (len==0)
			{
			continue;
			}
		if (ustart[0]==L'.')
			{
			if (len==1)
				{
				continue;	// directory - ignored
				}
			if (len==2 && ustart[1]==L'.')
				{
				// directory ..
				(void) aParse.PopDir();	// just stick at root dir, no errors
				continue;
				}
			}
		if (len>=KMaxFileName)
			{
			return ENAMETOOLONG;
			}
		if (c==L'\0' && aFileName!=NULL)
			{
			// it's the trailing filename
			aFileName->Copy(TPtrC16(ustart, len));
			break;
			}
		else	
			{
			TFileName nextBit;
			// it's a component of the accumulating path
			nextBit.Copy(TPtrC16(ustart, len));
			r = aParse.AddDir(nextBit);
			//If path is too long, then return the error code
			if(r != KErrNone && pathLength > KMaxPath)
				{
				return ENAMETOOLONG;
				}
			}
		}
	return r;
	}


GLDEF_C TInt GetFullFile(TFileName& aName, const TText16* upath, RFs& aSession)
// Use GetFullPath to establish the pathname, then add the filename onto the end
	{
	TParse path;
	TInt PathLen = WcsLen((wchar_t*) upath);
	if (!PathLen) //if null string
		{
		return KErrNotFound;
		}
	else if (PathLen >= KMaxFileName)
		{
		return ENAMETOOLONG;
		}
	TInt err = GetFullPath(path,upath,&aName);
	if (err!=KErrNone)
		{
		return err;
		}
		
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
		{
		aName = path.FullName();
		}
	return err;
	}

		
GLDEF_C TInt Find_Error(const wchar_t* name, RSessionBase& aSession)  
	{
	TInt Err;
	TFileName fullName;
	RFs& fsSession = static_cast<RFs&>(aSession);

    Err=GetFullFile(fullName ,(const TText16*)name, fsSession);
    if (Err)
    	{
    	int Err1;
    	MapError(Err,Err1);
       	return Err1;
    	}

	TParsePtr parser(fullName);
	TFullName PathIter = parser.FullName();
    
	do
    	{
		TEntry entry;
		Err = fsSession.Entry(PathIter, entry);
		if (!Err)
			{
			if (!entry.IsDir())
				{
				return ENOTDIR;
				}
    	   	}
		PathIter = parser.DriveAndPath();
		} while (parser.PopDir() == KErrNone);
    	
	return ENOENT;
	}
GLDEF_C TInt Find_Error(TFileName& aName, RSessionBase& aSession)
	{
	TParsePtr parser(aName);
	TFullName PathIter = parser.FullName();
    TInt Err;
    RFs& fsSession = static_cast<RFs&>(aSession);
	do
    	{
		TEntry entry;
		Err = fsSession.Entry(PathIter, entry);
		if (!Err)
			{
			if (!entry.IsDir())
				{
				return ENOTDIR;
				}
    	   	}
		PathIter = parser.DriveAndPath();
		} while (parser.PopDir() == KErrNone);
    	
	return ENOENT;
	}

TInt Get_PathCombinations(TFileName& aOldName, TFileName& aNewName, RFs &aFs)
	{
	TEntry Ent_Opath;     //Old path entry
	TEntry Ent_Npath;     //New Path
	TInt err1, err;
    err = aFs.Entry(aOldName, Ent_Opath);
    err1 = aFs.Entry(aNewName, Ent_Npath);
    
    /* If path is protected and AllFiles capability is not present*/
    if(err == KErrPermissionDenied || err1 == KErrPermissionDenied)
    	{
    	return Permission_Denied;
    	}
	if (err == KErrNotFound || err == KErrNotReady)
		{
		return Invalid_Path_Eent;
		}
	if (err1 == KErrNotReady)
    	{
    	return Invalid_Path_Eent;
    	}
    if (err1 == KErrNotFound)
    	{
    	if(aOldName.Find(aNewName) == 0 ) //Renaming child directory to parent directoy...
		     {
		     return DirEmpty_DirNotEmpty ;
		     }
	
	    if(aNewName.Find(aNewName) == 0 ) //Renaming parent directory to child directoy...
			   {
			   return DirNotEmpty_DirEmpty;
			   }
    	}
	if (err == KErrPathNotFound)   
    	{
		if (Find_Error(aOldName, aFs) == ENOTDIR)
			{
       		return Invalid_Path_EnotDir;
       		}
       	else
       		{
			return Invalid_Path_Eent;
       		}
		}
     else if (err1 == KErrPathNotFound)
		{
     	if (Find_Error(aNewName, aFs) == ENOTDIR)
     		{
     		return Invalid_Path_EnotDir;
     		}
     	else
     		{
     		return Invalid_Path_Eent;
     		}
		}

	if (Ent_Opath.IsDir())
		{   		 	
		CDir *EntryList = NULL;
		CDir *EntryList1 = NULL;
		TBool OldDirEmpty = true;
		TBool NewDirEmpty = true;
			if (!Ent_Npath.IsDir())
			{
			return Dir_File;
			}
			if(aOldName[aOldName.Length()-1] != L'\\')
				{
				aOldName.Append(TChar(KPathDelimiter));
				}	
			if ((err = aFs.GetDir(aOldName, KEntryAttMatchMask, 0, EntryList)) != KErrNone)
			{
			if (EntryList) 
				{
			 	delete EntryList;
				}
			return err;
			} 
		 
		if (EntryList->Count() > 0)
			{
			OldDirEmpty = false;
			}
		 
		delete EntryList;
		if(aNewName[aNewName.Length()-1] != L'\\')
				{
				aNewName.Append(TChar(KPathDelimiter));
				}	
		if ((err = aFs.GetDir(aNewName, KEntryAttMatchMask, 0, EntryList1)) != KErrNone)
			{
			if (EntryList1)
				{
			 	delete EntryList1;
				}
			return err;
			} 	
		 
		if (EntryList1->Count() > 0)
			{
			NewDirEmpty = false;
		 	}
		 	
		delete EntryList1;
		if (OldDirEmpty && NewDirEmpty)
			{
			return DirEmpty_DirEmpty;
		 	}
		else if (!OldDirEmpty && NewDirEmpty)
			{
			return DirNotEmpty_DirEmpty;
		  	}
		else if (OldDirEmpty && !NewDirEmpty)
			{
		  	return DirEmpty_DirNotEmpty;
		  	}
		else if (!OldDirEmpty && !NewDirEmpty)
		  	{
			return DirNotEmpty_DirNotEmpty;
			}
		}
		else
		{ //First component is file 
		if (Ent_Npath.IsDir())  
			{
			return File_Dir;
			}
		return File_File;
		}
	return 0; // return default if error
	}


// Set errno from an E32STD.H error code or a STDLIB errno value
static const TUint8 EPOCtoERRNO[] = {
	ENOENT,   // KErrNotFound=(-1);
	  0,      // KErrGeneral=(-2);
	EINTR,    // KErrCancel=(-3);
	ENOMEM,   // KErrNoMemory=(-4);
	ENOSYS,   // KErrNotSupported=(-5);
	EINVAL,   // KErrArgument=(-6);
	ERANGE,   // KErrTotalLossOfPrecision=(-7);
	EBADF,    // KErrBadHandle=(-8);
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
	ENODEV,   // KErrUnknown=(-19);
	  0,      // KErrCorrupt=(-20);
	EACCES,   // KErrAccessDenied=(-21);
	EACCES,   // KErrLocked=(-22);
	EIO,      // KErrWrite=(-23);
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
	EINVAL,   // KErrBadDescriptor=(-38);
	  0,      // KErrAbort=(-39);
	EFAULT,      // KErrTooBig=(-40);
	EDOM,     // KErrDivideByZero=(-41);
	EDOM,     // KErrBadPower=(-42);
	ENOSPC,   // KErrDirFull=(-43);
    ENODEV,   // KErrHardwareNotAvailable=(-44);
    EPIPE,    // KErrSessionClosed=(-45);
    EACCES,   // KErrPermissionDenied=(-46);
      0,      // KErrExtensionNotSupported=(-47);
    ECOMM     // KErrCommsBreak=(-48);
	};


EXPORT_C int MapError(TInt err, int& anErrno)
	{
	if (err==0)
		{
		return err;	// i.e. return 0 without changing errno
		}
	TInt ret=err;
	if (err<KErrNone && err>=KErrCommsBreak)
		{
		ret=EPOCtoERRNO[-1-err];
		if (ret==0)
			{
			ret=err;	// no sensible translation
			}
		}
	else if (KErrWouldBlock == err)
        {
        // Map Symbian non-blocking I/O (recv + send + connect + accept) 
        // specific error code to POSIX Standard error code
        ret = EWOULDBLOCK;
        }
	else if (KErrMaxFdOpened == err)
        {
		//The process already has the maximum number of files open
        ret = EMFILE;
        }
	else if (KErrDirectoryOpen == err)
		{
		//The named file is a directory, and the arguments specify it is 
		//to be opened for writing.
        ret = EISDIR;
		}

    if (ret < 0)
        {
        // return +ve err, offset by current max errno
        ret = __EMAXERRNO + (-ret);
        }
	anErrno=ret;	// KErr* values are negative, so will be distinct anyway
	return -1;
	}


// Comparing wide character strings
int WcsCmp(const wchar_t* aDst, const wchar_t* aSrc)
	{
	while (*aDst == *aSrc++)
		{
		if (*aDst++ == 0)
			{
			return 0;
			}
		}
	return (*(const unsigned short int *)aDst - *(const unsigned short int *)--aSrc);
	}


// length wide character strings
size_t WcsLen(wchar_t* _aSrc)
	{
	const wchar_t* p = _aSrc;
	
	while (*p)
		{
		if ((p - _aSrc) > KMaxFileName)
			{
			break;
			}
		p++;
     	}
	
	return p - _aSrc;
	}	


// Copying wide character strings
wchar_t* WcsCpy(wchar_t* __restrict aDst, const wchar_t * __restrict aSrc)
	{
	wchar_t *cp;
	cp = aDst;
	while ((*cp++ = *aSrc++) != L'\0') {}
	return (aDst);
	}	

// Comparing character strings
int StrCmp(const char *aDst, const char *aSrc)
	{
	while (*aDst == *aSrc++)
		{
		if (*aDst++ == 0)
			{
			return 0;
			}
		}
	return (*(const unsigned char *)aDst - *(const unsigned char *)(aSrc - 1));
	}

// Copying 2 character strings till length aSize
size_t StrlCopy(char *aDst, const char *aSrc, size_t aSize)
	{
	char *d = aDst;
	const char *s = aSrc;
	size_t n = aSize;

	// Copy as many bytes as will fit
	if (n != 0 && --n != 0) 
		{
		do 
			{
			if ((*d++ = *s++) == 0)
				{
				break;
				}
			} while (--n != 0);
		}

	// Not enough room in dst, add NUL and traverse rest of src
	if (n == 0) 
		{
		if (aSize != 0)
			{
			*d = '\0';		// NUL-terminate dst
			}
		while (*s++) {}
		}

	return(s - aSrc - 1);	// count does not include NUL
	}

// Copying one NULL terminated string to another
char* StrCopy(char* aDst, const char* aSrc)
	{
	char *result = aDst;
	
	while(*aSrc != 0) 
		{
		*aDst = *aSrc;
		aSrc++;
		aDst++;
		}
	
	*aDst = 0;
	return result;
	}
	

/************************************************************************************
This function converts unicode test to utf8 text. The memory for the utf8 text will
allocated in this function. It is responsibility of the caller to delete the memory 
allocated here.
*************************************************************************************/
TInt ConvertUnicodeToUtf8(const TDesC16 &aUnicode,HBufC8*& aUtf8Buf,TInt &aErrNum)
	{
	TInt utf8BufLength = aUnicode.Length()*2;
	if((aUtf8Buf = HBufC8::New(utf8BufLength)) == NULL)
		{
		aErrNum = KErrNoMemory; 
		return -1;
		}
	TPtr8 remainingUtf8Buf(aUtf8Buf->Des());
	TInt remainingUcharLeft = CnvUtfConverter::ConvertFromUnicodeToUtf8(remainingUtf8Buf,aUnicode);
	
	if(remainingUcharLeft > 0)
		{
		utf8BufLength += remainingUcharLeft*4;  //Assuming max number of bytes required r 4 times unicode.
	    HBufC8 *tbuf;
		if((tbuf=aUtf8Buf->ReAlloc(utf8BufLength)) == NULL)
			{
			delete aUtf8Buf;
			aErrNum = KErrNoMemory; 
			return -1;
			}
		aUtf8Buf = tbuf;
		TInt outputLength = aUtf8Buf->Length();
		remainingUtf8Buf.Set(&(aUtf8Buf->Des())[0] + outputLength, 0, utf8BufLength - outputLength);
		remainingUcharLeft = CnvUtfConverter::ConvertFromUnicodeToUtf8(remainingUtf8Buf,aUnicode.Right(remainingUcharLeft));
		aUtf8Buf->Des().SetLength(outputLength + remainingUtf8Buf.Length());
		}
	
	if (remainingUcharLeft == 0)
		 {
		 aUtf8Buf = aUtf8Buf->ReAlloc(aUtf8Buf->Length());
		 }
	 else
		 {
		 delete aUtf8Buf;
		 aErrNum = remainingUcharLeft; //Error code
		 return -1;
		 }
	 
	 return 0;
	}

/************************************************************************************
This function gives the complete path name in utf8
*************************************************************************************/
GLDEF_C TInt GetFullFileUtf8(TDes8 &aName, const char* apath, RFs& aSession)
	{
	TPtrC8 path8((TUint8*)apath);
	TFileName fname16;	
	TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(fname16, path8);
	if (err != KErrNone)
		{
		return err;
		}
		
	TFileName fullName;
	err = GetFullFile(fullName, (TText*) fname16.PtrZ(), aSession);
	if (err != KErrNone)
		{
		return err;
		}

	err = CnvUtfConverter::ConvertFromUnicodeToUtf8(aName, fullName);
	if (err != KErrNone)
		{
		return err;
		}		
	
	return KErrNone;
	}
#ifdef SYMBIAN_OE_LIBRT
//-------------------------------------------------------------------------------
// Function Name : GeneratePathKey()
// Description   : Shall generate the shmkey for given path
//-------------------------------------------------------------------------------
TInt GeneratePathKey(const TDesC& aName)
	{
	TBuf8<KMaxFileName> name;
	name.Copy(aName);
	char* p = (char*) name.PtrZ();
    TInt key = 0;
    TInt index = 0;
    while(*p != '\0')
        {
        index++;
        if(index > 16)
            {
            index = 0;
            }
        key += ((*p << index) + *p);
        p++;
        }	
    return key;
	}
#endif //SYMBIAN_OE_LIBRT

// extern "C"-ed routines to Alloc, Free, ReAlloc and AllocTLD on the private heap
#ifdef __cplusplus
extern "C" 
{
#endif

EXPORT_C void* BackendAlloc(size_t aNBytes)
	{
	return Backend()->Alloc(aNBytes);
	}
	
EXPORT_C void* BackendReAlloc(void* aPtr, size_t aNBytes)
	{
	return Backend()->ReAlloc(aPtr, aNBytes);
	}
	
EXPORT_C void BackendFree(void* aPtr)
	{
	Backend()->Free(aPtr);
	}

EXPORT_C void* BackendAllocTLD(size_t aSize)
	{
	return Backend()->AllocTLD(aSize);
	}

#ifdef __cplusplus
} // extern "C"
#endif
