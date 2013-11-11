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



#include "tcmsstep.h"
#include <test/testexecutelog.h>
#include <asnpkcs.h>
#include "pkcs7signedobject.h"
#include <asn1enc.h>
#include <cmssignedobject.h>
#include <cmsdefs.h>
#include <hash.h>
#include <asymmetric.h>
#include <cmssigneridentifier.h>
#include <x509certext.h>
#include <pkixcertchain.h>
#include <cmscontentinfo.h>
#include "validate.h"


CTCmsBaseStep::CTCmsBaseStep()
	{
	}

CTCmsBaseStep::~CTCmsBaseStep()
	{
	iFs.Close ();
	delete iDataContent;
	delete iExpectedEncoding;
	__UHEAP_MARKEND;
	}

TVerdict CTCmsBaseStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	User::LeaveIfError (iFs.Connect());	
	
	//Read the data to be signed
	iDataContent = readFileL(_L("Data"));
	
	if (iDataContent == NULL)
		{
		iDataContent=KNullDesC8().AllocL();
		}
	
	//Read the expected data type
	TPtrC contentDataType;
	if (GetStringFromConfig(ConfigSection(), _L("ExpectedDataType"), contentDataType))
		{
		iExpectedDataType=CovertContentDataTypeNameToDataType(contentDataType);			
		}

	//Read the expected result
	iExpectedEncoding = readFileL(_L("Result"));
	if (!iExpectedEncoding)
		{
		INFO_PRINTF1(_L("Failed to read 'Result' section of script"));
		SetTestStepResult(ETestSuiteError);			
		}

	GetIntFromConfig(ConfigSection(), _L("ExpectedResult"), iExpectedResult);	
	GetBoolFromConfig(ConfigSection(), _L("IsOOMTest"), iIsOOMTest);
	return TestStepResult();	
	}

TInt CTCmsBaseStep::CovertContentDataTypeNameToDataType(const TDesC& aDataTypeName)
	{
	if (aDataTypeName.Compare(_L("DATA"))==0)
		{
		return EContentTypeData;	
		}
	else	if (aDataTypeName.Compare(_L("SIGNEDDATA"))==0)
				{
				return EContentTypeSignedData;
				}
		 else	if (aDataTypeName.Compare(_L("ENVELOPEDDATA"))==0)
					 {
					 return EContentTypeEnvelopedData;	
					 }
			  else	if (aDataTypeName.Compare(_L("DIGESTEDDATA"))==0)
						{
						return EContentTypeDigestedData;
						}
					else 	if (aDataTypeName.Compare(_L("ENCRYPTEDDATA"))==0)
								{
								return EContentTypeEncryptedData;
								}
							else 	if (aDataTypeName.Compare(_L("SIGNEDANDENVELOPEDDATA"))==0)
										{
										return CPKCS7ContentInfo::EContentTypeSignedAndEnvelopedData;	
										}
									else	if (aDataTypeName.Compare(_L("AUTHDATA"))==0)
												{
												return EContentTypeAuthenticatedData;	
												}
											else
												{
												User::Leave(KErrArgument);
												return EContentTypeData;
												}
	}

HBufC8* CTCmsBaseStep::readFileL (TPtrC tag)
	{
	TPtrC fileName;
	if (GetStringFromConfig(ConfigSection(), tag, fileName) == EFalse)
		{
		return NULL;
		}

	RFile file;
	if (file.Open(iFs, fileName, EFileRead) != KErrNone)
		{
		INFO_PRINTF2(_L("Cannot open file %S for reading"), &fileName);
		return NULL;
		}
	CleanupClosePushL(file);
	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));
	HBufC8* result = HBufC8::NewMaxL(fileSize);
	TPtr8 rawDataPtr(result->Des());
	file.Read (rawDataPtr);
	CleanupStack::PopAndDestroy (&file);
	INFO_PRINTF3(_L("Read %d octets from %S"), result->Size(), &fileName);
	return result;
	}

void CTCmsBaseStep::OutputResultToFileL(const TDesC8& aSignature)
	{
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> rName (sysDrive.Name());;
	rName.Append(_L("\\tpkcs7\\myresults\\"));
			
	TInt err=iFs.MkDir(rName);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		{
		User::Leave(err);	
		}
				
	RFile file;
	CleanupClosePushL(file);
	
	_LIT(KExtension, ".der");
	rName.Append(ConfigSection());
	rName.Append(KExtension);
	rName.LowerCase();
	User::LeaveIfError(file.Replace(iFs, rName, EFileWrite | EFileStream));
	User::LeaveIfError(file.Write(aSignature));
	CleanupStack::PopAndDestroy(&file);
	}


HBufC8* CTCmsBaseStep::CreateDEREncodingLC(const CASN1EncBase& aEncoding)
	{	
	TUint len = aEncoding.LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(len);
	TUint pos = 0;
	TPtr8 bufptr(buf->Des());
	aEncoding.WriteDERL(bufptr, pos);
	return buf;
	}

