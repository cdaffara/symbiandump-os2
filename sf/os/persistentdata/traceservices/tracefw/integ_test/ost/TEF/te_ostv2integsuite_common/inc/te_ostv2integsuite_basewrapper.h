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
 @file te_ostv2integsuite_basewrapper.h
 @internalTechnology
*/

#if (!defined TE_ULOGGER_MCL_STEP_BASE)
#define TE_ULOGGER_MCL_STEP_BASE
#include <test/datawrapper.h>
#include <hal.h>
#include <opensystemtrace.h>

#include "te_tracerecord.h"
#include "te_dataparameters.h"
#include "te_parser.h"
#include "te_instrumentation_enum.h"


class CTe_MCLSuiteBaseWrapper : public CDataWrapper
	{
public:
	virtual ~CTe_MCLSuiteBaseWrapper();
	static CTe_MCLSuiteBaseWrapper* NewL();
	static CTe_MCLSuiteBaseWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);

protected:
	CTe_MCLSuiteBaseWrapper();
	void ConstructL();
	
	TInt CompareTraces();
	TInt CompareTraceHeaders();
	TInt StoreTraceInfo(TBool aEnumBased,TUPTApiUsed anApi,TComponentId aComponentId, TGroupId aGroupId,THasThreadIdentification aHasContext, THasProgramCounter aHasPc);
	TInt StoreTraceHeaderInfo(TComponentId aComponentId, TGroupId aGroupId,THasThreadIdentification aHasContext, THasProgramCounter aHasPc);
	TInt ReadFromLogFile(const TDesC& aFileName);
	TInt ReadHeaderFromLogFile(const TDesC& aFileName);
	TBool CreateTraceObject(TUPTApiUsed aApi, THasThreadIdentification aHasContext, THasProgramCounter aHasPc,RPointerArray<TTraceConfigs> & aTraceObjects);
	TBool CreateTraceObject(TComponentId aComponentId, TGroupId aGroupId,THasThreadIdentification aHasContext, THasProgramCounter aHasPc,RPointerArray<TTraceConfigs> & aTraceObjects);
	void CreateTraceHeaderObject(TComponentId aModuleUid, TGroupId aGroupId,THasThreadIdentification aHasContext, THasProgramCounter aHasPc,TTraceHeaderInfo*& aHeaderInfo);
	TTraceParser	iParser;
	TTraceHeaderParser	iHeaderParser;
	TInt GetDataL(TUint8*& aData, TUint& aSize,const TDesC& aFileName);
	TInt GetDataL(TUint8*& aData, TUint& aBytesRead, TUint aPosition, const TDesC& aFileName);
	TInt EmptyFile(const TDesC& aFileName);

protected:
	TInt iAlignPosition;
	TInt iNumMatchingTraces;
	TInt iPosition;
	TTraceConfigs	iTraceData;
	RPointerArray<TTraceConfigs> iLoggedTraces;
	RPointerArray<TTraceConfigs> iSentTraces;
	CArrayPtrSeg<TTraceHeaderInfo> *iSentTraceHeaders;
	CArrayPtrSeg<TTraceHeaderInfo> *iLoggedTraceHeaders;


protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	};

#endif
