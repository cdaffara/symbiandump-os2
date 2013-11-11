// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <cstdlib>

/*
 * Test macros - EXIT_FAILURE & EXIT_SUCCESS availability
 * Test functions abort, atexit and exit availability
 */
void foo()
{
}

void myabort()
{
	abort(); //just checking if the compilation is successful
}

typedef void(*fptr)(void);

int E32Main()
{
	fptr p = &foo;

	if( atexit(p) != 0)
		exit(EXIT_FAILURE);

	exit(EXIT_SUCCESS);
	return 0;
}
