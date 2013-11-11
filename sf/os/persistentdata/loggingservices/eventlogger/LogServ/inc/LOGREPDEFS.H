// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// LOGADD.H
// 
//

#ifndef __LOGREPDEFS_H__
#define __LOGREPDEFS_H__

#include "logcntdef.h"

const TUid KUidLogengRepository = { 0x101f401d }; //The repository for Log engine
const TInt KMaxLogSizeRepKey = 1;
const TInt KMaxRecentLogSizeRepKey = 2;
const TInt KMaxEventAgeRepKey = 3;
const TInt KContactMatchCountRepKey = 4;
const TInt KContactNameFormatRepKey = 5;

/**
Default contact matching digits count. 
If the LogEng repository cannot be open, this is the value which will be used by the LogEng server. 
@internalAll
*/
const TInt KLogContactMatchCount = 7;

/**
Default contact name format. 
If the LogEng repository cannot be open, this is the value which will be used by the LogEng server. 
@internalAll
*/
const TLogContactNameFormat KLogContactNameFormat = ELogWesternFormat;

#endif
