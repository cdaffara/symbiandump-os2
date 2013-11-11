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
* This file contains the implementation of the CPkcs12Parser class, to parse the PKCS#12 file.
*
*/


/**
 @file 
 @internalTechnology
*/

// System Include
#include <f32file.h>

//User Include 
#include "tpkcs12common.h"
using namespace PKCS12;
/**
Description:constructor
@internalTechnology:
@test
*/
CPkcs12Parser::CPkcs12Parser()
	{
	}
/**
Description:destructor
@internalTechnology:
@test
*/
CPkcs12Parser::~CPkcs12Parser()
	{
	delete iIntegrityPassword;	
	delete iRawData;	
	delete iPkcs12Header;
	iArraySafecontentBag.ResetAndDestroy();
	iArrayBagData.ResetAndDestroy();
	}

/**
Description:Function is intended to creates a new CPKCS12Parser object.	 
@return:CPKCS12Parser* : parser object to parse the PKCS12 file
@internalTechnology
@test
*/
CPkcs12Parser* CPkcs12Parser::NewL()
	{	
	CPkcs12Parser* parser = NewLC();
	CleanupStack::Pop(parser);	
	return parser;
	}
/**
Description:Function is intended to creates a new CPKCS12Parser object.	 
@return:CPKCS12Parser* : parser object to parse the PKCS12 file
@test
*/
CPkcs12Parser* CPkcs12Parser::NewLC()
	{	
	CPkcs12Parser* parser = new (ELeave) CPkcs12Parser();
	CleanupStack::PushL(parser);	
	return parser;
	}
/**
Description:Function is intended to set the integrity password 
@param	aIntegrityPassword, contains the integrity password
@internalTechnology
@test
*/
void CPkcs12Parser::SetIntegrityPasswordL(const TDesC& aIntegrityPassword)
	{
	iIntegrityPassword = aIntegrityPassword.AllocL();
	}

/**
Description:Function is intended to set the privacy password(s) 
@param aPrivacyPassword, contains  the privacy passwords.The ownership is not transferred
@internalTechnology
@test
*/
void CPkcs12Parser::SetPrivacyPassword(RPointerArray<TDesC>& aPrivacyPassword)
	{
	iPrivacyPassword = 	&aPrivacyPassword;
	}
/**
Description:Function is intended to set the test data. 
			It opens the data file and stores it in the buffer.
@param aDatapath, contains the path of the PKCS12 file
@internalTechnology
@test
*/
void CPkcs12Parser::SetDataL(const TDesC& aDatapath)
	{ 	
	if(iRawData)
		{
		delete iRawData ;
		iRawData  = NULL;
		}
	RFs fs;
	//connects the file server
	User::LeaveIfError (fs.Connect());
	//opens the data file
	CleanupClosePushL(fs);
	RFile file;
	User::LeaveIfError(file.Open(fs, aDatapath, EFileRead)) ; 
	CleanupClosePushL(file);
	
	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));	

	iRawData = HBufC8::NewL(fileSize);
	TPtr8 rawData(iRawData->Des());
	rawData.SetLength(fileSize);	
	file.Read (rawData);

	CleanupStack::PopAndDestroy(&file);
	//closes the file session
	CleanupStack::PopAndDestroy(&fs);
	}
