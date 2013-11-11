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
// 
//



#ifndef __ESTLIB_H__
#define __ESTLIB_H__

#include <e32base.h>
#include <stddef.h>

/*
Support for asynchronous ioctl
There are no guarantees about when the parameters will be processed - some implementations may
process the parameters in both the ioctl() and the ioctl_complete() functions.
Do not wait on the TRequestStatus unless ioctl() returns 0. 
*/
int ioctl (int aFid, int aCmd, void* aParam, TRequestStatus& aStatus);
int ioctl_complete (int aFid, int aCmd, void* aParam, TRequestStatus& aStatus);
int ioctl_cancel (int aFid);
int _ioctl_r (struct _reent *r, int aFid, int aCmd, void* aParam, TRequestStatus& aStatus);
int _ioctl_complete_r (struct _reent *r, int aFid, int aCmd, void* aParam, TRequestStatus& aStatus);
int _ioctl_cancel_r (struct _reent *r, int aFid);

/*
Support for crt0 variants
Do whatever is necessary to get the command line arguments, environment etc.
NB. This calls chdir(), so start the PosixServer before calling this if you want one.
*/
IMPORT_C void __crt0(int& argc, char**& argv, char**& envp);
IMPORT_C void __crt0(int& argc, wchar_t**& argv, wchar_t**& envp);

#endif /* __ESTLIB_H__ */
