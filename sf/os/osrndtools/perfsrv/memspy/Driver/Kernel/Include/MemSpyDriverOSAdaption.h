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

#ifndef MEMSPYDRIVEROSADAPTION_H
#define MEMSPYDRIVEROSADAPTION_H

// System includes
#include <kernel.h>
#include <e32const.h>

// User includes
#include "MemSpyDriverObjectIx.h"

// Classes referenced
class DMemSpyDriverDevice;
class DMemSpyPropertyRef;
class DMemSpyDriverOSAdaption;



class DMemSpyDriverOSAdaptionDObject : public DBase
    {
protected:
    DMemSpyDriverOSAdaptionDObject( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TUint8 GetContainerID( DObject& aObject ) const;
    TObjectType GetObjectType( DObject& aObject ) const;
    DObject* GetOwner( DObject& aObject ) const;
    DObject* GetOwner( DObject& aObject, TUint8 aExpectedContainerId ) const;
    TInt GetAccessCount( DObject& aObject ) const;
    TInt GetUniqueID( DObject& aObject ) const;
    TUint GetProtection( DObject& aObject ) const;
    TUint8* GetAddressOfKernelOwner( DObject& aObject ) const;

protected:
    DMemSpyDriverOSAdaption& OSAdaption() const { return iOSAdaption; }

private: // Data members
    DMemSpyDriverOSAdaption& iOSAdaption;
    };





class DMemSpyDriverOSAdaptionDThread : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDThread( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    NThread* GetNThread( DThread& aObject ) const;
    TExitType GetExitType( DThread& aObject ) const;
    TUint32 GetSupervisorStackBase( DThread& aObject ) const;
    TInt GetSupervisorStackSize( DThread& aObject ) const;
    RAllocator* GetAllocator( DThread& aObject ) const;
    CActiveScheduler* GetActiveScheduler( DThread& aObject ) const;
    TUint32 GetUserStackBase( DThread& aObject ) const;
    TInt GetUserStackSize( DThread& aObject ) const;
    DProcess* GetOwningProcess( DThread& aObject ) const;
    TUint GetId( DThread& aObject ) const;
    MemSpyObjectIx* GetHandles( DThread& aObject ) const;
    TUint GetOwningProcessId( DThread& aObject ) const;
    TInt GetPriority( DThread& aObject ) const;    
    TUint8* GetAddressOfOwningProcess( DThread& aObject ) const;    
    void GetNameOfOwningProcess( DThread& aObject, TDes& aName ) const;
    TBool IsHandleIndexValid( DThread& aObject ) const;

private: // Internal methods
    RAllocator* GetAllocatorAndStackAddress( DThread& aObject, TUint32& aStackAddress ) const;

private: // Data members
    TUint32 iOffset_NThread;
    TUint32 iOffset_ExitType;
    TUint32 iOffset_SupervisorStackBase;
    TUint32 iOffset_SupervisorStackSize;
    };







class DMemSpyDriverOSAdaptionDProcess : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDProcess( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TUint GetId( DProcess& aObject ) const;
    MemSpyObjectIx* GetHandles( DProcess& aObject ) const;
    TExitType GetExitType( DProcess& aObject ) const;
    DThread* OpenFirstThread( DProcess& aObject ) const;
    TUint32 GetSID( DProcess& aObject ) const;
    TUint GetSecurityZone( DProcess& aObject ) const;
    SSecurityInfo& GetSecurityInfo( DProcess& aObject ) const;
    TInt GetFlags( DProcess& aObject ) const;
    TInt GetGeneration( DProcess& aObject ) const;
    SDblQue& GetThreadQueue( DProcess& aObject ) const;
    DThread* GetThread( SDblQueLink* aLink ) const;
    void SetSID( DProcess& aObject, TUint32 aSID ) const;
    void SetSecurityZone( DProcess& aObject, TUint aSecurityZone ) const;
    TUint GetCreatorId( DProcess& aObject ) const;
    TInt GetAttributes( DProcess& aObject ) const;
    TInt GetPriority( DProcess& aObject ) const;
    TUint8* GetAddressOfOwningProcess( DProcess& aObject ) const;
    TUint8* GetAddressOfDataBssStackChunk( DProcess& aObject ) const;
    TBool IsHandleIndexValid( DProcess& aObject ) const;
	TBool IsKernProcess(DProcess& aProcess) const;

private: // Data members
    };















class DMemSpyDriverOSAdaptionDChunk : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDChunk( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TInt GetSize( DChunk& aObject ) const;
    TInt GetMaxSize( DChunk& aObject ) const;
    TUint8* GetBase( DChunk& aObject ) const;
    DProcess* GetOwningProcess( DChunk& aObject ) const;
    TUint GetOwningProcessId( DChunk& aObject ) const;
    TUint GetControllingOwnerId( DChunk& aObject ) const;
    TChunkType GetType( DChunk& aObject ) const;
    TInt GetAttributes( DChunk& aObject ) const;
    TUint8* GetAddressOfOwningProcess( DChunk& aObject ) const;
    TInt GetBottom( DChunk& aObject ) const;
    TInt GetTop( DChunk& aObject ) const;
    TInt GetStartPos( DChunk& aObject ) const;
    TUint GetRestrictions( DChunk& aObject ) const;
    TUint GetMapAttr( DChunk& aObject ) const;
    void GetNameOfOwningProcess( DChunk& aObject, TDes& aName ) const;

private: // Data members
    };








class DMemSpyDriverOSAdaptionDServer : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDServer( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    SDblQue& GetSessionQueue( DServer& aObject ) const;
    DSession* GetSession( SDblQueLink* aLink ) const;
    TInt GetSessionCount( DServer& aObject ) const;
    DThread* GetOwningThread( DServer& aObject ) const;
    TUint GetOwningThreadId( DServer& aObject ) const;
    TIpcSessionType GetSessionType( DServer& aObject ) const;
    TUint8* GetAddressOfOwningThread( DServer& aObject ) const;
    void GetNameOfOwningThread( DServer& aObject, TDes& aName ) const;

private: // Data members
    };







class DMemSpyDriverOSAdaptionDSession : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDSession( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TIpcSessionType GetSessionType( DSession& aObject ) const;
    DServer* GetServer( DSession& aObject ) const;
    TUint8* GetAddressOfServer( DSession& aObject ) const;
    TUint16 GetTotalAccessCount( DSession& aObject ) const;
    TUint8 GetSrvSessionType( DSession& aObject ) const;
    TInt GetMsgCount( DSession& aObject ) const;
    TInt GetMsgLimit( DSession& aObject ) const;

private: // Data members
    };









class DMemSpyDriverOSAdaptionDCodeSeg : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDCodeSeg( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    DCodeSeg* GetCodeSeg( SDblQueLink* aLink ) const;
    DCodeSeg* GetCodeSeg( DLibrary& aLibrary ) const;
    DCodeSeg* GetCodeSegFromHandle( TAny* aHandle ) const;
    TBool GetIsXIP( DCodeSeg& aCodeSeg ) const;
    TInt GetCodeSegQueue( DProcess& aObject, SDblQue& aQueue ) const;
    void EmptyCodeSegQueue( SDblQue& aQueue ) const;
    TUint32 GetSize( DCodeSeg& aCodeSeg ) const;
    void GetCreateInfo( DCodeSeg& aCodeSeg, TCodeSegCreateInfo& aInfo ) const;
    TUint8 GetState( DLibrary& aLibrary ) const;
    TInt GetMapCount( DLibrary& aLibrary ) const;

private: // Data members
    };





class DMemSpyDriverOSAdaptionDSemaphore : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDSemaphore( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TInt GetCount( DSemaphore& aObject ) const;
    TUint8 GetResetting( DSemaphore& aObject ) const;

private: // Data members
    };









class DMemSpyDriverOSAdaptionDMutex : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDMutex( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TInt GetHoldCount( DMutex& aObject ) const;
    TInt GetWaitCount( DMutex& aObject ) const;
    TUint8 GetResetting( DMutex& aObject ) const;
    TUint8 GetOrder( DMutex& aObject ) const;

private: // Data members
    };








class DMemSpyDriverOSAdaptionDLogicalDevice : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDLogicalDevice( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TInt GetOpenChannels( DLogicalDevice& aObject ) const;
    TVersion GetVersion( DLogicalDevice& aObject ) const;
    TUint GetParseMask( DLogicalDevice& aObject ) const;
    TUint GetUnitsMask( DLogicalDevice& aObject ) const;

private: // Data members
    };









class DMemSpyDriverOSAdaptionDPhysicalDevice : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDPhysicalDevice( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TVersion GetVersion( DPhysicalDevice& aObject ) const;
    TUint GetUnitsMask( DPhysicalDevice& aObject ) const;
    TUint8* GetAddressOfCodeSeg( DPhysicalDevice& aObject ) const;

private: // Data members
    };











class DMemSpyDriverOSAdaptionDChangeNotifier : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDChangeNotifier( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TUint GetChanges( DChangeNotifier& aObject ) const;
    TUint8* GetAddressOfOwningThread( DChangeNotifier& aObject ) const;
    void GetNameOfOwningThread( DChangeNotifier& aObject, TDes& aName ) const;

private: // Data members
    };











class DMemSpyDriverOSAdaptionDUndertaker : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDUndertaker( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TUint8* GetAddressOfOwningThread( DUndertaker& aObject ) const;
    void GetNameOfOwningThread( DUndertaker& aObject, TDes& aName ) const;

private: // Data members
    };












class DMemSpyDriverOSAdaptionDCondVar : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDCondVar( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TUint8 GetResetting( DCondVar& aObject ) const;
    TUint8* GetAddressOfMutex( DCondVar& aObject ) const;
    void GetNameOfMutex( DCondVar& aObject, TDes& aName ) const;
    TInt GetWaitCount( DCondVar& aObject ) const;
    SDblQue& GetSuspendedQ( DCondVar& aObject ) const;
    DThread* GetThread( SDblQueLink* aLink ) const;

private: // Data members
    };















class DMemSpyDriverOSAdaptionDTimer : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDTimer( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TTimer::TTimerType GetType( DTimer& aObject ) const;
    TTimer::TTimerState GetState( DTimer& aObject ) const;

private: // Data members
    };





class DMemSpyDriverOSAdaptionDPropertyRef : public DMemSpyDriverOSAdaptionDObject
    {
public:
    DMemSpyDriverOSAdaptionDPropertyRef( DMemSpyDriverOSAdaption& aOSAdaption );

public:
    TBool GetIsReady( DObject& aObject ) const;
    RProperty::TType GetType( DObject& aObject ) const;
    TUint GetCategory( DObject& aObject ) const;
    TUint GetKey( DObject& aObject ) const;
    TInt GetRefCount( DObject& aObject ) const;
    TUint GetThreadId( DObject& aObject ) const;
    TUint32 GetCreatorSID( DObject& aObject ) const;

private: // Internal methods
    DMemSpyPropertyRef* GetPropertyRef( DObject& aObject ) const;

private: // Data members
    };






NONSHARABLE_CLASS(DMemSpyDriverOSAdaption) : public DBase
	{
public:
    DMemSpyDriverOSAdaption( DMemSpyDriverDevice& aDevice );
    ~DMemSpyDriverOSAdaption();
    TInt Construct();

public: // API
    inline DMemSpyDriverDevice& Device() { return iDevice; }
    inline DMemSpyDriverOSAdaptionDProcess& DProcess() { return *iDProcess; }
    inline DMemSpyDriverOSAdaptionDThread& DThread() { return *iDThread; }
    inline DMemSpyDriverOSAdaptionDChunk& DChunk() { return *iDChunk; }
    inline DMemSpyDriverOSAdaptionDServer& DServer() { return *iDServer; }
    inline DMemSpyDriverOSAdaptionDSession& DSession() { return *iDSession; }
    inline DMemSpyDriverOSAdaptionDCodeSeg& DCodeSeg() { return *iDCodeSeg; }
    inline DMemSpyDriverOSAdaptionDSemaphore& DSemaphore() { return *iDSemaphore; }
    inline DMemSpyDriverOSAdaptionDMutex& DMutex() { return *iDMutex; }
    inline DMemSpyDriverOSAdaptionDLogicalDevice& DLogicalDevice() { return *iDLogicalDevice; }
    inline DMemSpyDriverOSAdaptionDPhysicalDevice& DPhysicalDevice() { return *iDPhysicalDevice; }
    inline DMemSpyDriverOSAdaptionDChangeNotifier& DChangeNotifier() { return *iDChangeNotifier; }
    inline DMemSpyDriverOSAdaptionDUndertaker& DUndertaker() { return *iDUndertaker; }
    inline DMemSpyDriverOSAdaptionDCondVar& DCondVar() { return *iDCondVar; }
    inline DMemSpyDriverOSAdaptionDTimer& DTimer() { return *iDTimer; }
    inline DMemSpyDriverOSAdaptionDPropertyRef& DPropertyRef() { return *iDPropertyRef; }

    inline const DMemSpyDriverDevice& Device() const { return iDevice; }
    inline const DMemSpyDriverOSAdaptionDProcess& DProcess() const { return *iDProcess; }
    inline const DMemSpyDriverOSAdaptionDThread& DThread() const { return *iDThread; }
    inline const DMemSpyDriverOSAdaptionDChunk& DChunk() const { return *iDChunk; }
    inline const DMemSpyDriverOSAdaptionDServer& DServer() const { return *iDServer; }
    inline const DMemSpyDriverOSAdaptionDSession& DSession() const { return *iDSession; }
    inline const DMemSpyDriverOSAdaptionDCodeSeg& DCodeSeg() const { return *iDCodeSeg; }
    inline const DMemSpyDriverOSAdaptionDSemaphore& DSemaphore() const { return *iDSemaphore; }
    inline const DMemSpyDriverOSAdaptionDMutex& DMutex() const { return *iDMutex; }
    inline const DMemSpyDriverOSAdaptionDLogicalDevice& DLogicalDevice() const { return *iDLogicalDevice; }
    inline const DMemSpyDriverOSAdaptionDPhysicalDevice& DPhysicalDevice() const { return *iDPhysicalDevice; }
    inline const DMemSpyDriverOSAdaptionDChangeNotifier& DChangeNotifier() const { return *iDChangeNotifier; }
    inline const DMemSpyDriverOSAdaptionDUndertaker& DUndertaker() const { return *iDUndertaker; }
    inline const DMemSpyDriverOSAdaptionDCondVar& DCondVar() const { return *iDCondVar; }
    inline const DMemSpyDriverOSAdaptionDTimer& DTimer() const { return *iDTimer; }
    inline const DMemSpyDriverOSAdaptionDPropertyRef& DPropertyRef() const { return *iDPropertyRef; }    
    
private: // Data members
    DMemSpyDriverDevice& iDevice;
    DMemSpyDriverOSAdaptionDThread* iDThread;
    DMemSpyDriverOSAdaptionDProcess* iDProcess;
    DMemSpyDriverOSAdaptionDChunk* iDChunk;
    DMemSpyDriverOSAdaptionDServer* iDServer;
    DMemSpyDriverOSAdaptionDSession* iDSession;
    DMemSpyDriverOSAdaptionDCodeSeg* iDCodeSeg;
    DMemSpyDriverOSAdaptionDSemaphore* iDSemaphore;
    DMemSpyDriverOSAdaptionDMutex* iDMutex;
    DMemSpyDriverOSAdaptionDLogicalDevice* iDLogicalDevice;
    DMemSpyDriverOSAdaptionDPhysicalDevice* iDPhysicalDevice;
    DMemSpyDriverOSAdaptionDChangeNotifier* iDChangeNotifier;
    DMemSpyDriverOSAdaptionDUndertaker* iDUndertaker;
    DMemSpyDriverOSAdaptionDCondVar* iDCondVar;
    DMemSpyDriverOSAdaptionDTimer* iDTimer;
    DMemSpyDriverOSAdaptionDPropertyRef* iDPropertyRef;
    };


// KNOWN OMISSIONS:
//
// + kernel container listing & query interface
// + event handler (events: process, threads, code segs, chunks, ... )
// + kernel heap information
// + end thread / process


#endif
