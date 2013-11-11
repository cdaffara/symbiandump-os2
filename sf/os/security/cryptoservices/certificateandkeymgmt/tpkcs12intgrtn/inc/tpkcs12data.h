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
* This file contains the classes CPfxHeader,CSafeBagAttribute ,CSafeContentBag ,CSafeBagData.
*
*/


/**
 @file 
 @internalTechnology
*/
#ifndef __TPKCS12DATA_H
#define __TPKCS12DATA_H
//System Include
#include <e32base.h> 
#include <pkcs12.h>
#include <asnpkcs.h>

using namespace PKCS12;

/** 
To store the PKCS12 file details
*/
class CPfxHeader : public CBase
	{

public:

	/** creates a CPfxHeader object*/
	static CPfxHeader* NewL(const CDecPkcs12& aDecPkcs12 , TInt aError);
	/** creates a CPfxHeader object, push it on Cleanupstack*/
	static CPfxHeader* NewLC(const CDecPkcs12& aDecPkcs12 , TInt aError);
	/** destructor*/
	~CPfxHeader();
	/**  to set the contentinfo count */
	inline void SetContentInfoCount(TInt aContentInfoCount){iContentInfoCount = aContentInfoCount;}
	/** to increment the keybagcount */
	inline void IncrementKeyBagCount(){iKeyBagCount++ ;}
	/** to increment the shroudedskeybagcount */
	inline void IncrementShroudedKeyBagCount(){iShroudedKeyBagCount++ ;}
	/** to increment the certbagcount */
	inline void IncrementCertBagCount(){iCertBagCount++;}
	/** to increment the crlbagcount */
	inline void IncrementCrlBagCount(){iCrlBagCount++;}
	/** to increment the secretbagcount */
	inline void IncrementSecretBagCount(){iSecretBagCount++;}
	/** to increment the safecontentbagcount */
	inline void IncrementSafecontentBagCount(){iSafeContentsBagCount++;}
	/**  to set the pkcs12 Main Class ExpectedError */
	inline void SetPkcs12ActualError(TInt aPkcs12ActualError){iPkcs12ActualError = aPkcs12ActualError;}	
	/**  to get the version number*/
	inline TInt Version(){return iVersion;}
	/**  to get the integrity mode*/
	inline CDecPkcs12::TIntegrityMode IntegrityMode(){return iIntegrityMode;}
	/**  to get the mac data presence */
	inline TBool MacData(){return iIsMacDataPresent;}
	/**  to get the mac Id */
	inline  TDesC8& MacId(){return *iMac;}
	/** to get the mac salt */ 
	inline TDesC8& MacSalt(){return *iMacSalt;}
	/**  to get the iteration count */
	inline TInt	IterationCount(){return iIterationCount;}
	/**  to get the contentinfo count */
	inline TInt ContentInfoCount(){return iContentInfoCount;}
	/** to get the keybagcount */
	inline TInt KeyBagCount(){return iKeyBagCount;}

	/** to get the shroudedskeybagcount */
	inline TInt ShroudedKeyBagCount(){return iShroudedKeyBagCount;}
	/** to get the certbagcount */
	inline TInt CertBagCount(){return iCertBagCount;}
	/** to get the crlbagcount */
	inline TInt CrlBagCount(){return iCrlBagCount;}
	/** to append the secretbagcount */
	inline TInt SecretBagCount(){return iSecretBagCount;}
	/** to append the safecontentbagcount */
	inline TInt SafecontentBagCount(){return iSafeContentsBagCount;}
	inline TInt TotalSafeBagCount(){return (iKeyBagCount+iShroudedKeyBagCount+
											iCertBagCount+iCrlBagCount+iSecretBagCount);}
		
	/**  to get the pkcs12 Main Class ExpectedError */
	inline TInt Pkcs12ActualError(){return iPkcs12ActualError;}	
private:
	void ConstructL(const CDecPkcs12& aDecPkcs12 , TInt aError);	
	
	/**  contains the version number of the PKCS#12 file */
	TInt iVersion ;
	/**  contains the integrity mode of the PKCS#12 file */
	CDecPkcs12::TIntegrityMode iIntegrityMode;
	/**  contains 1 if macData is present else 0 */ 
	TBool iIsMacDataPresent;
	/**  contains the mac ID */ 
	HBufC8* iMac;
	/**  contains the mac salt */ 
	HBufC8* iMacSalt;
	/**  contains the iteration count */ 
	TInt iIterationCount ;			
	/**  contains the total content info count in the PKCS#12 file */
	TInt iContentInfoCount;			 
	/**  contains the total keybag count in the PKCS#12 file */ 
	TInt iKeyBagCount;				
	/**  contains the total shroudedkeybag count in the PKCS#12 file */ 
	TInt iShroudedKeyBagCount;		
	/**  contains the total certbag count in the PKCS#12 file */ 
	TInt iCertBagCount;				
	/**  contains the total crlbag count in the PKCS#12 file */ 
	TInt iCrlBagCount;				
	/**  contains the total secretbag count in the PKCS#12 file */ 
	TInt iSecretBagCount;	
	/**  contains the total safecontent bag count in the PKCS#12 file */
	TInt iSafeContentsBagCount;	
	/** contains PKCS12 main class error */
	TInt iPkcs12ActualError;	 
	
	/* Constructor */
	CPfxHeader();	
};

