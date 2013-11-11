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

#include <memspy/api/memspyapikernelobjectitem.h>
#include <memspy/driver/memspydriverobjectsshared.h>

EXPORT_C CMemSpyApiKernelObjectItem::~CMemSpyApiKernelObjectItem()
	{
	delete iItem;
	}


CMemSpyApiKernelObjectItem::CMemSpyApiKernelObjectItem() : iItem(0)
	{
	}

CMemSpyApiKernelObjectItem* CMemSpyApiKernelObjectItem::NewL(const TMemSpyDriverHandleInfoGeneric& aData)
	{
	CMemSpyApiKernelObjectItem* self = CMemSpyApiKernelObjectItem::NewLC(aData);
	CleanupStack::Pop(self);
	return (self);
	}

CMemSpyApiKernelObjectItem* CMemSpyApiKernelObjectItem::NewLC(const TMemSpyDriverHandleInfoGeneric& aData)
	{
	CMemSpyApiKernelObjectItem* self = new (ELeave) CMemSpyApiKernelObjectItem;
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return (self);
	}

void CMemSpyApiKernelObjectItem::ConstructL(const TMemSpyDriverHandleInfoGeneric& aData)
	{
	iItem = new (ELeave) TMemSpyDriverHandleInfoGeneric(aData);
	}

EXPORT_C const TDesC8& CMemSpyApiKernelObjectItem::Name() const
	{
	/*
	TBuf<KMaxName> ret;	
	ret.Copy( iItem->iName );
	return ret;
	*/
	return iItem->iName;
	}

EXPORT_C TAny* CMemSpyApiKernelObjectItem::Handle() const
	{	
	return iItem->iHandle;
	}

EXPORT_C TMemSpyDriverContainerType CMemSpyApiKernelObjectItem::Type() const
	{
	return iItem->iType;
	}

// For Process, thread, chunk (owning process id), server (owning thread id)
EXPORT_C TInt CMemSpyApiKernelObjectItem::Id() const
	{
	return iItem->iId;
	}

// For Chunk, Library
EXPORT_C TUint32 CMemSpyApiKernelObjectItem::Size() const
	{
	return iItem->iSize;
	}

// For Semaphore, Mutex, Server
EXPORT_C TInt CMemSpyApiKernelObjectItem::Count() const
	{
	return iItem->iCount;
	}

// For Mutex
EXPORT_C TInt CMemSpyApiKernelObjectItem::WaitCount() const
	{
	return iItem->iWaitCount;
	}

// For Server, Session
EXPORT_C TIpcSessionType CMemSpyApiKernelObjectItem::SessionType() const
	{
	return iItem->iSessionType;
	}

// For Timer
EXPORT_C TMemSpyDriverTimerType CMemSpyApiKernelObjectItem::TimerType() const
	{
	return iItem->iTimerType;
	}

EXPORT_C TMemSpyDriverTimerState CMemSpyApiKernelObjectItem::TimerState() const
	{
	return iItem->iTimerState;
	}

// For Logical channel
EXPORT_C TInt CMemSpyApiKernelObjectItem::OpenChannels()
	{
	return iItem->iOpenChannels;
	}

// For most of the object types
EXPORT_C const TDesC8& CMemSpyApiKernelObjectItem::NameDetail() const
	{
	/*
	TBuf<KMaxName> ret;
	ret.Copy( iItem->iNameDetail );
	return ret;
	*/
	return iItem->iNameDetail;
	}

EXPORT_C TInt CMemSpyApiKernelObjectItem::AccessCount() const
	{
	return iItem->iAccessCount;
	}

EXPORT_C TInt CMemSpyApiKernelObjectItem::UniqueID() const
	{
	return iItem->iUniqueID;
	}

EXPORT_C TUint CMemSpyApiKernelObjectItem::Protection() const
	{
	return iItem->iProtection;
	}
		
EXPORT_C TUint8* CMemSpyApiKernelObjectItem::AddressOfKernelOwner()
	{
	return iItem->iAddressOfKernelOwner;
	}

EXPORT_C TInt CMemSpyApiKernelObjectItem::Priority() const
	{
	return iItem->iPriority;
	}

EXPORT_C TUint8* CMemSpyApiKernelObjectItem::AddressOfOwningProcess()
	{
	return iItem->iAddressOfOwningProcess;
	}

