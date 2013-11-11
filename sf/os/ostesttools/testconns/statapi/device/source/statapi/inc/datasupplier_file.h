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



 /********************************************************************************
 *
 * CDataSupplierFile
 *
 ********************************************************************************/
#ifndef __CDATASUPPLIERFILE_H__
#define __CDATASUPPLIERFILE_H__

 /*************************************************************************
 *
 * System Includes
 *
 *************************************************************************/

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

/********************************************************************************
 *
 * Local Includes
 *
 ********************************************************************************/

#include "stat_interfaces.h"

/********************************************************************************
 *
 * Types
 *
 ********************************************************************************/
class CDataSupplierFile :  public CBase, public MDataSupplier
{
public:
	// Construction and destruction
	static CDataSupplierFile *NewL( void );

	// from MDataSupplier
	void	Delete( void );
	TInt	GetTotalSize( TInt &aTotalSize );
	TInt	GetRemainingSize( TInt &aRemainingSize );

	// The data source is a string representing a file path.
	TInt 	SetData( const TDesC8 &aSource );
	TInt	GetData( HBufC8 &aDestination,
				TInt aLengthToCopy, TInt &aActuallyCopied );

private:
	CDataSupplierFile( void );
	virtual ~CDataSupplierFile();
	void ConstructL( void );

	HBufC8	*iDataBuffer;
	TInt	iCurrentLocation;

	TPath	iFilePath;
	RFs		iFsSession;

	RMutex	iLock;
};

#endif // __CDATASUPPLIERFILE_H__
