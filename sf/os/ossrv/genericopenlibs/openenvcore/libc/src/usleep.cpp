// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : usleep.cpp
// Part of     : libc library
// suspends for the specified number of miroseconds.
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia. All rights are reserved. Copying, including 
// reproducing, storing, adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia.
//



#include <e32std.h>

extern "C" {

EXPORT_C int usleep (unsigned int microsecs)
	{
	// we don't allow the possibility of being woken by signals
  	User::AfterHighRes(microsecs);
	return 0;
	}

} // extern "C"
