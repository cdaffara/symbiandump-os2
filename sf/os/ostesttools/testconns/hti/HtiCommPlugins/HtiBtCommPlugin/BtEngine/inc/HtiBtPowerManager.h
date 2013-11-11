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
* Description:  BT power manager definition based on btpm.h.
*                This is needed for accessing the blue tooth power management
*                DLL directly to turn on the blue tooth in NCP environment,
*                since the NCP releases do not contain the required headers.
*
*/


#ifndef __HTIBTPOWERMANAGER_H
#define __HTIBTPOWERMANAGER_H

//  INCLUDES
#include <e32std.h>
#include <es_sock.h>

// CLASS DECLARATION

/**
* The observer class that is interested in power mode change
*/
class MBTPowerManagerObserver
    {
    public:
        virtual void HandlePowerChange(TBool aNewState) = 0;
    };

// CLASS DECLARATION

/**
* This base class of BT power management
*/
class CBTPowerManagerBase : public CBase
    {
    public:

        /**
        * Get the current power state
        * @param aState Reference where the current power state to be saved to
        * @return KErrNone if successful otherwise one of the system wide error codes
        */
        virtual TInt GetPowerState(TBool& aState) = 0;

        /**
        * Set the current power state
        * @param aState the current power state
        * @return KErrNone if successful otherwise one of the system wide error codes
        */
        virtual TInt SetPowerState(TBool aState) = 0;

        /**
        * Notify observers that the power state has been changed
        */
        virtual void NotifyPowerChange() = 0;

        /**
        * Notify observers that the power change has been cancelled
        */
        virtual void NotifyPowerChangeCancel() = 0;

        /**
        * Set the device into test mode
        * @return KErrNone if successful otherwise one of the system wide error codes
        */
        virtual TInt SetDUTMode() = 0;
    };

typedef CBTPowerManagerBase* (*TNewBTPowerManagerL)(MBTPowerManagerObserver* aObserver);

#endif  // __HTIBTPOWERMANAGER_H

// End of File
