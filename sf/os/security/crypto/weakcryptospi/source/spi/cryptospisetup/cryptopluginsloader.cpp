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
* crypto plugins loader implementation
* crypto spi state implementation
*
*/


/**
 @file
*/
#include "cryptopluginsloader.h"
#include <f32file.h>
#include <cryptospi/cryptospidef.h>
#include "cryptospiproperty.h"
#include <e32property.h>

using namespace CryptoSpi;

_LIT(KPluginDir, "z:\\sys\\bin\\");

HBufC *GetConfigFileNameL();

//
// Implementation of TCharacteristicsDll
//
TCharacteristicsDll::TCharacteristicsDll(const TAny* aCharacteristics, TInt aIndex)
:iCharacteristics(aCharacteristics), iDllIndex(aIndex)
	{
	}

void TCharacteristicsDll::ExternalizeL(RWriteStream& aStream)
	{
	const TCommonCharacteristics* common=reinterpret_cast<const TCommonCharacteristics*>(iCharacteristics);
	switch (common->iInterfaceUID)
		{
	case KHashInterface:
	case KAsyncHashInterface:
		{
		const THashCharacteristics* characteristics=reinterpret_cast<const THashCharacteristics*>(iCharacteristics);
		characteristics->ExternalizeL(aStream);
		}
		break;
	case KRandomInterface:
	case KAsyncRandomInterface:
		{
		const TRandomCharacteristics* characteristics=reinterpret_cast<const TRandomCharacteristics*>(iCharacteristics);
		characteristics->ExternalizeL(aStream);		
		}
		break;

	case KSymmetricCipherInterface:
	case KAsyncSymmetricCipherInterface:
		{
		const TSymmetricCipherCharacteristics* characteristics=reinterpret_cast<const TSymmetricCipherCharacteristics*>(iCharacteristics);
		characteristics->ExternalizeL(aStream);		
		}
		break;

	case KAsymmetricCipherInterface:
	case KAsyncAsymmetricCipherInterface:
		{
		const TAsymmetricCipherCharacteristics* characteristics=reinterpret_cast<const TAsymmetricCipherCharacteristics*>(iCharacteristics);
		characteristics->ExternalizeL(aStream);		
		}
		break;

	case KSignerInterface:
	case KVerifierInterface:
	case KAsyncSignerInterface:
	case KAsyncVerifierInterface:
		{
		const TAsymmetricSignatureCharacteristics* characteristics=reinterpret_cast<const TAsymmetricSignatureCharacteristics*>(iCharacteristics);
		characteristics->ExternalizeL(aStream);		
		}
		break;
	
	case KKeyAgreementInterface:
	case KAsyncKeyAgreementInterface:
		{
		const TKeyAgreementCharacteristics* characteristics=reinterpret_cast<const TKeyAgreementCharacteristics*>(iCharacteristics);
		characteristics->ExternalizeL(aStream);
		}
		break;

	case KKeypairGeneratorInterface:
	case KAsyncKeypairGeneratorInterface:
		{
		const TAsymmetricKeypairGeneratorCharacteristics* characteristics=reinterpret_cast<const TAsymmetricKeypairGeneratorCharacteristics*>(iCharacteristics);
		characteristics->ExternalizeL(aStream);
		}
		break;
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT			
	case KMacInterface:
	case KAsyncMacInterface:
		{
		const TMacCharacteristics* characteristics=reinterpret_cast<const TMacCharacteristics*>(iCharacteristics);
		characteristics->ExternalizeL(aStream);
		}
		break;
#endif		
	default:
		User::Leave(KErrNotSupported); 
		}
		
	aStream.WriteInt8L(iDllIndex);
	}

//
// Implementation of CCharacteristicDllIndexList
//
CCharacteristicDllIndexList* CCharacteristicDllIndexList::NewL()
	{
	CCharacteristicDllIndexList* self=new(ELeave) CCharacteristicDllIndexList();
	return self;
	}

CCharacteristicDllIndexList* CCharacteristicDllIndexList::NewLC()
	{
	CCharacteristicDllIndexList* self=new(ELeave) CCharacteristicDllIndexList();
	CleanupStack::PushL(self);
	return self;		
	}

CCharacteristicDllIndexList::CCharacteristicDllIndexList()
	{
	}

CCharacteristicDllIndexList::~CCharacteristicDllIndexList()
	{
	iCharacteristicList.Close();	
	}

void CCharacteristicDllIndexList::ExternalizeL(RWriteStream& aStream)
	{
	TInt count=iCharacteristicList.Count();
	aStream.WriteInt16L(count);
	for (TInt i=0;i<count;i++)
		{
		iCharacteristicList[i].ExternalizeL(aStream);
		}
	}

