/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* definition of the cryptospi property
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOSPIPROPERTY_H_
#define __CRYPTOSPIPROPERTY_H_

namespace CryptoSpi
	{
	/**
	CryptoSpi Property Category
	*/	
	const TUid KCryptoSpiPropertyCat={0x10283514};

	/**
	The start of plugin dll name key
	*/
	const TUint KPluginsConfigurationKey=1;
	
	/**
	The maximum number of plugins in the configuartion file
	*/
	const TUint KPluginsConfigurationKeyLimit=100;
	
	/**
	The key for the number of plugin dlls 
	*/
	const TUint KPluginsConfigurationKeyCount=KPluginsConfigurationKeyLimit+1;	
	}
#endif //__CRYPTOSPIPROPERTY_H_
