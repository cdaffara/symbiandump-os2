// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file te_parser.cpp
 @internalTechnology
*/
#include "te_parser.h"
//#include "te_suite_defs.h"
#include "te_ostv2integsuite_defs.h"


TTraceParser::TTraceParser()
	{
	iMissingTraceFound = EFalse;
	iTimestamp = 0;
	iTimestamp2Present = 0;
	iLastTrace=EFalse;
	iTestTrace=EFalse;
	}

TTraceParser::~TTraceParser()
	{
	}


TInt TTraceParser::ParseRawBuffer(TUint8* aRawBuffer, TUint aBufferSize, RPointerArray<TTraceConfigs>& aLoggedTraces)
	{
	TInt error = KErrNone;
	iNumSystemTraces=0; //initialise count of system traces
	// process all the complete traces in buffer...
	const TUint8* data = aRawBuffer;
	TUint sizeRemaining = aBufferSize;
	while(sizeRemaining>BTrace::ESizeIndex)
		{
		iLastTrace=EFalse;
		TTraceConfigs* trace = new TTraceConfigs;
		ASSERT(trace);
		TTraceConfigsOperator::Init(*trace);
		while(!iLastTrace)
			{
			TUint traceSize = (data[BTrace::ESizeIndex]+3)&~3;
			if(traceSize>sizeRemaining)
				{
				error = KErrNoMemory;
				break;
				}

			TBool parseresult=PreProcessTrace(*trace,data);
			if(!parseresult)
				{
				if (iMissingTraceFound)
					{
					// bad trace, create dummy 1 byte trace record...
//					RDebug::Printf("Missing trace is found");
//					memset(trace,0,sizeof(*trace));
//					trace->iGroupId = BTrace::EMetaTrace;
//					trace->iSubCategory = KJunkTraceCategory;
//					trace->iRawDataSize = 4;
//					trace->iRawData[0] = *data;
//					traceSize = 1;
					}
				else // The buffer was filled so ignore the rest of the data
					{
					//RDebug::Printf("No missing trace found, buffer filled so ignore rest of data");
					}
				}
			data += traceSize;
			sizeRemaining -= traceSize;
			}
		if(error == KErrNoMemory)
			return error;
		if (iTestTrace)
			aLoggedTraces.Append(trace);
		else
			delete trace;
		}
	return error;
	}

