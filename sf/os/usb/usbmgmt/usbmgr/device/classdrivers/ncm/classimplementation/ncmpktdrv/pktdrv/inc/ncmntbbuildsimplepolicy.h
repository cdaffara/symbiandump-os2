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
* Header file for simple NTB build policy
*
*/


/**
@file
@internalComponent
*/


#ifndef NCMNTBBUILDSIMPLEPOLICY_H
#define NCMNTBBUILDSIMPLEPOLICY_H


#include <e32base.h>
#include "ncmntbbuildpolicy.h"


/**
* a simple build policy, fix packets count in a NTB and a timer to restrict maxiam time delay
*/

NONSHARABLE_CLASS(CNcmNtbBuildSimplePolicy) : public CNcmNtbBuildPolicy
    {
public:
    static CNcmNtbBuildPolicy* NewL(CNcmNtbBuilder& aPolicy);
    ~CNcmNtbBuildSimplePolicy();

public:
    /**
    derived from CNcmNtbBuildPolicy
    */
    virtual void StartNewNtb();
    virtual void UpdateNtb(TInt aSize);
    virtual void CompleteNtbBuild();
	
	virtual void UpdateBufferSize(TInt aSize);
    virtual void UpdateFreeBufferCount(TInt aCount);
	virtual void UpdateTotalBufferCount(TInt aCount);

private:    
    void RunL();
    void DoCancel();
    void ConstructL();
    CNcmNtbBuildSimplePolicy(CNcmNtbBuilder& aPolicy);
    /**
    * timer for maxiam packet time delay
    */        
    RTimer iTimer;
    /**
    * packets number in current NTB
    */    
    TInt iPacketsCount;
	TInt iTotalBufferCount;
	TInt iFreeBufferCount;
	TInt iBufferSize;
    
};


#endif //NCMNTBBUILDSIMPLEPOLICY_H

