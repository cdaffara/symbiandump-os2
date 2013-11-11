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


#include <piprofiler/ProfilerVersion.h>
#include <piprofiler/ProfilerTraces.h>

#include <kern_priv.h>
#include <plat_priv.h>

#include "MemSamplerImpl.h"

// for testing precise stack utilization tracing...
// crashes at the moment

#include <nk_cpu.h>

#if !defined(__NKERN_H__)
#include <nkern.h>
#endif 

#define TAG(obj) (*(TUint32*)&(obj->iAsyncDeleteNext))
#define PROFILER_CHUNK_MARK		((TUint32)0x00001000)
#define PROFILER_MEM_THREAD_MARK	((TUint32)0x00000001)
#define PROFILER_LIBRARY_MARK    ((TUint32)0x10000000)
#define PROFILER_MEM_THREAD_UNMARK  ~PROFILER_MEM_THREAD_MARK

#ifdef MEM_EVENT_HANDLER
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
_LIT8(KMemVersion,"2.03");
#else
_LIT8(KMemVersion, "2.02");
#endif
#else
_LIT8(KMemVersion, "1.56");
#endif

DMemSamplerImpl::DMemSamplerImpl() :
	sampleDescriptor(&(this->sample[1]),0,256)
    {
    LOGSTRING("MemSamplerImpl::MemSamplerImpl() - konstruktori");

	iCount = 0;
		
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
	iSampleType = ESampleThreads;
#else
	iSampleThreads = true;
#endif
	iTimeToSample = false;
	
	iTotalMemoryOk = false;
	iTotalMemoryNameOk = false;
	
	iNewChunkCount = 0;
	iChunkCount = 0;
	iChunksProcessing = ENothingToProcess;
    iThreadsProcessing = ENothingToProcess;
	
	iNewThreadCount = 0;
	iThreadCount = 0;
	
	// reset data structures
    for(TInt i(0);i<KProfilerMaxChunksAmount;i++)
        {
        // heap chunks
        this->heapChunksToSample[i] = 0;
        this->heapChunkNamesToReport[i] = 0;
        }
    
    for(TInt i(0);i<KProfilerMaxThreadsAmount;i++)
        {
        // threads
        this->threadsToSample[i] = 0;
        this->threadNamesToReport[i] = 0;
        }
    
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
    iLibrariesProcessing = ENothingToProcess;
    iNewLibraryCount = 0;
    iLibraryCount = 0;
    
    for(TInt i(0); i<KProfilerMaxLibrariesAmount; i++)
        {
        // libraries
        this->librariesToSample[i] = 0;
        this->libraryNamesToReport[i] = 0;
        }
#endif

    }

DMemSamplerImpl::~DMemSamplerImpl()
    {

    }

TInt DMemSamplerImpl::CreateFirstSample()
    {
    LOGSTRING("MemSamplerImpl::CreateFirstSample - entry");
	
	this->sampleDescriptor.Zero();
	this->sampleDescriptor.Append(_L8("Bappea_V"));
	this->sampleDescriptor.Append(KMemVersion);
	this->sampleDescriptor.Append(_L8("_MEM"));
	
	sample[0] = this->sampleDescriptor.Size();

	LOGSTRING("MemSamplerImpl::CreateFirstSample - exit");

	return (TInt)(sample[0]+1);
    }

