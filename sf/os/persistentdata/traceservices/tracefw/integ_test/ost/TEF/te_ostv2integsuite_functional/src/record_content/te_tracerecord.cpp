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
 @file te_trace_data.cpp
 @internalTechnology
*/
#include "te_tracerecord.h"


void TTraceConfigsOperator::Init(TTraceConfigs& aConfig)
	{
	SetHeader(aConfig, 0, 0, ENoThreadIdentification, ENoProgramCounter, 0);
	SetOptions(aConfig, UPTNone, 0, 0);
	aConfig.iFlags = 0;
	aConfig.iHeader2 = 0;
	aConfig.iTimestamp = 0;
	aConfig.iTimestamp2 = 0;
	aConfig.iPc = 0;
	aConfig.iExtra = 0;
	//memcpy(&aConfig.iUint32, 0, sizeof(aConfig.iUint32));
	memset(&aConfig.iUint32, 0, sizeof(aConfig.iUint32));
	memset(&aConfig.iChar, 0, sizeof(aConfig.iChar));
	//memset(&aConfig.iAny, 0, sizeof(aConfig.iChar));
	memset(&aConfig.iUint8Big, 0, sizeof(aConfig.iUint8Big));
	memset(&aConfig.iUint8Huge, 0, sizeof(aConfig.iUint8Huge));
#ifndef __KERNEL_MODE__
	aConfig.iDesC16 = KDefault16;
#endif
	aConfig.iDesC8 = KDefault8;
	aConfig.iAny1 = 0;
	memset(&aConfig.iRawData, 0, sizeof(aConfig.iRawData));
	aConfig.iContextId = 0;
	aConfig.iSubCategory = 0;
	aConfig.iRawDataSize = 0;
	aConfig.iUid3 = KComponentId;
	aConfig.iTemplated.iData1 = 0;
	aConfig.iTemplated.iData2 = 0;
	aConfig.iTemplated.iData3 = 0;
	aConfig.iTemplated.iData4 = 0;
	aConfig.iTemplated.iData5 = 0;
	}

void TTraceConfigsOperator::Init(TTraceHeaderInfo& aHeaderInfo)
	{
	SetHeader(aHeaderInfo, 0, 0, ENoThreadIdentification, ENoProgramCounter, 0);
	aHeaderInfo.iPc = 0;
	aHeaderInfo.iContextId = 0;
	aHeaderInfo.iSubCategory = 0;
	}

void TTraceConfigsOperator::SetHeader(TTraceConfigs& aConfig, TGroupId aGroupId, TComponentId aComponentId, THasThreadIdentification aContextId, THasProgramCounter aHasProgramCounter, TTraceId aTraceId)
	{
	aConfig.iGroupId = aGroupId;
	aConfig.iComponentId = aComponentId; 
	aConfig.iHasThreadId = aContextId; 
	aConfig.iHasProgramCounter = aHasProgramCounter;
	aConfig.iTraceId = aTraceId;
	}

void TTraceConfigsOperator::SetHeader(TTraceHeaderInfo& aHeaderInfo, TGroupId aGroupId, TComponentId aComponentId, THasThreadIdentification aContextId, THasProgramCounter aPc, TTraceId aTraceId)
	{
	aHeaderInfo.iGroupId = aGroupId; 
	aHeaderInfo.iComponentId = aComponentId; 
	aHeaderInfo.iHasThreadId = aContextId; 
	aHeaderInfo.iHasProgramCounter = aPc;
	aHeaderInfo.iTraceId = aTraceId;
	}
void TTraceConfigsOperator::SetOptions(TTraceConfigs& aConfig, TUPTApiUsed aApiUsed, TUint8 aLoggingEnabled, TUint8 aTracingBuiltIn)
	{
	aConfig.iApiUsed = aApiUsed;
	aConfig.iLoggingEnabled = aLoggingEnabled;
	aConfig.iTracingBuiltIn = aTracingBuiltIn;
	}

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint16 aAny1, const TDesC8& aDesC8)
	{
	aConfig.iUint32[0] = aData1;
	aConfig.iUint32[1] = aData2;
	aConfig.iDesC8 = aDesC8;
	aConfig.iAny1 = aAny1;	
	}
/*
void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const char* aChar, TInt aCharSize, const TUint32* aRawData, TInt aDataSize)
	{
	if((aChar == 0) || (aRawData == 0))
		return;
	if(aCharSize < KMaxSizeOfData)
		{
		for(TInt i = aCharSize; i > 0; i--)
			{
  			aConfig.iChar[i] = aChar[i]; //what about access outside if aChar length?
			}
  		} 
  	if(aDataSize < (8 + KMaxBTraceDataArray) / 4)
  		{
  		for(TInt i = aDataSize; i > 0; i--)
  			aConfig.iRawData[i] = aRawData[i];
  		aConfig.iRawDataSize = aDataSize;
  		}
	}
	*/
