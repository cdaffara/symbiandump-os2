// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32base.h>
#include <e32svr.h>
#include "StringPoolImplementation.h"

//#define BAFL_SHOW_TRACE		

#ifdef BAFL_SHOW_TRACE
void BaflShowTrace(TRefByValue<const TDesC> aFmt,...)
	{
	RDebug::Print(aFmt);
	}
#else
void BaflShowTrace(TRefByValue<const TDesC> /*aFmt*/,...)
	{
	}
#endif

#ifdef _DEBUG

_LIT8(KLogPrefix, "StringPool: ");
_LIT8(KTooLong, "(Value Too Long to print)");

NONSHARABLE_CLASS(TStringLogOverflow) : public TDes8Overflow
	{
public:
	virtual void Overflow(TDes8& aDes);
	};

void TStringLogOverflow::Overflow(TDes8& /* aDes*/)
	{
	}

void StringUtils::LogIt(TRefByValue<const TDesC8> aFmt, ...)
//
//	Messages to the front end emulator and to the WAP log
	{
	VA_LIST list;
	VA_START(list,aFmt);

	_LIT8(KLogPrefix, "StringPool: ");

	TStringLogOverflow overflow;

	TBuf8<KMaxFileName - 1> buf;
	buf.Copy(KLogPrefix);
	buf.AppendFormatList(aFmt, list, &overflow);
	VA_END(list);

	TBuf<KMaxFileName> buf16;
	buf16.Copy(buf);
	BaflShowTrace(buf16);
	BaflShowTrace(_L("\n"));
	}
void StringUtils::LogIt1(TRefByValue<const TDesC8> aFmt)
//
//	Messages to the front end emulator and to the WAP log
	{
	const TDesC8& format = aFmt;

	TBuf8<KMaxFileName - 1> buf;
	buf.Copy(KLogPrefix);
	if (buf.Length() + format.Length() > KMaxFileName - 1)
		buf.Append(KTooLong);
	else
		buf.Append(aFmt);

	TBuf<KMaxFileName> buf16;
	buf16.Copy(buf);
	BaflShowTrace(_L("%S"), &buf16);
	BaflShowTrace(_L("\n"));
	}

#endif // _DEBUG
