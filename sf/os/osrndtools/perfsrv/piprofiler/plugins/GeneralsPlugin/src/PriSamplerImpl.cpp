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

#include "PriSamplerImpl.h"

#if !defined(__NKERN_H__)
#include <nkern.h>
#endif 

#define TAG(obj) (*(TUint32*)&(obj->iAsyncDeleteNext))
#define PROFILER_THREAD_MARK		((TUint32)0x00000002)


DPriSamplerImpl::DPriSamplerImpl() :
	sampleDescriptor(&(this->sample[1]),0,256)
    {
	LOGTEXT("PriSamplerImpl::PriSamplerImpl() - konstruktori");

	iCountti = 50;	// sample threads 16 cycles before actual MEM and PRI sample time...
	iNewThreadCount = 0;
	iThreadCount = 0;
	iProcessing = ENothingToProcess;
	
	for(TInt i=0;i<KProfilerMaxThreadAmount;i++)
	    {
		this->threadsToSample[i] = 0;
		this->threadNamesToReport[i] = 0;
        }

    }

DPriSamplerImpl::~DPriSamplerImpl()
    {
	
    }

TInt DPriSamplerImpl::CreateFirstSample()
    {
	LOGTEXT("PriSamplerImpl::CreateFirstSample - entry");
	
	this->sampleDescriptor.Zero();
	this->sampleDescriptor.Append(_L8("Bappea_V"));
	this->sampleDescriptor.Append(PROFILER_PRI_SAMPLER_VERSION);
	this->sampleDescriptor.Append(_L8("_PRI"));

	sample[0] = this->sampleDescriptor.Size();

	LOGTEXT("PriSamplerImpl::CreateFirstSample - exit");

	return (TInt)(sample[0]+1);
    }

TBool DPriSamplerImpl::SampleNeeded()
    {
	iCountti++;
	if(iCountti % (iPriSamplingPeriod) == 0) 
	    {
		LOGTEXT("PriSamplerImpl::SampleNeeded - true");
		return true;
        }
	else 
	    {
		return false;
        }
    }


TInt DPriSamplerImpl::SampleImpl()
    {
	/*
	 *
	 *	EKA-2 implementation of PRI trace
	 *
	 */
	if(this->iProcessing == ENothingToProcess)
	    {
	    if((iCountti % iPriSamplingPeriod) != 0 ) return 0;
	    
        LOGTEXT("Processing threads...");

        DObjectCon& threads = *Kern::Containers()[EThread];

        // PRI trace variables
        this->iThreadCount = 0; 
        this->iNewThreadCount = 0;
        TInt totalThreadCount = threads.Count();

        for(TInt i=0;i<totalThreadCount;i++)
            {
            DThread* t = (DThread*)(threads)[i];
            LOGSTRING3("Processing thread %d, tag: 0x%x",i,TAG(t));

            if( (TAG(t) & PROFILER_THREAD_MARK) == 0)
                {
                LOGSTRING2("Marking thread %d",i);
                // this thread's chunk has not been reported yet
                this->threadNamesToReport[iNewThreadCount] = t;
                iNewThreadCount++;
                // tag the thread
                TAG(t) |= PROFILER_THREAD_MARK;
                LOGSTRING2("New Thread %d",i);
                }
            else
                {
                LOGSTRING3("Thread %d marked already - 0x%x",i,TAG(t));
                }

            // the thread has been tagged, add heap chunks to the list
            this->threadsToSample[this->iThreadCount] = t;
            this->iThreadCount++;
            LOGSTRING2("Added thread %d to threads to sample",i);
            }

        if(this->iThreadCount > 0 || this->iNewThreadCount > 0)
            {
            this->iProcessing = EStartingToProcess;
            
            // process the first sample
            TInt length = this->ProcessChunks();
            
            if(length == 0)
                {
                this->iProcessing = ENothingToProcess;
                }
        
            return length;
            }
        else
            {
            // there were no threads, should not take place
            LOGTEXT("PriSamplerImpl::SampleImpl - Error, no threads"); 
            return 0;
            }
	    }
	else
	    {
		TInt length = this->ProcessChunks();
		if(length == 0) 
		    {
			this->iProcessing = ENothingToProcess;
		    }
		return length;
	    }
    }

