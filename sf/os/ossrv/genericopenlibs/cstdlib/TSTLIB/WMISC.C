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
* Test code for bsearch, qsort etc. These are all functions which work directly
* without using the MSystemInterface - as such none of them use errno to indicate
* errors.
* 
*
*/



#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
	{
	int i;

	printf("  argc=%d\r\n", argc);
	for (i=0; i<argc; i++)
		{
		printf("  argv[%d]=\"%s\" length %d\r\n", i, argv[i], strlen(argv[i]));
		}
	return 0;
	}
