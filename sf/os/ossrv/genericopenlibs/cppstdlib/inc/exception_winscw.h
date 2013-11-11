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
* Name        : exception_winscw.h
* Part of     : standard c++ library.
* This file provides the required declararions/definitions some
* of the C++ exception features required of the CW runtime.
* 
*
*/



#ifndef __EXCEPTION_WINSCW_
#define __EXCEPTION_WINSCW_

extern "C" IMPORT_C bool __uncaught_exception(void);

inline bool std::uncaught_exception() throw()
  {
   /*
    * We rely on the CW runtime support to find it is an uncaught exception.
    */
  return __uncaught_exception();
  }

inline std::exception::exception() __NO_THROW
	{ }

inline std::exception::exception(const std::exception&) __NO_THROW
	{ }

inline std::exception& std::exception::operator=(const std::exception&) __NO_THROW
	{
	return *this;
	}

inline std::exception::~exception() __NO_THROW
	{ }

inline const char* std::exception::what() const __NO_THROW
	{ 
	return "class exception";
	}


inline std::bad_exception::bad_exception() __NO_THROW 
	{}

inline std::bad_exception::bad_exception(const std::bad_exception&) __NO_THROW 
	{}

inline std::bad_exception::~bad_exception() __NO_THROW 
	{}     

inline std::bad_exception& std::bad_exception::operator=(const std::bad_exception&) __NO_THROW
	{
	return *this;
	}

inline const char* std::bad_exception::what() const __NO_THROW
	{ 
	return "class bad_exception";
	}

#endif //__EXCEPTION_WINSCW_