void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TInt8* aData, const TInt aSize)
	{
	if(aSize <= sizeof(aConfig.iRawData))
		{
		memcpy(&aConfig.iRawData, aData, aSize);
		aConfig.iRawDataSize = aSize;
		}
	if (aSize <= sizeof(aConfig.iUint8Big))
		memcpy(&aConfig.iUint8Big, aData, aSize);
	else
		{
		if(aSize <= sizeof(aConfig.iUint8Huge))
			memcpy(&aConfig.iUint8Huge, aData, aSize);
		}
	}

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TAny* aData, const TInt aSize)
	{
	if(aSize <= sizeof(aConfig.iRawData))
		{
		memcpy(&aConfig.iRawData, aData, aSize);
		aConfig.iRawDataSize = aSize;
		}
	//if(aSize <= sizeof(aConfig.iChar))
		//memcpy(&aConfig.iAny, aData, aSize);
	}

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, TUint32 aTraceName, const TUint32* aData, const TUint32 aSize)
    {
       aConfig.iRawData[0] = aTraceName;
       aConfig.iRawDataSize = 4;
    if(aSize+4<= sizeof(aConfig.iRawData))
        {
        memcpy(aConfig.iRawData+1, aData, aSize);
        aConfig.iRawDataSize = 4+aSize;
        }
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const char* aData, const TInt aSize)
	{
	if(aSize <= sizeof(aConfig.iRawData))
		{
		memcpy(&aConfig.iRawData, aData, aSize);
		aConfig.iRawDataSize = aSize;
		}
	if(aSize <= sizeof(aConfig.iChar))
		memcpy(&aConfig.iChar, aData, aSize);
	}

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TDesC8& aData)
	{
	if(aData.Size() <= sizeof(aConfig.iRawData))
		{
		memcpy(&aConfig.iRawData, aData.Ptr(), aData.Size());
		aConfig.iRawDataSize = aData.Size();
		}
	aConfig.iDesC8.Copy(aData);
	}

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TDesC8& aData1, const TDesC8& aData2)
    {
    if((aData1.Size()+ aData2.Size()+ 8) <= sizeof(aConfig.iRawData))
        {
        TUint32* ptr;
        memcpy(&aConfig.iRawData, &aData1, aData1.Size()+4);
        ptr=aConfig.iRawData+((((aData1.Size()+4)+3)&~3)/4);
        memcpy(ptr, &aData2, aData2.Size()+4);
        aConfig.iRawDataSize = aData1.Size()+ aData2.Size()+ 8;
        }
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TDesC8& aData1, const TDesC8& aData2, const TUint32 aData3 )
    {
    if((aData1.Size()+ aData2.Size()+ 8 + 4) <= sizeof(aConfig.iRawData))
        {
        TUint32* ptr;
        memcpy(&aConfig.iRawData, &aData1, aData1.Size()+4);
        ptr=aConfig.iRawData+((((aData1.Size()+4)+3)&~3)/4);
        memcpy(ptr, &aData2, aData2.Size()+4);
        ptr=ptr+((((aData2.Size()+4)+3)&~3)/4);
        memcpy(ptr, &aData3, 4);
        aConfig.iRawDataSize = aData1.Size()+ aData2.Size()+ 8 + 4;
        }
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, TUint32 aTraceName, const TDesC8& aData)
    {
    aConfig.iRawData[0] = aTraceName;
    aConfig.iRawDataSize = 4;
    if(aData.Size()+4 <= sizeof(aConfig.iRawData))
        {
        memcpy(aConfig.iRawData+1, aData.Ptr(), aData.Size());
        aConfig.iRawDataSize = aData.Size()+4;
        }
    aConfig.iDesC8.Copy(aData);
    }

#ifndef __KERNEL_MODE__
void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TDesC16& aData)
	{
	if(aData.Size() <= sizeof(aConfig.iRawData))
		{
		memcpy(&aConfig.iRawData, aData.Ptr(), aData.Size());
		aConfig.iRawDataSize = aData.Size();
		}
	aConfig.iDesC16.Copy(aData);
	}
#else// __KERNEL_MODE__
void TTraceConfigsOperator::SetData(TTraceConfigs&, const TDesC16&)
	{}
