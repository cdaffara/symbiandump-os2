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

#include "MemSpyDriverUtils.h"

// System includes
#include <kern_priv.h>
#include <nk_plat.h>
#ifdef __MARM__
#include <arm.h>
#endif
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include "MemSpyDriverOSAdaption.h"

// Constants
static const char KFault[] = "MemSpy-Bug";



void MemSpyDriverUtils::DataDump( const char* aFmt, const TUint8* aAddress, TInt aLength, TInt aMaxLength)
    {
    const TInt maxLen = aMaxLength;
    TInt len = aLength;
    const TUint8* pDataAddr = aAddress;

	TBuf8<81> out;
	TBuf8<20> ascii;
	TInt offset = 0;
	const TUint8* a = pDataAddr;
    //
	while(len>0)
		{
		out.Zero();
		ascii.Zero();
		out.AppendNumFixedWidth((TUint) a, EHex, 8);
		out.Append(_L(": "));

        TUint b;
		for (b=0; b<16; b++)
			{
            TUint8 c = ' ';
            if	((pDataAddr + offset + b) < pDataAddr + maxLen)
	            {
	            c = *(pDataAddr + offset + b);
				out.AppendNumFixedWidth(c, EHex, 2);
	            }
            else
	            {
				out.Append(_L("  "));
	            }
			out.Append(' ');
			if (c<=0x20 || c == 0x27 || c>=0x7f || c=='%')
				c=0x2e;
			ascii.Append(TChar(c));
			}
		out.Append(ascii);
        out.Append(TChar(0));

        Kern::Printf(aFmt, &out);

        a += 16;
		offset += 16;
		len -= 16;
        }
    }


void MemSpyDriverUtils::PanicThread( DThread& aThread, TMemSpyDriverPanic aPanicType )
    {
    Kern::ThreadKill( &aThread, EExitPanic, aPanicType, KMemSpyClientPanic );
    }


void MemSpyDriverUtils::Fault( TInt aReason )
    {
    Kern::Fault( KFault, aReason );
    }


void MemSpyDriverUtils::GetThreadRegisters( NThread* aThread, TMemSpyDriverRegSet& aInfo )
	{
    DThread* dThread = Kern::NThreadToDThread( aThread );
    TRACE( Kern::Printf( "MemSpyDriverUtils::GetThreadRegisters() - START - aThread: %O", dThread ));
    memclr( &aInfo, sizeof( TMemSpyDriverRegSet ) );
	TRACE( Kern::Printf( "MemSpyDriverUtils::GetThreadRegisters() - cleared" ));

#ifdef __MARM__
    if  ( aThread != NULL )
        {
	    TArmRegSet regSet;
        memclr( &regSet, sizeof(TArmRegSet) );
        //
	    TUint32 unused;
	    TRACE( Kern::Printf( "MemSpyDriverUtils::GetThreadRegisters() - getting user context..." ));
	    NKern::ThreadGetUserContext( aThread, &regSet, unused);
	    TRACE( Kern::Printf( "MemSpyDriverUtils::GetThreadRegisters() - got user context" ));
        //
	    aInfo.iRn[ 0] = regSet.iR0;
	    aInfo.iRn[ 1] = regSet.iR1;
	    aInfo.iRn[ 2] = regSet.iR2;
	    aInfo.iRn[ 3] = regSet.iR3;
	    aInfo.iRn[ 4] = regSet.iR4;
	    aInfo.iRn[ 5] = regSet.iR5;
	    aInfo.iRn[ 6] = regSet.iR6;
	    aInfo.iRn[ 7] = regSet.iR7;
	    aInfo.iRn[ 8] = regSet.iR8;
	    aInfo.iRn[ 9] = regSet.iR9;
	    aInfo.iRn[10] = regSet.iR10;
	    aInfo.iRn[11] = regSet.iR11;
	    aInfo.iRn[12] = regSet.iR12;
	    aInfo.iRn[13] = regSet.iR13;
	    aInfo.iRn[14] = regSet.iR14;
	    aInfo.iRn[15] = regSet.iR15;
        //
	    aInfo.iCpsr = regSet.iFlags;
        }
#else
    (void) aThread;
#endif

    TRACE( Kern::Printf( "MemSpyDriverUtils::GetThreadRegisters() - END" ));
	}


TThreadPriority MemSpyDriverUtils::MapToUserThreadPriority( TInt aPriority )
    {
    TThreadPriority tp = EPriorityNormal;
    //
	switch(aPriority)
		{
	case EThrdPriorityMuchLess:
        tp=EPriorityMuchLess;
        break;
	case EThrdPriorityLess:
        tp=EPriorityLess;
        break;
	default:
	case EThrdPriorityNormal:
        tp=EPriorityNormal;
        break;
	case EThrdPriorityMore:
        tp=EPriorityMore;
        break;
	case EThrdPriorityMuchMore:
        tp=EPriorityMuchMore;
        break;
	case EThrdPriorityRealTime:
        tp=EPriorityRealTime;
        break;
	case EThrdPriorityAbsoluteVeryLow:
        tp=EPriorityAbsoluteVeryLow;
        break;
	case EThrdPriorityAbsoluteLow:
        tp=EPriorityAbsoluteLow;
        break;
	case EThrdPriorityAbsoluteBackground:
        tp=EPriorityAbsoluteBackground;
        break;
	case EThrdPriorityAbsoluteForeground:
        tp=EPriorityAbsoluteForeground;
        break;
	case EThrdPriorityAbsoluteHigh:
        tp=EPriorityAbsoluteHigh;
        break;
    	}
    //
    return tp;
    }