TBool DMemSamplerImpl::SampleNeeded()
    {
	iCount++;
#ifdef MEM_EVENT_HANDLER
    // make the collection of chunks/threads only once, rest will be collected with mem event handler
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
	if (iCount <= iMemSamplingPeriod && ((iCount % iMemSamplingPeriod) == 0 || (iCount % iMemSamplingPeriodDiv3) == 0))
#else
    if (iCount <= iMemSamplingPeriod && ((iCount % iMemSamplingPeriod) == 0 || (iCount % iMemSamplingPeriodDiv2) == 0))
#endif
#else
	if ((iCount % iMemSamplingPeriod) == 0 || (iCount % iMemSamplingPeriodDiv2) == 0)
#endif
	    {
        LOGSTRING2("MemSamplerImpl::SampleNeeded - time: %d", iCount);
		iTimeToSample = true;
		return true;
        }
	else 
	    {
		return false;
        }

    }
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
TInt DMemSamplerImpl::SampleImpl()
    {    
    // Sample threads:
    if( iSampleType == ESampleThreads )
        {    
        if(this->iThreadsProcessing == ENothingToProcess )
            {     
            if(!iTimeToSample)
                {
                return 0;
                }
            else
                {
                iTimeToSample = false;
                // gather first all thread stacks
                return GatherThreads();
                }
            }
        else
            {
            // process now thread stack list
            TInt length = this->ProcessThreads();

            if(length == 0)
                {
                this->iThreadsProcessing = ENothingToProcess;
                // switch to collect chunk data
                iSampleType = ESampleChunks;
                }
            return length;
            }
        }

    // Sample chunks:
    if( iSampleType == ESampleChunks )
        {
        if(this->iChunksProcessing == ENothingToProcess)
            {
            if(!iTimeToSample)
                {
                return 0;
                }
            else
                {
                iTimeToSample = false;
                // gather first all chunks
                return GatherChunks();
                }
            }
        else
            {
            // still something to go through in lists
            TInt length = this->ProcessChunks();
        
            if(length == 0) 
            {
                this->iChunksProcessing = ENothingToProcess;
                // switch to collect library data
                iSampleType = ESampleLibraries;
                //iSampleThreads = true;
            }
            return length;
            }
        }
        
    // Sample libraries:
    if( iSampleType == ESampleLibraries )
        {
        if(this->iLibrariesProcessing == ENothingToProcess )
            {        
            if(!iTimeToSample)
                {             
                return 0;
                }
            else
                {
                iTimeToSample = false;
                // gather libraries
                return GatherLibraries();
                }
            }
        else
            {
            // process now thread stack list
            TInt length = this->ProcessLibraries();
            if(length == 0)
                {
                this->iLibrariesProcessing = ENothingToProcess;
                // switch to collect chunk data
                iSampleType = ESampleThreads;
                }
            return length;
            }
        }

    // should not reach this point...
    return 0;
    }
#else
TInt DMemSamplerImpl::SampleImpl()
    {
    // check if either chunk or thread lists have unprocessed items
    if(this->iChunksProcessing == ENothingToProcess && !iSampleThreads)
        {
        if(!iTimeToSample)
            {
            return 0;
            }
        else
            {
            iTimeToSample = false;
            // gather first all chunks
            return GatherChunks();
            }
        }
    else if(!iSampleThreads)
        {
        // still something to go through in lists
        TInt length = this->ProcessChunks();
        
        if(length == 0) 
            {
            this->iChunksProcessing = ENothingToProcess;
            // switch to collect thread data
            iSampleThreads = true;
            }
        return length;
        }
    
    if(this->iThreadsProcessing == ENothingToProcess && iSampleThreads)
        {
        if(!iTimeToSample)
            {
            return 0;
            }
        else
            {
            iTimeToSample = false;
            // gather first all thread stacks
            return GatherThreads();
            }
        }
    
    else if(iSampleThreads)
        {
        // process now thread stack list
        TInt length = this->ProcessThreads();

        if(length == 0)
            {
            this->iThreadsProcessing = ENothingToProcess;
            // switch to collect chunk data
            iSampleThreads = false;
            }
        return length;
        }

    // should not reach this point...
    return 0;
    }
#endif

