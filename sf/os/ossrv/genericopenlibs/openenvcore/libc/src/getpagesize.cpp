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
* Description:  Contains the source for fchdir
 *
*/

 

#include <unistd.h>
#include<e32hal.h>

extern "C" {

/*
  The function getpagesize() returns the number of bytes in a page, 
  where "page" defines the unit of data that is brought into memory
  at once(ex: files are mapped in page-sized units).
*/

EXPORT_C int getpagesize(void)
	{
	TInt pageSize = -1;
	
	UserHal::PageSizeInBytes(pageSize);
	
	return pageSize;
	
	}
} // extern "C"
