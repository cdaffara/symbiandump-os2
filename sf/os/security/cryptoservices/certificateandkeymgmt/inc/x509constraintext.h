/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Symbian specific X509.v3 certificate extensions that constrain the
* the devices, secure ids, vendor ids and capabilities for which 
* a software install signing certificate is valid.
*
*/


/**
 @file 
 @internalTechnology 
*/

#ifndef __X509CONSTRAINTEXT_H__
#define __X509CONSTRAINTEXT_H__

#include <e32base.h>
#include <x509certext.h>

class CX509IntListExt : public CX509ExtensionBase 
/** 
A Symbian specific X.509 v3 certificate extension that contains an arbitrary 
list of 32 bit integers.	

@internalTechnology
@released
*/
	{
public:
	/** 
	Creates a new CX509IntListExt object from the binary (DER) encoded 
	representation of a sequence of integers.
	
	@param aBinaryData	The encoded binary representation.
	@return				The new CX509IntListExt object.
	*/
	IMPORT_C static CX509IntListExt* NewL(const TDesC8& aBinaryData);
	
	/**
	Creates a new CX509IntListExt object from the binary (DER) encoded 
	representation of a sequence of integers, and puts a pointer to it 
	onto the cleanup stack.
	
	@param aBinaryData	The encoded binary representation.
	@return				The new CX509IntListExt object. 
	*/
	IMPORT_C static CX509IntListExt* NewLC(const TDesC8& aBinaryData);
			
	/**
	Destructor.	
	Frees all resources owned by the object.
	*/
	~CX509IntListExt();
	
	/**	
	Gets a reference to the array of decoded integers.
	Ownership is not transferred.
	@return A reference to the array of decoded integers.
	*/
	IMPORT_C const RArray<TInt>& IntArray() const;
	
protected:
	/** Second-phase constructor.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The position from which to start decoding. */
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	
	/**
	Decodes the binary representation of a sequence of integers.
		
	@param aBinaryData	The encoded binary representation. This is the same as 
						passed to ConstructL().
	@param aPos			The position from which to start decoding. Note that 
						the value passed points, in effect, to the content, 
						bypassing the header data. 
	*/
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);	
	
private:
	/**	The decoded array of integers. */	
	RArray<TInt> iIntArray;
	};

class CX509Utf8StringListExt : public CX509ExtensionBase 
/** 
A Symbian specific X.509 v3 certificate extension that contains an arbitrary 
list of UTF-8 strings.

@internalTechnology
@released
*/
	{
public:
	/** 
	Creates a new CX509Utf8StringListExt object from the binary (DER) encoded 
	representation of a sequence of integers.
	
	@param aBinaryData	The encoded binary representation.
	@return				The new CX509Utf8StringListExt object.
	*/
	IMPORT_C static CX509Utf8StringListExt* NewL(const TDesC8& aBinaryData);
	
	/**
	Creates a new CX509Utf8StringListExt object from the binary (DER) encoded 
	representation of a sequence of integers, and puts a pointer to it 
	onto the cleanup stack.
	
	@param aBinaryData	The encoded binary representation.
	@return				The new CX509Utf8StringListExt object. 
	*/
	IMPORT_C static CX509Utf8StringListExt* NewLC(const TDesC8& aBinaryData);
		
	/**
	Destructor.	
	Frees all resources owned by the object.
	*/
	~CX509Utf8StringListExt();
	
	/**	
	Gets a reference to the array of decoded strings (UTF-16).
	Ownership is not transferred.				
	@return A reference to the array of decoded strings in UTF-16.
	*/
	IMPORT_C const RPointerArray<HBufC>& StringArray() const;
	
protected:
	/** Second-phase constructor.	 
	@param aBinaryData	The encoded binary representation.
	@param aPos			The position from which to start decoding. 
	*/
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	
	/**		
	Decodes the binary representation of a sequence of UTF-8 strings. The 
	strings are converted and stored internal as UTF-8.
		
	@param aBinaryData	The encoded binary representation. This is the same as 
						passed to ConstructL().
	@param aPos			The position from which to start decoding. Note that 
						the value passed points, in effect, to the content, 
						bypassing the header data. 
	*/
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);	
	
private:
	/**	The decoded array of strings in UTF-16.	*/
	RPointerArray<HBufC> iStringArray;
	};

/** 
A Symbian specific X.509 v3 certificate extension that contains a capability 
set encoded as a bit string.

@internalTechnology
@released
*/
class CX509CapabilitySetExt : public CX509ExtensionBase
	{
public:
	/** 
	Creates a new CX509CapabilitySetExt object from the binary (DER) encoded 
	representation of a sequence of integers.
	
	@param aBinaryData	The encoded binary representation.
	@return				The new CX509CapabilitySetExt object.
	*/
	IMPORT_C static CX509CapabilitySetExt* NewL(const TDesC8& aBinaryData);
	
	/**
	Creates a new CX509CapabilitySetExt object from the binary (DER) encoded 
	representation of a sequence of integers, and puts a pointer to it 
	onto the cleanup stack.
	
	@param aBinaryData	The encoded binary representation.
	@return				The new CX509CapabilitySetExt object. 
	*/
	IMPORT_C static CX509CapabilitySetExt* NewLC(const TDesC8& aBinaryData);
		
	/**
	Destructor.	
	Frees all resources owned by the object.
	*/
	~CX509CapabilitySetExt();
	
	/** Gets a reference to the capability set.	
	@return the capability set represented by the bit string.
	*/
	IMPORT_C const TCapabilitySet& CapabilitySet() const;

protected:
	/** Second-phase constructor.
	 
	@param aBinaryData	The encoded binary representation.
	@param aPos			The position from which to start decoding. 
	*/
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	
	/**
	Constructs the a TCapabilitySet from a DER encoded bit string.
	
	@param aBinaryData	The encoded binary representation. This is the same as 
						passed to ConstructL().
	@param aPos			The position from which to start decoding. Note that 
						the value passed points, in effect, to the content, 
						bypassing the header data. 
	*/
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	
private:	
	/** The decoded capability set */
	TCapabilitySet iCapabilitySet;
	};

#endif
