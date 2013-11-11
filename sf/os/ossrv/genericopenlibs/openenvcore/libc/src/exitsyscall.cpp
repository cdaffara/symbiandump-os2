/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/
/*
* ==============================================================================
*  Name        : exitsyscall.cpp
*  Part of     : libc library
*  Description : terminates the process.
*
* ============================================================================

*/

#include "sysreent.h"

// -----------------------------------------------------------------------------
// _exit :  terminates the current process.
// 
// This function is implemented to go thro' the syscall layer.
//
// Returns: nothing.
// -----------------------------------------------------------------------------
//

extern "C" {

EXPORT_C
void _exit(int code)
	{
	_exit_r(code);
	}

} // extern "C"
