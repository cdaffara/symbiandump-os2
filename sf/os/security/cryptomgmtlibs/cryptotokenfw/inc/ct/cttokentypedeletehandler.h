/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* CTTokenType Delete Handler
*
*/


/**
 @file 
 @internalComponent
*/

#ifndef CCTTOKENTYPEDELETEHANDLER_H
#define CCTTOKENTYPEDELETEHANDLER_H

#include <e32base.h> 

/**
 * Abstract base class for a handler object to be called when a CCTTokenType is
 * deleted.  The handler is called simply by being deleted.  It is called from
 * CCTTokenType's destructor.
 *
 * This allows for ecom-loaded tokens to be destroyed properly without
 * forcing clients of ctframework.dll to link against ecom.
 *
 */
class CCTTokenTypeDeleteHandler : public CBase
	{
 public:
	IMPORT_C virtual ~CCTTokenTypeDeleteHandler();
	};

#endif // CCTTOKENTYPEDELETEHANDLER_H
