// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef T_CENREP_HELPER_H
#define T_CENREP_HELPER_H


#include <e32def.h>
#include <e32cmn.h>
#include <f32file.h>
#include <centralrepository.h>

_LIT(KCPersistsDir,				"c:\\private\\10202BE9\\persists\\");
_LIT(KCPersistsFiles,			"c:\\private\\10202BE9\\persists\\*.*");

_LIT(KCInstallDir,				"c:\\private\\10202BE9\\");
_LIT(KCInstallFiles,			"c:\\private\\10202BE9\\*.*");

_LIT(KCRomVersionDir,				"c:\\private\\10202BE9\\romversion\\");
_LIT(KCRomVersionFiles,			"c:\\private\\10202BE9\\romversion\\*.*");

const TInt KGeneralDelay = 2*1000*1000;

_LIT(KTxtFileExt, ".txt");
_LIT(KCreFileExt, ".cre");

class RTest;

IMPORT_C void printDirL(const TDesC& aDirName);

IMPORT_C TInt KillProcess(const TDesC& aProcessName);

IMPORT_C void CleanupCDriveL(TBool aRemoveRomCache = ETrue);
IMPORT_C void CleanupFileFromCDriveL(const TUid aRepository);
IMPORT_C void CopyTestFilesL(CFileMan& aFm, const TDesC& aSrc, const TDesC& aDest);

IMPORT_C void CleanupRepositoryCache();

IMPORT_C void RecordPerformanceTimingL(TUint32 aTime);

/** This API is for internal use only and for testing purposes.
@internalComponent

Retrieves transaction state of the session.
*/
IMPORT_C TInt TransactionState(CRepository* aRep);

/** This API is for internal use only and for testing purposes.
@internalComponent

Sends EGetSetParameters to server.
*/
IMPORT_C TInt SetGetParameters(const TIpcArgs& aArgs);

#endif // T_CENREP_HELPER_H
