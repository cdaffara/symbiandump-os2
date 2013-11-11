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

// All the patchable variables in the BAFL DLLs need to be in a separate file.
// Care should be taken not to define the exported data in the same source file 
// in which they are referred, lest the compiler inline's it. If inlined, the 
// data cannot be patched. Hence, the definition of data must be put in a 
// separate source file and they must be declared in other source files by 
// #including "patchdata.h".

EXPORT_C extern const TInt KBaBackupFileLockReRegistrationTimeout = 3000000;

EXPORT_C extern const TInt KBaBackupCloseallFilesTimeout = 3000000;

