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

#include "dumpprocessor.h"
#include <e32cons.h>
#include <cryptospi/cryptospidef.h>
#include <bacline.h>
#include <cryptospi/cryptocharacteristics.h>
#include <cryptospi/cryptospistateapi.h>

using namespace CryptoSpi;

//Copy-right, Tool name format
_LIT(KShortName, "Dump CryptoSpi Tool");
_LIT(KName, "Symbian Dump CryptoSpi Tool");
_LIT(KCopyright, "Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).  All rights reserved.");
_LIT(KDone, "Complete Dumping... \n");

//dumpcryptoplugin options
_LIT(KAll, "-all");
_LIT(KAllShort, "-a");

_LIT(KInterface, "-interface");
_LIT(KInterfaceShort, "-i");

_LIT(KAlgorithm, "-algorithm");
_LIT(KAlgorithmShort, "-al");

_LIT(KExtended, "-extended");
_LIT(KExtendedShort, "-e");

_LIT(KDll, "-dll");
_LIT(KDllShort, "-d");

_LIT(KHelp, "-help");
_LIT(KHelpShort, "-h");

_LIT(KOutputFile, "-out");
_LIT(KOutputFileShort, "-o");


//Error messages
_LIT(KMissingInterfaceName, "Missing Interface Name. \n");
_LIT(KInvalidInterfaceName, "Invalid Interface Name. \n");
_LIT(KMissingAlgorithmName, "Missing Algorithm Name. \n");
_LIT(KInvalidAlgorithmName, "Invalid Algorithm Name. \n");
_LIT(KMissingDllName, "Missing Plugin Dll Name. \n");
_LIT(KMissingOutputName, "Missing Output Name. \n");
_LIT(KError, "Error=%d \n");

/**
The method converts the given interface name to interface uid
*/
TInt ConvertToInterfaceId(const TDesC& aInterfaceName, TInt32& aInterfaceId)
	{
	if (aInterfaceName.CompareF(KHash)==0)
		{
		aInterfaceId=KHashInterface;
		return KErrNone;
		}
	
	if (aInterfaceName.CompareF(KRandom)==0)
		{
		aInterfaceId=KRandomInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KSymmetricCipher)==0)
		{
		aInterfaceId=KSymmetricCipherInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KAsymmetricCipher)==0)
		{
		aInterfaceId=KAsymmetricCipherInterface;
		return KErrNone;
		}
	
	if (aInterfaceName.CompareF(KSigner)==0)
		{
		aInterfaceId=KSignerInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KVerifier)==0)
		{
		aInterfaceId=KVerifierInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KKeyPairGenerator)==0)
		{
		aInterfaceId=KKeypairGeneratorInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KKeyAgreement)==0)
		{
		aInterfaceId=KKeyAgreementInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KAsyncHash)==0)
		{
		aInterfaceId=KAsyncHashInterface;
		return KErrNone;
		}
	
	if (aInterfaceName.CompareF(KAsyncRandom)==0)
		{
		aInterfaceId=KAsyncRandomInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KAsyncSymmetricCipher)==0)
		{
		aInterfaceId=KAsyncSymmetricCipherInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KAsyncAsymmetricCipher)==0)
		{
		aInterfaceId=KAsyncAsymmetricCipherInterface;
		return KErrNone;
		}
	
	if (aInterfaceName.CompareF(KAsyncSigner)==0)
		{
		aInterfaceId=KAsyncSignerInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KAsyncVerifier)==0)
		{
		aInterfaceId=KAsyncVerifierInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KAsyncKeyPairGenerator)==0)
		{
		aInterfaceId=KAsyncKeypairGeneratorInterface;
		return KErrNone;
		}

	if (aInterfaceName.CompareF(KAsyncKeyAgreement)==0)
		{
		aInterfaceId=KAsyncKeyAgreementInterface;
		return KErrNone;
		}
		
	return KErrNotFound;
	}