EXPORT_C TUint CMemSpyApiKernelObjectItem::CreatorId() const
	{
	return iItem->iCreatorId;
	}

EXPORT_C TUint CMemSpyApiKernelObjectItem::SecurityZone() const
	{
	return iItem->iSecurityZone;
	}

EXPORT_C TInt CMemSpyApiKernelObjectItem::Attributes() const
	{
	return iItem->iAttributes;
	}

EXPORT_C TUint8* CMemSpyApiKernelObjectItem::AddressOfDataBssStackChunk()
	{
	return iItem->iAddressOfDataBssStackChunk;
	}

// For Server, Session
EXPORT_C TUint8* CMemSpyApiKernelObjectItem::AddressOfOwningThread()
	{
	return iItem->iAddressOfOwningThread;
	}

EXPORT_C TUint8* CMemSpyApiKernelObjectItem::AddressOfServer()
	{
	return iItem->iAddressOfServer;
	}

EXPORT_C TUint16 CMemSpyApiKernelObjectItem::TotalAccessCount() const
	{
	return iItem->iTotalAccessCount;
	}

EXPORT_C TUint8 CMemSpyApiKernelObjectItem::SvrSessionType() const
	{
	return iItem->iSvrSessionType;
	}

EXPORT_C TInt CMemSpyApiKernelObjectItem::MsgCount() const
	{
	return iItem->iMsgCount;
	}

EXPORT_C TInt CMemSpyApiKernelObjectItem::MsgLimit() const
	{
	return iItem->iMsgLimit;
	}

// For chunk
EXPORT_C TInt CMemSpyApiKernelObjectItem::MaxSize() const
	{
	return iItem->iMaxSize;
	}

EXPORT_C TInt CMemSpyApiKernelObjectItem::Bottom() const
	{
	return iItem->iBottom;
	}

EXPORT_C TInt CMemSpyApiKernelObjectItem::Top() const
	{
	return iItem->iTop;
	}

EXPORT_C TInt CMemSpyApiKernelObjectItem::StartPos() const
	{
	return iItem->iStartPos;
	}

EXPORT_C TUint CMemSpyApiKernelObjectItem::ControllingOwner() const
	{
	return iItem->iControllingOwner;
	}

EXPORT_C TUint CMemSpyApiKernelObjectItem::Restrictions() const
	{
	return iItem->iRestrictions;
	}

EXPORT_C TUint CMemSpyApiKernelObjectItem::MapAttr() const
	{
	return iItem->iMapAttr;
	}

EXPORT_C TUint CMemSpyApiKernelObjectItem::ChunkType() const
	{
	return iItem->iChunkType;
	}

EXPORT_C const TDesC8& CMemSpyApiKernelObjectItem::NameOfOwner() const
	{
	/*
	TBuf<KMaxName> ret;
	ret.Copy( iItem->iNameOfOwner );
	return ret;
	*/
	return iItem->iNameOfOwner;
	}

// For library
EXPORT_C TInt CMemSpyApiKernelObjectItem::MapCount() const
	{
	return iItem->iMapCount;
	}

EXPORT_C TUint8 CMemSpyApiKernelObjectItem::State() const
	{
	return iItem->iState;
	}

EXPORT_C TUint8* CMemSpyApiKernelObjectItem::AddressOfCodeSeg()
	{
	return iItem->iAddressOfCodeSeg;
	}

// Semaphore, mutex, condvar
EXPORT_C TUint8 CMemSpyApiKernelObjectItem::Resetting() const
	{
	return iItem->iResetting;
	}

EXPORT_C TUint8 CMemSpyApiKernelObjectItem::Order() const
	{
	return iItem->iOrder;
	}

// For Logical/Physical device
EXPORT_C TVersion CMemSpyApiKernelObjectItem::Version() const
	{
	return iItem->iVersion;
	}

EXPORT_C TUint CMemSpyApiKernelObjectItem::ParseMask() const
	{
	return iItem->iParseMask;	
	}

EXPORT_C TUint CMemSpyApiKernelObjectItem::UnitsMask() const
	{
	return iItem->iUnitsMask;
	}

// For change notifier
EXPORT_C TUint CMemSpyApiKernelObjectItem::Changes() const
	{
	return iItem->iChanges;
	}

