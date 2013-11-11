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

#include <opensystemtrace.h>
#include <uloggerclient.h>
#include "te_multiparttrace.h"
#include "uloggerconfig.h"



//const data for testing purposes
const TTraceId KTestTraceId = 10245;
const TClassification KTestClassification = 254;
_LIT(KLogPath, "c:\\logs\\ulogger\\multitrace.utf");

//global variables
TInt fillPattern = 123;
TInt dataSize = 255;
TInt traceMethod = 0;
static const TComponentId KMyTestUid = 12345678;
TInt ordinal = 0;
TInt tarcerecords_expected = 0;


template<typename T>
T ReadTypeFromBuffer(const TUint8* aData, TInt& aOffset)
	{
	T p = *((T*)(aData+aOffset));
	aOffset += sizeof(T);
	return p;
	}


CCTe_multiparttrace::CCTe_multiparttrace()
	{
	SetTestStepName(KMultiTraceTestStep);
	}


CCTe_multiparttrace::~CCTe_multiparttrace()
	{
	}



TVerdict CCTe_multiparttrace::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("*** This test case is aimed to test multipart-traces ***"));

	//delete old trace log
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	fs.Delete(KLogPath);

	// added this code  to ensure directories created on hardware
	TInt err = fs.MkDirAll(KLogPath);
	if( err != KErrNone && err != KErrAlreadyExists)
		{
		ERR_PRINTF2(_L("RFs::MkDirAll failed with error %d"), err);
		}


	fs.Close();



	//enable ULogger/Trace to log test traces
	INFO_PRINTF1(_L("INFO: configuring ULogger..."));
	User::LeaveIfError( iUlogger.Connect() );
	CClearConfig configIni;
	configIni.ClearL(iUlogger);

	iUlogger.SetSecondaryFilteringEnabled(EFalse);

	_LIT8(KUloggerFilePlugin, "uloggerfileplugin");


	err = iUlogger.ActivateOutputPlugin(KUloggerFilePlugin);
	if( err != KErrNone && err != KErrAlreadyExists)
		{
		ERR_PRINTF2(_L("ActivateOutputPlugin failed with error %d"), err);
		User::LeaveIfError(err);
		}
	TPluginConfiguration pluginConf;
	pluginConf.SetKey(_L8("output_path"));
	pluginConf.SetValue(KLogPath);
	User::LeaveIfError( iUlogger.SetPluginConfigurations(KUloggerFilePlugin, pluginConf) );

	CArrayFixFlat<TUint8>* array = new (ELeave) CArrayFixFlat<TUint8>(4);
	CleanupStack::PushL(array);
	array->AppendL(KTestClassification);
	User::LeaveIfError( iUlogger.SetPrimaryFiltersEnabled(*array, ETrue));
	CleanupStack::PopAndDestroy(array);

	User::LeaveIfError( iUlogger.Restart());

	iUlogger.Close();

	return TestStepResult();
	}



TVerdict CCTe_multiparttrace::doTestStepL()
	{
	INFO_PRINTF1(_L("*** CCTe_multiparttrace::doTestStepL ***"));

	//generate trace data
	traceMethod = ETraceUsingTraceBig;
	TInt e = GenerateTraceDataL((CCTe_multiparttrace::TTraceMethod)traceMethod, dataSize, fillPattern);
	if(e != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: generating trace data failed (%d)"), e);
		SetTestStepResult(EFail);
		return TestStepResult();
		}


	//stop ulogger
	INFO_PRINTF1(_L("INFO: stopping ULogger..."));
	User::LeaveIfError( iUlogger.Connect() );
	User::LeaveIfError( iUlogger.Stop() );
	iUlogger.Close();

	return TestStepResult();
	}


TVerdict CCTe_multiparttrace::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("*** CCTe_multiparttrace::doTestStepPostambleL ***"));

	//validate the data that we just logged
	TInt e = KErrNone;
	if((e=CheckDataL((CCTe_multiparttrace::TTraceMethod)traceMethod, dataSize, fillPattern)) != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: checking data failed (%d)"), e);
		SetTestStepResult(EFail);
		}

	//delete log file
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    fs.Delete(KLogPath);
	
	return TestStepResult();
	}


