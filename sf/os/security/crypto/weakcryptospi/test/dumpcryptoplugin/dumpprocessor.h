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
* definition of the dump processor classes
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef _DUMPPROCESSOR_H__
#define _DUMPPROCESSOR_H__

#include <e32base.h>
#include <f32file.h>

namespace CryptoSpi
	{
	//Interfaces Name
	_LIT(KHash, "hash");
	_LIT(KRandom, "random");
	_LIT(KSymmetricCipher, "symcipher");
	_LIT(KAsymmetricCipher, "asymcipher");
	_LIT(KSigner, "signer");
	_LIT(KVerifier, "verifier");
	_LIT(KKeyPairGenerator, "keypairgen");
	_LIT(KKeyAgreement, "keyagree");
	_LIT(KAsyncHash, "asynchash");
	_LIT(KAsyncRandom, "asyncrandom");
	_LIT(KAsyncSymmetricCipher, "asyncsymcipher");
	_LIT(KAsyncAsymmetricCipher, "asyncasymcipher");
	_LIT(KAsyncSigner, "asyncsigner");
	_LIT(KAsyncVerifier, "asyncverifier");
	_LIT(KAsyncKeyPairGenerator, "asynckeypairgen");
	_LIT(KAsyncKeyAgreement, "asynckeyagree");

	//Algorithms Name
	_LIT(KMd2, "md2");
	_LIT(KMd4, "md4");
	_LIT(KMd5, "md5");
	_LIT(KSha1, "sha1");
	_LIT(KDes, "des");
	_LIT(K3Des, "3des");
	_LIT(KRC2, "rc2");
	_LIT(KAes, "aes");
	_LIT(KRC4, "rc4");
	_LIT(KRsaCipher, "rsacipher");
	_LIT(KRsaSigner, "rsasigner");
	_LIT(KRsaVerifier, "rsaverifier");
	_LIT(KRsaKeyPair, "rsakeypair");
	_LIT(KDsaSigner, "dsasigner");
	_LIT(KDsaVerifier, "dsaverifier");
	_LIT(KDsaKeyPair, "dsakeypair");
	_LIT(KDhKeyPair, "dhkeypair");
	_LIT(KDhKeyAgreement, "dhkeyagree");

	_LIT(KNewLine, "\r\n");
	_LIT(KPressAnyKey, "Press any key to quit... \n");
	
	class CCharacteristicsAndPluginName;
	class CCharacteristics;
	class CExtendedCharacteristics;
	
	/**
	Class to store the command line parameters
	*/
	NONSHARABLE_CLASS(CDumpToolParameters) : public CBase
		{
	public:
		/**
		The options for the command line
		*/
		enum TOption
			{
			/**
			Help option
			*/
			EOptionHelp=0,
			/**
			Option to dump the extended characteristics
			*/			
			EOptionExtendedCharacteristic
			};	

		/**
		Create a CDumpToolParameters instance and leave it on the cleanup stack
		@return the pointer to newly created instance
		*/
		static CDumpToolParameters* NewLC();
		/**
		Desctructor
		*/		
		~CDumpToolParameters();
		/**
		Look for the command line option
		@return ETrue if found, otherwise EFalse
		*/
		TBool FindOption(TOption aOption) const;

	private:
		/**
		Constructor
		*/
		CDumpToolParameters();

	public:
		/**
		Command line list
		*/
		RArray<TInt> iOptions;
		/**
		The interface uid
		*/
		TInt32 iInterface;
		/**
		The algorithm uid
		*/
		TInt32 iAlgorithm;
		/**
		The dll name
		*/
		TFileName iDllName;
		/**
		Log file name
		*/
		TFileName iDumpFileName;
		/**
		Invalid options
		*/
		RPointerArray<HBufC> iInvalidOptions;
		};
	
	/**
	The class to retrieve the characteristics and extended characteristic,
	and output them to the text file
	*/
	NONSHARABLE_CLASS(CDumpToolProcessor) : public CBase
		{
	public:
		/**
		Create a CDumpToolProcessor instance
		@param aParams the input parameters
		@param aConsole the console pointer
		@return the pointer to the newly CDumpToolProcessor
		*/
		static CDumpToolProcessor* NewLC(const CDumpToolParameters& aParams, CConsoleBase* aConsole);
		
		/**
		Destructor
		*/
		~CDumpToolProcessor();
		
		/**
		Output the characteristics to the text file
		*/
		void ProcessL();
		
	private:
		/**
		Constructor
		@param aParams the command line parameters reference
		@param aConsole the console pointer
		*/
		CDumpToolProcessor(const CDumpToolParameters& aParams, CConsoleBase* aConsole);
		
		/**
		2nd phase constructor
		*/
		void ConstructL();
		
		/**
		Log the characteristics and dll name to the text file
		@param aElement the characteristics and Dll index instance
		*/
		void OutputCharcteristicsAndDllL(const CCharacteristicsAndPluginName& aElement);
		
		/**
		log the characteristics head
		@param aInterfaceUid the interface uid
		*/
		void OutputCharacteristicHeadL(TInt32 aInterfaceUid);
		
		/**
		Log the interface uid
		@param aInterfaceUid the interface uid
		*/
		void OutputInterfaceNameL(TInt32 aInterfaceUid);

		/**
		Log the algorithm uid
		@param aAlgorithmUid the algorithm uid
		*/		
		void OutputAlgorithmNameL(TInt32 aAlgorithmUid);

		/**
		Log the static characteristics
		@param aCharacter the characteristic instance
		*/		
		void OutputSpecificCharacteristicL(const CCharacteristics& aCharacter);
		
		/**
		Log the extended characteristics
		@param aCharacter the extended characteristic reference
		*/
		void OutputExtendedCharacteristicL(const CExtendedCharacteristics& aCharacter);
		
		/**
		Log the Hash operation mode
		@param aIndex the index of the operation mode
		@param aOperationMode the operation mode
		*/
		void OutputHashOperationModeL(TInt aIndex, TInt32 aOperationMode);
		
		/**
		Log the symmetric operation mode
		@param aIndex the index of the operation mode
		@param aOperationMode the operation mode
		*/		
		void OutputSymmetricOperationModeL(TInt aIndex, TInt32 aOperationMode);
		
		/**
		Log the padding mode
		@param aIndex the index of the padding mode
		@param aPaddingMode the padding mode
		*/		
		void OutputPaddingModeL(TInt aIndex, TInt32 aPaddingMode);
		
		/**
		Log functions
		*/
		void LogL(TRefByValue<const TDesC8> aFmt, ...);
		void LogL(TRefByValue<const TDesC> aFmt, ...);
		
		/**
		Dispaly functions
		*/
		void DisplayHelp();
		void DisplayInvalidOptions();
	private:
		/**
		The reference of the input parameters
		*/
		const CDumpToolParameters& iInputParams;
		/**
		Array of the characteristics
		*/
		RPointerArray<CCharacteristicsAndPluginName> iCharacters;
		/**
		File Server Handle
		*/
		RFs iFs;
		/**
		Log file handle
		*/
		RFile iLogFile;
		/**
		Log file name
		*/
		TFileName iDumpFileName;
		/**
		Console
		*/
		CConsoleBase* iConsole;
		};
	}
	
#endif //_DUMPPROCESSOR_H__



