// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Special build functions: ASSERTING, NOTIFY on Leave, TRACING
// 
//

#ifndef __D32ASSERT_H__
#define __D32ASSERT_H__

#include <e32std.h>

#if defined(_DEBUG)
	#define __DEBUG(s) s
	#define _ASSERTIONS
	//#define _NOTIFY
	//#define _TRACE
	//#define __DBDUMP__
	#define __DBINVARIANT__
#else
	#define __DEBUG(s)
#endif

#ifdef __DBDUMP__
#include <f32file.h>		//RFile
#endif

/**
DECLARE_DB_DUMP, DECLARE_DB_DUMP2, DECLARE_DB_DUMP3 macros can be used for printing out
DBMS security policy data to a text file. 
__DBDUMP__ macro must be defined if you want to use them.
Usage:
Use DECLARE_DB_DUMP macro for declaring pure virtual Dump(RFile& aFile) method in a particular class.
Use DECLARE_DB_DUMP2 macro for declaring virtual Dump(RFile& aFile) method in a particular class.
Use DECLARE_DB_DUMP3 macro for declaring non-virtual Dump(RFile& aFile) method in a particular class.
@internalComponent
*/

#ifdef __DBDUMP__
#define DECLARE_DB_DUMP(aFile) virtual void Dump(RFile& aFile) const = 0;
#define DECLARE_DB_DUMP2(aFile) virtual void Dump(RFile& aFile) const;
#define DECLARE_DB_DUMP3(aFile) void Dump(RFile& aFile) const;
#else
#define DECLARE_DB_DUMP(aFile)
#define DECLARE_DB_DUMP2(aFile)
#define DECLARE_DB_DUMP3(aFile)
#endif

/**
DECLARE_DB_INVARIANT, DECLARE_DB_INVARIANT2, DB_INVARIANT, DB_INVARIANT_ASSERT 
macros can be used for asserting the internal state of DBMS security policy classes. 
__DBINVARIANT__ macro must be defined if you want to use them.
Usage:
Use DECLARE_DB_INVARIANT macro for declaring virtual Invariant() method in a particular class.
Use DECLARE_DB_INVARIANT2 macro for declaring non-virtual Invariant() method in a particular class.
Use DB_INVARIANT macro to call Invariant() method somewhere in the code.
Use DB_INVARIANT_ASSERT macro in places, where you want to use __ASSERT macro (it may not be defined).
@internalComponent
*/

#ifdef __DBINVARIANT__
#define DECLARE_DB_INVARIANT() virtual void Invariant() const;
#define DECLARE_DB_INVARIANT2() void Invariant() const;
#define DB_INVARIANT() Invariant()
#define DB_INVARIANT_ASSERT(aExprVal) Util::Invariant(aExprVal)
#else
#define DECLARE_DB_INVARIANT()
#define DECLARE_DB_INVARIANT2()
#define DB_INVARIANT() void(0)
#define DB_INVARIANT_ASSERT(aExprVal) void(0)
#endif

/**
@internalComponent
*/
class Util
	{
public:
	static void Assert(const TText* aFile,TInt aLine);
//
	static void Leave(const TText* aFile,TInt aLine,TInt aError);
	static TInt LeaveIfError(const TText* aFile,TInt aLine,TInt aError);
//
	static void Trace(const TText* aFile,TInt aLine);
	static void Trace(const TText* aFile,TInt aLine,const TText* aString);
	static void Trace(const TText* aFile,TInt aLine,const TText* aExp,const TDesC& aDes);
	static void Trace(const TText* aFile,TInt aLine,const TText* aExp,TInt aVal);
	static void Trace(const TText* aFile,TInt aLine,const TText* aExp,const TAny* aPtr);
	static void Invariant(TBool aExprVal);
private:
	static TPtrC Filename(const TText* aFile);
	};

#define __STRING(s) _S(s)

#if defined(_ASSERTIONS)
	#define __ASSERT(c) (void)((c)||(Util::Assert(__STRING(__FILE__),__LINE__),0))
#else
	#define __ASSERT(c) void(0)
#endif

#if defined(_NOTIFY)
	#define __LEAVE(r) Util::Leave(__STRING(__FILE__),__LINE__,r)
	#define __LEAVE_IF_ERROR(r) Util::LeaveIfError(__STRING(__FILE__),__LINE__,r)
#else
	#define __LEAVE(r) User::Leave(r)
	#define __LEAVE_IF_ERROR(r) User::LeaveIfError(r)
#endif

#if defined(_TRACE)
	#define __TRACEP() Util::Trace(__STRING(__FILE__),__LINE__)
	#define __TRACES(string) Util::Trace(__STRING(__FILE__),__LINE__,_S(string))
	#define __TRACE(exp) Util::Trace(__STRING(__FILE__),__LINE__,__STRING(#exp),exp)
#else
	#define __TRACEP() void(0)
	#define __TRACES(string) void(0)
	#define __TRACE(exp) void(0)
#endif

#endif//__D32ASSERT_H__
