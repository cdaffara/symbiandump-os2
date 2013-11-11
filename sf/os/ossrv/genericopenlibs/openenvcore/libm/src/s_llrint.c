/*
Portions Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
* ============================================================================
*/
#include <sys/cdefs.h>
#ifndef __SYMBIAN32__
__FBSDID("$FreeBSD: src/lib/msun/src/s_llrint.c,v 1.1 2005/01/11 23:12:55 das Exp $");
#endif //__SYMBIAN32__
#define type		double
#define	roundit		rint
#define dtype		long long
#define	fn		llrint

#include "s_lrint.c"
