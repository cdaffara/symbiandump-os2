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
* Description:  Implements sender class
*
*/

/*
* %version: 12 %
*/

#include "am_debug.h"
#include "carddrv.h"
#include "sender_hw.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSender::CSender()
// -----------------------------------------------------------------------------
//
CSender::CSender() :
    CActive( CActive::EPriorityHigh )
	{
	}

// -----------------------------------------------------------------------------
// CSender::~CSender()
// -----------------------------------------------------------------------------
//
CSender::~CSender()
	{
	DEBUG("CSender::~CSender()");
	Cancel();
	}

// -----------------------------------------------------------------------------
// CSender::NewL()
// -----------------------------------------------------------------------------
//
CSender* CSender::NewL( CPcCardPktDrv* aParent )
    {
    DEBUG("CSender::NewL()");

    CSender *sd = new(ELeave) CSender;
    CleanupStack::PushL( sd );
    sd->InitL(aParent);
    CActiveScheduler::Add( sd );
    CleanupStack::Pop( sd );
    return sd;
    }

// -----------------------------------------------------------------------------
// CSender::InitL()
// -----------------------------------------------------------------------------
//
void CSender::InitL( CPcCardPktDrv* aParent )
    {
    DEBUG("CSender::InitL()");

	iParent = aParent;
	iStopSending = EFalse;
	iReqPending = EFalse;

    }

// -----------------------------------------------------------------------------
// CSender::DoCancel()
// -----------------------------------------------------------------------------
//
void CSender::DoCancel()
	{
	DEBUG("CSender::DoCancel()");
	iParent->iCard.ResumeTxCancel();
	}
