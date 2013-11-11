// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TASKFILE_H
#define TASKFILE_H

/**
Index for file server handle.  Used to retrieve task data in launched
exe, via RFile::AdoptFromCreator method.

@see RFile::AdoptFromCreator
@internalComponent
*/ 
const TInt KTaskFsHandleIndex = 1;

/**
Index for file handle.  Used to retrieve task data in launched
exe, via RFile::AdoptFromCreator method.

@see RFile::AdoptFromCreator
@internalComponent
*/ 
const TInt KTaskFileHandleIndex = 2;


#endif // TASKFILE_H
