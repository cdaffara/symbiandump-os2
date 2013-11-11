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

#ifndef MEMSPYDRIVERSTREAMWRITER_H
#define MEMSPYDRIVERSTREAMWRITER_H

// System includes
#include <e32cmn.h>
#include <kernel.h>

// Classes referenced
class DMemSpyDriverXferBuffer;


class RMemSpyMemStreamWriter
    {
public:
    RMemSpyMemStreamWriter();
    RMemSpyMemStreamWriter( DMemSpyDriverXferBuffer& aBuffer );

public: // API - closure
    void Close();
    TInt WriteAndClose( TDes8* aDestinationPointer );

public: // API - writing
    TInt32* WriteInt32( TInt32 aValue );
    TUint32* WriteUint32( TUint32 aValue );

public: // API - positioning
    TUint8* Current() const;
    void Seek( TInt aPosition );
    TUint32 Remaining() const;

public: // API - marking
    void MarkSet();
    void MarkResume();

public: // API - misc
    TBool IsFull() const;
    TBool IsOpen() const;

public: // API - operators
    RMemSpyMemStreamWriter& operator=( const RMemSpyMemStreamWriter& aCopy );

private: // Internal methods
    void IncrementPos( TInt aAmount );

private:
    DMemSpyDriverXferBuffer* iBuffer;
    TUint8* iCurrent;
    TUint8* iMax;
    TUint8* iMark;
    TInt iCommitted;
    };


#endif
