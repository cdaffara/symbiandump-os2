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


/**
 @file
 @internalComponent
*/

#ifndef OMXILUTIL_H
#define OMXILUTIL_H

#include <e32def.h>

#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Types.h>


class TOmxILUtil
	{

public:

	IMPORT_C static OMX_ERRORTYPE CheckOmxStructSizeAndVersion(
		OMX_PTR apHeader, OMX_U32 aSize);

	IMPORT_C static void ClearBufferContents(
		OMX_BUFFERHEADERTYPE* apBufferHeader);

	};


#endif // OMXILUTIL_H


