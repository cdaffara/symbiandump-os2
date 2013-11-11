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
* Description: 
*
*/

 

 
#ifndef __LIBDBUS_CONVENIENCE_WSD_SOLUTION_H__
#define __LIBDBUS_CONVENIENCE_WSD_SOLUTION_H__
#include "libdbus_convenience_wsd_macros.h"
#include "dbus-message-private.h"

#if EMULATOR
#ifdef __cplusplus
extern "C" 
{
#endif

struct libdbus_convenience_global_struct
{
#ifdef DBUS_BUILD_TESTS
	VARIABLE_DECL_ARRAY(validities_seen,s,dbus_message_util,int,DBUS_VALIDITY_LAST + _DBUS_NEGATIVE_VALIDITY_COUNT)
	VARIABLE_DECL(arrays_write_fixed_in_blocks,s,dbus_marshal_recursive_util,dbus_bool_t)
	VARIABLE_DECL(n_iterations_completed_total,s,dbus_marshal_recursive_util,int)
	VARIABLE_DECL(n_iterations_completed_this_test,s,dbus_marshal_recursive_util,int)
	VARIABLE_DECL(n_iterations_expected_this_test,s,dbus_marshal_recursive_util,int)
	VARIABLE_DECL(cycle,s,run_test_delete_values,int)
#endif //DBUS_BUILD_TESTS
	VARIABLE_DECL(babysit_sigchld_pipe,s,dbus_spawn,int)

	 /*END-global vars*/ 
};

struct libdbus_convenience_global_struct *libdbus_convenience_ImpurePtr();
int libdbus_convenience_Init(struct libdbus_convenience_global_struct *);

#ifdef __cplusplus
}
#endif

#endif //EMULATOR
#endif //__LIBDBUS_CONVENIENCE_WSD_SOLUTION_H__

