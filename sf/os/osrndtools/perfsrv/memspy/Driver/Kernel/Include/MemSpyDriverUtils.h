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

#ifndef MEMSPYDRIVERUTILS_H
#define MEMSPYDRIVERUTILS_H

// System includes
#include <kernel.h>
#include <memspy/driver/memspydriverpanics.h>

// User includes
#include "MemSpyDriverLog.h"

// Classes referenced
class TMemSpyDriverRegSet;
class DMemSpyDriverOSAdaption;


class MemSpyDriverUtils
	{
public:
    static void DataDump( const char* aFmt, const TUint8* aAddress, TInt aLength, TInt aMaxLength);
    static void PanicThread( DThread& aThread, TMemSpyDriverPanic aPanicType );
    static void Fault( TInt aReason );
    static void GetThreadRegisters( NThread* aThread, TMemSpyDriverRegSet& aInfo );
    static TThreadPriority MapToUserThreadPriority( TInt aPriority );
    static TInt MapToAbsoluteThreadPriority( TThreadPriority aPriority );
    static void PrintChunkInfo( DChunk& aChunk, DMemSpyDriverOSAdaption& aOSAdaption );
    };



#endif
