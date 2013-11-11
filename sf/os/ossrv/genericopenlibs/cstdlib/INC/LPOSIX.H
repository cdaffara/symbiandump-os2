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

#include <f32file.h>	// for TParse
#include <sys/dirent.h>

/**
@internalComponent
*/
GLREF_C TInt GetFullPath(TParse& aParse, const TText16* upath, RFs& aSession, TDes* aFileName);
GLREF_C TInt GetFullFile(TFileName& aName, const TText16* upath, RFs& aSession);
IMPORT_C int MapError(TInt err, int& anErrno);
