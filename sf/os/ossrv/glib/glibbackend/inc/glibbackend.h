/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


 

#ifndef __GLIBBACKEND_H__
#define __GLIBBACKEND_H__

#include <_ansi.h>
#include <unistd.h>

#define P_NOWAIT 0
#define P_WAIT 1

typedef int HANDLE;
typedef HANDLE* LPHANDLE;
typedef long DWORD;
typedef int BOOL;

#ifdef __cplusplus
extern "C"
{
#endif    /* __cplusplus */

IMPORT_C int spawnv(int mode,const char * path,const char **argv);
IMPORT_C int spawnvp(int mode,const char * path,const char **argv);
IMPORT_C BOOL CloseHandle(HANDLE handle);
IMPORT_C HANDLE GetCurrentProcess();
IMPORT_C int spawnve(int mode,const char * path,const char **argv,const char **envp);
IMPORT_C int spawnvpe(int mode,const char * path,const char **argv,const char **envp);
IMPORT_C char* applicationpath();
IMPORT_C int check_for_child_exited (int aHandle);
IMPORT_C void *pAlloc(size_t nBytes);
IMPORT_C char *getProgPath(char *progName);
IMPORT_C void pFree(void *ptr);
IMPORT_C void *pReAlloc(void *ptr,size_t nBytes);

#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif /* __GLIBBACKEND_H__ */