TVerdict CTCmsBaseStep::doTestStepL()
	{
	if (!iIsOOMTest)
		{
		TRAPD(err, doTestL();)
		if (err!=iExpectedResult)
			{
			SetTestStepResult(EFail);
			User::Leave(err);
			}
		return TestStepResult();
		}
	else
		{
		return doOOMTestL();	
		}	
	}

TVerdict CTCmsBaseStep::doOOMTestL()
	{
	TVerdict verdict = EFail;
 	TInt countAfter = 0;
	TInt countBefore = 0;
 	for (TInt oomCount = 0; ; oomCount++)
 		{
 		__UHEAP_RESET;
 		__UHEAP_SETFAIL(RHeap::EDeterministic, oomCount);
 		countBefore = User::CountAllocCells();
 		TRAPD(error, doTestL());
 		countAfter = User::CountAllocCells();
 		__UHEAP_RESET;
 		if (error != KErrNoMemory)
 			{
 			verdict = EPass;
 			INFO_PRINTF2(_L("OOM Status %d"),error);
			INFO_PRINTF1(_L("Test outcome : Passed"));
 			break;
 			}
 		else
 			{
 			if (countBefore != countAfter)
 				{
 				INFO_PRINTF2(_L("OOM Status %d"),error);
 				INFO_PRINTF2(_L("OOM Failed at %d"), oomCount);
 				SetTestStepResult(EFail);
 				break;
 				}
 			}
 		INFO_PRINTF2(_L("OOM Failed Point status %d"), error);
		}
	INFO_PRINTF3(_L("Heap alloc count ok: %d final vs %d initial"), countAfter,countBefore);
 	SetTestStepResult(verdict);
 	if (verdict==EFail)
	 	{
 		User::Leave(KErrGeneral);	 		
	 	}	 	
 	return verdict;
	}


//
// Implementation of CMS Data Test Step
//
CTCmsDataStep::CTCmsDataStep()
	{
	}

CTCmsDataStep::~CTCmsDataStep()
	{
	}

	
void CTCmsDataStep::doTestL()
	{
	__UHEAP_MARK;
	CCmsContentInfo* content=CCmsContentInfo::NewL(EContentTypeData, *iDataContent);
	CleanupStack::PushL(content);
	CASN1EncSequence* contentSeq=content->EncodeASN1DERLC();
	HBufC8* signature=CreateDEREncodingLC(*contentSeq);
	CleanupStack::Pop(signature);
	CleanupStack::PopAndDestroy(2, content);
	CleanupStack::PushL(signature);
	//OutputResultToFileL(signature->Des());

	TBool r=signature->Compare(*iExpectedEncoding);
	if (r!=0 && !iIsOOMTest)
		{
		INFO_PRINTF1(_L("CMS Data Type Encoding Error"));
		User::Leave(KErrGeneral);
		}
	else
		{
		DecodingAndCheckL(*iExpectedEncoding);
		}

	CleanupStack::PopAndDestroy(signature);	
	}
void CTCmsDataStep::DecodingAndCheckL(TDesC8& aRawData)
	{
	INFO_PRINTF1(_L("Start CMS Data Type Decoding"));
	CCmsContentInfo* content=CCmsContentInfo::NewL(aRawData);
	CleanupStack::PushL(content);
	if (content->ContentType()!=EContentTypeData)
		{
		INFO_PRINTF1(_L("CMS Data Type is not as expected"));
		User::Leave(KErrGeneral);
		}
	else
		{
		if (content->ContentData()!=iDataContent->Des())
			{
			INFO_PRINTF1(_L("CMS Data Content is not as expected"));
			User::Leave(KErrGeneral);					
			}
		}
	CleanupStack::PopAndDestroy(content);		
	}
//
// Implementation of CMS Data Test Step
//
CTCmsDataDecodingStep::CTCmsDataDecodingStep()
	{
	}

CTCmsDataDecodingStep::~CTCmsDataDecodingStep()
	{
	}

void CTCmsDataDecodingStep::doTestL()
	{
	DecodingAndCheckL(*iExpectedEncoding);	
	}


//
// Implementation of Signed Data Test Base Step
//
CTSignedDataBaseStep::CTSignedDataBaseStep() : iContentType(EContentTypeData), iRsaAlgorithm(ETrue) 
	{
	}

CTSignedDataBaseStep::~CTSignedDataBaseStep()
	{
	iDecPKCS8Data.ResetAndDestroy();
	iCertificates.ResetAndDestroy();
	iSignerInfoVersion.Close();
	iHashName.Close();
	iValidateResults.Close();
	iSignedAttributePresent.Close();
	iUnSignedAttributePresent.Close();
	delete iRootCertificate;
	delete iAdditionalCertificate;
	delete iAdditionalEncodedCertificate;
	}


