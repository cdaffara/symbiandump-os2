/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "fstokencliserv.h"
#include "tokentypesenum.h"
#include <mctkeystore.h>
#include <mctcertstore.h>
#include <mctcertapps.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
/** The UID of certificate application token type */
const TInt KTokenTypeCertApps = 0x101f7a37;
#endif

RSupportedTokensArray::RSupportedTokensArray()
	{
	_LIT(KFileKeyStoreName, "Software key store");
	_LIT(KFileCertStoreName, "Software certificate store");
	_LIT(KFileCertAppsName, "Software certificate applications store");
	
	const TDesC* array[] = {&KFileKeyStoreName, &KFileCertStoreName, &KFileCertAppsName};	
	Copy(array, ETotalTokensSupported);
	}

const TInt KTokenTypeUidValues[] = {KTokenTypeFileKeystore, KTokenTypeFileCertstore, KTokenTypeCertApps};

RTokenTypeUIDLookup::RTokenTypeUIDLookup() :
	TFixedArray<TInt, ETotalTokensSupported>(KTokenTypeUidValues, ETotalTokensSupported)
	{
	}
