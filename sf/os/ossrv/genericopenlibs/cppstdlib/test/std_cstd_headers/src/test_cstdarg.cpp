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
#include <cstddef>
#include <cstdarg>
#include "test_decls.h"

/*
 * Test macros - va_arg, va_end and va_start
 * Test typedef va_list availability
 */

void foo(char* argp,...)
{
	va_list list;
	int args = 0;
	
	va_start(list, argp);
	while(argp){
		args++;
		if(args%2) {
			int i = va_arg(list, int);
		}
		else {
			char *p = va_arg(list, char*);
		}
	}
	va_end(list);
	
	CPP_TESTS_ASSERT_ALLWAYS(args != 2);
}

int E32Main()
{
	foo("1st arg", 2, "3rd arg");
	return 0;
}
