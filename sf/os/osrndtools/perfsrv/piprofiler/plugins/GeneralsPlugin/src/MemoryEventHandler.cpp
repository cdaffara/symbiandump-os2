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

#include <e32def.h>
#include <e32cmn.h>
#include <arm.h>
#include <kernel.h>
#include <kern_priv.h>
#include <nk_trace.h>

#include "MemoryEventHandler.h"


DMemoryEventHandler::DMemoryEventHandler(DProfilerSampleBuffer* aSampleBuffer)
    :   DKernelEventHandler(EventHandler, this), 
        iSampleBuffer(aSampleBuffer), 
        iSampleDescriptor(&(this->iSample[1]),0,256)
    {
//    Kern::Printf("DMemoryEventHandler::DMemoryEventHandler()");
    iCount = 0;
    iPreviousCount = 0;
    }


TInt DMemoryEventHandler::Create()
    {
//    Kern::Printf("DMemoryEventHandler::Create()");

    TInt err(Kern::MutexCreate(iLock, _L("MemoryEventHandlerLock"), KMutexOrdGeneral0));
    if (err != KErrNone)
        return err;
    
    return Add();
    }


DMemoryEventHandler::~DMemoryEventHandler()
    {
//    Kern::Printf("DMemoryEventHandler::~DMemoryEventHandler()");

    if (iLock)
        iLock->Close(NULL);
       
    }


TInt DMemoryEventHandler::Start()
    {
//    Kern::Printf("DMemoryEventHandler::Start()");

    iTracking = ETrue;
    return KErrNone;
    }


TInt DMemoryEventHandler::Stop()
    {
//    Kern::Printf("DMemoryEventHandler::Stop()");

    iTracking = EFalse;
    return KErrNone;
    }

TBool DMemoryEventHandler::SampleNeeded()
    {
    LOGTEXT("DMemoryEventHandler::SampleNeeded()");
    
    // increase the coutner by one on each round
    iCount++;
    
    // check if event handler was not running
//    if(!iTracking)
//        return false; // return false
    
    return true;
    }


TUint DMemoryEventHandler::EventHandler(TKernelEvent aType, TAny* a1, TAny* a2, TAny* aThis)
    {
    return ((DMemoryEventHandler*)aThis)->HandleEvent(aType, a1, a2);
    }



TUint DMemoryEventHandler::HandleEvent(TKernelEvent aType, TAny* a1, TAny* a2)
    {
    // debug
//    Kern::Printf("New kernel event received, %d", aType);
    
    if (iTracking/* && iCount != iPreviousCount*/)
        {
//        iPreviousCount = iCount;
        iCounters[aType]++;
        switch (aType)
            {
            // capture only chunk creation, updates and destroyal
            case EEventNewChunk:
                {
                DChunk* chunk = (DChunk*)a1;
                HandleAddChunk(chunk);
                break;
                }
            case EEventUpdateChunk:   
                HandleUpdateChunk((DChunk*)a1);
                break;
            case EEventDeleteChunk:      
                HandleDeleteChunk((DChunk*)a1);
                break;
//            case EEventAddProcess:
//                Kern::Printf("Process added: 0x%08x", (DProcess*)a1);
//                break;
//            case EEventUpdateProcess:
//                Kern::Printf("DProcess updated: 0x%08x", (DProcess*)a1);
//                break;
//            case EEventRemoveProcess:
//                Kern::Printf("DProcess removed: 0x%08x", (DProcess*)a1);
//                break;
//            case EEventAddCodeSeg:
//                Kern::Printf("DCodeSeg added: 0x%08x", (DCodeSeg*)a1);
//                break;
//            case EEventRemoveCodeSeg:
//                Kern::Printf("DCodeSeg deleted: 0x%08x", (DCodeSeg*)a1);
//                break;
            case EEventAddThread:
                HandleAddThread((DThread*)a1);
                break;
            case EEventUpdateThread:    // thread renaming
                HandleUpdateThread((DThread*)a1);
                break;
//            case EEventKillThread:
            case EEventRemoveThread:
                HandleDeleteThread((DThread*)a1);
                break;
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
            case EEventAddLibrary:
                HandleAddLibrary((DLibrary*)a1, (DThread*)a2);
                break;
            case EEventRemoveLibrary:
                HandleDeleteLibrary((DLibrary*)a1);
                break;
#endif
            
            // ignore exception events
            case EEventSwExc:
            case EEventHwExc:
         
            default:
                break;
            }
        }
//    else if(iTracking && iCount == iPreviousCount)
//        {
//        // if time stamp is not updated profiling has stopped
//        Stop();
//        }
    return DKernelEventHandler::ERunNext;
    }

