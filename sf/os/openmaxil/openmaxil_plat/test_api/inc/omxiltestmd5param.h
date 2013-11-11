/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*    WARNING - This is a test extension which
*              a) Maybe REMOVED without warning
*              b) Maybe CHANGED without warning
*              c) Is only intended for use by internal test code, and should NEVER be used by production code or in a real device
*
*/


/**
@file
@internalComponent
*/

#ifndef OMXILTESTMD5PARAM_H
#define OMXILTESTMD5PARAM_H

#include <openmax/il/khronos/v1_x/OMX_Index.h>

const OMX_INDEXTYPE OMX_IndexParam_Symbian_MD5 = static_cast<OMX_INDEXTYPE>(0x7FD19E5C);

struct OMX_PARAM_SYMBIAN_MD5TYPE
	{
	OMX_VERSIONTYPE nVersion;
	OMX_U8 pHash[16];
	};

#endif //OMXILTESTMD5PARAM_H