inline TInt DPriSamplerImpl::ProcessChunks()
    {
	/*
	 *
	 *	EKA-2 implementation of PRI trace
	 *
	 */

	if(iNewThreadCount > 0)
	    {
		if(this->iProcessing == EStartingToProcess)
		    {
			// this is the first sample, encode a code for names
			this->iProcessing = EProcessingNames;
			return EncodeNameCode();
            }

		// there are new thread names to report
		iNewThreadCount--;
		DThread* t = this->threadNamesToReport[iNewThreadCount];
		return EncodeChunkName(*t);
        }
	else if(iThreadCount > 0)
	    {
		if(this->iProcessing == EProcessingNames || this->iProcessing == EStartingToProcess)
		    {
			// this is the first data sample, encode a code for data
			this->iProcessing = EProcessingData;
			return EncodeDataCode();
            }

		// there are no new chunks to report
		// thus generate the real report
		iThreadCount--;
		DThread* t = this->threadsToSample[iThreadCount];
		LOGSTRING2("PriSamplerImpl::ProcessChunks - starting to process thread 0x%x",t);
		return EncodeChunkData(*t);
        }
	else
	    {
		// everything is processed
		return 0;
        }
    }

inline TInt DPriSamplerImpl::EncodeNameCode()
    {
	sample[0] = 1;
	sample[1] = 0xbb;	// pri trace name code
	return 2;
    }

inline TInt DPriSamplerImpl::EncodeDataCode()
    {
	sample[0] = 1;
	sample[1] = 0xee;	// pri trace data code
	return 2;
    }


inline TInt DPriSamplerImpl::EncodeChunkName(DThread& t)
    {		
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
	this->sampleDescriptor.Zero();
	
	t.TraceAppendFullName(this->sampleDescriptor,false);
	*size += this->sampleDescriptor.Size();
	
	// copy the 4 bytes from the thread id field
	this->sampleDescriptor.Append((TUint8*)&(t.iId),sizeof(TUint));
	*size += sizeof(TUint);

	// the size is the descriptor length + the size field
	LOGSTRING2("Name size - %d",*size);
	return ((TInt)(*size))+1;
    }


inline TInt DPriSamplerImpl::EncodeChunkData(DThread& t)
    {
	LOGTEXT("PriSamplerImpl::EncodeChunkData - entry");
	LOGSTRING2("PriSamplerImpl::EncodeChunkData - processing thread 0x%x ",&t);
		
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
	this->sampleDescriptor.Zero();

	LOGTEXT("PriSamplerImpl::EncodeChunkData - cleared");

    // append the thread id
    this->sampleDescriptor.Append((TUint8*)&(t.iId),sizeof(TUint));
    *size += sizeof(TUint);
    
//    NKern::LockSystem();
//    TInt priority(-1);
//    if(&t && t.Open()== KErrNone)
//        {
//       priority = t.iDefaultPriority;
//        }
//    NKern::UnlockSystem();
    
    // append the priority of the nanokernel fast semaphore
//    this->sampleDescriptor.Append((TUint8*)&priority,sizeof(TUint8));
    // append the priority of the nanokernel fast semaphore
    this->sampleDescriptor.Append((TUint8*)&(t.iNThread.iPriority),sizeof(TUint8));
    // add space because EKA-1 implementation needs it
    this->sampleDescriptor.Append((TUint8)0x0);
    *size += 2*sizeof(TUint8);

    LOGTEXT("PriSamplerImpl::EncodeChunkData - appended priority");
        

    LOGSTRING2("Data size - %d",*size);
    return ((TInt)(*size))+1;	
    }


void DPriSamplerImpl::Reset()
    {
	/*
	 *
	 *	EKA-2 implementation of PRI trace
	 *
	 */

	LOGTEXT("PriSamplerImpl::Reset");
	iCountti = 50;	// sample threads 16 cycles before actual MEM and PRI sample time...
	this->iThreadCount = 0;
	this->iNewThreadCount = 0;
	this->iProcessing = ENothingToProcess;
	this->sampleDescriptor.Zero();


	// clear all thread tags
	DObjectCon* threads = Kern::Containers()[EThread];
	TInt totalThreadCount = threads->Count();
	for(TInt i=0;i<totalThreadCount;i++)
	    {
		DThread* t = (DThread*)(*threads)[i];
		TAG(t) = (TAG(t) & 0xfffffffd);
	    }
    }

