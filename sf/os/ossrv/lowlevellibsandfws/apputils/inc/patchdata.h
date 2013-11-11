// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>

/**
This 32-bit variable is used to define the timer length for the BAFL backup session 
file lock notification re-registration timer.

This is defined as amount of time it is expected for a client application to process
a file lock notification and re-register for future notifications. If the timer expires
a backup may be initiated without all file lock changes having been processed.

The default value is 3 seconds.

This variable is changed at ROM build time using the patchdata obey keyword.
See Symbian OS Developer Library documentation on how to patch DLL data exports.

@publishedPartner
@released
*/

IMPORT_C extern const TInt KBaBackupFileLockReRegistrationTimeout;

/**
This 32-bit variable is used to define the polling frequency for the non-derived BAFL 
backup server file lock notification timer.

If a non-derived version of the BAFL backup server is to be used to change backup file 
lock states, by calling CloseAllFiles, a timer will be started to periodically check 
if all file lock notifications have been processed. If they have or the timer expires 3 
times the CloseAllFiles function call will be completed.

This variable defines the period of that timer.

The default value is 3 seconds - leading to a default maximum of 9 seconds for file lock
changes to be processed in a non-derived backup server.

This variable is changed at ROM build time using the patchdata obey keyword.
See Symbian OS Developer Library documentation on how to patch DLL data exports.

@publishedPartner
@released
*/

IMPORT_C extern const TInt KBaBackupCloseallFilesTimeout;