TInt DMemoryEventHandler::EncodeNameCode()
    {
    iSample[0] = 1;
    iSample[1] = 0xaa;
    return 2;
    }

// encode mark for new chunk or thread
TInt DMemoryEventHandler::EncodeNewCode()
    {
    iSample[0] = 1;
    iSample[1] = 0xda;
    return 2;
    }

// encode mark for update of chunk or thread
TInt DMemoryEventHandler::EncodeUpdateCode()
    {
    iSample[0] = 1;
    iSample[1] = 0xdb;
    return 2;
    }

// encode mark for removal of chunk or thread
TInt DMemoryEventHandler::EncodeRemoveCode()
    {
    iSample[0] = 1;
    iSample[1] = 0xdc;
    return 2;
    }

// encode the memory sample header in all memory changes
TInt DMemoryEventHandler::AddHeader()
    {
    TInt err(KErrNone);
    
    TUint8 number(4);    // mem sampler id

    // check if iCount bigger than previous, i.e. at least 1 ms has passed from the previous sample
    if(iCount > iPreviousCount)
        {
        err = this->iSampleBuffer->AddSample(&number,1);
        err = this->iSampleBuffer->AddSample((TUint8*)&(iCount),4);
    
        // add data chunk header
        TInt length(EncodeUpdateCode());
        err = iSampleBuffer->AddSample(iSample, length);
        
        // add total memory sample in the beginning of each sample
        length = EncodeTotalMemory();
        err = iSampleBuffer->AddSample(iSample, length);
        AddFooter();    // end mark for total memory sample
        }
    iPreviousCount = iCount;
    
    // add actual sample
    err = this->iSampleBuffer->AddSample(&number,1);
    err = this->iSampleBuffer->AddSample((TUint8*)&(iCount),4);

    return err;
    }

// encode the memory sample header in all memory changes
TInt DMemoryEventHandler::AddFooter()
    {
    TInt err(KErrNone);
    
    TUint8 number(0);    // end mark
    err = this->iSampleBuffer->AddSample(&number,1);
    
    return err;
    }

TInt DMemoryEventHandler::EncodeTotalMemory()
    {   
    
    TUint8* size(&iSample[0]);
    *size = 0;

    NKern::LockSystem();
    TInt freeRam(Kern::FreeRamInBytes());
    TInt totalRam(Kern::SuperPage().iTotalRamSize);
    NKern::UnlockSystem();

    iSampleDescriptor.Zero();
    
    TUint32 id(0xbabbeaaa);
    TInt zero(0);
        
    iSampleDescriptor.Append((TUint8*)&(id),sizeof(TUint32));
    *size += sizeof(TUint);
    
    iSampleDescriptor.Append((TUint8*)&(totalRam),sizeof(TInt));
    *size += sizeof(TInt);
        
    // append the cell amount allocated
    iSampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
    *size += sizeof(TInt);
    
    // append the chunk size
    iSampleDescriptor.Append((TUint8*)&(freeRam),sizeof(TInt));
    *size += sizeof(TInt);
        
    // append the thread user stack size
    iSampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
    *size += sizeof(TInt);

    return ((TInt)(*size))+1;
    }

// handle chunk activity
TBool DMemoryEventHandler::HandleAddChunk(DChunk* aChunk)
    {    
//    Kern::Printf("New DChunk created: 0x%08x, time: %d", aChunk, iCount);
    
    NKern::ThreadEnterCS();
    Kern::MutexWait(*iLock);
    // add header first
    TInt err(AddHeader());
    
    if(err != KErrNone)
        {
        return EFalse;
        }
    
    // new chunk, add name of it
    TInt length(EncodeNameCode());
    iSampleBuffer->AddSample(iSample, length);

    // new chunk, add name of it
    length = EncodeChunkName(*aChunk);
    iSampleBuffer->AddSample(iSample, length);
    
    // add new chunk tag
    length = EncodeNewCode();
    iSampleBuffer->AddSample(iSample, length);

    length = EncodeChunkData(*aChunk);
    iSampleBuffer->AddSample(iSample, length);
    
    // add end mark
    AddFooter();
    Kern::MutexSignal(*iLock);
    NKern::ThreadLeaveCS();
    return ETrue;
    }

