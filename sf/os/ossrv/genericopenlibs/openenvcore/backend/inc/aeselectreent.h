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
#ifndef _AESELECTREENT_H_
#define _AESELECTREENT_H_

// System Includes
// This has not been included in sysreent.h as this requires e32cmn.h to be included, which cannot be done in .c sources
// This new file allows us to prevent the use of void* in the formal parameter of aselect and eselect
#include <e32cmn.h>
#include <sys/select.h>

IMPORT_C int _aselect_r(int *aErrno, int maxfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *tvptr, TRequestStatus* requeststatus);

IMPORT_C int _cancelaselect_r(int *aErrno, TRequestStatus* requeststatus);

IMPORT_C int _eselect_r(int *aErrno, int maxfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *tvptr,int numreqs, TRequestStatus* waitarray);

#endif // _AESELECTREENT_H_