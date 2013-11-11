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


#ifndef BASEPROFILETIMESTAMPING_H_
#define BASEPROFILETIMESTAMPING_H_

#include "baseprofilehandler.h"

NONSHARABLE_CLASS(CBaseProfileTimestampHandling) : public CBaseProfileHandler
    {
public:
    CBaseProfileTimestampHandling(ROmxScriptTest& aTestOwner, RASBreakEventHandler& aParentEventHandler);
    ~CBaseProfileTimestampHandling();
    
    void SetClockComponent(OMX_COMPONENTTYPE* aClockComponent);
    void QueueTimestampCheckL(TInt aPortIndex, TUint aTime, TUint aTolerance);
    void QueueCompareWithRefClockL(TInt aPortIndex, TUint aTolerance);
    
protected:

    struct TTimestampCheckingInfo
        {
        TInt iPortIndex;    //Index in the component
        TUint iExpectedTime;
        TUint iTolerance;
        TBool iCompareWithRefClock;
        };
    
    virtual void DoFillBufferDone(OMX_BUFFERHEADERTYPE* aFilledBuffer);
    virtual void DoEmptyBufferDone(OMX_BUFFERHEADERTYPE* aEmptiedBuffer); 

    void CheckTimestamp(OMX_BUFFERHEADERTYPE* aBuffer);
    

    
private:
    OMX_COMPONENTTYPE* iClockComponent;   //Not owned, if non null used as the reference clock for cmparison with the current set of timestamps
    RArray<TTimestampCheckingInfo> iPendingTimestampChecks;
    
    };

#endif /* BASEPROFILETIMESTAMPING_H_ */
