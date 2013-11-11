/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file RFileLoggermacro.h
*/

#if !(defined __ET_RFILELOGGERMACRO_H__)
#define __ET_RFILELOGGERMACRO_H__

#include <test/rfilelogger.h>
#ifdef _DEBUG
	#ifndef __FILELOGGER_ACTIVE
		#define __FILELOGGER_ACTIVE
	#endif
#endif

#if defined (__FILELOGGER_ACTIVE ) || defined (SWTRACE_ACTIVE)
#define __FLOG_DECLARATION_MEMBER		RFileFlogger __logger__
//------------
#define __FLOG_CONNECT				__logger__.Connect()

#define __FLOG_CREATE(alogfilepath,amode)	__logger__.CreateLog((alogfilepath),(amode))

#define __FLOG_CLOSE				__logger__.Close()

#define __FLOG_SETLOGLEVEL(aloglevel)		__logger__.SetLogLevel((aloglevel))


//--------
#define __FLOG_INFO1(p1)							__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (p1))
#define __FLOG_INFO2(p1, p2)						__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (p1), (p2))
#define __FLOG_INFO3(p1, p2, p3)					__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (p1), (p2), (p3))
#define __FLOG_INFO4(p1, p2, p3, p4)				__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (p1), (p2), (p3), (p4))
#define __FLOG_INFO5(p1, p2, p3, p4, p5)			__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (p1), (p2), (p3), (p4), (p5))
#define __FLOG_INFO6(p1, p2, p3, p4, p5, p6)		__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6))
#define __FLOG_INFO7(p1, p2, p3, p4, p5, p6, p7)	__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define __FLOG_WARN1(p1)							__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (p1))
#define __FLOG_WARN2(p1, p2)						__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (p1), (p2))
#define __FLOG_WARN3(p1, p2, p3)					__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (p1), (p2), (p3))
#define __FLOG_WARN4(p1, p2, p3, p4)				__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (p1), (p2), (p3), (p4))
#define __FLOG_WARN5(p1, p2, p3, p4, p5)			__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (p1), (p2), (p3), (p4), (p5))
#define __FLOG_WARN6(p1, p2, p3, p4, p5, p6)		__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6))
#define __FLOG_WARN7(p1, p2, p3, p4, p5, p6, p7)	__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define __FLOG_ERR1(p1)								__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (p1))
#define __FLOG_ERR2(p1, p2)							__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (p1), (p2))
#define __FLOG_ERR3(p1, p2, p3)						__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (p1), (p2), (p3)) ;
#define __FLOG_ERR4(p1, p2, p3, p4)					__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (p1), (p2), (p3), (p4))
#define __FLOG_ERR5(p1, p2, p3, p4, p5)				__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (p1), (p2), (p3), (p4), (p5))
#define __FLOG_ERR6(p1, p2, p3, p4, p5, p6)			__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6))
#define __FLOG_ERR7(p1, p2, p3, p4, p5, p6, p7)		__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

// for extra log fields
#define __FLOG_EXTRA_INFO1(length, extra, p1)							__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (length), (extra), (p1))
#define __FLOG_EXTRA_INFO2(length, extra, p1, p2)						__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (length), (extra), (p1), (p2))
#define __FLOG_EXTRA_INFO3(length, extra, p1, p2, p3)					__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (length), (extra), (p1), (p2), (p3))
#define __FLOG_EXTRA_INFO4(length, extra, p1, p2, p3, p4)				__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (length), (extra), (p1), (p2), (p3), (p4))
#define __FLOG_EXTRA_INFO5(length, extra, p1, p2, p3, p4, p5)			__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (length), (extra), (p1), (p2), (p3), (p4), (p5))
#define __FLOG_EXTRA_INFO6(length, extra, p1, p2, p3, p4, p5, p6)		__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (length), (extra), (p1), (p2), (p3), (p4), (p5), (p6))
#define __FLOG_EXTRA_INFO7(length, extra, p1, p2, p3, p4, p5, p6, p7)	__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrInfo, (length), (extra), (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define __FLOG_EXTRA_WARN1(length, extra, p1)							__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (length), (extra), (p1))
#define __FLOG_EXTRA_WARN2(length, extra, p1, p2)						__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (length), (extra), (p1), (p2))
#define __FLOG_EXTRA_WARN3(length, extra, p1, p2, p3)					__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (length), (extra), (p1), (p2), (p3))
#define __FLOG_EXTRA_WARN4(length, extra, p1, p2, p3, p4)				__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (length), (extra), (p1), (p2), (p3), (p4))
#define __FLOG_EXTRA_WARN5(length, extra, p1, p2, p3, p4, p5)			__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (length), (extra), (p1), (p2), (p3), (p4), (p5))
#define __FLOG_EXTRA_WARN6(length, extra, p1, p2, p3, p4, p5, p6)		__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (length), (extra), (p1), (p2), (p3), (p4), (p5), (p6))
#define __FLOG_EXTRA_WARN7(length, extra, p1, p2, p3, p4, p5, p6, p7)	__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrWarn, (length), (extra), (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define __FLOG_EXTRA_ERR1(length, extra, p1)								__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (length), (extra), (p1))
#define __FLOG_EXTRA_ERR2(length, extra, p1, p2)							__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (length), (extra), (p1), (p2))
#define __FLOG_EXTRA_ERR3(length, extra, p1, p2, p3)						__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (length), (extra), (p1), (p2), (p3)) ;
#define __FLOG_EXTRA_ERR4(length, extra, p1, p2, p3, p4)					__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (length), (extra), (p1), (p2), (p3), (p4))
#define __FLOG_EXTRA_ERR5(length, extra, p1, p2, p3, p4, p5)				__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (length), (extra), (p1), (p2), (p3), (p4), (p5))
#define __FLOG_EXTRA_ERR6(length, extra, p1, p2, p3, p4, p5, p6)			__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (length), (extra), (p1), (p2), (p3), (p4), (p5), (p6))
#define __FLOG_EXTRA_ERR7(length, extra, p1, p2, p3, p4, p5, p6, p7)		__logger__.Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrErr, (length), (extra), (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#else
#define __FILELOG_DECLARATION_MEMBER
//------------
#define __FLOG_CONNECT