TBool TTraceParser::PreProcessTrace(TTraceConfigs& aTrace, const TUint8* aData)
	{
	iTestTrace=EFalse;
	// process aTrace header...
	TUint traceSize = aData[BTrace::ESizeIndex];
	if(traceSize < 4u || traceSize > (TUint)KMaxBTraceRecordSize)
		return false; // bad size

	aTrace.iFlags = aData[BTrace::EFlagsIndex];
//	if(!TraceRecordId)						// first trace record...?
//		aTrace.iFlags &= ~BTrace::EMissingRecord;	// ignore missing traces before log start

	aTrace.iGroupId = aData[BTrace::ECategoryIndex];

	if (aTrace.iGroupId>=129)	//check trace is sent by the test suite and not from somewhere else in the system (This should also catch OST traces with a GroupId of 162)
		iTestTrace=ETrue;
	else
		iNumSystemTraces++;

	aTrace.iSubCategory = aData[BTrace::ESubCategoryIndex];
	if(aTrace.iSubCategory==EOstTraceActivationQuery)
	    iTestTrace=EFalse;
	    
	const TUint8* header = aData + 4;

	if(aTrace.iFlags&BTrace::EHeader2Present) //if header2 is present check that it is because the trace is multiparted not because there are multiple cores
		{
		aTrace.iHeader2 = ReadTraceWord(header);
		if ((aTrace.iHeader2 & BTrace::EMultipartFlagMask))
		     {
                //if this is the last part of the multi-part trace, set iLastTrace to true
                if((aTrace.iHeader2&BTrace::EMultipartFlagMask) == BTrace::EMultipartLast)
                    {
                    iLastTrace=ETrue;
                    }
		     }
		else //if this is not a multi-parted trace, set iLastTrace to true
		    {
		    iLastTrace=ETrue;		          
		    }
		}
    else //if header2 is not present, this is not a multi-parted trace, set iLastTrace to true
        {
        iLastTrace=ETrue;                 
        }
	
	// process timestamp...
	TUint64 timestamp = 0;
	if(aTrace.iFlags&BTrace::ETimestampPresent)
		{
		timestamp = ReadTraceWord(header);
		if(timestamp<(iTimestamp&0xffffffffu))
			iTimestamp += TUint64(1)<<32;
		iTimestamp &= TUint64(0xffffffff)<<32;
		iTimestamp |= timestamp;
		timestamp = iTimestamp;
//		if(!TraceRecordId)
//			TimestampBase = timestamp; // record timestamp of first trace
		}
	aTrace.iTimestamp = timestamp;

	// process timestamp2...
	if(aTrace.iFlags&BTrace::ETimestamp2Present)
		{
		iTimestamp2Present = true;
		aTrace.iTimestamp2 = ReadTraceWord(header);
		}

	// process context...
	if(aTrace.iFlags&BTrace::EContextIdPresent)
		{
		aTrace.iHasThreadId = EAddThreadIdentification;
		aTrace.iContextId = ReadTraceWord(header);
		}


	// process pc...
	if(aTrace.iFlags & BTrace::EPcPresent)
		{
		aTrace.iPc = ReadTraceWord(header);
		aTrace.iHasProgramCounter = EAddProgramCounter;
		}

	// process extra...
	if(aTrace.iFlags & BTrace::EExtraPresent)
		aTrace.iExtra = ReadTraceWord(header);

	// process beginning of payload data...
	TUint headerSize = header - aData;
	aData = (TUint8*)header;
	if(headerSize > traceSize)
		return false; // bad trace record, header was read beyond start point of payload


	TUint dataSize = traceSize - headerSize;
	if(dataSize > KMaxBTraceRecordSize)//sizeof(aTrace.iRawData)
		return false; // bad trace record, its bigger than max allowed size

	if(dataSize < 8)
		return false; //bad trace, it's meant to contain both uid and format id


	//process component id...
	if(aTrace.iFlags&BTrace::EHeader2Present)//if header2 is present check that it is because the trace is multiparted not because there are multiple cores
		{
        if ((aTrace.iHeader2 & BTrace::EMultipartFlagMask))
            {
            //if this is not the first part of the multi-part trace, store this value as the size (then offset), not ComponentId
            if((aTrace.iHeader2&BTrace::EMultipartFlagMask) != BTrace::EMultipartFirst)
                {
                ReadTraceWord(aData);
                ReadTraceWord(aData);
                dataSize -= 8;
                }
            else
                {
                ReadTraceWord(aData);
                aTrace.iComponentId = ReadTraceWord(aData);
                dataSize -= 8;
                }
		    }
        else
            {
            aTrace.iComponentId = ReadTraceWord(aData);
            dataSize -= 4;
            }
		}
    else
        {
        aTrace.iComponentId = ReadTraceWord(aData);
        dataSize -= 4;
        }
	//process format id...

	if ((aTrace.iFlags&BTrace::EHeader2Present))//if header2 is present check that it is because the trace is multiparted not because there are multiple cores
		{
		if ((aTrace.iHeader2 & BTrace::EMultipartFlagMask))
		    {
            if (((aTrace.iHeader2&BTrace::EMultipartFlagMask) == BTrace::EMultipartFirst))
                {
                aTrace.iTraceId = ReadTraceWord(aData);
                dataSize -= 4;
                }
		    }
		else
            {
            aTrace.iTraceId = ReadTraceWord(aData);
            dataSize -= 4;
            }
		}
	else
		{
        aTrace.iTraceId = ReadTraceWord(aData);
        dataSize -= 4;
		}

	//process rest of payload data...
	if(dataSize > 0)
		{
		TUint32 temp=(((aTrace.iRawDataSize)+3)&~3)/4; //ensure this is 4-byte aligned
		if(iTestTrace)    //only copy the data if it is a trace point we are interested in
		    memcpy(&aTrace.iRawData[temp] , aData, dataSize);//previously offset/4
		aTrace.iRawDataSize += dataSize;
		}
	/* Legacy code from btrace analyze...
	// clear pre-processor specific data...
	aTrace.iDataTypes[0] = 0;
	aTrace.iDataTypes[1] = 0;
	aTrace.iDataTypes[2] = 0;
	aTrace.iDataTypes[3] = 0;
	aTrace.iCalculatedData[0] = 0;
	aTrace.iCalculatedData[1] = 0;
	 */

	// check for missing.
	if(aTrace.iFlags & BTrace::EMissingRecord)
		{// Some trace was missing as the btrace buffer must have been filled.
		iMissingTraceFound = true;
		return false;
		}

	// update trace ID...
//	++TraceRecordId;
	
	return true;
	}

