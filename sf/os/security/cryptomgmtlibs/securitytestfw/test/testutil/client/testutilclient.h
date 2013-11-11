/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* TestUtil - client interface
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#ifndef __TESTUTILCLIENT_H__
#define __TESTUTILCLIENT_H__

#include <e32std.h>
#include <f32file.h>

class RTestUtilSession : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C TInt Copy(const TDesC& aSourceFile, const TDesC& aDestinationFile);
	IMPORT_C TInt Move(const TDesC& aSourceFile, const TDesC& aDestinationFile);
	IMPORT_C TInt Delete(const TDesC& aFileName);
	IMPORT_C TInt MkDirAll(const TDesC& aFileName);
	IMPORT_C TInt RmDir(const TDesC& aFileName);
	IMPORT_C TBool FileExistsL(const TDesC& aFileName);
	IMPORT_C TBool FileExistsL(const TDesC& aFileName, TInt aMsecTimeout);

	/**
	 * Format the specified drive
	 *
	 * If the drive is NOT mounted the format will fail with error -18 KErrNotReady
	 *
	 * nb. Formating a drive looks like an MMC insertion to the SWI
	 * daemon (it does NOT look like a remove followed by an insert).
	 */
	IMPORT_C TInt FormatDrive(TInt aDrive, TBool aFormatFatTableOnly = EFalse);
	/**
	 * Mount the specified drive
	 *
	 * Mounting an already mounted drive fails with error -21 KErrAccessDenied
	 * 
	 * Mounting an unmounted drive looks exactly like a MMC card
	 * insertion to the SWI daemon.
	 *
	 * [The SWI daemon detects MMC insertion by registering with the
	 * FS server using NotifyChange(ENotifyEntry,,) for a non-existent
	 * file on the drive being watched. It then checks if it can read
	 * the Volume info to decide if media has been inserted or
	 * removed.]
	 */
	IMPORT_C TInt MountDrive(TInt aDrive);
	/**
	 * UnMount the specified drive
	 *
	 * Un-mounting an already un-mounted drive fails with error -18 KErrNotReady
	 *
	 * Unmounting an mounted drive looks exactly like a MMC card
	 * removal to the SWI daemon.
	 *
	 * Will fail if there are any open file descriptors on the drive.
	 */
	IMPORT_C TInt UnMountDrive(TInt aDrive);

	IMPORT_C TInt Lock(const TDesC& aFileName);
	IMPORT_C TInt Unlock(const TDesC& aFileName);

	/**
	 * Set or clear the read only attribute for the specified file.
	 * Set the attribut if aSetReadOnly is non-zero, clear otherwise.
	 */	
	IMPORT_C TInt SetReadOnly(const TDesC& aFileName, TInt aSetReadOnly = 1);

	/**
	 * Get a file handle opened for reading for the specified filename - used
	 * to access files in private directories.
	 */	
	IMPORT_C TInt GetFileHandle(const TDesC& aFileName, RFile &aRFile);

	/**
	 *
	 * Proxy file change notify request (RFS::NotifyChange needs AllFiles)
	 *
	 */
	IMPORT_C void WatchFile(const TDesC& aFileName, TRequestStatus& aStatus);

	/**
	 *
	 * Cancel outstanding file watch request.
	 *
	 */
	IMPORT_C void WatchFileCancelL();

	IMPORT_C TInt GetNumFilesL(const TDesC& aDirName);

	/**
	 *
	 * Set the Secure clock time.This call requires 
	 * TCB and WriteDeviceData Capability.
	 * 
	 * @param aTimeOffset  The secure clock time is 
	 *					   incremented by this value.
	 *
	 */
	IMPORT_C TInt SetSecureClock (TInt aTimeOffset);
	};

#endif
