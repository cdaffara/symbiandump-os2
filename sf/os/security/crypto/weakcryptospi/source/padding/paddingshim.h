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


#ifndef __PADDINGSHIM_H__
#define __PADDINGSHIM_H__

/**
@file
@internalComponent
@released
*/

#include <padding.h>

/**
Shim class for CPaddingNone. This should only be created by CPaddingNone.
*/
NONSHARABLE_CLASS(CPaddingNoneShim) : public CPaddingNone
	{
public:
	static CPaddingNoneShim* NewL(TInt aBlockBytes);
	static CPaddingNoneShim* NewLC(TInt aBlockBytes);
	
protected:
	// From CBase
	/**
	Used by the CBufferedTransformShim class to determine the type of 
	the padding mode selected. The SPI plug-in is then configured to use
	the selected padding mdoe.
	
	@param aExtensionId The requested interface
	@param a0 Not used. This function does NOT return a real interface.
	@param a1 Not used.
	@return KErrNone if aExtensionId is CryptoSpi::KPaddingModeNoneUid; 
	otherwise, KErrNotFound is returned.
	*/
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);		
private:
	CPaddingNoneShim(TInt aBlockBytes);
	};

/**
Shim class for CPaddingPKCS1Signature. This should only be created by 
CPaddingPKCS1Signature.
*/
NONSHARABLE_CLASS(CPaddingPKCS1SignatureShim) : public CPaddingPKCS1Signature
	{
public:
	static CPaddingPKCS1SignatureShim* NewL(TInt aBlockBytes);
	static CPaddingPKCS1SignatureShim* NewLC(TInt aBlockBytes);
	
protected:
	// From CBase
	/**
	Used by the CBufferedTransformShim class to determine the type of 
	the padding mode selected. The SPI plug-in is then configured to use
	the selected padding mdoe.
	
	@param aExtensionId The requested interface
	@param a0 Not used. This function does NOT return a real interface.
	@param a1 Not used.
	@return KErrNone if aExtensionId is CryptoSpi::KPaddingModeNoneUid; 
	otherwise, KErrNotFound is returned.
	*/
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);		
private:
	CPaddingPKCS1SignatureShim(TInt aBlockBytes);
	};
	
/**
Shim class for CPaddingPKCS1Encryption. This should only be created by 
CPaddingPKCS1Encryption.
*/
NONSHARABLE_CLASS(CPaddingPKCS1EncryptionShim) : public CPaddingPKCS1Encryption
	{
public:
	static CPaddingPKCS1EncryptionShim* NewL(TInt aBlockBytes);
	static CPaddingPKCS1EncryptionShim* NewLC(TInt aBlockBytes);
	
protected:
	// From CBase
	/**
	Used by the CBufferedTransformShim class to determine the type of 
	the padding mode selected. The SPI plug-in is then configured to use
	the selected padding mdoe.
	
	@param aExtensionId The requested interface
	@param a0 Not used. This function does NOT return a real interface.
	@param a1 Not used.
	@return KErrNone if aExtensionId is CryptoSpi::KPaddingModeNoneUid; 
	otherwise, KErrNotFound is returned.
	*/
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);		
private:
	CPaddingPKCS1EncryptionShim(TInt aBlockBytes);
	};	

/**
Shim class for CPaddingSSLv3. This should only be created by 
CPaddingSSLv3.
*/
NONSHARABLE_CLASS(CPaddingSSLv3Shim) : public CPaddingSSLv3
	{
public:
	static CPaddingSSLv3Shim* NewL(TInt aBlockBytes);
	static CPaddingSSLv3Shim* NewLC(TInt aBlockBytes);
	
protected:
	// From CBase
	/**
	Used by the CBufferedTransformShim class to determine the type of 
	the padding mode selected. The SPI plug-in is then configured to use
	the selected padding mdoe.
	
	@param aExtensionId The requested interface
	@param a0 Not used. This function does NOT return a real interface.
	@param a1 Not used.
	@return KErrNone if aExtensionId is CryptoSpi::KPaddingModeNoneUid; 
	otherwise, KErrNotFound is returned.
	*/
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);		
private:
	CPaddingSSLv3Shim(TInt aBlockBytes);
	};	

/**
Shim class for CPaddingPKCS7. This should only be created by 
CPaddingPKCS7.
*/
NONSHARABLE_CLASS(CPaddingPKCS7Shim) : public CPaddingPKCS7
	{
public:
	static CPaddingPKCS7Shim* NewL(TInt aBlockBytes);
	static CPaddingPKCS7Shim* NewLC(TInt aBlockBytes);
	
protected:
	// From CBase
	/**
	Used by the CBufferedTransformShim class to determine the type of 
	the padding mode selected. The SPI plug-in is then configured to use
	the selected padding mdoe.
	
	@param aExtensionId The requested interface
	@param a0 Not used. This function does NOT return a real interface.
	@param a1 Not used.
	@return KErrNone if aExtensionId is CryptoSpi::KPaddingModeNoneUid; 
	otherwise, KErrNotFound is returned.
	*/
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);		
private:
	CPaddingPKCS7Shim(TInt aBlockBytes);
	};	

#endif // __PADDINGSHIM_H__
