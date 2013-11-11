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
* A small support DLL for STDLIB test programs
* 
*
*/



#ifdef __cplusplus
extern "C" {
#endif
#include <_ansi.h>

/* STDLIB equivalent of RTest
 */

struct __testdata {
	int iCheck;
	int iSubtest;
	int iErrno;
	};

extern struct __testdata __td;

IMPORT_C void td_Title(struct __testdata* td, char *title);
IMPORT_C void td_Next(struct __testdata* td, char *testname);
IMPORT_C void td_Test(struct __testdata* td, int line, int mustBeTrue);
IMPORT_C void td_TestErrno(struct __testdata* td, int line, int mustBeTrue, int expectedErrno);
IMPORT_C void td_Close(void);

#define test_Data		struct __testdata __td	/* declares global variable __td */

#define test_Title(title)		td_Title(&__td, title)
#define test_Next(testname)		td_Next(&__td, testname)
#define test_Close()			td_Close()
#define test_Test(line,cond)				td_Test(&__td, line, cond)
#define test_TestErrno(line,cond,expected)	td_TestErrno(&__td, line, cond, expected)

#define test_Start(x)			test_Next(x)
#define test(f)					test_Test(__LINE__, (f)?1:0)
#define test_ok(f)				test_TestErrno(__LINE__, (f)?1:0, errno)
#define test_errno(f,expected)	test_TestErrno(__LINE__, (f)?1:0, expected)


/* Routine to ensure that the Comms server is running
 */
IMPORT_C int CommInit(int enhanced);


/* C routine for starting the redirection server
 */

IMPORT_C int	start_redirection_server(void);


/* C routines for creating EPOC32 threads
 */

IMPORT_C int	start_posix_server(void);

IMPORT_C void*	create_thread(void (*aFn)(), char* aName);
IMPORT_C void	start_thread(void* aThread);
IMPORT_C int	wait_for_thread(void* aThread);

IMPORT_C void*	create_process(void (*aFn)(), char* aName, char* mode, int fids[3]);
IMPORT_C void	start_process(void* aThread);
IMPORT_C int	wait_for_process(void* aThread);
IMPORT_C int    wait_for_process_id(void* aProcess, int procid, int opt, int* status);
IMPORT_C int 	get_proc_id(void* aProcess);
/* C-callable routines to access the EPOC32 asynchonous ioctl facilities
 *
 * NB. The int* status parameter stands in for the usual TRequestStatus&
 */
IMPORT_C int	async_ioctl(int aFid, int aCmd, void* aParam, int* status);
IMPORT_C int	async_ioctl_completion(int aFid, int aCmd, void* aParam, int* status);

#ifdef __cplusplus
}
#endif