//
// Implementation of CCryptoPluginsLoader
//
CCryptoPluginsLoader* CCryptoPluginsLoader::NewL()
	{
	CCryptoPluginsLoader* self=NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CCryptoPluginsLoader* CCryptoPluginsLoader::NewLC()
	{
	CCryptoPluginsLoader* self=new(ELeave)CCryptoPluginsLoader();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CCryptoPluginsLoader::CCryptoPluginsLoader()
	{
	}

CCryptoPluginsLoader::~CCryptoPluginsLoader()
	{
	//Destroy the characteristic map
	THashMapIter<TInt32, CCharacteristicDllIndexList*> it(iInterfaceCharacteristicsMap);
	it.NextValue();
	while (it.CurrentValue())
		{
		delete *it.CurrentValue();
		it.NextValue();
		}
	iInterfaceCharacteristicsMap.Close();

	//Unload the plugin DLLs and release the array
	TInt dllCount=iPluginDllList.Count();
	for (TInt i=0;i<dllCount;i++)
		{
		iPluginDllList[i].Close();	
		}
	iPluginDllList.Close();
	iPluginNames.Close();
	}

void CCryptoPluginsLoader::ConstructL()
	{
	LoadPluginsL();
	}

void CCryptoPluginsLoader::LoadPluginsL()
	{
	HBufC *configFileBuf = GetConfigFileNameL();
	CleanupStack::PushL(configFileBuf);
	TPtr configFile(configFileBuf->Des());

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile file;
	User::LeaveIfError(file.Open(fs, configFile, KEntryAttNormal));
	CleanupClosePushL(file);

	TFileText ft;
	ft.Set(file);
	TFileName line;
	
	User::LeaveIfError(ft.Read(line));
		
	//Load all the plugins
	do
		{	
		TFileName filename;
		filename.Append(KPluginDir);
		filename.Append(line);

		//Load...
		RLibrary plugin;
		TInt err=plugin.Load(filename);
		if (err==KErrNone)
			{
			CleanupClosePushL(plugin);
			iPluginDllList.AppendL(plugin);
			CleanupStack::Pop(&plugin);
			iPluginNames.AppendL(line);
			}
		}
	while(ft.Read(line) == KErrNone);
		
		
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&fs);
	CleanupStack::PopAndDestroy(configFileBuf);
	
	BuildPluginCharacteristicsL();		
	}

void CCryptoPluginsLoader::BuildPluginCharacteristicsL()
	{
	TInt dllCount=iPluginDllList.Count();
	TInt interfaceCount=sizeof(KInterfacesUids)/sizeof(KInterfacesUids[0]);
	for (TInt i=0;i<dllCount;i++)
		{
		
		//find the enumeration function pointer
		EnumerateCharacteristicsFunc enumerateFunc=(EnumerateCharacteristicsFunc)iPluginDllList[i].Lookup(EEnumerateCharacteristicsOrdinal);
		if (enumerateFunc)
			{
			for (TInt j=0;j<interfaceCount;j++)
				{
				BuildInterfaceCharacteristicsL(KInterfacesUids[j], enumerateFunc, i);
				}
			}
		}
	}	

void CCryptoPluginsLoader::BuildInterfaceCharacteristicsL(TUid aInterfaceUid, 
													EnumerateCharacteristicsFunc aEntryFunc,
													TInt aDllIndex)
	{
	//Get the corresponding characteristics from the plug-in module
	TInt numPlugins=0;
	const TCharacteristics** p = aEntryFunc(aInterfaceUid, numPlugins);
	
	//characteristics are available
	if (p)
		{
		//look for it first
		CCharacteristicDllIndexList** charsListPtr=iInterfaceCharacteristicsMap.Find(aInterfaceUid.iUid);
		CCharacteristicDllIndexList* charsList=NULL;
		//create new one if it is not in the map
		if (!charsListPtr)
			{
			charsList=CCharacteristicDllIndexList::NewLC();
			iInterfaceCharacteristicsMap.InsertL(aInterfaceUid.iUid, charsList);
			CleanupStack::Pop(charsList);
			}
		else
			{
			//Use the existing one.
			charsList=*charsListPtr;
			}
		
		for (TInt i=0; i<numPlugins; ++i)
			{
			// The pointer should never be null, but try to prevent
			// rogue plug-ins from breaking the framework.
			ASSERT(p);
			if (p)
				{
				TCharacteristicsDll temp(*p, aDllIndex);
				charsList->iCharacteristicList.AppendL(temp);
				}
			p++;							
			}
		}
	}	

void CCryptoPluginsLoader::CreatePluginConfigPropertyL()
	{
	RProcess thisProcess;
	// sanity check that feature property category in common header equals SID of this process
	ASSERT(KCryptoSpiPropertyCat == thisProcess.SecureId());
	TSecurityPolicy readPolicy(TSecurityPolicy::EAlwaysPass);
	TSecurityPolicy writePolicy(thisProcess.SecureId());
	
	TInt count=iPluginNames.Count();
	TInt publishResult=KErrNone;
	for (TInt i=0;i<count;i++)
		{
		publishResult = RProperty::Define(KPluginsConfigurationKey+i, RProperty::EByteArray, readPolicy, writePolicy);
		if ((publishResult == KErrNone) || (publishResult == KErrAlreadyExists))
			{
			TSizeStream ss;
			RWriteStream ws(&ss);
			ws<<iPluginNames[i];
	
			HBufC8* buf = HBufC8::NewLC(ss.Size());
			TPtr8 bufPtr(buf->Des());
			RDesWriteStream dws(bufPtr);
			dws<<iPluginNames[i];
			dws.CommitL();
			publishResult = RProperty::Set(KCryptoSpiPropertyCat, KPluginsConfigurationKey+i, bufPtr);
			CleanupStack::PopAndDestroy(buf);
			}
		else
			{
			User::Leave(publishResult);
			}
		}
	publishResult = RProperty::Define(KPluginsConfigurationKeyCount, RProperty::EInt, readPolicy, writePolicy);	
	if ((publishResult == KErrNone) || (publishResult == KErrAlreadyExists))
		{
		publishResult = RProperty::Set(KCryptoSpiPropertyCat, KPluginsConfigurationKeyCount, count);
		}
	else
		{
		User::Leave(publishResult);
		}
	}

void CCryptoPluginsLoader::CreateInterfacePropertyL(TInt32 aInterface)
	{
	TSizeStream ss;
	RWriteStream ws(&ss);
	ws.WriteInt32L(0);
	DoCreateInterfacePropertyL(ws, aInterface);
	
	HBufC8* buf = HBufC8::NewLC(ss.Size());
	TPtr8 bufPtr(buf->Des());
	RDesWriteStream dws(bufPtr);
	dws.WriteInt32L(ss.Size());
	DoCreateInterfacePropertyL(dws, aInterface);
	dws.CommitL();
	User::LeaveIfError(RProperty::Set(KCryptoSpiPropertyCat, aInterface, bufPtr));
	CleanupStack::PopAndDestroy(buf);	
	}
	
void CCryptoPluginsLoader::DoCreateInterfacePropertyL(RWriteStream& aStream, TInt32 aInterface)
	{
	RProcess thisProcess;
	// sanity check that feature property category in common header equals SID of this process
	ASSERT(KCryptoSpiPropertyCat == thisProcess.SecureId());
	TSecurityPolicy readPolicy(TSecurityPolicy::EAlwaysPass);
	TSecurityPolicy writePolicy(thisProcess.SecureId());
	
	TInt publishResult = RProperty::Define(aInterface, RProperty::ELargeByteArray, readPolicy, writePolicy);
	if ((publishResult == KErrNone) || (publishResult == KErrAlreadyExists))
		{
		CCharacteristicDllIndexList** charsListPtr=iInterfaceCharacteristicsMap.Find(aInterface);
		if (charsListPtr)
			{
			(*charsListPtr)->ExternalizeL(aStream);
			}
		}
	else
		{
		User::Leave(publishResult);
		}		
	}
	
//
// Implementation of TSizeStream
//
void TSizeStream::DoWriteL(const TAny* /* aPtr */, TInt aLength)
	{
	iSize += aLength;
	}

_LIT(KPluginConfigFile, "z:\\resource\\cryptospi\\plug-ins.txt");
HBufC *GetConfigFileNameL()
	{
	// Check the command line. It should be empty or a single decimal digit
	TInt argsLen = User::CommandLineLength();

	if(argsLen == 0)
		{
		// No arguments so return the default config file name
		return KPluginConfigFile().AllocL();
		}

	// We only support a single digit argument
	if(argsLen != 1)
		{
		User::Leave(KErrArgument);
		}
	

	// Read the single char command line
	TBuf<1> argsBuf;
	User::CommandLine(argsBuf);
		
	// Check if it is a digit
	TChar ch(argsBuf[0]);
	if(!ch.IsDigit())
		{
		User::Leave(KErrArgument);
		}

	// Create buffer for config file name, inc space to append the digit
	HBufC *configFileBuf = HBufC::NewL(KPluginConfigFile().Length()+1);
	TPtr configFile(configFileBuf->Des());
		
	// Initialise it to the default file name
	configFile = KPluginConfigFile;
		
	// Append the digit to the config file name
	configFile.Append(ch);
	
	return configFileBuf;
	}

// End of file
