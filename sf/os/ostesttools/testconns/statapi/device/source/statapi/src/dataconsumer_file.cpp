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



 /*************************************************************************
 *
 * System Includes
 *
 *************************************************************************/

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
/*************************************************************************
 *
 * Local Includes
 *
 *************************************************************************/

#include "dataconsumer_file.h"
#include "stat.h"
/*************************************************************************
 *
 * Definitions
 *
 *************************************************************************/
_LIT( KTxtDiskError, "DiskError" );

/*************************************************************************
 *
 * CDataConsumerFile - Construction
 *
 *************************************************************************/
CDataConsumerFile *CDataConsumerFile::NewL()
	{
	CDataConsumerFile *self = new (ELeave) CDataConsumerFile();

	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();

	return self;
	}

CDataConsumerFile::CDataConsumerFile()
	{
	;
	}

void CDataConsumerFile::ConstructL( void ) 
	{
	
	_LIT(KFileSrvDll, "efsrv.dll");
	_LIT(KFileSeparator, "\\");
	TBuf<8> systemDrive;

	iLock.CreateLocal( EOwnerProcess );

	User::LeaveIfError( iFsSession.Connect( ) );
	
	TDriveNumber defaultSysDrive(EDriveC);
	RLibrary pluginLibrary;
	TInt pluginErr = pluginLibrary.Load(KFileSrvDll);
	
	if (pluginErr == KErrNone)
		{
		typedef TDriveNumber(*fun1)();
		fun1 sysdrive;
	
	#ifdef __EABI__
		sysdrive = (fun1)pluginLibrary.Lookup(336);
	#else
		sysdrive = (fun1)pluginLibrary.Lookup(304);
	#endif
		
		if(sysdrive!=NULL)
			{
			defaultSysDrive = sysdrive();
			}
		}
	pluginLibrary.Close();
	
	systemDrive.Append(TDriveUnit(defaultSysDrive).Name());
	systemDrive.Append(KFileSeparator);
	

	RFile	file;

	TInt err = file.Temp( iFsSession, systemDrive,iFilePath, EFileShareExclusive );

	file.Close( );

	User::LeaveIfError( err );
	}



CDataConsumerFile::~CDataConsumerFile( )
	{
	iLock.Wait( );
		{
		iFsSession.Close();

		iLock.Close( );
		}
	}

/*************************************************************************
 *
 * CDataConsumerFile - Public interface
 *
 *************************************************************************/

/*************************************************************************
 *
 * Delete
 *
 *************************************************************************/

void CDataConsumerFile::Delete( void ) 
	{
	delete this;
	}

/*************************************************************************
 *
 * GetTotalSize
 *
 *************************************************************************/

TInt CDataConsumerFile::GetTotalSize( TInt &aTotalSize )
	{
	TInt	err = KErrNone;
	TInt	size = 0;

	iLock.Wait( );
		{
		RFile	file;

		err = file.Open( iFsSession, iFilePath, EFileRead | EFileStream | EFileShareReadersOnly );

		if( err == KErrNone )
			{
			err = file.Size( size );

			if( err == KErrNone )
				{
				aTotalSize = size;
				}

			file.Close( );
			}

        iLock.Signal( );
		}

	return ( err );
	}

/*************************************************************************
 *
 * AddData
 *
 *************************************************************************/

TInt CDataConsumerFile::AddData( const TDesC8 &aSource )
	{
	TInt	err = KErrNone;

	if( err == KErrNone )
		{
		iLock.Wait( );
			{
			RFile	file;
			TInt	shareMode = EFileWrite | EFileStream | EFileShareExclusive;

			err = file.Open( iFsSession, iFilePath, shareMode );

			if( err == KErrNotFound )
    			{
    			err = file.Create( iFsSession, iFilePath, shareMode );
    			}

			if( err == KErrNone )
				{
				TInt pos = 0;
				err = file.Seek( ESeekEnd, pos );
	
				if( err == KErrNone )
					{
					err = file.Write( aSource );
					}

				file.Flush( );
				file.Close( );
				}

            if( err != KErrNone )
                {
                err = iFsSession.Delete(iFilePath);
                User::Panic(KTxtDiskError, err);
                }

			iLock.Signal( );
			}
		}

	return ( err );
	}

/*************************************************************************
 *
 * GetData
 *
 *************************************************************************/

TInt CDataConsumerFile::GetData( HBufC8 & )
	{
	return ( KErrNotSupported );
	}

/*************************************************************************
 *
 * SaveData
 *
 *************************************************************************/

TInt CDataConsumerFile::SaveData( const TDesC &filePath )
	{
	TInt	err = KErrNone;

	if( err == KErrNone )
		{
		iLock.Wait( );
			{
			
				CFileMan *theFile = CFileMan::NewL(iFsSession);
				CleanupStack::PushL(theFile);
				
				err = theFile->Copy( iFilePath, filePath );
				
				if( err == KErrNone )
					{
					err = theFile->Delete(iFilePath);	
					}
				
				CleanupStack::PopAndDestroy();

			iLock.Signal( );
			}
		}

	return ( err );
	}

/*************************************************************************
 *
 * operator const TDesC8&
 *
 *************************************************************************/

CDataConsumerFile::operator const TDesC8&( void ) const
	{
	_LIT8( empty, "" );
	return ( empty );
	}