inline TInt DMemSamplerImpl::GatherChunks()
    {
    // encode a process binary
    name.Zero();
    
    NKern::ThreadEnterCS(); // Prevent us from dying or suspending whilst holding a DMutex
    DObjectCon& chunks = *Kern::Containers()[EChunk];
    chunks.Wait();  // Obtain the container mutex so the list does get changed under us
    
    this->iChunkCount = 0; 
    this->iNewChunkCount = 0;
    this->iTotalMemoryOk = false;
    TInt totalChunkCount(chunks.Count());
    DChunk* c; 
    
    for(TInt i(0);i<totalChunkCount;i++)
        {
        c = (DChunk*)(chunks)[i];

        LOGSTRING3("Processing chunk %d, tag: 0x%x",i,TAG(c));
        
        if( (TAG(c) & 0x0000ffff) != PROFILER_CHUNK_MARK)
            {
            LOGSTRING4("Marking chunk %d/%d, old tag 0x%x",i,(totalChunkCount-1), TAG(c));
            // this chunk has not been tagged yet
            name.Zero();
            c->TraceAppendName(name,false);
            
            TAG(c) = (PROFILER_CHUNK_MARK);
            this->heapChunkNamesToReport[iNewChunkCount] = c;
            iNewChunkCount++;
            }

        // the chunk has been tagged, add heap chunks to the list
        this->heapChunksToSample[this->iChunkCount] = c;
        this->iChunkCount++;
        LOGSTRING2("Added chunk %d to Chunks",i);
        }

    if(this->iChunkCount > 0 || this->iNewChunkCount > 0)
        {
        this->iChunksProcessing = EStartingToProcess;
        
        // process the first sample
        TInt length = this->ProcessChunks();
        
        if(length == 0)
            {
            this->iChunksProcessing = ENothingToProcess;
            }
    
        chunks.Signal();  // Release the container mutex
        NKern::ThreadLeaveCS();  // End of critical section
        return length;
        }

    LOGTEXT("MemSamplerImpl::SampleImpl - Error, no threads"); 
    chunks.Signal();  // Release the container mutex
    NKern::ThreadLeaveCS();  // End of critical section
    return 0;
    }

inline TInt DMemSamplerImpl::GatherThreads()
    {
    // The thread memory consumption
    
    NKern::ThreadEnterCS(); // Prevent us from dying or suspending whilst holding a DMutex
    DObjectCon& threads = *Kern::Containers()[EThread];
    threads.Wait(); // Obtain the container mutex so the list does get changed under us
    
    this->iThreadCount = 0; 
    this->iNewThreadCount = 0;
    this->iTotalMemoryOk = false;           

    TInt totalThreadCount = threads.Count();

    for(TInt i(0);i<totalThreadCount;i++)
        {
        DThread* t = (DThread*)(threads)[i];

        LOGSTRING3("Processing thread %d, tag: 0x%x",i,TAG(t));

        if( (TAG(t) & PROFILER_MEM_THREAD_MARK) == 0)
            {
            LOGSTRING4("Marking thread %d/%d, old tag 0x%x",i,(totalThreadCount-1), TAG(t));
            // this thread's chunk has not been reported yet
            this->threadNamesToReport[iNewThreadCount] = t;
            iNewThreadCount++;
            // tag the thread
            TAG(t) |= PROFILER_MEM_THREAD_MARK;
            }

        // the chunk has been tagged, add heap chunks to the list
        this->threadsToSample[this->iThreadCount] = t;
        this->iThreadCount++;
        LOGSTRING2("Added thread %d to threads to sample",i);
        }
    
    if(this->iThreadCount > 0 || this->iNewThreadCount > 0)
        {
        this->iThreadsProcessing = EStartingToProcess;
        
        // process the first sample
        TInt length = this->ProcessThreads();
        
        if(length == 0)
            {
            this->iThreadsProcessing = ENothingToProcess;
            }
        threads.Signal();  // Release the container mutex
        NKern::ThreadLeaveCS();  // End of critical section
        return length;
        }
    
    LOGTEXT("MemSamplerImpl::SampleImpl - Error, no threads"); 
    threads.Signal();  // Release the container mutex
    NKern::ThreadLeaveCS();  // End of critical section
    return 0;
    }

#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS

