/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines inline funtions for shared between KMS extract LDD client-
* and driver-side code.
*
*/




/**
 @file
 @internalComponent
*/

#ifndef KMSEXTRLDDIMPL_INL
#define KMSEXTRLDDIMPL_INL

#include "kmsextrlddcommon.h"


inline TVersion KmsExtrLddImpl::Version()
/**
	This function is defined because there is no way to construct a literal
	version.  It is used to ensure that the driver was built with at least the
	same version as the client.
	
	@return					Version of KMS extract LDD.
 */
	{
	return TVersion(1, 0, 0);
	}

#endif	// KMSEXTRLDDIMPL_INL