#define __FLOG_CREATE(alogfilepath,amode)

#define __FLOG_CLOSE

//--------
#define __FLOG_INFO1(p1)
#define __FLOG_INFO2(p1, p2)
#define __FLOG_INFO3(p1, p2, p3)
#define __FLOG_INFO4(p1, p2, p3, p4)
#define __FLOG_INFO5(p1, p2, p3, p4, p5)
#define __FLOG_INFO6(p1, p2, p3, p4, p5, p6)
#define __FLOG_INFO7(p1, p2, p3, p4, p5, p6, p7)

#define __FLOG_WARN1(p1)
#define __FLOG_WARN2(p1, p2)
#define __FLOG_WARN3(p1, p2, p3)
#define __FLOG_WARN4(p1, p2, p3, p4)
#define __FLOG_WARN5(p1, p2, p3, p4, p5)
#define __FLOG_WARN6(p1, p2, p3, p4, p5, p6)
#define __FLOG_WARN7(p1, p2, p3, p4, p5, p6, p7)

#define __FLOG_ERR1(p1)
#define __FLOG_ERR2(p1, p2)
#define __FLOG_ERR3(p1, p2, p3)
#define __FLOG_ERR4(p1, p2, p3, p4)
#define __FLOG_ERR5(p1, p2, p3, p4, p5)
#define __FLOG_ERR6(p1, p2, p3, p4, p5, p6)
#define __FLOG_ERR7(p1, p2, p3, p4, p5, p6, p7)

// for extra log fields
#define __FLOG_EXTRA_INFO1(length, extra, p1)
#define __FLOG_EXTRA_INFO2(length, extra, p1, p2)
#define __FLOG_EXTRA_INFO3(length, extra, p1, p2, p3)
#define __FLOG_EXTRA_INFO4(length, extra, p1, p2, p3, p4)
#define __FLOG_EXTRA_INFO5(length, extra, p1, p2, p3, p4, p5)
#define __FLOG_EXTRA_INFO6(length, extra, p1, p2, p3, p4, p5, p6)
#define __FLOG_EXTRA_INFO7(length, extra, p1, p2, p3, p4, p5, p6, p7)

#define __FLOG_EXTRA_WARN1(length, extra, p1)
#define __FLOG_EXTRA_WARN2(length, extra, p1, p2)
#define __FLOG_EXTRA_WARN3(length, extra, p1, p2, p3)
#define __FLOG_EXTRA_WARN4(length, extra, p1, p2, p3, p4)
#define __FLOG_EXTRA_WARN5(length, extra, p1, p2, p3, p4, p5)
#define __FLOG_EXTRA_WARN6(length, extra, p1, p2, p3, p4, p5, p6)
#define __FLOG_EXTRA_WARN7(length, extra, p1, p2, p3, p4, p5, p6, p7)

#define __FLOG_EXTRA_ERR1(length, extra, p1)
#define __FLOG_EXTRA_ERR2(length, extra, p1, p2)
#define __FLOG_EXTRA_ERR3(length, extra, p1, p2, p3)
#define __FLOG_EXTRA_ERR4(length, extra, p1, p2, p3, p4)
#define __FLOG_EXTRA_ERR5(length, extra, p1, p2, p3, p4, p5)
#define __FLOG_EXTRA_ERR6(length, extra, p1, p2, p3, p4, p5, p6)
#define __FLOG_EXTRA_ERR7(length, extra, p1, p2, p3, p4, p5, p6, p7)

#endif
#endif
