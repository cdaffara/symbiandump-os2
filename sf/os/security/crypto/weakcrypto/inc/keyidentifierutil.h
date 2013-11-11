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
* ** IMPORTANT ** PublishedPartner API's in this file are published to 3rd party developers via the 
* Symbian website. Changes to these API's should be treated as PublishedAll API changes and the Security TA should be consulted.
* This utility class provides functions to generate key identifer data from
* supplied key data
*
*/


/**
 @file
 @internalTechnology
*/

#ifndef _KEYIDENTIFIERUTIL_H_
#define _KEYIDENTIFIERUTIL_H_

#include <securitydefs.h>

class CRSAPublicKey;
class CDSAPublicKey;
class RInteger;

/**
 * Utilities for generation of the key identifier
 *
 * @internalTechnology
 */
class KeyIdentifierUtil
	{
public:
	IMPORT_C static void RSAKeyIdentifierL(const CRSAPublicKey&, TKeyIdentifier&);
	IMPORT_C static void DSAKeyIdentifierL(const CDSAPublicKey&, TKeyIdentifier&);
	IMPORT_C static void DHKeyIdentifierL(const RInteger&, TKeyIdentifier&);
	};

#endif
