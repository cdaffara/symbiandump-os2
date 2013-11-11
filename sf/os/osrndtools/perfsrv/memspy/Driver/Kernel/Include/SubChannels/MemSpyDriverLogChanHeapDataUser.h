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

#ifndef MEMSPYDRIVERLOGICALCHANHEAPDATAUSER_H
#define MEMSPYDRIVERLOGICALCHANHEAPDATAUSER_H

// User includes
#include "MemSpyDriverLogChanHeapDataBase.h"

NONSHARABLE_CLASS(DMemSpyDriverLogChanHeapDataUser) : public DMemSpyDriverLogChanHeapDataBase
	{
public:
	DMemSpyDriverLogChanHeapDataUser( DMemSpyDriverDevice& aDevice, DThread& aThread );

private: // from DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt GetInfoData( TMemSpyDriverInternalHeapRequestParameters* aParams );

private: // Internal methods
    TInt GetFullData( TMemSpyDriverInternalHeapDataParams& aParams );
	};


#endif
