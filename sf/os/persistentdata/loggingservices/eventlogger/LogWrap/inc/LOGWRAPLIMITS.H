// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LOGWRAPLIMITS_H
#define LOGWRAPLIMITS_H

#include <e32base.h>
#include <logwrapconst.h>

/**
String lengths
@publishedAll
@released
*/
const TInt KLogMaxRemotePartyLength = 64;
const TInt KLogMaxDirectionLength = 64;
const TInt KLogMaxStatusLength = 64;
const TInt KLogMaxSubjectLength = 64;
const TInt KLogMaxDescriptionLength = 64;
const TInt KLogMaxSharedStringLength = 64;
const TInt KLogMaxNumberLength = 100;

/**
Limits
@publishedAll
@released
*/
   
const TLogId KLogNullId = -1;
const TLogDurationType KLogNullDurationType = -1;
const TLogDuration KLogNullDuration = 0;
const TLogLink KLogNullLink = 0;
const TLogRecentList KLogNullRecentList = -1;
const TLogStringId KLogNullStringId = -1;
const TLogTypeId KLogNullTypeId = -1;
const TInt KLogFlagsCount = 4;
const TInt KLogNeverUsedId = -2;

#endif
