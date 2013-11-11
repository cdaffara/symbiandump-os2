/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <cryptospi/cryptospidef.h>
#include <bacline.h>
#include <cryptospi/cryptocharacteristics.h>
#include <cryptospi/cryptospistateapi.h>
#include "dumpprocessor.h"
#include <cryptospi/cryptoparams.h>
#include <cryptospi/extendedcharacteristics.h>
#include <e32cons.h>
#include <f32file.h>

using namespace CryptoSpi;

//
//The name of characteristics log file
//
_LIT(KLogStartMessage, "DumpCryptoSpiTool");
const TInt KMaxLineLength = 1024;
_LIT8(KNewLine8, "\r\n");

//
//Characteristic Heads
//
_LIT(KLogHashCharacteristicHead, "[HashCharacteristic]");
_LIT(KLogRandomCharacteristicHead, "[RandomCharacteristic]");
_LIT(KLogSymmetricCipherCharacteristicHead, "[SymmetricCipherCharacteristic]");
_LIT(KLogAsymmetricCipherCharacteristicHead, "[AsymmetricCipherCharacteristic]");
_LIT(KLogSignerCharacteristicHead, "[SignerCharacteristic]");
_LIT(KLogVerifierCharacteristicHead, "[VerifierCharacteristic]");
_LIT(KLogKeyAgreementCharacteristicHead, "[KeyAgreementCharacteristic]");
_LIT(KLogKeypairGeneratorCharacteristicHead, "[KeypairGeneratorCharacteristic]");
_LIT(KLogUnknownCharacteristicHead, "[UnknownCharacteristic]");

//
//bool and unknow
//
_LIT(KLogBoolTrue, "True");
_LIT(KLogBoolFalse, "False");
_LIT(KLogUnknown, "Unknown");

//Common Characteristics
_LIT(KLogInterfaceName, "	InterfaceName = %S");
_LIT(KLogAlgorithmUid, "	AlgorithmUid = 0x%x");
_LIT(KLogImplementationUid, "	ImplementationUid = 0x%x");
_LIT(KLogCreatorName, "	CreatorName = %S");
_LIT(KLogFIPSApproved, "	FIPSApproved = %S");
_LIT(KLogHardwareImplementation, "	HardwareImplementation = %S");
_LIT(KLogAlgorithmName, "	AlgorithmName = %S");
_LIT(KLogMaxConcurrency, "	MaxConcurrency = %d");
_LIT(KLogLatency, "	Latency = %d");
_LIT(KLogThroughput, "	Throughput = %d");

//Hash Characteristics
_LIT(KLogBlockSize, "	BlockSize = %d");
_LIT(KLogOutputSize, "	OutputSize = %d");

//Random Characteristics
_LIT(KLogBlockMode, "	BlockMode = %d");

//Symmetric Cipher Characteristics
_LIT(KLogMaximumKeyLength, "	MaximumKeyLength = %d");
_LIT(KLogKeySupportMode, "	KeySupportMode = %d");

//Plugin Dll Name
_LIT(KDllName, "	PluginDllName = %S");

//Extended characteristics
_LIT(KLogExtendedCharacteristicInt, "	Ext[0x%x] = 0x%x");
_LIT(KLogExtendedCharacteristicDes, "	Ext[0x%x] = %S");
_LIT8(KLogExtendedCharacteristicDes8, "	Ext[0x%x] = %S");

//padding modes definitions	
_LIT(KLogPaddingModes1, "	SupportedPaddingModes_%d = %S");
_LIT(KLogPaddingModeNone, "NoneMode");
_LIT(KLogPaddingModeSSLv3, "SSLv3Mode");
_LIT(KLogPaddingModePKCS7, "PKCS7Mode");
_LIT(KLogPaddingModePkcs1_v1_5_Encryption, "Pkcs1_v1_5_EncryptionMode");
_LIT(KLogPaddingModePkcs1_v1_5_Signature, "Pkcs1_v1_5_SignatureMode");

