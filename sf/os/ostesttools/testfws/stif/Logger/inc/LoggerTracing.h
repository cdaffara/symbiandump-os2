/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains the tracing operations for the 
* CStifLogger and COutput.
*
*/

#ifndef LOGGERTRACING_H
#define LOGGERTRACING_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TUint KError           = 0x1;
const TUint KInfo            = 0x2;
const TUint KAlways          = 0xFFFFFFFF;
const TUint KDebugLevel      = ( KError );

// MACROS
#ifdef _DEBUG
#define __TRACING_ENABLED
#endif

#ifdef __TRACING_ENABLED
// Disable conditional expression is constant -warning
#pragma warning( disable : 4127 )
#define __TRACE(level,p) if ( (level) & KDebugLevel) { RDebug::Print p ;}
#else
#define __TRACE(level,p);
#endif

#endif // LOGGERTRACING_H

// End of File
