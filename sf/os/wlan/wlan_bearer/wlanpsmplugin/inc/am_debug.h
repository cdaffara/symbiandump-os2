/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Debug utilities
*
*/

/*
* %version: 3 %
*/

#ifndef AM_DEBUG_H
#define AM_DEBUG_H

#include <e32svr.h>

#ifdef _DEBUG

/**
* Class for producing debug traces
* @lib
* @since Series 60 5.0
*/

#define DEBUG(a)		            RDebug::Print(_L(a))
#define DEBUG1(a,b)		            RDebug::Print(_L(a),b)
#define DEBUG2(a,b,c)	            RDebug::Print(_L(a),b,c)
#define DEBUG3(a,b,c,d)	            RDebug::Print(_L(a),b,c,d)
#define DEBUG4(a,b,c,d,e)	        RDebug::Print(_L(a),b,c,d,e)
#define DEBUG5(a,b,c,d,e,f)	        RDebug::Print(_L(a),b,c,d,e,f)
#define DEBUG6(a,b,c,d,e,f,g)	    RDebug::Print(_L(a),b,c,d,e,f,g)
#define DEBUG7(a,b,c,d,e,f,g,h) 	RDebug::Print(_L(a),b,c,d,e,f,g,h)
#define DEBUG8(a,b,c,d,e,f,g,h,i)	RDebug::Print(_L(a),b,c,d,e,f,g,h,i)

#else // _DEBUG

#define DEBUG(a)                    /* _DEBUG is not defined. */
#define DEBUG1(a,b)                 /* _DEBUG is not defined. */
#define DEBUG2(a,b,c)               /* _DEBUG is not defined. */
#define DEBUG3(a,b,c,d)             /* _DEBUG is not defined. */
#define DEBUG4(a,b,c,d,e)           /* _DEBUG is not defined. */
#define DEBUG5(a,b,c,d,e,f)	        /* _DEBUG is not defined. */
#define DEBUG6(a,b,c,d,e,f,g)       /* _DEBUG is not defined. */
#define DEBUG7(a,b,c,d,e,f,g,h)     /* _DEBUG is not defined. */
#define DEBUG8(a,b,c,d,e,f,g,h,i)	/* _DEBUG is not defined. */

#endif // _DEBUG

#endif // AM_DEBUG_H
