/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file defines various typedefs needed by the system calls that support
* the C library.  Basically, they're just the POSIX versions with an '_'
* prepended.  This file lives in the `sys' directory so targets can provide
* their own if desired (or they can put target dependant conditionals here).
* 
*
*/



/**
 @file
 @publishedAll
 @released
*/

#ifndef	_SYS__TYPES_H
#define _SYS__TYPES_H

typedef long _off_t;
typedef long _ssize_t;

#endif	/* _SYS__TYPES_H */