/**
The method converts the given algorithm name to algorithm uid
*/
TInt ConvertToAlgorithmId(const TDesC& aAlgorithmName, TInt32& aAlgorithmId)
	{
	if (aAlgorithmName.CompareF(KMd2)==0)
		{
		aAlgorithmId=KAlgorithmHashMd2;
		return KErrNone;
		}
		
	if (aAlgorithmName.CompareF(KMd5)==0)
		{
		aAlgorithmId=KAlgorithmHashMd5;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(KMd4)==0)
		{
		aAlgorithmId=KAlgorithmHashMd4;
		return KErrNone;
		}
		
	if (aAlgorithmName.CompareF(KSha1)==0)
		{
		aAlgorithmId=KAlgorithmHashSha1;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(KDes)==0)
		{
		aAlgorithmId=KAlgorithmCipherDes;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(K3Des)==0)
		{
		aAlgorithmId=KAlgorithmCipher3Des;
		return KErrNone;
		}
		
	if (aAlgorithmName.CompareF(KRC2)==0)
		{
		aAlgorithmId=KAlgorithmCipherRc2;
		return KErrNone;
		}
		
	if (aAlgorithmName.CompareF(KAes)==0)
		{
		aAlgorithmId=KAlgorithmCipherAes;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(KRC4)==0)
		{
		aAlgorithmId=KAlgorithmCipherArc4;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(KRsaCipher)==0)
		{
		aAlgorithmId=KAlgorithmCipherRsa;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(KRsaSigner)==0)
		{
		aAlgorithmId=KAlgorithmSignerRsa;
		return KErrNone;
		}
		
	if (aAlgorithmName.CompareF(KRsaVerifier)==0)
		{
		aAlgorithmId=KAlgorithmVerifierRsa;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(KRsaKeyPair)==0)
		{
		aAlgorithmId=KAlgorithmRSAKeyPairGenerator;
		return KErrNone;
		}
		
	if (aAlgorithmName.CompareF(KDsaSigner)==0)
		{
		aAlgorithmId=KAlgorithmSignerDsa;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(KDsaVerifier)==0)
		{
		aAlgorithmId=KAlgorithmVerifierDsa;
		return KErrNone;
		}
		
	if (aAlgorithmName.CompareF(KDsaKeyPair)==0)
		{
		aAlgorithmId=KAlgorithmDSAKeyPairGenerator;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(KDhKeyPair)==0)
		{
		aAlgorithmId=KAlgorithmDHKeyPairGenerator;
		return KErrNone;
		}

	if (aAlgorithmName.CompareF(KDhKeyAgreement)==0)
		{
		aAlgorithmId=KAlgorithmKeyAgreementDH;
		return KErrNone;
		}
		
	return KErrNotFound;
	}


/**
Displays tool name and copy-right informations.
*/
void BoilerPlate(CConsoleBase* console) 
	{
	console->Printf(KNewLine);
	console->Printf(KName);
	console->Printf(KNewLine);	
	console->Printf(KCopyright);
	console->Printf(KNewLine);
	console->Printf(KNewLine);
	}

/**
The method retrieves the command line parameters and build the CDumpToolParameters object.
*/

