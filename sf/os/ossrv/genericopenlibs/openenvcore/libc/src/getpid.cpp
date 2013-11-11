/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Deal with getpid() and related functions


#include <e32std.h>
#include <stdlib.h>
#include <unistd.h>



extern "C" {

pid_t _getpid ()
	{
	RProcess p;
	TProcessId i=p.Id();
	return *REINTERPRET_CAST(TUint*,&i);
	}


EXPORT_C pid_t getpid ()
	{
	return _getpid() ;
	
	}

} // extern "C"



