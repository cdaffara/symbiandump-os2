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

#include "datasupplier_memory.h"

/*************************************************************************
 *
 * Definitions
 *
 *************************************************************************/

/*************************************************************************
 *
 * CDataSupplierMemory - Construction
 *
 *************************************************************************/
CDataSupplierMemory *CDataSupplierMemory::NewL()
	{
	CDataSupplierMemory *self = new (ELeave) CDataSupplierMemory();

	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();

	return self;
	}

CDataSupplierMemory::CDataSupplierMemory() : CBase(),
	iDataBuffer( NULL ), iCurrentLocation( 0 )
	{
	;
	}

void CDataSupplierMemory::ConstructL( void ) 
	{
	iLock.CreateLocal( EOwnerProcess );
	}

CDataSupplierMemory::~CDataSupplierMemory( )
	{
	iLock.Wait( );
		{
		delete iDataBuffer;
		iDataBuffer = NULL;
		iCurrentLocation = 0;

		iLock.Close( );
		}
	}

/*************************************************************************
 *
 * CDataSupplierMemory - Public interface
 *
 *************************************************************************/

/*************************************************************************
 *
 * Delete
 *
 *************************************************************************/

void CDataSupplierMemory::Delete( void ) 
	{
	delete this;
	}

/*************************************************************************
 *
 * GetTotalSize
 *
 *************************************************************************/

TInt CDataSupplierMemory::GetTotalSize( TInt &aTotalSize )
	{
	TInt	err = KErrNone;
	TInt	size = 0;

	iLock.Wait( );
		{
		size = iDataBuffer->Length( );

		aTotalSize = size;

		iLock.Signal( );
		}

	return ( err );
	}

/*************************************************************************
 *
 * GetRemainingSize
 *
 *************************************************************************/

TInt CDataSupplierMemory::GetRemainingSize( TInt &aRemainingSize )
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

TInt CDataSupplierMemory::SetData( const TDesC8 &aSource )
	{
	TInt	err = KErrNone;

	iLock.Wait( );
		{
		delete iDataBuffer;
		iDataBuffer = NULL;

		TInt length = aSource.Length( );
		iDataBuffer = HBufC8::New( length );

		if(iDataBuffer)
			{
			*iDataBuffer = aSource;
			}
		else
			{
			err = KErrNoMemory;
			}

		iCurrentLocation = 0;
		iLock.Signal( );
		}

#ifdef _DEBUG
	{
	TInt totalSize = 0;
	GetTotalSize( totalSize );
	TInt remainingSize = 0;
	GetRemainingSize( remainingSize );
	}
#endif

	return ( err );
	}

/*************************************************************************
 *
 * GetData
 *
 *************************************************************************/

TInt CDataSupplierMemory::GetData( HBufC8 &aDestination, 
				TInt aLengthToCopy, TInt &aActuallyCopied )
	{
	TInt	err = KErrNone;

	iLock.Wait( );
		{
		TInt remainingSize = 0;
		err = GetRemainingSize( remainingSize );

		if( err == KErrNone )
			{
			TInt toCopy = Min( aLengthToCopy, remainingSize );

			TPtr8 destPtr( aDestination.Des( ) );
			destPtr.Copy( iDataBuffer->Ptr( ) + iCurrentLocation,
							toCopy );

			if( err == KErrNone )
				{
				iCurrentLocation += toCopy;
				aActuallyCopied = toCopy;
				}
			}

		iLock.Signal( );
		}

	return ( err );
	}
