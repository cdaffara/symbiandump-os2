/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

 



#ifndef DBUS_SYSDEPS_UNIX_HELPER_H
#define DBUS_SYSDEPS_UNIX_HELPER_H

#include <dbus-sysdeps.h>

#ifdef __SYMBIAN32__
#ifdef __cplusplus
extern "C" {
#endif
  int AppendDir(DBusList **dirs);
  char getDaemonDriveLetter();
#ifdef __cplusplus
}; /* end of extern "C" */
#endif/* end of extern "C" */
#endif//__SYMBIAN32__
#endif//DBUS_SYSDEPS_UNIX_HELPER_H
