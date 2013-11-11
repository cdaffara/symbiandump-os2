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

/*************************************************************************
 *
 * Local Includes
 *
 *************************************************************************/

#include "datasupplier_file.h"

/*************************************************************************
 *
 * Definitions
 *
 *************************************************************************/

/*************************************************************************
 *
 * CDataSupplierFile - Construction
 *
 *************************************************************************/
CDataSupplierFile *CDataSupplierFile::NewL()
	{
	CDataSupplierFile *self = new (ELeave) CDataSupplierFile();

	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();

	return self;
	}

CDataSupplierFile::CDataSupplierFile() : CBase(),
	iDataBuffer( NULL ), iCurrentLocation( 0 )
	{
	;
	}

void CDataSupplierFile::ConstructL( void ) 
	{
	iLock.CreateLocal( EOwnerProcess );
	}

CDataSupplierFile::~CDataSupplierFile( )
	{
	iLock.Wait( );
		{
		iCurrentLocation = 0;

		iFsSession.Close();

		iLock.Close( );
		}
	}

/*************************************************************************
 *
 * CDataSupplierFile - Public interface
 *
 *************************************************************************/

/*************************************************************************
 *
 * Delete
 *
 *************************************************************************/

void CDataSupplierFile::Delete( void ) 
	{
	delete this;
	}

/*************************************************************************
 *
 * GetTotalSize
 *
 *************************************************************************/

TInt CDataSupplierFile::GetTotalSize( TInt &aTotalSize )
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
 * GetRemainingSize
 *
 *************************************************************************/

TInt CDataSupplierFile::GetRemainingSize( TInt &aRemainingSize )
	{
	TInt	err = KErrNone;
	TInt	remaining = 0;

	iLock.Wait( );
		{
		TInt	totalSize =	0;

		err = GetTotalSize( totalSize );

		if( err == KErrNone )
			{
			remaining = totalSize - iCurrentLocation;

			aRemainingSize = remaining;
			}

		iLock.Signal( );
		}

	return ( err );
	}

/*************************************************************************
 *
 * SetData
 *
 *************************************************************************/

TInt CDataSupplierFile::SetData( const TDesC8 &aSource )
	{
	TInt	err = KErrNone;

	err = iFsSession.Connect( );

	if( err == KErrNone )
		{
		iLock.Wait( );
			{
			iFilePath.Copy( aSource );

			// Do a query on the file size to check 
			// we can access it.
			TInt totalSize = 0;
			err = GetTotalSize( totalSize );

			iCurrentLocation = 0;
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

TInt CDataSupplierFile::GetData( HBufC8 &aDestination, 
				TInt aLengthToCopy, TInt &aActuallyCopied )
	{
	TInt	err = KErrNone;

	iLock.Wait( );
		{
		TInt remainingSize = 0;
		err = GetRemainingSize( remainingSize );

		RFile	file;
		err = file.Open( iFsSession, iFilePath, EFileRead | EFileStream | EFileShareReadersOnly );

		if( err == KErrNone )
			{
			TInt toCopy = Min( aLengthToCopy, remainingSize );

			TPtr8 pBuffer( aDestination.Des() );

			err = file.Read( iCurrentLocation,
								pBuffer, toCopy);

			if( err == KErrNone )
				{
				iCurrentLocation += toCopy;
				aActuallyCopied = toCopy;
				}

			file.Close( );
			}

		iLock.Signal( );
		}

	return ( err );
	}