TBool DMemoryEventHandler::HandleUpdateChunk(DChunk* aChunk)
    {
//    Kern::Printf("DChunk updated: 0x%08x, time: %d", aChunk, iCount);
    
    NKern::ThreadEnterCS();
    Kern::MutexWait(*iLock);
    // add header first
    TInt err(AddHeader());
    
    if(err != KErrNone)
        {
        return EFalse;
        }
    
    // add new chunk tag
    TInt length(EncodeUpdateCode());
    iSampleBuffer->AddSample(iSample, length);

    length = EncodeChunkData(*aChunk);
    iSampleBuffer->AddSample(iSample, length);

    // add end mark
    AddFooter();
    Kern::MutexSignal(*iLock);
    NKern::ThreadLeaveCS();
    return ETrue;
    }

TBool DMemoryEventHandler::HandleDeleteChunk(DChunk* aChunk)
    {
//    Kern::Printf("DChunk deleted: 0x%08x, time: %d", aChunk, iCount);
    NKern::ThreadEnterCS();
    Kern::MutexWait(*iLock);
    // add header first
    TInt err(AddHeader());
    
    if(err != KErrNone)
        {
        return EFalse;
        }
    
    // add new chunk tag
    TInt length(EncodeRemoveCode());
    iSampleBuffer->AddSample(iSample, length);

    length = EncodeChunkData(*aChunk);
    iSampleBuffer->AddSample(iSample, length);

    // add end mark
    AddFooter();
    Kern::MutexSignal(*iLock);
    NKern::ThreadLeaveCS();
    return ETrue;
    }

// handle process activity
TBool DMemoryEventHandler::HandleAddProcess(DProcess *aProcess)
    {
    return ETrue;
    }

TBool DMemoryEventHandler::HandleUpdateProcess(DProcess *aProcess)
    {
    return ETrue;
    }

TBool DMemoryEventHandler::HandleDeleteProcess(DProcess *aProcess)
    {
    return ETrue;
    }

// handle thread activity
TBool DMemoryEventHandler::HandleAddThread(DThread* aThread)
    {
//    Kern::Printf("DThread added: 0x%08x, time: %d", aThread->iId, iCount);
    NKern::ThreadEnterCS();
    Kern::MutexWait(*iLock);
    // add header first
    TInt err(AddHeader());
    
    if(err != KErrNone)
        {
        return EFalse;
        }
    
    // new thread, add name of it
    TInt length(EncodeNameCode());
    iSampleBuffer->AddSample(iSample, length);
    
    // new chunk, add name of it
    length = EncodeChunkName(*aThread);
    iSampleBuffer->AddSample(iSample, length);
    
    // add new chunk tag
    length = EncodeNewCode();
    iSampleBuffer->AddSample(iSample, length);

    length = EncodeChunkData(*aThread);
    iSampleBuffer->AddSample(iSample, length);

    // add end mark
    AddFooter();
    Kern::MutexSignal(*iLock);
    NKern::ThreadLeaveCS();
    return ETrue;
    }

TBool DMemoryEventHandler::HandleUpdateThread(DThread* aThread)
    {
//    Kern::Printf("DThread updated: 0x%08x, time: %d", aThread->iId, iCount);
    NKern::ThreadEnterCS();
    Kern::MutexWait(*iLock);
    // add header first
    TInt err(AddHeader());
    
    if(err != KErrNone)
        {
        return EFalse;
        }
    
    // add new chunk tag
    TInt length(EncodeUpdateCode());
    iSampleBuffer->AddSample(iSample, length);

    length = EncodeChunkData(*aThread);
    iSampleBuffer->AddSample(iSample, length);

    // add end mark
    AddFooter();
    Kern::MutexSignal(*iLock);
    NKern::ThreadLeaveCS();    
    return ETrue;
    }

