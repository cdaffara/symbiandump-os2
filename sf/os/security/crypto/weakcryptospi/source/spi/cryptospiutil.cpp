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
* legacy plugin selector implementation
* Legacy plugin selector implementation
*
*/


/**
 @file
*/
#include "cryptospiutil.h"
#include "cryptospiproperty.h"
#include "e32uid.h"
#include <cryptospi/cryptospidef.h>
#include <e32property.h>
#include "rulecharacteristics.h"

using namespace CryptoSpi;

/**
The name of the executable which set up all the crypto spi
*/
_LIT(KCryptoSpiPropertySetupExe, "z:\\sys\\bin\\cryptospisetup.exe");

void CryptoSpiUtil::RetrieveCharacteristicsL(TInt32 aInterface, RPointerArray<CCharacteristicsAndPluginName>& aList)
	{
	RDesReadStream stream;
	RBuf8 rBuf;
	TInt count=0;
	CleanupClosePushL(rBuf);
	TInt length=RetrieveCharacteristicsL(aInterface, stream, rBuf, count);
	if (length<=4)	
		{
		CleanupStack::PopAndDestroy(&rBuf);
		return;
		}
		
	//internalize the characteristics and dll index
	for (TInt i=0;i<count;i++)
		{
		CCharacteristicsAndPluginName* temp=CCharacteristicsAndPluginName::NewLC(aInterface);
		temp->InternalizeL(stream);
		aList.AppendL(temp);
		CleanupStack::Pop(temp); //temp
		}
	CleanupStack::PopAndDestroy(&rBuf);
	}

void CryptoSpiUtil::RetrieveCharacteristicsL(TInt32 aInterface, RPointerArray<CRulesCharacteristicsAndPluginName>& aList)
	{
	RDesReadStream stream;
	RBuf8 rBuf;
	TInt count=0;
	CleanupClosePushL(rBuf);
	TInt length=RetrieveCharacteristicsL(aInterface, stream, rBuf, count);
	if (length<=4)	
		{
		CleanupStack::PopAndDestroy(&rBuf);
		return;
		}
		
	//internalize the characteristics and dll index
	for (TInt i=0;i<count;i++)
		{
		CRulesCharacteristicsAndPluginName* temp=CRulesCharacteristicsAndPluginName::NewLC(aInterface);
		temp->InternalizeL(stream);
		aList.AppendL(temp);
		CleanupStack::Pop(temp); //temp
		}
	CleanupStack::PopAndDestroy(&rBuf);
	}

TInt CryptoSpiUtil::RetrieveCharacteristicsL(TInt32 aInterface, RDesReadStream& aStream, RBuf8& aBuf, TInt& aCount)
	{
	// first we are only trying to retrieve the length of the buffer
	TBuf8<sizeof(TInt32)> buf;
	TInt testResult = RProperty::Get(KCryptoSpiPropertyCat, aInterface, buf);
	if (testResult==KErrNotFound)
		{
		//run the exe to Publish the properties
		RunCryptoSpiPropertySetupExe();
		// testresult would be checked outside the loop
		testResult = RProperty::Get(KCryptoSpiPropertyCat, aInterface, buf);
		}
	
	// overflow will occur as we are only retrieving the length first.
	// any other error we should leave
	if(testResult != KErrOverflow)
		{
		User::LeaveIfError(testResult);
		}
	
	//read the length
	RDesReadStream rStream(buf);
	TInt32 len=rStream.ReadInt32L();
	
	//If the property is empty
	if (len<=4)
		{
		return len;
		}

	//Allocate memory 
	aBuf.CreateMaxL(len);
	User::LeaveIfError(RProperty::Get(KCryptoSpiPropertyCat, aInterface, aBuf));
	
	//Read the length
	aStream.Open(aBuf);
	len=aStream.ReadInt32L();
	
	//Read the count of the characteristics
	aCount=aStream.ReadInt16L();
	return len;	
	}

