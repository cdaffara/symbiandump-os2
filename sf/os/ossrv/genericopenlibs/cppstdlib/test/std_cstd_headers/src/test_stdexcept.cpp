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

#include <stdexcept>
#include <string>
#include <stdio.h>
/*
 * Checking for the availability of all logic_error and runtime_error 
 * exception classes as mentioned in Chapter 19.1
 */

#define MAX_VAL	9

void bar(int code)
{
	std::string str;
	switch(code)
	{
	case 0:
		str ="logic_error";
		throw std::logic_error(str);
	case 1:
		str = "domain_error";
		throw std::domain_error(str);
	case 2:
		str = "invalid_argument";
		throw std::invalid_argument(str);
	case 3:
		str = "length_error";
		throw std::length_error(str);
	case 4:
		str = "out_of_range";
		throw std::out_of_range(str);
	case 5:
		str = "runtime_error";
		throw std::runtime_error(str);
	case 6:
		str = "range_error";
		throw std::range_error(str);
	case 7:
		str = "overflow_error";
		throw std::overflow_error(str);
	case 8:
		str = "underflow_error";
		throw std::underflow_error(str);
	default:
		break;
	}
}

const char* foo(int code)
{
	try {
		bar(code);
	}catch (std::exception e) {
		return e.what();
	}
	return 0;
}

int E32Main()
{
	for(int i = 0; i < MAX_VAL; i++)
		printf("%s\n", foo(i));
	return 0;
}
