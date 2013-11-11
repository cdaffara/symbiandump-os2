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
* Description:  Start HTI framework and provide configuration parameters.
*
*/


#ifndef HTISERVER_H__
#define HTISERVER_H__

#include <e32base.h>
#include <e32svr.h>
#include <e32panic.h>

#include "HtiDispatcher.h"

class CHtiCfg;

class CHtiFramework : public CBase
    {
    public:
        static CHtiFramework* NewL();
        ~CHtiFramework();

        /**
        * Entry point, called after CHTIServer constructed
        */
        TInt StartL();

    protected:
        CHtiFramework();
        void ConstructL();
        void StartHtiWatchDogL();
        void WaitNormalState( TInt aMaxWaitTime, TInt aStartUpDelay );
        TBool IsHtiRunning();
        TBool IsStartAcceptedL( TBool aIsAutoStartEnabled );

    private:

    // HTI configuration
    CHtiCfg* iCfg;

    // Message dispatcher
    CHtiDispatcher* iDispatcher;

    // Pointer to original heap, used if bigger heap is created for HTI
    RHeap* iOriginalHeap;
    };

#endif
