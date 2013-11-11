/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __PLUGINTIMER_H_
#define __PLUGINTIMER_H_

#include <featmgr/featureinfoplugin.h>

/**
*  An interface for handling timer expiration events.
*/
class MPluginTimerCallback
    {
    public:
        /** 
        * Called when the timer fires
        */
        virtual void GenericTimerFiredL() = 0;
    };


class CPluginTimer : public CTimer
    {
    public:
        CPluginTimer( TInt aPriority, 
                      MPluginTimerCallback* aCallback );
        
        ~CPluginTimer();        

        static CPluginTimer* NewL( const TTimeIntervalMicroSeconds32& anInterval, 
                                   MPluginTimerCallback* aCallback );
                         
        void ConstructL();
        
        void RunL();
    
    private:
    
       MPluginTimerCallback* iCallback;  // Not owned    
    };


#endif // __PLUGINTIMER_H_
