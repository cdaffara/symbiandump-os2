// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// STDIO functions implemented over EPOC32
// 
//

#include <e32std.h>
#include <_ansi.h>
#include <stdio_r.h>

#define UNUSED_VAR(a) a = a

extern "C" {
int e32cvt (char *buf, int buflen, double d, int width, int prec, int /*flags*/, int fchar)
	{
	TPtr8 res((TText8*)buf, buflen);
	TBuf8<0x50> fmt;
	TBool capitalE=EFalse;
	switch(fchar)
	{
	case 'f':
		fchar='f';
		break;

	case 'E':
		capitalE=ETrue;	// and fallthrough
	case 'e':
		fchar='e';
		break;

	case 'G':
		capitalE=ETrue;	// and fallthrough
	case 'g':
		fchar='g';
		break;
	default:
		return 0;
	}
	// construct a format string for use with TDes::Format - we can't use the
	// VFPRINTF original because it doesn't quite match and it might involve '*'
	// to read field widths from VFPRINTF arguments.
	fmt.Zero();
	fmt.Append('%');
	// flags?
	if (width)
		fmt.AppendNum(width);
	if (prec>=0)
		{
		fmt.Append('.');
		fmt.AppendNum(prec);
		}
	fmt.Append(fchar);
	TRAPD(r,res.Format(fmt,d));
    UNUSED_VAR(r);
	if (capitalE)
		{
		TInt pos=res.Locate('e');
		if (pos!=KErrNotFound)
			res[pos]='E';
		}
	return res.Length();
	}
}
