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

#ifndef MEMSPYDRIVERXFERBUFFER_H
#define MEMSPYDRIVERXFERBUFFER_H

// System includes
#include <e32cmn.h>
#include <kernel.h>

// Classes referenced
class DMemSpyDriverDevice;


NONSHARABLE_CLASS(DMemSpyDriverXferBuffer) : public DBase
	{
    friend class RMemSpyMemStreamWriter;

public:
	DMemSpyDriverXferBuffer( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverXferBuffer();
    TInt Construct( TInt aSize );

public: // API
    TInt Size() const;
    TInt GrowBy( TInt aSize );
    TInt EnsureCapacity( TInt aSize );
    void Reset();

private: // Internal methods
    TInt WriteToClient( TDes8* aDestinationPointer, TInt aLength );
    DThread& ClientThread();
    TUint8* Ptr();

private: // Data members
    DMemSpyDriverDevice& iDevice;
	DThread& iClientThread;
    TInt iSize;
    TAny* iBuffer;
	};



#endif
