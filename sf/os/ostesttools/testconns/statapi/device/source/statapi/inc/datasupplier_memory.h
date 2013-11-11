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
 * CDataSupplierMemory
 *
 ********************************************************************************/
#ifndef __CDATASUPPLIERMEMORY_H__
#define __CDATASUPPLIERMEMORY_H__

 /*************************************************************************
 *
 * System Includes
 *
 *************************************************************************/

#include <e32std.h>
#include <e32base.h>

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
class CDataSupplierMemory :  public CBase, public MDataSupplier
{
public:
	// Construction and destruction
	static CDataSupplierMemory *NewL( void );

	// from MDataSupplier
	void	Delete( void );
	TInt	GetTotalSize( TInt &aTotalSize );
	TInt	GetRemainingSize( TInt &aRemainingSize );

	// The data source is a buffer of memory which is
	// copied to this object.
	TInt 	SetData( const TDesC8 &aSource );
	TInt	GetData( HBufC8 &aDestination,
				TInt aLengthToCopy, TInt &aActuallyCopied );
	// TInt	GetDataPtr( TPtrC8 &aDestination );

private:
	CDataSupplierMemory( void );
	virtual ~CDataSupplierMemory();
	void ConstructL( void );

	HBufC8 *iDataBuffer;
	TInt iCurrentLocation;

	RMutex iLock;
};

#endif // __CDATASUPPLIERMEMORY_H__
