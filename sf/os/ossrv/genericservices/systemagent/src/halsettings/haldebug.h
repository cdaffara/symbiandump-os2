// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HALDEBUG_H
#define HALDEBUG_H

#include <e32debug.h>

// Make sure release builds get a warning if 
#ifndef _DEBUG
#if (defined HALSETTINGS_TRACE)
#warning "Use of RDebug::Print tracing in a release build, check MMP files"
#endif
#endif


//
// MACROs for trace statements in client/server code.
//

#ifdef HALSETTINGS_TRACE

#define __HS_TRACE(_text)		RDebug::Print(_L(_text))
#define __HS_TRACE1(_text, _a1)	RDebug::Print(_L(_text), (_a1))
#define __HS_TRACE2(_text, _a1, _a2)	RDebug::Print(_L(_text), (_a1), (_a2))
#define __HS_TRACE3(_text, _a1, _a2, _a3)	RDebug::Print(_L(_text), (_a1), (_a2), (_a3))
#define __HS_TRACE4(_text, _a1, _a2, _a3, _a4)	RDebug::Print(_L(_text), (_a1), (_a2), (_a3), (_a4))
#define __HS_TRACE5(_text, _a1, _a2, _a3, _a4, _a5)	RDebug::Print(_L(_text), (_a1), (_a2), (_a3), (_a4), (_a5))
#define __HS_TRACE6(_text, _a1, _a2, _a3, _a4, _a5, _a6)	RDebug::Print(_L(_text), (_a1), (_a2), (_a3), (_a4), (_a5), (_a6))

#else

#define __HS_TRACE(_text)			
#define __HS_TRACE1(_text, _a1)		
#define __HS_TRACE2(_text, _a1, _a2)
#define __HS_TRACE3(_text, _a1, _a2, _a3)	
#define __HS_TRACE4(_text, _a1, _a2, _a3, _a4)
#define __HS_TRACE5(_text, _a1, _a2, _a3, _a4, _a5)
#define __HS_TRACE6(_text, _a1, _a2, _a3, _a4, _a5, _a6)

#endif



#endif // HALDEBUG_H

