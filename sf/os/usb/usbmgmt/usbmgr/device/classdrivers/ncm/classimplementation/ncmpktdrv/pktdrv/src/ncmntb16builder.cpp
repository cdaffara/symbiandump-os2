/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* implementation for NTB16 build  class
*
*/



/**
@file
@internalComponent
*/



#include <es_sock.h>
#ifndef __OVER_DUMMYUSBSCLDD__
#include <nifmbuf.h>
#else
#include <usb/testncm/ethinter.h>
#endif
#include "ncmntb16builder.h"
#include "ncmntbbuildpolicy.h"
#include "ncmpktdrvcommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmntb16builderTraces.h"
#endif


const TInt KNtb16InMaxSize = 32768;


const TInt KNth16Signature = 0x484D434E;
const TInt KNth16Length = 0x0C;

const TInt KNdp16Signature = 0x304D434E;
const TInt KNdp16HeaderSize = 8;
const TInt KNdp16EntrySize = 4;
const TInt KNdp16InDivisor = 4;
const TInt KNdp16InRemainder = 0;
const TInt KNdp16InAlignment = 4;


// ======== MEMBER FUNCTIONS ========
//


CNcmNtbBuilder* CNcmNtb16Builder::NewL(MNcmNtbBuildObserver& aObserver)
    {
    OstTraceFunctionEntry1( CNCMNTB16BUILDER_NEWL_ENTRY, ( TUint )&( aObserver ) );
    CNcmNtb16Builder *self=new (ELeave) CNcmNtb16Builder(aObserver);
    self->ConstructL();
    OstTraceFunctionExit0( CNCMNTB16BUILDER_NEWL_EXIT );
    return self;    
    }

CNcmNtb16Builder::CNcmNtb16Builder(MNcmNtbBuildObserver& aObserver)
    : CNcmNtbBuilder(aObserver)
    {
    }

void CNcmNtb16Builder::ConstructL()
    {
    iNtbInMaxSize = KNtb16InMaxSize;
    iNdpInDivisor = KNdp16InDivisor;
    iNdpInPayloadRemainder = KNdp16InRemainder;
    iNdpInAlignment = KNdp16InAlignment;  
    }

CNcmNtb16Builder::~CNcmNtb16Builder()
    {
    iPacketsArray.Reset();
    }


TInt CNcmNtb16Builder::SetNtbInMaxSize(TInt aSize)
    {
    OstTraceFunctionEntry1( CNCMNTB16BUILDER_SETNTBINMAXSIZE_ENTRY, this );
    if (aSize > KNtb16InMaxSize || aSize < MinNtbInMaxSize())
        {
        OstTraceFunctionExit1( CNCMNTB16BUILDER_SETNTBINMAXSIZE_EXIT, this );
        return KErrArgument;
        }
    iNtbInMaxSize = aSize;
    OstTraceFunctionExit1( CNCMNTB_DUP116BUILDER_SETNTBINMAXSIZE_EXIT, this );
    return KErrNone;
    }

//
//construct NTB header information
//

void CNcmNtb16Builder::BuildNtbHeader()
    {
    OstTraceFunctionEntry1( CNCMNTB16BUILDER_BUILDNTBHEADER_ENTRY, this );
    TPtr8 buf(iBuffer.iPtr, KNth16Length, KNth16Length);
    TInt pos = 0;
    LittleEndian::Put32(&buf[pos], KNth16Signature);
    pos += 4;
    LittleEndian::Put16(&buf[pos], KNth16Length);
    pos += 2;
    LittleEndian::Put16(&buf[pos], iSequence);
    pos += 2;
    LittleEndian::Put32(&buf[pos], iNdpOffset+iNdpLength);
    pos += 2;
    LittleEndian::Put16(&buf[pos], iNdpOffset);
    pos += 2;

    OstTraceExt3( TRACE_NORMAL, CNCMNTB16BUILDER_BUILDNTBHEADER, "CNcmNtb16Builder::BuildNtbHeader %d %d %d", iSequence, iNdpOffset, iNdpOffset+iNdpLength );
    OstTraceFunctionExit1( CNCMNTB16BUILDER_BUILDNTBHEADER_EXIT, this );
    }


//
//construct NDP inforamtion
//

void CNcmNtb16Builder::BuildNdp()
    {
    OstTraceFunctionEntry1( CNCMNTB16BUILDER_BUILDNDP_ENTRY, this );
    TPtr8 buf(iBuffer.iPtr+iNdpOffset, iNdpLength, iNdpLength);
    TInt pos = 0;

    LittleEndian::Put32(&buf[pos], KNdp16Signature);
    pos += 4;
    LittleEndian::Put16(&buf[pos], iNdpLength);
    pos += 2;
    LittleEndian::Put16(&buf[pos], 0);
    pos += 2;
     
    TInt i;
    for (i=0 ; i<iPacketsArray.Count() ; i++)
        {
        LittleEndian::Put16(&buf[pos], iPacketsArray[i].iWDatagramIndex);
        pos += 2;
        LittleEndian::Put16(&buf[pos], iPacketsArray[i].iWDatagramLength);
        pos += 2;
        }
    LittleEndian::Put16(&buf[pos], 0);
    pos += 2;
    LittleEndian::Put16(&buf[pos], 0);
    pos += 2;
    OstTraceFunctionExit1( CNCMNTB16BUILDER_BUILDNDP_EXIT, this );
    }