TInt CCTe_multiparttrace::GenerateTraceDataL(TTraceMethod aTraceMethod, TUint aMaxDataSize, TUint8 aFillPattern)
	{
	INFO_PRINTF4(_L("*** CCTe_multiparttrace::GenerateTraceDataL(%d, %d, %d) ***"), aTraceMethod, aMaxDataSize, aFillPattern);
	TInt e = KErrNone;

	INFO_PRINTF1(_L("INFO: generating trace data..."));
	TInt i,k;
	//generate and send data using desired Ost API
	TUint8* data = (TUint8*)User::Alloc(aMaxDataSize);
	for(i=0; i<aMaxDataSize; ++i)
		{
		//fill data with pattern provided
		for(k=0; k<i; ++k)
			data[k] = i-k;

		//send trace
		TBool loggedFlag=EFalse;
		if(aTraceMethod == ETraceUsingTraceBig)
			{
			loggedFlag = OstTrace(TTraceContext(KMyTestUid, KTestClassification), KTestTraceId, (const TAny*)data, i);
			//loggedFlag = BTraceContextBig(KTestClassification, 0, KMyTestUid, (const TAny*)data, i);
			}


		if(!loggedFlag)
			{
			INFO_PRINTF2(_L("ERROR: trace %d not logged!"), i);
			e = KErrGeneral;
			break;
			}
		}

	if(e==KErrNone)
	    {
		//INFO_PRINTF1(_L("INFO: data sent"));
	    }
	else
		INFO_PRINTF2(_L("ERROR: data sendiung failed (%d)"), e);

	//cleanup
	delete [] data;

	return e;
	}


TInt CCTe_multiparttrace::CheckDataL(TTraceMethod aTraceMethod, TUint aMaxDataSize, TUint8 aFillPattern)
	{
	INFO_PRINTF4(_L("*** CCTe_multiparttrace::CheckDataL(%d, %d, %d) ***"), aTraceMethod, aMaxDataSize, aFillPattern);

	INFO_PRINTF2(_L("INFO: opening log file... %S"), &(KLogPath()));
	RFs fs;
	RFile file;
	User::LeaveIfError( fs.Connect() );

	TInt err = file.Open(fs, KLogPath, EFileRead);
	if(err != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: opening log file error: err = %d filename=%S"), err, &(KLogPath()));
		return err;
		}

	//read trace log file
	TInt size = 0;
	file.Size(size);
	if(size > 0)
		{
		INFO_PRINTF2(_L("INFO: Trace Log file size: %d"), size);
		HBufC8* data = HBufC8::NewLC(size);
		TPtr8 p(data->Des());
		User::LeaveIfError( file.Read(p) );
		const TUint8* dataPtr = p.Ptr();
		err = ValidateTraceDataL(dataPtr, p.Length(), dataSize, fillPattern);
		CleanupStack::PopAndDestroy(data);
		}
	else{
		err = KErrGeneral;
		}

	//cleanup
	file.Close();
	fs.Close();

	return err;
	}


