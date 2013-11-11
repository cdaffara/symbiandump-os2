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
* Description:  Implements receiver class for target
*
*/

/*
* %version: 13 %
*/

#include <nifmbuf.h>
#include "am_debug.h"
#include "receiver_hw.h"
#include "WlanProto.h"
#include "carddrv.h"

// ---------------------------------------------------------
// CReceiver::CReceiver()
// ---------------------------------------------------------
//
CReceiver::CReceiver( CPcCardPktDrv* aParent ) : 
    CActive(EPriorityStandard),
    iParent(aParent),
    iFrameToFree( NULL )
    {
    }

// ---------------------------------------------------------
// CReceiver::ConstructL()
// ---------------------------------------------------------
//
void CReceiver::ConstructL()
    {
    DEBUG("CReceiver::ConstructL()");
    }

// ---------------------------------------------------------
// CReceiver::NewL()
// ---------------------------------------------------------
//
CReceiver* CReceiver::NewL( CPcCardPktDrv* aParent )
    {
    DEBUG("CReceiver::NewL()");
    CReceiver* self = new (ELeave) CReceiver( aParent );
    CleanupStack::PushL( self );
    self->ConstructL();
    CActiveScheduler::Add( self );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CReceiver::~CReceiver()
// ---------------------------------------------------------
//
CReceiver::~CReceiver()
    {
    DEBUG("CReceiver::~CReceiver()");
    Cancel();
    iFrameToFree = NULL;
    }

// ---------------------------------------------------------
// CReceiver::DoCancel()
// ---------------------------------------------------------
//
void CReceiver::DoCancel()
    {
    DEBUG("CReceiver::DoCancel()");
    iParent->iCard.ReadCancel();
    }

// ---------------------------------------------------------
// CReceiver::QueueRead()
// ---------------------------------------------------------
//
void CReceiver::QueueRead()
    {
    iParent->iCard.RequestFrame( iStatus );
    SetActive();
    }


// ---------------------------------------------------------
// CReceiver::RunL()
// ---------------------------------------------------------
//
void CReceiver::RunL()
    {
    DEBUG("CReceiver::RunL()" );

    TUint rxCount( 0 );
    const TUint KMaxRxCount( 4 );
    
    while( ( rxCount < KMaxRxCount ) &&
           ( iDataBuffer = iParent->iCard.GetRxFrame( iFrameToFree ) ) != NULL )
        {
        TUint8* buf = iDataBuffer->GetBuffer();
        TUint32 len = iDataBuffer->GetLength();

        //Save the original buf value which points 
        //to the beginning of the Ethernet header
        TUint8* bufOrig = buf;   

		DEBUG1("CReceiver::RunL() - packet length: %u", len );
    
        if( buf && len > 0 && len <= KMaxEthernetFrameLength )
            {        
            //Now set buf point to the beginning of the Payload
            buf = buf + KEtherHeaderSize;
               
            len = len - KEtherHeaderSize;
         
            TPtrC8 pkt(buf, len);
            RMBufPacket pFrame; //Payload
            TRAPD( ret, pFrame.CreateL( pkt ) );
            if( ret == KErrNone )
                {
                pFrame.Pack(); 
            
                iParent->iParent->Process( pFrame, bufOrig, iDataBuffer->UserPriority() );
                }
            }
            
        iFrameToFree = iDataBuffer;
        ++rxCount;
		DEBUG1("CReceiver::RunL() - %u packet(s) processed", rxCount );
        }

    if ( rxCount == KMaxRxCount )
        {
		DEBUG("CReceiver::RunL() - yield");
		
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    else
        {
        iFrameToFree = NULL;
            
        if( iParent->CardOpen() )
            {
            QueueRead();
            }
        }
    }