#endif

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TUint32 aData)
	{
	if(sizeof(aData) <= sizeof(aConfig.iRawData))
		{
		aConfig.iRawData[0] = aData;
		aConfig.iRawDataSize = sizeof(aData);
		}
	aConfig.iUint32[0] = aData;
	}

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TUint32 aData, const TUint32* aInstance)
    {
    if(sizeof(aData)+4 <= sizeof(aConfig.iRawData))
        {
        aConfig.iRawData[0] = aData;
        aConfig.iRawData[1] = (TUint32) aInstance;
        aConfig.iRawDataSize = sizeof(aData)+4;        
        }
    aConfig.iUint32[0] = aData;
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TInt8 aData)
    {
    if(sizeof(aData) <= sizeof(aConfig.iRawData))
        {
        aConfig.iRawData[0] = aData;
        aConfig.iRawDataSize = sizeof(aData);
        }
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2)
	{
	if(sizeof(aConfig.iRawData) >= 8) //size of 
		{
		aConfig.iRawData[0] = aData1;
		aConfig.iRawData[1] = aData2;
		aConfig.iRawDataSize = 8;
		}
	aConfig.iUint32[0] = aData1;
	aConfig.iUint32[1] = aData2;
	}

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3)
    {
    if(sizeof(aConfig.iRawData) >= 12) //size of 
        {
        aConfig.iRawData[0] = aData1;
        aConfig.iRawData[1] = aData2;
        aConfig.iRawData[2] = aData3;
        aConfig.iRawDataSize = 12;
        }
    aConfig.iUint32[0] = aData1;
    aConfig.iUint32[1] = aData2;
    aConfig.iUint32[2] = aData3;
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3, const TUint32 aData4)
    {
    if(sizeof(aConfig.iRawData) >= 16) //size of 
        {
        aConfig.iRawData[0] = aData1;
        aConfig.iRawData[1] = aData2;
        aConfig.iRawData[2] = aData3;
        aConfig.iRawData[3] = aData4;
        aConfig.iRawDataSize = 16;
        }
    aConfig.iUint32[0] = aData1;
    aConfig.iUint32[1] = aData2;
    aConfig.iUint32[2] = aData3;
    aConfig.iUint32[3] = aData4;
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3, const TUint32 aData4, const TUint32 aData5)
    {
    if(sizeof(aConfig.iRawData) >= 20) //size of 
        {
        aConfig.iRawData[0] = aData1;
        aConfig.iRawData[1] = aData2;
        aConfig.iRawData[2] = aData3;
        aConfig.iRawData[3] = aData4;
        aConfig.iRawData[4] = aData5;
        aConfig.iRawDataSize = 20;
        }
    aConfig.iUint32[0] = aData1;
    aConfig.iUint32[1] = aData2;
    aConfig.iUint32[2] = aData3;
    aConfig.iUint32[3] = aData4;
    aConfig.iUint32[4] = aData5;
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3, const TUint32 aData4, const TUint32 aData5, const TUint32 aData6)
    {
    if(sizeof(aConfig.iRawData) >= 24) //size of 
        {
        aConfig.iRawData[0] = aData1;
        aConfig.iRawData[1] = aData2;
        aConfig.iRawData[2] = aData3;
        aConfig.iRawData[3] = aData4;
        aConfig.iRawData[4] = aData5;
        aConfig.iRawData[5] = aData6;
        aConfig.iRawDataSize = 24;
        }
    aConfig.iUint32[0] = aData1;
    aConfig.iUint32[1] = aData2;
    aConfig.iUint32[2] = aData3;
    aConfig.iUint32[3] = aData4;
    aConfig.iUint32[4] = aData5;
    aConfig.iUint32[5] = aData6;
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3, const TUint32 aData4, const TUint32 aData5, const TUint32 aData6, const TUint32 aData7)
    {
    if(sizeof(aConfig.iRawData) >= 28) //size of 
        {
        aConfig.iRawData[0] = aData1;
        aConfig.iRawData[1] = aData2;
        aConfig.iRawData[2] = aData3;
        aConfig.iRawData[3] = aData4;
        aConfig.iRawData[4] = aData5;
        aConfig.iRawData[5] = aData6;
        aConfig.iRawData[6] = aData7;
        aConfig.iRawDataSize = 28;
        }
    aConfig.iUint32[0] = aData1;
    aConfig.iUint32[1] = aData2;
    aConfig.iUint32[2] = aData3;
    aConfig.iUint32[3] = aData4;
    aConfig.iUint32[4] = aData5;
    aConfig.iUint32[5] = aData6;
    aConfig.iUint32[6] = aData7;
    }

void TTraceConfigsOperator::SetData(TTraceConfigs& aConfig, const TTemplated4ByteAligned& aData)
	{
	if(sizeof(aData) <= sizeof(aConfig.iRawData))
		{
		memcpy(&aConfig.iRawData, &aData, sizeof(aData));
		aConfig.iRawDataSize = sizeof(aData);
		}
	aConfig.iTemplated = aData;
	}
