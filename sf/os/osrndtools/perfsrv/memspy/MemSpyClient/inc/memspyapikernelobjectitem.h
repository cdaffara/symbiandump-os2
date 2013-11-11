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
* Contains the declaration of  CMemSpyProcess class 
*/

#ifndef MEMSPYAPIKERNELOBJECTITEM_H
#define MEMSPYAPIKERNELOBJECTITEM_H

// System includes
#include <e32base.h>
#include <memspy/driver/memspydriverenumerationsshared.h>

class TMemSpyDriverHandleInfoGeneric;
//class TMemSpyDriverContainerType;	
//class TMemSpyDriverTimerType;
//class TMemSpyDriverTimerState;

NONSHARABLE_CLASS( CMemSpyApiKernelObjectItem ) 
	{
public:
	IMPORT_C ~CMemSpyApiKernelObjectItem();
	
	static CMemSpyApiKernelObjectItem* NewL(const TMemSpyDriverHandleInfoGeneric& aData);

	static CMemSpyApiKernelObjectItem* NewLC(const TMemSpyDriverHandleInfoGeneric& aData);
	
public:	
	
	IMPORT_C const TDesC8& Name() const;	
	
	IMPORT_C TAny* Handle() const;
	
	IMPORT_C TMemSpyDriverContainerType Type() const;
	
    // For Process, thread, chunk (owning process id), server (owning thread id)
    IMPORT_C TInt Id() const;

    // For Chunk, Library
    IMPORT_C TUint32 Size() const;
   
    // For Semaphore, Mutex, Server
    IMPORT_C TInt Count() const;

    // For Mutex
    IMPORT_C TInt WaitCount() const;

    // For Server, Session
    IMPORT_C TIpcSessionType SessionType() const;

    // For Timer
    IMPORT_C TMemSpyDriverTimerType TimerType() const;
    IMPORT_C TMemSpyDriverTimerState TimerState() const;

    // For Logical channel
    IMPORT_C TInt OpenChannels();
    
    // For most of the object types
    IMPORT_C const TDesC8& NameDetail() const; // Name
    IMPORT_C TInt AccessCount() const;
    IMPORT_C TInt UniqueID() const;
    IMPORT_C TUint Protection() const;
    IMPORT_C TUint8* AddressOfKernelOwner();
    IMPORT_C TInt Priority() const;
    IMPORT_C TUint8* AddressOfOwningProcess();
    IMPORT_C TUint CreatorId() const;
    IMPORT_C TUint SecurityZone() const;
    IMPORT_C TInt Attributes() const;
    IMPORT_C TUint8* AddressOfDataBssStackChunk();

    // For Server, Session
    IMPORT_C TUint8* AddressOfOwningThread();
    IMPORT_C TUint8* AddressOfServer();
    IMPORT_C TUint16 TotalAccessCount() const;
    IMPORT_C TUint8 SvrSessionType() const;
    IMPORT_C TInt MsgCount() const;
    IMPORT_C TInt MsgLimit() const;
    
    // For chunk
    IMPORT_C TInt MaxSize() const;
    IMPORT_C TInt Bottom() const;
    IMPORT_C TInt Top() const;
    IMPORT_C TInt StartPos() const;
    IMPORT_C TUint ControllingOwner() const;
    IMPORT_C TUint Restrictions() const;
    IMPORT_C TUint MapAttr() const;
    IMPORT_C TUint ChunkType() const;
    IMPORT_C const TDesC8& NameOfOwner() const; // chunk, server
    
    // For library
    IMPORT_C TInt MapCount() const;
    IMPORT_C TUint8 State() const;
    IMPORT_C TUint8* AddressOfCodeSeg();
    
    // Semaphore, mutex, condvar
    IMPORT_C TUint8 Resetting() const;
    IMPORT_C TUint8 Order() const;
    
    // For Logical/Physical device
    IMPORT_C TVersion Version() const;
    IMPORT_C TUint ParseMask() const;
    IMPORT_C TUint UnitsMask() const;
    
    // For change notifier
    IMPORT_C TUint Changes() const;
	
private:
	CMemSpyApiKernelObjectItem();
	
	void ConstructL(const TMemSpyDriverHandleInfoGeneric& aData);
		
private:
	TMemSpyDriverHandleInfoGeneric *iItem;
};

	
#endif // MEMSPYAPIKERNELOBJECTITEM_H
