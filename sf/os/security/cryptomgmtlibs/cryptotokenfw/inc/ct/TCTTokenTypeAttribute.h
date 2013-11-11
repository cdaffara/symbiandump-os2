/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @publishedAll
 @released    
*/

#ifndef __TCTTOKENTYPEATTRIBUTE_H__
#define __TCTTOKENTYPEATTRIBUTE_H__

/** 
 * Attribute UIDs 
 * @since v7.0 
 */

/** 
 * A UID representing an attribute type. 
 *
 * It has a corresponding TCTTokenTypeAttribute value, which is a boolean,
 * that indicates whether the token is implemented in software. 
 */
const TUid KCTSoftware = {0x101F527B};

/** 
 * A UID representing an attribute type. 
 * 
 * It has a corresponding TCTTokenTypeAttribute value, which is boolean,
 * that indicates whether the token is removable. 
 */
const TUid KCTRemovable = {0x101F527C};

/** 
 * A UID representing an attribute type. 
 * 
 * It has a corresponding TCTTokenTypeAttribute value, which is boolean,
 * that indicates whether the token type provides its own access control. 
 */
const TUid KCTAccessControlled = {0x101F527D};

/** 
 * A UID representing an attribute type. 
 * 
 * It has a corresponding TCTTokenTypeAttribute value, which is an integer, 
 * that represents the maximum number of tokens of this type.
 *
 * For example, if the token type is a WIM-card-hardware token and the device has two 
 * card readers, the maximum number of tokens of this type is 2. 
 */
const TUid KCTMaxTokenCount = {0x101F527E};

/** 
 * A UID representing an attribute type. 
 *
 * It has a corresponding TCTTokenTypeAttribute value, which is an integer, 
 * that represents the minimum number of tokens of this type.
 *
 * For example, if the token type is a WIM-card-hardware token, the minimum number of 
 * tokens of this type is 0).
 */
const TUid KCTMinTokenCount = {0x101F527F};

/** 
 * Arbitrary attributes can be stored as UID/Value pairs.
 * 
 * This allows any attributes that can be represented as 32 bit integers to be
 * associated with token types.
 * 
 * It consists of a UID to identify the type of the attribute and an unsigned 
 * integer to hold the data. For Boolean attributes, 0 means false and a 
 * positive integer means true.
 *
 * @since v7.0 
 */
struct TCTTokenTypeAttribute
	{
	/** The UID representing the attribute type. */
	TUid iUID;
	/** The value of the attribute. */
	TInt iVal;
	};

#endif // __TCTTOKENTYPEATTRIBUTE_H__
