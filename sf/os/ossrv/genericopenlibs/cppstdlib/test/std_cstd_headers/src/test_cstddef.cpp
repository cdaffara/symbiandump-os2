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

#include <cstddef>

char glob_c[10];
char glob_d[10];

struct my_struct {
	int		mem1;
	short	mem2;
	char	mem3[2];
	int		mem4;
};

/*
 * Test macros - NULL & offsetof availability
 * Test typedefs std::size_t and std::ptrdiff_t availability
 */
int E32Main()
{
	int *ptr = NULL;
	ptr = ptr;

	char *c = glob_c;
	char *d = glob_d;

	std::size_t	ui = 0x87654321;
	ui=ui;
	
	std::ptrdiff_t pd = c - d;
	pd=pd;
	int off = offsetof(my_struct, mem4);
	off=off;
	return 0;
}
