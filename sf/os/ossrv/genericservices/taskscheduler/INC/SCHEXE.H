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
// Starts scheduler under ARM
// 
//

#ifndef __SCHEXE_H__
#define __SCHEXE_H__

/**
@internalComponent
*/
IMPORT_C TInt RunScheduler();

/**
starts scheduler under EPOC32
@internalComponent
@deprecated
*/
IMPORT_C TInt StartSch32();
/**
@internalComponent
@deprecated
*/
const TInt KMaxScheduleName=50;

/**
@internalComponent
@deprecated
*/
_LIT(KScheduleServerName, "!TaskScheduler");




#endif
