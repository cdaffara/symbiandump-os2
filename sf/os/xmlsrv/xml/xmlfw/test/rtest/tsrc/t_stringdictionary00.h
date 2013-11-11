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


#ifndef __T_STRINGDICTIONARY00_H__
#define __T_STRINGDICTIONARY00_H__

#include <e32std.h>
#include <stringpool.h>

#include <xml/plugins/stringdictionary.h>


class CTestStringDictionary00 : public CBase, public Xml::MStringDictionary
/**
The TestStringDictionary00 class is a derived class defining the API of a single string dictionary. 

Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between 
strings and WBXML tokens should also be maintained.
*/
	{
public:

	static Xml::MStringDictionary* NewL(TAny* aStringPool);
	virtual ~CTestStringDictionary00();

	void ConstructL();

	void ElementL(TInt aToken, RString& aElement) const;
	void AttributeL(TInt aToken, RString& aAttribute) const;
	void AttributeValuePairL(TInt aToken, RString& aAttribute, RString& aValue) const;
	void AttributeValueL(TInt aToken, RString& aValue) const;

	TBool CompareThisDictionary(const RString& aDictionaryDescription) const;

	TInt SwitchCodePage(TInt aCodePage);
	void PublicIdentifier(RString& aPubId);
	void NamespaceUri(RString& aUri);

	// From MStringDictionary
	void Release();

public:

	// CodePage tokens.
	//
	// Note: There will be correlation between these token values and their
	// strings via the CDictionaryCodePage using the RStringPool.

	enum TTokenValue {
		// CodePage 00
		// -//SYNCML//DTD SyncML 1.1//EN
		ETokenValueAdd					= 0x05,
		ETokenValueAlert				= 0x06,
		ETokenValueArchive				= 0x07,
		ETokenValueAtomic				= 0x08,
		ETokenValueChal					= 0x09,
		ETokenValueCmd					= 0x0A,
		ETokenValueCmdID				= 0x0B,
		ETokenValueCmdRef				= 0x0C,
		ETokenValueCopy					= 0x0D,
		ETokenValueCred					= 0x0E,
		ETokenValueData					= 0x0F,
		ETokenValueDelete				= 0x10,
		ETokenValueExec					= 0x11,
		ETokenValueFinal				= 0x12,
		ETokenValueGet					= 0x13,
		ETokenValueItem					= 0x14,
		ETokenValueLang					= 0x15,
		ETokenValueLocName				= 0x16,
		ETokenValueLocURI				= 0x17,
		ETokenValueMap					= 0x18,
		ETokenValueMapItem				= 0x19,
		ETokenValueMeta					= 0x1A,
		ETokenValueMsgID				= 0x1B,
		ETokenValueMsgRef				= 0x1C,
		ETokenValueNoResp				= 0x1D,
		ETokenValueNoResults			= 0x1E,
		ETokenValuePut					= 0x1F,
		ETokenValueReplace				= 0x20,
		ETokenValueRespURI				= 0x21,
		ETokenValueResults				= 0x22,
		ETokenValueSearch				= 0x23,
		ETokenValueSequence				= 0x24,
		ETokenValueSessionID			= 0x25,
		ETokenValueSftDel				= 0x26,
		ETokenValueSource				= 0x27,
		ETokenValueSourceRef			= 0x28,
		ETokenValueStatus				= 0x29,
		ETokenValueSync					= 0x2A,
		ETokenValueSyncBody				= 0x2B,
		ETokenValueSyncHdr				= 0x2C,
		ETokenValueSyncML				= 0x2D,
		ETokenValueTarget				= 0x2E,
		ETokenValueTargetRef			= 0x2F,
		ETokenValueReservedForFutureUse	= 0x30,
		ETokenValueVerDTD				= 0x31,
		ETokenValueVerProto				= 0x32,
		ETokenValueNumberOfChanges		= 0x33,
		ETokenValueMoreData				= 0x34,
	};


private:

	CTestStringDictionary00(RStringPool* aStringPool);

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
	{CTestStringDictionary00::ETokenValueAdd,
	 CTestStringDictionary00::ETokenValueAlert,
	 CTestStringDictionary00::ETokenValueArchive,
	 CTestStringDictionary00::ETokenValueAtomic,
	 CTestStringDictionary00::ETokenValueChal,
	 CTestStringDictionary00::ETokenValueCmd,
	 CTestStringDictionary00::ETokenValueCmdID,
	 CTestStringDictionary00::ETokenValueCmdRef,
	 CTestStringDictionary00::ETokenValueCopy,
	 CTestStringDictionary00::ETokenValueCred,
	 CTestStringDictionary00::ETokenValueData,
	 CTestStringDictionary00::ETokenValueDelete,
	 CTestStringDictionary00::ETokenValueExec,
	 CTestStringDictionary00::ETokenValueFinal,
	 CTestStringDictionary00::ETokenValueGet,
	 CTestStringDictionary00::ETokenValueItem,
	 CTestStringDictionary00::ETokenValueLang,
	 CTestStringDictionary00::ETokenValueLocName,
	 CTestStringDictionary00::ETokenValueLocURI,
	 CTestStringDictionary00::ETokenValueMap,
	 CTestStringDictionary00::ETokenValueMapItem,
	 CTestStringDictionary00::ETokenValueMeta,
	 CTestStringDictionary00::ETokenValueMsgID,
	 CTestStringDictionary00::ETokenValueMsgRef,
	 CTestStringDictionary00::ETokenValueNoResp,
	 CTestStringDictionary00::ETokenValueNoResults,
	 CTestStringDictionary00::ETokenValuePut,
	 CTestStringDictionary00::ETokenValueReplace,
	 CTestStringDictionary00::ETokenValueRespURI,
	 CTestStringDictionary00::ETokenValueResults,
	 CTestStringDictionary00::ETokenValueSearch,
	 CTestStringDictionary00::ETokenValueSequence,
	 CTestStringDictionary00::ETokenValueSessionID,
	 CTestStringDictionary00::ETokenValueSftDel,
	 CTestStringDictionary00::ETokenValueSource,
	 CTestStringDictionary00::ETokenValueSourceRef,
	 CTestStringDictionary00::ETokenValueStatus,
	 CTestStringDictionary00::ETokenValueSync,
	 CTestStringDictionary00::ETokenValueSyncBody,
	 CTestStringDictionary00::ETokenValueSyncHdr,
	 CTestStringDictionary00::ETokenValueSyncML,
	 CTestStringDictionary00::ETokenValueTarget,
	 CTestStringDictionary00::ETokenValueTargetRef,
	 CTestStringDictionary00::ETokenValueReservedForFutureUse,
	 CTestStringDictionary00::ETokenValueVerDTD,
	 CTestStringDictionary00::ETokenValueVerProto,
	 CTestStringDictionary00::ETokenValueNumberOfChanges,
	 CTestStringDictionary00::ETokenValueMoreData,
	 0}; // ok to end with zero as this is used by a global token

#endif //__T_STRINGDICTIONARY00_H__
