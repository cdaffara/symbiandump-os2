// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : spawnchild.cpp
// 
//

// INCLUDE FILES
#include <stdio.h>
#include <stdlib.h>
#include <e32std.h>

TInt main(int, char** argv)
{	
	__UHEAP_MARK; 
	//Get the open file descriptor passed to child
	int fd=atoi(argv[1]);
	//Verify whether fd is valid or not
	if (fdopen(fd, "r"))
		{
		//If valid, print pass to stdout, which should go to fd2 in parent
		printf("pass\n");	
		}
	else
		printf("fail\n");
	__UHEAP_MARKEND; 
	return 0;
}


//  End of File