TInt MemSpyDriverUtils::MapToAbsoluteThreadPriority( TThreadPriority aPriority )
    {
	TInt tp = KErrNotSupported;
    switch( aPriority )
        {
	case EPriorityAbsoluteVeryLow:
        tp=EThrdPriorityAbsoluteVeryLow; break;
	case EPriorityAbsoluteLowNormal:
        tp=EThrdPriorityAbsoluteLowNormal; break;
	case EPriorityAbsoluteLow:
        tp=EThrdPriorityAbsoluteLow; break;
	case EPriorityAbsoluteBackgroundNormal:
        tp=EThrdPriorityAbsoluteBackgroundNormal; break;
	case EPriorityAbsoluteBackground:
        tp=EThrdPriorityAbsoluteBackground; break;
	case EPriorityAbsoluteForegroundNormal:
        tp=EThrdPriorityAbsoluteForegroundNormal; break;
	case EPriorityAbsoluteForeground:
        tp=EThrdPriorityAbsoluteForeground; break;
	case EPriorityAbsoluteHighNormal:
        tp=EThrdPriorityAbsoluteHighNormal; break;
	case EPriorityAbsoluteHigh:
        tp=EThrdPriorityAbsoluteHigh; break;
	case EPriorityAbsoluteRealTime1:
        tp=EThrdPriorityAbsoluteRealTime1; break;
	case EPriorityAbsoluteRealTime2:
        tp=EThrdPriorityAbsoluteRealTime2; break;
	case EPriorityAbsoluteRealTime3:
        tp=EThrdPriorityAbsoluteRealTime3; break;
	case EPriorityAbsoluteRealTime4:
        tp=EThrdPriorityAbsoluteRealTime4; break;
	case EPriorityAbsoluteRealTime5:
        tp=EThrdPriorityAbsoluteRealTime5; break;
	case EPriorityAbsoluteRealTime6:
        tp=EThrdPriorityAbsoluteRealTime6; break;
	case EPriorityAbsoluteRealTime7:
        tp=EThrdPriorityAbsoluteRealTime7; break;
	case EPriorityAbsoluteRealTime8:
        tp=EThrdPriorityAbsoluteRealTime8; break;
	default:
		break;
        }

    return tp;
    }


void MemSpyDriverUtils::PrintChunkInfo( DChunk& aChunk, DMemSpyDriverOSAdaption& aOSAdaption )
    {
    DMemSpyDriverOSAdaptionDChunk& chunkAdaption = aOSAdaption.DChunk();
    //
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - aChunk->Name:        %O", &aChunk ) );
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - sizeof(DChunk):      %d", sizeof(DChunk) ) );
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - aChunk*:             0x%08x", &aChunk ) );

    DObject* owner = chunkAdaption.GetOwner( aChunk );
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - aChunk->Owner():     0x%08x", owner ) );

    if  ( owner )
        {
        const TObjectType objectType = chunkAdaption.GetObjectType( *owner );
        if ( objectType == EProcess )
            {
            DProcess* ownerProc = (DProcess*) owner;
            const TUint pid = aOSAdaption.DProcess().GetId( *ownerProc );
            TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - aChunk->Owner()->Id: %d (%O)", pid, ownerProc ) );
            }
        }

    DProcess* owningProcess = chunkAdaption.GetOwningProcess( aChunk );
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - iOwningProcess:      0x%08x (%O)", owningProcess, owningProcess ) );

    const TUint controllingOwner = chunkAdaption.GetControllingOwnerId( aChunk );
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - iControllingOwner:   %d", controllingOwner ) );

    const TInt attribs = chunkAdaption.GetAttributes( aChunk );
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - iAttributes:         0x%08x", attribs ) );

    const TChunkType type = chunkAdaption.GetType( aChunk );
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - iChunkType:          %d", type ) );

    const TUint8* base = chunkAdaption.GetBase( aChunk );
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - aChunk->Base:        0x%08x", base ) );

    const TInt size = chunkAdaption.GetSize( aChunk );
    TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - aChunk->Size:        0x%08x", size ) );
    //
    const TInt mapType = attribs & 0x00c00000; // DMemModelChunk::EMapTypeMask
    switch( mapType )
        {
    case 0x00000000: // EMapTypeLocal
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - map type:            DMemModelChunk::EMapTypeLocal" ) );
        break;
    case 0x00400000: // EMapTypeGlobal
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - map type:            DMemModelChunk::EMapTypeGlobal" ) );
        break;
    case 0x00800000: // EMapTypeShared
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - map type:            DMemModelChunk::EMapTypeShared" ) );
        break;
    default:
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - map type:            UNKNOWN!" ) );
        break;
        }
    //
	const TInt addressRange = ( attribs & 0x0f000000 ); // EAddressRangeMask
	switch (addressRange)
		{
    case 0x00000000: //EAddressLocal
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - address range:       DMemModelChunk::EAddressLocal" ) );
        break;
	case 0x01000000: //EAddressShared
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - address range:       DMemModelChunk::EAddressShared" ) );
        break;
	case 0x02000000: //EAddressUserGlobal
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - address range:       DMemModelChunk::EAddressUserGlobal" ) );
        break;
	case 0x03000000: //EAddressKernel
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - address range:       DMemModelChunk::EAddressKernel" ) );
        break;
	case 0x04000000: //EAddressFixed
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - address range:       DMemModelChunk::EAddressFixed" ) );
        break;
    default:
        TRACE( Kern::Printf( "MemSpyDriverUtils::PrintChunkInfo() - address range:       UNKNOWN!" ) );
        break;
		}
    }

