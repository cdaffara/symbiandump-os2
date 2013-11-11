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
// Support routines for managing paths and directories
// provided by the LPOSIX.LIB component of STDLIB
// 
//

#ifndef LPOSIX_H
#define LPOSIX_H

#include <f32file.h>	// for TParse
#include <sys/dirent.h>

/*
@internalComponent
*/

/*
 various combinations that one can get  as parameters for link , rename system call
 */
 
#ifdef SYMBIAN_OE_LIBRT
#define SHM_CHUNKSIZE 4096
#endif //SYMBIAN_OE_LIBRT


enum Path_Combinations   
	{
	DirEmpty_DirEmpty= 0    ,
	DirEmpty_DirNotEmpty    ,
	DirEmpty_File	        ,
	DirNotEmpty_DirEmpty    ,
	DirNotEmpty_DirNotEmpty ,
	DirNotEmpty_File        ,
	Dir_NotEmpty            ,
	File_Dir                ,
	File_File               ,
	Dir_File                ,
	Invalid_Path_EnotDir    ,
	Invalid_Path_Eent       ,
	Permission_Denied		,
	} ; 
	  
	
GLREF_C TInt GetFullPath(TParse& aParse, const TText16* upath, TDes* aFileName);
GLREF_C TInt GetFullFile(TFileName& aName, const TText16* upath, RFs& aSession);
GLDEF_C TInt Find_Error(const wchar_t *name, RSessionBase& aSession) ; 
GLDEF_C TInt Find_Error(TFileName& aName, RSessionBase& aSession) ;
IMPORT_C int MapError(TInt err, int& anErrno);
int StrCmp(const char *aDst, const char *aSrc);
wchar_t* WcsCpy(wchar_t* __restrict aDst, const wchar_t * __restrict aSrc);
int WcsCmp(const wchar_t* aDst, const wchar_t* aSrc);
size_t WcsLen(wchar_t*  aSrc);
size_t StrlCopy(char *aDst, const char *aSrc, size_t aSize);
char* StrCopy(char* aDst, const char* aSrc);
GLDEF_C TInt Get_PathCombinations(TFileName& aOldName, TFileName& aNewName ,RFs & aSession) ; 
#ifdef SYMBIAN_OE_LIBRT
TInt GeneratePathKey(const TDesC& aName);
#endif //SYMBIAN_OE_LIBRT
TInt ConvertUnicodeToUtf8(const TDesC16 &aUnicode,HBufC8*& aUtf8Buf,TInt &aErrNum);
GLDEF_C TInt GetFullFileUtf8(TDes8 &aName, const char* apath, RFs& aSession);
#endif /* LPOSIX_H */
