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

#ifndef __PKCS12_H__
#define __PKCS12_H__

#include <s32strm.h>
#include <asn1dec.h>
#include <x509cert.h>
#include <asnpkcs.h>
#include <pkcs7contentinfo_v2.h>
#include <pkcs7digestinfo.h>
#include <pkcs7encrypteddataobject.h>
#include <pkcs12macdata.h>
#include <pkcs12safebag.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <pkcs7signedobject.h>
#endif

namespace PKCS12
{
class CDecPkcs12MacData;

/** PKCS12 Version */
const TInt KPkcs12Version = 3;   

/**
 Contains methods to decode and return the PFX structure.
 The structure contains the Version, MacData and AuthSafe.
 MacData is OPTIONAL.
 */
class CDecPkcs12 : public CBase
	{
public:
	enum TIntegrityMode
	/**
	 Identifies the type of Integrity Mode used in the PKCS12 PFX Structure
	 */
		{
		/** Password Integrity Mode used in the PKCS12 PFX Structure */
		EPasswordIntegrityMode = 1,
		
		/** Public Key Integrity Mode used in the PKCS12 PFX Structure */
		EPublicKeyIntegrityMode
		};  
    /**
	 Creates a new PKCS#12 object.
	 
	 @param  aRawData Contains a PKCS#12 PFX structure
	 @return A pointer to the newly allocated object.
	 @leave  KErrArgument if the aRawData is not Pkcs12 PFX Structure.
	 */
	IMPORT_C static CDecPkcs12* NewL(const TDesC8& aRawData); 
	
	/**
	 Creates a new PKCS#12 object.
	 
	 @param  aRawData Contains a PKCS#12 PFX structure
	 @return A pointer to the newly allocated object.
	 @leave  KErrArgument if the aRawData is not Pkcs12 PFX Structure.
	 */
	IMPORT_C static CDecPkcs12* NewLC(const TDesC8& aRawData);

	/**
	 Creates a new PKCS#12 object.
	 
	 @param  aStream contains a PKCS#12 PFX structure 
	 @return A pointer to the newly allocated object.
	 @leave  KErrArgument if the aRawData is not Pkcs12 PFX Structure.
	 */
	IMPORT_C static CDecPkcs12* NewL(RReadStream& aStream); 

	/**
	 Creates a new PKCS#12 object.
	  
	 @param  aStream Contains a PKCS#12 PFX structure
	 @return A pointer to the newly allocated object.
	 @leave  KErrArgument if the aRawData is not Pkcs12 PFX Structure.
	 */
	IMPORT_C static CDecPkcs12* NewLC(RReadStream& aStream);
       
    /**
	 Identifies the type of integrity mode used.
     In the case of Password Integrity mode, OID is 1.2.840.113549.1.7.1.
     In the case of Public Key Integrity mode, OID is 1.2.840.113549.1.7.2.
	 
     @return An enum that identifies the type of integrity mode used.
     */	
    IMPORT_C TIntegrityMode IntegrityMode() const;
	
	/**
	 Returns the Version number contained in the PKCS12 PFX Structure.
	 @return Returns the Version number contained in the PKCS12 PFX Structure..
     */
	IMPORT_C TInt Version() const;
	
	/**
	 Returns the authenticated safe.
	 This authenticated safe is used to find the integrity mode used
	 and to verify the integrity of the packet. 
	 
	 @return A reference to the CPKCS7ContentInfo object.
	 */	
	IMPORT_C const CPKCS7ContentInfo& AuthenticatedSafe() const;
	
	/**
	 The MacData, which contains:
	 - The Mac, that is the PKCS#7 digest info structure.
	 - The MacSalt.
	 - The iteration count.
	 
	 @return Returns CDecPKCS12MacData object pointer if the MacData is present in the PFX Structure
	         Returns NULL pointer if the MacData is absent in the PFX Structure.
	         Returned pointer ownership retains with the object.
	 */
	IMPORT_C const CDecPkcs12MacData* MacData() const;
	
	/**
	 These objects represents the ContentInfo Sequences present in the
	 AuthenticatedSafe Sequence. 
	 
	 @return An array of ContentInfo objects
	 @see	 CPKCS7ContentInfo
	 */
	IMPORT_C const RPointerArray<CPKCS7ContentInfo>& AuthenticatedSafeContents() const;
	
	/**
	 Destructor.
	 */
	 virtual ~CDecPkcs12();
	
private:
    /**
     This decodes the entire PFX structure
     
	 Main PKCS12 Structure.
	 PFX ::= SEQUENCE 
	  	{
	  	version		INTEGER {v3(3)}(v3,...),
	  	authSafe	ContentInfo,
	  	macData    	MacData OPTIONAL
	  	}
     
     @param aRawData Contains a PKCS#12 PFX Structure.
     @leave KErrArgument if the aRawData is not a Valid Pkcs12 PFX Structure.
     @see	CPKCS7ContentInfo, CDecPkcs12MacData, CPKCS7SignedObject.
     */
	void ConstructL(const TDesC8& aRawData);
	
	/**
	 This method is used to internalise that object and takes a reference 
	 to an RReadStream as the interface to the read stream. 
	 @param aStream 	 Contains a PKCS#12 PFX Structure.
	 @leave KErrArgument if the aStream is not Pkcs12 PFX Structure.
	 */
    void InternalizeL(RReadStream& aStream);
	
	/**
	 Constructor.
	 */
	 CDecPkcs12();
	
	/**
	 Construtor.
	 */
	 CDecPkcs12(RPointerArray<CPKCS7ContentInfo> aContentInfo);
	  
	/**
	 Copy Constructor.
	 @param aDecPkcs12 A CDecPkcs12 object
	 */
	 CDecPkcs12(const CDecPkcs12& aDecPkcs12);
	 
	 /**
	  Assignment operator.
	  @param aDecPkcs12 A CDecPkcs12 object.
	  @return A reference to CDecPkcs12 class.
	  */
	 CDecPkcs12& operator=(const CDecPkcs12& aDecPkcs12);

private:
	/** PKCS12 PFX Structure Version number */
	TInt iVersion;
	
	/** Integrity Mode used in PKCS12 PFX Structure*/	
	TIntegrityMode iMode;
	
	/** Contains the macData structure present 
	    in the PKCS12 PFX Structure*/
    CDecPkcs12MacData* iMacData;
    
    /** This contains the entire AuthenticatedSafe Data 
        present in the PKCS12 PFX Structure*/
    CPKCS7ContentInfo* iAuthenticatedSafeData;
    
    /** This Contains an Array of ContentInfos present 
        within the AuthenticatedSafe of PKCS12 PFX Structure*/
    RPointerArray<CPKCS7ContentInfo> iContentInfos;
  };
} // namespace PKCS12 

#endif // __PKCS12_H__
