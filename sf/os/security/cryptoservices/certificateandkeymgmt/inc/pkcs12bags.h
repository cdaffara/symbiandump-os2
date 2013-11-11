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
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __PKCS12_BAGS_H__
#define __PKCS12_BAGS_H__

#include <asn1dec.h>
#include <signed.h>
#include <asnpkcs.h>
#include <pkcs12kdf.h>
#include <pkcs12safebag.h>
#include <pkcs12attribute.h>
#include <pkcs7contentinfo_v2.h>
#include <pkcs7encrypteddataobject.h>

namespace PKCS12
{
/** Object identifier for KeyBag */
_LIT(KPkcs12KeyBagOID, "1.2.840.113549.1.12.10.1.1"); 	

/** Object identifier for ShroudedKeyBag */
_LIT(KPkcs12ShroudedKeyBagOID, "1.2.840.113549.1.12.10.1.2"); 

/** Object identifier for CertBag */
_LIT(KPkcs12CertBagOID, "1.2.840.113549.1.12.10.1.3"); 

/** Object identifier for CrlBag */
_LIT(KPkcs12CrlBagOID, "1.2.840.113549.1.12.10.1.4"); 

/** Object identifier for SecretBag */
_LIT(KPkcs12SecretBagOID, "1.2.840.113549.1.12.10.1.5"); 

/** Object identifier for SafeContentsBag */
_LIT(KPkcs12SafeContentsBagOID, "1.2.840.113549.1.12.10.1.6");

/** Object identifier for x509 certificate */
_LIT(KX509CertificateOID,"1.2.840.113549.1.9.22.1"); 

/**
 This class decodes the KeyBag present in the SafeBag.
 It has a method to get the PrivatKeyInfo
 */
class CDecPkcs12KeyBag : public CDecPkcs12SafeBag
	{
public:
    /**
	 Creates a new PKCS#12KeyBag object.
	  
	 @param  aSafeBagData Contains a PKCS#12 SafeBag Structure.
	 @return A pointer to the newly allocated object.
	 @leave  KErrAgrument if the data is not a sequence or class tag name 
	  		 is not Universal and if iPrivateKeyInfo is NULL.
	 */
	IMPORT_C static CDecPkcs12KeyBag* NewL(const TDesC8& aSafeBagData);
	
    /**
     The PrivateKey information present in the KeyBag.
     The returned ASN1 sequence respects the following grammar:
      
 	 	PrivateKeyInfo ::= SEQUENCE {
 	 	version Version,
 	 	privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
 	 	privateKey PrivateKey,
 	 	attributes [0] IMPLICIT Attributes OPTIONAL }
 	 	
 	 	Version ::= INTEGER
 	 	PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
 	 	PrivateKey ::= OCTET STRING
 	 	Attributes ::= SET OF Attribute
     
  	 @return A pointer to a CDecPKCS8Data object. Ownership of memory is transferred 
  	 to the caller.
  	 @see    CDecPKCS8Data
	 */
	IMPORT_C CDecPKCS8Data* PrivateKeyInfoL() const;
	
	/**
	 Destructor.
	 */
	virtual ~CDecPkcs12KeyBag();
	
private:
	/**
     Decodes the entire KeyBag structure.
     @param aSafeBagData Contains a PKCS#12 SafeBag Structure.
     @leave  KErrAgrument if the data is not a sequence or class tag name 
	  		 is not Universal and if iPrivateKeyInfo is NULL.
	 @see	 TASN1DecPKCS8		 
     */
	void ConstructL(const TDesC8& aSafeBagData);
    
    /**
	 Constructor.
	 */
	CDecPkcs12KeyBag();
	
	/**
	 Copy Constructor.
	 @param aDecPkcs12keyBag A CDecPkcs12KeyBag object.
	 */
	CDecPkcs12KeyBag(const CDecPkcs12KeyBag& aDecPkcs12keyBag);
	