inline TInt DMemSamplerImpl::GatherLibraries()
    {
    LOGTEXT("MemSamplerImpl::GatherLibraries() - entry");
    // encode a process binary
    name.Zero();
    
    NKern::ThreadEnterCS(); // Prevent us from dying or suspending whilst holding a DMutex
    DObjectCon& libs = *Kern::Containers()[ELibrary];
    libs.Wait();  // Obtain the container mutex so the list does get changed under us
    
    this->iLibraryCount = 0; 
    this->iNewLibraryCount = 0;
    this->iTotalMemoryOk = false;
    TInt totalLibCount(libs.Count());
    DLibrary* l; 
    
    for(TInt i(0);i<totalLibCount;i++)
        {
        l = (DLibrary*)(libs)[i];

        LOGSTRING3("Processing library %d, tag: 0x%x",i,TAG(l));
        
        if( (TAG(l) & 0xffffffff) != PROFILER_LIBRARY_MARK)
            {
            LOGSTRING4("Marking library %d/%d, old tag 0x%x",i,(totalLibCount-1), TAG(l));
            // this library has not been tagged yet
            name.Zero();
            l->TraceAppendName(name,false);
            
            TAG(l) = (PROFILER_LIBRARY_MARK);
            this->libraryNamesToReport[iNewLibraryCount] = l;
            iNewLibraryCount++;
            }

        // the library has been tagged, add library to the list
        this->librariesToSample[this->iLibraryCount] = l;
        this->iLibraryCount++;
        LOGSTRING2("Added library %d to Libraries",i);
        }

    if(this->iLibraryCount > 0 || this->iNewLibraryCount > 0)
        {
        this->iLibrariesProcessing = EStartingToProcess;
        
        // process the first sample
        TInt length = this->ProcessLibraries();
        
        if(length == 0)
            {
            this->iLibrariesProcessing = ENothingToProcess;
            }
    
        libs.Signal();  // Release the container mutex
        NKern::ThreadLeaveCS();  // End of critical section
        return length;
        }

    LOGTEXT("MemSamplerImpl::SampleImpl - Error, no libraries"); 
    libs.Signal();  // Release the container mutex
    NKern::ThreadLeaveCS();  // End of critical section
    return 0;
    }
#endif

inline TInt DMemSamplerImpl::ProcessChunks()
    {
    if(iNewChunkCount > 0)
        {
        if(this->iChunksProcessing == EStartingToProcess)
            {
            // this is the first sample, encode a code for names
            this->iChunksProcessing = EProcessingNames;
            return EncodeNameCode();
            }

        if(iTotalMemoryNameOk == false)
            {
            return EncodeTotalMemoryName();
            }
        
        // there are new chunk names to report
        iNewChunkCount--;
        DChunk* c = this->heapChunkNamesToReport[iNewChunkCount];
        return EncodeChunkName(*c);
        
        }
    else if(iChunkCount > 0)
        {
        if(this->iChunksProcessing == EProcessingNames || this->iChunksProcessing == EStartingToProcess)
            {
            // this is the first data sample, encode a code for data
            this->iChunksProcessing = EProcessingData;
            return EncodeDataCode();
            }
        
        if(this->iTotalMemoryOk == false)
            {
            return EncodeTotalMemory();	
            }

        // there are no new chunks to report
        // thus generate the real report
        iChunkCount--;
        DChunk* c = this->heapChunksToSample[iChunkCount];
        return EncodeChunkData(*c);
        }
    else
        {
        // everything is processed
        LOGSTRING2(" Chunks processed! Chunk count = %d", iChunkCount);
#ifdef MEM_EVENT_HANDLER
        this->iChunksGathered = true;
        Kern::Printf("MemSamplerImpl::ProcessChunks() - chunks gathered! Time: %d",iCount);
#endif
        return 0;
        }
    }

