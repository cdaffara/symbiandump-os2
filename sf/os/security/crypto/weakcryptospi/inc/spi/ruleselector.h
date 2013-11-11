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
* Rule-based plugin selector definition
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_RULESELECTOR_H__
#define __CRYPTOAPI_RULESELECTOR_H__

#include <cryptospi/pluginselectorbase.h>
#include <cryptospi/cryptocharacteristics.h>
#include <e32hashtab.h>
#include <cryptospi/cryptoparams.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <rulecharacteristics.h>
#endif

namespace CryptoSpi
	{
	
	/**
	The operators of the plugin selection rule
	*/
	enum TRuleOperator
		{
		/**
		Operator ==
		*/
		EOpEqual,

		/**
		Operator !=
		*/
		EOpNotEqual,

		/**
		Operator >
		*/
		EOpGreaterThan,

		/**
		Operator <
		*/
		EOpLessThan,

		/**
		Operator >=
		*/
		EOpGreaterThanOrEqual,

		/**
		Operator <=
		*/
		EOpLessThanOrEqual,

		/**
		Ascending Sort of an characteristic
		*/
		EOpAscendingSort,

		/**
		Descending Sort of an characteristic
		*/
		EOpDescendingSort,

		/**
		The total number of operators
		*/
		EOpInvalid
		};
		
	class CRulesCharacteristicsAndPluginName;
	/**
	A single plugin selection rule definition
	*/
	NONSHARABLE_CLASS(CSelectionRuleContent) : public CBase
		{
	public:
		/**
		Create a selection rule content instance; the supported characteristic uid's are
		defined in plugincharacteristics.h; the supported operators are defined in ruleselector.h
		@param aInterfaceScope		The Interface scope of this rule
		@param aAlgorithmScope		The Algorithm scope of this rule
		@param aCharacteristicValue	The value of the rule of this characteristic
		@param aOperator			The operator of the rule
		@param aIsOptional			Whether this rule is optional or mandatory
		@return A pointer to a CSelectionRuleContent instance
		*/
		IMPORT_C static CSelectionRuleContent* NewL(TUid aInterfaceScope,
													TUid aAlgorithmScope,
													CCryptoParam* aCharacteristicValue,
													TRuleOperator aOperator,
													TBool aIsOptional);
		/**
		Destructor
		*/
		IMPORT_C ~CSelectionRuleContent();

		/**
		 * @internalComponent
		 * 
		 * Get the Interface Scope of this rule
		 * @return the Interface Scope Uid of this rule
		 **/
		TUid GetInterfaceScope();

		/**
		 * @internalComponent
		 * 
		 * Get the Algorithm Scope of this rule
		 * @return the Algorithm Scope Uid of this rule
		 **/
		TUid GetAlgorithmScope();

		/**
		 * @internalComponent
		 * 
		 * Get the Rule operator
		 * @return The Rule operator
		 **/
		TInt GetOperator();

		/**
		 * @internalComponent
		 * 
		 * Get the value of the rule of this characteristic 
		 * @return The value of the rule of this characteristic 
		 **/
		const CCryptoParam* GetCharacteristicValue();

		/**
		 * @internalComponent
		 * 
		 * returns whether the rule is optional
		 * @return Whether the rule is optional
		 **/
		TBool IsOptionalRule();

	private:
		/**
		Constructor
		*/
		CSelectionRuleContent(TUid aInterfaceScope,
							TUid aAlgorithmScope,
							CCryptoParam* aCharacteristicValue,
							TRuleOperator aOperator,
							TBool aIsOptional);

		/**
		The scopes of this rule 
		*/
		TUid iInterfaceScope;
		TUid iAlgorithmScope;

		/**
		The value of the rule of this characteristic 
		*/
		const CCryptoParam* iCharacteristicValue;
		
		/**
		The Operator of the Characteristic
		*/
		TRuleOperator iOperator;

		/**
		if the rule on this characteristic is optional
		*/
		TBool iIsOptional;
		};

	
	/**
	Definition of rule-based plugin selection rules
	*/
	NONSHARABLE_CLASS(CSelectionRules) : public CBase
		{
	public:
		/**
		create a Selection Rule instance which will hold the rules to filter the plugins
		@return a pointer to a CSelectionRules instance
		*/
		IMPORT_C static CSelectionRules* NewL();
		
		/**
		Destructor
		*/
		IMPORT_C ~CSelectionRules();

		/**
		Add a selection rule the this object
		@param aSelectionRule	The rule to be added
		*/
		IMPORT_C void AddSelectionRuleL(CSelectionRuleContent* aSelectionRule);
		
		/**
		 * @internalComponent
		 *
		 * Get the selection rules
		 * @return The reference of the rules
		 **/
		RPointerArray<CSelectionRuleContent>& GetSelectionRules();

	private:
		/**
		Constructor
		*/
		CSelectionRules();
		
		/**
		a group of plugin rules
		*/
		RPointerArray<CSelectionRuleContent> iRules;
		};
	
	/**
	Selector apply the rule-based selection to find the plugin.
	*/
	NONSHARABLE_CLASS(CRuleSelector) : public CBase, public MPluginSelector
		{
	public:
		/**
		NewL create the ruled-based selector
		@param aRules The Rules to select the plugins
		@return a pointer to a CRuleSelector instance
		*/
		IMPORT_C static CRuleSelector* NewL(CSelectionRules* aRules);
		IMPORT_C static CRuleSelector* NewLC(CSelectionRules* aRules);
		
		/**
		Destructor
		*/
		IMPORT_C ~CRuleSelector();
				
		/**
		 * @deprecated
		 * 
		 * virtual function from MPluginSelector
		 */
		virtual void CreateHashL(CHash*& aHash,
											TUid aAlgorithmUid,
											TUid aOperationMode,
											const CKey* aKey,
											const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector						
		virtual void CreateRandomL(CRandom*& aRandom,
											TUid aAlgorithmUid,
											const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateSymmetricCipherL(CSymmetricCipher*& aCipher,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aCryptoMode,
											TUid aOperationMode,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateAsymmetricCipherL(CAsymmetricCipher*& aCipher,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aCryptoMode,
											TUid aPaddingMode,									
											const CCryptoParams* aAlgorithmParams);
											
		//virtual function from MPluginSelector
		virtual void CreateSignerL(CSigner*& aSigner,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateVerifierL(CVerifier*& aVerifier,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateKeyPairGeneratorL(CKeyPairGenerator*& aKeyPairGenerator,
											TUid aKeyAlgorithmUid,
											const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateKeyAgreementL(CKeyAgreement*& aKeyAgreement,
											TUid aAlgorithmUid,
											const CKey& aPrivateKey,
											const CCryptoParams* aAlgorithmParams);

		/**
		 * @deprecated
		 * 
		 * virtual function from MPluginSelector
		 */
		virtual void CreateAsyncHashL(CAsyncHash*& aHash,
										TUid aAlgorithmUid,
										TUid aOperationMode,
										const CKey* aKey,
										const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateAsyncRandomL(CAsyncRandom*& aRandom,
										TUid aAlgorithmUid,
										const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateAsyncSymmetricCipherL(CAsyncSymmetricCipher*& aCipher,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aCryptoMode,
										TUid aOperationMode,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateAsyncAsymmetricCipherL(CAsyncAsymmetricCipher*& aCipher,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aCryptoMode,
										TUid aPaddingMode,																						
										const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateAsyncSignerL(CAsyncSigner*& aSigner,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateAsyncVerifierL(CAsyncVerifier*& aVerifier,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateAsyncKeyPairGeneratorL(CAsyncKeyPairGenerator*& aKeyPairGenerator,
										TUid aAlgorithmUid,
										const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateAsyncKeyAgreementL(CAsyncKeyAgreement*& aKeyAgreement,
										TUid aAlgorithmUid,
										const CKey& aPrivateKey,
										const CCryptoParams* aAlgorithmParams);			
	
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
		
		//virtual function from MPluginSelector	
		virtual void CreateMacL(CMac*& aMac,
								const TUid aAlgorithmUid,
								const CKey& aKey,
								const CCryptoParams* aAlgorithmParams);
				
		//virtual function from MPluginSelector
		virtual void CreateAsyncMacL(CAsyncMac*& aMac,
								const TUid aAlgorithmUid,
								const CKey& aKey,
								const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateHashL(CHash*& aHash,
								TUid aAlgorithmUid,
								const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateAsyncHashL(CAsyncHash*& aHash,
								TUid aAlgorithmUid,
								const CCryptoParams* aAlgorithmParams);
		
#endif
		
		/** @internalComponent */
		static TInt AscendCreatorName(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendCreatorName(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);
		
		/** @internalComponent */
		static TInt AscendExtendedTDesC8L(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);
		
		/** @internalComponent */
		static TInt DescendExtendedTDesC8L(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendMaxConcurrencySupported(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);
		
		/** @internalComponent */
		static TInt DescendMaxConcurrencySupported(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendLatency(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendLatency(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendThroughput(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendThroughput(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendHashBlockSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendHashBlockSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);
		
		/** @internalComponent */
		static TInt AscendSymmetricCipherBlockSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendSymmetricCipherBlockSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendHashOutputSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendHashOutputSize(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);
		
		/** @internalComponent */
		static TInt AscendRandomBlockingMode(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendRandomBlockingMode(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendSymmetricCipherKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendSymmetricCipherKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendAsymmetricCipherKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendAsymmetricCipherKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendAsymmetricSignatureKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendAsymmetricSignatureKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendSymmetricCipherKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendSymmetricCipherKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendAsymmetricCipherKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendAsymmetricCipherKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendAsymmetricSignatureKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendAsymmetricSignatureKeySupport(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendAsymmetricKeypairGeneratorKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt DescendAsymmetricKeypairGeneratorKeyLength(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);

		/** @internalComponent */
		static TInt AscendExtendedCharacteristicL(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);
		
		/** @internalComponent */
		static TInt DescendExtendedCharacteristicL(const CRulesCharacteristicsAndPluginName& aLeft, const CRulesCharacteristicsAndPluginName& aRight);
	
	private:

		/**
		Constructor
		@param aRules the Rules to select the plugins
		*/
		CRuleSelector();

		/**
		Second Phase Constructor
		*/
		void ConstructL(CSelectionRules* aRules);

		void PerformFilterL(CSelectionRules* aRules);

		void PerformAlgorithmFilterL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
									CSelectionRuleContent* aRules, TInt& aLastPreference);

		TBool FilterCommonCharacteristicsL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
											CSelectionRuleContent* aRules, TInt& aLastPreference);

		TBool FilterNonCommonCharacteristicsL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
												CSelectionRuleContent* aRules, TInt& aLastPreference);

		TBool FilterExtendedCharacteristicsL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
												CSelectionRuleContent* aRules, TInt& aLastPreference);

		void FilterTInt32L(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics,
							CSelectionRuleContent* aRules, TInt& aLastPreference, TInt32 aCharValue);
		
		void FilterMultiTInt32L(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristicsDLL,
								CSelectionRuleContent* aRules, TInt& aLastPreference,
								TBool aSupport);

		void FilterCreatorNameL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
								CSelectionRuleContent* aRules, TInt& aLastPreference);

		void FilterExtendedTDesC8L(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
									CSelectionRuleContent* aRules, TInt& aLastPreference, const TDesC8& aCharValue);
										
		void FilterTBoolL(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics,
							CSelectionRuleContent* aRules, TInt& aLastPreference, TBool aCharValue);

		void FilterTIntL(TInt32 aInterfaceUid, RPointerArray<CRulesCharacteristicsAndPluginName>& aCharacteristicsDLL,
							CSelectionRuleContent* aRules, TInt& aLastPreference, TInt aCharValue);

		void TryAddToOptionalCharListL(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics,
										CSelectionRuleContent* aRules);

		void AddToCharListL(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics,
							TInt& aLastPreference);
		
		void AddOptionalToCharListL(TInt32 aInterfaceUid, CRulesCharacteristicsAndPluginName& aCharacteristics);
		
		void ConstructMapAL();

		void SetSearchRecord(TUid aInterfaceUid, TInt aValue);
		
		/**
		Loads the crypto plugins
		*/
		void LoadPluginsL();

		TInt FindPlugin(TUid aInterfaceUid, TUid aAlgorithmUid, TUid& aImplementationId, TFileName& aDllName);

		/**
		The selection rules to filter the plugins
		*/
		CSelectionRules* iSelectionRules;

		/**
		Maps of the interface UID and the pointer of algorithm's characteristic array
		They are used alternatively to filter from one array to the other
		The final array will be set to iActiveInterfaceCharacteristics_Map for plugin lookup
		*/
		RHashMap<TInt32, RPointerArray<CRulesCharacteristicsAndPluginName>*> iSelectedInterfaceCharacteristics_MapA;
		RHashMap<TInt32, RPointerArray<CRulesCharacteristicsAndPluginName>*> iSelectedInterfaceCharacteristics_MapB;
		
		/**
		Pointer of the active map of the interface UID and the characteristic list
		*/
		const RHashMap<TInt32, RPointerArray<CRulesCharacteristicsAndPluginName>*>* iActiveInterfaceCharacteristics_Map;
		
		/**
		Records the next index to try
		*/
		RHashMap<TInt32, TInt> iNextTryCharacteristicsIndex;
		
		/**
		To indicate whether to use iSelectedInterfaceCharacteristics_MapA 
		or iSelectedInterfaceCharacteristics_MapA to filter
		*/
		TBool iUseMapAToFilter;
		
		/**
		This is use to indicate whether to alternate the maps for next rule filtering or not
		*/
		TBool iToggleUseMap;
		
		/**
		The plugin DLL list, which holds all the plugin DLLs
		*/
		RArray<RLibrary> iPluginDllList;
		};
	}

#endif //__CRYPTOAPI_RULESELECTOR_H__
