// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ECOM_DEBUG_H
#define ECOM_DEBUG_H

#include <e32debug.h>
#include <ecom/ecompanics.h>

// Make sure release builds get a warning if 
#ifndef _DEBUG
#if (defined ECOM_TRACE)
#warning "Use of RDebug::Print tracing in a release build, check MMP files"
#endif
#endif

// Panic category string for client DLL panics, see ecompanic.h
_LIT(KEComClientDLLPanicCategory, "ECOMClientDLL");

// Panic category string for server panics, see ecompanic.h
_LIT(KEComServerPanicCategory, "ECOMServer");

//
// MACROs for log statements in client/server code.
//

#define __ECOM_LOG(_text)		RDebug::Print(_L(_text))
#define __ECOM_LOG1(_text, _a1)	RDebug::Print(_L(_text), (_a1))

//
// MACROs for trace statements in client/server code.
//

#ifdef ECOM_TRACE

#define __ECOM_TRACE(_text)		RDebug::Print(_L(_text))
#define __ECOM_TRACE1(_text, _a1)	RDebug::Print(_L(_text), (_a1))
#define __ECOM_TRACE2(_text, _a1, _a2)	RDebug::Print(_L(_text), (_a1), (_a2))
#define __ECOM_TRACE3(_text, _a1, _a2, _a3)	RDebug::Print(_L(_text), (_a1), (_a2), (_a3))
#define __ECOM_TRACE4(_text, _a1, _a2, _a3, _a4)	RDebug::Print(_L(_text), (_a1), (_a2), (_a3), (_a4))
#define __ECOM_TRACE5(_text, _a1, _a2, _a3, _a4, _a5)	RDebug::Print(_L(_text), (_a1), (_a2), (_a3), (_a4), (_a5))
#define __ECOM_TRACE6(_text, _a1, _a2, _a3, _a4, _a5, _a6)	RDebug::Print(_L(_text), (_a1), (_a2), (_a3), (_a4), (_a5), (_a6))

#else

#define __ECOM_TRACE(_text)			
#define __ECOM_TRACE1(_text, _a1)		
#define __ECOM_TRACE2(_text, _a1, _a2)
#define __ECOM_TRACE3(_text, _a1, _a2, _a3)	
#define __ECOM_TRACE4(_text, _a1, _a2, _a3, _a4)
#define __ECOM_TRACE5(_text, _a1, _a2, _a3, _a4, _a5)
#define __ECOM_TRACE6(_text, _a1, _a2, _a3, _a4, _a5, _a6)

#endif




#endif // ECOM_DEBUG_H

