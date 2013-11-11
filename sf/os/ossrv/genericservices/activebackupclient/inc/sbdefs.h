/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of Types and Constants for Secure Backup and Restore
* 
*
*/



/**
 @file
*/
#ifndef __SBDEFS_H__
#define __SBDEFS_H__

#include <e32std.h>
#include <f32file.h>

namespace conn
	{
	/** Secure Backup and Restore publish and subscribe key

	Secure Backup and Restore uses a Publish and Subscribe key to publish
	the current backup / restore state.
 
	The key is a system key so the category to be used when subscribing is KUidSystemCategoryValue
	The key to be used is KUidBackupRestoreKey.
	The value is a bit-wise OR of a TBURPartType value and a TBackupIncType value
	This means that a normal state (i.e. non-backup or restore) is EBURNormal | ENoBackup but
	If the key has not been set (i.e. == 0) then no backup or restore is in progress either

	@released
	@publishedAll
	*/
	const TUint KUidBackupRestoreKey = 0x10202792 ;


	/** TBURPartType Bitmask
	@released
	@publishedAll
	*/
	const TUint KBURPartTypeMask = 0xff;

	enum TBURPartType
		/** 
		The backup / restore state of a device.

		@released
		@publishedAll
		*/
		{
		EBURUnset          = 0x00000000, /*!< 0x00000000: The backup / restore mode has not been set */
		EBURNormal         = 0x00000001, /*!< 0x00000001: The device as a whole is not engaged in a backup or restore */
		EBURBackupFull     = 0x00000002, /*!< 0x00000002: The device as a whole is engaged in a backup of all components data */
		EBURBackupPartial  = 0x00000004, /*!< 0x00000004: The device as a whole is engaged in a backup of selected components data */
		EBURRestoreFull    = 0x00000008, /*!< 0x00000008: The device as a whole is engaged in a restore of all components data */
		EBURRestorePartial = 0x00000010  /*!< 0x00000010: The device as a whole is engaged in a restore of all components data */
		};

	/** TBackupIncType Bitmask
	@released
	@publishedAll
	*/
	const TUint KBackupIncTypeMask = 0xffffff00;

	enum TBackupIncType
		/** 
		The type of a backup.

		@released
		@publishedAll
		*/
		{
		ENoBackup        = 0x00000000, /*!< 0x00000000: No backup is in progress */
		EBackupBase      = 0x00000100, /*!< 0x00000100: The backup is a base backup, i.e. it includes all files / data */
		EBackupIncrement = 0x00000200  /*!< 0x00000200: The backup is an incremental backup, i.e. it includes only files / data that have changed since a previous backup */
		};

	} // end namespace
#endif
