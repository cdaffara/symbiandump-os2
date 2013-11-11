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
 @file te_tracecollectorclient.h
 @internalTechnology
*/
#if (!defined TE_ULOGGERAPITESTSTEP_H)
#define TE_ULOGGERAPITESTSTEP_H
#include <test/datawrapper.h>
#include "te_ostv2integsuite_basewrapper.h"
#include <e32base.h>
#include <e32math.h>
#include <uloggerclient.h>
#include "te_dataparameters.h"
#include "te_perftracetester.h"
using namespace Ulogger; //CA:added so can use old ulogger api


/*should put in a general filter test step class do derive the primary and secondary tests from*/
/*pass the filter values, have get/setfilter(0) as members*/
/*derive the relavent primary/secondary specifics*/

class CTCMGroupIDFilteringWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMGroupIDFilteringWrapper();
	static CTCMGroupIDFilteringWrapper* NewL();
	static CTCMGroupIDFilteringWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CTCMGroupIDFilteringWrapper();
	void ConstructL();
	};
	
class CTCMComponentIDFilteringWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMComponentIDFilteringWrapper();
	static CTCMComponentIDFilteringWrapper* NewL();
	static CTCMComponentIDFilteringWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CTCMComponentIDFilteringWrapper();
	void ConstructL();
	};
	
	
/*import UTrace code for this - sandwich e.g UTraceFunctionStep between ULooger start/stop*/
/* include more than one type of primary and secondary filter, ensure corresponding and unrelated tracing available from UTrace*/
class CTCMLoggingWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMLoggingWrapper();
	static CTCMLoggingWrapper* NewL();
	static CTCMLoggingWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CTCMLoggingWrapper();
	void ConstructL();
	TUptTraceTester iTestTracer;
	TApiRunConfig iApiRunConfig;
	};

class CTCMKernelLoggingWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMKernelLoggingWrapper();
	static CTCMKernelLoggingWrapper* NewL();
	static CTCMKernelLoggingWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CTCMKernelLoggingWrapper();
	void ConstructL();
	TUptTraceTester iTestTracer;
	TApiRunConfig iApiRunConfig;
	};

class CTCMPrintfLoggingWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMPrintfLoggingWrapper();
	static CTCMPrintfLoggingWrapper* NewL();
	static CTCMPrintfLoggingWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CTCMPrintfLoggingWrapper();
	void ConstructL();
	TUptTraceTester iTestTracer;
	TApiRunConfig iApiRunConfig;
	};

class CTCMKernelPrintfLoggingWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMKernelPrintfLoggingWrapper();
	static CTCMKernelPrintfLoggingWrapper* NewL();
	static CTCMKernelPrintfLoggingWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CTCMKernelPrintfLoggingWrapper();
	void ConstructL();
	TUptTraceTester iTestTracer;
	TApiRunConfig iApiRunConfig;
	};

_LIT(KTCMGroupIDFilteringWrapper,"TCMGroupIDFiltering");
_LIT(KTCMComponentIDFilteringWrapper,"TCMComponentIDFiltering");
_LIT(KTCMLoggingWrapper,"TCMLogging");
_LIT(KTCMKernelLoggingWrapper,"TCMKernelLogging");
_LIT(KTCMPrintfLoggingWrapper,"TCMPrintfLogging");
_LIT(KTCMKernelPrintfLoggingWrapper,"TCMKernelPrintfLogging");

#endif