//Symmetric cipher operation modes definitions
_LIT(KLogOperationModeNone, "NoneMode");
_LIT(KLogOperationModeECB, "ECBMode");
_LIT(KLogOperationModeCBC, "CBCMode");
_LIT(KLogOperationModeOFB, "OFBMode");
_LIT(KLogOperationModeCFB, "CFBMode");
_LIT(KLogOperationModeCTR, "CTRMode");

//Hash modes definitions
_LIT(KLogOperationModes1, "	SupportedOperationModes_%d = %S");
_LIT(KLogOperationModeHash, "HashMode");
_LIT(KLogOperationModeHmac, "HmacMode");

//Help text
_LIT(KUsage, "Usage: dumpcryptoplugin [switches] \n");
_LIT(KHelpInterface, "-interface/-i <interface name>	Dump characteristics for the interface, which can be\n");
_LIT(KHelpInterface1, "				hash,random,symcipher,asymcipher,signer,verifier,\n");
_LIT(KHelpInterface2, "				keypairgen,keyagree,asynchash,asyncrandom,asyncsymcipher\n");
_LIT(KHelpInterface3, "				asyncsigner,asyncverifier,asynckeypairgen,asynckeyagree.\n");
_LIT(KHelpAlgorithm, "-algorithm/-al <algorithm name>	Dump characteristics for the algorithm, which can be\n");
_LIT(KHelpAlgorithm1, "				md2,md4,md5,sha1,des,3des,rc2,aes,rc4,rsacipher,rsasigner\n");
_LIT(KHelpAlgorithm2, "				rsaverifier,rsakeypair,dsasigner,dsaverifier,dsakeypair\n");
_LIT(KHelpAlgorithm3, "				dhkeypair,dhkeyagree.\n");
_LIT(KHelpAll, "-all/-a				Dump all the characteristics of all the inplementations.\n");
_LIT(KHelpExtended, "-extended/-e			Dump the extended characteristics as well.\n");
_LIT(KHelpDll, "-dll/-d	<filename>		Only dump the characteristics for the given plugin Dll.\n");
_LIT(KHelpOutput, "-out/-o	<drive:><path><filename>Output file name.\n");
_LIT(KHelpHelp, "-help/-h			Help\n");

_LIT(KProgessStatus, "Progressing\n");
_LIT(KFileStatus, "Dump to %S");
_LIT(KInvalidArgument, "Invalid Argument:");
_LIT(KArgument, " %S");


//
//Implementation of CDumpToolParameters
//
CDumpToolParameters* CDumpToolParameters::NewLC()
	{
	CDumpToolParameters* self=new(ELeave) CDumpToolParameters();
	CleanupStack::PushL(self);
	return self;		
	}
	
CDumpToolParameters::~CDumpToolParameters()
	{
	iOptions.Close();
	iInvalidOptions.ResetAndDestroy();
	}
	
CDumpToolParameters::CDumpToolParameters()
	{
	}

TBool CDumpToolParameters::FindOption(TOption aOption) const
	{
	TBool found=EFalse;
	TInt count=iOptions.Count();
	for (TInt i=0;i<count;i++)
		{
		if (aOption==iOptions[i])
			{
			found=ETrue;
			break;	
			}
		}
	return found;
	}
	
//
//Implementation of CDumpToolProcessor
//
CDumpToolProcessor* CDumpToolProcessor::NewLC(const CDumpToolParameters& aParams, CConsoleBase* aConsole)
	{
	CDumpToolProcessor* self=new(ELeave) CDumpToolProcessor(aParams, aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;		
	}

CDumpToolProcessor::~CDumpToolProcessor()
	{
	iLogFile.Close();
	iFs.Close();
	iCharacters.ResetAndDestroy();	
	}

CDumpToolProcessor::CDumpToolProcessor(const CDumpToolParameters& aParams, CConsoleBase* aConsole)
: iInputParams(aParams), iConsole(aConsole)
	{
	}

void CDumpToolProcessor::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
		
	if (iInputParams.iDumpFileName.CompareF(KNullDesC)==0)
		{
		TUint driveCh(RFs::GetSystemDriveChar());

		_LIT(KDumpFileNameFmt, "%c:\\dumpcrypoplugin.txt");
		iDumpFileName.Format(KDumpFileNameFmt, driveCh);
		}
	else
		{
		TInt pos=iInputParams.iDumpFileName.LocateReverseF('\\');
		TFileName path=iInputParams.iDumpFileName.Left(pos+1);
		TInt ret=iFs.MkDirAll(path);
		if (ret!=KErrNone && ret!=KErrAlreadyExists)
			{
			User::LeaveIfError(ret);	
			}
		iDumpFileName=iInputParams.iDumpFileName;
		}
	
	User::LeaveIfError(iLogFile.Replace(iFs, iDumpFileName, EFileWrite | EFileShareExclusive));
	TBool extended=EFalse;
	if (iInputParams.FindOption(CDumpToolParameters::EOptionExtendedCharacteristic))
		{
		extended=ETrue;
		}
	CCryptoSpiStateApi::EnumerateCharacteristicsL(iCharacters, iInputParams.iInterface, extended);
	}
	
