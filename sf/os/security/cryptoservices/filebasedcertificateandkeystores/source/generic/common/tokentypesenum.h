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
* Various enumeration values shared between client and server (eg token
* and tokentype identification, panic codes etc)
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __TOKENTYPESENUM_H__
#define __TOKENTYPESENUM_H__

#include <e32std.h>


enum ETokenEnum	//	Interchangeable between tokens & tokentypes (ie 1:1 mapping)
	{
	EFileKeyStore			= 0,
	EFileCertStore			= 1,
	EFileCertApps			= 2,
	ETotalTokensSupported
	};

enum ETokenTypeClientPanic
{
	EInvalidTokenType		= 1,
	EInvalidToken			= 2,
	EInvalidInterface		= 3,
	EClientSession			= 4,
	EBadTokenHandle			= 5,
	EBadArgument			= 6,
	ENotInitialised			= 7,
	EBadTokenIFHandle		= 8,
	EInvalidRequest			= 9,
	ERequestOutstanding		= 10,
	ENoRequestOutstanding	= 11,
	ENoDataMarshalled		= 12,
	EMarshalDescriptorErr	= 13,
	EClientInvalidState		= 14
};

enum ETokenTypeServerPanic
{
	EPanicBadDescriptor			= 0,
	EPanicIllegalFunction		= 1,
	EPanicInvalidRequest		= 2,
	EPanicAlreadyInitialised	= 3,
	EPanicNotInitialised		= 4,
	EPanicBadSession			= 5,
	EPanicNoToken				= 6,
	EPanicInvalidTokenRequest	= 7,
	EPanicNotIndexed			= 8,
	EOutstandingIFPtr			= 9,
	EOutstandingTokenPtr		= 10,
	EOutstandingTokenTypePtr	= 11,
	EOutstanding				= 12,
	EPanicBadHandle				= 13,		
	EPanicRequestOutstanding	= 14,
	EPanicBadArgument			= 15,
	EPanicArrayCleanup			= 16,
	EPanicTokenMismatch			= 17,
	EPanicNoCert				= 18,
	EPanicNoAllocation			= 19,
	EPanicNoKeyStore			= 20,
	EPanicECreateKeyNotReady	= 21,
	EPanicNoClientData			= 22,
	EPanicInvalidKeyCreateReq	= 23,
	EPanicStoreInitialised		= 24,
	EPanicRootStreamNotReady	= 25,
	EPanicSaltStreamNotReady	= 26,
	EPanicManagerStreamNotReady = 27,
	ENoCreatedKeyData			= 28,
	EPanicInvalidState			= 30,

	// For cert store
	EPanicCertStoreReplaceArguments			 = 101,
	EPanicCertStoreRestoreState				 = 103,
	EPanicCertStoreOpenState				 = 104,
	EPanicCertStoreEntryConstructArguments	 = 102,
	EPanicCertStoreEntryInternalizeState	 = 105,
	EPanicCertStoreEntryListAppendArguments	 = 106,
	EPanicCertStoreEntryListReplaceArguments = 107
};

#endif	//	__TOKENTYPESENUM_H__
