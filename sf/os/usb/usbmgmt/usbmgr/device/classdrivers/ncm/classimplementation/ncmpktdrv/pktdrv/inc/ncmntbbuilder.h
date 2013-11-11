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
* header file for NTB build  base class
*
*/



/**
@file
@internalComponent
*/


#ifndef NCMNTBBUILDER_H
#define NCMNTBBUILDER_H

#include <e32base.h>

#include "ncmpktdrvcommon.h"


class MNcmNtbBuildObserver;
class RMBufChain;
class CNcmNtbBuildPolicy;
class TNcmNtbInParam;


const TInt KErrBufferFull = -6667;


/**
Base class for build NTB
*/

NONSHARABLE_CLASS(CNcmNtbBuilder) : public CBase
    {
public:
    /**
    * append a ethernet frame to current NTB
    *
    * @param aPacket ethernet packet buffer
    * @return KErrNone        if success
    *         KErrBufferFull  if current NTB is full and can't insert the new packet to NTB and send the current NTB immediately
    */
    virtual TInt AppendPacket(const RMBufChain& aPacket) = 0;  
    /**
    * complete the current NTB
    *
    */
    virtual void CompleteNtbBuild();
    /**
    * change the NtbInMaxSize
    *
    * @param aSize new size
    * @return  KErrNone        if success   
    *          KErrArgument    if the size is bigger than allowed size in NCM spec.
    */
    virtual TInt SetNtbInMaxSize(TInt aSize) = 0;
    /**
    * prepare build a new NTB
    *
    * @param aBuffer buffer to construct NTB
    */
    virtual void StartNewNtb(const TNcmBuffer& aBuffer);
    /**
    * reset builder to intial state.
    */
    virtual void Reset();

public:
    ~CNcmNtbBuilder();
    void GetNtbParam(TNcmNtbInParam& aParam);
    inline void SetBuildPolicy(CNcmNtbBuildPolicy& aPolicy);
    /**
    * @return  ETrue        if has called StartNewNtb for current NTB   
    *          EFalse       otherwise
    */
    inline TBool IsNtbStarted();
    inline TInt NtbInMaxSize();
    inline TInt MinNtbInMaxSize();	

protected:
    CNcmNtbBuilder(MNcmNtbBuildObserver&);
    void SendNtbPayload();

protected:
    /**
    * refer to NCM spec for following five variable meaning.
    */    
    TInt                     iNtbInMaxSize;
    TInt                     iNdpInDivisor;
    TInt                     iNdpInPayloadRemainder;
    TInt                     iNdpInAlignment;    
    TUint16                  iSequence;

    MNcmNtbBuildObserver&    iObserver;
    /**
    * buffer contains NTB
    */    
    TNcmBuffer               iBuffer;
    /**
    * point to builder policy, this is not ownership
    */     
    CNcmNtbBuildPolicy*      iBuildPolicy;
    /**
    * a NTB is started or not.
    */    
    TBool                    iNtbStarted;
    };


// Inline functions
#include "ncmntbbuilder.inl"


#endif /* NCMNTBBUILDER_H */
