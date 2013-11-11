/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for getdirentries API
*
*/


#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "netdb_r.h"


EXPORT_C int getdirentries(int x, char * ptr, int y, long * lptr)
{
	if(lptr)
		*lptr = lseek(x,(off_t) 0,SEEK_CUR);
	return read(x,ptr,y);
}

