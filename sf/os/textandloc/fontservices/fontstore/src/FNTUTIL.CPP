/*
* Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "FNTSTD.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FNTUTILTraces.h"
#endif


GLDEF_C void Panic(TFntStorePanic aPanic)
	{
	_LIT(KFntStorePanicCategory,"FntStore");
	OstTrace1( TRACE_FATAL, _PANIC, "::Panic;aPanic=%x", ( TUint )&( aPanic ) );	
	User::Panic(KFntStorePanicCategory,aPanic);
	}


