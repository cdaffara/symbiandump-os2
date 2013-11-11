/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Functions handling OOM situations.
*
*/


#ifndef _XMLSEC_ERROR_FLAG_H
#define _XMLSEC_ERROR_FLAG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include "xmlsec_exports.h"

XMLSEC_EXPORT void xmlSecSetErrorFlag(int error);
XMLSEC_EXPORT void xmlSecResetErrorFlag();
XMLSEC_EXPORT int xmlSecCheckErrorFlag();

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif // _XMLSEC_ERROR_FLAG_H