inline TInt DMemSamplerImpl::ProcessThreads()
    {

    if(iNewThreadCount > 0)
        {
        if(this->iThreadsProcessing == EStartingToProcess)
            {
            // this is the first sample, encode a code for names
            this->iThreadsProcessing = EProcessingNames;
            return EncodeNameCode();
            }
        
        if(iTotalMemoryNameOk == false)
            {
            return EncodeTotalMemoryName();
            }

        iNewThreadCount--;
        DThread* t = this->threadNamesToReport[iNewThreadCount];
        return EncodeChunkName(*t);
        }
    else if(iThreadCount > 0)
        {
        if(this->iThreadsProcessing == EProcessingNames || this->iThreadsProcessing == EStartingToProcess)
            {
            // this is the first data sample, encode a code for data
            this->iThreadsProcessing = EProcessingData;
            return EncodeDataCode();
            }

        if(this->iTotalMemoryOk == false)
            {
            return EncodeTotalMemory(); 
            }

        // there are no new threads to report
        // thus generate the real report
        iThreadCount--;
        DThread* t = this->threadsToSample[iThreadCount];
        return EncodeChunkData(*t);
        }
    else
        {   
        // everything is processed
        LOGSTRING2(" Threads processed! Thread count = %d", iThreadCount);
#ifdef MEM_EVENT_HANDLER
        this->iThreadsGathered = true;
        Kern::Printf("MemSamplerImpl::ProcessThreads() - threads gathered! Time: %d", iCount);
#endif
        return 0;
        }
    }
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
inline TInt DMemSamplerImpl::ProcessLibraries()
    {
    LOGTEXT("ProcessLibraries - entry");
    if(iNewLibraryCount > 0)
        {
        if(this->iLibrariesProcessing == EStartingToProcess)
            {
            // this is the first sample, encode a code for names
            this->iLibrariesProcessing = EProcessingNames;
            return EncodeNameCode();
            }

        if(iTotalMemoryNameOk == false)
            {
            return EncodeTotalMemoryName();
            }
        
        // there are new library names to report
        iNewLibraryCount--;
        DLibrary* l = this->libraryNamesToReport[iNewLibraryCount];
        return EncodeChunkName(*l);
        
        }
    else if(iLibraryCount > 0)
        {
        if(this->iLibrariesProcessing == EProcessingNames || this->iLibrariesProcessing == EStartingToProcess)
            {
            // this is the first data sample, encode a code for data
            this->iLibrariesProcessing = EProcessingData;
            return EncodeDataCode();
            }
        
        if(this->iTotalMemoryOk == false)
            {
            return EncodeTotalMemory(); 
            }

        // there are no new libraries to report
        // thus generate the real report
        iLibraryCount--;
        DLibrary* l = this->librariesToSample[iLibraryCount];
        return EncodeChunkData(*l);
        }
    else
        {
        // everything is processed
        LOGSTRING2(" Libraries processed! Library count = %d", iLibraryCount);

        this->iLibrariesGathered = true;
        Kern::Printf("MemSamplerImpl::ProcessLibraries() - libraries gathered! Time: %d",iCount);

        return 0;
        }
    }
#endif
inline TInt DMemSamplerImpl::EncodeNameCode()
    {
	sample[0] = 1;
	sample[1] = 0xaa;
	return 2;
    }

inline TInt DMemSamplerImpl::EncodeDataCode()
    {
	sample[0] = 1;
	sample[1] = 0xdd;
	return 2;
    }

inline TInt DMemSamplerImpl::EncodeTotalMemoryName()
    {
	this->iTotalMemoryNameOk = true;
	
	TUint8* size = &sample[0];
	*size = 0;
		
	// encode name
	this->sampleDescriptor.Zero();
	this->sampleDescriptor.Append(_L("TOTAL_MEMORY"));
	*size += this->sampleDescriptor.Size();
		
	// add id here
	TUint32 id(0xbabbeaaa);
	this->sampleDescriptor.Append((TUint8*)&(id),sizeof(TUint32));
	*size += sizeof(TUint32);
	
	// the size is the descriptor length + the size field
	return ((TInt)(*size))+1;	
    }

inline TInt DMemSamplerImpl::EncodeTotalMemory()
    {	
	
	TUint8* size = &sample[0];
	*size = 0;

	NKern::LockSystem();
	TInt freeRam = Kern::FreeRamInBytes();
	TInt totalRam = Kern::SuperPage().iTotalRamSize;
	NKern::UnlockSystem();

	this->sampleDescriptor.Zero();
	
	TUint32 id(0xbabbeaaa);
	TInt zero(0);
		
	this->sampleDescriptor.Append((TUint8*)&(id),sizeof(TUint32));
	*size += sizeof(TUint);
	
	this->sampleDescriptor.Append((TUint8*)&(totalRam),sizeof(TInt));
	*size += sizeof(TInt);
		
	// append the cell amount allocated
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
	*size += sizeof(TInt);
	
	// append the chunk size
	this->sampleDescriptor.Append((TUint8*)&(freeRam),sizeof(TInt));
	*size += sizeof(TInt);
		
	// append the thread user stack size
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
	*size += sizeof(TInt);

	this->iTotalMemoryOk = true;

	return ((TInt)(*size))+1;
    }

