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
* Description:  Interface for timeout notification.
*
*/


#ifndef __TIMEOUTNOTIFIER_H__
#define __TIMEOUTNOTIFIER_H__

// CLASS DECLARATION

class MTimeOutNotifier
    {
    public: // New functions

        /**
        * The function to be called when a timeout occurs.
        */
        virtual void TimerExpired() = 0;
    };

#endif // __TIMEOUTNOTIFIER_H__

// End of File
