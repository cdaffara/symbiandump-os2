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
* Description:  Implements sender class for target
*
*/

/*
* %version: 15 %
*/

#include "am_debug.h"
#include "sender_hw.h"
#include "WlanProto.h"
#include "carddrv.h"

// Protocol blocks sending if it receives a return <= 0
// This value should be propogated up through the stack
const TInt KStopSending		= 0;
const TInt KContinueSending	= 1;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CSender::Send()
// ---------------------------------------------------------
//
TInt CSender::Send( RMBufChain& aPacket )
    {

	TInt ret( KContinueSending );
	
    TDataBuffer* txDataBuffer = iParent->iCard.AllocTxBuffer( 
        aPacket.Length()
        // size of the packet's UP value
        - 1  );
    
    if ( txDataBuffer )
        {
        //First buffer contains UP value
        RMBuf* rmbuf = aPacket.First();
        TInt num = aPacket.NumBufs();
    
        TUint8 UPValue = rmbuf->Get();
        
        txDataBuffer->SetUserPriority( UPValue );
    
        //Frame starts from second buffer
        rmbuf = rmbuf->Next();
        num--;
    
        for( TInt i(0); i < num; i++ )
            {
            txDataBuffer->AppendBuffer( rmbuf->Ptr(), rmbuf->Length() );
            
            rmbuf = rmbuf->Next();
            }
        
        if ( !iParent->iCard.AddTxFrame( txDataBuffer ) )
            {
            DEBUG("CSender::Send: AddTxPacket returned false => stop flow");
            ret = KStopSending;
            iStopSending = ETrue;
            RequestResumeTx();
            }
        }
    else
        {
        DEBUG("CSender::Send: Tx buf could't be allocated => stop flow");
        
        // in this case the Tx packet could possibly be kept - if seen
        // feasible - until ResumeTx completes and then re-submitted 
        
        ret = KStopSending;
        iStopSending = ETrue;
        RequestResumeTx();
        }
	
    aPacket.Free();
	
	return ret;
    }

// ---------------------------------------------------------
// CSender::RequestResumeTx()
// ---------------------------------------------------------
//
void CSender::RequestResumeTx()
    {
    if ( !iReqPending )
        {
        DEBUG("CSender::RequestResumeTx()");
    
        iReqPending = ETrue;
    	SetActive();	
        iParent->iCard.ResumeTx( iStatus );
        }
    }

// ---------------------------------------------------------
// CSender::RunL()
// ---------------------------------------------------------
//
void CSender::RunL()
    {
    // Write completion from the LDD
	DEBUG("CSender::RunL()");
	
	iReqPending = EFalse;

	if (iStopSending)
	    {
		DEBUG("CSender::RunL(): resume flow");
		iStopSending = EFalse;
		iParent->iParent->ResumeSending();
		}
	else
	    {
	    DEBUG("CSender::RunL(): flow hasn't been stopped");
	    }
    }
