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
*
*/

/**
 @file 
 @publishedAll
 @released
*/


/**
 * 
 * 
 * @param aLeft
 * @param aRight
 * @return
*/
inline TKeyUsagePKCS15 operator|(TKeyUsagePKCS15 aLeft, TKeyUsagePKCS15 aRight)
	{
	return static_cast<TKeyUsagePKCS15>(static_cast<TUint>(aLeft) | static_cast<TUint>(aRight));
	}


/** 
 *
 *
 * @param aLeft
 * @param aRight
 * @return
 */
inline TKeyUsagePKCS15 operator&(TKeyUsagePKCS15 aLeft, TKeyUsagePKCS15 aRight)
	{
	return static_cast<TKeyUsagePKCS15>(static_cast<TUint>(aLeft) & static_cast<TUint>(aRight));
	}


/** 
 *
 *
 * @param aLeft
 * @param aRight
 * @return
 */
inline const TKeyUsagePKCS15& operator|=(TKeyUsagePKCS15& aLeft, TKeyUsagePKCS15 aRight)
	{
	aLeft = aLeft | aRight;
	return aLeft;
	}


/** 
 *
 *
 * @param aLeft
 * @param aRight
 * @return
 */
inline const TKeyUsagePKCS15& operator&=(TKeyUsagePKCS15& aLeft, TKeyUsagePKCS15 aRight)
	{
	aLeft = aLeft & aRight;
	return aLeft;
	}


/** 
 *
 *
 * @param aLeft
 * @param aRight
 * @return
 */
inline TKeyUsageX509 operator|(TKeyUsageX509 aLeft, TKeyUsageX509 aRight)
	{
	return static_cast<TKeyUsageX509>(static_cast<TUint>(aLeft) | static_cast<TUint>(aRight));
	}


/** 
 *
 *
 * @param aLeft
 * @param aRight
 * @return
 */
inline TKeyUsageX509 operator&(TKeyUsageX509 aLeft, TKeyUsageX509 aRight)
	{
	return static_cast<TKeyUsageX509>(static_cast<TUint>(aLeft) & static_cast<TUint>(aRight));
	}


/** 
 *
 *
 * @param aLeft
 * @param aRight
 * @return
 */
inline const TKeyUsageX509& operator|=(TKeyUsageX509& aLeft, TKeyUsageX509 aRight)
	{
	aLeft = aLeft | aRight;
	return aLeft;
	}


/** 
 *
 *
 * @param aLeft
 * @param aRight
 * @return
 */
inline const TKeyUsageX509& operator&=(TKeyUsageX509& aLeft, TKeyUsageX509 aRight)
	{
	aLeft = aLeft & aRight;
	return aLeft;
	}
