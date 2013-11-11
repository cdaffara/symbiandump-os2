/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <e32cons.h>
#include <stdarg.h>
#include <stdio.h>

void CConsoleBase::Printf(const TDesC &str, ...)
	{
		va_list args;
		va_start(args, str);

		TBuf8<200> nstr;
		nstr.Copy(str);
		
		char *p = (char *)nstr.Ptr();
		p[nstr.Length()] = '\0';

		vprintf(p, args);

		va_end(args);
	}

CConsoleBase *Console::NewL(const TDesC &, TSize)
	{
	return 0;
	}

void CleanupStack::PopAndDestroy(CConsoleBase *aCons)
{
	aCons->Close();
}

// End of file
