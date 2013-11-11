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
// Name        : exception.cpp
// Part of     : standard c++ library.
// 
//

#include <stdexcept>
#include <typeinfo>

namespace std
{
	EXPORT_C exception::exception() __NO_THROW  {}
	EXPORT_C exception::exception(const std::exception&) __NO_THROW  {}
	EXPORT_C exception::~exception() __NO_THROW  {}
	EXPORT_C const char* exception::what() const __NO_THROW  {return "exception";}
	EXPORT_C exception& exception::operator=(const exception&) __NO_THROW  { return *this; }
}	
	
	EXPORT_C std::bad_exception::bad_exception() __NO_THROW 
		{}

	EXPORT_C std::bad_exception::bad_exception(const std::bad_exception&) __NO_THROW 
		{}

	EXPORT_C std::bad_exception::~bad_exception() __NO_THROW 
		{}     

	EXPORT_C std::bad_exception& std::bad_exception::operator=(const std::bad_exception&) __NO_THROW
		{
		return *this;
		}

	EXPORT_C const char* std::bad_exception::what() const __NO_THROW
		{ 
		return "class bad_exception";
		}



