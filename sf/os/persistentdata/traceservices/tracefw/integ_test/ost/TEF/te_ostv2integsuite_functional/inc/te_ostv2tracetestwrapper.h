// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_tracecontroldefaultconfig.h
 @internalTechnology
*/
#if (!defined TE_OSTV2TRACETESTWRAPPER_H)
#define TE_OSTV2TRACETESTWRAPPER_H
#include <test/datawrapper.h>
#include "te_ostv2integsuite_basewrapper.h"
#include <e32base.h>
#include <e32math.h>
#include <uloggerclient.h>
using namespace Ulogger; 
class CTOstv2TraceTestWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTOstv2TraceTestWrapper();
	static CTOstv2TraceTestWrapper* NewL();
	static CTOstv2TraceTestWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CTOstv2TraceTestWrapper();
	void ConstructL();	
	void AddTraceHelper(const TGroupId aGroupId, const TTraceId aTraceId, TUint32 aValue);

private:
	TInt CheckResults(const TDesC& aLogFilename);
	void TestDuplicatesL();
	void TestOstDebugOnlyWrapL();
	void TestOstCompileTimeMacroL();
	TInt OpenUlogger(RULogger& aLogger);
	};

_LIT(KOstTraceTestWrapper, "OstTraceTestWrapper");

#endif // TE_OSTV2TRACETESTWRAPPER_H