inline TInt DMemSamplerImpl::EncodeChunkName(DChunk& c)
    {	
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
		
	// encode chunk name
	this->sampleDescriptor.Zero();
	this->sampleDescriptor.Append(_L("C_"));
	c.TraceAppendFullName(this->sampleDescriptor,false);
	*size += this->sampleDescriptor.Size();
		
	// add chunk object address here
	TUint32 chunkAddr((TUint32)&c);
	this->sampleDescriptor.Append((TUint8*)&(chunkAddr),sizeof(TUint32));
	*size += sizeof(TUint32);

	// the size is the descriptor length + the size field
	LOGSTRING2("Non-Heap Chunk Name - %d",*size);
	return ((TInt)(*size))+1;			
    }

inline TInt DMemSamplerImpl::EncodeChunkName(DThread& t)
    {		
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
	this->sampleDescriptor.Zero();
	
	this->sampleDescriptor.Append(_L("T_"));
	t.TraceAppendFullName(this->sampleDescriptor,false);
	*size += this->sampleDescriptor.Size();
	
	// copy the 4 bytes from the thread id field
	this->sampleDescriptor.Append((TUint8*)&(t.iId),sizeof(TUint));
	*size += sizeof(TUint);

	// the size is the descriptor length + the size field
	LOGSTRING2("Name - %d",*size);
	return ((TInt)(*size))+1;
    }
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
inline TInt DMemSamplerImpl::EncodeChunkName(DLibrary& l)
    {   
    // the size of the following name is in the first byte
    TUint8* size = &sample[0];
    *size = 0;
        
    // encode library name
    this->sampleDescriptor.Zero();
    this->sampleDescriptor.Append(_L("L_"));
    l.TraceAppendFullName(this->sampleDescriptor,false);
    *size += this->sampleDescriptor.Size();
        
    // add chunk object address here
    TUint32 libAddr((TUint32)&l);
    this->sampleDescriptor.Append((TUint8*)&(libAddr),sizeof(TUint32));
    *size += sizeof(TUint32);

    // the size is the descriptor length + the size field
    LOGSTRING2("Name - %d",*size);
    return ((TInt)(*size))+1;           
    }
#endif
inline TInt DMemSamplerImpl::EncodeChunkData(DChunk& c)
    {
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
	this->sampleDescriptor.Zero();
	TInt zero(0);

	TUint32 address((TUint32)&c);
		
	this->sampleDescriptor.Append((TUint8*)&address,sizeof(TUint32));
	*size += sizeof(TUint);
	
	// copy the total amount of memory allocated
	this->sampleDescriptor.Append((TUint8*)&(c.iSize),sizeof(TInt));
	*size += sizeof(TInt);
		
	// append the cell amount allocated
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
	*size += sizeof(TInt);
	
	// append the chunk size
	this->sampleDescriptor.Append((TUint8*)&(c.iSize),sizeof(TUint));
	*size += sizeof(TUint);
		
	// append the thread user stack size
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
	*size += sizeof(TInt);

	LOGSTRING2("Data - %d",*size);
	return ((TInt)(*size))+1;

    }

