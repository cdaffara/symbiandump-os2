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

#ifndef CONSOLEDWOPERATION_H
#define CONSOLEDWOPERATION_H

// Engine includes
#include <memspy/engine/memspydevicewideoperations.h>

// Classes referenced
class CMemSpyEngine;


class CMemSpyDeviceWideOperationWaiter : public CBase, public MMemSpyDeviceWideOperationsObserver
    {
public:
    static void ExecuteLD( CMemSpyEngine& aEngine, CMemSpyDeviceWideOperations::TOperation aOperation );
    ~CMemSpyDeviceWideOperationWaiter();

private:
    CMemSpyDeviceWideOperationWaiter( CMemSpyEngine& aEngine );
    void ExecuteL( CMemSpyDeviceWideOperations::TOperation aOperation );

private: // From MMemSpyDeviceWideOperationsObserver
    void HandleDeviceWideOperationEvent( TEvent aEvent, TInt aParam1, const TDesC& aParam2 );

private: // Internal 
    void StopWait();

private: // Member data
    CMemSpyEngine& iEngine;
    CActiveSchedulerWait* iWaiter;
    CMemSpyDeviceWideOperations* iOperation;
    };


#endif
