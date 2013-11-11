/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This Header has defintion for the TLS reentrant structure and related
 *                function prototypes
 *
*/


#ifndef __SIGNGAM_H__
#define __SIGNGAM_H__

#include "_ansi.h"
#define signgam (* __signgam())


	
#ifdef __cplusplus
extern "C" {
#endif  /*__cplusplus*/
			
IMPORT_C int* __signgam();
	
#ifdef __cplusplus
	}
#endif /*__cplusplus*/


#endif /*__SIGNGAM_H__*/



