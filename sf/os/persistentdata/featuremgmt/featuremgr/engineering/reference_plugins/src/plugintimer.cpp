// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#include "plugintimer.h"
#include <featmgr/featureinfoplugin.h>

CPluginTimer* CPluginTimer::NewL( const TTimeIntervalMicroSeconds32& anInterval, 
                                  MPluginTimerCallback* aCallback )
    {
    CPluginTimer* self = new( ELeave ) CPluginTimer(0, aCallback);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    CActiveScheduler::Add(self);

    self->After(anInterval);

    return self;
    }

CPluginTimer::~CPluginTimer()    
    {
    iCallback = NULL; 
    }

CPluginTimer::CPluginTimer( TInt aPriority, MPluginTimerCallback* aCallback )
    : CTimer(aPriority),
    iCallback(aCallback)
    {
    }

void CPluginTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

    
void CPluginTimer::RunL(  )
    {
    iCallback->GenericTimerFiredL();
    }