void CDumpToolProcessor::ProcessL()
	{
	if (iInputParams.FindOption(CDumpToolParameters::EOptionHelp))
		{
		DisplayHelp();
		return;	
		}
	
	if (iInputParams.iInvalidOptions.Count()!=0 )
		{
		DisplayInvalidOptions();
		return;
		}
	
	LogL(KLogStartMessage);
	TInt count=iCharacters.Count();
	iConsole->Printf(KProgessStatus);
	for (TInt i=0;i<count;i++)
		{
		if (iInputParams.iAlgorithm!=0 
			&& iInputParams.iAlgorithm!=iCharacters[i]->iCharacteristic->iAlgorithmUid)
			{
			continue;
			}
		
		if (iInputParams.iDllName.Compare(KNullDesC)!=0
			&&iInputParams.iDllName.Compare(iCharacters[i]->iDllName)!=0)
			{
			continue;
			}
		OutputCharcteristicsAndDllL(*iCharacters[i]);
		}
	iConsole->Printf(KFileStatus, &iDumpFileName);
	}

void CDumpToolProcessor::OutputCharacteristicHeadL(TInt32 aInterfaceUid)
	{
	LogL(KNewLine8);
	switch (aInterfaceUid)
		{
	case KHashInterface:
	case KAsyncHashInterface:
		LogL(KLogHashCharacteristicHead);
		break;

	case KRandomInterface:
	case KAsyncRandomInterface:
		LogL(KLogRandomCharacteristicHead);
		break;
		
	case KSymmetricCipherInterface:
	case KAsyncSymmetricCipherInterface:
		LogL(KLogSymmetricCipherCharacteristicHead);
		break;
		
	case KAsymmetricCipherInterface:
	case KAsyncAsymmetricCipherInterface:
		LogL(KLogAsymmetricCipherCharacteristicHead);
		break;
		
	case KSignerInterface:
	case KAsyncSignerInterface:
		LogL(KLogSignerCharacteristicHead);
		break;
		
	case KVerifierInterface:
	case KAsyncVerifierInterface:
		LogL(KLogVerifierCharacteristicHead);
		break;
		
	case KKeyAgreementInterface:
	case KAsyncKeyAgreementInterface:
		LogL(KLogKeyAgreementCharacteristicHead);
		break;
		
	case KKeypairGeneratorInterface:
	case KAsyncKeypairGeneratorInterface:
		LogL(KLogKeypairGeneratorCharacteristicHead);
		break;
		
	default:
		LogL(KLogUnknownCharacteristicHead);
		}
	}