	/**
	 Assignment operator.
	 @param aDecPkcs12keyBag A CDecPkcs12KeyBag object.
	 @return A CDecPkcs12KeyBag class.
	 */
	CDecPkcs12KeyBag& operator=(const CDecPkcs12KeyBag& aDecPkcs12keyBag);
};


/**
 This class decodes the ShroudedKeyBag present in the SafeBag.
 It has a method to get the PrivatKeyInfo
 */
class CDecPkcs12ShroudedKeyBag : public CDecPkcs12SafeBag
	{
public:
   /**
	Creates a new PKCS#12KeyBag object.
	 
	@param aSafeBagData Contains a PKCS#12 shroudedKeyBag structure.
	@return A pointer to the newly allocated object.
	@leave  KErrAgrument if the data is not safeBag structure.
	*/
	IMPORT_C static CDecPkcs12ShroudedKeyBag* NewL(const TDesC8& aSafeBagData);
	
   /**
    The PrivateKey Information present in the ShroudKeyBag. 
    Below is the ASN.1 sequence.
    
    ContentEncryptionAlgorithmIdentifier ::= AlgorithmIdentifier
 
 	AlgorithmIdentifier: A type that identifies an algorithm (by object identifier) and associated parameters
 	AlgorithmIdentifier ::= SEQUENCE 
                        {
                        algorithm   ALGORITHM.&id({SupportedAlgorithms}),
                        parameters  ALGORITHM.&Type({SupportedAlgorithms}{@ algorithm}) OPTIONAL
                        }    
                        
	@return A pointer to a CDecPKCS8Data object. Ownership is transferred to the caller.
	@leave  KErrAgrument if the data is not a sequence or class tag name 
	  		 is not Universal.
	@leave  KErrNotSupported if otherthan PKCS12 pbeIds. 
	@leave  KErrGeneral	   if the decryption of the final part length is less than 0.
	@see    TASN1DecPKCS5, CPBEncryptElement, PKCS12KDF, CPBDecryptor, CDecPKCS8Data.
	*/
	IMPORT_C CDecPKCS8Data* PrivateKeyInfoL(TDesC& aPassword) const;
		
	/**
	 Destructor.
	 */
	virtual ~CDecPkcs12ShroudedKeyBag();
	
private:
	/**
     This decodes the entire KeyBag structure.
     @param  aSafeBagData Contains a PKCS#12 shroudedKeyBag Structure.
     @leave  KErrAgrument if the data is not a sequence or class tag name 
	  		 is not Universal.
	 @see	 TASN1DecPKCS8
     */
	void ConstructL(const TDesC8& aSafeBagData);
    
    /**
	 Constructor.
	 */
	CDecPkcs12ShroudedKeyBag();
	
	/**
	 Copy Constructor.
	 @param aDecPkcs12ShroudedKeyBag A CDecPkcs12ShroudedKeyBag object.
	 @return A CDecPkcs12ShroudedKeyBag class.
	 */
	CDecPkcs12ShroudedKeyBag(const CDecPkcs12ShroudedKeyBag& aDecPkcs12ShroudedKeyBag);
	
	/**
	 Assignment operator.
	 @param aDecPkcs12ShroudedKeyBag A CDecPkcs12ShroudedKeyBag object.
	 */
	CDecPkcs12ShroudedKeyBag& operator=(const CDecPkcs12ShroudedKeyBag& aDecPkcs12ShroudedKeyBag);
	};


/**
 This class decodes the CertBag 
 It has methods to get the CertId and the CertValue present in the CertBag
 The X509Certificate() method returns the x509 certificate 
 in case the CertId is x509
 */
class CDecPkcs12CertBag : public CDecPkcs12SafeBag
	{
public:
	/**
	 Creates a new PKCS#12CertBag object.
	  
	 @param aCertBagData contains a PKCS#12 CertBag Structure.
	 @leave KErrArgument if the data is not a sequence or class tag name 
	  		 is not Universal.
     @leave	KErrNotSupported if otherthan X509 certificate is present.
	 @return A pointer to the newly allocated object.
	 */
	 IMPORT_C static CDecPkcs12CertBag* NewL(const TDesC8& aCertBagData);
	 
	/**
	 This method returns the OID present in the certId field of CertBag sequence.	 	 	 	 
	 @return Returns OID present in the certId feild of CertBag sequence
	 */
	 IMPORT_C const TDesC& CertId() const;
	 
	/**
	 This method returns the DER encoded certValue present in the CertBag sequence.	 	 
	 @return The DER encoded certValue present in the CertBag sequence.
	 @see  X509Certificate
	 */
	 IMPORT_C const TDesC8& CertValue() const;
	 
