/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/

#ifndef _SIGNAL_H_
#define _SIGNAL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "_ansi.h"
#include <sys/signal.h>

/** Atomic entity type (ANSI)
 */
typedef int	sig_atomic_t;		

/** Default action 
*/
#define SIG_DFL ((void (*)())0)		

/** Ignore action
 */
#define SIG_IGN ((void (*)())1)		

/** Error return 
*/
#define SIG_ERR ((void (*)())-1)	

typedef void (*_sig_func_ptr) ();

extern _sig_func_ptr signal(int, _sig_func_ptr);
extern int raise(int);

#ifdef __cplusplus
}
#endif
#endif /* _SIGNAL_H_ */
