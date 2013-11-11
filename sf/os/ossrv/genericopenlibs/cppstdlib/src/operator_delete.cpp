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
// Name        : operator_delete.cpp
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

#endif // __SYMBIAN_STDCPP_SUPPORT__ 
