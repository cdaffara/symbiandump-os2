/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Some exported functions have been removed from x509 - these were for pkcs10
* attributes, which are now implemented in pkcs10 and for the
* MX509SignableObject class which is no longer needed.  This file supplies
* placeholder functions which are used to preserve the order of the ordinals in
* the def files.  As new exports are added to x509, these will replace these
* placeholder exports in the def files.
*
*/


#include <e32base.h>

_LIT(KPanicCat, "X509");

EXPORT_C void RemovedExport1()
	{
	User::Panic(KPanicCat, KErrNotSupported);
	}

EXPORT_C void RemovedExport2()
	{
	User::Panic(KPanicCat, KErrNotSupported);
	}

EXPORT_C void RemovedExport3()
	{
	User::Panic(KPanicCat, KErrNotSupported);
	}

EXPORT_C void RemovedExport4()
	{
	User::Panic(KPanicCat, KErrNotSupported);
	}

EXPORT_C void RemovedExport5()
	{
	User::Panic(KPanicCat, KErrNotSupported);
	}

EXPORT_C void RemovedExport6()
	{
	User::Panic(KPanicCat, KErrNotSupported);
	}

EXPORT_C void RemovedExport7()
	{
	User::Panic(KPanicCat, KErrNotSupported);
	}

EXPORT_C void RemovedExport8()
	{
	User::Panic(KPanicCat, KErrNotSupported);
	}

EXPORT_C void RemovedExport9()
	{
	User::Panic(KPanicCat, KErrNotSupported);
	}
