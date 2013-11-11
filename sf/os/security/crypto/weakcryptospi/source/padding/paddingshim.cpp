/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "paddingshim.h"
#include <cryptospi/cryptospidef.h>

// CPaddingNoneShim
CPaddingNoneShim* CPaddingNoneShim::NewL(TInt aBlockBytes)
	{
	CPaddingNoneShim* self = CPaddingNoneShim::NewLC(aBlockBytes);
	CleanupStack::Pop(self);
	return self;
	}
CPaddingNoneShim* CPaddingNoneShim::NewLC(TInt aBlockBytes)
	{
	CPaddingNoneShim* self = new(ELeave) CPaddingNoneShim(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}
	
TInt CPaddingNoneShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt err(KErrNone);
	if (aExtensionId == CryptoSpi::KPaddingInterface)
		{
		TUid* uid(static_cast<TUid*>(a0));
		*uid = CryptoSpi::KPaddingModeNoneUid;
		}
	else 
		{
		err = KErrExtensionNotSupported;
		}
	return err;
	}

CPaddingNoneShim::CPaddingNoneShim(TInt aBlockBytes) :
	CPaddingNone(aBlockBytes)
	{
	}

// CPaddingPKCS1SignatureShim
CPaddingPKCS1SignatureShim* CPaddingPKCS1SignatureShim::NewL(TInt aBlockBytes)
	{
	CPaddingPKCS1SignatureShim* self = CPaddingPKCS1SignatureShim::NewLC(aBlockBytes);
	CleanupStack::Pop(self);
	return self;
	}
CPaddingPKCS1SignatureShim* CPaddingPKCS1SignatureShim::NewLC(TInt aBlockBytes)
	{
	CPaddingPKCS1SignatureShim* self = new(ELeave) CPaddingPKCS1SignatureShim(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}
	
TInt CPaddingPKCS1SignatureShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt err(KErrNone);
	if (aExtensionId == CryptoSpi::KPaddingInterface)
		{
		TUid* uid(static_cast<TUid*>(a0));
		*uid = CryptoSpi::KPaddingModePkcs1_v1_5_SignatureUid;
		}
	else 
		{
		err = KErrExtensionNotSupported;
		}
	return err;
	}

CPaddingPKCS1SignatureShim::CPaddingPKCS1SignatureShim(TInt aBlockBytes) :
	CPaddingPKCS1Signature(aBlockBytes)
	{
	}
	
// CPaddingPKCS1EncryptionShim
CPaddingPKCS1EncryptionShim* CPaddingPKCS1EncryptionShim::NewL(TInt aBlockBytes)
	{
	CPaddingPKCS1EncryptionShim* self = CPaddingPKCS1EncryptionShim::NewLC(aBlockBytes);
	CleanupStack::Pop(self);
	return self;
	}
CPaddingPKCS1EncryptionShim* CPaddingPKCS1EncryptionShim::NewLC(TInt aBlockBytes)
	{
	CPaddingPKCS1EncryptionShim* self = new(ELeave) CPaddingPKCS1EncryptionShim(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}
	
TInt CPaddingPKCS1EncryptionShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt err(KErrNone);
	if (aExtensionId == CryptoSpi::KPaddingInterface)
		{
		TUid* uid(static_cast<TUid*>(a0));
		*uid = CryptoSpi::KPaddingModePkcs1_v1_5_EncryptionUid;
		}
	else 
		{
		err = KErrExtensionNotSupported;
		}
	return err;
	}

CPaddingPKCS1EncryptionShim::CPaddingPKCS1EncryptionShim(TInt aBlockBytes) :
	CPaddingPKCS1Encryption(aBlockBytes)
	{
	}
	
// CPaddingSSLv3Shim
CPaddingSSLv3Shim* CPaddingSSLv3Shim::NewL(TInt aBlockBytes)
	{
	CPaddingSSLv3Shim* self = CPaddingSSLv3Shim::NewLC(aBlockBytes);
	CleanupStack::Pop(self);
	return self;
	}
CPaddingSSLv3Shim* CPaddingSSLv3Shim::NewLC(TInt aBlockBytes)
	{
	CPaddingSSLv3Shim* self = new(ELeave) CPaddingSSLv3Shim(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}
	
TInt CPaddingSSLv3Shim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt err(KErrNone);
	if (aExtensionId == CryptoSpi::KPaddingInterface)
		{
		TUid* uid(static_cast<TUid*>(a0));
		*uid = CryptoSpi::KPaddingModeSSLv3Uid;
		}
	else 
		{
		err = KErrExtensionNotSupported;
		}
	return err;
	}

CPaddingSSLv3Shim::CPaddingSSLv3Shim(TInt aBlockBytes) :
	CPaddingSSLv3(aBlockBytes)
	{
	}	
	
// CPaddingPKCS7Shim
CPaddingPKCS7Shim* CPaddingPKCS7Shim::NewL(TInt aBlockBytes)
	{
	CPaddingPKCS7Shim* self = CPaddingPKCS7Shim::NewLC(aBlockBytes);
	CleanupStack::Pop(self);
	return self;
	}
CPaddingPKCS7Shim* CPaddingPKCS7Shim::NewLC(TInt aBlockBytes)
	{
	CPaddingPKCS7Shim* self = new(ELeave) CPaddingPKCS7Shim(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}
	
TInt CPaddingPKCS7Shim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt err(KErrNone);
	if (aExtensionId == CryptoSpi::KPaddingInterface)
		{
		TUid* uid(static_cast<TUid*>(a0));
		*uid = CryptoSpi::KPaddingModePKCS7Uid;
		}
	else 
		{
		err = KErrExtensionNotSupported;
		}
	return err;	
	}

CPaddingPKCS7Shim::CPaddingPKCS7Shim(TInt aBlockBytes) :
	CPaddingPKCS7(aBlockBytes)
	{
	}			
