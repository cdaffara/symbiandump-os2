// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef SQLTRACEDEF_H
#define SQLTRACEDEF_H

#include <e32def.h>
#include "SqlAssert.h"

//////////////////////////////////////////////////////////////////////////////////////

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_BORDER_TRACE_ENABLED
    #define __SQLTRACE_BORDERVAR(var) var
    #define __SQLTRACE_BORDEREXPR(expr) expr
#else
    #define __SQLTRACE_BORDERVAR(var)
    #define __SQLTRACE_BORDEREXPR(expr)
#endif    

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_INTERNALS_TRACE_ENABLED
    #define __SQLTRACE_INTERNALSVAR(var) var
    #define __SQLTRACE_INTERNALSEXPR(expr) expr
#else
    #define __SQLTRACE_INTERNALSVAR(var)
    #define __SQLTRACE_INTERNALSEXPR(expr)
#endif

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_BUR_TRACE_ENABLED
    #define __SQLTRACE_BURVAR(var) var
    #define __SQLTRACE_BUREXPR(expr) expr
#else
    #define __SQLTRACE_BURVAR(var)
    #define __SQLTRACE_BUREXPR(expr)
#endif

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_COMPACT_TRACE_ENABLED
    #define __SQLTRACE_COMPACTVAR(var) var
    #define __SQLTRACE_COMPACTEXPR(expr) expr
#else
    #define __SQLTRACE_COMPACTVAR(var)
    #define __SQLTRACE_COMPACTEXPR(expr)
#endif

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_SESSION_TRACE_ENABLED
    #define __SQLTRACE_SESSIONVAR(var) var
    #define __SQLTRACE_SESSIONEXPR(expr) expr
#else
    #define __SQLTRACE_SESSIONVAR(var)
    #define __SQLTRACE_SESSIONEXPR(expr)
#endif

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_AUTHORIZER_TRACE_ENABLED
    #define __SQLTRACE_AUTHORIZERVAR(var) var
    #define __SQLTRACE_AUTHORIZEREXPR(expr) expr
#else
    #define __SQLTRACE_AUTHORIZERVAR(var)
    #define __SQLTRACE_AUTHORIZEREXPR(expr)
#endif

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_BLOB_TRACE_ENABLED
    #define __SQLTRACE_BLOBVAR(var) var
    #define __SQLTRACE_BLOBEXPR(expr) expr
#else
    #define __SQLTRACE_BLOBVAR(var)
    #define __SQLTRACE_BLOBEXPR(expr)
#endif

//////////////////////////////////////////////////////////////////////////////////////

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_RDEBUG_PRINT
    #undef OstTrace0 
    #undef OstTrace1 
    #undef OstTraceExt1 
    #undef OstTraceExt2 
    #undef OstTraceExt3 
    #undef OstTraceExt4 
    #undef OstTraceExt5

    void SqlPrintf(TInt aGroupName, TInt aTraceName, const char* aFormat, ...);

    #define OstTrace0    SqlPrintf 
    #define OstTrace1    SqlPrintf
    #define OstTraceExt1 SqlPrintf
    #define OstTraceExt2 SqlPrintf
    #define OstTraceExt3 SqlPrintf
    #define OstTraceExt4 SqlPrintf
    #define OstTraceExt5 SqlPrintf
#endif//defined OST_TRACE_COMPILER_IN_USE && defined _SQL_RDEBUG_PRINT

//////////////////////////////////////////////////////////////////////////////////////

#endif//SQLTRACEDEF_H