void ExtractOptionL(const CCommandLineArguments& aInput, CDumpToolParameters& aParam, CConsoleBase* aConsole)
	{
	TInt count=aInput.Count();
	TInt i=0;
	while (i<count-1)
	 	{
		i++;
		
		//Handle -a and -all		
		if ((aInput.Arg(i).Compare(KAll)==0)||(aInput.Arg(i).Compare(KAllShort)==0))
			{
			continue;
			}		
		
		//Handle -i and -interface
		if ((aInput.Arg(i).Compare(KInterface)==0)||(aInput.Arg(i).Compare(KInterfaceShort)==0))
			{
			//Move to interface name
			i++;
			if (i>count-1)
				{
				aConsole->Printf(KMissingInterfaceName);
				User::Leave(KErrArgument);	
				}
			
			//Currently point to interface name
			if (KErrNone!=ConvertToInterfaceId(aInput.Arg(i), aParam.iInterface))
				{
				//Invalid interface name
				aConsole->Printf(KInvalidInterfaceName);
				User::Leave(KErrArgument);
				}
			continue;
			}
			
		//Handle -al and -algorithm
		if ((aInput.Arg(i).Compare(KAlgorithm)==0)||(aInput.Arg(i).Compare(KAlgorithmShort)==0))
			{
			//Move to algorithm name
			i++;
			if (i>count-1)
				{
				aConsole->Printf(KMissingAlgorithmName);
				User::Leave(KErrArgument);	
				}
			
			//Currently point to algorithm name
			if (KErrNone!=ConvertToAlgorithmId(aInput.Arg(i), aParam.iAlgorithm))
				{
				//Invalid algorithm name
				aConsole->Printf(KInvalidAlgorithmName);
				User::Leave(KErrArgument);
				}
			continue;
			}
		
		//Handle -e and -extended
		if ((aInput.Arg(i).Compare(KExtended)==0)||(aInput.Arg(i).Compare(KExtendedShort)==0))
			{
			aParam.iOptions.AppendL(CDumpToolParameters::EOptionExtendedCharacteristic);
			continue;
			}		
		
		//Handle -d and -dll
		if ((aInput.Arg(i).Compare(KDll)==0)||(aInput.Arg(i).Compare(KDllShort)==0))
			{
			i++;
			if (i>count-1)
				{
				aConsole->Printf(KMissingDllName);
				User::Leave(KErrArgument);	
				}
			aParam.iDllName=aInput.Arg(i);
			continue;
			}

		//Handle -o and -out
		if ((aInput.Arg(i).Compare(KOutputFile)==0)||(aInput.Arg(i).Compare(KOutputFileShort)==0))
			{
			i++;
			if (i>count-1)
				{
				aConsole->Printf(KMissingOutputName);
				User::Leave(KErrArgument);	
				}
			aParam.iDumpFileName=aInput.Arg(i);
			continue;
			}
		
		//Handle -h and -help
 		if ((aInput.Arg(i).Compare(KHelp)==0)||(aInput.Arg(i).Compare(KHelpShort)==0))
 			{
  			aParam.iOptions.AppendL(CDumpToolParameters::EOptionHelp);
  			continue;
   			}
   			
   		HBufC* temp=aInput.Arg(i).AllocLC();
   		aParam.iInvalidOptions.AppendL(temp);
   		CleanupStack::Pop(temp);
	 	}
	}


void DoProcessL(CConsoleBase* aConsole)
	{
	//Get the command line arguments
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	
	//Construct the parameters holder
	CDumpToolParameters* params = CDumpToolParameters::NewLC();
	
	//Retrieve the command line options
	ExtractOptionL(*args, *params, aConsole);
	
	//Process the command line and output the characteristics.
	CDumpToolProcessor* processor=CDumpToolProcessor::NewLC(*params, aConsole);
	processor->ProcessL();
	aConsole->Printf(KNewLine);
	CleanupStack::PopAndDestroy(processor);			
	CleanupStack::PopAndDestroy(2, args); //	params, args, 
	}

/**
Main function to retrieve and log the characteristics
*/
void DoMainL()
	{
	//New a console and print tool information
	CConsoleBase* console = Console::NewL(KShortName, TSize(KDefaultConsWidth, KDefaultConsHeight));
	CleanupStack::PushL(console);
	BoilerPlate(console);
	TRAPD(err, DoProcessL(console));
	if (err!=KErrArgument && err!=KErrNone)
		{
		console->Printf(KError, err);
		}
	if (err!=KErrNone)
		{
		console->Printf(KPressAnyKey);
		console->Getch();			
		}
	console->Printf(KDone);
	//console->Getch();
	CleanupStack::PopAndDestroy(console); //	params, args, console
	}

/**
@return KErrNone (0) if successful, KErrMemory if out of memory
otherwise error result from DoMainL
*/
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}
				
	TRAPD(err, DoMainL());
	delete trapCleanup;
	__UHEAP_MARKEND;
	return err;
	}
		




