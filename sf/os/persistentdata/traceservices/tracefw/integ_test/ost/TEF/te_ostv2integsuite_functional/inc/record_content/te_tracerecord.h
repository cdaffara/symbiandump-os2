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

#ifndef TE_TRACERECORD_H_
#define TE_TRACERECORD_H_

#include "te_dataparameters.h"
#include "te_ostv2integsuite_defs.h"
#include "te_instrumentation_enum.h"
#include <e32btrace.h> //for KMaxBTraceDataArray


/**
 * Because we need to transfer data across user and kernel side
 * and that needs to be able to do a shallow copy we need to keep it really simple.
 * Methods to fill the variables are provided by TTraceConfigsOperator
 */
class TTraceConfigs
{
public:
	//config stuff relevant to unit testing only
	TUPTApiUsed		iApiUsed;
	//variability
	TUint8				iLoggingEnabled;
	TUint8				iTracingBuiltIn;

	//Header things
	TGroupId		iGroupId;
	TComponentId			iComponentId;
	TComponentId			iUid3;	
	THasThreadIdentification		iHasThreadId;	
	THasProgramCounter	iHasProgramCounter;				
	TUint32				iPc;
	TUint32				iContextId;
	TTraceId			iTraceId;
	TUint64				iTimestamp;
	TUint32				iTimestamp2;	

	//payload related
	TUint32					iUint32[KMaxNumberOfParams];						
	char					iChar[KMaxSizeOfData];
	TBuf8<1030>	iDesC8;//KHugeSize8
	TInt8 					iUint8Big[KBigSize8];
	TInt8 					iUint8Huge[KHugeSize8];
	//TAny*					iAny;
	TUint32 				iRawData[(8+KHugeSize8)/2];//KMaxBTraceDataArray
	TInt					iRawDataSize;
#ifndef __KERNEL_MODE__
	TBuf16<2060> 	iDesC16;//KHugeSize16			
#endif
	TUint16 				iAny1;
	TTemplated4ByteAligned	iTemplated;
	//btrace format related as opposed to ost
	TUint8				iFlags;
	TUint32				iHeader2;
	TUint32				iExtra;
	TUint8				iSubCategory;
};

class TTraceHeaderInfo
{
public:

	//Header things
	TGroupId		iGroupId;
	TComponentId			iComponentId;	
	THasThreadIdentification		iHasThreadId;	
	THasProgramCounter	iHasProgramCounter;				
	TUint32				iPc;
	TUint32				iContextId;
	TTraceId			iTraceId;	
	//btrace format related as opposed to ost
	TUint8				iSubCategory;
};

class TTraceConfigsOperator
{
public:
	static void	Init(TTraceConfigs& aConfig);
	static void	Init(TTraceHeaderInfo& aHeaderInfo);
	static void	SetHeader(TTraceConfigs& aConfig, TGroupId aGroupId, TComponentId aComponentId, THasThreadIdentification aContextId, THasProgramCounter aPc, TTraceId aTraceId);
	static void	SetHeader(TTraceHeaderInfo& aHeaderInfo, TGroupId aGroupId, TComponentId aComponentId, THasThreadIdentification aContextId, THasProgramCounter aPc, TTraceId aTraceId);
	static void	SetOptions(TTraceConfigs& aConfig, TUPTApiUsed aApiUsed, TUint8 aLoggingEnabled, TUint8 aTracingBuiltIn);
	static void SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint16 aAny1, const TDesC8& aDesC8);
//	static void SetData(TTraceConfigs& aConfig, const char* aChar, TInt aCharSize, const TUint32* aRawData, TInt aDataSize);
	static void SetData(TTraceConfigs& aConfig, const char* aData, const TInt aSize);
	static void SetData(TTraceConfigs& aConfig, const TInt8* aData, const TInt aSize);
	static void SetData(TTraceConfigs& aConfig, const TAny* aData, const TInt aSize);
	static void SetData(TTraceConfigs& aConfig, const TDesC8& aData);
	static void SetData(TTraceConfigs& aConfig, const TDesC8& aData1, const TDesC8& aData2);
	static void SetData(TTraceConfigs& aConfig, const TDesC8& aData1, const TDesC8& aData2, const TUint32 aData3 );
	static void SetData(TTraceConfigs& aConfig, const TDesC16& aData);
	static void SetData(TTraceConfigs& aConfig, const TInt8 aData);
	static void SetData(TTraceConfigs& aConfig, const TUint32 aData);
	static void SetData(TTraceConfigs& aConfig, const TUint32 aData, const TUint32* aInstance);
	static void SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2);
	static void SetData(TTraceConfigs& aConfig, TUint32 aTraceName, const TDesC8& aData);
	static void SetData(TTraceConfigs& aConfig, TUint32 aTraceName, const TUint32* aData, const TUint32 aSize);
	static void SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3);
	static void SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3, const TUint32 aData4);
	static void SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3, const TUint32 aData4, const TUint32 aData5);
	static void SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3, const TUint32 aData4, const TUint32 aData5, const TUint32 aData6);
	static void SetData(TTraceConfigs& aConfig, const TUint32 aData1, const TUint32 aData2, const TUint32 aData3, const TUint32 aData4, const TUint32 aData5, const TUint32 aData6, const TUint32 aData7);
	static void SetData(TTraceConfigs& aConfig, const TTemplated4ByteAligned& aData);
};


#endif /*TE_TRACERECORD_H_*/
