/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements PcCardPktDrv class
*
*/

/*
* %version: 14 %
*/

#include <nifmbuf.h>
#include <es_mbuf.h>
#include "carddrv.h"
#include "EtherCardApi.h"
#include "am_debug.h" 
#include "WlanProto.h"
#include "sender_hw.h"
#include "receiver_hw.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPcCardPktDrvFactory::NewDriverL()
// -----------------------------------------------------------------------------
//
CPcCardPktDrv* CPcCardPktDrv::NewL( CLANLinkCommon* aParent )
    {
    // Create the packet driver object
    CPcCardPktDrv *drv = new(ELeave) CPcCardPktDrv();

    CleanupStack::PushL(drv);
    drv->ConstructL(aParent);
    CleanupStack::Pop(drv);

    return drv;
    }


// -----------------------------------------------------------------------------
// CPcCardPktDrv::CPcCardPktDrv()
// -----------------------------------------------------------------------------
//
// Packet Driver object
CPcCardPktDrv::CPcCardPktDrv()
    :iCardOpen( EFalse )
    {   
    }

// -----------------------------------------------------------------------------
// CPcCardPktDrvFactory::~CPcCardPktDrv()
// -----------------------------------------------------------------------------
//
CPcCardPktDrv::~CPcCardPktDrv()
    {
    delete iReceiver; 
    delete iSender;  
    }

// -----------------------------------------------------------------------------
// CPcCardPktDrvFactory::ConstructL()
// -----------------------------------------------------------------------------
//
void CPcCardPktDrv::ConstructL( CLANLinkCommon* aParent )
    {
    DEBUG("CPcCardPktDrv::ConstructL()");

    iParent = aParent;

    iSender = CSender::NewL( this );
    iReceiver = CReceiver::NewL( this );

    User::LeaveIfError( iCard.Open() );
    
    CleanupClosePushL( iCard );

    User::LeaveIfError( iCard.InitialiseBuffers() );
    
    CleanupStack::Pop( &iCard );
  }


// -----------------------------------------------------------------------------
// CPcCardPktDrvFactory::StartInterface()
// -----------------------------------------------------------------------------
//
TInt CPcCardPktDrv::StartInterface()
    {
    DEBUG("CPcCardPktDrv::StartInterface()");
    // Opens the card and queues a read
    // validates
    TRAPD( err, StartL() );
    return err;
    }
 
// -----------------------------------------------------------------------------
// CPcCardPktDrvFactory::StopInterface()
// -----------------------------------------------------------------------------
//
TInt CPcCardPktDrv::StopInterface()
    {
    DEBUG("CPcCardPktDrv::StopInterface()");
    // Closes the card
    // validates
    Stop();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPcCardPktDrvFactory::ResetInterface()
// -----------------------------------------------------------------------------
//
TInt CPcCardPktDrv::ResetInterface()
    {
    DEBUG("CPcCardPktDrv::ResetInterface()");
    Stop();
    TRAPD( err, StartL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CPcCardPktDrvFactory::Send()
// -----------------------------------------------------------------------------
//
TInt CPcCardPktDrv::Send( RMBufChain &aPacket )
    {
    if( !CardOpen() )
        {
        aPacket.Free();
        return KErrNotReady;
        }

    return iSender->Send( aPacket );
    }

// -----------------------------------------------------------------------------
// CPcCardPktDrvFactory::GetInterfaceAddress()
// -----------------------------------------------------------------------------
//
TUint8* CPcCardPktDrv::GetInterfaceAddress()
    {
    DEBUG("CPcCardPktDrv::GetInterfaceAddress()");
    
    iConfig.SetMax();
    iCard.GetConfig( iConfig );

    // MAC address is located 3 bytes from the start of the buffer
    return( (TUint8*)iConfig.Ptr() )+3;
    }

// -----------------------------------------------------------------------------
// CPcCardPktDrv::StartL()
// -----------------------------------------------------------------------------
//
void CPcCardPktDrv::StartL()
    {
    // Open the Card LDD
    DEBUG("CPcCardPktDrv::StartL()");

    iCardOpen = ETrue;
    
    // We just assume that "link layer" is always immediatedly up
    iParent->LinkLayerUp();
    iReceiver->QueueRead();
    }

// -----------------------------------------------------------------------------
// CPcCardPktDrv::Stop()
// -----------------------------------------------------------------------------
//
void CPcCardPktDrv::Stop()
    {
    DEBUG("CPcCardPktDrv::Stop()");

    iCard.ResumeTxCancel();
    iSender->Cancel();

    iCard.ReadCancel();
    iReceiver->Cancel();
    
    iCard.ReleaseBuffers();

    iCard.Close();
    iCardOpen = EFalse; 
    } 


/**
* Implementation for methods declared in 802dot11.h
*/

// -----------------------------------------------------------------------------
// os_memcpy
// -----------------------------------------------------------------------------
//
TAny* os_memcpy( TAny* aDest, const TAny* aSrc, TUint32 aLengthinBytes )
    {
    Mem::Copy( aDest, aSrc, aLengthinBytes );
    return aDest;
    }

// -----------------------------------------------------------------------------
// os_memcmp
// -----------------------------------------------------------------------------
//
TInt os_memcmp( const TAny* aSrc1, const TAny* aSrc2, TUint32 aLengthinBytes )
    {
    return Mem::Compare( reinterpret_cast<const TUint8*>( aSrc1 ), aLengthinBytes,
        reinterpret_cast<const TUint8*>( aSrc2 ), aLengthinBytes );
    }


