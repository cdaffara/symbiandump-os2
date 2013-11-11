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

#ifndef _SYS_AESELECT_H_
#define _SYS_AESELECT_H_

#include <e32base.h>
#include <sys/select.h>

IMPORT_C int aselect(int maxfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *tvptr, TRequestStatus* requeststatus);

IMPORT_C int cancelaselect(TRequestStatus* requeststatus);

IMPORT_C int eselect(int maxfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *tvptr, int numreqs, TRequestStatus* waitarray);

#endif // _SYS_AESELECT_H_