TInt CTSignedDataBaseStep::CovertHashNameToAlgorithmId(const TDesC& aHashName)
	{
	if (aHashName.Compare(_L("SHA1"))==0)
		{
		return ESHA1;	
		}
	else if (aHashName.Compare(_L("MD5"))==0)
			{
			return EMD5;
			}
		 else
			 {
			 return EMD2;	
			 }
	}

TInt CTSignedDataBaseStep::CovertCertificateNameToCertificateType(const TDesC& aCertificateName)
	{
	if (aCertificateName.Compare(_L("X509"))==0)
		{
		return CCmsCertificateChoice::ECertificateX509;	
		}
	else if (aCertificateName.Compare(_L("Attribute"))==0)
			{
			return CCmsCertificateChoice::ECertificateAttribute;
			}
		 else
			 {
			 return CCmsCertificateChoice::ECertificateExtendedCerificate;	
			 }
	}


TVerdict CTSignedDataBaseStep::doTestStepPreambleL()
	{
	if (CTCmsBaseStep::doTestStepPreambleL()==EFail)
		{
		SetTestStepResult(EFail);
		}
	else
		{
		//Read the configurations
		GetBoolFromConfig(ConfigSection(), _L("HashAvailable"), iIsHashAvailable);
		GetBoolFromConfig(ConfigSection(), _L("DataDetached"), iIsDetached);
		GetBoolFromConfig(ConfigSection(), _L("CertificateSetPresent"), iCertificateSetPresent);
		GetBoolFromConfig(ConfigSection(), _L("CRLsSetPresent"), iCRLsSetPresent);
		GetIntFromConfig(ConfigSection(), _L("SignedDataVersion"), iSignedDataVersion);
		GetIntFromConfig(ConfigSection(), _L("AlgorithmCount"), iAlgorithmCount);
		GetIntFromConfig(ConfigSection(), _L("CertsCount"), iCertsCount);
		GetIntFromConfig(ConfigSection(), _L("SignerCount"), iSignerCount);
		GetBoolFromConfig(ConfigSection(), _L("NoCertSet"), iNoCertSet);
		GetBoolFromConfig(ConfigSection(), _L("ValidateUsingUserCerts"), iValidateUsingUserCerts);
		GetBoolFromConfig(ConfigSection(), _L("NoSigning"), iNoSigning);
		GetBoolFromConfig(ConfigSection(), _L("NoValidationTest"), iNoValidationTest);
		GetBoolFromConfig(ConfigSection(), _L("TwoStepCreation"), iTwoStepCreation);
		GetBoolFromConfig(ConfigSection(), _L("ValidationDetachedWithoutInput"), iValidationDetachedWithoutInput);
		

		HBufC8* certificate = readFileL(_L("RootCertificate"));
		if (certificate)
			{
			CleanupStack::PushL(certificate);
			iRootCertificate = CX509Certificate::NewL(*certificate);
			CleanupStack::PopAndDestroy (certificate);
			}

		certificate = readFileL(_L("AddtionalCertificate"));
		if (certificate)
			{
			CleanupStack::PushL(certificate);
			iAdditionalCertificate = CX509Certificate::NewL(*certificate);
			CleanupStack::PopAndDestroy (certificate);			
			}
		TPtrC certTypeName;
		if (GetStringFromConfig(ConfigSection(), _L("AdditionalEncodedCertificateType"), certTypeName))
			{
			iAdditionalEncodedCertificateType=CovertCertificateNameToCertificateType(certTypeName);
			iAdditionalEncodedCertificate=readFileL (_L("AdditionalEncodedCertificate"));				
			}
					
		//Read the certificates, private keys and hash algorithm
		TInt index(0);
		
		TName fKeyName;
		fKeyName.Format(_L("PrivateKey_%d"), index);
		
		TName fCertName;
		fCertName.Format(_L("Certificate_%d"), index);
		
		TName fHashAlgorithmName;
		fHashAlgorithmName.Format(_L("HashAlgorithm_%d"), index);

		TName fValidationResult;
		fValidationResult.Format(_L("ValidationResult_%d"), index);
		
		TName fSignedAttributePresent;
		fSignedAttributePresent.Format(_L("SignedAttributePresent_%d"), index);
		
		TName fUnSignedAttributePresent;
		fUnSignedAttributePresent.Format(_L("UnSignedAttributePresent_%d"), index);

		TName fSignerInfoVersion;
		fSignerInfoVersion.Format(_L("SignerInfoVersion_%d"), index);

		TPtrC hashName;
		TBool vResult(EFalse);
		TBool sAP(EFalse);
		TBool uSAP(EFalse);
		TInt signerInfoVersion;
		TPtrC keyName;
		TPtrC certName;
		
		while ( GetStringFromConfig(ConfigSection(), fKeyName, keyName)
				&& GetStringFromConfig(ConfigSection(), fCertName, certName)
				&& GetStringFromConfig(ConfigSection(), fHashAlgorithmName, hashName)
				&& GetBoolFromConfig(ConfigSection(), fValidationResult, vResult)
				&& GetBoolFromConfig(ConfigSection(), fSignedAttributePresent, sAP)
				&& GetBoolFromConfig(ConfigSection(), fUnSignedAttributePresent, uSAP)
				&& GetIntFromConfig(ConfigSection(), fSignerInfoVersion, signerInfoVersion) )
			{
			//Construct private keys
			HBufC8* privateKey(NULL);
			if ((privateKey=readFileL(fKeyName))!=NULL)
				{
				CleanupStack::PushL (privateKey);
				CDecPKCS8Data* pkcs8Data=TASN1DecPKCS8::DecodeDERL(privateKey->Des());
				CleanupStack::PushL (pkcs8Data);
				iDecPKCS8Data.AppendL(pkcs8Data);
				CleanupStack::Pop(pkcs8Data);
				CleanupStack::PopAndDestroy(privateKey);	
				}
			
			//Construct X509 certificate
			HBufC8* cert(NULL);
			if ((cert=readFileL(fCertName))!=NULL)
				{
				CleanupStack::PushL (cert);
				CX509Certificate* x509cert=CX509Certificate::NewLC(cert->Des());		
				iCertificates.AppendL(x509cert);
				CleanupStack::Pop(x509cert);
				CleanupStack::PopAndDestroy(cert);				
				}
				
			TInt hashId=CovertHashNameToAlgorithmId(hashName);		
			iHashName.AppendL(hashId);
					
			iValidateResults.AppendL(vResult);
			iSignedAttributePresent.AppendL(sAP);
			iUnSignedAttributePresent.AppendL(uSAP);
			iSignerInfoVersion.AppendL(signerInfoVersion);
			
			//for next pair
			index++;
			fKeyName.Format(_L("PrivateKey_%d"), index);
			fCertName.Format(_L("Certificate_%d"), index);
			fHashAlgorithmName.Format(_L("HashAlgorithm_%d"), index);
			fValidationResult.Format(_L("ValidationResult_%d"), index);
			fSignedAttributePresent.Format(_L("SignedAttributePresent_%d"), index);
			fUnSignedAttributePresent.Format(_L("UnSignedAttributePresent_%d"), index);
			fSignerInfoVersion.Format(_L("SignerInfoVersion_%d"), index);
			}			
		}
	return TestStepResult();
	}

