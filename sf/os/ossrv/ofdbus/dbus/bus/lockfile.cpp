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
#include <f32file.h>
#include <fcntl.h>
#include <errno.h>
#include "dbus-bus.h"
#include <dbus/config.h>
#include <unistd.h> 


extern "C"
{
	
void lock_file()
{
					//code to lock the file 
					TBuf8<30>buf;
   		 			buf.Copy((const TUint8 *)DBUS_LOCK_FILE);	
   					TBuf16<30>buf16;
   					buf16.Copy(buf);	
    				RFs rf;
    				User::LeaveIfError(rf.Connect());
    				RFile fds;
    				User::LeaveIfError(fds.Open(rf,buf16,EFileShareAny));
    				fds.Lock(0,10);
}

} //end extern "C"
