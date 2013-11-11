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
// This file define all the common values thoughout your test project
//



/**
 @file te_suite_defs.h
 @internalTechnology
*/
#ifndef TE_SUITE_DEFS
#define TE_SUITE_DEFS

_LIT(KTracingBuiltIntoSystem, "TracingBuiltIntoSystem");

#include <uloggerclient.h>

using namespace Ulogger;

// For test step panics
//_LIT(KTe_UTraceSuitePanic,"Te_UTraceSuite");
#ifdef TEST_DISABLED
_LIT(KTe_UTraceSuitePanic,"te_utrace_disabled");
#elif TEST_ENABLED
_LIT(KTe_UTraceSuitePanic,"te_utrace_enabled");
#endif



#endif