CMessageDigest* CTSignedDataBaseStep::CreateHashLC(TAlgorithmId aAlgorithmId)
	{
	CMessageDigest* hash(NULL);
	switch (aAlgorithmId)
		{
	case EMD2:
		hash=CMD2::NewL();
		break;
		
	case EMD5:
		hash=CMD5::NewL();
		break;
		
	case ESHA1:
		hash=CSHA1::NewL();
		break;
		
	default:
		User::Leave(KErrNotSupported);
		}
	CleanupStack::PushL(hash);
	return hash;		
	}


//
// Implementation of CMS Signed Data Test Step
//
CTCmsSignedDataStep::CTCmsSignedDataStep()
	{
	}

CTCmsSignedDataStep::~CTCmsSignedDataStep()
	{
	}


void CTCmsSignedDataStep::CheckAndValidateSignedDataL(TDesC8& aRawData)
	{
	//Decode the content info encoding read from predefined file	
	CCmsContentInfo* content=CCmsContentInfo::NewL(aRawData);
	CleanupStack::PushL(content);
	if (content->ContentType()!=EContentTypeSignedData)
		{
		INFO_PRINTF1(_L("Content Type is not Signed Data"));
		User::Leave(KErrGeneral);
		}
	else
		{
		//Decode the signed data and check the fields
		CCmsSignedObject* signedData=CCmsSignedObject::NewL(*content);
		CleanupStack::PushL(signedData);	
		CheckSignedDataFieldsL(*signedData);
		
		//Validate the signatures
		const RPointerArray<CCmsSignerInfo>& signerInfos=signedData->SignerInfo();
		CheckSignerInfoFieldsL(signerInfos);
		
		if (!iNoValidationTest)
			{
			TInt count=signerInfos.Count();
			for (TInt i=0;i<count;i++)
				{
				HBufC8* certificateEncoding = NULL;
				
				CMessageDigest* hash(NULL);
				if (iIsHashAvailable)
					{
					hash=CreateHashLC((TAlgorithmId)iHashName[i]);
					}
					
				TBool isValid(EFalse);
				if (iValidateUsingUserCerts)
					{
					INFO_PRINTF1(_L("Test validation by using given certificates"));
					if (iIsHashAvailable)
						{
						isValid = signedData->ValidateSignerLC(*signerInfos[i], iCertificates, certificateEncoding, ETrue, hash->Hash(iDataContent->Des()));
						}
					else 
						{
						if (iIsDetached)
							{
							if (!iValidationDetachedWithoutInput)
								{
								isValid = signedData->ValidateSignerLC(*signerInfos[i], iCertificates, certificateEncoding, EFalse, iDataContent->Des());
								}
							else
								{
								isValid = signedData->ValidateSignerLC(*signerInfos[i], iCertificates, certificateEncoding);	
								}							
							}
						else
							{
							isValid = signedData->ValidateSignerLC(*signerInfos[i], iCertificates, certificateEncoding);	
							}
						}					
					}
				else
					{
					INFO_PRINTF1(_L("Test validation by using the embedded certificates"));
					
					if (iIsHashAvailable)
						{
						isValid = signedData->ValidateSignerLC(*signerInfos[i], certificateEncoding, ETrue, hash->Hash(iDataContent->Des()));
						}
					else 
						{
						if (iIsDetached)
							{
							if (!iValidationDetachedWithoutInput)
								{
								isValid = signedData->ValidateSignerLC(*signerInfos[i], certificateEncoding, EFalse, iDataContent->Des());	
								}
							else
								{
								isValid = signedData->ValidateSignerLC(*signerInfos[i], certificateEncoding);	
								}
							}
						else
							{
							isValid = signedData->ValidateSignerLC(*signerInfos[i], certificateEncoding);	
							}
						}					
					}
									
				if (!isValid)
					{
					INFO_PRINTF1(_L("Couldn't validate signer"));
					}
				else
					{
					CActiveScheduler* sched = NULL;
					if (CActiveScheduler::Current() == NULL)
						{
						INFO_PRINTF1(_L("Installing scheduler"));
						sched = new (ELeave) CActiveScheduler();
						CleanupStack::PushL (sched);
						CActiveScheduler::Install (sched);
						}
					RPointerArray<CX509Certificate> roots (&iRootCertificate, 1);
					CPKIXCertChain * chain = CPKIXCertChain::NewLC(iFs, *certificateEncoding, roots);
					
					TTime tm;
					_LIT(KDateCorrect1,"20061128:"); 
					TBuf <24> theDate(KDateCorrect1); 
					TInt err=tm.Set(theDate); 
					if(err)
						{
						tm.HomeTime();
						}
						
					CPKIXValidationResult* result = CPKIXValidationResult::NewLC();
					CTPKCS7Validator* validator = new (ELeave) CTPKCS7Validator (chain, result, &tm);
					validator->doValidate ();
					sched->Start ();
					if (result->Error().iReason == EValidatedOK)
						{
						isValid = ETrue;
						INFO_PRINTF1(_L("Validation success"));
						}
					else
						{
						isValid = EFalse;
						INFO_PRINTF2(_L("Validation failed: %d"), result->Error().iReason);
						}
	        		delete validator;
					CleanupStack::PopAndDestroy(result);
					CleanupStack::PopAndDestroy(chain);
					if (sched)
						{
						CActiveScheduler::Install (NULL);
						CleanupStack::PopAndDestroy (sched);
						}
					}
				
				CleanupStack::PopAndDestroy(certificateEncoding);				
				if (hash)
					{
					CleanupStack::PopAndDestroy(hash);	
					}				
				
				if (isValid!=iValidateResults[i])
					{
					INFO_PRINTF1(_L("validate result not as expected"));
					User::Leave(KErrGeneral);
					}
				}			
			}
		CleanupStack::PopAndDestroy(signedData);
		}
	CleanupStack::PopAndDestroy(content);						
	}

