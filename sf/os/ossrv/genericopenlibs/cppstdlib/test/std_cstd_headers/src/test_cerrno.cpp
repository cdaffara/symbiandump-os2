// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <cerrno>

/*
 * Check availability of macros in cerrno
 */

int foo()
{
	int errVal  = EDOM;
	errVal += EILSEQ;
	errVal += ERANGE;
	errVal += errno;

	return errVal;
}

int E32Main()
{
	return foo();
}
