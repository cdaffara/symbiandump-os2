/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of pipe (POSIX Standard).
*
*/


// INCLUDE FILES
#include <sys/unistd.h>
#include "sysif.h"
#include "reent.h"
#include "sysreent.h"
#include <errno.h>

// -----------------------------------------------------------------------------
// Funcation name: pipe
// Description: It provides mechanism to flow data in one direction.
// Returns: 0     : On success
//          -1    : On error
// In case of error, errno value set to
//          EFAULT: filedes is not valid.
//          EMFILE: Too many file descriptors are in use by the process.
//          ENFILE: System limit on the total number of open files has been
//                  reached.
// -----------------------------------------------------------------------------
//

extern "C" {


EXPORT_C int pipe(int fildes[2])
    {
    return _pipe_r(&errno, fildes);
    }


} // extern "C"

//  End of File
