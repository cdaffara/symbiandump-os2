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
*
*/



// -----------------------------------------------------------------------------
// issetugid :  Indicates if the current executable is running setuid or setgid
// 
// This function is implemented to return 0 as there is no concept of 
// user/group IDs in symbian, for now. It is implemented to go thro' the syscall
// layer.
//
// Returns: 0: not supported for now.
// -----------------------------------------------------------------------------
//
#include <_ansi.h>

int _issetugid(void)
{
	return 0;
}

EXPORT_C
int issetugid(void)
{
	return _issetugid();
}


