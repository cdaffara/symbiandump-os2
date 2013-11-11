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
 @file te_tracecontroldefaultconfig.h
 @internalTechnology
*/
#if (!defined TE_ULOGGERDEFAULTCONFIGSTEP_H)
#define TE_ULOGGERDEFAULTCONFIGSTEP_H
#include <test/datawrapper.h>
#include "te_ostv2integsuite_basewrapper.h"
#include <e32base.h>
#include <e32math.h>
#include <uloggerclient.h>
using namespace Ulogger; //CA:added so can use old ulogger api
class CDefaultTraceControlConfigWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CDefaultTraceControlConfigWrapper();
	static CDefaultTraceControlConfigWrapper* NewL();
	static CDefaultTraceControlConfigWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CDefaultTraceControlConfigWrapper();
	void ConstructL();

// Please add/modify your class members here:
private:
	};

_LIT(KDefaultTraceControlConfigWrapper,"DefaultTraceControlConfig");

#endif
