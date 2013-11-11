/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYDRIVERLOGICALCHANPROCESSINSPECTION_H
#define MEMSPYDRIVERLOGICALCHANPROCESSINSPECTION_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverLogChanBase.h"

// Classes referenced
class DMemSpyDriverDevice;
class TMemSpyDriverProcessInspectionInfo;


NONSHARABLE_CLASS(DMemSpyDriverLogChanProcessInspection) : public DMemSpyDriverLogChanBase
	{
public:
	DMemSpyDriverLogChanProcessInspection( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanProcessInspection();

public: // From DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt ProcessInspectionOpen( TUint aPid );
    TInt ProcessInspectionClose( TUint aPid );
    TInt ProcessInspectionRequestChanges( TRequestStatus* aStatus, TMemSpyDriverProcessInspectionInfo* aInfo );
    TInt ProcessInspectionRequestChangesCancel( TUint aPid );
    TInt ProcessInspectionAutoStartItemsClear();
    TInt ProcessInspectionAutoStartItemsAdd( TUint aSID );

private: // Data members
	};


#endif
