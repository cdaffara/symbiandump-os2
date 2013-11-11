/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* header file for NTB16 build  class
*
*/


/**
@file
@internalComponent
*/


#ifndef NCMNTB16BUILDER_H
#define NCMNTB16BUILDER_H

#include <e32base.h>
#include "ncmntbbuilder.h"
#include "ncmpktdrvcommon.h"


class MNcmNtbBuildObserver;


NONSHARABLE_CLASS(TDatagramEntry)
    {
public:
    TUint16 iWDatagramIndex;
    TUint16 iWDatagramLength;    
    };
    
/**
Responsible for build NTB16 payload
*/

NONSHARABLE_CLASS(CNcmNtb16Builder) : public CNcmNtbBuilder
    {
public:
    static CNcmNtbBuilder* NewL(MNcmNtbBuildObserver&);
    ~CNcmNtb16Builder();

public:
/**
*derived from CNcmNtbBuilder
*/
    void StartNewNtb(const TNcmBuffer& aBuffer);
    TInt AppendPacket(const RMBufChain& aPacket);
    TInt SetNtbInMaxSize(TInt aSize);
    void CompleteNtbBuild();
    void Reset();
    
private:
    CNcmNtb16Builder(MNcmNtbBuildObserver&);
    void ConstructL();
    void BuildNtbHeader();
    void BuildNdp();
        

private:
    /**
    * offset of NDP in buffer
    */
    TUint16    iNdpOffset;
    /**
    * length of NDP
    */
    TUint16    iNdpLength;
    /**
    * offset of next datagram can be in.
    */
    TUint16    iDatagramOffset;
    /**
    * array of datagrams index and length in NTB.
    */    
    RArray<TDatagramEntry>    iPacketsArray;
    };


#endif //NCMNTB16BUILDER_H

