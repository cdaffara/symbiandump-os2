/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : stdcpp_support.h
* Part of     : standard c++ library.
* 
*
*/



#ifndef _STDCPP_SUPPORT_H_
#define _STDCPP_SUPPORT_H_

#include <exception>
//This is required for the TRAP macro
#include <e32cmn.h>
// This is required for the Symbian error numbers
#include <e32err.h>

/* A utility funtion that takes a SymbianC++ error number and throws a corresponding 
C++ exception. This mapping is done based on what is mentioned in Chapter 19.1 of 
the C++ specification and Symbian's e32err.h.
*/
IMPORT_C void TranslateSymErrorToCppException(TInt);


/* A utility function that takes an instance of std::exception and returns a
corresponding SymbianC++ error number. This mapping is done based on what is
mentioned in Chapter 19.1 of the C++ specification and Symbian's e32err.h.
*/
IMPORT_C TInt TranslateCppExceptionToSymError(const std::exception&);


/* Executes the set of C++ statements _s under a trap harness and throws a suitable  
C++ exception that matches the Symbian error code.

Use this macro as a C++ statement to translate a User::Leave

_s can consist of multiple C++ statements; in theory, _s can consist
of any legal C++ code but in practice, such statements consist of Symbian C++
function calls that may leave, e.g. FooL() or an assignment of some value to 
the result of a function call, e.g. functionValue=GetFooL().
*/
#define TRANSLATE_SYM_CPP_LEAVES(_s)			\
	{											\
		TInt err;								\
		TRAP(err, _s);							\
		if(err) TranslateSymErrorToCppException(err);	\
	}

class Symbian_error : public std::exception
{
public:
	/* A Symbian specific error can be encapsulated within this object*/
	Symbian_error(TInt e):error_code(e) {}

	inline TInt error() { return error_code; }
private:
	TInt error_code;
};


#endif //STDCPP_SUPPORT_H

