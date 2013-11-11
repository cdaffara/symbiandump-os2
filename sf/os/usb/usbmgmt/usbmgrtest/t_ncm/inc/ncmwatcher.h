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

/** @file
 @internalComponent
 @test
 */


#ifndef NCMWATCHER_H
#define NCMWATCHER_H

#include <e32property.h>
#include <usb/usbncm.h>
#include <e32base.h> 

class CUsbNcmConsole;

NONSHARABLE_CLASS(CSharedStateWatcher) : public CActive
    {
public:
    static CSharedStateWatcher* NewL(CUsbNcmConsole& aConsole);
    ~CSharedStateWatcher();

private:
    CSharedStateWatcher(CUsbNcmConsole& aConsole);
    void ConstructL();
    void reportState();

    //From CActive
    void RunL();
    void DoCancel();

private:
    RProperty iProperty;
    TInt iIapId;
    TInt iErrCode;
    TPtr8 iNcmStateDes;
    UsbNcm::TNcmConnectionEvent iSharedState;
    CUsbNcmConsole& iConsole;
    };

#endif  // NCMWATCHER_H