TUint32 TTraceParser::ReadTraceWord(const TUint8*& data)
	{
	TUint32 word;
	memcpy(&word, data, sizeof(TUint32));
	data += sizeof(TUint32);
	return word;
	}

/******************TTraceHeaderParser Implementation********************/

TTraceHeaderParser::TTraceHeaderParser()
	{
	iMissingTraceFound = EFalse;
	iLastTrace=EFalse;
	iTestTrace=EFalse;
	iNumSystemTraces=0;
	}

TTraceHeaderParser::~TTraceHeaderParser()
	{
	}

TInt TTraceHeaderParser::ParseRawBuffer(TUint8* aRawBuffer, TUint aBufferSize, CArrayPtrSeg<TTraceHeaderInfo>*& aLoggedTraces)
	{
	TInt error = KErrNone;
	//iNumSystemTraces=0; //initialise count of system traces
	// process all the complete traces in buffer...
	const TUint8* data = aRawBuffer;
	TUint sizeRemaining = aBufferSize;
	while(sizeRemaining>BTrace::ESizeIndex)
		{
		iLastTrace=EFalse;
		TTraceHeaderInfo* traceHeader = new TTraceHeaderInfo;
		ASSERT(traceHeader);
		TTraceConfigsOperator::Init(*traceHeader);
		while(!iLastTrace)
			{
			TUint traceSize = (data[BTrace::ESizeIndex]+3)&~3;
			if(traceSize>sizeRemaining)
				{
				error = KErrNoMemory;
				break;
				}

			TBool parseresult=PreProcessTrace(*traceHeader,data);
			if(!parseresult)
				{
				if (iMissingTraceFound)//for debugging
					{
					//RDebug::Printf("Missing trace found");
					}
				else // The buffer was filled so ignore the rest of the data
					{
					//RDebug::Printf("The buffer was filled, rest of the data ignored ");
					}
				}
			data += traceSize;
			sizeRemaining -= traceSize;
			}
		if(error == KErrNoMemory)
			return error;
		if (iTestTrace)
			{
			TRAP(error,(aLoggedTraces->AppendL(traceHeader)));
			if (error)
				{
				delete traceHeader;
				return error;
				}

			}
		else
			delete traceHeader;
		}
	return error;
	}

