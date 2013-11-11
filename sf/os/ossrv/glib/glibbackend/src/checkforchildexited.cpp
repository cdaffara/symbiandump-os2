/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32std.h>
#include <e32base.h>

extern "C" EXPORT_C int check_for_child_exited (TInt aHandle)
{
	RProcess iProcess;
	if( iProcess.Open(aHandle) < 0)
		return FALSE;
	
	TExitType exit_type = iProcess.ExitType();	
	if(exit_type == EExitTerminate || exit_type == EExitKill || exit_type == EExitPanic)
		return TRUE;
	else
		return FALSE;
}