TInt CCTe_multiparttrace::ValidateTraceDataL(const TUint8* aDataPtr, TUint aDataSize, TUint aTraceRecords, TUint8 /*aFillPattern*/)
	{
	INFO_PRINTF1(_L("*** CCTe_multiparttrace::ValidateTraceDataL ***"));

	TUint8* tracePtr = (TUint8*)aDataPtr;
	TInt traceSize = aDataSize;

	TUint8 recSize=0, flags=0, category=0; //subCategory=0;
	TInt32 flags2=0;//, contextId=0, extra=0, timestamp2=0, pc=0;
	TBool missing = EFalse;
	TInt offset = 0;
	//TInt currentTraceSize=0;
	const TUint8* endPtr = tracePtr+traceSize;
	//parse trace data
	TInt traceRecordCounter=0;
	TInt testRecordCounter=0;

	while(tracePtr < endPtr)
		{
		User::After(100); 
		//INFO_PRINTF1(_L(" "));
		traceRecordCounter++;
		offset = 0;
		recSize = flags = flags2 /*= extra*/ = category /*= subCategory = contextId */= 0;
		missing = EFalse;

		//read trace header
		recSize = tracePtr[BTrace::ESizeIndex];
		flags = tracePtr[BTrace::EFlagsIndex];
		category = tracePtr[BTrace::ECategoryIndex];
		//subCategory = tracePtr[BTrace::ESubCategoryIndex];
		offset += 4;

			//read header extension
		if(flags & BTrace::EHeader2Present)
			flags2 = ReadTypeFromBuffer<TInt32>(tracePtr, offset);
		if(flags & BTrace::ETimestampPresent)
			ReadTypeFromBuffer<TInt32>(tracePtr, offset);
		if(flags & BTrace::ETimestamp2Present)
			/*timestamp2 =*/ ReadTypeFromBuffer<TInt32>(tracePtr, offset);
		if(flags & BTrace::EContextIdPresent)
			/*contextId =*/ ReadTypeFromBuffer<TInt32>(tracePtr, offset);
		if(flags & BTrace::EPcPresent)
			{
			/*pc = */ ReadTypeFromBuffer<TInt32>(tracePtr, offset);
			//INFO_PRINTF1(_L("INFO: PC present"));
			}
		if(flags & BTrace::EExtraPresent)
			{
			//extra = ETrue;
			ReadTypeFromBuffer<TInt32>(tracePtr, offset);
			//INFO_PRINTF1(_L("INFO: EXTRA present"));
			}


		if(flags & BTrace::ECpuIdMask)
			{
			//INFO_PRINTF1(_L("INFO: CPUID present"));
			}


		if(flags & BTrace::ERecordTruncated)
			{
			//trauncated = ETrue;
			}
		if(flags & BTrace::EMissingRecord)
			missing = ETrue;

		if(missing)
			{
			INFO_PRINTF1(_L("ERROR: ***** !!! MISSING TRACES ERROR !!! *****"));
			return KErrCorrupt; //indicates corrupted trace data
			}

		TInt _payloadSize = recSize-offset;
		//INFO_PRINTF2(_L("INFO: payloadSize = %d"), _payloadSize);

		//parse payload
		if(category == KTestClassification)
			{
			if(flags2 & BTrace::EMultipartFlagMask)
				{
//				TInt32 d1 = *(TUint32*)(tracePtr+offset); //total trace size
//				TInt32 d2 = *(TUint32*)(tracePtr+offset+4); //offset in collected data or UID
//				TUint8 d3 = *(TUint8*)(tracePtr+offset+8); //UID or random val
//				TUint8 d4_01 = *(TUint8*)(tracePtr+offset+12); //TraceId
//				TUint8 d4_02 = *(TUint8*)(tracePtr+offset+13); //reserved
//				TUint8 d5 = *(TUint8*)(tracePtr+offset+14); //TraceId

				//INFO_PRINTF7(_L("INFO: trace data: %d, %d, %d, %d, %d, %d)"), d1, d2, d3, d4_01, d4_02, d5);
				//INFO_PRINTF2(_L("INFO: trace number: %d"), testRecordCounter);

				//check TMultipart flag (and not CPU ID)
				switch(flags2 & BTrace::EMultipartFlagMask)
					{
					case BTrace::EMultipartFirst:
						{
						//TInt32 d1 = *(TUint32*) (tracePtr+offset); //extra size (Total trace size)
						//TInt32 d2 = *(TUint32*) (tracePtr+offset+4); //UID
						//TInt16 d3_01 = *(TUint16*) (tracePtr+offset+8); //TraceId
						//TInt16 d3_02 = *(TUint16*) (tracePtr+offset+10); //reserved
						//TUint8 d4 = *(TUint8*) (tracePtr+offset+12); //payload 1
						//INFO_PRINTF6(_L("FIRST TRACE: %d, %d, %d, %d, %d"), d1, d2, d3_01, d3_02, d4);

						//+4 is to mask TraceId and reserved value (2+2)
						if(ValidateTracePayloadL((const TUint8*)tracePtr+offset, _payloadSize, testRecordCounter, ETrue, testRecordCounter, ETrue, EFalse) != KErrNone)
							return KErrGeneral;
						} break;

					case BTrace::EMultipartMiddle:
						{
						TInt32 d1 = *(TUint32*)(tracePtr+offset); //extra size (Total trace size)
						TInt32 d2 = *(TUint32*)(tracePtr+offset+4); //current offset
						//TUint8 d3 = *(TUint8*)(tracePtr+offset+8); //payload 1
						//INFO_PRINTF4(_L("MIDDLE TRACE: %d, %d, %d"), d1, d2, d3);
						
						if(ValidateTracePayloadL((const TUint8*)tracePtr+offset, _payloadSize, testRecordCounter, ETrue, d1-d2, EFalse, EFalse) != KErrNone)
							return KErrGeneral;
						} break;

					case BTrace::EMultipartLast:
						{
						TInt32 d1 = *(TUint32*)(tracePtr+offset); //extra size (Total trace size)
						TInt32 d2 = *(TUint32*)(tracePtr+offset+4); //current offset
						//TUint8 d3 = *(TUint8*)(tracePtr+offset+8); //payload 1
						//INFO_PRINTF4(_L("LAST TRACE: %d, %d, %d"), d1, d2, d3);
						
						if(ValidateTracePayloadL((const TUint8*)tracePtr+offset, _payloadSize, testRecordCounter, ETrue, d1-d2, EFalse, ETrue) != KErrNone)
							return KErrGeneral;

						testRecordCounter++;
						} break;

					default:
						{
						INFO_PRINTF1(_L("ERROR: wrong value of flags2"));
						return KErrGeneral;
						}//break;
					}


				}
			else //not multiparted trace
				{
				if(ValidateTracePayloadL((const TUint8*)tracePtr+offset, _payloadSize, testRecordCounter, EFalse, testRecordCounter, ETrue, ETrue) != KErrNone)
					return KErrGeneral;
				testRecordCounter++;
				}
			}

		tracePtr = BTrace::NextRecord((TAny*)tracePtr);
		}

	INFO_PRINTF2(_L("INFO: %d trace records parsed"), traceRecordCounter);
	INFO_PRINTF2(_L("INFO: %d test trace records parsed"), testRecordCounter);
	if(testRecordCounter != aTraceRecords)
		{
		INFO_PRINTF3(_L("ERROR: wrong number of trace records with desired category (%d vs %d)"), testRecordCounter, aTraceRecords);
		return KErrGeneral;
		}

	return KErrNone;
	}


