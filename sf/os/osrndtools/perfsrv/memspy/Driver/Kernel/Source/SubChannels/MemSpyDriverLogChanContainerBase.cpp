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

#include "MemSpyDriverLogChanContainerBase.h"

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include <memspy/driver/memspydriverobjectsshared.h>
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"


DMemSpyDriverLogChanContainerBase::DMemSpyDriverLogChanContainerBase( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanBase( aDevice, aThread )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::DMemSpyDriverLogChanContainerBase() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanContainerBase::~DMemSpyDriverLogChanContainerBase()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::~DMemSpyDriverLogChanContainerBase() - START - this: 0x%08x", this ));

    ResetTempHandles();

	TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::~DMemSpyDriverLogChanContainerBase() - END - this: 0x%08x", this ));
	}






DObject* DMemSpyDriverLogChanContainerBase::CheckedOpen(TMemSpyDriverContainerType aContainerType, DObject* aObject, TBool aQuick)
	{
	__ASSERT_CRITICAL;
	__ASSERT_DEBUG(aObject != NULL, MemSpyDriverUtils::Fault( __LINE__ ));
    const TObjectType expectedType = ObjectTypeFromMemSpyContainerType(aContainerType);

    // Quick mode means we just check container ids and we trust that the object will exist.
	// [TomS: not entirely convinced we can ever be certain of that]
    TInt err = KErrNotFound;
    if (aQuick)
        {
		LOG("quick CheckedOpen of %08x", aObject);
        const TObjectType objectType = OSAdaption().DThread().GetObjectType(*aObject);
        if (objectType == expectedType)
            {
            err = aObject->Open();
            }
        }
	else
		{
        DObjectCon* container = Kern::Containers()[expectedType];
        container->Wait();
        const TInt count = container->Count();
        for (TInt i = 0; i < count; i++)
            {
            DObject* object = (*container)[i];
            if (object == aObject)
                {
                err = aObject->Open();
				break;
				}
			}
		container->Signal();
		}

	LOG("CheckedOpen(%d, 0x%08x, quick=%d) returned error %d", aContainerType, aObject, aQuick, err);
	return (err == KErrNone) ? aObject : NULL;
	}



TObjectType DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType( TMemSpyDriverContainerType aType )
    {
    // Map type
    TObjectType type = EObjectTypeAny;
    switch( aType )
        {
    case EMemSpyDriverContainerTypeThread:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeThread" ));
        type = EThread;
        break;
    case EMemSpyDriverContainerTypeProcess:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeProcess" ));
        type = EProcess;
        break;
    case EMemSpyDriverContainerTypeChunk:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeChunk" ));
        type = EChunk;
        break;
    case EMemSpyDriverContainerTypeLibrary:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeLibrary" ));
        type = ELibrary;
        break;
    case EMemSpyDriverContainerTypeSemaphore:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeSemaphore" ));
        type = ESemaphore;
        break;
    case EMemSpyDriverContainerTypeMutex:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeMutex" ));
        type = EMutex;
        break;
    case EMemSpyDriverContainerTypeTimer:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeTimer" ));
        type = ETimer;
        break;
    case EMemSpyDriverContainerTypeServer:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeServer" ));
        type = EServer;
        break;
    case EMemSpyDriverContainerTypeSession:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeSession" ));
        type = ESession;
        break;
    case EMemSpyDriverContainerTypeLogicalDevice:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeLogicalDevice" ));
        type = ELogicalDevice;
        break;
    case EMemSpyDriverContainerTypePhysicalDevice:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypePhysicalDevice" ));
        type = EPhysicalDevice;
        break;
    case EMemSpyDriverContainerTypeLogicalChannel:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeLogicalChannel" ));
        type = ELogicalChannel;
        break;
    case EMemSpyDriverContainerTypeChangeNotifier:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeChangeNotifier" ));
        type = EChangeNotifier;
        break;
    case EMemSpyDriverContainerTypeUndertaker:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeUndertaker" ));
        type = EUndertaker;
        break;
    case EMemSpyDriverContainerTypeMsgQueue:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeMsgQueue" ));
        type = EMsgQueue;
        break;
    case EMemSpyDriverContainerTypePropertyRef:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypePropertyRef" ));
        type = EPropertyRef;
        break;
    case EMemSpyDriverContainerTypeCondVar:
 	    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType() - EMemSpyDriverContainerTypeCondVar" ));
        type = ECondVar;
        break;
    default:
        Kern::Printf("DMemSpyDriverLogChanContainerBase::ObjectTypeFromMemSpyContainerType - unsupported container type: %d", aType);
        break;
        }
    //
    return type;
    }




























    

void DMemSpyDriverLogChanContainerBase::ResetTempHandles()
    {
    iTempHandleCount = 0;
    }


void DMemSpyDriverLogChanContainerBase::AddTempHandle( TAny* aHandle )
    {
    __ASSERT_ALWAYS( iTempHandleCount >= 0, MemSpyDriverUtils::Fault( __LINE__) );
    if  ( iTempHandleCount < KMemSpyDriverMaxHandles )
        {
        iTempHandles[ iTempHandleCount++ ] = aHandle;
        }
    }


TAny* DMemSpyDriverLogChanContainerBase::TempHandleAt( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < KMemSpyDriverMaxHandles, MemSpyDriverUtils::Fault(__LINE__) );
    __ASSERT_ALWAYS( aIndex < iTempHandleCount, MemSpyDriverUtils::Fault(__LINE__) );
    return iTempHandles[ aIndex ];
    }


TInt DMemSpyDriverLogChanContainerBase::TempHandleCount() const
    {
    TRACE( Kern::Printf( "DMemSpyDriverLogChanContainerBase::TempHandleCount() - END - count is: %d", iTempHandleCount ));
    return iTempHandleCount;
    }


TInt DMemSpyDriverLogChanContainerBase::WriteToClient( TAny** aHandlePtr, TInt* aCountPtr, TInt aMaxCount )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::WriteToClient() - START - iTempHandleCount: %d", iTempHandleCount ));

    TInt r = KErrNone;

    // This variable holds the number of handles that we have already
	// written to the client-side.
	TInt currentWriteIndex = 0;

    // If the client passed a bad descriptor then we panic it. Otherwise, we always update the amount
    // of handles we have found (even if there was an error - i.e. we set the value to zero) or else
    // the client will attempt to index through its huge stack-based handle array.
	NKern::ThreadEnterCS();
    TInt tempHandleCount = TempHandleCount();
    TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::GetContainerHandles - trying to write %d handles to client...", tempHandleCount ));

    for( ; currentWriteIndex<tempHandleCount && r == KErrNone && currentWriteIndex < aMaxCount; )
        {
        TAny* handle = TempHandleAt( currentWriteIndex );
        r = Kern::ThreadRawWrite( &ClientThread(), aHandlePtr + currentWriteIndex, &handle, sizeof(TAny*) );
        if  (r == KErrNone)
            {
            ++currentWriteIndex;
            }
        }

	if  ( r == KErrBadDescriptor )
        {
        MemSpyDriverUtils::PanicThread( ClientThread(), EPanicBadDescriptor );
        }
    else
        {
        const TInt finalWrite = Kern::ThreadRawWrite( &ClientThread(), aCountPtr, &currentWriteIndex, sizeof(TInt) );
        if  ( r == KErrNone )
            {
            r = finalWrite;
            }
        }
	NKern::ThreadLeaveCS();

	TRACE( Kern::Printf("DMemSpyDriverLogChanContainerBase::GetContainerHandles() - END - r: %d", r));
	return r;
    }
