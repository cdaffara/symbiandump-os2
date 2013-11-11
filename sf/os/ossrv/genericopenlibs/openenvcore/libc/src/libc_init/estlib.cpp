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
// 
//

#include <stdlib.h>
#include <unistd.h> //environ
#include <string.h>
#include <stdio.h>
#include <sys/syslimits.h>
#include <time.h>
#include "sysreent.h"
#include "mmapcontrollernode.h"
#include "reent.h"
#include "sysif.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#include "localeinfo.h"
#include "localetlsinfo.h"
#endif

#ifdef __EPOC32__
class XLibcTLSCleanup
	{
public:
	~XLibcTLSCleanup()
		{
		Dll::FreeTls();
		}
	};

// Create a global here. It's destructor invoked on libc unload will reset libc's TLS
XLibcTLSCleanup gLibcTLSCleanup;
#endif

#ifdef __WINSCW__ 
#include <pls.h> // For emulator WSD API
const TUid KLibcUid3 = {0x10207369};  
#endif //__WINSCW__ 

// Access for the outside world
extern "C" {

//Definition of environ variable and __progname which will be used by other module as extern
#ifndef EMULATOR
char** environp;
const char* __progname;
#else //EMULATOR

GET_GLOBAL_VAR_FROM_TLS(environp, char **)
#define environp (*GET_WSD_VAR_NAME(environp, g)())
const char** progname(void)
	{
	return &(GetGlobals()->_g___progname);	
	}
#endif //EMULATOR
void GetTmpDirName();
// -----------------------------------------------------------------------------
// SetupEnviron
// This function sets up the environment by populating the per process variable 
// environ with a list strings of the form "name=value".
// -----------------------------------------------------------------------------
//
void SetupEnviron()
	{
	const TUint KPathSize = 15;
	const TUint KNumEnvironVars = 3;
	environp = (char**)Backend()->Alloc(KNumEnvironVars * sizeof(char *));
	if (!environp)
		return;
	
	int idx = 0;
	char* path = (char*)Backend()->Alloc(KPathSize);
	if (path)
		{
	strcpy(path, "home=\\home");
		environp[idx] = path;
		++idx;
		}
	//get private path here
	GetTmpDirName();
	// Insert other default environ vars here
	char* tmpfilepath = (char*)Backend()->Alloc(PATH_MAX);
	if(tmpfilepath == 0)
		return;
	strcpy(tmpfilepath, "TMPDIR=");
	strcat(tmpfilepath, __tmpdirptr);
	if (tmpfilepath)
		{
		environp[idx] = tmpfilepath;
		++idx;
		}
	environp[idx] = NULL;
	}
// -----------------------------------------------------------------------------
// GetTmpDirName
// This API is used for get path of private directory of user
// -----------------------------------------------------------------------------
//
void GetTmpDirName()
{
	TFileName file;
	TInt r = KErrNone;
	RFs iTestSession;
	TBuf8<PATH_MAX> buf1;
	TInt len;
	file.Append(iTestSession.GetSystemDriveChar());
	file.Append(TChar(KDriveDelimiter));
	file.Append(TChar(KPathDelimiter));
	file.Append(_L("Private"));
	__tmpdirptr = (char*)Backend()->Alloc(PATH_MAX);
	if(__tmpdirptr == NULL)
	{
		User::Panic(_L("LIBRARY-INIT"), KErrNoMemory);
	}
	r = iTestSession.Connect(); 
    if (r != KErrNone)
        {
            User::Panic(_L("Connect failed"), r);
        }
	iTestSession.SessionPath(file);		
	buf1.Copy(file);
	iTestSession.Close();
	
	len=buf1.Length();
	strncpy(__tmpdirptr,(char*) buf1.Ptr(),len);
	__tmpdirptr[len] = '\0';
	strcat(__tmpdirptr, "tmp");
}

#ifdef __WINSCW__
struct _libc_wsd* GetGlobals()
	{
	 // Access the PLS of this process 
	_libc_wsd* p = Pls<_libc_wsd>(KLibcUid3, &InitWSDVar);
	return p;
	}
#endif //__WINSCW__

// -----------------------------------------------------------------------------
// ImpurePtr
// This exported API is used for fetching reent structure for TLS
// This also does the initialization of all libc specific variables
// on the first call
// -----------------------------------------------------------------------------
//
struct _reent* ImpurePtr(void)
	{
#ifdef __WINSCW__
	GetGlobals();
#endif // __WINSCW__
	//Create Reent Structure for this thread if Dll::Tls is NULL
	struct _reent* p=(struct _reent*)Dll::Tls();
	if (p)
		return p;

	// First use, so construct the default struct _reent
	// Do the allocation in backend's private heap
	p = (struct _reent*)BackendAllocTLD( sizeof(struct _reent) );
	if (p==NULL)
		User::Panic(KEstlibInit, KErrNoMemory);
	
	TInt err = Dll::SetTls(p);
	Mem::FillZ( p, sizeof(struct _reent) );
	return p;
	}

EXPORT_C int* __errno(void)
	{
	return &(ImpurePtr()->_errno);
	}

EXPORT_C char*** __environ(void)
	{
	if(!environp)
		{
		SetupEnviron();
		}
		
	return &environp;
	}
int GetStdOffset(void)
	{
	TTimeIntervalSeconds t = User::UTCOffset();
	return t.Int();
	}
	
int GetIsDst(void)
	{
	TLocale iLocale;
	if(iLocale.QueryHomeHasDaylightSavingOn())
		return 1;
	return 0;
	}


#ifndef __WINSCW__
extern char **	_tzname;
#else	
GET_GLOBAL_VAR_FROM_TLS(_tzname, char **)
#define _tzname (*GET_WSD_VAR_NAME(_tzname, g)())
#endif	

// -----------------------------------------------------------------------------
// SetupTzname
// This function sets up the Tzname variable by populating the per process variable 
// _tzname with a list strings .
// -----------------------------------------------------------------------------
//
void SetupTzname()
	{
	const TUint KNumTzVars = 2;
	_tzname = (char**)Backend()->Alloc(KNumTzVars * sizeof(char *));
	if (!_tzname)
		return;
	_tzname[0] = "WILDABBR";
	_tzname[1] = "WILDABBR";
	}


EXPORT_C char*** __tzname(void)
	{
		if(!_tzname)
		{
		SetupTzname();
		}
		
		return &_tzname;	
	} 

} // extern "C"

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
//
// DLL entry point
//
	{
	return KErrNone;
	}
#endif