/**
Description:Function is intended to parse the PKCS#12 data
@leave:- if the number of contentinfos and number of privacy passwords does not match.	
@internalTechnology
@test
*/
void CPkcs12Parser::ParseL()
	{
	//creates the CDecPkcs12 object		
	CDecPkcs12* decPkcs12 = NULL;
	TRAPD(error, decPkcs12	= CDecPkcs12::NewL(*iRawData));
	if(error == KErrNone)
		CleanupStack::PushL(decPkcs12);	

	//Creates a CPfxHeader object
	iPkcs12Header = CPfxHeader::NewL(*decPkcs12 , error);	
	if(error != KErrNone)
		{
		
		return ;	
		}
	const CDecPkcs12MacData* macData = decPkcs12->MacData();
	TRAPD(err,macData->VerifyIntegrityL(iIntegrityPassword->Des()));
	if(err)
		{
		iPkcs12Header->SetPkcs12ActualError(err);
		CleanupStack::PopAndDestroy(decPkcs12);
		return;
		}
	//get the ContentInfo collection
	const RPointerArray<CPKCS7ContentInfo>& contentInfos = decPkcs12->AuthenticatedSafeContents();	
	TInt contentInfoCount = contentInfos.Count();
	//put the content info count in the Header
	iPkcs12Header->SetContentInfoCount(contentInfoCount) ;

	// start iterate through the contentinfos
	for(TInt index = 0 ; index < contentInfoCount; index++)
		{
		TInt contentType = contentInfos[index]->ContentType() ; 
		TPtrC8 contentData = contentInfos[index]->ContentData();
		if( contentData == KNullDesC8()) 
			{
			iPkcs12Header->SetPkcs12ActualError(0);
			continue;
			}
		CDecPkcs12SafeContents* pkcs12SafeContents = NULL;			
		switch(contentType)
			{
			case CPKCS7ContentInfo::EContentTypeData:
				{
				TRAPD(error1,pkcs12SafeContents = CDecPkcs12SafeContents::NewL(*contentInfos[index]));	
				if(error1)
					{
					iPkcs12Header->SetPkcs12ActualError(error1);
					CSafeBagData* bagData = NULL; 
					bagData = CreateBagDataL(index,contentType);
					iArrayBagData.AppendL(bagData) ;
					CleanupStack::PopAndDestroy(decPkcs12);
					return;
					}	
				CleanupStack::PushL(pkcs12SafeContents);			
				break;
				}
			case CPKCS7ContentInfo::EContentTypeEncryptedData:
				{
				if(contentInfoCount != iPrivacyPassword->Count())	
					{
					User::Leave(KErrArgument);
					}
						
				TRAPD(error2,pkcs12SafeContents = CDecPkcs12SafeContents::NewL(*contentInfos[index],
																	*((*iPrivacyPassword)[index]) ) );
				if(error2)
					{
					iPkcs12Header->SetPkcs12ActualError(error2);
					CSafeBagData* bagData = NULL; 
					bagData = CreateBagDataL(index,contentType);
					iArrayBagData.AppendL(bagData) ;
					CleanupStack::PopAndDestroy(decPkcs12);
					return;
					}							
				CleanupStack::PushL(pkcs12SafeContents);	
				break;							
				}
			case CPKCS7ContentInfo::EContentTypeEnvelopedData:
				{
				//creates a Bagdata object in the heap to store the content type. 
				CSafeBagData* bagData = CreateBagDataL(index+1 ,contentType);
				iArrayBagData.AppendL(bagData) ;
				//continue in the loop
				continue;
				}

			default:
				{	
				//continue in the loop
				continue;
				}
			}
		// Get the safebags count.
		const RPointerArray<CDecPkcs12SafeBag>& safeBags = pkcs12SafeContents->SafeContentsBags();
		TInt safeBagCount = safeBags.Count();
	
		
		//start parsing the bags
		for (TInt bagsCount=0; bagsCount < safeBagCount; bagsCount++)
			{	
			TRAPD(error3,ParseSafeBagL(index+1, contentType , *safeBags[bagsCount]));
			if(error3 != KErrNone)
				{
				iPkcs12Header->SetPkcs12ActualError(error3);
				CSafeBagData* bagData = NULL; 
				bagData = CreateBagDataL(index,contentType);
				iArrayBagData.AppendL(bagData) ;
				CleanupStack::PopAndDestroy(2,decPkcs12);
				return;
				}	
			}	
			CleanupStack::PopAndDestroy(pkcs12SafeContents);
		}		
	CleanupStack::PopAndDestroy(decPkcs12);	

	}

/**
Description: Creates a CSafeBagData to store the safebag details
@param:aContentInfo, the contentinfo number of the safe bag 
@param:aContentType, the content type of the safe contents.
@return:a pointer to CSafeBagData, which is used to store the safebag details 
@internalTechnology:
@test:
*/
CSafeBagData* CPkcs12Parser::CreateBagDataL(TInt aContentInfo, TInt aContentType)
	{
	CSafeBagData* bagData = CSafeBagData::NewL();	
	if ( iPkcs12Header->Pkcs12ActualError() == KErrNone )
		{
		//set the contentinfo # and content type in CSafeBagData
		bagData->SetContentInfoNumber(aContentInfo);	
		bagData->SetBagNumber(++iSafebagNumber );	
		// set the content type in CSafeBagData		
		bagData->SetContentInfoType(aContentType);
		}
	return bagData ;
	}