//
//Append a datagram to NTB
//

TInt CNcmNtb16Builder::AppendPacket(const RMBufChain& aPacket)
    {
    OstTraceFunctionEntry1( CNCMNTB16BUILDER_APPENDPACKET_ENTRY, this );
// datagram must align with  iNdpInDivisor and remainder is iNdpInPayloadRemainder, refer to NCM spec 3.34 for detail   
    TInt len = aPacket.Length();
    TInt alignedDataOffset;
    int remainder = iDatagramOffset%iNdpInDivisor;
    if (iNdpInPayloadRemainder < remainder)
        {
        alignedDataOffset = iDatagramOffset + iNdpInDivisor - remainder + iNdpInPayloadRemainder;  
        }
    else
        {
        alignedDataOffset = iDatagramOffset + (iNdpInPayloadRemainder-remainder);
        }
    TInt newDatagramOffset = alignedDataOffset+len;
// NDP must align with  iNdpInAlignment, refer to NCM spec Table 6-3 for detail         
    TInt newNdpOffset = (newDatagramOffset+iNdpInAlignment-1)&~(iNdpInAlignment-1);

//    must have a zero entry in the end of NDP, so the NDP length is added with KNdp16EntrySize
    TInt newNdpLen = iNdpLength + KNdp16EntrySize;

//   iBuffer.iMaxLength and iNtbInMaxSize may not same.
    if (newNdpLen+newNdpOffset > iBuffer.iMaxLength || newNdpLen+newNdpOffset > iNtbInMaxSize)
        {
        CompleteNtbBuild();
        OstTraceFunctionExit1( CNCMNTB16BUILDER_APPENDPACKET_EXIT, this );
        return KErrBufferFull;
        }

    TPtr8 ptr((TUint8*)(alignedDataOffset+iBuffer.iPtr), len, len);
    aPacket.CopyOut(ptr);
    TDatagramEntry entry;
    entry.iWDatagramIndex = alignedDataOffset;
    entry.iWDatagramLength = len;
    iPacketsArray.Append(entry);
    iNdpOffset = newNdpOffset;
    iNdpLength = newNdpLen;     
    iDatagramOffset = newDatagramOffset;
    if (iBuildPolicy)
        {
        iBuildPolicy->UpdateNtb(iNdpOffset+iNdpLength);
        }
    OstTraceFunctionExit1( CNCMNTB_DUP116BUILDER_APPENDPACKET_EXIT, this );
    return KErrNone;
    }

//
//complete a NTB, is called by buildpolicy or current NTB is full.
//


void CNcmNtb16Builder::CompleteNtbBuild()
    {
    OstTraceFunctionEntry1( CNCMNTB16BUILDER_COMPLETENTBBUILD_ENTRY, this );
    
    BuildNtbHeader();
    BuildNdp();
    iBuffer.iLen = iNdpOffset+iNdpLength;
    SendNtbPayload();
    CNcmNtbBuilder::CompleteNtbBuild();    
    OstTraceFunctionExit1( CNCMNTB16BUILDER_COMPLETENTBBUILD_EXIT, this );
    }


//
//start a new ntb, is called by sender
//

void CNcmNtb16Builder::StartNewNtb(const TNcmBuffer& aBuffer)
    {
    OstTraceFunctionEntry1( CNCMNTB16BUILDER_STARTNEWNTB_ENTRY, this );
    
    iBuffer = aBuffer;
    iNdpOffset = 0;
    iNdpLength = 0;
    iPacketsArray.Reset();
    iDatagramOffset = KNth16Length;
// NDP must align with  iNdpInAlignment, refer to NCM spec Table 6-3 for detail     
    iNdpOffset = (iDatagramOffset+iNdpInAlignment-1)&~(iNdpInAlignment-1);
//    must have a zero entry in the end of NDP, so the NDP length is added with KNdp16EntrySize
    iNdpLength = KNdp16HeaderSize + KNdp16EntrySize;
    
    if (iBuildPolicy)
        {
        iBuildPolicy->StartNewNtb();
        }
    CNcmNtbBuilder::StartNewNtb(aBuffer);
    OstTraceFunctionExit1( CNCMNTB16BUILDER_STARTNEWNTB_EXIT, this );
    }


void CNcmNtb16Builder::Reset()
    {
    OstTraceFunctionEntry1( CNCMNTB16BUILDER_RESET_ENTRY, this );
    iNtbInMaxSize = KNtb16InMaxSize;
    iNdpOffset = 0;
    iNdpLength = 0;    
    iDatagramOffset = 0;    
    iPacketsArray.Reset();
    CNcmNtbBuilder::Reset();
    OstTraceFunctionExit1( CNCMNTB16BUILDER_RESET_EXIT, this );
    }
    