	/**
	 This method returns the decoded x509 certificate.	 
	 @return Returns a pointer to a CX509Certificate object if the certificate of type 
	 X509; otherwise, null is returned. Ownership of memory is transferred to the caller.			 
	 @see CertValue
	 */
	 IMPORT_C CX509Certificate* X509CertificateL() const;
	 
	 /**
	 Destructor.
	 */
	virtual ~CDecPkcs12CertBag();
private:
	/**
     This decodes the entire CertBag structure.
     @param aCertBagData contains a PKCS#12 CertBag Structure.
     @leave KErrArgument if the data is not a sequence or class tag name 
	  		 is not Universal.      
     @see	CDecPkcs12SafeBag, CX509Certificate.
     */
	 void ConstructL(const TDesC8& aCertBagData);
	        
    /**
	 Constructor.
	 */
	 CDecPkcs12CertBag();
	
	/**
	 Copy Constructor.
	 @param aDecPkcs12CertBag A CDecPkcs12CertBag object.
	 */
	CDecPkcs12CertBag(const CDecPkcs12CertBag& aDecPkcs12CertBag);
	
	/**
	 Assignment operator.
	 @param aDecPkcs12CertBag A CDecPkcs12CertBag object.
	 @return A CDecPkcs12CertBag class.
	 */
	CDecPkcs12CertBag& operator=(const CDecPkcs12CertBag& aDecPkcs12CertBag);
     
private:
    /** Contains Object identifier indicating the certificate type*/
	HBufC* iCertId;
	
	/** Contains the certificate which is encoded and is an OCTET String */
	TPtrC8 iCertValue;	
	};

/**
 Decodes the SafeContents bag present within a SafeBag and 
 returns an array of Safebags present within this SafeContents bag.
 */
class CDecPkcs12SafeContentsBag : public CDecPkcs12SafeBag
	{
public:
    /**
	 Creates a new PKCS#12SafeContentsBag object.
	  
	 @param aSafeContentsBagData Contains a PKCS#12 SafeBag structure.
	 @return A pointer to the newly allocated object.
	 @leave KErrArgument if the data is not a sequence or class tag name 
	  		 is not Universal.
	 */
	IMPORT_C static CDecPkcs12SafeContentsBag* NewL(const TDesC8& aSafeContentsBagData);
	/**
	 The SafeContents Bag contains one or more Safe Bags in it.This 
	 method returns the reference to all these SafeBags.
	 
	 @return An array of SafeBags present within the SafeContentsBag
	 */
	IMPORT_C const RPointerArray<CDecPkcs12SafeBag>& SafeBags() const; 
	
	/**
	 Destructor.
	 */
	virtual ~CDecPkcs12SafeContentsBag();
	
private:
    /**
     This decodes the entire SafeContentsBag structure.
     @param aSafeContentsBagData Contains a PKCS#12 SafeBag structure.
     @leave KErrArgument if the data is not a sequence or class tag name 
	  		 is not Universal.
	 @see   CDecPkcs12SafeBag
     */
	void ConstructL(const TDesC8& aSafeContentsBagData);     
	
	/**
	 Constructor.
	 */
	 CDecPkcs12SafeContentsBag();
		
	/**
	 Copy Constructor.
	 @param aDecPkcs12SafeContentsBag A CDecPkcs12SafeContentsBag object.
	 */
	CDecPkcs12SafeContentsBag(const CDecPkcs12SafeContentsBag& aDecPkcs12SafeContentsBag);
	
	/**
	 Assignment operator.
	 @param aDecPkcs12SafeContentsBag A CDecPkcs12SafeContentsBag object.
	 @return A reference to CDecPkcs12SafeContentsBag class.
	 */
	CDecPkcs12SafeContentsBag& operator=(const CDecPkcs12SafeContentsBag& aDecPkcs12SafeContentsBag);
    
private:   
     /** Contains an array of SafeBags present within the SafeContents Bag */
	 RPointerArray<CDecPkcs12SafeBag> iSafeBags;
	 };	
	 
class CDecPkcs12SafeContents : public CBase
	{
public:
	/**
	 Creates a new CDecPkcs12SafeContents object for plain Data.
	  
	 @param  aSafeContentsBagData Contains a PKCS#7 ContentInfo Structure.
	 @return A pointer to the newly allocated object.
	 @leave  KErrArgument if the data is not a sequence or class tag name 
	  		 is not Universal.
	 */
	IMPORT_C static CDecPkcs12SafeContents* NewL(const CPKCS7ContentInfo& aSafeContentsBagData);
	
