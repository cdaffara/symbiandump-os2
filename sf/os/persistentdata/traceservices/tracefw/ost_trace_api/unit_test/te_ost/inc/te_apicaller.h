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
// UTrace Unit Tests.
// Created: 03/04/2008
//



/**
 @file te_apicaller.h
 @internalTechnology
 @prototype
*/

#ifndef TE_APICALLER_H_
#define TE_APICALLER_H_

#include "te_tracerecord.h"
#include "te_apienums.h"


class TApiCaller
{
public:
	static TBool IsKernelTrace(TTraceApiUsed aApi);
	static TInt DoSendTraceL(TTraceConfigs& aTraceData);
	static TUint32 PcOfPreviousTraceCall();
	static TBool WouldBeLogged(TTraceConfigs& aTraceData);
};

#endif //TE_APICALLER_H_
