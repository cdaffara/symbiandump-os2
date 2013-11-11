// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef SQLITETRACEDEF_H
#define SQLITETRACEDEF_H

#include <e32def.h>
#include "SqliteUtil.h"

//////////////////////////////////////////////////////////////////////////////////////

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQLITE_OS_TRACE_ENABLED
    #define __SQLITETRACE_OSVAR(var) var
    #define __SQLITETRACE_OSEXPR(expr) expr
#else
    #define __SQLITETRACE_OSVAR(var)
    #define __SQLITETRACE_OSEXPR(expr)
#endif    

//////////////////////////////////////////////////////////////////////////////////////

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQLITE_RDEBUG_PRINT
    #undef OstTrace0 
    #undef OstTrace1 
    #undef OstTraceExt1 
    #undef OstTraceExt2 
    #undef OstTraceExt3 
    #undef OstTraceExt4 
    #undef OstTraceExt5

    void SqlitePrintf(TInt aGroupName, TInt aTraceName, const char* aFormat, ...);

    #define OstTrace0    SqlitePrintf 
    #define OstTrace1    SqlitePrintf
    #define OstTraceExt1 SqlitePrintf
    #define OstTraceExt2 SqlitePrintf
    #define OstTraceExt3 SqlitePrintf
    #define OstTraceExt4 SqlitePrintf
    #define OstTraceExt5 SqlitePrintf
#endif//defined OST_TRACE_COMPILER_IN_USE && defined _SQLITE_RDEBUG_PRINT

//////////////////////////////////////////////////////////////////////////////////////

#endif//SQLITETRACEDEF_H
