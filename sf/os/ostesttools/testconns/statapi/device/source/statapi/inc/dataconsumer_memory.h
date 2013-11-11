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
 * CDataConsumerMemory
 *
 ********************************************************************************/
#ifndef __CDATACONSUMERMEMORY_H__
#define __CDATACONSUMERMEMORY_H__

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
class CDataConsumerMemory :  public CBase, public MDataConsumer
{
public:
	// Construction and destruction
	static CDataConsumerMemory *NewL( void );

	// from MDataConsumer
	void	Delete( void );
	TInt	GetTotalSize( TInt &aTotalSize );

	// The data source is a buffer of memory which is
	// copied to this object.
	TInt	AddData( const TDesC8 &aSource );
	TInt	GetData( HBufC8 &aDestination );
	TInt	SaveData( const TDesC &filePath );
	operator const TDesC8&( void ) const;

private:
	CDataConsumerMemory( void );
	virtual ~CDataConsumerMemory();
	void ConstructL( void );

	HBufC8 *iDataBuffer;

	RMutex iLock;
};

#endif // __CDATACONSUMERMEMORY_H__