/*
To store the safecag attribute details
*/

class CSafeBagAttribute : public CBase
	{
public:	
	/** destructor*/
	~CSafeBagAttribute();	
	/** creates a CSafeBagAttribute object*/
	static CSafeBagAttribute* NewL(const CDecPkcs12Attribute& aAttribute);	
	/** creates a CSafeBagAttribute object, push it on Cleanupstack*/
	static CSafeBagAttribute* NewLC(const CDecPkcs12Attribute& aAttribute);

	/**  to set the Attribute Id */
	inline  void SetAttributeId(const TDesC& aAttributeId){iAttrId = aAttributeId.AllocL();}
	/**  to set the Attribute value count */
	inline  void SetAttributeValueCount(TInt aAttributeValCount){iAttrValCount = aAttributeValCount;}
	
	/**  to get the Attribute Id */
	inline  const TDesC& AttributeId(){return *iAttrId;}
	/**  to get the Attribute value count */
	inline  TInt AttributeValueCount(){return iAttrValCount;}
	
private:
/** constructor */
	CSafeBagAttribute();
	void ConstructL(const CDecPkcs12Attribute& aAttribute);
	/**contains the attribute id*/
	HBufC*	iAttrId;
	/**contains the attributes count*/		
	TInt	iAttrValCount;
	};

/* 
To store the safecontent bag details
*/ 
class CSafeContentBag : public CBase
	{
public:

	/** creates a CSafeContentBag object*/	
	static CSafeContentBag* NewL();
	/** creates a CSafeContentBag object, push it on Cleanupstack*/	
	static CSafeContentBag* NewLC();
	
	/** to set the safecontent bag number */
	inline void SetBagNumber(TInt aBagNumber){iBagNumber = aBagNumber;}
	/** to set the safebagcount in the safecontent bag*/
	inline void SetSafeBagCount(TInt aSafeBagCount){iSafeBagCount = aSafeBagCount;}
	
	/** to get the safecontent bag number */
	inline TInt BagNumber(){return iBagNumber;}
	/** to get the safebagcount in the safecontent bag*/
	inline TInt SafeBagCount(){return iSafeBagCount;}
	
	
private:
	/* constructor */
	CSafeContentBag();	
	/** contains the safecontent bag number in the PKCS#12 file*/
	TInt	iBagNumber ;	
	/** contains the safebag count in the safecontent bag*/
	TInt	iSafeBagCount;		
	/** Unit Test */
	TInt iExpectedError;
	
	};