void CDumpToolProcessor::OutputInterfaceNameL(TInt32 aInterfaceUid)
	{
	switch (aInterfaceUid)
		{
	case KHashInterface:
		LogL(KLogInterfaceName, &KHash);
		break;
		
	case KAsyncHashInterface:
		LogL(KLogInterfaceName, &KAsyncHash);
		break;

	case KRandomInterface:
		LogL(KLogInterfaceName, &KRandom);
		break;
	
	case KAsyncRandomInterface:
		LogL(KLogInterfaceName, &KAsyncRandom);
		break;
		
	case KSymmetricCipherInterface:
		LogL(KLogInterfaceName, &KSymmetricCipher);
		break;
	
	case KAsyncSymmetricCipherInterface:
		LogL(KLogInterfaceName, &KAsyncSymmetricCipher);
		break;
		
	case KAsymmetricCipherInterface:
		LogL(KLogInterfaceName, &KAsymmetricCipher);
		break;
	
	case KAsyncAsymmetricCipherInterface:
		LogL(KLogInterfaceName, &KAsyncAsymmetricCipher);
		break;
		
	case KSignerInterface:
		LogL(KLogInterfaceName, &KSigner);
		break;
	
	case KAsyncSignerInterface:
		LogL(KLogInterfaceName, &KAsyncSigner);
		break;
		
	case KVerifierInterface:
		LogL(KLogInterfaceName, &KVerifier);
		break;
	
	case KAsyncVerifierInterface:
		LogL(KLogInterfaceName, &KAsyncVerifier);
		break;
		
	case KKeyAgreementInterface:
		LogL(KLogInterfaceName, &KKeyAgreement);
		break;
	
	case KAsyncKeyAgreementInterface:
		LogL(KLogInterfaceName, &KAsyncKeyAgreement);
		break;
		
	case KKeypairGeneratorInterface:
		LogL(KLogInterfaceName, &KKeyPairGenerator);
		break;
	
	case KAsyncKeypairGeneratorInterface:
		LogL(KLogInterfaceName, &KAsyncKeyPairGenerator);
		break;
		
	default:
		LogL(KLogInterfaceName, &KLogUnknown);
		}		
	}

void CDumpToolProcessor::OutputAlgorithmNameL(TInt32 aAlgorithmUid)
	{
	switch (aAlgorithmUid)
		{
	case KAlgorithmHashMd2:
		LogL(KLogAlgorithmName, &KMd2);
		break;
	case KAlgorithmHashMd4:
		LogL(KLogAlgorithmName, &KMd4);
		break;		
	case KAlgorithmHashMd5:
		LogL(KLogAlgorithmName, &KMd5);
		break;
	case KAlgorithmHashSha1:
		LogL(KLogAlgorithmName, &KSha1);
		break;
	case KAlgorithmCipherDes:
		LogL(KLogAlgorithmName, &KDes);	
		break;
	case KAlgorithmCipher3Des:
		LogL(KLogAlgorithmName, &K3Des);	
		break;
	case KAlgorithmCipherRc2:
		LogL(KLogAlgorithmName, &KRC2);	
		break;
	case KAlgorithmCipherAes:
		LogL(KLogAlgorithmName, &KAes);	
		break;
	case KAlgorithmCipherArc4:
		LogL(KLogAlgorithmName, &KRC4);	
		break;
	case KAlgorithmCipherRsa:
		LogL(KLogAlgorithmName, &KRsaCipher);	
		break;
	case KAlgorithmSignerRsa:
		LogL(KLogAlgorithmName, &KRsaSigner);	
		break;
	case KAlgorithmVerifierRsa:
		LogL(KLogAlgorithmName, &KRsaVerifier);	
		break;
	case KAlgorithmRSAKeyPairGenerator:
		LogL(KLogAlgorithmName, &KRsaKeyPair);	
		break;
	case KAlgorithmSignerDsa:
		LogL(KLogAlgorithmName, &KDsaSigner);	
		break;
	case KAlgorithmVerifierDsa:
		LogL(KLogAlgorithmName, &KDsaVerifier);	
		break;
	case KAlgorithmDSAKeyPairGenerator:
		LogL(KLogAlgorithmName, &KDsaKeyPair);	
		break;
	case KAlgorithmDHKeyPairGenerator:
		LogL(KLogAlgorithmName, &KDhKeyPair);	
		break;
	case KAlgorithmKeyAgreementDH:
		LogL(KLogAlgorithmName, &KDhKeyAgreement);	
		break;
	default:;
		}
	}

