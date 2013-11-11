// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include <e32std.h>

/** The following 3 function are defined so that the functions IdealLanguage,
SetIdealLanguage, ReleaseIdealLanguage will compile as part as an .exe
These functions use a global TAny* to store the data that would have been
stored in the TLS under normal compilation (ie If the BaUtils was compilled 
as part of a DLL).
*/

#ifndef __DLL__
TAny* ptr(NULL); 

TAny* Dll::Tls()
	{
	return ptr;	
	}

TInt Dll::SetTls(TAny* aPtr)
	{
	ptr = aPtr;
	return KErrNone;
	}
	
void Dll::FreeTls()
	{
	ptr = NULL;	
	}
#endif