/** 
To store the Safebag details 
*/
class CSafeBagData : public CBase
	{  
public: 
	/** creates a CSafeBagData object */	
	static CSafeBagData* NewL();	
	/** creates a CSafeBagData object, push it on Cleanupstack*/
	static CSafeBagData* NewLC();
	/*destructor*/
	~CSafeBagData();
	
	/** to set the contentinfo number corresponding to the bag*/
	inline void SetContentInfoNumber(TInt aContentInfo){iContentInfo = aContentInfo;}
	/** to set the contenttype  corresponding to the bag*/
	inline void SetContentInfoType(TInt aContentType){iContentType = aContentType;}
	/** to set the bag number corresponding to the bag*/
	inline void SetBagNumber(TInt aBagNumber){iBagNumber = aBagNumber;}
	/** to set the PrivateKeyInfoVersion corresponding to the bag*/
	inline void SetPrivateKeyInfoVersion(TInt aPrivateKeyInfoVersion){iPrivateKeyInfoVersion = aPrivateKeyInfoVersion;}
	/** to set the bag id corresponding to the bag*/
	inline void SetBagType(CDecPkcs12SafeBag::TBagId aBagId){iBagId = aBagId;}
	/** to set the bag value corresponding to the bag*/
	inline void SetBagValue(const TDesC8& aBagValue){iBagValue = aBagValue.AllocL();}
	/** to set the Algorithem id corresponding to the bag*/
	inline void SetAlgorithmId(const TDesC& aAlgorithmId){iAlgorithmID = aAlgorithmId.AllocL();}
	/** to set the Certificate id corresponding to the certbag*/
	inline void SetCertificateId(const TDesC& aCertificateID){iCertificateID = aCertificateID.AllocL();}
	/** to set the key type corresponding to the bag*/
	inline void SetKeyType(TAlgorithmId aKeyType){iKeyType = aKeyType;}
		
	/** to set the X509Certificate corresponding to the bag*/
	inline void SetX509Certificate(const CX509Certificate* aX509Certificate){iX509certificate = aX509Certificate;}
	/** to set the DerEncodedShroudedKey */
	inline void SetEncodedShroudedKey(const TDesC8& aEncodedShroudedKey){iEncodedShroudedKey = aEncodedShroudedKey.AllocL();}
	
	/** to get the contentinfo number corresponding to the bag*/
	inline TInt ContentInfoNumber(){return iContentInfo;}
	/** to get the contenttype  corresponding to the bag*/
	inline TInt ContentInfoType(){return iContentType ;}
	/** to get the bag number corresponding to the bag*/
	inline TInt BagNumber(){return iBagNumber;}
	/** to get the PrivateKeyInfoVersion corresponding to the bag*/
	inline TInt PrivateKeyInfoVersion(){return iPrivateKeyInfoVersion;}
	/** to get the bag id corresponding to the bag*/
	inline CDecPkcs12SafeBag::TBagId BagType(){return iBagId;}
	/** to get the bag value corresponding to the bag*/
	inline const TDesC8& BagValue(){return *iBagValue;}
	/** to get the Algorithem id corresponding to the bag*/
	inline const TDesC& AlgorithmId(){return *iAlgorithmID ;}
	/** to get the Certificate id corresponding to the certbag*/
	inline const TDesC& CertificateId(){return *iCertificateID ;}
	/** to get the key type corresponding to the bag*/
	inline TAlgorithmId KeyType(){return iKeyType;}
	/** to get the X509Certificate corresponding to the bag*/
	inline const CX509Certificate& X509Certificate(){return *iX509certificate;}
	/** to get the DerEncodedShroudedKey*/
	inline TDesC8& EncodedShroudedKey(){return *iEncodedShroudedKey;}
	
	/**  contains the attribute IDs and attribute values count*/	
	RPointerArray<CSafeBagAttribute> iAttributeIDs;	
	/**  contains the attribute values */	
	RPointerArray<HBufC8> iAttributeValues;
private:
	
	/**  contains the content info number of the safebag */ 
	TInt	iContentInfo;	
	/**  contains the content type of the safebag */ 
	TInt	iContentType ;	
	/** contains the bag number of the safebag */ 
	TInt	iBagNumber;
	/** contains the privatekey info version of the safebag */ 
	TInt iPrivateKeyInfoVersion ;	
	/**  contains the bag ID of the safebag */ 
	CDecPkcs12SafeBag::TBagId	iBagId;	
	/**  contains the bag value of the safebag */ 
	HBufC8*	iBagValue;	
	/**  contains the algorithem ID */	
	HBufC* iAlgorithmID;	
	/**  contains the certificate ID */	
	HBufC* iCertificateID;	
	/**  contains the key type */	
	TAlgorithmId  iKeyType;			

	/**  contains the X509Certificate */			
	const CX509Certificate* iX509certificate;
	/*contains the der encoded shrouded key */
	HBufC8*	iEncodedShroudedKey;	

	/** Unit Test */
	TInt iVersion;

	TAlgorithmId iAlgortihmId;

	TInt iExpectedError;
	/*Constructor*/
	CSafeBagData();
	
	};

#endif