TBool DMemoryEventHandler::HandleDeleteThread(DThread* aThread)
    {
//    Kern::Printf("DThread deleted: 0x%08x, time: %d", aThread->iId, iCount);
    NKern::ThreadEnterCS();
    Kern::MutexWait(*iLock);
    // add header first
    TInt err(AddHeader());
    
    if(err != KErrNone)
        {
        return EFalse;
        }
    
    // add new chunk tag
    TInt length(EncodeRemoveCode());
    iSampleBuffer->AddSample(iSample, length);

    length = EncodeChunkData(*aThread);
    iSampleBuffer->AddSample(iSample, length);

    // add end mark
    AddFooter();
    Kern::MutexSignal(*iLock);
    NKern::ThreadLeaveCS();  
    return ETrue;
    }

TBool DMemoryEventHandler::HandleAddLibrary(DLibrary* aLibrary, DThread* aThread)
    {
    LOGTEXT("DMemoryEventHandler::HandleAddLibrary");
    Kern::Printf("DLibrary added: 0x%08x, time: %d", aLibrary, iCount);
    // add header first
    NKern::ThreadEnterCS();
    Kern::MutexWait(*iLock);
    TInt err(AddHeader());
        
    if(err != KErrNone)
        {
        return EFalse;
        }
    
    // new library, add name of it
    TInt length(EncodeNameCode());
    iSampleBuffer->AddSample(iSample, length);
        
    // new chunk, add name of it
    length = EncodeChunkName(*aLibrary);
    iSampleBuffer->AddSample(iSample, length);
        
    // add new chunk tag
    length = EncodeNewCode();
    iSampleBuffer->AddSample(iSample, length);

    length = EncodeChunkData(*aLibrary, *aThread);
    iSampleBuffer->AddSample(iSample, length);

    // add end mark
    AddFooter();
    Kern::MutexSignal(*iLock);
    NKern::ThreadLeaveCS();      
    return ETrue;
    }

TBool DMemoryEventHandler::HandleDeleteLibrary(DLibrary* aLibrary)
    {
    Kern::Printf("DLibrary deleted: 0x%08x, time: %d", aLibrary, iCount);
    NKern::ThreadEnterCS();
    Kern::MutexWait(*iLock);
    // add header first
    TInt err(AddHeader());
        
    if(err != KErrNone)
        {
        return EFalse;
        }
        
    // add new chunk tag
    TInt length(EncodeRemoveCode());
    iSampleBuffer->AddSample(iSample, length);
    
    DThread* nullPointer = NULL;
    length = EncodeChunkData(*aLibrary, *nullPointer);
    iSampleBuffer->AddSample(iSample, length);

    // add end mark
    AddFooter();
    Kern::MutexSignal(*iLock);
    NKern::ThreadLeaveCS();        
    return ETrue;
    }

// encode chunk name 
TInt DMemoryEventHandler::EncodeChunkName(DChunk& c)
    {   
    // the size of the following name is in the first byte
    TUint8* size(&iSample[0]);
    *size = 0;
        
    // encode chunk name
    iSampleDescriptor.Zero();
    iSampleDescriptor.Append(_L("C_"));
    c.TraceAppendFullName(iSampleDescriptor,false);
    *size += iSampleDescriptor.Size();
        
    // add chunk object address here
    TUint32 chunkAddr((TUint32)&c);
    iSampleDescriptor.Append((TUint8*)&(chunkAddr),sizeof(TUint32));
    *size += sizeof(TUint32);

    // the size is the descriptor length + the size field
    LOGSTRING2("Non-Heap Chunk Name - %d",*size);
    return ((TInt)(*size))+1;           
    }

// encode chunk name 
TInt DMemoryEventHandler::EncodeChunkName(DThread& t)
    {       
    // the size of the following name is in the first byte
    TUint8* size(&iSample[0]);
    *size = 0;
    iSampleDescriptor.Zero();
    
    iSampleDescriptor.Append(_L("T_"));
    t.TraceAppendFullName(iSampleDescriptor,false);
    *size += iSampleDescriptor.Size();
    
    // copy the 4 bytes from the thread id field
    iSampleDescriptor.Append((TUint8*)&(t.iId),sizeof(TUint));
    *size += sizeof(TUint);

    // the size is the descriptor length + the size field
    LOGSTRING2("Name - %d",*size);
    return ((TInt)(*size))+1;
    }

