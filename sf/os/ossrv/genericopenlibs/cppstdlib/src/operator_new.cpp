// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : operator_new.cpp
// Part of     : libstdcpp
// Adaptation layer to get locale functionality.
// Version     : 
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing, adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//


#include "new"
#include <e32std.h>


#ifdef __SYMBIAN_STDCPP_SUPPORT__ 

EXPORT_C void* operator new(std::size_t a_size) __THROW(std::bad_alloc)
	{
	for (;;)
		{
		void* p = User::Alloc(a_size);

		if (p)
			{
			return p; 
			}

		std::new_handler nh_func ;
		std::new_handler *ptr = reinterpret_cast<std::new_handler*>(Dll::Tls());
		nh_func = (ptr)?(*ptr):NULL;

		if (nh_func)
			{
			nh_func();
			}
		else
			{
			__THROW(std::bad_alloc());
			}
		}
	}

EXPORT_C void* operator new[](std::size_t a_size) __THROW(std::bad_alloc)
	{
	return ::operator new(a_size);
	}

EXPORT_C void* operator new(std::size_t a_size, const std::nothrow_t&) __NO_THROW
	{
	for (;;)
		{
		void* p = User::Alloc(a_size);

		if (p)
			{
			return p;
			}

		std::new_handler nh_func ;
		std::new_handler *ptr = reinterpret_cast<std::new_handler*>(Dll::Tls());
		nh_func = (ptr)?(*ptr):NULL;

		if (nh_func)
			{
			try
				{
				nh_func();
				}
			catch (...)
				{
				return 0;
				}
			}
		else
			{
			return 0;
			}
		}
	}

EXPORT_C void* operator new[](std::size_t a_size, const std::nothrow_t&) __NO_THROW
	{
	return ::operator new(a_size, std::nothrow);
	}

// Symbian-specific addition.
EXPORT_C void* operator new(std::size_t a_size, std::size_t a_extra_size) __NO_THROW
	{
	return User::Alloc(a_size + a_extra_size);
	}

#ifdef __EABI__
//TODO: MOVE to another file
EXPORT_C std::bad_alloc::bad_alloc() __NO_THROW 
	{}
EXPORT_C std::bad_alloc::bad_alloc(const std::bad_alloc&) __NO_THROW
	{}
EXPORT_C std::bad_alloc& std::bad_alloc::operator=(const std::bad_alloc&) __NO_THROW
	{ return *this;}
EXPORT_C std::bad_alloc::~bad_alloc() __NO_THROW
	{}
EXPORT_C const char* std::bad_alloc::what() const __NO_THROW
    { return "bad_alloc";}
#endif  // __EABI__

#endif // __SYMBIAN_STDCPP_SUPPORT__ 
