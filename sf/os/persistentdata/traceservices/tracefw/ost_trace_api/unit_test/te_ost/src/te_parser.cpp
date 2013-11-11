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
#include "te_logger.h"
#include "te_suite_defs.h"
#include "te_tracer.h"


TTraceParser::TTraceParser()
	{
	iMissingTraceFound = false;
	iTimestamp = 0;
	iTimestamp2Present = 0;
	}

TTraceParser::~TTraceParser()
	{
	}


TInt TTraceParser::ParseRawBuffer(TUint8* aRawBuffer, TUint aBufferSize, RPointerArray<TTraceConfigs>& aLoggedTraces)
	{
	TInt error = KErrNone;
	// process all the complete traces in buffer...
	const TUint8* data = aRawBuffer;
	TUint sizeRemaining = aBufferSize;
	while(sizeRemaining>BTrace::ESizeIndex)
		{
		TUint traceSize = (data[BTrace::ESizeIndex]+3)&~3;
		if(traceSize>sizeRemaining)
			{
			error = KErrNoMemory;
			break;
			}

		TTraceConfigs* trace = new TTraceConfigs;
		ASSERT(trace);
		TTraceConfigsOperator::Init(*trace);
		if(!PreProcessTrace(*trace,data))
			{
			if (iMissingTraceFound)
				{
				// bad trace, create dummy 1 byte trace record...
				memset(trace,0,sizeof(*trace));
				trace->iGroupId = BTrace::EMetaTrace;
				trace->iSubCategory = KJunkTraceCategory;
				trace->iRawDataSize = 4;
					trace->iRawData[0] = *data;
				traceSize = 1;
				}
			else // The buffer was filled so ignore the rest of the data
				{
				break;
				}
			}
		data += traceSize;
		sizeRemaining -= traceSize;
		aLoggedTraces.Append(trace);
		}
	return error;
	}

TBool TTraceParser::PreProcessTrace(TTraceConfigs& aTrace, const TUint8* aData)
	{
	// process aTrace header...
	TUint traceSize = aData[BTrace::ESizeIndex];
	if(traceSize < 4u || traceSize > (TUint)KMaxBTraceRecordSize)
		return false; // bad size

	aTrace.iFlags = aData[BTrace::EFlagsIndex];
//	if(!TraceRecordId)						// first trace record...?
//		aTrace.iFlags &= ~BTrace::EMissingRecord;	// ignore missing traces before log start

	aTrace.iGroupId = aData[BTrace::ECategoryIndex];
	aTrace.iSubCategory = aData[BTrace::ESubCategoryIndex];

	const TUint8* header = aData + 4;
	
	if(aTrace.iFlags&BTrace::EHeader2Present)
		aTrace.iHeader2 = ReadTraceWord(header);

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
	if(dataSize > sizeof(aTrace.iRawData))
		return false; // bad trace record, its bigger than max allowed size

	if(dataSize < 8)
		return false; //bad trace, it's meant to contain both uid and format id
	
	//process module uid...
	aTrace.iComponentId = ReadTraceWord(aData);
	dataSize -= 4;
	//process format id...
	
	aTrace.iTraceId = ReadTraceWord(aData);
	dataSize -= 4;

	//process rest of payload data...
	if(dataSize > 0)
		{
		aTrace.iRawDataSize = dataSize;
		memcpy(&aTrace.iRawData, aData, dataSize);
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
