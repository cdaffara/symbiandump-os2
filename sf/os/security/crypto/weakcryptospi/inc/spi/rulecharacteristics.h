/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 @released
*/

#ifndef CRYPTOAPI_RULECHARACTERISTICS_H
#define CRYPTOAPI_RULECHARACTERISTICS_H

#include <cryptospi/pluginselectorbase.h>
#include <cryptospi/cryptocharacteristics.h>
#include <e32hashtab.h>
#include <cryptospi/cryptoparams.h>

namespace CryptoSpi
	{
	/**
	 *
	 * The class holds characteristics and dll index.
	 **/
	NONSHARABLE_CLASS(CRulesCharacteristicsAndPluginName) : public CCharacteristicsAndPluginName
		{
	public:
		/**
		 * Create a new instance of CRulesCharacteristicsAndPluginName.
		 *
		 * @param aInterface the interface uid
		 * @return the pointer of the new object
		 **/		
		static CRulesCharacteristicsAndPluginName* NewL(TInt32 aInterface);
		
		/**
		 * Create a new instance of CRulesCharacteristicsAndPluginName.
		 * Leave it on the cleanup stack.
		 *
		 * @param aInterface the interface uid
		 * @return the pointer of the new object
		 **/				
		static CRulesCharacteristicsAndPluginName* NewLC(TInt32 aInterface);
		
		/**
		Destructor
		*/		
		virtual ~CRulesCharacteristicsAndPluginName();
		
	private:
		/**
		Constructors
		*/
		CRulesCharacteristicsAndPluginName();
		void ConstructL(TInt32 aInterface);
		
	public:
		/**
		This is the extended characteristic uid that need to be sorted.
		*/
		TInt32 iSortUid;
		};
		
	}

#endif //CRYPTOAPI_RULECHARACTERISTICS_H
