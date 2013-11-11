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

#ifndef __TEST_DECLS_H__
#define __TEST_DECLS_H__

#include <assert.h>
#include <stdio.h>

inline void log_stat(const char* funcName, const char* fileName, int line, const char* expr, int status)
{
	FILE* file = fopen("c:\\logs\\libstdcpp_runtime_tests.log", "a+");
	fprintf(file, "%s %s %d %s ::",fileName, funcName, line, expr);
	if(status == 0)
		fprintf(file, "%s", "PASSED\n");
	else if(status == 1)
		fprintf(file, "%s", "FAILED\n");
	else
		fprintf(file, "%s","IGNORED\n");

	fclose (file);
}

inline void log_success(const char* funcName, const char* fileName, int line, const char* expr)
{
	log_stat(funcName, fileName, line, expr, 0);
}

inline void log_failure(const char* funcName, const char* fileName, int line, const char* expr)
{
	log_stat(funcName, fileName, line, expr, 1);
}

#define CPP_TESTS_ASSERT_ALLWAYS(expr)	( (expr) ? log_success(__PRETTY_FUNCTION__, __FILE__,\
													__LINE__, #expr) : \
												   log_failure(__PRETTY_FUNCTION__, __FILE__, \
													__LINE__, #expr))

#endif //__TEST_DECLS_H__