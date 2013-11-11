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

#include <cctype>
#include <stdio.h>

int foo(int test_id)
{
	printf("Test ID %d failed\n", test_id);
	return 1;
}

void test_ctype()
{
	int res, test_id = 0;

	res = isalnum('A')? 0: foo(++test_id);
	res += isalnum('a')? 0: foo(++test_id);
	res += isalnum('1')? 0: foo(++test_id);
	res += (!isalnum('#'))? 0: foo(++test_id);

	res += isdigit('5')? 0: foo(++test_id);
	res += isdigit('~')? 0: foo(++test_id);
	
	res += isprint('$')? 0: foo(++test_id);
	res += isprint(' ')? 0: foo(++test_id);

	res += isupper('Z')? 0: foo(++test_id);
	res += isupper('u')? 0: foo(++test_id); 

	res += (tolower('F') == 'f')? 0: foo(++test_id);

	res += isalpha('Q')? 0: foo(++test_id); 
	res += isgraph('F')? 0: foo(++test_id); 
	res += (!isgraph(' '))? 0: foo(++test_id); 

	res += ispunct(',')? 0: foo(++test_id); 
	
	res += isxdigit('E')? 0: foo(++test_id); 
	res += (!isxdigit('K'))? 0: foo(++test_id); 
	res += isxdigit('9')? 0: foo(++test_id); 

	res += (toupper('b')=='B')? 0: foo(++test_id);

	res += iscntrl('^')? 0: foo(++test_id); 
	res += islower('l')? 0: foo(++test_id); 
	res += (!islower('M'))? 0: foo(++test_id); 
	res += isspace('\t')? 0: foo(++test_id);
	res += isspace('\n')? 0: foo(++test_id);
	res += isspace(' ')? 0: foo(++test_id);
	res += (!isspace('p'))? 0: foo(++test_id);

	if(res==0)
		printf("All %d Tests passed\n", test_id);
}

int E32Main()
{
	test_ctype();

	return 0;
}