TInt CryptoSpiUtil::DllIndexToName(TInt aDllIndex, TFileName& aName)
	{
	TInt dllCount=0;
	TInt result = RProperty::Get(KCryptoSpiPropertyCat, KPluginsConfigurationKeyCount, dllCount);	
	if (result==KErrNotFound)
		{
		//run the exe to publish the properties
		RunCryptoSpiPropertySetupExe();
		result = RProperty::Get(KCryptoSpiPropertyCat, KPluginsConfigurationKeyCount, dllCount);
		if (result==KErrNotFound)
			{
			return KErrNotFound;
			}
		}
	if (aDllIndex>=dllCount)
		{
		return KErrNotFound;
		}
	
	TBuf8<KMaxFileName> buf;
	result = RProperty::Get(KCryptoSpiPropertyCat, KPluginsConfigurationKey+aDllIndex, buf);
	if (result==KErrNotFound)
		{
		return KErrNotFound;
		}
				
	RDesReadStream rStream(buf);
	rStream>>aName;
	return KErrNone;		
	}
		
void CryptoSpiUtil::LoadPluginDllLC(RLibrary& aLib, TFileName& aFileName)
	{
	User::LeaveIfError(aLib.Load(aFileName));
	CleanupClosePushL(aLib);
	RLibrary plugin=aLib;
	CleanupClosePushL(plugin);
	User::LeaveIfError(aLib.Duplicate(RThread(), EOwnerProcess));
	CleanupStack::PopAndDestroy(&plugin);		
	}

TInt CryptoSpiUtil::RunCryptoSpiPropertySetupExe()
	{
	const TUidType setupExeUidType(KExecutableImageUid, TUid::Null(), KCryptoSpiPropertyCat);
	RProcess setupProc;
	TInt result = setupProc.Create(KCryptoSpiPropertySetupExe, KNullDesC, setupExeUidType);
	if (result != KErrNone)
		{
		return result;
		}
	TRequestStatus setupStatus;
	// request Rendezvous before Resume() to avoid race condition.
	// Also note if request to rendezvous fails (OOM etc.) then setup exe may
	// complete after query code, with feature property possibly undefined
	setupProc.Rendezvous(setupStatus);
	setupProc.Resume();
	setupProc.Close();
	User::WaitForRequest(setupStatus);
	return setupStatus.Int();
	}

CCharacteristics* CryptoSpiUtil::CreateCharacteristicsL(TInt32 aInterface)
	{
	CCharacteristics* character=NULL;
	switch (aInterface)
		{
	case KHashInterface:
	case KAsyncHashInterface:
		character=CHashCharacteristics::NewL();
		break;
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT				
	case KMacInterface:
	case KAsyncMacInterface:
		character=CMacCharacteristics::NewL();
		break;
#endif		
	case KRandomInterface:
	case KAsyncRandomInterface:
		character=CRandomCharacteristics::NewL();
		break;

	case KSymmetricCipherInterface:
	case KAsyncSymmetricCipherInterface:
		character=CSymmetricCipherCharacteristics::NewL();
		break;

	case KAsymmetricCipherInterface:
	case KAsyncAsymmetricCipherInterface:
		character=CAsymmetricCipherCharacteristics::NewL();
		break;

	case KSignerInterface:
	case KAsyncSignerInterface:
	case KVerifierInterface:
	case KAsyncVerifierInterface:
		character=CAsymmetricSignatureCharacteristics::NewL();
		break;

	case KKeyAgreementInterface:
	case KAsyncKeyAgreementInterface:
		character=CKeyAgreementCharacteristics::NewL();
		break;

	case KKeypairGeneratorInterface:
	case KAsyncKeypairGeneratorInterface:
		character=CKeypairGeneratorCharacteristics::NewL();
		break;
	
	default:
		User::Leave(KErrNotSupported);
		}
	return character;
	}

