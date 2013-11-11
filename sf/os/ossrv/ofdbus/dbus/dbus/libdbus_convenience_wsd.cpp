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

 
 
 
 
 
#include <e32std.h> 
#include "libdbus_convenience_wsd_solution.h"

#include <pls.h> // For emulator WSD API 

struct libdbus_convenience_global_struct *libdbus_convenience_ImpurePtr()
{
#if defined(__WINSCW__) || defined(__WINS__)
    const TUid KLibDbusConvenienceUid3 = {0x20010154};     // This is the UID of the library
	// Access the PLS of this process
	struct libdbus_convenience_global_struct* p  = Pls<struct libdbus_convenience_global_struct>(KLibDbusConvenienceUid3, &libdbus_convenience_Init);
    return p;

#else

	return NULL;	

#endif	  
}

int libdbus_convenience_Init(libdbus_convenience_global_struct *g)
{
#if defined(__WINSCW__) || defined(__WINS__)
	  if(g)
	  {
        g->GET_DBUS_WSD_VAR_NAME(babysit_sigchld_pipe,dbus_spawn,s) = -1;
        return 0;
	  }
	  return 1;
#else
	return 1;
#endif		  
}


