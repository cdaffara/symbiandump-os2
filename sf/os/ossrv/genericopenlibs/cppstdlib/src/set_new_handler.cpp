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
// Name        : new_handler.cpp
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

_LIT(K,"set_new_handler");


using std::new_handler;

#ifdef __SYMBIAN_STDCPP_SUPPORT__ 

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

#endif // __SYMBIAN_STDCPP_SUPPORT__ 
