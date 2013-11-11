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
* Description:  Timer.
*
*/


#ifndef __TIMEOUTTIMER_H__
#define __TIMEOUTTIMER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MTimeOutNotifier;

// CLASS DECLARATION
class CTimeOutTimer : public CTimer
    {
    public: // Constructors and destructors

        static CTimeOutTimer* NewL( const TInt aPriority,
                                    MTimeOutNotifier& aObserver);
        virtual ~CTimeOutTimer();

    protected: // Functions from base classes

        void RunL();

    private: // Constructors and destructors

        CTimeOutTimer( const TInt aPriority,
                       MTimeOutNotifier& aTimeOutNotify );
        void ConstructL();

    private: // Data

        MTimeOutNotifier& iObserver;
    };

#endif // __TIMEOUTTIMER_H__

// End of File
