/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Class with implementation of xmlenc padding.
*
*/


#ifndef C_XMLSEC_PADDING_H
#define C_XMLSEC_PADDING_H

#include <padding.h>
    
/**
 * This concrete subclass of CPadding implements padding according to 
 * the "XML Encryption Syntax and Processing" W3C Recomendation
 * ( @see http://www.w3.org/TR/xmlenc-core/#sec-Alg-Block). 
 *
 *  @lib xmlseccrypto.dll
 *  @since S60 v3.2
 */
class CXmlSecPadding : public CPadding
	{ 
 public:
   /**
	* Creates a new CXmlSecPadding object.
	* 
	* @since S60 v3.2
	* @param aBlockBytes	The block size in bytes.
	* @return				A pointer to the new CXmlSecPadding object.
	*/
	static CXmlSecPadding* NewL(TInt aBlockBytes);

   /**
	* Creates a new CXmlSecPadding object and leaves a pointer to it on the cleanup stack.
	*
	* @since S60 v3.2
	* @param aBlockBytes	The block size in bytes.
	* @return				A pointer to the new CXmlSecPadding object.
	*/
	static CXmlSecPadding* NewLC(TInt aBlockBytes);
   /**
	* Pads aInput and places the result in aOutput.  
	*
	* @since S60 v3.2
	* @param aInput		Data to be padded.  
	* @param aOutput	On return, the resulting padded.
	*/
	void DoPadL(const TDesC8& aInput,TDes8& aOutput);
	
   /**
	* Removes padding from aInput and puts result to aOutput.
	* 
	* @since S60 v3.2
	* @param aInput		Data to be unpadded.
	* @param aOutput	The unpadded data.
	*/
	void UnPadL(const TDesC8& aInput,TDes8& aOutput);
	
   /**
	* The smallest number of bytes that PadL() will add to aInput
	*
	* @since S60 v3.2
	* @return	The smallest number of padding bytes possible.
	*/
	TInt MinPaddingLength(void) const;
	
protected:
   /** 
	* Constructor
	* 
	* @since S60 v3.2
	* @param aBlockBytes	The block size in bytes.
	*/
	CXmlSecPadding(TInt aBlockBytes);
	
private:
    /**
     * Minimum pad lenght
     */
    static const TInt KMinPaddingLenght = 1;
};
    
#endif // C_XMLSEC_PADDING_H