TBool TTraceHeaderParser::PreProcessTrace(TTraceHeaderInfo& aTraceHeader, const TUint8* aData)
	{
	iTestTrace=EFalse;
	TUint32 header2 = 0;
	TUint8 flags=0;
	// process aTrace header...
	TUint traceSize = aData[BTrace::ESizeIndex];
	if(traceSize < 4u || traceSize > (TUint)KMaxBTraceRecordSize)
		return false; // bad size

	flags = aData[BTrace::EFlagsIndex];

	aTraceHeader.iGroupId = aData[BTrace::ECategoryIndex];

	if (aTraceHeader.iGroupId>129)	//check trace is sent by the test suite and not from somewhere else in the system
		iTestTrace=ETrue;
	else
		iNumSystemTraces++;

	aTraceHeader.iSubCategory = aData[BTrace::ESubCategoryIndex];

	const TUint8* header = aData + 4;

	if(flags&BTrace::EHeader2Present)
		{
		header2 = ReadTraceWord(header);
		//if this is the last part of the multi-part trace, set iLastTrace to true
		if((header2&BTrace::EMultipartFlagMask) == BTrace::EMultipartLast)
			iLastTrace=ETrue;
		}
	else	//no header2 present so it is the "last trace"
		iLastTrace=ETrue;

	// process timestamp...
	if(flags&BTrace::ETimestampPresent)
		header+=sizeof(TUint32);

	// process timestamp2...
	if(flags&BTrace::ETimestamp2Present)
		header+=sizeof(TUint32);

	// process context...
	if(flags&BTrace::EContextIdPresent)
		{
		aTraceHeader.iHasThreadId = EAddThreadIdentification;
		aTraceHeader.iContextId = ReadTraceWord(header);
		}


	// process pc...
	if(flags & BTrace::EPcPresent)
		{
		aTraceHeader.iPc = ReadTraceWord(header);
		aTraceHeader.iHasProgramCounter = EAddProgramCounter;
		}

	// process extra...
	if(flags & BTrace::EExtraPresent)
		header+=sizeof(TUint32);

	// process beginning of payload data...
	TUint headerSize = header - aData;
	aData = (TUint8*)header;
	if(headerSize > traceSize)
		return false; // bad trace record, header was read beyond start point of payload


	TUint dataSize = traceSize - headerSize;
	if(dataSize > KMaxBTraceRecordSize)//KMaxBTraceRecordSize
		return false; // bad trace record, its bigger than max allowed size

	if(dataSize < 8)
		return false; //bad trace, it's meant to contain both uid and format id

	//TUint32 offset = 0;
	//TUint32 totalsize = 0;

	//process module uid...
	if(flags&BTrace::EHeader2Present)
		{
		//if this is not the first part of the multi-part trace, store this value as the size (then offset), not ComponentId
		if((header2&BTrace::EMultipartFlagMask) != BTrace::EMultipartFirst)
			{
			/*totalsize =*/ ReadTraceWord(aData);
			/*offset =*/ ReadTraceWord(aData);
			dataSize -= 8;
			}
		else
			{
			/*totalsize =*/ ReadTraceWord(aData);
			aTraceHeader.iComponentId = ReadTraceWord(aData);
			dataSize -= 8;
			}

		}
	else
		{
		aTraceHeader.iComponentId = ReadTraceWord(aData);
		dataSize -= 4;
		}
	//process format id...

	if ((flags&BTrace::EHeader2Present))
		{
		if (((header2&BTrace::EMultipartFlagMask) == BTrace::EMultipartFirst))
			{
			aTraceHeader.iTraceId = ReadTraceWord(aData);
			dataSize -= 4;
			}
		}
	else
		{
		aTraceHeader.iTraceId = ReadTraceWord(aData);
		dataSize -= 4;
		}

	// check for missing.
	if(flags & BTrace::EMissingRecord)
		{// Some trace was missing as the btrace buffer must have been filled.
		iMissingTraceFound = ETrue;
		return EFalse;
		}

	return ETrue;
	}

TUint32 TTraceHeaderParser::ReadTraceWord(const TUint8*& data)
	{
	TUint32 word;
	memcpy(&word, data, sizeof(TUint32));
	data += sizeof(TUint32);
	return word;
	}