void CTCmsSignedDataStep::CheckEncapsulatedContentFieldsL(const CEncapsulatedContentInfo& aEncapContentInfo)
	{
	if (aEncapContentInfo.ContentType()!=EContentTypeData)
		{
		INFO_PRINTF1(_L("Encapsulated data Content is not data content type"));
		User::Leave(KErrGeneral);
		}
	else
		{
		if (aEncapContentInfo.IsContentDataPresent() == iIsDetached)
			{
			INFO_PRINTF1(_L("Encapsulated data Content attachment not as expected"));
			User::Leave(KErrGeneral);
			}
		else
			{
			if (aEncapContentInfo.IsContentDataPresent() && aEncapContentInfo.ContentData()!=*iDataContent)
				{
				INFO_PRINTF1(_L("Encapsulated data Content not as expected"));
				User::Leave(KErrGeneral);
				}
			}
		}		
	}

void CTCmsSignedDataStep::CheckAlgorithmSetFieldsL(const RPointerArray<CX509AlgorithmIdentifier>& aAlgorithms)
	{
	if (iAlgorithmCount!=aAlgorithms.Count())
		{
		INFO_PRINTF1(_L("Number of Algorithm ID is not as expected"));
		User::Leave(KErrGeneral);
		}
	}

void CTCmsSignedDataStep::CheckCertificateSetFieldsL(const CCmsSignedObject& aSignedData)
	{
	if (aSignedData.IsCertificateSetPresent())
		{
		const RPointerArray<CCmsCertificateChoice>& certSet=aSignedData.Certificates();
		if (iCertsCount!=certSet.Count())
			{
			INFO_PRINTF1(_L("Number of Certificates is not as expected"));
			User::Leave(KErrGeneral);
			}
		else
			{
			//Signer Certificate is in the Signed data
			if (!iNoCertSet)
				{
				TInt count = iCertificates.Count();
				for (TInt i=0;i<count;i++)
					{
					if (certSet[i]->CertificateType()==CCmsCertificateChoice::ECertificateX509 && !iCertificates[i]->IsEqualL(certSet[i]->Certificate()))
						{
						INFO_PRINTF2(_L("X509 Certificates %d is not as expected"), i);
						User::Leave(KErrGeneral);						
						}
					}
				}
			
			if (iAdditionalCertificate || iAdditionalEncodedCertificate)
				{
				if (certSet[iCertsCount-1]->CertificateType()==CCmsCertificateChoice::ECertificateAttribute && 
						certSet[iCertsCount-1]->AttributeCertificate()->Compare(*iAdditionalEncodedCertificate)!=0)
					{
					INFO_PRINTF1(_L("Additional Attribute Certificates is not as expected"));
					User::Leave(KErrGeneral);						
					}
				else if (certSet[iCertsCount-1]->CertificateType()==CCmsCertificateChoice::ECertificateX509)
						{
						if (iAdditionalCertificate && !certSet[iCertsCount-1]->Certificate().IsEqualL(*iAdditionalCertificate))
							{
							INFO_PRINTF1(_L("Additional X509 Certificates is not as expected"));
							User::Leave(KErrGeneral);																					
							}
						else
							{
							if (iAdditionalEncodedCertificate)
								{
								CX509Certificate* addX509Cert=CX509Certificate::NewLC(*iAdditionalEncodedCertificate);
								if (!certSet[iCertsCount-1]->Certificate().IsEqualL(*addX509Cert))
									{
									INFO_PRINTF1(_L("Additional X509 Certificates is not as expected"));
									User::Leave(KErrGeneral);										
									}
								CleanupStack::PopAndDestroy(addX509Cert);
								}
							}
						}
				}
			}
		}
	}