// encode chunk name 
TInt DMemoryEventHandler::EncodeChunkName(DLibrary& l)
    {       
    LOGTEXT("DMemoryEventHandler::EncodeChunkName (LIBRARY)");
    // the size of the following name is in the first byte
    TUint8* size(&iSample[0]);
    *size = 0;
    iSampleDescriptor.Zero();
    
    iSampleDescriptor.Append(_L("L_"));
    l.TraceAppendFullName(iSampleDescriptor,false);
    *size += iSampleDescriptor.Size();
    
    // copy the library address here
    TUint32 libAddr((TUint32)&l);
    iSampleDescriptor.Append((TUint8*) &libAddr,sizeof(TUint32));
    *size += sizeof(TUint32);

    // the size is the descriptor length + the size field
    LOGSTRING2("Name - %d",*size);
    return ((TInt)(*size))+1;
    }

// record thread stack changes
TInt DMemoryEventHandler::EncodeChunkData(DThread& t)
    {
    LOGSTRING("DMemoryEventHandler::EncodeChunkDataT - entry");
        
    // the size of the following name is in the first byte
    TUint8* size(&iSample[0]);
    *size = 0;
    iSampleDescriptor.Zero();

    iSampleDescriptor.Append((TUint8*)&(t.iId),sizeof(TUint));
    *size += sizeof(TUint);
        
    // copy the total amount of memory allocated for user side stack
    iSampleDescriptor.Append((TUint8*)&(t.iUserStackSize),sizeof(TInt));
    *size += sizeof(TInt);

    TInt zero(0);      
    // append the cell amount allocated (zero, not in use here)
    iSampleDescriptor.Append((TUint8*)&zero,sizeof(TInt));
    *size += sizeof(TInt);
    
    // append the chunk size (this is not a chunk)
    iSampleDescriptor.Append((TUint8*)&(zero),sizeof(TUint));
    *size += sizeof(TUint);

    // append user stack (max) size
    iSampleDescriptor.Append((TUint8*)&(t.iUserStackSize),sizeof(TInt));
    *size += sizeof(TInt);

//    Kern::Printf("TData -> %d",*size);
    return ((TInt)(*size))+1;
    }

// record chunk changes
TInt DMemoryEventHandler::EncodeChunkData(DChunk& c)
    {
    LOGSTRING("DMemoryEventHandler::EncodeChunkDataC - entry");
    
    // the size of the following name is in the first byte
    TUint8* size(&iSample[0]);
    *size = 0;
    iSampleDescriptor.Zero();
    TInt zero(0);

    TUint32 address((TUint32)&c);
        
    iSampleDescriptor.Append((TUint8*)&address,sizeof(TUint32));
    *size += sizeof(TUint);
    
    // copy the total amount of memory allocated
    iSampleDescriptor.Append((TUint8*)&(c.iSize),sizeof(TInt));
    *size += sizeof(TInt);
        
    // append the cell amount allocated
    iSampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
    *size += sizeof(TInt);
    
    // append the chunk size
    iSampleDescriptor.Append((TUint8*)&(c.iSize),sizeof(TUint));
    *size += sizeof(TUint);
        
    // append the thread user stack size
    iSampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
    *size += sizeof(TInt);

//    Kern::Printf("CData - %d",*size);
    return ((TInt)(*size))+1;
    }

// record loaded libraries changes
TInt DMemoryEventHandler::EncodeChunkData(DLibrary& l, DThread& t)
    {    
    LOGSTRING("DMemoryEventHandler::EncodeChunkDataL - entry");
    
    // the size of the following name is in the first byte
    TUint8* size(&iSample[0]);
    *size = 0;
    iSampleDescriptor.Zero();
    TInt zero(0);

    TUint32 address((TUint32)&l);
    
    iSampleDescriptor.Append((TUint8*)&address,sizeof(TUint32));
    *size += sizeof(TUint);
    
    // append amount of memory that library is allocated
    iSampleDescriptor.Append((TUint8*)&(l.iCodeSeg->iSize),sizeof(TUint32));
    *size += sizeof(TInt);
            
    // append count of how many times librarys is allocated
    iSampleDescriptor.Append((TUint8*)&(l.iMapCount),sizeof(TInt));
    *size += sizeof(TInt);
    
    if(&t != NULL)
        {
        // created by thread
        iSampleDescriptor.Append((TUint8*)&(t),sizeof(TUint32));
        }
    else
        {
        // removed
        iSampleDescriptor.Append((TUint8*)&(zero),sizeof(TUint32));
        }
    *size += sizeof(TUint);
            
    // append the thread user stack size
    iSampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
    *size += sizeof(TInt);
    return ((TInt)(*size))+1;
    }

