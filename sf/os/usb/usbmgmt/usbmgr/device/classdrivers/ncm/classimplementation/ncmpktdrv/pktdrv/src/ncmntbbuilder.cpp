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
* implementation for NTB build  base class
*
*/


/**
@file
@internalComponent
*/



#include "ncmntbbuilder.h"
#include "ncmdatasender.h"
#include "ncmpktdrvcommon.h"
#include "ncmntbbuildpolicy.h"


// ======== MEMBER FUNCTIONS ========
//


CNcmNtbBuilder::CNcmNtbBuilder(MNcmNtbBuildObserver& aObserver)
    : iObserver(aObserver)
    {
    iSequence = 0xFFFF;
    }

CNcmNtbBuilder::~CNcmNtbBuilder()
    {
    }

void CNcmNtbBuilder::SendNtbPayload()
    {
    iObserver.SendNtbPayload(iBuffer);
    }

void CNcmNtbBuilder::StartNewNtb(const TNcmBuffer& aBuffer)
    {
    iSequence++;
    iBuffer = aBuffer;
    iNtbStarted = ETrue;
    }

void CNcmNtbBuilder::GetNtbParam(TNcmNtbInParam& aParam)
    {
    aParam.iNtbInMaxSize = iNtbInMaxSize;
    aParam.iNdpInDivisor = iNdpInDivisor;
    aParam.iNdpInPayloadRemainder = iNdpInPayloadRemainder;
    aParam.iNdpInAlignment = iNdpInAlignment;
    }

void CNcmNtbBuilder::Reset()
    {
    iNtbStarted = EFalse;   
    iSequence = 0xFFFF;
    }

void CNcmNtbBuilder::CompleteNtbBuild()
    {    
    iNtbStarted = EFalse;
    iBuildPolicy->CompleteNtbBuild();    
    }

