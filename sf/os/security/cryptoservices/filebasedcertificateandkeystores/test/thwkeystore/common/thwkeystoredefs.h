/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Contains the messages which the ctf plug-in sends to itself. In a 
* client server model, these messages will be sent to server.
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef HWKEYSTOREDEFS_H
#define HWKEYSTOREDEFS_H

#include <e32base.h>

const TInt KHwKeystoreMajorVersion = 1;
const TInt KHwKeystoreMinorVersion = 0;
const TInt KHwKeystoreBuildVersion = 0;

/**	
 * Request message enumeration within the client. This is needed since 
 * unified key store calls are asynchronous in nature. Hence client 
 * needs to send messages to itself.
 */
enum THwKeystoreMessages
	{
	ERequireNoCapabilities=0x00,
	EIdle,
	ECancel,
	EImportKey,
	EExportKey,
	EKeyPresent,
	ECreateSigner,
	ECreateAsymmetricCipher,
	ESignInHardware,
	EDecryptInHardware
	};



#endif	//	HWKEYSTOREDEFS_H