void CTCmsSignedDataStep::CheckSignerInfoFieldsL(const RPointerArray<CCmsSignerInfo>& signerInfos)
	{
	TInt count=signerInfos.Count();
	if (iDecPKCS8Data.Count()!=count && iSignerCount!=count)
		{
		INFO_PRINTF1(_L("Number of Signer Info is not as expected"));
		User::Leave(KErrGeneral);
		}
	for (TInt i=0;i<count;i++)
		{
		if (signerInfos[i]->IsSignedAttributesPresent()!=iSignedAttributePresent[i]
			|| signerInfos[i]->IsUnsignedAttributesPresent()!=iUnSignedAttributePresent[i]
			|| signerInfos[i]->Version()!=iSignerInfoVersion[i])
			{
			INFO_PRINTF1(_L("Signed or Unsigned Attribute presence or Signer Version is not as expected"));
			User::Leave(KErrGeneral);
			}
			
		const CX509AlgorithmIdentifier& digestId=signerInfos[i]->DigestAlgorithm();
		if (digestId.Algorithm()!=(TAlgorithmId)iHashName[i])
			{
			INFO_PRINTF1(_L("Digest Algorithm ID is not as expected"));
			User::Leave(KErrGeneral);
			}
			
		const CX509AlgorithmIdentifier& sigId=signerInfos[i]->SignatureAlgorithm();
		if (iDecPKCS8Data[i]->Algorithm()!=sigId.Algorithm())
			{
			INFO_PRINTF1(_L("Signature Algorithm ID is not as expected"));
			User::Leave(KErrGeneral);
			}
			
		const CCmsSignerIdentifier& signerId=signerInfos[i]->SignerIdentifier();
		if (signerId.SignerIdentifierType()==CCmsSignerIdentifier::EIssuerAndSerialNumber)
			{
			if (!iCertificates[i]->IssuerName().ExactMatchL(signerId.IssuerAndSerialNumber()->IssuerName()))
				{
				INFO_PRINTF1(_L("Issuer name is not as expected"));
				User::Leave(KErrGeneral);
				}
			else
				{
				RInteger sn1=RInteger::NewL(iCertificates[i]->SerialNumber());
				CleanupClosePushL(sn1);
				RInteger sn2=RInteger::NewL(signerId.IssuerAndSerialNumber()->SerialNumber());
				CleanupClosePushL(sn2);
				if (sn1!=sn2)
					{
					INFO_PRINTF1(_L("Serial number is not as expected"));
					User::Leave(KErrGeneral);					
					}
				CleanupStack::PopAndDestroy(2, &sn1);//sn2, sn1
					
				}
			}
		else if (signerId.SignerIdentifierType()==CCmsSignerIdentifier::ESubjectKeyIdentifier)
				{
				const CX509CertExtension* certExt = iCertificates[i]->Extension(KSubjectKeyId);
				if (certExt)
					{
					CX509SubjectKeyIdExt* ext=CX509SubjectKeyIdExt::NewLC(certExt->Data());
					if (signerId.SubjectKeyIdentifier().Compare(ext->KeyId())!=0)
						{
						INFO_PRINTF1(_L("Subject Key Id is not as expected"));
						User::Leave(KErrGeneral);
						}
					CleanupStack::PopAndDestroy(ext);
					}
				}
		}
	}

