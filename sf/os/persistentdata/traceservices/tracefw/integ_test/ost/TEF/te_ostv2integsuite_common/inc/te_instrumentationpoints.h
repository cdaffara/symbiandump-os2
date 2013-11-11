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
// Ost Performance Tests Ost Call Controller
//



/**
 @file te_instrumentationpoints.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTUTRACECALLS_H
#define TE_UPTUTRACECALLS_H


#include <opensystemtrace.h>
#include "te_instrumentation_enum.h"
#include "te_apirunconfig.h" 	//for TApiRunConfig

/**
 * This file contains all UTrace APIs that are used from all test steps.
 * They are configured and parameterized in order to test content of data, boundaries and in different areas.
 */
class TUptTraceCalls
	{
public:
	static TBool IsKernelTrace(TInt aApiId);
	static TInt DoSendTraceL(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults);
	};

#endif


