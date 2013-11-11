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

#include <kern_priv.h>
#include <plat_priv.h>

#include "IttSamplerImpl.h"

#ifdef ITT_EVENT_HANDLER
_LIT8(KIttVersion, "2.00");
#else
_LIT8(KIttVersion, "1.22");
#endif

/*
 *	ITT sampler definition
 *  	
 */
IttSamplerImpl::IttSamplerImpl():
        sampleDescriptor(&(this->sample[1]),0,KITTSampleBufferSize)
{
	this->currentLibCount = 0;
	iTimeToSample = EFalse;
	this->Reset();
}

/*
 * destructor
 */
IttSamplerImpl::~IttSamplerImpl()
{

}

/*
 * IttSamplerImpl::CreateFirstSample()
 * 
 * Function for creating the first sample to the log file
 */
TInt IttSamplerImpl::CreateFirstSample() 
{	
    Kern::Printf("ittSamplerImpl::createFirstSample\n");
	this->iVersionData.Zero();
	this->iVersionData.Append(_L8("Bappea_ITT_V"));
	this->iVersionData.Append(KIttVersion);
	this->itt_sample = (TUint8*)iVersionData.Ptr();
	return iVersionData.Length();
}

/*
 * IttSamplerImpl::SampleNeeded(TUint32 sampleNum)
 * 
 * @param TUint32 Sample number
 * 
 */
TBool IttSamplerImpl::SampleNeeded(TUint32 sampleNum)
{
#ifdef ITT_EVENT_HANDLER
    iCount++;
    if (iCount <= iIttSamplingPeriod && ((iCount % iIttSamplingPeriod) == 0 || (iCount % iIttSamplingPeriodDiv2) == 0))
    {
        LOGSTRING2("IttSamplerImpl::SampleNeeded - time: %d", iCount);
        iTimeToSample = true;
#else
    // no need to do anything, always a good time to sample.
    // Sample time filtering is done in IttSamplerImpl:SampleImpl() function
#endif
        return true;
#ifdef ITT_EVENT_HANDLER    
    }
    else 
    {
        return false;
    }
#endif
}

/*
 * IttSamplerImpl::SampleImpl(TUint32 pc, TUint32 sampleNum)
 * 
 * @param TUint32 program counter
 * @param TUint32 sample number
 */
TInt IttSamplerImpl::SampleImpl(TUint32 pc,TUint32 sampleNum)
{	
    // in order to avoid overloading the interrupt
	// only one dynamic file in each 50ms is added to the stream
	// with the application of the tool in mind, this is
	// a reasonable measure

    // encode a process binary
    sampleDescriptor.Zero();
	// original 
	if((sampleNum % 20) != 0) return 0;
	if((sampleNum % 40) == 0)
	{
		// encode a library binary
		sampleDescriptor.Zero();
		DObjectCon* libs = Kern::Containers()[ELibrary];
		TInt libCount = libs->Count();
		
		// go 20 binaries through at a time
		for(TInt i=0;i<20;i++)
		{
			if(currentLibCount >= libCount)
			{
				currentLibCount = 0;
			}
			
			DLibrary* lib = (DLibrary*)(*libs)[currentLibCount];
			currentLibCount++;
			
			DCodeSeg* seg = lib->iCodeSeg;
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->sample[0] = seg->iFileName->Length();
					sampleDescriptor.Append(*(seg->iFileName));
					sampleDescriptor.Append((TUint8*)&(seg->iRunAddress),4);
					sampleDescriptor.Append((TUint8*)&(seg->iSize),4);
#ifdef ITT_EVENT_HANDLER
					sampleDescriptor.Append((TUint8*)&(sampleNum),4);
					//Kern::Printf("DLL: NM %S : RA:0x%x SZ:0x%x, SN:0x%x",seg->iFileName,seg->iRunAddress,seg->iSize, sampleNum);
					this->iFirstSampleTaken = ETrue;
#else
		            //Kern::Printf("DLL: NM %S : RA:0x%x SZ:0x%x",seg->iFileName,seg->iRunAddress,seg->iSize);
#endif
					seg->iMark = (seg->iMark | 0x80);
					
					this->sample[0] = sampleDescriptor.Size();
					return sampleDescriptor.Size()+1;
				}
			}
		}
	} else
	{
		SDblQue* codeSegList = Kern::CodeSegList();
		//Kern::Printf("PI");
		//TUint c = 0;
		// the global list
		for (SDblQueLink* codeseg= codeSegList->First(); codeseg!=(SDblQueLink*) codeSegList; codeseg=codeseg->iNext)
		{				
			DCodeSeg* seg = _LOFF(codeseg, DCodeSeg, iLink);
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->sample[0] = seg->iFileName->Length();
					sampleDescriptor.Append(*(seg->iFileName));
					sampleDescriptor.Append((TUint8*)&(seg->iRunAddress),4);
					sampleDescriptor.Append((TUint8*)&(seg->iSize),4);
#ifdef ITT_EVENT_HANDLER
                    sampleDescriptor.Append((TUint8*)&(sampleNum),4);
                    //Kern::Printf("EXE2: NM %S : RA:0x%x SZ:0x%x, time: %d",seg->iFileName,seg->iRunAddress,seg->iSize, sampleNum);
                    this->iFirstSampleTaken = ETrue;                    
#else
					//Kern::Printf("EXE2: NM %S : RA:0x%x SZ:0x%x, time: %d",seg->iFileName,seg->iRunAddress,seg->iSize, sampleNum);
#endif					
					seg->iMark = (seg->iMark | 0x80);
					
					this->sample[0] = sampleDescriptor.Size();
					return sampleDescriptor.Size()+1;
				}
			}
		}	
	}
	return 0;
}

/*
 * IttSamplerImpl::Reset()
 */
void IttSamplerImpl::Reset()
{
    iTimeToSample = EFalse;
#ifdef ITT_EVENT_HANDLER
    iFirstSampleTaken = EFalse;
#endif
	this->currentLibCount = 0;
	this->itt_sample = (TUint8*)&(this->sample[0]);
	sampleDescriptor.Zero();

//	#ifdef ITT_TEST	
	SDblQue* codeSegList = Kern::CodeSegList();
	// the global list
	for (SDblQueLink* codeseg= codeSegList->First(); codeseg!=(SDblQueLink*) codeSegList; codeseg=codeseg->iNext)
	{				
		DCodeSeg* seg = _LOFF(codeseg,DCodeSeg, iLink);
		//if(seg != 0)
		{
			if( (seg->iMark & 0x80) > 0)
			{
				seg->iMark = (seg->iMark & ~0x80);
			}
		}
	}	
	// the garbage list
	DObjectCon* libs = Kern::Containers()[ELibrary];
	TInt libCount = libs->Count();
	for(TInt i=0;i<libCount;i++)
	{
		DLibrary* lib = (DLibrary*)(*libs)[i];
		DCodeSeg* seg = lib->iCodeSeg;
		if( (seg->iMark & 0x80) > 0)
		{
			seg->iMark = (seg->iMark & ~0x80);
		}
	}
	
	DObjectCon* procs = Kern::Containers()[EProcess];
	TInt procCount = procs->Count();
	for(TInt i=0;i<procCount;i++)
	{
		DProcess* pro = (DProcess*)(*procs)[i];
		DCodeSeg* seg = pro->iCodeSeg;
		if(seg != 0)
		{
			if( (seg->iMark & 0x80) > 0)
			{
				seg->iMark = (seg->iMark & ~0x80);
			}
		}
	}
	//#endif   //ITT_TEST
}
// end of file
