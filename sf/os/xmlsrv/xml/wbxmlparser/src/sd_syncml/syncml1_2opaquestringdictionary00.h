// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Plugin interface
// 
//


#ifndef __SYNCML1_2OPAQUESTRINGDICTIONARY00_H__
#define __SYNCML1_2OPAQUESTRINGDICTIONARY00_H__

#include <e32std.h>
#include <stringpool.h>

#include <xml/plugins/stringdictionary.h>

class CSyncml1_2OpaqueStringDictionary00 : public CBase, public Xml::MStringDictionary
/**
The Syncml1_2OpaqueStringDictionary00 class is a derived class defining the API of a single string dictionary. 

Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between 
strings and WBXML tokens should also be maintained.

@internalTechnology
*/
	{
public:

	static Xml::MStringDictionary* NewL(TAny* aStringPool);
	virtual ~CSyncml1_2OpaqueStringDictionary00();
	void ConstructL();

	void ElementL(TInt aToken, RString& aElement) const;
	void AttributeL(TInt aToken, RString& aAttribute) const;
	void AttributeValuePairL(TInt aToken, RString& aAttribute, RString& aValue) const;
	void AttributeValueL(TInt aToken, RString& aValue) const;

	TBool CompareThisDictionary(const RString& aDictionaryDescription) const;

	TInt SwitchCodePage(TInt aCodePage);
	void PublicIdentifier(RString& aPubId);
	void NamespaceUri(RString& aUri);

	void Release();

public:

	// CodePage tokens.
	//
	// Note: There will be correlation between these token values and their
	// strings via the CDictionaryCodePage using the RStringPool.

	enum TTokenValue {
		// CodePage 00
		// -//SYNCML//DTD DevInf 1.1//EN
		ECTCap						= 0x05,
		ECTType						= 0x06,	
		EDataStore					= 0x07,	
		EDataType					= 0x08,	
		EDevID						= 0x09,	
		EDevInf						= 0x0A,	
		EDevTyp						= 0x0B,	
		EDisplayName				= 0x0C,	
		EDSMem						= 0x0D,	
		EExt						= 0x0E,	
		EFwV						= 0x0F,	
		EHwV						= 0x10,	
		EMan						= 0x11,	
		EMaxGUIDSize				= 0x12,	
		EMaxID						= 0x13,	
		EMaxMem						= 0x14,	
		EMod						= 0x15,	
		EOEM						= 0x16,	
		EParamName					= 0x17,	
		EPropName					= 0x18,	
		ERx							= 0x19,	
		ERxPref	     				= 0x1A,	
		ESharedMem 				 	= 0x1B,	
		EMaxSize					= 0x1C,	
		ESourceRef					= 0x1D,	
		ESwV						= 0x1E,	
		ESyncCap					= 0x1F,	
		ESyncType					= 0x20,	
		ETx							= 0x21,	
		ETxPref	    				= 0x22,	
		EValEnum					= 0x23,	
		EVerCT						= 0x24,	
		EVerDTD						= 0x25,	
		EXnam						= 0x26,	
		EXval						= 0x27,	
		EUTC						= 0x28,	
		ESupportNumberOfChanges		= 0x29,	
		ESupportLargeObjs			= 0x2A,
		EProperty					= 0x2B,
		EPropParam					= 0x2C,
		EMaxOccur					= 0x2D,
		ENoTruncate					= 0x2E,
		EFilterRx					= 0x30,
		EFilterCap					= 0x31,
		EFilterKeyword				= 0x32,
		EFieldLevel					= 0x33,
		ESupportHierarchicalSync	= 0x34
		};


private:

	CSyncml1_2OpaqueStringDictionary00(RStringPool* aStringPool);

private:

/**
The StringPool for this string dictionary.
We don't own this.
*/
	RStringPool				iStringPool;

	Xml::CDictionaryCodePage*	iCodepage00Table;

	TInt					iCodePage;

	};


const TInt iCodePage00[] = 
	{
	CSyncml1_2OpaqueStringDictionary00::ECTCap,
	CSyncml1_2OpaqueStringDictionary00::ECTType,	
	CSyncml1_2OpaqueStringDictionary00::EDataStore,	
	CSyncml1_2OpaqueStringDictionary00::EDataType,	
	CSyncml1_2OpaqueStringDictionary00::EDevID,	
	CSyncml1_2OpaqueStringDictionary00::EDevInf,	
	CSyncml1_2OpaqueStringDictionary00::EDevTyp,	
	CSyncml1_2OpaqueStringDictionary00::EDisplayName,	
	CSyncml1_2OpaqueStringDictionary00::EDSMem,	
	CSyncml1_2OpaqueStringDictionary00::EExt,	
	CSyncml1_2OpaqueStringDictionary00::EFwV,	
	CSyncml1_2OpaqueStringDictionary00::EHwV,	
	CSyncml1_2OpaqueStringDictionary00::EMan,	
	CSyncml1_2OpaqueStringDictionary00::EMaxGUIDSize,	
	CSyncml1_2OpaqueStringDictionary00::EMaxID,	
	CSyncml1_2OpaqueStringDictionary00::EMaxMem,	
	CSyncml1_2OpaqueStringDictionary00::EMod,	
	CSyncml1_2OpaqueStringDictionary00::EOEM,	
	CSyncml1_2OpaqueStringDictionary00::EParamName,	
	CSyncml1_2OpaqueStringDictionary00::EPropName,	
	CSyncml1_2OpaqueStringDictionary00::ERx,	
	CSyncml1_2OpaqueStringDictionary00::ERxPref,	
	CSyncml1_2OpaqueStringDictionary00::ESharedMem,	
	CSyncml1_2OpaqueStringDictionary00::EMaxSize,	
	CSyncml1_2OpaqueStringDictionary00::ESourceRef,	
	CSyncml1_2OpaqueStringDictionary00::ESwV,	
	CSyncml1_2OpaqueStringDictionary00::ESyncCap,	
	CSyncml1_2OpaqueStringDictionary00::ESyncType,	
	CSyncml1_2OpaqueStringDictionary00::ETx,	
	CSyncml1_2OpaqueStringDictionary00::ETxPref,	
	CSyncml1_2OpaqueStringDictionary00::EValEnum,	
	CSyncml1_2OpaqueStringDictionary00::EVerCT,	
	CSyncml1_2OpaqueStringDictionary00::EVerDTD,	
	CSyncml1_2OpaqueStringDictionary00::EXnam,	
	CSyncml1_2OpaqueStringDictionary00::EXval,	
	CSyncml1_2OpaqueStringDictionary00::EUTC,	
	CSyncml1_2OpaqueStringDictionary00::ESupportNumberOfChanges,	
	CSyncml1_2OpaqueStringDictionary00::ESupportLargeObjs,
	CSyncml1_2OpaqueStringDictionary00::EProperty,
	CSyncml1_2OpaqueStringDictionary00::EPropParam,
	CSyncml1_2OpaqueStringDictionary00::EMaxOccur,
	CSyncml1_2OpaqueStringDictionary00::ENoTruncate,
	CSyncml1_2OpaqueStringDictionary00::EFilterRx,
	CSyncml1_2OpaqueStringDictionary00::EFilterCap,
	CSyncml1_2OpaqueStringDictionary00::EFilterKeyword,
	CSyncml1_2OpaqueStringDictionary00::EFieldLevel,
	CSyncml1_2OpaqueStringDictionary00::ESupportHierarchicalSync,
	 0}; // ok to end with zero as this is used by a global token

#endif //__SYNCML1_2OPAQUESTRINGDICTIONARY00_H__
