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
* Header file NTB build policy base class
*
*/


/**
@file
@internalComponent
*/

#ifndef NCMNTBBUILDPOLICY_H
#define NCMNTBBUILDPOLICY_H


#include <e32base.h>
#include "ncmpktdrvcommon.h"

class CNcmNtbBuilder;

/**
base class for ntb build policy
*/

NONSHARABLE_CLASS(CNcmNtbBuildPolicy) : public CActive
    {
public:
    /**
    *    call by builder when begin a new NTB
    */
    virtual void StartNewNtb() = 0;
    /**
    *     call by builder when a ethernet frame is added to NTB
    */    
    virtual void UpdateNtb(TInt aSize) = 0;
    /**
    *   call by builder when complete a NTB    
    */
    virtual void CompleteNtbBuild() = 0;
	virtual void UpdateBufferSize(TInt aSize) = 0;
    virtual void UpdateFreeBufferCount(TInt aCount) = 0;
	virtual void UpdateTotalBufferCount(TInt aCount) = 0;
    ~CNcmNtbBuildPolicy();
    inline CNcmNtbBuilder& NtbBuilder();
    
protected:
    CNcmNtbBuildPolicy(CNcmNtbBuilder&);
    CNcmNtbBuilder&    iNtbBuilder;
};


inline CNcmNtbBuilder& CNcmNtbBuildPolicy::NtbBuilder()
    {
    return iNtbBuilder;
    }


#endif //NCMNTBBUILDPOLICY_H
