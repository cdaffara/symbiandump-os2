// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : uname.cpp
// Part of     : LIBC
// Contains the source for fchdir
// Version     : 1.0
//



#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <e32std.h>
#include <e32cmn.h>
#include <string.h>
#include <errno.h>

#define KValBuf	15

/*
The  uname function stores  NUL -terminated strings of information
identifying the current system into the structure referenced by buf.

The utsname structure is defined in the 

#include <sys/utsname.h> header file, and contains the following members:
sysname 	Name of the operating system implementation.
nodename 	Network name of this machine.
release 	Release level of the operating system.
version 	Version level of the operating system.
machine 	Machine hardware platform.

Upon successful completion, a non-negative value shall be returned. 
Otherwise, -1 shall be returned and errno set to indicate the error.

*/
extern "C" {
EXPORT_C int uname(struct utsname *buf)
	{
	
	char name[KMaxFileName], buffer[KValBuf];
	size_t len = KMaxFileName;
	char* retDest2 = NULL;
	char* retDest4 = NULL;
	
	if(buf)
		{
		//sysname
		char* retDest1 = strcpy(buf->sysname, "Symbian");
		
		//nodename
		int retGetHostName = gethostname(name, len);
		
		if( !retGetHostName )
			{
			 retDest2 = strncpy (buf->nodename , name, SYS_NMLN);
			 buf->nodename[SYS_NMLN-1] = '\0';
			}
		
		
		//Release
		char* retDest3 = strcpy (buf->release , "\0");
		
		//Version
		TVersion rett = User::Version();
		int retNoOfBytes = sprintf(buffer, "%d:%d:%d", rett.iMajor, rett.iMinor, rett.iBuild);
		if(retNoOfBytes)
			{
			retDest4 = strcpy (buf->version , buffer);	
			}
		
		
		//Machine
		char* retDest5 = strcpy (buf->machine , "\0");
		
		
		if((retDest1!=NULL)&&(retDest2!=NULL)&&(retDest3!=NULL)&&(retDest4!=NULL)&&(retDest5!=NULL))
			{
			return 0;	
			}
		else
			{
			errno = EINVAL;
			return -1;
			}
			
		}
	else
		{
		errno = EINVAL;
		return -1;
		}
	}
} //extern "C"