/**
Description:To parse the safebags
@param:aContentInfoIndex	content info number
@param:aContentType	contentinfo type
@param:aSafeBag	reference to the CDecPkcs12SafeBag 
@internalTechnology:
@test:
*/
void CPkcs12Parser::ParseSafeBagL(TInt aContentInfo,TInt aContentType,CDecPkcs12SafeBag& aSafeBag)
	{
	//get the bag type 
	CDecPkcs12SafeBag::TBagId bagType = aSafeBag.BagID();
	switch(bagType)
		{
		case CDecPkcs12SafeBag::ESafeContentsBag:
			{
			CDecPkcs12SafeContentsBag* safeContentsBag = static_cast<CDecPkcs12SafeContentsBag *>(&aSafeBag);
			// increment the count
			iPkcs12Header->IncrementSafecontentBagCount();
			// parsing code to parse SafeContentsBag		
			ParseSafeContentBagL(*safeContentsBag, aContentInfo ,aContentType );
			break;
			}
		case CDecPkcs12SafeBag::EKeyBag:
			{
			CDecPkcs12KeyBag* keyBag = static_cast<CDecPkcs12KeyBag *>(&aSafeBag);					
			//create the CSafeBagData object in heap to store the bag details
			CSafeBagData* bagData = CreateBagDataL(aContentInfo,aContentType);
			CleanupStack::PushL(bagData);			
			ParseKeyBag(*keyBag , *bagData);
			//get the safeBag attributes
			BagAttributesL(*keyBag ,*bagData) ;	
			iArrayBagData.AppendL(bagData) ;
			CleanupStack::Pop(bagData);					
			break;
			}
		case CDecPkcs12SafeBag::EShroudedKeyBag:
			{
			CDecPkcs12ShroudedKeyBag* shroudedkeyBag = static_cast<CDecPkcs12ShroudedKeyBag *>(&aSafeBag);
			//create the CSafeBagData object in heap to store the bag details
			CSafeBagData* bagData = CreateBagDataL(aContentInfo,aContentType );
			CleanupStack::PushL(bagData);			
			ParseShroudedKeyBagL(*shroudedkeyBag , *bagData ,aContentInfo );		
			//get the safeBag attributes
			BagAttributesL(*shroudedkeyBag ,*bagData) ;	
			iArrayBagData.AppendL(bagData) ;	
			CleanupStack::Pop(bagData);			
			break;
			}
		case CDecPkcs12SafeBag::ECertBag:
			{
			CDecPkcs12CertBag* certBag = static_cast<CDecPkcs12CertBag *>(&aSafeBag);					
			//create the CSafeBagData object in heap to store the bag details
			CSafeBagData* bagData = CreateBagDataL(aContentInfo,aContentType);
			CleanupStack::PushL(bagData);		
			ParseCertBag(*certBag , *bagData);
			// Get the bag Attributes
			BagAttributesL(*certBag , *bagData) ;	
			iArrayBagData.AppendL(bagData) ;
			CleanupStack::Pop(bagData);			
			break;
			}
		case CDecPkcs12SafeBag::ECrlBag:
			{
			//create the CSafeBagData object in heap to store the bag details
			CSafeBagData* bagData = CreateBagDataL(aContentInfo,aContentType);;
			CleanupStack::PushL(bagData);				
			//set the bagId in CSafeBagData
			bagData->SetBagType(bagType) ; 
			//increment the count - unsupported
			iPkcs12Header->IncrementCrlBagCount();
			iArrayBagData.AppendL(bagData) ;
			CleanupStack::Pop(bagData);				
			break;
			}
		case CDecPkcs12SafeBag::ESecretBag:
			{
			//create the CSafeBagData object in heap to store the bag details
			CSafeBagData* bagData = CreateBagDataL(aContentInfo,aContentType );	
			CleanupStack::PushL(bagData);
			//set the iBagId in CSafeBagData
			bagData->SetBagType(bagType) ; 
			//increment the count - unsupported
			iPkcs12Header->IncrementSecretBagCount();
			iArrayBagData.AppendL(bagData) ;		
			CleanupStack::Pop(bagData);	
			break;
			}
		default:
			{
			}
		}
	}

