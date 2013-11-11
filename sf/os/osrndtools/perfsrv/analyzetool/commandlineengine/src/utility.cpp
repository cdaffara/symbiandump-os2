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
* Description:  Miscellaneous utility functions.
*
*/
#include "../inc/ATCommonDefines.h"
#include "../inc/CATBase.h"
#include "../inc/CATProject.h"

//Analyze report logging level.
#define MAX_LOG_LEVEL 3

//Function declarations.
bool CheckSBS2Folder( void );

/**
* Helper function which checks does current dir contain atool_temp\build folder which
* is used in sbs2 support
*/
bool CheckSBS2Folder( void )
{
	LOG_FUNC_ENTRY("CheckSBS2Folder");
	if ( CATBase::DirectoryExists( RAPTOR_MAKEFILE_DIR ) )
		return true;
	return false;
}
//EOF