TInt CCTe_multiparttrace::ValidateTracePayloadL(const TUint8* aDataPtr, TUint aDataSize, TInt aTotalTraceSize, TBool aMultiPart, TUint8 aExpectedFirstVal, TBool aFirstTrace, TBool aLastTrace)
	{
	TInt offset = 0;

	//check some metadata that is stored in payload
	if(aFirstTrace)
		{
		ordinal = 0;

		if(aMultiPart)
			{
			TUint totalTraceSize = (TUint32) *(TUint32*)((TUint8*)aDataPtr+offset);
			offset+=4;
			if(totalTraceSize != aExpectedFirstVal)
				{
				INFO_PRINTF3(_L("ERROR: wrong totalTraceSize value (%d vs %d)"), totalTraceSize, aExpectedFirstVal);
				return KErrGeneral;
				}
			}

		TUint32 uid = (TUint32) *(TUint32*)((TUint8*)aDataPtr+offset);
		offset += 4;
		if(uid != KMyTestUid)
			{
			INFO_PRINTF3(_L("ERROR: wrong uid value (%d vs %d)"), uid, KMyTestUid);
			return KErrGeneral;
			}


		//read and test Ost TraceId
		TUint16 traceId = (TUint16) *(TUint16*)((TUint8*)aDataPtr+offset);
		if(traceId != KTestTraceId)
			{
			INFO_PRINTF3(_L("ERROR: wrong traceId value (%d vs %d)"), traceId, KTestTraceId);
			return KErrGeneral;
			}
		offset += 2;

		//read reserved value (not used by Ost at the moment)
		//TUint16 unused = (TUint16) *(TUint16*)((TUint8*)aDataPtr+offset);
		offset += 2;
		}
	else if(aMultiPart)
		{
		//non-first multipart trace
		TUint totalTraceSize = (TUint32) *(TUint32*)((TUint8*)aDataPtr+offset);
		offset+=4;

		if(totalTraceSize != aTotalTraceSize)
			{
			INFO_PRINTF3(_L("ERROR: wrong totalTraceSize (in MIDDLE or LAST multipart trace) value (%d vs %d)"), totalTraceSize, aTotalTraceSize);
			return KErrGeneral;
			}

		//TUint32 tracerDataOffset = tracerDataOffset = (TUint32) *(TUint32*)((TUint8*)aDataPtr+offset);
		offset+=4;

		}

	//validate content of the trace data
	TInt expectedVal = aExpectedFirstVal;
	TBool reallyFirstRecord = aFirstTrace;
	while (offset < aDataSize)
		{
		TUint8 d = *((TUint8*)aDataPtr+offset);
		++ordinal;

		if(reallyFirstRecord)
			{
			tarcerecords_expected = d;
			reallyFirstRecord = EFalse;
			}

		if (d != expectedVal)
			{
			INFO_PRINTF3(_L("ERROR: wrong data value (%d vs %d)"), d, expectedVal);
			return KErrGeneral;
			}
		++offset;
		--expectedVal;
		}

	if(offset != aDataSize)
		{
		INFO_PRINTF3(_L("ERROR: wrong payload size (%d vs %d)"), offset, aDataSize);
		return KErrGeneral;
		}

	if(aLastTrace && tarcerecords_expected != ordinal)
		{
		INFO_PRINTF3(_L("ERROR: wrong number of traces (%d vs %d)"), tarcerecords_expected, ordinal);
		return KErrGeneral;
		}

	//INFO_PRINTF1(_L("INFO: trace data checked"));
	return KErrNone;
	}