/**
Description:Function is intended to get the attributes of Safebag
@param-aSafeBag: reference to the CDecPkcs12SafeBag.
@param-abagData: reference to the CSafeBagData
@internalTechnology:
@test
*/
void CPkcs12Parser::BagAttributesL(const CDecPkcs12SafeBag& aSafeBag,CSafeBagData& aBagData )
	{
	const RPointerArray<CDecPkcs12Attribute>& safeBagAttributes = aSafeBag.BagAttributes();

	TInt attributeCount = safeBagAttributes.Count() ;
	for (TInt index = 0;index < attributeCount; index++ )
		{						
		//get the attribute object pointer
		CDecPkcs12Attribute* attribute = safeBagAttributes[index];
		//create the CSafeBagAttribute object in heap.	
		CSafeBagAttribute* bagAttr = CSafeBagAttribute::NewL(*safeBagAttributes[index]);
		CleanupStack::PushL(bagAttr);
		//get the attribute values
		const RPointerArray<TDesC8>& attrValues = attribute->AttributeValue();			
		//append the pointer in the iAttributeIDs array
		aBagData.iAttributeIDs.AppendL(bagAttr) ;
		CleanupStack::Pop(bagAttr);	
		//store the attribute values
		TInt numberOfAttributes = bagAttr->AttributeValueCount();
		for(TInt attrvalCnt = 0 ; attrvalCnt < numberOfAttributes ; attrvalCnt++)
			{
			//get the atribute value and store the it address in the aBagData.iAttributeValues array				
			HBufC8* ptr =  attrValues[attrvalCnt]->AllocLC();								
			aBagData.iAttributeValues.AppendL(ptr);
			CleanupStack::Pop(ptr);
			}			
		}	
	}
/**
Description:Function is intended to parse the  keybag
@param-aKeyBag: reference to the keybag
@param-abagData: reference to the CSafeBagData
@internalTechnology:
@test:
*/
void CPkcs12Parser::ParseKeyBag(CDecPkcs12KeyBag& aKeyBag , CSafeBagData& aBagData)
	{
	//set the bagID
	aBagData.SetBagType( CDecPkcs12SafeBag::EKeyBag ); 				
	// Get the Algorithm(RSA key or DSA key) ID 
	CDecPKCS8Data* pkcs8Data = aKeyBag.PrivateKeyInfoL();				
	aBagData.SetPrivateKeyInfoVersion(pkcs8Data->Version() );
	TAlgorithmId algorithmId = pkcs8Data->Algorithm();
	// Set the Algorithm Id in CSafeBagData
	aBagData.SetKeyType( algorithmId );
	//set the bag value in CSafeBagData
	aBagData.SetBagValue( aKeyBag.BagValue());				
	// increment the count
	iPkcs12Header->IncrementKeyBagCount();
	delete pkcs8Data;
	}
