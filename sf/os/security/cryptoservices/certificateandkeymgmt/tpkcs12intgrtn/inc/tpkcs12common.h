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
* This file contains the declaration of classe CPkcs12Parser to parse the PKCS12 data
*
*/


/**
 @file 
 @internalTechnology
*/
#ifndef __TPKCS12COMMON_H
#define __TPKCS12COMMON_H

#include <pkcs12.h> 

using namespace PKCS12;
//System Include
#include <e32base.h>
#include <pkcs12bags.h>
#include <pkcs12attribute.h>
#include <pkcs12macdata.h>
#include <asn1enc.h> 
#include <asymmetrickeys.h>

//user include
#include "tpkcs12data.h"

/**
 Contains methods to parse the PFX structure. 
*/
class CPkcs12Parser : public CBase
	{
public:
	
	static CPkcs12Parser* NewL();
		 
	static CPkcs12Parser* NewLC();	 
	
	void SetIntegrityPasswordL(const TDesC& aIntegrityPassword);
 		
	void SetPrivacyPassword(RPointerArray<TDesC>& aPrivacyPassword);
			
	void SetDataL(const TDesC& aDatapath);
		
	void ParseL();
	/**  to get the reference to PfxHeader object*/
	inline CPfxHeader& PfxHeader()  {return *iPkcs12Header;}
	/**  to get the reference to array contains the pointers to CSafeContentBag objects*/
	inline RPointerArray<CSafeContentBag>& ArraySafeContentBag()  {return iArraySafecontentBag;}
	/**  to get the reference to array contains the pointers to CSafeBagData objects*/
	inline RPointerArray<CSafeBagData>& ArraySafeBagData()  {return iArrayBagData;}

	~CPkcs12Parser();	
private:
	
	CPkcs12Parser();

	CSafeBagData* CreateBagDataL(TInt aContentInfo, TInt aContentType );

	void ParseSafeBagL(TInt aContentInfo , TInt aContentType ,CDecPkcs12SafeBag& aSafeBag);

	void BagAttributesL(const CDecPkcs12SafeBag& aSafeBag  ,CSafeBagData& abagData) ;

	void ParseKeyBag(CDecPkcs12KeyBag& aKeyBag , CSafeBagData& abagData);

	void ParseShroudedKeyBagL(CDecPkcs12ShroudedKeyBag& aKeyBag , CSafeBagData& abagData , TInt aPassIndex);

	void ParseCertBag(CDecPkcs12CertBag& aCertBag , CSafeBagData& abagData);

	void ParseSafeContentBagL(CDecPkcs12SafeContentsBag& safeContentsBag, TInt aContentinfo , TInt aContentType ) ;


					
private:
	/** Contains the pointers  to the safes bags in the PKCS12 PFX Structure*/
	RPointerArray<CSafeBagData> iArrayBagData;	
	/** Contains the safe content bag number and the number of bags in it */
	RPointerArray<CSafeContentBag> iArraySafecontentBag ;
	/** Contains the details of the PKCS12 PFX Structure*/
	CPfxHeader* iPkcs12Header ;
	/**contains the integritypassword of the PKCS#12 file */	
	HBufC* iIntegrityPassword;
	/**contains the privacy passwords of the PKCS#12 file */	
	RPointerArray<TDesC>* iPrivacyPassword;
	/**contains the PKCS#12 data */	
	HBufC8*	iRawData;		
		
	/**store the safebag number */	
	TInt iSafebagNumber ; 

	};
	
#endif

