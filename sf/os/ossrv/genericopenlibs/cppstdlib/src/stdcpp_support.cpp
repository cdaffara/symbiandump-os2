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
// Name        : stdcpp_support.cpp
// Part of     : standard c++ library.
// 
//

#include <new>
#include <stdcpp_support.h>
#include <typeinfo>
#include <string>
#include <stdexcept>


EXPORT_C void TranslateSymErrorToCppException(TInt aError)
{
	switch(aError)
	{
	case KErrNoMemory:
		// This is about to use the emergency buffer!
		__THROW(std::bad_alloc());
	case KErrArgument:
		__THROW(std::invalid_argument("Invalid argument"));
	case KErrOverflow :
		__THROW(std::overflow_error("Overflow error"));
	case KErrUnderflow:
		__THROW(std::underflow_error("Underflow error"));
	default:
		__THROW(Symbian_error(KErrGeneral));
	}
}

EXPORT_C TInt TranslateCppExceptionToSymError(const std::exception& aThrow)
{
#ifndef __GCCXML__
	const std::type_info& atype = typeid(aThrow);

	if(atype == typeid (std::bad_alloc))
		return KErrNoMemory;
	else if(atype == typeid(std::invalid_argument))
		return KErrArgument;
	else if(atype == typeid(std::out_of_range))
		// std::out_of_range is of type logic_error which by definition means that it is
		// "presumably detectable before the program executes".
		// std::out_of_range is used to report an argument is not within the expected range.
		// The description of KErrArgument says an argument is out of range. Hence the mapping.
		return KErrArgument;
	else if(atype == typeid(std::overflow_error))
		return KErrOverflow;
	else if(atype == typeid(std::underflow_error))
		return KErrUnderflow;
	else
		return KErrGeneral;
#else
	return KErrGeneral;
#endif
}

#ifdef __WINSCW__
/*
 * The StdC++ global new and delete operators defined here. They are done only for the emulator
 * as for EABI platforms, it is part of the runtime support(i.e., stdnew.dll).
 */
#include <e32std.h>

EXPORT_C const std::nothrow_t std::nothrow;

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

EXPORT_C void* operator new[](std::size_t a_size, const std::nothrow_t) __NO_THROW
	{
    return ::operator new(a_size, std::nothrow);
	}

// Symbian-specific addition.
EXPORT_C void* operator new(std::size_t a_size, std::size_t a_extra_size) __NO_THROW
	{
    return ::operator new(a_size + a_extra_size, std::nothrow);
	}

EXPORT_C void operator delete(void* p) __NO_THROW
	{
	User::Free(p);
	}

EXPORT_C void operator delete(void* p, const std::nothrow_t&) __NO_THROW
	{
	User::Free(p);
	}

EXPORT_C void operator delete[](void* p) __NO_THROW
	{
	User::Free(p);
	}

EXPORT_C void operator delete[](void* p, const std::nothrow_t&) __NO_THROW
	{
	User::Free(p);
	}

// Symbian-specific addition.
EXPORT_C void operator delete(void* p, std::size_t) __NO_THROW
	{
	User::Free(p);
	}
_LIT(K,"set_new_handler");


using std::new_handler;

EXPORT_C new_handler std::set_new_handler(new_handler a_new_nh) __NO_THROW
	{
	new_handler current_nh;
	TAny* tls_word_p = Dll::Tls();

	if (!tls_word_p)
		{
		// This is the first time we're called, so we need to set up the TLS.

		tls_word_p = User::Alloc( sizeof(new_handler) );
		if ( !tls_word_p )
			{
			User::Panic(K, KErrNoMemory);
			}

		Dll::SetTls(tls_word_p);
		current_nh = 0;
		}
	else
		{
		// Get the currently installed new_handler function.
		current_nh = *reinterpret_cast<new_handler*>(tls_word_p);
		}
	
	// Set the new new_handler.
	*reinterpret_cast<new_handler*>(tls_word_p) = a_new_nh;

	return current_nh;
	}

#endif //__WINSCW__