void CDumpToolProcessor::OutputHashOperationModeL(TInt aIndex, TInt32 aOperationMode)
	{
	switch(aOperationMode)
		{
	case KHashMode:
		LogL(KLogOperationModes1, aIndex, &KLogOperationModeHash);
		break;
	case KHmacMode:
		LogL(KLogOperationModes1, aIndex, &KLogOperationModeHmac);
		break;
	default:;
		}
	}

void CDumpToolProcessor::OutputSymmetricOperationModeL(TInt aIndex, TInt32 aOperationMode)
	{
	switch(aOperationMode)
		{
	case KOperationModeNone:
		LogL(KLogOperationModes1, aIndex, &KLogOperationModeNone);
		break;
	case KOperationModeECB:
		LogL(KLogOperationModes1, aIndex, &KLogOperationModeECB);
		break;
	case KOperationModeCBC:
		LogL(KLogOperationModes1, aIndex, &KLogOperationModeCBC);
		break;
	case KOperationModeOFB:
		LogL(KLogOperationModes1, aIndex, &KLogOperationModeOFB);
		break;
	case KOperationModeCFB:
		LogL(KLogOperationModes1, aIndex, &KLogOperationModeCFB);
		break;
	case KOperationModeCTR:
		LogL(KLogOperationModes1, aIndex, &KLogOperationModeCTR);
		break;		
	default:;
		}	
	}

void CDumpToolProcessor::OutputPaddingModeL(TInt aIndex, TInt32 aPaddingMode)
	{
	switch(aPaddingMode)
		{
	case KPaddingModeNone:
		LogL(KLogPaddingModes1, aIndex, &KLogPaddingModeNone);
		break;
	case KPaddingModeSSLv3:
		LogL(KLogPaddingModes1, aIndex, &KLogPaddingModeSSLv3);
		break;
	case KPaddingModePKCS7:
		LogL(KLogPaddingModes1, aIndex, &KLogPaddingModePKCS7);
		break;
	case KPaddingModePkcs1_v1_5_Encryption:
		LogL(KLogPaddingModes1, aIndex, &KLogPaddingModePkcs1_v1_5_Encryption);
		break;
	case KPaddingModePkcs1_v1_5_Signature:
		LogL(KLogPaddingModes1, aIndex, &KLogPaddingModePkcs1_v1_5_Signature);
		break;
	default:;
		}		
	}

void CDumpToolProcessor::OutputSpecificCharacteristicL(const CCharacteristics& aCharacter)
	{
	switch (aCharacter.iInterfaceUid)
		{
	case KHashInterface:
	case KAsyncHashInterface:
		{
		const CHashCharacteristics& sCharacter=static_cast<const CHashCharacteristics&>(aCharacter);
		LogL(KLogBlockSize, sCharacter.iBlockSize);
		LogL(KLogOutputSize, sCharacter.iOutputSize);
		TInt count=sCharacter.iSupportedOperationModes.Count();
		for (TInt i=0;i<count;i++)
			{
			OutputHashOperationModeL(i, sCharacter.iSupportedOperationModes[i]);
			}			
		}
		break;

	case KRandomInterface:
	case KAsyncRandomInterface:
		{
		const CRandomCharacteristics& sCharacter=static_cast<const CRandomCharacteristics&>(aCharacter);
		LogL(KLogBlockMode, sCharacter.iBlockingMode);			
		}
		break;
		
	case KSymmetricCipherInterface:
	case KAsyncSymmetricCipherInterface:
		{
		const CSymmetricCipherCharacteristics& sCharacter=static_cast<const CSymmetricCipherCharacteristics&>(aCharacter);
		LogL(KLogMaximumKeyLength, sCharacter.iMaximumKeyLength);
		LogL(KLogBlockSize, sCharacter.iBlockSize);
		TInt count=sCharacter.iSupportedPaddingModes.Count();
		for (TInt i=0;i<count;i++)
			{
			OutputPaddingModeL(i, sCharacter.iSupportedPaddingModes[i]);
			}			
		count=sCharacter.iSupportedOperationModes.Count();
		for (TInt i=0;i<count;i++)
			{
			OutputSymmetricOperationModeL(i, sCharacter.iSupportedOperationModes[i]);
			}
		LogL(KLogKeySupportMode, sCharacter.iKeySupportMode);
		}
		break;
		
	case KAsymmetricCipherInterface:
	case KAsyncAsymmetricCipherInterface:
		{
		const CAsymmetricCipherCharacteristics& sCharacter=static_cast<const CAsymmetricCipherCharacteristics&>(aCharacter);
		LogL(KLogMaximumKeyLength, sCharacter.iMaximumKeyLength);
		TInt count=sCharacter.iSupportedPaddingModes.Count();
		for (TInt i=0;i<count;i++)
			{
			OutputPaddingModeL(i, sCharacter.iSupportedPaddingModes[i]);
			}			
		LogL(KLogKeySupportMode, sCharacter.iKeySupportMode);	
		}
		break;
		
	case KSignerInterface:
	case KAsyncSignerInterface:
	case KVerifierInterface:
	case KAsyncVerifierInterface:
		{
		const CAsymmetricSignatureCharacteristics& sCharacter=static_cast<const CAsymmetricSignatureCharacteristics&>(aCharacter);
		LogL(KLogMaximumKeyLength, sCharacter.iMaximumKeyLength);
		TInt count=sCharacter.iSupportedPaddingModes.Count();
		for (TInt i=0;i<count;i++)
			{
			OutputPaddingModeL(i, sCharacter.iSupportedPaddingModes[i]);
			}			
		LogL(KLogKeySupportMode, sCharacter.iKeySupportMode);	
		}	
		break;
		
	case KKeypairGeneratorInterface:
	case KAsyncKeypairGeneratorInterface:
		{
		const CKeypairGeneratorCharacteristics& sCharacter=static_cast<const CKeypairGeneratorCharacteristics&>(aCharacter);
		LogL(KLogMaximumKeyLength, sCharacter.iMaximumKeyLength);			
		}
		break;
		
	case KKeyAgreementInterface:
	case KAsyncKeyAgreementInterface:
	default:
		;
		}
	}

