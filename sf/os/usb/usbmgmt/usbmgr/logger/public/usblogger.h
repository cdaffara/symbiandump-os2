/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
*/


#ifndef USBLOGGER_H
#define USBLOGGER_H

#include <e32base.h>

#define LEAVEIFERRORL(err, trace) \
{ \
TInt munged_err=(err); \
if (munged_err < 0) \
{ \
trace; \
User::Leave(munged_err);\
}\
}

#endif  // USBLOGGER_H

