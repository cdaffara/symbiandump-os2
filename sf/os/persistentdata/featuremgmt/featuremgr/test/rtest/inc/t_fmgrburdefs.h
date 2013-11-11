// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent
*/


#if (!defined __T_FMGRBURDEFS_H__)
#define __T_FMGRBURDEFS_H__

#include <connect/sbdefs.h>

const TUid KTestUid = TUid::Uid( 0x10283709 );

#ifndef EXTENDED_FEATURE_MANAGER_TEST
	// for UID 0x10205054
	_LIT( KEfmBackupFileName,  "C:\\Private\\10205054\\features.dat" );	
	_LIT( KEfmApplicationDir,  "C:\\Private\\10205054\\" );	
	_LIT( KRegistrationFile,   "Z:\\private\\10205054\\backup_registration.xml" );
#else
	// for UID 0x102836E5 0x102836E5
	_LIT( KEfmBackupFileName,  "C:\\Private\\102836E5\\runtime\\features.dat" );	
	_LIT( KEfmApplicationDir,  "C:\\Private\\102836E5\\" );	
	_LIT( KRegistrationFile,   "Z:\\private\\102836E5\\backup_registration.xml" );
#endif 
	
#endif