void CDumpToolProcessor::OutputExtendedCharacteristicL(const CExtendedCharacteristics& aCharacter)
	{
	CExtendedCharacteristics& character=const_cast<CExtendedCharacteristics&>(aCharacter);
	const CCryptoParams* params=character.ListExtendedCharacteristics();
	const RPointerArray<CCryptoParam>& list=params->GetParams();
	TInt count=list.Count();
	for (TInt i=0;i<count;i++)
		{
		TInt type=list[i]->Type();
		switch (type)
			{
		case CCryptoParam::EInt:
			LogL(KLogExtendedCharacteristicInt, list[i]->Uid().iUid, ((CCryptoIntParam*)list[i])->Value());
			break;
		case CCryptoParam::EBigInt:
			{
			const TInteger& bigInt=((CCryptoBigIntParam*)list[i])->Value();
			HBufC8* buf8=bigInt.BufferLC();
			LogL(KLogExtendedCharacteristicDes8, list[i]->Uid().iUid, buf8);
			CleanupStack::PopAndDestroy(buf8);				
			}
			break;
		case CCryptoParam::EDesC8:
			LogL(KLogExtendedCharacteristicDes8, list[i]->Uid().iUid, ((CCryptoIntParam*)list[i])->Value());
			break;
		case CCryptoParam::EDesC16:
			LogL(KLogExtendedCharacteristicDes, list[i]->Uid().iUid, ((CCryptoIntParam*)list[i])->Value());
			break;
		default:;
			}
		}
	}

