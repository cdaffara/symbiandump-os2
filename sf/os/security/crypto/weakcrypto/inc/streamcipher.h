/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __STREAMCIPHER_H__
#define __STREAMCIPHER_H__

#include "msymmetriccipher.h"

/**
*	Abstract interface class to be implemented by Stream Ciphers.
*/
class CStreamCipher : public CSymmetricCipher
{
public:	//	From CSymmetricCipher
	/**	
	*	Implemented by calling the DoProcess() pure virtual function, 
	*	to be implemented by subclasses.
	*
	*	@param aInput	Input text.
	*	@param aOutput	Text after processing.
	*/
	IMPORT_C virtual void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);

	/**	
	*	Implemented by calling the DoProcess() pure virtual function, 
	*	to be implemented by subclasses.
	*
	*	@param aInput	Input text.
	*	@param aOutput	Text after processing.
	*/
	IMPORT_C virtual void Process(const TDesC8& aInput, TDes8& aOutput);	

	/**
	*	Gets the block size in bytes (always = 1 for stream ciphers).
	*
	*	@return	Cipher block size (in bytes).
	*/
	IMPORT_C virtual TInt BlockSize(void) const;
	IMPORT_C virtual TInt MaxOutputLength(TInt aInputLength) const;
	IMPORT_C virtual TInt MaxFinalOutputLength(TInt aInputLength) const;
protected:
	/**	
	*	DoProcess() pure virtual function, 
	*	to be implemented by subclasses.
	*
	*	@param aData	On input, text to be processed; on return, processed text.
	*/
	IMPORT_C virtual void DoProcess(TDes8& aData) = 0;
};

#endif	//	__STREAMCIPHER_H__
