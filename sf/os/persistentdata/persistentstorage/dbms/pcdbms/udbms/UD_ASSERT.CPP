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
// Ut_Assert.cpp
// 
//

#include "D32Assert.h"

#if defined(_ASSERTIONS) || defined(_NOTIFY) || defined(_TRACE)
TPtrC Util::Filename(const TText* aFile)
	{
	TPtrC p(aFile);
	TInt ix=p.LocateReverse('\\');	// MSVC uses \ as path separator, GCC uses /
	if (ix<0)
		ix=p.LocateReverse('/');
	if (ix>=0)
		p.Set(p.Mid(1+ix));
	return p;
	}
#endif//defined(_ASSERTIONS) || defined(_NOTIFY) || defined(_TRACE)

#if defined(_ASSERTIONS)
void Util::Assert(const TText* aFile,TInt aLine)
	{
	TBuf<32> b=_S("dbms:");
	b+=Filename(aFile);
	User::Panic(b,aLine);
	}
#endif//defined(_ASSERTIONS)

#if defined(_NOTIFY)
#pragma message(__FILE__ " : Leave Notification is enabled")

void Util::Leave(const TText* aFile,TInt aLine,TInt aError)
	{
	TPtrC f(Filename(aFile));
	TBuf<60> buf;
	_LIT(KFormat,"***Error=%d at %S(%d)");
	buf.Format(KFormat,aError,&f,aLine);
	__TRACE(buf);
	_LIT(KNotify,"DBMS Leave ");
	buf.Insert(0,KNotify);
//	User::InfoPrint(buf);
	__TRACE(buf);
	User::Leave(aError);
	}

TInt Util::LeaveIfError(const TText* aFile,TInt aLine,TInt aError)
	{
	if (aError<0)
		Leave(aFile,aLine,aError);
	return aError;
	}
#endif//defined(_NOTIFY)

#if defined(_TRACE)
#pragma message(__FILE__ " : Tracing is enabled")

void Util::Trace(const TText* aFile,TInt aLine)
	{
	TPtrC f(Filename(aFile));
	_LIT(KFormat,"%S(%d)\n");
	RDebug::Print(KFormat,&f,aLine);
	}

void Util::Trace(const TText* aFile,TInt aLine,const TText* aString)
	{
	TPtrC f(Filename(aFile));
	_LIT(KFormat,"%S(%d) : %s\n");
	RDebug::Print(KFormat,&f,aLine,aString);
	}
void Util::Trace(const TText* aFile,TInt aLine,const TText* aExp,const TDesC& aDes)
	{
	TPtrC f(Filename(aFile));
	_LIT(KFormat,"%S(%d) : %s = \"%S\"\n");
	RDebug::Print(KFormat,&f,aLine,aExp,&aDes);
	}

void Util::Trace(const TText* aFile,TInt aLine,const TText* aExp,const TAny* aPtr)
	{
	TPtrC f(Filename(aFile));
	_LIT(KFormat,"%S(%d) : %s = %08x\n");
	RDebug::Print(KFormat,&f,aLine,aExp,TUint(aPtr));
	}

void Util::Trace(const TText* aFile,TInt aLine,const TText* aExp,TInt aVal)
	{
	TPtrC f(Filename(aFile));
	_LIT(KFormat,"%S(%d) : %s = %d (0x%x)\n");
	RDebug::Print(KFormat,&f,aLine,aExp,aVal,aVal);
	}
#endif//defined(_TRACE)

#ifdef __DBINVARIANT__
void Util::Invariant(TBool aExprVal)
	{
	if(!aExprVal)
		{
		User::Invariant();
		}
	}
#endif//__DBINVARIANT__