void CDumpToolProcessor::OutputCharcteristicsAndDllL(const CCharacteristicsAndPluginName& aElement)
	{
	//Output Head
	OutputCharacteristicHeadL(aElement.iCharacteristic->iInterfaceUid);
	
	//Start of Common Characteristics output
	//Output Interface name
	OutputInterfaceNameL(aElement.iCharacteristic->iInterfaceUid);
	
	//Output Algorithm Uid
	LogL(KLogAlgorithmUid, aElement.iCharacteristic->iAlgorithmUid);
	
	//Output Implementation Uid
	LogL(KLogImplementationUid, aElement.iCharacteristic->iImplementationUid);
	
	//Output Creator's name
	TFileName name=aElement.iCharacteristic->iCreatorName;
	LogL(KLogCreatorName, &name);
	
	//Output FIPS Approved Flag
	if (aElement.iCharacteristic->iIsFIPSApproved)
		{
		LogL(KLogFIPSApproved, &KLogBoolTrue);
		}
	else
		{
		LogL(KLogFIPSApproved, &KLogBoolFalse);
		}
		
	//Output Hardware supported flag
	if (aElement.iCharacteristic->iIsHardwareSupported)
		{
		LogL(KLogHardwareImplementation, &KLogBoolTrue);			
		}
	else
		{
		LogL(KLogHardwareImplementation, &KLogBoolFalse);		
		}
		
	//Output Max concurrency supported
	LogL(KLogMaxConcurrency, aElement.iCharacteristic->iMaxConcurrencySupported);
	
	//Output Algorithm Name
	name=aElement.iCharacteristic->iAlgorithmName;
	LogL(KLogAlgorithmName, &name);
	
	//Output Latency
	LogL(KLogLatency, aElement.iCharacteristic->iLatency);
	
	//Output throughput
	LogL(KLogThroughput, aElement.iCharacteristic->iThroughput);
	//end of Common Characteristics output
	
	//Output Specific Characteristic
	OutputSpecificCharacteristicL(*aElement.iCharacteristic);
	
	//Output extended characteristics
	if (aElement.iExtendedCharacteristic)
		{
		OutputExtendedCharacteristicL(*aElement.iExtendedCharacteristic);
		}
	
	//Output the name of the plugin dll, which contains this implementation
	LogL(KDllName, &aElement.iDllName);	
	}
	
void CDumpToolProcessor::LogL(TRefByValue<const TDesC8> aFmt, ...)
	{
	TBuf8<KMaxLineLength> buf;
	VA_LIST args;
	VA_START(args, aFmt);
	buf.AppendFormatList(aFmt, args);
	VA_END(args);

	User::LeaveIfError(iLogFile.Write(buf));
	User::LeaveIfError(iLogFile.Write(KNewLine8));
	User::LeaveIfError(iLogFile.Flush());
	}
	
void CDumpToolProcessor::LogL(TRefByValue<const TDesC> aFmt, ...)
	{
	TBuf<KMaxLineLength> buf;
	VA_LIST args;
	VA_START(args, aFmt);
	buf.AppendFormatList(aFmt, args);
	VA_END(args);

	TBuf8<KMaxLineLength> buf8;
	buf8.Copy(buf);
	User::LeaveIfError(iLogFile.Write(buf8));
	User::LeaveIfError(iLogFile.Write(KNewLine8));
	User::LeaveIfError(iLogFile.Flush());
	}

/**
Display usage information
*/
void CDumpToolProcessor::DisplayHelp()
	{
	iConsole->Printf(KUsage);
	iConsole->Printf(KHelpInterface);
	iConsole->Printf(KHelpInterface1);
	iConsole->Printf(KHelpInterface2);
	iConsole->Printf(KHelpInterface3);
	iConsole->Printf(KHelpAlgorithm);
	iConsole->Printf(KHelpAlgorithm1);
	iConsole->Printf(KHelpAlgorithm2);
	iConsole->Printf(KHelpAlgorithm3);
	iConsole->Printf(KHelpAll);
	iConsole->Printf(KHelpExtended);
	iConsole->Printf(KHelpDll);
	iConsole->Printf(KHelpOutput);
	iConsole->Printf(KHelpHelp);
	iConsole->Printf(KNewLine);
	iConsole->Printf(KPressAnyKey);
	iConsole->Getch();
			
	}


void CDumpToolProcessor::DisplayInvalidOptions()
	{
	iConsole->Printf(KInvalidArgument);
	TInt count=iInputParams.iInvalidOptions.Count();
	for (TInt i=0;i<count;i++)
		{
		iConsole->Printf(KArgument, iInputParams.iInvalidOptions[i]);	
		}
	iConsole->Printf(KNewLine);
	iConsole->Printf(KPressAnyKey);
	iConsole->Getch();
	
	}
	


