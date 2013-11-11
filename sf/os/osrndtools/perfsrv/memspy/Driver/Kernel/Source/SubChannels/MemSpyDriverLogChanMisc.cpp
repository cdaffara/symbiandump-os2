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

#include "MemSpyDriverLogChanMisc.h"

// System includes
#include <memspy/driver/memspydriverconstants.h>
#include <memspy/driver/memspydriverconstants.h>
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverHeap.h"
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"



DMemSpyDriverLogChanMisc::DMemSpyDriverLogChanMisc( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanBase( aDevice, aThread )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanMisc::DMemSpyDriverLogChanMisc() - START - this: 0x%08x", this ));

    DProcess* process = OSAdaption().DThread().GetOwningProcess( aThread );
    if  ( process )
        {
        DMemSpyDriverOSAdaptionDProcess& processAdaption = OSAdaption().DProcess();
        //
        iSID = processAdaption.GetSID( *process );
        iSecurityZone = processAdaption.GetSecurityZone( *process );
        //
        iClientIsMemSpy = ( iSID == KMemSpyUiSID || iSID == KMemSpyConsoleUiSID );
        }
    TRACE( Kern::Printf("DMemSpyDriverLogChanMisc::DMemSpyDriverLogChanMisc() - END - sid: 0x%08x, sz: 0x%08x, isMemSpy: %d", iSID, iSecurityZone, iClientIsMemSpy ));
    }


DMemSpyDriverLogChanMisc::~DMemSpyDriverLogChanMisc()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanMisc::~DMemSpyDriverLogChanMisc() - START - this: 0x%08x", this ));

	TRACE( Kern::Printf("DMemSpyDriverLogChanMisc::~DMemSpyDriverLogChanMisc() - END - this: 0x%08x", this ));
	}



TInt DMemSpyDriverLogChanMisc::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
        case EMemSpyDriverOpCodeMiscSetRHeapVTable:
            r = SetRHeapVTable( a1 );
            break;
        case EMemSpyDriverOpCodeMiscGetMemoryModelType:
            r = GetMemoryModelType();
            break;
        case EMemSpyDriverOpCodeMiscGetRoundToPageSize:
            r = GetRoundToPageSize( (TUint32*) a1 );
            break;
        case EMemSpyDriverOpCodeMiscImpersonate:
            r = Impersonate( (TUint32) a1 );
            break;

        default:
            r = KErrNotSupported;
		    break;
		    }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanMisc::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeMiscBase && aFunction < EMemSpyDriverOpCodeMiscEnd );
    }







TInt DMemSpyDriverLogChanMisc::SetRHeapVTable( TAny* aRHeapVTable )
    {
    const TUint32 vTable = (TUint32) aRHeapVTable;
    MemSpyDevice().SetRHeapVTable( vTable );
    return KErrNone;
    }





TInt DMemSpyDriverLogChanMisc::GetMemoryModelType()
    {
    TInt ret = EMemSpyMemoryModelTypeUnknown;
    //
  	const TUint32 memModelAttrib = (TUint32) Kern::HalFunction( EHalGroupKernel, EKernelHalMemModelInfo, NULL, NULL );	
    TRACE( Kern::Printf("DMemSpyDriverLogChanMisc::GetMemoryModelType() - memModelAttrib: 0x%08x", memModelAttrib));
    //
    if  ( (memModelAttrib & EMemModelTypeMask ) == EMemModelTypeMultiple )
        {
        ret = EMemSpyMemoryModelTypeMultiple;
        }
    else if ( (memModelAttrib & EMemModelTypeMask ) == EMemModelTypeMoving )
        {
        ret = EMemSpyMemoryModelTypeMoving;
        }
    else if ( (memModelAttrib & EMemModelTypeMask ) == EMemModelTypeEmul )
        {
        ret = EMemSpyMemoryModelTypeEmulator;
        }
    //
    return ret;
    }


TInt DMemSpyDriverLogChanMisc::GetRoundToPageSize( TUint32* aValue )
    {
    // We should probably do this user side... I think the HAL contains the page size.
    TUint32 value = 0;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aValue, &value, sizeof(TUint32) );
    //
    if  ( r == KErrNone )
        {
        value = Kern::RoundToPageSize( value );
        r = Kern::ThreadRawWrite( &ClientThread(), aValue, &value, sizeof(TUint32) );
        }
    //
    return r;
    }


TInt DMemSpyDriverLogChanMisc::Impersonate( TUint32 aValue )
    {
    DMemSpyDriverOSAdaptionDThread& threadAdaption = OSAdaption().DThread();
    DMemSpyDriverOSAdaptionDProcess& processAdaption = OSAdaption().DProcess();
    //
    TInt r = KErrNotFound;
    //
    TUint32 sid = iSID;
    TUint32 sz = iSecurityZone;
    //
    if  ( aValue != 0 )
        {
        // Must impersonate a SID:
	    NKern::ThreadEnterCS();

        DObjectCon* container = Kern::Containers()[ EProcess ];
        container->Wait();

        const TInt count = container->Count();
        for(TInt i=0; i<count; i++)
            {
            DObject* object = (*container)[ i ];
            if  ( object != NULL )
                {
                DProcess* proc = (DProcess*) object;

                const TUint32 processSID = processAdaption.GetSID( *proc );
                if  ( processSID == aValue )
                    {
                    sid = processSID;
                    sz = processAdaption.GetSecurityZone( *proc );
                    TRACE( Kern::Printf("DMemSpyDriverLogChanMisc::Impersonate() - MemSpy impersonates - pid: %d, sid: 0x%08x, sz: %d", proc->iId, sid, sz ));
                    break;
                    }

                }
            }

        container->Signal();
    	NKern::ThreadLeaveCS();
        }

    DThread& thread = ClientThread();
    DProcess* myOwningProcess = threadAdaption.GetOwningProcess( thread );
    //
    if  ( myOwningProcess )
        {
        if  ( iClientIsMemSpy )
            {
            processAdaption.SetSID( *myOwningProcess, sid );
            processAdaption.SetSecurityZone( *myOwningProcess, sz);
            //
            r = KErrNone;
            }
        else
            {
            r = KErrAccessDenied;
            }
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanMisc::Impersonate() - MemSpy is now: 0x%08x", sid ));
    return r;
    }