void CTCmsSignedDataStep::CheckSignedDataFieldsL(const CCmsSignedObject& aSignedData)
	{		
	if (aSignedData.IsCertificateSetPresent()!=iCertificateSetPresent ||
		aSignedData.IsCertificateRevocationListsPresent()!=iCRLsSetPresent ||
		aSignedData.Version()!=iSignedDataVersion)
		{
		INFO_PRINTF1(_L("cert present or CRL present or version not as expected"));
		User::Leave(KErrGeneral);
		}
	else
		{
		const CEncapsulatedContentInfo& encapContentInfo=aSignedData.ContentInfo();
		CheckEncapsulatedContentFieldsL(encapContentInfo);
		const RPointerArray<CX509AlgorithmIdentifier>& algorithms=aSignedData.DigestAlgorithms();
		CheckAlgorithmSetFieldsL(algorithms);
		CheckCertificateSetFieldsL(aSignedData);
		}
	}

void CTCmsSignedDataStep::doTestL()
	{
	__UHEAP_MARK;
	
	CCmsSignedObject* signedData(NULL);
	TInt count=iDecPKCS8Data.Count();
	
	//Create Signed Object
	for (TInt i=0;i<count;i++)
		{
		//Get the key pair
		CDecPKCS8Data* decPKCS8Data=iDecPKCS8Data[i];
		MPKCS8DecodedKeyPairData* keyPair = decPKCS8Data->KeyPairData();
		
		CMessageDigest* hash(NULL);
		TPtrC8 hashValue;
		if (iIsHashAvailable)
			{
			hash=CreateHashLC((TAlgorithmId)iHashName[i]);
			hashValue.Set(hash->Hash(iDataContent->Des()));
			}
		
		//If it is the first time, a signed object needs to be created
		if (i==0)
			{
			if (iIsHashAvailable)
				{
				if (decPKCS8Data->Algorithm()==ERSA)
					{
					const CRSAPrivateKey& RSAPrivateKey=static_cast<CPKCS8KeyPairRSA*>(keyPair)->PrivateKey();
					if (!iTwoStepCreation)
						{
						signedData=CCmsSignedObject::NewL((TCmsContentInfoType)iContentType,
															hashValue,
															(TAlgorithmId)iHashName[i],
															RSAPrivateKey,
															*iCertificates[i],
															!iNoCertSet);
						CleanupStack::PushL(signedData);																
						}
					else
						{
						signedData=CCmsSignedObject::NewL((TCmsContentInfoType)iContentType, iIsDetached, iDataContent->Des());
						CleanupStack::PushL(signedData);
						signedData->SignL(hashValue, (TAlgorithmId)iHashName[i], RSAPrivateKey, *iCertificates[i], !iNoCertSet);
						}
					}
				else
					{
					const CDSAPrivateKey& DSAPrivateKey=static_cast<CPKCS8KeyPairDSA*>(keyPair)->PrivateKey();
					if (!iTwoStepCreation)
						{
						signedData=CCmsSignedObject::NewL((TCmsContentInfoType)iContentType,
															hashValue,
															(TAlgorithmId)iHashName[i],
															DSAPrivateKey,
															*iCertificates[i],
															!iNoCertSet);
						CleanupStack::PushL(signedData);																	
						}
					else
						{
						signedData=CCmsSignedObject::NewL((TCmsContentInfoType)iContentType, iIsDetached, iDataContent->Des());
						CleanupStack::PushL(signedData);
						signedData->SignL(hashValue, (TAlgorithmId)iHashName[i], DSAPrivateKey, *iCertificates[i], !iNoCertSet);
						}
					iRsaAlgorithm=EFalse;			
					}
				CleanupStack::Pop(signedData);	
				CleanupStack::PopAndDestroy(hash);
				CleanupStack::PushL(signedData);
				}		
			else
				{
				signedData=CCmsSignedObject::NewL((TCmsContentInfoType)iContentType, iIsDetached, iDataContent->Des());
				CleanupStack::PushL(signedData);
				if (!iNoSigning)
					{
					if (decPKCS8Data->Algorithm()==ERSA)
						{
						const CRSAPrivateKey& RSAPrivateKey=static_cast<CPKCS8KeyPairRSA*>(keyPair)->PrivateKey();
						signedData->SignL(KNullDesC8, (TAlgorithmId)iHashName[i], RSAPrivateKey, *iCertificates[i], !iNoCertSet);
						}
					else
						{
						const CDSAPrivateKey& DSAPrivateKey=static_cast<CPKCS8KeyPairDSA*>(keyPair)->PrivateKey();
						signedData->SignL(KNullDesC8, (TAlgorithmId)iHashName[i], DSAPrivateKey, *iCertificates[i], !iNoCertSet);
						iRsaAlgorithm=EFalse;
						}												
					}
				}
			}
		else
			{
			//multiple signatures
			if (iIsHashAvailable)
				{
				if (decPKCS8Data->Algorithm()==ERSA)
					{
					const CRSAPrivateKey& RSAPrivateKey=static_cast<CPKCS8KeyPairRSA*>(keyPair)->PrivateKey();
					signedData->SignL(hashValue, (TAlgorithmId)iHashName[i], RSAPrivateKey, *iCertificates[i], !iNoCertSet);
					}
				else
					{
					const CDSAPrivateKey& DSAPrivateKey=static_cast<CPKCS8KeyPairDSA*>(keyPair)->PrivateKey();
					signedData->SignL(hashValue, (TAlgorithmId)iHashName[i], DSAPrivateKey, *iCertificates[i], !iNoCertSet);
					iRsaAlgorithm=EFalse;
					}
				CleanupStack::PopAndDestroy(hash);	
				}
			else
				{
				if (decPKCS8Data->Algorithm()==ERSA)
					{
					const CRSAPrivateKey& RSAPrivateKey=static_cast<CPKCS8KeyPairRSA*>(keyPair)->PrivateKey();
					signedData->SignL(KNullDesC8, (TAlgorithmId)iHashName[i], RSAPrivateKey, *iCertificates[i], !iNoCertSet);
					}
				else
					{
					const CDSAPrivateKey& DSAPrivateKey=static_cast<CPKCS8KeyPairDSA*>(keyPair)->PrivateKey();
					signedData->SignL(KNullDesC8, (TAlgorithmId)iHashName[i], DSAPrivateKey, *iCertificates[i], !iNoCertSet);
					iRsaAlgorithm=EFalse;
					}						
				}
			}	
		}
	
	if (iAdditionalCertificate)
		{
		signedData->AddCertificateL(*iAdditionalCertificate);	
		}

 	if (iAdditionalEncodedCertificate)
 		{
 		signedData->AddCertificateL(*iAdditionalEncodedCertificate, (CCmsCertificateChoice::TCertificateType)iAdditionalEncodedCertificateType);
 		}
		
	//Encoding the Signed object
	CASN1EncSequence* signedObjectSeq=signedData->EncodeASN1DERLC();
	HBufC8* buf=CreateDEREncodingLC(*signedObjectSeq);

	//Encoding the wrapper Content Info
	CCmsContentInfo* content=CCmsContentInfo::NewL(EContentTypeSignedData, *buf);
	CleanupStack::PushL(content);
	CASN1EncSequence* contentSeq=content->EncodeASN1DERLC();
	HBufC8* signature=CreateDEREncodingLC(*contentSeq);
	CleanupStack::Pop(signature);
	CleanupStack::PopAndDestroy(5, signedData);	//contentSeq,content,buf,signedObjectSeq,signedData
	CleanupStack::PushL(signature);


	//write the result to a file, for initial debuging
	//OutputResultToFileL(signature->Des());
	
	//Compare the result with the expected result, if the signature algorithms are RSA
		
	if (iRsaAlgorithm)
		{
		
		//Check if the signature is the same as expected
		TBool r=signature->Compare(*iExpectedEncoding);
		if (r!=0 && !iIsOOMTest)
			{
			INFO_PRINTF1(_L("RSA Signature Encoding Error"));
			User::Leave(KErrGeneral);
			}
		else
			{
			CheckAndValidateSignedDataL(*iExpectedEncoding);
			}
		}
	else	
		{
		CheckAndValidateSignedDataL(*iExpectedEncoding);
		CheckAndValidateSignedDataL(*signature);		
		}	
			
	CleanupStack::PopAndDestroy(signature);
	__UHEAP_MARKEND;
	}

//
// Implementation of CMS Signed Data Decoding Test Step
//

CTCmsSignedDataDecodingStep::CTCmsSignedDataDecodingStep()
	{
	}

CTCmsSignedDataDecodingStep::~CTCmsSignedDataDecodingStep()
	{
	}

void CTCmsSignedDataDecodingStep::doTestL()
	{
	__UHEAP_MARK;
	CheckAndValidateSignedDataL(*iExpectedEncoding);
	__UHEAP_MARKEND;
	}

//
// Implementation of CMS Content Info Test step
//
CTCmsContentInfoDecodingStep::CTCmsContentInfoDecodingStep()
	{	
	}

CTCmsContentInfoDecodingStep::~CTCmsContentInfoDecodingStep()
	{	
	}
	
void CTCmsContentInfoDecodingStep::doTestL()
	{
	INFO_PRINTF1(_L("Start CMS Data Type Decoding"));
	CCmsContentInfo* content=CCmsContentInfo::NewL(*iExpectedEncoding);
	CleanupStack::PushL(content);
	if (content->ContentType()!=iExpectedDataType)
		{
		INFO_PRINTF1(_L("CMS Data Type is not as expected"));
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(content);		
	}









