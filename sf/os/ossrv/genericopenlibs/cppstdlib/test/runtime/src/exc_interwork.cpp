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
// Name        : exc_interwork.cpp
// Part of     : standard c++ tests.
// 
//

#include <stdcpp_support.h>
#include <stdexcept>
#include <new>
#include "test_decls.h"

// Test that the Symbian error codes are converted into propoer 
// Cpp exceptions.
void foo()
{
	int aPass = 0;
	int TotalCases = 0;

	try{
		TotalCases++;
		TranslateSymErrorToCppException(KErrNoMemory);
	}
	catch(std::bad_alloc a)	{
		aPass++;
	}
	catch(...){
		CPP_TESTS_ASSERT_ALLWAYS(0);
	}
	try{
		TotalCases++;
		TranslateSymErrorToCppException(KErrArgument);
	}
	catch(std::invalid_argument ia)	{
		aPass++;
	}
	catch(...){
		CPP_TESTS_ASSERT_ALLWAYS(0);
	}
	
	try	{
		TotalCases++;
		TranslateSymErrorToCppException(KErrOverflow);
	}
	catch(std::overflow_error oe)	{
		aPass++;
	}
	catch(...){
		CPP_TESTS_ASSERT_ALLWAYS(0);
	}

	try	{
		TotalCases++;
		TranslateSymErrorToCppException(KErrUnderflow);
	}
	catch(std::underflow_error ue)	{
		aPass++;
	}
	catch(...){
		CPP_TESTS_ASSERT_ALLWAYS(0);
	}
	
	try{
		TotalCases++;
		TranslateSymErrorToCppException(1);
	}
	catch(Symbian_error e)	{
		aPass++;
	}
	catch(...){
		CPP_TESTS_ASSERT_ALLWAYS(0);
	}

	CPP_TESTS_ASSERT_ALLWAYS(TotalCases == aPass);
}

// Test that the exception is converted to proper Symbian error code
void bar()
{
	std::bad_alloc a;
	int x = TranslateCppExceptionToSymError(a);
	CPP_TESTS_ASSERT_ALLWAYS(x == KErrNoMemory);
}

int E32Main()
{
	int *ptr = new int(1);
	foo();
	bar();
	return 0;
}