inline TInt DMemSamplerImpl::EncodeChunkData(DThread& t)
    {
	LOGTEXT("MemSamplerImpl::EncodeChunkData - entry");
	//LOGSTRING2("MemSamplerImpl::EncodeChunkData - processing thread 0x%x ",&t);
		
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
	this->sampleDescriptor.Zero();

	LOGTEXT("MemSamplerImpl::EncodeChunkData - cleared");

	this->sampleDescriptor.Append((TUint8*)&(t.iId),sizeof(TUint));
	*size += sizeof(TUint);
		
	// copy the total amount of memory allocated for user side stack
	this->sampleDescriptor.Append((TUint8*)&(t.iUserStackSize),sizeof(TInt));
	*size += sizeof(TInt);

	TInt zero(0);		
	// append the cell amount allocated (zero, not in use here)
	this->sampleDescriptor.Append((TUint8*)&zero,sizeof(TInt));
	*size += sizeof(TInt);
	
	// append the chunk size (this is not a chunk)
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TUint));
	*size += sizeof(TUint);

	// append user stack (max) size
	this->sampleDescriptor.Append((TUint8*)&(t.iUserStackSize),sizeof(TInt));
	*size += sizeof(TInt);

	LOGSTRING2("Data -> %d",*size);
	return ((TInt)(*size))+1;
    }
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
inline TInt DMemSamplerImpl::EncodeChunkData(DLibrary& l)
    {
    LOGTEXT("MemSamplerImpl::EncodeChunkData (Library) - entry");
    // the size of the following name is in the first byte
    TUint8* size = &sample[0];
    *size = 0;
    this->sampleDescriptor.Zero();
    
    TUint32 address((TUint32)&l);
        
    this->sampleDescriptor.Append((TUint8*)&address,sizeof(TUint32));
    *size += sizeof(TUint);
             
	this->sampleDescriptor.Append((TUint8*)&(l.iCodeSeg->iSize),sizeof(TUint32));
    *size += sizeof(TInt); 
             
    this->sampleDescriptor.Append((TUint8*)&(l.iMapCount),sizeof(TInt));
    *size += sizeof(TInt);  
        
    TInt zero(0);   
    this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
    *size += sizeof(TInt);   
        
    this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
    *size += sizeof(TInt);   

    LOGSTRING2("LData - %d",*size);
    return ((TInt)(*size))+1;

    }
#endif
void DMemSamplerImpl::Reset()
    {
	Kern::Printf("MemSamplerImpl::Reset");
	iCount = 0; // sample threads 1 cycle after actual MEM sample time...
    this->iTimeToSample = false;
    this->iChunkCount = 0;
	this->iNewChunkCount = 0;
	
	this->iTotalMemoryOk = false;
	this->iTotalMemoryNameOk = false;

	this->iChunksProcessing = ENothingToProcess;
    this->iThreadsProcessing = ENothingToProcess;
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
    this->iLibrariesProcessing = ENothingToProcess;
    this->iSampleType = ESampleThreads;
#else
    this->iSampleThreads = true;
#endif
    
	this->sampleDescriptor.Zero();
	
	// clear all chunk tags
    NKern::ThreadEnterCS(); // Prevent us from dying or suspending whilst holding a DMutex
	DObjectCon* chunks = Kern::Containers()[EChunk];
    chunks->Wait(); // Obtain the container mutex so the list does get changed under us

	TInt totalChunkCount = chunks->Count();
	for(TInt i=0;i<totalChunkCount;i++)
	    {
		DChunk* c = (DChunk*)(*chunks)[i];
		TAG(c) = 0;
	    }
	chunks->Signal();  // Release the container mutex

	Kern::Printf("MemSamplerImpl::Reset");
	this->iThreadCount = 0;
	this->iNewThreadCount = 0;
	this->sampleDescriptor.Zero();

	// clear all chunk tags
	DObjectCon* threads = Kern::Containers()[EThread];
    threads->Wait(); // Obtain the container mutex so the list does get changed under us

	TInt totalThreadCount = threads->Count();
	for(TInt i=0;i<totalThreadCount;i++)
	    {
		DThread* t = (DThread*)(*threads)[i];
		TAG(t) = (TAG(t) & 0xfffffffe);
	    }
	threads->Signal();  // Release the container mutex
	
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
	this->iLibraryCount = 0;
	this->iNewLibraryCount = 0;
	this->sampleDescriptor.Zero();

	// clear all library tags
	DObjectCon* libs = Kern::Containers()[ELibrary];
	libs->Wait(); // Obtain the container mutex so the list does get changed under us

	TInt totalLibraryCount = libs->Count();
	for(TInt i=0; i<totalLibraryCount; i++)
	    {
        DLibrary* l = (DLibrary*)(*libs)[i];
        TAG(l) = (TAG(l) & 0xefffffff);
	    }
	libs->Signal();  // Release the container mutex
#endif

    NKern::ThreadLeaveCS();  // End of critical section
    }

