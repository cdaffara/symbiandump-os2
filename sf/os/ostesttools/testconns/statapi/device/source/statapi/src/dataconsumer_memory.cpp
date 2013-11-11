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

#include "dataconsumer_memory.h"

/*************************************************************************
 *
 * Definitions
 *
 *************************************************************************/

/*************************************************************************
 *
 * CDataConsumerMemory - Construction
 *
 *************************************************************************/
CDataConsumerMemory *CDataConsumerMemory::NewL()
	{
	CDataConsumerMemory *self = new (ELeave) CDataConsumerMemory();

	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();

	return self;
	}

CDataConsumerMemory::CDataConsumerMemory() : CBase(),
	iDataBuffer( NULL )
	{
	;
	}

void CDataConsumerMemory::ConstructL( void ) 
	{
	iLock.CreateLocal( EOwnerProcess );
	}

CDataConsumerMemory::~CDataConsumerMemory( )
	{
	iLock.Wait( );
		{
		delete iDataBuffer;
		iDataBuffer = NULL;

		iLock.Close( );
		}
	}

/*************************************************************************
 *
 * CDataConsumerMemory - Public interface
 *
 *************************************************************************/

/*************************************************************************
 *
 * Delete
 *
 *************************************************************************/

void CDataConsumerMemory::Delete( void ) 
	{
	delete this;
	}

/*************************************************************************
 *
 * GetTotalSize
 *
 *************************************************************************/

TInt CDataConsumerMemory::GetTotalSize( TInt &aTotalSize )
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
 * AddData
 *
 *************************************************************************/

TInt CDataConsumerMemory::AddData( const TDesC8 &aSource )
	{
	TInt	err = KErrNone;

	iLock.Wait( );
		{
		TInt currentLength = 0;

		if(iDataBuffer)
			{
			currentLength = iDataBuffer->Length( );
			}

		HBufC8 *currentDataBuffer = iDataBuffer;

		TInt srcLength = aSource.Length( );
		TInt length = currentLength + srcLength;

		iDataBuffer = HBufC8::New( length );

		if(iDataBuffer)
			{
				TPtr8	destPtr( iDataBuffer->Des( ) );

				if(currentDataBuffer)
					{
					destPtr.Copy( currentDataBuffer->Ptr( ),
										currentLength );
					delete currentDataBuffer;
					currentDataBuffer = NULL;
					}

				destPtr.Append( aSource.Ptr( ), srcLength );
			}
		else
			{
			err = KErrNoMemory;
			}

		iLock.Signal( );
		}

#ifdef _DEBUG
	{
	TInt totalSize = 0;
	GetTotalSize( totalSize );
	}
#endif

	return ( err );
	}

/*************************************************************************
 *
 * GetData
 *
 *************************************************************************/

TInt CDataConsumerMemory::GetData( HBufC8 &aDestination )
	{
	TInt	err = KErrNone;

	iLock.Wait( );
		{
		TInt currentLength = 0;

		if(iDataBuffer)
			{
			currentLength = iDataBuffer->Length( );

			TPtr8 destPtr( aDestination.Des( ) );
			destPtr.Copy( iDataBuffer->Ptr( ), currentLength );
			}

		iLock.Signal( );
		}

	return ( err );
	}

/*************************************************************************
 *
 * SaveData
 *
 *************************************************************************/

TInt CDataConsumerMemory::SaveData( const TDesC &filePath )
	{
	TInt	err = KErrNone;
	RFs		FsSession;

	err = FsSession.Connect( );

	if( err == KErrNone )
		{
		iLock.Wait( );
			{
			RFile	file;
			TInt	shareMode = EFileWrite | EFileStream | EFileShareExclusive;

			err = file.Open( FsSession, filePath, shareMode );

			if( err == KErrNotFound )
    			{
    			err = file.Create( FsSession, filePath, shareMode );
    			}

			if( err == KErrNone )
				{
				
				if( NULL != iDataBuffer )
					{
					err = file.Write( iDataBuffer->Des( ) );
					}

				file.Flush( );
				file.Close( );
				}

			iLock.Signal( );
			}

		FsSession.Close( );
		}

	return ( err );
	}

/*************************************************************************
 *
 * operator const TDesC8&
 *
 *************************************************************************/

CDataConsumerMemory::operator const TDesC8&( void ) const
	{
	return ( *iDataBuffer );
	}