/**
Description:Function is intended to parse the  ShroudedKeyBag
@param-aShroudedkeyBag: pointer to the CDecPkcs12ShroudedKeyBag
@param-abagData: reference to the CSafeBagData
@param- aPassIndex: the index of the array from where the privacy password is to be taken
@internalTechnology:
@test
*/
void CPkcs12Parser::ParseShroudedKeyBagL(CDecPkcs12ShroudedKeyBag& aShroudedkeyBag , CSafeBagData& aBagData , TInt aPassIndex)
	{
	//set the bag Id in CSafeBagData
	aBagData.SetBagType( CDecPkcs12SafeBag::EShroudedKeyBag ); 				
	// Get the Algorithm(RSA key or DSA key) ID 
	CDecPKCS8Data* pkcs8Data = aShroudedkeyBag.PrivateKeyInfoL(*(*iPrivacyPassword)[aPassIndex-1]);
	if(pkcs8Data)
		{
		aBagData.SetPrivateKeyInfoVersion(pkcs8Data->Version() );	
		TAlgorithmId algorithm  = pkcs8Data->Algorithm();	
		// Set the Algorithm Id in CSafeBagData
		aBagData.SetKeyType(algorithm);
	
		CASN1EncSequence* seq = NULL ;
		switch(algorithm)
			{
			case ERSA:
				{	
				
				CPKCS8KeyPairRSA* keypair = static_cast<CPKCS8KeyPairRSA*>(pkcs8Data->KeyPairData());

				const CRSAPrivateKey& priv = keypair->PrivateKey();
				
				const CRSAPublicKey& publicKey = keypair->PublicKey(); 
				
				TRSAPrivateKeyType keytype = priv.PrivateKeyType();
				
				switch(keytype)
					{
					case EStandardCRT:
						{
						const CRSAPrivateKeyCRT* privateKeyCRT = static_cast<const CRSAPrivateKeyCRT*>(&priv);
						seq =  TASN1EncPKCS8::EncodeL(*privateKeyCRT, publicKey,pkcs8Data->PKCS8Attributes());	
					
						break;
						}					
					case EStandard:		
						{
						User::Leave(KErrNotSupported);
						}
					default:
						{
						User::Leave(KErrNotSupported);	
						}
					}
					
				break;
				}		
			case EDSA:
				{
				
				CPKCS8KeyPairDSA* keypair = static_cast<CPKCS8KeyPairDSA*>(pkcs8Data->KeyPairData());
			
				const CDSAPrivateKey& priv = keypair->PrivateKey();
				
				seq =  TASN1EncPKCS8::EncodeL(priv , pkcs8Data->PKCS8Attributes());
			
				break;
				}			
			default:
				User::Leave(KErrNotSupported);

			}
	
		HBufC8* bufSeq = HBufC8::NewMaxLC(seq->LengthDER());
		TPtr8 temp(bufSeq->Des());

		TUint pos = 0;
		seq->WriteDERL(temp ,pos);
			
		aBagData.SetEncodedShroudedKey(temp);
		CleanupStack::PopAndDestroy();

		delete pkcs8Data;
		}
		//set the bag value in CSafeBagData
		aBagData.SetBagValue(aShroudedkeyBag.BagValue()) ;				
		// increment the count 
		iPkcs12Header->IncrementShroudedKeyBagCount();											
	}

/**
Description:Function is intended to parse the  certbag
@param-aCertBag: pointer to the CDecPkcs12CertBag
@param-abagData: reference to the CSafeBagData
@internalTechnology:
@test
*/
void CPkcs12Parser::ParseCertBag(CDecPkcs12CertBag& aCertBag , CSafeBagData& aBagData)
	{
	//set the bagID 
	aBagData.SetBagType( CDecPkcs12SafeBag::ECertBag );	
	aBagData.SetCertificateId(aCertBag.CertId()); 		
	//set the bag value in CSafeBagData
	aBagData.SetBagValue(aCertBag.CertValue());
	
	//set the X509Certificate
	aBagData.SetX509Certificate((aCertBag.X509CertificateL()) );
	// increment the count 
	iPkcs12Header->IncrementCertBagCount();			 
	}

/**
Description:Function is intended to parse the  safecontentbag
@param-safeContentsBag: reference to the CDecPkcs12SafeContentsBag
@param-aContentinfo: contentinfo number
@param- aContentType : contentinfo type
@internalTechnology
@test
*/
void CPkcs12Parser::ParseSafeContentBagL(CDecPkcs12SafeContentsBag& aSafeContntBag, TInt aContentinfo , TInt aContentType )
	{	
	const RPointerArray<CDecPkcs12SafeBag>& safebags = aSafeContntBag.SafeBags();  
	// Get the safe bag count
	TInt safeBagCount = safebags.Count();
	//Create a CSafeContentBag , store the bag number and the number of bags in it,append it in the array
	CSafeContentBag* safecontentbag = CSafeContentBag::NewL() ;	
	if(safecontentbag)
		{
		safecontentbag->SetBagNumber(iPkcs12Header->SafecontentBagCount()) ; 
		safecontentbag->SetSafeBagCount(safeBagCount) ;
		CleanupStack::PushL(&iArraySafecontentBag);
		iArraySafecontentBag.AppendL(safecontentbag) ;
		CleanupStack::Pop(&iArraySafecontentBag);
		}
	if(safeBagCount>0) 
		{				
		TInt loopindex = 0 ;
		while(loopindex < safeBagCount )
			{
			ParseSafeBagL(aContentinfo ,aContentType, *(safebags[loopindex]));
			loopindex++;
			}//end while
		}
	}

