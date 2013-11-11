/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef FEATMGRTIMER_H
#define FEATMGRTIMER_H

//  INCLUDES
#include <e32base.h>

/**
*  An interface for handling timer expiration events.
*/
NONSHARABLE_CLASS(MFeatMgrTimerCallback)
    {
    public:
        /** 
        * Called when the timer fires
        */
        virtual void TimerFired() = 0;
    };

/**
*  Timer class for supervising Genericting time.
*/
NONSHARABLE_CLASS(CFeatMgrTimer) : public CTimer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aCallback Callback object for informing timer firing
        */
        static CFeatMgrTimer* NewL( MFeatMgrTimerCallback& aCallback );
                
        /**
        * Destructor.
        */
        ~CFeatMgrTimer();
        
        
    public:  // From base classes

        // From CTimer
        void RunL();
        
    protected:
    
        /**
        * Constructor
        *
        * @param aCallback Callback object for informing timer firing
        */
        CFeatMgrTimer( MFeatMgrTimerCallback& aCallback );
        
  private:
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
                     
        /**
        * Set the timer.
        *
        * @param aInterval timer will expire after this duration.
        */
        void Set( const TTimeIntervalMicroSeconds32& aInterval );
        
    private: // Data
        MFeatMgrTimerCallback&        iCallback;    // Callback for timer fired messages. Not owned.
    };

#endif // FEATMGRTIMER_H   
            
// End of File