	/**
	 Creates a new CDecPkcs12SafeContents object for Encrypted Data.
	  
	 @param  aSafeContentsBagData Contains a PKCS#7 ContentInfo Structure.
	 @param	 aPassword			  aPassword is the password used for decryption. 
	 @return A pointer to the newly allocated object.
	 @leave  KErrArgument if the data is not a sequence or class tag name 
	  		 is not Universal.
	 */
	IMPORT_C static CDecPkcs12SafeContents* NewL(const CPKCS7ContentInfo& aSafeContentsBagData, const TDesC& aPassword);

	/**
	 Creates a new CDecPkcs12SafeContents object for Enveloped Data.
	 The class doesn't support the public key privacy mode  if the 
	 ContentInfo contains an EnvelopedData object.Client should decrypt 
	 the Envelope Data. A recipient opens the envelope by decrypting the 
	 one of the encrypted content-encryption keys with the recipient's 
	 private key and decrypts the encrypted content with the recovered 
	 content-encryption key and pass the plain data. 
	 @param  aSafeContentsBagData Contains a PKCS#7 ContentInfo Structure ContentData.
	 @return A pointer to the newly allocated object.
	 @leave  KErrArgument if the data is not a sequence or class tag name 
	  		 is not Universal.
	 */
	IMPORT_C static CDecPkcs12SafeContents* NewL(const TDesC8& aSafeContentsBagData);
	
	/**
	 The method returns the plain data.
	 @return A pointer to descriptor containing decrypted data.
	 		 Returns NULL pointer if the decrypted data is not present.		 
	 */
	IMPORT_C const TDesC8* DecryptedData() const;
	
	/**
	 The method returns array of safebags objects.
	 @return A pointer to array of safeBag objects.
	 */
	IMPORT_C const RPointerArray<CDecPkcs12SafeBag>& SafeContentsBags() const;
	
	/**
	 Destructor.
	 */
	virtual ~CDecPkcs12SafeContents();
	
private:
	/**
	 These objects represents the SafeBag Sequences present in the 
	 SafeContents Bag Sequence.
	 
	 @param aSafeContent Contains data to decode the bags. 
	 @leave KErrArgument if the aSafeContent is not a Sequence and class tag is not Universal.
	 @leave	KErrNotSupported if any bag otherthan keyBag, shroudedKeyBag,CertBag,CRLBag,SecretBag
	 						 and SafeContentBag.
	 @see   CDecPkcs12SafeBag
	 */
	void ConstructL(const TDesC8& aSafeContent);
	
	/**
	 This method decrypts the encrypted information. 
	 @param		aContentInfo Contains a PKCS#7 ContentInfo Structure.
	 @param     aPassword is the password used for decryption.
	 */
	void DecodeEncryptedDataL(const CPKCS7ContentInfo& aContentInfo, const TDesC& aPassword);
	
	/**
	 Constructor.
	 */
	CDecPkcs12SafeContents();
	
	/**
	 Copy Constructor.
	 @param aDecPkcs12SafeContents A CDecPkcs12SafeContents object.
	 */
	CDecPkcs12SafeContents(const CDecPkcs12SafeContents& aDecPkcs12SafeContents);
	
	/**
	 Assignment operator.
	 @param aDecPkcs12SafeContents A CDecPkcs12SafeContents object.
	 @return A reference to CDecPkcs12SafeContents class.
	 */
	CDecPkcs12SafeContents& operator=(const CDecPkcs12SafeContents& aDecPkcs12SafeContents);
	

private:
	/** Contains an array of SafeBags objects */
	RPointerArray<CDecPkcs12SafeBag> iSafeBags;
	
	/** Contains plain data */
	HBufC8* iDecryptedData;
	
	};
} // namespace PKCS12
#endif // __PKCS12BAGS_H__
