// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : wfindfirst.cpp
// Part of     : libc library
// The _wfindfirst function provides information about the first 
// instance of a file name that matches the file specified in the 
// filespec argument. Any wildcard combination supported by the host 
// operating system can be used in filespec. 
// File information is returned in a _wfinddata_t structure, defined in wchar.h
// FUNCTIONS
// _wfindfirst, _wfindnext, _findclose
// INDEX
// _wfindfirst
// _wfindnext
// _findclose
// ANSI_SYNOPSIS
// #include <wchar.h>
// int _wfindnext(intptr_t, struct _wfinddata_t *); 
// intptr_t wfindfirst(const wchar_t* , struct _wfinddata_t* );
// int findclose( intptr_t handle);
// The _wfindfirst function provides information about the first 
// instance of a file name that matches the file specified in the 
// filespec argument. Any wildcard combination supported by the host 
// operating system can be used in filespec. 
// File information is returned in a _wfinddata_t structure, defined in wchar.h
// wfindnext find the next name, if any, that matches the filespec argument 
// in a previous call to _findfirst, and then alter the fileinfo structure 
// contents accordingly.
// _findclose  
// Closes the specified search handle and releases associated resources.
// RETURNS
// _findclose
// If successful, returns 0. Otherwise, it returns –1 and sets errno to ENOENT
// _wfindnext
// If successful, returns 0. Otherwise, returns –1 and sets errno to a value indicating the nature of the failure. Possible error codes are shown below. 
// EINVAL
// Invalid parameter: fileinfo was NULL. Or, the operating system returned an unexpected error.
// ENOENT
// No more matching files could be found.
// _wfindfirst
// If successful, _findfirst returns a unique search handle identifying the file or group of files
// matching the filespec specification, which can be used in a subsequent call to _findnext or to 
// _findclose. Otherwise, _findfirst returns –1 and sets errno to one of the following values.
// EINVAL
// Invalid parameter: filespec or fileinfo was NULL. Or, the operating system returned an unexpected error.
// ENOENT
// File specification that could not be matched.
// ENOMEM
// Not enough memory or the file name given was greater than MAX_PATH.
// EINVAL
// Invalid file name specification.
//




#include <stdlib.h>
#include <wchar.h>
#include <errno.h> 
#include "sysif.h"
#include<e32base.h>
#include<e32cmn.h>
#include<ltime.h>
#define MAXPATHLEN  260 
#define ATTMASK (_A_ARCH|_A_HIDDEN|_A_NORMAL|_A_RDONLY|_A_SYSTEM|_A_SUBDIR )

class CFindFileByPath : public CBase
    {
    public:
        CFindFileByPath(RFs& aFs) : iFinder(aFs),iLastCount(-1)
            {  
            }
        ~CFindFileByPath()
            {
            delete iDir;
            iDir = NULL;
            }
        const TEntry& Entry()
            {
            return (*iDir)[iLastCount];
            }
        TInt FindFirst(const TDesC&,const TDesC&);
        TInt FindNext();
    private:
        TFindFile iFinder;
        CDir* iDir;
        TInt iLastCount;
        TBuf<MAXPATHLEN> iPath;
    };

TInt CFindFileByPath::FindFirst(const TDesC &aPattern, const TDesC& aPath)
    {
    iPath = aPath;
    TInt ret = iFinder.FindWildByPath(aPattern,&iPath,iDir);
    if(ret != KErrNone)
        {
        return ret;
        }
    return FindNext();
    }

TInt CFindFileByPath::FindNext()
    {
    TInt count = iDir->Count();
//    Results from the search either in pattern path or in the cwd.
    if(++iLastCount<count)
        {
        return KErrNone;
        }
    return KErrNotFound;
    }

void UpdateFileInfo(struct _wfinddata_t* aFileinfo, CFindFileByPath& aFinder)
    {
    wcscpy(aFileinfo->name ,(wchar_t *)aFinder.Entry().iName.Ptr());
    TInt k = aFinder.Entry().iName.Size()/sizeof(wchar_t);
    aFileinfo->name[k] = L'\0';


    aFileinfo->size = aFinder.Entry().iSize;

    // Unmask unnecessary attributes in iAtt. 
    // All constant definitions are in sync with what is expected in finddata structure.
    aFileinfo->attrib = aFinder.Entry().iAtt;    
    aFileinfo->attrib &= (ATTMASK);

    time_t time_modify = as_time_t(aFinder.Entry().iModified); 
    aFileinfo->time_write = time_modify;

    aFileinfo->time_create = -1L;
    aFileinfo->time_access = -1L;

    }

extern "C" 
{
     //RFs FileServerSession ; 
EXPORT_C intptr_t wfindfirst(const wchar_t*  filespec, struct _wfinddata_t* fileinfo)
 {
   
   if(!filespec||!fileinfo)
   return EINVAL;
   
   long handle = -1;    
   wchar_t *dirf =(wchar_t*)malloc(MAXPATHLEN * sizeof(wchar_t)); // getting the cuurent directory
   wgetcwd(dirf,MAXPATHLEN);
   TPtrC16 dird((const TUint16*)dirf); // converting it into descriptor
   TPtrC16 fsd((const TUint16*)filespec );
 
   CFindFileByPath *temp1= new CFindFileByPath(Backend()->FileSession());    
   if(temp1== NULL)
       {
       return EINVAL;
       }
   int k = temp1->FindFirst(fsd,dird);
   if(k==KErrNone)
       {        
       UpdateFileInfo(fileinfo,*temp1);
       handle = reinterpret_cast<long>(temp1);
       }
   else
       {
        handle = -1;
        delete temp1;
        errno = ENOENT ;
       }
   delete dirf; // delete directory pointer 
   return handle;
 
 }


EXPORT_C intptr_t wfindnext(intptr_t handle, struct _wfinddata_t * fileinfo)
 {
        if((handle<=0)||!fileinfo)
    return EINVAL;  
    
    CFindFileByPath *temp1 = reinterpret_cast<CFindFileByPath *>(handle); 
    int k = temp1->FindNext();
    if(KErrNone==k)
    {
    UpdateFileInfo(fileinfo,*temp1);
    return 0;   
    }
    else 
    {
        errno = ENOENT ;
        return -1;
    }
 }
    
 EXPORT_C int findclose( intptr_t handle)
 {
    if(handle <=0)
    {
        errno = ENOENT ;
        return -1;
    }
    CFindFileByPath *temp1 = reinterpret_cast<CFindFileByPath *>(handle); 
    delete temp1;
    return 0;
  }
} 

