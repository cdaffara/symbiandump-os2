/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Example Code for showing usage of MAC APIs. 
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#include <cryptospi/cryptospidef.h>
#include <cryptospi/cryptomacapi.h>
#include <cryptospi/cryptoparams.h>
#include <cryptospi/keys.h>
#include <f32file.h>
#include "utf.h" 

using namespace CryptoSpi;


const TUint8 KMacBlockSize = 16;                                                               

const TUint8 key[]  = 	{
						 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
						 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
						};

const TUint8 data[] = 	{
						 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
						 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
						};


#define __CLEAN_UP 	logBuffer = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*macResult); \
					CleanupStack::PopAndDestroy(macResult);  \
					CleanupStack::PushL(logBuffer); \
					User::LeaveIfError(log.Write(*logBuffer)); \
					CleanupStack::PopAndDestroy(logBuffer);  


class TVariantPtrC : public TPtrC
	{
	public:
	HBufC* HexStringLC();
	};                                  

HBufC* TVariantPtrC::HexStringLC()
	{
	HBufC* parsedString = HBufC::NewLC(this->Length()*2);
	TBuf<2> pair;
	_LIT(KFormat2,"%02x");                                    
						    
   	for(TInt i = 0; i<this->Length() ; ++i)
    	{
		TInt num  = (*this)[i];
    	pair.Format(KFormat2,num);
    	parsedString->Des().Append(pair);
    	}
	    
	return parsedString;
	}

void EvaluateMacL(const TDesC8& aKey, const TDesC8& aSourceData)
	{
	//-----------------------------CREATION-OF-KEY-AND-MAC-INSTANCE--------------------------

	// Set the key
	CryptoSpi::TKeyProperty keyProperty = {CryptoSpi::KAesUid, KNullUid, CryptoSpi::KSymmetricKey, CryptoSpi::KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, CryptoSpi::KSymmetricKeyParameterUid);
	CKey* uniKey=CKey::NewLC(keyProperty, *keyParam);
	
	//Retrieve a Synchronous MAC Factory Object and use AES-XCBC-MAC-96 or any other MAC algorithm.
	CMac* macImpl = NULL;
	CMacFactory::CreateMacL(macImpl, CryptoSpi::KAesXcbcMac96Uid,*uniKey, NULL);

	//cleanup uniKey and keyParam.
	CleanupStack::PopAndDestroy(2,keyParam);
	CleanupStack::PushL(macImpl);
	
	//---------------------------------EVALUATION-OF-MAC-VALUE---------------------------------
	
	// Usage of Mac APIs: MacL, UpdateL, and FinalL 
	
	//Retrieve the 8bit mac value
	HBufC* macValueIntermediate = HBufC::NewLC(KMacBlockSize);
	HBufC* macValueFinal        = HBufC::NewLC(KMacBlockSize);

	TPtr macPtrIntermediate = macValueIntermediate->Des();
	TPtr macPtrFinal 		= macValueFinal->Des();
	
	//Retrieve the final 8bit mac value and convert to 16bit
	macPtrIntermediate.Copy(macImpl->MacL(aSourceData.Mid(0,3)));
	macImpl->UpdateL(aSourceData.Mid(3,5));
	macPtrFinal.Copy(macImpl->FinalL(aSourceData.Mid(8,8)));

	//---------------------------------WRITE-RESULTS-TO-LOG--------------------------

	// Add log file for automated test environment
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile log;
	CleanupClosePushL(log);
	User::LeaveIfError(log.Replace(fs, _L("c:\\t_mac_apis.log"), EFileShareAny|EFileWrite));
	User::LeaveIfError(log.Write(_L8("\n*** Key Used for Mac Evaluation (in hex) is = 0x000102030405060708090a0b0c0d0e0f ")));

	
	//Take the 16bit descriptor and convert the string to hexadecimal
	TVariantPtrC convertMac;
	convertMac.Set(macPtrIntermediate);
	HBufC* macResult = convertMac.HexStringLC();	
	HBufC8* logBuffer = NULL;
	
	User::LeaveIfError(log.Write(_L8("\n*** Mac value for hex data 0x000102 is = ")));
	
	// cleanup macResult and logBuffer
	__CLEAN_UP;
	
	convertMac.Set(macPtrFinal);
	macResult = convertMac.HexStringLC();
	
	User::LeaveIfError(log.Write(_L8("\n*** Mac value for hex data 0x000102030405060708090a0b0c0d0e0f is = ")));
	
	// cleanup macResult and logBuffer
	__CLEAN_UP
	
	User::LeaveIfError(log.Write(_L8("\n\n 0 tests failed out of 1 \t\n")));
	//------------------------------------------CLEANUP------------------------------------------

	//cleanup log, fs, macValueFinal, macValueIntermediate and macImpl
	CleanupStack::PopAndDestroy(5, macImpl);
	}

void MainL()
	{
	const TPtrC8 Key(key, KMacBlockSize);
	const TPtrC8 Data(data, KMacBlockSize);

	// pass the key and the source data for MAC evaluation
	__UHEAP_MARK;
		EvaluateMacL(Key,Data);
	__UHEAP_MARKEND;
	}

TInt E32Main()
	{
 	CTrapCleanup* cleanupStack = CTrapCleanup::New();
 	if (cleanupStack == NULL)
 		{
 		return KErrNoMemory;
 		}
 	TRAP_IGNORE(MainL());
 	delete cleanupStack;
 	return KErrNone;
	}
