/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of the DWlanPowerHandler class
*
*/

/*
* %version: 6 %
*/

#include "wllddpowerhandler.h"
#include "wllddpowerindicator.h"

_LIT(KWlanPowerHandlerName,"WLAN power handler");


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
DWlanPowerHandler::DWlanPowerHandler( 
    MWlanPowerIndicator& aWlanLdd ,
    TDynamicDfcQue& aDfcQ ):
    DPowerHandler( KWlanPowerHandlerName ),
    iWlanLdd( &aWlanLdd ),
    iPowerUpDfc( PowerUpDfcFn, this, NULL, 0 ),
    iPowerDownDfc( PowerDownDfcFn, this, NULL, 0 )
    {
    iPowerUpDfc.SetDfcQ( &aDfcQ );
    iPowerDownDfc.SetDfcQ( &aDfcQ );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
DWlanPowerHandler::~DWlanPowerHandler()
    {
    iWlanLdd = NULL;
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void DWlanPowerHandler::PowerDownDfcFn( TAny *aPtr )
    {
    DWlanPowerHandler *pwrHandler = static_cast<DWlanPowerHandler*>(aPtr);
    pwrHandler->PowerDownOperation();
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void DWlanPowerHandler::PowerUpDfcFn( TAny *aPtr )
    {
    DWlanPowerHandler *pwrHandler = static_cast<DWlanPowerHandler*>(aPtr);
    pwrHandler->PowerUpOperation();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanPowerHandler::PowerDown( TPowerState ) 
    {
    iPowerDownDfc.Enque();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanPowerHandler::PowerUp() 
    {
    iPowerUpDfc.Enque();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanPowerHandler::OnPowerDownDone() 
    {
    // Acknowledge back to power manager that power down has been done
    PowerDownDone();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanPowerHandler::OnPowerUpDone() 
    {
    // Acknowledge back to power manager that power up handling has been done
    PowerUpDone();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanPowerHandler::PowerDownOperation()
    {
    // Forward the request
    iWlanLdd->OnPowerDown();
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void DWlanPowerHandler::PowerUpOperation()
    {
    // Forward the indication
    iWlanLdd->OnPowerUp();
    }
