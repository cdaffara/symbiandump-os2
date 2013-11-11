/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* ASN1 constants
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#if !defined(__ASN1CONS_H__)
#define __ASN1CONS_H__

#include <e32std.h>

const TInt KASN1ObjectMinLength = 2;

/**
 * @publishedAll
 * @released
 */
enum TASN1Class 
		{
		EUniversal=0, 
		EApplicationSpecific=1, 
		EContextSpecific=2, 
		EPrivate=3
		};

/**
 * @publishedAll
 * @released
 */
typedef TUint TTagType; // This will limit us to dealing with tags up to 255, I have put in the infrastructure to deal with encoding longer lengths but we probably don't want to cart them around all the time.

/**
 * @publishedAll
 * @released
 */
enum 
		{
		EASN1EOC=0,
		EASN1Boolean=1,
		EASN1Integer=2,
		EASN1BitString=3,
		EASN1OctetString=4,
		EASN1Null=5,
		EASN1ObjectIdentifier=6,
		EASN1ObjectDescriptor=7,
		EASN1External=8,
		EASN1Real=9,
		EASN1Enumerated=10,
		EASN1UTF8String=12,
		EASN1Sequence=16,
		EASN1Set=17,
		EASN1NumericString=18,
		EASN1PrintableString=19,
		EASN1TeletexString=20,
		EASN1VideotexString=21,
		EASN1IA5String=22,
		EASN1UTCTime=23,
		EASN1GeneralizedTime=24,
		EASN1GraphicString=25,
		EASN1VisibleString=26,
		EASN1GeneralString=27,
		EASN1UniversalString=28,
		EASN1BMPString=30
		};

#endif
