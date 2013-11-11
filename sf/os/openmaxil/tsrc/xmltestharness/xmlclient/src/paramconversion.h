/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@internalComponent
*/

#ifndef PARAMCONVERSION_H_
#define PARAMCONVERSION_H_

#include <e32cmn.h>

struct OMX_PARAM_CONTENTURITYPE;

class ParamConversion
	{
public:
	/**
	 * Converts a file name to a URI, then allocates and fills an 
	 * OMX_PARAM_CONTENTURITYPE structure for OpenMAX IL v1.0. In OpenMax IL
	 * v1.0, this structure cannot contain URIs larger than 256 bytes
	 * (including the null terminator). If the converted URI exceeds this
	 * limit, this method leaves with KErrTooBig.
	 */
	static OMX_PARAM_CONTENTURITYPE*
		FilenameAsContentUriStructV1_0L(const TDesC& aFileName);
	
	/**
	 * Converts a file name to a URI, then allocates and fills an
	 * OMX_PARAM_CONTENTURITYPE structure for OpenMAX IL v1.1. In OpenMax IL
	 * v1.1, this structure can contain URIs of arbitrary length.
	 */
	static OMX_PARAM_CONTENTURITYPE*
		FilenameAsContentUriStructV1_1L(const TDesC& aFileName);
	};

#endif /*PARAMCONVERSION_H_*/
